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


#include "ConfigurablePendulum.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include "Physics/SimBody.h"
#include "Physics/SimObject.h"
#include "Value/ColorValue.h"
#include "Value/Vector3DValue.h"
#include "Physics/SphereGeom.h"
#include "Collision/CollisionObject.h"


#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))

#define INIT_PARAMS SimObject *__paraObj = 0;

#define MAKE_CURRENT(paraObj) __paraObj = paraObj;

#define SETP(paraName, value) \
		{ if(__paraObj == 0) { Core::log("No Current Param found", true); } \
		else { Value *__v = __paraObj->getParameter(paraName); \
			if(__v == 0) { Core::log("Param Problem", true); } \
			else { __v->setValueFromString(value); }}}

using namespace std;

namespace nerd {

/**
 * Constructs a new ConfigurablePendulum.
 */
ConfigurablePendulum::ConfigurablePendulum(const QString &groupName, int motorCount, double weightMass, double rodLength)
	: ModelInterface(groupName)
{
	mMotorCount = new IntValue(motorCount);
	addParameter("MotorCount", mMotorCount);

	mWeightMass = new DoubleValue(weightMass);
	addParameter("WeightMass", mWeightMass);

	mRodLength = new DoubleValue(rodLength);
	addParameter("RodLength", mRodLength);

	mPosition->addValueChangedListener(this);
}


/**
 * Copy constructor.
 *
 * @param other the ConfigurablePendulum object to copy.
 */
ConfigurablePendulum::ConfigurablePendulum(const ConfigurablePendulum &other)
	: Object(), ValueChangedListener(), EventListener(), ModelInterface(other),
	  mMotorCount(other.mMotorCount), mWeightMass(other.mWeightMass), mRodLength(other.mRodLength)
{
	mMotorCount = dynamic_cast<IntValue*>(getParameter("MotorCount"));
	mWeightMass = dynamic_cast<DoubleValue*>(getParameter("WeightMass"));
	mRodLength = dynamic_cast<DoubleValue*>(getParameter("RodLength"));

	mPosition->addValueChangedListener(this);
}

/**
 * Destructor.
 */
ConfigurablePendulum::~ConfigurablePendulum() {
}


SimObject* ConfigurablePendulum::createCopy() const {
	ConfigurablePendulum *copy = new ConfigurablePendulum("Default", mMotorCount->get(), mWeightMass->get(), mRodLength->get());
	return copy;
}

void ConfigurablePendulum::createModel() {

	mGroupName = getName();
	mNamePrefix = "/";

	QString groupPrefix = "/";
	groupPrefix.append(mGroupName).append("/");

	// load object and motor templates
	PhysicsManager *pm = Physics::getPhysicsManager();
	SimObject *dummy;

	SimObject *boxBody = pm->getPrototype(
			SimulationConstants::PROTOTYPE_BOX_BODY);
	SimObject *sphereBody = pm->getPrototype(
			SimulationConstants::PROTOTYPE_SPHERE_BODY);
	SimObject *fixedJoint = pm->getPrototype(
			SimulationConstants::PROTOTYPE_FIXED_JOINT);

	SimObject *motor;
	switch(mMotorCount->get())
	{
	case 1: motor = pm->getPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_1); break;
	case 2: motor = pm->getPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_2); break;
	case 3: motor = pm->getPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_3); break;
	case 4: motor = pm->getPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_4); break;
	default: motor = pm->getPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_0);
	}

	SimObjectGroup *agent = mAgent;

	// rod
	mRod = dynamic_cast<BoxBody*>(boxBody->createCopy());
	PARAM(StringValue, mRod, "Name")->set(mNamePrefix + ("Rod"));
	PARAM(BoolValue, mRod, "Dynamic")->set(true);
	agent->addObject(mRod);


	// weight
	mWeight = dynamic_cast<SphereBody*>(sphereBody->createCopy());
	PARAM(StringValue, mWeight, "Name")->set(mNamePrefix + ("Weight"));
	PARAM(BoolValue, mWeight, "Dynamic")->set(true);
	agent->addObject(mWeight);


	// fix weight at rod
	dummy = fixedJoint->createCopy();
	PARAM(StringValue, dummy, "Name")->set(mNamePrefix + ("RodWeightConnector"));
	PARAM(StringValue, dummy, "FirstBody")->set(groupPrefix + "Rod");
	PARAM(StringValue, dummy, "SecondBody")->set(groupPrefix + "Weight");
	agent->addObject(dummy);


	// base for the rod
	mBase = dynamic_cast<BoxBody*>(boxBody->createCopy());
	PARAM(StringValue, mBase, "Name")->set(mNamePrefix + "Base");
	PARAM(BoolValue, mBase, "Dynamic")->set(false);
	agent->addObject(mBase);

	// connect rod to base
	mMotor = motor->createCopy();
	PARAM(StringValue, mMotor, "Name")->set(mNamePrefix + ("Motor"));
	PARAM(StringValue, mMotor, "FirstBody")->set(groupPrefix + "Rod");
	PARAM(StringValue, mMotor, "SecondBody")->set(groupPrefix + "Base");
	agent->addObject(mMotor);

	pm->addSimObjectGroup(agent);

	disableCollisions(mBase, mRod);
	disableCollisions(mRod, mWeight);

	//set parameters
	// some params
	Color color(128, 128, 128, 255);
	double baseHeight = 0.1;
	double weightRadius = 0.1;

	double rodY = baseHeight/2 + mRodLength->get()/2;
	double weightY = rodY + mRodLength->get()/2 + weightRadius;

	// base
	PARAM(Vector3DValue, mBase, "Position")->set(0.0, 0.0, 0.0);
	PARAM(Vector3DValue, mBase, "CenterOfMass")->set(0.0, baseHeight/2, 0.0);
	PARAM(DoubleValue, mBase, "Width")->set(1.0);
	PARAM(DoubleValue, mBase, "Height")->set(baseHeight);
	PARAM(DoubleValue, mBase, "Depth")->set(1.0);
	PARAM(DoubleValue, mBase, "Mass")->set(100.0);
	PARAM(ColorValue, mBase, "Color")->set(color);
	PARAM(StringValue, mBase, "Material")->set("ABS");

	// rod
	PARAM(Vector3DValue, mRod, "Position")->set(0.0, rodY, 0.0);
	PARAM(Vector3DValue, mRod, "CenterOfMass")->set(0.0, rodY, 0.0);
	PARAM(DoubleValue, mRod, "Width")->set(0.05);
	PARAM(DoubleValue, mRod, "Height")->set(mRodLength->get());
	PARAM(DoubleValue, mRod, "Depth")->set(0.05);
	PARAM(DoubleValue, mRod, "Mass")->set(0.001);
	PARAM(ColorValue, mRod, "Color")->set(color);
	PARAM(StringValue, mRod, "Material")->set("ABS");

	// weight
	PARAM(Vector3DValue, mWeight, "Position")->set(0.0, weightY, 0.0);
	PARAM(Vector3DValue, mWeight, "CenterOfMass")->set(0.0, weightY, 0.0);
	PARAM(DoubleValue, mWeight, "Radius")->set(weightRadius);
	PARAM(DoubleValue, mWeight, "Mass")->set(0.05);
	PARAM(ColorValue, mWeight, "Color")->set(color);
	PARAM(StringValue, mWeight, "Material")->set("ABS");

	// motor
	PARAM(Vector3DValue, mMotor, "AxisPoint1")->set(0.1, baseHeight, 0.0);
	PARAM(Vector3DValue, mMotor, "AxisPoint2")->set(0.0, baseHeight, 0.0);

}

void ConfigurablePendulum::layoutObjects() {
	
}

/**
 * Disable collisions between all collision objects of body1 and body2. This method is save
 * when called with NULL pointers.
 */
void ConfigurablePendulum::disableCollisions(SimBody *body1, SimBody *body2) {
	if(body1 != 0 && body2 != 0) {
		CollisionManager *collisionManager = Physics::getCollisionManager();

		collisionManager->disableCollisions(body1->getCollisionObjects(),
		body2->getCollisionObjects(), true);
	}
}

}
