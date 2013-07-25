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
#include "Physics/LightSensor.h"
#include "Physics/SimpleLightSource.h"
#include "TestLightSensor.h"

using namespace std;

namespace nerd{

// josef
void TestLightSensor::testConstruction() {
	Core::resetCore();

	// construct default sensor
	LightSensor *lightSensor_1 = new LightSensor("LightSensor_1");
	QVERIFY(lightSensor_1 != 0);

	// check default parameters
	QVERIFY(dynamic_cast<StringValue*>(lightSensor_1->
				getParameter("HostBodyName"))->get() == "");
	QVERIFY(dynamic_cast<DoubleValue*>(lightSensor_1->
				getParameter("Noise"))->get() == 0.0);

	InterfaceValue *brightness_1 = dynamic_cast<InterfaceValue*>
		(lightSensor_1->getParameter("Brightness"));
	QVERIFY(brightness_1->getName() == "Brightness");
	QVERIFY(brightness_1->getMin() == 0.0);
	QVERIFY(brightness_1->getMax() == 1.0);

	Vector3DValue *localPosition_1 = dynamic_cast<Vector3DValue*>
		(lightSensor_1->getParameter("LocalPosition"));
	QVERIFY(localPosition_1->getX() == 0);
	QVERIFY(localPosition_1->getY() == 0);
	QVERIFY(localPosition_1->getZ() == 0);

	QVERIFY(dynamic_cast<BoolValue*>(lightSensor_1->
				getParameter("AmbientSensor"))->get() == false);

	Vector3DValue *angleDiff_1 = dynamic_cast<Vector3DValue*>
		(lightSensor_1->getParameter("AngleDifferences"));
	QVERIFY(angleDiff_1->equals(new Vector3DValue()));

	Vector3DValue *localOrientation_1 = dynamic_cast<Vector3DValue*>
		(lightSensor_1->getParameter("LocalOrientation"));
	QVERIFY(localOrientation_1->equals(new Vector3DValue(0,0,0)));

	QVERIFY(dynamic_cast<StringValue*>(lightSensor_1->
				getParameter("DetectableTypes"))->get() == "0");

	QVERIFY(dynamic_cast<BoolValue*>(lightSensor_1->
				getParameter("RestrictToPlane"))->get() == true);

	QVERIFY(dynamic_cast<DoubleValue*>(lightSensor_1->
				getParameter("MaxDetectionAngle"))->get() == 180.0);

	QVERIFY(dynamic_cast<RangeValue*>(lightSensor_1->
				getParameter("DetectableRange"))->getMin() == 0.0);
	QVERIFY(dynamic_cast<RangeValue*>(lightSensor_1->
				getParameter("DetectableRange"))->getMax() == 1.0);

	QList<InterfaceValue*> outputVals_1 = lightSensor_1->getOutputValues();
	QVERIFY(outputVals_1.size() == 1);
	QVERIFY(outputVals_1.at(0) == brightness_1);

	QList<CollisionObject*> collObjs_1 = lightSensor_1->getCollisionObjects();
	QVERIFY(collObjs_1.size() == 1);
	CollisionObject* collObj_1 = 
		dynamic_cast<CollisionObject*>(collObjs_1.first());
	QVERIFY(collObj_1 != 0);
	
	BoxGeom *collGeom_1 = dynamic_cast<BoxGeom*>(collObj_1->getGeometry());
	QVERIFY(collGeom_1 != 0);
	QVERIFY(collGeom_1->getWidth() == 0.1);
	QVERIFY(collGeom_1->getHeight() == 0.1);
	QVERIFY(collGeom_1->getDepth() == 0.1);
	QVERIFY(collGeom_1->getSimObject() == lightSensor_1);

	delete lightSensor_1;
}


// josef
void TestLightSensor::testCopy() {
	Core::resetCore();

	LightSensor *lightSensor_1 = new LightSensor("LightSensor_1");
	QVERIFY(lightSensor_1 != 0);

	dynamic_cast<StringValue*>(lightSensor_1->
			getParameter("DetectableTypes"))->set("1,3");
	dynamic_cast<Vector3DValue*>(lightSensor_1->
			getParameter("LocalPosition"))->set(1,0.5,0);
	dynamic_cast<BoolValue*>(lightSensor_1->
			getParameter("AmbientSensor"))->set(true);
	dynamic_cast<DoubleValue*>(lightSensor_1->
			getParameter("MaxDetectionAngle"))->set(90.0);
	
	LightSensor *lightSensor_2 = dynamic_cast<LightSensor*>
		(lightSensor_1->createCopy());
	QVERIFY(lightSensor_2 != 0);

	QVERIFY(lightSensor_1 != lightSensor_2);

	QVERIFY(dynamic_cast<StringValue*>(lightSensor_2->
				getParameter("HostBodyName"))->get() == "");
	QVERIFY(dynamic_cast<DoubleValue*>(lightSensor_2->
				getParameter("Noise"))->get() == 0.0);

	InterfaceValue *brightness_2 = dynamic_cast<InterfaceValue*>
		(lightSensor_2->getParameter("Brightness"));
	QVERIFY(brightness_2->getName() == "Brightness");
	QVERIFY(brightness_2->getMin() == 0.0);
	QVERIFY(brightness_2->getMax() == 1.0);

	Vector3DValue *localPosition_2 = dynamic_cast<Vector3DValue*>
		(lightSensor_2->getParameter("LocalPosition"));
	QVERIFY(localPosition_2->getX() == 1.0);
	QVERIFY(localPosition_2->getY() == 0.5);
	QVERIFY(localPosition_2->getZ() == 0.0);

	QVERIFY(dynamic_cast<BoolValue*>(lightSensor_2->
				getParameter("AmbientSensor"))->get() == true);

	Vector3DValue *angleDiff_2 = dynamic_cast<Vector3DValue*>
		(lightSensor_2->getParameter("AngleDifferences"));
	QVERIFY(angleDiff_2->equals(new Vector3DValue()));

	Vector3DValue *localOrientation_2 = dynamic_cast<Vector3DValue*>
		(lightSensor_2->getParameter("LocalOrientation"));
	QVERIFY(localOrientation_2->equals(new Vector3DValue(0,0,0)));

	QVERIFY(dynamic_cast<StringValue*>(lightSensor_2->
				getParameter("DetectableTypes"))->get() == "1,3");

	QVERIFY(dynamic_cast<BoolValue*>(lightSensor_2->
				getParameter("RestrictToPlane"))->get() == true);

	QVERIFY(dynamic_cast<DoubleValue*>(lightSensor_2->
				getParameter("MaxDetectionAngle"))->get() == 90.0);

	QVERIFY(dynamic_cast<RangeValue*>(lightSensor_2->
				getParameter("DetectableRange"))->getMin() == 0.0);
	QVERIFY(dynamic_cast<RangeValue*>(lightSensor_2->
				getParameter("DetectableRange"))->getMax() == 1.0);

	QList<InterfaceValue*> outputVals_2 = lightSensor_2->getOutputValues();
	QVERIFY(outputVals_2.size() == 1);
	QVERIFY(outputVals_2.at(0) == brightness_2);

	QList<CollisionObject*> collObjs_2 = lightSensor_2->getCollisionObjects();
	QVERIFY(collObjs_2.size() == 1);
	CollisionObject* collObj_2 = 
		dynamic_cast<CollisionObject*>(collObjs_2.first());
	QVERIFY(collObj_2 != 0);
	
	BoxGeom *collGeom_2 = dynamic_cast<BoxGeom*>(collObj_2->getGeometry());
	QVERIFY(collGeom_2 != 0);
	QVERIFY(collGeom_2->getWidth() == 0.1);
	QVERIFY(collGeom_2->getHeight() == 0.1);
	QVERIFY(collGeom_2->getDepth() == 0.1);
	QVERIFY(collGeom_2->getSimObject() == lightSensor_2);

	delete lightSensor_1;
	delete lightSensor_2;
}


// josef
void TestLightSensor::testMethods() {
	Core::resetCore();

	// create a host body for testing
	QString hostBodyName_1 = "TestHostBody_1";
	SimBody *hostBody_1 = new SimBody(hostBodyName_1);
	QVERIFY(hostBody_1 != 0);

	Vector3DValue *hostOrientation_1 = dynamic_cast<Vector3DValue*>
		(hostBody_1->getParameter("Orientation"));
	hostOrientation_1->set(1.0, 2.0, 3.0);

	Vector3DValue *hostPosition_1 = dynamic_cast<Vector3DValue*>
		(hostBody_1->getParameter("Position"));
	hostPosition_1->set(1.2, 2.3, 3.4);

	Physics::getPhysicsManager()->addSimObject(hostBody_1);
	hostBody_1->setup();
	

	// create light sensor
	LightSensor *lightSensor_1 = new LightSensor("LightSensor_1");

	// set host body of light sensor
	dynamic_cast<StringValue*>(lightSensor_1->
			getParameter("HostBodyName"))->set(hostBodyName_1);

	// call setup with host body but no light sources
	Physics::getPhysicsManager()->addSimObject(lightSensor_1);
	lightSensor_1->setup();

	// position and orientation should have changed
	Vector3DValue *newPosition_1 = dynamic_cast<Vector3DValue*>
		(lightSensor_1->getParameter("Position"));
	Vector3DValue *newOrientation_1 = dynamic_cast<Vector3DValue*>
		(lightSensor_1->getParameter("Orientation"));

	QVERIFY(newPosition_1->equals(hostPosition_1));
	QVERIFY(newOrientation_1->equals(hostOrientation_1));

	lightSensor_1->clear();
	Physics::getPhysicsManager()->removeSimObject(lightSensor_1);
	hostBody_1->clear();
	Physics::getPhysicsManager()->removeSimObject(hostBody_1);

	delete hostBody_1;
	delete lightSensor_1;
}


// josef
void TestLightSensor::testSensor() {
	Core::resetCore();
	Core *core = Core::getInstance();

	DoubleValue *timeStep = new DoubleValue(0.01);
	core->getValueManager()->addValue(
			SimulationConstants::VALUE_TIME_STEP_SIZE, timeStep);


	// create one simple light source at the center
	// type: 1
	double brightness_set_1 = 1.0;
	SimpleLightSource *lightSource_1 =
		new SimpleLightSource("LightSource_1", brightness_set_1, 2.0, 1);
	QVERIFY(lightSource_1 != 0);

	// register with physics manager and setup
	Physics::getPhysicsManager()->addSimObject(lightSource_1);
	lightSource_1->setup();
	

	// create host body for the sensor
	//
	SimBody *hostBody_1 = new SimBody("HostBody_1");
	QVERIFY(hostBody_1 != 0);

	// get position and orientation parameters to change them
	Vector3DValue *hostPosition_1 = dynamic_cast<Vector3DValue*>
		(hostBody_1->getParameter("Position"));
	Vector3DValue *hostOrientation_1 = dynamic_cast<Vector3DValue*>
		(hostBody_1->getParameter("Orientation"));

	// move host body to look at light source from half the radius
	hostPosition_1->set(1.0, 1.0, 0.0);
	hostOrientation_1->set(45.0, 0.0, 0.0);

	// register with physics manager and setup
	Physics::getPhysicsManager()->addSimObject(hostBody_1);
	hostBody_1->setup();


	// create the light sensor
	LightSensor *lightSensor_1 = new LightSensor("LightSensor_1");
	QVERIFY(lightSensor_1 != 0);

	// attach to the host body!
	dynamic_cast<StringValue*>(lightSensor_1->
			getParameter("HostBodyName"))->set("HostBody_1");

	// get parameters to change between single tests
	StringValue *detectableType_1 = dynamic_cast<StringValue*>
		(lightSensor_1->getParameter("DetectableTypes"));
	BoolValue *ambientSensor_1 = dynamic_cast<BoolValue*>
		(lightSensor_1->getParameter("AmbientSensor"));

	// light type according to source
	detectableType_1->set("1");
	// check the ambience function first
	ambientSensor_1->set(true);

	// setup in physics manager
	Physics::getPhysicsManager()->addSimObject(lightSensor_1);
	lightSensor_1->setup();

	// get brightness parameter for checking
	InterfaceValue *brightness_1 = dynamic_cast<InterfaceValue*>
		(lightSensor_1->getParameter("Brightness"));

	
	// initially, brightness should be zero (no noise, no simulation)
	QVERIFY(brightness_1->get() == 0.0);

	//Physics::getPhysicsManager()->executeSimulationStep();
	lightSensor_1->updateSensorValues();

	// ambient sensor should just give center brightness
	QVERIFY(brightness_1->get() == brightness_set_1);

	ambientSensor_1->set(false);
	lightSensor_1->updateSensorValues();

	QVERIFY(brightness_1->get() == 0.5);

	// TODO
	// * refactor calculateBrightness() of LightSensor
	// * extend sensor tests to include all(?) possible scenarios
	// * verify results and adapt script to changed LightSensor parameters

}

}
