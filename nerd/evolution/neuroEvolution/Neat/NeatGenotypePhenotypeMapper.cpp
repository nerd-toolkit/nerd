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



#include "NeatGenotypePhenotypeMapper.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Neat/NeatGenome.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "Network/Neuron.h"
#include "Network/Synapse.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NeatGenotypePhenotypeMapper.
 */
NeatGenotypePhenotypeMapper::NeatGenotypePhenotypeMapper()
	: GenotypePhenotypeMapper("Neat")
{
}


/**
 * Copy constructor. 
 * 
 * @param other the NeatGenotypePhenotypeMapper object to copy.
 */
NeatGenotypePhenotypeMapper::NeatGenotypePhenotypeMapper(const NeatGenotypePhenotypeMapper &other) 
	: GenotypePhenotypeMapper(other)
{
}

/**
 * Destructor.
 */
NeatGenotypePhenotypeMapper::~NeatGenotypePhenotypeMapper() {
}

GenotypePhenotypeMapper* NeatGenotypePhenotypeMapper::createCopy() const {
	return new NeatGenotypePhenotypeMapper(*this);
}


bool NeatGenotypePhenotypeMapper::createPhenotype(Individual *genomeOwner) {
	if(genomeOwner == 0) {
		return false;
	}

	//destroy the old phenotype.
	Object *phenotype = genomeOwner->getPhenotype();
	genomeOwner->setPhenotype(0);
	if(phenotype != 0) {
		delete phenotype;
	}


	NeatGenome *genome = dynamic_cast<NeatGenome*>(genomeOwner->getGenome());
	if(genome == 0) {
		return false;
	}
	ModularNeuralNetwork *network = new ModularNeuralNetwork(
				AdditiveTimeDiscreteActivationFunction(),
				TransferFunctionTanh(), 
				SimpleSynapseFunction());
	
	const QList<NeatNodeGene*> &nodes = genome->getNodeGenes();
	for(QListIterator<NeatNodeGene*> i(nodes); i.hasNext();) {
		NeatNodeGene *node = i.next();

		if(node->mType == NeatNodeGene::BIAS) {
			//hide bias node.
			continue;
		}
		
		Neuron *neuron = new Neuron("Hidden", *(network->getDefaultTransferFunction()),
						*(network->getDefaultActivationFunction()), node->mId);

		if(node->mType == NeatNodeGene::SENSOR) {
			neuron->setProperty(Neuron::NEURON_TYPE_INPUT);
		}
		if(node->mType == NeatNodeGene::OUTPUT) {
			neuron->setProperty(Neuron::NEURON_TYPE_OUTPUT);
		}
		
		network->addNeuron(neuron);
	}

	const QList<NeatConnectionGene*> &connections = genome->getConnectionGenes();
	for(QListIterator<NeatConnectionGene*> i(connections); i.hasNext();) {
		NeatConnectionGene *connection = i.next();
		
		if(connection->mEnabled == false) {
			//ignore disabled synapses.
			continue;
		}

		Neuron *source = NeuralNetwork::selectNeuronById(connection->mOutputNode, 
							network->getNeurons());
		Neuron *target = NeuralNetwork::selectNeuronById(connection->mInputNode, 
							network->getNeurons());

		if(target == 0) {
			if(!isBiasNode(connection->mOutputNode, genome)) {
				Core::log("NeatGenotypePhenotypeMapper: Genotype was invalid. Could not find "
						"target neuron " + QString::number(connection->mInputNode) 
							+ ". [SKIPPING SYNAPSE]");
				cerr << "NeatGenotypePhenotypeMapper: Genotype invalid. See logfile." << endl;
			}
			continue;
		}

		if(isBiasNode(connection->mOutputNode, genome)) {
			target->getBiasValue().set(connection->mWeight);
		}
		else {
			if(source == 0) {
				if(!isBiasNode(connection->mInputNode, genome)) {
					Core::log("NeatGenotypePhenotypeMapper: Genotype was invalid. Could not find "
							"source neuron " + QString::number(connection->mOutputNode)
							 + ". [SKIPPING SYNAPSE]");
					cerr << "NeatGenotypePhenotypeMapper: Genotype invalid. See logfile." << endl;
				}
				continue;
			}

			Synapse *synapse = new Synapse(source, target, connection->mWeight, 
							*(network->getDefaultSynapseFunction()), connection->mId);
			target->addSynapse(synapse);
		}

		
	}

	genomeOwner->setPhenotype(network);
	return true;

}


bool NeatGenotypePhenotypeMapper::updateGenotype(Individual*) {
	//TODO
	return false;
}

bool NeatGenotypePhenotypeMapper::isBiasNode(int id, NeatGenome *genome) {
	if(genome == 0) {
		return false;
	}
	QList<NeatNodeGene*> &genes = genome->getNodeGenes();
	for(QListIterator<NeatNodeGene*> i(genes); i.hasNext();) {
		NeatNodeGene *node = i.next();
		if(node->mId == id) {
			return node->mType == NeatNodeGene::BIAS;
		}
	}
	return false;
}




}



