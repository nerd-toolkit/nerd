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


#include "InvertedPendulum.h"
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
 * Constructs a new InvertedPendulum.
 */
InvertedPendulum::InvertedPendulum(const QString &groupName)
	: ModelInterface(groupName), mGroupName(groupName)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the InvertedPendulum object to copy.
 */
InvertedPendulum::InvertedPendulum(const InvertedPendulum &other) 
	: Object(), ValueChangedListener(), EventListener(), ModelInterface(other), mGroupName(other.mGroupName)
{
}

/**
 * Destructor.
 */
InvertedPendulum::~InvertedPendulum() {
}


SimObject* InvertedPendulum::createCopy() const {
	return new InvertedPendulum(*this);
}

void InvertedPendulum::createModel() {

	PhysicsManager *pm = Physics::getPhysicsManager();

	mNamePrefix = "/";
	mGroupPrefix = QString("/").append(mGroupName);
	
	mStickLength = 0.2;
	mHalfBodyWidth = 0.015;
	mBodyLength = 0.1;

	createCart();

	mSimObjects = mAgent->getObjects();
	pm->addSimObjectGroup(mAgent);

}

void InvertedPendulum::layoutObjects() {

}

void InvertedPendulum::createCart() {

	CollisionManager *cm = Physics::getCollisionManager();
	PhysicsManager *pm = Physics::getPhysicsManager();
	SimObject *box = pm->getPrototype(SimulationConstants::PROTOTYPE_BOX_BODY);
	SimObject *cylinder = pm->getPrototype(SimulationConstants::PROTOTYPE_CYLINDER_BODY);
	SimObject *servo = pm->getPrototype(SimulationConstants::PROTOTYPE_SERVO_MOTOR);
	SimObject *fix = pm->getPrototype(SimulationConstants::PROTOTYPE_FIXED_JOINT);
	SimObject *hinge = pm->getPrototype(SimulationConstants::PROTOTYPE_HINGE_JOINT);
	SimObject *other;

	INIT_PARAMS;

	SimBody *body = dynamic_cast<SimBody*>(box->createCopy());
	mAgent->addObject(body);
	MAKE_CURRENT(body);
	SETP("Name", mNamePrefix + "Cart");
	SETP("Mass", QString::number(0.05));
	SETP("Position", Vector3DValue(0.0, mHalfBodyWidth, 0.0).getValueAsString());
	SETP("CenterOfMass", "(0,0,0)");
	SETP("Orientation", "(0,0,0)");
	SETP("Dynamic", "true");
	SETP("Width", DoubleValue(mBodyLength).getValueAsString());
	SETP("Height", DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
	SETP("Depth",  DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
	SETP("Color", "red");
	SETP("Material", "ABS");

	other = cylinder->createCopy();
	mAgent->addObject(other);
	MAKE_CURRENT(other);
	SETP("Name", mNamePrefix + "RightWheel");
	SETP("Color", "(96, 96, 96)");
	SETP("Position", Vector3DValue(mBodyLength / 2.0, -0.01, 0.0).getValueAsString());
	SETP("Radius", "0.01");
	SETP("Length", DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
	SETP("Orientation", "(0, 0, 0)");
	SETP("Mass", "0.01");
	SETP("CenterOfMass", "(0, 0, 0)");
	SETP("Dynamic", "true");
	
	cm->disableCollisions(
			dynamic_cast<SimBody*>(body)->getCollisionObjects(),
			dynamic_cast<SimBody*>(other)->getCollisionObjects(),
			true);

	other = cylinder->createCopy();
	mAgent->addObject(other);
	MAKE_CURRENT(other);
	SETP("Name", mNamePrefix + "LeftWheel");
	SETP("Color", "(96, 96, 96)");
	SETP("Position", Vector3DValue(-mBodyLength / 2.0, -0.01, 0.0).getValueAsString());
	SETP("Radius", "0.01");
	SETP("Length", DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
	SETP("Orientation", "(0, 0, 0)");
	SETP("Mass", "0.01");
	SETP("CenterOfMass", "(0, 0, 0)");
	SETP("Dynamic", "true");
	
	other = servo->createCopy();
	mAgent->addObject(other);
	MAKE_CURRENT(other);
	SETP("MaxForce", "10.0");
	SETP("MaxTorque", "10.0");
	PARAM(StringValue, other, "Name")->set("/RightMotor");
	PARAM(Vector3DValue, other, "AxisPoint1")->set(mBodyLength / 2.0, -0.01, -1);
	PARAM(Vector3DValue, other, "AxisPoint2")->set(mBodyLength / 2.0, -0.01, 1);
	PARAM(BoolValue, other, "IsAngularMotor")->set(false);
	PARAM(StringValue, other, "FirstBody")->set(mGroupPrefix + "/Cart");
	PARAM(StringValue, other, "SecondBody")->set(mGroupPrefix + "/RightWheel");

	other = servo->createCopy();
	mAgent->addObject(other);
	MAKE_CURRENT(other);
	SETP("MaxForce", "10.0");
	SETP("MaxTorque", "10.0");
	PARAM(StringValue, other, "Name")->set("/LeftMotor");
	PARAM(Vector3DValue, other, "AxisPoint1")->set(-mBodyLength / 2.0, -0.01, -1);
	PARAM(Vector3DValue, other, "AxisPoint2")->set(-mBodyLength / 2.0, -0.01, 1);
	PARAM(BoolValue, other, "IsAngularMotor")->set(false);
	PARAM(StringValue, other, "FirstBody")->set(mGroupPrefix + "/Cart");
	PARAM(StringValue, other, "SecondBody")->set(mGroupPrefix + "/LeftWheel");


	SimObject *stick = box->createCopy();
	mAgent->addObject(stick);
	MAKE_CURRENT(stick);
	SETP("Name", mNamePrefix + "Stick");
	SETP("Mass", QString::number(0.00002));
	SETP("Position", Vector3DValue(0.0, mHalfBodyWidth * 2.0 + mStickLength / 2.0, 0.0).getValueAsString());
	SETP("CenterOfMass", "(0,0,0)");
	SETP("Orientation", "(0,0,0)");
	SETP("Dynamic", "true");
	SETP("Width", DoubleValue(0.005).getValueAsString());
	SETP("Height", DoubleValue(mStickLength).getValueAsString());
	SETP("Depth",  DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
	SETP("Color", "blue");
	
	other = hinge->createCopy();
	mAgent->addObject(other);
	PARAM(StringValue, other, "Name")->set("/StickToBody");
	PARAM(Vector3DValue, other, "AxisPoint1")->set(0, mHalfBodyWidth * 2.0, -1);
	PARAM(Vector3DValue, other, "AxisPoint2")->set(0, mHalfBodyWidth * 2.0, 1);
	PARAM(StringValue, other, "FirstBody")->set(mGroupPrefix + "/Cart");
	PARAM(StringValue, other, "SecondBody")->set(mGroupPrefix + "/Stick");

	SimObject *bob = cylinder->createCopy();
	mAgent->addObject(bob);
	MAKE_CURRENT(bob);
	SETP("Name", mNamePrefix + "Bob");
	SETP("Position", Vector3DValue(0, mHalfBodyWidth * 2.0 + mStickLength, 0.0).getValueAsString());
	SETP("Radius", "0.01");
	SETP("Length", DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
	SETP("Orientation", "(0, 0, 0)");
	SETP("Mass", "0.0005");
	SETP("CenterOfMass", "(0, 0, 0)");
	SETP("Dynamic", "true");
	SETP("Color", "blue");

	other = fix->createCopy();
	mAgent->addObject(other);
	PARAM(StringValue, other, "Name")->set("/BobFix");
	PARAM(StringValue, other, "FirstBody")->set(mGroupPrefix + "/Stick");
	PARAM(StringValue, other, "SecondBody")->set(mGroupPrefix + "/Bob");

	cm->disableCollisions(
			dynamic_cast<SimBody*>(body)->getCollisionObjects(),
			dynamic_cast<SimBody*>(stick)->getCollisionObjects(),
			true);

	cm->disableCollisions(
			dynamic_cast<SimBody*>(stick)->getCollisionObjects(),
			dynamic_cast<SimBody*>(bob)->getCollisionObjects(),
			true);
}


}



