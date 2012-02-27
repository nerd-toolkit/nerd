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

#include "TestSimulationEnvironmentManager.h"
#include "Core/Core.h"
#include "Physics/SimulationEnvironmentManager.h"
#include "Physics/SimObjectAdapter.h"
#include "Physics/Physics.h"
#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include "Value/BoolValue.h"
#include "Event/Event.h"
#include "NerdConstants.h"
#include "Randomization/ListRandomizer.h"

namespace nerd {
	
void TestSimulationEnvironmentManager::testStoreAndResetParams() {
	Core::resetCore();
	SimulationEnvironmentManager *sManager = Physics::getSimulationEnvironmentManager();
	
	SimObjectAdapter obj1("Obj1", "");
	DoubleValue *test1 = new DoubleValue(1.1);
	obj1.addParameter("Test1", test1);

	StringValue *test2 = new StringValue("Hehe");
	obj1.addParameter("Test2", test2);

	BoolValue *test3 = new BoolValue(true);
	obj1.addParameter("Test3", test3);
	
	sManager->storeParameter(&obj1, "Test1", "2.2");
	sManager->storeParameter(&obj1, "Test2", "Test");
	sManager->storeParameter(&obj1, "Test3", "F");
	
	sManager->resetStartConditions();
	QCOMPARE(test1->get(), 2.2);
	QVERIFY(test2->get().compare("Test") == 0);
	QVERIFY(!test3->get());

	sManager->clearParameter(&obj1, "Test2");
	test2->set("NewText");
	sManager->resetStartConditions();
	QVERIFY(test2->get().compare("NewText") == 0);

	SimObjectAdapter obj2("Obj2", "");
	IntValue *test4 = new IntValue(5);
	obj2.addParameter("Test1", test4);

	Vector3DValue *test5 = new Vector3DValue(1.1, 2.2, -3.3);
	obj2.addParameter("Test2", test5);

	BoolValue *test6 = new BoolValue(true);
	obj2.addParameter("Test3", test6);

	sManager->storeParameter(&obj2, "Test1", "111");
	sManager->storeParameter(&obj2, "Test2", "(-1.2, 0.05, 33.21)");
	sManager->storeParameter(&obj2, "Test3", "F");

	sManager->resetStartConditions();
	
	QCOMPARE(test4->get(), 111);
	QCOMPARE(test5->getX(), -1.2);
	QCOMPARE(test5->getY(), 0.05);
	QCOMPARE(test5->getZ(), 33.21);
	QVERIFY(!test6->get());

	sManager->storeParameter(&obj2, "Test1", "122");
	sManager->storeParameter(&obj2, "Test2", "(1.2, -0.05, 33.21)");
	sManager->storeParameter(&obj2, "Test3", "T");
	
	sManager->resetStartConditions();
	QCOMPARE(test4->get(), 122);
	QCOMPARE(test5->getX(), 1.2);
	QCOMPARE(test5->getY(), -0.05);
	QCOMPARE(test5->getZ(), 33.21);
	QVERIFY(test6->get());

	sManager->storeParameter(&obj2, "Test1", "111");
	sManager->storeParameter(&obj2, "Test2", "(-1.2, 0.05, 33.21)");
	sManager->storeParameter(&obj2, "Test3", "F");

	sManager->clearParameters(&obj2);	
	sManager->resetStartConditions();
	QCOMPARE(test4->get(), 122);
	QCOMPARE(test5->getX(), 1.2);
	QCOMPARE(test5->getY(), -0.05);
	QCOMPARE(test5->getZ(), 33.21);
	QVERIFY(test6->get());

	sManager->storeParameter(&obj1, "Test1", "1.23");
	sManager->storeParameter(&obj1, "Test2", "AnotherText");
	sManager->storeParameter(&obj1, "Test3", "T");
	sManager->storeParameter(&obj2, "Test1", "222");
	sManager->storeParameter(&obj2, "Test2", "(2.1, 500.0, 3.21)");
	sManager->storeParameter(&obj2, "Test3", "T");
	sManager->resetStartConditions();
	
	QCOMPARE(test1->get(), 1.23);
	QVERIFY(test2->get().compare("AnotherText") == 0);
	QVERIFY(test3->get());
	QCOMPARE(test4->get(), 222);
	QCOMPARE(test5->getX(), 2.1);
	QCOMPARE(test5->getY(), 500.0);
	QCOMPARE(test5->getZ(), 3.21);
	QVERIFY(test6->get());

	sManager->storeParameter(&obj1, "Test1", "-1.11");
	sManager->storeParameter(&obj1, "Test2", "OldText");
	sManager->storeParameter(&obj1, "Test3", "F");
	sManager->storeParameter(&obj2, "Test1", "0");
	sManager->storeParameter(&obj2, "Test2", "(0.0, 0.0, -0.01)");
	sManager->storeParameter(&obj2, "Test3", "F");
	
	sManager->clearAllParameters();
	sManager->resetStartConditions();
	QCOMPARE(test1->get(), 1.23);
	QVERIFY(test2->get().compare("AnotherText") == 0);
	QVERIFY(test3->get());
	QCOMPARE(test4->get(), 222);
	QCOMPARE(test5->getX(), 2.1);
	QCOMPARE(test5->getY(), 500.0);
	QCOMPARE(test5->getZ(), 3.21);
	QVERIFY(test6->get());

}


// void TestSimulationEnvironmentManager::testCreateSnapshot() {
// 	Core::resetCore();
// 	SimulationEnvironmentManager *sManager = Physics::getSimulationEnvironmentManager();
// 	//TODO (if necessary)
// }
// 
// void TestSimulationEnvironmentManager::testRandomization() {
// 	Core::resetCore();
// 	SimulationEnvironmentManager *sManager = Physics::getSimulationEnvironmentManager();	
// 	
// 	DoubleValue randomizedDouble(0.0);
// 	ListRandomizer randomizer1(&randomizedDouble);
// 	randomizer1.addListItem("1.1");
// 	randomizer1.addListItem("-2.5");
// 	randomizer1.addListItem("55.05");
// 	randomizer1.addListItem("-0.01");
// 	randomizer1.setSequential(true);
// 
// 	StringValue randomizedString("");
// 	ListRandomizer randomizer2(&randomizedString);
// 	randomizer2.addListItem("a");
// 	randomizer2.addListItem("b");
// 	randomizer2.addListItem("c");
// 	randomizer2.setSequential(true);
// 
// 	sManager->addRandomizer(&randomizer1);
// 	sManager->addRandomizer(&randomizer2);
// 
// 	QCOMPARE(randomizedDouble.get(), 0.0);
// 	QVERIFY(randomizedString.get().compare("") == 0);
// 
// 	sManager->resetStartConditions();
// 	QCOMPARE(randomizedDouble.get(), 1.1);
// 	QVERIFY(randomizedString.get().compare("a") == 0);
// 
// 	sManager->resetStartConditions();
// 	QCOMPARE(randomizedDouble.get(), -2.5);
// 	QVERIFY(randomizedString.get().compare("b") == 0);
// 
// 	sManager->resetStartConditions();
// 	QCOMPARE(randomizedDouble.get(), 55.05);
// 	QVERIFY(randomizedString.get().compare("c") == 0);
// 
// 	sManager->resetStartConditions();
// 	QCOMPARE(randomizedDouble.get(), -0.01);
// 	QVERIFY(randomizedString.get().compare("a") == 0);
// 	
// }
}
