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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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

#include "ControlParameter.h"

namespace nerd {

ControlParameter::ControlParameter(const QString &name)
	: SimSensor(), SimObject(name), mControlParameter(0), mAutoControl(0)
{
	mControlParameter = new InterfaceValue(getName(), "Control", 0.0, 0.0, 1.0);
	mAutoControl = new BoolValue(true);
	mInitialValue = new DoubleValue(0.0);

	addParameter("ControlParameter", mControlParameter);
	addParameter("AutoControl", mAutoControl);
	addParameter("InitialValue", mInitialValue);

	mOutputValues.append(mControlParameter);
}

ControlParameter::ControlParameter(const ControlParameter &other)
	: Object(), ValueChangedListener(), SimSensor(), SimObject(other)
{
	mControlParameter = dynamic_cast<InterfaceValue*>(getParameter("ControlParameter"));
	mAutoControl = dynamic_cast<BoolValue*>(getParameter("AutoControl"));
	mInitialValue = dynamic_cast<DoubleValue*>(getParameter("InitialValue"));

	mOutputValues.append(mControlParameter);
}

ControlParameter::~ControlParameter() {
}


SimObject* ControlParameter::createCopy() const {
	return new ControlParameter(*this);
}



void ControlParameter::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm*) {

}


void ControlParameter::setup() {
	SimObject::setup();
	if(mAutoControl->get()) {
		mControlParameter->set(mInitialValue->get());
	}
}


void ControlParameter::clear() {
	SimObject::clear();
}

void ControlParameter::valueChanged(Value *value) {
	SimObject::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mNameValue) {
		mControlParameter->setInterfaceName(mNameValue->get());
	}
}

void ControlParameter::updateSensorValues() {

}

InterfaceValue* ControlParameter::getControlParameter() const {
	return mControlParameter;
}



}


