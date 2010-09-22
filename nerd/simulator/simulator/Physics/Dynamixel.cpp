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

#include "Dynamixel.h"
#include "Core/Core.h"
#include "Math/Math.h"
#include "Math/Vector3D.h"
#include "SimulationConstants.h"	
#include <math.h>

namespace nerd {

Dynamixel::Dynamixel(const QString &name, const QString &globalParameterPrefix)
	: SimSensor(), SimActuator(), HingeJoint(name), mGlobalParameterPrefix(globalParameterPrefix)
{
	//TODO: these parameters are not used. If individual dynamixels should be able to have own parameter settings, instead of the global ones, these parameters should be included and the according lines in the setup-method should be included as well. But due to the fact, that this motor is an implementation of a specific physical motor, no differences between instances should be allowed.
// 	mPID_Proportional = new DoubleValue(1.0);
// 	mPID_Integral = new DoubleValue(0.1);
// 	mPID_Differential = new DoubleValue(0.1);
	mMinAngleValue = new DoubleValue(-150.0);
	mMaxAngleValue = new DoubleValue(150.0);
// 	mMaxTorque = new DoubleValue(0.0);
	mOffsetValue = new DoubleValue(0.0);
	mDesiredMotorAngleValue = new InterfaceValue(getName(), "DesiredAngle", 0, -150.0, 150.0);
	mDesiredMotorAngleValue->setNormalizedMin(-1.0);
	mDesiredMotorAngleValue->setNormalizedMax(1.0);

	mAngleValue = new InterfaceValue(getName(), "JointAngle", 0.0, -150.0, 150.0);
	mAngleValue->setNormalizedMin(-1);
	mAngleValue->setNormalizedMax(1);

	//ensures that tanh in the range [-1 0] produces a desired torque 0.
	mEliminateNegativeTorqueRange = new BoolValue(true);

	mDesiredMotorTorqueValue = new InterfaceValue(getName(), "DesiredTorque", 
		0.586810005, 0.0, 1.0);
	if(mEliminateNegativeTorqueRange->get()) {
		mDesiredMotorTorqueValue->setMin(-1.0);
	}
	mDesiredMotorTorqueValue->setNormalizedMin(0.0);
	mDesiredMotorTorqueValue->setNormalizedMax(1.0);
	mDesiredMotorTorqueValue->addValueChangedListener(this);

	ValueManager *vm = Core::getInstance()->getValueManager();

	if(vm->getValue(globalParameterPrefix + "/PID/Proportional") == 0) {
		mProportional = new DoubleValue(20);
		vm->addValue(globalParameterPrefix + "/PID/Proportional", mProportional);
	}
	else {
		mProportional = vm->getDoubleValue(globalParameterPrefix + "/PID/Proportional");
	}
// 	mPID_Proportional->set(mProportional->get());
	
	
	if(vm->getValue(globalParameterPrefix + "/IncludeNoise") == 0) {
		mIncludeNoiseValue = new BoolValue(true);
		vm->addValue(globalParameterPrefix + "/IncludeNoise", mIncludeNoiseValue);
	}
	else {
		mIncludeNoiseValue = vm->getBoolValue(globalParameterPrefix + "/IncludeNoise");
	}

	if(vm->getValue(globalParameterPrefix + "/SensorNoiseDeviation") == 0) {
		mSensorNoiseDeviationValue = new DoubleValue(0.189076);
		vm->addValue(globalParameterPrefix + "/SensorNoiseDeviation", mSensorNoiseDeviationValue);
	}
	else {
		mSensorNoiseDeviationValue = vm->
			getDoubleValue(globalParameterPrefix + "/SensorNoiseDeviation");
	}

	if(vm->getValue(globalParameterPrefix + "/PID/Differential") == 0) {
		mDamping = new DoubleValue(0.0);
		vm->addValue(globalParameterPrefix + "/PID/Differential", mDamping);
	}
	else {
		mDamping = vm->getDoubleValue(globalParameterPrefix + "/PID/Differential");
	}
// 	mPID_Differential->set(mDamping->get());

	if(vm->getValue(globalParameterPrefix + "/PID/Integral") == 0) {
		mIntegral = new DoubleValue(0.0);
		vm->addValue(globalParameterPrefix + "/PID/Integral", mIntegral);
	}
	else {
		mIntegral = vm->getDoubleValue(globalParameterPrefix + "/PID/Integral");
	}
// 	mPID_Integral->set(mIntegral->get());

	if(vm->getValue(globalParameterPrefix + "/PID/HistorySize") == 0) {
		mHistorySizeValue = new IntValue(10);
		vm->addValue(globalParameterPrefix + "/PID/HistorySize", mHistorySizeValue);
	}
	else {
		mHistorySizeValue = vm->getIntValue(globalParameterPrefix + "/PID/HistorySize");
	}

	if(vm->getValue(globalParameterPrefix + "/FMaxFactor") == 0) {
		mFMaxFactor = new DoubleValue(1.04803939);
		vm->addValue(globalParameterPrefix + "/FMaxFactor", mFMaxFactor);
	}
	else {
		mFMaxFactor = vm->getDoubleValue(globalParameterPrefix + "/FMaxFactor");
	}

	if(vm->getValue(globalParameterPrefix + "/MaxVelocity") == 0) {
		mMaxVelocityValue = new DoubleValue(3.5);
		vm->addValue(globalParameterPrefix + "/MaxVelocity", mMaxVelocityValue);
	}
	else {
		mMaxVelocityValue = vm->getDoubleValue(globalParameterPrefix + "/MaxVelocity");
	}
	
	if(vm->getValue(globalParameterPrefix + "/StaticFriction") == 0) {
		mStaticFrictionValue = new DoubleValue(0.001);
		vm->addValue(globalParameterPrefix + "/StaticFriction", mStaticFrictionValue);
	}
	else {
		mStaticFrictionValue = vm->getDoubleValue(globalParameterPrefix + "/StaticFriction");
	}

	if(vm->getValue(globalParameterPrefix + "/StaticFrictionBorder") == 0) {
		mStaticFrictionBorderValue = new DoubleValue(0.1);
		vm->addValue(globalParameterPrefix + "/StaticFrictionBorder", mStaticFrictionBorderValue);
	}
	else {
		mStaticFrictionBorderValue = vm->
			getDoubleValue(globalParameterPrefix + "/StaticFrictionBorder");
	}

	if(vm->getValue(globalParameterPrefix + "/DynamicFrictionOffset") == 0) {
		mDynamicFrictionOffsetValue = new DoubleValue(0.0);
		vm->addValue(globalParameterPrefix + "/DynamicFrictionOffset", mDynamicFrictionOffsetValue);
	}
	else {
		mDynamicFrictionOffsetValue = vm->
			getDoubleValue(globalParameterPrefix + "/DynamicFrictionOffset");
	}

	if(vm->getValue(globalParameterPrefix + "/DynamicFriction") == 0) {
		mDynamicFrictionValue = new DoubleValue(0.16);
		vm->addValue(globalParameterPrefix + "/DynamicFriction", mDynamicFrictionValue);
	}
	else {
		mDynamicFrictionValue = vm->getDoubleValue(globalParameterPrefix + "/DynamicFriction");
	}
	
	if(vm->getValue(globalParameterPrefix + "/MotorNoise") == 0) {
		mMotorValueNoiseValue = new DoubleValue(0.0);
		vm->addValue(globalParameterPrefix + "/MotorNoise", mMotorValueNoiseValue);
	}
	else {
		mMotorValueNoiseValue = vm->getDoubleValue(globalParameterPrefix + "/MotorNoise");
	}
	mCurrentVelocityValue = new DoubleValue(0.0);

// 	addParameter("PID_P", mPID_Proportional);
// 	addParameter("PID_I", mPID_Integral);
// 	addParameter("PID_D", mPID_Differential);
	addParameter("MinAngle", mMinAngleValue);
	addParameter("MaxAngle", mMaxAngleValue);
// 	addParameter("MaxTorque", mMaxTorque);
	addParameter("DesiredAngle", mDesiredMotorAngleValue);
	addParameter("DesiredTorque", mDesiredMotorTorqueValue);
	addParameter("Offset", mOffsetValue);
	addParameter("CurrentVelocity", mCurrentVelocityValue);
	addParameter("JointAngle", mAngleValue);
	addParameter("EliminateNegativeTorqueRange", mEliminateNegativeTorqueRange);

	mInputValues.append(mDesiredMotorAngleValue);
	mInputValues.append(mDesiredMotorTorqueValue);
	mOutputValues.append(mAngleValue);

	mTimeStepValue = vm->getDoubleValue(SimulationConstants::VALUE_TIME_STEP_SIZE);
	if(mTimeStepValue == 0) {
		Core::log("ODE_DynamixelMotor: Could not find required value.");
		return;
	}

	mMotorValueNoise = mMotorValueNoiseValue->get();
	mDynamicFriction = mDynamicFrictionValue->get();
	mStaticFriction = mStaticFrictionValue->get();
	mDynamicFrictionOffset = mDynamicFrictionOffsetValue->get();
	mStaticFrictionBorder = mStaticFrictionBorderValue->get();
	mMaxVelocity = mMaxVelocityValue->get();
	mIncludeNoise = mIncludeNoiseValue->get();
	mSensorNoiseDeviation = mSensorNoiseDeviationValue->get();

	mHistorySize = mHistorySizeValue->get();

	mMinAngle = (mMinAngleValue->get() * Math::PI) / 180.0;
	mMaxAngle = (mMaxAngleValue->get() * Math::PI) / 180.0;

// 	mDesiredMotorAngleValue->setMin(mMinAngleValue->get());
// 	mDesiredMotorAngleValue->setMax(mMaxAngleValue->get());
// 	mAngleValue->setMin(mMinAngleValue->get());
// 	mAngleValue->setMax(mMaxAngleValue->get());

	mPidProportional = mProportional->get();
	mPidIntegral = mIntegral->get();
	mPidDifferential = mDamping->get();

	mOffset = mOffsetValue->get();
	mTimeStepSize = mTimeStepValue->get();
}


Dynamixel::Dynamixel(const Dynamixel &joint) 
	: Object(), ValueChangedListener(), SimSensor(),
	  SimActuator(), HingeJoint(joint), mGlobalParameterPrefix(joint.mGlobalParameterPrefix)
{
	ValueManager *vm = Core::getInstance()->getValueManager();
// 	mPID_Proportional = dynamic_cast<DoubleValue*>(getParameter("PID_P"));
// 	mPID_Integral = dynamic_cast<DoubleValue*>(getParameter("PID_I"));
// 	mPID_Differential = dynamic_cast<DoubleValue*>(getParameter("PID_D"));
	mMinAngleValue = dynamic_cast<DoubleValue*>(getParameter("MinAngle"));
	mMaxAngleValue = dynamic_cast<DoubleValue*>(getParameter("MaxAngle"));
// 	mMaxTorque = dynamic_cast<DoubleValue*>(getParameter("MaxTorque"));
	mDesiredMotorAngleValue = dynamic_cast<InterfaceValue*>(getParameter("DesiredAngle"));
	mDesiredMotorTorqueValue = dynamic_cast<InterfaceValue*>(getParameter("DesiredTorque"));
	mAngleValue = dynamic_cast<InterfaceValue*>(getParameter("JointAngle"));
	mOffsetValue = dynamic_cast<DoubleValue*>(getParameter("Offset"));
	mCurrentVelocityValue = dynamic_cast<DoubleValue*>(getParameter("CurrentVelocity"));
	mEliminateNegativeTorqueRange = dynamic_cast<BoolValue*>(getParameter("EliminateNegativeTorqueRange"));

	mDesiredMotorTorqueValue->addValueChangedListener(this);

	mInputValues.append(mDesiredMotorAngleValue);
	mInputValues.append(mDesiredMotorTorqueValue);
	mOutputValues.append(mAngleValue);

	mDynamicFrictionValue = vm->getDoubleValue(mGlobalParameterPrefix + "/DynamicFriction");
	mMotorValueNoiseValue = vm->getDoubleValue(mGlobalParameterPrefix + "/MotorNoise");
	mDynamicFrictionOffsetValue = vm->getDoubleValue(mGlobalParameterPrefix + "/DynamicFrictionOffset");
	mStaticFrictionBorderValue = vm->getDoubleValue(mGlobalParameterPrefix + "/StaticFrictionBorder");
	mStaticFrictionValue = vm->getDoubleValue(mGlobalParameterPrefix + "/StaticFriction");
	mMaxVelocityValue = vm->getDoubleValue(mGlobalParameterPrefix + "/MaxVelocity");
	mFMaxFactor = vm->getDoubleValue(mGlobalParameterPrefix + "/FMaxFactor");
	mHistorySizeValue = vm->getIntValue(mGlobalParameterPrefix + "/PID/HistorySize");
	mIntegral = vm->getDoubleValue(mGlobalParameterPrefix + "/PID/Integral");
	mDamping = vm->getDoubleValue(mGlobalParameterPrefix + "/PID/Differential");
	mProportional = vm->getDoubleValue(mGlobalParameterPrefix + "/PID/Proportional");
	mSensorNoiseDeviationValue = vm->getDoubleValue(mGlobalParameterPrefix + "/SensorNoiseDeviation");
	mTimeStepValue = vm->getDoubleValue(SimulationConstants::VALUE_TIME_STEP_SIZE);

	mMotorValueNoise = mMotorValueNoiseValue->get();
	mDynamicFriction = mDynamicFrictionValue->get();
	mStaticFriction = mStaticFrictionValue->get();
	mDynamicFrictionOffset = mDynamicFrictionOffsetValue->get();
	mStaticFrictionBorder = mStaticFrictionBorderValue->get();
	mMaxVelocity = mMaxVelocityValue->get();
	mSensorNoiseDeviation = mSensorNoiseDeviationValue->get();

	mHistorySize = mHistorySizeValue->get();

	mMinAngle = (mMinAngleValue->get() * Math::PI) / 180.0;
	mMaxAngle = (mMaxAngleValue->get() * Math::PI) / 180.0;

// 	mDesiredMotorAngleValue->setMin(mMinAngleValue->get());
// 	mDesiredMotorAngleValue->setMax(mMaxAngleValue->get());
// 	mAngleValue->setMin(mMinAngleValue->get());
// 	mAngleValue->setMax(mMaxAngleValue->get());
	
	mPidProportional = mProportional->get();
	mPidIntegral = mIntegral->get();
	mPidDifferential = mDamping->get();

	mOffset = mOffsetValue->get();
	mTimeStepSize = mTimeStepValue->get();
}


Dynamixel::~Dynamixel() {
}


SimObject* Dynamixel::createCopy() const {
	return new Dynamixel(*this);
}
		
void Dynamixel::valueChanged(Value *value) {
	SimJoint::valueChanged(value);
	if(value == 0) {
		return;
	}
}


QString Dynamixel::getName() const {
	return mNameValue->get();
}

/**
 * Updates the motor by using the motor inputs. This method needs to be reimplemented by
 * the physics-specific subclasses. To perform a proper update:
 * 1) calculate the current motor angle
 * 2) call 	calculateMotorFrictionAndVelocity(): this method calculates the new velocity and 
 * friction for the motor.
 * 3) Use the new calcluated velocity (mCalculatedVelocity) and friction (mCalculatedFriction) 
 * to update the motor.
 */
void Dynamixel::updateActuators() {
	
}


/**
 * Encapsulates the Dynamixel-specific calculations (PID-controller and motorfriction). 
 * After setting the current motor angle, this method is tu be used to update the motor.
 */
void Dynamixel::calculateMotorFrictionAndVelocity() {

	if(mStepSize == 0.0) {
		Core::log("Dynamixel Warning: StepSize was 0.0, prevented division by zero!");
		return;
	}

	mVelocity = (mCurrentPosition - mLastPosition) / mStepSize;
	double calcVel;

	double desiredAngle = (mDesiredMotorAngleValue->get() * Math::PI / 180.0);
	if(desiredAngle > Math::PI){
		calcVel = calculateVelocity(mCurrentPosition, desiredAngle - 2.0 * Math::PI);
	}
	else if(desiredAngle >= (-1.0 * Math::PI)) {
		calcVel = calculateVelocity(mCurrentPosition, desiredAngle);
	}
	else {
		calcVel = calculateVelocity(mCurrentPosition, desiredAngle + 2.0 * Math::PI);
	}

	if(mMaxVelocity >= 0) {
		if(calcVel > 0) {
			calcVel = Math::min(calcVel, mMaxVelocity);
		}
		else {
			calcVel = Math::max(calcVel, -mMaxVelocity);
		}
	}

	mCalculatedFriction = calculateFriction(mVelocity);
	
	if(mIncludeNoise == true)
	{
		calcVel = Math::calculateGaussian(calcVel, mMotorValueNoise);
	}

	mCurrentVelocityValue->set(calcVel);
	mCalculatedVelocity = calcVel;

	mLastPosition = mCurrentPosition;
}

/**
 * Update the motor angle information. This method needs to be implemented by the 
 * physic-specific subclasses.
 */
void Dynamixel::updateSensorValues() {
}


void Dynamixel::setup() {
	HingeJoint::setup();
	mVelocity = 0.0;
	mLastPosition = 0.0;

	mErrorHistory.clear();
	
// 	if(mProportional->get() != mPID_Proportional->get()) {
// 		mPidProportional = mPID_Proportional->get();
// 	}
// 	else {
	mPidProportional = mProportional->get();
// 	}
// 	if(mIntegral->get() != mPID_Integral->get()) {
// 		mPidIntegral = mPID_Integral->get();
// 	}
// 	else {
	mPidIntegral = mIntegral->get();
// 	}
// 	if(mDamping->get() != mPID_Differential->get()) {
// 		mPidDifferential = mPID_Differential->get();
// 	}
// 	else {
	mPidDifferential = mDamping->get();
// 	}
	mHistorySize = mHistorySizeValue->get();

	if(mEliminateNegativeTorqueRange->get()) {
		mDesiredMotorTorqueValue->setMin(-1.0);
	}
	else {
		mDesiredMotorTorqueValue->setMin(0.0);
	}

	mDynamicFriction = mDynamicFrictionValue->get();
	mStaticFriction = mStaticFrictionValue->get();
	mDynamicFrictionOffset = mDynamicFrictionOffsetValue->get();
	mStaticFrictionBorder = mStaticFrictionBorderValue->get();
	mSensorNoiseDeviation = mSensorNoiseDeviationValue->get();
	mMotorValueNoise = mMotorValueNoiseValue->get();

	mMaxVelocity = mMaxVelocityValue->get();
	mTimeStepSize = mTimeStepValue->get();
	
	mOffset = mOffsetValue->get();
	mMinAngle = ((mMinAngleValue->get()) * Math::PI) / 180.0;
	mMaxAngle = ((mMaxAngleValue->get()) * Math::PI) / 180.0;

// 	mDesiredMotorAngleValue->setMin(mMinAngleValue->get());
// 	mDesiredMotorAngleValue->setMax(mMaxAngleValue->get());
// 	mAngleValue->setMin(mMinAngleValue->get());
// 	mAngleValue->setMax(mMaxAngleValue->get());

	if(mTimeStepValue != 0) {
		mStepSize = mTimeStepValue->get();
	}
	else {
		mStepSize = 0;
	}
}

void Dynamixel::clear() {
	HingeJoint::clear();
	mLastError = 0.0;
	iState = 0.0;
}

/**
 * Calculates the velocity that should be reached by the motor in the next timestep. 
 * Implements a PID-Controller.
 * @param currentPosition 
 * @param desiredPosition 
 * @return The new desired velocity of the motor.
 */
double Dynamixel::calculateVelocity(double currentPosition, double desiredPosition) {

	double velocity = 0.0;
	double currentError = desiredPosition - currentPosition;

	mErrorHistory.push_back(currentError);

	if(mErrorHistory.size() > mHistorySize) {
		mErrorHistory.pop_front();
	}

	double currentI = 0.0;

	for(int i = 0; i < mErrorHistory.size(); i++) {
		currentI += mErrorHistory.at(i);
	}
	double errorDistance = currentError - mLastError;
	double iPart;
	if(Math::compareDoubles(mPidIntegral, 0.0, 10)) {
		iPart= 0.0;
	}
	else {
		iPart= mTimeStepSize / mPidIntegral * currentI;
	}

	if(mTimeStepSize == 0.0) {
		Core::log("Dynamixel Warning: TimeStepSize was 0.0, prevented division by zero.");
		return 0.0;
	}

	velocity = mPidProportional
		* (currentError + iPart + mPidDifferential / mTimeStepSize * errorDistance);

	mLastError = currentError;

	return velocity;
}

/**
 * Calcluated the total motor friction (static and dynamic) that is modelled by a simple
 * linear friction model. 
 */
double Dynamixel::calculateFriction(double velocity) {
	double friction = 0;

	if(mStaticFriction == 0.0 && mDynamicFriction == 0.0) {
		return friction;
	}
	else {
		if(velocity < mStaticFrictionBorder && velocity > -mStaticFrictionBorder) {
			friction = mStaticFriction;
		}
		friction += mDynamicFriction * fabs(velocity) + mDynamicFrictionOffset;
		if(friction < 0.0) {
			friction = 0.0;
		}
		return fabs(friction);
	}
}

void Dynamixel::setName(const QString &name) {
	HingeJoint::setName(name);
	QString newName = name;	
}

}
