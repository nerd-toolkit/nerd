/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Project homepage: nerd.x-bot.org                                      *
 *                                                                         *
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
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
 *   clearly by citing the nerd homepage and the nerd overview paper.      *
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

namespace nerd {
	
	
	ExternalSensorAdapter::ExternalSensorAdapter(const QString &name) 
		: SimObject(name), mMonitoredValue(0)
	{
		mSensorValue = new InterfaceValue(getName(), "Sensor", 0.0, 0.0, 1.0);
		mMonitoredValueName = new StringValue();
		mCustomSensorName = new StringValue("Sensor");
		mSensorRange = new RangeValue(0.0, 1.0);
		
		addParameter("Sensor", mSensorValue);
		addParameter("MonitoredValueName", mMonitoredValueName);
		addParameter("CustomSensorName", mCustomSensorName);
		addParameter("SensorRange", mSensorRange);
		
		mOutputValues.append(mSensorValue);
	}
	
	
	
	
	ExternalSensorAdapter::ExternalSensorAdapter(const ExternalSensorAdapter &sensor) 
		: Object(), ValueChangedListener(), SimObject(sensor), mMonitoredValue(0)
	{
		mSensorValue = dynamic_cast<InterfaceValue*>(getParameter("Sensor"));
		mMonitoredValueName = dynamic_cast<StringValue*>(getParameter("MonitoredValueName"));
		mCustomSensorName = dynamic_cast<StringValue*>(getParameter("CustomSensorName"));
		mSensorRange = dynamic_cast<RangeValue*>(getParameter("SensorRange"));
		
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
		
		mMonitoredValue = vm->getDoubleValue(mMonitoredValueName->get());
		
		if(mMonitoredValue == 0) {
			Core::log("ExternalSensorAdapter [" + getName() + "]: Could not find required "
					  "monitored DoubleValue with name [" + mMonitoredValueName->get() + "]. "
					  "This sensor will be disabled!", true);
		}
	}
	
	void ExternalSensorAdapter::clear() {
		mMonitoredValue = 0;
	}
	
	void ExternalSensorAdapter::updateSensorValues() {
		if(mMonitoredValue != 0) {
			mSensorValue->set(mMonitoredValue->get());
		}
	}
	
	
}
