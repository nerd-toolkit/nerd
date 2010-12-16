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



#ifndef NERDMSeriesV2Model_H
#define NERDMSeriesV2Model_H

#include <QList>
#include "ModelInterface.h"
#include "Collision/CollisionObject.h"
#include "Physics/BoxInertiaBody.h"
#include "Physics/CylinderBody.h"

namespace nerd {

	/**
	 * MSeriesV2Model.
	 *
	 */
	class MSeriesV2Model : public ModelInterface {
	public:
		MSeriesV2Model(const QString &groupName = "", const QString &bodyPartList = "", bool useAlternativeForceSensorNames = false);
		MSeriesV2Model(const MSeriesV2Model &other);
		virtual ~MSeriesV2Model();

		virtual SimObject* createCopy() const;
		virtual void createModel();

	protected:
		virtual void layoutObjects();

		void disableCollisions(SimBody *body1, SimBody *body2);
		void initializeMotorModels();

	private:
		QString mGroupName;
		QString mNamePrefix;

	private:
		StringValue *mBodyPartSelectorValue;
		bool mFirstLayout;

		bool mIncludeHead;
		bool mIncludeLeftArm;
		bool mIncludeRightArm;
		bool mIncludeBody;
		bool mIncludeHip;
		bool mIncludeLeftLeg;
		bool mIncludeRightLeg;
		bool mIncludeLeftHand;
		bool mIncludeRightHand;
		bool mUseAlternativeLeftArm;
		bool mUseAlternativeRightArm;
		// Free model parameters, see constructor for details
		BoolValue* mIncludeAccus;
		DoubleValue* mDistanceXHipYaw;
		DoubleValue* mLegDepth;
		DoubleValue* mLegWidth;
		DoubleValue* mHeelRadius;
		DoubleValue* mEndOfHeelZ;
		DoubleValue* mSoleOfFootForceSensorDepth;
		DoubleValue* mBallOfFootForceSensorDepth;
		DoubleValue* mFootForceSensorHeight;
		DoubleValue* mToesJointY;
		DoubleValue* mToesJointZ;
		DoubleValue* mAnkleJointY;
		DoubleValue* mToesDepth;
		DoubleValue* mTipToesRadius;
		DoubleValue* mDistanceYAnkleToKneeJoint;
		DoubleValue* mKneeJointZ;
		DoubleValue* mKneeRadius;
		DoubleValue* mDistanceYKneeToHipPitchRollJoint;
		DoubleValue* mDistanceYHipPitchRollToHipYawJoint;
		DoubleValue* mPelvisWidth;
		DoubleValue* mDistanceXShoulderPitch;
		DoubleValue* mPelvisDepth;
		DoubleValue* mDistanceYHipYawToWaistRollJoint;
		DoubleValue* mDistanceYWaistRollToHeadFlange;
		DoubleValue* mDistanceYHeadFlangeToHeadPitchJoint;
		DoubleValue* mUpperBodyDepth;
		DoubleValue* mDistanceYHeadPitchToHeadYawRollJoint;
		DoubleValue* mThroatWidth;
		DoubleValue* mThroatDepth;
		DoubleValue* mHeadYawJointZ;
		DoubleValue* mHeadRollPartWidth;
		DoubleValue* mDistanceZHeadYawRollJointToBackOfHead;
		DoubleValue* mDistanceZHeadYawRollJointToFrontOfHead;
		DoubleValue* mHeadRollPartHeight;
		DoubleValue* mDistanceXShoulderPitchToShoulderRollJoint;
		DoubleValue* mArmWidth;
		DoubleValue* mArmDepth;
		DoubleValue* mDistanceYHeadFlangeToShoulderPitchJoint;
		DoubleValue* mShoulderHeight;
		DoubleValue* mDistanceYShoulderRollToElbowPitchJoint;
		DoubleValue* mElbowRadius;
		DoubleValue* mElbowJointZ;
		DoubleValue* mDistanceYElbowPitchToWristJoint;

		// SimObjects
		BoxInertiaBody *mRightFoot;
		BoxInertiaBody *mLeftFoot;
		CollisionObject *mRightSoleOfFoot;
		CollisionObject *mLeftSoleOfFoot;
		CollisionObject *mRightSoleOfFootRightForceSensorCollObj;
		CollisionObject *mRightSoleOfFootLeftForceSensorCollObj;
		CollisionObject *mLeftSoleOfFootRightForceSensorCollObj;
		CollisionObject *mLeftSoleOfFootLeftForceSensorCollObj;
		CollisionObject *mRightHeelRightForceSensorCollObj;
		CollisionObject *mRightHeelLeftForceSensorCollObj;
		CollisionObject *mLeftHeelRightForceSensorCollObj;
		CollisionObject *mLeftHeelLeftForceSensorCollObj;
		CollisionObject *mRightBallOfFootLeftForceSensorCollObj;
		CollisionObject *mRightBallOfFootRightForceSensorCollObj;
		CollisionObject *mLeftBallOfFootLeftForceSensorCollObj;
		CollisionObject *mLeftBallOfFootRightForceSensorCollObj;
		BoxInertiaBody *mRightToes;
		BoxInertiaBody *mLeftToes;
		CollisionObject *mRightTipToes;
		CollisionObject *mLeftTipToes;
		BoxInertiaBody *mRightAnkle;
		BoxInertiaBody *mLeftAnkle;
		BoxInertiaBody *mRightTibia;
		BoxInertiaBody *mLeftTibia;
		CollisionObject *mRightKnee;
		CollisionObject *mLeftKnee;
		BoxInertiaBody *mRightFemur;
		BoxInertiaBody *mLeftFemur;
		BoxInertiaBody *mRightHip;
		BoxInertiaBody *mLeftHip;
		BoxInertiaBody *mRightHipCross;
		BoxInertiaBody *mLeftHipCross;
		BoxInertiaBody *mPelvis;
		BoxInertiaBody *mUpperBody;
		CollisionObject *mThroat;

		BoxInertiaBody *mHeadPitchPart;
		BoxInertiaBody *mHeadYawPart;
		BoxInertiaBody *mHeadRollPart;

		BoxInertiaBody *mRightShoulder;
		BoxInertiaBody *mLeftShoulder;
		BoxInertiaBody *mRightUpperArm;
		BoxInertiaBody *mLeftUpperArm;
		CollisionObject *mRightElbow;
		CollisionObject *mLeftElbow;
		BoxInertiaBody *mRightLowerArm;
		BoxInertiaBody *mLeftLowerArm;

		SimBody *mRightHandPalm;
		SimBody *mRightThumbBase;
		SimBody *mRightThumbTip; 
		SimBody *mRightFingerBase;
		SimBody *mRightFingerTip;
		SimObject *mRightHandRollMotor;
		SimObject *mRightHandMainMotor;
		SimObject *mRightHandThumbTipMotor;
		SimObject *mRightHandFingerBaseMotor;
		SimObject *mRightHandFingerTipMotor;

		SimBody *mLeftHandPalm;
		SimBody *mLeftThumbBase;
		SimBody *mLeftThumbTip; 
		SimBody *mLeftFingerBase;
		SimBody *mLeftFingerTip;
		SimObject *mLeftHandRollMotor;
		SimObject *mLeftHandMainMotor;
		SimObject *mLeftHandThumbTipMotor;
		SimObject *mLeftHandFingerBaseMotor;
		SimObject *mLeftHandFingerTipMotor;

		SimObject *mForceSensorRightSoleOfFootRight;
		SimObject *mForceSensorRightSoleOfFootLeft;
		SimObject *mForceSensorRightBallOfFootRight;
		SimObject *mForceSensorRightBallOfFootLeft;
		SimObject *mForceSensorLeftSoleOfFootRight;
		SimObject *mForceSensorLeftSoleOfFootLeft;
		SimObject *mForceSensorLeftBallOfFootRight;
		SimObject *mForceSensorLeftBallOfFootLeft;

		SimObject *mRightAnklePitchMotor;
		SimObject *mLeftAnklePitchMotor;
		SimObject *mRightAnkleRollMotor;
		SimObject *mLeftAnkleRollMotor;
		SimObject *mRightToesJoint;
		SimObject *mLeftToesJoint;
		SimObject *mRightKneeMotor;
		SimObject *mLeftKneeMotor;
		SimObject *mRightHipPitchMotor;
		SimObject *mLeftHipPitchMotor;
		SimObject *mRightHipRollMotor;
		SimObject *mLeftHipRollMotor;
		SimObject *mRightHipYawMotor;
		SimObject *mLeftHipYawMotor;
		SimObject *mWaistRollMotor;
		SimObject *mWaistSpringAdapter;
		SimObject *mHeadPitchMotor;
		SimObject *mHeadYawMotor;
		SimObject *mHeadRollMotor;
		SimObject *mRightShoulderPitchMotor;
		SimObject *mLeftShoulderPitchMotor;
		SimObject *mRightShoulderRollMotor;
		SimObject *mLeftShoulderRollMotor;
		SimObject *mRightElbowPitchMotor;
		SimObject *mLeftElbowPitchMotor;

		SimObject *mAB00;
		SimObject *mAB10;
		SimObject *mAB01;
		SimObject *mAB11;
		SimObject *mAB02;
		SimObject *mAB12;
		SimObject *mAB03;
		SimObject *mAB13;
		SimObject *mAB04;
		SimObject *mAB14;
		SimObject *mAB20;
		SimObject *mAB30;
		SimObject *mAB21;
		SimObject *mAB31;
		SimObject *mAB40;
		SimObject *mAB41;
		SimObject *mAB42;
		SimObject *mAB43;
		SimObject *mAB44;
		SimObject *mAB50;
		SimObject *mAB51;

		bool mUseAlternativeForceSensorNames;
	};

}

#endif



