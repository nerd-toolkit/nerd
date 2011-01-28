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

#include "SliderMotor.h"
#include "Value/BoolValue.h"
#include "Core/Core.h"
#include "SimulationConstants.h"
#include "NerdConstants.h"
#include "Math/Math.h"

namespace nerd {

SliderMotor::SliderMotor(const QString &name) 
	: EventListener(), SimSensor(), SimActuator(), 
	  SliderJoint(name), mIsInitialized(false), mResetEvent(0) 
{
	mLastMotorPosition = 0.0;
	mCurrentMotorPosition = 0.0;
	mVelocity = 0.0;

	mControlMotorPosition = new BoolValue(true);
	mSensorNoiseValue = new DoubleValue(0.00033);
	mMaxVelocityValue = new DoubleValue(1.0);
	mPID_Proportional = new DoubleValue(1.0);
	mPID_Integral = new DoubleValue(0.0);
	mPID_Differential = new DoubleValue(0.0);
	mHistorySizeValue = new IntValue(10);
	mStaticFrictionValue = new DoubleValue(0.001);
	mStaticFrictionBorderValue = new DoubleValue(0.1);
	mDynamicFrictionOffsetValue = new DoubleValue(0.0);
	mDynamicFrictionValue = new DoubleValue(0.16);

	mSensorNoise = mSensorNoiseValue->get();
	mMinPosition = mMinPositionValue->get();
	mMaxPosition = mMaxPositionValue->get();
	
	mStaticFriction = mStaticFrictionValue->get();
	mStaticFrictionBorder = mStaticFrictionBorderValue->get();
	mDynamicFriction = mDynamicFrictionValue->get();
	mDynamicFrictionOffset = mDynamicFrictionOffsetValue->get();

	mDesiredMotorSetting = new InterfaceValue(
		getName(), "DesiredSetting", 0.0, mMinPosition, mMaxPosition);
	mMotorPositionSensor = new InterfaceValue(
		getName(), "Position", 0.0, mMinPosition, mMaxPosition);

	mInputValues.append(mDesiredMotorSetting);
	mOutputValues.append(mMotorPositionSensor);

	mFMaxValue = new DoubleValue(1.0);

	addParameter("ControlPosition", mControlMotorPosition);
	addParameter("SensorNoise", mSensorNoiseValue);
	addParameter("DesiredSetting", mDesiredMotorSetting);
	addParameter("Position", mMotorPositionSensor);
	addParameter("MaxVelocity", mMaxVelocityValue);
	
	addParameter("Friction/StaticFriction", mStaticFrictionValue);
	addParameter("Friction/StaticBorder", mStaticFrictionBorderValue);
	addParameter("Friction/DynamicFriction", mDynamicFrictionValue);
	addParameter("Friction/DynamicOffset", mDynamicFrictionOffsetValue);

	addParameter("PID_P", mPID_Proportional);
	addParameter("PID_I", mPID_Integral);
	addParameter("PID_D", mPID_Differential);
	addParameter("HistorySize", mHistorySizeValue);
	addParameter("MaxForce", mFMaxValue);
	mHistorySize = mHistorySizeValue->get();

	mTimeStepSize = dynamic_cast<DoubleValue*>(
		Core::getInstance()->getValueManager()->getValue(SimulationConstants::VALUE_TIME_STEP_SIZE));
	if(mTimeStepSize == 0) {
		Core::log("ODE_TorqueDrivenDynamixel: Could not find required value: \""
			"/Simulation/TimeStepSize\".");
	}	
}

SliderMotor::SliderMotor(const SliderMotor &joint) 
	: Object(), ValueChangedListener(), EventListener(), SimSensor(),
	  SimActuator(), SliderJoint(joint), mResetEvent(0)
{
	mControlMotorPosition = dynamic_cast<BoolValue*>(getParameter("ControlPosition"));
	mSensorNoiseValue = dynamic_cast<DoubleValue*>(getParameter("SensorNoise"));
	mDesiredMotorSetting = dynamic_cast<InterfaceValue*>(getParameter("DesiredSetting"));
	mMotorPositionSensor = dynamic_cast<InterfaceValue*>(getParameter("Position"));	
	mMaxVelocityValue = dynamic_cast<DoubleValue*>(getParameter("MaxVelocity"));

	mStaticFrictionValue = 
		dynamic_cast<DoubleValue*>(getParameter("Friction/StaticFriction"));
	mStaticFrictionBorderValue = 		
		dynamic_cast<DoubleValue*>(getParameter("Friction/StaticBorder"));
	mDynamicFrictionValue = 
		dynamic_cast<DoubleValue*>(getParameter("Friction/DynamicFriction"));
	mDynamicFrictionOffsetValue = 
		dynamic_cast<DoubleValue*>(getParameter("Friction/DynamicOffset"));

	mPID_Proportional = dynamic_cast<DoubleValue*>(getParameter("PID_P"));
	mPID_Integral = dynamic_cast<DoubleValue*>(getParameter("PID_I"));
	mPID_Differential = dynamic_cast<DoubleValue*>(getParameter("PID_D"));
	mHistorySizeValue = dynamic_cast<IntValue*>(getParameter("HistorySize"));
	mFMaxValue = dynamic_cast<DoubleValue*>(getParameter("MaxForce"));

	mInputValues.append(mDesiredMotorSetting);
	mOutputValues.append(mMotorPositionSensor);
	mIsInitialized = false;

	mTimeStepSize = dynamic_cast<DoubleValue*>(
		Core::getInstance()->getValueManager()->getValue(SimulationConstants::VALUE_TIME_STEP_SIZE));
	if(mTimeStepSize == 0) {
		Core::log("ODE_TorqueDrivenDynamixel: Could not find required value: \""
			"/Simulation/TimeStepSize\".");
	}	
}

SliderMotor::~SliderMotor() {
	if(mResetEvent != 0) {
		mResetEvent->removeEventListener(this);
	}
}

SimObject* SliderMotor::createCopy() const {
	return new SliderMotor(*this);
}

void SliderMotor::valueChanged(Value *value) {
	SliderJoint::valueChanged(value);
}

void SliderMotor::controlMotorPosition(bool controlPosition) {
	mControlMotorPosition->set(controlPosition);
}

bool SliderMotor::isControllingMotorPosition() const {
	return mControlMotorPosition->get();
}

void SliderMotor::eventOccured(Event *event) {
	if(event == mResetEvent) {
		mSensorNoise = mSensorNoiseValue->get();
		mLastMotorPosition = 0.0;
		mMinPosition = mMinPositionValue->get();
		mMaxPosition = mMaxPositionValue->get();
		mCurrentMotorPosition = 0.0;
		mVelocity = 0.0;
		mProportional = mPID_Proportional->get();
		mIntegral = mPID_Integral->get();
		mDamping = mPID_Differential->get();
		mLastError = 0.0;
		mHistorySize = mHistorySizeValue->get();
		mErrorHistory.clear();
	
		mStaticFriction = mStaticFrictionValue->get();
		mStaticFrictionBorder = mStaticFrictionBorderValue->get();
		mDynamicFriction = mDynamicFrictionValue->get();
		mDynamicFrictionOffset = mDynamicFrictionOffsetValue->get();

		mMotorPositionSensor->setMin(mMinPosition);
		mMotorPositionSensor->setMax(mMaxPosition);
		if(mControlMotorPosition->get()) {
			mDesiredMotorSetting->setMin(mMinPosition);
			mDesiredMotorSetting->setMax(mMaxPosition);
		}
		else {
			mDesiredMotorSetting->setMin(-mMaxVelocityValue->get());
			mDesiredMotorSetting->setMax(mMaxVelocityValue->get());
		}
	}
}

QString SliderMotor::getName() const {
	return mNameValue->get();
}

void SliderMotor::setup() {
	SliderJoint::setup();
	if(!mIsInitialized) {
		initialize();
	}
}

void SliderMotor::clear() {
	SliderJoint::clear();
}

void SliderMotor::updateActuators() {
}

void SliderMotor::updateSensorValues() {
}

void SliderMotor::initialize() {
	mResetEvent = Core::getInstance()->getEventManager()->
		registerForEvent(NerdConstants::EVENT_EXECUTION_RESET, this);
	if(mResetEvent == 0) {
		Core::log(QString("SliderMotor: Could not find required event: [")
			.append(NerdConstants::EVENT_EXECUTION_RESET)
			.append("]! [IGNORING]"));
	}
	mIsInitialized = true;
}

double SliderMotor::calculateVelocity(double currentPosition) {
	double velocity = 0.0;
	double motorInput = mDesiredMotorSetting->get();
	
	double error = motorInput - currentPosition;
	double errorDistance = error - mLastError;
	double iPart;

	mErrorHistory.push_back(error);

	if(mErrorHistory.size() > mHistorySize) {
		mErrorHistory.pop_front();
	}

	double currentI = 0.0;

	for(int i = 0; i < mErrorHistory.size(); i++) {
		currentI += mErrorHistory.at(i);
	}
	if(Math::compareDoubles(mPID_Integral->get(), 0, 10)) {
		iPart= 0.0;
	}else{
		iPart= mTimeStepSize->get() / mPID_Integral->get() * currentI;
	}

	if(mTimeStepSize->get() == 0.0) {
		Core::log("SliderMotor Warning: TimeStepSize was 0.0, prevented division by zero.");
		return 0.0;
	}

	velocity = mPID_Proportional->get()
		* (error + iPart + mPID_Differential->get() / mTimeStepSize->get() * errorDistance);

	mLastError = error;
	
	return velocity;
}


double SliderMotor::calculateFriction(double velocity) {
	double friction = 0;

	if(mStaticFriction == 0.0 && mDynamicFriction == 0.0) {
		return friction;
	}
	else {
		if(velocity < mStaticFrictionBorder && velocity > -mStaticFrictionBorder) {
			friction = mStaticFriction;
		}
		friction += mDynamicFriction * Math::abs(velocity) + mDynamicFrictionOffset;
		if(friction < 0.0) {
			friction = 0.0;
		}
		return Math::abs(friction);
	}
	return friction;
}

}
