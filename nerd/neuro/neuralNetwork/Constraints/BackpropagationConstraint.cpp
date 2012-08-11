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



#include "BackpropagationConstraint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Util/Util.h"
#include "Util/NeuralNetworkUtil.h"
#include "NeuralNetworkConstants.h"
#include "NerdConstants.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new BackpropagationConstraint.
 */
BackpropagationConstraint::BackpropagationConstraint()
	: GroupConstraint("Backpropagation"), mNetworksReplacedEvent(0), 
		mNextStepEvent(0), mNetworksModifiedEvent(0), mConfigurationValid(false),
		mIntervalStep(0)
{
	mInputOutputReferences = new StringValue();
	mInputOutputReferences->addValueChangedListener(this);

	mLearningRate = new DoubleValue(0.1);
	mLearningRate->addValueChangedListener(this);

	mUpdateInterval = new IntValue(1);

	addParameter("References", mInputOutputReferences);
	addParameter("LearningRate", mLearningRate);
	addParameter("UpdateInterval", mUpdateInterval);

	mBackPropAlgorithm.setLearningRate(mLearningRate->get());
}


/**
 * Copy constructor. 
 * 
 * @param other the BackpropagationConstraint object to copy.
 */
BackpropagationConstraint::BackpropagationConstraint(const BackpropagationConstraint &other)
	: Object(), ValueChangedListener(), GroupConstraint(other), mNetworksReplacedEvent(0), 
		mNextStepEvent(0), mNetworksModifiedEvent(0), mConfigurationValid(false), mIntervalStep(0)
{
	mInputOutputReferences = dynamic_cast<StringValue*>(getParameter("References"));
	mLearningRate = dynamic_cast<DoubleValue*>(getParameter("LearningRate"));
	mUpdateInterval = dynamic_cast<IntValue*>(getParameter("UpdateInterval"));
	mBackPropAlgorithm.setLearningRate(mLearningRate->get());
}

/**
 * Destructor.
 */
BackpropagationConstraint::~BackpropagationConstraint() {
	if(mNetworksReplacedEvent != 0) {
		mNetworksReplacedEvent->removeEventListener(this);
	}
	disconnectEvents();
}

GroupConstraint* BackpropagationConstraint::createCopy() const {
	return new BackpropagationConstraint(*this);
}


QString BackpropagationConstraint::getName() const {
	return GroupConstraint::getName();
}


void BackpropagationConstraint::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mNextStepEvent) {
		if(mUpdateInterval->get() == 0) {
			return;
		}
		mIntervalStep++;
		if(mIntervalStep < mUpdateInterval->get()) {
			return;
		}
		mIntervalStep = 0;
		if(!mConfigurationValid) {
			return;
		}
		NeuronGroup *group = getOwnerGroup();
		if(group == 0) {
			return;
		}
		QVector<double> currentValues;

		for(QListIterator<QVector<Neuron*> > i(mReferencedNeurons); i.hasNext();) {
			QVector<Neuron*> pair = i.next();
			currentValues.append(pair[0]->getOutputActivationValue().get());
		}
		mBackPropAlgorithm.backpropagate(currentValues);
	}
	else if(event == mNetworksModifiedEvent) {
		mIntervalStep = 0;
		NeuronGroup *group = getOwnerGroup();
		if(group == 0) {
			mConfigurationValid = false;
			mBackPropAlgorithm.clear();
			return;
		}
		mConfigurationValid = true;
		if(!mBackPropAlgorithm.setNeurons(group->getAllEnclosedNeurons())) {
			mConfigurationValid = false;
		}
		if(!updateReferences()) {
			mConfigurationValid = false;
		}
	}
	else if(event == mNetworksReplacedEvent) {
		mIntervalStep = 0;
		checkIfNetworkIsActive();
	}
}

void BackpropagationConstraint::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mInputOutputReferences) {
		updateReferences();
	}
	else if(value == mLearningRate) {
		mBackPropAlgorithm.setLearningRate(mLearningRate->get());
	}
}

bool BackpropagationConstraint::attachToGroup(NeuronGroup *group) {
	bool ok = GroupConstraint::attachToGroup(group);

	EventManager *em = Core::getInstance()->getEventManager();
	mNetworksReplacedEvent = em->registerForEvent(
			NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED, this);

	checkIfNetworkIsActive();
	return ok;
}

bool BackpropagationConstraint::detachFromGroup(NeuronGroup *group) {
	bool ok = GroupConstraint::detachFromGroup(group);

	disconnectEvents();

	if(mNetworksReplacedEvent != 0) {
		mNetworksReplacedEvent->removeEventListener(this);
	}
	return ok;
}

bool BackpropagationConstraint::isValid(NeuronGroup *owner) {
	return true;
}


bool BackpropagationConstraint::applyConstraint(NeuronGroup *owner, CommandExecutor *executor, 
									 QList<NeuralNetworkElement*> &trashcan)
{
	return true;
}

		
bool BackpropagationConstraint::equals(GroupConstraint *constraint) const {
	if(!GroupConstraint::equals(constraint)) {
		return false;
	}
	BackpropagationConstraint *c = 
					dynamic_cast<BackpropagationConstraint*>(constraint);

	if(c == 0) {
		return false;
	}
	return true;
}

void BackpropagationConstraint::checkIfNetworkIsActive() {
	NeuronGroup *group = getOwnerGroup();
	if(group == 0) {
		disconnectEvents();
		updateReferences();
		mConfigurationValid = false;
		return;
	}
	ModularNeuralNetwork *net = group->getOwnerNetwork();
	if(net == 0) {
		mBackPropAlgorithm.clear();
		disconnectEvents();
		updateReferences();
		mConfigurationValid = false;
		return;
	}
	mConfigurationValid = true;

	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();
	if(nnm->getNeuralNetworks().contains(net)) {
		connectEvents();
		if(!mBackPropAlgorithm.setNeurons(group->getAllEnclosedNeurons())) {
			mConfigurationValid = false;
		}
	}
	else {
		disconnectEvents();
		mBackPropAlgorithm.clear();
		mConfigurationValid = false;
	}
	if(!updateReferences()) {
		mConfigurationValid = false;
	}
}

void BackpropagationConstraint::connectEvents() {
	disconnectEvents();

	EventManager *em = Core::getInstance()->getEventManager();

	mNextStepEvent = em->registerForEvent(
			NerdConstants::EVENT_EXECUTION_PRE_STEP_COMPLETED, this);
	mNetworksModifiedEvent = em->registerForEvent(
			NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED, this);
}


void BackpropagationConstraint::disconnectEvents() {
	if(mNextStepEvent != 0) {
		mNextStepEvent->removeEventListener(this);
	}
	if(mNetworksModifiedEvent != 0) {
		mNetworksModifiedEvent->removeEventListener(this);
	}
}

bool BackpropagationConstraint::updateReferences() {

	mReferencedNeurons.clear();

	NeuronGroup *group = getOwnerGroup();
	if(group == 0) {
		return false;
	}
	ModularNeuralNetwork *network = group->getOwnerNetwork();
	if(network == 0) {
		return false;
	}

	QHash<Neuron*, Neuron*> referencedNeurons;

	Core::log("Refs: " + mInputOutputReferences->get(), true);

	QStringList references = mInputOutputReferences->get().split("|");
	for(QListIterator<QString> i(references); i.hasNext();) {
		QStringList pairs = i.next().split(",");
		if(pairs.size() != 2) {
			Core::log("BackpropagationConstraint: Reference list corrupt! [" 
						+ mInputOutputReferences->get() + "]", true);
			return false;
		}
		bool ok = true;
		qulonglong refId = pairs[0].toULongLong(&ok);
		if(!ok) {
			Core::log("BackpropagationConstraint: Reference list corrupt (conversion1)! [" 
						+ mInputOutputReferences->get() + "]", true);
			return false;
		}
		qulonglong goalId = pairs[1].toULongLong(&ok);
		if(!ok) {
			Core::log("BackpropagationConstraint: Reference list corrupt (conversion2)! [" 
						+ mInputOutputReferences->get() + "]", true);
			return false;
		}
		//allow both directions of references (goal-target / target-goal)
		Neuron *goalNeuron = NeuralNetwork::selectNeuronById(goalId, mBackPropAlgorithm.getOutputLayer());
		if(goalNeuron == 0) {
			qulonglong id = refId;
			refId = goalId;
			goalId = id;
			goalNeuron = NeuralNetwork::selectNeuronById(goalId, mBackPropAlgorithm.getOutputLayer());
		}

		Neuron *refNeuron = NeuralNetwork::selectNeuronById(refId, network->getNeurons());
		if(refNeuron == 0 || goalNeuron == 0 || refNeuron == goalNeuron) {
			Core::log("BackpropagationConstraint: Could not find referenced neurons "
					"(Output layer or external neurons) [" 
						+ pairs[0] + "," + pairs[1] + "]", true);
			return false;
		}
		referencedNeurons.insert(goalNeuron, refNeuron);

	}
	//create reference pairs in the order given by the backpropAlgorithm.
	QList<Neuron*> outputLayer = mBackPropAlgorithm.getOutputLayer();
	for(QListIterator<Neuron*> i(outputLayer); i.hasNext();) {
		Neuron *neuron = i.next();
		Neuron *refNeuron = referencedNeurons.value(neuron);
		if(!outputLayer.contains(refNeuron) && refNeuron != 0 && neuron != 0) {
			QVector<Neuron*> refList;
			refList.append(refNeuron);
			refList.append(neuron);
			mReferencedNeurons.append(refList);
		}
	}	


	return true;
}

}



