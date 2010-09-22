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

#ifndef NERD_DYNAMIXELFRICTIONMOTOR_H
#define NERD_DYNAMIXELFRICTIONMOTOR_H


#include "MotorModels/DynamixelMotor.h"
#include <QList>

namespace nerd { class DoubleValue; }
namespace nerd { class InterfaceValue; }
namespace nerd { class IntValue; }

namespace nerd {

class DynamixelFrictionMotor : public DynamixelMotor {
  public:
    DynamixelFrictionMotor(const QString &name);
    DynamixelFrictionMotor(const DynamixelFrictionMotor &dynamixelFrictionMotor); 

    virtual QString getMotorName() const;

    virtual bool setInputValues(const QList<InterfaceValue*> &values);

    virtual void setupComponents();
    virtual void clearComponents();

    virtual void updateComponentInput() = 0;
    virtual void updateComponentOutput() = 0;

    double calculateVelocity(double current, double desired);
    double calculateFriction(double velocity);
    void calculateMotorFrictionAndVelocity();

  protected:

    BoolValue *mIncludeNoise;
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

    DoubleValue *mSensorNoiseDeviationValue;

    DoubleValue *mTimeStepValue;

    IntValue* mHistorySizeValue;

    double mOffset;
    double mMinAngle;
    double mMaxAngle;
    double mPidProportional;
    double mPidIntegral;
    double mPidDifferential;
    double mLastError;
    double iState;

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
};

} // namespace nerd
#endif
