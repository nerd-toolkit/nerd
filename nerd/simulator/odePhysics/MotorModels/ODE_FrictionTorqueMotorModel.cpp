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



#include "ODE_FrictionTorqueMotorModel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/MotorAdapter.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ODE_FrictionTorqueMotorModel.
 */
ODE_U_FrictionTorqueMotorModel::ODE_U_FrictionTorqueMotorModel(const QString &name)
	: FrictionTorqueMotorModel(name, MotorModel::UNIVERSAL_JOINT), ODE_Joint()
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ODE_FrictionTorqueMotorModel object to copy.
 */
ODE_U_FrictionTorqueMotorModel::ODE_U_FrictionTorqueMotorModel(
					const ODE_U_FrictionTorqueMotorModel &other) 
	: FrictionTorqueMotorModel(other), ODE_Joint()
{
}

/**
 * Destructor.
 */
ODE_U_FrictionTorqueMotorModel::~ODE_U_FrictionTorqueMotorModel() {
}

SimObject* ODE_U_FrictionTorqueMotorModel::createCopy() const {
	return new ODE_U_FrictionTorqueMotorModel(*this);
}


void ODE_U_FrictionTorqueMotorModel::setup() {

	FrictionTorqueMotorModel::setup();
	ODE_Joint::setup();

	if(mJoint == 0) {
		mJoint = (dJointID) ODE_Joint::createJoint();
	}
	if(mJoint == 0) {
		Core::log("ODE_DynamixelFrictionMotor: dJoint could not be created.");
		return;
	}
}


void ODE_U_FrictionTorqueMotorModel::clear() {
	ODE_Joint::clearJoint();
	FrictionTorqueMotorModel::clear();
	mJoint = 0;
}

	
void ODE_U_FrictionTorqueMotorModel::updateInputValues() {
	FrictionTorqueMotorModel::updateInputValues();
	if(mJoint != 0) {
// 		mCurrentPosition = dJointGetAMotorAngle(mJoint, 0);
// 		calculateMotorFrictionAndVelocity();
// 	
// 		dJointSetHingeParam(mHingeJoint, dParamFMax, mCalculatedFriction);
// 		dJointSetAMotorParam(mJoint, dParamVel, mCalculatedVelocity);
	}
}


void ODE_U_FrictionTorqueMotorModel::updateOutputValues() {
	FrictionTorqueMotorModel::updateOutputValues();
	if(mJoint != 0) {
// 		//read sensor value and add noise.
// 		double sensorValue = dJointGetAMotorAngle(mJoint, 0);
// 	
// 		// First add the choosen offset
// 		// Add desired noise to the measured angle value
// 		// Map the value into the interval -150 - 150 degree ( = -1 - 1)
// 		
// 		sensorValue = (sensorValue * 180.0)/Math::PI;
// 
// 		if(mIncludeNoise->get() == true)
// 		{
// 			sensorValue = Math::calculateGaussian(sensorValue, mSensorNoiseDeviation);
// 		}
// 		mAngleValue->set(sensorValue);
	}
}

	
bool ODE_U_FrictionTorqueMotorModel::setOwner(MotorAdapter *owner) {

	if(owner == 0) {
		return false;
	}

	mJointAxis1Point1 = 0;
	mJointAxis1Point2 = 0;
	mJointAxis2Point1 = 0;
	mJointAxis2Point2 = 0;
	mAnchor = 0;
	mAxis1MinAngle = 0;
	mAxis1MaxAngle = 0;
	mAxis2MinAngle = 0;
	mAxis2MaxAngle = 0;
	mDesiredMotorAngleValue = 0;
	mAngleAxis1Value = 0;
	mAngleAxis2Value = 0;

	bool ok = FrictionTorqueMotorModel::setOwner(owner);
	if(ok) {
		//get required parameters from owner.

		mJointAxis1Point1 = dynamic_cast<Vector3DValue*>(owner->getParameter("JointAxis1Point1"));
		mJointAxis1Point2 = dynamic_cast<Vector3DValue*>(owner->getParameter("JointAxis1Point2"));
		mJointAxis2Point1 = dynamic_cast<Vector3DValue*>(owner->getParameter("JointAxis2Point1"));
		mJointAxis2Point2 = dynamic_cast<Vector3DValue*>(owner->getParameter("JointAxis2Point2"));
		mAnchor = dynamic_cast<Vector3DValue*>(owner->getParameter("JointAnchor"));

		mAxis1MinAngle = dynamic_cast<DoubleValue*>(owner->getParameter("Axis1MinAngle"));
		mAxis1MaxAngle = dynamic_cast<DoubleValue*>(owner->getParameter("Axis1MaxAngle"));
		mAxis2MinAngle = dynamic_cast<DoubleValue*>(owner->getParameter("Axis2MinAngle"));
		mAxis2MaxAngle = dynamic_cast<DoubleValue*>(owner->getParameter("Axis2MaxAngle"));

		mDesiredMotorTorqueValue = dynamic_cast<InterfaceValue*>(owner->getParameter("DesiredTorque"));
		mDesiredMotorAngleValue = dynamic_cast<InterfaceValue*>(owner->getParameter("DesiredAngle"));
		mAngleAxis1Value = dynamic_cast<InterfaceValue*>(owner->getParameter("AngleAxis1"));
		mAngleAxis2Value = dynamic_cast<InterfaceValue*>(owner->getParameter("AngleAxis2"));

		if(mJointAxis1Point1 == 0 || mJointAxis1Point2 == 0 || mJointAxis2Point1 == 0
			|| mJointAxis2Point2 == 0 || mAnchor == 0 || mAxis1MinAngle == 0
			|| mAxis1MaxAngle == 0 || mAxis2MinAngle == 0 || mAxis2MaxAngle == 0 
			|| mDesiredMotorTorqueValue == 0 || mDesiredMotorAngleValue == 0
			|| mAngleAxis1Value == 0 || mAngleAxis2Value == 0)
		{
			Core::log("ODE_U_FrictionTorqueMotorModel: Could not find all required"
					  " parameter Values from owner.");
			return false;
		}
		return true;
	}
	return false;
}


dJointID ODE_U_FrictionTorqueMotorModel::createJoint(dBodyID body1, dBodyID body2) {

	if(mJointAxis1Point1 == 0 || mJointAxis1Point2 == 0
		|| mJointAxis2Point1 == 0 || mJointAxis2Point2 == 0) 
	{
		Core::log("ODE_U_FrictionTorqueMotorModel: Could not find all required parmaeter Values.");
		return 0;
	}

	if(mJointAxis1Point1->get().equals(mJointAxis1Point2->get(), -1)
		|| mJointAxis2Point1->get().equals(mJointAxis2Point2->get(), -1)) 
	{
		Core::log("Invalid axes for ODE_U_FrictionTorqueMotorModel: " + mJointAxis1Point1->getValueAsString() + " "
			+ mJointAxis1Point2->getValueAsString() + " " + mJointAxis2Point1->getValueAsString() + " "
			+ mJointAxis2Point2->getValueAsString());
		return 0;
	}

	Vector3D anchor = mAnchor->get();
	Vector3D axis1 = mJointAxis1Point2->get() - mJointAxis1Point1->get();
	Vector3D axis2 = mJointAxis2Point2->get() - mJointAxis2Point1->get();

	dJointID joint = dJointCreateAMotor(mWorldID, mGeneralJointGroup);
	dJointAttach(joint, body1, body2);

	dJointSetAMotorMode(joint, dAMotorEuler);
	dJointSetAMotorNumAxes(joint, 2);

	dJointSetAMotorParam(joint, dParamVel, 0.0);
	dJointSetAMotorParam(joint, dParamFMax, mDesiredMotorTorqueValue->get());
	dJointSetAMotorParam(joint, dParamCFM, mCFMValue->get());
	dJointSetAMotorParam(joint, dParamStopERP, mStopERPValue->get());
	dJointSetAMotorParam(joint, dParamStopCFM, mStopCFMValue->get());
	dJointSetAMotorParam(joint, dParamBounce, mBounceValue->get());
	dJointSetAMotorParam(joint, dParamFudgeFactor, mFudgeFactorValue->get());

	

	axis1.normalize();
	Vector3D perpedicular1;
	if(axis1.getY() != 0.0 || axis1.getX() != 0.0) {
		perpedicular1.set(-axis1.getY(), axis1.getX(), 0);
	}
	else {
		perpedicular1.set(0, -axis1.getZ(), axis1.getY());
	}
	perpedicular1.normalize();

	

	// If one of the bodies is static, the motor axis need to be defined in a different way. 
	// For different constellations of static and dynamic bodies, the turn direction 
	// of the motor changed, so this had to be added.
	if(body1 == 0) {
		dJointSetAMotorAxis(joint, 0, 0, -axis1.getX(), -axis1.getY(), -axis1.getZ());
	}
	else {
		dJointSetAMotorAxis(joint, 0, 1, axis1.getX(), axis1.getY(), axis1.getZ());
	}
	if(body2 == 0) {
		dJointSetAMotorAxis(joint, 2, 0, -perpedicular1.getX(), -perpedicular1.getY(), 
			-perpedicular1.getZ());
	}
	else {
		dJointSetAMotorAxis(joint, 2, 2, perpedicular1.getX(), perpedicular1.getY(), 
			perpedicular1.getZ());
	}

	mUniversalJoint = dJointCreateUniversal(mWorldID, mGeneralJointGroup);
	dJointAttach(mUniversalJoint, body1, body2);

	dJointSetUniversalAnchor(mUniversalJoint, anchor.getX(), anchor.getY(), anchor.getZ());
	dJointSetUniversalAxis1(mUniversalJoint, axis1.getX(), axis1.getY(), axis1.getZ());
	dJointSetUniversalAxis2(mUniversalJoint, axis2.getX(), axis2.getY(), axis2.getZ());

	double minAngle1 = (mAxis1MinAngle->get() * Math::PI) / 180.0;
	double minAngle2 = (mAxis2MinAngle->get() * Math::PI) / 180.0;
	double maxAngle1 = (mAxis1MaxAngle->get() * Math::PI) / 180.0;
	double maxAngle2 = (mAxis2MaxAngle->get() * Math::PI) / 180.0;

// 	if(body1 == 0) {
// 		double tmp = 
// 		mMinAngle = -1.0 * mMaxAngle;
// 		mMaxAngle = -1.0 * tmp;
// 	}

	dJointSetUniversalParam(mUniversalJoint, dParamLoStop, minAngle1);
	dJointSetUniversalParam(mUniversalJoint, dParamHiStop, maxAngle1);
	dJointSetUniversalParam(mUniversalJoint, dParamLoStop2, minAngle2);
	dJointSetUniversalParam(mUniversalJoint, dParamHiStop2, maxAngle2);

	dJointSetUniversalParam(mUniversalJoint, dParamVel, 0.0);
	dJointSetUniversalParam(mUniversalJoint, dParamVel2, 0.0);

	return joint;
}


}



