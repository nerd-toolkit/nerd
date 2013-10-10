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


#include "Core/Core.h"
#include "Value/QuaternionValue.h"
#include "Value/Vector3DValue.h"
#include "SimulationConstants.h"	
#include "Physics/Physics.h"
#include "Physics/BoxBody.h"
#include "Physics/DistanceSensor.h"
#include <Physics/LightSensor.h>
#include "Math/Math.h"
#include <cmath>
#include "TestDistanceSensor.h"
#include "../odePhysics/Collision/ODE_CollisionHandler.h"
#include "../odePhysics/Physics/ODE_SimulationAlgorithm.h"

using namespace std;

namespace nerd{

// josef
void TestDistanceSensor::testConstruction() {
	Core::resetCore();

	// construct default sensor
	DistanceSensor *sensor_1 = new DistanceSensor("DistanceSensor_1");
	QVERIFY(sensor_1 != 0);

	// check default parameters
	QVERIFY(dynamic_cast<StringValue*>(sensor_1->
				getParameter("HostBody"))->get() == "");
	QVERIFY(sensor_1->getHostBody() == 0);
	
	QVERIFY(dynamic_cast<DoubleValue*>(sensor_1->
				getParameter("Noise"))->get() == 0.0);

	InterfaceValue *distance_1 = dynamic_cast<InterfaceValue*>
		(sensor_1->getParameter("Distance"));
	QVERIFY(distance_1->getName() == "Distance");
	QVERIFY(distance_1->getMin() == 0.0);
	QVERIFY(distance_1->getMax() == 1.0);
	QVERIFY(distance_1->get() == 1.0);

	QVERIFY(dynamic_cast<StringValue*>(sensor_1->
				getParameter("IgnoreList"))->get() == "");

	QVERIFY(dynamic_cast<BoolValue*>(sensor_1->
				getParameter("CalculateMinimum"))->get() == true);

	QVERIFY(dynamic_cast<DoubleValue*>(sensor_1->
				getParameter("AngleOfAperture"))->get() == 0.0);

	QVERIFY(dynamic_cast<DoubleValue*>(sensor_1->
				getParameter("RayOffset"))->get() == 0.0);
	QVERIFY(dynamic_cast<DoubleValue*>(sensor_1->
				getParameter("RayLength"))->get() == 1.0);
	
	QVERIFY(dynamic_cast<ColorValue*>(sensor_1->
				getParameter("ActiveColor"))->
				equals(new ColorValue(Color(255, 0, 0))));
	
	QVERIFY(dynamic_cast<ColorValue*>(sensor_1->
				getParameter("InactiveColor"))->
				equals(new ColorValue(Color(255, 255, 255))));
	
	QVERIFY(dynamic_cast<ColorValue*>(sensor_1->
				getParameter("DisabledColor"))->
				equals(new ColorValue(Color(255, 255, 255, 50))));
	
	QVERIFY(dynamic_cast<IntValue*>(sensor_1->
				getParameter("NumberOfRays"))->get() == 1);

	QList<InterfaceValue*> outputVals_1 = sensor_1->getOutputValues();
	QVERIFY(outputVals_1.size() == 1);
	QVERIFY(outputVals_1.at(0) == distance_1);

	delete sensor_1;
}


// josef
void TestDistanceSensor::testCopy() {
	Core::resetCore();

	DistanceSensor *sensor_1 = new DistanceSensor("DistanceSensor_1");
	QVERIFY(sensor_1 != 0);

	dynamic_cast<DoubleValue*>(sensor_1->
	getParameter("RayOffset"))->set(0.2);
	dynamic_cast<DoubleValue*>(sensor_1->
			getParameter("RayLength"))->set(5.1);
	dynamic_cast<IntValue*>(sensor_1->
			getParameter("NumberOfRays"))->set(5);
	dynamic_cast<DoubleValue*>(sensor_1->
			getParameter("AngleOfAperture"))->set(90.0);
	
	DistanceSensor *sensor_2 = dynamic_cast<DistanceSensor*>
		(sensor_1->createCopy());
	QVERIFY(sensor_2 != 0);

	QVERIFY(sensor_1 != sensor_2);

	QVERIFY(dynamic_cast<StringValue*>(sensor_2->
				getParameter("HostBody"))->get() == "");
	QVERIFY(dynamic_cast<DoubleValue*>(sensor_2->
				getParameter("Noise"))->get() == 0.0);
	
	QVERIFY(dynamic_cast<DoubleValue*>(sensor_2->
				getParameter("RayOffset"))->get() == 0.2);
	QVERIFY(dynamic_cast<DoubleValue*>(sensor_2->
				getParameter("RayLength"))->get() == 5.1);
	
	InterfaceValue *distance_2 = dynamic_cast<InterfaceValue*>
		(sensor_2->getParameter("Distance"));
	QVERIFY(distance_2->getName() == "Distance");
	QVERIFY(distance_2->getMin() == 0.0);
	QCOMPARE(distance_2->getMax(), 4.9);
	//QVERIFY(distance_2->getMax() == 4.9); // 5.1 - 0.2
	//Core::log(QString("DistanceMax: ").append(QString::number(distance_2->getMax())), true);

	QVERIFY(dynamic_cast<IntValue*>(sensor_2->
				getParameter("NumberOfRays"))->get() == 5);


	QVERIFY(dynamic_cast<DoubleValue*>(sensor_2->
				getParameter("AngleOfAperture"))->get() == 90.0);

	QVERIFY(dynamic_cast<StringValue*>(sensor_2->
				getParameter("IgnoreList"))->get() == "");

	QList<InterfaceValue*> outputVals_2 = sensor_2->getOutputValues();
	QVERIFY(outputVals_2.size() == 1);
	QVERIFY(outputVals_2.at(0) == distance_2);

	delete sensor_1;
	delete sensor_2;
}


// josef
void TestDistanceSensor::testMethods() {

}


// josef
void TestDistanceSensor::testSensor() {
	Core::resetCore();
	PhysicsManager *pManager = Physics::getPhysicsManager();
	CollisionManager *cManager = Physics::getCollisionManager();
	cManager->setCollisionHandler(
		new ODE_CollisionHandler());
	ODE_SimulationAlgorithm *sAlgo = new ODE_SimulationAlgorithm();
	pManager->setPhysicalSimulationAlgorithm(sAlgo);
	
	// create an object to detect
	//
	SimBody *object_1 = new BoxBody("Object_1", 0.5, 0.5, 0.5);
	QVERIFY(object_1 != 0);
	
	Vector3DValue *objectPosition_1 = dynamic_cast<Vector3DValue*>
		(object_1->getParameter("Position"));
	Vector3DValue *objectOrientation_1 = dynamic_cast<Vector3DValue*>
		(object_1->getParameter("Orientation"));
		
	objectPosition_1->set(0,0,0);
	objectOrientation_1->set(0,0,0);
	
	pManager->addSimObject(object_1);
	object_1->setup();
	
	// create host body for the sensor
	//
	SimBody *hostBody_1 = new BoxBody("HostBody_1", 0.1, 0.1, 0.1);
	QVERIFY(hostBody_1 != 0);

	// get position and orientation parameters to change them
	Vector3DValue *hostPosition_1 = dynamic_cast<Vector3DValue*>
		(hostBody_1->getParameter("Position"));
	Vector3DValue *hostOrientation_1 = dynamic_cast<Vector3DValue*>
		(hostBody_1->getParameter("Orientation"));

	hostPosition_1->set(0.0, 0.0, 1.0);
	hostOrientation_1->set(0.0, 0.0, 0.0);

	// register with physics manager and setup
	pManager->addSimObject(hostBody_1);
	hostBody_1->setup();


	// create the distance sensor
	DistanceSensor *sensor_1 = new DistanceSensor("DistanceSensor_1");
	QVERIFY(sensor_1 != 0);

	// attach to the host body!
	dynamic_cast<StringValue*>(sensor_1->
			getParameter("HostBody"))->set("HostBody_1");

	// get parameters to change between single tests
	DoubleValue *minRange_1 = dynamic_cast<DoubleValue*>
		(sensor_1->getParameter("RayOffset"));
	DoubleValue *maxRange_1 = dynamic_cast<DoubleValue*>
		(sensor_1->getParameter("RayLength"));
	BoolValue *calculateMinimum_1 = dynamic_cast<BoolValue*>
		(sensor_1->getParameter("CalculateMinimum"));
	IntValue *numberOfRays_1 = dynamic_cast<IntValue*>
		(sensor_1->getParameter("NumberOfRays"));
	DoubleValue *angleOfAperture_1 = dynamic_cast<DoubleValue*>
		(sensor_1->getParameter("AngleOfAperture"));
	StringValue *ignoreList_1 = dynamic_cast<StringValue*>
		(sensor_1->getParameter("IgnoreList"));
		
	numberOfRays_1->set(1);
	angleOfAperture_1->set(90);

	// setup in physics manager
	pManager->addSimObject(sensor_1);
	sensor_1->setup();
	
	cManager->updateCollisionModel();
	
	QVERIFY(sensor_1->getHostBody() == hostBody_1);

	// get distance parameter for checking
	InterfaceValue *distance_1 = dynamic_cast<InterfaceValue*>
		(sensor_1->getParameter("Distance"));
		
	// initially, distance should be one (no noise, no simulation)
	QVERIFY(distance_1->get() == 1.0);
	
	//pManager->executeSimulationStep(); // SEGFAULTS
	cManager->updateCollisionModel();
	cManager->updateCollisionRules();
	sensor_1->updateSensorValues();
	QVERIFY(distance_1->get() == 1.0);
	
	hostPosition_1->set(0,0,-0.5);
	//pManager->executeSimulationStep(); // SEgFAULTS
	cManager->updateCollisionModel();
	cManager->updateCollisionRules();
	sensor_1->updateSensorValues();
	
	// TODO figure out why the collisions don't work here!
	
	cManager->printCollisionRules();
	QList<Contact> cols = cManager->getCollisions();
	Core::log("Contacts: " + cols.size(), true);
	
	Core::log(QString("Distance: ").append(QString::number(distance_1->get())), true);

	// clean up
	sensor_1->clear();
	hostBody_1->clear();
	object_1->clear();

	Physics::getPhysicsManager()->removeSimObject(sensor_1);
	Physics::getPhysicsManager()->removeSimObject(hostBody_1);
	Physics::getPhysicsManager()->removeSimObject(object_1);

	delete sensor_1;
	delete hostBody_1;
	delete object_1;
}

}

