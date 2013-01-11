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

#include "Test_ODESimJoints.h"
#include "Physics/ODE_HingeJoint.h"
#include "Physics/ODE_BoxBody.h"
#include "Physics/Physics.h"
#include "Physics/ODE_JointAdapter.h"
#include "Physics/ODE_Dynamixel.h"
#include "Physics/ODE_SimulationAlgorithm.h"
#include "Core/Core.h"

namespace nerd{

//Verena ?
void Test_ODESimJoints::testODEHingeJoint() {
	Core::resetCore();
	ODE_HingeJoint *j1 = new ODE_HingeJoint("Joint1");
	QCOMPARE(j1->getParameters().size(), 5);

	ODE_BoxBody *box1 = new ODE_BoxBody("Box1", 1, 1, 1);
	ODE_BoxBody *box2 = new ODE_BoxBody("Box2", 1, 1, 1);
	box1->setup();
	box2->setup();
	Physics::getPhysicsManager()->addSimObject(box1);
	Physics::getPhysicsManager()->addSimObject(box2);
	
	QVERIFY(j1->getParameter("Name")->getValueAsString().compare("Joint1") == 0);
	QVERIFY(j1->getParameter("AxisPoint1")->getValueAsString().compare("(0,0,0)") == 0);
	QVERIFY(j1->getParameter("AxisPoint2")->getValueAsString().compare("(0,0,0)") == 0);

	j1->getParameter("AxisPoint1")->setValueFromString("(1.0, 0.0, 0.0)");
	QVERIFY(j1->getParameter("AxisPoint1")->getValueAsString().compare("(1,0,0)") == 0);

	j1->getParameter("FirstBody")->setValueFromString("Box1");
	j1->getParameter("SecondBody")->setValueFromString("Box2");
	j1->setup();
		
	Physics::getPhysicsManager()->addSimObject(j1);

	ODE_HingeJoint *copy1 = dynamic_cast<ODE_HingeJoint*>(j1->createCopy());

	QCOMPARE(copy1->getParameters().size(), 5);
	QVERIFY(copy1->getParameter("Name")->getValueAsString().compare("Joint1") == 0);
	QVERIFY(copy1->getParameter("AxisPoint1")->getValueAsString().compare("(1,0,0)") == 0);
	QVERIFY(copy1->getParameter("AxisPoint2")->getValueAsString().compare("(0,0,0)") == 0);
	QVERIFY(copy1->getParameter("FirstBody")->getValueAsString().compare("Box1") == 0);
	QVERIFY(copy1->getParameter("SecondBody")->getValueAsString().compare("Box2") == 0);

}

void Test_ODESimJoints::testODEDynamixel() {
	Core::resetCore();
	PhysicsManager *pManager = Physics::getPhysicsManager();
	ODE_SimulationAlgorithm *sAlgo = new ODE_SimulationAlgorithm();
	pManager->setPhysicalSimulationAlgorithm(sAlgo);
	sAlgo->resetPhysics();
	
	//Joint is no SimJoint instance.
	ODE_JointAdapter *joint = new ODE_JointAdapter();
	QVERIFY(joint->createJoint() == 0);
	
	ODE_BoxBody *body1 = new ODE_BoxBody("Box1", 0.1, 0.1, 0.2);
	ODE_BoxBody *body2 = new ODE_BoxBody("Box2", 0.1, 0.1, 0.2);
	body2->getPositionValue()->set(0.0, 0.0, 0.25);
	body1->setup();
	body2->setup();
	QVERIFY(pManager->addSimObject(body1));
	QVERIFY(pManager->addSimObject(body2));

	ODE_Dynamixel dynamixel1("Joint1");
	QVERIFY(dynamixel1.getJoint() == 0);
	dynamixel1.getParameter("FirstBody")->setValueFromString("Box1");
	dynamixel1.getParameter("SecondBody")->setValueFromString("Box2");

	dynamixel1.setup();
	QVERIFY(dynamixel1.getJoint() == 0);

	dynamixel1.getParameter("AxisPoint1")->setValueFromString("(0.0, 0.0, 0.125");
	dynamixel1.getParameter("AxisPoint2")->setValueFromString("(1.0, 0.0, 0.125)");
	
	dynamixel1.setup();
	QVERIFY(dynamixel1.getJoint() != 0);
	dJointID dynamixel = dynamixel1.getJoint();

	QVERIFY(dJointGetType(dynamixel) == dJointTypeAMotor);
	
	QCOMPARE(dJointGetBody(dynamixel, 0), body1->getRigidBodyID());
	QCOMPARE(dJointGetBody(dynamixel, 1), body2->getRigidBodyID());


}

}
