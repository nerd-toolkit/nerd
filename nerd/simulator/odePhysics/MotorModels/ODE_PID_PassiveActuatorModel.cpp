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



#include "ODE_PID_PassiveActuatorModel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"
#include "Value/BoolValue.h"
#include "Value/Vector3DValue.h"
#include "Value/DoubleValue.h"
#include "Physics/MotorAdapter.h"
#include "MotorModels/PassiveActuatorAdapter.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ODE_PID_PassiveActuatorModel.
 */
ODE_PID_PassiveActuatorModel::ODE_PID_PassiveActuatorModel(const QString &name)
	: PID_PassiveActuatorModel(name), ODE_Joint()
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ODE_PID_PassiveActuatorModel object to copy.
 */
ODE_PID_PassiveActuatorModel::ODE_PID_PassiveActuatorModel(const ODE_PID_PassiveActuatorModel &other) 
	: PID_PassiveActuatorModel(other), ODE_Joint()
{
}

/**
 * Destructor.
 */
ODE_PID_PassiveActuatorModel::~ODE_PID_PassiveActuatorModel() {
}

SimObject* ODE_PID_PassiveActuatorModel::createCopy() const {
	return new ODE_PID_PassiveActuatorModel(*this);
}


void ODE_PID_PassiveActuatorModel::setup() {
	ODE_Joint::setup();
	PID_PassiveActuatorModel::setup();
	if(mJoint == 0) {
		mJoint = (dJointID) ODE_Joint::createJoint();
	}
	if(mJoint == 0) {
		Core::log("ODE_DynamixelFrictionMotor: dJoint could not be created.");
		return;
	}
}

void ODE_PID_PassiveActuatorModel::clear() {
	ODE_Joint::clearJoint();
	PID_PassiveActuatorModel::clear();
	mJoint = 0;
}

void ODE_PID_PassiveActuatorModel::valueChanged(Value *value) 
{
	PID_PassiveActuatorModel::valueChanged(value);
}

void ODE_PID_PassiveActuatorModel::updateInputValues() 
{
	if(mJoint != 0) {
		double desiredAngle = 0.0;
		PassiveActuatorAdapter *owner = dynamic_cast<PassiveActuatorAdapter*>(mOwner);

		if(owner != 0) {
			desiredAngle = (owner->getReferenceAngle() + owner->getReferenceOffsetAngle()) 
							* owner->getGearRatio() + owner->getOffsetAngle();
		}


		double currentAngle = dJointGetAMotorAngle(mJoint, 0);
		mPID_Controller.update(currentAngle, desiredAngle * Math::PI / 180.0);

		dJointSetAMotorParam(mJoint, dParamFMax, mMaxForce->get());
		dJointSetHingeParam(mHingeJoint, dParamFMax, mFriction->get());
		dJointSetAMotorParam(mJoint, dParamVel, mPID_Controller.getVelocity());
	}
}


/**
 * Calculates the new motor angle.
 */
void ODE_PID_PassiveActuatorModel::updateOutputValues() {

}


dJointID ODE_PID_PassiveActuatorModel::createJoint(dBodyID body1, dBodyID body2) {

	Vector3DValue *jointAxisPoint1 = dynamic_cast<Vector3DValue*>(mOwner->getParameter("AxisPoint1"));
	Vector3DValue *jointAxisPoint2 = dynamic_cast<Vector3DValue*>(mOwner->getParameter("AxisPoint2"));
	DoubleValue *minAngleValue = dynamic_cast<DoubleValue*>(mOwner->getParameter("MinAngle"));
	DoubleValue *maxAngleValue = dynamic_cast<DoubleValue*>(mOwner->getParameter("MaxAngle"));

	if(jointAxisPoint1 == 0 || jointAxisPoint2 == 0 || minAngleValue == 0 || maxAngleValue == 0) {
		Core::log("ODE_PID_PassiveActuatorModel: Could not find all required parameter values.");
		return 0;
	}

	if(jointAxisPoint1->get().equals(jointAxisPoint2->get(), -1)) {
		Core::log("ODE_PID_PassiveActuatorModel: Invalid axis points " + jointAxisPoint1->getValueAsString() + " and " + jointAxisPoint2->getValueAsString() + ".");
		return 0;
	}

	if(jointAxisPoint1->get().equals(jointAxisPoint2->get(), -1)) {
		Core::log("Invalid axes for ODE_PID_PassiveActuatorModel.");
		return 0;
	}

	Vector3D anchor = jointAxisPoint1->get();
	Vector3D axis = jointAxisPoint2->get() - jointAxisPoint1->get();

	dJointID joint = dJointCreateAMotor(mWorldID, mGeneralJointGroup);
	dJointAttach(joint, body1, body2);
	dJointSetAMotorMode(joint, dAMotorEuler);
	dJointSetAMotorParam(joint, dParamVel, 0.0);
	dJointSetAMotorParam(joint, dParamFMax, 1.0);
	dJointSetAMotorParam(joint, dParamCFM, mCFMValue->get());
	dJointSetAMotorParam(joint, dParamStopERP, mStopERPValue->get());
	dJointSetAMotorParam(joint, dParamStopCFM, mStopCFMValue->get());
	dJointSetAMotorParam(joint, dParamBounce, mBounceValue->get());
	dJointSetAMotorParam(joint, dParamFudgeFactor, mFudgeFactorValue->get());

	axis.normalize();
	Vector3D perpedicular;
	if(axis.getY() != 0.0 || axis.getX() != 0.0) {
		perpedicular.set(-axis.getY(), axis.getX(), 0);
	}
	else {
		perpedicular.set(0, -axis.getZ(), axis.getY());
	}

	perpedicular.normalize();

	// If one of the bodies is static, the motor axis need to be defined in a different way. For different constellations of static and dynamic bodies, the turn direction of the motor changed, so this had to be added.
	if(body1 == 0) {
		dJointSetAMotorAxis(joint, 0, 0, -axis.getX(), -axis.getY(), -axis.getZ());
	}
	else {
		dJointSetAMotorAxis(joint, 0, 1, axis.getX(), axis.getY(), axis.getZ());
	}
	if(body2 == 0) {
		dJointSetAMotorAxis(joint, 2, 0, -perpedicular.getX(), -perpedicular.getY(), 
			-perpedicular.getZ());
	}
	else {
		dJointSetAMotorAxis(joint, 2, 2, perpedicular.getX(), perpedicular.getY(), 
			perpedicular.getZ());
	}

	mHingeJoint = dJointCreateHinge(mWorldID, mGeneralJointGroup);
	dJointAttach(mHingeJoint, body1, body2);

	dJointSetHingeAnchor(mHingeJoint, anchor.getX(), anchor.getY(), anchor.getZ());
	dJointSetHingeAxis(mHingeJoint, axis.getX(), axis.getY(), axis.getZ());

	double minAngle = (minAngleValue->get() * Math::PI) / 180.0;
	double maxAngle = (maxAngleValue->get() * Math::PI) / 180.0;

	if(body1 == 0) {
		double tmp = minAngle;
		minAngle = -1.0 * maxAngle;
		maxAngle = -1.0 * tmp;
	}

	dJointSetHingeParam(mHingeJoint, dParamLoStop, minAngle);
	dJointSetHingeParam(mHingeJoint, dParamHiStop, maxAngle);

	dJointSetHingeParam(mHingeJoint, dParamVel, 0.0);
	return joint;
}




}



