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

#include "InsertNeuronOperator.h"
#include "Network/NeuralNetwork.h"
#include "Core/Core.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Math/Random.h"
#include <iostream>
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include "Network/Synapse.h"
#include "EvolutionConstants.h"
#include "Network/NeuroTagManager.h"

using namespace std;

namespace nerd {

InsertNeuronOperator::InsertNeuronOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mMaximalNumberOfNeurons(0),
	  mMaximalNumberOfNewNeurons(0), mInsertionProbabilityPerNewNeuron(0)
{
	mMaximalNumberOfNeurons = new IntValue(50);
	mMaximalNumberOfNewNeurons = new IntValue(5);
	mInsertionProbabilityPerNewNeuron = new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0);
	mInsertToModuleProbability = new NormalizedDoubleValue(1.0, 0.0, 1.0, 0.0, 1.0);
// 	mExtendSynapticConnections = new BoolValue(false);
	mInitialConnectionProportion = new NormalizedDoubleValue(0.3, 0.0, 1.0, 0.0, 1.0);

	addParameter("MaxNumberOfHiddenNeurons", mMaximalNumberOfNeurons);
	addParameter("MaxNumberOfNewNeurons", mMaximalNumberOfNewNeurons);
	addParameter("InsertionProbability", mInsertionProbabilityPerNewNeuron);
	addParameter("AddToModuleProbability", mInsertToModuleProbability);
// 	addParameter("ExtendSynapticConnections", mExtendSynapticConnections);
	addParameter("InitConnectionProportion", mInitialConnectionProportion);
	
	NeuroTagManager *ntm = NeuroTagManager::getInstance();
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MODULE_MAX_NUMBER_OF_NEURONS,
						NeuralNetworkConstants::TAG_TYPE_MODULE,
						"Restricts the number of neurons for a single group or neuro-module."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MODULE_PROTECT_NEURONS,
						NeuralNetworkConstants::TAG_TYPE_MODULE,
						"Prevents the addition and removal of neurons to/from a neuro-module."));
}

InsertNeuronOperator::InsertNeuronOperator(const InsertNeuronOperator &other)
	: NeuralNetworkManipulationOperator(other)
{
	mMaximalNumberOfNeurons = dynamic_cast<IntValue*>(
			getParameter("MaxNumberOfHiddenNeurons"));
	mMaximalNumberOfNewNeurons = dynamic_cast<IntValue*>(
			getParameter("MaxNumberOfNewNeurons"));
	mInsertionProbabilityPerNewNeuron = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("InsertionProbability"));
	mInsertToModuleProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("AddToModuleProbability"));
// 	mExtendSynapticConnections = dynamic_cast<BoolValue*>(
// 			getParameter("ExtendSynapticConnections"));
	mInitialConnectionProportion = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("InitConnectionProportion"));
}

InsertNeuronOperator::~InsertNeuronOperator() {
}

NeuralNetworkManipulationOperator* InsertNeuronOperator::createCopy() const
{
	return new InsertNeuronOperator(*this);
}


bool InsertNeuronOperator::applyOperator(Individual *individual, CommandExecutor*) {

	NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("InsertNeuronOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	QString currentGenString = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());

	int maxNumberOfNeurons = mMaximalNumberOfNeurons->get();
	int maxNumberOfNewNeurons = mMaximalNumberOfNewNeurons->get();
	double insertionProbability = mInsertionProbabilityPerNewNeuron->get();
	int numberOfInterfaceNeurons = net->getInputNeurons().size() + net->getOutputNeurons().size();
	bool extendSynapse = false; //mExtendSynapticConnections->get();
	double initConnectionProportion = mInitialConnectionProportion->get();

	if(maxNumberOfNeurons <= 0 || maxNumberOfNewNeurons <= 0) {
		return true;
	}

	for(int i = 0; i < maxNumberOfNewNeurons; ++i) {
		if(net->getNeurons().size() - numberOfInterfaceNeurons >= maxNumberOfNeurons) {
			//don't change anything beacuse the maximal number of hidden neurons is exceeded.
			return true;
		}

		if(Random::nextDouble() < insertionProbability) {

			//check if the neuron has to extend a synapse
			Synapse *extendedSynapse = 0;
			if(extendSynapse) {
				QList<Synapse*> synapses = net->getSynapses();

				//remove protected synapses
				{
					QList<Synapse*> allSynapses = net->getSynapses();
					for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
						Synapse *synapse = i.next();
			
						if(synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
							|| synapse->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE))
						{
							synapses.removeAll(synapse);
							continue;
						}
					}
				}
				if(!synapses.empty()) {
					extendedSynapse = synapses.at(Random::nextInt(synapses.size()));
				}
			}

			//add a neuron
			Neuron *neuron = new Neuron("", 
						*(net->getDefaultTransferFunction()),
						*(net->getDefaultActivationFunction()));


			neuron->setProperty(EvolutionConstants::TAG_EVO_CREATION_DATE, currentGenString);
			if(initConnectionProportion > 0) {
				neuron->setProperty(NeuralNetworkConstants::TAG_NEURON_INITIAL_CONNECTION_PROPORTION,
									QString::number(initConnectionProportion));
			}
			
			NeuroModule *ownerModule = 0;
			
			//check whether to put the new neuron to a module
			ModularNeuralNetwork *modularNetwork = dynamic_cast<ModularNeuralNetwork*>(net);
			if(net != 0 && Random::nextDouble() < mInsertToModuleProbability->get()) {
				QList<NeuroModule*> modules = modularNetwork->getNeuroModules();
				QList<NeuroModule*> candidates;

				for(QListIterator<NeuroModule*> j(modules); j.hasNext();) {
					NeuroModule *module = j.next();

					if(module->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
						|| module->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE)
						|| module->hasProperty(NeuralNetworkConstants::TAG_MODULE_PROTECT_NEURONS))
					{
						continue;
					}
					
					//Check if the maximal number of neurons for this module is reached.
					if(module->hasProperty(NeuralNetworkConstants::TAG_MODULE_MAX_NUMBER_OF_NEURONS)) {
						bool ok = true;
						int size = module->getProperty(NeuralNetworkConstants::TAG_MODULE_MAX_NUMBER_OF_NEURONS)
											.toInt(&ok);
						if(ok && size <= module->getNeurons().size()) {
							continue;
						}
					}

					candidates.append(module);
				}

				if(!candidates.empty()) {
					ownerModule = candidates.at(Random::nextInt(candidates.size()));
// 					NeuroModule *parentModule = candidates.at(Random::nextInt(candidates.size()));
// 					if(parentModule != 0) {
// 						parentModule->addNeuron(neuron);
// 
// 						Vector3D pos = parentModule->getPosition();
// 						QSizeF size = parentModule->getSize();
// 						neuron->setPosition(Vector3D(
// 								pos.getX() + 10.0 + (Random::nextDouble() * (size.width() - 20.0)),
// 								pos.getY() + 10.0 + (Random::nextDouble() * (size.height() - 20.0)),
// 								0.0));
// 					}
				}
				else if(mInsertToModuleProbability->get() >= 1.0) {
					//stop trying to add neurons, because there is no place in modules left...
					return true;
				}
			}

			net->addNeuron(neuron);

			if(extendedSynapse != 0) {
				SynapseTarget *target = extendedSynapse->getTarget();
				target->removeSynapse(extendedSynapse);
				extendedSynapse->setTarget(neuron);
				extendedSynapse->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
				neuron->addSynapse(extendedSynapse);

				Synapse *s = extendedSynapse->createCopy();
				s->setId(NeuralNetwork::generateNextId());
				s->getStrengthValue().set(1.0);
				s->setSource(neuron);
				s->setTarget(target);
				s->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
				target->addSynapse(s);
			}
			
			if(ownerModule != 0) {
				ownerModule->addNeuron(neuron);

				Vector3D pos = ownerModule->getPosition();
				QSizeF size = ownerModule->getSize();
				neuron->setPosition(Vector3D(
						pos.getX() + 10.0 + (Random::nextDouble() * (size.width() - 20.0)),
						pos.getY() + 10.0 + (Random::nextDouble() * (size.height() - 20.0)),
						0.0));
			}

			//mark as modified.
			neuron->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
			
			//mark as new
			neuron->setProperty(NeuralNetworkConstants::TAG_EVOLUTION_NEW_ELEMENT);

			//mark the individual as significantly modified
			individual->setProperty(EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE,
									currentGenString);

			
		}
	}

	return true;
}




}


