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



#include "RemoveBiasOperator.h"
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
 * Constructs a new RemoveBiasOperator.
 */
RemoveBiasOperator::RemoveBiasOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mRemoveProbability(0), mMaximalNumberOfRemovedBiases(0)
{
	mRemoveProbability = new NormalizedDoubleValue(0.05, 0.0, 1.0, 0.0, 1.0);
	mMaximalNumberOfRemovedBiases = new IntValue(10);

	addParameter("RemoveProbability", mRemoveProbability);
	addParameter("MaxNumberOfRemovedBiases", mMaximalNumberOfRemovedBiases);
}


/**
 * Copy constructor. 
 * 
 * @param other the RemoveBiasOperator object to copy.
 */
RemoveBiasOperator::RemoveBiasOperator(const RemoveBiasOperator &other) 
	: NeuralNetworkManipulationOperator(other)
{
	mRemoveProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("RemoveProbability"));
	mMaximalNumberOfRemovedBiases = dynamic_cast<IntValue*>(
			getParameter("MaxNumberOfRemovedBiases"));
}

/**
 * Destructor.
 */
RemoveBiasOperator::~RemoveBiasOperator() {
}


NeuralNetworkManipulationOperator* RemoveBiasOperator::createCopy() const {
	return new RemoveBiasOperator(*this);
}


bool RemoveBiasOperator::applyOperator(Individual *individual, CommandExecutor*) {

	NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("RemoveBiasOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	QString generationDate = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());

	QList<Neuron*> neurons = net->getNeurons();
	QList<Neuron*> consideredNeurons = neurons;

	//remove protected neurons and neurons without bias values.
	{
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *neuron = i.next();

			if(neuron->getBiasValue().get() == 0.0) {
				consideredNeurons.removeAll(neuron);
				continue;
			}
			if(neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
				consideredNeurons.removeAll(neuron);
				continue;
			}
			if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_PROTECT_BIAS_EXISTENCE)) {
				consideredNeurons.removeAll(neuron);
				continue;
			}
			else if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_PROTECT_BIAS)) {
				consideredNeurons.removeAll(neuron);
				continue;
			}
		}
	}

	double probability = mRemoveProbability->get();
	int maxNumberOfRemovedBiases = mMaximalNumberOfRemovedBiases->get();

	for(int i = 0; i < maxNumberOfRemovedBiases && !consideredNeurons.empty(); ++i) {
		
		if(Random::nextDouble() >= probability) {
			continue;
		}

		Neuron *neuron = consideredNeurons.value(Random::nextInt(consideredNeurons.size()));

		if(neuron == 0) {
			continue;
		}

		consideredNeurons.removeAll(neuron);
		neuron->getBiasValue().set(0.0);

		neuron->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);

		//mark the individual as significantly modified
		individual->setProperty(EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE,
						generationDate);
		
		individual->setProperty(EvolutionConstants::TAG_GENOME_CHANGE_SUMMARY,
						individual->getProperty(EvolutionConstants::TAG_GENOME_CHANGE_SUMMARY) 
							+ ",N:" + QString::number(neuron->getId()) + ":rB"); 
	}

	return true;
}


}



