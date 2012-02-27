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



#include "MotorModel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/MotorAdapter.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new MotorModel.
 */
MotorModel::MotorModel(const QString &name, JointType type)
	: SimJoint(name), mOwner(0), mJointType(type)
{ 
	//remove unused parameters from ValueManager.
	publishParameter(mFirstBodyName, false);
	publishParameter(mSecondBodyName, false);
}


/**
 * Copy constructor. 
 * 
 * @param other the MotorModel object to copy.
 */
MotorModel::MotorModel(const MotorModel &other) 
	: Object(), ValueChangedListener(), SimJoint(other), mOwner(0), mJointType(other.mJointType)
{
	publishParameter(mFirstBodyName, false);
	publishParameter(mSecondBodyName, false);
}

MotorModel::~MotorModel() 
{
}


void MotorModel::setup() {
	if(mOwner != 0) {
		StringValue *firstBodyName = dynamic_cast<StringValue*>(mOwner->getParameter("FirstBody"));
		StringValue *secondBodyName = dynamic_cast<StringValue*>(mOwner->getParameter("SecondBody"));

		if(firstBodyName != 0) {
			mFirstBodyName->set(firstBodyName->get());
		}
		if(secondBodyName != 0) {
			mSecondBodyName->set(secondBodyName->get());
		}
	}
	SimJoint::setup();
}

void MotorModel::clear() {
	SimJoint::clear();
}



bool MotorModel::setOwner(MotorAdapter *owner) {
	mOwner = owner;
	return true;
}


/**
 * Use this function only (!!!) in setOwner to add double parameters of the model to the ValueManager with the prefix of the
 * owning adapter prototype name and the current model name. This is intended to support parameters which should ne global
 * for all copies of an adapter prototype.
 * For example the MSeries Robot has two HipPitch motors, right and left. Right and
 * left HipPitchMotor use the same Adapter Prototype (PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_HIP_PITCH).
 * There are parameters of the motor models used by this prototype which should be used by both motors, left and right (e.g. the
 * transmission ratio from the joint to the motor). These parameters, which should be the same for all copies of one
 * prototpye, can be created with this function. This is especially important for model optimization!
 * 
 * @param owner The owning MotorAdapter of this model. Use the parameter from the setOwner function!
 * @param name The name of the parameter to add (e.g. "SpringConstant")
 * @param d The return DoubleValue is initialized with this parameter
 * @return The created DoubleValue.
 */
DoubleValue* MotorModel::createGlobalPrototypeDoubleValue(MotorAdapter *owner, QString name, double d) {
	// Get ValueManager
	ValueManager *vm = Core::getInstance()->getValueManager();

	// Get Model Name without Prototypes prefix
	QString modelName = getName().mid(11);

	// Name in ValueManager is "/PrototypeName/ModelName/ValueName"
	QString valueName = owner->getAbsoluteName() + "/" + modelName + "/" + name;

	// Check if Value already exists
	DoubleValue *doubleValue = vm->getDoubleValue(valueName);

	// If not, create it and add it to ValueManager
	if(doubleValue == 0) {
		doubleValue = new DoubleValue(d);
		vm->addValue(valueName, doubleValue);
	}

	return doubleValue;
}

MotorAdapter* MotorModel::getOwner() const {
	return mOwner;
}


int MotorModel::getType() const {
	return mJointType;
}

}



