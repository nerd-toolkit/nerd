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



#include "TestSimJoint.h"
#include "Physics/SimJoint.h"
#include "Physics/SimBody.h"
#include "Value/StringValue.h"
#include "Core/Core.h"
#include "Physics/HingeJoint.h"
#include "Physics/ServoMotor.h"
#include "Physics/Physics.h"
#include "Physics/BoxBody.h"
#include "Math/Math.h"
#include "Physics/SphereGeom.h"
#include "Physics/TorqueDrivenDynamixel.h"
#include "Physics/SliderJoint.h"
#include "Physics/Dynamixel.h"
#include <limits>
#include <iostream>

using namespace std;

using namespace nerd;

// TODO: add test of new mechanism of using the parameters to define the two joint-bodies.

void TestSimJoint::testConstructorAndSetBodies(){
	Core::resetCore();

	//standard constructor.
	SimJoint sj1("SimJoint1");
	QVERIFY(sj1.getName().compare("SimJoint1") == 0);
	QVERIFY(sj1.getPrefix().compare("") == 0);
	
	QVERIFY(sj1.getFirstBody() == 0);
	QVERIFY(sj1.getSecondBody() == 0);
	
	StringValue *name1 = dynamic_cast<StringValue*>(sj1.getParameter("FirstBody"));
	StringValue *name2 = dynamic_cast<StringValue*>(sj1.getParameter("SecondBody"));
	
	QVERIFY(name1 != 0);
	QVERIFY(name2 != 0);
	QVERIFY(name1->get().compare("") == 0);
	QVERIFY(name2->get().compare("") == 0);

	QCOMPARE(sj1.getGeometries().size(), 0);
	QCOMPARE(sj1.getInfoValues().size(), 0);
	QCOMPARE(sj1.getInputValues().size(), 0);
	QCOMPARE(sj1.getOutputValues().size(), 0);
	
	//add SimBodies
// 	SimBody b1("Body1");
// 	SimBody b2("Body2");
// 	SimBody b3("Body3");
// 	Physics::getPhysicsManager()->addSimObject(&b1);
// 	Physics::getPhysicsManager()->addSimObject(&b2);
// 	Physics::getPhysicsManager()->addSimObject(&b3);

	BoxBody *b1 = new BoxBody("Box1");
	b1->getPositionValue()->set(0.0, 0.0, 0.0);
	Physics::getPhysicsManager()->addSimObject(b1);
	
	BoxBody *b2= new BoxBody("Box2");
	b2->getPositionValue()->set(0.0, 0.0, 0.0);
	Physics::getPhysicsManager()->addSimObject(b2);
	
	BoxBody *b3= new BoxBody("Box3");
	b3->getPositionValue()->set(0.0, 0.0, 0.0);
	Physics::getPhysicsManager()->addSimObject(b3);
	

	sj1.addGeometry(new SphereGeom(b1, 0.2));
	QCOMPARE(sj1.getGeometries().size(), 1);
	
// 	add first body.
	sj1.getParameter("FirstBody")->setValueFromString("Box1");
	sj1.setup();
// 	sj1.setFirstBody(&b1);
	QVERIFY(sj1.getFirstBody() == b1);
	QVERIFY(sj1.getSecondBody() == 0);
	QVERIFY(name1->get().compare("Box1") == 0);
	QVERIFY(name2->get().compare("") == 0);
	
	//add second body.
	sj1.getParameter("SecondBody")->setValueFromString("Box2");
	sj1.clear();
	sj1.setup();
// 	sj1.setSecondBody(&b2);
	QVERIFY(sj1.getFirstBody() == b1);
	QVERIFY(sj1.getSecondBody() == b2);
	QVERIFY(name1->get().compare("Box1") == 0);
	QVERIFY(name2->get().compare("Box2") == 0);
	
	//remove body1
	sj1.getParameter("FirstBody")->setValueFromString("");
	sj1.clear();
	sj1.setup();
// 	sj1.setFirstBody(0);
	QVERIFY(sj1.getFirstBody() == 0);
	QVERIFY(sj1.getSecondBody() == b2);
	QVERIFY(name1->get().compare("") == 0);
	QVERIFY(name2->get().compare("Box2") == 0);
	
	//set both bodies simultaneous
// 	sj1.setBodies(&b3, &b1);
	sj1.getParameter("FirstBody")->setValueFromString("Box3");
	sj1.getParameter("SecondBody")->setValueFromString("Box1");
	sj1.clear();
	sj1.setup();
	QVERIFY(sj1.getFirstBody() == b3);
	QVERIFY(sj1.getSecondBody() == b1);
	QVERIFY(name1->get().compare("Box3") == 0);
	QVERIFY(name2->get().compare("Box1") == 0);
	
	
// 	createCopy SimJoint (SimBodies are NOT copied and set to NULL)
	SimJoint *sj2 = dynamic_cast<SimJoint*>(sj1.createCopy());

	QCOMPARE(sj1.getGeometries().size(), 1);
	QCOMPARE(sj2->getGeometries().size(), 0);
	QCOMPARE(sj2->getInfoValues().size(), 0);
	QCOMPARE(sj2->getInputValues().size(), 0);
	QCOMPARE(sj2->getOutputValues().size(), 0);
	
	QVERIFY(sj2 != 0);
	QVERIFY(sj2->getName().compare("SimJoint1") == 0);
	QVERIFY(sj2->getPrefix().compare("") == 0);
	
	QVERIFY(sj2->getFirstBody() == 0);
	QVERIFY(sj2->getSecondBody() == 0);
	
	StringValue *name3 = dynamic_cast<StringValue*>(sj2->getParameter("FirstBody"));
	StringValue *name4 = dynamic_cast<StringValue*>(sj2->getParameter("SecondBody"));
	
	QVERIFY(name3 != 0);
	QVERIFY(name4 != 0);
	QVERIFY(name1 != name3);
	QVERIFY(name2 != name4);
	QVERIFY(name3->get().compare("Box3") == 0);
	QVERIFY(name4->get().compare("Box1") == 0);
	
	delete sj2;
	
	
}



void TestSimJoint::testHingeJoint() {
	Core::resetCore();
	
	HingeJoint hj1("Hinge1");
	
	Vector3DValue *axisPoint1 = dynamic_cast<Vector3DValue*>(hj1.getParameter("AxisPoint1"));
	Vector3DValue *axisPoint2 = dynamic_cast<Vector3DValue*>(hj1.getParameter("AxisPoint2"));
	
	QVERIFY(axisPoint1 != 0);
	QVERIFY(axisPoint2 != 0);
	
	axisPoint1->set(1.0, 2.0, 3.0);
	axisPoint2->set(3.0, 4.0, 5.0);
	
	
	//create a copy of the HingeJoint.
	HingeJoint *hj2 = dynamic_cast<HingeJoint*>(hj1.createCopy());
	QVERIFY(hj2->getName().compare("Hinge1") == 0);
	
	Vector3DValue *axisPoint1b = dynamic_cast<Vector3DValue*>(hj2->getParameter("AxisPoint1"));
	Vector3DValue *axisPoint2b = dynamic_cast<Vector3DValue*>(hj2->getParameter("AxisPoint2"));
	
	QVERIFY(axisPoint1b != 0);
	QVERIFY(axisPoint2b != 0);
	
	QVERIFY(axisPoint1b->get().equals(axisPoint1->get()));
	QVERIFY(axisPoint2b->get().equals(axisPoint2->get()));
	
	delete hj2;
	
}

void TestSimJoint::testSliderJoint() {
	Core::resetCore();

	SliderJoint *slider = new SliderJoint("SliderJoint");
	QCOMPARE(slider->getName(), QString("SliderJoint"));
	QCOMPARE(slider->getParameter("Name")->getValueAsString(), QString("SliderJoint"));

	QCOMPARE(dynamic_cast<DoubleValue*>(slider->getParameter("MinPosition"))->get(), -1 * std::numeric_limits<double>::max());
	slider->getParameter("MinPosition")->setValueFromString("2.1");
	QCOMPARE(dynamic_cast<DoubleValue*>(slider->getParameter("MaxPosition"))->get(), std::numeric_limits<double>::max());
	QCOMPARE(slider->getParameter("FirstBody")->getValueAsString(), QString(""));
	QCOMPARE(slider->getParameter("AxisPoint1")->getValueAsString(),QString("(0,0,0)"));
	slider->getParameter("AxisPoint1")->setValueFromString("(1.0,0.0,2.2)");
	QCOMPARE(slider->getParameter("AxisPoint1")->getValueAsString(), QString("(1,0,2.2)"));

	slider->getParameter("FirstBody")->setValueFromString("Body1");
	QCOMPARE(slider->getParameter("FirstBody")->getValueAsString(), QString("Body1"));

	SliderJoint *copy = dynamic_cast<SliderJoint*>(slider->createCopy());
	QVERIFY(copy != 0);
	QCOMPARE(copy->getParameter("FirstBody")->getValueAsString(), QString("Body1"));
	QCOMPARE(copy->getParameter("MinPosition")->getValueAsString(), QString("2.1"));
	QCOMPARE(dynamic_cast<DoubleValue*>(copy->getParameter("MaxPosition"))->get(), std::numeric_limits<double>::max());
	QCOMPARE(copy->getParameter("AxisPoint1")->getValueAsString(), QString("(1,0,2.2)"));

}

void TestSimJoint:: testServoMotorJoint() {
	Core::resetCore();
	
	ServoMotor mj1("ServoMotorJoint1");
	
	DoubleValue *pid_Proportional = dynamic_cast<DoubleValue*>(mj1.getParameter("PID_P"));
	DoubleValue *pid_Integral = dynamic_cast<DoubleValue*>(mj1.getParameter("PID_I"));
	DoubleValue *pid_Differential = dynamic_cast<DoubleValue*>(mj1.getParameter("PID_D"));
	DoubleValue *minAngle = dynamic_cast<DoubleValue*>(mj1.getParameter("MinAngle"));
	DoubleValue *maxAngle = dynamic_cast<DoubleValue*>(mj1.getParameter("MaxAngle"));
	DoubleValue *maxTorque = dynamic_cast<DoubleValue*>(mj1.getParameter("MaxTorque"));
	BoolValue *controlMotorAngle = dynamic_cast<BoolValue*>(mj1.getParameter("IsAngularMotor"));
	InterfaceValue *desiredMotorSetting =	
						dynamic_cast<InterfaceValue*>(mj1.getParameter("DesiredSetting"));
	
	QVERIFY(pid_Proportional != 0);
	QVERIFY(pid_Integral != 0);
	QVERIFY(pid_Differential != 0);
	QVERIFY(minAngle != 0);
	QVERIFY(maxAngle != 0);
	QVERIFY(maxTorque != 0);
	QVERIFY(controlMotorAngle != 0);
	QVERIFY(desiredMotorSetting != 0);
	
	//TODO check default values once plausible default values have been set.
	
	//interface value
// 	qDebug("%s", desiredMotorSetting->getName().toStdString().c_str());
	QVERIFY(desiredMotorSetting->getName().compare("ServoMotorJoint1/DesiredSetting") == 0);
	QVERIFY(desiredMotorSetting->getMin() == -10.0);
	QVERIFY(desiredMotorSetting->getMax() == 10.0);
	QVERIFY(desiredMotorSetting->get() == 0.0);
	
	minAngle->set(-100.0);
	QVERIFY(desiredMotorSetting->getMin() == -10.0);
	QVERIFY(minAngle->get() == -100.0);
	maxAngle->set(500.0);
	QVERIFY(desiredMotorSetting->getMax() == 10.0);
	
	//min/max of motor angle is NOT coupled to changes in the interface value
	desiredMotorSetting->setMin(5.0);
	desiredMotorSetting->setMax(15.0);
	QVERIFY(minAngle->get() == -100.0);
	QVERIFY(maxAngle->get() == 500.0);
	
	//control motor angle
	QVERIFY(controlMotorAngle->get() == true);
	QVERIFY(mj1.isControllingMotorAngle() == true);
	mj1.controlMotorAngle(false);
	QVERIFY(controlMotorAngle->get() == false);
	QVERIFY(mj1.isControllingMotorAngle() == false);
	
	ServoMotor *copy = dynamic_cast<ServoMotor*>(mj1.createCopy());
	QCOMPARE(copy->getInputValues().size(), 1);
	QCOMPARE(copy->getOutputValues().size(), 1);

	DoubleValue *cpid_Proportional = dynamic_cast<DoubleValue*>(copy->getParameter("PID_P"));
	DoubleValue *cpid_Integral = dynamic_cast<DoubleValue*>(copy->getParameter("PID_I"));
	DoubleValue *cpid_Differential = dynamic_cast<DoubleValue*>(copy->getParameter("PID_D"));
	DoubleValue *cminAngle = dynamic_cast<DoubleValue*>(copy->getParameter("MinAngle"));
	DoubleValue *cmaxAngle = dynamic_cast<DoubleValue*>(copy->getParameter("MaxAngle"));
	DoubleValue *cmaxTorque = dynamic_cast<DoubleValue*>(copy->getParameter("MaxTorque"));
	BoolValue *ccontrolMotorAngle = dynamic_cast<BoolValue*>(copy->getParameter("IsAngularMotor"));
	InterfaceValue *cdesiredMotorSetting =	
						dynamic_cast<InterfaceValue*>(copy->getParameter("DesiredSetting"));
	
	QVERIFY(cpid_Proportional != 0);
	QVERIFY(cpid_Integral != 0);
	QVERIFY(cpid_Differential != 0);
	QVERIFY(cminAngle != 0);
	QVERIFY(cmaxAngle != 0);
	QVERIFY(cmaxTorque != 0);
	QVERIFY(ccontrolMotorAngle != 0);
	QVERIFY(cdesiredMotorSetting != 0);
	
	QCOMPARE(cpid_Proportional->get(), pid_Proportional->get());
	QCOMPARE(cpid_Integral->get(), pid_Integral->get());
	QCOMPARE(cpid_Differential->get(), pid_Differential->get());
	QCOMPARE(cminAngle->get(), minAngle->get());
	QCOMPARE(maxAngle->get(), maxAngle->get());
	QCOMPARE(ccontrolMotorAngle->get(), controlMotorAngle->get());
	QCOMPARE(cdesiredMotorSetting->get(), desiredMotorSetting->get());

	QVERIFY(copy->getInputValues().at(0) != mj1.getInputValues().at(0));
	QVERIFY(copy->getOutputValues().at(0) != mj1.getOutputValues().at(0));

	QCOMPARE(copy->getName(), QString("ServoMotorJoint1"));
	QCOMPARE(copy->getInputValues().at(0)->getName(), QString("ServoMotorJoint1/DesiredSetting"));

	copy->setName("Copy");
	QCOMPARE(copy->getInputValues().at(0)->getName(), QString("Copy/DesiredSetting"));
	
}

void TestSimJoint::testDynamixelMotor() {
	Core::resetCore();

}

void TestSimJoint::testDynamixel() {
	Core::resetCore();
	DoubleValue *timeStep = new DoubleValue(0.01);
	Core::getInstance()->getValueManager()->addValue(SimulationConstants::VALUE_TIME_STEP_SIZE, timeStep);
	Dynamixel dynamixel("Dynamixel");
	
	QVERIFY(dynamixel.getParameter("MinAngle") != 0);
	QVERIFY(dynamixel.getParameter("MaxAngle") != 0);
	QVERIFY(dynamixel.getParameter("DesiredAngle") != 0);
	QVERIFY(dynamixel.getParameter("DesiredTorque") != 0);
	QVERIFY(dynamixel.getParameter("Offset") != 0);
	QVERIFY(dynamixel.getParameter("CurrentVelocity") != 0);
	QVERIFY(dynamixel.getParameter("JointAngle") != 0);
	
	QCOMPARE(dynamixel.getInputValues().size(), 2);
	QCOMPARE(dynamixel.getOutputValues().size(), 1);
	
	QCOMPARE(dynamixel.getOutputValues().at(0), dynamixel.getParameter("JointAngle"));

	dynamixel.getParameter("MinAngle")->setValueFromString("100.0");
	QVERIFY(dynamixel.getParameter("MinAngle")->getValueAsString().compare("100") == 0);

	Dynamixel *copy = dynamic_cast<Dynamixel*>(dynamixel.createCopy());
	QVERIFY(copy != 0);
	QCOMPARE(copy->getInputValues().size(), 2);
	QCOMPARE(copy->getOutputValues().size(), 1);

	QVERIFY(copy->getParameter("MinAngle") != 0);
	QVERIFY(copy->getParameter("MaxAngle") != 0);
	QVERIFY(copy->getParameter("DesiredAngle") != 0);
	QVERIFY(copy->getParameter("DesiredTorque") != 0);
	QVERIFY(copy->getParameter("Offset") != 0);
	QVERIFY(copy->getParameter("CurrentVelocity") != 0);
	QVERIFY(copy->getParameter("JointAngle") != 0);
	QVERIFY(copy->getParameter("MinAngle")->getValueAsString().compare("100") == 0);
	

}
