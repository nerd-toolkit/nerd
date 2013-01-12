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


#include "ODE_Dynamixel.h"
#include "Math/Math.h"
#include "Math/Vector3D.h"
#include "Core/Core.h"
#include <iostream>

using namespace std;

namespace nerd {

/**
 * Constructor.
 * @param name The name of the ODE_Dynamixel Object.
 */
ODE_Dynamixel::ODE_Dynamixel(const QString &name, bool hideTorqueInputs)
		: SimSensor(), SimActuator(), Dynamixel(name, hideTorqueInputs), ODE_Joint(), mHingeJoint(0)
{
	// ODE parameters, which define the behavior of a joint (bounciness when reaching a border...)
// 	ValueManager *vm = Core::getInstance()->getValueManager();
// 	if(vm->getValue("/AngularMotor/MotorBounce") == 0) {
// 		mMotorBounceValue = new DoubleValue(0.0);
// 		vm->addValue("/AngularMotor/MotorBounce", mMotorBounceValue);
// 	}
// 	else {
// 		mMotorBounceValue = vm->getDoubleValue("/AngularMotor/MotorBounce");
// 	}
// 
// 	if(vm->getValue("/AngularMotor/MotorStopCFM") == 0) {
// 		mMotorStopCFMValue = new DoubleValue(0.00001);
// 		vm->addValue("/AngularMotor/MotorStopCFM", mMotorStopCFMValue);
// 	}
// 	else {
// 		mMotorStopCFMValue = vm->getDoubleValue("/AngularMotor/MotorStopCFM");
// 	}
// 
// 	if(vm->getValue("/AngularMotor/MotorCFM") == 0) {
// 		mMotorCFMValue = new DoubleValue(0.00001);
// 		vm->addValue("/AngularMotor/MotorCFM", mMotorCFMValue);
// 	}
// 	else {
// 		mMotorCFMValue = vm->getDoubleValue("/AngularMotor/MotorCFM");
// 	}
// 
// 	if(vm->getValue("/AngularMotor/MotorStopERP") == 0) {
// 		mMotorStopERPValue = new DoubleValue(0.2);
// 		vm->addValue("/AngularMotor/MotorStopERP", mMotorStopERPValue);
// 	}
// 	else {
// 		mMotorStopERPValue = vm->getDoubleValue("/AngularMotor/MotorStopERP");
// 	}
// 
// 	if(vm->getValue("/AngularMotor/FudgeFactor") == 0) {
// 		mMotorFudgeFactorValue = new DoubleValue(1.0);
// 		vm->addValue("/AngularMotor/FudgeFactor", mMotorFudgeFactorValue);
// 	}
// 	else {
// 		mMotorFudgeFactorValue = vm->getDoubleValue("/AngularMotor/FudgeFactor");
// 	}
}

/**
 * Copy-Constructor.
 * @param motor 
 */
ODE_Dynamixel::ODE_Dynamixel(const ODE_Dynamixel &motor) : Object(),
		ValueChangedListener(), SimSensor(), SimActuator(),
		 Dynamixel(motor), ODE_Joint(), mHingeJoint(0)
{
// 	ValueManager *vm = Core::getInstance()->getValueManager();
// 	mMotorBounceValue = vm->getDoubleValue("/AngularMotor/MotorBounce");
// 	mMotorStopCFMValue = vm->getDoubleValue("/AngularMotor/MotorStopCFM");
// 	mMotorCFMValue = vm->getDoubleValue("/AngularMotor/MotorCFM");
// 	mMotorStopERPValue = vm->getDoubleValue("/AngularMotor/MotorStopERP");
// 	mMotorFudgeFactorValue = vm->getDoubleValue("/AngularMotor/FudgeFactor");
}

/**
 * Destructor.
 */
ODE_Dynamixel::~ODE_Dynamixel() {
}

SimObject* ODE_Dynamixel::createCopy() const {
	return new ODE_Dynamixel(*this);
}

void ODE_Dynamixel::setup() {
	ODE_Joint::setup();
	Dynamixel::setup();
	if(mJoint == 0) {
		mJoint = (dJointID) ODE_Joint::createJoint();
	}
	if(mJoint == 0) {
		Core::log("ODE_Dynamixel: dJoint could not be created.");
		return;
	}

}


void ODE_Dynamixel::clear() {
	ODE_Joint::clearJoint();
	Dynamixel::clear();
	if(mHingeJoint != 0) {
		dJointDestroy(mHingeJoint);
		mHingeJoint = 0;
	}
	mJoint = 0;
}


void ODE_Dynamixel::valueChanged(Value *value) {

	Dynamixel::valueChanged(value);

	if((value == mDesiredMotorTorqueValue || value == mFMaxFactor) && mJoint != 0) {
		dJointSetAMotorParam(mJoint, dParamFMax, mFMaxFactor->get() 
			* Math::max(0.0, mDesiredMotorTorqueValue->get()));
	}
}

void ODE_Dynamixel::updateActuators() {

	if(mJoint != 0) {
		mCurrentPosition = dJointGetAMotorAngle(mJoint, 0);
		calculateMotorFrictionAndVelocity();
	
		dJointSetHingeParam(mHingeJoint, dParamFMax, mCalculatedFriction);
		dJointSetAMotorParam(mJoint, dParamVel, mCalculatedVelocity);
	}
}

/**
 * Calculates the new motor angle.
 */
void ODE_Dynamixel::updateSensorValues() {

	if(mJoint != 0) {
		//read sensor value and add noise.
		double sensorValue = dJointGetAMotorAngle(mJoint, 0);
	
		// First add the choosen offset
		// Add desired noise to the measured angle value
		// Map the value into the interval -150 - 150 degree ( = -1 - 1)
		
		sensorValue = (sensorValue * 180.0)/Math::PI;
		if(mIncludeNoise == true)
		{
			sensorValue = Math::calculateGaussian(sensorValue, mSensorNoiseDeviation);
		}

		mAngleValue->set(sensorValue);
	}

}


dJointID ODE_Dynamixel::createJoint(dBodyID body1, dBodyID body2) {

	if(mJointAxisPoint1->get().equals(mJointAxisPoint2->get(), -1)) {
		Core::log("ODE_Dynamixel: Invalid axes for ODE_Dynamixel.", true);
		return 0;
	}

	Vector3D anchor = mJointAxisPoint1->get();
	Vector3D axis = mJointAxisPoint2->get() - mJointAxisPoint1->get();

	dJointID joint = dJointCreateAMotor(mWorldID, mGeneralJointGroup);
	dJointAttach(joint, body1, body2);
	dJointSetAMotorMode(joint, dAMotorEuler);
	dJointSetAMotorParam(joint, dParamVel, 0.0);
	dJointSetAMotorParam(joint, dParamFMax, mDesiredMotorTorqueValue->get());
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

	if(body1 == 0) {
		double tmp = mMinAngle;
		mMinAngle = -1.0 * mMaxAngle;
		mMaxAngle = -1.0 * tmp;
	}

	dJointSetHingeParam(mHingeJoint, dParamLoStop, mMinAngle);
	dJointSetHingeParam(mHingeJoint, dParamHiStop, mMaxAngle);

	dJointSetHingeParam(mHingeJoint, dParamVel, 0.0);
	return joint;
}




}
