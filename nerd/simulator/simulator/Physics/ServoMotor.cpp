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

#include "ServoMotor.h"
#include "Core/Core.h"
#include "Math/Math.h"
#include "NerdConstants.h"
#include "SimulationConstants.h"
#include <QDebug>
#include <iostream>

using namespace std;

namespace nerd {

ServoMotor::ServoMotor(const QString &name) : EventListener(), 
		SimSensor(), SimActuator(), HingeJoint(name), mIsInitialized(false), mResetEvent(0)
{
	mPID_Proportional = new DoubleValue(1.0);
	mPID_Integral = new DoubleValue(0.0);
	mPID_Differential = new DoubleValue(0.0);
	mMinAngleValue = new DoubleValue(-180.0);
	mMaxAngleValue = new DoubleValue(180.0);
	mMaxTorque = new DoubleValue(1.0);
	mControlMotorAngle = new BoolValue(true);
	mDesiredMotorSetting = new InterfaceValue(getName(), "DesiredSetting",
								0, -10.0, 10.0);
	mDesiredMotorSetting->setNormalizedMin(-1.0);
	mDesiredMotorSetting->setNormalizedMax(1.0);
	mMotorAngleSensor = new InterfaceValue(getName(), "MotorAngle", 0,-180.0, 180.0);
	mMotorAngleSensor->setNormalizedMin(-1.0);
	mMotorAngleSensor->setNormalizedMax(1.0);
	mHistorySizeValue = new IntValue(10);
	mFMaxValue = new DoubleValue(1.0);
	
	mInputValues.append(mDesiredMotorSetting);
	mOutputValues.append(mMotorAngleSensor);

	mOffsetValue = new DoubleValue(0.0);
	mOffset = mOffsetValue->get();

	mSensorNoiseValue = new DoubleValue(0.00033);

	addParameter("PID_P", mPID_Proportional);
	addParameter("PID_I", mPID_Integral);
	addParameter("PID_D", mPID_Differential);
	addParameter("MinAngle", mMinAngleValue);
	addParameter("MaxAngle", mMaxAngleValue);
	addParameter("MaxTorque", mMaxTorque);
	addParameter("IsAngularMotor", mControlMotorAngle);
	addParameter("DesiredSetting", mDesiredMotorSetting);
	addParameter("MotorAngle", mMotorAngleSensor);
	addParameter("Offset", mOffsetValue);
	addParameter("SensorNoise", mSensorNoiseValue);	
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


ServoMotor::ServoMotor(const ServoMotor &joint) : Object(), ValueChangedListener(),
	 EventListener(), SimSensor(), SimActuator(), HingeJoint(joint), mResetEvent(0)
{
	mPID_Proportional = dynamic_cast<DoubleValue*>(getParameter("PID_P"));
	mPID_Integral = dynamic_cast<DoubleValue*>(getParameter("PID_I"));
	mPID_Differential = dynamic_cast<DoubleValue*>(getParameter("PID_D"));
	mMinAngleValue = dynamic_cast<DoubleValue*>(getParameter("MinAngle"));
	mMaxAngleValue = dynamic_cast<DoubleValue*>(getParameter("MaxAngle"));
	mMaxTorque = dynamic_cast<DoubleValue*>(getParameter("MaxTorque"));
	mSensorNoiseValue = dynamic_cast<DoubleValue*>(getParameter("SensorNoise"));
	mOffsetValue = dynamic_cast<DoubleValue*>(getParameter("Offset"));
	mControlMotorAngle = dynamic_cast<BoolValue*>(getParameter("IsAngularMotor"));
	mDesiredMotorSetting = dynamic_cast<InterfaceValue*>(getParameter("DesiredSetting"));
	mMotorAngleSensor = dynamic_cast<InterfaceValue*>(getParameter("MotorAngle"));
	mHistorySizeValue = dynamic_cast<IntValue*>(getParameter("HistorySize"));
	mFMaxValue = dynamic_cast<DoubleValue*>(getParameter("MaxForce"));
	
	mInputValues.append(mDesiredMotorSetting);
	mOutputValues.append(mMotorAngleSensor);

	mTimeStepSize = dynamic_cast<DoubleValue*>(
		Core::getInstance()->getValueManager()->getValue(SimulationConstants::VALUE_TIME_STEP_SIZE));
	if(mTimeStepSize == 0) {
		Core::log("ODE_TorqueDrivenDynamixel: Could not find required value: \""
			"/Simulation/TimeStepSize\".");
	}	

	mIsInitialized = false;
}


ServoMotor::~ServoMotor() {
	if(mResetEvent != 0) {
		mResetEvent->removeEventListener(this);
	}
}


SimObject* ServoMotor::createCopy() const {
	return new ServoMotor(*this);
}

void ServoMotor::initialize() {

	mResetEvent = Core::getInstance()->getEventManager()->
		registerForEvent(NerdConstants::EVENT_EXECUTION_RESET, this);
	if(mResetEvent == 0) {
		Core::log(QString("ServoMotor: Could not find required event: [")
			.append(NerdConstants::EVENT_EXECUTION_RESET)
			.append("]! [IGNORING]"));
	}
	mIsInitialized = true;
}

void ServoMotor::setup() {
	HingeJoint::setup();
	SimJoint::setup();
	if(!mIsInitialized) {
		initialize();
	}
}

void ServoMotor::clear() {
	HingeJoint::clear();
	SimJoint::clear();
}
		
void ServoMotor::valueChanged(Value *value) {
	SimJoint::valueChanged(value);
}

void ServoMotor::eventOccured(Event *event) {
	if(event == mResetEvent) {
		mOffset = mOffsetValue->get();
		mMinAngle = (mMinAngleValue->get() * Math::PI) / 180.0;
		mMaxAngle = (mMaxAngleValue->get() * Math::PI) / 180.0;
		mProportional = mPID_Proportional->get();
		mIntegral = mPID_Integral->get();
		mDamping = mPID_Differential->get();
		mSensorNoise = mSensorNoiseValue->get();
		mLastError = 0.0;
		mHistorySize = mHistorySizeValue->get();
		mErrorHistory.clear();
	}
}

QString ServoMotor::getName() const {
	return mNameValue->get();
} 

void ServoMotor::controlMotorAngle(bool controlAngle) {
	mControlMotorAngle->set(controlAngle);
}


bool ServoMotor::isControllingMotorAngle() const {
	return mControlMotorAngle->get();
}

void ServoMotor::updateActuators() {
}


/**
 * Update the sensor value of the ServoMotor. A ServoMotor offers an angle 
 * sensor to report the current angle of the hinge joint. Using the Parameter 
 * "SensorNoise", noise is added to the value physically calculated. The variable 
 * "mCurrentMotorAngle" is supposed to carry the currently calid angle (in radians) 
 * that is reported back from the physics engine.
 */
void ServoMotor::updateSensorValues() {
}

double ServoMotor::calculatedTorque(double, double currentPosition) {
	double motorInput = mDesiredMotorSetting->get();
	double torque = 0.0;
	if(mControlMotorAngle->get()) {
		motorInput = motorInput * 180.0;
		double error = motorInput - (currentPosition * 180.0 / Math::PI);
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
		}
		else {
			iPart= mTimeStepSize->get() / mPID_Integral->get() * currentI;
		}
		torque = mPID_Proportional->get()
				* (error + iPart + mPID_Differential->get() 
				/ mTimeStepSize->get() * errorDistance);
		mLastError = error;
	}
	return torque;
	
}

void ServoMotor::setName(const QString &name) {
	HingeJoint::setName(name);
}

}





