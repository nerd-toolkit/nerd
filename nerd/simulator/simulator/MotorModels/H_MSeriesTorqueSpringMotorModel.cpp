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


#include "H_MSeriesTorqueSpringMotorModel.h"
#include "MotorModels/TorqueDynamixelMotorAdapter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"
#include "SimulationConstants.h"
#include <QDebug>

using namespace std;

namespace nerd {


/**
 * Constructs a new H_MSeriesTorqueSpringMotorModel.
 */
H_MSeriesTorqueSpringMotorModel::H_MSeriesTorqueSpringMotorModel(const QString &name)
	: MSeriesTorqueSpringMotorModel(name, MotorModel::HINGE_JOINT), mCurrentJointAngle(0), mNumberOfMotors(0)
{
//	mTorqueConstant = new DoubleValue(0.08);
	mTorqueConstant = new DoubleValue(0.08);
	mSpeedConstant = new DoubleValue(0.12);
	mMotorInertia = new DoubleValue(0.001);
	mVoltage = new DoubleValue(12.0);
//	mBackEMFConstant = new DoubleValue(0.00992);
//	mTerminalResistance = new DoubleValue(8.3);
	mStaticToDynamicFrictionSpeedThreshold = new DoubleValue(0.05);
	mDynamicFrictionConstant = new DoubleValue(0.008);
	mStaticFrictionTorque = new DoubleValue(0.005);
//	mMotorGearTransmissionRatio = new DoubleValue(193.0);
	mMotorToJointTransmissionRatio = new DoubleValue(1.0);
	mJointToAngularSensorTransmissionRatio = new DoubleValue(1.0);
	mUseSpringCouplings = new BoolValue(false);
	mCurrentConsumptionVoltageConstant = new DoubleValue(-0.02);
	mCurrentConsumptionSpeedConstant = new DoubleValue(-0.034);
	mSpringConstant = new DoubleValue(9.0);
	mSpringDampingConstant = new DoubleValue(1.0);
	mSpringConstantForRigidCoupling = new DoubleValue(40.0);
	mSpringDampingConstantForRigidCoupling = new DoubleValue(10.0);
	mJointCoulombFriction = new DoubleValue(0.0);
	mGearEfficiencyFactor = new DoubleValue(0.21);

	addParameter("TorqueConstant", mTorqueConstant);
	addParameter("SpeedConstant", mSpeedConstant);
	addParameter("MotorInertia", mMotorInertia);
	addParameter("Voltage", mVoltage);
//	addParameter("BackEMFConstant", mBackEMFConstant);
//	addParameter("TerminalResistance", mTerminalResistance);
	addParameter("StaticToDynamicFrictionSpeedThreshold", mStaticToDynamicFrictionSpeedThreshold);
	addParameter("DynamicFrictionConstant", mDynamicFrictionConstant);
	addParameter("StaticFrictionTorque", mStaticFrictionTorque);
//	addParameter("MotorGearTransmissionRatio", mMotorGearTransmissionRatio);
	addParameter("MotorToJointTransmissionRatio", mMotorToJointTransmissionRatio);
	addParameter("JointToAngularSensorTransmissionRatio", mJointToAngularSensorTransmissionRatio);
	addParameter("CurrentConsumptionVoltageConstant", mCurrentConsumptionVoltageConstant);
	addParameter("CurrentConsumptionSpeedConstant", mCurrentConsumptionSpeedConstant);
	addParameter("UseSpringCouplings", mUseSpringCouplings);
	addParameter("SpringConstant", mSpringConstant);
	addParameter("SpringDampingConstant", mSpringDampingConstant);
	addParameter("SpringConstantForRigidCoupling", mSpringConstantForRigidCoupling);
	addParameter("SpringDampingConstantForRigidCoupling", mSpringDampingConstantForRigidCoupling);
	addParameter("JointCoulombFriction", mJointCoulombFriction);
	addParameter("GearEfficiencyFactor", mGearEfficiencyFactor);
}


/**
 * Copy constructor. 
 * 
 * @param other the MSeriesTorqueSpringMotorModel object to copy.
 */
H_MSeriesTorqueSpringMotorModel::H_MSeriesTorqueSpringMotorModel(const H_MSeriesTorqueSpringMotorModel &other) 
	: Object(other), ValueChangedListener(other), MSeriesTorqueSpringMotorModel(other), mCurrentJointAngle(0), mNumberOfMotors(0)
{
	mTorqueConstant = dynamic_cast<DoubleValue*>(getParameter("TorqueConstant"));
	mSpeedConstant = dynamic_cast<DoubleValue*>(getParameter("SpeedConstant"));
	mMotorInertia = dynamic_cast<DoubleValue*>(getParameter("MotorInertia"));
	mVoltage = dynamic_cast<DoubleValue*>(getParameter("Voltage"));
//	mTerminalResistance = dynamic_cast<DoubleValue*>(getParameter("TerminalResistance"));
//	mBackEMFConstant = dynamic_cast<DoubleValue*>(getParameter("BackEMFConstant"));
	mStaticToDynamicFrictionSpeedThreshold = dynamic_cast<DoubleValue*>(getParameter("StaticToDynamicFrictionSpeedThreshold"));
	mDynamicFrictionConstant = dynamic_cast<DoubleValue*>(getParameter("DynamicFrictionConstant"));
	mStaticFrictionTorque = dynamic_cast<DoubleValue*>(getParameter("StaticFrictionTorque"));
//	mMotorGearTransmissionRatio = dynamic_cast<DoubleValue*>(getParameter("MotorGearTransmissionRatio"));
	mMotorToJointTransmissionRatio = dynamic_cast<DoubleValue*>(getParameter("MotorToJointTransmissionRatio"));
	mJointToAngularSensorTransmissionRatio = dynamic_cast<DoubleValue*>(getParameter("JointToAngularSensorTransmissionRatio"));
	mCurrentConsumptionVoltageConstant = dynamic_cast<DoubleValue*>(getParameter("CurrentConsumptionVoltageConstant"));
	mCurrentConsumptionSpeedConstant = dynamic_cast<DoubleValue*>(getParameter("CurrentConsumptionSpeedConstant"));
	mUseSpringCouplings = dynamic_cast<BoolValue*>(getParameter("UseSpringCouplings"));
	mSpringConstant = dynamic_cast<DoubleValue*>(getParameter("SpringConstant"));
	mSpringDampingConstant = dynamic_cast<DoubleValue*>(getParameter("SpringDampingConstant"));
	mSpringConstantForRigidCoupling = dynamic_cast<DoubleValue*>(getParameter("SpringConstantForRigidCoupling"));
	mSpringDampingConstantForRigidCoupling = dynamic_cast<DoubleValue*>(getParameter("SpringDampingConstantForRigidCoupling"));
	mJointCoulombFriction = dynamic_cast<DoubleValue*>(getParameter("JointCoulombFriction"));
	mGearEfficiencyFactor = dynamic_cast<DoubleValue*>(getParameter("GearEfficiencyFactor"));
	
	
}

/**
 * Destructor.
 */
H_MSeriesTorqueSpringMotorModel::~H_MSeriesTorqueSpringMotorModel() {
}

bool H_MSeriesTorqueSpringMotorModel::setOwner(MotorAdapter *owner) {
	if(!MSeriesTorqueSpringMotorModel::setOwner(owner)) {
		return false;
	}

	TorqueDynamixelMotorAdapter *castOwner = dynamic_cast<TorqueDynamixelMotorAdapter*>(owner);

	if(castOwner != 0) {
		mNumberOfMotors = castOwner->getNumberOfTorqueInputValues();

		for(int i = 0; i < mNumberOfMotors; i++) {
			mCurrentMotorPositions.append(0.0);
			mLastMotorPositions.append(0.0);
			mCurrentMotorSpeeds.append(0.0);
			mLastSpringTensions.append(0.0);

			//TODO remove after model tests.
			mTmpTorqueSpring.append(new DoubleValue());
			addParameter("tmp/TorqueSpring" + QString::number(i), mTmpTorqueSpring.last(), true);
			mTmpTorqueComplete.append(new DoubleValue());
			addParameter("tmp/TorqueComplete" + QString::number(i), mTmpTorqueComplete.last(), true);
			mTmpTorquWithoutFriction.append(new DoubleValue());
			addParameter("tmp/TorqueWithoutFriction" + QString::number(i), mTmpTorquWithoutFriction.last(), true);
			mTmpTorqueFromMotor.append(new DoubleValue());
			addParameter("tmp/TorqueFromMotor" + QString::number(i), mTmpTorqueFromMotor.last(), true);

			DoubleValue *jointToMotorDisplacement = dynamic_cast<DoubleValue*>(getParameter("JointToMotorDisplacement" + QString::number(i)));
			if(jointToMotorDisplacement == 0) {
				jointToMotorDisplacement = new DoubleValue(0.0);
				addParameter("JointToMotorDisplacement" + QString::number(i), jointToMotorDisplacement);
			}
			mJointToMotorDisplacements.append(jointToMotorDisplacement);
		}

		mLastSpringTensionWithoutMotor = 0.0;

/*		// Add the values which should be used globally by all Copies of these prototype
		mSpringConstant = createGlobalPrototypeDoubleValue(castOwner, "SpringConstant", 9.0);
		mSpringDampingConstant = createGlobalPrototypeDoubleValue(castOwner, "SpringDampingConstant", 1.0);
		mSpringConstantForRigidCoupling = createGlobalPrototypeDoubleValue(castOwner, "SpringConstantForRigidCoupling", 40.0);
		mSpringDampingConstantForRigidCoupling = createGlobalPrototypeDoubleValue(castOwner, "SpringDampingConstantForRigidCoupling", 10.0);
*/
	}

	return true;
}

void H_MSeriesTorqueSpringMotorModel::setup() {
	MSeriesTorqueSpringMotorModel::setup();

	for(int i = 0; i < mNumberOfMotors; i++) {
		mCurrentMotorPositions[i] = 0.0;
		mLastMotorPositions[i] = 0.0;
		mCurrentMotorSpeeds[i] = 0.0;
		mLastSpringTensions[i] = 0.0;
	}
}


void H_MSeriesTorqueSpringMotorModel::clear() {
	MSeriesTorqueSpringMotorModel::clear();
}

void H_MSeriesTorqueSpringMotorModel::updateInputValues() {

}


void H_MSeriesTorqueSpringMotorModel::updateOutputValues() {
	TorqueDynamixelMotorAdapter *owner = dynamic_cast<TorqueDynamixelMotorAdapter*>(mOwner);

	if(owner != 0) {
		// Get Output Values and Parameters from owner
		QList<InterfaceValue*> motorAngleSensorValues = owner->getMotorAngleSensorValues();
		QList<DoubleValue*> currentConsumptionSensorValues = owner->getCurrentConsumptionValues();
		QList<BoolValue*> motorFlipStates = owner->getMotorFlipStateValues();

		for(int i = 0; i < mNumberOfMotors; i++) {
			double flipped = motorFlipStates.at(i)->get() ? -1.0 : 1.0;
			if(mJointToMotorDisplacements.size() >= i + 1) {
				motorAngleSensorValues.at(i)->set(mCurrentMotorPositions.at(i) * flipped 
							* 180.0 / Math::PI - mJointToMotorDisplacements.at(i)->get());
			}
		}
	}
}

/**
 * This function is the main function of the model. It calculates the torque that all motors and springs involved
 * in the whole motor apply to the joint. The friction of the joint itself is included in this torque.
 * .............................. ETC ETC
 * TODO
 */
double H_MSeriesTorqueSpringMotorModel::calculateJointTorque() {


	double jointTorque = 0.0;

	TorqueDynamixelMotorAdapter *owner = dynamic_cast<TorqueDynamixelMotorAdapter*>(mOwner);

	if(owner != 0) {
		// Get Input Values from owner
		QList<InterfaceValue*> torqueInputValues = owner->getTorqueInputValues();
		QList<InterfaceValue*> freerunInputValues = owner->getFreerunInputValues();
		QList<DoubleValue*> currentConsumptionValues = owner->getCurrentConsumptionValues();
		QList<BoolValue*> flippedStateValues = owner->getMotorFlipStateValues();
		QList<DoubleValue*> individualMotorTorqueValues = owner->getIndividualMotorTorqueValues();

		// Get timeStepSize for speed and acceleration calculation
		double timeStepSize = Core::getInstance()->getValueManager()->getDoubleValue(SimulationConstants::VALUE_TIME_STEP_SIZE)->get();

		for(int i = 0; i < mNumberOfMotors; i++) {
			//flip motor output when the motor is flipped
			double flipFactor = flippedStateValues.at(i)->get() ? -1.0 : 1.0;

			// 1. Compute the tension of the spring from the current motor
			double currentSpringTension = mCurrentMotorPositions[i] - mCurrentJointAngle / mMotorToJointTransmissionRatio->get();
			// 2. Compute the speed of the spring tension change for spring damping
			double springTensionSpeed = currentSpringTension - mLastSpringTensions[i];
			mLastSpringTensions[i] = currentSpringTension;
			// 3. Compute the torque that the spring of the current motor applies to the motor
			double torqueSpring;
			if(mUseSpringCouplings->get()) {
				torqueSpring = currentSpringTension * mSpringConstant->get() + springTensionSpeed * mSpringDampingConstant->get();
			} else {
				torqueSpring = currentSpringTension * mSpringConstantForRigidCoupling->get() + springTensionSpeed * mSpringDampingConstantForRigidCoupling->get();
			}
			mTmpTorqueSpring.at(i)->set(torqueSpring); //TODO remove

			// 4. Compute the torque that the motor produces itself depending on the freerunValue
			double torqueFromMotor;
			if(owner->isUsingFreerunInputValues() && freerunInputValues.at(i)->get() > 0) {	// Freerun used and set?
				torqueFromMotor = 0.0;
			} else {
				torqueFromMotor = mTorqueConstant->get() * (torqueInputValues.at(i)->get() * flipFactor) * mVoltage->get() - mSpeedConstant->get() * mCurrentMotorSpeeds.at(i);
			}
			mTmpTorqueFromMotor.at(i)->set(torqueFromMotor); //TODO remove
			// 5. Compute the complete torque that is applied to the motor without friction. This is the torque from the motor itself and the torque
			// of the spring
			double torqueWithoutFriction = 0.0;
			// If the spring torque is working against the new motor speed, then the spring works against the motor gear and
			// the efficiency factor of the gear has to be taken into account
			// Then the torques and the newMotorSpeed have to be recomputed.
			double newMotorSpeed = 0.0;
			bool recompute = false;
			do {
				if(recompute) {
					// Compute the torqueWithoutFriction with the GearEfficiencyFactor taken into account
					torqueWithoutFriction = torqueFromMotor - torqueSpring * mGearEfficiencyFactor->get();
				} else {
					torqueWithoutFriction = torqueFromMotor - torqueSpring;
				}
				mTmpTorquWithoutFriction.at(i)->set(torqueWithoutFriction); //TODO remove
				// 6. Compute resulting complete torque with friction
				double torqueComplete;
				if(Math::abs(mCurrentMotorSpeeds.at(i)) < mStaticToDynamicFrictionSpeedThreshold->get()) {	// Static Friction?
					if(Math::abs(torqueWithoutFriction) <= mStaticFrictionTorque->get()) {
						torqueComplete = 0;
						mCurrentMotorSpeeds[i] *= 0.9;		// To avoid motor drift in static friction
					} else {	// Dynamic Friction
						torqueComplete = torqueWithoutFriction - mDynamicFrictionConstant->get() * mCurrentMotorSpeeds.at(i);
					}
				} else {
					torqueComplete = torqueWithoutFriction - mDynamicFrictionConstant->get() * mCurrentMotorSpeeds.at(i);
				}
				mTmpTorqueComplete.at(i)->set(torqueComplete); //TODO remove
				// 7. Compute the current acceleration of the current motor from the diff-equation J*acc = torqueComplete
				double currentAcc = torqueComplete / mMotorInertia->get();
				// Cut of unrealistic accelerations
				if(currentAcc > 200.0) {
					currentAcc = 200.0;
				}
				if(currentAcc < -200.0) {
					currentAcc = -200.0;
				}
				// 8. Compute the new Motor Speed
				double oldMotorSpeed = newMotorSpeed;
				newMotorSpeed = mCurrentMotorSpeeds[i] + currentAcc * timeStepSize;
				if(recompute) {
					// The Speed direction cannot change becauce of the gear efficiency factor!
					if(((oldMotorSpeed < 0.0) && (newMotorSpeed > 0.0)) || ((oldMotorSpeed > 0.0) && (newMotorSpeed < 0.0))) {
						newMotorSpeed = 0.0;
					}
					recompute = false;
				} else {
					// Spring torque working against new motor speed?
					if(((newMotorSpeed < 0.0) && (torqueFromMotor >= 0.0)) ||
					   ((newMotorSpeed > 0.0) && (torqueFromMotor <= 0.0))) 
					{
						recompute = true;
					}
				}
			} while(recompute);
			// Avoid fast oscillations of motor speed
			if(mCurrentMotorSpeeds[i] < 0.0 && newMotorSpeed > 0.0) {
				newMotorSpeed = 0.0001;
			}
			if(mCurrentMotorSpeeds[i] > 0.0 && newMotorSpeed < 0.0) {
				newMotorSpeed = -0.0001;
			}
			// Cut of unrealistic speeds
			if(newMotorSpeed > 20.0) {
				newMotorSpeed = 20.0;
			}
			if(newMotorSpeed < -20.0) {
				newMotorSpeed = -20.0;
			}
			// 9. Update state variables
			mCurrentMotorSpeeds[i] = newMotorSpeed;
			mLastMotorPositions[i] = mCurrentMotorPositions.at(i);
			mCurrentMotorPositions[i] += mCurrentMotorSpeeds.at(i) * timeStepSize;

			// Compute the torque that the current motor applies to the joint and add
			// this torque to the total joint torque
			double torqueFromCurrentMotor = torqueSpring / mMotorToJointTransmissionRatio->get();
			jointTorque += torqueFromCurrentMotor;

			if(i < individualMotorTorqueValues.size()) {
				individualMotorTorqueValues[i]->set(torqueFromCurrentMotor);
			}
	

			// Compute the current consumption value for this motor
			if(currentConsumptionValues.size() > 0) {
				if(owner->isUsingFreerunInputValues() && freerunInputValues.at(i)->get() > 0) {	// Freerun used and set?
					currentConsumptionValues[i]->set(-Math::abs(mCurrentConsumptionSpeedConstant->get() * mCurrentMotorSpeeds.at(i)));
				} else {
					if(Math::compareDoubles(torqueInputValues.at(i)->get(), 0.0, 0.0001)) {	// Motor short-circuit at zero torque
						currentConsumptionValues[i]->set(0.0);
					} else {
						currentConsumptionValues[i]->set(Math::abs(torqueInputValues.at(i)->get() * mVoltage->get() * mCurrentConsumptionVoltageConstant->get() -
								mCurrentConsumptionSpeedConstant->get() * mCurrentMotorSpeeds.at(i)));
					}
				}
			}
		}

		// No Motors, just spring coupling
		if(mNumberOfMotors == 0) {
			double currentSpringTension = - mCurrentJointAngle / mMotorToJointTransmissionRatio->get();
			double springTensionSpeed = currentSpringTension - mLastSpringTensionWithoutMotor;
			mLastSpringTensionWithoutMotor = currentSpringTension;
			double torqueSpring;
			if(mUseSpringCouplings->get()) {
				torqueSpring = currentSpringTension * mSpringConstant->get() + springTensionSpeed * mSpringDampingConstant->get();
			} else {
				torqueSpring = currentSpringTension * mSpringConstantForRigidCoupling->get() + springTensionSpeed * mSpringDampingConstantForRigidCoupling->get();
			}
			jointTorque = torqueSpring / mMotorToJointTransmissionRatio->get();
		}
	}

	// Catch non-valid values before they are transmitted to the physics engine.
	if(jointTorque != jointTorque) {
		jointTorque = 0.0;
	}
	if(jointTorque > 50.0) {
		jointTorque = 50.0;
	}
	if(jointTorque < -50.0) {
		jointTorque = -50.0;
	}

	return jointTorque;
}

/**
 * Calculates the current sensor angle ([-180, 180] degrees) from the current joint angle (in radians).
 * This method consideres the transmission ration between the joint angle and the (potentiometer) angle
 * sensor and an optional sensor angle offset.
 * Output angles are only defines in the range [-170, 170]. Higher or lower values can derive from the 
 * desired plausible behavior.
 */
double H_MSeriesTorqueSpringMotorModel::calculateAngularSensor(double currentJointAngleInRadians) {

	TorqueDynamixelMotorAdapter *owner = dynamic_cast<TorqueDynamixelMotorAdapter*>(mOwner);
	
	if(owner == 0) {
		return 0.0;
	}
	//Note: The angle sensor offset has to be considered here to allow the
			//adjustment of the joint angle sensor potis.
	DoubleValue *mJointAngleSensorOffset = owner->getJointAngleOffsetValue();
			
	if(mJointAngleSensorOffset != 0) {
		currentJointAngleInRadians -= ((mJointAngleSensorOffset->get() / 180.0) * Math::PI);
	}

	double currentAngleInDegree = (currentJointAngleInRadians * 180.0) / Math::PI;
	
	//consider angular sensor transmission ratio.
	currentAngleInDegree *= mJointToAngularSensorTransmissionRatio->get();
	
	while(currentAngleInDegree > 180.0) {
		currentAngleInDegree -= 360.0;
	}
	while(currentAngleInDegree < -180.0) {
		currentAngleInDegree += 360.0;
	}

	//TODO: Add noise

	return currentAngleInDegree;
}


}



