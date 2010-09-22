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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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



#include "UniversalJointMotorAdapter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "MotorModels/MotorModel.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new UniversalJointMotorAdapter.
 */
UniversalJointMotorAdapter::UniversalJointMotorAdapter(const QString &name)
	: MotorAdapter(MotorModel::UNIVERSAL_JOINT), UniversalJoint(name)
{
	initializeConstruction();

	mAxis1MinAngle = new DoubleValue(-150.0);
	mAxis1MaxAngle = new DoubleValue(150.0);
	mAxis2MinAngle = new DoubleValue(-150.0);
	mAxis2MaxAngle = new DoubleValue(150.0);

	mDesiredMotorTorqueValue = new InterfaceValue(getName(), "DesiredTorque", 0.0, -1.0, 1.0);
	mDesiredMotorAngleValue = new InterfaceValue(getName(), "DesiredAngle", 0.0, -150.0, 150.0);
	mAngleAxis1Value = new InterfaceValue(getName(), "AngleAxis1", 0.0, -150.0, 150.0);
	mAngleAxis2Value = new InterfaceValue(getName(), "AngleAxis2", 0.0, -150.0, 150.0);


	addParameter("Axis1MinAngle", mAxis1MinAngle);
	addParameter("Axis1MaxAngle", mAxis1MaxAngle);
	addParameter("Axis2MinAngle", mAxis2MinAngle);
	addParameter("Axis2MaxAngle", mAxis2MaxAngle);

	addParameter("DesiredTorque", mDesiredMotorTorqueValue);
	addParameter("DesiredAngle", mDesiredMotorAngleValue);
	addParameter("AngleAxis1", mAngleAxis1Value);
	addParameter("AngleAxis2", mAngleAxis2Value);

	mInputValues.append(mDesiredMotorTorqueValue);
	mInputValues.append(mDesiredMotorAngleValue);
	mOutputValues.append(mAngleAxis1Value);
	mOutputValues.append(mAngleAxis2Value);

}


/**
 * Copy constructor. 
 * 
 * @param other the UniversalJointMotorAdapter object to copy.
 */
UniversalJointMotorAdapter::UniversalJointMotorAdapter(const UniversalJointMotorAdapter &other) 
	: MotorAdapter(other), Object(), ValueChangedListener(), UniversalJoint(other)
{
	initializeCopyConstruction();

	mAxis1MinAngle = dynamic_cast<DoubleValue*>(getParameter("Axis1MinAngle"));
	mAxis1MaxAngle = dynamic_cast<DoubleValue*>(getParameter("Axis1MaxAngle"));
	mAxis2MinAngle = dynamic_cast<DoubleValue*>(getParameter("Axis2MinAngle"));
	mAxis2MaxAngle = dynamic_cast<DoubleValue*>(getParameter("Axis2MaxAngle"));

	mDesiredMotorTorqueValue = dynamic_cast<InterfaceValue*>(getParameter("DesiredTorque"));
	mDesiredMotorAngleValue = dynamic_cast<InterfaceValue*>(getParameter("DesiredAngle"));
	mAngleAxis1Value = dynamic_cast<InterfaceValue*>(getParameter("AngleAxis1"));
	mAngleAxis2Value = dynamic_cast<InterfaceValue*>(getParameter("AngleAxis2"));

	mInputValues.append(mDesiredMotorTorqueValue);
	mInputValues.append(mDesiredMotorAngleValue);
	mOutputValues.append(mAngleAxis1Value);
	mOutputValues.append(mAngleAxis2Value);

}

/**
 * Destructor.
 */
UniversalJointMotorAdapter::~UniversalJointMotorAdapter() {
}


SimObject* UniversalJointMotorAdapter::createCopy() const {
	UniversalJointMotorAdapter *ret = new UniversalJointMotorAdapter(*this);
	ret->collectCompatibleMotorModels();
	return ret;
}
		

void UniversalJointMotorAdapter::setup() {
	UniversalJoint::setup();
	MotorAdapter::setup();
}


void UniversalJointMotorAdapter::clear() {
	MotorAdapter::clear();
	UniversalJoint::clear();
}


QString UniversalJointMotorAdapter::getAbsoluteName() const {
	return SimJoint::getAbsoluteName();
}


bool UniversalJointMotorAdapter::addParameter(const QString &name, Value *value) {
	return UniversalJoint::addParameter(name, value);
}	


Value* UniversalJointMotorAdapter::getParameter(const QString &name) const {
	return UniversalJoint::getParameter(name);
}


const QList<InterfaceValue*>& UniversalJointMotorAdapter::getInputValues() const {
	return UniversalJoint::getInputValues();
}


const QList<InterfaceValue*>& UniversalJointMotorAdapter::getOutputValues() const {
	return UniversalJoint::getOutputValues();
}


void UniversalJointMotorAdapter::valueChanged(Value *value) {
	MotorAdapter::valueChanged(value);
	UniversalJoint::valueChanged(value);
}	


bool UniversalJointMotorAdapter::isValidMotorModel(MotorModel *model) const {
	if(model != 0) {
		return true;
	}
	return false;
}


}



