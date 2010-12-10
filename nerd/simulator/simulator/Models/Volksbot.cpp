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

#include "Volksbot.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Collision/CollisionManager.h"
#include "SimulationConstants.h"
#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))

namespace nerd{

Volksbot::Volksbot(const QString &agentName) : ModelInterface(agentName) {
}


Volksbot::Volksbot(const Volksbot &volksbot) : Object(), ValueChangedListener(), EventListener(), 
		ModelInterface(volksbot) 
{
}

void Volksbot::createModel() {
	setName(mNameValue->get());
	PhysicsManager *pm = Physics::getPhysicsManager();

	SimObject *boxBody = (pm->getPrototype(SimulationConstants::PROTOTYPE_BOX_BODY)); 
	SimObject *sphereBody = (pm->getPrototype(SimulationConstants::PROTOTYPE_SPHERE_BODY)); 
	SimObject *servo = (pm->getPrototype("Prototypes/ServoMotor"));
	SimObject *fixed =  (pm->getPrototype(SimulationConstants::PROTOTYPE_FIXED_JOINT));

	QString name = getName();
	QString namePrefix = "/";

	//Agent SimObject group.
	SimObjectGroup *agent = mAgent;
	
	double bodySize = 0.05;

	// Body
	SimObject *body = boxBody->createCopy();
	PARAM(StringValue, body, "Name")->set(namePrefix + "Body");
	PARAM(DoubleValue, body, "Mass")->set(0.5);
	PARAM(Vector3DValue, body, "Position")->set(0.0, bodySize/2.0 + 0.005 , 0.0);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0,0,0);
	PARAM(Vector3DValue, body, "Orientation")->set(0,0,0);
	PARAM(BoolValue, body, "Dynamic")->set(true);
	PARAM(DoubleValue, body, "Width")->set(bodySize);
	PARAM(DoubleValue, body, "Height")->set(bodySize);
	PARAM(DoubleValue, body, "Depth")->set(bodySize);
	PARAM(ColorValue, body, "Color")->set(Color(100,100,100));
	SimBody *anker = dynamic_cast<SimBody*>(body);
	agent->addObject(body);

	SimObject *sphere = sphereBody->createCopy();
	PARAM(StringValue, sphere, "Name")->set(namePrefix + "RightWheel");
	PARAM(DoubleValue, sphere, "Mass")->set(0.1);
	PARAM(Vector3DValue, sphere, "Position")->set(-bodySize/2.0, 0.005 , bodySize/2.0);
	PARAM(BoolValue, sphere, "Dynamic")->set(true);
	PARAM(DoubleValue, sphere, "Radius")->set(0.01);
	PARAM(ColorValue, sphere, "Color")->set(Color(255,0,0));
	SimBody *rightWheel = dynamic_cast<SimBody*>(sphere);
	agent->addObject(sphere);
	
	sphere = sphereBody->createCopy();
	PARAM(StringValue, sphere, "Name")->set(namePrefix + "LeftWheel");
	PARAM(DoubleValue, sphere, "Mass")->set(0.1);
	PARAM(Vector3DValue, sphere, "Position")->set(bodySize/2.0,0.005 , bodySize/2.0);
	PARAM(BoolValue, sphere, "Dynamic")->set(true);
	PARAM(DoubleValue, sphere, "Radius")->set(0.01);
	PARAM(ColorValue, sphere, "Color")->set(Color(255,0,0));
	SimBody *leftWheel = dynamic_cast<SimBody*>(sphere);
	agent->addObject(sphere);
	
	sphere = sphereBody->createCopy();
	PARAM(StringValue, sphere, "Name")->set(namePrefix + "BackWheel");
	PARAM(DoubleValue, sphere, "Mass")->set(0.05);
	PARAM(Vector3DValue, sphere, "Position")->set(0.0, 0.005, -bodySize/2.0);
	PARAM(BoolValue, sphere, "Dynamic")->set(true);
	PARAM(DoubleValue, sphere, "Radius")->set(0.01);
	PARAM(ColorValue, sphere, "Color")->set(Color(255,0,0));
	PARAM(StringValue, sphere, "Material")->set("");
	SimBody *backWheel = dynamic_cast<SimBody*>(sphere);
	agent->addObject(sphere);

	SimObject *jointOne = servo->createCopy();
	PARAM(StringValue, jointOne, "Name")->set(namePrefix + "JointOne");
	PARAM(Vector3DValue, jointOne, "AxisPoint1")->set(-0.0151, 0.005 , bodySize/2.0);
	PARAM(Vector3DValue, jointOne, "AxisPoint2")->set(-1.0151, 0.005 , bodySize/2.0);
	PARAM(BoolValue, jointOne, "IsAngularMotor")->set(false);
	PARAM(StringValue, jointOne, "FirstBody")->set(anker->getAbsoluteName());
	PARAM(StringValue, jointOne, "SecondBody")->set(rightWheel->getAbsoluteName());
	PARAM(DoubleValue, jointOne, "MaxTorque")->set(1.0);
	agent->addObject(jointOne);
	
	SimObject *jointTwo = servo->createCopy();
	PARAM(StringValue, jointTwo, "Name")->set(namePrefix + "JointTwo");
	PARAM(Vector3DValue, jointTwo, "AxisPoint1")->set(1.0151, 0.005 , bodySize/2.0);
	PARAM(Vector3DValue, jointTwo, "AxisPoint2")->set(0.0151, 0.005 , bodySize/2.0);
	PARAM(BoolValue, jointTwo, "IsAngularMotor")->set(false);
	PARAM(StringValue, jointTwo, "FirstBody")->set(anker->getAbsoluteName());
	PARAM(StringValue, jointTwo, "SecondBody")->set(leftWheel->getAbsoluteName());
	PARAM(DoubleValue, jointTwo, "MaxTorque")->set(1.0);
	agent->addObject(jointTwo);
	
	SimObject *fixedJoint = fixed->createCopy();
	PARAM(StringValue, fixedJoint, "Name")->set(namePrefix + "Fixed");
	PARAM(StringValue, fixedJoint, "FirstBody")->set(anker->getAbsoluteName());
	PARAM(StringValue, fixedJoint, "SecondBody")->set(backWheel->getAbsoluteName());
	agent->addObject(fixedJoint);

	pm->addSimObjectGroup(agent);

	CollisionManager *collisionManager = Physics::getCollisionManager();

	collisionManager->disableCollisions(anker->getCollisionObjects(), 
		rightWheel->getCollisionObjects(), true);
	collisionManager->disableCollisions(anker->getCollisionObjects(), 
		leftWheel->getCollisionObjects(), true);
	collisionManager->disableCollisions(anker->getCollisionObjects(),
		backWheel->getCollisionObjects(), true);
	mSimObjects = agent->getObjects();

}

SimObject* Volksbot::createCopy() const {
	Volksbot *copy = new Volksbot("Default");
	return copy;
}
}
