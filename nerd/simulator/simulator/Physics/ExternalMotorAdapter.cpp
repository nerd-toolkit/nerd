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

#include "ExternalMotorAdapter.h"
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
	
	
	ExternalMotorAdapter::ExternalMotorAdapter(const QString &name) 
		: SimObject(name), mControlledValue(0)
	{
		mMotorValue = new InterfaceValue(getName(), "Sensor", 0.0, 0.0, 1.0);
		mControlledValueName = new StringValue();
		mCustomMotorName = new StringValue("Sensor");
		mMotorRange = new RangeValue(0.0, 1.0);
		
		addParameter("Motor", mMotorValue);
		addParameter("ControlledValueName", mControlledValueName);
		addParameter("CustomMotorName", mCustomMotorName);
		addParameter("MotorRange", mMotorRange);
		
		
		
		mOutputValues.append(mMotorValue);
	}
	
	
	
	
	ExternalMotorAdapter::ExternalMotorAdapter(const ExternalMotorAdapter &sensor) 
		: Object(), ValueChangedListener(), SimObject(sensor), mControlledValue(0)
	{
		mMotorValue = dynamic_cast<InterfaceValue*>(getParameter("Motor"));
		mControlledValueName = dynamic_cast<StringValue*>(getParameter("ControlledValueName"));
		mCustomMotorName = dynamic_cast<StringValue*>(getParameter("CustomMotorName"));
		mMotorRange = dynamic_cast<RangeValue*>(getParameter("MotorRange"));
		
		mOutputValues.append(mMotorValue);
	}
	
	ExternalMotorAdapter::~ExternalMotorAdapter() {
	}
	
	SimObject* ExternalMotorAdapter::createCopy() const {
		return new ExternalMotorAdapter(*this);
	}
	
	
	QString ExternalMotorAdapter::getName() const {
		return SimObject::getName();
	}


	void ExternalMotorAdapter::valueChanged(Value *value) {
		SimObject::valueChanged(value);
		if(value == 0) {
			return;
		}
		else if(value == mMotorRange) {
			mMotorValue->setMin(mMotorRange->getMin());
			mMotorValue->setMax(mMotorRange->getMax());
		}
		else if(value == mCustomMotorName) {
			mMotorValue->setInterfaceName(mCustomMotorName->get());
		}
	}
	
	void ExternalMotorAdapter::setup() {
		
		ValueManager *vm = Core::getInstance()->getValueManager();
		
		mControlledValue = vm->getDoubleValue(mControlledValueName->get());
		if(mControlledValue == 0) {
			Core::log("ExternalMotorAdapter [" + getName() + "]: Could not find the "
						"external value [" + mControlledValueName->get() + "] to monitor. Ignoring that value!", true);
		}
	}
	
	void ExternalMotorAdapter::clear() {
		mControlledValue = 0;
	}
	
	void ExternalMotorAdapter::updateActuators() {
		
		if(mControlledValue == 0) {
			return;
		}
		
		mControlledValue->set(mMotorValue->get());
	}
	
	
}
