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


#include "MSeriesV1Model.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Collision/CollisionManager.h"
#include "Physics/SimObjectGroup.h"
#include "Util/Color.h"
#include "Physics/Physics.h"
#include "SimulationConstants.h"
#include "Physics/BoxGeom.h"
#include "Physics/CylinderGeom.h"
#include "Collision/CollisionObject.h"
#include <iostream>

#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))

using namespace nerd;


MSeriesV1Model::MSeriesV1Model(const QString &groupName) : ModelInterface(groupName) {
	mPosition->addValueChangedListener(this);

	// Create the free model Parameters
		// Width (X), Depth (Z), Height (Y)
	mIncludeAccus = new BoolValue(false);
	addParameter(mMorphologyParametersPrefix + "IncludeAccus", mIncludeAccus);

	mDistanceXHipYaw = new DoubleValue(0.150);		// Distance between the two hip yaw joints in X-direction
	addParameter(mMorphologyParametersPrefix + "DistanceXHipYaw", mDistanceXHipYaw);


	mLegDepth = new DoubleValue(0.090);		// This is the Depth of the pure leg (tibia and femur)
	addParameter(mMorphologyParametersPrefix + "LegDepth", mLegDepth);
	mLegWidth = new DoubleValue(0.080);		// This is the Width of the pure leg (tibia and femur)
	addParameter(mMorphologyParametersPrefix + "LegWidth", mLegWidth);

	mHeelRadius = new DoubleValue(0.011);
	addParameter(mMorphologyParametersPrefix + "HeelRadius", mHeelRadius);

	mEndOfHeelZ = new DoubleValue(-0.054);		// The Z-end of the foot (heel)
	addParameter(mMorphologyParametersPrefix + "EndOfHeelZ", mEndOfHeelZ);

	mSoleOfFootForceSensorDepth = new DoubleValue(0.015);		// The Depth of the Sole of Foot Force Sensor
	addParameter(mMorphologyParametersPrefix + "SoleOfFootForceSensorDepth", mSoleOfFootForceSensorDepth);

	mBallOfFootForceSensorDepth = new DoubleValue(0.016);	// The Depth of the Ball of Foot Force Sensor
	addParameter(mMorphologyParametersPrefix + "BallOfFootForceSensorDepth", mBallOfFootForceSensorDepth);

	mFootForceSensorHeight = new DoubleValue(0.001);	// The Height of the Foot Force Sensors
	addParameter(mMorphologyParametersPrefix + "FootForceSensorHeight", mFootForceSensorHeight);

	mToesJointY = new DoubleValue(0.019);
	addParameter(mMorphologyParametersPrefix + "ToesJointY", mToesJointY);
	mToesJointZ = new DoubleValue(0.091);
	addParameter(mMorphologyParametersPrefix + "ToesJointZ", mToesJointZ);

	mAnkleJointY = new DoubleValue(0.058);
	addParameter(mMorphologyParametersPrefix + "AnkleJointY", mAnkleJointY);

	mToesDepth = new DoubleValue(0.031);
	addParameter(mMorphologyParametersPrefix + "ToesDepth", mToesDepth);

	mTipToesRadius = new DoubleValue(0.009);
	addParameter(mMorphologyParametersPrefix + "TipToesRadius", mTipToesRadius);

	mDistanceYAnkleToKneeJoint = new DoubleValue(0.274);
	addParameter(mMorphologyParametersPrefix + "DistanceYAnkleToKneeJoint", mDistanceYAnkleToKneeJoint);

	mKneeJointZ = new DoubleValue(-0.054);
	addParameter(mMorphologyParametersPrefix + "KneeJointZ", mKneeJointZ);

	mKneeRadius = new DoubleValue(0.048);
	addParameter(mMorphologyParametersPrefix + "KneeRadius", mKneeRadius);

	mDistanceYKneeToHipPitchRollJoint = new DoubleValue(0.275);
	addParameter(mMorphologyParametersPrefix + "DistanceYKneeToHipPitchRollJoint", mDistanceYKneeToHipPitchRollJoint);

	mDistanceYHipPitchRollToHipYawJoint = new DoubleValue(0.100);
	addParameter(mMorphologyParametersPrefix + "DistanceYHipPitchRollToHipYawJoint", mDistanceYHipPitchRollToHipYawJoint);
		// DistanceY between the HipPitchRollJoint and the flange to the torso (HipYawJoint). This is the hipHeight.

	mPelvisWidth = new DoubleValue(0.276);
	addParameter(mMorphologyParametersPrefix + "PelvisWidth", mPelvisWidth);

	mDistanceXShoulderPitch = new DoubleValue(0.286);
	addParameter(mMorphologyParametersPrefix + "DistanceXShoulderPitch", mDistanceXShoulderPitch);
					// DistanceX between the shoulder pitch joints (i.e. distance between the points where the arms starts to pitch (the flanges))
					// This is the upperBody width.

	mPelvisDepth = new DoubleValue(0.158);
	addParameter(mMorphologyParametersPrefix + "PelvisDepth", mPelvisDepth);

	mDistanceYHipYawToWaistRollJoint = new DoubleValue(0.030);
	addParameter(mMorphologyParametersPrefix + "DistanceYHipYawToWaistRollJoint", mDistanceYHipYawToWaistRollJoint);
					// DistanceY between the HipYawJoint (flange) and the WaistRollJoint. This is the pelvisHeight.

	mDistanceYWaistRollToHeadFlange = new DoubleValue(0.272);
	addParameter(mMorphologyParametersPrefix + "DistanceYWaistRollToHeadFlange", mDistanceYWaistRollToHeadFlange);
				// DistanceY between the WaistRollJoint and the flange of the head.

	mDistanceYHeadFlangeToHeadPitchJoint = new DoubleValue(0.051);
	addParameter(mMorphologyParametersPrefix + "DistanceYHeadFlangeToHeadPitchJoint", mDistanceYHeadFlangeToHeadPitchJoint);
				// DistanceY between the head flange (which is the upper end of the upper body) and the HeadPitchJoint.

	mUpperBodyDepth = new DoubleValue(0.131);
	addParameter(mMorphologyParametersPrefix + "UpperBodyDepth", mUpperBodyDepth);

	mDistanceYHeadPitchToHeadYawRollJoint = new DoubleValue(0.046);
	addParameter(mMorphologyParametersPrefix + "DistanceYHeadPitchToHeadYawRollJoint", mDistanceYHeadPitchToHeadYawRollJoint);

	mThroatWidth = new DoubleValue(0.070);
	addParameter(mMorphologyParametersPrefix + "ThroatWidth", mThroatWidth);
	mThroatDepth = new DoubleValue(0.070);
	addParameter(mMorphologyParametersPrefix + "ThroatDepth", mThroatDepth);

	mHeadYawJointZ = new DoubleValue(0.005);
	addParameter(mMorphologyParametersPrefix + "HeadYawJointZ", mHeadYawJointZ);

	mHeadRollPartWidth = new DoubleValue(0.189);
	addParameter(mMorphologyParametersPrefix + "HeadRollPartWidth", mHeadRollPartWidth);
	mDistanceZHeadYawRollJointToBackOfHead = new DoubleValue(0.100);
	addParameter(mMorphologyParametersPrefix + "DistanceZHeadYawRollJointToBackOfHead", mDistanceZHeadYawRollJointToBackOfHead);
	mDistanceZHeadYawRollJointToFrontOfHead = new DoubleValue(0.120);
	addParameter(mMorphologyParametersPrefix + "DistanceZHeadYawRollJointToFrontOfHead", mDistanceZHeadYawRollJointToFrontOfHead);
	mHeadRollPartHeight = new DoubleValue(0.080);
	addParameter(mMorphologyParametersPrefix + "HeadRollPartHeight", mHeadRollPartHeight);

	mDistanceXShoulderPitchToShoulderRollJoint = new DoubleValue(0.054);
	addParameter(mMorphologyParametersPrefix + "DistanceXShoulderPitchToShoulderRollJoint", mDistanceXShoulderPitchToShoulderRollJoint);
			// DistanceX between the shoulder pitch joint (i.e. the shoulder flange) and the shoulder roll joint

	mArmWidth = new DoubleValue(0.080);
	addParameter(mMorphologyParametersPrefix + "ArmWidth", mArmWidth);
	mArmDepth = new DoubleValue(0.072);
	addParameter(mMorphologyParametersPrefix + "ArmDepth", mArmDepth);

	mDistanceYHeadFlangeToShoulderPitchJoint = new DoubleValue(-0.052);
	addParameter(mMorphologyParametersPrefix + "DistanceYHeadFlangeToShoulderPitchJoint", mDistanceYHeadFlangeToShoulderPitchJoint);

	mShoulderHeight = new DoubleValue(0.088);
	addParameter(mMorphologyParametersPrefix + "ShoulderHeight", mShoulderHeight);

	mDistanceYShoulderRollToElbowPitchJoint = new DoubleValue(-0.215);
	addParameter(mMorphologyParametersPrefix + "DistanceYShoulderRollToElbowPitchJoint", mDistanceYShoulderRollToElbowPitchJoint);

	mElbowRadius = new DoubleValue(0.040);
	addParameter(mMorphologyParametersPrefix + "ElbowRadius", mElbowRadius);

	mElbowJointZ = new DoubleValue(0.002);
	addParameter(mMorphologyParametersPrefix + "ElbowJointZ", mElbowJointZ);

	mDistanceYElbowPitchToWristJoint = new DoubleValue(-0.197);
	addParameter(mMorphologyParametersPrefix + "DistanceYElbowPitchToWristJoint", mDistanceYElbowPitchToWristJoint);
			// DistanceY between the elbow pitch joint and the wrist joint (i.e. the flange to the grabber)

}

MSeriesV1Model::MSeriesV1Model(const MSeriesV1Model &model) : Object(), ValueChangedListener(), 
		EventListener(), ModelInterface(model) 
{
	mIncludeAccus = dynamic_cast<BoolValue*>(getParameter(mMorphologyParametersPrefix + "IncludeAccus"));	
	mDistanceXHipYaw = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceXHipYaw"));
	mLegDepth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "LegDepth"));
	mLegWidth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "LegWidth"));
	mHeelRadius = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "HeelRadius"));
	mEndOfHeelZ = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "EndOfHeelZ"));
	mSoleOfFootForceSensorDepth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "SoleOfFootForceSensorDepth"));
	mBallOfFootForceSensorDepth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "BallOfFootForceSensorDepth"));
	mFootForceSensorHeight = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "FootForceSensorHeight"));
	mToesJointY = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "ToesJointY"));
	mToesJointZ = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "ToesJointZ"));
	mAnkleJointY = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "AnkleJointY"));
	mToesDepth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "ToesDepth"));
	mTipToesRadius = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "TipToesRadius"));
	mDistanceYAnkleToKneeJoint = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceYAnkleToKneeJoint"));
	mKneeJointZ = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "KneeJointZ"));
	mKneeRadius = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "KneeRadius"));
	mDistanceYKneeToHipPitchRollJoint = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceYKneeToHipPitchRollJoint"));
	mDistanceYHipPitchRollToHipYawJoint = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceYHipPitchRollToHipYawJoint"));
	mPelvisWidth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "PelvisWidth"));
	mDistanceXShoulderPitch = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceXShoulderPitch"));
	mPelvisDepth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "PelvisDepth"));
	mDistanceYHipYawToWaistRollJoint = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceYHipYawToWaistRollJoint"));
	mDistanceYWaistRollToHeadFlange = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceYWaistRollToHeadFlange"));
	mDistanceYHeadFlangeToHeadPitchJoint = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceYHeadFlangeToHeadPitchJoint"));
	mUpperBodyDepth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "UpperBodyDepth"));
	mDistanceYHeadPitchToHeadYawRollJoint = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceYHeadPitchToHeadYawRollJoint"));
	mThroatWidth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "ThroatWidth"));
	mThroatDepth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "ThroatDepth"));
	mHeadYawJointZ = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "HeadYawJointZ"));
	mHeadRollPartWidth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "HeadRollPartWidth"));
	mDistanceZHeadYawRollJointToBackOfHead = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceZHeadYawRollJointToBackOfHead"));
	mDistanceZHeadYawRollJointToFrontOfHead = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceZHeadYawRollJointToFrontOfHead"));
	mHeadRollPartHeight = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "HeadRollPartHeight"));
	mDistanceXShoulderPitchToShoulderRollJoint = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceXShoulderPitchToShoulderRollJoint"));
	mArmWidth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "ArmWidth"));
	mArmDepth = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "ArmDepth"));
	mDistanceYHeadFlangeToShoulderPitchJoint = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceYHeadFlangeToShoulderPitchJoint"));
	mShoulderHeight = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "ShoulderHeight"));
	mDistanceYShoulderRollToElbowPitchJoint = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceYShoulderRollToElbowPitchJoint"));
	mElbowRadius = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "ElbowRadius"));
	mElbowJointZ = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "ElbowJointZ"));
	mDistanceYElbowPitchToWristJoint = dynamic_cast<DoubleValue*>(getParameter(mMorphologyParametersPrefix + "DistanceYElbowPitchToWristJoint"));

	mPosition->addValueChangedListener(this);

}

MSeriesV1Model::~MSeriesV1Model(){
}

SimObject* MSeriesV1Model::createCopy() const {
	MSeriesV1Model *copy = new MSeriesV1Model("Default");
	return copy;
}



void MSeriesV1Model::createModel() {
	mGroupName = getName();

	mNamePrefix = "/";

	PhysicsManager *pm = Physics::getPhysicsManager();

	SimObject *boxInertiaBody = pm->getPrototype(
			SimulationConstants::PROTOTYPE_BOX_INERTIA_BODY); 

	SimObject *forceSensor = pm->getPrototype(
			SimulationConstants::PROTOTYPE_FORCE_SENSOR);
	SimObject *currentConsumptionSensor = pm->getPrototype(
			SimulationConstants::PROTOTYPE_CURRENT_CONSUMPTION_SENSOR);
	SimObject *accelBoard = pm->getPrototype(
			SimulationConstants::PROTOTYPE_ACCELSENSOR_3D);

	//Agent SimObject group.
	SimObjectGroup *agent = mAgent;

	// Right Foot
		// Main Body (uUperFoot Geom)
		mRightFoot = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mRightFoot, "Name")->set(mNamePrefix + ("Right/Foot"));
		PARAM(BoolValue, mRightFoot, "Dynamic")->set(true);

		agent->addObject(mRightFoot);

		// Sole of Foot	
		mRightSoleOfFoot = new CollisionObject(BoxGeom(mRightFoot, 0.0, 0.0, 0.0), mRightFoot, true);	// Geometries are set later by layoutObjects
		mRightFoot->addCollisionObject(mRightSoleOfFoot);

		// Sole of Foot Force Sensors
		mRightSoleOfFootRightForceSensorCollObj = new CollisionObject(BoxGeom(mRightFoot, 0.0, 0.0, 0.0), mRightFoot, true);
		mRightFoot->addCollisionObject(mRightSoleOfFootRightForceSensorCollObj);
		mRightSoleOfFootLeftForceSensorCollObj = new CollisionObject(BoxGeom(mRightFoot, 0.0, 0.0, 0.0), mRightFoot, true);
		mRightFoot->addCollisionObject(mRightSoleOfFootLeftForceSensorCollObj);

		// Heel Force Sensors
		mRightHeelRightForceSensorCollObj = new CollisionObject(CylinderGeom(mRightFoot, 0.0, 0.0), mRightFoot, true);
		mRightFoot->addCollisionObject(mRightHeelRightForceSensorCollObj);
		mRightHeelLeftForceSensorCollObj = new CollisionObject(CylinderGeom(mRightFoot, 0.0, 0.0), mRightFoot, true);
		mRightFoot->addCollisionObject(mRightHeelLeftForceSensorCollObj);

		// Ball of Foot Force Sensors
		mRightBallOfFootRightForceSensorCollObj = new  CollisionObject(BoxGeom(mRightFoot, 0.0, 0.0, 0.0), mRightFoot, true);
		mRightFoot->addCollisionObject(mRightBallOfFootRightForceSensorCollObj);
		mRightBallOfFootLeftForceSensorCollObj = new  CollisionObject(BoxGeom(mRightFoot, 0.0, 0.0, 0.0), mRightFoot, true);
		mRightFoot->addCollisionObject(mRightBallOfFootLeftForceSensorCollObj);

	// Left Foot
		// Main Body (UpperFoot Geom)
		mLeftFoot = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mLeftFoot, "Name")->set(mNamePrefix + ("Left/Foot"));
		PARAM(BoolValue, mLeftFoot, "Dynamic")->set(true);

		agent->addObject(mLeftFoot);

		// Sole Of Foot	
		mLeftSoleOfFoot = new CollisionObject(BoxGeom(mLeftFoot, 0.0, 0.0, 0.0), mLeftFoot, true);	// Geometries are set later by layoutObjects
		mLeftFoot->addCollisionObject(mLeftSoleOfFoot);

		// Sole of Foot Force Sensors
		mLeftSoleOfFootRightForceSensorCollObj = new CollisionObject(BoxGeom(mLeftFoot, 0.0, 0.0, 0.0), mLeftFoot, true);
		mLeftFoot->addCollisionObject(mLeftSoleOfFootRightForceSensorCollObj);
		mLeftSoleOfFootLeftForceSensorCollObj = new CollisionObject(BoxGeom(mLeftFoot, 0.0, 0.0, 0.0), mLeftFoot, true);
		mLeftFoot->addCollisionObject(mLeftSoleOfFootLeftForceSensorCollObj);

		// Heel Force Sensors
		mLeftHeelRightForceSensorCollObj = new CollisionObject(CylinderGeom(mLeftFoot, 0.0, 0.0), mLeftFoot, true);
		mLeftFoot->addCollisionObject(mLeftHeelRightForceSensorCollObj);
		mLeftHeelLeftForceSensorCollObj = new CollisionObject(CylinderGeom(mLeftFoot, 0.0, 0.0), mLeftFoot, true);
		mLeftFoot->addCollisionObject(mLeftHeelLeftForceSensorCollObj);

		// Ball of Foot Force Sensors
		mLeftBallOfFootRightForceSensorCollObj = new CollisionObject(BoxGeom(mLeftFoot, 0.0, 0.0, 0.0), mLeftFoot, true);
		mLeftFoot->addCollisionObject(mLeftBallOfFootRightForceSensorCollObj);
		mLeftBallOfFootLeftForceSensorCollObj = new CollisionObject(BoxGeom(mLeftFoot, 0.0, 0.0, 0.0), mLeftFoot, true);
		mLeftFoot->addCollisionObject(mLeftBallOfFootLeftForceSensorCollObj);

	// Right Toes
		// Main Body
		mRightToes = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mRightToes, "Name")->set(mNamePrefix + "Right/Toes");
		PARAM(BoolValue, mRightToes, "Dynamic")->set(true);

		agent->addObject(mRightToes);

		// TipToes
		mRightTipToes = new CollisionObject(CylinderGeom(mRightToes, 0.0, 0.0), mRightToes, true);
		mRightToes->addCollisionObject(mRightTipToes);

	// Left Toes
		// Main Body
		mLeftToes = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mLeftToes, "Name")->set(mNamePrefix + "Left/Toes");
		PARAM(BoolValue, mLeftToes, "Dynamic")->set(true);

		agent->addObject(mLeftToes);

		// TipToes
		mLeftTipToes = new CollisionObject(CylinderGeom(mLeftToes, 0.0, 0.0), mLeftToes, true);
		mLeftToes->addCollisionObject(mLeftTipToes);

	// Right Ankle
		mRightAnkle = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mRightAnkle, "Name")->set(mNamePrefix + "Right/Ankle");
		PARAM(BoolValue, mRightAnkle, "Dynamic")->set(true);

		agent->addObject(mRightAnkle);

	// Left Ankle
		mLeftAnkle = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mLeftAnkle, "Name")->set(mNamePrefix + "Left/Ankle");
		PARAM(BoolValue, mLeftAnkle, "Dynamic")->set(true);

		agent->addObject(mLeftAnkle);

	// Right Tibia
		// Main Body
		mRightTibia = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mRightTibia, "Name")->set(mNamePrefix + "Right/Tibia");
		PARAM(BoolValue, mRightTibia, "Dynamic")->set(true);

		agent->addObject(mRightTibia);

		// Knee
		mRightKnee = new CollisionObject(CylinderGeom(mRightTibia, 0.0, 0.0), mRightTibia, true);
		mRightTibia->addCollisionObject(mRightKnee);

	// Left Tibia
		// Main Body
		mLeftTibia = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mLeftTibia, "Name")->set(mNamePrefix + "Left/Tibia");
		PARAM(BoolValue, mLeftTibia, "Dynamic")->set(true);

		agent->addObject(mLeftTibia);

		// Knee
		mLeftKnee = new CollisionObject(CylinderGeom(mLeftTibia, 0.0, 0.0), mLeftTibia, true);
		mLeftTibia->addCollisionObject(mLeftKnee);

	// Right Femur
		mRightFemur = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mRightFemur, "Name")->set(mNamePrefix + "Right/Femur");
		PARAM(BoolValue, mRightFemur, "Dynamic")->set(true);

		agent->addObject(mRightFemur);

	// Left Femur
		mLeftFemur = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mLeftFemur, "Name")->set(mNamePrefix + "Left/Femur");
		PARAM(BoolValue, mLeftFemur, "Dynamic")->set(true);

		agent->addObject(mLeftFemur);

	// Right Hip
		mRightHip = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mRightHip, "Name")->set(mNamePrefix + "Right/Hip");
		PARAM(BoolValue, mRightHip, "Dynamic")->set(true);

		agent->addObject(mRightHip);

	// Left Hip
		mLeftHip = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mLeftHip, "Name")->set(mNamePrefix + "Left/Hip");
		PARAM(BoolValue, mLeftHip, "Dynamic")->set(true);

		agent->addObject(mLeftHip);

	// Right HipCross
		mRightHipCross = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mRightHipCross, "Name")->set(mNamePrefix + "Right/HipCross");
		PARAM(BoolValue, mRightHipCross, "Dynamic")->set(true);

		agent->addObject(mRightHipCross);

	// Left HipCross
		mLeftHipCross = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mLeftHipCross, "Name")->set(mNamePrefix + "Left/HipCross");
		PARAM(BoolValue, mLeftHipCross, "Dynamic")->set(true);

		agent->addObject(mLeftHipCross);

	// Pelvis
		mPelvis = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mPelvis, "Name")->set(mNamePrefix + "Middle/Pelvis");
		PARAM(BoolValue, mPelvis, "Dynamic")->set(true);

		agent->addObject(mPelvis);

	// Upper Body
		// Main Body
		mUpperBody = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mUpperBody, "Name")->set(mNamePrefix + "Middle/UpperBody");
		PARAM(BoolValue, mUpperBody, "Dynamic")->set(true);

		agent->addObject(mUpperBody);

		// Throat
		mThroat = new CollisionObject(BoxGeom(mUpperBody, 0.0, 0.0, 0.0), mUpperBody, true);
		mUpperBody->addCollisionObject(mThroat);

	// HeadPitchPart
		mHeadPitchPart = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mHeadPitchPart, "Name")->set(mNamePrefix + "Middle/HeadPitchPart");
		PARAM(BoolValue, mHeadPitchPart, "Dynamic")->set(true);

		agent->addObject(mHeadPitchPart);

	// HeadYawPart
		mHeadYawPart = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mHeadYawPart, "Name")->set(mNamePrefix + "Middle/HeadYawPart");
		PARAM(BoolValue, mHeadYawPart, "Dynamic")->set(true);

		agent->addObject(mHeadYawPart);

	// HeadRollPart
		mHeadRollPart = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mHeadRollPart, "Name")->set(mNamePrefix + "Middle/HeadRollPart");
		PARAM(BoolValue, mHeadRollPart, "Dynamic")->set(true);

		agent->addObject(mHeadRollPart);

	// Right Shoulder
		mRightShoulder = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mRightShoulder, "Name")->set(mNamePrefix + "Right/Shoulder");
		PARAM(BoolValue, mRightShoulder, "Dynamic")->set(true);

		agent->addObject(mRightShoulder);

	// Left Shoulder
		mLeftShoulder = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mLeftShoulder, "Name")->set(mNamePrefix + "Left/Shoulder");
		PARAM(BoolValue, mLeftShoulder, "Dynamic")->set(true);

		agent->addObject(mLeftShoulder);

	// Right UpperArm
		// Main Body
		mRightUpperArm = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mRightUpperArm, "Name")->set(mNamePrefix + "Right/UpperArm");
		PARAM(BoolValue, mRightUpperArm, "Dynamic")->set(true);

		agent->addObject(mRightUpperArm);

		// Elbow
		mRightElbow = new CollisionObject(CylinderGeom(mRightUpperArm, 0.0, 0.0), mRightUpperArm, true);
		mRightUpperArm->addCollisionObject(mRightElbow);

	// Left UpperArm
		// Main Body
		mLeftUpperArm = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mLeftUpperArm, "Name")->set(mNamePrefix + "Left/UpperArm");
		PARAM(BoolValue, mLeftUpperArm, "Dynamic")->set(true);

		agent->addObject(mLeftUpperArm);

		// Elbow
		mLeftElbow = new CollisionObject(CylinderGeom(mLeftUpperArm, 0.0, 0.0), mLeftUpperArm, true);
		mLeftUpperArm->addCollisionObject(mLeftElbow);

	// Right LowerArm
		mRightLowerArm = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mRightLowerArm, "Name")->set(mNamePrefix + "Right/LowerArm");
		PARAM(BoolValue, mRightLowerArm, "Dynamic")->set(true);

		agent->addObject(mRightLowerArm);

	// Left LowerArm
		mLeftLowerArm = dynamic_cast<BoxInertiaBody*>(boxInertiaBody->createCopy());
		PARAM(StringValue, mLeftLowerArm, "Name")->set(mNamePrefix + "Left/LowerArm");
		PARAM(BoolValue, mLeftLowerArm, "Dynamic")->set(true);

		agent->addObject(mLeftLowerArm);

	// Sensors
		QString groupPrefix = "/";
		groupPrefix.append(mGroupName).append("/");

		// Right Foot force sensors
		mForceSensorRightSoleOfFootRight = forceSensor->createCopy();
		PARAM(StringValue, mForceSensorRightSoleOfFootRight, "Name")->set(mNamePrefix + 
									"Right/ForceSensorSoleOfFootRight");
		PARAM(StringValue, mForceSensorRightSoleOfFootRight, "HostBodyName")->set(groupPrefix + ("Right/Foot"));
		PARAM(Vector3DValue, mForceSensorRightSoleOfFootRight, "LocalSensorAxis")->set(0.0, 1.0, 0.0);
		PARAM(StringValue, mForceSensorRightSoleOfFootRight, "CollisionObjectIndices")->set("1,3");
		//PARAM(StringValue, mForceSensorRightSoleOfFootRight, "CollisionObjectIndices")->set("3");
		agent->addObject(mForceSensorRightSoleOfFootRight);

		mForceSensorRightSoleOfFootLeft = forceSensor->createCopy();
		PARAM(StringValue, mForceSensorRightSoleOfFootLeft, "Name")->set(mNamePrefix + 
									"Right/ForceSensorSoleOfFootLeft");
		PARAM(StringValue, mForceSensorRightSoleOfFootLeft, "HostBodyName")->set(groupPrefix + ("Right/Foot"));
		PARAM(Vector3DValue, mForceSensorRightSoleOfFootLeft, "LocalSensorAxis")->set(0.0, 1.0, 0.0);
		PARAM(StringValue, mForceSensorRightSoleOfFootLeft, "CollisionObjectIndices")->set("2,4");
		//PARAM(StringValue, mForceSensorRightSoleOfFootLeft, "CollisionObjectIndices")->set("4");
		agent->addObject(mForceSensorRightSoleOfFootLeft);

		mForceSensorRightBallOfFootRight = forceSensor->createCopy();
		PARAM(StringValue, mForceSensorRightBallOfFootRight, "Name")->set(mNamePrefix + 
									"Right/ForceSensorBallOfFootRight");
		PARAM(StringValue, mForceSensorRightBallOfFootRight, "HostBodyName")->set(groupPrefix + ("Right/Foot"));
		PARAM(Vector3DValue, mForceSensorRightBallOfFootRight, "LocalSensorAxis")->set(0.0, 1.0, 0.0);
		PARAM(StringValue, mForceSensorRightBallOfFootRight, "CollisionObjectIndices")->set("5");
		agent->addObject(mForceSensorRightBallOfFootRight);

		mForceSensorRightBallOfFootLeft = forceSensor->createCopy();
		PARAM(StringValue, mForceSensorRightBallOfFootLeft, "Name")->set(mNamePrefix + 
									"Right/ForceSensorBallOfFootLeft");
		PARAM(StringValue, mForceSensorRightBallOfFootLeft, "HostBodyName")->set(groupPrefix + ("Right/Foot"));
		PARAM(Vector3DValue, mForceSensorRightBallOfFootLeft, "LocalSensorAxis")->set(0.0, 1.0, 0.0);
		PARAM(StringValue, mForceSensorRightBallOfFootLeft, "CollisionObjectIndices")->set("6");
		agent->addObject(mForceSensorRightBallOfFootLeft);

		// Left Foot force sensors
		mForceSensorLeftSoleOfFootRight = forceSensor->createCopy();
		PARAM(StringValue, mForceSensorLeftSoleOfFootRight, "Name")->set(mNamePrefix + 
									"Left/ForceSensorSoleOfFootRight");
		PARAM(StringValue, mForceSensorLeftSoleOfFootRight, "HostBodyName")->set(groupPrefix + ("Left/Foot"));
		PARAM(Vector3DValue, mForceSensorLeftSoleOfFootRight, "LocalSensorAxis")->set(0.0, 1.0, 0.0);
		PARAM(StringValue, mForceSensorLeftSoleOfFootRight, "CollisionObjectIndices")->set("1,3");
		//PARAM(StringValue, mForceSensorLeftSoleOfFootRight, "CollisionObjectIndices")->set("3");
		agent->addObject(mForceSensorLeftSoleOfFootRight);

		mForceSensorLeftSoleOfFootLeft = forceSensor->createCopy();
		PARAM(StringValue, mForceSensorLeftSoleOfFootLeft, "Name")->set(mNamePrefix + 
									"Left/ForceSensorSoleOfFootLeft");
		PARAM(StringValue, mForceSensorLeftSoleOfFootLeft, "HostBodyName")->set(groupPrefix + ("Left/Foot"));
		PARAM(Vector3DValue, mForceSensorLeftSoleOfFootLeft, "LocalSensorAxis")->set(0.0, 1.0, 0.0);
		PARAM(StringValue, mForceSensorLeftSoleOfFootLeft, "CollisionObjectIndices")->set("2,4");
		//PARAM(StringValue, mForceSensorLeftSoleOfFootLeft, "CollisionObjectIndices")->set("4");
		agent->addObject(mForceSensorLeftSoleOfFootLeft);

		mForceSensorLeftBallOfFootRight = forceSensor->createCopy();
		PARAM(StringValue, mForceSensorLeftBallOfFootRight, "Name")->set(mNamePrefix + 
									"Left/ForceSensorBallOfFootRight");
		PARAM(StringValue, mForceSensorLeftBallOfFootRight, "HostBodyName")->set(groupPrefix + ("Left/Foot"));
		PARAM(Vector3DValue, mForceSensorLeftBallOfFootRight, "LocalSensorAxis")->set(0.0, 1.0, 0.0);
		PARAM(StringValue, mForceSensorLeftBallOfFootRight, "CollisionObjectIndices")->set("5");
		agent->addObject(mForceSensorLeftBallOfFootRight);

		mForceSensorLeftBallOfFootLeft = forceSensor->createCopy();
		PARAM(StringValue, mForceSensorLeftBallOfFootLeft, "Name")->set(mNamePrefix + 
									"Left/ForceSensorBallOfFootLeft");
		PARAM(StringValue, mForceSensorLeftBallOfFootLeft, "HostBodyName")->set(groupPrefix + ("Left/Foot"));
		PARAM(Vector3DValue, mForceSensorLeftBallOfFootLeft, "LocalSensorAxis")->set(0.0, 1.0, 0.0);
		PARAM(StringValue, mForceSensorLeftBallOfFootLeft, "CollisionObjectIndices")->set("6");
		agent->addObject(mForceSensorLeftBallOfFootLeft);


	// Motors
	SimObject *dynamixel_m0 = pm->getPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_0);
	SimObject *dynamixel_m1 = pm->getPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_1);
	SimObject *dynamixel_m2 = pm->getPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_2);
	SimObject *dynamixel_m3 = pm->getPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_3);
	SimObject *dynamixel_m4 = pm->getPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_4);

	// Right Toes
	mRightToesJoint = dynamixel_m0->createCopy();
	PARAM(StringValue, mRightToesJoint, "Name")->set(mNamePrefix + ("Right/MotorToes"));
	PARAM(StringValue, mRightToesJoint, "FirstBody")->set(groupPrefix + "Right/Foot");
	PARAM(StringValue, mRightToesJoint, "SecondBody")->set(groupPrefix + ("Right/Toes"));

	agent->addObject(mRightToesJoint);

	// Left Toes
	mLeftToesJoint = dynamixel_m0->createCopy();
	PARAM(StringValue, mLeftToesJoint, "Name")->set(mNamePrefix + ("Left/MotorToes"));
	PARAM(StringValue, mLeftToesJoint, "FirstBody")->set(groupPrefix + "Left/Foot");
	PARAM(StringValue, mLeftToesJoint, "SecondBody")->set(groupPrefix + ("Left/Toes"));

	agent->addObject(mLeftToesJoint);

	// Right Ankle Pitch
	mRightAnklePitchMotor = dynamixel_m4->createCopy();
	PARAM(StringValue, mRightAnklePitchMotor, "Name")->set(mNamePrefix + ("Right/MotorAnklePitch"));
	PARAM(StringValue, mRightAnklePitchMotor, "FirstBody")->set(groupPrefix + "Right/Ankle");
	PARAM(StringValue, mRightAnklePitchMotor, "SecondBody")->set(groupPrefix + ("Right/Tibia"));

	agent->addObject(mRightAnklePitchMotor);

	// Left Ankle Pitch
	mLeftAnklePitchMotor = dynamixel_m4->createCopy();
	PARAM(StringValue, mLeftAnklePitchMotor, "Name")->set(mNamePrefix + ("Left/MotorAnklePitch"));
	PARAM(StringValue, mLeftAnklePitchMotor, "FirstBody")->set(groupPrefix + "Left/Ankle");
	PARAM(StringValue, mLeftAnklePitchMotor, "SecondBody")->set(groupPrefix + ("Left/Tibia"));

	agent->addObject(mLeftAnklePitchMotor);

	// Right Ankle Roll
	mRightAnkleRollMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mRightAnkleRollMotor, "Name")->set(mNamePrefix + ("Right/MotorAnkleRoll"));
	PARAM(StringValue, mRightAnkleRollMotor, "FirstBody")->set(groupPrefix + "Right/Foot");
	PARAM(StringValue, mRightAnkleRollMotor, "SecondBody")->set(groupPrefix + ("Right/Ankle"));

	agent->addObject(mRightAnkleRollMotor);

	// Left Ankle Roll
	mLeftAnkleRollMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mLeftAnkleRollMotor, "Name")->set(mNamePrefix + ("Left/MotorAnkleRoll"));
	PARAM(StringValue, mLeftAnkleRollMotor, "FirstBody")->set(groupPrefix + "Left/Foot");
	PARAM(StringValue, mLeftAnkleRollMotor, "SecondBody")->set(groupPrefix + ("Left/Ankle"));

	agent->addObject(mLeftAnkleRollMotor);

	// Right Knee
	mRightKneeMotor = dynamixel_m3->createCopy();
	PARAM(StringValue, mRightKneeMotor, "Name")->set(mNamePrefix + ("Right/MotorKneePitch"));
	PARAM(StringValue, mRightKneeMotor, "FirstBody")->set(groupPrefix + "Right/Tibia");
	PARAM(StringValue, mRightKneeMotor, "SecondBody")->set(groupPrefix + ("Right/Femur"));

	agent->addObject(mRightKneeMotor);

	// Left Knee
	mLeftKneeMotor = dynamixel_m3->createCopy();
	PARAM(StringValue, mLeftKneeMotor, "Name")->set(mNamePrefix + ("Left/MotorKneePitch"));
	PARAM(StringValue, mLeftKneeMotor, "FirstBody")->set(groupPrefix + "Left/Tibia");
	PARAM(StringValue, mLeftKneeMotor, "SecondBody")->set(groupPrefix + ("Left/Femur"));

	agent->addObject(mLeftKneeMotor);

	// Right HipRoll
	mRightHipRollMotor = dynamixel_m2->createCopy();
	PARAM(StringValue, mRightHipRollMotor, "Name")->set(mNamePrefix + ("Right/MotorHipRoll"));
	PARAM(StringValue, mRightHipRollMotor, "FirstBody")->set(groupPrefix + "Right/HipCross");
	PARAM(StringValue, mRightHipRollMotor, "SecondBody")->set(groupPrefix + ("Right/Hip"));

	agent->addObject(mRightHipRollMotor);

	// Left HipRoll
	mLeftHipRollMotor = dynamixel_m2->createCopy();
	PARAM(StringValue, mLeftHipRollMotor, "Name")->set(mNamePrefix + ("Left/MotorHipRoll"));
	PARAM(StringValue, mLeftHipRollMotor, "FirstBody")->set(groupPrefix + "Left/HipCross");
	PARAM(StringValue, mLeftHipRollMotor, "SecondBody")->set(groupPrefix + ("Left/Hip"));

	agent->addObject(mLeftHipRollMotor);

	// Right HipPitch
	mRightHipPitchMotor = dynamixel_m3->createCopy();
	PARAM(StringValue, mRightHipPitchMotor, "Name")->set(mNamePrefix + ("Right/MotorHipPitch"));
	PARAM(StringValue, mRightHipPitchMotor, "FirstBody")->set(groupPrefix + "Right/Femur");
	PARAM(StringValue, mRightHipPitchMotor, "SecondBody")->set(groupPrefix + ("Right/HipCross"));

	agent->addObject(mRightHipPitchMotor);

	// Left HipPitch
	mLeftHipPitchMotor = dynamixel_m3->createCopy();
	PARAM(StringValue, mLeftHipPitchMotor, "Name")->set(mNamePrefix + ("Left/MotorHipPitch"));
	PARAM(StringValue, mLeftHipPitchMotor, "FirstBody")->set(groupPrefix + "Left/Femur");
	PARAM(StringValue, mLeftHipPitchMotor, "SecondBody")->set(groupPrefix + ("Left/HipCross"));

	agent->addObject(mLeftHipPitchMotor);

	// Right HipYaw
	mRightHipYawMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mRightHipYawMotor, "Name")->set(mNamePrefix + ("Right/MotorHipYaw"));
	PARAM(StringValue, mRightHipYawMotor, "FirstBody")->set(groupPrefix + "Right/Hip");
	PARAM(StringValue, mRightHipYawMotor, "SecondBody")->set(groupPrefix + ("Middle/Pelvis"));

	agent->addObject(mRightHipYawMotor);

	// Left HipYaw
	mLeftHipYawMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mLeftHipYawMotor, "Name")->set(mNamePrefix + ("Left/MotorHipYaw"));
	PARAM(StringValue, mLeftHipYawMotor, "FirstBody")->set(groupPrefix + "Left/Hip");
	PARAM(StringValue, mLeftHipYawMotor, "SecondBody")->set(groupPrefix + ("Middle/Pelvis"));

	agent->addObject(mLeftHipYawMotor);

	// WaistRoll
	mWaistRollMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mWaistRollMotor, "Name")->set(mNamePrefix + ("Middle/MotorWaistRoll"));
	PARAM(StringValue, mWaistRollMotor, "FirstBody")->set(groupPrefix + "Middle/Pelvis");
	PARAM(StringValue, mWaistRollMotor, "SecondBody")->set(groupPrefix + ("Middle/UpperBody"));

	agent->addObject(mWaistRollMotor);

	// HeadPitch
	mHeadPitchMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mHeadPitchMotor, "Name")->set(mNamePrefix + ("Middle/MotorHeadPitch"));
	PARAM(StringValue, mHeadPitchMotor, "FirstBody")->set(groupPrefix + "Middle/UpperBody");
	PARAM(StringValue, mHeadPitchMotor, "SecondBody")->set(groupPrefix + ("Middle/HeadPitchPart"));

	agent->addObject(mHeadPitchMotor);

	// HeadYaw
	mHeadYawMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mHeadYawMotor, "Name")->set(mNamePrefix + ("Middle/MotorHeadYaw"));
	PARAM(StringValue, mHeadYawMotor, "FirstBody")->set(groupPrefix + "Middle/HeadPitchPart");
	PARAM(StringValue, mHeadYawMotor, "SecondBody")->set(groupPrefix + ("Middle/HeadYawPart"));

	agent->addObject(mHeadYawMotor);

	// HeadRoll
	mHeadRollMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mHeadRollMotor, "Name")->set(mNamePrefix + ("Middle/MotorHeadRoll"));
	PARAM(StringValue, mHeadRollMotor, "FirstBody")->set(groupPrefix + "Middle/HeadYawPart");
	PARAM(StringValue, mHeadRollMotor, "SecondBody")->set(groupPrefix + ("Middle/HeadRollPart"));

	agent->addObject(mHeadRollMotor);

	// Right Shoulder Pitch
	mRightShoulderPitchMotor = dynamixel_m2->createCopy();
	PARAM(StringValue, mRightShoulderPitchMotor, "Name")->set(mNamePrefix + ("Right/MotorShoulderPitch"));
	PARAM(StringValue, mRightShoulderPitchMotor, "FirstBody")->set(groupPrefix + "Middle/UpperBody");
	PARAM(StringValue, mRightShoulderPitchMotor, "SecondBody")->set(groupPrefix + ("Right/Shoulder"));

	agent->addObject(mRightShoulderPitchMotor);

	// Left Shoulder Pitch
	mLeftShoulderPitchMotor = dynamixel_m2->createCopy();
	PARAM(StringValue, mLeftShoulderPitchMotor, "Name")->set(mNamePrefix + ("Left/MotorShoulderPitch"));
	PARAM(StringValue, mLeftShoulderPitchMotor, "FirstBody")->set(groupPrefix + "Middle/UpperBody");
	PARAM(StringValue, mLeftShoulderPitchMotor, "SecondBody")->set(groupPrefix + ("Left/Shoulder"));

	agent->addObject(mLeftShoulderPitchMotor);

	// Right Shoulder Roll
	mRightShoulderRollMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mRightShoulderRollMotor, "Name")->set(mNamePrefix + ("Right/MotorShoulderRoll"));
	PARAM(StringValue, mRightShoulderRollMotor, "FirstBody")->set(groupPrefix + "Right/Shoulder");
	PARAM(StringValue, mRightShoulderRollMotor, "SecondBody")->set(groupPrefix + ("Right/UpperArm"));

	agent->addObject(mRightShoulderRollMotor);

	// Left Shoulder Roll
	mLeftShoulderRollMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mLeftShoulderRollMotor, "Name")->set(mNamePrefix + ("Left/MotorShoulderRoll"));
	PARAM(StringValue, mLeftShoulderRollMotor, "FirstBody")->set(groupPrefix + "Left/Shoulder");
	PARAM(StringValue, mLeftShoulderRollMotor, "SecondBody")->set(groupPrefix + ("Left/UpperArm"));

	agent->addObject(mLeftShoulderRollMotor);

	// Right Elbow Pitch
	mRightElbowPitchMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mRightElbowPitchMotor, "Name")->set(mNamePrefix + ("Right/MotorElbowPitch"));
	PARAM(StringValue, mRightElbowPitchMotor, "FirstBody")->set(groupPrefix + "Right/UpperArm");
	PARAM(StringValue, mRightElbowPitchMotor, "SecondBody")->set(groupPrefix + ("Right/LowerArm"));

	agent->addObject(mRightElbowPitchMotor);

	// Left Elbow Pitch
	mLeftElbowPitchMotor = dynamixel_m1->createCopy();
	PARAM(StringValue, mLeftElbowPitchMotor, "Name")->set(mNamePrefix + ("Left/MotorElbowPitch"));
	PARAM(StringValue, mLeftElbowPitchMotor, "FirstBody")->set(groupPrefix + "Left/UpperArm");
	PARAM(StringValue, mLeftElbowPitchMotor, "SecondBody")->set(groupPrefix + ("Left/LowerArm"));

	agent->addObject(mLeftElbowPitchMotor);

	//Add accelboards
	QList<SimObject*> accelBoards;

	mAB00 = accelBoard->createCopy();
// 	mAB00->setName(mNamePrefix + ("ABRightLegLowerBottom_AB00"));
	mAB00->setName(mNamePrefix + ("Right/AB_LegLowerBottom"));
	PARAM(StringValue, mAB00, "ReferenceBody")->set(groupPrefix + "Right/Tibia");
	accelBoards.append(mAB00);
	agent->addObject(mAB00);

	mAB10 = accelBoard->createCopy();
// 	mAB10->setName(mNamePrefix + ("ABLeftLegLowerBottom_AB10"));
	mAB10->setName(mNamePrefix + ("Left/AB_LegLowerBottom"));
	PARAM(StringValue, mAB10, "ReferenceBody")->set(groupPrefix + "Left/Tibia");
	accelBoards.append(mAB10);
	agent->addObject(mAB10);

	mAB01 = accelBoard->createCopy();
// 	mAB01->setName(mNamePrefix + ("ABRightLegLowerTop_AB01"));
	mAB01->setName(mNamePrefix + ("Right/AB_LegLowerTop"));
	PARAM(StringValue, mAB01, "ReferenceBody")->set(groupPrefix + "Right/Tibia");
	accelBoards.append(mAB01);
	agent->addObject(mAB01);

	mAB11 = accelBoard->createCopy();
// 	mAB11->setName(mNamePrefix + ("ABLeftLegLowerTop_AB11"));
	mAB11->setName(mNamePrefix + ("Left/AB_LegLowerTop"));
	PARAM(StringValue, mAB11, "ReferenceBody")->set(groupPrefix + "Left/Tibia");
	accelBoards.append(mAB11);
	agent->addObject(mAB11);

	mAB02 = accelBoard->createCopy();
// 	mAB02->setName(mNamePrefix + ("ABRightLegUpperBottom_AB02"));
	mAB02->setName(mNamePrefix + ("Right/AB_LegUpperBottom"));
	PARAM(StringValue, mAB02, "ReferenceBody")->set(groupPrefix + "Right/Femur");
	accelBoards.append(mAB02);
	agent->addObject(mAB02);

	mAB12 = accelBoard->createCopy();
// 	mAB12->setName(mNamePrefix + ("ABLeftLegUpperBottom_AB12"));
	mAB12->setName(mNamePrefix + ("Left/AB_LegUpperBottom"));
	PARAM(StringValue, mAB12, "ReferenceBody")->set(groupPrefix + "Left/Femur");
	accelBoards.append(mAB12);
	agent->addObject(mAB12);

	mAB03 = accelBoard->createCopy();
// 	mAB03->setName(mNamePrefix + ("ABRightLegUpperMiddle_AB03"));
	mAB03->setName(mNamePrefix + ("Right/AB_LegUpperMiddle"));
	PARAM(StringValue, mAB03, "ReferenceBody")->set(groupPrefix + "Right/Femur");
	accelBoards.append(mAB03);
	agent->addObject(mAB03);

	mAB13 = accelBoard->createCopy();
// 	mAB13->setName(mNamePrefix + ("ABLeftLegUpperMiddle_AB13"));
	mAB13->setName(mNamePrefix + ("Left/AB_LegUpperMiddle"));
	PARAM(StringValue, mAB13, "ReferenceBody")->set(groupPrefix + "Left/Femur");
	accelBoards.append(mAB13);
	agent->addObject(mAB13);

	mAB04 = accelBoard->createCopy();
// 	mAB04->setName(mNamePrefix + ("ABRightLegUpperTop_AB04"));
	mAB04->setName(mNamePrefix + ("Right/AB_LegUpperTop"));
	PARAM(StringValue, mAB04, "ReferenceBody")->set(groupPrefix + "Right/Femur");
	accelBoards.append(mAB04);
	agent->addObject(mAB04);

	mAB14 = accelBoard->createCopy();
// 	mAB14->setName(mNamePrefix + ("ABLeftLegUpperTop_AB14"));
	mAB14->setName(mNamePrefix + ("Left/AB_LegUpperTop"));
	PARAM(StringValue, mAB14, "ReferenceBody")->set(groupPrefix + "Left/Femur");
	accelBoards.append(mAB14);
	agent->addObject(mAB14);

	mAB20 = accelBoard->createCopy();
// 	mAB20->setName(mNamePrefix + ("ABRightArmLower_AB20"));
	mAB20->setName(mNamePrefix + ("Right/AB_ArmLower"));
	PARAM(StringValue, mAB20, "ReferenceBody")->set(groupPrefix + "Right/LowerArm");
	accelBoards.append(mAB20);
	agent->addObject(mAB20);

	mAB30 = accelBoard->createCopy();
// 	mAB30->setName(mNamePrefix + ("ABLeftArmLower_AB30"));
	mAB30->setName(mNamePrefix + ("Left/AB_ArmLower"));
	PARAM(StringValue, mAB30, "ReferenceBody")->set(groupPrefix + "Left/LowerArm");
	accelBoards.append(mAB30);
	agent->addObject(mAB30);

	mAB21 = accelBoard->createCopy();
// 	mAB21->setName(mNamePrefix + ("ABRightArmUpper_AB21"));
	mAB21->setName(mNamePrefix + ("Right/AB_ArmUpper"));
	PARAM(StringValue, mAB21, "ReferenceBody")->set(groupPrefix + "Right/UpperArm");
	accelBoards.append(mAB21);
	agent->addObject(mAB21);

	mAB31 = accelBoard->createCopy();
// 	mAB31->setName(mNamePrefix + ("ABLeftArmUpper_AB31"));
	mAB31->setName(mNamePrefix + ("Left/AB_ArmUpper"));
	PARAM(StringValue, mAB31, "ReferenceBody")->set(groupPrefix + "Left/UpperArm");
	accelBoards.append(mAB31);
	agent->addObject(mAB31);

	mAB40 = accelBoard->createCopy();
// 	mAB40->setName(mNamePrefix + ("ABLeftBodyUpper_AB40"));
	mAB40->setName(mNamePrefix + ("Left/AB_BodyUpper"));
	PARAM(StringValue, mAB40, "ReferenceBody")->set(groupPrefix + "Middle/UpperBody");
	accelBoards.append(mAB40);
	agent->addObject(mAB40);

	mAB41 = accelBoard->createCopy();
// 	mAB41->setName(mNamePrefix + ("ABRightBodyUpper_AB41"));
	mAB41->setName(mNamePrefix + ("Right/AB_BodyUpper"));
	PARAM(StringValue, mAB41, "ReferenceBody")->set(groupPrefix + "Middle/UpperBody");
	accelBoards.append(mAB41);
	agent->addObject(mAB41);

	mAB42 = accelBoard->createCopy();
// 	mAB42->setName(mNamePrefix + ("ABLeftBodyLower_AB42"));
	mAB42->setName(mNamePrefix + ("Left/AB_BodyLower"));
	PARAM(StringValue, mAB42, "ReferenceBody")->set(groupPrefix + "Middle/UpperBody");
	accelBoards.append(mAB42);
	agent->addObject(mAB42);

	mAB43 = accelBoard->createCopy();
// 	mAB43->setName(mNamePrefix + ("ABRightBodyLower_AB43"));
	mAB43->setName(mNamePrefix + ("Right/AB_BodyLower"));
	PARAM(StringValue, mAB43, "ReferenceBody")->set(groupPrefix + "Middle/UpperBody");
	accelBoards.append(mAB43);
	agent->addObject(mAB43);

	mAB44 = accelBoard->createCopy();
// 	mAB44->setName(mNamePrefix + ("ABBodyBottom_AB44"));
	mAB44->setName(mNamePrefix + ("Middle/AB_BodyBottom"));
	PARAM(StringValue, mAB44, "ReferenceBody")->set(groupPrefix + "Middle/UpperBody");
	accelBoards.append(mAB44);
	agent->addObject(mAB44);

	mAB50 = accelBoard->createCopy();
// 	mAB50->setName(mNamePrefix + ("ABHeadLeft_AB50"));
	mAB50->setName(mNamePrefix + ("Middle/AB_HeadLeft"));
	PARAM(StringValue, mAB50, "ReferenceBody")->set(groupPrefix + "Middle/HeadRollPart");
	accelBoards.append(mAB50);
	agent->addObject(mAB50);

	mAB51 = accelBoard->createCopy();
// 	mAB51->setName(mNamePrefix + ("ABHeadRight_AB51"));
	mAB51->setName(mNamePrefix + ("Middle/AB_HeadRight"));
	PARAM(StringValue, mAB51, "ReferenceBody")->set(groupPrefix + "Middle/HeadRollPart");
	accelBoards.append(mAB51);
	agent->addObject(mAB51);

	//Add current consumption sensors
	//RightAnkleRoll
	SimObject *ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/AnkleRoll/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorAnkleRoll/CurrentConsumption0");
	agent->addObject(ccs);

	//RightAnklePitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/AnklePitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorAnklePitch/CurrentConsumption0, " 
				+ "/Sim/" + mGroupName + "/Right/MotorAnklePitch/CurrentConsumption1");
	agent->addObject(ccs);

	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/AnklePitch/CurrentSensor1"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorAnklePitch/CurrentConsumption1, " 
				+ "/Sim/" + mGroupName + "/Right/MotorAnklePitch/CurrentConsumption2");
	agent->addObject(ccs);

	//RightKneePitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/KneePitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorKneePitch/CurrentConsumption1, " 
				+ "/Sim/" + mGroupName + "/Right/MotorKneePitch/CurrentConsumption2");
	agent->addObject(ccs);

	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/KneePitch/CurrentSensor1"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorKneePitch/CurrentConsumption0");
	agent->addObject(ccs);

	//RightHipRoll
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/HipRoll/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorHipRoll/CurrentConsumption0");
	agent->addObject(ccs);

	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/HipRoll/CurrentSensor1"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorHipRoll/CurrentConsumption1");
	agent->addObject(ccs);

	//RightHipPitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/HipPitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorHipPitch/CurrentConsumption0, " 
				+ "/Sim/" + mGroupName + "/Right/MotorHipPitch/CurrentConsumption1");
	agent->addObject(ccs);

	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/HipPitch/CurrentSensor1"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorHipPitch/CurrentConsumption2");
	agent->addObject(ccs);

	//RightHipYaw
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/HipYaw/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorHipYaw/CurrentConsumption0");
	agent->addObject(ccs);

	//LeftAnkleRoll
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/AnkleRoll/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorAnkleRoll/CurrentConsumption0");
	agent->addObject(ccs);

	//LeftAnklePitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/AnklePitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorAnklePitch/CurrentConsumption0, " 
				+ "/Sim/" + mGroupName + "/Left/MotorAnklePitch/CurrentConsumption1");
	agent->addObject(ccs);

	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/AnklePitch/CurrentSensor1"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorAnklePitch/CurrentConsumption1, " 
				+ "/Sim/" + mGroupName + "/Left/MotorAnklePitch/CurrentConsumption2");
	agent->addObject(ccs);

	//LeftKneePitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/KneePitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorKneePitch/CurrentConsumption1, " 
				+ "/Sim/" + mGroupName + "/Left/MotorKneePitch/CurrentConsumption2");
	agent->addObject(ccs);

	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/KneePitch/CurrentSensor1"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorKneePitch/CurrentConsumption0");
	agent->addObject(ccs);

	//LeftHipRoll
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/HipRoll/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorHipRoll/CurrentConsumption0");
	agent->addObject(ccs);

	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/HipRoll/CurrentSensor1"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorHipRoll/CurrentConsumption1");
	agent->addObject(ccs);

	//LeftHipPitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/HipPitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorHipPitch/CurrentConsumption0, " 
				+ "/Sim/" + mGroupName + "/Left/MotorHipPitch/CurrentConsumption1");
	agent->addObject(ccs);

	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/HipPitch/CurrentSensor1"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorHipPitch/CurrentConsumption2");
	agent->addObject(ccs);

	//LeftHipYaw
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/HipYaw/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorHipYaw/CurrentConsumption0");
	agent->addObject(ccs);

	//WaistRoll
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Middle/WaistRoll/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Middle/MotorWaistRoll/CurrentConsumption0");
	agent->addObject(ccs);

	//RightShoulderRoll
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/ShoulderRoll/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorShoulderRoll/CurrentConsumption0");
	agent->addObject(ccs);

	//RightShoulderPitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/ShoulderPitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorShoulderPitch/CurrentConsumption0");
	agent->addObject(ccs);

	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/ShoulderPitch/CurrentSensor1"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorShoulderPitch/CurrentConsumption1");
	agent->addObject(ccs);

	//RightElbowPitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Right/ElbowPitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Right/MotorElbowPitch/CurrentConsumption0");
	agent->addObject(ccs);

	//LeftShoulderRoll
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/ShoulderRoll/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorShoulderRoll/CurrentConsumption0");
	agent->addObject(ccs);

	//LeftShoulderPitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/ShoulderPitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorShoulderPitch/CurrentConsumption0");
	agent->addObject(ccs);

	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/ShoulderPitch/CurrentSensor1"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorShoulderPitch/CurrentConsumption1");
	agent->addObject(ccs);

	//LeftElbowPitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Left/ElbowPitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Left/MotorElbowPitch/CurrentConsumption0");
	agent->addObject(ccs);

	//HeadPitch
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Middle/HeadPitch/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(1.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Middle/MotorHeadPitch/CurrentConsumption0");
	agent->addObject(ccs);

	//HeadRoll and HeadYaw
	ccs = currentConsumptionSensor->createCopy();
	PARAM(StringValue, ccs, "Name")->set(mNamePrefix + ("Middle/HeadRollYaw/CurrentSensor0"));
	PARAM(DoubleValue, ccs, "Min")->set(-1.0);
	PARAM(DoubleValue, ccs, "Max")->set(2.0);
	PARAM(DoubleValue, ccs, "Noise")->set(0.0001);
	PARAM(StringValue, ccs, "TorqueValueList")
			->set("/Sim/" + mGroupName + "/Middle/MotorHeadRoll/CurrentConsumption0, "
				+ "/Sim/" + mGroupName + "/Middle/MotorHeadYaw/CurrentConsumption0");
	agent->addObject(ccs);


	mSimObjects = agent->getObjects();

	//add agent SimObjectGroup at PhysicsManager to provide the interface to the robot.
	pm->addSimObjectGroup(agent);

	// Setup Accelboards
	for(QListIterator<SimObject*> i(accelBoards); i.hasNext();) {
		i.next()->setup();
	}

	// Disable collision between objects which are allowed to interpenetrate
	CollisionManager *collisionManager = Physics::getCollisionManager();

	collisionManager->disableCollisions(mLeftFoot->getCollisionObjects(), 
		mLeftTibia->getCollisionObjects(), true);
	collisionManager->disableCollisions(mRightFoot->getCollisionObjects(), 
		mRightTibia->getCollisionObjects(), true);
	collisionManager->disableCollisions(mLeftFemur->getCollisionObjects(), 
		mLeftTibia->getCollisionObjects(), true);
	collisionManager->disableCollisions(mRightFemur->getCollisionObjects(), 
		mRightTibia->getCollisionObjects(), true);
	collisionManager->disableCollisions(mLeftFemur->getCollisionObjects(), 
		mLeftHip->getCollisionObjects(), true);
	collisionManager->disableCollisions(mRightFemur->getCollisionObjects(), 
		mRightHip->getCollisionObjects(), true);
	collisionManager->disableCollisions(mUpperBody->getCollisionObjects(), 
		mPelvis->getCollisionObjects(), true);
	collisionManager->disableCollisions(mUpperBody->getCollisionObjects(),
		mLeftHip->getCollisionObjects(), true);
	collisionManager->disableCollisions(mUpperBody->getCollisionObjects(),
		mRightHip->getCollisionObjects(), true);
	collisionManager->disableCollisions(mHeadPitchPart->getCollisionObjects(),
		mUpperBody->getCollisionObjects(), true);
	collisionManager->disableCollisions(mLeftShoulder->getCollisionObjects(),
		mLeftUpperArm->getCollisionObjects(), true);
	collisionManager->disableCollisions(mRightShoulder->getCollisionObjects(),
		mRightUpperArm->getCollisionObjects(), true);
	collisionManager->disableCollisions(mLeftAnkle->getCollisionObjects(),
		mLeftFoot->getCollisionObjects(), true);
	collisionManager->disableCollisions(mRightAnkle->getCollisionObjects(),
		mRightFoot->getCollisionObjects(), true);
	collisionManager->disableCollisions(mLeftAnkle->getCollisionObjects(),
		mLeftTibia->getCollisionObjects(), true);
	collisionManager->disableCollisions(mRightAnkle->getCollisionObjects(),
		mRightTibia->getCollisionObjects(), true);
	collisionManager->disableCollisions(mLeftHip->getCollisionObjects(),
		mLeftHipCross->getCollisionObjects(), true);
	collisionManager->disableCollisions(mRightHip->getCollisionObjects(),
		mRightHipCross->getCollisionObjects(), true);
	collisionManager->disableCollisions(mLeftHipCross->getCollisionObjects(),
		mLeftFemur->getCollisionObjects(), true);
	collisionManager->disableCollisions(mRightHipCross->getCollisionObjects(),
		mRightFemur->getCollisionObjects(), true);
	collisionManager->disableCollisions(mHeadPitchPart->getCollisionObjects(),
		mHeadYawPart->getCollisionObjects(), true);
	collisionManager->disableCollisions(mHeadRollPart->getCollisionObjects(),
		mHeadYawPart->getCollisionObjects(), true);
	collisionManager->disableCollisions(mHeadPitchPart->getCollisionObjects(),
		mHeadRollPart->getCollisionObjects(), true);
}


void MSeriesV1Model::layoutObjects() {

	// ----------------------------------------------------------
	// Masses and Inertias, absolute values. TODO: concept for parameterization needed!
	// ----------------------------------------------------------

/* Absolute Values
		double rightFootComX = -0.08;
		double leftFootComX = -rightFootComX;
		double rightFootComY = 0.018;
		double leftFootComY = rightFootComY;
		double rightFootComZ = 0.055;
		double leftFootComZ = rightFootComZ;
*/
		double rightFootComX = -0.005;
		double leftFootComX = -rightFootComX;
		double rightFootComY = -0.022;
		double leftFootComY = rightFootComY;
		double rightFootComZ = 0.058;
		double leftFootComZ = rightFootComZ;

		double footWeight = 0.160;		// Body with inertia tensor set. So the complete mass is in the footWeight (without toes, because toes are extra body)

		Matrix3x3 rightFootInertia(0.000430, 0.0, 0.0, 0.0, 0.000430, 0.0, 0.0, 0.0, 0.000120);
		Matrix3x3 leftFootInertia(0.000430, 0.0, 0.0, 0.0, 0.000430, 0.0, 0.0, 0.0, 0.000120);

		double toesWeight = 0.032;		// Body with inertia tensor set.
		double tipToesWeight = 0.0;

/* Absolute Values
		double rightToesComX = -0.081;
		double rightToesComY = 0.012;
		double rightToesComZ = 0.103;
*/
		double rightToesComX = -0.006;
		double rightToesComY = 0.003;
		double rightToesComZ = -0.0035;

		double leftToesComX = -rightToesComX;
		double leftToesComY = rightToesComY;
		double leftToesComZ = rightToesComZ;

		Matrix3x3 rightToesInertia(0.000013, 0.0, 0.0, 0.0, 0.000030, 0.0, 0.0, 0.0, 0.000023);
		Matrix3x3 leftToesInertia(0.000013, 0.0, 0.0, 0.0, 0.000030, 0.0, 0.0, 0.0, 0.000023);

		// Ankle is a small invisible BoxInertiaBody connecting the Tibia and the foot
		double ankleWeight = 0.180;

/* Absolute Values
		double rightAnkleComX = -0.075;
		double rightAnkleComY = 0.045;
		double rightAnkleComZ = -0.005;
*/
		double rightAnkleComX = 0.0;
		double rightAnkleComY = -0.013;
		double rightAnkleComZ = -0.005;

		double leftAnkleComX = -rightAnkleComX;
		double leftAnkleComY = rightAnkleComY;
		double leftAnkleComZ = rightAnkleComZ;

		Matrix3x3 rightAnkleInertia(0.00013, 0.0, 0.0, 0.0, 0.00011, 0.0, 0.0, 0.0, 0.00011);
		Matrix3x3 leftAnkleInertia(0.00013, 0.0, 0.0, 0.0, 0.00011, 0.0, 0.0, 0.0, 0.00011);

		double kneeWeight = 0.0;

		double rightTibiaComX;
		double rightTibiaComY;
			double rightTibiaComZ;
		Matrix3x3 rightTibiaInertia;
		Matrix3x3 leftTibiaInertia;
		double tibiaWeight;

		if(mIncludeAccus->get()) {

/* Absolute Values
			rightTibiaComX = -0.071;
			rightTibiaComY = 0.214;
			rightTibiaComZ = -0.013;
*/
			rightTibiaComX = 0.004;
			rightTibiaComY = 0.019;
			rightTibiaComZ = -0.013;

			rightTibiaInertia.set(0.00385, -0.00007, -0.00007, -0.00007, 0.00098, -0.00019, -0.00007, -0.00019, 0.00385);
			leftTibiaInertia.set(0.00385, 0.00007, 0.00007, 0.00007, 0.00098, -0.00019, 0.00007, -0.00019, 0.00385);

			tibiaWeight = 0.973;
		} else {
/* Absolute Values
			rightTibiaComX = -0.075;
			rightTibiaComY = 0.195;
			rightTibiaComZ = -0.001;
*/
			rightTibiaComX = 0.0;
			rightTibiaComY = 0.0;
			rightTibiaComZ = -0.001;

			rightTibiaInertia.set(0.00277, -0.00018, 0.0, -0.00018, 0.00058, 0.00016, 0.0, 0.00016, 0.0029);
			leftTibiaInertia.set(0.00277, 0.00018, 0.0, 0.00018, 0.00058, 0.00016, 0.0, 0.00016, 0.0029);

			tibiaWeight = 0.612;
		}
		double leftTibiaComX = -rightTibiaComX;
		double leftTibiaComY = rightTibiaComY;
		double leftTibiaComZ = rightTibiaComZ;

/* Absolute Values
		double rightFemurComX = -0.075;
		double rightFemurComY = 0.452;
		double rightFemurComZ = -0.014;
*/
		double rightFemurComX = 0.0;
		double rightFemurComY = -0.0175;
		double rightFemurComZ = -0.014;

		double leftFemurComX = -rightFemurComX;
		double leftFemurComY = rightFemurComY;
		double leftFemurComZ = rightFemurComZ;

		double femurWeight = 0.849;

		Matrix3x3 rightFemurInertia(0.0039, 0.0001, -0.0001, 0.0001, 0.0008, -0.0001, -0.0001, -0.0001, 0.0039);
		Matrix3x3 leftFemurInertia(0.0039, -0.0001, 0.0001, -0.0001, 0.0008, -0.0001, 0.0001, -0.0001, 0.0039);

		// Hip. Hip is a body on top of the femur which can be rotated with the HipYawJoint.
		// The Weight and Inertia properties include the swivel in the torso which is also rotated in the
		// real robot.
		double hipWeight = 0.443;

/* Absolute Values
		double rightHipComX = -0.083;
		double rightHipComY = 0.679;
		double rightHipComZ = -0.011;
*/
		double rightHipComX = 0.0035;
		double rightHipComY = 0.022;
		double rightHipComZ = -0.011;

		double leftHipComX = -rightHipComX;
		double leftHipComY = rightHipComY;
		double leftHipComZ = rightHipComZ;

		Matrix3x3 rightHipInertia(0.00071, 0.00005, 0.00002, 0.00005, 0.00059, 0.00009, 0.00002, 0.00009, 0.00075);
		Matrix3x3 leftHipInertia(0.00071, -0.00005, -0.00002, -0.00005, 0.00059, 0.00009, -0.00002, 0.00009, 0.00075);

		// HipCross. HipCross is an invisible small body connecting the hip and the femur. It can be rotated by
		// the HipRollJoint.
		double hipCrossWeight = 0.076;

/* Absolute Values
		double rightHipCrossComX = -0.069;
		double rightHipCrossComY = 0.604;
		double rightHipCrossComZ = -0.005;
*/
		double rightHipCrossComX = 0.006;
		double rightHipCrossComY = -0.003;
		double rightHipCrossComZ = -0.005;

		double leftHipCrossComX = -rightHipCrossComX;
		double leftHipCrossComY = rightHipCrossComY;
		double leftHipCrossComZ = rightHipCrossComZ;

		Matrix3x3 rightHipCrossInertia(0.00006, 0.0, 0.0, 0.0, 0.00006, 0.0, 0.0, 0.0, 0.00004);
		Matrix3x3 leftHipCrossInertia(0.00006, 0.0, 0.0, 0.0, 0.00006, 0.0, 0.0, 0.0, 0.00004);

		// Pelvis. Pelvis is a box body on top of the hips and under the chest and the waist roll joint.
		// This does not include thw swivels in the torso which hold the hips!
		double pelvisWeight = 0.396;

/* Absolute Values
		double pelvisComX = 0.0;
		double pelvisComY = 0.732;
		double pelvisComZ = 0.003;
*/
		double pelvisComX = 0.0;
		double pelvisComY = 0.01;
		double pelvisComZ = 0.003;

		Matrix3x3 pelvisInertia(0.0010, 0.0, 0.0, 0.0, 0.0019, 0.0, 0.0, 0.0, 0.0016);

		// UpperBody is a body on top of the pelvis, below the head and between the shoulders. It is the torso without the pelvis and the shoulders.
		// The weight includes the part of the throat which is cannot be moved by the head joints!
		// The weight does not include the parts that are rotated with the ShoulderRoll joints!
		// UpperBody can be moved with the WaistRollJoint.
		double upperBodyWeight;
		Matrix3x3 upperBodyInertia;
		double upperBodyComX;
		double upperBodyComY;
		double upperBodyComZ;

		if(mIncludeAccus->get()) {
			upperBodyWeight = 1.648;
			upperBodyInertia.set(0.00878, 0.00003, 0.0, 0.00003, 0.00936, 0.00024, 0.0, 0.00024, 0.01302);
/* Absolute Values
			upperBodyComX = 0.0;
			upperBodyComY = 0.907;
			upperBodyComZ = -0.007;
*/
			upperBodyComX = 0.0;
			upperBodyComY = 0.034;
			upperBodyComZ = -0.007;
		} else {
			upperBodyWeight = 1.287;
			upperBodyInertia.set(0.00771, 0.00003, 0.0, 0.00003, 0.00852, 0.00003, 0.0, 0.00003, 0.01258);
/* Absolute Values
			upperBodyComX = 0.0;
			upperBodyComY = 0.910;
			upperBodyComZ = 0.004;
*/
			upperBodyComX = 0.0;
			upperBodyComY = 0.037;
			upperBodyComZ = 0.004;
		}

		// HeadPitchPart is the body which is moved by the HeadPitchJoint and to which the HeadYawJoint is connected.
		double headPitchPartWeight = 0.099;
/* Absolute Values
		double headPitchPartComX = 0.0;
		double headPitchPartComY = 1.065;
		double headPitchPartComZ = -0.009;
*/
		double headPitchPartComX = 0.0;
		double headPitchPartComY = -0.018;
		double headPitchPartComZ = -0.009;

		Matrix3x3 headPitchPartInertia(0.00004, 0.0, 0.0, 0.0, 0.00004, 0.0, 0.0, 0.0, 0.00004);

		// HeadYawPart is a small invisible BoxInertiaBody connecting the HeadPitchPart and HeadRollPart
		double headYawPartWeight = 0.099;
/* Absolute Values
		double headYawPartComX = 0.0;
		double headYawPartComY = 1.124;
		double headYawPartComZ = 0.016;
*/
		double headYawPartComX = 0.0;
		double headYawPartComY = 0.018;
		double headYawPartComZ = 0.011;

		Matrix3x3 headYawPartInertia(0.00005, 0.0, 0.0, 0.0, 0.00004, 0.0, 0.0, 0.0, 0.00004);

		// HeadRollPart is the upper body of the head which is moved by the HeadRollJoint.
		double headRollPartWeight;
		double headRollPartComX;
		double headRollPartComY;
		double headRollPartComZ;
		Matrix3x3 headRollPartInertia;

		if(mIncludeAccus->get()) {
			headRollPartWeight = 1.008;
/* Absolute Values
			headRollPartComX = 0.0;
			headRollPartComY = 1.141;
			headRollPartComZ = -0.028;
*/
			headRollPartComX = 0.0;
			headRollPartComY = -0.005;
			headRollPartComZ = -0.049;

			headRollPartInertia.set(0.00445, 0.00002, 0.0, 0.00002, 0.00551, 0.00011, 0.0, 0.00011, 0.00249);
		} else {
			headRollPartWeight = 0.647;
/* Absolute Values
			headRollPartComX = 0.0;
			headRollPartComY = 1.148;
			headRollPartComZ = -0.003;
*/
			headRollPartComX = 0.0;
			headRollPartComY = 0.002;
			headRollPartComZ = -0.018;

			headRollPartInertia.set(0.00301, 0.00001, 0.0, 0.00001, 0.004, -0.00022, 0.0, -0.00022, 0.00202);
		}

		// Shoulder is a box body which is rotated by the ShoulderPitchJoint. The mass includes everything that is rotated in the shoulder
		// and the torso!
		double shoulderWeight = 0.203;
/* Absolute Values
		double rightShoulderComX = -0.143;
		double shoulderComY = 0.956;
		double shoulderComZ = 0.0;
*/
		double rightShoulderComX = 0.047;
		double shoulderComY = -0.001;
		double shoulderComZ = 0.0;

		double leftShoulderComX = -rightShoulderComX;

		Matrix3x3 rightShoulderInertia(0.00026, 0.0, 0.00001, 0.0, 0.00029, 0.0, 0.00001, 0.0, 0.00028);
		Matrix3x3 leftShoulderInertia(0.00026, 0.0, -0.00001, 0.0, 0.00029, 0.0, -0.00001, 0.0, 0.00028);

		// UpperArm is a box body which is connected to the ShoulderPitchJoint and a cylinder geom at the bottom (the elbow).
		double upperArmWeight;
		double rightUpperArmComX;
		double leftUpperArmComX;
		double upperArmComY;
		double upperArmComZ;

		Matrix3x3 rightUpperArmInertia;
		Matrix3x3 leftUpperArmInertia;

		if(mIncludeAccus->get()) {
			upperArmWeight = 0.497;
/* Absolute Values
			rightUpperArmComX = -0.205;
			upperArmComY = 0.816;
			upperArmComZ = -0.014;
*/
			rightUpperArmComX = -0.008;
			upperArmComY = -0.0099;
			upperArmComZ = -0.014;

			rightUpperArmInertia.set(0.00113, 0.00004, 0.0, 0.00004, 0.00031, 0.0, 0.0, 0.0, 0.00114);
			leftUpperArmInertia.set(0.00113, -0.00004, 0.0, -0.00004, 0.00031, 0.0, 0.0, 0.0, 0.00114);
		} else {
			upperArmWeight = 0.136;
/* Absolute Values
			rightUpperArmComX = -0.202;
			upperArmComY = 0.840;
			upperArmComZ = -0.005;
*/
			rightUpperArmComX = -0.005;
			upperArmComY = -0.0075;
			upperArmComZ = -0.005;

			rightUpperArmInertia.set(0.00069, 0.00003, 0.0, 0.00003, 0.00014, -0.00005, 0.0, -0.00005, 0.00065);
			leftUpperArmInertia.set(0.00069, -0.00003, 0.0, -0.00003, 0.00014, -0.00005, 0.0, -0.00005, 0.00065);
		}
		leftUpperArmComX = - rightUpperArmComX;


		// LowerArm is a box body. The weight includes all parts that can be moved by the ElbowPitchJoint without the gripper and
		// everything that is rotated by the Wrist Joint
		double lowerArmWeight = 0.180;
/* Absolute Values
		double rightLowerArmComX = -0.205;
		double lowerArmComY = -0.205;
		double lowerArmComZ = -0.005;
*/
		double rightLowerArmComX = -0.008;
		double lowerArmComY = -0.8185;
		double lowerArmComZ = -0.007;

		double leftLowerArmComX = -rightLowerArmComX;

		Matrix3x3 rightLowerArmInertia(0.00047, -0.00006, 0.0, -0.00006, 0.00012, 0.0, 0.0, 0.0, 0.00050);
		Matrix3x3 leftLowerArmInertia(0.00047, 0.00006, 0.0, 0.00006, 0.00012, 0.0, 0.0, 0.0, 0.00050);

	// ----------------------------------------------------------
	// Now all parameters which can be computed from the free parameters!
	// ----------------------------------------------------------

		double heelY = mHeelRadius->get();
		double heelZ = mEndOfHeelZ->get() + mHeelRadius->get();

		double footForceSensorY = mFootForceSensorHeight->get() / 2.0;

		double soleOfFootForceSensorZ = heelZ + mSoleOfFootForceSensorDepth->get() / 2.0;
		double rightFootRightForceSensorX = -mDistanceXHipYaw->get() / 2.0 - mLegWidth->get() / 4.0;
		double rightFootLeftForceSensorX = -mDistanceXHipYaw->get() / 2.0 + mLegWidth->get() / 4.0;
		double leftFootRightForceSensorX = -rightFootRightForceSensorX;
		double leftFootLeftForceSensorX = -rightFootLeftForceSensorX;
		double ballOfFootForceSensorZ = mToesJointZ->get() - mBallOfFootForceSensorDepth->get() / 2.0;

		double soleHeight = mHeelRadius->get() * 2.0 - mFootForceSensorHeight->get();
		double soleDepth = mToesJointZ->get() - heelZ - mHeelRadius->get();	// Sole is between the heel and the toes
		double soleWidth = mLegWidth->get();

		double upperFootWidth = mLegWidth->get();
		double upperFootHeight = mAnkleJointY->get() - soleHeight - mFootForceSensorHeight->get();
		double upperFootDepth = mLegDepth->get() - mHeelRadius->get() * 2.0;

		double upperFootY = mAnkleJointY->get() - upperFootHeight / 2.0;
		double upperFootZ = heelZ + mHeelRadius->get() + upperFootDepth / 2.0;	// UpperFoot is positioned in front of the heel

		double soleY = mFootForceSensorHeight->get() + soleHeight / 2.0;
		double soleZ = heelZ + mHeelRadius->get() + soleDepth / 2.0;	// Sole is positioned bewteen the heel and the toes

		double toesWidth = mLegWidth->get();
		double toesHeight = mTipToesRadius->get() * 2.0;

		double tipToesWidth = mLegWidth->get();

		double rightToesX = -mDistanceXHipYaw->get() / 2.0;
		double leftToesX = -rightToesX;
		double toesY = mTipToesRadius->get();
		double toesZ = mToesJointZ->get() + mToesDepth->get() / 2.0;

		double rightTipToesX = rightToesX;
		double leftTipToesX = -rightTipToesX;
		double tipToesY = mTipToesRadius->get();
		double tipToesZ = toesZ + mToesDepth->get() / 2.0 + mTipToesRadius->get();

		double kneeJointY = mAnkleJointY->get() + mDistanceYAnkleToKneeJoint->get();

		double tibiaHeight = mDistanceYAnkleToKneeJoint->get();
		double tibiaWidth = mLegWidth->get();
		double tibiaDepth = mLegDepth->get();

		double kneeWidth = mLegWidth->get() - mLegWidth->get() / 10.0;

		double tibiaY = mAnkleJointY->get() + tibiaHeight / 2.0;

		double rightTibiaX = -mDistanceXHipYaw->get() / 2.0;
		double leftTibiaX = -rightTibiaX;

		double rightKneeX = -mDistanceXHipYaw->get() / 2.0;
		double leftKneeX = -rightKneeX;

		double hipPitchRollJointY = kneeJointY + mDistanceYKneeToHipPitchRollJoint->get();

		double femurHeight = mDistanceYKneeToHipPitchRollJoint->get();

		double femurY = kneeJointY + femurHeight / 2.0;

		double hipY = hipPitchRollJointY + mDistanceYHipPitchRollToHipYawJoint->get() / 2.0;

		double hipWidth = 0.5 * (mPelvisWidth->get() - mDistanceXHipYaw->get()) + mLegWidth->get() * 0.5;
		double rightHipX = (-mPelvisWidth->get() + hipWidth) / 2.0;
		double leftHipX = -rightHipX;

		double waistRollY = hipPitchRollJointY + mDistanceYHipPitchRollToHipYawJoint->get() + mDistanceYHipYawToWaistRollJoint->get();

		double pelvisY = waistRollY - mDistanceYHipYawToWaistRollJoint->get() / 2.0;

		double upperBodyY = waistRollY + mDistanceYWaistRollToHeadFlange->get() / 2.0;

		double throatY = waistRollY + mDistanceYWaistRollToHeadFlange->get() + mDistanceYHeadFlangeToHeadPitchJoint->get() / 2.0;

		double headPitchY = upperBodyY + mDistanceYWaistRollToHeadFlange->get() / 2.0 + mDistanceYHeadFlangeToHeadPitchJoint->get();

		double headPitchPartY = headPitchY + mDistanceYHeadPitchToHeadYawRollJoint->get() / 2.0;

		double headYawRollY = headPitchY + mDistanceYHeadPitchToHeadYawRollJoint->get();

		double headRollPartY = headYawRollY + mHeadRollPartHeight->get() / 2.0;
		double headRollPartZ = mHeadYawJointZ->get() + (mDistanceZHeadYawRollJointToFrontOfHead->get() - mDistanceZHeadYawRollJointToBackOfHead->get()) / 2.0;
		double headRollPartDepth = mDistanceZHeadYawRollJointToBackOfHead->get() + mDistanceZHeadYawRollJointToFrontOfHead->get();

		double rightShoulderRollX = -mDistanceXShoulderPitch->get() / 2.0 - mDistanceXShoulderPitchToShoulderRollJoint->get();
		double leftShoulderRollX = -rightShoulderRollX;

		double shoulderWidth = mDistanceXShoulderPitchToShoulderRollJoint->get() + mArmWidth->get() / 2.0;
		double rightShoulderX = -mDistanceXShoulderPitch->get() / 2.0 -shoulderWidth / 2.0;
		double leftShoulderX = -rightShoulderX;
		double shoulderY = upperBodyY + mDistanceYWaistRollToHeadFlange->get() / 2.0 + mDistanceYHeadFlangeToShoulderPitchJoint->get();

		double elbowY = shoulderY + mDistanceYShoulderRollToElbowPitchJoint->get();

		double upperArmHeight = -mDistanceYShoulderRollToElbowPitchJoint->get() - mShoulderHeight->get() / 2.0 - mElbowRadius->get();

		double upperArmY = elbowY + mElbowRadius->get() + upperArmHeight / 2.0;

		double lowerArmHeight = -mDistanceYElbowPitchToWristJoint->get() - mElbowRadius->get() / 2.0;

		double lowerArmY = elbowY - mElbowRadius->get() - lowerArmHeight / 2.0;

		// Accelboard positions relative to center of host body
		double abLegLowerBottomYRel = (mAnkleJointY->get() + 0.064) - tibiaY;
		double abLegLowerBottomZRel = -0.039;
		double abLegLowerTopYRel = (kneeJointY - 0.078) - tibiaY;
		double abLegLowerTopZRel = 0.030;
		double abLegUpperBottomYRel = (kneeJointY + 0.055) - femurY;
		double abLegUpperMiddleYRel = (kneeJointY + 0.115) - femurY;
		double abLegUpperTopYRel = (kneeJointY + 0.175) - femurY;
		double abLegUpperZRel = 0.024;
		double abArmLowerYRel = (elbowY - 0.074) - lowerArmY;
		double abArmLowerZRel = -0.027;
		double abArmUpperYRel = (shoulderY - 0.067) - upperArmY;
		double abArmUpperZRel = 0.024;
		double abBodyUpperYRel = (waistRollY + 0.168) - upperBodyY;
		double abBodyUpperZRel = -0.035;
		double abRightBodyUpperXRel = -0.035;
		double abLeftBodyUpperXRel = -abRightBodyUpperXRel;
		double abBodyLowerYRel = (waistRollY + 0.124) - upperBodyY;
		double abBodyLowerZRel = 0.026;
		double abRightBodyLowerXRel = -0.026;
		double abLeftBodyLowerXRel = -abRightBodyLowerXRel;
		double abBodyBottomYRel = (waistRollY + 0.042) - upperBodyY;
		double abBodyBottomZRel = 0.053;
		double abHeadYRel = (headYawRollY + 0.058) - headRollPartY;
		double abHeadZRel = 0.026;
		double abRightHeadXRel = -0.034;
		double abLeftHeadXRel = -abRightHeadXRel;

	// ----------------------------------------------------------
	// Now set the parameters of the Bodies and CollisionObjects
	// ----------------------------------------------------------

	Color absColor(255, 255, 255, 170);
	Color cylinderColor(0, 0, 100, 200);
	Color footSensorColor(180, 0, 0, 200);

	// Right Foot
		// UpperFoot (main body of foot) Body
		double rightUpperFootX = -mDistanceXHipYaw->get() / 2.0;
		PARAM(Vector3DValue, mRightFoot, "Position")->set(rightUpperFootX, upperFootY, upperFootZ);
//		PARAM(Vector3DValue, mRightFoot, "CenterOfMass")->set(rightFootComX - rightUpperFootX, rightFootComY - upperFootY, rightFootComZ - upperFootZ);
		PARAM(Vector3DValue, mRightFoot, "CenterOfMass")->set(rightFootComX, rightFootComY, rightFootComZ);
		PARAM(DoubleValue, mRightFoot, "Width")->set(upperFootWidth);
		PARAM(DoubleValue, mRightFoot, "Height")->set(upperFootHeight);
		PARAM(DoubleValue, mRightFoot, "Depth")->set(upperFootDepth);
		PARAM(DoubleValue, mRightFoot, "Mass")->set(footWeight);
		PARAM(ColorValue, mRightFoot, "Color")->set(absColor);
		PARAM(StringValue, mRightFoot, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mRightFoot, "InertiaTensor")->set(rightFootInertia);

		// SoleOfFoot is a box body under the UpperFoot body, over the FootForceSensors and between the heel cylinder and the toes
		mRightSoleOfFoot->setMass(0.0);
		mRightSoleOfFoot->setMaterialType("Latex");
		BoxGeom *rightSoleOfFootGeom = dynamic_cast<BoxGeom*>(mRightSoleOfFoot->getGeometry());
		rightSoleOfFootGeom->setSize(soleWidth, soleHeight, soleDepth);
		rightSoleOfFootGeom->setAutomaticColor(false);
		rightSoleOfFootGeom->setColor(absColor);
		rightSoleOfFootGeom->setLocalPosition(Vector3D(0.0, 
													   soleY - upperFootY, 
													   soleZ - upperFootZ));

		// Heel Force Sensors are cylinder bodies before the sole
		mRightHeelRightForceSensorCollObj->setMass(0.0);
		mRightHeelRightForceSensorCollObj->setMaterialType("Latex");
		CylinderGeom *rightHeelRightForceSensorGeom = dynamic_cast<CylinderGeom*>(mRightHeelRightForceSensorCollObj->getGeometry());
		rightHeelRightForceSensorGeom->setRadius(mHeelRadius->get());
		rightHeelRightForceSensorGeom->setLength(mLegWidth->get() / 2.0);
		rightHeelRightForceSensorGeom->setAutomaticColor(false);
		rightHeelRightForceSensorGeom->setColor(footSensorColor);
		rightHeelRightForceSensorGeom->setLocalPosition(Vector3D(rightFootRightForceSensorX - rightUpperFootX,
																heelY - upperFootY,
																heelZ - upperFootZ));
		rightHeelRightForceSensorGeom->setLocalOrientation(Quaternion(0.707, 0.0, 0.707, 0.0));

		mRightHeelLeftForceSensorCollObj->setMass(0.0);
		mRightHeelLeftForceSensorCollObj->setMaterialType("Latex");
		CylinderGeom *rightHeelLeftForceSensorGeom = dynamic_cast<CylinderGeom*>(mRightHeelLeftForceSensorCollObj->getGeometry());
		rightHeelLeftForceSensorGeom->setRadius(mHeelRadius->get());
		rightHeelLeftForceSensorGeom->setLength(mLegWidth->get() / 2.0);
		rightHeelLeftForceSensorGeom->setAutomaticColor(false);
		rightHeelLeftForceSensorGeom->setColor(footSensorColor);
		rightHeelLeftForceSensorGeom->setLocalPosition(Vector3D(rightFootLeftForceSensorX - rightUpperFootX,
																heelY - upperFootY,
																heelZ - upperFootZ));
		rightHeelLeftForceSensorGeom->setLocalOrientation(Quaternion(0.707, 0.0, 0.707, 0.0));

		// Sole of Foot Force Sensors are box bodies behind the heel and under the sole
		mRightSoleOfFootRightForceSensorCollObj->setMass(0.0);
		mRightSoleOfFootRightForceSensorCollObj->setMaterialType("Latex");
		BoxGeom *rightSoleOfFootRightForceSensorGeom = dynamic_cast<BoxGeom*>(mRightSoleOfFootRightForceSensorCollObj->getGeometry());
		rightSoleOfFootRightForceSensorGeom->setSize(mLegWidth->get() / 2.0, mFootForceSensorHeight->get(), mSoleOfFootForceSensorDepth->get());
		rightSoleOfFootRightForceSensorGeom->setAutomaticColor(false);
		rightSoleOfFootRightForceSensorGeom->setColor(footSensorColor);
		rightSoleOfFootRightForceSensorGeom->setLocalPosition(Vector3D(rightFootRightForceSensorX - rightUpperFootX, 
													   footForceSensorY - upperFootY, 
													   soleOfFootForceSensorZ - upperFootZ));

		mRightSoleOfFootLeftForceSensorCollObj->setMass(0.0);
		mRightSoleOfFootLeftForceSensorCollObj->setMaterialType("Latex");
		BoxGeom *rightSoleOfFootLeftForceSensorGeom = dynamic_cast<BoxGeom*>(mRightSoleOfFootLeftForceSensorCollObj->getGeometry());
		rightSoleOfFootLeftForceSensorGeom->setSize(mLegWidth->get() / 2.0, mFootForceSensorHeight->get(), mSoleOfFootForceSensorDepth->get());
		rightSoleOfFootLeftForceSensorGeom->setAutomaticColor(false);
		rightSoleOfFootLeftForceSensorGeom->setColor(footSensorColor);
		rightSoleOfFootLeftForceSensorGeom->setLocalPosition(Vector3D(rightFootLeftForceSensorX - rightUpperFootX, 
													   footForceSensorY - upperFootY, 
													   soleOfFootForceSensorZ - upperFootZ));

		// Ball of Foot Force Sensors are box bodies before the toes and under the sole
		mRightBallOfFootRightForceSensorCollObj->setMass(0.0);
		mRightBallOfFootRightForceSensorCollObj->setMaterialType("Latex");
		BoxGeom *rightBallOfFootRightForceSensorGeom = dynamic_cast<BoxGeom*>(mRightBallOfFootRightForceSensorCollObj->getGeometry());
		rightBallOfFootRightForceSensorGeom->setSize(mLegWidth->get() / 2.0, mFootForceSensorHeight->get(), mBallOfFootForceSensorDepth->get());
		rightBallOfFootRightForceSensorGeom->setAutomaticColor(false);
		rightBallOfFootRightForceSensorGeom->setColor(footSensorColor);
		rightBallOfFootRightForceSensorGeom->setLocalPosition(Vector3D(rightFootRightForceSensorX - rightUpperFootX,
														footForceSensorY - upperFootY,
														ballOfFootForceSensorZ - upperFootZ));

		mRightBallOfFootLeftForceSensorCollObj->setMass(0.0);
		mRightBallOfFootLeftForceSensorCollObj->setMaterialType("Latex");
		BoxGeom *rightBallOfFootLeftForceSensorGeom = dynamic_cast<BoxGeom*>(mRightBallOfFootLeftForceSensorCollObj->getGeometry());
		rightBallOfFootLeftForceSensorGeom->setSize(mLegWidth->get() / 2.0, mFootForceSensorHeight->get(), mBallOfFootForceSensorDepth->get());
		rightBallOfFootLeftForceSensorGeom->setAutomaticColor(false);
		rightBallOfFootLeftForceSensorGeom->setColor(footSensorColor);
		rightBallOfFootLeftForceSensorGeom->setLocalPosition(Vector3D(rightFootLeftForceSensorX - rightUpperFootX,
														footForceSensorY - upperFootY,
														ballOfFootForceSensorZ - upperFootZ));

	// Left Foot
		// UpperFoot (main body of foot) Body
		double leftUpperFootX = -rightUpperFootX;
		PARAM(Vector3DValue, mLeftFoot, "Position")->set(leftUpperFootX, upperFootY, upperFootZ);
//		PARAM(Vector3DValue, mLeftFoot, "CenterOfMass")->set(leftFootComX - leftUpperFootX, leftFootComY - upperFootY, leftFootComZ - upperFootZ);
		PARAM(Vector3DValue, mLeftFoot, "CenterOfMass")->set(leftFootComX, leftFootComY, leftFootComZ);
		PARAM(DoubleValue, mLeftFoot, "Width")->set(upperFootWidth);
		PARAM(DoubleValue, mLeftFoot, "Height")->set(upperFootHeight);
		PARAM(DoubleValue, mLeftFoot, "Depth")->set(upperFootDepth);
		PARAM(DoubleValue, mLeftFoot, "Mass")->set(footWeight);
		PARAM(ColorValue, mLeftFoot, "Color")->set(absColor);
		PARAM(StringValue, mLeftFoot, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mLeftFoot, "InertiaTensor")->set(leftFootInertia);

		// SoleOfFoot is a box body under the UpperFoot body, over the FootForceSensors and between the heel cylinder and the toes
		mLeftSoleOfFoot->setMass(0.0);
		mLeftSoleOfFoot->setMaterialType("Latex");
		BoxGeom *leftSoleOfFootGeom = dynamic_cast<BoxGeom*>(mLeftSoleOfFoot->getGeometry());
		leftSoleOfFootGeom->setSize(soleWidth, soleHeight, soleDepth);
		leftSoleOfFootGeom->setAutomaticColor(false);
		leftSoleOfFootGeom->setColor(absColor);
		leftSoleOfFootGeom->setLocalPosition(Vector3D(0.0, 
													   soleY - upperFootY, 
													   soleZ - upperFootZ));

		// Heel Force Sensors are cylinder bodies before the sole
		mLeftHeelRightForceSensorCollObj->setMass(0.0);
		mLeftHeelRightForceSensorCollObj->setMaterialType("Latex");
		CylinderGeom *leftHeelRightForceSensorGeom = dynamic_cast<CylinderGeom*>(mLeftHeelRightForceSensorCollObj->getGeometry());
		leftHeelRightForceSensorGeom->setRadius(mHeelRadius->get());
		leftHeelRightForceSensorGeom->setLength(mLegWidth->get() / 2.0);
		leftHeelRightForceSensorGeom->setAutomaticColor(false);
		leftHeelRightForceSensorGeom->setColor(footSensorColor);
		leftHeelRightForceSensorGeom->setLocalPosition(Vector3D(leftFootRightForceSensorX - leftUpperFootX,
																heelY - upperFootY,
																heelZ - upperFootZ));
		leftHeelRightForceSensorGeom->setLocalOrientation(Quaternion(0.707, 0.0, 0.707, 0.0));

		mLeftHeelLeftForceSensorCollObj->setMass(0.0);
		mLeftHeelLeftForceSensorCollObj->setMaterialType("Latex");
		CylinderGeom *leftHeelLeftForceSensorGeom = dynamic_cast<CylinderGeom*>(mLeftHeelLeftForceSensorCollObj->getGeometry());
		leftHeelLeftForceSensorGeom->setRadius(mHeelRadius->get());
		leftHeelLeftForceSensorGeom->setLength(mLegWidth->get() / 2.0);
		leftHeelLeftForceSensorGeom->setAutomaticColor(false);
		leftHeelLeftForceSensorGeom->setColor(footSensorColor);
		leftHeelLeftForceSensorGeom->setLocalPosition(Vector3D(leftFootLeftForceSensorX - leftUpperFootX,
																heelY - upperFootY,
																heelZ - upperFootZ));
		leftHeelLeftForceSensorGeom->setLocalOrientation(Quaternion(0.707, 0.0, 0.707, 0.0));

		// Sole of Foot Force Sensors are box bodies behind the heel and under the sole
		mLeftSoleOfFootRightForceSensorCollObj->setMass(0.0);
		mLeftSoleOfFootRightForceSensorCollObj->setMaterialType("Latex");
		BoxGeom *leftSoleOfFootRightForceSensorGeom = dynamic_cast<BoxGeom*>(mLeftSoleOfFootRightForceSensorCollObj->getGeometry());
		leftSoleOfFootRightForceSensorGeom->setSize(mLegWidth->get() / 2.0, mFootForceSensorHeight->get(), mSoleOfFootForceSensorDepth->get());
		leftSoleOfFootRightForceSensorGeom->setAutomaticColor(false);
		leftSoleOfFootRightForceSensorGeom->setColor(footSensorColor);
		leftSoleOfFootRightForceSensorGeom->setLocalPosition(Vector3D(leftFootRightForceSensorX - leftUpperFootX, 
													   footForceSensorY - upperFootY, 
													   soleOfFootForceSensorZ - upperFootZ));

		mLeftSoleOfFootLeftForceSensorCollObj->setMass(0.0);
		mLeftSoleOfFootLeftForceSensorCollObj->setMaterialType("Latex");
		BoxGeom *leftSoleOfFootLeftForceSensorGeom = dynamic_cast<BoxGeom*>(mLeftSoleOfFootLeftForceSensorCollObj->getGeometry());
		leftSoleOfFootLeftForceSensorGeom->setSize(mLegWidth->get() / 2.0, mFootForceSensorHeight->get(), mSoleOfFootForceSensorDepth->get());
		leftSoleOfFootLeftForceSensorGeom->setAutomaticColor(false);
		leftSoleOfFootLeftForceSensorGeom->setColor(footSensorColor);
		leftSoleOfFootLeftForceSensorGeom->setLocalPosition(Vector3D(leftFootLeftForceSensorX - leftUpperFootX, 
													   footForceSensorY - upperFootY, 
													   soleOfFootForceSensorZ - upperFootZ));

		// Ball of Foot Force Sensors are box bodies before the toes and under the sole
		mLeftBallOfFootRightForceSensorCollObj->setMass(0.0);
		mLeftBallOfFootRightForceSensorCollObj->setMaterialType("Latex");
		BoxGeom *leftBallOfFootRightForceSensorGeom = dynamic_cast<BoxGeom*>(mLeftBallOfFootRightForceSensorCollObj->getGeometry());
		leftBallOfFootRightForceSensorGeom->setSize(mLegWidth->get() / 2.0, mFootForceSensorHeight->get(), mBallOfFootForceSensorDepth->get());
		leftBallOfFootRightForceSensorGeom->setAutomaticColor(false);
		leftBallOfFootRightForceSensorGeom->setColor(footSensorColor);
		leftBallOfFootRightForceSensorGeom->setLocalPosition(Vector3D(leftFootRightForceSensorX - leftUpperFootX,
														footForceSensorY - upperFootY,
														ballOfFootForceSensorZ - upperFootZ));

		mLeftBallOfFootLeftForceSensorCollObj->setMass(0.0);
		mLeftBallOfFootLeftForceSensorCollObj->setMaterialType("Latex");
		BoxGeom *leftBallOfFootLeftForceSensorGeom = dynamic_cast<BoxGeom*>(mLeftBallOfFootLeftForceSensorCollObj->getGeometry());
		leftBallOfFootLeftForceSensorGeom->setSize(mLegWidth->get() / 2.0, mFootForceSensorHeight->get(), mBallOfFootForceSensorDepth->get());
		leftBallOfFootLeftForceSensorGeom->setAutomaticColor(false);
		leftBallOfFootLeftForceSensorGeom->setColor(footSensorColor);
		leftBallOfFootLeftForceSensorGeom->setLocalPosition(Vector3D(leftFootLeftForceSensorX - leftUpperFootX,
														footForceSensorY - upperFootY,
														ballOfFootForceSensorZ - upperFootZ));

	// Right Toes ( Toes consists of toe main body and a tiptoe cylinder)
		// Toe Body
		PARAM(Vector3DValue, mRightToes, "Position")->set(rightToesX, toesY, toesZ);
//		PARAM(Vector3DValue, mRightToes, "CenterOfMass")->set(rightToesComX - rightToesX, rightToesComY - toesY, rightToesComZ - toesZ);
		PARAM(Vector3DValue, mRightToes, "CenterOfMass")->set(rightToesComX, rightToesComY, rightToesComZ);
		PARAM(DoubleValue, mRightToes, "Width")->set(toesWidth);
		PARAM(DoubleValue, mRightToes, "Height")->set(toesHeight);
		PARAM(DoubleValue, mRightToes, "Depth")->set(mToesDepth->get());
		PARAM(DoubleValue, mRightToes, "Mass")->set(toesWeight);
		PARAM(ColorValue, mRightToes, "Color")->set(absColor);
		PARAM(StringValue, mRightToes, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mRightToes, "InertiaTensor")->set(rightToesInertia);

		// TipToes is a cylinder body behind the toes
		mRightTipToes->setMass(tipToesWeight);
		mRightTipToes->setMaterialType("Latex");
		CylinderGeom *rightTipToesGeom = dynamic_cast<CylinderGeom*>(mRightTipToes->getGeometry());
		rightTipToesGeom->setRadius(mTipToesRadius->get());
		rightTipToesGeom->setLength(tipToesWidth);
		rightTipToesGeom->setAutomaticColor(false);
		rightTipToesGeom->setColor(cylinderColor);
		rightTipToesGeom->setLocalPosition(Vector3D(rightTipToesX - rightToesX,
													   tipToesY - toesY,
													   tipToesZ - toesZ));
		rightTipToesGeom->setLocalOrientation(Quaternion(0.707, 0.0, 0.707, 0.0));

	// Left Toes ( Toes consists of toe main body and a tiptoe cylinder)
		// Toe Body
		PARAM(Vector3DValue, mLeftToes, "Position")->set(leftToesX, toesY, toesZ);
//		PARAM(Vector3DValue, mLeftToes, "CenterOfMass")->set(leftToesComX - leftToesX, leftToesComY - toesY, leftToesComZ - toesZ);
		PARAM(Vector3DValue, mLeftToes, "CenterOfMass")->set(leftToesComX, leftToesComY, leftToesComZ);
		PARAM(DoubleValue, mLeftToes, "Width")->set(toesWidth);
		PARAM(DoubleValue, mLeftToes, "Height")->set(toesHeight);
		PARAM(DoubleValue, mLeftToes, "Depth")->set(mToesDepth->get());
		PARAM(DoubleValue, mLeftToes, "Mass")->set(toesWeight);
		PARAM(ColorValue, mLeftToes, "Color")->set(absColor);
		PARAM(StringValue, mLeftToes, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mLeftToes, "InertiaTensor")->set(leftToesInertia);

		// TipToes is a cylinder body behind the toes
		mLeftTipToes->setMass(tipToesWeight);
		mLeftTipToes->setMaterialType("Latex");
		CylinderGeom *leftTipToesGeom = dynamic_cast<CylinderGeom*>(mLeftTipToes->getGeometry());
		leftTipToesGeom->setRadius(mTipToesRadius->get());
		leftTipToesGeom->setLength(tipToesWidth);
		leftTipToesGeom->setAutomaticColor(false);
		leftTipToesGeom->setColor(cylinderColor);
		leftTipToesGeom->setLocalPosition(Vector3D(leftTipToesX - leftToesX,
													   tipToesY - toesY,
													   tipToesZ - toesZ));
		leftTipToesGeom->setLocalOrientation(Quaternion(0.707, 0.0, 0.707, 0.0));

	// Right Ankle is a small BoxInertiaBody not visible from outside connecting the Tibia and the Foot
		double rightAnkleX = -mDistanceXHipYaw->get() / 2.0;
		PARAM(Vector3DValue, mRightAnkle, "Position")->set(rightAnkleX, mAnkleJointY->get(), 0.0);
//		PARAM(Vector3DValue, mRightAnkle, "CenterOfMass")->set(rightAnkleComX - rightAnkleX, rightAnkleComY - mAnkleJointY->get(), rightAnkleComZ);
		PARAM(Vector3DValue, mRightAnkle, "CenterOfMass")->set(rightAnkleComX, rightAnkleComY, rightAnkleComZ);
		PARAM(DoubleValue, mRightAnkle, "Width")->set(0.01);
		PARAM(DoubleValue, mRightAnkle, "Height")->set(0.01);
		PARAM(DoubleValue, mRightAnkle, "Depth")->set(0.01);
		PARAM(DoubleValue, mRightAnkle, "Mass")->set(ankleWeight);
		PARAM(ColorValue, mRightAnkle, "Color")->set(absColor);
		PARAM(StringValue, mRightAnkle, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mRightAnkle, "InertiaTensor")->set(rightAnkleInertia);

	// Left Ankle is a small BoxInertiaBody not visible from outside connecting the Tibia and the Foot
		double leftAnkleX = -rightAnkleX;
		PARAM(Vector3DValue, mLeftAnkle, "Position")->set(leftAnkleX, mAnkleJointY->get(), 0.0);
//		PARAM(Vector3DValue, mLeftAnkle, "CenterOfMass")->set(leftAnkleComX - leftAnkleX, leftAnkleComY - mAnkleJointY->get(), leftAnkleComZ);
		PARAM(Vector3DValue, mLeftAnkle, "CenterOfMass")->set(leftAnkleComX, leftAnkleComY, leftAnkleComZ);
		PARAM(DoubleValue, mLeftAnkle, "Width")->set(0.01);
		PARAM(DoubleValue, mLeftAnkle, "Height")->set(0.01);
		PARAM(DoubleValue, mLeftAnkle, "Depth")->set(0.01);
		PARAM(DoubleValue, mLeftAnkle, "Mass")->set(ankleWeight);
		PARAM(ColorValue, mLeftAnkle, "Color")->set(absColor);
		PARAM(StringValue, mLeftAnkle, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mLeftAnkle, "InertiaTensor")->set(leftAnkleInertia);

	// Right Tibia (Tibia consist of a long box (the tibia) and a cylinder on top of the box (the knee joint)
		// Tibia Body
		PARAM(Vector3DValue, mRightTibia, "Position")->set(rightTibiaX, tibiaY, 0.0);
//		PARAM(Vector3DValue, mRightTibia, "CenterOfMass")->set(rightTibiaComX - rightTibiaX, rightTibiaComY - tibiaY, rightTibiaComZ);
		PARAM(Vector3DValue, mRightTibia, "CenterOfMass")->set(rightTibiaComX, rightTibiaComY, rightTibiaComZ);
		PARAM(DoubleValue, mRightTibia, "Width")->set(tibiaWidth);
		PARAM(DoubleValue, mRightTibia, "Height")->set(tibiaHeight);
		PARAM(DoubleValue, mRightTibia, "Depth")->set(tibiaDepth);
		PARAM(DoubleValue, mRightTibia, "Mass")->set(tibiaWeight);
		PARAM(ColorValue, mRightTibia, "Color")->set(absColor);
		PARAM(StringValue, mRightTibia, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mRightTibia, "InertiaTensor")->set(rightTibiaInertia);

		// Knee is a cylinder body on top of the tibia
		mRightKnee->setMass(kneeWeight);
		mRightKnee->setMaterialType("Latex");
		CylinderGeom *rightKneeGeom = dynamic_cast<CylinderGeom*>(mRightKnee->getGeometry());
		rightKneeGeom->setRadius(mKneeRadius->get());
		rightKneeGeom->setLength(kneeWidth);
		rightKneeGeom->setAutomaticColor(false);
		rightKneeGeom->setColor(cylinderColor);
		rightKneeGeom->setLocalPosition(Vector3D(rightKneeX - rightTibiaX,
													   kneeJointY - tibiaY,
													   mKneeJointZ->get()));
		rightKneeGeom->setLocalOrientation(Quaternion(0.707, 0.0, 0.707, 0.0));

	// Left Tibia (Tibia consist of a long box (the tibia) and a cylinder on top of the box (the knee joint)
		// Tibia Body
		PARAM(Vector3DValue, mLeftTibia, "Position")->set(leftTibiaX, tibiaY, 0.0);
//		PARAM(Vector3DValue, mLeftTibia, "CenterOfMass")->set(leftTibiaComX - leftTibiaX, leftTibiaComY - tibiaY, leftTibiaComZ);
		PARAM(Vector3DValue, mLeftTibia, "CenterOfMass")->set(leftTibiaComX, leftTibiaComY, leftTibiaComZ);
		PARAM(DoubleValue, mLeftTibia, "Width")->set(tibiaWidth);
		PARAM(DoubleValue, mLeftTibia, "Height")->set(tibiaHeight);
		PARAM(DoubleValue, mLeftTibia, "Depth")->set(tibiaDepth);
		PARAM(DoubleValue, mLeftTibia, "Mass")->set(tibiaWeight);
		PARAM(ColorValue, mLeftTibia, "Color")->set(absColor);
		PARAM(StringValue, mLeftTibia, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mLeftTibia, "InertiaTensor")->set(leftTibiaInertia);

		// Knee is a cylinder body on top of the tibia
		mLeftKnee->setMass(kneeWeight);
		mLeftKnee->setMaterialType("Latex");
		CylinderGeom *leftKneeGeom = dynamic_cast<CylinderGeom*>(mLeftKnee->getGeometry());
		leftKneeGeom->setRadius(mKneeRadius->get());
		leftKneeGeom->setLength(kneeWidth);
		leftKneeGeom->setAutomaticColor(false);
		leftKneeGeom->setColor(cylinderColor);
		leftKneeGeom->setLocalPosition(Vector3D(leftKneeX - leftTibiaX,
													   kneeJointY - tibiaY,
													   mKneeJointZ->get()));
		leftKneeGeom->setLocalOrientation(Quaternion(0.707, 0.0, 0.707, 0.0));

	// Right Femur
		// Femur Body
		double rightFemurX = -mDistanceXHipYaw->get() / 2.0;
		PARAM(Vector3DValue, mRightFemur, "Position")->set(rightFemurX, femurY, 0.0);
//		PARAM(Vector3DValue, mRightFemur, "CenterOfMass")->set(rightFemurComX - rightFemurX, rightFemurComY - femurY, rightFemurComZ);
		PARAM(Vector3DValue, mRightFemur, "CenterOfMass")->set(rightFemurComX, rightFemurComY, rightFemurComZ);
		PARAM(DoubleValue, mRightFemur, "Width")->set(mLegWidth->get());
		PARAM(DoubleValue, mRightFemur, "Height")->set(femurHeight);
		PARAM(DoubleValue, mRightFemur, "Depth")->set(mLegDepth->get());
		PARAM(DoubleValue, mRightFemur, "Mass")->set(femurWeight);
		PARAM(ColorValue, mRightFemur, "Color")->set(absColor);
		PARAM(StringValue, mRightFemur, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mRightFemur, "InertiaTensor")->set(rightFemurInertia);

	// Left Femur
		// Femur Body
		double leftFemurX = -rightFemurX;
		PARAM(Vector3DValue, mLeftFemur, "Position")->set(leftFemurX, femurY, 0.0);
//		PARAM(Vector3DValue, mLeftFemur, "CenterOfMass")->set(leftFemurComX - leftFemurX, leftFemurComY - femurY, leftFemurComZ);
		PARAM(Vector3DValue, mLeftFemur, "CenterOfMass")->set(leftFemurComX, leftFemurComY, leftFemurComZ);
		PARAM(DoubleValue, mLeftFemur, "Width")->set(mLegWidth->get());
		PARAM(DoubleValue, mLeftFemur, "Height")->set(femurHeight);
		PARAM(DoubleValue, mLeftFemur, "Depth")->set(mLegDepth->get());
		PARAM(DoubleValue, mLeftFemur, "Mass")->set(femurWeight);
		PARAM(ColorValue, mLeftFemur, "Color")->set(absColor);
		PARAM(StringValue, mLeftFemur, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mLeftFemur, "InertiaTensor")->set(leftFemurInertia);

	// Right HipCross. HipCross is a small body not visible from outside the robot connecting the
	// Hip and the Femur. It can be rotated by the HipRollJoint
		double rightHipCrossX = -mDistanceXHipYaw->get() / 2.0;
		PARAM(Vector3DValue, mRightHipCross, "Position")->set(rightHipCrossX, hipPitchRollJointY, 0.0);
//		PARAM(Vector3DValue, mRightHipCross, "CenterOfMass")->set(rightHipCrossComX - rightHipCrossX, rightHipCrossComY - hipPitchRollJointY, rightHipCrossComZ);
		PARAM(Vector3DValue, mRightHipCross, "CenterOfMass")->set(rightHipCrossComX, rightHipCrossComY, rightHipCrossComZ);
		PARAM(DoubleValue, mRightHipCross, "Width")->set(0.01);
		PARAM(DoubleValue, mRightHipCross, "Height")->set(0.01);
		PARAM(DoubleValue, mRightHipCross, "Depth")->set(0.01);
		PARAM(DoubleValue, mRightHipCross, "Mass")->set(hipCrossWeight);
		PARAM(ColorValue, mRightHipCross, "Color")->set(absColor);
		PARAM(StringValue, mRightHipCross, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mRightHipCross, "InertiaTensor")->set(rightHipCrossInertia);

	// Left HipCross. HipCross is a small body not visible from outside the robot connecting the
	// Hip and the Femur. It can be rotated by the HipRollJoint
		double leftHipCrossX = -rightHipCrossX;
		PARAM(Vector3DValue, mLeftHipCross, "Position")->set(leftHipCrossX, hipPitchRollJointY, 0.0);
//		PARAM(Vector3DValue, mLeftHipCross, "CenterOfMass")->set(leftHipCrossComX - leftHipCrossX, leftHipCrossComY - hipPitchRollJointY, leftHipCrossComZ);
		PARAM(Vector3DValue, mLeftHipCross, "CenterOfMass")->set(leftHipCrossComX, leftHipCrossComY, leftHipCrossComZ);
		PARAM(DoubleValue, mLeftHipCross, "Width")->set(0.01);
		PARAM(DoubleValue, mLeftHipCross, "Height")->set(0.01);
		PARAM(DoubleValue, mLeftHipCross, "Depth")->set(0.01);
		PARAM(DoubleValue, mLeftHipCross, "Mass")->set(hipCrossWeight);
		PARAM(ColorValue, mLeftHipCross, "Color")->set(absColor);
		PARAM(StringValue, mLeftHipCross, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mLeftHipCross, "InertiaTensor")->set(leftHipCrossInertia);

	// Right Hip. Hip is a body on top of the HipCross which can be rotated with the HipYawJoint.
	// The Weight and Inertia properties include the swivel in the torso which is also rotated in the
	// real robot.
		PARAM(Vector3DValue, mRightHip, "Position")->set(rightHipX, hipY, 0.0);
//		PARAM(Vector3DValue, mRightHip, "CenterOfMass")->set(rightHipComX - rightHipX, rightHipComY - hipY, rightHipComZ);
		PARAM(Vector3DValue, mRightHip, "CenterOfMass")->set(rightHipComX, rightHipComY, rightHipComZ);
		PARAM(DoubleValue, mRightHip, "Width")->set(hipWidth);
		PARAM(DoubleValue, mRightHip, "Height")->set(mDistanceYHipPitchRollToHipYawJoint->get());
		PARAM(DoubleValue, mRightHip, "Depth")->set(mLegDepth->get());
		PARAM(DoubleValue, mRightHip, "Mass")->set(hipWeight);
		PARAM(ColorValue, mRightHip, "Color")->set(absColor);
		PARAM(StringValue, mRightHip, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mRightHip, "InertiaTensor")->set(rightHipInertia);

	// Left Hip. Hip is a body on top of the HipCross which can be rotated with the HipYawJoint.
	// The Weight and Inertia properties include the swivel in the torso which is also rotated in the
	// real robot.
		PARAM(Vector3DValue, mLeftHip, "Position")->set(leftHipX, hipY, 0.0);
//		PARAM(Vector3DValue, mLeftHip, "CenterOfMass")->set(leftHipComX - leftHipX, leftHipComY - hipY, leftHipComZ);
		PARAM(Vector3DValue, mLeftHip, "CenterOfMass")->set(leftHipComX, leftHipComY, leftHipComZ);
		PARAM(DoubleValue, mLeftHip, "Width")->set(hipWidth);
		PARAM(DoubleValue, mLeftHip, "Height")->set(mDistanceYHipPitchRollToHipYawJoint->get());
		PARAM(DoubleValue, mLeftHip, "Depth")->set(mLegDepth->get());
		PARAM(DoubleValue, mLeftHip, "Mass")->set(hipWeight);
		PARAM(ColorValue, mLeftHip, "Color")->set(absColor);
		PARAM(StringValue, mLeftHip, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mLeftHip, "InertiaTensor")->set(leftHipInertia);

	// Pelvis is a body on top of the hips. It is connected with the HipYawJoints to the hips and with the WaistRollJoint to the chest.
		PARAM(Vector3DValue, mPelvis, "Position")->set(0.0, pelvisY, 0.0);
//		PARAM(Vector3DValue, mPelvis, "CenterOfMass")->set(pelvisComX, pelvisComY - pelvisY, pelvisComZ);
		PARAM(Vector3DValue, mPelvis, "CenterOfMass")->set(pelvisComX, pelvisComY, pelvisComZ);
		PARAM(DoubleValue, mPelvis, "Width")->set(mPelvisWidth->get());
		PARAM(DoubleValue, mPelvis, "Height")->set(mDistanceYHipYawToWaistRollJoint->get());
		PARAM(DoubleValue, mPelvis, "Depth")->set(mPelvisDepth->get());
		PARAM(DoubleValue, mPelvis, "Mass")->set(pelvisWeight);
		PARAM(ColorValue, mPelvis, "Color")->set(absColor);
		PARAM(StringValue, mPelvis, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mPelvis, "InertiaTensor")->set(pelvisInertia);

	// UpperBody is a body on top of the pelvis, below the head and between the shoulders. It is the torso without the pelvis and the shoulders, but with the throat.
	// UpperBody can be moved with the WaistRollJoint.
		// Main body
		PARAM(Vector3DValue, mUpperBody, "Position")->set(0.0, upperBodyY, 0.0);
//		PARAM(Vector3DValue, mUpperBody, "CenterOfMass")->set(upperBodyComX, upperBodyComY - upperBodyY, upperBodyComZ);
		PARAM(Vector3DValue, mUpperBody, "CenterOfMass")->set(upperBodyComX, upperBodyComY, upperBodyComZ);
		PARAM(DoubleValue, mUpperBody, "Width")->set(mDistanceXShoulderPitch->get());
		PARAM(DoubleValue, mUpperBody, "Height")->set(mDistanceYWaistRollToHeadFlange->get());
		PARAM(DoubleValue, mUpperBody, "Depth")->set(mUpperBodyDepth->get());
		PARAM(DoubleValue, mUpperBody, "Mass")->set(upperBodyWeight);
		PARAM(ColorValue, mUpperBody, "Color")->set(absColor);
		PARAM(StringValue, mUpperBody, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mUpperBody, "InertiaTensor")->set(upperBodyInertia);

		// Throat is a box body under the head
		mThroat->setMass(0.0);
		mThroat->setMaterialType("Latex");
		BoxGeom *throatGeom = dynamic_cast<BoxGeom*>(mThroat->getGeometry());
		throatGeom->setSize(mThroatWidth->get(), mDistanceYHeadFlangeToShoulderPitchJoint->get(), mThroatDepth->get());
		throatGeom->setAutomaticColor(false);
		throatGeom->setColor(absColor);
		throatGeom->setLocalPosition(Vector3D(0.0, 
											   throatY - upperBodyY, 
											   0.0));


	// HeadPitchPart is the body which is moved by the HeadPitchJoint and to which the HeadYawJoint is connected.
		PARAM(Vector3DValue, mHeadPitchPart, "Position")->set(0.0, headPitchPartY, 0.0);
//		PARAM(Vector3DValue, mHeadPitchPart, "CenterOfMass")->set(headPitchPartComX, headPitchPartComY - headPitchPartY, headPitchPartComZ);
		PARAM(Vector3DValue, mHeadPitchPart, "CenterOfMass")->set(headPitchPartComX, headPitchPartComY, headPitchPartComZ);
		PARAM(DoubleValue, mHeadPitchPart, "Width")->set(mThroatWidth->get());
		PARAM(DoubleValue, mHeadPitchPart, "Height")->set(mDistanceYHeadPitchToHeadYawRollJoint->get());
		PARAM(DoubleValue, mHeadPitchPart, "Depth")->set(mThroatDepth->get());
		PARAM(DoubleValue, mHeadPitchPart, "Mass")->set(headPitchPartWeight);
		PARAM(ColorValue, mHeadPitchPart, "Color")->set(absColor);
		PARAM(StringValue, mHeadPitchPart, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mHeadPitchPart, "InertiaTensor")->set(headPitchPartInertia);

	// HeadYawPart is a small body not visible from outside the robot connecting the HeadPitch and HeadRollPart. It can be rotated
	// by the HeadYawJoint.
		PARAM(Vector3DValue, mHeadYawPart, "Position")->set(0.0, headYawRollY, mHeadYawJointZ->get());
//		PARAM(Vector3DValue, mHeadYawPart, "CenterOfMass")->set(headYawPartComX, headYawPartComY - headYawRollY, headYawPartComZ - mHeadYawJointZ->get());
		PARAM(Vector3DValue, mHeadYawPart, "CenterOfMass")->set(headYawPartComX, headYawPartComY, headYawPartComZ);
		PARAM(DoubleValue, mHeadYawPart, "Width")->set(0.01);
		PARAM(DoubleValue, mHeadYawPart, "Height")->set(0.01);
		PARAM(DoubleValue, mHeadYawPart, "Depth")->set(0.01);
		PARAM(DoubleValue, mHeadYawPart, "Mass")->set(headYawPartWeight);
		PARAM(ColorValue, mHeadYawPart, "Color")->set(absColor);
		PARAM(StringValue, mHeadYawPart, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mHeadYawPart, "InertiaTensor")->set(headYawPartInertia);

	// HeadRollPart is the body which is moved by the HeadRollJoint and connected to the HeadYawPart
		PARAM(Vector3DValue, mHeadRollPart, "Position")->set(0.0, headRollPartY, headRollPartZ);
//		PARAM(Vector3DValue, mHeadRollPart, "CenterOfMass")->set(headRollPartComX, headRollPartComY - headRollPartY, headRollPartComZ - headRollPartZ);
		PARAM(Vector3DValue, mHeadRollPart, "CenterOfMass")->set(headRollPartComX, headRollPartComY, headRollPartComZ);
		PARAM(DoubleValue, mHeadRollPart, "Width")->set(mHeadRollPartWidth->get());
		PARAM(DoubleValue, mHeadRollPart, "Height")->set(mHeadRollPartHeight->get());
		PARAM(DoubleValue, mHeadRollPart, "Depth")->set(headRollPartDepth);
		PARAM(DoubleValue, mHeadRollPart, "Mass")->set(headRollPartWeight);
		PARAM(ColorValue, mHeadRollPart, "Color")->set(absColor);
		PARAM(StringValue, mHeadRollPart, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mHeadRollPart, "InertiaTensor")->set(headRollPartInertia);

	// Right Shoulder
		// Shoulder Body
		PARAM(Vector3DValue, mRightShoulder, "Position")->set(rightShoulderX, shoulderY, 0.0);
//		PARAM(Vector3DValue, mRightShoulder, "CenterOfMass")->set(rightShoulderComX - rightShoulderX, shoulderComY - shoulderY, shoulderComZ);
		PARAM(Vector3DValue, mRightShoulder, "CenterOfMass")->set(rightShoulderComX, shoulderComY, shoulderComZ);
		PARAM(DoubleValue, mRightShoulder, "Width")->set(shoulderWidth);
		PARAM(DoubleValue, mRightShoulder, "Height")->set(mShoulderHeight->get());
		PARAM(DoubleValue, mRightShoulder, "Depth")->set(mArmDepth->get());
		PARAM(DoubleValue, mRightShoulder, "Mass")->set(shoulderWeight);
		PARAM(ColorValue, mRightShoulder, "Color")->set(absColor);
		PARAM(StringValue, mRightShoulder, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mRightShoulder, "InertiaTensor")->set(rightShoulderInertia);

	// Left Shoulder
		// Shoulder Body
		PARAM(Vector3DValue, mLeftShoulder, "Position")->set(leftShoulderX, shoulderY, 0.0);
//		PARAM(Vector3DValue, mLeftShoulder, "CenterOfMass")->set(leftShoulderComX - leftShoulderX, shoulderComY - shoulderY, shoulderComZ);
		PARAM(Vector3DValue, mLeftShoulder, "CenterOfMass")->set(leftShoulderComX, shoulderComY, shoulderComZ);
		PARAM(DoubleValue, mLeftShoulder, "Width")->set(shoulderWidth);
		PARAM(DoubleValue, mLeftShoulder, "Height")->set(mShoulderHeight->get());
		PARAM(DoubleValue, mLeftShoulder, "Depth")->set(mArmDepth->get());
		PARAM(DoubleValue, mLeftShoulder, "Mass")->set(shoulderWeight);
		PARAM(ColorValue, mLeftShoulder, "Color")->set(absColor);
		PARAM(StringValue, mLeftShoulder, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mLeftShoulder, "InertiaTensor")->set(leftShoulderInertia);

	// Right UpperArm (UpperArm consist of a long box (the upperArm) and a cylinder at the bottom of the box (the elbow joint)
		// UpperArm Body
		PARAM(Vector3DValue, mRightUpperArm, "Position")->set(rightShoulderRollX, upperArmY, 0.0);
//		PARAM(Vector3DValue, mRightUpperArm, "CenterOfMass")->set(rightUpperArmComX - rightShoulderRollX, upperArmComY - upperArmY, upperArmComZ);
		PARAM(Vector3DValue, mRightUpperArm, "CenterOfMass")->set(rightUpperArmComX, upperArmComY, upperArmComZ);
		PARAM(DoubleValue, mRightUpperArm, "Width")->set(mArmWidth->get());
		PARAM(DoubleValue, mRightUpperArm, "Height")->set(upperArmHeight);
		PARAM(DoubleValue, mRightUpperArm, "Depth")->set(mArmDepth->get());
		PARAM(DoubleValue, mRightUpperArm, "Mass")->set(upperArmWeight);
		PARAM(ColorValue, mRightUpperArm, "Color")->set(absColor);
		PARAM(StringValue, mRightUpperArm, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mRightUpperArm, "InertiaTensor")->set(rightUpperArmInertia);

		// Elbow is a cylinder body on top of the upperArm
		mRightElbow->setMass(0.0);
		mRightElbow->setMaterialType("Latex");
		CylinderGeom *rightElbowGeom = dynamic_cast<CylinderGeom*>(mRightElbow->getGeometry());
		rightElbowGeom->setRadius(mElbowRadius->get());
		rightElbowGeom->setLength(mArmWidth->get());
		rightElbowGeom->setAutomaticColor(false);
		rightElbowGeom->setColor(cylinderColor);
		rightElbowGeom->setLocalPosition(Vector3D(0.0,
												elbowY - upperArmY,
												mElbowJointZ->get()));
		rightElbowGeom->setLocalOrientation(Quaternion(0.707, 0.0, 0.707, 0.0));

	// Left UpperArm (UpperArm consist of a long box (the upperArm) and a cylinder at the bottom of the box (the elbow joint)
		// UpperArm Body
		PARAM(Vector3DValue, mLeftUpperArm, "Position")->set(leftShoulderRollX, upperArmY, 0.0);
//		PARAM(Vector3DValue, mLeftUpperArm, "CenterOfMass")->set(leftUpperArmComX - leftShoulderRollX, upperArmComY - upperArmY, upperArmComZ);
		PARAM(Vector3DValue, mLeftUpperArm, "CenterOfMass")->set(leftUpperArmComX, upperArmComY, upperArmComZ);
		PARAM(DoubleValue, mLeftUpperArm, "Width")->set(mArmWidth->get());
		PARAM(DoubleValue, mLeftUpperArm, "Height")->set(upperArmHeight);
		PARAM(DoubleValue, mLeftUpperArm, "Depth")->set(mArmDepth->get());
		PARAM(DoubleValue, mLeftUpperArm, "Mass")->set(upperArmWeight);
		PARAM(ColorValue, mLeftUpperArm, "Color")->set(absColor);
		PARAM(StringValue, mLeftUpperArm, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mLeftUpperArm, "InertiaTensor")->set(leftUpperArmInertia);

		// Elbow is a cylinder body on top of the upperArm
		mLeftElbow->setMass(0.0);
		mLeftElbow->setMaterialType("Latex");
		CylinderGeom *leftElbowGeom = dynamic_cast<CylinderGeom*>(mLeftElbow->getGeometry());
		leftElbowGeom->setRadius(mElbowRadius->get());
		leftElbowGeom->setLength(mArmWidth->get());
		leftElbowGeom->setAutomaticColor(false);
		leftElbowGeom->setColor(cylinderColor);
		leftElbowGeom->setLocalPosition(Vector3D(0.0,
												elbowY - upperArmY,
												mElbowJointZ->get()));
		leftElbowGeom->setLocalOrientation(Quaternion(0.707, 0.0, 0.707, 0.0));

	// Right LowerArm
		PARAM(Vector3DValue, mRightLowerArm, "Position")->set(rightShoulderRollX, lowerArmY, mElbowJointZ->get());
//		PARAM(Vector3DValue, mRightLowerArm, "CenterOfMass")->set(rightLowerArmComX - rightShoulderRollX, lowerArmComY - lowerArmY, lowerArmComZ - mElbowJointZ->get());
		PARAM(Vector3DValue, mRightLowerArm, "CenterOfMass")->set(rightLowerArmComX, lowerArmComY, lowerArmComZ);
		PARAM(DoubleValue, mRightLowerArm, "Width")->set(mArmWidth->get());
		PARAM(DoubleValue, mRightLowerArm, "Height")->set(lowerArmHeight);
		PARAM(DoubleValue, mRightLowerArm, "Depth")->set(mArmDepth->get());
		PARAM(DoubleValue, mRightLowerArm, "Mass")->set(lowerArmWeight);
		PARAM(ColorValue, mRightLowerArm, "Color")->set(absColor);
		PARAM(StringValue, mRightLowerArm, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mRightLowerArm, "InertiaTensor")->set(rightLowerArmInertia);

	// Left LowerArm
		PARAM(Vector3DValue, mLeftLowerArm, "Position")->set(leftShoulderRollX, lowerArmY, mElbowJointZ->get());
//		PARAM(Vector3DValue, mLeftLowerArm, "CenterOfMass")->set(leftLowerArmComX - leftShoulderRollX, lowerArmComY - lowerArmY, lowerArmComZ - mElbowJointZ->get());
		PARAM(Vector3DValue, mLeftLowerArm, "CenterOfMass")->set(leftLowerArmComX, lowerArmComY, lowerArmComZ);
		PARAM(DoubleValue, mLeftLowerArm, "Width")->set(mArmWidth->get());
		PARAM(DoubleValue, mLeftLowerArm, "Height")->set(lowerArmHeight);
		PARAM(DoubleValue, mLeftLowerArm, "Depth")->set(mArmDepth->get());
		PARAM(DoubleValue, mLeftLowerArm, "Mass")->set(lowerArmWeight);
		PARAM(ColorValue, mLeftLowerArm, "Color")->set(absColor);
		PARAM(StringValue, mLeftLowerArm, "Material")->set("ABS");
		PARAM(Matrix3x3Value, mLeftLowerArm, "InertiaTensor")->set(leftLowerArmInertia);

	// ----------------------------------------------------------
	// Set the parameters of the Accelboards
	// ----------------------------------------------------------
	QuaternionValue *orientation;
		// AB00 RightLegLowerBottom
		PARAM(Vector3DValue, mAB00, "LocalPosition")->set(0.0, abLegLowerBottomYRel, abLegLowerBottomZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB00->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 180,0);

		// AB10 LeftLegLowerBottom
		PARAM(Vector3DValue, mAB10, "LocalPosition")->set(0.0, abLegLowerBottomYRel, abLegLowerBottomZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB10->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 180,0);

		// AB01 RightLegLowerTop
		PARAM(Vector3DValue, mAB01, "LocalPosition")->set(0.0, abLegLowerTopYRel, abLegLowerTopZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB01->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 0,0);

		// AB11 LeftLegLowerTop
		PARAM(Vector3DValue, mAB11, "LocalPosition")->set(0.0, abLegLowerTopYRel, abLegLowerTopZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB11->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 0,180);

		// AB02 RightLegUpperBottom
		PARAM(Vector3DValue, mAB02, "LocalPosition")->set(0.0, abLegUpperBottomYRel, abLegUpperZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB02->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 0,0);

		// AB12 LeftLegUpperBottom
		PARAM(Vector3DValue, mAB12, "LocalPosition")->set(0.0, abLegUpperBottomYRel, abLegUpperZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB12->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 0,180);

		// AB03 RightLegUpperMiddle
		PARAM(Vector3DValue, mAB03, "LocalPosition")->set(0.0, abLegUpperMiddleYRel, abLegUpperZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB03->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 0,0);

		// AB13 LeftLegUpperMiddle
		PARAM(Vector3DValue, mAB13, "LocalPosition")->set(0.0, abLegUpperMiddleYRel, abLegUpperZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB13->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 0,180);

		// AB04 RightLegUpperTop
		PARAM(Vector3DValue, mAB04, "LocalPosition")->set(0.0, abLegUpperTopYRel, abLegUpperZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB04->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 0,0);

		// AB14 LeftLegUpperTop
		PARAM(Vector3DValue, mAB14, "LocalPosition")->set(0.0, abLegUpperTopYRel, abLegUpperZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB14->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 0,180);

		// AB20 RightArmLower
		PARAM(Vector3DValue, mAB20, "LocalPosition")->set(0.0, abArmLowerYRel, abArmLowerZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB20->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 180,180);

		// AB30 LeftArmLower
		PARAM(Vector3DValue, mAB30, "LocalPosition")->set(0.0, abArmLowerYRel, abArmLowerZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB30->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 180,180);

		// AB21 RightArmUpper
		PARAM(Vector3DValue, mAB21, "LocalPosition")->set(0.0, abArmUpperYRel, abArmUpperZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB21->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 0,180);

		// AB31 LeftArmUpper
		PARAM(Vector3DValue, mAB31, "LocalPosition")->set(0.0, abArmUpperYRel, abArmUpperZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB31->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 0,180);

		// AB40 LeftBodyUpper
		PARAM(Vector3DValue, mAB40, "LocalPosition")->set(abLeftBodyUpperXRel, abBodyUpperYRel, abBodyUpperZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB40->getParameter("LocalOrientation"));
		orientation->setFromAngles(180, 0,90);

		// AB41 RightBodyUpper
		PARAM(Vector3DValue, mAB41, "LocalPosition")->set(abRightBodyUpperXRel, abBodyUpperYRel, abBodyUpperZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB41->getParameter("LocalOrientation"));
		orientation->setFromAngles(180, 0,-90);

		// AB42 LeftBodyLower
		PARAM(Vector3DValue, mAB42, "LocalPosition")->set(abLeftBodyLowerXRel, abBodyLowerYRel, abBodyLowerZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB42->getParameter("LocalOrientation"));
		orientation->setFromAngles(0, 90,0);

		// AB43 RightBodyLower
		PARAM(Vector3DValue, mAB43, "LocalPosition")->set(abRightBodyLowerXRel, abBodyLowerYRel, abBodyLowerZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB43->getParameter("LocalOrientation"));
		orientation->setFromAngles(0, 90,0);

		// AB44 BodyBottom
		PARAM(Vector3DValue, mAB44, "LocalPosition")->set(0.0, abBodyBottomYRel, abBodyBottomZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB44->getParameter("LocalOrientation"));
		orientation->setFromAngles(90, 180,180);

		// AB50 HeadLeft
		PARAM(Vector3DValue, mAB50, "LocalPosition")->set(abLeftHeadXRel, abHeadYRel, abHeadZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB50->getParameter("LocalOrientation"));
		orientation->setFromAngles(0, -90,0);

		// AB51 HeadRight
		PARAM(Vector3DValue, mAB51, "LocalPosition")->set(abRightHeadXRel, abHeadYRel, abHeadZRel);
		orientation = dynamic_cast<QuaternionValue*>(mAB51->getParameter("LocalOrientation"));
		orientation->setFromAngles(0, -90,0);

	// ----------------------------------------------------------
	// Now set the parameters of the Motors and Joints
	// ----------------------------------------------------------

	PARAM(Vector3DValue, mRightToesJoint, "AxisPoint1")->set(0.0, mToesJointY->get(), mToesJointZ->get());
	PARAM(Vector3DValue, mRightToesJoint, "AxisPoint2")->set(0.1, mToesJointY->get(), mToesJointZ->get());
	PARAM(DoubleValue, mRightToesJoint, "MinAngle")->set(0);
	PARAM(DoubleValue, mRightToesJoint, "MaxAngle")->set(90);

	PARAM(Vector3DValue, mLeftToesJoint, "AxisPoint1")->set(0.0, mToesJointY->get(), mToesJointZ->get());
	PARAM(Vector3DValue, mLeftToesJoint, "AxisPoint2")->set(0.1, mToesJointY->get(), mToesJointZ->get());
	PARAM(DoubleValue, mLeftToesJoint, "MinAngle")->set(0);
	PARAM(DoubleValue, mLeftToesJoint, "MaxAngle")->set(90);

	PARAM(Vector3DValue, mRightAnklePitchMotor, "AxisPoint1")->set(0.0, mAnkleJointY->get(), 0.0);
	PARAM(Vector3DValue, mRightAnklePitchMotor, "AxisPoint2")->set(0.1, mAnkleJointY->get(), 0.0);
	PARAM(DoubleValue, mRightAnklePitchMotor, "MinAngle")->set(-80);
	PARAM(DoubleValue, mRightAnklePitchMotor, "MaxAngle")->set(50);

	PARAM(Vector3DValue, mRightAnkleRollMotor, "AxisPoint1")->set(-mDistanceXHipYaw->get() / 2.0, mAnkleJointY->get(), 0.0);
	PARAM(Vector3DValue, mRightAnkleRollMotor, "AxisPoint2")->set(-mDistanceXHipYaw->get() / 2.0, mAnkleJointY->get(), 0.1);
	PARAM(DoubleValue, mRightAnkleRollMotor, "MinAngle")->set(-23);
	PARAM(DoubleValue, mRightAnkleRollMotor, "MaxAngle")->set(20);

	PARAM(Vector3DValue, mLeftAnklePitchMotor, "AxisPoint1")->set(0.0, mAnkleJointY->get(), 0.0);
	PARAM(Vector3DValue, mLeftAnklePitchMotor, "AxisPoint2")->set(0.1, mAnkleJointY->get(), 0.0);
	PARAM(DoubleValue, mLeftAnklePitchMotor, "MinAngle")->set(-80);
	PARAM(DoubleValue, mLeftAnklePitchMotor, "MaxAngle")->set(50);

	PARAM(Vector3DValue, mLeftAnkleRollMotor, "AxisPoint1")->set(mDistanceXHipYaw->get() / 2.0, mAnkleJointY->get(), 0.1);
	PARAM(Vector3DValue, mLeftAnkleRollMotor, "AxisPoint2")->set(mDistanceXHipYaw->get() / 2.0, mAnkleJointY->get(), 0.0);
	PARAM(DoubleValue, mLeftAnkleRollMotor, "MinAngle")->set(-23);
	PARAM(DoubleValue, mLeftAnkleRollMotor, "MaxAngle")->set(20);

	PARAM(Vector3DValue, mRightKneeMotor, "AxisPoint1")->set(0.0, kneeJointY, mKneeJointZ->get());
	PARAM(Vector3DValue, mRightKneeMotor, "AxisPoint2")->set(0.1, kneeJointY, mKneeJointZ->get());
	PARAM(DoubleValue, mRightKneeMotor, "MinAngle")->set(0);
	PARAM(DoubleValue, mRightKneeMotor, "MaxAngle")->set(164);

	PARAM(Vector3DValue, mLeftKneeMotor, "AxisPoint1")->set(0.0, kneeJointY, mKneeJointZ->get());
	PARAM(Vector3DValue, mLeftKneeMotor, "AxisPoint2")->set(0.1, kneeJointY, mKneeJointZ->get());
	PARAM(DoubleValue, mLeftKneeMotor, "MinAngle")->set(0);
	PARAM(DoubleValue, mLeftKneeMotor, "MaxAngle")->set(164);

	PARAM(Vector3DValue, mRightHipPitchMotor, "AxisPoint1")->set(0.0, hipPitchRollJointY, 0.0);
	PARAM(Vector3DValue, mRightHipPitchMotor, "AxisPoint2")->set(0.1, hipPitchRollJointY, 0.0);
	PARAM(DoubleValue, mRightHipPitchMotor, "MinAngle")->set(-110);
	PARAM(DoubleValue, mRightHipPitchMotor, "MaxAngle")->set(20);

	PARAM(Vector3DValue, mLeftHipPitchMotor, "AxisPoint1")->set(0.0, hipPitchRollJointY, 0.0);
	PARAM(Vector3DValue, mLeftHipPitchMotor, "AxisPoint2")->set(0.1, hipPitchRollJointY, 0.0);
	PARAM(DoubleValue, mLeftHipPitchMotor, "MinAngle")->set(-110);
	PARAM(DoubleValue, mLeftHipPitchMotor, "MaxAngle")->set(20);

	PARAM(Vector3DValue, mRightHipRollMotor, "AxisPoint1")->set(-mDistanceXHipYaw->get() / 2.0, hipPitchRollJointY, 0.0);
	PARAM(Vector3DValue, mRightHipRollMotor, "AxisPoint2")->set(-mDistanceXHipYaw->get() / 2.0, hipPitchRollJointY, 0.1);
	PARAM(DoubleValue, mRightHipRollMotor, "MinAngle")->set(-44);
	PARAM(DoubleValue, mRightHipRollMotor, "MaxAngle")->set(20);

	PARAM(Vector3DValue, mLeftHipRollMotor, "AxisPoint1")->set(mDistanceXHipYaw->get() / 2.0, hipPitchRollJointY, 0.1);
	PARAM(Vector3DValue, mLeftHipRollMotor, "AxisPoint2")->set(mDistanceXHipYaw->get() / 2.0, hipPitchRollJointY, 0.0);
	PARAM(DoubleValue, mLeftHipRollMotor, "MinAngle")->set(-44);
	PARAM(DoubleValue, mLeftHipRollMotor, "MaxAngle")->set(20);

	PARAM(Vector3DValue, mRightHipYawMotor, "AxisPoint1")->set(-mDistanceXHipYaw->get() / 2.0, 0.0, 0.0);
	PARAM(Vector3DValue, mRightHipYawMotor, "AxisPoint2")->set(-mDistanceXHipYaw->get() / 2.0, 0.1, 0.0);
	PARAM(DoubleValue, mRightHipYawMotor, "MinAngle")->set(-44);
	PARAM(DoubleValue, mRightHipYawMotor, "MaxAngle")->set(20);

	PARAM(Vector3DValue, mLeftHipYawMotor, "AxisPoint1")->set(mDistanceXHipYaw->get() / 2.0, 0.1, 0.0);
	PARAM(Vector3DValue, mLeftHipYawMotor, "AxisPoint2")->set(mDistanceXHipYaw->get() / 2.0, 0.0, 0.0);
	PARAM(DoubleValue, mLeftHipYawMotor, "MinAngle")->set(-44);
	PARAM(DoubleValue, mLeftHipYawMotor, "MaxAngle")->set(20);

	PARAM(Vector3DValue, mWaistRollMotor, "AxisPoint1")->set(0.0, waistRollY, 0.0);
	PARAM(Vector3DValue, mWaistRollMotor, "AxisPoint2")->set(0.0, waistRollY, 0.1);
	PARAM(DoubleValue, mWaistRollMotor, "MinAngle")->set(-20);
	PARAM(DoubleValue, mWaistRollMotor, "MaxAngle")->set(20);

	PARAM(Vector3DValue, mHeadPitchMotor, "AxisPoint1")->set(0.0, headPitchY, 0.0);
	PARAM(Vector3DValue, mHeadPitchMotor, "AxisPoint2")->set(0.1, headPitchY, 0.0);
	PARAM(DoubleValue, mHeadPitchMotor, "MinAngle")->set(-30);
	PARAM(DoubleValue, mHeadPitchMotor, "MaxAngle")->set(50);

	PARAM(Vector3DValue, mHeadYawMotor, "AxisPoint1")->set(0.0, 0.0, mHeadYawJointZ->get());
	PARAM(Vector3DValue, mHeadYawMotor, "AxisPoint2")->set(0.0, 0.1, mHeadYawJointZ->get());
	PARAM(DoubleValue, mHeadYawMotor, "MinAngle")->set(-90);
	PARAM(DoubleValue, mHeadYawMotor, "MaxAngle")->set(90);

	PARAM(Vector3DValue, mHeadRollMotor, "AxisPoint1")->set(0.0, headYawRollY, 0.0);
	PARAM(Vector3DValue, mHeadRollMotor, "AxisPoint2")->set(0.0, headYawRollY, 0.1);
	PARAM(DoubleValue, mHeadRollMotor, "MinAngle")->set(-30);
	PARAM(DoubleValue, mHeadRollMotor, "MaxAngle")->set(30);

	PARAM(Vector3DValue, mRightShoulderPitchMotor, "AxisPoint1")->set(0.0, shoulderY, 0.0);
	PARAM(Vector3DValue, mRightShoulderPitchMotor, "AxisPoint2")->set(0.1, shoulderY, 0.0);
	PARAM(DoubleValue, mRightShoulderPitchMotor, "MinAngle")->set(-50);
	PARAM(DoubleValue, mRightShoulderPitchMotor, "MaxAngle")->set(175);

	PARAM(Vector3DValue, mLeftShoulderPitchMotor, "AxisPoint1")->set(0.0, shoulderY, 0.0);
	PARAM(Vector3DValue, mLeftShoulderPitchMotor, "AxisPoint2")->set(0.1, shoulderY, 0.0);
	PARAM(DoubleValue, mLeftShoulderPitchMotor, "MinAngle")->set(-50);
	PARAM(DoubleValue, mLeftShoulderPitchMotor, "MaxAngle")->set(175);

	PARAM(Vector3DValue, mRightShoulderRollMotor, "AxisPoint1")->set(rightShoulderRollX, shoulderY, 0.0);
	PARAM(Vector3DValue, mRightShoulderRollMotor, "AxisPoint2")->set(rightShoulderRollX, shoulderY, 0.1);
	PARAM(DoubleValue, mRightShoulderRollMotor, "MinAngle")->set(-25);
	PARAM(DoubleValue, mRightShoulderRollMotor, "MaxAngle")->set(90);

	PARAM(Vector3DValue, mLeftShoulderRollMotor, "AxisPoint1")->set(leftShoulderRollX, shoulderY, 0.1);
	PARAM(Vector3DValue, mLeftShoulderRollMotor, "AxisPoint2")->set(leftShoulderRollX, shoulderY, 0.0);
	PARAM(DoubleValue, mLeftShoulderRollMotor, "MinAngle")->set(-25);
	PARAM(DoubleValue, mLeftShoulderRollMotor, "MaxAngle")->set(90);

	PARAM(Vector3DValue, mRightElbowPitchMotor, "AxisPoint1")->set(0.0, elbowY, mElbowJointZ->get());
	PARAM(Vector3DValue, mRightElbowPitchMotor, "AxisPoint2")->set(0.1, elbowY, mElbowJointZ->get());
	PARAM(DoubleValue, mRightElbowPitchMotor, "MinAngle")->set(0);
	PARAM(DoubleValue, mRightElbowPitchMotor, "MaxAngle")->set(90);

	PARAM(Vector3DValue, mLeftElbowPitchMotor, "AxisPoint1")->set(0.0, elbowY, mElbowJointZ->get());
	PARAM(Vector3DValue, mLeftElbowPitchMotor, "AxisPoint2")->set(0.1, elbowY, mElbowJointZ->get());
	PARAM(DoubleValue, mLeftElbowPitchMotor, "MinAngle")->set(0);
	PARAM(DoubleValue, mLeftElbowPitchMotor, "MaxAngle")->set(90);
}



