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



#include "NeuralNetworkAttributes.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "NeuralNetworkConstants.h"
#include "Event/EventManager.h"
#include "Network/NeuralNetwork.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Network/Neuro.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NeuralNetworkAttributes.
 */
NeuralNetworkAttributes::NeuralNetworkAttributes()
	: mNeuralNetworkModifiedEvent(0), mNeuralNetworkReplacedEvent(0),
	  mNumberOfInputNeurons(0), mNumberOfOutputNeurons(0), mNumberOfHiddenNeurons(0),
	  mNumberOfSynapses(0), mNumberOfGroups(0), mNumberOfModules(0), mNumberOfLoops(0),
	  mNumberOfHigherOrderSynapses(0)
{
	mNumberOfInputNeurons = new IntValue(0);
	mNumberOfOutputNeurons = new IntValue(0);
	mNumberOfHiddenNeurons = new IntValue(0);
	mNumberOfSynapses = new IntValue(0);
	mNumberOfModules = new IntValue(0);
	mNumberOfGroups = new IntValue(0);
	mNumberOfLoops = new IntValue(0);
	mNumberOfHigherOrderSynapses = new IntValue(0);
	mNumberOfSlaveSynapses = new IntValue(0);

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/NeuralNetwork/Attributes/Net1/NumberOfInputNeurons", mNumberOfInputNeurons);
	vm->addValue("/NeuralNetwork/Attributes/Net1/NumberOfOutputNeurons", mNumberOfOutputNeurons);
	vm->addValue("/NeuralNetwork/Attributes/Net1/NumberOfHiddenNeurons", mNumberOfHiddenNeurons);
	vm->addValue("/NeuralNetwork/Attributes/Net1/NumberOfSynapspes", mNumberOfSynapses);
	vm->addValue("/NeuralNetwork/Attributes/Net1/NumberOfModules", mNumberOfModules);
	vm->addValue("/NeuralNetwork/Attributes/Net1/NumberOfGroups", mNumberOfGroups);
	vm->addValue("/NeuralNetwork/Attributes/Net1/NumberOf1Loops", mNumberOfLoops);
	vm->addValue("/NeuralNetwork/Attributes/Net1/NumberOfHigherOrderSynapses", mNumberOfHigherOrderSynapses);
	vm->addValue("/NeuralNetwork/Attributes/Net1/NumberOfSlaveSynapses", mNumberOfSlaveSynapses);

	Core::getInstance()->addSystemObject(this);
}



/**
 * Destructor.
 */
NeuralNetworkAttributes::~NeuralNetworkAttributes() {
}

QString NeuralNetworkAttributes::getName() const {
	return "NeuralNetworkAttributes";
}


void NeuralNetworkAttributes::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mNeuralNetworkReplacedEvent || event == mNeuralNetworkModifiedEvent) {
		determineAttributes();
	}
}

bool NeuralNetworkAttributes::init() {
	return true;
}


bool NeuralNetworkAttributes::bind() {
	EventManager *em = Core::getInstance()->getEventManager();
	mNeuralNetworkReplacedEvent = em->registerForEvent(
					NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED, this);
	mNeuralNetworkModifiedEvent = em->registerForEvent(
					NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED, this);

	if(mNeuralNetworkReplacedEvent == 0 || mNeuralNetworkModifiedEvent == 0) {
		Core::log("NeuralNetworkAttributes: Could not register for network replaced / modified events.");
	}
	return true;
}


bool NeuralNetworkAttributes::cleanUp() {
	return true;
}

void NeuralNetworkAttributes::determineAttributes() {
	QList<NeuralNetwork*> networks = Neuro::getNeuralNetworkManager()->getNeuralNetworks();

	if(networks.empty()) {
		mNumberOfInputNeurons->set(0);
		mNumberOfOutputNeurons->set(0);
		mNumberOfHiddenNeurons->set(0);
		mNumberOfSynapses->set(0);
		
		return;
	}

	//TODO characterize ALL networks
	NeuralNetwork *network = networks.at(0);
	if(network == 0) {
		Core::log("NeuralNetworkAttributes: Could not find a network...");
		return;
	}
	QList<Synapse*> synapses = network->getSynapses();	

	int numberOfInputs = network->getInputNeurons().size();
	int numberOfOutputs = network->getOutputNeurons().size();
	int numberOfLoops = 0;
	int numberOfHigherOrderSynapses = 0;

	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {	
		Synapse *synapse = i.next();
		if(synapse->getTarget() == synapse->getSource()) {
			++numberOfLoops;
		}
		if(dynamic_cast<Synapse*>(synapse->getTarget()) != 0) {
			++numberOfHigherOrderSynapses;
		}
	}

	int numberOfSlaveSynapses = 0;
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {	
		Synapse *synapse = i.next();
		if(synapse->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE)) {
			++numberOfSlaveSynapses;
		}
	}


	mNumberOfInputNeurons->set(numberOfInputs);
	mNumberOfOutputNeurons->set(numberOfOutputs);
	mNumberOfHiddenNeurons->set(network->getNeurons().size() - numberOfInputs - numberOfOutputs);
	mNumberOfSynapses->set(synapses.size());
	mNumberOfLoops->set(numberOfLoops);
	mNumberOfHigherOrderSynapses->set(numberOfHigherOrderSynapses);
	mNumberOfSlaveSynapses->set(numberOfSlaveSynapses);



	ModularNeuralNetwork *modularNetwork = dynamic_cast<ModularNeuralNetwork*>(network);
	if(modularNetwork != 0) {
		int numberOfModules = modularNetwork->getNeuroModules().size();

		mNumberOfModules->set(numberOfModules);
		mNumberOfGroups->set(modularNetwork->getNeuronGroups().size() - numberOfModules);
	}

}

}



