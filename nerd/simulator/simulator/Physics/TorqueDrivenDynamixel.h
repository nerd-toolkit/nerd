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



#ifndef TorqueDrivenDynamixel_H_
#define TorqueDrivenDynamixel_H_
#include "HingeJoint.h"

namespace nerd{

class TorqueDrivenDynamixel : public HingeJoint, public virtual EventListener {

	public:
		TorqueDrivenDynamixel(const QString &name);
		TorqueDrivenDynamixel(const TorqueDrivenDynamixel &joint);
		virtual ~TorqueDrivenDynamixel();
		
		virtual SimObject* createCopy() const;
		virtual void setup();

		virtual void eventOccured(Event *event);
		virtual QString getName() const;

	protected:
		virtual void updateMotors();
		virtual void updateSensors();
		double getFrictionMoment(double currentVelocity);

	private:
		void setDynamixelParameter();

	protected:
		InterfaceValue *mDesiredMotorSetting;
		InterfaceValue *mJointAngle;
		DoubleValue *mMinAngleValue;	
		DoubleValue *mMaxAngleValue;
		double mMinAngle;
		double mMaxAngle;


		Event *mUpdateMotors;
		Event *mUpdateSensors;
		Event *mResetEvent;
		DoubleValue *mSensorNoiseValue;
		DoubleValue *mOffsetValue;
		DoubleValue *mTimeStepSize;
		IntValue* mIterationSteps;

		double mOffset;

		double mH;

		double mJ;
		double mM_max;
		double mM;

		double mP;
		double mW;
		double mExp1;
		double mExp2;
		double mBeta;
		
		double mE;
		double mO;
		
		double mE0;
		double mE1;
		double mE2;
		double mC1;
		double mC2;
		double mZ1;
		double mZ2;

		double mReport;
		double mPi56;

// Friction parameter
		double mZ;
		double mZp;
		double mG;
		double mSigma0;
		double mSigma1;
		double mFc;
		double mFs;
		double mFv;
		double mVS;
		double mVD;
		double mDS;
		double mSgnv;	
};
}
#endif

