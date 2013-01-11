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


#include "Test_ODECollisionHandler.h"
#include "ODE_CollisionHandlerAdapter.h"
#include "Physics/ODE_SimulationAlgorithm.h"
#include "Physics/PhysicsManager.h"
#include "Physics/ODE_BoxBody.h"
#include "Collision/CollisionManager.h"
#include "Core/Core.h"
#include "Physics/AccelSensor.h"
#include "Physics/Physics.h"

namespace nerd{

//TODO: check CollisionObject::areCollisionsReported() and the correct reaction in the CollisionHandler (if for two colliding collision objects, both have areCollisionsReported set to false, the collision is handled but the contact is not "reported" back to the CollisionManager, so that the contact will not be reported to CollisionRules.

//chris ?
void Test_ODECollisionHandler::testDisableCollision() {
	Core::resetCore();
	ODE_CollisionHandlerAdapter *handler = new ODE_CollisionHandlerAdapter();
	QCOMPARE(handler->getContacts().size(), 0);
	QCOMPARE(handler->getLookUpTable().size(), 0);
	QCOMPARE(handler->getAllowedCollisionPairs().size(), 0);

	SimBody *simObj = new SimBody("Object");
	CollisionObject *ob = new CollisionObject(BoxGeom(simObj, 1, 1, 1), simObj, false);
	ob->setHostBody(simObj);
	
	SimBody *simObj2 = new SimBody("Object2");
	CollisionObject *ob2 = new CollisionObject(BoxGeom(simObj2, 1, 1, 1), simObj, false);
	ob2->setHostBody(simObj2);

	SimBody *simObj3 = new SimBody("Object3");
	CollisionObject *ob3 = new CollisionObject(BoxGeom(simObj3, 1, 1, 1), simObj, false);
	ob3->setHostBody(simObj3);
	
	SimBody *simObj4 = new SimBody("Object4");
	CollisionObject *ob4 = new CollisionObject(BoxGeom(simObj4, 1, 1, 1), simObj, false);
	ob4->setHostBody(simObj4);

	handler->disableCollisions(ob, ob2, true);
	QCOMPARE(handler->getAllowedCollisionPairs().size(), 1);
	handler->disableCollisions(ob4, ob3, true);
	QCOMPARE(handler->getAllowedCollisionPairs().size(), 2);
	handler->disableCollisions(ob, ob2, false);
	QCOMPARE(handler->getAllowedCollisionPairs().size(), 1);
	handler->disableCollisions(ob2, ob4, true);
	QCOMPARE(handler->getAllowedCollisionPairs().size(), 1);
	QCOMPARE(handler->getAllowedCollisionPairs().value(ob4).size(), 2);
}


//chris ?
void Test_ODECollisionHandler::testUpdateAndCreate() {
	Core::resetCore();
	ODE_CollisionHandlerAdapter *handler = new ODE_CollisionHandlerAdapter();
	ODE_SimulationAlgorithm *algorithm = new ODE_SimulationAlgorithm();
	
	Physics::getPhysicsManager()->setPhysicalSimulationAlgorithm(algorithm);
	Physics::getCollisionManager()->setCollisionHandler(handler);

	QCOMPARE(handler->getContacts().size(), 0);
	QCOMPARE(handler->getAllowedCollisionPairs().size(), 0);
	QCOMPARE(handler->getLookUpTable().size(), 0);
	QCOMPARE(handler->getMaxContactPoints(), 0);

	Core::getInstance()->init();

	QCOMPARE(handler->getMaxContactPoints(), 0);
	handler->updateCollisionHandler(Physics::getCollisionManager());
	QCOMPARE(handler->getMaxContactPoints(), 100);

	algorithm->getParameter("ODE/MaxContactPoints")->setValueFromString("1111");
	handler->updateCollisionHandler(Physics::getCollisionManager());
	QCOMPARE(handler->getMaxContactPoints(), 1111);
	
	ODE_BoxBody *simObj = new ODE_BoxBody("Box", 1.0, 1.0, 2.0);
	simObj->getParameter("Mass")->setValueFromString("0.5");
	Physics::getPhysicsManager()->addSimObject(simObj);
	Physics::getPhysicsManager()->resetSimulation();
	QCOMPARE(simObj->getCollisionObjects().size(), 1);
	QVERIFY(simObj->getCollisionObjects().at(0)->getNativeCollisionObject() != 0);

	handler->updateCollisionHandler(Physics::getCollisionManager());
	QCOMPARE(handler->getLookUpTable().size(), 1);

	ODE_BoxBody *simObj2 = new ODE_BoxBody("Box2", 2.0, 1.0, 2.0);
	simObj2->getParameter("Mass")->setValueFromString("0.5");
	Physics::getPhysicsManager()->addSimObject(simObj2);
	Physics::getPhysicsManager()->resetSimulation();
	
	handler->updateCollisionHandler(Physics::getCollisionManager());
	QCOMPARE(handler->getLookUpTable().size(), 2);
	
	AccelSensor *sensor = new AccelSensor("Sensor", 2);
	sensor->getParameter("ReferenceBody")->setValueFromString("Box2");
	sensor->setup();
	Physics::getPhysicsManager()->addSimObject(sensor);
	QCOMPARE(simObj2->getCollisionObjects().size(), 2);
	QVERIFY(simObj2->getCollisionObjects().at(1)->getNativeCollisionObject() == 0);

	Physics::getPhysicsManager()->resetSimulation();
	
	QVERIFY(simObj2->getCollisionObjects().at(1)->getNativeCollisionObject() != 0);
	handler->updateCollisionHandler(Physics::getCollisionManager());
	QCOMPARE(handler->getLookUpTable().size(), 3);
}



}
