/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Project homepage: nerd.x-bot.org                                      *
 *                                                                         *
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Toolkit is part of the EU project ALEAR                      *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work was funded (2008 - 2011) by EU-Project Number ICT 214856    *
 *                                                                         *
 *                                                                         *
 *   License Agreement:                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   Publications based on work using the NERD kit have to state this      *
 *   clearly by citing the NERD homepage and the NERD overview paper.      *  
 ***************************************************************************/

#include "ExternalSensorAdapter.h"
#include "Value/InterfaceValue.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Core/Core.h"
#include "SimBody.h"
#include "Math/Math.h"
#include "Math/Quaternion.h"
#include <math.h>
#include "Math/Math.h"
#include "Value/Vector3DValue.h"
#include "NerdConstants.h"
#include "Value/NormalizedDoubleValue.h"
#include "Value/StringValue.h"
#include "Value/RangeValue.h"
#include <QStringList>


namespace nerd {
	
	
	ExternalSensorAdapter::ExternalSensorAdapter(const QString &name) 
		: SimObject(name)
	{
		mSensorValue = new InterfaceValue(getName(), "Sensor", 0.0, 0.0, 1.0);
		mMonitoredValueNames = new StringValue();
		mCustomSensorName = new StringValue("Sensor");
		mSensorRange = new RangeValue(0.0, 1.0);
		mCalculationMode = new IntValue(0);
		
		mCalculationMode->setDescription("The way multiple monitored values are combined:\n"
											"0: mean\n"
											"1: min\n"
											"2: max");
		
		addParameter("Sensor", mSensorValue);
		addParameter("MonitoredValueNames", mMonitoredValueNames);
		addParameter("CustomSensorName", mCustomSensorName);
		addParameter("SensorRange", mSensorRange);
		addParameter("Mode", mCalculationMode);
		
		mOutputValues.append(mSensorValue);
	}
	
	
	
	
	ExternalSensorAdapter::ExternalSensorAdapter(const ExternalSensorAdapter &sensor) 
		: Object(), ValueChangedListener(), SimObject(sensor)
	{
		mSensorValue = dynamic_cast<InterfaceValue*>(getParameter("Sensor"));
		mMonitoredValueNames = dynamic_cast<StringValue*>(getParameter("MonitoredValueNames"));
		mCustomSensorName = dynamic_cast<StringValue*>(getParameter("CustomSensorName"));
		mSensorRange = dynamic_cast<RangeValue*>(getParameter("SensorRange"));
		mCalculationMode = dynamic_cast<IntValue*>(getParameter("Mode"));
		
		mOutputValues.append(mSensorValue);
	}
	
	ExternalSensorAdapter::~ExternalSensorAdapter() {
	}
	
	SimObject* ExternalSensorAdapter::createCopy() const {
		return new ExternalSensorAdapter(*this);
	}
	
	
	QString ExternalSensorAdapter::getName() const {
		return SimObject::getName();
	}


	void ExternalSensorAdapter::valueChanged(Value *value) {
		SimObject::valueChanged(value);
		if(value == 0) {
			return;
		}
		else if(value == mSensorRange) {
			mSensorValue->setMin(mSensorRange->getMin());
			mSensorValue->setMax(mSensorRange->getMax());
		}
		else if(value == mCustomSensorName) {
			mSensorValue->setInterfaceName(mCustomSensorName->get());
		}
	}
	
	void ExternalSensorAdapter::setup() {
		
		ValueManager *vm = Core::getInstance()->getValueManager();
		
		QStringList monitoredValueNames = mMonitoredValueNames->get().split(",");

		for(QListIterator<QString> i(monitoredValueNames); i.hasNext();) {
			QString name = i.next();
			DoubleValue *value = vm->getDoubleValue(name);
			if(value == 0) {
				Core::log("ExternalSensorAdapter [" + getName() + "]: Could not find the "
							"external value [" + name + "] to monitor. Ignoring that value!", true);
			}
			else {
				mMonitoredValues.append(value);
			}
		}
		
		if(mMonitoredValues.empty()) {
			Core::log("ExternalSensorAdapter [" + getName() + "]: Could not find any valid "
						"DoubleValue to monitor [" + mMonitoredValueNames->get() + "]. "
					  "This sensor will be disabled!", true);
		}
	}
	
	void ExternalSensorAdapter::clear() {
		mMonitoredValues.clear();
	}
	
	void ExternalSensorAdapter::updateSensorValues() {
		
		double sensorValue = 0.0;
		
		if(!mMonitoredValues.empty()) {
			switch(mCalculationMode->get()) {
				case 0:
					//mean
					for(int i = 0; i < mMonitoredValues.size(); ++i) {
						sensorValue += mMonitoredValues.at(i)->get();
					}
					sensorValue = sensorValue / ((double) mMonitoredValues.size());
					break;
				case 1:
					//min
					sensorValue = mMonitoredValues.at(0)->get();
					for(int i = 1; i < mMonitoredValues.size(); ++i) {
						sensorValue = Math::min(sensorValue, mMonitoredValues.at(i)->get());
					}
					break;
				case 2:
					//max
					sensorValue = mMonitoredValues.at(0)->get();
					for(int i = 1; i < mMonitoredValues.size(); ++i) {
						sensorValue = Math::max(sensorValue, mMonitoredValues.at(i)->get());
					}
					break;
				default:
					;
			}
		}
		mSensorValue->set(sensorValue);
	}
	
	
}
