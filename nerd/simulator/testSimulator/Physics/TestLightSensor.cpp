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
#include "Math/Math.h"
#include <cmath>
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
				getParameter("HostBody"))->get() == "");
	QVERIFY(lightSensor_1->getHostBody() == 0);
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

	Vector3DValue *localOrientation_1 = dynamic_cast<Vector3DValue*>
		(lightSensor_1->getParameter("LocalOrientation"));
	QVERIFY(localOrientation_1->equals(new Vector3DValue(0,0,0)));

	QVERIFY(dynamic_cast<StringValue*>(lightSensor_1->
				getParameter("DetectableTypes"))->get() == "0");

	QVERIFY(dynamic_cast<BoolValue*>(lightSensor_1->
				getParameter("RestrictToPlane"))->get() == true);

	QVERIFY(dynamic_cast<DoubleValue*>(lightSensor_1->
				getParameter("DetectionAngle"))->get() == 180.0);

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
	QVERIFY(collGeom_1->getWidth() == 0.01);
	QVERIFY(collGeom_1->getHeight() == 0.01);
	QVERIFY(collGeom_1->getDepth() == 0.01);
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
	dynamic_cast<Vector3DValue*>(lightSensor_1->
			getParameter("LocalOrientation"))->set(10,-90,3.4);
	dynamic_cast<BoolValue*>(lightSensor_1->
			getParameter("AmbientSensor"))->set(true);
	dynamic_cast<DoubleValue*>(lightSensor_1->
			getParameter("DetectionAngle"))->set(90.0);
	
	LightSensor *lightSensor_2 = dynamic_cast<LightSensor*>
		(lightSensor_1->createCopy());
	QVERIFY(lightSensor_2 != 0);

	QVERIFY(lightSensor_1 != lightSensor_2);

	QVERIFY(dynamic_cast<StringValue*>(lightSensor_2->
				getParameter("HostBody"))->get() == "");
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

	Vector3DValue *localOrientation_2 = dynamic_cast<Vector3DValue*>
		(lightSensor_2->getParameter("LocalOrientation"));
	QVERIFY(localOrientation_2->equals(new Vector3DValue(10,-90,3.4)));

	QVERIFY(dynamic_cast<StringValue*>(lightSensor_2->
				getParameter("DetectableTypes"))->get() == "1,3");

	QVERIFY(dynamic_cast<BoolValue*>(lightSensor_2->
				getParameter("RestrictToPlane"))->get() == true);

	QVERIFY(dynamic_cast<DoubleValue*>(lightSensor_2->
				getParameter("DetectionAngle"))->get() == 90.0);

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
	QVERIFY(collGeom_2->getWidth() == 0.01);
	QVERIFY(collGeom_2->getHeight() == 0.01);
	QVERIFY(collGeom_2->getDepth() == 0.01);
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
			getParameter("HostBody"))->set(hostBodyName_1);

	// call setup with host body but no light sources
	Physics::getPhysicsManager()->addSimObject(lightSensor_1);
	lightSensor_1->setup();

	QVERIFY(lightSensor_1->getHostBody() == hostBody_1);

	// position and orientation should have changed
	Vector3DValue *sensorPosition_1 = dynamic_cast<Vector3DValue*>
		(lightSensor_1->getParameter("Position"));
	Vector3DValue *sensorOrientation_1 = dynamic_cast<Vector3DValue*>
		(lightSensor_1->getParameter("Orientation"));

	QVERIFY(sensorPosition_1->get() == hostPosition_1->get());
	QVERIFY(sensorOrientation_1->get() == hostOrientation_1->get());

	lightSensor_1->updateSensorValues();
	// no light sources => no brightness (except noise, if set)
	InterfaceValue *brightness_1 = dynamic_cast<InterfaceValue*>
		(lightSensor_1->getParameter("Brightness"));
	QVERIFY(brightness_1->get() == 0.0);

	lightSensor_1->clear();
	Physics::getPhysicsManager()->removeSimObject(lightSensor_1);
	hostBody_1->clear();
	Physics::getPhysicsManager()->removeSimObject(hostBody_1);

	delete hostBody_1;
	delete lightSensor_1;

	// test without a host body
	LightSensor *lightSensor_2 = new LightSensor("LightSensor_2");
	Vector3DValue *localPosition_2 = dynamic_cast<Vector3DValue*>
		(lightSensor_2->getParameter("LocalPosition"));
	Vector3DValue *localOrientation_2 = dynamic_cast<Vector3DValue*>
		(lightSensor_2->getParameter("LocalOrientation"));

	Vector3DValue *sensorPosition_2 = dynamic_cast<Vector3DValue*>
		(lightSensor_2->getParameter("Position"));
	Vector3DValue *sensorOrientation_2 = dynamic_cast<Vector3DValue*>
		(lightSensor_2->getParameter("Orientation"));

	localPosition_2->set(0,0,1);
	localOrientation_2->set(0,270,180);

	Physics::getPhysicsManager()->addSimObject(lightSensor_2);
	lightSensor_2->setup();

	QVERIFY(sensorPosition_2->get() == localPosition_2->get());
	QVERIFY(sensorOrientation_2->get() == localOrientation_2->get());

	lightSensor_2->updateSensorValues();
	InterfaceValue *brightness_2 = dynamic_cast<InterfaceValue*>
		(lightSensor_2->getParameter("Brightness"));
	QVERIFY(brightness_2->get() == 0.0);

	lightSensor_2->clear();
	Physics::getPhysicsManager()->removeSimObject(lightSensor_2);

	delete lightSensor_2;

}


// josef
void TestLightSensor::testSensor() {
	Core::resetCore();

	// create one simple light source at the center
	// type: 1
	double brightness_set_1 = 1.0;
	SimpleLightSource *lightSource_1 =
		new SimpleLightSource("LightSource_1", brightness_set_1, 2.0, 1);
	QVERIFY(lightSource_1 != 0);

	// get distribution type parameter
	IntValue *distType_1 = dynamic_cast<IntValue*>
		(lightSource_1->getParameter("DistributionType"));
	QVERIFY(distType_1->get() == 0);
	Vector3DValue *lightPosition_1 = dynamic_cast<Vector3DValue*>
		(lightSource_1->getParameter("Position"));
	lightPosition_1->set(0,0,0);

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

	// move host body to look at light source from the left (half radius)
	hostPosition_1->set(1.0, 0.0, 0.0);
	hostOrientation_1->set(0.0, 270.0, 0.0);

	// register with physics manager and setup
	Physics::getPhysicsManager()->addSimObject(hostBody_1);
	hostBody_1->setup();


	// create the light sensor
	LightSensor *lightSensor_1 = new LightSensor("LightSensor_1");
	QVERIFY(lightSensor_1 != 0);

	// attach to the host body!
	dynamic_cast<StringValue*>(lightSensor_1->
			getParameter("HostBody"))->set("HostBody_1");

	// get parameters to change between single tests
	StringValue *detectableType_1 = dynamic_cast<StringValue*>
		(lightSensor_1->getParameter("DetectableTypes"));
	BoolValue *ambientSensor_1 = dynamic_cast<BoolValue*>
		(lightSensor_1->getParameter("AmbientSensor"));
	DoubleValue *detectionAngle_1 = dynamic_cast<DoubleValue*>
		(lightSensor_1->getParameter("DetectionAngle"));
	//BoolValue *restrictToPlane_1 = dynamic_cast<BoolValue*>
	//	(lightSensor_1->getParameter("RestrictToPlane"));
	Vector3DValue *localOrientation_1 = dynamic_cast<Vector3DValue*>
		(lightSensor_1->getParameter("LocalOrientation"));

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

	// should still be zero, as light source is not in detectable types yet
	QVERIFY(brightness_1->get() == 0.0);

	// light type according to source
	detectableType_1->set("1");

	// check the ambience function first
	ambientSensor_1->set(true);

	// calculate values
	lightSensor_1->updateSensorValues();

	// ambient sensor should just give center brightness
	QVERIFY(brightness_1->get() == brightness_set_1);

	// turn off ambient and update
	ambientSensor_1->set(false);
	detectionAngle_1->set(360.0);
	lightSensor_1->updateSensorValues();

	// helper value to compare the calculated
	double maxDiff = pow(10.0, -6);


	// position: (1,0,0)
	// detectionAngle: 360
	// restrictToPlane: true
	//
	// brightness should still be 1 as distType is set to the uniform default
	// and sensor is directly aligned towards the light source
	QVERIFY(Math::compareDoubles(
				brightness_1->get(), brightness_set_1, maxDiff));

	// change orientation
	hostOrientation_1->set(0,180,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(
				brightness_1->get(), brightness_set_1/2, maxDiff));

	// change orientation again
	hostOrientation_1->set(0,45,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(
				brightness_1->get(), brightness_set_1/4, maxDiff));

	// and again, now facing away from the light source
	hostOrientation_1->set(0,90,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(
				brightness_1->get(), 0.0, maxDiff));

	// at the edge of light source radius
	hostPosition_1->set(2,0,0);
	hostOrientation_1->set(0,270,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), brightness_set_1, maxDiff));

	// outside the light source's radius
	hostPosition_1->set(2,0,2);
	hostOrientation_1->set(0,225,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), 0.0, maxDiff));

	// out-of-range orientation value
	hostPosition_1->set(0,0,1);
	hostOrientation_1->set(0,-495,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), brightness_set_1/4*3, maxDiff));


	// hostBody north to the light source
	// oriented towards the right
	detectionAngle_1->set(90); // 45 degree to either side
	hostOrientation_1->set(0,-90,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), 0.0, maxDiff));

	// south of light source
	hostPosition_1->set(0,0,-1);
	hostOrientation_1->set(0,0,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), brightness_set_1, maxDiff));

	detectionAngle_1->set(180);
	hostOrientation_1->set(0,-45,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), brightness_set_1/2, maxDiff));

	hostPosition_1->set(-1,0,0);
	hostOrientation_1->set(0,0,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), 0.0, maxDiff));

	hostOrientation_1->set(0,45,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), brightness_set_1/2, maxDiff));

	hostOrientation_1->set(0,90,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), brightness_set_1, maxDiff));


	// see if change of local orientation has an effect
	// by turning the sensor 90 degrees in direction of light
	// such that it should now look at the source again
	detectionAngle_1->set(90);
	hostPosition_1->set(0,0,1);
	hostOrientation_1->set(0,-90,0);
	localOrientation_1->set(0,-90,0);
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), brightness_set_1, maxDiff));

	// change orientation once again
	detectionAngle_1->set(180);
	localOrientation_1->set(0,225,0); // facing south-west
	lightSensor_1->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_1->get(), brightness_set_1/2, maxDiff));



	// create a second light source of different type above the old one
	// brightness: 1.0, radius: 3.0, type: 2
	//
	double brightness_set_2 = 1.0;
	SimpleLightSource *lightSource_2 =
		new SimpleLightSource("LightSource_2", brightness_set_2, 3.0, 2);
	QVERIFY(lightSource_2 != 0);

	Vector3DValue *lightPosition_2 = dynamic_cast<Vector3DValue*>
		(lightSource_2->getParameter("Position"));
	lightPosition_2->set(0,1,0);

	IntValue *distType_2 = dynamic_cast<IntValue*>
		(lightSource_2->getParameter("DistributionType"));
	distType_2->set(0);

	Physics::getPhysicsManager()->addSimObject(lightSource_2);
	lightSource_2->setup();


	// create a second light sensor w/o host body
	LightSensor *lightSensor_2 = new LightSensor("LightSensor_2");
	QVERIFY(lightSensor_2 != 0);

	Physics::getPhysicsManager()->addSimObject(lightSensor_2);
	lightSensor_2->setup();
	
	Vector3DValue *sensorPosition_2 = dynamic_cast<Vector3DValue*>
		(lightSensor_2->getParameter("LocalPosition"));
	Vector3DValue *sensorOrientation_2 = dynamic_cast<Vector3DValue*>
		(lightSensor_2->getParameter("LocalOrientation"));
	
	BoolValue *restrictToPlane_2 = dynamic_cast<BoolValue*>
		(lightSensor_2->getParameter("RestrictToPlane"));
	BoolValue *ambientSensor_2 = dynamic_cast<BoolValue*>
		(lightSensor_2->getParameter("AmbientSensor"));
	StringValue *detectableTypes_2 = dynamic_cast<StringValue*>
		(lightSensor_2->getParameter("DetectableTypes"));
	DoubleValue *detectionAngle_2 = dynamic_cast<DoubleValue*>
		(lightSensor_2->getParameter("DetectionAngle"));

	InterfaceValue *brightness_2 = dynamic_cast<InterfaceValue*>
		(lightSensor_2->getParameter("Brightness"));

	// sensor is still only detecting type 0, so no brightness measured
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), 0.0, maxDiff));

	// now change to detect the new light source
	// should then instantly collect the new source
	detectableTypes_2->set("2");

	// and give corresponding brightness (Ambient!)
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2, maxDiff));

	// turn on directionality
	ambientSensor_2->set(false);

	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2, maxDiff));

	// change position and orientation
	sensorPosition_2->set(1,0,0);
	sensorOrientation_2->set(0,-90,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_1, maxDiff));

	sensorPosition_2->set(-2,2,0);
	sensorOrientation_2->set(0,135,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2/2, maxDiff));

	sensorOrientation_2->set(0,-135,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), 0.0, maxDiff));


	//
	// NOW TEST 3D
	//
	restrictToPlane_2->set(false);

	// 45 degree angle
	lightPosition_2->set(0,1,0);
	sensorPosition_2->set(-1,0,0);
	sensorOrientation_2->set(0,90,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2/2, maxDiff));
	
	// again
	sensorOrientation_2->set(-90,0,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2/2, maxDiff));

	// now directly towards source
	sensorOrientation_2->set(-90,0,-45);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2, maxDiff));

	// position somewhere in the 3D space
	lightPosition_2->set(1,1,1);
	sensorPosition_2->set(-1,1,1);
	sensorOrientation_2->set(0,90,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2, maxDiff));

	sensorPosition_2->set(-1,1,-1);
	sensorOrientation_2->set(0,45,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2, maxDiff));
	
	sensorPosition_2->set(1,2,1);
	sensorOrientation_2->set(90,90,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2, maxDiff));

	lightPosition_2->set(0,0,1);
	sensorPosition_2->set(0,0,-1);
	sensorOrientation_2->set(0,0,180);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2, maxDiff));

	detectionAngle_2->set(360);
	sensorOrientation_2->set(-90,0,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), brightness_set_2/2, maxDiff));

	// a simple fail test
	detectableTypes_2->set("3");
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), 0.0, maxDiff));


	// shortly test distribution of light with new source
	SimpleLightSource *lightSource_3 = new SimpleLightSource("LightSource_3",
															 4.0,
															 4.0,
															 4);
	QVERIFY(lightSource_3 != 0);

	IntValue *distType_3 = dynamic_cast<IntValue*>
		(lightSource_3->getParameter("DistributionType"));

	Physics::getPhysicsManager()->addSimObject(lightSource_3);
	lightSource_3->setup();
	detectableTypes_2->set("4");

	// still uniform light
	detectionAngle_2->set(180);
	sensorPosition_2->set(-2,0,0);
	sensorOrientation_2->set(0,90,0);
	lightSensor_2->updateSensorValues();
	// detectableRange is (0,1) by default
	QVERIFY(Math::compareDoubles(brightness_2->get(), 1.0, maxDiff));

	RangeValue *detectableRange_2 = dynamic_cast<RangeValue*>
		(lightSensor_2->getParameter("DetectableRange"));
	detectableRange_2->set(0,4.0);

	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), 4.0, maxDiff));

	distType_3->set(1);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), 2.0, maxDiff));

	sensorOrientation_2->set(0,45,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), 1.0, maxDiff));

	sensorPosition_2->set(-3,0,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), 0.5, maxDiff));

	distType_3->set(2);
	sensorPosition_2->set(0,0,2);
	sensorOrientation_2->set(0,-180,0);
	lightSensor_2->updateSensorValues();
	QVERIFY(Math::compareDoubles(brightness_2->get(), 1.0, maxDiff));


	// clean up
	lightSource_3->clear();
	lightSensor_2->clear();
	lightSource_2->clear();
	lightSensor_1->clear();
	lightSource_1->clear();

	Physics::getPhysicsManager()->removeSimObject(lightSource_3);
	Physics::getPhysicsManager()->removeSimObject(lightSensor_2);
	Physics::getPhysicsManager()->removeSimObject(lightSource_2);
	Physics::getPhysicsManager()->removeSimObject(lightSensor_1);
	Physics::getPhysicsManager()->removeSimObject(lightSource_1);

	delete lightSource_3;
	delete lightSensor_2;
	delete lightSource_2;
	delete lightSensor_1;
	delete lightSource_1;
}

}

