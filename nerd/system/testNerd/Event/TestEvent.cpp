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



#include "TestEvent.h"
#include "Event/Event.h"
#include "EventListenerAdapter.h"
#include "Core/Core.h"
#include "EventListenerAdapter.h"
#include "EventAdapter.h" 

namespace nerd{


//chris
void TestEvent::testCreateEvent() {
	Core::resetCore();
	
	Event *firstEvent = new Event("Event1");	
	Event *secondEvent = new Event("Event2", "Second Event for testing correct creation.");
	
	QCOMPARE(firstEvent->getUpstreamEvents().size(), 0);
	QCOMPARE(firstEvent->getEventListeners().size(), 0);
	QCOMPARE(firstEvent->getNames().size(), 1);
	QVERIFY(firstEvent->getNames().at(0).compare("Event1") == 0);	
	QVERIFY(firstEvent->getName().compare("Event [Event1]") == 0);
	QVERIFY(firstEvent->getDescription().isEmpty());
	QVERIFY(secondEvent->getDescription().compare("Second Event for testing correct creation.") 
							== 0);	
							
	delete firstEvent;
	delete secondEvent;
	

}


//chris
void TestEvent::testSetterAndGetter(){
	Core::resetCore();
	
	Event *firstEvent = new Event("First");
	QCOMPARE(firstEvent->getNames().size(), 1);
	QVERIFY(firstEvent->getNames().at(0) == "First");	
	QVERIFY(firstEvent->getName() == "Event [First]");
	QVERIFY(firstEvent->getDescription().isEmpty());
	
	//add an alias name
	QVERIFY(firstEvent->addName("Event1"));
	QCOMPARE(firstEvent->getNames().size(), 2);
	QVERIFY(firstEvent->getNames().at(0) == "First");	
	QVERIFY(firstEvent->getNames().at(1) == "Event1");
	QVERIFY(firstEvent->getName() == "Event [First, Event1]");
	QVERIFY(firstEvent->getDescription().isEmpty());

	//add the same alias name again (fails)
	QVERIFY(firstEvent->addName("Event1") == false);
	QCOMPARE(firstEvent->getNames().size(), 2);
	QVERIFY(firstEvent->getNames().at(0) == "First");	
	QVERIFY(firstEvent->getNames().at(1) == "Event1");
	QVERIFY(firstEvent->getName() == "Event [First, Event1]");

	//remove a non existing name (fails)
	QVERIFY(firstEvent->removeName("NotThere") == false);
	QCOMPARE(firstEvent->getNames().size(), 2);
	QVERIFY(firstEvent->getNames().at(0) == "First");	
	QVERIFY(firstEvent->getNames().at(1) == "Event1");
	QVERIFY(firstEvent->getName() == "Event [First, Event1]");

	//remove one of the names
	QVERIFY(firstEvent->removeName("First") == true);
	QCOMPARE(firstEvent->getNames().size(), 1);
	QVERIFY(firstEvent->getNames().at(0) == "Event1");
	QVERIFY(firstEvent->getName() == "Event [Event1]");

	//remove the last name (fails, there is always at least one name)
	QVERIFY(firstEvent->removeName("Event1") == false);
	QCOMPARE(firstEvent->getNames().size(), 1);
	QVERIFY(firstEvent->getNames().at(0) == "Event1");
	QVERIFY(firstEvent->getName() == "Event [Event1]");
	
	firstEvent->setDescription("First Event");
	QVERIFY(firstEvent->getDescription().compare("First Event") == 0);
	
	delete firstEvent;
	

}


//verena
void TestEvent::testListenerNotification() {
	Core::resetCore();
	
	EventAdapter *first = new EventAdapter("Event1");
	EventAdapter *second = new EventAdapter("Event2");
	
	EventListenerAdapter *listener = new EventListenerAdapter("Upstream Test");
	first->addEventListener(listener);
	second->addEventListener(listener);
	QCOMPARE(listener->mCountEventOccured, 0);

	first->trigger();
	QCOMPARE(listener->mCountEventOccured, 1);
	QCOMPARE(listener->mLastTriggeredEvent, first);
	QCOMPARE(first->mTriggerCount, 1);
	QCOMPARE(second->mTriggerCount, 0);

	first->addUpstreamEvent(second);
	QCOMPARE(first->getUpstreamEvents().size(), 1);
	
	first->trigger();
	
	QCOMPARE(listener->mCountEventOccured, 3);
	QCOMPARE(listener->mLastTriggeredEvent, first);
	QCOMPARE(first->mTriggerCount, 2);
	
	// TODO: why is the increment not updated but the listener is notified correctly?
	QCOMPARE(second->mTriggerCount, 1);
	QVERIFY(listener->mEventNames->compare(
			QString("Event [Event1],Event [Event2],Event [Event1],")) == 0);
	
	delete first;
	delete second;
	delete listener;
	
}

}
