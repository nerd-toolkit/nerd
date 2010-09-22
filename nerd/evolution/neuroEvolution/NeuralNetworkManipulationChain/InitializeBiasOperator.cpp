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

#include "InitializeBiasOperator.h"
#include "Network/NeuralNetwork.h"
#include "Core/Core.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Math/Random.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <iostream>
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include "Math/Math.h"
#include "EvolutionConstants.h"


using namespace std;

namespace nerd {

InitializeBiasOperator::InitializeBiasOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mInitializationProbability(0),
	  mMinimumBias(0), mMaximumBias(0)
{
	mInitializationProbability = new NormalizedDoubleValue(0.1, 0.0, 1.0, 0.0, 1.0);
	mMinimumBias = new DoubleValue(-2.0);
	mMaximumBias = new DoubleValue(2.0);

	addParameter("MinimumBias", mMinimumBias);
	addParameter("MaximumBias", mMaximumBias);
	//addParameter("InitProbability", mInitializationProbability);
}

InitializeBiasOperator::InitializeBiasOperator(const InitializeBiasOperator &other)
	: NeuralNetworkManipulationOperator(other)
{
	mMinimumBias = dynamic_cast<DoubleValue*>(
			getParameter("MinimumBias"));
	mMaximumBias = dynamic_cast<DoubleValue*>(
			getParameter("MaximumBias"));
// 	mInitializationProbability = dynamic_cast<NormalizedDoubleValue*>(
// 			getParameter("InitProbability"));
}

InitializeBiasOperator::~InitializeBiasOperator() {
}

NeuralNetworkManipulationOperator* InitializeBiasOperator::createCopy() const
{
	return new InitializeBiasOperator(*this);
}


bool InitializeBiasOperator::applyOperator(Individual *individual, CommandExecutor*) {

	NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("InitializeBiasOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	//int currentGeneration = Evolution::getEvolutionManager()->getCurrentGenerationValue()->get();

	QList<Neuron*> neurons = net->getNeurons();

	{
	//remove protected neurons
		QList<Neuron*> allNeurons = neurons;
		for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
	
			//TODO correct string and extend
			if(neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
				|| neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_PROTECT_BIAS)
				|| neuron->hasProperty(Neuron::NEURON_TYPE_INPUT)) 
			{
				neurons.removeAll(neuron);
				continue;
			}
			
			//ignore all neurons that have a REINIT marker.
			if(!neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_REINIT_BIAS)) {
				neurons.removeAll(neuron);
				continue;
			}
	// 		QString creationDate = neuron->getProperty(EvolutionConstants::TAG_EVO_CREATION_DATE);
	// 		if(creationDate != "") {
	// 			bool ok = false;
	// 			int creationGeneration = creationDate.toInt(&ok);
	// 			if(!ok) {
	// 				neurons.removeAll(neuron);
	// 				continue;
	// 			}
	// 			else if(creationGeneration != currentGeneration) 
	// 			{
	// 				neurons.removeAll(neuron);
	// 				continue;
	// 			}
	// 		}
		}
	}

	double initProbability = 0.0; //mInitializationProbability->get();
	double min = mMinimumBias->get();
	double max = Math::max(mMaximumBias->get(), min);
	double range = max - min;
	

	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {

		Neuron *neuron = i.next();

		if(Random::nextDouble() >= initProbability 
			&& !neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_REINIT_BIAS)) 
		{
			continue;
		}

		//initialize bias
		double bias = min + (Random::nextDouble() * (range));
		neuron->getBiasValue().set(bias);

		//mark as modified.
		neuron->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
		neuron->removeProperty(NeuralNetworkConstants::TAG_NEURON_REINIT_BIAS);
	}

	return true;
}


}


