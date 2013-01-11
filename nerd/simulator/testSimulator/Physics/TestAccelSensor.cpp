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


#include "TestAccelSensor.h"
#include "Physics/AccelSensor.h"
#include "Value/Vector3DValue.h"
#include "Value/QuaternionValue.h"
#include "Value/InterfaceValue.h"
#include "Physics/BoxBody.h"
#include "Core/Core.h"
#include "Math/Math.h"
#include "Physics/Physics.h"
#include "SimulationConstants.h"	
#include <iostream>
#include <math.h>

using namespace std;

namespace nerd{

//Verena
const double TestAccelSensor::mMax = 16.072704;

// Verena
void TestAccelSensor::testConstruction() {
	Core::resetCore();
	Core *core = Core::getInstance();

	DoubleValue *timeStep = new DoubleValue(0.01);
	core->getValueManager()->addValue(SimulationConstants::VALUE_TIME_STEP_SIZE, timeStep);
	DoubleValue *gravitation = new DoubleValue(9.81);
	core->getValueManager()->addValue("/Simulation/Gravitation", gravitation);

	AccelSensor *accelBoard = new AccelSensor("AccelBoard", 3);
	QVERIFY(accelBoard->getParameter("Name")->getValueAsString().compare("AccelBoard") == 0);
	QVERIFY(accelBoard->getParameter("ReferenceBody")->getValueAsString().compare("") == 0);
	QCOMPARE(accelBoard->getParameters().size(), 13);
	
	core->getValueManager()->getDoubleValue("/AccelSensor/Deviation")->set(0.0);

	Vector3DValue *pos = dynamic_cast<Vector3DValue*>(accelBoard->getParameter("LocalPosition"));
	QVERIFY(pos != 0);
	QuaternionValue *orientation = dynamic_cast<QuaternionValue*>(accelBoard->getParameter("LocalOrientation"));
	QVERIFY(orientation != 0);
	
	Vector3DValue *axis1 = dynamic_cast<Vector3DValue*>(accelBoard->getParameter("Axis1"));
	QVERIFY(axis1 != 0);
	
	Vector3DValue *axis2 = dynamic_cast<Vector3DValue*>(accelBoard->getParameter("Axis2"));
	QVERIFY(axis2 != 0);

	Vector3DValue *axis3 = dynamic_cast<Vector3DValue*>(accelBoard->getParameter("Axis3"));
	QVERIFY(axis3 != 0);
	
	QCOMPARE(accelBoard->getOutputValues().size(), 3);
	InterfaceValue *firstSensorValue = accelBoard->getOutputValues().at(0);
	QVERIFY(firstSensorValue->getName().compare("AccelBoard/AccelAxis1") == 0);
	InterfaceValue *secondSensorValue = accelBoard->getOutputValues().at(1);
	QVERIFY(secondSensorValue->getName().compare("AccelBoard/AccelAxis2") == 0);
	InterfaceValue *thirdSensorValue = accelBoard->getOutputValues().at(2);
	QVERIFY(thirdSensorValue->getName().compare("AccelBoard/AccelAxis3") == 0);

	QCOMPARE(pos->getX(), 0.0);
	QCOMPARE(pos->getY(), 0.0);
	QCOMPARE(pos->getZ(), 0.0);
	QCOMPARE(orientation->getW(), 1.0);
	QCOMPARE(orientation->getX(), 0.0);
	QCOMPARE(orientation->getY(), 0.0);
	QCOMPARE(orientation->getZ(), 0.0);

	QCOMPARE(axis1->getX(), 0.0);
	QCOMPARE(axis1->getY(), 0.0);
	QCOMPARE(axis1->getZ(), 1.0);
	QCOMPARE(axis2->getX(), 1.0);
	QCOMPARE(axis2->getY(), 0.0);
	QCOMPARE(axis2->getZ(), 0.0);
	QCOMPARE(axis3->getX(), 0.0);
	QCOMPARE(axis3->getY(), -1.0);
	QCOMPARE(axis3->getZ(), 0.0);

	QCOMPARE(firstSensorValue->getMin(), -16.072704);
	QCOMPARE(firstSensorValue->getMax(), 16.072704);
	QCOMPARE(secondSensorValue->getMin(), -16.072704);
	QCOMPARE(secondSensorValue->getMax(), 16.072704);
	QCOMPARE(thirdSensorValue->getMin(), -16.072704);
	QCOMPARE(thirdSensorValue->getMax(), 16.072704);
	
	QCOMPARE(firstSensorValue->get(), 0.0);
	QCOMPARE(secondSensorValue->get(), 0.0);
	QCOMPARE(thirdSensorValue->get(), 0.0);

	pos->set(Vector3D(0.5, -0.1, 2.21));
	orientation->set(Quaternion(1.57, 0.0, 0.0, 1.0));
	
	QVERIFY(accelBoard->getParameter("LocalPosition")->getValueAsString().compare("(0.5,-0.1,2.21)") == 0);
	QVERIFY(accelBoard->getParameter("LocalOrientation")->getValueAsString().compare("(1.57,0,0,1)") == 0);

	firstSensorValue->setMin(-12.2121);
	QCOMPARE(firstSensorValue->getMin(), -12.2121);


	//calling setup and calculate on uninitialized Sensor does not result in a SegFault
	accelBoard->setup();
// 	updateSenors->trigger();
	accelBoard->updateSensorValues();
// 	accelBoard->calculateSensorValue();
	
	AccelSensor *copy = dynamic_cast<AccelSensor*>(accelBoard->createCopy());
	QVERIFY(copy != 0);
	
	QCOMPARE(copy->getParameters().size(), 13);

	QVERIFY(copy->getParameter("LocalPosition")->getValueAsString().compare("(0.5,-0.1,2.21)") == 0);

	Value *accelAxis1 = accelBoard->getParameter("AccelAxis1");
	Value *accelAxis1Copy = copy->getParameter("AccelAxis1");
	QVERIFY(accelAxis1 != 0);
	QVERIFY(accelAxis1Copy != 0);
	QVERIFY(accelAxis1 != accelAxis1Copy);

	QVERIFY(copy->getParameter("LocalPosition") != accelBoard->getParameter("LocalPosition"));

	QVERIFY(copy->getParameter("LocalOrientation")->getValueAsString().compare(accelBoard->getParameter("LocalOrientation")->getValueAsString()) == 0);

	QVERIFY(copy->getParameter("Measurements") != 0);

	QCOMPARE(copy->getOutputValues().size(), 3);
	InterfaceValue *sensor1Copy = copy->getOutputValues().at(0);
	InterfaceValue *sensor2Copy = copy->getOutputValues().at(1);
	InterfaceValue *sensor3Copy = copy->getOutputValues().at(2);

	QCOMPARE(sensor1Copy->getMin(), -12.2121);
	QVERIFY(accelBoard->getOutputValues().at(0) != copy->getOutputValues().at(0));
	QVERIFY(accelBoard->getOutputValues().at(1) != sensor2Copy);
	QVERIFY(accelBoard->getOutputValues().at(2) != sensor3Copy);

}

// Verena
void TestAccelSensor::testSensor() {
	Core::resetCore();
	Core *core = Core::getInstance();
	
	DoubleValue *timeStep = new DoubleValue(0.01);
	Core::getInstance()->getValueManager()->addValue(SimulationConstants::VALUE_TIME_STEP_SIZE, timeStep);
	
	DoubleValue *gravitation = new DoubleValue(9.81);
	Core::getInstance()->getValueManager()->addValue("/Simulation/Gravitation", gravitation);

	AccelSensor *accelBoard = new AccelSensor("AccelBoard", 2);
	core->getValueManager()->getDoubleValue("/AccelSensor/Deviation")->set(0.0);

	BoxBody *body = new BoxBody("Box");
	body->getPositionValue()->set(1.0, 0.0, 0.0);
	Physics::getPhysicsManager()->addSimObject(body);

	accelBoard->getParameter("ReferenceBody")->setValueFromString(body->getName());
	
	Vector3DValue *axis1 = dynamic_cast<Vector3DValue*>(accelBoard->getParameter("Axis1"));
	Vector3DValue *axis2 = dynamic_cast<Vector3DValue*>(accelBoard->getParameter("Axis2"));
	axis1->set(1, 0, 0);
	axis2->set(0, 0, 1);

	accelBoard->setup();
	NormalizedDoubleValue *sensorValue1 = dynamic_cast<NormalizedDoubleValue*>(accelBoard->getOutputValues().at(0));
	NormalizedDoubleValue *sensorValue2 = dynamic_cast<NormalizedDoubleValue*>(accelBoard->getOutputValues().at(1));
	QVERIFY(sensorValue1 != 0);
	QVERIFY(sensorValue2 != 0);
	
	QCOMPARE(sensorValue1->get(), 0.0);
	QCOMPARE(sensorValue2->get(), 0.0);

	body->getPositionValue()->set(1.000145, 0.0, 0.0);
// 	updateSenors->trigger();
	accelBoard->updateSensorValues();

	double velocity = (0.000145/ timeStep->get()) / timeStep->get();	
	QCOMPARE(sensorValue1->getNormalized(), -velocity/mMax);
	QCOMPARE(sensorValue2->get(), 0.0);

	body->getPositionValue()->set(1.000290, 0.005, 0.0);
// 	updateSenors->trigger();
	accelBoard->updateSensorValues();
	QVERIFY(Math::compareDoubles(sensorValue1->get(), 0.0, 2));
	QCOMPARE(sensorValue2->get(), 0.0);

	body->getPositionValue()->set(1.000435, 0.005, -0.001);
// 	updateSenors->trigger();
	accelBoard->updateSensorValues();

	velocity = (-0.001/ timeStep->get()) / timeStep->get();	

	QVERIFY(Math::compareDoubles(sensorValue1->get(), 0.0, 2));
	QCOMPARE(sensorValue2->getNormalized(), -velocity/mMax);
	double sensorValueAxis1 = sensorValue1->getNormalized();

	body->getPositionValue()->set(1.000580, 0.005, 0.001);
// 	updateSenors->trigger();
	accelBoard->updateSensorValues();

	// Value exceeds the min/max of the accelBoard. Therefore the old velocity value should be reported for both! axis.
	QVERIFY(Math::compareDoubles(sensorValue1->get(), 0.0, 2));
	QCOMPARE(sensorValue2->getNormalized(), -velocity/mMax);
	QCOMPARE(sensorValue1->getNormalized(), sensorValueAxis1);

	AccelSensor *copy = dynamic_cast<AccelSensor*>(accelBoard->createCopy());
	QVERIFY(copy != 0);
	Vector3DValue *copyAxis1 = dynamic_cast<Vector3DValue*>(copy->getParameter("Axis1"));
	QCOMPARE(axis1->getX(), copyAxis1->getX());
	QCOMPARE(axis1->getY(), copyAxis1->getY());
	QCOMPARE(axis1->getZ(), copyAxis1->getZ());

	copy->getParameter("ReferenceBody")->setValueFromString("Box");
	copy->getParameter("Axis1")->setValueFromString("(-1, 0, 0)");
	copy->getParameter("Axis2")->setValueFromString("(0, 0, 1)");

// 	copy->createSensor(body, Vector3D(-1, 0, 0), Vector3D(0, 0, 1));
// 	QVERIFY(axis1->getX() != copyAxis1->getX());
	copy->setup();

	NormalizedDoubleValue *sensorValue1Copy = dynamic_cast<NormalizedDoubleValue*>(copy->getOutputValues().at(0));
	NormalizedDoubleValue *sensorValue2Copy = dynamic_cast<NormalizedDoubleValue*>(copy->getOutputValues().at(1));
	QCOMPARE(sensorValue1Copy->getNormalized(), 0.0);
	QCOMPARE(sensorValue2Copy->getNormalized(), 0.0);
	body->getPositionValue()->set(1.05, 0.005, 0.003);
// 	updateSenors->trigger();
	accelBoard->updateSensorValues();
	copy->updateSensorValues();
	QCOMPARE(sensorValue1Copy->getNormalized(), 0.0);
	QCOMPARE(sensorValue2Copy->getNormalized(), 0.0);
	body->getPositionValue()->set(1.0, 0.0, 0.0);
// 	updateSenors->trigger();
	accelBoard->updateSensorValues();
	copy->updateSensorValues();
	QCOMPARE(sensorValue1Copy->getNormalized(), 0.0);
	QCOMPARE(sensorValue2Copy->getNormalized(), 0.0);


	body->getPositionValue()->set(0.0, 0.0, 0.0);
	accelBoard->clear();
	copy->clear();
	QuaternionValue *orientation = dynamic_cast<QuaternionValue*>(accelBoard->getParameter("LocalOrientation"));
	orientation->setFromAngles(0,180,0);
	QuaternionValue *orientationCopy = dynamic_cast<QuaternionValue*>(copy->getParameter("LocalOrientation"));
	orientationCopy->setFromAngles(0,180,0);
	accelBoard->setup();
	copy->setup();
// 	updateSenors->trigger();
	accelBoard->updateSensorValues();
	copy->updateSensorValues();
	QVERIFY(Math::compareDoubles(sensorValue1->getNormalized(), 0.0, 2));
	QVERIFY(Math::compareDoubles(sensorValue2->getNormalized(), 0.0, 2));
	QVERIFY(Math::compareDoubles(sensorValue1Copy->getNormalized(), 0.0, 2));
	QVERIFY(Math::compareDoubles(sensorValue2Copy->getNormalized(), 0.0, 2));

	velocity = (0.000145/ timeStep->get()) / timeStep->get();	
	body->getPositionValue()->set(0.000145, 0.0, 0.0);
// 	updateSenors->trigger();
	accelBoard->updateSensorValues();
	copy->updateSensorValues();
	QVERIFY(Math::compareDoubles(sensorValue1->getNormalized(), velocity/mMax, 2));
	QVERIFY(Math::compareDoubles(sensorValue2->getNormalized(), 0.0, 2));
	QVERIFY(Math::compareDoubles(sensorValue1Copy->getNormalized(), -velocity/mMax, 2));
	QVERIFY(Math::compareDoubles(sensorValue2Copy->getNormalized(), 0.0, 2));

	velocity = (0.0002/ timeStep->get()) / timeStep->get();	
	body->getPositionValue()->set(0.000290, 0, -0.0002);
// 	updateSenors->trigger();
	accelBoard->updateSensorValues();
	copy->updateSensorValues();
	QVERIFY(Math::compareDoubles(sensorValue1->getNormalized(), 0, 2));
	QCOMPARE(sensorValue2->getNormalized(), -velocity/mMax);

	body->getPositionValue()->set(0, 0, 0);
	copy->clear();
// 	copy->createSensor(body, Vector3D(-1, 0, 0), Vector3D(0,1,0));

	}

void TestAccelSensor::testSensorCurve() {
	Core::resetCore();
	DoubleValue *timeStep = new DoubleValue(0.01);
	Core::getInstance()->getValueManager()->addValue(SimulationConstants::VALUE_TIME_STEP_SIZE, timeStep);
	
	DoubleValue *gravitation = new DoubleValue(9.81);
	Core::getInstance()->getValueManager()->addValue("/Simulation/Gravitation", gravitation);

	BoxBody *body = new BoxBody("Box");
	body->getPositionValue()->set(0.0, 0.0, 0.0);
	Physics::getPhysicsManager()->addSimObject(body);
	
	AccelSensor *accelBoard = new AccelSensor("AccelBoard", 2);
	accelBoard->getParameter("ReferenceBody")->setValueFromString(body->getName());
	Core::getInstance()->getValueManager()->getDoubleValue("/AccelSensor/Deviation")->set(0.0);

// 	accelBoard->createSensor(body);
	accelBoard->setup();
	NormalizedDoubleValue *sensorValue1 = dynamic_cast<NormalizedDoubleValue*>(accelBoard->getOutputValues().at(0));
	NormalizedDoubleValue *sensorValue2 = dynamic_cast<NormalizedDoubleValue*>(accelBoard->getOutputValues().at(1));
	QVERIFY(sensorValue1 != 0);
	QVERIFY(sensorValue2 != 0);

	double alpha = 0.0;
	QuaternionValue* rot = new QuaternionValue();
	for(int i = 0; i < 300; i++) {
		rot->setFromAngles(0, 0,alpha);
		body->getQuaternionOrientationValue()->set(rot->getW(), rot->getX(), rot->getY(), rot->getZ());
	// 	updateSenors->trigger();
		accelBoard->updateSensorValues();
		if(fabs(alpha - 90.0) <= 0.001 ) {
			QVERIFY(Math::compareDoubles(sensorValue2->getNormalized(), -1*gravitation->get()/mMax, 2));
		} 
		else {
		alpha += 0.5;
		}
	}
	for(int i = 0; i < 300; i++) {
		rot->setFromAngles(0, 0,alpha);
		body->getQuaternionOrientationValue()->set(rot->getW(), rot->getX(), rot->getY(), rot->getZ());
	// 	updateSenors->trigger();
		accelBoard->updateSensorValues();
		if(fabs(alpha - 180.0) <= 0.001 ) {
			QVERIFY(Math::compareDoubles(sensorValue1->getNormalized(), 0.0, 2));
		} 
		else {
		alpha += 0.5;
		}
	}
	for(int i = 0; i < 300; i++) {
		rot->setFromAngles(0, 0,alpha);
		body->getQuaternionOrientationValue()->set(rot->getW(), rot->getX(), rot->getY(), rot->getZ());
	// 	updateSenors->trigger();
		accelBoard->updateSensorValues();
		if(fabs(alpha - 270.0) <= 0.001 ) {
			QVERIFY(Math::compareDoubles(sensorValue2->getNormalized(), gravitation->get()/mMax, 2));
		} 
		else {
		alpha += 0.5;
		}
	}
	for(int i = 0; i < 300; i++) {
		rot->setFromAngles(0, 0,alpha);
		body->getQuaternionOrientationValue()->set(rot->getW(), rot->getX(), rot->getY(), rot->getZ());
	// 	updateSenors->trigger();
		accelBoard->updateSensorValues();
		if(fabs(alpha - 360.0) <= 0.001 ) {
			QVERIFY(Math::compareDoubles(sensorValue2->getNormalized(), 0.0, 2));
		} 
		else {
		alpha += 0.5;
		}
		}
	}

}
