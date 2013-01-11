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



#include "SpringAdapter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "Physics/SliderJoint.h"
#include "Physics/HingeJoint.h"
#include "MotorModels/SpringModel.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SpringAdapter.
 */
SpringAdapter::SpringAdapter(MotorModel::JointType type, const QString &name)
	: MotorAdapter(type), SimObject(name), mType(type), mTargetJoint(0)
{

	initializeConstruction();

	mCurrentTorque = new DoubleValue();
	mTargetJointName = new StringValue("");

	addParameter("CurrentTorque", mCurrentTorque);
	addParameter("TargetJointName", mTargetJointName);
}


/**
 * Copy constructor. 
 * 
 * @param other the SpringAdapter object to copy.
 */
SpringAdapter::SpringAdapter(const SpringAdapter &other) 
	: Object(), ValueChangedListener(), MotorAdapter(other), SimObject(other), 
	  mType(other.mType), mTargetJoint(0)
{
	initializeCopyConstruction();

	mCurrentTorque = dynamic_cast<DoubleValue*>(getParameter("CurrentTorque"));
	mTargetJointName = dynamic_cast<StringValue*>(getParameter("TargetJointName"));
}

/**
 * Destructor.
 */
SpringAdapter::~SpringAdapter() {
}

void SpringAdapter::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa) {
	SimObject::synchronizeWithPhysicalModel(psa);
}


void SpringAdapter::setup() {
	SimObject::setup();
	MotorAdapter::setup();

	SimJoint *joint = Physics::getPhysicsManager()->getSimJoint(mTargetJointName->get());

	mTargetJoint = 0;

	if(mType == MotorModel::HINGE_JOINT) {
		if(dynamic_cast<HingeJoint*>(joint) != 0) {
			mTargetJoint = joint;
		}
	}
	else if(mType == MotorModel::SLIDER_JOINT) {
		if(dynamic_cast<SliderJoint*>(joint) != 0) {
			mTargetJoint = joint;
		}
	}

	if(mTargetJoint == 0) {
		Core::log("SpringAdapter: Could not find matching target Joint [" 
					+ mTargetJointName->get() + "]", true);
	}
}


void SpringAdapter::clear() {
	SimObject::clear();
	MotorAdapter::clear();

	mTargetJoint = 0;
}


MotorModel::JointType SpringAdapter::getType() const {
	return mType;
}


SimJoint* SpringAdapter::getCurrentTargetJoint() const {
	return mTargetJoint;
}


DoubleValue* SpringAdapter::getCurrentTorqueValue() const {
	return mCurrentTorque;
}

SimObject* SpringAdapter::createCopy() const {
	SpringAdapter *ret = new SpringAdapter(*this);
	ret->collectCompatibleMotorModels();
	return ret;
}


QString SpringAdapter::getAbsoluteName() const {
	return SimObject::getAbsoluteName();
}


bool SpringAdapter::addParameter(const QString &name, Value *value) {
	return SimObject::addParameter(name, value);
}


Value* SpringAdapter::getParameter(const QString &name) const {
	return SimObject::getParameter(name);
}

		

QList<InterfaceValue*> SpringAdapter::getInputValues() const {
	return SimObject::getInputValues();
}


QList<InterfaceValue*> SpringAdapter::getOutputValues() const {
	return SimObject::getOutputValues();
}


bool SpringAdapter::isValidMotorModel(MotorModel *model) const {
	SpringModel *springModel = dynamic_cast<SpringModel*>(model);
	if(springModel != 0 && springModel->getType() == mJointType) {
		return true;
	}
	return false;
}


}



