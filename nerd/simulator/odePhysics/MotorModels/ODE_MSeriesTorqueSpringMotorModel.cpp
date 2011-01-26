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
#include "ODE_MSeriesTorqueSpringMotorModel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/MotorAdapter.h"
#include "MotorModels/TorqueDynamixelMotorAdapter.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ODE_MSeriesTorqueSpringMotorModel.
 */
ODE_H_MSeriesTorqueSpringMotorModel::ODE_H_MSeriesTorqueSpringMotorModel(const QString &name)
	: H_MSeriesTorqueSpringMotorModel(name), ODE_Joint()
{
/*	mJointCFMValue = new DoubleValue(0.1);
	mJointFudgeFactorValue = new DoubleValue(0.5);

	addParameter("JointCFM", mJointCFMValue);
	addParameter("JointFudgeFactor", mJointFudgeFactorValue);
*/}


/**
 * Copy constructor. 
 * 
 * @param other the ODE_MSeriesTorqueSpringMotorModel object to copy.
 */
ODE_H_MSeriesTorqueSpringMotorModel::ODE_H_MSeriesTorqueSpringMotorModel(
					const ODE_H_MSeriesTorqueSpringMotorModel &other) 
	: Object(other), ValueChangedListener(other), H_MSeriesTorqueSpringMotorModel(other), ODE_Joint()
{
/*	mJointCFMValue = dynamic_cast<DoubleValue*>(getParameter("JointCFM"));
	mJointFudgeFactorValue = dynamic_cast<DoubleValue*>(getParameter("JointFudgeFactor"));
*/}

/**
 * Destructor.
 */
ODE_H_MSeriesTorqueSpringMotorModel::~ODE_H_MSeriesTorqueSpringMotorModel() {
}

SimObject* ODE_H_MSeriesTorqueSpringMotorModel::createCopy() const {
	return new ODE_H_MSeriesTorqueSpringMotorModel(*this);
}


void ODE_H_MSeriesTorqueSpringMotorModel::setup() {

	H_MSeriesTorqueSpringMotorModel::setup();
	ODE_Joint::setup();

	if(mJoint == 0) {
		mJoint = (dJointID) ODE_Joint::createJoint();
	}
	if(mJoint == 0) {
		Core::log("ODE_H_MSeriesTorqueSpringModel: dJoint could not be created.", true);
		return;
	}
}


void ODE_H_MSeriesTorqueSpringMotorModel::clear() {
	ODE_Joint::clearJoint();
	H_MSeriesTorqueSpringMotorModel::clear();
	mJoint = 0;
}

	
void ODE_H_MSeriesTorqueSpringMotorModel::updateInputValues() {
	H_MSeriesTorqueSpringMotorModel::updateInputValues();
	if(mJoint != 0) {
		// Get the joint angle from ODE as input to the model
		mCurrentJointAngle = dJointGetHingeAngle(mJoint);
		// Update the model and calculate the torque that is applied to the joint
		double torque = calculateJointTorque();
		// Set the calculated torque on the ODE joint
		dJointAddHingeTorque(mJoint, torque);
		// Set the joint friction
		dJointSetHingeParam(mJoint, dParamVel, 0.0);
		dJointSetHingeParam(mJoint, dParamFMax, mJointCoulombFriction->get());
	}
}


void ODE_H_MSeriesTorqueSpringMotorModel::updateOutputValues() {
	H_MSeriesTorqueSpringMotorModel::updateOutputValues();

	if(mJoint != 0) {

		TorqueDynamixelMotorAdapter *owner = dynamic_cast<TorqueDynamixelMotorAdapter*>(mOwner);

		if(owner != 0 && owner->isUsingJointAngleSensor()) {

			// Read out joint angle from Physics and map to -180° +180°
			double jointAngle = dJointGetHingeAngle(mJoint);

			owner->getJointAngleSensorValue()->set(calculateAngularSensor(jointAngle));
		}
	}
}

dJointID ODE_H_MSeriesTorqueSpringMotorModel::createJoint(dBodyID body1, dBodyID body2) {
	Vector3DValue *jointAxisPoint1 = dynamic_cast<Vector3DValue*>(mOwner->getParameter("AxisPoint1"));
	Vector3DValue *jointAxisPoint2 = dynamic_cast<Vector3DValue*>(mOwner->getParameter("AxisPoint2"));
	DoubleValue *minAngleValue = dynamic_cast<DoubleValue*>(mOwner->getParameter("MinAngle"));
	DoubleValue *maxAngleValue = dynamic_cast<DoubleValue*>(mOwner->getParameter("MaxAngle"));

	if(jointAxisPoint1 == 0 || jointAxisPoint2 == 0 || minAngleValue == 0 || maxAngleValue == 0) {
		Core::log("ODE_H_MSeriesTorqueSpringMotorModel: Could not find all required parameter values.");
		return 0;
	}

	if(jointAxisPoint1->get().equals(jointAxisPoint2->get(), -1)) {
		Core::log("ODE_H_MSeriesTorqueSpringMotorModel: Invalid axis points " + jointAxisPoint1->getValueAsString() + " and " + jointAxisPoint2->getValueAsString() + ".");
		return 0;
	}

	Vector3D anchor = jointAxisPoint1->get();
	Vector3D axis = jointAxisPoint2->get() - jointAxisPoint1->get();

/*	// Create the motor

	dJointID motorJoint = dJointCreateAMotor(mWorldID, mGeneralJointGroup);
	dJointAttach(motorJoint, body1, body2);
	dJointSetAMotorMode(motorJoint, dAMotorEuler);
	dJointSetAMotorParam(motorJoint, dParamVel, 0.0);
	dJointSetAMotorParam(motorJoint, dParamFMax, mJointCoulombFriction->get());
	dJointSetAMotorParam(motorJoint, dParamCFM, mCFMValue->get());
	dJointSetAMotorParam(motorJoint, dParamStopERP, mStopERPValue->get());
	dJointSetAMotorParam(motorJoint, dParamStopCFM, mStopCFMValue->get());
	dJointSetAMotorParam(motorJoint, dParamBounce, mBounceValue->get());
	dJointSetAMotorParam(motorJoint, dParamFudgeFactor, mFudgeFactorValue->get());

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
		dJointSetAMotorAxis(motorJoint, 0, 0, -axis.getX(), -axis.getY(), -axis.getZ());
	}
	else {
		dJointSetAMotorAxis(motorJoint, 0, 1, axis.getX(), axis.getY(), axis.getZ());
	}
	if(body2 == 0) {
		dJointSetAMotorAxis(motorJoint, 2, 0, -perpedicular.getX(), -perpedicular.getY(), 
			-perpedicular.getZ());
	}
	else {
		dJointSetAMotorAxis(motorJoint, 2, 2, perpedicular.getX(), perpedicular.getY(), 
			perpedicular.getZ());
	}
*/
	// Create the Hinge Joint

	dJointID joint = dJointCreateHinge(mWorldID, mGeneralJointGroup);
	dJointAttach(joint, body1, body2);

	dJointSetHingeAnchor(joint, anchor.getX(), anchor.getY(), anchor.getZ());
	dJointSetHingeAxis(joint, axis.getX(), axis.getY(), axis.getZ());

	double minAngle = (minAngleValue->get() * Math::PI) / 180.0;
	double maxAngle = (maxAngleValue->get() * Math::PI) / 180.0;

	dJointSetHingeParam(joint, dParamLoStop, minAngle);
	dJointSetHingeParam(joint, dParamHiStop, maxAngle);

	dJointSetHingeParam(joint, dParamCFM, mCFMValue->get());
	dJointSetHingeParam(joint, dParamStopERP, mStopERPValue->get());
	dJointSetHingeParam(joint, dParamStopCFM, mStopCFMValue->get());
	dJointSetHingeParam(joint, dParamBounce, mBounceValue->get());
	dJointSetHingeParam(joint, dParamFudgeFactor, mFudgeFactorValue->get());

//	dJointSetHingeParam(joint, dParamVel, 0.0);

	return joint;
}


}



