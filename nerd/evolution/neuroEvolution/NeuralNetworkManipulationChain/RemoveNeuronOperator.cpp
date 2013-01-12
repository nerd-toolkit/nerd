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

#include "RemoveNeuronOperator.h"
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
#include "EvolutionConstants.h"
#include "Evolution/Evolution.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"

using namespace std;

namespace nerd {

RemoveNeuronOperator::RemoveNeuronOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mRemoveProbability(0),
	  mMaxNumberOfRemovedNeurons(0)
{
	mRemoveProbability = new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0);
	mMaxNumberOfRemovedNeurons = new IntValue(5);

	addParameter("RemoveProbability", mRemoveProbability);
	addParameter("MaxNumberOfRemovedNeurons", mMaxNumberOfRemovedNeurons);
}

RemoveNeuronOperator::RemoveNeuronOperator(const RemoveNeuronOperator &other)
	: NeuralNetworkManipulationOperator(other)
{
	mRemoveProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("RemoveProbability"));
	mMaxNumberOfRemovedNeurons = dynamic_cast<IntValue*>(
			getParameter("MaxNumberOfRemovedNeurons"));
}

RemoveNeuronOperator::~RemoveNeuronOperator() {
}

NeuralNetworkManipulationOperator* RemoveNeuronOperator::createCopy() const
{
	return new RemoveNeuronOperator(*this);
}


bool RemoveNeuronOperator::applyOperator(Individual *individual, CommandExecutor*) {

	ModularNeuralNetwork *net = dynamic_cast<ModularNeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("RemoveNeuronOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	QString currentGenString = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());

	QList<Neuron*> neurons = net->getNeurons();
	
	//remove all input and output neurons
	{
		QList<Neuron*> inputNeurons = net->getInputNeurons();
		for(QListIterator<Neuron*> i(inputNeurons); i.hasNext();) {
			neurons.removeAll(i.next());
		}
		QList<Neuron*> outputNeurons = net->getOutputNeurons();
		for(QListIterator<Neuron*> i(outputNeurons); i.hasNext();) {
			neurons.removeAll(i.next());
		}
	}
	
	QList<NeuronGroup*> neuronGroups = net->getNeuronGroups();
	
	//remove all protected neurons
	{
		QList<Neuron*> allNeurons = neurons;
		for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
			if(neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
				|| neuron->hasProperty("ProtectExistence")) 
			{
				neurons.removeAll(neuron);
				continue;
			}
			if(neuron->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE))
// 				|| neuron->hasProperty(NeuralNetworkConstants::TAG_MODULE_INPUT)
// 				|| neuron->hasProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT))
			{
				neurons.removeAll(neuron);
				continue;
			}
			bool isProtectedByOwnerModule = false;
			for(QListIterator<NeuronGroup*> i(neuronGroups); i.hasNext();) {
				NeuronGroup *group = i.next();
				if(group->getNeurons().contains(neuron)) {
					if(group->hasProperty(NeuralNetworkConstants::TAG_MODULE_PROTECT_NEURONS)) {
						isProtectedByOwnerModule = true;
						break;
					}
				}
			}
			if(isProtectedByOwnerModule) {
				neurons.removeAll(neuron);
				continue;
			}
		}
	}

	double probability = mRemoveProbability->get();
	int maxNumberOfNeuronsToRemove = mMaxNumberOfRemovedNeurons->get();

	for(int i = 0; i < maxNumberOfNeuronsToRemove; ++i) {
		if(Random::nextDouble() >= probability) {
			continue;
		}

		Neuron *neuron = neurons.value(Random::nextInt(neurons.size()));

		if(neuron == 0) {
			continue;
		}

		neurons.removeAll(neuron);
		
		QString removeString = "";

		QList<NeuralNetworkElement*> deletedElements = net->savelyRemoveNetworkElement(neuron);
		while(!deletedElements.empty()) {
			NeuralNetworkElement *elem = deletedElements.at(0);
			
			QString type = "N";
			if(dynamic_cast<Synapse*>(elem) != 0) {
				type = "S";
			}
			removeString += "," + type + ":" + QString::number(elem->getId()) + ":r" + type;
			
			deletedElements.removeAll(elem);
			delete elem;
		}

		//mark the individual as significantly modified
		individual->setProperty(EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE,
									currentGenString);
		
		individual->setProperty(EvolutionConstants::TAG_GENOME_CHANGE_SUMMARY,
						individual->getProperty(EvolutionConstants::TAG_GENOME_CHANGE_SUMMARY) 
							+ removeString); 
	}

	return true;
}


}


