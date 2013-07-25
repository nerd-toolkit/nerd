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
#include "Value/BoolValue.h"
#include "SimulationConstants.h"
#include "Physics/SimpleLightSource.h"
#include "TestSimpleLightSource.h"
#include "Physics/CylinderGeom.h"
#include "Physics/Physics.h"
#include "Math/Math.h"

using namespace std;

namespace nerd{


// Josef
void TestSimpleLightSource::testConstruction() {

	Core::resetCore();

	double brightness_set_1 = 0.564;
	double radius_set_1 = 4.2;
	int type_set_1 = 1;
	SimpleLightSource *lightSource_1 = new SimpleLightSource("LightSource_1",
																brightness_set_1,
																radius_set_1,
																type_set_1);

	QVERIFY(lightSource_1 != 0);
	QVERIFY(dynamic_cast<IntValue*>(lightSource_1->
				getParameter("LightType"))->get() == 1);
	QVERIFY(dynamic_cast<Vector3DValue*>(lightSource_1->
				getParameter("LocalPosition"))->get().equals(Vector3D(0,0,0)));

	// check defaults and set initial values
	QVERIFY(dynamic_cast<DoubleValue*>(lightSource_1->
				getParameter("Radius"))->get() == radius_set_1);


	DoubleValue* centerBrightness = 
		dynamic_cast<DoubleValue*>(lightSource_1->
				getParameter("CenterBrightness"));

	QVERIFY(centerBrightness != 0);
	QVERIFY(centerBrightness->get() == brightness_set_1);

	QVERIFY(lightSource_1->getCenterBrightness() == brightness_set_1);

	// check set LightColor
	ColorValue* color_1 = dynamic_cast<ColorValue*>(lightSource_1->
			getParameter("LightColor"));
	QVERIFY(color_1 != 0);
	QVERIFY(color_1->getValueAsString() == "yellow");
	QVERIFY(color_1->get().equals(Color(255,255,0)));
	
	// Color parameter inherited from SimBody
	ColorValue* geomColor_1 = dynamic_cast<ColorValue*>(lightSource_1->
			getParameter("Color"));
	QVERIFY(geomColor_1 != 0);
	QVERIFY(geomColor_1->get().equals(Color(255,255,0,Math::abs(brightness_set_1*80.0))));

	//should be same as the collisionObject's geometry
	QVERIFY(lightSource_1->getCollisionObjects().size() == 1);
	CollisionObject* colObj_1 = lightSource_1->getCollisionObjects().first();
	QVERIFY(colObj_1 != 0);
	QVERIFY(colObj_1->getOwner() == 0);
	CylinderGeom* colGeom_1 = dynamic_cast<CylinderGeom*>(colObj_1->getGeometry());

	QVERIFY(colGeom_1 != 0);
	// method updateLightCone changes only geometry color alpha value
	QVERIFY(colGeom_1->getColor().equals(geomColor_1->get()));

	// LightCone visualization settings
 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_1->
				getParameter("HideLightCone"))->get() == false);

	// empty reference object
 	QVERIFY(dynamic_cast<StringValue*>(lightSource_1->
				getParameter("ReferenceObject"))->get() == "");

	// position at the center
	Vector3DValue* localPosition_1 = dynamic_cast<Vector3DValue*>
		(lightSource_1->getParameter("LocalPosition"));

	QVERIFY(localPosition_1->getX() == 0);
	QVERIFY(localPosition_1->getY() == 0);
	QVERIFY(localPosition_1->getZ() == 0);

	// default distribution is uniform (0)
 	QVERIFY(dynamic_cast<IntValue*>(lightSource_1->
				getParameter("DistributionType"))->get() == 0);

	// setup without a reference object
	lightSource_1->setup();

	// only the owner should change
	QVERIFY(colObj_1->getOwner() == lightSource_1);
	QVERIFY(localPosition_1->get().equals(Vector3D(0,0,0)));

	// don't delete, if registered at the PhysicsManager
	// or a segmentation fault will happen at Core::resetCore()
	lightSource_1->clear();
	delete lightSource_1;


	// different object, other values
	double brightness_set_2 = 4.56;
	double radius_set_2 = -5;
	int type_set_2 = 2;
	SimpleLightSource *lightSource_2 = new SimpleLightSource("LightSource_1",
																brightness_set_2,
																radius_set_2,
																type_set_2);
	QVERIFY(lightSource_2 != 0);

	Physics::getPhysicsManager()->addSimObject(lightSource_2);

	QVERIFY(dynamic_cast<IntValue*>(lightSource_2->
				getParameter("LightType"))->get() == type_set_2);

	QVERIFY(dynamic_cast<Vector3DValue*>(lightSource_2->
				getParameter("Position"))->get().equals(Vector3D(0,0,0)));

	QVERIFY(dynamic_cast<Vector3DValue*>(lightSource_2->
				getParameter("Orientation"))->get().equals(Vector3D(0,0,0)));

	// check that radius is normalized to 0 from below
	QVERIFY(dynamic_cast<DoubleValue*>(lightSource_2->
				getParameter("Radius"))->get() == 0);

	QVERIFY(dynamic_cast<DoubleValue*>(lightSource_2->
				getParameter("CenterBrightness"))->get() == brightness_set_2);

	QVERIFY(lightSource_2->getCenterBrightness() == brightness_set_2);

	// create generic SimObject to attach to ...
	QString reference_name = "LightReferenceObject";
	SimBody *referenceObject_2 = new SimBody(reference_name);

	Physics::getPhysicsManager()->addSimObject(referenceObject_2);

	dynamic_cast<Vector3DValue*>(referenceObject_2->
			getParameter("Orientation"))->set(10, 40, 20);

	dynamic_cast<Vector3DValue*>(referenceObject_2->
			getParameter("Position"))->set(1.2, 2.3, 3.4);

	referenceObject_2->setup();
	
	// set the reference object
	StringValue *referenceObjectName_2 = dynamic_cast<StringValue*>
		(lightSource_2->getParameter("ReferenceObject"));

	referenceObjectName_2->set(reference_name);

	// call setup with set reference object
	lightSource_2->setup();

	// verify reference object
	QVERIFY(referenceObjectName_2->get() == reference_name);

	QVERIFY(!dynamic_cast<Vector3DValue*>(lightSource_2->
				getParameter("Position"))->get().equals(Vector3D(0,0,0)));

	lightSource_2->clear();
	Physics::getPhysicsManager()->removeSimObject(lightSource_2);
	delete lightSource_2;

	referenceObject_2->clear();
	Physics::getPhysicsManager()->removeSimObject(referenceObject_2);
	delete referenceObject_2;
}


// josef
void TestSimpleLightSource::testCopy() {

	//Core::resetCore();

	double brightness_set_1 = 0.444;
	double radius_set_1 = 4.2;
	int type_set_1 = 1;
	SimpleLightSource* lightSource_1 = new SimpleLightSource("LightSource_1",
																brightness_set_1,
																radius_set_1,
																type_set_1);
	QVERIFY(lightSource_1 != 0);

	// change color
	dynamic_cast<ColorValue*>(lightSource_1->
			getParameter("LightColor"))->set(255,0,0);

	QVERIFY(dynamic_cast<ColorValue*>(lightSource_1->
				getParameter("LightColor"))->get().equals(Color(255,0,0)));

	SimpleLightSource* lightSource_2 = new SimpleLightSource(*lightSource_1);
	QVERIFY(lightSource_2 != 0);

	QVERIFY(dynamic_cast<DoubleValue*>(lightSource_2->
				getParameter("Radius"))->get() == radius_set_1);

	DoubleValue* centerBrightness = 
		dynamic_cast<DoubleValue*>(lightSource_2->
				getParameter("CenterBrightness"));

	QVERIFY(centerBrightness->get() == brightness_set_1);
	QVERIFY(lightSource_2->getCenterBrightness() == brightness_set_1);

	ColorValue* color_2 = dynamic_cast<ColorValue*>(lightSource_2->
			getParameter("LightColor"));

	QVERIFY(color_2 != 0);
	QVERIFY(color_2->getValueAsString() == "red");
	QVERIFY(color_2->get().equals(Color(255,0,0)));

 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_2->
				getParameter("HideLightCone"))->get() == false);

 	QVERIFY(dynamic_cast<StringValue*>(lightSource_2->
				getParameter("ReferenceObject"))->get() == "");

	Vector3DValue* localPosition_2 = 
		dynamic_cast<Vector3DValue*>(lightSource_2->
				getParameter("LocalPosition"));

	QVERIFY(localPosition_2->get().equals(Vector3D(0,0,0)));

 	QVERIFY(dynamic_cast<IntValue*>(lightSource_2->
				getParameter("DistributionType"))->get() == 0);

	// collisionObject
	QVERIFY(lightSource_2->getCollisionObjects().size() == 1);

	CollisionObject* colObj_2 = lightSource_2->getCollisionObjects().first();

	CylinderGeom* colGeom_2 = 
		dynamic_cast<CylinderGeom*>(colObj_2->getGeometry());

	QVERIFY(colGeom_2 != 0);
	QVERIFY(colGeom_2->getColor().equals(
				Color(255,0,0,Math::abs(brightness_set_1*80.0))));

	QVERIFY(dynamic_cast<ColorValue*>(lightSource_2->getParameter("Color"))->
			get().equals((colGeom_2->getColor())));

	delete lightSource_2;
	delete lightSource_1;
}


// josef
void TestSimpleLightSource::testMethods() {
	Core::resetCore();

	double brightness_set_1 = 1;
	double radius_set_1 = 2;
	int type_set_1 = 3;
	SimpleLightSource *lightSource_1 = new SimpleLightSource("LightSource_1",
																brightness_set_1,
																radius_set_1,
																type_set_1);
	QVERIFY(lightSource_1 != 0);

	Vector3D* lp = new Vector3D();
	bool rh = true;
	double bs = brightness_set_1;
	IntValue* distType = dynamic_cast<IntValue*>(lightSource_1->
			getParameter("DistributionType"));
	bool mSwitchYZAxes = Core::getInstance()->getValueManager()->
		getBoolValue(SimulationConstants::VALUE_SWITCH_YZ_AXES);

	// uniform distribution
	//
	distType->set(0);
	lp->set(0.0, 0.0, 0.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == bs);
	lp->set(0.2, -0.6, 1.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == bs);
	lp->set(0.0, 1.0, 0.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == bs);
	lp->set(0.8, 1.4, -1.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == bs);
	lp->set(0.0, 2.0, 0.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == bs);
	lp->set(0.0, 0.0, 8.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0);
	rh = false;
	lp->set(0.0, 0.0, 0.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == bs);
	lp->set(0.2, -0.6, 1.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == bs);
	lp->set(0.0, 1.0, 0.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == bs);
	lp->set(0.8, 1.4, -1.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == bs);
	lp->set(0.0, 2.0, 0.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == bs);
	lp->set(0.0, 0.0, 8.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0);


	// linear decay
	//
	distType->set(1);
	rh = true;
	lp->set(0.0, 0.0, 0.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == 1.0);
	if(mSwitchYZAxes) {
		lp->set(0.0, 0.0, 1.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.5);
		lp->set(0.0, 8.0, 1.5);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.25);
		lp->set(1.0, -1.0, 0.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.5);
		lp->set(0.0, 0.0, 2.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0);
		lp->set(0.0, 1.0, 8.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0);
	} else {
		lp->set(0.0, 1.0, 0.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.5);
		lp->set(0.0, 1.5, 6.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.25);
		lp->set(1.0, 0.0, -1.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.5);
		lp->set(0.0, 2.0, 0.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0);
		lp->set(0.0, 8.0, 1.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0);
	}

	// qudratic decay
	//
	distType->set(2);
	lp->set(0.0, 0.0, 0.0);
	QVERIFY(lightSource_1->getBrightness(*lp, rh) == 1.0);
	if(mSwitchYZAxes) {
		lp->set(0.0, 0.0, 1.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.25);
		lp->set(0.0, 8.0, 1.5);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0625);
		lp->set(-1.0, 0.0, -1.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) <= 0.25);
		lp->set(0.0, 0.0, 2.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0);
		lp->set(0.0, 1.0, 8.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0);
	} else {
		lp->set(0.0, 1.0, 0.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.25);
		lp->set(0.0, 1.5, 6.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0625);
		lp->set(-1.0, 1.0, 0.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) <= 0.25);
		lp->set(0.0, 2.0, 0.0);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0);
		lp->set(1.4, 8.0, 1.1);
		QVERIFY(lightSource_1->getBrightness(*lp, rh) == 0.0);
	}

	delete lightSource_1;

}

}
