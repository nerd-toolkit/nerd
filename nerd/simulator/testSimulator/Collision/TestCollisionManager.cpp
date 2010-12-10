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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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


#include "TestCollisionManager.h"
#include "Collision/CollisionManager.h"
#include "Collision/CollisionRule.h"
#include "CollisionRuleAdapter.h"
#include "Core/Core.h"
#include "CollisionHandlerAdapter.h"
#include "CollisionObjectAdapter.h"
#include "Physics/BoxBody.h"
#include "Physics/BoxGeom.h"
#include "CollisionRuleAdapter.h"
#include "Collision/EventCollisionRule.h"
#include "Event/EventListenerAdapter.h"


namespace nerd{

void TestCollisionManager::testCreateAndDelete() {
	Core::resetCore();
	CollisionManager *cManager = new CollisionManager();

	QVERIFY(cManager->getMaterialProperties() != 0);

	bool destroy1 = false;
	CollisionRuleAdapter *rule1 = new CollisionRuleAdapter("rule1", &destroy1);	
	bool destroy2 = false;
	CollisionRuleAdapter *rule2 = new CollisionRuleAdapter("rule2", &destroy2);	
	bool destroy3 = false;
	CollisionRuleAdapter *rule3 = new CollisionRuleAdapter("rule3", &destroy3);	
	bool destroy4 = false;
	CollisionRuleAdapter *rule4 = new CollisionRuleAdapter("rule4", &destroy4);	

	bool destroyHandler = false;
	CollisionHandlerAdapter *handler = new CollisionHandlerAdapter(cManager, &destroyHandler);
	cManager->setCollisionHandler(handler);

	QCOMPARE(handler, cManager->getCollisionHandler());
	QCOMPARE(cManager->getCollisionObjectsIncludedInRule().size(), 0);
	QCOMPARE(cManager->getCollisionObjectsIncludedInRule().size(), 0);
	QCOMPARE(cManager->getCollisions().size(), 0);
	
	BoxBody owner1("Body1", 1, 1, 1);
	BoxBody owner2("Body2", 1, 1, 1);
	BoxGeom *geom1 = dynamic_cast<BoxGeom*>(owner1.getGeometries().at(0));
	BoxGeom *geom2 = dynamic_cast<BoxGeom*>(owner2.getGeometries().at(0));

	bool destroyCo1 = false;
	CollisionObjectAdapter *cObject1 = new CollisionObjectAdapter(*geom1, &owner1, &destroyCo1);
	bool destroyCo2 = false;
	CollisionObjectAdapter *cObject2 = new CollisionObjectAdapter(*geom2, &owner2, &destroyCo2);

	cManager->disableCollisions(cObject1, cObject2, true);

	cManager->addCollisionRule(rule1);
	cManager->addCollisionRule(rule2);
	cManager->addCollisionRule(rule3);
	cManager->addCollisionRule(rule4);


	QCOMPARE(handler, cManager->getCollisionHandler());
	QCOMPARE(cManager->getCollisionObjectsIncludedInRule().size(), 0);
	QCOMPARE(cManager->getCollisionRules().size(), 4);
	QCOMPARE(cManager->getCollisions().size(), 0);

	delete cManager;
	
	QVERIFY(destroy1 == true);
	QVERIFY(destroy2 == true);
	QVERIFY(destroy3 == true);
	QVERIFY(destroy4 == true);
	
	QVERIFY(destroyHandler == true);
	
	QVERIFY(destroyCo1 == false);
	QVERIFY(destroyCo2 == false);
	
	delete cObject1;
	delete cObject2;


}

void TestCollisionManager::testManipulateCollisionRules() {
	Core::resetCore();
	CollisionManager *cManager = new CollisionManager();

	CollisionRuleAdapter *rule1 = new CollisionRuleAdapter("Rule1", 0);
	CollisionRuleAdapter *rule2 = new CollisionRuleAdapter("Rule2", 0);
	CollisionRuleAdapter *rule3 = new CollisionRuleAdapter("Rule3", 0);
	CollisionRuleAdapter *rule4 = new CollisionRuleAdapter("Rule4", 0);
	
	QCOMPARE(cManager->getCollisionRules().size(), 0);
	cManager->addCollisionRule(rule1);
	cManager->addCollisionRule(rule2);
	cManager->addCollisionRule(rule3);
	QCOMPARE(cManager->getCollisionRules().size(), 3);

	cManager->removeCollisionRule(rule2);
	QCOMPARE(cManager->getCollisionRules().size(), 2);
	QVERIFY(!cManager->getCollisionRules().contains(rule2));	

	cManager->addCollisionRule(rule4);
	QCOMPARE(cManager->getCollisionRules().size(), 3);
	cManager->removeCollisionRule("Rule4");
	QCOMPARE(cManager->getCollisionRules().size(), 2);	
	QVERIFY(!cManager->getCollisionRules().contains(rule4));	

	delete cManager;

}

void TestCollisionManager::testDisableCollision() {
	Core::resetCore();
	
	CollisionManager *cManager = new CollisionManager();
	CollisionHandlerAdapter *handler = new CollisionHandlerAdapter(cManager, 0);
	cManager->setCollisionHandler(handler);
	QVERIFY(cManager->getCollisionHandler() == handler);

	BoxBody *owner1 = new BoxBody("Body1", 1, 1, 1);
	BoxBody *owner2 = new BoxBody("Body2", 1, 1, 1);
	BoxBody *owner3 = new BoxBody("Body3", 1, 1, 1);
	BoxBody *owner4 = new BoxBody("Body4", 1, 1, 1);
	BoxBody *owner5 = new BoxBody("Body5", 1, 1, 1);
	BoxGeom *geom1 = dynamic_cast<BoxGeom*>(owner1->getGeometries().at(0));
	BoxGeom *geom2 = dynamic_cast<BoxGeom*>(owner2->getGeometries().at(0));
	BoxGeom *geom3 = dynamic_cast<BoxGeom*>(owner3->getGeometries().at(0));
	BoxGeom *geom4 = dynamic_cast<BoxGeom*>(owner4->getGeometries().at(0));
	BoxGeom *geom5 = dynamic_cast<BoxGeom*>(owner5->getGeometries().at(0));
	
	CollisionObject *c1 = new CollisionObject(*geom1, owner1);
	CollisionObject *c2 = new CollisionObject(*geom2, owner2);
	CollisionObject *c3 = new CollisionObject(*geom3, owner3);
	CollisionObject *c4 = new CollisionObject(*geom4, owner4);
	CollisionObject *c5 = new CollisionObject(*geom5, owner5);

	QCOMPARE(handler->getContacts().size(), 0);
	cManager->disableCollisions(c1, c2, true);

	QCOMPARE(handler->getContacts().size(), 1);

	cManager->disableCollisions(c3, c4, true);
	QCOMPARE(handler->getContacts().size(), 2);

	cManager->disableCollisions(c1, c4, true);
	QCOMPARE(handler->getContacts().size(), 3);

		// test update. On calling update all previously set allowed collision need to be "recreated"
	handler->clearHandler();
	QCOMPARE(handler->getContacts().size(), 0);
	cManager->updateCollisionModel();
	QCOMPARE(handler->getContacts().size(), 3);
	QCOMPARE(c1, handler->getContacts().at(0).getFirstCollisionObject());
	QCOMPARE(c1, handler->getContacts().at(2).getFirstCollisionObject());
	QCOMPARE(c4, handler->getContacts().at(2).getSecondCollisionObject());

	cManager->disableCollisions(c1, c4, false);
	QCOMPARE(handler->getContacts().size(), 2);

	handler->clearHandler();
	QCOMPARE(handler->getContacts().size(), 0);
	cManager->updateCollisionModel();
	QCOMPARE(handler->getContacts().size(), 2);
	QCOMPARE(c3, handler->getContacts().at(1).getFirstCollisionObject());
	QCOMPARE(c4, handler->getContacts().at(1).getSecondCollisionObject());

	cManager->disableCollisions(c3, c4, false);
	cManager->disableCollisions(c1, c2, false);
	QCOMPARE(handler->getContacts().size(), 0);

	QList<CollisionObject*> groupOne;
	QList<CollisionObject*> groupTwo;
	
	groupOne.push_back(c1);
	groupOne.push_back(c3);
	groupOne.push_back(c4);
	groupTwo.push_back(c2);

	cManager->disableCollisions(groupOne, groupTwo, true);
	QCOMPARE(handler->getContacts().size(), 3);
	
	handler->clearHandler();
	cManager->updateCollisionModel();
	groupTwo.push_back(c5);
	cManager->disableCollisions(groupOne, groupTwo, true);
	QCOMPARE(handler->getContacts().size(), 6);
	
	QCOMPARE(c1->areCollisionsDisabled(), false);
	cManager->disableCollisions(c1, true);
	QCOMPARE(c1->areCollisionsDisabled(), true);
 
	QCOMPARE(c2->areCollisionsDisabled(), false);
	c2->disableCollisions(true);
	QCOMPARE(c2->areCollisionsDisabled(), true);
}

void TestCollisionManager::testExecute() {
	Core::resetCore();
	CollisionManager *cManager = new CollisionManager();
	CollisionHandlerAdapter *handler = new CollisionHandlerAdapter(cManager, 0);
	cManager->setCollisionHandler(handler);
	BoxBody *owner1 = new BoxBody("Body1", 1, 1, 1);
	BoxBody *owner2 = new BoxBody("Body2", 1, 1, 1);
	BoxGeom *geom1 = dynamic_cast<BoxGeom*>(owner1->getGeometries().at(0));
	BoxGeom *geom2 = dynamic_cast<BoxGeom*>(owner2->getGeometries().at(0));
	BoxBody *owner3 = new BoxBody("Body3", 1, 1, 1);
	BoxBody *owner4 = new BoxBody("Body4", 1, 1, 1);
	BoxGeom *geom3 = dynamic_cast<BoxGeom*>(owner3->getGeometries().at(0));
	BoxGeom *geom4 = dynamic_cast<BoxGeom*>(owner4->getGeometries().at(0));
	
	CollisionObject *c1 = new CollisionObject(*geom1, owner1);
	CollisionObject *c2 = new CollisionObject(*geom2, owner2);
	CollisionObject *c3 = new CollisionObject(*geom3, owner3);
	CollisionObject *c4 = new CollisionObject(*geom4, owner4);

	cManager->disableCollisions(c1, c2, true);
	cManager->disableCollisions(c3, c4, true);

	QList<CollisionObject*> firstGroup;
	firstGroup.push_back(c1);
	QList<CollisionObject*> secondGroup;
	secondGroup.push_back(c2);
	QList<CollisionObject*> thirdGroup;
	thirdGroup.push_back(c3);
	thirdGroup.push_back(c4);
	
	EventCollisionRule *cR1 = new EventCollisionRule("Rule1");
	EventCollisionRule *cR2 = new EventCollisionRule("Rule3");
	
	cR1->set(firstGroup, secondGroup, "/Collision/TestEvent1");
	cR2->set(firstGroup, thirdGroup, "/Collision/TestEvent2");

	
	Event *event1 = Core::getInstance()->getEventManager()->getEvent(
						"/Collision/TestEvent1", false);

	QVERIFY(event1 != 0);
	EventListenerAdapter listener1("Listener1");
	event1->addEventListener(&listener1);
	Event *event2 = Core::getInstance()->getEventManager()->getEvent(
						"/Collision/TestEvent2", false);
	EventListenerAdapter listener2("Listener2");
	event2->addEventListener(&listener2);

	cManager->addCollisionRule(cR1);
	cManager->addCollisionRule(cR2);

	QCOMPARE(cManager->getCollisionObjectsIncludedInRule().size(), 0);
	
	QCOMPARE(listener1.mCountEventOccured, 0);
	QCOMPARE(listener2.mCountEventOccured, 0);

	//CollisionHandlerAdapter returns the List of allowed collisions on calling getContacts().
	// updateCollisionRules therefore reports all those CollisionObject-Pairs as collisions.
	cManager->updateCollisionRules();
	QCOMPARE(listener1.mCountEventOccured ,1);
	QCOMPARE(listener2.mCountEventOccured, 0);
	
	QCOMPARE(handler->getContacts().size(), cManager->getCollisions().size());
	for(int i = 0; i < handler->getContacts().size(); i++) {
		QCOMPARE(&handler->getContacts().at(i), &cManager->getCollisions().at(i));
	}

	cManager->disableCollisions(c1, c2, false);

	cManager->updateCollisionRules();
	QCOMPARE(listener1.mCountEventOccured ,1);
	QCOMPARE(listener2.mCountEventOccured, 0);

	cManager->disableCollisions(c1, c3, true);

	cManager->updateCollisionRules();
	QCOMPARE(listener1.mCountEventOccured ,1);
	QCOMPARE(listener2.mCountEventOccured, 1);

	
	QCOMPARE(cManager->getCollisionObjectsIncludedInRule().size(), 0);
	cManager->updateCollisionModel();
	QCOMPARE(cManager->getCollisionObjectsIncludedInRule().size(), 4);
	

}

}
