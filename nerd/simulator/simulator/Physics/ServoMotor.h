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

#ifndef NERDServoMotor_H
#define NERDServoMotor_H

#include "Physics/HingeJoint.h"
#include <QString>
#include "Value/DoubleValue.h"
#include "Value/InterfaceValue.h"
#include "Value/BoolValue.h"
#include "Event/EventListener.h"
#include "Physics/SimSensor.h"
#include "Physics/SimActuator.h"

namespace nerd {


/**
 * ServoMotor: MinAngle and MaxAngle mark the maximum position of the motor that are
 * achievable due to architectural constraints. If the motor is required to have a 
 * constraint working range (default is [-180, 180] from start position), a new Motor,
 * inheriting the ServoMotor, needs to be implemented (currently). 
 * In torque-driven mode, the parameter "MaxTorque" can be used to change the speed of the motor.
 */
class ServoMotor : public HingeJoint, public virtual EventListener, 
				   public virtual SimSensor, public virtual SimActuator 
{
	public:
		ServoMotor(const QString &name, bool enableTemperature = false);
		ServoMotor(const ServoMotor &joint);
		virtual ~ServoMotor();
		
		virtual SimObject* createCopy() const;
		
		virtual void valueChanged(Value *value);
		virtual void controlMotorAngle(bool controlAngle);
		virtual bool isControllingMotorAngle() const;

		virtual void eventOccured(Event *event);
		virtual QString getName() const;
		virtual void setName(const QString &name);
		virtual void setup();
		virtual void clear();
		virtual void updateSensorValues();
		virtual void updateActuators();

	protected:
		virtual double calculatedTorque(double currentVelocity, double currentPosition);
		void initialize();

	protected:		
		DoubleValue *mPID_Proportional;
		DoubleValue *mPID_Integral;
		DoubleValue *mPID_Differential;
		DoubleValue *mMinAngleValue;
		DoubleValue *mMaxAngleValue;
		DoubleValue *mMaxTorque;
		BoolValue *mControlMotorAngle;
		DoubleValue *mAngleSensorRangeMin;
		DoubleValue *mAngleSensorRangeMax;
		DoubleValue *mTemperatureGainFactor;
		DoubleValue *mTemperatureCoolingRate;
		DoubleValue *mEnergyConsumptionFactor;
		DoubleValue *mTemperatureThresholdForFailure;
		StringValue *mExternalEnergyValueName;
		BoolValue *mIgnoreEnergyEmpty;

		bool mIsInitialized;

		InterfaceValue *mDesiredMotorSetting;
		InterfaceValue *mMotorAngleSensor;
		InterfaceValue *mMotorTemperature;

		DoubleValue *mTimeStepSize;
		DoubleValue *mSensorNoiseValue;
		DoubleValue *mOffsetValue;
		DoubleValue *mExternalEnergyValue;

		IntValue *mHistorySizeValue;
		DoubleValue *mFMaxValue;

		Event *mResetEvent;

		double mOffset;
		double mMinAngle;
		double mMaxAngle;
		double mProportional;
		double mIntegral;
		double mDamping;
		double mCurrentMotorAngle;
		double mLastMotorAngle;
		double mLastSensorMotorAngle;
		double mSensorNoise;

		double mLastError;
		int mHistorySize;
		QVector<double> mErrorHistory;
		bool mEnableTemperature;
		
	};

}

#endif

