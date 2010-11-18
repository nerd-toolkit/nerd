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

#include "ASeriesMorph.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Collision/CollisionManager.h"
#include "Physics/SimObjectGroup.h"
#include "Util/Color.h"
#include "Physics/Physics.h"
#include "SimulationConstants.h"
#include "Physics/BoxGeom.h"
#include "Collision/CollisionObject.h"
#include <iostream>
using namespace std;

#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))

using namespace nerd;


ASeriesMorph::ASeriesMorph(const QString &groupName) : ModelInterface(groupName) {
	mPosition->addValueChangedListener(this);
}

ASeriesMorph::ASeriesMorph(const ASeriesMorph &model) : Object(), ValueChangedListener(), 
		EventListener(), ModelInterface(model) 
{
	mPosition->addValueChangedListener(this);
}

ASeriesMorph::~ASeriesMorph(){
}

void ASeriesMorph::createModel() {

	mUseDynamixels = false;
	
	mGroupName = getName();

// 	mNamePrefix = mGroupName;
// 	mNamePrefix.append("/");
	mNamePrefix = "/";

	PhysicsManager *pm = Physics::getPhysicsManager();

	mIncludeAccus = false;
	
	SimObject *boxBody = pm->getPrototype(
			SimulationConstants::PROTOTYPE_BOX_BODY); 
	SimObject *dynamixel;
	if(mUseDynamixels) {
	//TODO: The prototype of the torque driven dynamixel needs to be added here.
	 dynamixel = (pm->getPrototype(
			SimulationConstants::PROTOTYPE_DYNAMIXEL));
	}
	else {
	 dynamixel = (pm->getPrototype(
			SimulationConstants::PROTOTYPE_DYNAMIXEL));
	}
	SimObject *fixed =  (pm->getPrototype(
			SimulationConstants::PROTOTYPE_FIXED_JOINT));
	SimObject *accelSensor = (pm->getPrototype(
			SimulationConstants::PROTOTYPE_ACCELSENSOR));


	SimObject *forceSensor = pm->getPrototype(
			"Prototypes/ForceSensor");

	double artificialMassScale = 1.0;
	if(mUseDynamixels) {
		artificialMassScale = 10.0;
	}
	else {
		artificialMassScale = 1.0;
	}

	double accuWeight = 0.047 * artificialMassScale;
	if(!mIncludeAccus) {
		accuWeight = 0.0;
	}

	double weightDiff = 0.37294 * artificialMassScale;
	double totalScrewWeight = 0.1 * artificialMassScale;
	double cableWeight = weightDiff - totalScrewWeight;

	double screwNumber = 243.0;
	double screwWeight = totalScrewWeight/screwNumber;
	double cableLength = 529.0;
	double cablePartWeight = cableWeight/cableLength;

	double dynamixelMass = 0.055 * artificialMassScale;
	double smallPlateWeight = 0.01/3.0 * artificialMassScale;
	double headWeight = 0.03 * artificialMassScale;
	double handWeight = 0.010 * artificialMassScale;
	double pdaWeight = (0.181 + 0.02999) * artificialMassScale;
	double soleOfFootWeight = 0.015 * artificialMassScale;
	double footWeight = 0.0689 * artificialMassScale + 20.0*cablePartWeight;
	
	double mainBoardWeight = 0.025 * artificialMassScale;
	double hipPlateWeight = 0.053 * artificialMassScale;
	double accuCaseWeight = 0.00747 * artificialMassScale;
	double femurABSWeight = 0.03658 * artificialMassScale 
				- accuCaseWeight 
				+ 24.0*cablePartWeight;
	double mainBodyWeight = 0.06 * artificialMassScale 
				+ 2.0 * dynamixelMass 
				+ 20.0*screwWeight 
				+ 146.0*cablePartWeight;
	double ankleWeight = 2.0 * dynamixelMass 
				+ 3.0*smallPlateWeight 
				+ 15.0*screwWeight 
				+ 15.0*cablePartWeight;
	double tibiaWeight = dynamixelMass 
				+ 0.01 * artificialMassScale 
				+ 9.0 * screwWeight 
				+ 15.0 * cablePartWeight;
	double femurWeight = femurABSWeight;
	double uFemurWeight = 2.0 * dynamixelMass 
				+ 2.0 * smallPlateWeight 
				+ 27.0 * screwWeight 
				+ 30.0 * cablePartWeight; //+ 3.0 * smallPlateWeight;	
	double hipPartWeight = 0.015 * artificialMassScale 
				+ 4.0 * cablePartWeight;
	double shoulderWeight = 0.012 * artificialMassScale 
				+ 4.0 * screwWeight 
				+ 10.0*cablePartWeight;
	double upperArmWeight = dynamixelMass 
				+ 0.015 * artificialMassScale 
				+ 11.0 * screwWeight 
				+ 12.5 * cablePartWeight;
	double lowerArmWeight = dynamixelMass 
				+ smallPlateWeight 
				+ 14.0*screwWeight 
				+ 14.0*cablePartWeight;
// 0.007 as mass of the additional plate under the accuCase
	double invisibleHipWeight = dynamixelMass / 2.0 
				+ 0.007 
				+ 53.0 * screwWeight 
				+ 72.0 * cablePartWeight;
	double neckWeight = dynamixelMass + 12.0*cablePartWeight;
	double headBaseWeight = dynamixelMass + 10.0 * screwWeight + 10.0 * cablePartWeight;

	double sizeScale = 1.0;

	double legOffset = 0.0325 * sizeScale;
	double footOffset = 0.011 * sizeScale;
	double footHeight = 0.0164 * sizeScale;
	double accelBoardHeight = 0.0125 * sizeScale;
	double accelBoardDepth = 0.026 * sizeScale;
	double accelOffsetFootX = 0.0072 * sizeScale;
	double accelOffsetFootY = accelBoardHeight / 2.0 + footHeight / 2.0;
	double accelOffsetFootZ = 0.03005 * sizeScale;

	double dynamixelWidth = 0.032 * sizeScale;
	double dynamixelHeight = 0.038 * sizeScale;
	double dynamixelDepth = 0.05 * sizeScale;

	double ankleHeight = 0.0505 * sizeScale;
	double anklePosZ = -0.0245;
	double height = ankleHeight - dynamixelDepth / 2.0 + 0.0115;//0.026 + 0.011 / 2.0;
	double tibiaPosY = ankleHeight 
				+ dynamixelDepth / 2.0 
				+ 0.008 
				+ (dynamixelWidth + 0.017) / 2.0;
	double tibiaHeight = dynamixelWidth + 0.017 * sizeScale;
	double legOffsetZ = -0.0055 * sizeScale;

	double femurPosY = tibiaHeight/2.0 + tibiaPosY + 0.015	;
	double femurHeight = 0.021 * sizeScale;

	double upperFemurPosY = femurPosY + femurHeight / 2.0 + 0.003 + dynamixelDepth / 2.0;

	double space = 0.0005;

	double soleOfFootHeight = 0.001;
	double footW = 0.07;
	double footH = 0.0164 - soleOfFootHeight;
	double footD = 0.1005;

	double footX = -legOffset-footOffset;
	double footY = (footHeight - soleOfFootHeight) / 2.0 + soleOfFootHeight;
	double footZ = 	0.0;

	double rubberSoleHeight = footH + soleOfFootHeight - space;
	
	double soleOfFootFW = 0.0154;

	double soleOfFootFH = rubberSoleHeight;
	double soleOfFootFD = 0.0154;

	double soleOfFootFX = footW/2.0 - soleOfFootFW/2.0 - 0.004;
	double soleOfFootFY = -soleOfFootHeight/2.0 - space;
	double soleOfFootFZ = footD/2.0 - soleOfFootFD/2.0 - 0.004;
	
	double soleOfFootOutSideW = 0.013;
	double soleOfFootOutSideH = soleOfFootFH;
	double soleOfFootOutSideD = 0.013;

	double soleOfFootFOutX = -footW/2.0 + soleOfFootOutSideW/2.0 + 0.004;
	double soleOfFootFOutY = -soleOfFootHeight/2.0 - space;
	double soleOfFootFOutZ = footD/2.0 - soleOfFootFD - soleOfFootOutSideD/2.0;

	double soleOfFootBX = 0.0;
	double soleOfFootBY = -soleOfFootHeight/2.0 - space; 
	double soleOfFootBW = 0.0375;
	double soleOfFootBH = rubberSoleHeight;
	double soleOfFootBD = 0.014;
	double soleOfFootBZ = -footD/2.0 + soleOfFootBD/2.0 + 0.004;

	double ankleW = dynamixelHeight;
	double ankleH = dynamixelDepth;
	double ankleD = dynamixelWidth + dynamixelHeight;
	double ankleX = legOffset;
	double ankleY = ankleHeight;
	double ankleZ = anklePosZ;

	double tibiaW = 0.0425;
	double tibiaH = dynamixelWidth + 0.017 * sizeScale;
	double tibiaD = dynamixelDepth;
	double tibiaX = legOffset;
	double tibiaY = ankleHeight + dynamixelDepth / 2.0 + 0.008 + (dynamixelWidth + 0.017) / 2.0;
	double tibiaZ = legOffsetZ;

	double femurW = 0.047;
	double femurH = 0.021;
	double femurD = 0.043;
	double femurX = legOffset - 0.00025;
	double femurY = tibiaHeight / 2.0 + tibiaPosY + 0.021 / 2.0 + 0.001;
	double femurZ = legOffsetZ - 0.005;
	
	double uFemurX = legOffset;
	double uFemurY = upperFemurPosY;
	double uFemurZ = -0.0245 * sizeScale;
	double uFemurW = dynamixelHeight;
	double uFemurH = dynamixelDepth;
	double uFemurD = dynamixelWidth + dynamixelHeight;

	double diffX = (-0.005- 0.047/2.0) - 0.00025 - space;
	double diffY = -0.0095;
	double diffZ = femurD/2.0-0.00381-0.049/2.0;
	
	double accuCaseW = 0.01;
	double accuCaseH = 0.094;
	double accuCaseD = 0.049;

	double rightHipPartY = upperFemurPosY + dynamixelDepth / 2.0 - 0.0115 + 0.0289 - 0.003;

	double hipPartX = legOffset;
	double hipPartY = rightHipPartY;
	double hipPartZ = anklePosZ;
	double hipPartW = 0.0276;
	double hipPartH = 0.006;
	double hipPartD = 0.078;

	double hipPlatePosY =rightHipPartY + 0.003 + 0.0025 + 0.001;

	double hipPosX = 0.038/2.0 + 0.027/2.0;
	double hipPosY = hipPlatePosY + 0.038/2.0 + 0.0025 + space;
	double hipPosZ = anklePosZ + 0.011 - 0.0533/2.0;

	double middleHipX = 0;
	double middleHipY = hipPosY;
	double middleHipZ = hipPosZ + 0.034;

	double middleHipDiffX = 0.0;
	double middleHipDiffY = 0.00125;
	double middleHipDiffZ = -0.022;
	
	double hipDiffX = -hipPosX;
	double hipDiffY = hipPosY - (hipPosY);
	double hipDiffZ =  -(hipPosZ + 0.034 - (hipPosZ));

	double hipSideX = hipDiffX;
	double hipSideY = hipDiffY - middleHipDiffY;
	double hipSideZ = hipDiffZ - middleHipDiffZ;

	double hipSideW = 0.0295;
	double hipSideH = 0.038;
	double hipSideD = 0.0533;

	double hipPlateDiffX = 0;
	double hipPlateDiffY = hipPosY - (hipPlatePosY);
	double hipPlateDiffZ = hipPosZ + 0.034 - (legOffsetZ - 0.052/2.0);

	double accuCasePosY = hipPosY + dynamixelHeight/2.0 + 0.0075/2.0;

	double accuCaseDiffX = 0;
	double accuCaseDiffY = hipPosY - (accuCasePosY);
	double accuCaseDiffZ = hipPosZ + 0.034 - (hipPosZ);

	double pdaPosY = hipPosY + 0.052 - dynamixelHeight/2.0;
	double pdaPosZ = hipPosZ - 0.01015 - 0.0533/2.0 - 0.0005;

	double upperBodyPosY = accuCasePosY + 0.0034 + 0.063/2.0 + 0.0075/2.0;
	double upperBodyPosZ = pdaPosZ + 0.0203/2.0 + 0.0276 + 0.0656/2.0;

	double neckPosZ = upperBodyPosZ -(0.0656)/2.0  + 0.025;
	double neckPosY = upperBodyPosY + 0.063/2.0 + 0.037/2.0 + 0.0005;

	double headBasePosZ = neckPosZ + 0.025 - 0.0115 - 0.0115 + 0.025;
	double headBasePosY = neckPosY + 0.0348/2.0 + 0.037/2.0 + 0.001;

	double shoulderPosX = (0.1047/2.0 + 0.0035 + 0.001);
	double shoulderPosY = upperBodyPosY + 0.063/2.0 - 0.005 - 0.0124;
	double shoulderPosZ = upperBodyPosZ - 0.004;

	double shoulderW = 0.007;
	double shoulderH = 0.0248;
	double shoulderD = 0.048;

	double upperArmPosX = (shoulderPosX - 0.007/2.0 + 0.0231 + 0.0609/2.0 - 0.0115);
	double upperArmPosY = upperBodyPosY + 0.063/2.0 - 0.005 - 0.0124;
	double upperArmPosZ = upperBodyPosZ - 0.004 + 0.0148;

	double upperArmW = 0.0609;
	double upperArmH = 0.032;
	double upperArmD = 0.038;

	double lowerArmPosX = upperArmPosX + 0.0609/2.0 + 0.0565/2.0 + 0.0075;
	double lowerArmW = 0.0565;
	double lowerArmH = 0.032;
	double lowerArmD = 0.038;

	Color *dynamixelColor = new Color(75, 75, 75, 255);
	Color *absColor = new Color(25, 25, 25, 255);

	//Agent SimObject group.
	SimObjectGroup *agent = new SimObjectGroup(mGroupName, "Agent");
	mAgent = agent;
	
	
	// Right Foot
	SimObject *body = boxBody->createCopy();
	mRightFoot = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Right/Foot"));
	PARAM(DoubleValue, body, "Mass")->set(footWeight - soleOfFootWeight);
	PARAM(Vector3DValue, body, "Position")->set(footX, footY, footZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(DoubleValue, body, "Width")->set(footW * sizeScale);
	PARAM(DoubleValue, body, "Height")->set(footH * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(footD * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*absColor);
	PARAM(StringValue, body, "Material")->set("ABS");
	//pm->addSimObject(body);
	SimBody *rightFoot = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	// Inner Sole Of Foot	
	CollisionObject *rightSoleOfFootFront = new CollisionObject(BoxGeom(rightFoot, 
		soleOfFootFW, soleOfFootFH, soleOfFootFD),rightFoot,true);
	mRightSoleOfFootFront = rightSoleOfFootFront;
	BoxGeom *rightSoleOfFootFrontBox = 
		dynamic_cast<BoxGeom*>(rightSoleOfFootFront->getGeometry());
	rightSoleOfFootFrontBox->setAutomaticColor(false);
	rightSoleOfFootFront->setMass(soleOfFootWeight/4.0);
	rightSoleOfFootFront->setMaterialType("Latex");
	rightSoleOfFootFrontBox->setLocalPosition(Vector3D(soleOfFootFX, 
													   soleOfFootFY, 
													   soleOfFootFZ));
	rightSoleOfFootFrontBox->setColor(0,0,0,255);
	rightFoot->addCollisionObject(rightSoleOfFootFront);
	
	CollisionObject *rightSoleOfFootFrontOut = new CollisionObject(BoxGeom(rightFoot,
		soleOfFootOutSideW, soleOfFootOutSideH, soleOfFootOutSideD),rightFoot,true);
	mRightSoleOfFootFrontOut = rightSoleOfFootFrontOut;
	BoxGeom *rightSoleOfFootFrontOutBox = 
		dynamic_cast<BoxGeom*>(rightSoleOfFootFrontOut->getGeometry());
	rightSoleOfFootFrontBox->setAutomaticColor(false);
	rightSoleOfFootFrontOut->setMass(soleOfFootWeight/4.0);
	rightSoleOfFootFrontOut->setMaterialType("Latex");
	rightSoleOfFootFrontOutBox->setLocalPosition(Vector3D(soleOfFootFOutX, 
														  soleOfFootFOutY, 
														  soleOfFootFOutZ));
	rightSoleOfFootFrontOutBox->setColor(0,0,0,255);
	rightFoot->addCollisionObject(rightSoleOfFootFrontOut);

	CollisionObject *rightSoleOfFootBack = new CollisionObject(BoxGeom(rightFoot, 
		soleOfFootBW, soleOfFootBH, soleOfFootBD),rightFoot,true);
	mRightSoleOfFootBack = rightSoleOfFootBack;
	BoxGeom *rightSoleOfFootBackBox = 
		dynamic_cast<BoxGeom*>(rightSoleOfFootBack->getGeometry());
	rightSoleOfFootBackBox->setAutomaticColor(false);
	rightSoleOfFootBack->setMass(soleOfFootWeight/2.0);
	rightSoleOfFootBack->setMaterialType("Latex");
	rightSoleOfFootBackBox->setLocalPosition(Vector3D(soleOfFootBX, soleOfFootBY, soleOfFootBZ));
	rightSoleOfFootBackBox->setColor(0,0,0,255);
	rightFoot->addCollisionObject(rightSoleOfFootBack);

	body = boxBody->createCopy();
	mRightAnkleBox = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Right/AnkleBox"));
	PARAM(DoubleValue, body, "Mass")->set(ankleWeight);
	PARAM(Vector3DValue, body, "Position")->set(-ankleX,ankleY, ankleZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(DoubleValue, body, "Width")->set(ankleW);
	PARAM(DoubleValue, body, "Height")->set(ankleH);
	PARAM(DoubleValue, body, "Depth")->set(ankleD);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	
	//pm->addSimObject(body);
	SimBody *rightAnkle = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = rightAnkle->createCopy();
	mRightTibia = body;
	body->setName(mNamePrefix + ("Right/Tibia"));
	PARAM(DoubleValue, body, "Mass")->set(tibiaWeight);
	PARAM(Vector3DValue, body, "Position")->set(-tibiaX,tibiaY,tibiaZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(StringValue, body, "Texture")->set("Dynamixel");
	PARAM(DoubleValue, body, "Width")->set(tibiaW);
	PARAM(DoubleValue, body, "Height")->set(tibiaH);
	PARAM(DoubleValue, body, "Depth")->set(tibiaD);
	
	//pm->addSimObject(body);
	SimBody *rightTibia = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mRightFemur = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Right/Femur"));
	PARAM(DoubleValue, body, "Mass")->set(femurWeight);
	PARAM(Vector3DValue, body, "Position")->set(-femurX, femurY, femurZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("ABS");
	PARAM(DoubleValue, body, "Width")->set(femurW);
	PARAM(DoubleValue, body, "Height")->set(femurH);
	PARAM(DoubleValue, body, "Depth")->set(femurD);
	PARAM(ColorValue, body, "Color")->set(*absColor);
	//pm->addSimObject(body);
	SimBody *rightFemur = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mRightUpperFemur = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Right/UpperFemur"));
	PARAM(DoubleValue, body, "Mass")->set(uFemurWeight);
	PARAM(Vector3DValue, body, "Position")->set(-uFemurX, uFemurY, uFemurZ);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(DoubleValue, body, "Width")->set(uFemurW);
	PARAM(DoubleValue, body, "Height")->set(uFemurH);
	PARAM(DoubleValue, body, "Depth")->set(uFemurD);
	//pm->addSimObject(body);
	SimBody *rightUpperFemur = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	CollisionObject *rightAccuCase = new CollisionObject(BoxGeom(rightFemur, accuCaseW, 
		accuCaseH, accuCaseD),rightFemur,true);
	mRightAccuCase = rightAccuCase;
	BoxGeom *rightAccuCaseBox= dynamic_cast<BoxGeom*>(rightAccuCase->getGeometry());
	rightAccuCaseBox->setAutomaticColor(false);
	rightAccuCase->setMass(accuCaseWeight + accuWeight);
	rightAccuCase->setMaterialType("ABS");
	rightAccuCaseBox->setLocalPosition(Vector3D(diffX, diffY, diffZ));
	rightAccuCaseBox->setColor(*absColor);
	rightFemur->addCollisionObject(rightAccuCase);
	
	// Left Foot
	body = boxBody->createCopy();
	mLeftFoot = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Left/Foot"));
	PARAM(DoubleValue, body, "Mass")->set(footWeight - soleOfFootWeight);
	PARAM(Vector3DValue, body, "Position")->set(-footX, footY, footZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(DoubleValue, body, "Width")->set(footW * sizeScale);
	PARAM(DoubleValue, body, "Height")->set(footH * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(footD * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*absColor);
	PARAM(StringValue, body, "Material")->set("ABS");
	//pm->addSimObject(body);
	SimBody *leftFoot = dynamic_cast<SimBody*>(body);
	agent->addObject(body);
	
	CollisionObject *leftSoleOfFootFront = new CollisionObject(BoxGeom(leftFoot, soleOfFootFW,
		soleOfFootFH, soleOfFootFD),leftFoot,true);
	mLeftSoleOfFootFront = leftSoleOfFootFront;
	BoxGeom *leftSoleOfFootFrontBox= dynamic_cast<BoxGeom*>(leftSoleOfFootFront->getGeometry());
	leftSoleOfFootFrontBox->setAutomaticColor(false);
	leftSoleOfFootFront->setMass(soleOfFootWeight/4.0);
	leftSoleOfFootFront->setMaterialType("Latex");
	leftSoleOfFootFrontBox->setLocalPosition(Vector3D(-soleOfFootFX, 
													  soleOfFootFY, 
													  soleOfFootFZ));
	leftSoleOfFootFrontBox->setColor(0,0,0,255);
	leftFoot->addCollisionObject(leftSoleOfFootFront);
	
	CollisionObject *leftSoleOfFootFrontOut = new CollisionObject(BoxGeom(leftFoot,
		soleOfFootOutSideW, soleOfFootOutSideH, soleOfFootOutSideD),rightFoot,true);
	mLeftSoleOfFootFrontOut = leftSoleOfFootFrontOut;
	BoxGeom *leftSoleOfFootFrontOutBox= dynamic_cast<BoxGeom*>(leftSoleOfFootFrontOut
		->getGeometry());
	leftSoleOfFootFrontOutBox->setAutomaticColor(false);
	leftSoleOfFootFrontOut->setMass(soleOfFootWeight/4.0);
	leftSoleOfFootFrontOut->setMaterialType("Latex");
	leftSoleOfFootFrontOutBox->setLocalPosition(Vector3D(-soleOfFootFOutX, 
		soleOfFootFOutY, soleOfFootFOutZ));
	leftSoleOfFootFrontOutBox->setColor(0,0,0,255);
	leftFoot->addCollisionObject(leftSoleOfFootFrontOut);


	CollisionObject *leftSoleOfFootBack = new CollisionObject(BoxGeom(leftFoot, 
		soleOfFootBW, soleOfFootBH, soleOfFootBD),leftFoot,true);
	mLeftSoleOfFootBack = leftSoleOfFootBack;
	BoxGeom *leftSoleOfFootBackBox = dynamic_cast<BoxGeom*>(leftSoleOfFootBack
		->getGeometry());
	leftSoleOfFootBackBox->setAutomaticColor(false);
	leftSoleOfFootBack->setMass(soleOfFootWeight/2.0);
	leftSoleOfFootBack->setMaterialType("Latex");
	leftSoleOfFootBackBox->setLocalPosition(Vector3D(soleOfFootBX, 
													 soleOfFootBY, 
													 soleOfFootBZ));
	leftSoleOfFootBackBox->setColor(0,0,0,255);
	leftFoot->addCollisionObject(leftSoleOfFootBack);
	
	body = boxBody->createCopy();
	mLeftAnkleBox = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Left/AnkleBox"));
	PARAM(DoubleValue, body, "Mass")->set(ankleWeight);
	PARAM(Vector3DValue, body, "Position")->set(ankleX, ankleY, ankleZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(DoubleValue, body, "Width")->set(ankleW);
	PARAM(DoubleValue, body, "Height")->set(ankleH);
	PARAM(DoubleValue, body, "Depth")->set(ankleD);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	
	//pm->addSimObject(body);
	SimBody *leftAnkle = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mLeftTibia = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Left/Tibia"));
	PARAM(DoubleValue, body, "Mass")->set(tibiaWeight);
	PARAM(Vector3DValue, body, "Position")->set(tibiaX, tibiaY, tibiaZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(StringValue, body, "Texture")->set("Dynamixel");
	PARAM(DoubleValue, body, "Width")->set(tibiaW);
	PARAM(DoubleValue, body, "Height")->set(tibiaH);
	PARAM(DoubleValue, body, "Depth")->set(tibiaD);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	
	//pm->addSimObject(body);
	SimBody *leftTibia = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();	
	mLeftFemur = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Left/Femur"));
	PARAM(DoubleValue, body, "Mass")->set(femurWeight);
	PARAM(Vector3DValue, body, "Position")->set(femurX, femurY, femurZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("ABS");
	PARAM(DoubleValue, body, "Width")->set(femurW);
	PARAM(DoubleValue, body, "Height")->set(femurH);
	PARAM(DoubleValue, body, "Depth")->set(femurD);
	PARAM(ColorValue, body, "Color")->set(*absColor);
	
	//pm->addSimObject(body);
	SimBody *leftFemur = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mLeftUpperFemur = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Left/UpperFemur"));
	PARAM(DoubleValue, body, "Mass")->set(uFemurWeight);
	PARAM(Vector3DValue, body, "Position")->set(uFemurX, uFemurY, uFemurZ);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(DoubleValue, body, "Width")->set(uFemurW);
	PARAM(DoubleValue, body, "Height")->set(uFemurH);
	PARAM(DoubleValue, body, "Depth")->set(uFemurD);
	
	//pm->addSimObject(body);
	SimBody *leftUpperFemur = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	CollisionObject *leftAccuCase = new CollisionObject(BoxGeom(leftFemur, accuCaseW, accuCaseH, accuCaseD),leftFemur,true);
	mLeftAccuCase = leftAccuCase;
	BoxGeom *leftAccuCaseBox = dynamic_cast<BoxGeom*>(leftAccuCase->getGeometry());
	leftAccuCaseBox->setAutomaticColor(false);
	leftAccuCase->setMass(accuCaseWeight + accuWeight);
	leftAccuCase->setMaterialType("ABS");
	leftAccuCaseBox->setLocalPosition(Vector3D(-diffX, diffY, diffZ));
	leftAccuCaseBox->setColor(*absColor);
	leftFemur->addCollisionObject(leftAccuCase);
	
	// Hipplate
	body = boxBody->createCopy();
	mHipPlate = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Right/HipPart"));
	PARAM(DoubleValue, body, "Mass")->set(hipPartWeight);
	PARAM(Vector3DValue, body, "Position")->set(-hipPartX, hipPartY, hipPartZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(DoubleValue, body, "Width")->set(hipPartW * sizeScale);
	PARAM(DoubleValue, body, "Height")->set(hipPartH * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(hipPartD * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*absColor);
	
	//pm->addSimObject(body);
	agent->addObject(body);


	body = boxBody->createCopy();
	mLeftHipPart = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Left/HipPart"));
	PARAM(DoubleValue, body, "Mass")->set(hipPartWeight);
	PARAM(Vector3DValue, body, "Position")->set(hipPartX, hipPartY, hipPartZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(DoubleValue, body, "Width")->set(hipPartW * sizeScale);
	PARAM(DoubleValue, body, "Height")->set(hipPartH * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(hipPartD * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*absColor);
	
	//pm->addSimObject(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mMiddleInvisiblePart = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Middle/InvisiblePart"));
	PARAM(DoubleValue, body, "Mass")->set(invisibleHipWeight);
	PARAM(Vector3DValue, body, "Position")->set(middleHipX, 
												middleHipY + 0.00125, 
												middleHipZ - 0.022);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(DoubleValue, body, "Width")->set(0.097 * sizeScale);
	PARAM(DoubleValue, body, "Height")->set((0.0505 - (2.0*0.00125) )* sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(2.0*(0.0533/2.0 + 0.012)* sizeScale);
	PARAM(ColorValue, body, "Color")->set(0,0,0,0);
	body->getGeometries().at(0)->enableDrawing(false);
	
	//pm->addSimObject(body);
	SimBody *invisibleHip = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	CollisionObject *hipMiddleObject= new CollisionObject(BoxGeom(invisibleHip, 0.038,
		 0.038, 0.05),invisibleHip,true);
	mHipMiddleObject = hipMiddleObject;
	BoxGeom *hipMiddleBox = dynamic_cast<BoxGeom*>(hipMiddleObject->getGeometry());
	hipMiddleBox->setAutomaticColor(false);
	hipMiddleObject->setMass(dynamixelMass/2.0);
	hipMiddleObject->setMaterialType("PlasticsWhite");
	hipMiddleObject->setTextureType("Dynamixel");
	hipMiddleBox->setLocalPosition(Vector3D(middleHipDiffX, 
											-middleHipDiffY, 
											-middleHipDiffZ));
	hipMiddleBox->setColor(*dynamixelColor);
	invisibleHip->addCollisionObject(hipMiddleObject);

	CollisionObject *rightHipPartObject= new CollisionObject(BoxGeom(invisibleHip, 
		hipSideW, hipSideH, hipSideD),invisibleHip,true);
	mRightHipPartObject = rightHipPartObject;
	BoxGeom *rightHipPartBox = dynamic_cast<BoxGeom*>(rightHipPartObject->getGeometry());
	rightHipPartBox->setAutomaticColor(false);
	rightHipPartObject->setMass(dynamixelMass);
	rightHipPartObject->setMaterialType("PlasticsWhite");
	rightHipPartObject->setTextureType("Dynamixel");
	rightHipPartBox->setLocalPosition(Vector3D(hipSideX, hipSideY, hipSideZ));
	rightHipPartBox->setColor(*dynamixelColor);
	invisibleHip->addCollisionObject(rightHipPartObject);
	
	CollisionObject *leftHipPartObject= new CollisionObject(BoxGeom(invisibleHip,hipSideW, hipSideH, 
		hipSideD),invisibleHip,true);
	mLeftHipPartObject = leftHipPartObject;
	BoxGeom *leftHipPartBox = dynamic_cast<BoxGeom*>(leftHipPartObject->getGeometry());
	leftHipPartBox->setAutomaticColor(false);
	leftHipPartObject->setMass(dynamixelMass);
	leftHipPartObject->setMaterialType("PlasticsWhite");
	leftHipPartObject->setTextureType("Dynamixel");
	leftHipPartBox->setLocalPosition(Vector3D(-hipSideX, hipSideY, hipSideZ));
	leftHipPartBox->setColor(*dynamixelColor);
	invisibleHip->addCollisionObject(leftHipPartObject);
	
	CollisionObject *hipPlateObject= new CollisionObject(BoxGeom(invisibleHip, 0.1, 
		0.005, 0.052),invisibleHip,true);
	mHipPlateObject = hipPlateObject;
	BoxGeom *hipPlateBox = dynamic_cast<BoxGeom*>(hipPlateObject->getGeometry());
	hipPlateBox->setAutomaticColor(false);
	hipPlateObject->setMass(hipPlateWeight);
	hipPlateObject->setMaterialType("PlasticsGray");
	hipPlateObject->setTextureType("Alu");
	hipPlateBox->setLocalPosition(Vector3D(hipPlateDiffX, -hipPlateDiffY - space 
		- middleHipDiffY, -hipPlateDiffZ - middleHipDiffZ));
	hipPlateBox->setColor(*absColor);
	invisibleHip->addCollisionObject(hipPlateObject);

	CollisionObject *accuCaseObject= new CollisionObject(BoxGeom(invisibleHip, 
		0.0295*2.0 + 0.038, 0.0075, 0.0533),invisibleHip,true);
	mAccuCaseObject = accuCaseObject;
	BoxGeom *accuCaseBox = dynamic_cast<BoxGeom*>(accuCaseObject->getGeometry());
	accuCaseBox->setAutomaticColor(false);
	accuCaseObject->setMass(accuCaseWeight * artificialMassScale + accuWeight);
	accuCaseObject->setMaterialType("ABS");
	accuCaseBox->setLocalPosition(Vector3D(accuCaseDiffX, 
										   -accuCaseDiffY + space - middleHipDiffY, 
										   -accuCaseDiffZ - middleHipDiffZ));
	accuCaseBox->setColor(*absColor);
	invisibleHip->addCollisionObject(accuCaseObject);

	body = boxBody->createCopy();
	mMiddlePda = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Middle/PDA"));
	PARAM(DoubleValue, body, "Mass")->set(pdaWeight);
	PARAM(Vector3DValue, body, "Position")->set(0.0, pdaPosY, pdaPosZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("ABS");
	PARAM(DoubleValue, body, "Width")->set((0.078 * sizeScale));
	PARAM(DoubleValue, body, "Height")->set(0.104 * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(0.0203 * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*absColor);
	//pm->addSimObject(body);
	SimBody *pda = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mMiddleUpperBody = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Middle/UpperBody"));
	PARAM(DoubleValue, body, "Mass")->set(mainBodyWeight + mainBoardWeight);
	PARAM(Vector3DValue, body, "Position")->set(0.0, upperBodyPosY, upperBodyPosZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsGray");
	PARAM(DoubleValue, body, "Width")->set((0.1047 * sizeScale));
	PARAM(DoubleValue, body, "Height")->set(0.063 * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(0.0656 * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);

//remove
// 	Vector3DValue *threeDValue = PARAM(Vector3DValue, body, "Position");
// 	if(threeDValue != 0) {
// 		DoubleValue *value = dynamic_cast<DoubleValue*>(threeDValue->getValuePart(1)); 
// 		if(value != 0) {
// 			Core::getInstance()->getValueManager()->addValue("UpperBody/Height", value);
// 		}
// 	}
	//pm->addSimObject(body);
	SimBody *upperBody = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mMiddleNeck = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Middle/Neck"));
	PARAM(DoubleValue, body, "Mass")->set(neckWeight);
	PARAM(Vector3DValue, body, "Position")->set(0.0, neckPosY, neckPosZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(StringValue, body, "Texture")->set("Dynamixel");
	PARAM(DoubleValue, body, "Width")->set((0.032 * sizeScale));
	PARAM(DoubleValue, body, "Height")->set(0.037 * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(0.05 * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	
	//pm->addSimObject(body);
	SimBody *neck = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mMiddleHeadBase = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Middle/HeadBase"));
	PARAM(DoubleValue, body, "Mass")->set(headBaseWeight);
	PARAM(Vector3DValue, body, "Position")->set(0.0, headBasePosY, headBasePosZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(StringValue, body, "Texture")->set("Dynamixel");
	PARAM(DoubleValue, body, "Width")->set((0.038 * sizeScale));
	PARAM(DoubleValue, body, "Height")->set(0.0348 * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(0.05 * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	
	//pm->addSimObject(body);
	SimBody *headBase = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mMiddleHead = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Middle/Head"));
	PARAM(DoubleValue, body, "Mass")->set(headWeight);
	PARAM(Vector3DValue, body, "Position")->set(0.0, headBasePosY , headBasePosZ + 0.025 
			- 0.0115 - 0.0115 + 0.074/2.0);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("ABS");
	PARAM(DoubleValue, body, "Width")->set((0.06 * sizeScale));
	PARAM(DoubleValue, body, "Height")->set(0.0422 * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(0.074 * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	
	//pm->addSimObject(body);
	SimBody *head = dynamic_cast<SimBody*>(body);
	agent->addObject(body);


	// Right Arm
	body = boxBody->createCopy();
	mRightShoulder = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Right/Shoulder"));
	PARAM(DoubleValue, body, "Mass")->set(shoulderWeight);
	PARAM(Vector3DValue, body, "Position")->set(-shoulderPosX, 
												shoulderPosY, 
												shoulderPosZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(DoubleValue, body, "Width")->set((shoulderW * sizeScale));
	PARAM(DoubleValue, body, "Height")->set(shoulderH * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(shoulderD * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*absColor);
	
	//pm->addSimObject(body);
	SimBody *rightShoulder = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mRightUpperArm = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Right/UpperArm"));
	PARAM(DoubleValue, body, "Mass")->set(upperArmWeight);
	PARAM(Vector3DValue, body, "Position")->set(-upperArmPosX, 
												upperArmPosY, 
												upperArmPosZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(StringValue, body, "Texture")->set("Dynamixel");
	PARAM(DoubleValue, body, "Width")->set((upperArmW * sizeScale));
	PARAM(DoubleValue, body, "Height")->set(upperArmH * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(upperArmD * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	
	//pm->addSimObject(body);
	SimBody *rightUpperArm = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mRightLowerArm = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Right/LowerArm"));
	PARAM(DoubleValue, body, "Mass")->set(lowerArmWeight);
	PARAM(Vector3DValue, body, "Position")->set(-1*lowerArmPosX, 
												upperArmPosY, 
												upperArmPosZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(StringValue, body, "Texture")->set("Dynamixel");
	PARAM(DoubleValue, body, "Width")->set((lowerArmW * sizeScale));
	PARAM(DoubleValue, body, "Height")->set(lowerArmH * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(lowerArmD * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	
	//pm->addSimObject(body);
	SimBody *rightLowerArm = dynamic_cast<SimBody*>(body);
	agent->addObject(body);


	CollisionObject *rightHandObject = new CollisionObject(BoxGeom(rightLowerArm, 0.057, 
			0.0321, 0.01),rightLowerArm,true);
	mRightHandObject = rightHandObject;
	BoxGeom *rightHandBox = dynamic_cast<BoxGeom*>(rightHandObject->getGeometry());
	rightHandBox->setAutomaticColor(false);
	rightHandObject->setMass(handWeight);
	rightHandObject->setMaterialType("PlasticsGray");
	rightHandBox->setLocalPosition(Vector3D(-1*(0.0565/2.0 + 0.057/2.0 + 0.0005),0.0, 
			-0.038/2.0 + 0.01/2.0));
	rightHandBox->setColor(*absColor);
	rightLowerArm->addCollisionObject(rightHandObject);
	
	// Left Arm
	// 	Here is an additional scaling of the weight with 10!!!
	body = boxBody->createCopy();
	mLeftShoulder = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Left/Shoulder"));
	PARAM(DoubleValue, body, "Mass")->set(shoulderWeight);
	PARAM(Vector3DValue, body, "Position")->set(shoulderPosX, 
												shoulderPosY, 
												shoulderPosZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(DoubleValue, body, "Width")->set((shoulderW));
	PARAM(DoubleValue, body, "Height")->set(shoulderH);
	PARAM(DoubleValue, body, "Depth")->set(shoulderD);
	PARAM(ColorValue, body, "Color")->set(*absColor);
	
	//pm->addSimObject(body);
	SimBody *leftShoulder = dynamic_cast<SimBody*>(body);
	agent->addObject(body);


	body = boxBody->createCopy();
	mLeftUpperArm = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Left/UpperArm"));
	PARAM(DoubleValue, body, "Mass")->set(upperArmWeight);
	PARAM(Vector3DValue, body, "Position")->set(upperArmPosX, 
												upperArmPosY, 
												upperArmPosZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(StringValue, body, "Texture")->set("Dynamixel");
	PARAM(DoubleValue, body, "Width")->set((upperArmW * sizeScale));
	PARAM(DoubleValue, body, "Height")->set(upperArmH * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(upperArmD * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);

	//pm->addSimObject(body);
	SimBody *leftUpperArm = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	body = boxBody->createCopy();
	mLeftLowerArm = body;
	PARAM(StringValue, body, "Name")->set(mNamePrefix + ("Left/LowerArm"));
	PARAM(DoubleValue, body, "Mass")->set(lowerArmWeight);
	PARAM(Vector3DValue, body, "Position")->set(lowerArmPosX, 
												upperArmPosY, 
												upperArmPosZ);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(StringValue, body, "Material")->set("PlasticsWhite");
	PARAM(StringValue, body, "Texture")->set("Dynamixel");
	PARAM(DoubleValue, body, "Width")->set((lowerArmW * sizeScale));
	PARAM(DoubleValue, body, "Height")->set(lowerArmH * sizeScale);
	PARAM(DoubleValue, body, "Depth")->set(lowerArmD * sizeScale);
	PARAM(ColorValue, body, "Color")->set(*dynamixelColor);
	

	//pm->addSimObject(body);
	SimBody *leftLowerArm = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	CollisionObject *handObject = new CollisionObject(BoxGeom(leftLowerArm, 0.057, 0.0321,
			 0.01),leftLowerArm,true);
	mLeftHandObject = handObject;
	BoxGeom *box = dynamic_cast<BoxGeom*>(handObject->getGeometry());
	box->setAutomaticColor(false);
	handObject->setMass(handWeight);
	handObject->setMaterialType("PlasticsGray");
	box->setLocalPosition(Vector3D(0.0565/2.0 + 0.057/2.0 + 0.0005,
								   0.0, 
								   - 0.038/2.0 + 0.01/2.0));
	box->setColor(*absColor);
	leftLowerArm->addCollisionObject(handObject);
	

	QString groupPrefix = "/";
	groupPrefix.append(mGroupName).append("/");

	SimObject *pdaConnector = fixed->createCopy();
	PARAM(StringValue, pdaConnector, "Name")->set(mNamePrefix + ("Middle/PDAConnector"));
	PARAM(StringValue, pdaConnector, "FirstBody")->set(groupPrefix + "Middle/InvisiblePart");
	PARAM(StringValue, pdaConnector, "SecondBody")->set(groupPrefix + "Middle/PDA");
	//pm->addSimObject(pdaConnector);
	agent->addObject(pdaConnector);

	SimObject *neckConnector = fixed->createCopy();
	PARAM(StringValue, neckConnector, "Name")->set(mNamePrefix + ("Middle/NeckConnector"));
	PARAM(StringValue, neckConnector, "FirstBody")->set(groupPrefix + "Middle/UpperBody");
	PARAM(StringValue, neckConnector, "SecondBody")->set(groupPrefix + "Middle/Neck");
	//pm->addSimObject(neckConnector);
	agent->addObject(neckConnector);

	// Motors:

	// 1: Hip
	SimObject *hipMotor = dynamixel->createCopy();
	mHipMotor = hipMotor;
	PARAM(StringValue, hipMotor, "Name")->set(mNamePrefix + ("Waist"));
	PARAM(StringValue, hipMotor, "FirstBody")->set(groupPrefix + "Middle/InvisiblePart");
	PARAM(StringValue, hipMotor, "SecondBody")->set(groupPrefix + ("Middle/UpperBody"));
	PARAM(Vector3DValue, hipMotor, "AxisPoint1")->set(0.0, hipPosY, hipPosZ + 0.034 + 0.025 - 0.0115);
	PARAM(Vector3DValue, hipMotor, "AxisPoint2")->set(-0.1, hipPosY, hipPosZ + 0.034 + 0.025 - 0.0115);
	PARAM(DoubleValue, hipMotor, "MinAngle")->set(0);
	PARAM(DoubleValue, hipMotor, "MaxAngle")->set(150);
	//pm->addSimObject(hipMotor);
	agent->addObject(hipMotor);

	// 2: Right Shoulder Yaw
	SimObject *rightShoulderMotorRoll = dynamixel->createCopy();
	mRightShoulderMotorRoll = rightShoulderMotorRoll;
	PARAM(StringValue, rightShoulderMotorRoll, "Name")
		->set(mNamePrefix + ("Right/ShoulderYaw"));
	PARAM(StringValue, rightShoulderMotorRoll, "FirstBody")
		->set(groupPrefix + "Middle/UpperBody");
	PARAM(StringValue, rightShoulderMotorRoll, "SecondBody")
		->set(groupPrefix + "Right/Shoulder");
	PARAM(Vector3DValue, rightShoulderMotorRoll, "AxisPoint1")->set(shoulderPosX, shoulderPosY, shoulderPosZ);
	PARAM(Vector3DValue, rightShoulderMotorRoll, "AxisPoint2")->set(shoulderPosX + 0.1, shoulderPosY, shoulderPosZ);
	PARAM(DoubleValue, rightShoulderMotorRoll, "MinAngle")->set(-150);
	PARAM(DoubleValue, rightShoulderMotorRoll, "MaxAngle")->set(150);
	//pm->addSimObject(rightShoulderMotorRoll);
	agent->addObject(rightShoulderMotorRoll);

// 	//TODO Remove later: LeftSholuderYar'w.(replaces 3 and 5.
// 	SimObject *universalJointAdapterPrototype = 
// 		pm->getPrototype(SimulationConstants::PROTOTYPE_UNIVERSAL_JOINT_MSERIES_MOTOR_ADAPTER);
// 	shoulderPosX = (0.1047/2.0 + 0.0035 + 0.001);
// 	shoulderPosY = upperBodyPosY + 0.063/2.0 - 0.005 - 0.0124;
// 	shoulderPosZ = upperBodyPosZ - 0.004;
// 	double shoulder2PosX = (0.1047/2.0 + 0.0035 + 0.001);
// 	double shoulder2PosY = upperArmPosY;
// 	double shoulder2PosZ = upperArmPosZ;
// 
// 	SimObject *leftShoulderMotor = universalJointAdapterPrototype->createCopy();
// 	leftShoulderMotor->setName(mNamePrefix + ("Left/ShoulderMotor"));
// 
// 	PARAM(StringValue, leftShoulderMotor, "FirstBody")->set(groupPrefix + "Middle/UpperBody");
// 	PARAM(StringValue, leftShoulderMotor, "SecondBody")->set(groupPrefix + "Left/Shoulder");
// 	
// 	PARAM(Vector3DValue, leftShoulderMotor, "JointAxis1Point1")
// 			->set(shoulderPosX, shoulderPosY, shoulderPosZ);
// 	PARAM(Vector3DValue, leftShoulderMotor, "JointAxis1Point2")
// 			->set(shoulderPosX - 0.1, shoulderPosY, shoulderPosZ);
// 	PARAM(Vector3DValue, leftShoulderMotor, "JointAxis2Point1")
// 			->set(shoulder2PosX, shoulder2PosY, shoulder2PosZ);
// 	PARAM(Vector3DValue, leftShoulderMotor, "JointAxis2Point2")
// 			->set(shoulder2PosX, shoulder2PosY + 0.1, shoulder2PosZ);
// 	PARAM(Vector3DValue, leftShoulderMotor, "JointAnchor")
// 			->set(shoulder2PosX, shoulderPosY, shoulderPosZ);
// 
// 	PARAM(DoubleValue, leftShoulderMotor, "Axis1MinAngle")->set(-150);
// 	PARAM(DoubleValue, leftShoulderMotor, "Axis1MaxAngle")->set(150);
// 	PARAM(DoubleValue, leftShoulderMotor, "Axis2MinAngle")->set(-150);
// 	PARAM(DoubleValue, leftShoulderMotor, "Axis2MaxAngle")->set(-150);
// 	agent->addObject(leftShoulderMotor);


	// 3: left Shoulder Yaw
	SimObject *leftShoulderMotorRoll = dynamixel->createCopy();
	mLeftShoulderMotorRoll = leftShoulderMotorRoll;
	leftShoulderMotorRoll->setName(mNamePrefix + ("Left/ShoulderYaw"));
	PARAM(StringValue, leftShoulderMotorRoll, "FirstBody")->set(groupPrefix + "Middle/UpperBody");
	PARAM(StringValue, leftShoulderMotorRoll, "SecondBody")->set(groupPrefix + "Left/Shoulder");
	PARAM(Vector3DValue, leftShoulderMotorRoll, "AxisPoint1")->set(-shoulderPosX, shoulderPosY, shoulderPosZ);
	PARAM(Vector3DValue, leftShoulderMotorRoll, "AxisPoint2")->set(-shoulderPosX - 0.1, shoulderPosY, shoulderPosZ);
	PARAM(DoubleValue, leftShoulderMotorRoll, "MinAngle")->set(-150);
	PARAM(DoubleValue, leftShoulderMotorRoll, "MaxAngle")->set(150);
	//pm->addSimObject(leftShoulderMotorRoll);
	agent->addObject(leftShoulderMotorRoll);


	// 4: Right shoulder pitch
	SimObject *rightShoulderMotor= dynamixel->createCopy();
	mRightShoulderMotor = rightShoulderMotor;
	PARAM(StringValue, rightShoulderMotor, "Name")->set(mNamePrefix + ("Right/ShoulderPitch"));
	PARAM(StringValue, rightShoulderMotor, "FirstBody")->set(groupPrefix + "Right/Shoulder");
	PARAM(StringValue, rightShoulderMotor, "SecondBody")->set(groupPrefix + "Right/UpperArm");
	PARAM(Vector3DValue, rightShoulderMotor, "AxisPoint1")->set(-1*(upperArmPosX - 0.0609/2.0 + 0.0115), upperArmPosY, upperArmPosZ);
	PARAM(Vector3DValue, rightShoulderMotor, "AxisPoint2")->set(-1*(upperArmPosX - 0.0609/2.0 + 0.0115), upperArmPosY + 0.1, upperArmPosZ);
	PARAM(DoubleValue, rightShoulderMotor, "MinAngle")->set(-106.5);
	PARAM(DoubleValue, rightShoulderMotor, "MaxAngle")->set(94.5);
	//pm->addSimObject(rightShoulderMotor);
	agent->addObject(rightShoulderMotor);
	
	// 5: left shoulder pitch
	SimObject *leftShoulderMotor= dynamixel->createCopy();
	mLeftShoulderMotor = leftShoulderMotor;
	PARAM(StringValue, leftShoulderMotor, "Name")->set(mNamePrefix + ("Left/ShoulderPitch"));
	PARAM(StringValue, leftShoulderMotor, "FirstBody")->set(groupPrefix + "Left/Shoulder");
	PARAM(StringValue, leftShoulderMotor, "SecondBody")->set(groupPrefix + "Left/UpperArm");
	PARAM(Vector3DValue, leftShoulderMotor, "AxisPoint1")->set(1*(upperArmPosX - 0.0609/2.0 + 0.0115), upperArmPosY, upperArmPosZ);
	PARAM(Vector3DValue, leftShoulderMotor, "AxisPoint2")->set(1*(upperArmPosX - 0.0609/2.0 + 0.0115), upperArmPosY + 0.1, upperArmPosZ);
	PARAM(DoubleValue, leftShoulderMotor, "MinAngle")->set(-94.5);
	PARAM(DoubleValue, leftShoulderMotor, "MaxAngle")->set(106.5);
	//pm->addSimObject(leftShoulderMotor);
	agent->addObject(leftShoulderMotor);

	// 6:right ellbow
	SimObject *rightElbowMotor = dynamixel->createCopy();
	mRightElbowMotor = rightElbowMotor;
	PARAM(StringValue, rightElbowMotor, "Name")->set(mNamePrefix + ("Right/Elbow"));
	PARAM(StringValue, rightElbowMotor, "FirstBody")->set(groupPrefix + "Right/LowerArm");
	PARAM(StringValue, rightElbowMotor, "SecondBody")->set(groupPrefix + "Right/UpperArm");
	PARAM(Vector3DValue, rightElbowMotor, "AxisPoint1")->set(-1*(lowerArmPosX - 0.0565/2.0 + 0.0115), upperArmPosY , upperArmPosZ);
	PARAM(Vector3DValue, rightElbowMotor, "AxisPoint2")->set(-1*(lowerArmPosX - 0.0565/2.0 + 0.0115), upperArmPosY - 0.1, upperArmPosZ);
	PARAM(DoubleValue, rightElbowMotor, "MinAngle")->set(-106.5);
	PARAM(DoubleValue, rightElbowMotor, "MaxAngle")->set(87.0);
	//pm->addSimObject(rightElbowMotor);
	agent->addObject(rightElbowMotor);

	// 7: left ellbow
	SimObject *leftElbowMotor = dynamixel->createCopy();
	mLeftElbowMotor = leftElbowMotor;
	PARAM(StringValue, leftElbowMotor, "Name")->set(mNamePrefix + ("Left/Elbow"));
	PARAM(StringValue, leftElbowMotor, "FirstBody")->set(groupPrefix + "Left/LowerArm");
	PARAM(StringValue, leftElbowMotor, "SecondBody")->set(groupPrefix + "Left/UpperArm");
	PARAM(Vector3DValue, leftElbowMotor, "AxisPoint1")->set(1*(lowerArmPosX - 0.0565/2.0 + 0.0115), upperArmPosY, upperArmPosZ);
	PARAM(Vector3DValue, leftElbowMotor, "AxisPoint2")->set(1*(lowerArmPosX - 0.0565/2.0 + 0.0115), upperArmPosY - 0.1, upperArmPosZ);
	PARAM(DoubleValue, leftElbowMotor, "MinAngle")->set(-87.0);
	PARAM(DoubleValue, leftElbowMotor, "MaxAngle")->set(106.5);
	//pm->addSimObject(leftElbowMotor);
	agent->addObject(leftElbowMotor);

	// 8:right hip roll
	SimObject *rightHipRoll = dynamixel->createCopy();
	mRightHipRoll = rightHipRoll;
	PARAM(StringValue, rightHipRoll, "Name")->set(mNamePrefix + ("Right/HipRoll"));
	PARAM(StringValue, rightHipRoll, "FirstBody")->set(groupPrefix + "Right/HipPart");
	PARAM(StringValue, rightHipRoll, "SecondBody")->set(groupPrefix + "Middle/InvisiblePart");
	PARAM(Vector3DValue, rightHipRoll, "AxisPoint1")->set(-legOffset, hipPlatePosY, anklePosZ);
	PARAM(Vector3DValue, rightHipRoll, "AxisPoint2")->set(-legOffset, hipPlatePosY - 0.0115, anklePosZ);
	PARAM(DoubleValue, rightHipRoll, "MinAngle")->set(-45);
	PARAM(DoubleValue, rightHipRoll, "MaxAngle")->set(150);
	//pm->addSimObject(rightHipRoll);
	agent->addObject(rightHipRoll);

	// 9: left hip roll
	SimObject *leftHipRoll = dynamixel->createCopy();
	mLeftHipRoll = leftHipRoll;
	PARAM(StringValue, leftHipRoll, "Name")->set(mNamePrefix + ("Left/HipRoll"));
	PARAM(StringValue, leftHipRoll, "FirstBody")->set(groupPrefix + "Left/HipPart");
	PARAM(StringValue, leftHipRoll, "SecondBody")->set(groupPrefix + "Middle/InvisiblePart");
	PARAM(Vector3DValue, leftHipRoll, "AxisPoint1")->set(legOffset, hipPlatePosY, anklePosZ);
	PARAM(Vector3DValue, leftHipRoll, "AxisPoint2")->set(legOffset, hipPlatePosY - 0.0115, anklePosZ);
	PARAM(DoubleValue, leftHipRoll, "MinAngle")->set(-150);
	PARAM(DoubleValue, leftHipRoll, "MaxAngle")->set(45);
	//pm->addSimObject(leftHipRoll);
	agent->addObject(leftHipRoll);

	// 10:right hip pitch
	SimObject *rightHipSide = dynamixel->createCopy();
	mRightHipSide = rightHipSide;
	PARAM(StringValue, rightHipSide, "Name")->set(mNamePrefix + ("Right/HipPitch"));
	PARAM(StringValue, rightHipSide, "FirstBody")->set(groupPrefix + "Right/UpperFemur");
	PARAM(StringValue, rightHipSide, "SecondBody")->set(groupPrefix + "Right/HipPart");
	PARAM(Vector3DValue, rightHipSide, "AxisPoint1")->set(-legOffset, upperFemurPosY + 0.025 - 0.0115, anklePosZ);
	PARAM(Vector3DValue, rightHipSide, "AxisPoint2")->set(-legOffset, upperFemurPosY + 0.025 - 0.0115, anklePosZ + 0.1);
	PARAM(DoubleValue, rightHipSide, "MinAngle")->set(-75);
	PARAM(DoubleValue, rightHipSide, "MaxAngle")->set(40);
	//pm->addSimObject(rightHipSide);
	agent->addObject(rightHipSide);

	// 11:left hip pitch
	SimObject *leftHipSide = dynamixel->createCopy();
	mLeftHipSide = leftHipSide;
	PARAM(StringValue, leftHipSide, "Name")->set(mNamePrefix + ("Left/HipPitch"));
	PARAM(StringValue, leftHipSide, "FirstBody")->set(groupPrefix + "Left/UpperFemur");
	PARAM(StringValue, leftHipSide, "SecondBody")->set(groupPrefix + "Left/HipPart");
	PARAM(Vector3DValue, leftHipSide, "AxisPoint1")->set(legOffset, upperFemurPosY + 0.025 - 0.0115, anklePosZ);
	PARAM(Vector3DValue, leftHipSide, "AxisPoint2")->set(legOffset, upperFemurPosY + 0.025 - 0.0115, anklePosZ + 0.1);
	PARAM(DoubleValue, leftHipSide, "MinAngle")->set(-40);
	PARAM(DoubleValue, leftHipSide, "MaxAngle")->set(75);
	//pm->addSimObject(leftHipSide);
	agent->addObject(leftHipSide);

	// 12:right hip yaw
	SimObject *rightHipFB = dynamixel->createCopy();
	mRightHipFB = rightHipFB;
	PARAM(StringValue, rightHipFB, "Name")->set(mNamePrefix + ("Right/HipYaw"));
	PARAM(StringValue, rightHipFB, "FirstBody")->set(groupPrefix + "Right/Femur");
	PARAM(StringValue, rightHipFB, "SecondBody")->set(groupPrefix + "Right/UpperFemur");
	PARAM(Vector3DValue, rightHipFB, "AxisPoint1")->set(0, upperFemurPosY + dynamixelDepth/2.0 - 0.0115 , legOffsetZ);
	PARAM(Vector3DValue, rightHipFB, "AxisPoint2")->set(-0.01, upperFemurPosY + dynamixelDepth/2.0 - 0.0115, legOffsetZ);
	PARAM(DoubleValue, rightHipFB, "MinAngle")->set(-15);
	PARAM(DoubleValue, rightHipFB, "MaxAngle")->set(150);
	//pm->addSimObject(rightHipFB);
	agent->addObject(rightHipFB);

	// 13:left hip yaw
	SimObject *leftHipFB = dynamixel->createCopy();
	mLeftHipFB = leftHipFB;
	PARAM(StringValue, leftHipFB, "Name")->set(mNamePrefix + ("Left/HipYaw"));
	PARAM(StringValue, leftHipFB, "FirstBody")->set(groupPrefix + "Left/Femur");
	PARAM(StringValue, leftHipFB, "SecondBody")->set(groupPrefix + "Left/UpperFemur");
	PARAM(Vector3DValue, leftHipFB, "AxisPoint1")->set(0, upperFemurPosY + dynamixelDepth/2.0 - 0.0115, legOffsetZ);
	PARAM(Vector3DValue, leftHipFB, "AxisPoint2")->set(0.01, upperFemurPosY + dynamixelDepth/2.0 - 0.0115, legOffsetZ);
	PARAM(DoubleValue, leftHipFB, "MinAngle")->set(-150);
	PARAM(DoubleValue, leftHipFB, "MaxAngle")->set(15);
	//pm->addSimObject(leftHipFB);
	agent->addObject(leftHipFB);

	// 14:right knee
	SimObject *rightKnee = dynamixel->createCopy();
	mRightKnee = rightKnee;
	PARAM(StringValue, rightKnee, "Name")->set(mNamePrefix + ("Right/Knee"));
	PARAM(StringValue, rightKnee, "FirstBody")->set(groupPrefix + "Right/Tibia");
	PARAM(StringValue, rightKnee, "SecondBody")->set(groupPrefix + "Right/Femur");
	PARAM(Vector3DValue, rightKnee, "AxisPoint1")->set(-legOffset, tibiaPosY + tibiaHeight/2.0 - 0.016, legOffsetZ - 0.0135 * sizeScale);
	PARAM(Vector3DValue, rightKnee, "AxisPoint2")->set(-legOffset + 0.01, tibiaPosY + tibiaHeight/2.0 - 0.016, legOffsetZ - 0.0135 * sizeScale);
	PARAM(DoubleValue, rightKnee, "MinAngle")->set(0);
	PARAM(DoubleValue, rightKnee, "MaxAngle")->set(150);
	//pm->addSimObject(rightKnee);
	agent->addObject(rightKnee);

	//15: left knee
	SimObject *leftKnee = dynamixel->createCopy();
	mLeftKnee = leftKnee;
	PARAM(StringValue, leftKnee, "Name")->set(mNamePrefix + ("Left/Knee"));
	PARAM(StringValue, leftKnee, "FirstBody")->set(groupPrefix + "Left/Tibia");
	PARAM(StringValue, leftKnee, "SecondBody")->set(groupPrefix + "Left/Femur");
	PARAM(Vector3DValue, leftKnee, "AxisPoint1")->set(legOffset, tibiaPosY + tibiaHeight/2.0 - 0.016,legOffsetZ - 0.0135 * sizeScale);
	PARAM(Vector3DValue, leftKnee, "AxisPoint2")->set(legOffset - 0.01, tibiaPosY + tibiaHeight/2.0 - 0.016, legOffsetZ - 0.0135 * sizeScale);
	PARAM(DoubleValue, leftKnee, "MinAngle")->set(-150);
	PARAM(DoubleValue, leftKnee, "MaxAngle")->set(0);
	//pm->addSimObject(leftKnee);
	agent->addObject(leftKnee);

	// 16:right ankle yaw
	SimObject *rightFootUD = dynamixel->createCopy();
	mRightFootUD = rightFootUD;
	PARAM(StringValue, rightFootUD, "Name")->set(mNamePrefix + ("Right/AnkleYaw"));
	PARAM(StringValue, rightFootUD, "FirstBody")->set(groupPrefix + "Right/AnkleBox");
	PARAM(StringValue, rightFootUD, "SecondBody")->set(groupPrefix + "Right/Tibia");
	PARAM(Vector3DValue, rightFootUD, "AxisPoint1")->set(0, height * sizeScale, legOffsetZ);
	PARAM(Vector3DValue, rightFootUD, "AxisPoint2")->set(0.01, height * sizeScale, legOffsetZ);
	PARAM(DoubleValue, rightFootUD, "MinAngle")->set(-81);
	PARAM(DoubleValue, rightFootUD, "MaxAngle")->set(29.5);
	//pm->addSimObject(rightFootUD);
	agent->addObject(rightFootUD);

	// 17:left ankle yaw
	SimObject *leftFootUD = dynamixel->createCopy();
	mLeftFootUD = leftFootUD;
	PARAM(StringValue, leftFootUD, "Name")->set(mNamePrefix + ("Left/AnkleYaw"));
	PARAM(StringValue, leftFootUD, "FirstBody")->set(groupPrefix + "Left/AnkleBox");
	PARAM(StringValue, leftFootUD, "SecondBody")->set(groupPrefix + "Left/Tibia");
	PARAM(Vector3DValue, leftFootUD, "AxisPoint1")->set(0, height * sizeScale, legOffsetZ);
	PARAM(Vector3DValue, leftFootUD, "AxisPoint2")->set(-0.01, height * sizeScale, legOffsetZ);
	PARAM(DoubleValue, leftFootUD, "MinAngle")->set(-29.5);
	PARAM(DoubleValue, leftFootUD, "MaxAngle")->set(81.0);
	//pm->addSimObject(leftFootUD);
	agent->addObject(leftFootUD);

	// 18: right ankle pitch
	SimObject *rightFootLR = dynamixel->createCopy();
	mRightFootLR = rightFootLR;
	PARAM(StringValue, rightFootLR, "Name")->set(mNamePrefix + ("Right/AnklePitch"));
	PARAM(StringValue, rightFootLR, "FirstBody")->set(groupPrefix + "Right/Foot");
	PARAM(StringValue, rightFootLR, "SecondBody")->set(groupPrefix + "Right/AnkleBox");
	PARAM(Vector3DValue, rightFootLR, "AxisPoint1")->set(-legOffset, height * sizeScale, -0.011 * sizeScale);
	PARAM(Vector3DValue, rightFootLR, "AxisPoint2")->set(-legOffset, height * sizeScale, -0.036 * sizeScale - 0.011 * sizeScale);
	PARAM(DoubleValue, rightFootLR, "MinAngle")->set(-66.5);
	PARAM(DoubleValue, rightFootLR, "MaxAngle")->set(55.0);

	//pm->addSimObject(rightFootLR);
	agent->addObject(rightFootLR);

	// 19: left ankle pitch
	SimObject *leftFootLR = dynamixel->createCopy();
	mLeftFootLR = leftFootLR;
	PARAM(StringValue, leftFootLR, "Name")->set(mNamePrefix + ("Left/AnklePitch"));
	PARAM(StringValue, leftFootLR, "FirstBody")->set(groupPrefix + "Left/Foot");
	PARAM(StringValue, leftFootLR, "SecondBody")->set(groupPrefix + "Left/AnkleBox");
	PARAM(Vector3DValue, leftFootLR, "AxisPoint1")->set(legOffset, height * sizeScale, -0.011 * sizeScale);
	PARAM(Vector3DValue, leftFootLR, "AxisPoint2")->set(legOffset, height * sizeScale, -0.036 * sizeScale - 0.011 * sizeScale);
	PARAM(DoubleValue, leftFootLR, "MinAngle")->set(-55.0);
	PARAM(DoubleValue, leftFootLR, "MaxAngle")->set(66.5);
	//pm->addSimObject(leftFootLR);
	agent->addObject(leftFootLR);

	// 20: head pan
	SimObject *headSide = dynamixel->createCopy();
	mHeadSide = headSide;
	PARAM(StringValue, headSide, "Name")->set(mNamePrefix + ("Head/Pan"));
	PARAM(StringValue, headSide, "FirstBody")->set(groupPrefix + "Middle/Neck");
	PARAM(StringValue, headSide, "SecondBody")->set(groupPrefix + "Middle/HeadBase");
	PARAM(Vector3DValue, headSide, "AxisPoint1")->set(0.0, neckPosY + 0.0348/2.0 + 0.001, neckPosZ + 0.025 - 0.0115);
	PARAM(Vector3DValue, headSide, "AxisPoint2")->set(0.0, neckPosY + 0.0348/2.0 + 0.001 - 0.1, neckPosZ + 0.025 - 0.0115);
	PARAM(DoubleValue, headSide, "MinAngle")->set(-135);
	PARAM(DoubleValue, headSide, "MaxAngle")->set(135);
	//pm->addSimObject(headSide);
	agent->addObject(headSide);

	// 21: head tilt
	SimObject *headUp = dynamixel->createCopy();
	mHeadUp = headUp;
	PARAM(StringValue, headUp, "Name")->set(mNamePrefix + ("Head/Tilt"));
	PARAM(StringValue, headUp, "FirstBody")->set(groupPrefix + "Middle/Head");
	PARAM(StringValue, headUp, "SecondBody")->set(groupPrefix + "Middle/HeadBase");
	PARAM(Vector3DValue, headUp, "AxisPoint1")->set(0.0, headBasePosY, headBasePosZ + 0.025 - 0.0115);
	PARAM(Vector3DValue, headUp, "AxisPoint2")->set(-0.1, headBasePosY, headBasePosZ + 0.025 - 0.0115);
	PARAM(DoubleValue, headUp, "MinAngle")->set(-95);
	PARAM(DoubleValue, headUp, "MaxAngle")->set(150);
	//pm->addSimObject(headUp);
	agent->addObject(headUp);


	QList<SimObject*> accelBoards;

	// Acceleration Sensors:
	SimObject *abfr = accelSensor->createCopy();
	mAbfr = abfr;
	abfr->setName(mNamePrefix + ("ABFR"));
	Vector3DValue *pos = dynamic_cast<Vector3DValue*>(abfr->getParameter("LocalPosition"));
	pos->set(accelOffsetFootX, accelOffsetFootY, accelOffsetFootZ);
	QuaternionValue *orientation = dynamic_cast<QuaternionValue*>(abfr->getParameter("LocalOrientation"));
	orientation->setFromAngles(0, 90,0);
	PARAM(StringValue, abfr, "ReferenceBody")->set(groupPrefix + "Right/Foot");
	accelBoards.append(abfr);
	//pm->addSimObject(abfr);
	agent->addObject(abfr);

	SimObject *abhr = accelSensor->createCopy();
	mAbhr = abhr;
	abhr->setName(mNamePrefix + ("ABHR"));
	pos = dynamic_cast<Vector3DValue*>(abhr->getParameter("LocalPosition"));
	pos->set(-0.0088 - accelBoardHeight/2.0 + diffX, diffY + 0.011, diffZ);
	orientation = dynamic_cast<QuaternionValue*>(abhr->getParameter("LocalOrientation"));
	orientation->setFromAngles(0, 0,90);
	PARAM(StringValue, abhr, "ReferenceBody")->set(groupPrefix + "Right/Femur");
	accelBoards.append(abhr);
	//pm->addSimObject(abhr);
	agent->addObject(abhr);

	SimObject *absr = accelSensor->createCopy();
	mAbsr = absr;
	absr->setName(mNamePrefix + ("ABSR"));
	pos = dynamic_cast<Vector3DValue*>(absr->getParameter("LocalPosition"));
	pos->set(-(0.1047/2.0 - 0.0064 - accelBoardDepth/2.0) , 0.063/2.0 + accelBoardHeight/2.0 + 0.0005, - 0.038/2.0 - 0.011 + 0.0656/2.0);
	orientation = dynamic_cast<QuaternionValue*>(absr->getParameter("LocalOrientation"));
	orientation->setFromAngles(0,0,0);
	PARAM(StringValue, absr, "ReferenceBody")->set(groupPrefix + "Middle/UpperBody");
	accelBoards.append(absr);
	//pm->addSimObject(absr);
	agent->addObject(absr);


	SimObject *abar = accelSensor->createCopy();
	mAbar = abar;
	abar->setName(mNamePrefix + ("ABAR"));
	pos = dynamic_cast<Vector3DValue*>(abar->getParameter("LocalPosition"));
	pos->set(-(0.0565/2.0 - 0.0065 - 0.026/2.0),0, - 0.032/2.0 - 0.0125/2.0 - 0.0055);
	orientation = dynamic_cast<QuaternionValue*>(abar->getParameter("LocalOrientation"));
	orientation->setFromAngles(-90,0,0);
	PARAM(StringValue, abar, "ReferenceBody")->set(groupPrefix + "Right/LowerArm");
	accelBoards.append(abar);
	//pm->addSimObject(abar);
	agent->addObject(abar);


	SimObject *abfl = accelSensor->createCopy();
	mAbfl = abfl;
	abfl->setName(mNamePrefix + ("ABFL"));
	pos = dynamic_cast<Vector3DValue*>(abfl->getParameter("LocalPosition"));	pos->set(-accelOffsetFootX, accelOffsetFootY, accelOffsetFootZ);
	orientation = dynamic_cast<QuaternionValue*>(abfl->getParameter("LocalOrientation"));
	orientation->setFromAngles(0, -90,0);
	PARAM(StringValue, abfl, "ReferenceBody")->set(groupPrefix + "Left/Foot");
	accelBoards.append(abfl);
	//pm->addSimObject(abfl);
	agent->addObject(abfl);


	SimObject *abhl = accelSensor->createCopy();
	mAbhl = abhl;
	abhl->setName(mNamePrefix + ("ABHL"));
	pos = dynamic_cast<Vector3DValue*>(abhl->getParameter("LocalPosition"));
		pos->set(0.0088 + accelBoardHeight/2.0 - diffX, diffY + 0.011, diffZ);
	orientation = dynamic_cast<QuaternionValue*>(abhl->getParameter("LocalOrientation"));
	orientation->setFromAngles(0,0,-90);
	PARAM(StringValue, abhl, "ReferenceBody")->set(groupPrefix + "Left/Femur");
	accelBoards.append(abhl);
	//pm->addSimObject(abhl);
	agent->addObject(abhl);


	SimObject *absl = accelSensor->createCopy();
	mAbsl = absl;
	absl->setName(mNamePrefix + ("ABML"));
	absl->setName(mNamePrefix + "ABML");
	pos = dynamic_cast<Vector3DValue*>(absl->getParameter("LocalPosition"));
	pos->set((0.05235 - 0.0064 - accelBoardDepth/2.0) , 0.063/2.0 + accelBoardHeight/2.0 + 0.0005, - 0.038/2.0 - 0.011 + 0.0656/2.0);
	orientation = dynamic_cast<QuaternionValue*>(absl->getParameter("LocalOrientation"));
	orientation->setFromAngles(0,0,0);
	PARAM(StringValue, absl, "ReferenceBody")->set(groupPrefix + "Middle/UpperBody");
	accelBoards.append(absl);
	//pm->addSimObject(absl);
	agent->addObject(absl);

	SimObject *abal = accelSensor->createCopy();
	mAbal = abal;
	abal->setName(mNamePrefix + ("ABAL"));
	pos = dynamic_cast<Vector3DValue*>(abal->getParameter("LocalPosition"));
	pos->set((0.0565/2.0 - 0.0065 - 0.026/2.0),0, - 0.032/2.0 - 0.0125/2.0 - 0.0055);
	orientation = dynamic_cast<QuaternionValue*>(abal->getParameter("LocalOrientation"));
	orientation->setFromAngles(-90,0,0);
	PARAM(StringValue, abal, "ReferenceBody")->set(groupPrefix + "Left/LowerArm");
	accelBoards.append(abal);
	//pm->addSimObject(abal);
	agent->addObject(abal);



	SimObject *testForceSensor = forceSensor->createCopy();
	agent->addObject(testForceSensor);



	//add agent SimObjectGroup at PhysicsManager to provide the interface to the robot.
	pm->addSimObjectGroup(agent);

	for(QListIterator<SimObject*> i(accelBoards); i.hasNext();) {
		i.next()->setup();
	}

	


	CollisionManager *collisionManager = Physics::getCollisionManager();

	collisionManager->disableCollisions(upperBody->getCollisionObjects(), 
		rightShoulder->getCollisionObjects(), true);

	collisionManager->disableCollisions(upperBody->getCollisionObjects(), 
		rightUpperArm->getCollisionObjects(), true);

	collisionManager->disableCollisions(upperBody->getCollisionObjects(), 
		leftUpperArm->getCollisionObjects(), true);
	
	collisionManager->disableCollisions(rightLowerArm->getCollisionObjects().at(2), 
		pda->getCollisionObjects(), true);

	collisionManager->disableCollisions(leftLowerArm->getCollisionObjects().at(2), 
		pda->getCollisionObjects(), true);

	collisionManager->disableCollisions(upperBody->getCollisionObjects(), 
		leftShoulder->getCollisionObjects(), true);

	collisionManager->disableCollisions(rightShoulder->getCollisionObjects(), 
		rightUpperArm->getCollisionObjects(), true);

	collisionManager->disableCollisions(leftShoulder->getCollisionObjects(), 
		leftUpperArm->getCollisionObjects(), true);

	collisionManager->disableCollisions(rightUpperArm->getCollisionObjects(), 
		rightLowerArm->getCollisionObjects(), true);

	collisionManager->disableCollisions(leftUpperArm->getCollisionObjects(), 
		leftLowerArm->getCollisionObjects(), true);

	collisionManager->disableCollisions(rightFoot->getCollisionObjects(), 
		rightFoot->getCollisionObjects(), true);

	collisionManager->disableCollisions(rightFoot->getCollisionObjects(), 
		rightAnkle->getCollisionObjects(), true);

	collisionManager->disableCollisions(rightAnkle->getCollisionObjects(), 
		rightTibia->getCollisionObjects(), true);

	collisionManager->disableCollisions(rightFemur->getCollisionObjects(), 
		rightUpperFemur->getCollisionObjects(), true);

	collisionManager->disableCollisions(rightTibia->getCollisionObjects(), 
		rightFemur->getCollisionObjects(), true);

	collisionManager->disableCollisions(leftFoot->getCollisionObjects(), 
		leftFoot->getCollisionObjects(), true);

	collisionManager->disableCollisions( leftAnkle->getCollisionObjects().at(0), 
		leftFoot->getCollisionObjects(), true);

	collisionManager->disableCollisions(leftAnkle->getCollisionObjects(), 
		leftTibia->getCollisionObjects(), true);

	collisionManager->disableCollisions(leftFemur->getCollisionObjects(), 
		leftUpperFemur->getCollisionObjects(), true);

	collisionManager->disableCollisions(leftTibia->getCollisionObjects(), 
		leftFemur->getCollisionObjects(), true);

	collisionManager->disableCollisions(invisibleHip->getCollisionObjects(), 	
		pda->getCollisionObjects(), true);

	collisionManager->disableCollisions(invisibleHip->getCollisionObjects(), 
		invisibleHip->getCollisionObjects(), true);

	collisionManager->disableCollisions(upperBody->getCollisionObjects(), 
		invisibleHip->getCollisionObjects(), true);

	collisionManager->disableCollisions(upperBody->getCollisionObjects(), 
		neck->getCollisionObjects(), true);

	collisionManager->disableCollisions(neck->getCollisionObjects(), 
		headBase->getCollisionObjects(), true);

	collisionManager->disableCollisions(headBase->getCollisionObjects(), 
		head->getCollisionObjects(), true);

	collisionManager->disableCollisions(head->getCollisionObjects(), 
		neck->getCollisionObjects(),true);

	collisionManager->disableCollisions(rightLowerArm->getCollisionObjects(), 
		rightLowerArm->getCollisionObjects(), true);

	collisionManager->disableCollisions(leftLowerArm->getCollisionObjects(), 
		leftLowerArm->getCollisionObjects(), true);

	collisionManager->disableCollisions(invisibleHip->getCollisionObjects().at(0), true);


	//Show weight of all registered components so far.
	QList<SimObject*> simObjects = pm->getSimObjects();
	double weight = 0.0;
	for(int i = 0; i < simObjects.size(); ++i) {
		SimBody *body = dynamic_cast<SimBody*>(simObjects.at(i));
		if(body != 0) {
			QList<CollisionObject*> collisionObjs = body->getCollisionObjects();
			weight += PARAM(DoubleValue, body, "Mass")->get();
			for(int j = 0; j < collisionObjs.size(); ++j) {
				weight += collisionObjs.at(j)->getMass();
			}
		}
	}
	double heightModification = footY - footH/2.0;
	QList <SimObject*> agentObjects = agent->getObjects();
	for(int i = 0; i < agentObjects.size(); i++) {
		SimBody *body = dynamic_cast<SimBody*>(agentObjects.at(i));
		if(body != 0) {
			Vector3DValue *pos = body->getPositionValue();
			pos->set(pos->getX(), pos->getY() + heightModification, pos->getZ());
		}
	}

	mSimObjects = agent->getObjects();
	//cerr << "Weight of all registered SimBodies: " << weight << endl;

}

SimObject* ASeriesMorph::createCopy() const {
	ASeriesMorph *copy = new ASeriesMorph("Default");
	return copy;
}

void ASeriesMorph::layoutObjects() {

	double artificialMassScale = 1.0;
	if(mUseDynamixels) {
		artificialMassScale = 10.0;
	}
	else {
		artificialMassScale = 1.0;
	}

	double accuWeight = 0.047 * artificialMassScale;
	if(!mIncludeAccus) {
		accuWeight = 0.0;
	}

	double weightDiff = 0.37294 * artificialMassScale;
	double totalScrewWeight = 0.1 * artificialMassScale;
	double cableWeight = weightDiff - totalScrewWeight;

	double screwNumber = 243.0;
	double screwWeight = totalScrewWeight/screwNumber;
	double cableLength = 529.0;
	double cablePartWeight = cableWeight/cableLength;

	double dynamixelMass = 0.055 * artificialMassScale;
	double smallPlateWeight = 0.01/3.0 * artificialMassScale;
	double headWeight = 0.03 * artificialMassScale;
	double handWeight = 0.010 * artificialMassScale;
	double pdaWeight = (0.181 + 0.02999) * artificialMassScale;
	double soleOfFootWeight = 0.015 * artificialMassScale;
	double footWeight = 0.0689 * artificialMassScale + 20.0*cablePartWeight;
	
	double mainBoardWeight = 0.025 * artificialMassScale;
	double hipPlateWeight = 0.053 * artificialMassScale;
	double accuCaseWeight = 0.00747 * artificialMassScale;
	double femurABSWeight = 0.03658 * artificialMassScale 
				- accuCaseWeight 
				+ 24.0*cablePartWeight;
	double mainBodyWeight = 0.06 * artificialMassScale 
				+ 2.0 * dynamixelMass 
				+ 20.0*screwWeight 
				+ 146.0*cablePartWeight;
	double ankleWeight = 2.0 * dynamixelMass 
				+ 3.0*smallPlateWeight 
				+ 15.0*screwWeight 
				+ 15.0*cablePartWeight;
	double tibiaWeight = dynamixelMass 
				+ 0.01 * artificialMassScale 
				+ 9.0 * screwWeight 
				+ 15.0 * cablePartWeight;
	double femurWeight = femurABSWeight;
	double uFemurWeight = 2.0 * dynamixelMass 
				+ 2.0 * smallPlateWeight 
				+ 27.0 * screwWeight 
				+ 30.0 * cablePartWeight; //+ 3.0 * smallPlateWeight;	
	double hipPartWeight = 0.015 * artificialMassScale 
				+ 4.0 * cablePartWeight;
	double shoulderWeight = 0.012 * artificialMassScale 
				+ 4.0 * screwWeight 
				+ 10.0*cablePartWeight;
	double upperArmWeight = dynamixelMass 
				+ 0.015 * artificialMassScale 
				+ 11.0 * screwWeight 
				+ 12.5 * cablePartWeight;
	double lowerArmWeight = dynamixelMass 
				+ smallPlateWeight 
				+ 14.0*screwWeight 
				+ 14.0*cablePartWeight;
// 0.007 as mass of the additional plate under the accuCase
	double invisibleHipWeight = dynamixelMass / 2.0 
				+ 0.007 
				+ 53.0 * screwWeight 
				+ 72.0 * cablePartWeight;
	double neckWeight = dynamixelMass + 12.0*cablePartWeight;
	double headBaseWeight = dynamixelMass + 10.0 * screwWeight + 10.0 * cablePartWeight;

	double sizeScale = 1.0;

	double legOffset = 0.0325 * sizeScale;
	double footOffset = 0.011 * sizeScale;
	double footHeight = 0.0164 * sizeScale;
	double accelBoardHeight = 0.0125 * sizeScale;
	double accelBoardDepth = 0.026 * sizeScale;
	double accelOffsetFootX = 0.0072 * sizeScale;
	double accelOffsetFootY = accelBoardHeight / 2.0 + footHeight / 2.0;
	double accelOffsetFootZ = 0.03005 * sizeScale;

	double dynamixelWidth = 0.032 * sizeScale;
	double dynamixelHeight = 0.038 * sizeScale;
	double dynamixelDepth = 0.05 * sizeScale;

	double ankleHeight = 0.0505 * sizeScale;
	double anklePosZ = -0.0245;
	double height = ankleHeight - dynamixelDepth / 2.0 + 0.0115;//0.026 + 0.011 / 2.0;
	double tibiaPosY = ankleHeight 
				+ dynamixelDepth / 2.0 
				+ 0.008 
				+ (dynamixelWidth + 0.017) / 2.0;
	double tibiaHeight = dynamixelWidth + 0.017 * sizeScale;
	double legOffsetZ = -0.0055 * sizeScale;

	double femurPosY = tibiaHeight/2.0 + tibiaPosY + 0.015	;
	double femurHeight = 0.021 * sizeScale;

	double upperFemurPosY = femurPosY + femurHeight / 2.0 + 0.003 + dynamixelDepth / 2.0;

	double space = 0.0005;

	double soleOfFootHeight = 0.001;
	double footW = 0.07;
	double footH = 0.0164 - soleOfFootHeight;
	double footD = 0.1005;

	double footX = -legOffset-footOffset;
	double footY = (footHeight - soleOfFootHeight) / 2.0 + soleOfFootHeight;
	double footZ = 	0.0;

	double rubberSoleHeight = footH + soleOfFootHeight - space;
	
	double soleOfFootFW = 0.0154;

	double soleOfFootFH = rubberSoleHeight;
	double soleOfFootFD = 0.0154;

	double soleOfFootFX = footW/2.0 - soleOfFootFW/2.0 - 0.004;
	double soleOfFootFY = -soleOfFootHeight/2.0 - space;
	double soleOfFootFZ = footD/2.0 - soleOfFootFD/2.0 - 0.004;
	
	double soleOfFootOutSideW = 0.013;
	double soleOfFootOutSideH = soleOfFootFH;
	double soleOfFootOutSideD = 0.013;

	double soleOfFootFOutX = -footW/2.0 + soleOfFootOutSideW/2.0 + 0.004;
	double soleOfFootFOutY = -soleOfFootHeight/2.0 - space;
	double soleOfFootFOutZ = footD/2.0 - soleOfFootFD - soleOfFootOutSideD/2.0;

	double soleOfFootBX = 0.0;
	double soleOfFootBY = -soleOfFootHeight/2.0 - space; 
	double soleOfFootBW = 0.0375;
	double soleOfFootBH = rubberSoleHeight;
	double soleOfFootBD = 0.014;
	double soleOfFootBZ = -footD/2.0 + soleOfFootBD/2.0 + 0.004;

	double ankleW = dynamixelHeight;
	double ankleH = dynamixelDepth;
	double ankleD = dynamixelWidth + dynamixelHeight;
	double ankleX = legOffset;
	double ankleY = ankleHeight;
	double ankleZ = anklePosZ;

	double tibiaW = 0.0425;
	double tibiaH = dynamixelWidth + 0.017 * sizeScale;
	double tibiaD = dynamixelDepth;
	double tibiaX = legOffset;
	double tibiaY = ankleHeight + dynamixelDepth / 2.0 + 0.008 + (dynamixelWidth + 0.017) / 2.0;
	double tibiaZ = legOffsetZ;

	double femurW = 0.047;
	double femurH = 0.021;
	double femurD = 0.043;
	double femurX = legOffset - 0.00025;
	double femurY = tibiaHeight / 2.0 + tibiaPosY + 0.021 / 2.0 + 0.001;
	double femurZ = legOffsetZ - 0.005;
	
	double uFemurX = legOffset;
	double uFemurY = upperFemurPosY;
	double uFemurZ = -0.0245 * sizeScale;
	double uFemurW = dynamixelHeight;
	double uFemurH = dynamixelDepth;
	double uFemurD = dynamixelWidth + dynamixelHeight;

	double diffX = (-0.005- 0.047/2.0) - 0.00025 - space;
	double diffY = -0.0095;
	double diffZ = femurD/2.0-0.00381-0.049/2.0;
	
	double accuCaseW = 0.01;
	double accuCaseH = 0.094;
	double accuCaseD = 0.049;

	double rightHipPartY = upperFemurPosY + dynamixelDepth / 2.0 - 0.0115 + 0.0289 - 0.003;

	double hipPartX = legOffset;
	double hipPartY = rightHipPartY;
	double hipPartZ = anklePosZ;
	double hipPartW = 0.0276;
	double hipPartH = 0.006;
	double hipPartD = 0.078;

	double hipPlatePosY =rightHipPartY + 0.003 + 0.0025 + 0.001;

	double hipPosX = 0.038/2.0 + 0.027/2.0;
	double hipPosY = hipPlatePosY + 0.038/2.0 + 0.0025 + space;
	double hipPosZ = anklePosZ + 0.011 - 0.0533/2.0;

	double middleHipX = 0;
	double middleHipY = hipPosY;
	double middleHipZ = hipPosZ + 0.034;

	double middleHipDiffX = 0.0;
	double middleHipDiffY = 0.00125;
	double middleHipDiffZ = -0.022;
	
	double hipDiffX = -hipPosX;
	double hipDiffY = hipPosY - (hipPosY);
	double hipDiffZ =  -(hipPosZ + 0.034 - (hipPosZ));

	double hipSideX = hipDiffX;
	double hipSideY = hipDiffY - middleHipDiffY;
	double hipSideZ = hipDiffZ - middleHipDiffZ;

	double hipSideW = 0.0295;
	double hipSideH = 0.038;
	double hipSideD = 0.0533;

	double hipPlateDiffX = 0;
	double hipPlateDiffY = hipPosY - (hipPlatePosY);
	double hipPlateDiffZ = hipPosZ + 0.034 - (legOffsetZ - 0.052/2.0);

	double accuCasePosY = hipPosY + dynamixelHeight/2.0 + 0.0075/2.0;

	double accuCaseDiffX = 0;
	double accuCaseDiffY = hipPosY - (accuCasePosY);
	double accuCaseDiffZ = hipPosZ + 0.034 - (hipPosZ);

	double pdaPosY = hipPosY + 0.052 - dynamixelHeight/2.0;
	double pdaPosZ = hipPosZ - 0.01015 - 0.0533/2.0 - 0.0005;

	double upperBodyPosY = accuCasePosY + 0.0034 + 0.063/2.0 + 0.0075/2.0;
	double upperBodyPosZ = pdaPosZ + 0.0203/2.0 + 0.0276 + 0.0656/2.0;

	double neckPosZ = upperBodyPosZ -(0.0656)/2.0  + 0.025;
	double neckPosY = upperBodyPosY + 0.063/2.0 + 0.037/2.0 + 0.0005;

	double headBasePosZ = neckPosZ + 0.025 - 0.0115 - 0.0115 + 0.025;
	double headBasePosY = neckPosY + 0.0348/2.0 + 0.037/2.0 + 0.001;

	double shoulderPosX = (0.1047/2.0 + 0.0035 + 0.001);
	double shoulderPosY = upperBodyPosY + 0.063/2.0 - 0.005 - 0.0124;
	double shoulderPosZ = upperBodyPosZ - 0.004;

	double shoulderW = 0.007;
	double shoulderH = 0.0248;
	double shoulderD = 0.048;

	double upperArmPosX = (shoulderPosX - 0.007/2.0 + 0.0231 + 0.0609/2.0 - 0.0115);
	double upperArmPosY = upperBodyPosY + 0.063/2.0 - 0.005 - 0.0124;
	double upperArmPosZ = upperBodyPosZ - 0.004 + 0.0148;

	double upperArmW = 0.0609;
	double upperArmH = 0.032;
	double upperArmD = 0.038;

	double lowerArmPosX = upperArmPosX + 0.0609/2.0 + 0.0565/2.0 + 0.0075;
	double lowerArmW = 0.0565;
	double lowerArmH = 0.032;
	double lowerArmD = 0.038;
	
	
	// Right Foot
	PARAM(DoubleValue, mRightFoot, "Mass")->set(footWeight - soleOfFootWeight);
	PARAM(Vector3DValue, mRightFoot, "Position")->set(footX, footY, footZ);
	PARAM(Vector3DValue, mRightFoot, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mRightFoot, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mRightFoot, "Width")->set(footW * sizeScale);
	PARAM(DoubleValue, mRightFoot, "Height")->set(footH * sizeScale);
	PARAM(DoubleValue, mRightFoot, "Depth")->set(footD * sizeScale);


	// Inner Sole Of Foot	
	mRightSoleOfFootFront->setMass(soleOfFootWeight/4.0);
	BoxGeom *rightSoleOfFootFrontBox = 
		dynamic_cast<BoxGeom*>(mRightSoleOfFootFront->getGeometry());
	
	rightSoleOfFootFrontBox->setSize(soleOfFootFW, soleOfFootFH, soleOfFootFD);
	rightSoleOfFootFrontBox->setLocalPosition(Vector3D(soleOfFootFX, 
													   soleOfFootFY, 
													   soleOfFootFZ));

	
	mRightSoleOfFootFrontOut->setMass(soleOfFootWeight/4.0);
	BoxGeom *rightSoleOfFootFrontOutBox = 
		dynamic_cast<BoxGeom*>(mRightSoleOfFootFrontOut->getGeometry());
	rightSoleOfFootFrontOutBox->setSize(soleOfFootOutSideW, soleOfFootOutSideH, soleOfFootOutSideD);
	rightSoleOfFootFrontOutBox->setLocalPosition(Vector3D(soleOfFootFOutX, 
														  soleOfFootFOutY, 
														  soleOfFootFOutZ));


	mRightSoleOfFootBack->setMass(soleOfFootWeight/2.0);
	BoxGeom *rightSoleOfFootBackBox = 
		dynamic_cast<BoxGeom*>(mRightSoleOfFootBack->getGeometry());
	rightSoleOfFootBackBox->setAutomaticColor(false);
	rightSoleOfFootBackBox->setSize(soleOfFootBW, soleOfFootBH, soleOfFootBD);
	rightSoleOfFootBackBox->setLocalPosition(Vector3D(soleOfFootBX, soleOfFootBY, soleOfFootBZ));


	//Right Ankle Box
	PARAM(DoubleValue, mRightAnkleBox, "Mass")->set(ankleWeight);
	PARAM(Vector3DValue, mRightAnkleBox, "Position")->set(-ankleX,ankleY, ankleZ);
	PARAM(Vector3DValue, mRightAnkleBox, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mRightAnkleBox, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mRightAnkleBox, "Width")->set(ankleW);
	PARAM(DoubleValue, mRightAnkleBox, "Height")->set(ankleH);
	PARAM(DoubleValue, mRightAnkleBox, "Depth")->set(ankleD);
	
	//Right Tibia
	PARAM(DoubleValue, mRightTibia, "Mass")->set(tibiaWeight);
	PARAM(Vector3DValue, mRightTibia, "Position")->set(-tibiaX,tibiaY,tibiaZ);
	PARAM(Vector3DValue, mRightTibia, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mRightTibia, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mRightTibia, "Width")->set(tibiaW);
	PARAM(DoubleValue, mRightTibia, "Height")->set(tibiaH);
	PARAM(DoubleValue, mRightTibia, "Depth")->set(tibiaD);
	
	//Right Femur
	PARAM(DoubleValue, mRightFemur, "Mass")->set(femurWeight);
	PARAM(Vector3DValue, mRightFemur, "Position")->set(-femurX, femurY, femurZ);
	PARAM(Vector3DValue, mRightFemur, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mRightFemur, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mRightFemur, "Width")->set(femurW);
	PARAM(DoubleValue, mRightFemur, "Height")->set(femurH);
	PARAM(DoubleValue, mRightFemur, "Depth")->set(femurD);

	//Right Upper Femur
	PARAM(DoubleValue, mRightUpperFemur, "Mass")->set(uFemurWeight);
	PARAM(Vector3DValue, mRightUpperFemur, "Position")->set(-uFemurX, uFemurY, uFemurZ);
	PARAM(Vector3DValue, mRightUpperFemur, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mRightUpperFemur, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mRightUpperFemur, "Width")->set(uFemurW);
	PARAM(DoubleValue, mRightUpperFemur, "Height")->set(uFemurH);
	PARAM(DoubleValue, mRightUpperFemur, "Depth")->set(uFemurD);

	//rightAccuCase (CO)
	BoxGeom *rightAccuCaseBox= dynamic_cast<BoxGeom*>(mRightAccuCase->getGeometry());
	mRightAccuCase->setMass(accuCaseWeight + accuWeight);
	rightAccuCaseBox->setLocalPosition(Vector3D(diffX, diffY, diffZ));
	rightAccuCaseBox->setSize(accuCaseW, accuCaseH, accuCaseD);
	
	// Left Foot
	PARAM(DoubleValue, mLeftFoot, "Mass")->set(footWeight - soleOfFootWeight);
	PARAM(Vector3DValue, mLeftFoot, "Position")->set(-footX, footY, footZ);
	PARAM(Vector3DValue, mLeftFoot, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mLeftFoot, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mLeftFoot, "Width")->set(footW * sizeScale);
	PARAM(DoubleValue, mLeftFoot, "Height")->set(footH * sizeScale);
	PARAM(DoubleValue, mLeftFoot, "Depth")->set(footD * sizeScale);
	
	BoxGeom *leftSoleOfFootFrontBox= dynamic_cast<BoxGeom*>(mLeftSoleOfFootFront->getGeometry());
	mLeftSoleOfFootFront->setMass(soleOfFootWeight/4.0);
	leftSoleOfFootFrontBox->setLocalPosition(Vector3D(-soleOfFootFX, 
													  soleOfFootFY, 
													  soleOfFootFZ));
	leftSoleOfFootFrontBox->setSize(soleOfFootFW, soleOfFootFH, soleOfFootFD);


	mLeftSoleOfFootFrontOut->setMass(soleOfFootWeight/4.0);
	BoxGeom *leftSoleOfFootFrontOutBox= dynamic_cast<BoxGeom*>(
				mLeftSoleOfFootFrontOut->getGeometry());
	leftSoleOfFootFrontOutBox->setLocalPosition(Vector3D(-soleOfFootFOutX, 
		soleOfFootFOutY, soleOfFootFOutZ));
	leftSoleOfFootFrontOutBox->setSize(soleOfFootOutSideW, soleOfFootOutSideH, soleOfFootOutSideD);


	mLeftSoleOfFootBack->setMass(soleOfFootWeight/2.0);
	BoxGeom *leftSoleOfFootBackBox = dynamic_cast<BoxGeom*>(mLeftSoleOfFootBack
		->getGeometry());
	leftSoleOfFootBackBox->setLocalPosition(Vector3D(soleOfFootBX, 
													 soleOfFootBY, 
													 soleOfFootBZ));
	leftSoleOfFootBackBox->setSize(soleOfFootBW, soleOfFootBH, soleOfFootBD);

	
	//LeftAnkleBox
	PARAM(DoubleValue, mLeftAnkleBox, "Mass")->set(ankleWeight);
	PARAM(Vector3DValue, mLeftAnkleBox, "Position")->set(ankleX, ankleY, ankleZ);
	PARAM(Vector3DValue, mLeftAnkleBox, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mLeftAnkleBox, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mLeftAnkleBox, "Width")->set(ankleW);
	PARAM(DoubleValue, mLeftAnkleBox, "Height")->set(ankleH);
	PARAM(DoubleValue, mLeftAnkleBox, "Depth")->set(ankleD);


	//Left Tibia
	PARAM(DoubleValue, mLeftTibia, "Mass")->set(tibiaWeight);
	PARAM(Vector3DValue, mLeftTibia, "Position")->set(tibiaX, tibiaY, tibiaZ);
	PARAM(Vector3DValue, mLeftTibia, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mLeftTibia, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mLeftTibia, "Width")->set(tibiaW);
	PARAM(DoubleValue, mLeftTibia, "Height")->set(tibiaH);
	PARAM(DoubleValue, mLeftTibia, "Depth")->set(tibiaD);
	

	//Left Femur
	PARAM(DoubleValue, mLeftFemur, "Mass")->set(femurWeight);
	PARAM(Vector3DValue, mLeftFemur, "Position")->set(femurX, femurY, femurZ);
	PARAM(Vector3DValue, mLeftFemur, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mLeftFemur, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mLeftFemur, "Width")->set(femurW);
	PARAM(DoubleValue, mLeftFemur, "Height")->set(femurH);
	PARAM(DoubleValue, mLeftFemur, "Depth")->set(femurD);


	//Left Upper Femur
	PARAM(DoubleValue, mLeftUpperFemur, "Mass")->set(uFemurWeight);
	PARAM(Vector3DValue, mLeftUpperFemur, "Position")->set(uFemurX, uFemurY, uFemurZ);
	PARAM(Vector3DValue, mLeftUpperFemur, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mLeftUpperFemur, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mLeftUpperFemur, "Width")->set(uFemurW);
	PARAM(DoubleValue, mLeftUpperFemur, "Height")->set(uFemurH);
	PARAM(DoubleValue, mLeftUpperFemur, "Depth")->set(uFemurD);
	
	mLeftAccuCase->setMass(accuCaseWeight + accuWeight);
	BoxGeom *leftAccuCaseBox = dynamic_cast<BoxGeom*>(mLeftAccuCase->getGeometry());
	leftAccuCaseBox->setLocalPosition(Vector3D(-diffX, diffY, diffZ));
	leftAccuCaseBox->setSize(accuCaseW, accuCaseH, accuCaseD);
	
	// Hipplate
	PARAM(DoubleValue, mHipPlate, "Mass")->set(hipPartWeight);
	PARAM(Vector3DValue, mHipPlate, "Position")->set(-hipPartX, hipPartY, hipPartZ);
	PARAM(Vector3DValue, mHipPlate, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mHipPlate, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mHipPlate, "Width")->set(hipPartW * sizeScale);
	PARAM(DoubleValue, mHipPlate, "Height")->set(hipPartH * sizeScale);
	PARAM(DoubleValue, mHipPlate, "Depth")->set(hipPartD * sizeScale);


	//Left Hip Part
	PARAM(DoubleValue, mLeftHipPart, "Mass")->set(hipPartWeight);
	PARAM(Vector3DValue, mLeftHipPart, "Position")->set(hipPartX, hipPartY, hipPartZ);
	PARAM(Vector3DValue, mLeftHipPart, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mLeftHipPart, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mLeftHipPart, "Width")->set(hipPartW * sizeScale);
	PARAM(DoubleValue, mLeftHipPart, "Height")->set(hipPartH * sizeScale);
	PARAM(DoubleValue, mLeftHipPart, "Depth")->set(hipPartD * sizeScale);


	//Middle Invisible Part
	PARAM(DoubleValue, mMiddleInvisiblePart, "Mass")->set(invisibleHipWeight);
	PARAM(Vector3DValue, mMiddleInvisiblePart, "Position")->set(middleHipX, 
												middleHipY + 0.00125, 
												middleHipZ - 0.022);
	PARAM(Vector3DValue, mMiddleInvisiblePart, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mMiddleInvisiblePart, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mMiddleInvisiblePart, "Width")->set(0.097 * sizeScale);
	PARAM(DoubleValue, mMiddleInvisiblePart, "Height")->set((0.0505 - (2.0*0.00125) )* sizeScale);
	PARAM(DoubleValue, mMiddleInvisiblePart, "Depth")->set(2.0*(0.0533/2.0 + 0.012)* sizeScale);


	mHipMiddleObject->setMass(dynamixelMass/2.0);
	BoxGeom *hipMiddleBox = dynamic_cast<BoxGeom*>(mHipMiddleObject->getGeometry());
	hipMiddleBox->setLocalPosition(Vector3D(middleHipDiffX, 
											-middleHipDiffY, 
											-middleHipDiffZ));
	hipMiddleBox->setSize(0.038, 0.038, 0.05); //TODO add variables!


	mRightHipPartObject->setMass(dynamixelMass);
	BoxGeom *rightHipPartBox = dynamic_cast<BoxGeom*>(mRightHipPartObject->getGeometry());
	rightHipPartBox->setLocalPosition(Vector3D(hipSideX, hipSideY, hipSideZ));
	rightHipPartBox->setSize(hipSideW, hipSideH, hipSideD);

	
	mLeftHipPartObject->setMass(dynamixelMass);
	BoxGeom *leftHipPartBox = dynamic_cast<BoxGeom*>(mLeftHipPartObject->getGeometry());
	leftHipPartBox->setLocalPosition(Vector3D(-hipSideX, hipSideY, hipSideZ));
	leftHipPartBox->setSize(hipSideW, hipSideH, hipSideD);

	
	mHipPlateObject->setMass(hipPlateWeight);
	BoxGeom *hipPlateBox = dynamic_cast<BoxGeom*>(mHipPlateObject->getGeometry());
	hipPlateBox->setLocalPosition(Vector3D(hipPlateDiffX, -hipPlateDiffY - space 
		- middleHipDiffY, -hipPlateDiffZ - middleHipDiffZ));
	hipPlateBox->setSize(0.1, 0.005, 0.052); //TODO add variables!
	

	mAccuCaseObject->setMass(accuCaseWeight * artificialMassScale + accuWeight);
	BoxGeom *accuCaseBox = dynamic_cast<BoxGeom*>(mAccuCaseObject->getGeometry());
	accuCaseBox->setLocalPosition(Vector3D(accuCaseDiffX, 
										   -accuCaseDiffY + space - middleHipDiffY, 
										   -accuCaseDiffZ - middleHipDiffZ));
	accuCaseBox->setSize(0.0295*2.0 + 0.038, 0.0075, 0.0533); //TODO add variables!


	//Middle PDA
	PARAM(DoubleValue, mMiddlePda, "Mass")->set(pdaWeight);
	PARAM(Vector3DValue, mMiddlePda, "Position")->set(0.0, pdaPosY, pdaPosZ);
	PARAM(Vector3DValue, mMiddlePda, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mMiddlePda, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mMiddlePda, "Width")->set((0.078 * sizeScale));
	PARAM(DoubleValue, mMiddlePda, "Height")->set(0.104 * sizeScale);
	PARAM(DoubleValue, mMiddlePda, "Depth")->set(0.0203 * sizeScale);


	//Middle Upper Body
	PARAM(DoubleValue, mMiddleUpperBody, "Mass")->set(mainBodyWeight + mainBoardWeight);
	PARAM(Vector3DValue, mMiddleUpperBody, "Position")->set(0.0, upperBodyPosY, upperBodyPosZ);
	PARAM(Vector3DValue, mMiddleUpperBody, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mMiddleUpperBody, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mMiddleUpperBody, "Width")->set((0.1047 * sizeScale));
	PARAM(DoubleValue, mMiddleUpperBody, "Height")->set(0.063 * sizeScale);
	PARAM(DoubleValue, mMiddleUpperBody, "Depth")->set(0.0656 * sizeScale);


	//Middle Neck
	PARAM(DoubleValue, mMiddleNeck, "Mass")->set(neckWeight);
	PARAM(Vector3DValue, mMiddleNeck, "Position")->set(0.0, neckPosY, neckPosZ);
	PARAM(Vector3DValue, mMiddleNeck, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mMiddleNeck, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mMiddleNeck, "Width")->set((0.032 * sizeScale));
	PARAM(DoubleValue, mMiddleNeck, "Height")->set(0.037 * sizeScale);
	PARAM(DoubleValue, mMiddleNeck, "Depth")->set(0.05 * sizeScale);


	//Middle Head Base
	PARAM(DoubleValue, mMiddleHeadBase, "Mass")->set(headBaseWeight);
	PARAM(Vector3DValue, mMiddleHeadBase, "Position")->set(0.0, headBasePosY, headBasePosZ);
	PARAM(Vector3DValue, mMiddleHeadBase, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mMiddleHeadBase, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mMiddleHeadBase, "Width")->set((0.038 * sizeScale));
	PARAM(DoubleValue, mMiddleHeadBase, "Height")->set(0.0348 * sizeScale);
	PARAM(DoubleValue, mMiddleHeadBase, "Depth")->set(0.05 * sizeScale);


	//Middle head
	PARAM(DoubleValue, mMiddleHead, "Mass")->set(headWeight);
	PARAM(Vector3DValue, mMiddleHead, "Position")->set(0.0, headBasePosY , headBasePosZ + 0.025 
			- 0.0115 - 0.0115 + 0.074/2.0);
	PARAM(Vector3DValue, mMiddleHead, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mMiddleHead, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mMiddleHead, "Width")->set((0.06 * sizeScale));
	PARAM(DoubleValue, mMiddleHead, "Height")->set(0.0422 * sizeScale);
	PARAM(DoubleValue, mMiddleHead, "Depth")->set(0.074 * sizeScale);


	// Right Arm
	PARAM(DoubleValue, mRightShoulder, "Mass")->set(shoulderWeight);
	PARAM(Vector3DValue, mRightShoulder, "Position")->set(-shoulderPosX, 
												shoulderPosY, 
												shoulderPosZ);
	PARAM(Vector3DValue, mRightShoulder, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mRightShoulder, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mRightShoulder, "Width")->set((shoulderW * sizeScale));
	PARAM(DoubleValue, mRightShoulder, "Height")->set(shoulderH * sizeScale);
	PARAM(DoubleValue, mRightShoulder, "Depth")->set(shoulderD * sizeScale);
	
	
	//Right Upper Arm
	PARAM(DoubleValue, mRightUpperArm, "Mass")->set(upperArmWeight);
	PARAM(Vector3DValue, mRightUpperArm, "Position")->set(-upperArmPosX, 
												upperArmPosY, 
												upperArmPosZ);
	PARAM(Vector3DValue, mRightUpperArm, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mRightUpperArm, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mRightUpperArm, "Width")->set((upperArmW * sizeScale));
	PARAM(DoubleValue, mRightUpperArm, "Height")->set(upperArmH * sizeScale);
	PARAM(DoubleValue, mRightUpperArm, "Depth")->set(upperArmD * sizeScale);


	//Right Lower Arm
	PARAM(DoubleValue, mRightLowerArm, "Mass")->set(lowerArmWeight);
	PARAM(Vector3DValue, mRightLowerArm, "Position")->set(-1*lowerArmPosX, 
												upperArmPosY, 
												upperArmPosZ);
	PARAM(Vector3DValue, mRightLowerArm, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mRightLowerArm, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mRightLowerArm, "Width")->set((lowerArmW * sizeScale));
	PARAM(DoubleValue, mRightLowerArm, "Height")->set(lowerArmH * sizeScale);
	PARAM(DoubleValue, mRightLowerArm, "Depth")->set(lowerArmD * sizeScale);
	

	mRightHandObject->setMass(handWeight);
	BoxGeom *rightHandBox = dynamic_cast<BoxGeom*>(mRightHandObject->getGeometry());
	rightHandBox->setLocalPosition(Vector3D(-1*(0.0565/2.0 + 0.057/2.0 + 0.0005),0.0, 
			-0.038/2.0 + 0.01/2.0));
	rightHandBox->setSize(0.057, 0.0321, 0.01); //TODO add variables!

	
	// Left Arm
	// 	Here is an additional scaling of the weight with 10!!!
	PARAM(DoubleValue, mLeftShoulder, "Mass")->set(shoulderWeight);
	PARAM(Vector3DValue, mLeftShoulder, "Position")->set(shoulderPosX, 
												shoulderPosY, 
												shoulderPosZ);
	PARAM(Vector3DValue, mLeftShoulder, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mLeftShoulder, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mLeftShoulder, "Width")->set((shoulderW));
	PARAM(DoubleValue, mLeftShoulder, "Height")->set(shoulderH);
	PARAM(DoubleValue, mLeftShoulder, "Depth")->set(shoulderD);


	//Left Upper Arm
	PARAM(DoubleValue, mLeftUpperArm, "Mass")->set(upperArmWeight);
	PARAM(Vector3DValue, mLeftUpperArm, "Position")->set(upperArmPosX, 
												upperArmPosY, 
												upperArmPosZ);
	PARAM(Vector3DValue, mLeftUpperArm, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mLeftUpperArm, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mLeftUpperArm, "Width")->set((upperArmW * sizeScale));
	PARAM(DoubleValue, mLeftUpperArm, "Height")->set(upperArmH * sizeScale);
	PARAM(DoubleValue, mLeftUpperArm, "Depth")->set(upperArmD * sizeScale);


	//Left Lower Arm
	PARAM(DoubleValue, mLeftLowerArm, "Mass")->set(lowerArmWeight);
	PARAM(Vector3DValue, mLeftLowerArm, "Position")->set(lowerArmPosX, 
												upperArmPosY, 
												upperArmPosZ);
	PARAM(Vector3DValue, mLeftLowerArm, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, mLeftLowerArm, "Orientation")->set(0,0,0);
	PARAM(DoubleValue, mLeftLowerArm, "Width")->set((lowerArmW * sizeScale));
	PARAM(DoubleValue, mLeftLowerArm, "Height")->set(lowerArmH * sizeScale);
	PARAM(DoubleValue, mLeftLowerArm, "Depth")->set(lowerArmD * sizeScale);


	mLeftHandObject->setMass(handWeight);
	BoxGeom *leftHandObjectBox = dynamic_cast<BoxGeom*>(mLeftHandObject->getGeometry());
	leftHandObjectBox->setLocalPosition(Vector3D(0.0565/2.0 + 0.057/2.0 + 0.0005,
								   0.0, 
								   - 0.038/2.0 + 0.01/2.0));
	leftHandObjectBox->setSize(0.057, 0.0321, 0.01); //TODO add variables!

	// Motors:

	// 1: Hip
	PARAM(Vector3DValue, mHipMotor, "AxisPoint1")->set(0.0, hipPosY, hipPosZ + 0.034 + 0.025 - 0.0115);
	PARAM(Vector3DValue, mHipMotor, "AxisPoint2")->set(-0.1, hipPosY, hipPosZ + 0.034 + 0.025 - 0.0115);
	PARAM(DoubleValue, mHipMotor, "MinAngle")->set(0);
	PARAM(DoubleValue, mHipMotor, "MaxAngle")->set(150);


	// 2: Right Shoulder Yaw
	PARAM(Vector3DValue, mRightShoulderMotorRoll, "AxisPoint1")->set(shoulderPosX, shoulderPosY, shoulderPosZ);
	PARAM(Vector3DValue, mRightShoulderMotorRoll, "AxisPoint2")->set(shoulderPosX + 0.1, shoulderPosY, shoulderPosZ);
	PARAM(DoubleValue, mRightShoulderMotorRoll, "MinAngle")->set(-150);
	PARAM(DoubleValue, mRightShoulderMotorRoll, "MaxAngle")->set(150);


	// 3: left Shoulder Yaw
	PARAM(Vector3DValue, mLeftShoulderMotorRoll, "AxisPoint1")->set(-shoulderPosX, shoulderPosY, shoulderPosZ);
	PARAM(Vector3DValue, mLeftShoulderMotorRoll, "AxisPoint2")->set(-shoulderPosX - 0.1, shoulderPosY, shoulderPosZ);
	PARAM(DoubleValue, mLeftShoulderMotorRoll, "MinAngle")->set(-150);
	PARAM(DoubleValue, mLeftShoulderMotorRoll, "MaxAngle")->set(150);


	// 4: Right shoulder pitch
	PARAM(Vector3DValue, mRightShoulderMotor, "AxisPoint1")->set(-1*(upperArmPosX - 0.0609/2.0 + 0.0115), upperArmPosY, upperArmPosZ);
	PARAM(Vector3DValue, mRightShoulderMotor, "AxisPoint2")->set(-1*(upperArmPosX - 0.0609/2.0 + 0.0115), upperArmPosY + 0.1, upperArmPosZ);
	PARAM(DoubleValue, mRightShoulderMotor, "MinAngle")->set(-106.5);
	PARAM(DoubleValue, mRightShoulderMotor, "MaxAngle")->set(94.5);

	
	// 5: left shoulder pitch
	PARAM(Vector3DValue, mLeftShoulderMotor, "AxisPoint1")->set(1*(upperArmPosX - 0.0609/2.0 + 0.0115), upperArmPosY, upperArmPosZ);
	PARAM(Vector3DValue, mLeftShoulderMotor, "AxisPoint2")->set(1*(upperArmPosX - 0.0609/2.0 + 0.0115), upperArmPosY + 0.1, upperArmPosZ);
	PARAM(DoubleValue, mLeftShoulderMotor, "MinAngle")->set(-94.5);
	PARAM(DoubleValue, mLeftShoulderMotor, "MaxAngle")->set(106.5);


	// 6:right ellbow
	PARAM(Vector3DValue, mRightElbowMotor, "AxisPoint1")->set(-1*(lowerArmPosX - 0.0565/2.0 + 0.0115), upperArmPosY , upperArmPosZ);
	PARAM(Vector3DValue, mRightElbowMotor, "AxisPoint2")->set(-1*(lowerArmPosX - 0.0565/2.0 + 0.0115), upperArmPosY - 0.1, upperArmPosZ);
	PARAM(DoubleValue, mRightElbowMotor, "MinAngle")->set(-106.5);
	PARAM(DoubleValue, mRightElbowMotor, "MaxAngle")->set(87.0);


	// 7: left ellbow
	PARAM(Vector3DValue, mLeftElbowMotor, "AxisPoint1")->set(1*(lowerArmPosX - 0.0565/2.0 + 0.0115), upperArmPosY, upperArmPosZ);
	PARAM(Vector3DValue, mLeftElbowMotor, "AxisPoint2")->set(1*(lowerArmPosX - 0.0565/2.0 + 0.0115), upperArmPosY - 0.1, upperArmPosZ);
	PARAM(DoubleValue, mLeftElbowMotor, "MinAngle")->set(-87.0);
	PARAM(DoubleValue, mLeftElbowMotor, "MaxAngle")->set(106.5);


	// 8:right hip roll
	PARAM(Vector3DValue, mRightHipRoll, "AxisPoint1")->set(-legOffset, hipPlatePosY, anklePosZ);
	PARAM(Vector3DValue, mRightHipRoll, "AxisPoint2")->set(-legOffset, hipPlatePosY - 0.0115, anklePosZ);
	PARAM(DoubleValue, mRightHipRoll, "MinAngle")->set(-45);
	PARAM(DoubleValue, mRightHipRoll, "MaxAngle")->set(150);


	// 9: left hip roll
	PARAM(Vector3DValue, mLeftHipRoll, "AxisPoint1")->set(legOffset, hipPlatePosY, anklePosZ);
	PARAM(Vector3DValue, mLeftHipRoll, "AxisPoint2")->set(legOffset, hipPlatePosY - 0.0115, anklePosZ);
	PARAM(DoubleValue, mLeftHipRoll, "MinAngle")->set(-150);
	PARAM(DoubleValue, mLeftHipRoll, "MaxAngle")->set(45);


	// 10:right hip pitch
	PARAM(Vector3DValue, mRightHipSide, "AxisPoint1")->set(-legOffset, upperFemurPosY + 0.025 - 0.0115, anklePosZ);
	PARAM(Vector3DValue, mRightHipSide, "AxisPoint2")->set(-legOffset, upperFemurPosY + 0.025 - 0.0115, anklePosZ + 0.1);
	PARAM(DoubleValue, mRightHipSide, "MinAngle")->set(-75);
	PARAM(DoubleValue, mRightHipSide, "MaxAngle")->set(40);


	// 11:left hip pitch
	PARAM(Vector3DValue, mLeftHipSide, "AxisPoint1")->set(legOffset, upperFemurPosY + 0.025 - 0.0115, anklePosZ);
	PARAM(Vector3DValue, mLeftHipSide, "AxisPoint2")->set(legOffset, upperFemurPosY + 0.025 - 0.0115, anklePosZ + 0.1);
	PARAM(DoubleValue, mLeftHipSide, "MinAngle")->set(-40);
	PARAM(DoubleValue, mLeftHipSide, "MaxAngle")->set(75);


	// 12:right hip yaw
	PARAM(Vector3DValue, mRightHipFB, "AxisPoint1")->set(0, upperFemurPosY + dynamixelDepth/2.0 - 0.0115 , legOffsetZ);
	PARAM(Vector3DValue, mRightHipFB, "AxisPoint2")->set(-0.01, upperFemurPosY + dynamixelDepth/2.0 - 0.0115, legOffsetZ);
	PARAM(DoubleValue, mRightHipFB, "MinAngle")->set(-15);
	PARAM(DoubleValue, mRightHipFB, "MaxAngle")->set(150);


	// 13:left hip yaw
	PARAM(Vector3DValue, mLeftHipFB, "AxisPoint1")->set(0, upperFemurPosY + dynamixelDepth/2.0 - 0.0115, legOffsetZ);
	PARAM(Vector3DValue, mLeftHipFB, "AxisPoint2")->set(0.01, upperFemurPosY + dynamixelDepth/2.0 - 0.0115, legOffsetZ);
	PARAM(DoubleValue, mLeftHipFB, "MinAngle")->set(-150);
	PARAM(DoubleValue, mLeftHipFB, "MaxAngle")->set(15);


	// 14:right knee
	PARAM(Vector3DValue, mRightKnee, "AxisPoint1")->set(-legOffset, tibiaPosY + tibiaHeight/2.0 - 0.016, legOffsetZ - 0.0135 * sizeScale);
	PARAM(Vector3DValue, mRightKnee, "AxisPoint2")->set(-legOffset + 0.01, tibiaPosY + tibiaHeight/2.0 - 0.016, legOffsetZ - 0.0135 * sizeScale);
	PARAM(DoubleValue, mRightKnee, "MinAngle")->set(0);
	PARAM(DoubleValue, mRightKnee, "MaxAngle")->set(150);


	//15: left knee
	PARAM(Vector3DValue, mLeftKnee, "AxisPoint1")->set(legOffset, tibiaPosY + tibiaHeight/2.0 - 0.016,legOffsetZ - 0.0135 * sizeScale);
	PARAM(Vector3DValue, mLeftKnee, "AxisPoint2")->set(legOffset - 0.01, tibiaPosY + tibiaHeight/2.0 - 0.016, legOffsetZ - 0.0135 * sizeScale);
	PARAM(DoubleValue, mLeftKnee, "MinAngle")->set(-150);
	PARAM(DoubleValue, mLeftKnee, "MaxAngle")->set(0);


	// 16:right ankle yaw
	PARAM(Vector3DValue, mRightFootUD, "AxisPoint1")->set(0, height * sizeScale, legOffsetZ);
	PARAM(Vector3DValue, mRightFootUD, "AxisPoint2")->set(0.01, height * sizeScale, legOffsetZ);
	PARAM(DoubleValue, mRightFootUD, "MinAngle")->set(-81);
	PARAM(DoubleValue, mRightFootUD, "MaxAngle")->set(29.5);


	// 17:left ankle yaw
	PARAM(Vector3DValue, mLeftFootUD, "AxisPoint1")->set(0, height * sizeScale, legOffsetZ);
	PARAM(Vector3DValue, mLeftFootUD, "AxisPoint2")->set(-0.01, height * sizeScale, legOffsetZ);
	PARAM(DoubleValue, mLeftFootUD, "MinAngle")->set(-29.5);
	PARAM(DoubleValue, mLeftFootUD, "MaxAngle")->set(81.0);


	// 18: right ankle pitch
	PARAM(Vector3DValue, mRightFootLR, "AxisPoint1")->set(-legOffset, height * sizeScale, -0.011 * sizeScale);
	PARAM(Vector3DValue, mRightFootLR, "AxisPoint2")->set(-legOffset, height * sizeScale, -0.036 * sizeScale - 0.011 * sizeScale);
	PARAM(DoubleValue, mRightFootLR, "MinAngle")->set(-66.5);
	PARAM(DoubleValue, mRightFootLR, "MaxAngle")->set(55.0);


	// 19: left ankle pitch
	PARAM(Vector3DValue, mLeftFootLR, "AxisPoint1")->set(legOffset, height * sizeScale, -0.011 * sizeScale);
	PARAM(Vector3DValue, mLeftFootLR, "AxisPoint2")->set(legOffset, height * sizeScale, -0.036 * sizeScale - 0.011 * sizeScale);
	PARAM(DoubleValue, mLeftFootLR, "MinAngle")->set(-55.0);
	PARAM(DoubleValue, mLeftFootLR, "MaxAngle")->set(66.5);


	// 20: head pan
	PARAM(Vector3DValue, mHeadSide, "AxisPoint1")->set(0.0, neckPosY + 0.0348/2.0 + 0.001, neckPosZ + 0.025 - 0.0115);
	PARAM(Vector3DValue, mHeadSide, "AxisPoint2")->set(0.0, neckPosY + 0.0348/2.0 + 0.001 - 0.1, neckPosZ + 0.025 - 0.0115);
	PARAM(DoubleValue, mHeadSide, "MinAngle")->set(-135);
	PARAM(DoubleValue, mHeadSide, "MaxAngle")->set(135);


	// 21: head tilt
	PARAM(Vector3DValue, mHeadUp, "AxisPoint1")->set(0.0, headBasePosY, headBasePosZ + 0.025 - 0.0115);
	PARAM(Vector3DValue, mHeadUp, "AxisPoint2")->set(-0.1, headBasePosY, headBasePosZ + 0.025 - 0.0115);
	PARAM(DoubleValue, mHeadUp, "MinAngle")->set(-95);
	PARAM(DoubleValue, mHeadUp, "MaxAngle")->set(150);


	QList<SimObject*> accelBoards;

	// Acceleration Sensors:
	//ABFR
	PARAM(Vector3DValue, mAbfr, "LocalPosition")->set(accelOffsetFootX, accelOffsetFootY, accelOffsetFootZ);
	PARAM(QuaternionValue, mAbfr, "LocalOrientation")->setFromAngles(0, 90,0);


	//ABHR
	PARAM(Vector3DValue, mAbhr, "LocalPosition")->set(-0.0088 - accelBoardHeight/2.0 + diffX, diffY + 0.011, diffZ);
	PARAM(QuaternionValue, mAbhr, "LocalOrientation")->setFromAngles(0, 0,90);

	//ABSR
	PARAM(Vector3DValue, mAbsr, "LocalPosition")->set(-(0.1047/2.0 - 0.0064 - accelBoardDepth/2.0) , 0.063/2.0 + accelBoardHeight/2.0 + 0.0005, - 0.038/2.0 - 0.011 + 0.0656/2.0);
	PARAM(QuaternionValue, mAbsr, "LocalOrientation")->setFromAngles(0,0,0);
	

	//ABAR
	PARAM(Vector3DValue, mAbar, "LocalPosition")->set(-(0.0565/2.0 - 0.0065 - 0.026/2.0),0, - 0.032/2.0 - 0.0125/2.0 - 0.0055);
	PARAM(QuaternionValue, mAbar, "LocalOrientation")->setFromAngles(-90,0,0);
	

	//ABFL
	PARAM(Vector3DValue, mAbfl, "LocalPosition")->set(-accelOffsetFootX, accelOffsetFootY, accelOffsetFootZ);
	PARAM(QuaternionValue, mAbfl, "LocalOrientation")->setFromAngles(0, -90,0);


	//ABHL
	PARAM(Vector3DValue, mAbhl, "LocalPosition")->set(0.0088 + accelBoardHeight/2.0 - diffX, diffY + 0.011, diffZ);
	PARAM(QuaternionValue, mAbhl, "LocalOrientation")->setFromAngles(0,0,-90);
	

	//ABSL
	PARAM(Vector3DValue, mAbsl, "LocalPosition")->set((0.05235 - 0.0064 - accelBoardDepth/2.0) , 0.063/2.0 + accelBoardHeight/2.0 + 0.0005, - 0.038/2.0 - 0.011 + 0.0656/2.0);
	PARAM(QuaternionValue, mAbsl, "LocalOrientation")->setFromAngles(0,0,0);

	//ABAL
	PARAM(Vector3DValue, mAbal, "LocalPosition")->set((0.0565/2.0 - 0.0065 - 0.026/2.0),0, - 0.032/2.0 - 0.0125/2.0 - 0.0055);
	PARAM(QuaternionValue, mAbal, "LocalOrientation")->setFromAngles(-90,0,0);

}
