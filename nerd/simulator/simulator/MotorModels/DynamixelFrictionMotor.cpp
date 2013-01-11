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


#include "MotorModels/DynamixelFrictionMotor.h"

#include <QString>
#include "Value/DoubleValue.h"
#include "Value/InterfaceValue.h"
#include "Value/BoolValue.h"
#include "Core/Core.h"
#include "Math/Math.h"
#include "Math/Vector3D.h"
#include "SimulationConstants.h"  
#include <math.h>

namespace nerd {

DynamixelFrictionMotor::DynamixelFrictionMotor(const QString &name)
	: DynamixelMotor(name)
{
	mOffsetValue = new DoubleValue(0.0);
	
	//ensures that tanh in the range [-1 0] produces a desired torque 0.
	mEliminateNegativeTorqueRange = new BoolValue(true);
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	
	if(vm->getValue("/AngularMotor/IncludeNoise") == 0) {
		mIncludeNoise = new BoolValue(true);
		vm->addValue("/AngularMotor/IncludeNoise", mIncludeNoise);
	}
	else {
		mIncludeNoise = vm->getBoolValue("/AngularMotor/IncludeNoise");
	}
	
	if(vm->getValue("/AngularMotor/PID/Proportional") == 0) {
		mProportional = new DoubleValue(20.3207413833929);
		vm->addValue("/AngularMotor/PID/Proportional", mProportional);
	}
	else {
		mProportional = vm->getDoubleValue("/AngularMotor/PID/Proportional");
	}
	
	if(vm->getValue("/AngularMotor/SensorNoiseDeviation") == 0) {
		mSensorNoiseDeviationValue = new DoubleValue(0.189076);
		vm->addValue("/AngularMotor/SensorNoiseDeviation", mSensorNoiseDeviationValue);
	}
	else {
		mSensorNoiseDeviationValue = vm->
		getDoubleValue("/AngularMotor/SensorNoiseDeviation");
	}
	
	if(vm->getValue("/AngularMotor/PID/Differential") == 0) {
		mDamping = new DoubleValue(0.0438401562341989);
		vm->addValue("/AngularMotor/PID/Differential", mDamping);
	}
	else {
		mDamping = vm->getDoubleValue("/AngularMotor/PID/Differential");
	}
	
	if(vm->getValue("/AngularMotor/PID/Integral") == 0) {
		mIntegral = new DoubleValue(18.2368306244975);
		vm->addValue("/AngularMotor/PID/Integral", mIntegral);
	}
	else {
		mIntegral = vm->getDoubleValue("/AngularMotor/PID/Integral");
	}
	
	if(vm->getValue("/AngularMotor/PID/HistorySize") == 0) {
		mHistorySizeValue = new IntValue(10);
		vm->addValue("/AngularMotor/PID/HistorySize", mHistorySizeValue);
	}
	else {
		mHistorySizeValue = vm->getIntValue("/AngularMotor/PID/HistorySize");
	}
	
	if(vm->getValue("/AngularMotor/FMaxFactor") == 0) {
		mFMaxFactor = new DoubleValue(1.16320990542266);
		vm->addValue("/AngularMotor/FMaxFactor", mFMaxFactor);
	}
	else {
		mFMaxFactor = vm->getDoubleValue("/AngularMotor/FMaxFactor");
	}
	
	if(vm->getValue("/AngularMotor/MaxVelocity") == 0) {
		mMaxVelocityValue = new DoubleValue(4.12277657797321);
		vm->addValue("/AngularMotor/MaxVelocity", mMaxVelocityValue);
	}
	else {
		mMaxVelocityValue = vm->getDoubleValue("/AngularMotor/MaxVelocity");
	}
	
	if(vm->getValue("/AngularMotor/StaticFriction") == 0) {
		mStaticFrictionValue = new DoubleValue(0.0164486925082403);
		vm->addValue("/AngularMotor/StaticFriction", mStaticFrictionValue);
	}
	else {
		mStaticFrictionValue = vm->getDoubleValue("/AngularMotor/StaticFriction");
	}
	
	if(vm->getValue("/AngularMotor/StaticFrictionBorder") == 0) {
		mStaticFrictionBorderValue = new DoubleValue(0.0627667593182704);
		vm->addValue("/AngularMotor/StaticFrictionBorder", mStaticFrictionBorderValue);
	}
	else {
		mStaticFrictionBorderValue = vm->
		getDoubleValue("/AngularMotor/StaticFrictionBorder");
	}
	
	if(vm->getValue("/AngularMotor/DynamicFrictionOffset") == 0) {
		mDynamicFrictionOffsetValue = new DoubleValue(0.19221690480294);
		vm->addValue("/AngularMotor/DynamicFrictionOffset", mDynamicFrictionOffsetValue);
	}
	else {
		mDynamicFrictionOffsetValue = vm->
		getDoubleValue("/AngularMotor/DynamicFrictionOffset");
	}
	
	if(vm->getValue("/AngularMotor/DynamicFriction") == 0) {
		mDynamicFrictionValue = new DoubleValue(0.082407962544635);
		vm->addValue("/AngularMotor/DynamicFriction", mDynamicFrictionValue);
	}
	else {
		mDynamicFrictionValue = vm->getDoubleValue("/AngularMotor/DynamicFriction");
	}
	
	if(vm->getValue("/AngularMotor/MotorNoise") == 0) {
		mMotorValueNoiseValue = new DoubleValue(0.0);
		vm->addValue("/AngularMotor/MotorNoise", mMotorValueNoiseValue);
	}
	else {
		mMotorValueNoiseValue = vm->getDoubleValue("/AngularMotor/MotorNoise");
	}
	mCurrentVelocityValue = new DoubleValue(0.0);
	
	addParameter("Offset", mOffsetValue);
	addParameter("CurrentVelocity", mCurrentVelocityValue);
	addParameter("EliminateNegativeTorqueRange", mEliminateNegativeTorqueRange);
	
	mTimeStepValue = vm->getDoubleValue(SimulationConstants::VALUE_TIME_STEP_SIZE);
	if(mTimeStepValue == 0) {
		Core::log("ODE_DynamixelFrictionMotorMotor: Could not find required value.");
		return;
	}
	
	mMotorValueNoise = mMotorValueNoiseValue->get();
	mDynamicFriction = mDynamicFrictionValue->get();
	mStaticFriction = mStaticFrictionValue->get();
	mDynamicFrictionOffset = mDynamicFrictionOffsetValue->get();
	mStaticFrictionBorder = mStaticFrictionBorderValue->get();
	mMaxVelocity = mMaxVelocityValue->get();
	mSensorNoiseDeviation = mSensorNoiseDeviationValue->get();
	
	mHistorySize = mHistorySizeValue->get();
	
	mMinAngle = (-150 * Math::PI) / 180.0;
	mMaxAngle = (150 * Math::PI) / 180.0;
	
	mPidProportional = mProportional->get();
	mPidIntegral = mIntegral->get();
	mPidDifferential = mDamping->get();
	
	mOffset = mOffsetValue->get();
	mTimeStepSize = mTimeStepValue->get();
}

DynamixelFrictionMotor::DynamixelFrictionMotor(const DynamixelFrictionMotor &dynamixelFrictionMotor) 
	: DynamixelMotor(dynamixelFrictionMotor)
{
	ValueManager *vm = Core::getInstance()->getValueManager();
		
	mOffsetValue = dynamic_cast<DoubleValue*>(getParameter("Offset"));
	mCurrentVelocityValue = dynamic_cast<DoubleValue*>(getParameter("CurrentVelocity"));
	mEliminateNegativeTorqueRange =
 			dynamic_cast<BoolValue*>(getParameter("EliminateNegativeTorqueRange"));
	
	if(mDesiredMotorTorqueValue != 0) {
		mDesiredMotorTorqueValue->addValueChangedListener(this);
	}
	
	mDynamicFrictionValue = vm->getDoubleValue("/AngularMotor/DynamicFriction");
	mMotorValueNoiseValue = vm->getDoubleValue("/AngularMotor/MotorNoise");
	mDynamicFrictionOffsetValue = vm->getDoubleValue("/AngularMotor/DynamicFrictionOffset");
	mStaticFrictionBorderValue = vm->getDoubleValue("/AngularMotor/StaticFrictionBorder");
	mStaticFrictionValue = vm->getDoubleValue("/AngularMotor/StaticFriction");
	mMaxVelocityValue = vm->getDoubleValue("/AngularMotor/MaxVelocity");
	mFMaxFactor = vm->getDoubleValue("/AngularMotor/FMaxFactor");
	mHistorySizeValue = vm->getIntValue("/AngularMotor/PID/HistorySize");
	mIntegral = vm->getDoubleValue("/AngularMotor/PID/Integral");
	mDamping = vm->getDoubleValue("/AngularMotor/PID/Differential");
	mProportional = vm->getDoubleValue("/AngularMotor/PID/Proportional");
	mSensorNoiseDeviationValue = vm->getDoubleValue("/AngularMotor/SensorNoiseDeviation");
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
	
	mPidProportional = mProportional->get();
	mPidIntegral = mIntegral->get();
	mPidDifferential = mDamping->get();
	
	mOffset = mOffsetValue->get();
	mTimeStepSize = mTimeStepValue->get();
}


bool DynamixelFrictionMotor::setInputValues(QList<InterfaceValue*> values)
{
	if(!DynamixelMotor::setInputValues(values)){
		return false;
	}
	if(mDesiredMotorTorqueValue != 0) {
		mDesiredMotorTorqueValue->addValueChangedListener(this);
	}
	
	return true;
}

QString DynamixelFrictionMotor::getMotorName() const {
	return "DynamixelFrictionMotor";
}



void DynamixelFrictionMotor::setupComponents() {
	HingeJoint::setup();
	mVelocity = 0.0;
	mLastPosition = 0.0;
	
	mErrorHistory.clear();
	
	mPidProportional = mProportional->get();
	mPidIntegral = mIntegral->get();
	mPidDifferential = mDamping->get();
	mHistorySize = mHistorySizeValue->get();
	
	if(mDesiredMotorTorqueValue != 0) {
		if(mEliminateNegativeTorqueRange->get()) {
			mDesiredMotorTorqueValue->setMin(-1.0);
		}
		else {
			mDesiredMotorTorqueValue->setMin(0.0);
		}
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
	
	if(mTimeStepValue != 0) {
		mStepSize = mTimeStepValue->get();
	}
	else {
		mStepSize = 0;
	}
}

void DynamixelFrictionMotor::clearComponents() {
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
double DynamixelFrictionMotor::calculateVelocity(double currentPosition, double desiredPosition) {
	
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
		Core::log("DynamixelFrictionMotor Warning: TimeStepSize was 0.0, prevented division by zero.");
		return 0.0;
	}
	
	velocity = mPidProportional
		* (currentError + iPart + mPidDifferential / mTimeStepSize * errorDistance);
	
	mLastError = currentError;
	
	return velocity;
}

/**
 * Encapsulates the DynamixelFrictionMotor-specific calculations (PID-controller and motorfriction). 
 * After setting the current motor angle, this method is tu be used to update the motor.
 */
void DynamixelFrictionMotor::calculateMotorFrictionAndVelocity() {
	
	if(mStepSize == 0.0) {
		Core::log("DynamixelFrictionMotor Warning: StepSize was 0.0, prevented division by zero!");
		return;
	}
	
	if((mCurrentPosition >= 0.0 && mLastPosition >= 0.0) || (mCurrentPosition <= 0.0 && mLastPosition <= 0.0)) {
		mVelocity = (mCurrentPosition - mLastPosition);
	}
	else {
		mVelocity = -(mLastPosition + mCurrentPosition);
	}
	
	//allow sign changes (rotational joints)

	mVelocity = mVelocity / mStepSize;
	
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
	
	if(mIncludeNoise->get() == true)
	{
		calcVel = Math::calculateGaussian(calcVel, mMotorValueNoise);
	}
	
	mCurrentVelocityValue->set(calcVel);
	mCalculatedVelocity = calcVel;
	
	mLastPosition = mCurrentPosition;
}

/**
 * Calcluated the total motor friction (static and dynamic) that is modelled by a simple
 * linear friction model. 
 */
double DynamixelFrictionMotor::calculateFriction(double velocity) {
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

} // namespace nerd
