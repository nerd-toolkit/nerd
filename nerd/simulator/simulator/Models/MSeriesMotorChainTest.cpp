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


#include "MSeriesMotorChainTest.h"
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


MSeriesMotorChainTest::MSeriesMotorChainTest(const QString &groupName) : ModelInterface(groupName) {
	mPosition->addValueChangedListener(this);

}

MSeriesMotorChainTest::MSeriesMotorChainTest(const MSeriesMotorChainTest &model) : Object(), ValueChangedListener(), 
		EventListener(), ModelInterface(model) 
{
	mPosition->addValueChangedListener(this);

}

MSeriesMotorChainTest::~MSeriesMotorChainTest(){
}

void MSeriesMotorChainTest::layoutObjects() {

}

void MSeriesMotorChainTest::createModel() {
	mGroupName = getName();

	mNamePrefix = "/";

	PhysicsManager *pm = Physics::getPhysicsManager();

//	SimObject *boxInertiaBody = pm->getPrototype(
//			SimulationConstants::PROTOTYPE_BOX_INERTIA_BODY); 

	SimObject *testMotor = pm->getPrototype(
			SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_TEST);

	SimObject *boxBody = pm->getPrototype(
			SimulationConstants::PROTOTYPE_BOX_BODY); 

//	SimObject *cylinderBody = pm->getPrototype(
//			SimulationConstants::PROTOTYPE_CYLINDER_BODY); 

	//Agent SimObject group.
	SimObjectGroup *agent = mAgent;

	// First box
	double mFirstBoxSize = 0.056;

	mFirstBox = dynamic_cast<BoxBody*>(boxBody->createCopy());
	PARAM(StringValue, mFirstBox, "Name")->set(mNamePrefix + "FirstBox");
	PARAM(BoolValue, mFirstBox, "Dynamic")->set(false);
	PARAM(Vector3DValue, mFirstBox, "Position")->set(0.0, mFirstBoxSize / 2.0, 0.0);
	PARAM(Vector3DValue, mFirstBox, "CenterOfMass")->set(0.0, 0.0, 0.0);
	PARAM(DoubleValue, mFirstBox, "Width")->set(mFirstBoxSize);
	PARAM(DoubleValue, mFirstBox, "Height")->set(mFirstBoxSize);
	PARAM(DoubleValue, mFirstBox, "Depth")->set(mFirstBoxSize);
	PARAM(DoubleValue, mFirstBox, "Mass")->set(0.1);
	PARAM(ColorValue, mFirstBox, "Color")->set(Color(255, 255, 255, 230));
	PARAM(StringValue, mFirstBox, "Material")->set("ABS");

	agent->addObject(mFirstBox);

	// Long Arm
	double armHeight = 0.50;
	double armDepth = 0.002;
	double armWidth = 0.025;
	double armWeight = 0.3;

	mArm1 = dynamic_cast<BoxBody*>(boxBody->createCopy());
	PARAM(StringValue, mArm1, "Name")->set(mNamePrefix + "Arm1");
	PARAM(BoolValue, mArm1, "Dynamic")->set(true);
	PARAM(Vector3DValue, mArm1, "Position")->set(0.0, mFirstBoxSize + armHeight / 2.0, 0.0);
	PARAM(Vector3DValue, mArm1, "CenterOfMass")->set(0.0, 0.0, 0.0);
	PARAM(DoubleValue, mArm1, "Width")->set(armWidth);
	PARAM(DoubleValue, mArm1, "Height")->set(armHeight);
	PARAM(DoubleValue, mArm1, "Depth")->set(armDepth);
	PARAM(DoubleValue, mArm1, "Mass")->set(armWeight);
	PARAM(ColorValue, mArm1, "Color")->set(Color(255, 255, 0, 230));
	PARAM(StringValue, mArm1, "Material")->set("ABS");

	agent->addObject(mArm1);

	mArm2 = dynamic_cast<BoxBody*>(boxBody->createCopy());
	PARAM(StringValue, mArm2, "Name")->set(mNamePrefix + "Arm2");
	PARAM(BoolValue, mArm2, "Dynamic")->set(true);
	PARAM(Vector3DValue, mArm2, "Position")->set(0.0, mFirstBoxSize + armHeight + armHeight / 2.0, 0.0);
	PARAM(Vector3DValue, mArm2, "CenterOfMass")->set(0.0, 0.0, 0.0);
	PARAM(DoubleValue, mArm2, "Width")->set(armWidth);
	PARAM(DoubleValue, mArm2, "Height")->set(armHeight);
	PARAM(DoubleValue, mArm2, "Depth")->set(armDepth);
	PARAM(DoubleValue, mArm2, "Mass")->set(armWeight);
	PARAM(ColorValue, mArm2, "Color")->set(Color(255, 255, 0, 230));
	PARAM(StringValue, mArm2, "Material")->set("ABS");

	agent->addObject(mArm2);

	mArm3 = dynamic_cast<BoxBody*>(boxBody->createCopy());
	PARAM(StringValue, mArm3, "Name")->set(mNamePrefix + "Arm3");
	PARAM(BoolValue, mArm3, "Dynamic")->set(true);
	PARAM(Vector3DValue, mArm3, "Position")->set(0.0, mFirstBoxSize + 2 * armHeight + armHeight / 2.0, 0.0);
	PARAM(Vector3DValue, mArm3, "CenterOfMass")->set(0.0, 0.0, 0.0);
	PARAM(DoubleValue, mArm3, "Width")->set(armWidth);
	PARAM(DoubleValue, mArm3, "Height")->set(armHeight);
	PARAM(DoubleValue, mArm3, "Depth")->set(armDepth);
	PARAM(DoubleValue, mArm3, "Mass")->set(armWeight);
	PARAM(ColorValue, mArm3, "Color")->set(Color(255, 255, 0, 230));
	PARAM(StringValue, mArm3, "Material")->set("ABS");

	agent->addObject(mArm3);

	// Add motor
	QString groupPrefix = "/";
	groupPrefix.append(mGroupName).append("/");

	mMotor1 = dynamic_cast<TorqueDynamixelMotorAdapter*>(testMotor->createCopy());

	PARAM(StringValue, mMotor1, "Name")->set(mNamePrefix + "TestMotor1");
	PARAM(StringValue, mMotor1, "FirstBody")->set(groupPrefix + "FirstBox");
	PARAM(StringValue, mMotor1, "SecondBody")->set(groupPrefix + "Arm1");
	PARAM(Vector3DValue, mMotor1, "AxisPoint1")->set(0.0, mFirstBoxSize, 0.1);
	PARAM(Vector3DValue, mMotor1, "AxisPoint2")->set(0.0, mFirstBoxSize, 0.0);
	PARAM(DoubleValue, mMotor1, "MinAngle")->set(-360);
	PARAM(DoubleValue, mMotor1, "MaxAngle")->set(360);

	agent->addObject(mMotor1);

	mMotor2 = dynamic_cast<TorqueDynamixelMotorAdapter*>(testMotor->createCopy());

	PARAM(StringValue, mMotor2, "Name")->set(mNamePrefix + "TestMotor2");
	PARAM(StringValue, mMotor2, "FirstBody")->set(groupPrefix + "Arm1");
	PARAM(StringValue, mMotor2, "SecondBody")->set(groupPrefix + "Arm2");
	PARAM(Vector3DValue, mMotor2, "AxisPoint1")->set(0.0, mFirstBoxSize + armHeight, 0.1);
	PARAM(Vector3DValue, mMotor2, "AxisPoint2")->set(0.0, mFirstBoxSize + armHeight, 0.0);
	PARAM(DoubleValue, mMotor2, "MinAngle")->set(-360);
	PARAM(DoubleValue, mMotor2, "MaxAngle")->set(360);

	agent->addObject(mMotor2);

	mMotor3 = dynamic_cast<TorqueDynamixelMotorAdapter*>(testMotor->createCopy());

	PARAM(StringValue, mMotor3, "Name")->set(mNamePrefix + "TestMotor3");
	PARAM(StringValue, mMotor3, "FirstBody")->set(groupPrefix + "Arm2");
	PARAM(StringValue, mMotor3, "SecondBody")->set(groupPrefix + "Arm3");
	PARAM(Vector3DValue, mMotor3, "AxisPoint1")->set(0.0, mFirstBoxSize + 2 * armHeight, 0.1);
	PARAM(Vector3DValue, mMotor3, "AxisPoint2")->set(0.0, mFirstBoxSize + 2 * armHeight, 0.0);
	PARAM(DoubleValue, mMotor3, "MinAngle")->set(-360);
	PARAM(DoubleValue, mMotor3, "MaxAngle")->set(360);

	agent->addObject(mMotor3);

	// Disable collision between objects which are allowed to interpenetrate
	CollisionManager *collisionManager = Physics::getCollisionManager();

	collisionManager->disableCollisions(mFirstBox->getCollisionObjects(), 
		mArm1->getCollisionObjects(), true);
	collisionManager->disableCollisions(mFirstBox->getCollisionObjects(), 
		mArm2->getCollisionObjects(), true);
	collisionManager->disableCollisions(mArm1->getCollisionObjects(), 
		mArm2->getCollisionObjects(), true);
	collisionManager->disableCollisions(mArm2->getCollisionObjects(), 
		mArm3->getCollisionObjects(), true);
	collisionManager->disableCollisions(mArm1->getCollisionObjects(), 
		mArm3->getCollisionObjects(), true);
	collisionManager->disableCollisions(mFirstBox->getCollisionObjects(), 
		mArm3->getCollisionObjects(), true);

	mSimObjects = agent->getObjects();

	//add agent SimObjectGroup at PhysicsManager to provide the interface to the robot.
	pm->addSimObjectGroup(agent);

}

SimObject* MSeriesMotorChainTest::createCopy() const {
	MSeriesMotorChainTest *copy = new MSeriesMotorChainTest("Default");
	return copy;
}
