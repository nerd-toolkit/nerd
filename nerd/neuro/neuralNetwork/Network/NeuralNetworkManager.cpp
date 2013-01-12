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

#include "NeuralNetworkManager.h"
#include "Core/Core.h"
#include "Event/EventManager.h"
#include <QListIterator>
#include "NeuralNetworkConstants.h"
#include "NerdConstants.h"
#include <iostream>
#include "Util/Tracer.h"
#include <QMutexLocker>
#include <QThread>
#include <QCoreApplication>
#include "Event/TriggerEventTask.h"
#include "Network/NeuroTagManager.h"
#include "NeuralNetworkConstants.h"
#include "Network/Neuro.h"


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {

NeuralNetworkManager::NeuralNetworkManager()
	: mStepStartedEvent(0), mStepCompletedEvent(0), mResetEvent(0), mResetNetworksEvent(0),
 	  mCurrentNetworksReplacedEvent(0), mNetworkEvaluationStarted(0), 
	  mNetworkEvaluationCompleted(0), mNetworkStructuresChanged(0), 
	  mNetworkIterationCompleted(0),
	  mNetworkExecutionMutex(QMutex::Recursive), mBypassNetworkValue(0)
{
	EventManager *em = Core::getInstance()->getEventManager();
	
	mCurrentNetworksReplacedEvent = em->createEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED,
				"This Event is triggered whenever at least one of the controlling "
				"NeuralNetworks was replaced by another one, thus when an agent is controlled "
				"by a different neuro-controller.");
	mNetworkEvaluationStarted = em->createEvent(
				NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_STARTED,
				"This Event is triggered before the network execution starts. When triggered, "
				"all currently active NeuralNetworks are updated and calculate their next state. ");
	mNetworkEvaluationCompleted = em->createEvent(
				NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_COMPLETED,
				"This Event is triggered when all active NeuralNetworks have been updated to "
				"the next state. Hereafter the controlled SimObjectGroups' InputValues have "
				"been updated with the control activations of the Networks' output neurons.");
	mNetworkStructuresChanged = em->createEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED,
				"This Event is triggered whenever at least on of the active NeuralNetworks "
				"was modified in structure or relevant parameters.");
	mNetworkParametersChanged = em->createEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_PARAMETERS_CHANGED,
				"This Event is triggered, when parameters (not the structure) of the network changed. "
				"This event is called as upstream event of the NetworkStructureChanged event when that "
				"event is triggered.");
	mNetworkStructuresChanged->addUpstreamEvent(mNetworkParametersChanged);

	mNetworkIterationCompleted = em->createEvent(
				NeuralNetworkConstants::EVENT_NNM_NETWORK_ITERATION_COMPLETED,
				"This Event is triggered after each single network iteration, which can occure "
				"multiple times during a network update.");
	
	mResetNetworksEvent = em->createEvent(
				NeuralNetworkConstants::EVENT_NNM_RESET_NETWORKS,
				"When this event is triggered, then the networks are reset "
				"(independently of a simulation or system reset)");
	

	//this value can be switched to disable the neural network control.
	mBypassNetworkValue = new BoolValue(false);
	mBypassNetworkValue->addValueChangedListener(this);
	mBypassNetworkValue->setDescription("If true, then the networks are executed each step, but the output of the "
										"output neurons is NOT applied to the motors, so the network has no effect"
										"on the controlled animat.");
	Core::getInstance()->getValueManager()->addValue(
				NeuralNetworkConstants::VALUE_NNM_BYPASS_NETWORKS, mBypassNetworkValue);
	
	mNumberOfNetworkUpdatesPerStep = new IntValue(1);
	mNumberOfNetworkUpdatesPerStep->setDescription("Number of network update steps during each network update. "
												   "This allows networks running faster than the physical simulation.");
	Core::getInstance()->getValueManager()->addValue(
				NeuralNetworkConstants::VALUE_NNM_NUMBER_OF_ITERATIONS_PER_STEP, mNumberOfNetworkUpdatesPerStep);
	
	mDisablePlasticity = new BoolValue(false);
	mDisablePlasticity->setDescription("Gives a hint to all plastic synapse and activation functions to stop plasticity.\n"
										"Note: There is no guarantee that there are no violations of that hint!");
	Core::getInstance()->getValueManager()->addValue(
				NeuralNetworkConstants::VALUE_DISABLE_NEURAL_PLASTICITY, mDisablePlasticity);
	
	mDisableNetworkUpdate = new BoolValue(false);
	mDisableNetworkUpdate->setDescription("Disables the entire network update of all networks.");
	Core::getInstance()->getValueManager()->addValue(
				NeuralNetworkConstants::VALUE_DISABLE_NETWORK_UPDATE, mDisableNetworkUpdate);
	
	//add default tags
	NeuroTagManager *ntm = NeuroTagManager::getInstance();
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_FLIP_NEURON_ACTIVITY, 
						 NeuralNetworkConstants::TAG_TYPE_NEURON, 
						 "Flips the activity of the neuron (interfaces only)"));
	ntm->addTag(NeuroTag(Neuron::NEURON_TYPE_INPUT, 
						 NeuralNetworkConstants::TAG_TYPE_NEURON, 
						 "Indicates that a neuron is used as input neuron."));
	ntm->addTag(NeuroTag(Neuron::NEURON_TYPE_OUTPUT, 
						 NeuralNetworkConstants::TAG_TYPE_NEURON,
						 "Indicates that a neuron is used as output neuron."));
						 
	//BDN specific //TODO (may be moved into a collection ?
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NETWORK_BDN_FADE_IN_RATE, 
						 NeuralNetworkConstants::TAG_TYPE_NETWORK,
						 "Gives the fade in rate when an exported BDN network is started [0,1]"));
						 
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NEURON_BDN_INPUT, 
						 NeuralNetworkConstants::TAG_TYPE_NEURON,
						 "Creates an additional input in the super module for exported BDN projects"));
	
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NEURON_BDN_OUTPUT, 
						 NeuralNetworkConstants::TAG_TYPE_NEURON,
						 "Creates an additional output in the super module for exported BDN projects"));
						 
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NEURON_BDN_BOARD_INTERFACE,
						NeuralNetworkConstants::TAG_TYPE_NEURON,
						"Splits the neuron into two new neurons and creates a spinal cord interface.\n"
						"The tag requires a number that specifies the spinal cord adress to be used."));
}

/**
 * Destructor.
 * Destroys all prototypes and all currently registered NeuralNetwork objects.
 */
NeuralNetworkManager::~NeuralNetworkManager() {
	TRACE("NeuralNetworkManager::~NeuralNetworkManager");

	//do not destroy mBypassNetworkValue (is destroyed by ValueManager.
	
	destroyNeuralNetworks();
	while(!mTransferFunctionPrototypes.empty()) {
		TransferFunction *tf = mTransferFunctionPrototypes.at(0);
		mTransferFunctionPrototypes.removeAll(tf);
		delete tf;
	}
	while(!mActivationFunctionPrototypes.empty()) {
		ActivationFunction *af = mActivationFunctionPrototypes.at(0);
		mActivationFunctionPrototypes.removeAll(af);
		delete af;
	}
	while(!mSynapseFunctionPrototypes.empty()) {
		SynapseFunction *sf = mSynapseFunctionPrototypes.at(0);
		mSynapseFunctionPrototypes.removeAll(sf);
		delete sf;
	}

	//clear helper class.
	Neuro::reset();
}

QString NeuralNetworkManager::getName() const {
	return "NeuralNetworkManager";
}



bool NeuralNetworkManager::registerAsGlobalObject() {
	TRACE("NeuralNetworkManager::registerAsGlobalObject");

	Core::getInstance()->addGlobalObject(
		NeuralNetworkConstants::OBJECT_NEURAL_NETWORK_MANAGER, this);

	return Core::getInstance()->getGlobalObject(
		NeuralNetworkConstants::OBJECT_NEURAL_NETWORK_MANAGER) == this;
}

bool NeuralNetworkManager::init() {
	TRACE("NeuralNetworkManager::init");

	bool ok = true;
	if(mCurrentNetworksReplacedEvent == 0) {
		Core::log(QString("NeuralNetworkManager: Could not create Event [")
				.append(NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED)
				.append("]! [TERMINATING]"));
		ok = false;
	}
	if(mNetworkEvaluationStarted == 0) {
		Core::log(QString("NeuralNetworkManager: Could not create Event [")
				.append(NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_STARTED)
				.append("]! [TERMINATING]"));
		ok = false;
	}
	else {
		//register as listener to react on this event.
		mNetworkEvaluationStarted->addEventListener(this);
	}
	if(mNetworkEvaluationCompleted == 0) {
		Core::log(QString("NeuralNetworkManager: Could not create Event [")
				.append(NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_COMPLETED)
				.append("]! [TERMINATING]"));
		ok = false;
	}
	if(mNetworkStructuresChanged == 0) {
		Core::log(QString("NeuralNetworkManager: Could not create Event [")
				.append(NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED)
				.append("]! [TERMINATING]"));
		ok = false;
	}
	if(mResetNetworksEvent == 0) {
		Core::log(QString("NeuralNetworkManager: Could not create Event [")
				.append(NeuralNetworkConstants::EVENT_NNM_RESET_NETWORKS)
				.append("]! [TERMINATING]"));
		ok = false;
	}
	else {
		mResetNetworksEvent->addEventListener(this);
	}
	if(Core::getInstance()->getGlobalObject(
			NeuralNetworkConstants::OBJECT_NEURAL_NETWORK_MANAGER) != this) 
	{
		Core::log("NeuralNetworkManager: Was not registered as global object at Core! "
				"[TERMINATING]");
		ok = false;
	}
	return ok;
}

bool NeuralNetworkManager::bind() {
	TRACE("NeuralNetworkManager::bind");

	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();
	mStepStartedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP, true);
	mStepCompletedEvent = em->getEvent(
			NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, true);
	mResetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET, true);

	if(mStepStartedEvent == 0) {
		Core::log(QString("NeuralNetworkManager: Could not find Event [")
				.append(NerdConstants::EVENT_EXECUTION_NEXT_STEP)
				.append("]! [TERMINATING]"));
		ok = false;
	}
	else {
		mStepStartedEvent->addUpstreamEvent(mNetworkEvaluationStarted);
	}
	if(mStepCompletedEvent == 0 || !mStepCompletedEvent->addEventListener(this)) {
		Core::log(QString("NeuralNetworkManager: Could not find Event [")
				.append(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED)
				.append("]! [TERMINATING]"));
		ok = false;
	}
	if(mResetEvent == 0 || !mResetEvent->addEventListener(this)) {
		Core::log(QString("NeuralNetworkManager: Could not find Event [")
				.append(NerdConstants::EVENT_EXECUTION_RESET)
				.append("]! [TERMINATING]"));
		ok = false;
	}

	return ok;
}


bool NeuralNetworkManager::cleanUp() {
	return true;
}


void NeuralNetworkManager::eventOccured(Event *event) {
	TRACE("NeuralNetworkManager::eventOccured");

	if(event == 0) {
		return;
	}
	else if(Core::getInstance()->isShuttingDown()) {
		return;
	}
	else if(event == mNetworkEvaluationStarted) {
		executeNeuralNetworks();
	}
	else if(event == mResetEvent || event == mResetNetworksEvent) {
		resetNeuralNetworks();
	}
}


void NeuralNetworkManager::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	if(value == mBypassNetworkValue) {
		QMutexLocker locker(&mNetworkExecutionMutex);

		for(QListIterator<NeuralNetwork*> i(mNeuralNetworks); i.hasNext();) {
			NeuralNetwork *net = i.next();
			net->bypassNetwork(mBypassNetworkValue->get());
		}
	}
}


bool NeuralNetworkManager::addNeuralNetwork(NeuralNetwork *neuralNetwork) {
	TRACE("NeuralNetworkManager::addNeuralNetwork");

	QMutexLocker locker(&mNetworkExecutionMutex);

	if(neuralNetwork == 0 || mNeuralNetworks.contains(neuralNetwork)) {
		return false;
	}
	mNeuralNetworks.append(neuralNetwork);
	return true;
}


bool NeuralNetworkManager::removeNeuralNetwork(NeuralNetwork *neuralNetwork) {
	TRACE("NeuralNetworkManager::removeNeuralNetwork");

	QMutexLocker locker(&mNetworkExecutionMutex);

	if(neuralNetwork == 0 || !mNeuralNetworks.contains(neuralNetwork)) {
		return false;
	}
	mNeuralNetworks.removeAll(neuralNetwork);
	return true;
}


QList<NeuralNetwork*> NeuralNetworkManager::getNeuralNetworks() const {
	return mNeuralNetworks;
}


void NeuralNetworkManager::destroyNeuralNetworks() {
	TRACE("NeuralNetworkManager::destroyNeuralNetworks");

	QMutexLocker locker(&mNetworkExecutionMutex);

	QList<NeuralNetwork*> networks = mNeuralNetworks;
	mNeuralNetworks.clear();

	if(mCurrentNetworksReplacedEvent != 0) {
		mCurrentNetworksReplacedEvent->trigger();
	}

	while(!networks.empty()) {
		NeuralNetwork *net = networks.at(0);
		networks.removeAll(net);
		delete net;
	}
}


void NeuralNetworkManager::triggerCurrentNetworksReplacedEvent() {
	TRACE("NeuralNetworkManager::triggerCurrentNetworksReplacedEvent");

	if(mCurrentNetworksReplacedEvent != 0) {

		if(Core::getInstance()->isMainExecutionThread()) {
			mCurrentNetworksReplacedEvent->trigger();
		}
		else {
			Core::getInstance()->scheduleTask(new TriggerEventTask(mCurrentNetworksReplacedEvent));
		}
	}
}

void NeuralNetworkManager::triggerNetworkStructureChangedEvent() {
	TRACE("NeuralNetworkManager::triggerNetworkStructureChangedEvent");

	if(mNetworkStructuresChanged != 0) {
		if(Core::getInstance()->isMainExecutionThread()) {
			mNetworkStructuresChanged->trigger();
		}
		else {
			Core::getInstance()->scheduleTask(new TriggerEventTask(mNetworkStructuresChanged));
		}
	}
}

void NeuralNetworkManager::triggerNetworkParametersChangedEvent() {
	if(mNetworkStructuresChanged != 0) {
		if(Core::getInstance()->isMainExecutionThread()) {
			mNetworkParametersChanged->trigger();
		}
		else {
			Core::getInstance()->scheduleTask(new TriggerEventTask(mNetworkParametersChanged));
		}
	}
}

void NeuralNetworkManager::triggerNetworkIterationCompleted() {
	if(Core::getInstance()->isMainExecutionThread()) {
		if(mNetworkIterationCompleted != 0) {
			mNetworkIterationCompleted->trigger();
		}
	}
}


bool NeuralNetworkManager::addTransferFunctionPrototype(const TransferFunction &tf) {
	TRACE("NeuralNetworkManager::addTransferFunctionPrototype");

	for(QListIterator<TransferFunction*> i(mTransferFunctionPrototypes); i.hasNext();) {
		if(i.next()->getName() == tf.getName()) {
			return false;
		}
	}
	mTransferFunctionPrototypes.append(tf.createCopy());
	return true;
}


bool NeuralNetworkManager::addActivationFunctionPrototype(const ActivationFunction &af) {
	TRACE("NeuralNetworkManager::addActivationFunctionPrototype");

	for(QListIterator<ActivationFunction*> i(mActivationFunctionPrototypes); i.hasNext();) {
		if(i.next()->getName() == af.getName()) {
			return false;
		}
	}
	mActivationFunctionPrototypes.append(af.createCopy());
	return true;
}


bool NeuralNetworkManager::addSynapseFunctionPrototype(const SynapseFunction &sf) {
	TRACE("NeuralNetworkManager::addSynapseFunctionPrototype");

	for(QListIterator<SynapseFunction*> i(mSynapseFunctionPrototypes); i.hasNext();) {
		if(i.next()->getName() == sf.getName()) {
			return false;
		}
	}
	mSynapseFunctionPrototypes.append(sf.createCopy());
	return true;
}

QList<TransferFunction*> NeuralNetworkManager::getTransferFunctionPrototypes() const {
	TRACE("NeuralNetworkManager::getTransferFunctionPrototypes");

	return mTransferFunctionPrototypes;
}


QList<ActivationFunction*> NeuralNetworkManager::getActivationFunctionPrototypes() const {
	return mActivationFunctionPrototypes;
}


QList<SynapseFunction*> NeuralNetworkManager::getSynapseFunctionPrototypes() const {
	return mSynapseFunctionPrototypes;
}


BoolValue* NeuralNetworkManager::getBypassNetworksValue() const {
	return mBypassNetworkValue;
}

BoolValue* NeuralNetworkManager::getDisablePlasticityValue() const {
	return mDisablePlasticity;
}

BoolValue* NeuralNetworkManager::getDisableNetworkUpdateValue() const {
	return mDisableNetworkUpdate;
}

QMutex* NeuralNetworkManager::getNetworkExecutionMutex() {
	return &mNetworkExecutionMutex;
}

Event* NeuralNetworkManager::getResetEvent() const {
	return mResetEvent;
}


Event* NeuralNetworkManager::getResetNetworksEvent() const {
	return mResetNetworksEvent;
}


Event* NeuralNetworkManager::getIterationCompletedEvent() const {
	return mNetworkIterationCompleted;
}


void NeuralNetworkManager::executeNeuralNetworks() {
	TRACE("NeuralNetworkManager::executeNeuralNetworks");

	if(!mDisableNetworkUpdate->get()) {
		//mNetworkEvaluationStarted is triggered as upstream event of NextStep.
		QMutexLocker locker(&mNetworkExecutionMutex);
		for(QListIterator<NeuralNetwork*> i(mNeuralNetworks); i.hasNext();) {
			NeuralNetwork *net = i.next();
			net->executeStep(mNumberOfNetworkUpdatesPerStep->get());
		}
	}
	//trigger evaluation competed even if the update was not triggered
	//because it may still be changed by a third-party plug-in, e.g. a playback device.
	mNetworkEvaluationCompleted->trigger();
}


void NeuralNetworkManager::resetNeuralNetworks() {
	TRACE("NeuralNetworkManager::resetNeuralNetworks");

	QMutexLocker locker(&mNetworkExecutionMutex);
	for(QListIterator<NeuralNetwork*> i(mNeuralNetworks); i.hasNext();) {
		NeuralNetwork *net = i.next();
		net->reset();
	}
}

}


