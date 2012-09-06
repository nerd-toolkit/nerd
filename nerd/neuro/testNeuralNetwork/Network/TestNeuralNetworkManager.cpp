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




#include "TestNeuralNetworkManager.h"
#include <iostream>
#include "Network/NeuralNetworkManager.h"
#include "Network/NeuralNetworkAdapter.h"
#include <QList>
#include "Core/Core.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "Event/EventManager.h"
#include "Event/Event.h"
#include "NeuralNetworkConstants.h"
#include "NerdConstants.h"
#include "Event/EventListenerAdapter.h"
#include "Event/EventManager.h"
#include "Network/Neuro.h"

using namespace std;
using namespace nerd;

void TestNeuralNetworkManager::initTestCase() {
}

void TestNeuralNetworkManager::cleanUpTestCase() {
}


//Chris
void TestNeuralNetworkManager::testConstruction() {
	Core::resetCore();

	NeuralNetworkManager nnm1;

	//NeuralNetworkManager does NOT automatically register at the Core as global object.
	QVERIFY(!Core::getInstance()->getGlobalObjects().contains(&nnm1));
	
	QVERIFY(nnm1.getName() == "NeuralNetworkManager");
	QVERIFY(nnm1.getTransferFunctionPrototypes().size() == 0);
	QVERIFY(nnm1.getActivationFunctionPrototypes().size() == 0);
	QVERIFY(nnm1.getSynapseFunctionPrototypes().size() == 0);


}


//Chris
void TestNeuralNetworkManager::testPrototypes() {
	Core::resetCore();

	TransferFunctionAdapter tfa1("F-1", 0.1, 1.0);
	TransferFunctionAdapter tfa2("F-2", -1.0, 1.0);
	TransferFunctionAdapter tfa3("F-1", 0.5, 1.0);

	ActivationFunctionAdapter afa1("F-1");
	ActivationFunctionAdapter afa2("F-2");
	ActivationFunctionAdapter afa3("F-1");

	SynapseFunctionAdapter sfa1("F-1");
	SynapseFunctionAdapter sfa2("F-2");
	SynapseFunctionAdapter sfa3("F-1");

	//used for destruction proof.
	TransferFunctionAdapter *tap1 = 0;
	ActivationFunctionAdapter *aap1 = 0;
	SynapseFunctionAdapter *sap1 = 0;
	bool destroyedTransferFunctions = false;
	bool destroyedActivationFunctions = false;
	bool destroyedSynapseFunctions = false;
	
	{
		NeuralNetworkManager nn1;
	
		QVERIFY(nn1.getTransferFunctionPrototypes().size() == 0);
		QVERIFY(nn1.getActivationFunctionPrototypes().size() == 0);
		QVERIFY(nn1.getSynapseFunctionPrototypes().size() == 0);
	
		//*********************
		//TransferFunctions
	
		//add a transferfunction prototype
		QVERIFY(nn1.addTransferFunctionPrototype(tfa1) == true);
		
		QVERIFY(nn1.getTransferFunctionPrototypes().size() == 1);
		QVERIFY(nn1.getActivationFunctionPrototypes().size() == 0);
		QVERIFY(nn1.getSynapseFunctionPrototypes().size() == 0);
	
		QVERIFY(nn1.getTransferFunctionPrototypes().at(0) != &tfa1); //is a copy
		QVERIFY(nn1.getTransferFunctionPrototypes().at(0)->getName() == "F-1");
		QVERIFY(nn1.getTransferFunctionPrototypes().at(0)->getLowerBound() == 0.1);
	
		//add the same transferfunction or a transferfunction with the same name 
		//(both fail because of a name conflict, each prototype has to have an own name).
		QVERIFY(nn1.addTransferFunctionPrototype(tfa1) == false);
		QVERIFY(nn1.addTransferFunctionPrototype(tfa3) == false); //same name F-1
		
		QVERIFY(nn1.getTransferFunctionPrototypes().size() == 1);
		QVERIFY(nn1.getActivationFunctionPrototypes().size() == 0);
		QVERIFY(nn1.getSynapseFunctionPrototypes().size() == 0);
	
		//add a transferfunction prototype with a different name (works)
		QVERIFY(nn1.addTransferFunctionPrototype(tfa2));
		QVERIFY(nn1.getTransferFunctionPrototypes().size() == 2);
		
	
		//*********************
		//ActivationFunctions
		
		QVERIFY(nn1.getTransferFunctionPrototypes().size() == 2);
		QVERIFY(nn1.getActivationFunctionPrototypes().size() == 0);
		QVERIFY(nn1.getSynapseFunctionPrototypes().size() == 0);
	
		//add an ActivationFunctionPrototype 
		//(works, even if the name is the same as one of the TransferFunctions).
		QVERIFY(nn1.addActivationFunctionPrototype(afa1) == true);
	
		QVERIFY(nn1.getTransferFunctionPrototypes().size() == 2);
		QVERIFY(nn1.getActivationFunctionPrototypes().size() == 1);
		QVERIFY(nn1.getSynapseFunctionPrototypes().size() == 0);
	
		QVERIFY(nn1.getActivationFunctionPrototypes().at(0) != &afa1); //is a copy
		QVERIFY(nn1.getActivationFunctionPrototypes().at(0)->getName() == "F-1");
	
		//add the same ActivationFunction or one with the same name (both fails)
		QVERIFY(nn1.addActivationFunctionPrototype(afa1) == false);
		QVERIFY(nn1.addActivationFunctionPrototype(afa3) == false);
		QVERIFY(nn1.getActivationFunctionPrototypes().size() == 1);
	
		//add an ActivationFunction with another name
		QVERIFY(nn1.addActivationFunctionPrototype(afa2) == true);
		QVERIFY(nn1.getActivationFunctionPrototypes().size() == 2);
	
	
		//*********************
		//SynapseFunctions
		
		QVERIFY(nn1.getTransferFunctionPrototypes().size() == 2);
		QVERIFY(nn1.getActivationFunctionPrototypes().size() == 2);
		QVERIFY(nn1.getSynapseFunctionPrototypes().size() == 0);
	
		//add a SynapseFunctionPrototype 
		//(works, even if the name is the same as one of the ActivationFunctions).
		QVERIFY(nn1.addSynapseFunctionPrototype(sfa1) == true);
	
		QVERIFY(nn1.getTransferFunctionPrototypes().size() == 2);
		QVERIFY(nn1.getActivationFunctionPrototypes().size() == 2);
		QVERIFY(nn1.getSynapseFunctionPrototypes().size() == 1);
	
		QVERIFY(nn1.getSynapseFunctionPrototypes().at(0) != &sfa1); //is a copy
		QVERIFY(nn1.getSynapseFunctionPrototypes().at(0)->getName() == "F-1");
	
		//add the same ActivationFunction or one with the same name (both fails)
		QVERIFY(nn1.addSynapseFunctionPrototype(sfa1) == false);
		QVERIFY(nn1.addSynapseFunctionPrototype(sfa3) == false);
		QVERIFY(nn1.getSynapseFunctionPrototypes().size() == 1);
	
		//add an ActivationFunction with another name
		QVERIFY(nn1.addSynapseFunctionPrototype(sfa2) == true);
		QVERIFY(nn1.getSynapseFunctionPrototypes().size() == 2);


		aap1 = dynamic_cast<ActivationFunctionAdapter*>(
					nn1.getActivationFunctionPrototypes().at(0));

		tap1 = dynamic_cast<TransferFunctionAdapter*>(
					nn1.getTransferFunctionPrototypes().at(0));

		sap1 = dynamic_cast<SynapseFunctionAdapter*>(
					nn1.getSynapseFunctionPrototypes().at(0));

		QVERIFY(aap1 != 0);
		QVERIFY(tap1 != 0);
		QVERIFY(sap1 != 0);

		aap1->mDeletedFlag = &destroyedActivationFunctions;
		tap1->mDeletedFlag = &destroyedTransferFunctions;
		sap1->mDeletedFlag = &destroyedSynapseFunctions;

	}

	//all prototypes are destroyed with the NeuralNetworkManager (here only 1 sample per type)
	QVERIFY(destroyedTransferFunctions == true);
	QVERIFY(destroyedActivationFunctions == true);
	QVERIFY(destroyedSynapseFunctions == true);


}


//Chris
void TestNeuralNetworkManager::testAddAndRemoveNeuralNetworks() {
	Core::resetCore();

	NeuralNetworkAdapter *nn1 = new NeuralNetworkAdapter(ActivationFunctionAdapter(""),
			TransferFunctionAdapter("", 0.1, 0.9), SynapseFunctionAdapter(""));
	
	NeuralNetworkAdapter *nn2 = dynamic_cast<NeuralNetworkAdapter*>(nn1->createCopy());

	bool destroyedN1 = false;
	bool destroyedN2 = false;
	nn1->mDestroyedFlag = &destroyedN1;
	nn2->mDestroyedFlag = &destroyedN2;

	{
		NeuralNetworkManager nm;
		QVERIFY(nm.getNeuralNetworks().size() == 0);
	
		//add the null pointer
		QVERIFY(nm.addNeuralNetwork(0) == false);
		QVERIFY(nm.getNeuralNetworks().size() == 0);
	
		//add a neural network
		QVERIFY(nm.addNeuralNetwork(nn1) == true);
		QVERIFY(nm.getNeuralNetworks().size() == 1);
		QVERIFY(nm.getNeuralNetworks().contains(nn1));
	
		//add the same network again (fails)
		QVERIFY(nm.addNeuralNetwork(nn1) == false);
		QVERIFY(nm.getNeuralNetworks().size() == 1);
	
		//add a second network
		QVERIFY(nm.addNeuralNetwork(nn2) == true);
		QVERIFY(nm.getNeuralNetworks().size() == 2);
		QVERIFY(nm.getNeuralNetworks().contains(nn1));
		QVERIFY(nm.getNeuralNetworks().contains(nn2));

		//remove the null pointer
		QVERIFY(nm.removeNeuralNetwork(0) == false);
		QVERIFY(nm.getNeuralNetworks().size() == 2);

		//remove network1
		QVERIFY(nm.removeNeuralNetwork(nn1) == true);
		QVERIFY(nm.getNeuralNetworks().size() == 1);
		QVERIFY(nm.getNeuralNetworks().contains(nn2));

		//destroy all neural networks
		nm.destroyNeuralNetworks();
		QVERIFY(nm.getNeuralNetworks().size() == 0);
		QVERIFY(destroyedN2 == true); //network2 was destroyed
		QVERIFY(destroyedN1 == false); //network2 was NOT registered at the NeuralNetworkManager.

		//add network1 again
		QVERIFY(nm.addNeuralNetwork(nn1) == true);
		QVERIFY(nm.getNeuralNetworks().size() == 1);
		QVERIFY(nm.getNeuralNetworks().contains(nn1));

	}

	//network1 was still registered when the NeuralNetworkManager was destroyed (so it is, too).
	QVERIFY(destroyedN1 == true);


}



//Chris
void TestNeuralNetworkManager::testEvents() {
	Core::resetCore();

	EventManager *em = Core::getInstance()->getEventManager();
	int numberOfEvents = em->getEvents().size();

	QVERIFY(em->getEvent(NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED) == 0);
	QVERIFY(em->getEvent(NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_STARTED) == 0);
	QVERIFY(em->getEvent(NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_COMPLETED) == 0);
	QVERIFY(em->getEvent(NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED) == 0);
	QVERIFY(em->getEvent(NeuralNetworkConstants::EVENT_NNM_NETWORK_ITERATION_COMPLETED) == 0);
	QVERIFY(em->getEvent(NeuralNetworkConstants::EVENT_NNM_RESET_NETWORKS) == 0);

	NeuralNetworkManager *nnm = new NeuralNetworkManager();

	QVERIFY(em->getEvents().size() == numberOfEvents + 6);

	Event *replacedEvent = 
		em->getEvent(NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED);
	Event *executionStarted = 
		em->getEvent(NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_STARTED);
	Event *executionCompleted =
		em->getEvent(NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_COMPLETED);
	Event *structuresChanged =
		em->getEvent(NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED);
	Event *networkIteration =
		em->getEvent(NeuralNetworkConstants::EVENT_NNM_NETWORK_ITERATION_COMPLETED);
	Event *resetNetworks =
	em->getEvent(NeuralNetworkConstants::EVENT_NNM_RESET_NETWORKS);

	QVERIFY(replacedEvent != 0);
	QVERIFY(executionStarted != 0);
	QVERIFY(executionCompleted != 0);
	QVERIFY(structuresChanged != 0);
	QVERIFY(networkIteration != 0);
	QVERIFY(resetNetworks != 0);

	//initialization fails, because NeuralNetworkManager was not registered as global object.
	QVERIFY(nnm->init() == false);

	//initialization works, because all Events could be created.
	int numberOfGlobalObjects = Core::getInstance()->getGlobalObjects().size();
	QVERIFY(nnm->registerAsGlobalObject() == true);
	QVERIFY(nnm->registerAsGlobalObject() == true); //second time registration also returns true
	QVERIFY(Core::getInstance()->getGlobalObjects().size() == numberOfGlobalObjects + 1);

	QVERIFY(nnm->init() == true);

	//a second NeuralNetworkManager will not initialze successful, as the required Events
	//can not be created and a registration as global object is not possible.
	NeuralNetworkManager *nnm2 = new NeuralNetworkManager();
	QVERIFY(em->getEvents().size() == numberOfEvents + 6); //no additional Events created.
	QVERIFY(nnm2->registerAsGlobalObject() == false);
	QVERIFY(nnm2->init() == false);


	//remove old network manager
	QVERIFY(Core::getInstance()->removeGlobalObject(
		NeuralNetworkConstants::OBJECT_NEURAL_NETWORK_MANAGER) == nnm); 
	QVERIFY(nnm2->registerAsGlobalObject() == true);
	 //still initialization fails, because of missing Events.
	QVERIFY(nnm2->init() == false);
	
	//nnm and nnm2 will be destroyed by the Core

}


//Chris
void TestNeuralNetworkManager::testBehavior() {
	Core::resetCore();

	EventManager *em = Core::getInstance()->getEventManager();

	em->createEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP, "");
	em->createEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, "");

	NeuralNetworkAdapter *nn1 = new NeuralNetworkAdapter(ActivationFunctionAdapter(""),
			TransferFunctionAdapter("", 0.1, 0.9), SynapseFunctionAdapter(""));
	NeuralNetworkAdapter *nn2 = new NeuralNetworkAdapter(ActivationFunctionAdapter(""),
			TransferFunctionAdapter("", 0.2, 0.8), SynapseFunctionAdapter(""));
	
	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();
	nnm->registerAsGlobalObject();
	
	//add some networks
	QVERIFY(nnm->addNeuralNetwork(nn1) == true);
	QVERIFY(nnm->addNeuralNetwork(nn2) == true);
	

	EventListenerAdapter networkReplacedListener("NetworkReplacedListener");
	EventListenerAdapter networkStructureListener("NetworkStructureListener");
	EventListenerAdapter executionListener("ExecutionListener");


	Event *netReplacedEvent = em->registerForEvent(
			NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED, 
			&networkReplacedListener);
	Event *execStartEvent = em->registerForEvent(
			NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_STARTED, 
			&executionListener);
	Event *execStopEvent = em->registerForEvent(
			NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_COMPLETED, 
			&executionListener);
	Event *netStructureEvent = em->registerForEvent(
			NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED,
			&networkStructureListener);

	Event *nextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	Event *nextStepCompleted = em->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);
	Event *resetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET);

	QVERIFY(netReplacedEvent != 0);
	QVERIFY(execStartEvent != 0);
	QVERIFY(execStopEvent != 0);
	QVERIFY(netStructureEvent != 0);
	QVERIFY(nextStepEvent != 0);
	QVERIFY(nextStepCompleted != 0);
	QVERIFY(resetEvent == 0);

	int numberOfNextStepUpstreamEvents = nextStepEvent->getUpstreamEvents().size();

	QVERIFY(Core::getInstance()->init() == true);

	resetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET);

	QVERIFY(resetEvent != 0);

	//execStartEvent is registered as upstream event for nextStepEvent.
	QVERIFY(nextStepEvent->getUpstreamEvents().size() == numberOfNextStepUpstreamEvents + 1);
	QVERIFY(nextStepEvent->getUpstreamEvents().contains(execStartEvent) == true);

	//trigger network replaced
	QVERIFY(networkReplacedListener.mEventOccuredCounter == 0);
	nnm->triggerCurrentNetworksReplacedEvent();
	QVERIFY(networkReplacedListener.mEventOccuredCounter == 1);
	QVERIFY(networkReplacedListener.mLastTriggeredEvent == netReplacedEvent);

	//trigger structure changed
	QVERIFY(networkStructureListener.mEventOccuredCounter == 0);
	nnm->triggerNetworkStructureChangedEvent();
	QVERIFY(networkStructureListener.mEventOccuredCounter == 1);
	QVERIFY(networkStructureListener.mLastTriggeredEvent == netStructureEvent);
	
	//trigger next step event
	QVERIFY(executionListener.mEventOccuredCounter == 0);
	QVERIFY(nn1->mExecuteStepCounter == 0);
	QVERIFY(nn2->mExecuteStepCounter == 0);
	nextStepEvent->trigger();
	QCOMPARE(executionListener.mEventOccuredCounter, 2); //upsteam event triggered.
	QVERIFY(nn1->mExecuteStepCounter == 1); //networks are executed
	QVERIFY(nn2->mExecuteStepCounter == 1);
	
	//trigger reset event
	QVERIFY(nn1->mResetCounter == 0);
	QVERIFY(nn2->mResetCounter == 0);
	resetEvent->trigger();
	QVERIFY(nn1->mResetCounter == 1); //networks are reset.
	QVERIFY(nn2->mResetCounter == 1);

	Core::resetCore();

}





