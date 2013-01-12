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


#include "TestCollisionRule.h"
#include "Core/Core.h"
#include "Collision/CollisionRule.h"
#include "Physics/SimBody.h"
#include "Collision/EventCollisionRule.h"
#include "Physics/SphereGeom.h"
#include "Collision/CollisionObject.h"
#include "Event/EventListenerAdapter.h"

namespace nerd{


//chris
void TestCollisionRule::testConstructor(){
	Core::resetCore();
	
	EventCollisionRule *cr = new EventCollisionRule("Rule1");
	SphereGeom geom(0);
	
	QVERIFY(cr->getName().compare(QString("Rule1"))==0);
	
	CollisionObject *co = new CollisionObject(geom);
	
	QList<CollisionObject*> collisionObjects;
	collisionObjects.push_back(co);
	CollisionObject *co2 = new CollisionObject(geom);
	collisionObjects.push_back(co2);


	CollisionObject *co3 = new CollisionObject(geom);
	QList<CollisionObject*> collisionObjects2;
	collisionObjects2.push_back(co3);

	cr->set(collisionObjects, collisionObjects2, "TestEvent");
	QVERIFY(Core::getInstance()->getEventManager()->getEvent("TestEvent", false) 
							!= NULL);
	QCOMPARE(cr->getSourceGroup().size(), 2);
	QCOMPARE(cr->getTargetGroup().size(), 1);
	QCOMPARE(cr->getEvents().size(), 1);
	
	delete cr;
	delete co;
	delete co2;
	delete co3;
	

}


//chris
void TestCollisionRule::testAddAndRemoveBodies() {
	Core::resetCore();
	
	EventCollisionRule *cr = new EventCollisionRule("Rule1");
	SphereGeom geom(0);
	
	
	CollisionObject *co = new CollisionObject(geom);
	QList<CollisionObject*> sourceGroup;
	sourceGroup.push_back(co);
	CollisionObject *obj1 = co;
	CollisionObject *co2 = new CollisionObject(geom);
	sourceGroup.push_back(co2);
	CollisionObject *obj2 = co2;
	
	CollisionObject *co3 = new CollisionObject(geom);
	QList<CollisionObject*> targetGroup;
	targetGroup.push_back(co3);
	CollisionObject *co4 = new CollisionObject(geom);
	targetGroup.push_back(co4);
	CollisionObject *co5 = new CollisionObject(geom);
	CollisionObject *obj5 = co5;


	cr->set(sourceGroup, targetGroup, "TestEvent");
	QCOMPARE(cr->getSourceGroup().size(), 2);
	QCOMPARE(cr->getTargetGroup().size(), 2);
	cr->addToSourceGroup(obj5);
	QCOMPARE(cr->getSourceGroup().size(), 3);
	QCOMPARE(cr->getTargetGroup().size(), 2);
	cr->addToSourceGroup(obj2);
	QCOMPARE(cr->getSourceGroup().size(), 3);
	QCOMPARE(cr->getTargetGroup().size(), 2);

	cr->addToTargetGroup(obj2);
	QCOMPARE(cr->getSourceGroup().size(), 3);
	QCOMPARE(cr->getTargetGroup().size(), 3);



	QCOMPARE(cr->getSourceGroup().size(), 3);
	QCOMPARE(cr->getTargetGroup().size(), 3);
	
	cr->removeFromSourceGroup(obj2);
	QCOMPARE(cr->getSourceGroup().size(), 2);
	QCOMPARE(cr->getTargetGroup().size(), 3);
	

	cr->removeFromTargetGroup(obj1);
	QCOMPARE(cr->getSourceGroup().size(), 2);
	QCOMPARE(cr->getTargetGroup().size(), 3);
	
	delete cr;
	delete co;
	delete co2;
	delete co3;
	delete co4;
	delete co5;
	

}

//chris
void TestCollisionRule::testAddAndRemoveEvents() {
	Core::resetCore();
	
	EventCollisionRule *cr = new EventCollisionRule("Rule1");
	SphereGeom geom(0);
	
	CollisionObject *co = new CollisionObject(geom);
	
	QList<CollisionObject*> sourceGroup;
	sourceGroup.push_back(co);
	CollisionObject *co2 = new CollisionObject(geom);
	sourceGroup.push_back(co2);
	
	CollisionObject *co3 = new CollisionObject(geom);
	QList<CollisionObject*> targetGroup;
	targetGroup.push_back(co3);
	CollisionObject *co4 = new CollisionObject(geom);
	targetGroup.push_back(co4);

	cr->set(sourceGroup, targetGroup, "TestEvent1");
	QCOMPARE(cr->getEvents().size(), 1);
	cr->addEvent("TestEvent1");
	QCOMPARE(cr->getEvents().size(), 1);
	cr->addEvent("TestEvent2");
	QCOMPARE(cr->getEvents().size(), 2);

	cr->replaceEvent("TestEvent1", "NewTestEvent1");
	QCOMPARE(cr->getEvents().size(), 2);
	Event *event = Core::getInstance()->getEventManager()
							->getEvent("NewTestEvent1", false);
	QVERIFY(event != 0);
	QVERIFY(cr->getEvents().contains(event));	

	event = Core::getInstance()->getEventManager()->getEvent("TestEvent3", true);
	cr->addEvent(event);
	QCOMPARE(cr->getEvents().size(), 3);
	
	//there is no event with that name (missing /Collision/ prefix),
/*	event = Core::getInstance()->getEventManager()->getEvent("NewTestEvent1", false);
	QVERIFY(event == 0);*/
	
	event = Core::getInstance()->getEventManager()
							->getEvent("NewTestEvent1", true);
							
	//can not be added because this event already exists in this rule.
	cr->addEvent(event);
	QCOMPARE(cr->getEvents().size(), 3);
	
	cr->addEvent("TestEvent3");
	QCOMPARE(cr->getEvents().size(), 3);
	
	cr->replaceEvent("NewTestEvent1", "TestEvent3");
	QCOMPARE(cr->getEvents().size(), 2);

	event = Core::getInstance()->getEventManager()->getEvent("TestEvent3", true);
	
	Event *newEvent = Core::getInstance()->getEventManager()->getEvent("VeryNewTestEvent", true);
	cr->replaceEvent(event, newEvent);
	QVERIFY(cr->getEvents().contains(newEvent) == true);
	QVERIFY(cr->getEvents().contains(event) == false);
	QCOMPARE(cr->getEvents().size(), 2);

	cr->removeEvent(newEvent);
	QCOMPARE(cr->getEvents().size(), 1);
	QCOMPARE(cr->getEvents().at(0)->getNames().size(), 1);
	QVERIFY(cr->getEvents().at(0)->getNames().at(0).compare("TestEvent2") == 0);
	
	delete cr;
	delete co;
	delete co2;
	delete co3;
	delete co4;
	
}


//chris
void TestCollisionRule::testSetAndGet() {
	Core::resetCore();

	EventCollisionRule *cr = new EventCollisionRule("Rule1");
	SphereGeom geom(0);
	
	QVERIFY(cr->getName().compare("Rule1") == 0);

	CollisionObject *co = new CollisionObject(geom);
	QList<CollisionObject*> sourceGroup;
	sourceGroup.push_back(co);
	CollisionObject *obj1 = co;
	CollisionObject *co2 = new CollisionObject(geom);
	sourceGroup.push_back(co2);
	CollisionObject *obj2 = co2;

	CollisionObject *co3 = new CollisionObject(geom);
	QList<CollisionObject*> targetGroup;
	targetGroup.push_back(co3);
	CollisionObject *obj3 = co3;
	CollisionObject *co4 = new CollisionObject(geom);
	targetGroup.push_back(co4);
	CollisionObject *obj4 = co4;
	CollisionObject *co5 = new CollisionObject(geom);
	CollisionObject *obj5 = co5;

	cr->set(sourceGroup, targetGroup, "TestEvent");
	QCOMPARE(cr->getSourceGroup().size(), 2);	
	QCOMPARE(cr->getSourceGroup().at(1), obj2);
	QVERIFY(cr->getSourceGroup().at(1) != obj4);

	QList<CollisionObject*> newGroup;
	newGroup.push_back(obj5);
	newGroup.push_back(obj1);
	newGroup.push_back(obj3);
	newGroup.push_back(obj2);
	
	cr->setSourceGroup(newGroup);
	QCOMPARE(cr->getSourceGroup().size(), 4);
	QCOMPARE(cr->getSourceGroup().at(1), obj1);
	
	cr->setTargetGroup(sourceGroup);
	QCOMPARE(cr->getTargetGroup().size(), 2);
	QCOMPARE(cr->getTargetGroup().at(1), obj2);
	

	QCOMPARE(cr->getEvents().size(), 1);
	QList<Event*> newEventList;
	Event *event1 = Core::getInstance()->getEventManager()->getEvent("NewEvent1");
	Event *event2 = Core::getInstance()->getEventManager()->getEvent("NewEvent2");
	Event *event3 = Core::getInstance()->getEventManager()->getEvent("NewEvent3");
	newEventList.push_back(event1);
	newEventList.push_back(event2);
	newEventList.push_back(event3);
	cr->setEvents(newEventList);
	QCOMPARE(cr->getEvents().size(), 3);
	QCOMPARE(cr->getEvents().at(1), event2);
	
	delete cr;
	delete co;
	delete co2;
	delete co3;
	delete co4;
	delete co5;
	

}


//chris
void TestCollisionRule::testHandleContact(){
	Core::resetCore();
	
	EventCollisionRule *cr = new EventCollisionRule("Rule1");
	SphereGeom geom(0);
	
	QVERIFY(!cr->isNegated());
	
	CollisionObject *co = new CollisionObject(geom);
	QList<CollisionObject*> sourceGroup;
	sourceGroup.push_back(co);
	CollisionObject *obj1 = co;
	CollisionObject *co2 = new CollisionObject(geom);
	sourceGroup.push_back(co2);
	CollisionObject *obj2 = co2;

	CollisionObject *co3 = new CollisionObject(geom);
	QList<CollisionObject*> targetGroup;
	targetGroup.push_back(co3);
	CollisionObject *obj3 = co3;
	CollisionObject *co4 = new CollisionObject(geom);
	targetGroup.push_back(co4);
	CollisionObject *co5 = new CollisionObject(geom);
	CollisionObject *obj5 = co5;

	cr->set(sourceGroup, targetGroup, "TestEvent");
	
	Event *event = Core::getInstance()->getEventManager()->getEvent(
						"TestEvent", false);

	QVERIFY(event != 0);
	
	EventListenerAdapter listener("Listener1");
	event->addEventListener(&listener);
	
	QVERIFY(listener.mLastTriggeredEvent == 0);
	QVERIFY(listener.mCountEventOccured == 0);

	//Contact triggers the CollisionRule
	Contact one(obj1, obj3);
	QVERIFY(cr->handleContact(one) == true);
	
	QVERIFY(listener.mCountEventOccured == 1);
	QVERIFY(listener.mLastTriggeredEvent == event);
	
	listener.mLastTriggeredEvent = 0;
	listener.mCountEventOccured = 0;
	
	//the CollisionObject objects are elemts of the same group in the collisionRule
	//therefore the rule is not triggered.
	Contact two(obj1, obj2);
	QVERIFY(cr->handleContact(two) == false);
	
	QVERIFY(listener.mCountEventOccured == 0);
	
	//one of the bodies is not an element of one of the groups.
	Contact three(obj3, obj5);
	QVERIFY(cr->handleContact(three) == false);
	
	QVERIFY(listener.mCountEventOccured == 0);
	
	Contact five(obj3, obj1); //like contact one, but the other way round.
	QVERIFY(cr->handleContact(five) == true);
	
	QVERIFY(listener.mCountEventOccured == 1);
	QVERIFY(listener.mLastTriggeredEvent == event);

	cr->negateRule(true);
	QVERIFY(cr->isNegated());
	QVERIFY(cr->handleContact(five) == false);
	QVERIFY(listener.mCountEventOccured == 1);

	QVERIFY(cr->handleContact(two) == true);
	QVERIFY(listener.mCountEventOccured == 2);
	
	
	delete cr;
	delete co;
	delete co2;
	delete co3;
	delete co4;
	delete co5;


}

}
