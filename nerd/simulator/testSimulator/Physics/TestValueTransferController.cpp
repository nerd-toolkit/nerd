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

#include "Core/Core.h"
#include "Physics/DistanceRay.h"
#include "Physics/DistanceSensor.h"
#include "Physics/RayGeom.h"
#include "Physics/SimObjectAdapter.h"
#include "TestValueTransferController.h"
#include "Physics/ValueTransferController.h"
#include <iostream>
#include "Math/Math.h"

using namespace std;
using namespace nerd;


//chris
void TestValueTransferController::testConstructor() {
	Core::resetCore();
	
	ValueTransferController *vtf = new ValueTransferController("TFController");
	
	QCOMPARE(vtf->getParameters().size(), 12);
	
	StringValue *nameOfSource = dynamic_cast<StringValue*>(vtf->getParameter("SourceValueName"));
	StringValue *nameOfTarget = dynamic_cast<StringValue*>(vtf->getParameter("TargetValueName"));
	StringValue *customControllerName = dynamic_cast<StringValue*>(vtf->getParameter("CustomControllerName"));
	StringValue *customSensorName = dynamic_cast<StringValue*>(vtf->getParameter("CustomSensorName"));
	InterfaceValue *controller = dynamic_cast<InterfaceValue*>(vtf->getParameter("Control"));
	InterfaceValue *sensor = dynamic_cast<InterfaceValue*>(vtf->getParameter("Sensor"));
	IntValue *mode = dynamic_cast<IntValue*>(vtf->getParameter("TransferMode"));
	DoubleValue *rate = dynamic_cast<DoubleValue*>(vtf->getParameter("TransferRate"));
	DoubleValue *cost = dynamic_cast<DoubleValue*>(vtf->getParameter("TransferCost"));
	
	QVERIFY(nameOfSource != 0);
	QVERIFY(nameOfTarget != 0);
	QVERIFY(customControllerName != 0);
	QVERIFY(customSensorName != 0);
	QVERIFY(controller != 0);
	QVERIFY(sensor != 0);
	QVERIFY(mode != 0);
	QVERIFY(rate != 0);
	QVERIFY(cost != 0);
	
	QCOMPARE(vtf->getOutputValues().size(), 1);
	QVERIFY(vtf->getOutputValues().contains(sensor));
	QCOMPARE(vtf->getInputValues().size(), 1);
	QVERIFY(vtf->getInputValues().contains(controller));
	
	QVERIFY(controller->getName() == "TFController/Control");
	QVERIFY(sensor->getName() == "TFController/Transfer");
	
	QCOMPARE(controller->getMin(), -1.0);
	QCOMPARE(controller->getMax(), 1.0);
	QCOMPARE(sensor->getMin(), -1.0);
	QCOMPARE(sensor->getMax(), 1.0);
	
	//change the custom names 
	customControllerName->set("NewName");
	QVERIFY(controller->getName() == "TFController/NewName");
	customSensorName->set("NewSensorName");
	QVERIFY(sensor->getName() == "TFController/NewSensorName");
	
	
	//change some values
	nameOfSource->set("MySource");
	nameOfTarget->set("MyTarget");
	mode->set(10);
	rate->set(0.12345);
	cost->set(-5.123);
	
	
	//*************************************************************************************
	//copy constructor
	
	ValueTransferController *copy = dynamic_cast<ValueTransferController*>(vtf->createCopy());
	
	QVERIFY(copy != 0);
	
	copy->setName("MyCopy");
	
	StringValue *nameOfSourceC = dynamic_cast<StringValue*>(copy->getParameter("SourceValueName"));
	StringValue *nameOfTargetC = dynamic_cast<StringValue*>(copy->getParameter("TargetValueName"));
	StringValue *customControllerNameC = dynamic_cast<StringValue*>(copy->getParameter("CustomControllerName"));
	StringValue *customSensorNameC = dynamic_cast<StringValue*>(copy->getParameter("CustomSensorName"));
	InterfaceValue *controllerC = dynamic_cast<InterfaceValue*>(copy->getParameter("Control"));
	InterfaceValue *sensorC = dynamic_cast<InterfaceValue*>(copy->getParameter("Sensor"));
	IntValue *modeC = dynamic_cast<IntValue*>(copy->getParameter("TransferMode"));
	DoubleValue *rateC = dynamic_cast<DoubleValue*>(copy->getParameter("TransferRate"));
	DoubleValue *costC = dynamic_cast<DoubleValue*>(copy->getParameter("TransferCost"));
	
	QVERIFY(nameOfSourceC != 0);
	QVERIFY(nameOfTargetC != 0);
	QVERIFY(customControllerNameC != 0);
	QVERIFY(customSensorNameC != 0);
	QVERIFY(controllerC != 0);
	QVERIFY(sensorC != 0);
	QVERIFY(modeC != 0);
	QVERIFY(rateC != 0);
	QVERIFY(costC != 0);
	
	QVERIFY(vtf->getSource() == 0);
	QVERIFY(vtf->getTarget() == 0);
	
	QCOMPARE(copy->getOutputValues().size(), 1);
	QVERIFY(copy->getOutputValues().contains(sensorC));
	QCOMPARE(copy->getInputValues().size(), 1);
	QVERIFY(copy->getInputValues().contains(controllerC));
	
	QVERIFY(sensor != sensorC);
	QVERIFY(controller != controllerC);
	
	QVERIFY(controllerC->getName() == "MyCopy/NewName");
	QVERIFY(sensorC->getName() == "MyCopy/NewSensorName");
	
	QCOMPARE(modeC->get(), 10);
	QCOMPARE(rateC->get(), 0.12345);
	QCOMPARE(costC->get(), -5.123);
	QVERIFY(nameOfSourceC->get() == "MySource");
	QVERIFY(nameOfTargetC->get() == "MyTarget");
	QVERIFY(customControllerNameC->get() == "NewName");
	QVERIFY(customSensorNameC->get() == "NewSensorName");
	
}

//Chris 
void TestValueTransferController::testSimpleTransfer() {
	Core::resetCore();

	ValueTransferController *vtf = new ValueTransferController("TFController");
	
	StringValue *nameOfSource = dynamic_cast<StringValue*>(vtf->getParameter("SourceValueName"));
	StringValue *nameOfTarget = dynamic_cast<StringValue*>(vtf->getParameter("TargetValueName"));
	InterfaceValue *controller = dynamic_cast<InterfaceValue*>(vtf->getParameter("Control"));
	InterfaceValue *sensor = dynamic_cast<InterfaceValue*>(vtf->getParameter("Sensor"));
	IntValue *mode = dynamic_cast<IntValue*>(vtf->getParameter("TransferMode"));
	DoubleValue *rate = dynamic_cast<DoubleValue*>(vtf->getParameter("TransferRate"));
	DoubleValue *cost = dynamic_cast<DoubleValue*>(vtf->getParameter("TransferCost"));
	
	NormalizedDoubleValue *source = new NormalizedDoubleValue(0.0, 0.0, 1.0, -1.0, 1.0);
	NormalizedDoubleValue *target = new NormalizedDoubleValue(0.0, 0.0, 1.0, -1.0, 1.0);
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/MySource", source);
	vm->addValue("/TheTarget", target);
	
	nameOfSource->set("NonexistingName");
	
	QVERIFY(vtf->getSource() == 0);
	QVERIFY(vtf->getTarget() == 0);
	
	//no valid source and target given, so setup() succeeds, but the controller will not work.
	vtf->setup();
	
	QVERIFY(vtf->getSource() == 0);
	QVERIFY(vtf->getTarget() == 0);
	QVERIFY(vtf->transferActivations() == false);
	
	nameOfSource->set("");
	vtf->setup();
	
	QVERIFY(vtf->getSource() != 0); //uses the internal, infinite source.
	QVERIFY(vtf->getSource() != source);
	QVERIFY(vtf->getTarget() == 0);
	
	
	nameOfSource->set("/MySource"); 
	vtf->setup();
	
	QVERIFY(vtf->getSource() == source); //uses the specified source
	QVERIFY(vtf->getTarget() == 0);
	QVERIFY(vtf->transferActivations() == false);
	
	nameOfTarget->set("/TheTarget");
	
	vtf->setup();
	
	QVERIFY(vtf->getSource() == source);
	QVERIFY(vtf->getTarget() == target);
	
	
	//do the actual transfers
	
	//******************************************
	//Simple model (positive)
	source->set(0.0);
	target->set(0.0);
	controller->set(0.5);
	mode->set(0);
	rate->set(1.0);
	cost->set(0.1);
	
	QCOMPARE(source->get(), 0.0);
	QCOMPARE(target->get(), 0.0);
	
	QVERIFY(vtf->transferActivations() == true);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);
	QCOMPARE(source->get(), 0.0);
	QCOMPARE(target->get(), 0.0);
	
	source->set(0.7);
	target->set(0.0);
	controller->set(0.0); //half strong source->target
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(sensor->getNormalized(), 0.0); //0.5 * maximalTransferRate
	QCOMPARE(source->get(), 0.7); //0.05 transfer + 0.01 cost
	QCOMPARE(target->get(), 0.0);
	
	source->set(0.7);
	target->set(0.0);
	controller->set(0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.5);  
	QVERIFY(Math::compareDoubles(sensor->getNormalized(), 0.454545, 0.0001)); //0.5 * maximalTransferRate (Range [-1.1, 1.1])
	QCOMPARE(source->get(), 0.1); //0.05 transfer + 0.01 cost
	QCOMPARE(target->get(), 0.5);
	
	source->set(0.5);
	target->set(0.0);
	controller->set(0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.4);  
	QVERIFY(Math::compareDoubles(sensor->getNormalized(), 0.36363636, 0.0001));
	QCOMPARE(source->get(), 0.0); 
	QCOMPARE(target->get(), 0.4);
	
	source->set(0.6);
	target->set(0.0);
	controller->set(0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.5);  
	QCOMPARE(source->get(), 0.0); 
	QCOMPARE(target->get(), 0.5);
	
	source->set(0.9);
	target->set(0.1);
	controller->set(0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.5);  
	QCOMPARE(source->get(), 0.3); 
	QCOMPARE(target->get(), 0.6);
	
	source->set(0.8);
	target->set(0.8);
	controller->set(0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.2);  
	QCOMPARE(source->get(), 0.5);
	QCOMPARE(target->get(), 1.0);
	
	source->set(0.8);
	target->set(1.0);
	controller->set(0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 0.8); //does not consume energy, because no transfer takes place.
	QCOMPARE(target->get(), 1.0);
	
	source->set(0.8);
	target->set(0.95);
	controller->set(0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.05);  
	QCOMPARE(source->get(), 0.65);
	QCOMPARE(target->get(), 1.0);
	
	source->set(0.1);
	target->set(0.5);
	controller->set(0.1); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 0.1); //does not consume energy, because no transfer takes plae
	QCOMPARE(target->get(), 0.5);
	
	source->set(0.15);
	target->set(0.5);
	controller->set(0.1); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.05);  
	QCOMPARE(source->get(), 0.0);
	QCOMPARE(target->get(), 0.55);
	
	
	//******************************************
	//Simple model (negative) target->source
	source->set(0.0);
	target->set(0.0);
	controller->set(-0.5);
	mode->set(0);
	rate->set(1.0);
	cost->set(0.1);
	
	QCOMPARE(source->get(), 0.0);
	QCOMPARE(target->get(), 0.0);
	
	QVERIFY(vtf->transferActivations() == true);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);
	QCOMPARE(source->get(), 0.0);
	QCOMPARE(target->get(), 0.0);
	
	source->set(0.0);
	target->set(0.7);
	controller->set(0.0); //half strong source->target
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(sensor->getNormalized(), 0.0); //0.5 * maximalTransferRate
	QCOMPARE(source->get(), 0.0); //0.05 transfer + 0.01 cost
	QCOMPARE(target->get(), 0.7);
	
	source->set(0.0);
	target->set(0.7);
	controller->set(-0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), -0.5);  
	QVERIFY(Math::compareDoubles(sensor->getNormalized(), -0.454545, 0.0001)); //0.5 * maximalTransferRate
	QCOMPARE(source->get(), 0.5); //0.05 transfer + 0.01 cost
	QCOMPARE(target->get(), 0.1);
	
	source->set(0.0);
	target->set(0.5);
	controller->set(-0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), -0.4);  
	QVERIFY(Math::compareDoubles(sensor->getNormalized(), -0.36363636, 0.0001));
	QCOMPARE(source->get(), 0.4); 
	QCOMPARE(target->get(), 0.0);
	
	source->set(0.0);
	target->set(0.6);
	controller->set(-0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), -0.5);  
	QCOMPARE(source->get(), 0.5); 
	QCOMPARE(target->get(), 0.0);
	
	source->set(0.1);
	target->set(0.9);
	controller->set(-0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), -0.5);  
	QCOMPARE(source->get(), 0.6); 
	QCOMPARE(target->get(), 0.3);
	
	source->set(0.8);
	target->set(0.8);
	controller->set(-0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), -0.2);  
	QCOMPARE(source->get(), 1.0);
	QCOMPARE(target->get(), 0.5);
	
	source->set(1.0);
	target->set(0.8);
	controller->set(-0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 1.0);
	QCOMPARE(target->get(), 0.8); //no transfer took place => no energy used.
	
	source->set(0.95);
	target->set(0.8);
	controller->set(-0.5); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), -0.05);  
	QCOMPARE(source->get(), 1.0);
	QCOMPARE(target->get(), 0.65);
	
	source->set(0.5);
	target->set(0.1);
	controller->set(-0.1); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 0.5); 
	QCOMPARE(target->get(), 0.1); //no transfer took place => no energy used.
	
	source->set(0.5);
	target->set(0.15);
	controller->set(-0.1); 
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), -0.05);  
	QCOMPARE(source->get(), 0.55);
	QCOMPARE(target->get(), 0.0);
	
	//******************************************
	//Some more arbitrary configurations
	
	source->setMin(-0.1);
	source->setMax(0.5);
	source->set(0.3);
	target->setMin(0.1);
	target->setMax(0.6);
	target->set(0.2);
	controller->set(-0.2);
	mode->set(0);
	rate->set(0.4); //0.08
	cost->set(0.01);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), -0.08);
	QVERIFY(Math::compareDoubles(sensor->getNormalized(), -0.195122, 0.0001));
	QCOMPARE(source->get(), 0.38); //+0.08
	QCOMPARE(target->get(), 0.11); //-0.09 = -0.08 - 0.01 cost
	
	
	//not enough energy left in source.
	source->setMin(-0.1);
	source->setMax(0.5);
	source->set(0.0);
	target->setMin(0.1);
	target->setMax(0.6);
	target->set(0.5);
	controller->set(0.5);
	rate->set(0.4); 
	cost->set(0.05);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.05);
	QVERIFY(Math::compareDoubles(sensor->getNormalized(), 0.111111, 0.0001));
	QCOMPARE(source->get(), -0.1); //
	QCOMPARE(target->get(), 0.55); //
	
	
	//not enough space left at target
	source->setMin(-0.1);
	source->setMax(0.5);
	source->set(0.5);
	target->setMin(0.1);
	target->setMax(0.6);
	target->set(0.5);
	controller->set(0.5);
	rate->set(0.4); 
	cost->set(0.05);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.1);
	QVERIFY(Math::compareDoubles(sensor->getNormalized(), 0.22222, 0.0001));
	QCOMPARE(source->get(), 0.35); //-0.15 = -(0.1 + 0.05 cost)
	QCOMPARE(target->get(), 0.6); //+0.1 (maxed out)
	
	
	//should work fine
	source->setMin(-0.1);
	source->setMax(0.5);
	source->set(0.5);
	target->setMin(0.1);
	target->setMax(0.6);
	target->set(0.2);
	controller->set(0.75);
	rate->set(0.4); 
	cost->set(0.05);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.3);
	QVERIFY(Math::compareDoubles(sensor->getNormalized(), 0.666667, 0.0001));
	QCOMPARE(source->get(), 0.15); //
	QCOMPARE(target->get(), 0.5); //
}


void TestValueTransferController::testAutomaticSimpleTransfer() {
	Core::resetCore();
	
	ValueTransferController *vtf = new ValueTransferController("TFController", true);
	
	QCOMPARE(vtf->getParameters().size(), 6);
	
	StringValue *nameOfSource = dynamic_cast<StringValue*>(vtf->getParameter("SourceValueName"));
	StringValue *nameOfTarget = dynamic_cast<StringValue*>(vtf->getParameter("TargetValueName"));
	StringValue *customControllerName = dynamic_cast<StringValue*>(vtf->getParameter("CustomControllerName"));
	StringValue *customSensorName = dynamic_cast<StringValue*>(vtf->getParameter("CustomSensorName"));
	InterfaceValue *controller = dynamic_cast<InterfaceValue*>(vtf->getParameter("Control"));
	InterfaceValue *sensor = dynamic_cast<InterfaceValue*>(vtf->getParameter("Sensor"));
	IntValue *mode = dynamic_cast<IntValue*>(vtf->getParameter("TransferMode"));
	DoubleValue *rate = dynamic_cast<DoubleValue*>(vtf->getParameter("TransferRate"));
	DoubleValue *cost = dynamic_cast<DoubleValue*>(vtf->getParameter("TransferCost"));
	
	QVERIFY(nameOfSource != 0); //!
	QVERIFY(nameOfTarget != 0); //!
	QVERIFY(customControllerName == 0);
	QVERIFY(customSensorName == 0);
	QVERIFY(controller == 0);
	QVERIFY(sensor == 0);
	QVERIFY(mode != 0); //!
	QVERIFY(rate != 0); //!
	QVERIFY(cost != 0); //!
	
	NormalizedDoubleValue *target = new NormalizedDoubleValue(0.0, 0.0, 1.0, -1.0, 1.0);
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/TheTarget", target);
	
	QVERIFY(vtf->getSource() == 0);
	QVERIFY(vtf->getTarget() == 0);
	
	//no valid source and target given, so setup() succeeds, but the controller will not work.
	vtf->setup();
	
	QVERIFY(vtf->getSource() != 0);
	QVERIFY(vtf->getTarget() == 0);
	QVERIFY(vtf->transferActivations() == false);
	
	nameOfTarget->set("/TheTarget");
	
	vtf->setup();
	
	QVERIFY(vtf->getSource() != 0);
	QVERIFY(vtf->getTarget() == target);
	
	//do the automatic transfer
	
	target->set(0.5);
	mode->set(0);
	rate->set(-0.01);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(target->get(), 0.49);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(target->get(), 0.48);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(target->get(), 0.47);
	
	rate->set(-0.1);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(target->get(), 0.37);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(target->get(), 0.27);
	
	rate->set(0.05);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(target->get(), 0.32);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(target->get(), 0.37);
}

void TestValueTransferController::testBothProportional() {
	Core::resetCore();
	
	ValueTransferController *vtf = new ValueTransferController("TFController");

	StringValue *nameOfSource = dynamic_cast<StringValue*>(vtf->getParameter("SourceValueName"));
	StringValue *nameOfTarget = dynamic_cast<StringValue*>(vtf->getParameter("TargetValueName"));
	InterfaceValue *controller = dynamic_cast<InterfaceValue*>(vtf->getParameter("Control"));
	InterfaceValue *sensor = dynamic_cast<InterfaceValue*>(vtf->getParameter("Sensor"));
	IntValue *mode = dynamic_cast<IntValue*>(vtf->getParameter("TransferMode"));
	DoubleValue *rate = dynamic_cast<DoubleValue*>(vtf->getParameter("TransferRate"));
	DoubleValue *cost = dynamic_cast<DoubleValue*>(vtf->getParameter("TransferCost"));
	
	QVERIFY(nameOfSource != 0);
	QVERIFY(nameOfTarget != 0);
	

	NormalizedDoubleValue *source = new NormalizedDoubleValue(0.0, 0.0, 1.0, -1.0, 1.0);
	NormalizedDoubleValue *target = new NormalizedDoubleValue(0.0, 0.0, 1.0, -1.0, 1.0);
	

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/MySource", source);
	vm->addValue("/TheTarget", target);
	

	nameOfSource->set("/MySource"); 
	nameOfTarget->set("/TheTarget");
	

	vtf->setup();
	
	QVERIFY(vtf->getSource() == source);
	QVERIFY(vtf->getTarget() == target);
	

	//do the actual transfers
	
	//******************************************
	//BothProportional Model
	
	source->set(0.0);
	target->set(0.0);
	controller->set(0.5);
	mode->set(1);
	rate->set(-1.0);
	cost->set(-0.5);
	

	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);
	QCOMPARE(source->get(), 0.0);
	QCOMPARE(target->get(), 0.0);
	
	source->set(0.8);
	target->set(0.0);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 0.8); 
	QCOMPARE(target->get(), 0.0);
	

	source->set(0.0);
	target->set(0.8);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 0.0); 
	QCOMPARE(target->get(), 0.8);
	
	source->set(0.8);
	target->set(0.8);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.5);  
	QCOMPARE(source->get(), 0.55); 
	QCOMPARE(target->get(), 0.3);
	
	source->set(0.8);
	target->set(0.4);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.4);  
	QCOMPARE(source->get(), 0.6); 
	QCOMPARE(target->get(), 0.0);
	
	source->set(0.125);
	target->set(0.9);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.25);  
	QCOMPARE(source->get(), 0.0); 
	QCOMPARE(target->get(), 0.65);
	
	source->set(0.125);
	target->set(0.9);
	controller->set(1.0);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.25);  
	QCOMPARE(source->get(), 0.0); 
	QCOMPARE(target->get(), 0.65);
	
	source->set(0.125);
	target->set(0.9);
	controller->set(-1.0);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 0.125); 
	QCOMPARE(target->get(), 0.9);
	
	
	//positive changes
	//***************************************************
	cost->set(0.5);
	rate->set(1.0);
	source->set(0.1);
	target->set(1.0);
	controller->set(0.5);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 0.1); 
	QCOMPARE(target->get(), 1.0);
	
	source->set(1.0);
	target->set(0.2);
	controller->set(0.5);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 1.0); 
	QCOMPARE(target->get(), 0.2);
	
	source->set(0.1);
	target->set(0.2);
	controller->set(0.5);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.5);
	QCOMPARE(source->get(), 0.35);
	QCOMPARE(target->get(), 0.7);
	
	source->set(0.1);
	target->set(0.75);
	controller->set(0.5);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.25);
	QCOMPARE(source->get(), 0.225);
	QCOMPARE(target->get(), 1.0);
	
	source->set(0.975);
	target->set(0.2);
	controller->set(0.1);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.05);
	QCOMPARE(source->get(), 1.0);
	QCOMPARE(target->get(), 0.25);
	
	
	//opposite change directions
	//***************************************************
	cost->set(-0.5);
	rate->set(1.0);
	source->set(0.0);
	target->set(0.2);
	controller->set(0.5);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 0.0); 
	QCOMPARE(target->get(), 0.2);
	
	source->set(0.9);
	target->set(1.0);
	controller->set(0.5);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);  
	QCOMPARE(source->get(), 0.9); 
	QCOMPARE(target->get(), 1.0);
	
	source->set(0.9);
	target->set(0.2);
	controller->set(0.5);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.5);  
	QCOMPARE(source->get(), 0.65); 
	QCOMPARE(target->get(), 0.7);
	
	source->set(0.125);
	target->set(0.2);
	controller->set(0.5);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.25);  
	QCOMPARE(source->get(), 0.0); 
	QCOMPARE(target->get(), 0.45);
	
	source->set(0.9);
	target->set(0.9);
	controller->set(0.5);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.1);  
	QCOMPARE(source->get(), 0.85); 
	QCOMPARE(target->get(), 1.0);
	
}


void TestValueTransferController::testEquilibrium() {
	Core::resetCore();
	
	ValueTransferController *vtf = new ValueTransferController("TFController");
	
	StringValue *nameOfSource = dynamic_cast<StringValue*>(vtf->getParameter("SourceValueName"));
	StringValue *nameOfTarget = dynamic_cast<StringValue*>(vtf->getParameter("TargetValueName"));
	InterfaceValue *controller = dynamic_cast<InterfaceValue*>(vtf->getParameter("Control"));
	InterfaceValue *sensor = dynamic_cast<InterfaceValue*>(vtf->getParameter("Sensor"));
	IntValue *mode = dynamic_cast<IntValue*>(vtf->getParameter("TransferMode"));
	DoubleValue *rate = dynamic_cast<DoubleValue*>(vtf->getParameter("TransferRate"));
	DoubleValue *cost = dynamic_cast<DoubleValue*>(vtf->getParameter("TransferCost"));
	
	QVERIFY(nameOfSource != 0);
	QVERIFY(nameOfTarget != 0);
	
	
	NormalizedDoubleValue *source = new NormalizedDoubleValue(0.0, 0.0, 1.0, -1.0, 1.0);
	NormalizedDoubleValue *target = new NormalizedDoubleValue(0.0, 0.0, 1.0, -1.0, 1.0);
	
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/MySource", source);
	vm->addValue("/TheTarget", target);
	
	
	nameOfSource->set("/MySource"); 
	nameOfTarget->set("/TheTarget");
	
	
	vtf->setup();
	
	QVERIFY(vtf->getSource() == source);
	QVERIFY(vtf->getTarget() == target);
	
	
	//do the actual transfers
	
	//******************************************
	//Equilibrium Model (target only)
	
	source->set(0.0);
	target->set(0.0);
	controller->set(0.5);
	mode->set(2); //equilibrium target only
	rate->set(0.2);
	cost->set(0.4);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);
	QCOMPARE(source->get(), 0.0);
	QCOMPARE(target->get(), 0.0);
	
	
	source->set(1);
	target->set(0.0);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.2);
	QCOMPARE(source->get(), 1.0);
	QCOMPARE(target->get(), 0.2);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.16);
	QCOMPARE(source->get(), 1.0);
	QCOMPARE(target->get(), 0.36);
	
	
	source->set(0.5);
	target->set(0.7);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), -0.04);
	QCOMPARE(source->get(), 0.5);
	QCOMPARE(target->get(), 0.66);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), -0.032);
	QCOMPARE(source->get(), 0.5);
	QCOMPARE(target->get(), 0.628);
	
	
	//******************************************
	//Equilibrium Model (change both)
	
	source->set(0.0);
	target->set(0.0);
	controller->set(0.5);
	mode->set(3); //equlibrium mutual
	cost->set(0.4);
	rate->set(0.2);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);
	QCOMPARE(source->get(), 0.0);
	QCOMPARE(target->get(), 0.0);
	
	
	source->set(1);
	target->set(0.0);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.6);
	QCOMPARE(source->get(), 0.6);
	QCOMPARE(target->get(), 0.2);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.24);
	QCOMPARE(source->get(), 0.44);
	QCOMPARE(target->get(), 0.28);
	
	
	source->set(0.1);
	target->set(0.7);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.36);
	QCOMPARE(source->get(), 0.34);
	QCOMPARE(target->get(), 0.58);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.144);
	QCOMPARE(source->get(), 0.436);
	QCOMPARE(target->get(), 0.532);
	
	//rate/cost sum > 1 (error is fully corrected in a single step)
	cost->set(2.4);
	rate->set(0.8);
	source->set(0.1);
	target->set(0.7);
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.6);
	QCOMPARE(source->get(), 0.55);
	QCOMPARE(target->get(), 0.55);
	
	updateActuatorAndSensor(vtf);
	QCOMPARE(sensor->get(), 0.0);
	QCOMPARE(source->get(), 0.55);
	QCOMPARE(target->get(), 0.55);
}


void TestValueTransferController::updateActuatorAndSensor(ValueTransferController *controller) {
	if(controller == 0) {
		return;
	}
	controller->updateActuators();
	controller->updateSensorValues();
}
