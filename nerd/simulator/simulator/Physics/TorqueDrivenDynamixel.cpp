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



#include "TorqueDrivenDynamixel.h"
#include "Math/Math.h"
#include "Core/Core.h"
#include <math.h>
#include "SimulationConstants.h"
#include "NerdConstants.h"	

namespace nerd
{

TorqueDrivenDynamixel::TorqueDrivenDynamixel(const QString &name) 
	: HingeJoint(name),	mUpdateMotors(0), mUpdateSensors(0), mResetEvent(0)
{
	Core *core = Core::getInstance();

	mDesiredMotorSetting = new InterfaceValue(getName(), "DesiredTorque",
			0.0, -1.0, 1.0);
	addParameter("DesiredTorque", mDesiredMotorSetting);

	mJointAngle = new InterfaceValue(getName(), "JointAngle", 0.0, -150.0, 150.0);
	mJointAngle->setNormalizedMin(-1.0);
	mJointAngle->setNormalizedMax(1.0);
	addParameter("MotorAngle", mJointAngle);

	mMinAngleValue = new DoubleValue(-150.0);
	addParameter("MinAngle", mMinAngleValue);

	mMaxAngleValue = new DoubleValue(150.0);
	addParameter("MaxAngle", mMaxAngleValue);

	mMinAngle = mMinAngleValue->get() * Math::PI / 180.0;
	mMaxAngle = mMaxAngleValue->get() * Math::PI / 180.0;

	mInputValues.append(mDesiredMotorSetting);
	mOutputValues.append(mJointAngle);

	mOffsetValue = new DoubleValue(0.0);
	mOffset = mOffsetValue->get();
	addParameter("Offset", mOffsetValue);

	mSensorNoiseValue = new DoubleValue(0.00033);
	addParameter("SensorNoise", mSensorNoiseValue);

	mUpdateSensors = core->getEventManager()->registerForEvent(
		SimulationConstants::EVENT_SIMULATION_UPDATE_SENSORS, this);
	if(mUpdateSensors == 0)	{
		Core::log("TorqueDrivenDynamixel: Could not register for required event: \""
			"SimulationConstants::EVENT_SIMULATION_UPDATE_SENSORS\".");
	}
	mUpdateMotors = core->getEventManager()->registerForEvent(
		SimulationConstants::EVENT_SIMULATION_UPDATE_MOTORS, this);
	if(mUpdateMotors == 0) {
		Core::log("TorqueDrivenDynamixel: Could not find required event: \""
			"SimulationConstants::EVENT_SIMULATION_UPDATE_MOTORS\".");
	}
	mResetEvent = core->getEventManager()
		->registerForEvent(NerdConstants::EVENT_EXECUTION_RESET, this);
	if(mResetEvent == 0) {
		Core::log(QString("TorqueDrivenDynamixel: Could not register for required event [")
				.append(NerdConstants::EVENT_EXECUTION_RESET).append("]! [IGNORING]"));
	}
	mTimeStepSize = dynamic_cast<DoubleValue*> (core->getValueManager()
						->getValue(SimulationConstants::VALUE_TIME_STEP_SIZE));
	if(mTimeStepSize == 0) {
		Core::log("TorqueDrivenDynamixel: Could not find required value: \"/Simulation/"
			"TimeStepSize\".");
	}
	mIterationSteps = dynamic_cast<IntValue*> 
		(core->getValueManager()->getValue("/Simulation/IterationsPerStep"));
	if(mIterationSteps == 0) {
		Core::log("TorqueDrivenDynamixel: Could not find required value: \"/Simulation"
			"/IterationsPerStep\".");
	}
}

TorqueDrivenDynamixel::TorqueDrivenDynamixel(const TorqueDrivenDynamixel &joint) 
		: Object(), ValueChangedListener(), EventListener(), HingeJoint(joint),
		  mUpdateMotors(0), mUpdateSensors(0), mResetEvent(0)
{
	mDesiredMotorSetting = dynamic_cast<InterfaceValue*>(getParameter("DesiredTorque"));
	mJointAngle = new InterfaceValue(*(joint.getOutputValues().at(0)));

	mDesiredMotorSetting->setInterfaceName("DesiredTorque");

	mInputValues.append(mDesiredMotorSetting);
	mOutputValues.append(mJointAngle);

	mSensorNoiseValue = dynamic_cast<DoubleValue*> (getParameter( "SensorNoise"));
	mOffsetValue = dynamic_cast<DoubleValue*> (getParameter("Offset"));
	mOffset = mOffsetValue->get();
	mUpdateSensors = Core::getInstance()->getEventManager()->registerForEvent( SimulationConstants::EVENT_SIMULATION_UPDATE_SENSORS, this);
	if(mUpdateSensors == 0)	{
		Core::log("TorqueDrivenDynamixel: Could not register for required event: \""
			"SimulationConstants::EVENT_SIMULATION_UPDATE_SENSORS\".");
	}
	mUpdateMotors = Core::getInstance()->getEventManager()->registerForEvent( SimulationConstants::EVENT_SIMULATION_UPDATE_MOTORS, this);
	if(mUpdateMotors == 0) {
		Core::log("TorqueDrivenDynamixel: Could not find required event: \""
			"SimulationConstants::EVENT_SIMULATION_UPDATE_MOTORS\"." );
	}
	mResetEvent = Core::getInstance()->getEventManager()
		->registerForEvent(NerdConstants::EVENT_EXECUTION_RESET, this);
	if(mResetEvent == 0) {
		Core::log(QString("TorqueDrivenDynamixel: Could not register for required event [")
				.append(NerdConstants::EVENT_EXECUTION_RESET).append("]! [IGNORING]"));
	}
	mTimeStepSize = dynamic_cast<DoubleValue*> (Core::getInstance()->getValueManager()
									->getValue(SimulationConstants::VALUE_TIME_STEP_SIZE));
	if(mTimeStepSize == 0) {
		Core::log("TorqueDrivenDynamixel: Could not find required value: \""
			"/Simulation/TimeStepSize\".");
	}
	mIterationSteps = dynamic_cast<IntValue*> (Core::getInstance()->getValueManager()->getValue("/Simulation/IterationsPerStep"));
	if(mIterationSteps == 0) {
		Core::log("TorqueDrivenDynamixel: Could not find required value: \""
			"/Simulation/IterationsPerStep\".");
	}
}

TorqueDrivenDynamixel::~TorqueDrivenDynamixel() {
	if(mUpdateSensors != 0) {
		mUpdateSensors->removeEventListener(this);
	}
	if(mUpdateMotors != 0) {
		mUpdateMotors->removeEventListener(this);
	}
	if(mResetEvent != 0) {
		mResetEvent->removeEventListener(this);
	}
}

SimObject* TorqueDrivenDynamixel::createCopy() const {
	return new TorqueDrivenDynamixel(*this);
}

void TorqueDrivenDynamixel::setup() {
	HingeJoint::setup();
	mOffset = mOffsetValue->get();
	setDynamixelParameter();
	mMinAngle = (mMinAngleValue->get() * Math::PI) / 180.0;
	mMaxAngle = (mMaxAngleValue->get() * Math::PI) / 180.0;

	mH = mTimeStepSize->get();
}

void TorqueDrivenDynamixel::eventOccured(Event *event) {
	if(event == mUpdateMotors) {
		updateMotors();
	}
	else if(event == mUpdateSensors) {
		updateSensors();
	}
}

QString TorqueDrivenDynamixel::getName() const {
	return mNameValue->get();
}

void TorqueDrivenDynamixel::updateMotors() {
}

void TorqueDrivenDynamixel::updateSensors() {
}


void TorqueDrivenDynamixel::setDynamixelParameter() {
// 	mSimulationTime = 0.0;
	mO = 0;
// 	mFriction = 0;
	mBeta = 0;
	mE = 0;
	mReport = 0;
	mZp = 0;
	mG = 0;
	mSgnv = 0;
	mM = 0.0;
	mJ = 0.016412;
	mM_max = 1.834058;

// 	mMax_torque = mM_max;

	mPi56 = 6.0/ (5.0*3.1415927);

	mE0 = 7.3132;
	mE1 = 0.5776;
	mE2 = 0.9291;
	mC1 = 1.6962;
	mC2 = 38.2799;

	mH = 0.01;

	mExp1 = exp (-(mE1 / mC1) * mH);
	mExp2 = exp (-(mE2 / mC2) * mH);

	mP = 0.0;
	mW = 0.0;

	mZ1 = 0.0;
	mZ2 = 0.0;

// friction parameter
	mZ = 0.00001;
// 	eF = 2.71828;
	mFc = 0.091992;
	mFs = 0.199354;
	mFv = 0.290498;
	mSigma0 = 40.064335;
	mSigma1 = 2.0 * sqrt(mSigma0 * mJ);
	mVS = 0.100232;
	mVD = 1.494997;
	mDS = 1.0;

}

double TorqueDrivenDynamixel::getFrictionMoment(double currentVelocity) {
	double frictionMoment;
	double lExp1;
	double lExp2;
	double lExp3;
	double an;
	double bn;

	if(currentVelocity > 0) {
		mSgnv = 1;
	}
	else if(currentVelocity < 0) {
		mSgnv = -1;
	}
	else {
		mSgnv = 0;
	}

	if(mVS == 0.0 || mVD == 0.0 || mG) {
		Core::log("TorqueDrivenDynamixel Warning: mVS, mVD or mG was 0.0, prevented division by zero.");
		return 0.0;
	}

	lExp1 = exp(-(pow((currentVelocity / mVS ), 2)));
	lExp2 = exp(-(pow((currentVelocity / mVD ), 2)));
	mG = (mFc + (mFs - mFc) * lExp1);
	an = mSigma0 * (fabs(currentVelocity) / mG);
	bn = mSigma0 * currentVelocity;
	lExp3 = exp(-an * mH);
	mZp = bn - an * mZ;
	if(an == 0) {
		mZ = mH * bn + mZ;
	}
	else {
		mZ = mZ * lExp3 + (1 - lExp3) * bn / an;
	}
	if(mSigma0 == 0.0) {
		Core::log("TorqueDrivenDynamixel Warning: mSigma0 was 0.0, prevented division by zero.");
		return 0.0;
	}
	frictionMoment = mZ + ((mSigma1 * lExp2) / mSigma0) * mZp + mFv 
					* (pow(fabs(currentVelocity), mDS)) * mSgnv;

	return frictionMoment;
}

}
