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

#include "TestFitnessManager.h"
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "Fitness/FitnessManager.h"
#include "Fitness/Fitness.h"
#include "FitnessFunctionAdapter.h"
#include "EvolutionConstants.h"
#include "NerdConstants.h"

namespace nerd{


//Verena
void TestFitnessManager::testFitnessManager() {
	Core::resetCore();
	QCOMPARE(Core::getInstance()->getGlobalObjects().size(), 0);
	Fitness::install();
	FitnessManager *fManager = Fitness::getFitnessManager();
	QCOMPARE(Core::getInstance()->getGlobalObjects().size(), 1);
	QCOMPARE(fManager->getFitnessFunctionPrototypes().size(), 0);
	QCOMPARE(fManager->getFitnessFunctions().size(), 0);	
	QVERIFY(fManager->getName().compare(EvolutionConstants::OBJECT_FITNESS_MANAGER) == 0);
	
	EventManager *eManager = Core::getInstance()->getEventManager();
	Event *eventNextIndividual = eManager->createEvent(EvolutionConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL);
	Event *eventTryCompleted = eManager->createEvent(EvolutionConstants::EVENT_EXECUTION_TRY_COMPLETED);
	Event *eventNextTry = eManager->createEvent(EvolutionConstants::EVENT_EXECUTION_NEXT_TRY);
	Event *eventNextStepCompleted = eManager->createEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);
	Event *eventResetSimulation = eManager->createEvent(NerdConstants::EVENT_EXECUTION_RESET);

	if(eventNextStepCompleted == 0) {
		eventNextStepCompleted = eManager->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);
	}
	Event *eventGenerationCompleted = eManager->createEvent(
			EvolutionConstants::EVENT_EXECUTION_GENERATION_COMPLETED);	
	Event *eventIndividualCompleted = eManager->createEvent(
			EvolutionConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED);	

	QVERIFY(eventNextTry != 0);
	QVERIFY(eventNextIndividual != 0);
	QVERIFY(eventTryCompleted != 0);
	QVERIFY(eventNextStepCompleted != 0);
	QVERIFY(eventGenerationCompleted != 0);
	QVERIFY(eventIndividualCompleted != 0);
	QVERIFY(eventResetSimulation != 0);

	Core::log("Test!");

	QVERIFY(fManager->init());
	QVERIFY(fManager->bind());
	bool fitnessFunction1Destroyed = false;
	FitnessFunctionAdapter *fitness1 = new FitnessFunctionAdapter("Fitness1", &fitnessFunction1Destroyed);
	FitnessFunctionAdapter *fitness2 = new FitnessFunctionAdapter("Fitness2");
	FitnessFunctionAdapter *fitness3 = new FitnessFunctionAdapter("Fitness1");
	QVERIFY(fManager->addFitnessFunction(fitness1));
	QCOMPARE(fManager->getFitnessFunctions().size(), 1);
	QVERIFY(fManager->getFitnessFunctionNames().contains("Fitness1"));

// 	A fitness function is only added once to the manager
	QVERIFY(!fManager->addFitnessFunction(fitness1));
// 	A fitness function with the same name is not allowed to be added to the manager.
	QVERIFY(!fManager->addFitnessFunction(fitness3));
	QCOMPARE(fManager->getFitnessFunctions().size(), 1);
	QVERIFY(fManager->addFitnessFunction(fitness2));
	
// Test FitnessFunction-call mechanism.
	fitness1->mCalculateReturnValue = 1.0;
	eventNextIndividual->trigger();
	QCOMPARE(fitness1->mResetCounter, 1);
	QCOMPARE(fitness1->mResetTryCounter, 0);
	QCOMPARE(fitness1->mPrepareNextTryCounter, 0);
	
	eventNextTry->trigger();
	QCOMPARE(fitness1->mResetCounter, 1);
	QCOMPARE(fitness1->mResetTryCounter, 1);
	QCOMPARE(fitness1->mPrepareNextTryCounter, 1);
		
	eventNextStepCompleted->trigger();
	QCOMPARE(fitness1->mUpdateCounter, 1);
	eventNextStepCompleted->trigger();
	QCOMPARE(fitness1->mUpdateCounter, 2);
	eventNextStepCompleted->trigger();
	QCOMPARE(fitness1->mUpdateCounter, 3);
	QCOMPARE(fitness1->mCalculateCurrentFitnessCounter, 3);
	
	eventTryCompleted->trigger();
	QCOMPARE(fitness1->mCalculateFitnessCounter, 1);
	QCOMPARE(fitness1->getFitness(), 3.0);
	
	eventNextTry->trigger();
	QCOMPARE(fitness1->getCurrentFitness(), 0.0);
	QCOMPARE(fitness1->getFitness(), 3.0);

	eventNextStepCompleted->trigger();
	eventNextStepCompleted ->trigger();
	QCOMPARE(fitness1->getCurrentFitness(), 2.0);
	
	eventNextIndividual->trigger();
	QCOMPARE(fitness1->getCurrentFitness(), 0.0);
	QCOMPARE(fitness1->getFitness(), 0.0);

// Test removing fitness functions.
	QVERIFY(fManager->removeFitnessFunction("Fitness2"));
	QVERIFY(fManager->addFitnessFunction(fitness2));
	QVERIFY(fManager->removeFitnessFunction(fitness2));
	QVERIFY(!fManager->removeFitnessFunction(fitness2));
	QVERIFY(!fManager->removeFitnessFunction(fitness3));
	
	delete fitness2;
	delete fitness3;
	Core::resetCore();
	
	QVERIFY(fitnessFunction1Destroyed == true);
}


//Verena
void TestFitnessManager::testPrototyping() {
	Core::resetCore();
	
	FitnessManager *fManager = new FitnessManager();
	
	EventManager *eManager = Core::getInstance()->getEventManager();
	Event *eventNextIndividual = eManager->createEvent(EvolutionConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL);
	Event *eventTryCompleted = eManager->createEvent(EvolutionConstants::EVENT_EXECUTION_TRY_COMPLETED);
	Event *eventNextTry = eManager->createEvent(EvolutionConstants::EVENT_EXECUTION_NEXT_TRY);
	Event *eventNextStep =   eManager->createEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	if(eventNextStep == 0) {
		eventNextStep = eManager->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	}
	QVERIFY(eventNextTry != 0);
	QVERIFY(eventNextIndividual != 0);
	QVERIFY(eventTryCompleted != 0);
	QVERIFY(eventNextStep != 0);

	fManager->init();
	fManager->bind();
	
	QCOMPARE(fManager->getFitnessFunctionPrototypes().size(), 0);
	FitnessFunctionAdapter *fitness1 = new FitnessFunctionAdapter("Function1");
	fitness1->mCalculateReturnValue = 11.0;
	FitnessFunctionAdapter *fitness2 = new FitnessFunctionAdapter("Function2");
	QVERIFY(fManager->addFitnessFunctionPrototype("Fitness1", fitness1));
	QVERIFY(fManager->addFitnessFunctionPrototype("Fitness2", fitness2));
	QCOMPARE(fManager->getFitnessFunctionPrototypes().size(), 2);
	QVERIFY(fManager->getFitnessFunctionPrototypes().contains("Fitness1"));

	FitnessFunctionAdapter *fitness3 = new FitnessFunctionAdapter("Function3");
	QVERIFY(!fManager->addFitnessFunctionPrototype("Fitness3", fitness1));
	QVERIFY(!fManager->addFitnessFunctionPrototype("Fitness1", fitness3));

	FitnessFunctionAdapter *fitnessFromPrototype1 = dynamic_cast<FitnessFunctionAdapter*>(
					fManager->createFitnessFunctionFromPrototype("Fitness1", "PrototypeCopy"));
	QVERIFY(fitnessFromPrototype1 != 0);
	QVERIFY(fitnessFromPrototype1->getName().compare("PrototypeCopy") == 0);
	QCOMPARE(fitnessFromPrototype1->mCalculateReturnValue, 11.0);
	QVERIFY(fitnessFromPrototype1 != fitness1);
	
	QVERIFY(fManager->createFitnessFunctionFromPrototype("Test") == 0);
	FitnessFunctionAdapter *fitnessFromPrototype2 = dynamic_cast<FitnessFunctionAdapter*>(
					fManager->createFitnessFunctionFromPrototype("Fitness2"));
	QVERIFY(fitnessFromPrototype2->getName().compare("Fitness2") == 0);
	
	QVERIFY(fManager->removeFitnessFunctionPrototype("Fitness1"));
	QVERIFY(fManager->getFitnessFunctionPrototypes().keys().contains("Fitness2"));
	QVERIFY(!fManager->getFitnessFunctionPrototypes().keys().contains("Fitness1"));
	QVERIFY(fManager->createFitnessFunctionFromPrototype("Fitness1") == 0);


	delete fitnessFromPrototype2;
	delete fitnessFromPrototype1;
	delete fitness1;
	delete fitness3;
	delete fManager;
	Core::resetCore();

}
}
