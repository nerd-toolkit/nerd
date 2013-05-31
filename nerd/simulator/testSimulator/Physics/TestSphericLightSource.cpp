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
#include "Physics/SphericLightSource.h"
#include "TestSphericLightSource.h"
#include "Physics/SphereGeom.h"
#include "Physics/CylinderGeom.h"
#include "Math/Math.h"

using namespace std;

namespace nerd{

// Josef
void TestSphericLightSource::testConstruction() {
	Core::resetCore();

	double brightness_set_1 = 0.564;
	double range_set_1 = 4.2;
	int type_set_1 = 1;
	SphericLightSource *lightSource_1 = new SphericLightSource("LightSource_1",
																brightness_set_1,
																range_set_1,
																type_set_1);

	QVERIFY(lightSource_1 != 0);

	// check defaults and set initial values
	QVERIFY(dynamic_cast<DoubleValue*>(lightSource_1->
				getParameter("Range"))->get() == range_set_1);

	InterfaceValue* currentBrightness = 
		dynamic_cast<InterfaceValue*>(lightSource_1->getParameter("Brightness"));

	QVERIFY(currentBrightness->get() == brightness_set_1);

	DoubleValue* desiredBrightness = 
		dynamic_cast<DoubleValue*>(lightSource_1->
				getParameter("DesiredBrightness"));

	QVERIFY(desiredBrightness->get() == brightness_set_1);


	ColorValue* color_1 = dynamic_cast<ColorValue*>(lightSource_1->
			getParameter("LightColor"));
	QVERIFY(color_1 != 0);
	QVERIFY(color_1->getValueAsString() == "yellow");
	QVERIFY(color_1->get().equals(Color(255,255,0)));

 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_1->
				getParameter("HideLightCone"))->get() == false);

 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_1->
				getParameter("SphericLightCone"))->get() == false);

 	QVERIFY(dynamic_cast<StringValue*>(lightSource_1->
				getParameter("ReferenceObject"))->get() == "");

	Vector3DValue* localPosition_1 = 
		dynamic_cast<Vector3DValue*>(lightSource_1->getParameter("LocalPosition"));

	QVERIFY(localPosition_1->getX() == 0);
	QVERIFY(localPosition_1->getY() == 0);
	QVERIFY(localPosition_1->getZ() == 0);

 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_1->
				getParameter("UniformLight"))->get() == false);

 	QVERIFY(dynamic_cast<RangeValue*>(lightSource_1->
				getParameter("BrightnessRange"))->getMin() == 0);

 	QVERIFY(dynamic_cast<RangeValue*>(lightSource_1->
				getParameter("BrightnessRange"))->getMax() == 1);
 
 	QVERIFY(lightSource_1->getOutputValues().size() == 1);
 	QVERIFY(lightSource_1->getInputValues().size() == 1);

	// collisionObject
	QVERIFY(lightSource_1->getCollisionObjects().size() == 1);
	CollisionObject* colObj_1 = lightSource_1->getCollisionObjects().first();
	QVERIFY(colObj_1 != 0);
	QVERIFY(colObj_1->getOwner() == 0);
	CylinderGeom* colGeom_1 = dynamic_cast<CylinderGeom*>(colObj_1->getGeometry());

	QVERIFY(colGeom_1 != 0);
	// method updateLightColor changes only geometry color alpha value
	QVERIFY(colGeom_1->getColor().equals(
				Color(255,255,0,Math::abs(brightness_set_1*80.0))));

	delete lightSource_1;


	// different object, other values
	double brightness_set_2 = 4.56;
	double range_set_2 = -5;
	int type_set_2 = 2;
	SphericLightSource *lightSource_2 = new SphericLightSource("LightSource_1",
																brightness_set_2,
																range_set_2,
																type_set_2);
	QVERIFY(lightSource_2 != 0);

	// negative ranges are possible? no checks? TODO
	QVERIFY(dynamic_cast<DoubleValue*>(lightSource_2->
				getParameter("Range"))->get() == range_set_2);

	// brightness range is restricted to (0,1) initially by default
	// this is stupid and should probably be changed ... TODO
	QVERIFY(dynamic_cast<InterfaceValue*>(lightSource_2->
				getParameter("Brightness"))->get() == 1);

	QVERIFY(dynamic_cast<DoubleValue*>(lightSource_2->
				getParameter("DesiredBrightness"))->get() == brightness_set_2);

	RangeValue* brightnessRange_2 = dynamic_cast<RangeValue*>(lightSource_2->
			getParameter("BrightnessRange"));
	brightnessRange_2->set(-5,5);

	QVERIFY(dynamic_cast<InterfaceValue*>(lightSource_2->
				getParameter("Brightness"))->get() == brightness_set_2);

	delete lightSource_2;

}

// josef
void TestSphericLightSource::testCopy() {
	Core::resetCore();

	double brightness_set_1 = 0.444;
	double range_set_1 = 4.2;
	int type_set_1 = 1;
	SphericLightSource* lightSource_1 = new SphericLightSource("LightSource_1",
																brightness_set_1,
																range_set_1,
																type_set_1);
	QVERIFY(lightSource_1 != 0);

	// change color
	dynamic_cast<ColorValue*>(lightSource_1->
			getParameter("LightColor"))->set(255,0,0);

	QVERIFY(dynamic_cast<ColorValue*>(lightSource_1->
				getParameter("LightColor"))->get().equals(Color(255,0,0)));

	SphericLightSource* lightSource_2 = new SphericLightSource(*lightSource_1);
	QVERIFY(lightSource_2 != 0);

	QVERIFY(dynamic_cast<DoubleValue*>(lightSource_2->
				getParameter("Range"))->get() == range_set_1);

	InterfaceValue* currentBrightness = 
		dynamic_cast<InterfaceValue*>(lightSource_2->getParameter("Brightness"));

	QVERIFY(currentBrightness->get() == brightness_set_1);

	DoubleValue* desiredBrightness = 
		dynamic_cast<DoubleValue*>(lightSource_2->
				getParameter("DesiredBrightness"));

	QVERIFY(desiredBrightness->get() == brightness_set_1);


	ColorValue* color_2 = dynamic_cast<ColorValue*>(lightSource_2->
			getParameter("LightColor"));

	QVERIFY(color_2 != 0);
	QVERIFY(color_2->getValueAsString() == "red");
	QVERIFY(color_2->get().equals(Color(255,0,0)));

 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_2->
				getParameter("HideLightCone"))->get() == false);

 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_2->
				getParameter("SphericLightCone"))->get() == false);

 	QVERIFY(dynamic_cast<StringValue*>(lightSource_2->
				getParameter("ReferenceObject"))->get() == "");

	Vector3DValue* localPosition_2 = 
		dynamic_cast<Vector3DValue*>(lightSource_2->
				getParameter("LocalPosition"));

	QVERIFY(localPosition_2->getX() == 0);
	QVERIFY(localPosition_2->getY() == 0);
	QVERIFY(localPosition_2->getZ() == 0);

 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_2->
				getParameter("UniformLight"))->get() == false);

 	QVERIFY(dynamic_cast<RangeValue*>(lightSource_2->
				getParameter("BrightnessRange"))->getMin() == 0);

 	QVERIFY(dynamic_cast<RangeValue*>(lightSource_2->
				getParameter("BrightnessRange"))->getMax() == 1);
 
 	QVERIFY(lightSource_2->getOutputValues().size() == 1);
 	QVERIFY(lightSource_2->getInputValues().size() == 1);

	// collisionObject
	QVERIFY(lightSource_2->getCollisionObjects().size() == 1);

	CollisionObject* colObj_2 = lightSource_2->getCollisionObjects().first();

	CylinderGeom* colGeom_2 = 
		dynamic_cast<CylinderGeom*>(colObj_2->getGeometry());

	QVERIFY(colGeom_2 != 0);
	QVERIFY(colGeom_2->getColor().equals(
				Color(255,0,0,Math::abs(brightness_set_1*80.0))));

	delete lightSource_1;
	delete lightSource_2;

}

// josef
void TestSphericLightSource::testMethods() {
	Core::resetCore();

	double brightness_set_1 = 0.66645;
	double range_set_1 = 2;
	int type_set_1 = 3;
	SphericLightSource *lightSource_1 = new SphericLightSource("LightSource_1",
																brightness_set_1,
																range_set_1,
																type_set_1);

	QVERIFY(lightSource_1 != 0);

	// collisionObject defaults
	QVERIFY(lightSource_1->getCollisionObjects().size() == 1);
	CollisionObject* colObj_1 = lightSource_1->getCollisionObjects().first();

	QVERIFY(colObj_1 != 0);
	QVERIFY(colObj_1->getOwner() == 0);
	QVERIFY(colObj_1->getHostBody() == lightSource_1); // set by addCollisionObject

	CylinderGeom* colGeom_1 = dynamic_cast<CylinderGeom*>(colObj_1->getGeometry());
	QVERIFY(colGeom_1 != 0);
	QVERIFY(colGeom_1->getRadius() == range_set_1);
	QVERIFY(colGeom_1->getColor().equals(
				Color(255,255,0,Math::abs(brightness_set_1*80.0))));

	// setup w/o reference object
	lightSource_1->setup();

	// owner of collisionObject should have changed
	QVERIFY(colObj_1->getOwner() == lightSource_1);

	// changing the ColorValue should change the geometry color as well
	// relates to updateLightColor() method
	ColorValue* color_1 = dynamic_cast<ColorValue*>(lightSource_1->
			getParameter("LightColor"));
	color_1->set(Color(255,0,0));
	QVERIFY(colGeom_1->getColor().equals(
				Color(255,0,0,Math::abs(brightness_set_1*80))));

	delete lightSource_1;


	// TODO test referenceObject

}

}
