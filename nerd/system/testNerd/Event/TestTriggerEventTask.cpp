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



#include "TestTriggerEventTask.h"
#include "Event/TriggerEventTask.h"
#include "Event/Event.h"
#include "Core/Core.h"

namespace nerd {

/**
 * Helper Class for testScheduleEvent
 * Just an Event with a trigger-counter and
 * a triggered-from-core-counter
 */
class TestTriggerEvent : public Event {

	public:
		int mEventTriggered;
		int mEventTriggeredFromCore;

		TestTriggerEvent() : Event("TestTriggerEvent") {
			mEventTriggered = 0;
			mEventTriggeredFromCore = 0;
		}

		virtual void trigger() {
			mEventTriggered++;
			if(Core::getInstance()->isMainExecutionThread()) {
				mEventTriggeredFromCore++;
			}
		}
};

/**
 * Helper Class for testScheduleEvent
 * A seperate thread which calls scheduleEvent with the event declared in the constructor
 */
class TestScheduleEventThread : public QThread {
	public:
		TestScheduleEventThread(TestTriggerEvent *testTriggerEvent) {
			mTestTriggerEvent = testTriggerEvent;
		}
		void run() {
			TriggerEventTask::scheduleEvent(mTestTriggerEvent);
			// This should be ignored by the Core!
			Core::getInstance()->executePendingTasks();
		}
	private:
		TestTriggerEvent *mTestTriggerEvent;
};

void TestTriggerEventTask::testScheduleEvent() {
	Core::resetCore();
	// set this thread to be the main execution thread of the core
	Core::getInstance()->setMainExecutionThread();

	// schedule an event in a seperate thread
	TestTriggerEvent *myTestTriggerEvent = new TestTriggerEvent;
	TestScheduleEventThread *myThread = new TestScheduleEventThread(myTestTriggerEvent);
	myThread->start();
	// wait until the event is scheduled by the seperate thread
	myThread->wait();

	QCOMPARE(myTestTriggerEvent->mEventTriggered, 0);
	QCOMPARE(myTestTriggerEvent->mEventTriggeredFromCore, 0);

	// let the core execute the scheduled event task
	Core::getInstance()->executePendingTasks();

	QCOMPARE(myTestTriggerEvent->mEventTriggered, 1);
	QCOMPARE(myTestTriggerEvent->mEventTriggeredFromCore, 1);

	// trigger once more
	myThread->start();
	myThread->wait();
	Core::getInstance()->executePendingTasks();

	QCOMPARE(myTestTriggerEvent->mEventTriggered, 2);
	QCOMPARE(myTestTriggerEvent->mEventTriggeredFromCore, 2);



	delete myTestTriggerEvent;
	delete myThread;
}

}
