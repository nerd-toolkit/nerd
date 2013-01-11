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




#include "TestEvolutionManager.h"
#include <iostream>
#include "Evolution/EvolutionManager.h"
#include "Evolution/WorldAdapter.h"
#include "Core/Core.h"
#include "Evolution/Evolution.h"
#include "Value/ValueManager.h"
#include "Event/EventManager.h"
#include "EvolutionConstants.h"
#include "Event/EventListener.h"
#include "Evolution/EvolutionAlgorithmAdapter.h"
#include "SelectionMethod/SelectionMethodAdapter.h"
#include "Evolution/EvaluationMethodAdapter.h"
#include "Evolution/IndividualAdapter.h"

using namespace std;
namespace nerd {

void TestEvolutionManager::initTestCase() {
}

void TestEvolutionManager::cleanUpTestCase() {
}

//chris
void TestEvolutionManager::testConstruction() {
	Core::resetCore();

	ValueManager *vm = Core::getInstance()->getValueManager();
	EventManager *evm = Core::getInstance()->getEventManager();

	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_TRIGGER_NEXT_GENERATION) == 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_GENERATION_STARTED) == 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_GENERATION_COMPLETED) == 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_EVALUATION_STARTED) == 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_EVALUATION_COMPLETED) == 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_SELECTION_STARTED) == 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_SELECTION_COMPLETED) == 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_STARTED) == 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_COMPLETED) == 0);

	EvolutionManager *em = new EvolutionManager();

	QVERIFY(em->getName() == "EvolutionManager");
	QVERIFY(em->getEvolutionWorlds().size() == 0);

	QVERIFY(em->getNextGenerationTriggerEvent() != 0);
	QVERIFY(em->getNextGenerationTriggerEvent() ==
 			evm->getEvent(EvolutionConstants::EVENT_EVO_TRIGGER_NEXT_GENERATION));

	//EvolutionManager added several Events to the EventManager.
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_GENERATION_COMPLETED) != 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_EVALUATION_STARTED) != 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_EVALUATION_COMPLETED) != 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_SELECTION_STARTED) != 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_SELECTION_COMPLETED) != 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_STARTED) != 0);
	QVERIFY(evm->getEvent(EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_COMPLETED) != 0);

	QVERIFY(em->getCurrentGenerationValue() != 0);
	QVERIFY(em->getCurrentGenerationValue() == 	
			vm->getIntValue(EvolutionConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER));

	//register globally
	QVERIFY(Core::getInstance()->getGlobalObject(
			EvolutionConstants::OBJECT_EVOLUTION_MANAGER) == 0);
	
	QVERIFY(em->registerAsGlobalObject() == true);

	QVERIFY(Core::getInstance()->getGlobalObject(
			EvolutionConstants::OBJECT_EVOLUTION_MANAGER) == em);

	//registering a second time is fine
	QVERIFY(em->registerAsGlobalObject() == true);

	//registering a different EvolutionManager fails.
	EvolutionManager em2;
	QVERIFY(em2.registerAsGlobalObject() == false); //because the name is already in used.

	Core::getInstance()->removeSystemObject(em);
	delete em;

	Core::resetCore();
}

//chris
void TestEvolutionManager::testAddAndRemoveEvolutionWorlds() {

	Core::resetCore();

	bool destroyedWorld = false;

	{
		EvolutionManager *em = new EvolutionManager();

		WorldAdapter *world1 = new WorldAdapter("World1");
		WorldAdapter *world2 = new WorldAdapter("World2");
		world2->mDestroyFlag = &destroyedWorld;

		QVERIFY(em->addEvolutionWorld(0) == false); //fails
		QVERIFY(em->getEvolutionWorlds().size() == 0);

		QVERIFY(em->addEvolutionWorld(world1) == true);
		QVERIFY(em->getEvolutionWorlds().size() == 1);
		QVERIFY(em->getEvolutionWorlds().at(0) == world1);

		QVERIFY(em->addEvolutionWorld(world1) == false); //can not add a second time
		QVERIFY(em->getEvolutionWorlds().size() == 1);

		QVERIFY(em->addEvolutionWorld(world2) == true);
		QVERIFY(em->getEvolutionWorlds().size() == 2);
		QVERIFY(em->getEvolutionWorlds().contains(world1));
		QVERIFY(em->getEvolutionWorlds().contains(world2));

		QVERIFY(em->removeEvolutionWorld(0) == false);
		QVERIFY(em->getEvolutionWorlds().size() == 2);

		QVERIFY(em->removeEvolutionWorld(world1) == true);
		QVERIFY(em->getEvolutionWorlds().size() == 1);
		QVERIFY(em->getEvolutionWorlds().contains(world2)); 

		delete world1;
		delete em;
	}
	QVERIFY(destroyedWorld == true); // was destroyed by EvolutionManager.

	Core::resetCore();
}

//chris
class TestGenerationProcessingEventListener : public virtual EventListener {
	public:
	TestGenerationProcessingEventListener(EvolutionManager *evolutionManager) 
				: mSequenceCounter(0), mEvolutionManager(evolutionManager) 
	{
	
		EventManager *em = Core::getInstance()->getEventManager();

		mTriggerNextGenerationEvent = em->registerForEvent(
				EvolutionConstants::EVENT_EVO_TRIGGER_NEXT_GENERATION, this);
		mNextGenerationCompletedEvent = em->registerForEvent(
				EvolutionConstants::EVENT_EVO_GENERATION_COMPLETED, this);
		mEvaluationStartedEvent = em->registerForEvent(
			EvolutionConstants::EVENT_EVO_EVALUATION_STARTED, this);
		mEvaluationCompletedEvent = em->registerForEvent(
			EvolutionConstants::EVENT_EVO_EVALUATION_COMPLETED, this);
		mSelectionStartedEvent = em->registerForEvent(
			EvolutionConstants::EVENT_EVO_SELECTION_STARTED, this);
		mSelectionCompletedEvent = em->registerForEvent(
			EvolutionConstants::EVENT_EVO_SELECTION_COMPLETED, this);
		mEvolutionAlgorithmStartedEvent = em->registerForEvent(
			EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_STARTED, this);
		mEvolutionAlgorithmCompletedEvent = em->registerForEvent(
			EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_COMPLETED, this);

		QVERIFY(mTriggerNextGenerationEvent != 0);
		QVERIFY(mNextGenerationCompletedEvent != 0);
		QVERIFY(mEvaluationStartedEvent != 0);
		QVERIFY(mEvaluationCompletedEvent != 0);
		QVERIFY(mSelectionStartedEvent != 0);
		QVERIFY(mSelectionCompletedEvent != 0);
		QVERIFY(mEvolutionAlgorithmStartedEvent != 0);
		QVERIFY(mEvolutionAlgorithmCompletedEvent != 0);

		QVERIFY(mEvolutionManager != 0);

		//create evolution worlds
		mWorld1 = new WorldAdapter("World1");
		mEvolutionManager->addEvolutionWorld(mWorld1);
		mEvoAlg1 = new EvolutionAlgorithmAdapter("Evolution1");
		mWorld1->setEvolutionAlgorithm(mEvoAlg1);
		mEvoAlg1->mNumberOfRequiredParents = 3;
		mEval1 = new EvaluationMethodAdapter("Eval1");
		mWorld1->setEvaluationMethod(mEval1);
		mPop1 = new Population("Pop1");
		mPop2 = new Population("Pop2");
		mWorld1->addPopulation(mPop1);
		mWorld1->addPopulation(mPop2);

		mSelection1 = new SelectionMethodAdapter("Selection1", mPop1);
		mSelection2 = new SelectionMethodAdapter("Selection2", mPop1);
		mSelection3 = new SelectionMethodAdapter("Selection3", mPop2);

		mDestroyedInd1 = false;
		mDestroyedInd2 = false;
		mDestroyedInd3 = false;
		mDestroyedInd5 = false;
		mInd1 = new IndividualAdapter(&mDestroyedInd1);
		mInd2 = new IndividualAdapter(&mDestroyedInd2);
		mInd3 = new IndividualAdapter(&mDestroyedInd3);
		mPop1->getIndividuals().append(mInd1);
		mPop1->getIndividuals().append(mInd2);
		mPop1->getIndividuals().append(mInd3);
		mInd5 = new IndividualAdapter(&mDestroyedInd5);
		mPop2->getIndividuals().append(mInd5);

		//world2
		mWorld2 = new WorldAdapter("World2");
		mEvolutionManager->addEvolutionWorld(mWorld2);
		mEvoAlg2 = new EvolutionAlgorithmAdapter("Evolution2");
		mWorld2->setEvolutionAlgorithm(mEvoAlg2);
		mEvoAlg2->mNumberOfRequiredParents = 2;
		mEval2 = new EvaluationMethodAdapter("Eval2");
		mWorld2->setEvaluationMethod(mEval2);
		mPop3 = new Population("Pop3");
		mWorld2->addPopulation(mPop3);

		mSelection4 = new SelectionMethodAdapter("Selection4", mPop3);

		mDestroyedInd6 = false;
		mInd6 = new IndividualAdapter(&mDestroyedInd6);
		mPop3->getIndividuals().append(mInd6);

		//setting up world1
		mSelection1->getPopulationProportion()->set(0.5);
		mSelection2->getPopulationProportion()->set(1.5);
	
		
	}
	virtual ~TestGenerationProcessingEventListener() {}

	virtual QString getName() const {
		return "TestGenerationProcessingEventListener";
	}

	virtual void eventOccured(Event *event) {
		if(event == 0) {
			QVERIFY(false);
			return;
		}
		else if(event == mTriggerNextGenerationEvent) {
			QCOMPARE(0, mSequenceCounter++);
			QCOMPARE(mEvolutionManager->getCurrentGenerationValue()->get(), 0);
		}
		else if(event == mSelectionStartedEvent) {
			//check
			QCOMPARE(1, mSequenceCounter++);
			QCOMPARE(mEvolutionManager->getCurrentGenerationValue()->get(), 1);

			//prepare
			mPop1->getDesiredPopulationSizeValue()->set(100);
			mPop1->getNumberOfPreservedParentsValue()->set(1);
			mPop2->getDesiredPopulationSizeValue()->set(15);
			mPop2->getNumberOfPreservedParentsValue()->set(2);
			mPop3->getDesiredPopulationSizeValue()->set(20);
			mPop3->getNumberOfPreservedParentsValue()->set(3);
			mInd4 = new IndividualAdapter();
			mSelection1->mSeedListToReturn.append(mInd4);
			mSelection1->mSeedListToReturn.append(mInd2); //take ind2 to next generation.

			QCOMPARE(mPop1->getPopulationSizeValue()->get(), 0);
			QCOMPARE(mPop2->getPopulationSizeValue()->get(), 0);
			QCOMPARE(mPop3->getPopulationSizeValue()->get(), 0);
		}
		else if(event == mSelectionCompletedEvent) {
			//check
			QCOMPARE(2, mSequenceCounter++);
			QCOMPARE(mSelection1->mLastNumberOfIndividuals, 25);
			QCOMPARE(mSelection1->mLastNumberOfPreservedParents, 1);
			QCOMPARE(mSelection1->mLastNumberOfRequiredParents, 3);
			QCOMPARE(mSelection2->mLastNumberOfIndividuals, 75);
			QCOMPARE(mSelection2->mLastNumberOfPreservedParents, 1);
			QCOMPARE(mSelection2->mLastNumberOfRequiredParents, 3);
			QCOMPARE(mSelection3->mLastNumberOfIndividuals, 15);
			QCOMPARE(mSelection3->mLastNumberOfPreservedParents, 2);
			QCOMPARE(mSelection3->mLastNumberOfRequiredParents, 3);
			QCOMPARE(mSelection4->mLastNumberOfIndividuals, 20);
			QCOMPARE(mSelection4->mLastNumberOfPreservedParents, 3);
			QCOMPARE(mSelection4->mLastNumberOfRequiredParents, 2);

			QVERIFY(!mDestroyedInd1 && !mDestroyedInd2 && !mDestroyedInd3);
			QVERIFY(!mDestroyedInd5 && !mDestroyedInd6);
			
			//still only the 3 individuals from the last generation are available.
			QCOMPARE(mPop1->getIndividuals().size(), 3);
		}
		else if(event == mEvolutionAlgorithmStartedEvent) {
			//check
			//now the individuals of the new generation are available
			QCOMPARE(mPop1->getIndividuals().size(), 100);
			QVERIFY(mPop1->getIndividuals().contains(mInd2));
			QVERIFY(mPop1->getIndividuals().contains(mInd4));
			QCOMPARE(mPop2->getIndividuals().size(), 15);
			QCOMPARE(mPop3->getIndividuals().size(), 20);

			QCOMPARE(mPop1->getPopulationSizeValue()->get(), 100);
			QCOMPARE(mPop2->getPopulationSizeValue()->get(), 15);
			QCOMPARE(mPop3->getPopulationSizeValue()->get(), 20);


			QCOMPARE(3, mSequenceCounter++);
			QCOMPARE(mEvoAlg1->mCreateGenerationCounter, 0);
			QCOMPARE(mEvoAlg2->mCreateGenerationCounter, 0);
		}
		else if(event == mEvolutionAlgorithmCompletedEvent) {
			//check
			QCOMPARE(4, mSequenceCounter++);
			QCOMPARE(mEvoAlg1->mCreateGenerationCounter, 1);
			QCOMPARE(mEvoAlg2->mCreateGenerationCounter, 1);
			
			//parent objects are still not destroyed!
			QVERIFY(!mDestroyedInd1 && !mDestroyedInd2 && !mDestroyedInd3);
			QVERIFY(!mDestroyedInd5 && !mDestroyedInd6);
		}
		else if(event == mEvaluationStartedEvent) {
			//check
			QCOMPARE(5, mSequenceCounter++);

			//now all previous indiviudals, that are not part of a population any more,
			//are destroyed.
			QVERIFY(mDestroyedInd1 && mDestroyedInd3 && mDestroyedInd5 && mDestroyedInd6);
			//mInd2 was not destroyed because it is part of population mPop1.
			QVERIFY(!mDestroyedInd2);

			QCOMPARE(mEval1->mEvaluateCounter, 0);
			QCOMPARE(mEval2->mEvaluateCounter, 0);
		}
		else if(event == mEvaluationCompletedEvent) {
			//check
			QCOMPARE(6, mSequenceCounter++);
			QCOMPARE(mEval1->mEvaluateCounter, 1);
			QCOMPARE(mEval2->mEvaluateCounter, 1);
		}
		else if(event == mNextGenerationCompletedEvent) {
			//check
			QCOMPARE(7, mSequenceCounter++);
		}
		else {
			QVERIFY(false);
		}
	}

	void checkAfterResetState() {
		QCOMPARE(mEval1->mResetCounter, 1);
		QCOMPARE(mEval2->mResetCounter, 1);
		QCOMPARE(mEvoAlg1->mResetCounter, 1);
		QCOMPARE(mEvoAlg2->mResetCounter, 1);
	}

	public:
		int mSequenceCounter;
		EvolutionManager *mEvolutionManager;
		Event *mTriggerNextGenerationEvent;
		Event *mNextGenerationCompletedEvent;
		Event *mEvaluationStartedEvent;
		Event *mEvaluationCompletedEvent;
		Event *mSelectionStartedEvent;
		Event *mSelectionCompletedEvent;
		Event *mEvolutionAlgorithmStartedEvent;
		Event *mEvolutionAlgorithmCompletedEvent;

		WorldAdapter *mWorld1;
		WorldAdapter *mWorld2;
		EvolutionAlgorithmAdapter *mEvoAlg1;
		EvolutionAlgorithmAdapter *mEvoAlg2;
		EvaluationMethodAdapter *mEval1;
		EvaluationMethodAdapter *mEval2;
		SelectionMethodAdapter *mSelection1;
		SelectionMethodAdapter *mSelection2;
		SelectionMethodAdapter *mSelection3;
		SelectionMethodAdapter *mSelection4;
		Population *mPop1;
		Population *mPop2;
		Population *mPop3;
		IndividualAdapter *mInd1;
		IndividualAdapter *mInd2;
		IndividualAdapter *mInd3;
		IndividualAdapter *mInd4;
		IndividualAdapter *mInd5;
		IndividualAdapter *mInd6;
		bool mDestroyedInd1;
		bool mDestroyedInd2;
		bool mDestroyedInd3;
		bool mDestroyedInd5;
		bool mDestroyedInd6;
};

//chris
void TestEvolutionManager::testGenerationProcessing() {
	Core::resetCore();

	EvolutionManager *em = new EvolutionManager();

	TestGenerationProcessingEventListener testListener(em);

	QVERIFY(em->init());
	QVERIFY(em->getNextGenerationTriggerEvent() != 0);
	em->getNextGenerationTriggerEvent()->trigger();

	QCOMPARE(testListener.mSequenceCounter, 8);

	em->restartEvolution();
	testListener.checkAfterResetState();
	
	delete em;

	Core::resetCore();
}

}

