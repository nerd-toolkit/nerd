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



#ifndef NERDH_MSeriesTorqueSpringMotorModel_H_
#define NERDH_MSeriesTorqueSpringMotorModel_H_

#include <QString>
#include <QHash>
#include "MotorModels/MSeriesTorqueSpringMotorModel.h"
#include "Physics/HingeJoint.h"

namespace nerd {

	/**
	 * H_MSeriesTorqueSpringMotorModel.
	 * 
	 * Physical Model of the M-Series Motor. 
	 *
	 */
	class H_MSeriesTorqueSpringMotorModel : public MSeriesTorqueSpringMotorModel {
	public:
		H_MSeriesTorqueSpringMotorModel(const QString &name);
		H_MSeriesTorqueSpringMotorModel(const H_MSeriesTorqueSpringMotorModel &other);
		virtual ~H_MSeriesTorqueSpringMotorModel();

		virtual void setup();
		virtual void clear();
	
		virtual void updateInputValues();
		virtual void updateOutputValues();

	protected:
		double calculateJointTorque();
		double calculateCurrent();
		double calculateAngularSensor(double currentJointAngleInRadians);

		virtual bool setOwner(MotorAdapter *owner);

	protected:
		double mCurrentJointAngle;		// Angle of the corresponding joint in current step

		QList<double> mCurrentMotorPositions;	// Array of all motor positions in current step
		QList<double> mLastMotorPositions;		// Array of all motor positions in last step
		QList<double> mCurrentMotorSpeeds;		// Array of all motor speeds in current step
		QList<double> mLastSpringTensions;		// Array of all spring tensions in last step (used for spring damping)
		double mLastSpringTensionWithoutMotor;	// If no motor is used and the whole motor is just a spring coupling, this variable is used for the last spring tension

		BoolValue *mUseSpringCouplings;			// Bool Value: Use Spring Coupling or Rigid Coupling between Motor and Joint
		
		DoubleValue *mTorqueConstant;	// TorqueConstant from the formula above (Torque originating from electronics.)
		DoubleValue *mSpeedConstant;	// SpeedConstant from the formula above (BackEMF (Gegenspannung) (rotation horizontally)
		DoubleValue *mMotorInertia;		// Intertia of a single motor (incl. spring and motor_intern * gear_transmission_ratio)

		DoubleValue *mVoltage;			// Absolute value of voltage supplied to the motor. This could be feeded by
										// another power supply model

//		DoubleValue *mBackEMFConstant;	// BackEMFConstant from the RE-Max17 Datasheet (it is called SpeedConstant there!). Be careful with the entities!
											// Has to be [V*sec] ! This is needed for Current Sensor.
//		DoubleValue *mTerminalResistance;	// TerminalResistance from the RE-Max17 Datasheet. This is needed for Current Sensor.

		DoubleValue *mStaticToDynamicFrictionSpeedThreshold;	// Speed Threshold of the motor where the static friction changes to dynamic friction

		DoubleValue *mDynamicFrictionConstant;	// DynamicFrictionConstant from the formula above
		DoubleValue *mStaticFrictionTorque;		// Torque that has to be applied in the Freerun-Mode, so that the motor starts to move

//		DoubleValue *mMotorGearTransmissionRatio;	// TransmissionRatio of the motor gear. See dynamixel datasheet.

		DoubleValue *mSpringConstant;				// Spring constant for spring coupling.
		DoubleValue *mSpringDampingConstant;	// Damping coefficient of the spring

		DoubleValue *mSpringConstantForRigidCoupling;	// If the model doesn't use spring couplings, a very high spring constant is used in the model.
														// This is the high constant.
		DoubleValue *mSpringDampingConstantForRigidCoupling;

		QList<DoubleValue*> mJointToMotorDisplacements;	// This factors can be used, if there is a displacement (wanted or not wanted) between the
														// zero position of the joint and the zero positions of the motors. It's in degrees!

		DoubleValue *mMotorToJointTransmissionRatio;	// Transmission Ratio between motor and joint. If it's 1:2 set this value to 0.5.
		DoubleValue *mJointToAngularSensorTransmissionRatio;

		DoubleValue *mCurrentConsumptionVoltageConstant;		// Constant for current consumption computation depending on the voltage applied to the motor.
		DoubleValue *mCurrentConsumptionSpeedConstant;			// Constant for current consumption computation depending on the motor speed.

		int mNumberOfMotors;			// Number of motors included in the complete joint motor

		DoubleValue *mJointCoulombFriction;		// This is the CoulombFriction constant for the joint.
												// (In ODE the desired joint velocity is set to zero and this is the FMax Parameter)

		DoubleValue *mGearEfficiencyFactor;		// This is the efficiency factor of the motor gear. This is escpecially important if
												// on motor works against another or if an external force works against a motor.

		QList<DoubleValue*> mTmpTorqueSpring;
		QList<DoubleValue*> mTmpTorqueFromMotor;
		QList<DoubleValue*> mTmpTorqueComplete;
		QList<DoubleValue*> mTmpTorquWithoutFriction;

	};

}

#endif



