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



#include "NetworkDegreeOfFreedomCalculator.h"
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
 * Constructs a new NetworkDegreeOfFreedomCalculator.
 */
NetworkDegreeOfFreedomCalculator::NetworkDegreeOfFreedomCalculator()
	: mUpdateDegreeOfFreedomInformationEvent(0)
{
	mDOFAll = new IntValue(0);
	mDOFMain = new IntValue(0);
	mDOFBiasTerms = new IntValue(0);
	mDOFSynapseWeights = new IntValue(0);
	mDOFTransferFunctions = new IntValue(0);
	mDOFActivationFunctions = new IntValue(0);
	mDOFSynapseFunctions = new IntValue(0);

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/NeuralNetwork/DegreesOfFreedom/All", mDOFAll);
	vm->addValue("/NeuralNetwork/DegreesOfFreedom/Main", mDOFMain);
	vm->addValue("/NeuralNetwork/DegreesOfFreedom/BiasTerms", mDOFBiasTerms);
	vm->addValue("/NeuralNetwork/DegreesOfFreedom/SynapseWeights", mDOFSynapseWeights);
	vm->addValue("/NeuralNetwork/DegreesOfFreedom/TransferFunctions", mDOFTransferFunctions);
	vm->addValue("/NeuralNetwork/DegreesOfFreedom/ActivationFunctions", mDOFActivationFunctions);
	vm->addValue("/NeuralNetwork/DegreesOfFreedom/SynapseFunctions", mDOFSynapseFunctions);

	Core::getInstance()->addSystemObject(this);
}



/**
 * Destructor.
 */
NetworkDegreeOfFreedomCalculator::~NetworkDegreeOfFreedomCalculator() {
}

QString NetworkDegreeOfFreedomCalculator::getName() const {
	return "NetworkDegreeOfFreedomCalculator";
}


void NetworkDegreeOfFreedomCalculator::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mUpdateDegreeOfFreedomInformationEvent) {
		calculateDegreesOfFreedom();
	}
}

bool NetworkDegreeOfFreedomCalculator::init() {
	return true;
}


bool NetworkDegreeOfFreedomCalculator::bind() {
	EventManager *em = Core::getInstance()->getEventManager();
	mUpdateDegreeOfFreedomInformationEvent = em->registerForEvent(
					NeuralNetworkConstants::EVENT_CONSTRAINTS_UPDATED, this);

	if(mUpdateDegreeOfFreedomInformationEvent == 0) {
		Core::log("NetworkDegreeOfFreedomCalculator: Could not register for calculate DOF events.");
	}
	return true;
}


bool NetworkDegreeOfFreedomCalculator::cleanUp() {
	return true;
}

void NetworkDegreeOfFreedomCalculator::calculateDegreesOfFreedom() {

	QList<NeuralNetwork*> networks = Neuro::getNeuralNetworkManager()->getNeuralNetworks();

	if(networks.empty()) {
		mDOFAll->set(0);
		mDOFMain->set(0);
		mDOFBiasTerms->set(0);
		mDOFSynapseWeights->set(0);
		mDOFTransferFunctions->set(0);
		mDOFActivationFunctions->set(0);
		mDOFSynapseFunctions->set(0);
		return;
	}

	NeuralNetwork *network = networks.at(0);
	if(network == 0) {
		Core::log("NetworkDegreeOfFreedomCalculator: Could not find a network...");
		return;
	}

	int dofBias = 0;
	int dofWeights = 0;
	int dofTF = 0;
	int dofAF = 0;
	int dofSF = 0;
	
	
	QList<Neuron*> neurons = network->getNeurons();
	
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		
		bool bias = true;
		bool tf = true;
		bool af = true;
		
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
			bias = false;
			tf = false;
			af = false;
		}
		else {
			QString reducedDOFs = neuron->getProperty(
					NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM);
			if(reducedDOFs != "") {
				if(reducedDOFs.contains("B")) {
					bias = false;
				}
				if(reducedDOFs.contains("A")) {
					af = false;
				}
				if(reducedDOFs.contains("T")) {
					tf = false;
				}
			}
			//Count bias only as degree of freedom, if there is one set.
			if(neuron->getBiasValue().get() == 0.0
				|| neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_PROTECT_BIAS)) 
			{
				bias = false;
			}
		}
		if(bias) { ++dofBias; }
		if(tf) { ++dofTF; }
		if(af) { ++dofAF; }
	}
	
	QList<Synapse*> synapses = network->getSynapses();	
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();
		
		bool weight = true;
		bool sf = true;
		
		if(synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
			weight = false;
			sf = false;
		}
		else {
			QString reducedDOFs = synapse->getProperty(
					NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM);
			if(reducedDOFs != "") {
				if(reducedDOFs.contains("W")) {
					weight = false;
				}
				if(reducedDOFs.contains("S")) {
					sf = false;
				}
			}
			if(synapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_PROTECT_STRENGTH)) {
				weight = false;
			}
		}
		if(weight) { ++dofWeights; }
		if(sf) { ++dofSF; }
	}

	
	mDOFAll->set(dofBias + dofWeights + dofTF + dofAF + dofSF);
	mDOFMain->set(dofBias + dofWeights);
	mDOFBiasTerms->set(dofBias);
	mDOFSynapseWeights->set(dofWeights);
	mDOFTransferFunctions->set(dofTF);
	mDOFActivationFunctions->set(dofAF);
	mDOFSynapseFunctions->set(dofSF);
}

}



