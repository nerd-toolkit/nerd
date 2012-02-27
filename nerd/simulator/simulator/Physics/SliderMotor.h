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
#ifndef SliderMotor_H_
#define SliderMotor_H_

#include "Event/EventListener.h"
#include "Physics/SliderJoint.h"
#include "Physics/SimSensor.h"
#include "Physics/SimActuator.h"

namespace nerd {

/**
 * Physics-engine independent definition of a SliderMotor. 
 * A slider motor defines several parameters to control the behavior of the motor.
 * ControlPosition, SensorNoise, DesiredSetting, Position, MaxVelocity, Friction/StaticFriction, Friction/DynamicFriction, Friction/StaticBorder, Friction/DynamicOffset, PID_P, PID_I, PID_D, HistorySize
**/

class BoolValue;

class SliderMotor : public SliderJoint, 
					public virtual SimSensor, public virtual SimActuator 
{

	public:
		SliderMotor(const QString &name);
		SliderMotor(const SliderMotor &joint);
		virtual ~SliderMotor();
		
		virtual SimObject* createCopy() const;
		
		virtual void valueChanged(Value *value);
		virtual void controlMotorPosition(bool controlPosition);
		virtual bool isControllingMotorPosition() const;

		virtual QString getName() const;
		virtual void setup();
		virtual void clear();
		virtual void updateActuators();
		virtual void updateSensorValues();

	protected:
		void initialize();
		void resetSlider();
		double calculateVelocity(double currentPosition);
		double calculateFriction(double velocity);

	protected:		
		DoubleValue *mMaxTorque;
		BoolValue *mControlMotorPosition;

		InterfaceValue *mDesiredMotorSetting;
		InterfaceValue *mMotorPositionSensor;

		DoubleValue *mTimeStepSize;
		DoubleValue *mSensorNoiseValue;
		DoubleValue *mMaxVelocityValue;
		
		Event *mResetEvent;

		double mSensorNoise;
		double mMinPosition;
		double mMaxPosition;
		double mLastMotorPosition;
		double mCurrentMotorPosition;
		double mVelocity;

		DoubleValue *mPID_Proportional;
		DoubleValue *mPID_Integral;
		DoubleValue *mPID_Differential;
		IntValue *mHistorySizeValue;

		DoubleValue *mStaticFrictionValue;
		DoubleValue *mStaticFrictionBorderValue;

		DoubleValue *mDynamicFrictionValue;
		DoubleValue *mDynamicFrictionOffsetValue;


		double mProportional;
		double mIntegral;
		double mDamping;

		double mLastError;
		int mHistorySize;
		QVector<double> mErrorHistory;

		double mStaticFriction;
		double mStaticFrictionBorder;

		double mDynamicFriction;
		double mDynamicFrictionOffset;

		DoubleValue *mFMaxValue;

};
}
#endif

