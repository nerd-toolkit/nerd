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



#include "HingeJointMotorAdapter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new HingeJointMotorAdapter.
 */
HingeJointMotorAdapter::HingeJointMotorAdapter(const QString &name)
	: MotorAdapter(MotorModel::HINGE_JOINT), HingeJoint(name)
{
	initializeConstruction();

	mMinAngleValue = new DoubleValue(-180.0);
	mMaxAngleValue = new DoubleValue(+180.0);

	addParameter("MinAngle", mMinAngleValue);
	addParameter("MaxAngle", mMaxAngleValue);
}


/**
 * Copy constructor. 
 * 
 * @param other the HingeJointMotorAdapter object to copy.
 */
HingeJointMotorAdapter::HingeJointMotorAdapter(const HingeJointMotorAdapter &other)
	: SimSensor(other), SimActuator(other), Object(), ValueChangedListener(), MotorAdapter(other), HingeJoint(other)
{
	initializeCopyConstruction();

	mMinAngleValue = dynamic_cast<DoubleValue*>(getParameter("MinAngle"));
	mMaxAngleValue = dynamic_cast<DoubleValue*>(getParameter("MaxAngle"));
}

/**
 * Destructor.
 */
HingeJointMotorAdapter::~HingeJointMotorAdapter() {
}


SimObject* HingeJointMotorAdapter::createCopy() const {
	HingeJointMotorAdapter *ret = new HingeJointMotorAdapter(*this);
	ret->collectCompatibleMotorModels();
	return ret;
}

void HingeJointMotorAdapter::setup() {
	HingeJoint::setup();
	MotorAdapter::setup();
}


void HingeJointMotorAdapter::clear() {
	MotorAdapter::clear();
	HingeJoint::clear();
}



QString HingeJointMotorAdapter::getAbsoluteName() const {
	return SimJoint::getAbsoluteName();
}



bool HingeJointMotorAdapter::addParameter(const QString &name, Value *value) {
	return HingeJoint::addParameter(name, value);
}


Value* HingeJointMotorAdapter::getParameter(const QString &name) const {
	return HingeJoint::getParameter(name);
}



const QList<InterfaceValue*>& HingeJointMotorAdapter::getInputValues() const {
	return HingeJoint::getInputValues();
}


const QList<InterfaceValue*>& HingeJointMotorAdapter::getOutputValues() const {
	return HingeJoint::getOutputValues();
}	



void HingeJointMotorAdapter::valueChanged(Value *value) {
	MotorAdapter::valueChanged(value);
	HingeJoint::valueChanged(value);
}


bool HingeJointMotorAdapter::isValidMotorModel(MotorModel *model) const {
	if(model != 0) {
		return true;
	}
	return false;
}




}



