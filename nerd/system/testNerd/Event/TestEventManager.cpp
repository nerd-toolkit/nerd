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



#include "TestEventManager.h"
#include "Event/EventManager.h"
#include "Core/Core.h"
#include "EventAdapter.h"
#include "EventListenerAdapter.h"
#include "EventManagerAdapter.h"

namespace nerd{


//chris
void TestEventManager::testCreateAndGetEvent() {
	Core::resetCore();
	
	EventManager *emanager = new EventManager();
	
	Event *first = emanager->createEvent("Event1");
	QVERIFY(first != 0);
	
	QCOMPARE(emanager->getEvents().size(), 1);
	QCOMPARE(emanager->getEvents().at(0), emanager->getEvent("Event1"));
	QVERIFY(emanager->getEvent("Event1")->getDescription().compare("") == 0);

	Event *empty = emanager->createEvent("Event1");
	QVERIFY(empty == 0);
	QCOMPARE(emanager->getEvents().size(),1 );
	
	Event *second = emanager->createEvent("Event2", "second event");
	QVERIFY(second != 0);
	QCOMPARE(emanager->getEvents().size(), 2);
	QVERIFY(emanager->getEvent("Event2")->getDescription().compare("second event") == 0);	
	
	second->addName("EventTwo");
	QCOMPARE(second->getName(), emanager->getEvents().at(1)->getName());

	Event *third = emanager->getEvent("Event3", true);
	QVERIFY(third != 0);	
	QCOMPARE(emanager->getEvents().at(2), third);

	Event *empty2 = emanager->getEvent("Empty", false);
	QVERIFY(empty2 == 0);
	QCOMPARE(emanager->getEvents().size(), 3);

	empty2 = emanager->getEvent("Empty"); //second default parameter is false.
	QVERIFY(empty2 == 0);
	QCOMPARE(emanager->getEvents().size(), 3);

	Event *lastEvent1 = emanager->getEvent("Last1E", true);
	QCOMPARE(emanager->getEvents().size(), 4);

	Event *lastEvent2 = emanager->getEvent("Last2", true); 
	QVERIFY(lastEvent2 != 0);
	QCOMPARE(emanager->getEvents().size(), 5);

	QCOMPARE(emanager->getEventsMatchingPattern("Event.*").size(), 3);
	QVERIFY(emanager->getEventsMatchingPattern("Event.*").contains(first));
	QVERIFY(emanager->getEventsMatchingPattern("Event.*").contains(second));
	QVERIFY(emanager->getEventsMatchingPattern("Event.*").contains(third));

	QCOMPARE(emanager->getEventsMatchingPattern(".*1.*").size(), 2);
	QVERIFY(emanager->getEventsMatchingPattern(".*1.*").contains(lastEvent1));

	QCOMPARE(emanager->getNamesMatchingPattern(".*1.*").size(), 2);
	QVERIFY(emanager->getNamesMatchingPattern(".*1.*").at(1).compare(QString("Last1E")) == 0);
	
	delete emanager;
	
	
}

void TestEventManager::testNotificationStack() {
}


//chris
void TestEventManager::testRegisterAndNotify() {
	Core::resetCore();
	
	EventManager *eManager = new EventManager();
	
	Event *event1 = eManager->getEvent("Event1", true);
	Event *event2 = eManager->getEvent("Event2", true);

	EventListenerAdapter *listener1 = new EventListenerAdapter("Listener1");
	eManager->registerForEvent("Event1", listener1);
	QCOMPARE(event1->getEventListeners().size(), 1);
	QCOMPARE(listener1->mCountEventOccured, 0);

	EventListenerAdapter *listener2 = new EventListenerAdapter("Listener2");
	eManager->registerForEvent("Event2", listener2);

	EventListenerAdapter *listener = new EventListenerAdapter("Listener");
	eManager->registerForEvent("Event1", listener);
	eManager->registerForEvent("Event2", listener);
	QCOMPARE(event1->getEventListeners().size(), 2);
	QCOMPARE(event2->getEventListeners().size(), 2);
	QCOMPARE(listener->mCountEventOccured, 0);

	event1->trigger();
	QCOMPARE(listener1->mCountEventOccured, 1);
	QCOMPARE(listener2->mCountEventOccured, 0);
	QCOMPARE(listener->mCountEventOccured, 1);

	event2->trigger();
	QCOMPARE(listener1->mCountEventOccured, 1);
	QCOMPARE(listener2->mCountEventOccured, 1);
	QCOMPARE(listener->mCountEventOccured, 2);
	
	eManager->deregisterFromEvent("Event1", listener);
	QCOMPARE(event1->getEventListeners().size(), 1);
	
	event1->trigger();
	QCOMPARE(listener1->mCountEventOccured, 2);
	QCOMPARE(listener2->mCountEventOccured, 1);
	QCOMPARE(listener->mCountEventOccured, 2);

	delete eManager;
	delete listener1;
	delete listener2;
	delete listener;
	
}


//chris
void TestEventManager::testDestruction() {
	Core::resetCore();

	EventManagerAdapter *eManager = new EventManagerAdapter();
	
	bool destroyFlag = false;
	EventAdapter *event1 = new EventAdapter("Event1", &destroyFlag);
	QCOMPARE(event1->mTriggerCount, 0);
	QCOMPARE(destroyFlag, false);
	
	eManager->addEvent(event1);
	
	delete eManager;
	QCOMPARE(destroyFlag, true);

}

}
