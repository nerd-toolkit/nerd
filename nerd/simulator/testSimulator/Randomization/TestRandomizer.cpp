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

#include "TestRandomizer.h"
#include "RandomizerAdapter.h"
#include "Core/Core.h"
#include "SimulationConstants.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Physics/Physics.h"
#include "Physics/SimulationEnvironmentManager.h"
#include "Randomization/ListRandomizer.h"
#include "Randomization/DoubleRandomizer.h"
#include "Math/Random.h"
#include "InstallAdapter.h"
#include "Math/Math.h"

namespace nerd{

// verena
void TestRandomizer::testRandomizer() {
	Core::resetCore();
	Event *nextIndividual = Core::getInstance()->getEventManager()->
		getEvent(SimulationConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL, true);
	if(nextIndividual == 0) {
		qDebug("Could not create event \"EVENT_EXECUTION_NEXT_INDIVIDUAL\".");
		QVERIFY(false);
		return;
	}
	RandomizerAdapter *randomizer = new RandomizerAdapter();
	QCOMPARE(randomizer->getResetCount(), 0);
	QCOMPARE(randomizer->getRandomizerCount(), 0);
	
	nextIndividual->trigger();
	QCOMPARE(randomizer->getResetCount(), 1);
	
	randomizer->applyRandomization();
	QCOMPARE(randomizer->getResetCount(), 1);
	QCOMPARE(randomizer->getRandomizerCount(), 1);

	nextIndividual->trigger();
	QCOMPARE(randomizer->getResetCount(), 2);
	QCOMPARE(randomizer->getRandomizerCount(), 0);

	delete randomizer;

	Core::resetCore();
	Physics::install();
	Core::getInstance()->init();
	
	SimulationEnvironmentManager *sManager = Physics::getSimulationEnvironmentManager();
	
	RandomizerAdapter *randomizer2 = new RandomizerAdapter();
	sManager->addRandomizer(randomizer2);
	QCOMPARE(randomizer2->getRandomizerCount(), 0);
	sManager->resetStartConditions();
	QCOMPARE(randomizer2->getRandomizerCount(), 1);
	
	//don't delete randomizer2 because it is destroyed by EnvironmentManager.	
	//TODO add test for this
}	

// verena
void TestRandomizer::testListRandomizer() {
	Core::resetCore();
	new InstallAdapter();
	Physics::install();

	Core::getInstance()->init();

	SimulationEnvironmentManager *sManager = Physics::getSimulationEnvironmentManager();
	int seed = Random::nextInt();
	
	IntValue *seedValue = Core::getInstance()->getValueManager()->
		getIntValue(SimulationConstants::VALUE_RANDOMIZATION_SIMULATION_SEED);
	if(seedValue == 0) {
		QVERIFY(false);
		qDebug("Could not find required value.");
		return;
	}

	IntValue *currentTry = Core::getInstance()->getValueManager()->
		getIntValue(SimulationConstants::VALUE_EXECUTION_CURRENT_TRY);
	if(currentTry == 0) {
		QVERIFY(false);
		qDebug("Could not find required value.");
		return;
	}
	Event *nextIndividual = Core::getInstance()->getEventManager()->
		getEvent(SimulationConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL, true);
	if(nextIndividual == 0) {
		qDebug("Could not create event \"EVENT_EXECUTION_NEXT_INDIVIDUAL\".");
		QVERIFY(false);
		return;
	}
	seedValue->set(seed);
	currentTry->set(1);
	
	DoubleValue *doubleValue = new DoubleValue();
	ListRandomizer *listRand = new ListRandomizer(doubleValue);
	listRand->addListItem("1.0");
	listRand->addListItem("2.0");
	listRand->addListItem("3.3");
	listRand->setSequential(true);
	sManager->addRandomizer(listRand);
	
	// test the sequential mode of the listRandomizer
	QCOMPARE(doubleValue->get(), 0.0);
	sManager->resetStartConditions();
	QCOMPARE(doubleValue->get(), 1.0);
	sManager->resetStartConditions();
	QCOMPARE(doubleValue->get(), 2.0);
	currentTry->set(2);
	sManager->resetStartConditions();
	QCOMPARE(doubleValue->get(), 3.3);
	currentTry->set(3);
	sManager->resetStartConditions();
	QCOMPARE(doubleValue->get(), 1.0);
	currentTry->set(4);
	sManager->resetStartConditions();
	QCOMPARE(doubleValue->get(), 2.0);
	//after resetting to the next individual the list index will be reset to 0.
	nextIndividual->trigger();
	currentTry->set(3);
	sManager->resetStartConditions();
	QCOMPARE(doubleValue->get(), 1.0);

	//test random-mode of the list-randomization
	listRand->setSequential(false);
	listRand->setAdditive(false);
	listRand->addListItem("43.21");
	listRand->addListItem("56.78");
		
	seedValue->set(seed);
	QList<double> resultsFirstRun; 
	currentTry->set(1);
	sManager->resetStartConditions();
	resultsFirstRun.push_back(doubleValue->get());
	currentTry->set(2);
	sManager->resetStartConditions();
	resultsFirstRun.push_back(doubleValue->get());
	currentTry->set(3);
	sManager->resetStartConditions();
	resultsFirstRun.push_back(doubleValue->get());
	
	int seed2 = 1234;
	seedValue->set(seed2);
	QList<double> resultsSecondRun; 
	currentTry->set(1);
	sManager->resetStartConditions();
	resultsSecondRun.push_back(doubleValue->get());
	currentTry->set(2);
	sManager->resetStartConditions();
	resultsSecondRun.push_back(doubleValue->get());
	currentTry->set(3);
	sManager->resetStartConditions();
	resultsSecondRun.push_back(doubleValue->get());

	seedValue->set(seed);
	currentTry->set(1);
	sManager->resetStartConditions();
	QCOMPARE(resultsFirstRun.at(0), doubleValue->get());
	currentTry->set(2);
	sManager->resetStartConditions();
	QCOMPARE(resultsFirstRun.at(1), doubleValue->get());
	currentTry->set(3);
	sManager->resetStartConditions();
	QCOMPARE(resultsFirstRun.at(2), doubleValue->get());
	
	seedValue->set(seed2);
	currentTry->set(1);
	sManager->resetStartConditions();
	QCOMPARE(resultsSecondRun.at(0), doubleValue->get());
	currentTry->set(2);
	sManager->resetStartConditions();
	QCOMPARE(resultsSecondRun.at(1), doubleValue->get());
	currentTry->set(3);
	sManager->resetStartConditions();
	QCOMPARE(resultsSecondRun.at(2), doubleValue->get());

}

// verena
void TestRandomizer::testDoubleRandomizer() {
	Core::resetCore();
	new InstallAdapter();
	Physics::install();

	Core::getInstance()->init();

	SimulationEnvironmentManager *sManager = Physics::getSimulationEnvironmentManager();
	int seed = Random::nextInt();
	
	IntValue *seedValue = Core::getInstance()->getValueManager()->
		getIntValue(SimulationConstants::VALUE_RANDOMIZATION_SIMULATION_SEED);
	if(seedValue == 0) {
		QVERIFY(false);
		qDebug("Could not find required value.");
		return;
	}

	IntValue *currentTry = Core::getInstance()->getValueManager()->
		getIntValue(SimulationConstants::VALUE_EXECUTION_CURRENT_TRY);
	if(currentTry == 0) {
		QVERIFY(false);
		qDebug("Could not find required value.");
		return;
	}
	seedValue->set(seed);
	currentTry->set(1);

	Event *nextIndividual = Core::getInstance()->getEventManager()->
		getEvent(SimulationConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL, true);
	if(nextIndividual == 0) {
		qDebug("Could not create event \"EVENT_EXECUTION_NEXT_INDIVIDUAL\".");
		QVERIFY(false);
		return;
	}

	//test random-mode
	DoubleValue *doubleValue = new DoubleValue(0.4);
	DoubleRandomizer *doubleRand= new DoubleRandomizer(doubleValue);
	doubleRand->setMin(0.4);
	doubleRand->setMax(12.21);
	doubleRand->setProbability(0.75);

	doubleValue->set(0.4);
	doubleRand->setAdditive(false);
	seedValue->set(seed);
	QList<double> resultsFirstRun; 
	currentTry->set(1);
	sManager->resetStartConditions();
	resultsFirstRun.push_back(doubleValue->get());
	currentTry->set(2);
	sManager->resetStartConditions();
	resultsFirstRun.push_back(doubleValue->get());
	currentTry->set(3);
	sManager->resetStartConditions();
	resultsFirstRun.push_back(doubleValue->get());
	
	doubleValue->set(0.4);
	int seed2 = 1234;
	seedValue->set(seed2);
	QList<double> resultsSecondRun; 
	currentTry->set(1);
	sManager->resetStartConditions();
	resultsSecondRun.push_back(doubleValue->get());
	currentTry->set(2);
	sManager->resetStartConditions();
	resultsSecondRun.push_back(doubleValue->get());
	currentTry->set(3);
	sManager->resetStartConditions();
	resultsSecondRun.push_back(doubleValue->get());

	doubleValue->set(0.4);
	seedValue->set(seed);
	currentTry->set(1);
	sManager->resetStartConditions();
	QCOMPARE(resultsFirstRun.at(0), doubleValue->get());
	currentTry->set(2);
	sManager->resetStartConditions();
	QCOMPARE(resultsFirstRun.at(1), doubleValue->get());
	currentTry->set(3);
	sManager->resetStartConditions();
	QCOMPARE(resultsFirstRun.at(2), doubleValue->get());
	
	doubleValue->set(0.4);
	seedValue->set(seed2);
	currentTry->set(1);
	sManager->resetStartConditions();
	QCOMPARE(resultsSecondRun.at(0), doubleValue->get());
	currentTry->set(2);
	sManager->resetStartConditions();
	QCOMPARE(resultsSecondRun.at(1), doubleValue->get());
	currentTry->set(3);
	sManager->resetStartConditions();
	QCOMPARE(resultsSecondRun.at(2), doubleValue->get());

	//test additive-mode. 
	DoubleValue *doubleValue2 = new DoubleValue(0.4);
	DoubleRandomizer *doubleRand2 = new DoubleRandomizer(doubleValue2);
	doubleRand2->setAdditive(true);
	doubleRand2->setMin(0.3);
	doubleRand2->setMax(11.12);
	doubleRand2->setDeviation(0.1);
	doubleRand2->setProbability(0.7);
	sManager->addRandomizer(doubleRand2);

	double lastValue = doubleValue2->get();
	double deviation = 0.1;
	currentTry->set(1);
	sManager->resetStartConditions();
	QVERIFY(Math::abs(doubleValue2->get() - lastValue) <= deviation);
	lastValue = doubleValue2->get();
	double tryOneResult = lastValue;

	currentTry->set(2);
	sManager->resetStartConditions();
	QVERIFY(Math::abs(doubleValue2->get() - lastValue) <= deviation);
	lastValue = doubleValue2->get();

	currentTry->set(3);
	sManager->resetStartConditions();
	QVERIFY(Math::abs(doubleValue2->get() - lastValue) <= deviation);
	lastValue = doubleValue2->get();
	
	nextIndividual->trigger();
	doubleValue2->set(0.4);
	currentTry->set(1);
	sManager->resetStartConditions();
	QCOMPARE(doubleValue2->get(), tryOneResult);
}

}
