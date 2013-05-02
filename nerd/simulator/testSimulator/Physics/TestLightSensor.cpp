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
#include "TestLightSensor.h"

using namespace std;

namespace nerd{

// Josef WORK IN PROGRESS
void TestLightSensor::testConstruction() {
	Core::resetCore();
	Core *core = Core::getInstance();

	DoubleValue *timeStep = new DoubleValue(0.01);
	core->getValueManager()->addValue(SimulationConstants::VALUE_TIME_STEP_SIZE, timeStep);
	DoubleValue *gravitation = new DoubleValue(9.81);
	core->getValueManager()->addValue("/Simulation/Gravitation", gravitation);

	// construct default sensor
	LightSensor *lightSensor_1 = new LightSensor("LightSensor_1");
	QVERIFY(lightSensor_1 != 0);

	// check default parameters
	QVERIFY(dynamic_cast<StringValue*>(lightSensor_1->getParameter("HostBody"))->get() == "");
	QVERIFY(dynamic_cast<DoubleValue*>(lightSensor_1->getParameter("Noise"))->get() == 0.0);

	InterfaceValue *brightness_1 = dynamic_cast<InterfaceValue*>(lightSensor_1->getParameter("Brightness"));
	QVERIFY(brightness_1->getName() == "Brightness");
	QVERIFY(brightness_1->getMin() == 0.0);
	QVERIFY(brightness_1->getMax() == 1.0);

	Vector3DValue *localPosition_1 = dynamic_cast<Vector3DValue*>(lightSensor_1->getParameter("LocalPosition"));
	QVERIFY(localPosition_1->getX() == 0);
	QVERIFY(localPosition_1->getY() == 0);
	QVERIFY(localPosition_1->getZ() == 0);

	QVERIFY(dynamic_cast<BoolValue*>(lightSensor_1->getParameter("UseAsAmbientLightSensor"))->get() == false);
	// TODO angleDifferences
	QVERIFY(dynamic_cast<StringValue*>(lightSensor_1->getParameter("DetectableLightTypes"))->get() == "0");
	QVERIFY(dynamic_cast<DoubleValue*>(lightSensor_1->getParameter("MaxDetectionAngle"))->get() == 180.0);
	QVERIFY(dynamic_cast<RangeValue*>(lightSensor_1->getParameter("DetectableRange"))->getMin() == 0.0);
	QVERIFY(dynamic_cast<RangeValue*>(lightSensor_1->getParameter("DetectableRange"))->getMax() == 1.0);

	// TODO collisionObject, outputValues, copy, setup, ...

}

// josef
void TestLightSensor::testSensor() {
	// TODO
}

}
