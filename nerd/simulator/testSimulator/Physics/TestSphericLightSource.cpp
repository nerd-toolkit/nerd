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

using namespace std;

namespace nerd{

// Josef WORK IN PROGRESS
void TestSphericLightSource::testConstruction() {
	Core::resetCore();
	Core::getInstance();
	
	DoubleValue *timeStep = new DoubleValue(0.01);
	Core::getInstance()->getValueManager()->addValue(
			SimulationConstants::VALUE_TIME_STEP_SIZE, timeStep);

	double brightness_set_1 = 0.564;
	double range_set_1 = 4.2;
	int type_set_1 = 1;
	SphericLightSource *lightSource_1 = new SphericLightSource("LightSource_1",
																brightness_set_1,
																range_set_1,
																type_set_1);

	// check defaults and set initial values
	QVERIFY(lightSource_1->getRange() == range_set_1);
 	QCOMPARE(lightSource_1->getCurrentBrightness(), brightness_set_1);
 	QCOMPARE(lightSource_1->getDesiredBrightness(), brightness_set_1);
 
	// default light color should be "yellow", but is "black" WHY? TODO
	// ColorValue seems to be working ok on itself, Tests are running and
	// seem sufficiently constructed (see TestValue.cpp)
	ColorValue* color_1 = dynamic_cast<ColorValue*>(lightSource_1->
											getParameter("Color"));
	QWARN("LightColor: " + color_1->getValueAsString().toLocal8Bit());
	//QVERIFY(color_1->get().equals(Color(255,255,0)));
	//QVERIFY(dynamic_cast<ColorValue*>(lightSource_1->getParameter("Color"))->
	//										getValueAsString() == "yellow");
	//
 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_1->getParameter("HideLightCone"))->get() == false);
 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_1->getParameter("UseSphericLightCone"))->get() == false);
 	QVERIFY(dynamic_cast<StringValue*>(lightSource_1->getParameter("ReferenceObject"))->get() == "");
	Vector3DValue* localPosition_1 = dynamic_cast<Vector3DValue*>
									(lightSource_1->getParameter("LocalPosition"));
	QVERIFY(localPosition_1->getX() == 0);
	QVERIFY(localPosition_1->getY() == 0);
	QVERIFY(localPosition_1->getZ() == 0);
 	QVERIFY(dynamic_cast<BoolValue*>(lightSource_1->getParameter("UniformLight"))->get() == false);
 	QVERIFY(dynamic_cast<RangeValue*>(lightSource_1->getParameter("BrightnessRange"))->getMin() == 0);
 	QVERIFY(dynamic_cast<RangeValue*>(lightSource_1->getParameter("BrightnessRange"))->getMax() == 1);
 
 	QVERIFY(lightSource_1->getOutputValues().size() == 1);
 	QVERIFY(lightSource_1->getInputValues().size() == 1);

	// collisionObject
	QVERIFY(lightSource_1->getCollisionObjects().size() == 1);
	CollisionObject* colObj_1 = lightSource_1->getCollisionObjects().first();
	CylinderGeom* colGeom_1 = dynamic_cast<CylinderGeom*>(colObj_1->getGeometry());
	QVERIFY(colGeom_1 != 0);
	//QVERIFY(colObj_1->getMaterialType() == "Light");

	// TODO lots!

}
};
