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



#include "InsertBiasOperator.h"
#include <iostream>
#include <QList>
#include "Network/NeuralNetwork.h"
#include "Core/Core.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Math/Random.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include "Core/Properties.h"
#include <iostream>
#include "Evolution/Evolution.h"
#include "EvolutionConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new InsertBiasOperator.
 */
InsertBiasOperator::InsertBiasOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mInsertionProbability(0),
	  mMaximalNumberOfNewBiases(0)
{
	mInsertionProbability = new NormalizedDoubleValue(0.05, 0.0, 1.0, 0.0, 1.0);
	mMaximalNumberOfNewBiases = new IntValue(10);

	addParameter("InsertionProbability", mInsertionProbability);
	addParameter("MaxNumberOfNewBiases", mMaximalNumberOfNewBiases);
}


/**
 * Copy constructor. 
 * 
 * @param other the InsertBiasOperator object to copy.
 */
InsertBiasOperator::InsertBiasOperator(const InsertBiasOperator &other) 
	: NeuralNetworkManipulationOperator(other)
{
	mInsertionProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("InsertionProbability"));
	mMaximalNumberOfNewBiases = dynamic_cast<IntValue*>(
			getParameter("MaxNumberOfNewBiases"));
}

/**
 * Destructor.
 */
InsertBiasOperator::~InsertBiasOperator() {
}


NeuralNetworkManipulationOperator* InsertBiasOperator::createCopy() const {
	return new InsertBiasOperator(*this);
}


bool InsertBiasOperator::applyOperator(Individual *individual, CommandExecutor*) {

	NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("InsertBiasOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	int maxNumberOfNewBiases = mMaximalNumberOfNewBiases->get();
	if(maxNumberOfNewBiases <= 0) {
		return true;
	}

	QList<Neuron*> networkNeurons = net->getNeurons();
	QList<Neuron*> consideredNeurons(networkNeurons);
	for(QListIterator<Neuron*> i(networkNeurons); i.hasNext();) {
		//remove all neurons that can not be changed.
		Neuron *neuron = i.next();
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
			consideredNeurons.removeAll(neuron);
			continue;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_PROTECT_BIAS)) {
			consideredNeurons.removeAll(neuron);
			continue;
		}
		if(neuron->getBiasValue().get() != 0.0) {
			//this neuron has already a bias
			consideredNeurons.removeAll(neuron);
		}
		//TODO add more
	}

	double probability = mInsertionProbability->get();

	if(consideredNeurons.empty()) {
		return true;
	}

	//prepare the generation date as string.
	QString generationDate = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());
	
	for(int i = 0; i < maxNumberOfNewBiases; ++i) {
		
		if(Random::nextDouble() >= probability) {
			continue;
		}

		//select neuron
		Neuron *neuron = consideredNeurons.value(Random::nextInt(consideredNeurons.size()));

		if(neuron == 0) {
			continue;
		}
		consideredNeurons.removeAll(neuron);


		neuron->setProperty(NeuralNetworkConstants::TAG_NEURON_REINIT_BIAS);
		neuron->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
		neuron->setProperty(NeuralNetworkConstants::TAG_EVOLUTION_NEW_BIAS);

		//mark the individual as significantly modified
		individual->setProperty(EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE,
						generationDate);
		
	}

	return true;

}


}



