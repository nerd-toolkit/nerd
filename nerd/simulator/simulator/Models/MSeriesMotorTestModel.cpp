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


#include "MSeriesMotorTestModel.h"
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


MSeriesMotorTestModel::MSeriesMotorTestModel(const QString &groupName) : ModelInterface(groupName) {
	mPosition->addValueChangedListener(this);

}

MSeriesMotorTestModel::MSeriesMotorTestModel(const MSeriesMotorTestModel &model) : Object(), ValueChangedListener(), 
		EventListener(), ModelInterface(model) 
{
	mPosition->addValueChangedListener(this);

}

MSeriesMotorTestModel::~MSeriesMotorTestModel(){
}

void MSeriesMotorTestModel::layoutObjects() {

}

void MSeriesMotorTestModel::createModel() {
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
	double mLongArmHeight = 0.50;
	double mLongArmDepth = 0.002;
	double mLongArmWidth = 0.025;
	double mLongArmWeight = 0.065;

	mLongArm = dynamic_cast<BoxBody*>(boxBody->createCopy());
	PARAM(StringValue, mLongArm, "Name")->set(mNamePrefix + "LongArm");
	PARAM(BoolValue, mLongArm, "Dynamic")->set(true);
	PARAM(Vector3DValue, mLongArm, "Position")->set(0.0, mFirstBoxSize + mLongArmHeight / 2.0 - 0.017 / 2.0, 0.0);
	PARAM(Vector3DValue, mLongArm, "CenterOfMass")->set(0.0, 0.0, 0.0);
	PARAM(DoubleValue, mLongArm, "Width")->set(mLongArmWidth);
	PARAM(DoubleValue, mLongArm, "Height")->set(mLongArmHeight);
	PARAM(DoubleValue, mLongArm, "Depth")->set(mLongArmDepth);
	PARAM(DoubleValue, mLongArm, "Mass")->set(mLongArmWeight);
	PARAM(ColorValue, mLongArm, "Color")->set(Color(255, 255, 0, 230));
	PARAM(StringValue, mLongArm, "Material")->set("ABS");

	agent->addObject(mLongArm);

	// Add motor
	QString groupPrefix = "/";
	groupPrefix.append(mGroupName).append("/");

	mMotor = dynamic_cast<TorqueDynamixelMotorAdapter*>(testMotor->createCopy());

	PARAM(StringValue, mMotor, "Name")->set(mNamePrefix + "TestMotor");
	PARAM(StringValue, mMotor, "FirstBody")->set(groupPrefix + "FirstBox");
	PARAM(StringValue, mMotor, "SecondBody")->set(groupPrefix + "LongArm");
	PARAM(Vector3DValue, mMotor, "AxisPoint1")->set(0.0, mFirstBoxSize, 0.1);
	PARAM(Vector3DValue, mMotor, "AxisPoint2")->set(0.0, mFirstBoxSize, 0.0);
	PARAM(DoubleValue, mMotor, "MinAngle")->set(-150);
	PARAM(DoubleValue, mMotor, "MaxAngle")->set(150);

	agent->addObject(mMotor);

	// Disable collision between objects which are allowed to interpenetrate
	CollisionManager *collisionManager = Physics::getCollisionManager();

	collisionManager->disableCollisions(mFirstBox->getCollisionObjects(), 
		mLongArm->getCollisionObjects(), true);

	mSimObjects = agent->getObjects();

	//add agent SimObjectGroup at PhysicsManager to provide the interface to the robot.
	pm->addSimObjectGroup(agent);

}

SimObject* MSeriesMotorTestModel::createCopy() const {
	MSeriesMotorTestModel *copy = new MSeriesMotorTestModel("Default");
	return copy;
}
