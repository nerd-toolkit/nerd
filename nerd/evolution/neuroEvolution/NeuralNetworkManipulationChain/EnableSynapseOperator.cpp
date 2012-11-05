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



#include "EnableSynapseOperator.h"
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
#include <EvolutionConstants.h>

using namespace std;

namespace nerd {


/**
 * Constructs a new EnableSynapseOperator.
 */
EnableSynapseOperator::EnableSynapseOperator(const QString &name, bool operatorEnablesSynapse)
	: NeuralNetworkManipulationOperator(name), mEnableProbability(0),
	  mMaximalNumberOfAffectedSynapses(0), mOperatorEnablesSynapse(operatorEnablesSynapse)
{
	mEnableProbability = new NormalizedDoubleValue(0.0, 0.0, 1.0, 0.0, 1.0);
	mMaximalNumberOfAffectedSynapses = new IntValue(10);

	addParameter("EnableProbability", mEnableProbability);
	addParameter("MaxNumberOfAffectedSynapses", mMaximalNumberOfAffectedSynapses);
}


/**
 * Copy constructor. 
 * 
 * @param other the EnableSynapseOperator object to copy.
 */
EnableSynapseOperator::EnableSynapseOperator(const EnableSynapseOperator &other) 
	: NeuralNetworkManipulationOperator(other)
{
	mEnableProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("EnableProbability"));
	mMaximalNumberOfAffectedSynapses = dynamic_cast<IntValue*>(
			getParameter("MaxNumberOfAffectedSynapses"));
}

/**
 * Destructor.
 */
EnableSynapseOperator::~EnableSynapseOperator() {
}


NeuralNetworkManipulationOperator* EnableSynapseOperator::createCopy() const
{
	return new EnableSynapseOperator(*this);
}



bool EnableSynapseOperator::applyOperator(Individual *individual, CommandExecutor*) {

	NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("InsertSynapseOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	int maxNumberOfNewlyEnabledSynapses = mMaximalNumberOfAffectedSynapses->get();
	if(maxNumberOfNewlyEnabledSynapses <= 0) {
		return true;
	}

	QList<Synapse*> synapses = net->getSynapses();
	QList<Synapse*> consideredSynapses = synapses;
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		//remove all neurons that can not be changed.
		Synapse *synapse = i.next();

		if(synapse->getEnabledValue().get() == mOperatorEnablesSynapse) {
			consideredSynapses.removeAll(synapse);
			continue;
		}
		
		if(synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED) 
			|| synapse->hasProperty("ProtectEnableState")) 
		{
			consideredSynapses.removeAll(synapse);
			continue;
		}
		//TODO add more
	}

	double probability = mEnableProbability->get();

	if(consideredSynapses.empty()) {
		return true;
	}
	
	for(int i = 0; i < maxNumberOfNewlyEnabledSynapses; ++i) {
		
		if(Random::nextDouble() >= probability) {
			continue;
		}

		//select synapse
		Synapse *synapse = consideredSynapses.value(Random::nextInt(consideredSynapses.size()));

		if(synapse == 0) {
			continue;
		}

		consideredSynapses.removeAll(synapse);
		synapse->getEnabledValue().set(mOperatorEnablesSynapse);

		//mark as modified.
		synapse->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
		
		individual->setProperty(EvolutionConstants::TAG_GENOME_CHANGE_SUMMARY,
						individual->getProperty(EvolutionConstants::TAG_GENOME_CHANGE_SUMMARY) 
							+ ",S:" + QString::number(synapse->getId()) + ":eS"); 
		
	}

	return true;
}



}



