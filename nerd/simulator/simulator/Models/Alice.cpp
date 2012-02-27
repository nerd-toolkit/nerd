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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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

#include "Collision/CollisionManager.h"
#include "Alice.h"
#include "Physics/DistanceSensor.h"
#include "Physics/Physics.h"
#include "SimulationConstants.h"
#include <iostream>
#include "Physics/WavefrontGeom.h"
#include "Physics/WavefrontBody.h"
#include "Physics/BoxGeom.h"
#include "Physics/BoxBody.h"

using namespace std;

#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))
#define PRINTQ(q) std::cout << "(" << q.getW() << ", " << q.getX() << ", " \
		<< q.getY() << ", " << q.getZ() << ")" << std::endl;
#define PRINTV(v) std::cout << "(" << v.getX() << ", " << v.getY() << ", " \
		<< v.getZ() << ")" << std::endl;

namespace nerd {

Alice::Alice(const QString &name)
	: ModelInterface(name)
{
}

Alice::Alice(const Alice &alice)
	: Object(), ValueChangedListener(), EventListener(), ModelInterface(alice)
{
}

void Alice::createModel() {
	CollisionManager *cm = Physics::getCollisionManager();
	PhysicsManager *pm = Physics::getPhysicsManager();

	QString pathName = "/";
	pathName = pathName.append(getName());

	SimObject *cylinder
			= pm->getPrototype(SimulationConstants::PROTOTYPE_CYLINDER_BODY);
	SimObject *distance
			= pm->getPrototype(SimulationConstants::PROTOTYPE_DISTANCE_SENSOR);
	SimObject *fix
			= pm->getPrototype(SimulationConstants::PROTOTYPE_FIXED_JOINT);
	mLightSensorPrototype
			= pm->getPrototype(SimulationConstants::PROTOTYPE_LIGHT_SENSOR);
	SimObject *servo
			= pm->getPrototype(SimulationConstants::PROTOTYPE_SERVO_MOTOR);
	SimObject *sphere
			= pm->getPrototype(SimulationConstants::PROTOTYPE_SPHERE_BODY);
	SimObject *wavefront
			= pm->getPrototype(SimulationConstants::PROTOTYPE_WAVEFRONT_BODY_NO_PHYSICS);

	if(cylinder == 0 || distance == 0 || fix == 0 || wavefront == 0
			|| mLightSensorPrototype == 0 || servo == 0 || sphere == 0) {
		cerr << "Could not find all required prototypes!" << endl;
	}

	InterfaceValue *value = 0;
	SimObject *body = 0;
	SimObject *other = 0;

	// Counteract the rotation of the cylinder body.
	Quaternion p(0.7071067811865476, 0.7071067811865475, 0, 0);
	Color active(255, 0, 0, 255);
	Color inactive(255, 255, 255, 255);
	double bodyAboveGround = 0.0015;
	double bodyHeight = 0.03;
	double bodyRadius = 0.0275;
	double wheelRadius = 0.0065;
	double wheelBreadth = 0.0015;
	double wheelXOffset = 0.025;
	double stabilizerAboveGround = 0.0005;
	double stabilizerDynamicFriction = 0.0;
	double stabilizerStaticFriction = 0.0;
	double stabilizerZOffset = 0.025;
	double distanceSensorAngle = 35.0;
	double distanceSensorNoise = 0.0005;
	double distanceSensorRange = 0.045;
	int distanceSensorRays = 3;

	
	body = cylinder->createCopy();
	mSimObjects.append(body);
	PARAM(StringValue, body, "Name")->set("/Body");
	PARAM(ColorValue, body, "Color")->set(0, 255, 0, 0);
	PARAM(Vector3DValue, body, "Position")
			->set(0, bodyHeight / 2.0 + bodyAboveGround, 0);
	PARAM(DoubleValue, body, "Radius")->set(bodyRadius);
	PARAM(DoubleValue, body, "Length")->set(bodyHeight);
	PARAM(Vector3DValue, body, "Orientation")->set(-90, 0, 0);
	PARAM(DoubleValue, body, "Mass")->set(0.08);
	PARAM(Vector3DValue, body, "CenterOfMass")->set(0, 0, 0);
	PARAM(BoolValue, body, "Dynamic")->set(true);

	//other = wavefront->createCopy();
	other = new WavefrontBody("", "alice_dress.obj", 0.02);
	mSimObjects.append(other);
	PARAM(StringValue, other, "ReferenceObject")->set(pathName + "/Body");
 	PARAM(DoubleValue, other, "Scale")->set(0.02);
 	//PARAM(StringValue, other, "Filename")->set("alice_dress.obj");
	PARAM(StringValue, other, "Name")->set("/AliceDress");
	PARAM(ColorValue, other, "Color")->set(0, 0, 180);
// 	PARAM(Vector3DValue, other, "Position")->set(0, -0.09, 0);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
// 	PARAM(Vector3DValue, other, "Orientation")->set(0, 0, 0);
	PARAM(Vector3DValue, other, "LocalOrientation")->set(90, 180, 0);
	PARAM(DoubleValue, other, "Mass")->set(0.01);
	PARAM(Vector3DValue, other, "CenterOfMass")->set(0, 0, 0);
	PARAM(BoolValue, other, "Dynamic")->set(false);
	cm->disableCollisions(
			dynamic_cast<SimBody*>(body)->getCollisionObjects(),
			dynamic_cast<SimBody*>(other)->getCollisionObjects(),
			true);

	other = new WavefrontBody("", "alice_skirt.obj", 0.02);
	mSimObjects.append(other);
	PARAM(StringValue, other, "ReferenceObject")->set(pathName + "/Body");
 	PARAM(DoubleValue, other, "Scale")->set(0.02);
 	//PARAM(StringValue, other, "Filename")->set("alice_skirt.obj");
	PARAM(StringValue, other, "Name")->set("/AliceSkirt");
	PARAM(ColorValue, other, "Color")->set(255, 255, 255);
// 	PARAM(Vector3DValue, other, "Position")->set(0, -0.09, 0);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
// 	PARAM(Vector3DValue, other, "Orientation")->set(0, 0, 0);
	PARAM(Vector3DValue, other, "LocalOrientation")->set(90, 180, 0);
	PARAM(DoubleValue, other, "Mass")->set(0.01);
	PARAM(Vector3DValue, other, "CenterOfMass")->set(0, 0, 0);
	PARAM(BoolValue, other, "Dynamic")->set(false);
	cm->disableCollisions(
			dynamic_cast<SimBody*>(body)->getCollisionObjects(),
			dynamic_cast<SimBody*>(other)->getCollisionObjects(),
			true);

	other = new WavefrontBody("", "alice_arms.obj", 0.02);
	mSimObjects.append(other);
	PARAM(StringValue, other, "ReferenceObject")->set(pathName + "/Body");
 	PARAM(DoubleValue, other, "Scale")->set(0.02);
 	//PARAM(StringValue, other, "Filename")->set("alice_arms.obj");
	PARAM(StringValue, other, "Name")->set("/AliceArms");
	PARAM(ColorValue, other, "Color")->set(255, 212, 134);
// 	PARAM(Vector3DValue, other, "Position")->set(0, -0.09, 0);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
// 	PARAM(Vector3DValue, other, "Orientation")->set(0, 0, 0);
	PARAM(Vector3DValue, other, "LocalOrientation")->set(90, 180, 0);
	PARAM(DoubleValue, other, "Mass")->set(0.01);
	PARAM(Vector3DValue, other, "CenterOfMass")->set(0, 0, 0);
	PARAM(BoolValue, other, "Dynamic")->set(false);
	cm->disableCollisions(
			dynamic_cast<SimBody*>(body)->getCollisionObjects(),
			dynamic_cast<SimBody*>(other)->getCollisionObjects(),
			true);

	other = cylinder->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/LeftWheel");
	PARAM(ColorValue, other, "Color")->set(96, 96, 96);
	PARAM(Vector3DValue, other, "Position")->set(wheelXOffset, wheelRadius, 0);
	PARAM(DoubleValue, other, "Radius")->set(wheelRadius);
	PARAM(DoubleValue, other, "Length")->set(wheelBreadth);
	PARAM(Vector3DValue, other, "Orientation")->set(0, 90, 0);
	PARAM(DoubleValue, other, "Mass")->set(0.01);
	PARAM(Vector3DValue, other, "CenterOfMass")->set(0, 0, 0);
	PARAM(BoolValue, other, "Dynamic")->set(true);
	cm->disableCollisions(
			dynamic_cast<SimBody*>(body)->getCollisionObjects(),
			dynamic_cast<SimBody*>(other)->getCollisionObjects(),
			true);

	other = cylinder->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/RightWheel");
	PARAM(ColorValue, other, "Color")->set(96, 96, 96);
	PARAM(Vector3DValue, other, "Position")->set(-wheelXOffset, wheelRadius, 0);
	PARAM(DoubleValue, other, "Radius")->set(wheelRadius);
	PARAM(DoubleValue, other, "Length")->set(wheelBreadth);
	PARAM(Vector3DValue, other, "Orientation")->set(0, 90, 0);
	PARAM(DoubleValue, other, "Mass")->set(0.01);
	PARAM(Vector3DValue, other, "CenterOfMass")->set(0, 0, 0);
	PARAM(BoolValue, other, "Dynamic")->set(true);
	cm->disableCollisions(
			dynamic_cast<SimBody*>(body)->getCollisionObjects(),
			dynamic_cast<SimBody*>(other)->getCollisionObjects(),
			true);

	other = sphere->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/FrontStabilizer");
	PARAM(ColorValue, other, "Color")->set(255, 255, 255);
	PARAM(Vector3DValue, other, "Position")
			->set(0, bodyAboveGround, stabilizerZOffset);
	PARAM(DoubleValue, other, "Radius")
			->set(bodyAboveGround - stabilizerAboveGround);
	PARAM(Vector3DValue, other, "Orientation")->set(0, 0, 0);
	PARAM(DoubleValue, other, "Mass")->set(0.01);
	PARAM(Vector3DValue, other, "CenterOfMass")->set(0, 0, 0);
	PARAM(BoolValue, other, "Dynamic")->set(true);
	PARAM(DoubleValue, other, "DynamicFriction")
			->set(stabilizerDynamicFriction);
	PARAM(DoubleValue, other, "StaticFriction")->set(stabilizerStaticFriction);
	cm->disableCollisions(
			dynamic_cast<SimBody*>(body)->getCollisionObjects(),
			dynamic_cast<SimBody*>(other)->getCollisionObjects(),
			true);

	other = sphere->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/BackStabilizer");
	PARAM(ColorValue, other, "Color")->set(255, 255, 255);
	PARAM(Vector3DValue, other, "Position")
			->set(0, bodyAboveGround, -stabilizerZOffset);
	PARAM(DoubleValue, other, "Radius")
			->set(bodyAboveGround - stabilizerAboveGround);
	PARAM(Vector3DValue, other, "Orientation")->set(0, 0, 0);
	PARAM(DoubleValue, other, "Mass")->set(0.01);
	PARAM(Vector3DValue, other, "CenterOfMass")->set(0, 0, 0);
	PARAM(BoolValue, other, "Dynamic")->set(true);
	PARAM(DoubleValue, other, "DynamicFriction")
			->set(stabilizerDynamicFriction);
	PARAM(DoubleValue, other, "StaticFriction")->set(stabilizerStaticFriction);
	cm->disableCollisions(
			dynamic_cast<SimBody*>(body)->getCollisionObjects(),
			dynamic_cast<SimBody*>(other)->getCollisionObjects(),
			true);

	other = servo->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/LeftMotor");
	PARAM(Vector3DValue, other, "AxisPoint1")->set(1, wheelRadius, 0);
	PARAM(Vector3DValue, other, "AxisPoint2")->set(-1, wheelRadius, 0);
	PARAM(BoolValue, other, "IsAngularMotor")->set(false);
	PARAM(StringValue, other, "FirstBody")->set(pathName + "/Body");
	PARAM(StringValue, other, "SecondBody")->set(pathName + "/LeftWheel");
	value = dynamic_cast<InterfaceValue*>(other->getParameter("MotorAngle"));
	if(value != 0) {
		other->useOutputAsInfoValue(value, true);
	}

	other = servo->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/RightMotor");
	PARAM(Vector3DValue, other, "AxisPoint1")->set(1, wheelRadius, 0);
	PARAM(Vector3DValue, other, "AxisPoint2")->set(-1, wheelRadius, 0);
	PARAM(BoolValue, other, "IsAngularMotor")->set(false);
	PARAM(StringValue, other, "FirstBody")->set(pathName + "/Body");
	PARAM(StringValue, other, "SecondBody")->set(pathName + "/RightWheel");
	value = dynamic_cast<InterfaceValue*>(other->getParameter("MotorAngle"));
	if(value != 0) {
		other->useOutputAsInfoValue(value, true);
	}

	other = fix->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/FrontStabilizerFix");
	PARAM(StringValue, other, "FirstBody")->set(pathName + "/Body");
	PARAM(StringValue, other, "SecondBody")->set(pathName + "/FrontStabilizer");

	other = fix->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/BackStabilizerFix");
	PARAM(StringValue, other, "FirstBody")->set(pathName + "/Body");
	PARAM(StringValue, other, "SecondBody")->set(pathName + "/BackStabilizer");

	/*
	 * Commented LocalPosition and LocalOrientation parameters are the ones from
	 * the real robot.  But until the RayGeom Visualization bug isn't fixed they
	 * can't be used.
	 */
	//Add distance sensors
	other = distance->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Sensor0");
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
//		PARAM(Vector3DValue, other, "LocalPosition")->set(0.023, 0, 0.0095);
//		PARAM(QuaternionValue, other, "LocalOrientation")
//				->set(Quaternion(0.7071067811865476, 0, 0, 0.7071067811865475)
//				* p);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
	PARAM(QuaternionValue, other, "LocalOrientation")
			->set(Quaternion(0.8311919617091449, 0, 0, 0.5559855418894484)
			* p);
	PARAM(DoubleValue, other, "AngleOfAperture")->set(distanceSensorAngle);
	PARAM(DoubleValue, other, "MaxRange")
			->set(distanceSensorRange + bodyRadius);
	PARAM(DoubleValue, other, "MinRange")->set(bodyRadius);
	PARAM(IntValue, other, "NumberOfRays")->set(distanceSensorRays);
	PARAM(ColorValue, other, "ActiveColor")->set(active);
	PARAM(ColorValue, other, "InactiveColor")->set(inactive);
	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);

	other = distance->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Sensor1");
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
//		PARAM(Vector3DValue, other, "LocalPosition")->set(0.0175, 0, 0.018);
//		PARAM(QuaternionValue, other, "LocalOrientation")
//				->set(Quaternion(0.9238795325112867, 0, 0, 0.3826834323650898)
//				* p);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
	PARAM(QuaternionValue, other, "LocalOrientation")
			->set(Quaternion(0.9265513797988838, 0, 0, 0.3761682344281408)
			* p);
	PARAM(DoubleValue, other, "AngleOfAperture")->set(distanceSensorAngle);
	PARAM(DoubleValue, other, "MaxRange")
			->set(distanceSensorRange + bodyRadius);
	PARAM(DoubleValue, other, "MinRange")->set(bodyRadius);
	PARAM(IntValue, other, "NumberOfRays")->set(distanceSensorRays);
	PARAM(ColorValue, other, "ActiveColor")->set(active);
	PARAM(ColorValue, other, "InactiveColor")->set(inactive);
	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);

	other = distance->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Sensor2");
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
//		PARAM(Vector3DValue, other, "LocalPosition")->set(0.0055, 0, 0.0245);
//		PARAM(QuaternionValue, other, "LocalOrientation")
//				->set(Quaternion(1, 0, 0, 0) * p);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
	PARAM(QuaternionValue, other, "LocalOrientation")
			->set(Quaternion(0.9939105221531691, 0, 0, 0.11019017176325077)
			* p);
	PARAM(DoubleValue, other, "AngleOfAperture")->set(distanceSensorAngle);
	PARAM(DoubleValue, other, "MaxRange")
			->set(distanceSensorRange + bodyRadius);
	PARAM(DoubleValue, other, "MinRange")->set(bodyRadius);
	PARAM(IntValue, other, "NumberOfRays")->set(distanceSensorRays);
	PARAM(ColorValue, other, "ActiveColor")->set(active);
	PARAM(ColorValue, other, "InactiveColor")->set(inactive);
	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);

	other = distance->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Sensor3");
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
//		PARAM(Vector3DValue, other, "LocalPosition")->set(-0.0055, 0, 0.0245);
//		PARAM(QuaternionValue, other, "LocalOrientation")
//				->set(Quaternion(1, 0, 0, 0) * p);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
	PARAM(QuaternionValue, other, "LocalOrientation")
			->set(Quaternion(0.9939105221531691, 0, 0, -0.11019017176325077)
			* p);
	PARAM(DoubleValue, other, "AngleOfAperture")->set(distanceSensorAngle);
	PARAM(DoubleValue, other, "MaxRange")
			->set(distanceSensorRange + bodyRadius);
	PARAM(DoubleValue, other, "MinRange")->set(bodyRadius);
	PARAM(IntValue, other, "NumberOfRays")->set(distanceSensorRays);
	PARAM(ColorValue, other, "ActiveColor")->set(active);
	PARAM(ColorValue, other, "InactiveColor")->set(inactive);
	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);

	other = distance->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Sensor4");
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
//		PARAM(Vector3DValue, other, "LocalPosition")->set(-0.0175, 0, 0.018);
//		PARAM(QuaternionValue, other, "LocalOrientation")
//				->set(Quaternion(0.9238795325112867, 0, 0, -0.3826834323650898)
//				* p);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
	PARAM(QuaternionValue, other, "LocalOrientation")
			->set(Quaternion(0.9265513797988838, 0, 0, -0.3761682344281408)
			* p);
	PARAM(DoubleValue, other, "AngleOfAperture")->set(distanceSensorAngle);
	PARAM(DoubleValue, other, "MaxRange")
			->set(distanceSensorRange + bodyRadius);
	PARAM(DoubleValue, other, "MinRange")->set(bodyRadius);
	PARAM(IntValue, other, "NumberOfRays")->set(distanceSensorRays);
	PARAM(ColorValue, other, "ActiveColor")->set(active);
	PARAM(ColorValue, other, "InactiveColor")->set(inactive);
	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);

	other = distance->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Sensor5");
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
//		PARAM(Vector3DValue, other, "LocalPosition")->set(-0.023, 0, 0.0095);
//		PARAM(QuaternionValue, other, "LocalOrientation")
//				->set(Quaternion(0.7071067811865476, 0, 0, -0.7071067811865475)
//				* p);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
	PARAM(QuaternionValue, other, "LocalOrientation")
			->set(Quaternion(0.8311919617091449, 0, 0, -0.5559855418894484)
			* p);
	PARAM(DoubleValue, other, "AngleOfAperture")->set(distanceSensorAngle);
	PARAM(DoubleValue, other, "MaxRange")
			->set(distanceSensorRange + bodyRadius);
	PARAM(DoubleValue, other, "MinRange")->set(bodyRadius);
	PARAM(IntValue, other, "NumberOfRays")->set(distanceSensorRays);
	PARAM(ColorValue, other, "ActiveColor")->set(active);
	PARAM(ColorValue, other, "InactiveColor")->set(inactive);
	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);

	other = distance->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Sensor6");
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
//		PARAM(Vector3DValue, other, "LocalPosition")->set(-0.0095, 0, -0.023);
//		PARAM(QuaternionValue, other, "LocalOrientation")
//				->set(Quaternion(0, 0, 0, 1) * p);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
	PARAM(QuaternionValue, other, "LocalOrientation")
			->set(Quaternion(0.1946003256805792, 0, 0, 0.9808826195039917)
			* p);
	PARAM(DoubleValue, other, "AngleOfAperture")->set(distanceSensorAngle);
	PARAM(DoubleValue, other, "MaxRange")
			->set(distanceSensorRange + bodyRadius);
	PARAM(DoubleValue, other, "MinRange")->set(bodyRadius);
	PARAM(IntValue, other, "NumberOfRays")->set(distanceSensorRays);
	PARAM(ColorValue, other, "ActiveColor")->set(active);
	PARAM(ColorValue, other, "InactiveColor")->set(inactive);
	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);

	other = distance->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Sensor7");
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
//		PARAM(Vector3DValue, other, "LocalPosition")->set(0.0095, 0, -0.023);
//		PARAM(QuaternionValue, other, "LocalOrientation")
//				->set(Quaternion(0, 0, 0, 1) * p);
	PARAM(Vector3DValue, other, "LocalPosition")->set(0, 0, 0);
	PARAM(QuaternionValue, other, "LocalOrientation")
			->set(Quaternion(0.1946003256805792, 0, 0, -0.9808826195039917)
			* p);
	PARAM(DoubleValue, other, "AngleOfAperture")->set(distanceSensorAngle);
	PARAM(DoubleValue, other, "MaxRange")
			->set(distanceSensorRange + bodyRadius);
	PARAM(DoubleValue, other, "MinRange")->set(bodyRadius);
	PARAM(IntValue, other, "NumberOfRays")->set(distanceSensorRays);
	PARAM(ColorValue, other, "ActiveColor")->set(active);
	PARAM(ColorValue, other, "InactiveColor")->set(inactive);
	PARAM(DoubleValue, other, "Noise")->set(distanceSensorNoise);


	//Add light sensors
	addLightSensors("BadGuys", 1);
	addLightSensors("Friends", 2);
	addLightSensors("Campfire", 3);
	addLightSensors("Food", 4);
	addLightSensors("FoodType", 5);

/*
	// Bug in visualisation of RayGeom.
	Quaternion s(0.9238795325112867, 0, 0, 0.3826834323650898);
	CollisionObject *co = new CollisionObject(RayGeom(2));
	dynamic_cast<SimBody*>(body)->addCollisionObject(co);
	co->disableCollisions(true);
	RayGeom *ray = dynamic_cast<RayGeom*>(co->getGeometry());
	ray->setLocalPosition(Vector3D(0, 0, 0.1));
	ray->setLocalOrientation(p);
*/

	for(QListIterator<SimObject*> i(mSimObjects); i.hasNext();) {
		SimObject *sim = i.next();
		mAgent->addObject(sim);
	}
	pm->addSimObjectGroup(mAgent);
}

SimObject* Alice::createCopy() const {
	return new Alice(*this);
}


void Alice::addLightSensors(const QString &name, int type) {
	double lightSensorNoise = 0.01;
	double lightSensorOffset = 0.025;
// 	double bodyAboveGround = 0.0015;
// 	double bodyHeight = 0.03;
	QString pathName = QString("/").append(getName());

	SimObject *other = mLightSensorPrototype->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Left/" + name);
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
	PARAM(Vector3DValue, other, "LocalPosition")
			->set(lightSensorOffset, 0, 0);
	PARAM(Vector3DValue, other, "LocalOrientation")
			->set(0.0, 60.0, 0.0);
	PARAM(DoubleValue, other, "Noise")->set(lightSensorNoise);
	PARAM(StringValue, other, "DetectableLightTypes")->set(QString::number(type));

	other = mLightSensorPrototype->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Mid/" + name);
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
	PARAM(Vector3DValue, other, "LocalPosition")
			->set(0, -lightSensorOffset, 0);
	//bodyHeight + bodyAboveGround
	PARAM(Vector3DValue, other, "LocalOrientation")
			->set(0.0, 0.0, 0.0);
	PARAM(DoubleValue, other, "Noise")->set(lightSensorNoise);
	PARAM(StringValue, other, "DetectableLightTypes")->set(QString::number(type));

	other = mLightSensorPrototype->createCopy();
	mSimObjects.append(other);
	PARAM(StringValue, other, "Name")->set("/Right/" + name);
	PARAM(StringValue, other, "HostBody")->set(pathName + "/Body");
	PARAM(Vector3DValue, other, "LocalPosition")
			->set(-lightSensorOffset, 0, 0);
	PARAM(Vector3DValue, other, "LocalOrientation")
			->set(0.0, -60.0, 0.0);
	PARAM(DoubleValue, other, "Noise")->set(lightSensorNoise);
	PARAM(StringValue, other, "DetectableLightTypes")->set(QString::number(type));
}

}

