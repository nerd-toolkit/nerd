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

#include "InsertSynapseOperator.h"
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

InsertSynapseOperator::InsertSynapseOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mInsertionProbability(0),
	  mMaximalNumberOfNewSynapses(0), mAllowSynapsesAsTargets(0)
{
	mInsertionProbability = new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0);
	mMaximalNumberOfNewSynapses = new IntValue(10);
	mAllowSynapsesAsTargets = new BoolValue(false);

	addParameter("InsertionProbability", mInsertionProbability);
	addParameter("MaxNumberOfNewSynapses", mMaximalNumberOfNewSynapses);
	addParameter("AllowSynapsesAsTargets", mAllowSynapsesAsTargets);
}

InsertSynapseOperator::InsertSynapseOperator(const InsertSynapseOperator &other)
	: NeuralNetworkManipulationOperator(other)
{
	mInsertionProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("InsertionProbability"));
	mMaximalNumberOfNewSynapses = dynamic_cast<IntValue*>(
			getParameter("MaxNumberOfNewSynapses"));
	mAllowSynapsesAsTargets = dynamic_cast<BoolValue*>(
			getParameter("AllowSynapsesAsTargets"));
}

InsertSynapseOperator::~InsertSynapseOperator() {
}

NeuralNetworkManipulationOperator* InsertSynapseOperator::createCopy() const
{
	return new InsertSynapseOperator(*this);
}



bool InsertSynapseOperator::applyOperator(Individual *individual, CommandExecutor*) {

	NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("InsertSynapseOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	//prepare the generation date as string.
	QString generationDate = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());

	int maxNumberOfNewSynapses = mMaximalNumberOfNewSynapses->get();
	if(maxNumberOfNewSynapses <= 0) {
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
		//TODO add more
	}

	QList<Neuron*> consideredSourceNeurons = consideredNeurons;
	for(QListIterator<Neuron*> i(networkNeurons); i.hasNext();) {
		//remove all neurons that can not be a source neuron.
		Neuron *neuron = i.next();
		if(neuron->hasProperty("ProtectSynapses") //TODO remove this tag (replaced by NoSynapseSource)
			|| neuron->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_SOURCE)) 
		{
			consideredSourceNeurons.removeAll(neuron);
			continue;
		}
	}

	double probability = mInsertionProbability->get();

	if(consideredNeurons.empty()) {
		return true;
	}
	
	for(int i = 0; i < maxNumberOfNewSynapses; ++i) {
		
		if(Random::nextDouble() >= probability) {
			continue;
		}

		//select source neuron
		Neuron *source = consideredSourceNeurons.value(Random::nextInt(consideredSourceNeurons.size()));

		if(source == 0) {
			continue;
		}

		//bool isSlave = source->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE);

		//select target candidates
		QList<SynapseTarget*> targetCandidates;
		for(QListIterator<Neuron*> n(consideredNeurons); n.hasNext();) {
			SynapseTarget *target = n.next();
			//ignore input neurons
			Properties *p = dynamic_cast<Properties*>(target);
			if(p != 0 && p->hasProperty(Neuron::NEURON_TYPE_INPUT)) {
				continue;
			}
			targetCandidates.append(target);
		}
		if(mAllowSynapsesAsTargets->get()) {
			for(QListIterator<Synapse*> s(net->getSynapses()); s.hasNext();) {
				targetCandidates.append(s.next());
			}
		}

		//remove all target candidates that are not valid
		QList<SynapseTarget*> consideredTargets(targetCandidates);
		for(QListIterator<SynapseTarget*> s(targetCandidates); s.hasNext();) {

			SynapseTarget *target = s.next();

			//remove all targets that already are target of a synapse from neuron source
			QList<Synapse*> incommingSynapses = target->getSynapses();
			bool validTarget = true;

			for(QListIterator<Synapse*> k(incommingSynapses); k.hasNext();) {
				Synapse *inSyn = k.next();
				if(inSyn != 0 && inSyn->getSource() == source) {
					validTarget = false;
					break;
				}
			}	
			if(!validTarget) {
				consideredTargets.removeAll(target);
				continue;
			}
			//TODO more
			
			Properties *p = dynamic_cast<Properties*>(target);
			if(p->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_TARGET)) {
				consideredTargets.removeAll(target);
				continue;
			}
		}

		if(consideredTargets.empty()) {
			continue;
		}

		SynapseTarget *synapseTarget = consideredTargets.value(
					Random::nextInt(consideredTargets.size()));

		if(synapseTarget == 0) {
			continue;
		}

		//create synapse
		Synapse *synapse = new Synapse(source, synapseTarget, 0.0, 
								*(net->getDefaultSynapseFunction()));

		synapse->setProperty(EvolutionConstants::TAG_EVO_CREATION_DATE, generationDate);
		synapseTarget->addSynapse(synapse);

		//mark as modified.
		synapse->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);

		//mark the individual as significantly modified
		individual->setProperty(EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE,
						generationDate);
		
	}

	return true;
}


}



