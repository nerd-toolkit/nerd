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

#ifndef NERDDynamixel_H
#define NERDDynamixel_H

#include "Physics/HingeJoint.h"
#include <QString>
#include "Value/DoubleValue.h"
#include "Value/InterfaceValue.h"
#include "Value/BoolValue.h"
#include "Physics/SimSensor.h"
#include "Physics/SimActuator.h"

namespace nerd {

/**
 * Dynamixel: MinAngle and MaxAngle mark the maximum position of the motor that are 
 * achievable due to architectural constraints. If the motor is required to have a 
 * constraint working range (default is [-180, 180] from start position), a new Motor,
 * inheriting the Dynamixel, needs to be implemented (currently). 
*/
class Dynamixel : public HingeJoint, public virtual SimSensor, public virtual SimActuator {
	public:
		Dynamixel(const QString &name, bool hideTorqueInputs = false, 
					const QString &globalParameterPrefix = "/AngularMotor");
		Dynamixel(const Dynamixel &joint);
		virtual ~Dynamixel();
		
		virtual SimObject* createCopy() const;
		
		virtual void valueChanged(Value *value);

		virtual void setName(const QString &name);
		virtual QString getName() const;
		virtual void updateSensorValues();
		virtual void updateActuators();

	protected:
	
		virtual void setup();
		virtual void clear();
		double calculateVelocity(double current, double desired);
		double calculateFriction(double velocity);
		void calculateMotorFrictionAndVelocity();

	protected:		
		QString mGlobalParameterPrefix;

// 		DoubleValue *mPID_Proportional;
// 		DoubleValue *mPID_Integral;
// 		DoubleValue *mPID_Differential;
		DoubleValue *mMinAngleValue;
		DoubleValue *mMaxAngleValue;
// 		DoubleValue *mMaxTorque;
		InterfaceValue *mDesiredMotorTorqueValue;
		InterfaceValue *mDesiredMotorAngleValue;
		InterfaceValue *mAngleValue;
		BoolValue *mEliminateNegativeTorqueRange;

		
		
		DoubleValue *mFMaxFactor;
		DoubleValue *mDamping;
		DoubleValue *mProportional;
		DoubleValue *mIntegral;
		
		DoubleValue *mCurrentVelocityValue;
		DoubleValue *mMaxVelocityValue;

		DoubleValue *mMotorValueNoiseValue;
		DoubleValue *mOffsetValue;

		DoubleValue *mStaticFrictionValue;
		DoubleValue *mStaticFrictionBorderValue;

		DoubleValue *mDynamicFrictionValue;
		DoubleValue *mDynamicFrictionOffsetValue;

		BoolValue *mIncludeNoiseValue;
		DoubleValue *mSensorNoiseDeviationValue;

		DoubleValue *mTimeStepValue;

		IntValue* mHistorySizeValue;
		
		DoubleValue *mMinSensorAngleValue;
		DoubleValue *mMaxSensorAngleValue;

		double mOffset;
		double mMinAngle;
		double mMaxAngle;
		double mPidProportional;
		double mPidIntegral;
		double mPidDifferential;
		double mLastError;
		double iState;

		bool mIncludeNoise;
		double mSensorNoiseDeviation;

		double mDynamicFriction;
		double mStaticFriction;
		double mDynamicFrictionOffset;
		double mStaticFrictionBorder;
		double mMaxVelocity;
		double mMotorValueNoise;

		double mVelocity;
		double mLastPosition;
		double mStepSize;

		double mTimeStepSize;

		int mHistorySize;
		QVector<double> mErrorHistory;

		double mCalculatedVelocity;
		double mCalculatedFriction;
		double mCurrentPosition;
	
		bool mHideTorqueInputs;
};

}

#endif

