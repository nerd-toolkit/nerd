/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   Nehring library by Jan Nehring (Diploma Thesis)                       *
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

#include "JanPrey.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Collision/CollisionManager.h"
#include "SimulationConstants.h"
#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))

namespace nerd{

JanPrey::JanPrey(const QString &agentName) : ModelInterface(agentName) {
	mAmountOfFood = new DoubleValue( 0 );
	addParameter( "AmountOfFood", mAmountOfFood );
}


JanPrey::JanPrey(const JanPrey &volksbot) : Object(), ValueChangedListener(), EventListener(), 
		ModelInterface(volksbot) 
{
	mAmountOfFood = dynamic_cast<DoubleValue*>(getParameter( "AmountOfFood" ));
}

void JanPrey::createModel() {
	PhysicsManager *pm = Physics::getPhysicsManager();
	CollisionManager *cm = Physics::getCollisionManager();
	
	Quaternion p(0.7071067811865476, 0.7071067811865475, 0, 0);
	double lightSensorNoise = 0.0;
	double lightSensorOffset = 0.025;
	double distanceSensorNoise = 0.0;
	double distanceSensorOffset = 0.025;
	double distanceSensorAngle = 0;
	double distanceSensorRange = 0.08;
	int distanceSensorRays = 1;
	double bodyAboveGround = 0.0015;
	double bodyHeight = 0.03;
	double bodyRadius = 0.0275;
	Color active(255, 0, 0, 255);
	Color inactive(255, 255, 255, 255);
	
	
	SimObject *boxBody = (pm->getPrototype(SimulationConstants::PROTOTYPE_BOX_BODY)); 
	SimObject *sphereBody = (pm->getPrototype(SimulationConstants::PROTOTYPE_SPHERE_BODY)); 
	SimObject *servo = (pm->getPrototype("Prototypes/ServoMotor"));
	SimObject *fixed =  (pm->getPrototype(SimulationConstants::PROTOTYPE_FIXED_JOINT));

	SimObject *light 
		= pm->getPrototype(SimulationConstants::PROTOTYPE_LIGHT_SENSOR);
	SimObject *food
		= pm->getPrototype("Prototypes/FoodSensor");
	SimObject *distance
		= pm->getPrototype( SimulationConstants::PROTOTYPE_DISTANCE_SENSOR );
	
	SimObject *other = 0;
	
	QString pathName = "/";
	pathName = pathName.append(getName());
	QString namePrefix = "/";

	//Agent SimObject group.
	SimObjectGroup *agent = new SimObjectGroup(getName(), "Agent");
	
	double bodySize = 0.05;

	// Body
	SimObject *body = boxBody->createCopy();
	PARAM(StringValue, body, "Name")->set( namePrefix + "Body");
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
	
	other = food->createCopy();
	agent->addObject( other );
	PARAM(StringValue, other, "Name")->set("/FoodLeft");
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
	PARAM(Vector3DValue, other, "LocalPosition")
		->set(bodySize/2.0,0.005 , bodySize/2.0);
	PARAM(Vector3DValue, other, "LocalOrientation")
		->set(0.0, 60.0, 0.0);
	PARAM(DoubleValue, other, "Noise")->set(lightSensorNoise);

	other = food->createCopy();
	agent->addObject( other );
	PARAM(StringValue, other, "Name")->set("/FoodRight");
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
	PARAM(Vector3DValue, other, "LocalPosition")
		->set(-bodySize/2.0,0.005 , bodySize/2.0);
	PARAM(Vector3DValue, other, "LocalOrientation")
		->set(0.0, 60.0, 0.0);
	PARAM(DoubleValue, other, "Noise")->set(lightSensorNoise);

// 	other = distance->createCopy();
// 	agent->addObject( other );
// 	PARAM(StringValue, other, "Name")->set("/DistanceLeft");
// 	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
// 	PARAM(Vector3DValue, other, "LocalPosition")->set(bodySize/2.0,0.005 , bodySize/2.0);
// 	PARAM(QuaternionValue, other, "LocalOrientation")
// 		->set(Quaternion(0, 0, 0, 0)
// 		* p);
// 	PARAM(DoubleValue, other, "AngleOfAperture")->set(distanceSensorAngle);
// 	PARAM(DoubleValue, other, "MaxRange")
// 			->set(distanceSensorRange +bodyRadius);
// 	PARAM(DoubleValue, other, "MinRange")->set(bodyRadius);
// 	PARAM(IntValue, other, "NumberOfRays")->set(distanceSensorRays);
// 	PARAM(ColorValue, other, "ActiveColor")->set(active);
// 	PARAM(ColorValue, other, "InactiveColor")->set(inactive);
// 	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);

// 	other = distance->createCopy();
// 	agent->addObject( other );
// 	PARAM(StringValue, other, "Name")->set("/DistanceRight");
// 	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
// 	PARAM(Vector3DValue, other, "LocalPosition")->set(bodySize/2.0,0.005 , bodySize/2.0);
// 	PARAM(QuaternionValue, other, "LocalOrientation")
// 		->set(Quaternion(1, 1, 0, 0)
// 		* p);
// 	PARAM(DoubleValue, other, "AngleOfAperture")->set(distanceSensorAngle);
// 	PARAM(DoubleValue, other, "MaxRange")
// 			->set(distanceSensorRange);
// 	PARAM(DoubleValue, other, "MinRange")->set(0);
// 	PARAM(IntValue, other, "NumberOfRays")->set(distanceSensorRays);
// 	PARAM(ColorValue, other, "ActiveColor")->set(active);
// 	PARAM(ColorValue, other, "InactiveColor")->set(inactive);
// 	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);

// 	other = distance->createCopy();
// 	agent->addObject( other );
// 	PARAM(StringValue, other, "Name")->set("/DistanceRight");
// 	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
// 	PARAM(Vector3DValue, other, "LocalPosition")
// 		->set(-bodySize/2.0,0.005 , bodySize/2.0);
// 	PARAM(Vector3DValue, other, "LocalOrientation")
// 		->set(0.0, 60.0, 0.0);
// 	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);
// 	
// 	other = light->createCopy();
// 	agent->addObject( other );
// 	PARAM(StringValue, other, "Name")->set(namePrefix + "Light");
// 	PARAM(StringValue, other, "HostBody")->set( pathName + "/Body");
// 	PARAM(Vector3DValue, other, "LocalPosition")
// 			->set(0, bodyHeight + bodyAboveGround, 0);
// 	PARAM(DoubleValue, other, "Noise")->set(0);
// 	mSimObjects.append(other);
// 	
// 	other = light->createCopy();
// 	PARAM(StringValue, other, "Name")->set(namePrefix + "FoodRight");
// 	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
// 	PARAM(Vector3DValue, other, "LocalPosition")
// 			->set(-lightSensorOffset, bodyHeight + bodyAboveGround, 0);
// 	PARAM(DoubleValue, other, "Noise")->set(lightSensorNoise);
// 	PARAM(DoubleValue, other, "Food")->set( 1.0 );
// 	mSimObjects.append(other);
// 	
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
	PARAM(StringValue, jointOne, "Name")->set(namePrefix + "ServoLeft");
	PARAM(Vector3DValue, jointOne, "AxisPoint1")->set(-0.0151, 0.005 , bodySize/2.0);
	PARAM(Vector3DValue, jointOne, "AxisPoint2")->set(-1.0151, 0.005 , bodySize/2.0);
	PARAM(BoolValue, jointOne, "IsAngularMotor")->set(false);
	PARAM(StringValue, jointOne, "FirstBody")->set(anker->getAbsoluteName());
	PARAM(StringValue, jointOne, "SecondBody")->set(rightWheel->getAbsoluteName());
	PARAM(DoubleValue, jointOne, "MaxTorque")->set(1.0);
	agent->addObject(jointOne);
	
	SimObject *jointTwo = servo->createCopy();
	PARAM(StringValue, jointTwo, "Name")->set(namePrefix + "ServoRight");
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

	CollisionManager *collisionManager = Physics::getCollisionManager();

	collisionManager->disableCollisions(anker->getCollisionObjects(), 
		rightWheel->getCollisionObjects(), true);
	collisionManager->disableCollisions(anker->getCollisionObjects(), 
		leftWheel->getCollisionObjects(), true);
	collisionManager->disableCollisions(anker->getCollisionObjects(),
		backWheel->getCollisionObjects(), true);
	mSimObjects = agent->getObjects();

	for(QListIterator<SimObject*> i(mSimObjects); i.hasNext();) {
		SimObject *sim = i.next();
		agent->addObject(sim);
	}
	pm->addSimObjectGroup(agent);


}

SimObject* JanPrey::createCopy() const {
	JanPrey *copy = new JanPrey("Default");
	return copy;
}
}
