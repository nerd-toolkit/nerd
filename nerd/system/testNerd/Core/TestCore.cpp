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



#include "TestCore.h"
#include "Core/SystemObject.h"
#include "Value/IntValue.h"
#include "MSystemObject.h"
#include <QString>
#include "Event/EventListenerAdapter.h"
#include "Core/TaskAdapter.h"

namespace nerd{

//verena
void TestCore::testCoreInstance() {
	Core::resetCore();

	Core *cInstance = Core::getInstance();
	QVERIFY(!cInstance->isInitialized());

	//begin chris
	QVERIFY(cInstance->getValueManager() != 0);
	QVERIFY(cInstance->getEventManager() != 0);
	QVERIFY(cInstance->getPlugInManager() != 0);
	//end chris

	cInstance->init();
	QVERIFY(cInstance->isInitialized());
	
	//begin chris
	bool destroy1;
	MSystemObject *sysob1 = new MSystemObject(&destroy1);
	QVERIFY(destroy1 == false);
	
	QVERIFY(cInstance->addSystemObject(sysob1) == true);
	QCOMPARE(cInstance->getSystemObjects().size(), 1);
	QCOMPARE(cInstance->getPendingTasks().size(), 0);
	
	Core::resetCore();
	QVERIFY(destroy1 == true); //system object was destroyed.
	
	cInstance = Core::getInstance(); //new instance
	QCOMPARE(cInstance->getSystemObjects().size(), 0);
	//end chris
	
	QVERIFY(!cInstance->isInitialized());	
	
	Core::resetCore();
}

//verena
void TestCore::testSystemObjectManagement() {
	
	Core::resetCore();
	Core *cInstance = Core::getInstance();
	
	MSystemObject *mSysO = new MSystemObject();
	QVERIFY(cInstance->addSystemObject(mSysO));
	QCOMPARE(cInstance->getSystemObjects().size(), 1);
	
	MSystemObject *mSysO2 = new MSystemObject();
	QVERIFY(cInstance->addSystemObject(mSysO2));
	QVERIFY(!cInstance->addSystemObject(mSysO));
	QCOMPARE(cInstance->getSystemObjects().size(), 2);

	//begin chris
	QVERIFY(cInstance->getSystemObjects().contains(mSysO2));

	QVERIFY(cInstance->addGlobalObject("Global1", mSysO2));
	QVERIFY(cInstance->getGlobalObjects().size() == 1);
	QVERIFY(cInstance->getGlobalObjects().contains(mSysO2));
	
	//remove objects
	QVERIFY(cInstance->removeSystemObject(0) == false);
	QVERIFY(cInstance->removeSystemObject(mSysO) == true);
	QCOMPARE(cInstance->getSystemObjects().size(), 1);
	QVERIFY(cInstance->getSystemObjects().contains(mSysO2));
	QVERIFY(cInstance->getGlobalObjects().contains(mSysO2));

	QVERIFY(cInstance->removeSystemObject(mSysO2) == true);
	QCOMPARE(cInstance->getSystemObjects().size(), 0);
	QCOMPARE(cInstance->getGlobalObjects().size(), 0);
	//end Chris

	delete mSysO;
	delete mSysO2;

	Core::resetCore();
}


//chris
void TestCore::testInitAndShutDown() {
	Core::resetCore();
	Core *cInstance = Core::getInstance();
	
	MSystemObject *mSysO = new MSystemObject();
	MSystemObject *mSysO2 = new MSystemObject();
	QVERIFY(cInstance->addSystemObject(mSysO) == true);
	QVERIFY(cInstance->addSystemObject(mSysO2) == true);
	QVERIFY(cInstance->getSystemObjects().size() == 2);
	
	QVERIFY(mSysO->mInitCount == 0);
	QVERIFY(mSysO->mBindCount == 0);
	QVERIFY(mSysO->mCleanUpCount == 0);

	QVERIFY(mSysO2->mInitCount == 0);
	QVERIFY(mSysO2->mBindCount == 0);
	QVERIFY(mSysO2->mCleanUpCount == 0);
	
	mSysO->mInitReturnValue = false;
	
	QVERIFY(cInstance->isInitialized() == false);
	
	QVERIFY(cInstance->init() == false); 
	//because one of the SystemObjects returned false at init,
	
	QVERIFY(cInstance->isInitialized() == false);
	QVERIFY(mSysO->mInitCount == 1);
	QVERIFY(mSysO->mBindCount == 0);
	QVERIFY(mSysO->mCleanUpCount == 0);
	QVERIFY(mSysO2->mInitCount == 1);
	QVERIFY(mSysO2->mBindCount == 0);
	QVERIFY(mSysO2->mCleanUpCount == 0);
	
	mSysO->mInitReturnValue = true;
	mSysO->mBindReturnValue = false;
	
	QVERIFY(cInstance->init() == false); 
	//because one of the SystemObjects returned false at bind.
	
	QVERIFY(cInstance->isInitialized() == false);
	QVERIFY(mSysO->mInitCount == 2);
	QVERIFY(mSysO->mBindCount == 1);
	QVERIFY(mSysO->mCleanUpCount == 0);
	QVERIFY(mSysO2->mInitCount == 2);
	QVERIFY(mSysO2->mBindCount == 1);
	QVERIFY(mSysO2->mCleanUpCount == 0);
	
	
	mSysO->mInitReturnValue = true;
	mSysO->mBindReturnValue = true;
	
	QVERIFY(cInstance->init() == true); 
	
	QVERIFY(cInstance->isInitialized() == true);
	QVERIFY(mSysO->mInitCount == 3);
	QVERIFY(mSysO->mBindCount == 2);
	QVERIFY(mSysO->mCleanUpCount == 0);
	QVERIFY(mSysO2->mInitCount == 3);
	QVERIFY(mSysO2->mBindCount == 2);
	QVERIFY(mSysO2->mCleanUpCount == 0);
	
	
	//only binding
	mSysO->mBindReturnValue = false;
	QVERIFY(cInstance->bindSystemObjects() == false);
	QVERIFY(mSysO->mBindCount == 3);
	QVERIFY(mSysO2->mBindCount == 3);
	
	mSysO->mBindReturnValue = true;
	QVERIFY(cInstance->bindSystemObjects() == true);
	QVERIFY(mSysO->mBindCount == 4);
	QVERIFY(mSysO2->mBindCount == 4);
	
	//cleanUp
	mSysO->mCleanUpReturnValue = false;
	Event *mShutDownEvent = cInstance->getShutDownEvent();
	EventListenerAdapter ela("ELA1");
	mShutDownEvent->addEventListener(&ela);
	QVERIFY(ela.mLastTriggeredEvent == 0);
	QVERIFY(ela.mCountEventOccured == 0);
	
	QVERIFY(cInstance->shutDown() == false);
	//failing cleanUp is just reported, but has no other effects!
	
	QVERIFY(ela.mCountEventOccured == 1);
	QVERIFY(ela.mLastTriggeredEvent == mShutDownEvent);
	
	QVERIFY(cInstance->isInitialized() == false);
	QVERIFY(mSysO->mInitCount == 3);
	QVERIFY(mSysO->mBindCount == 4);
	QVERIFY(mSysO->mCleanUpCount == 1);
	QVERIFY(mSysO2->mInitCount == 3);
	QVERIFY(mSysO2->mBindCount == 4);
	QVERIFY(mSysO2->mCleanUpCount == 1);

	Core::resetCore();
}


//verena
void TestCore::testGlobalObjects() {

	Core::resetCore();
	Core *cInstance = Core::getInstance();
	
	QVERIFY(cInstance->getSystemObjects().size() == 0);
	
	MSystemObject *mSysO = new MSystemObject();
	QString test = "First";
	QVERIFY(cInstance->addGlobalObject(test, mSysO) == true);
	QVERIFY(cInstance->getSystemObjects().size() == 1);

	MSystemObject *mSysO2 = new MSystemObject();
	QString test2 = "Second";
	QVERIFY(cInstance->addGlobalObject(test2, mSysO2) == true);
	QVERIFY(cInstance->getSystemObjects().size() == 2);

	MSystemObject *mSysO3 = new MSystemObject();
	QString test3 = "Third";
	QVERIFY(cInstance->addGlobalObject(test3, mSysO3) == true);
	QVERIFY(cInstance->getSystemObjects().size() == 3);

	QVERIFY(cInstance->addGlobalObject("Test", mSysO3) == true);

	//add a global object with an already existing name (fails)
	MSystemObject *mSysO4 = new MSystemObject();
	QVERIFY(cInstance->addGlobalObject(test3, mSysO4) == false);

	MSystemObject *fromCore = dynamic_cast<MSystemObject*>(cInstance->getGlobalObject("Test"));
	QCOMPARE(fromCore, mSysO3);
	
	QVERIFY(cInstance->getGlobalObject("TEst") == NULL);

	MSystemObject *removed =
 				dynamic_cast<MSystemObject*>(cInstance->removeGlobalObject("Second"));
 				
	QCOMPARE(removed, mSysO2);
	
	QVERIFY(cInstance->getSystemObjects().size() == 3); //was not removed as SystemObject!

	delete mSysO4;
	
	Core::resetCore();
}


//chris
void TestCore::testTaskScheduling() {
	Core::resetCore();
	Core *cInstance = Core::getInstance();
	
	bool destroyFlag1 = false;
	bool destroyFlag2 = false;
	bool destroyFlag3 = false;
	bool destroyFlag4 = false;
	
	int countRuns1 = 0;
	int countRuns2 = 0;
	int countRuns3 = 0;
	int countRuns4 = 0;
	
	TaskAdapter *ta1 = new TaskAdapter(&destroyFlag1, &countRuns1);
	TaskAdapter *ta2 = new TaskAdapter(&destroyFlag2, &countRuns2);
	TaskAdapter *ta3 = new TaskAdapter(&destroyFlag3, &countRuns3);
	TaskAdapter *ta4 = new TaskAdapter(&destroyFlag4, &countRuns4);
	
	QCOMPARE(cInstance->getPendingTasks().size(), 0);
	
	//add a NULL task (fails)
	QVERIFY(cInstance->scheduleTask(0) == false);
	QCOMPARE(cInstance->getPendingTasks().size(), 0);
	
	//add a task
	QVERIFY(cInstance->scheduleTask(ta1) == true);
	QCOMPARE(cInstance->getPendingTasks().size(), 1);
	
	//add the same task again (fails to avoid double destruction
	QVERIFY(cInstance->scheduleTask(ta1) == false);
	QCOMPARE(cInstance->getPendingTasks().size(), 1);
	
	//add another task
	QVERIFY(cInstance->scheduleTask(ta2) == true);
	QCOMPARE(cInstance->getPendingTasks().size(), 2);
	
	//execute pending tasks (will also delete the tasks)
	cInstance->executePendingTasks();
	QCOMPARE(cInstance->getPendingTasks().size(), 0);
	QCOMPARE(countRuns1, 1);
	QCOMPARE(countRuns2, 1);
	QCOMPARE(destroyFlag1, true);
	QCOMPARE(destroyFlag2, true);
	
	//add another task, then...
	QVERIFY(cInstance->scheduleTask(ta3) == true);
	QCOMPARE(cInstance->getPendingTasks().size(), 1);
	
	//... clear tasks
	cInstance->clearPendingTasks();
	QCOMPARE(cInstance->getPendingTasks().size(), 0);
	QCOMPARE(countRuns3, 0); //was never executed.
	QCOMPARE(destroyFlag3, true);
	
	//add another task, then...
	QVERIFY(cInstance->scheduleTask(ta4) == true);
	QCOMPARE(cInstance->getPendingTasks().size(), 1);
	
	//clear core...
	Core::resetCore();
	
	QCOMPARE(countRuns4, 0); //was never executed.
	QCOMPARE(destroyFlag4, true);
	
}


}
