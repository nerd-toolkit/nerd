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

#include "RemoveSynapseOperator.h"
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


using namespace std;

namespace nerd {

RemoveSynapseOperator::RemoveSynapseOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mRemoveProbability(0),
	  mMaxNumberOfSynapsesToRemove(0)
{
	mRemoveProbability = new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0);
	mMaxNumberOfSynapsesToRemove = new IntValue(5);

	addParameter("RemoveProbability", mRemoveProbability);
	addParameter("MaxNumberOfRemovedSynapses", mMaxNumberOfSynapsesToRemove);
}

RemoveSynapseOperator::RemoveSynapseOperator(const RemoveSynapseOperator &other)
	: NeuralNetworkManipulationOperator(other)
{
	mRemoveProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("RemoveProbability"));
	mMaxNumberOfSynapsesToRemove = dynamic_cast<IntValue*>(
			getParameter("MaxNumberOfRemovedSynapses"));
}

RemoveSynapseOperator::~RemoveSynapseOperator() {
}

NeuralNetworkManipulationOperator* RemoveSynapseOperator::createCopy() const
{
	return new RemoveSynapseOperator(*this);
}


bool RemoveSynapseOperator::applyOperator(Individual *individual, CommandExecutor*) {

	NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("RemoveSynapseOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	QString currentGenString = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());

	QList<Synapse*> synapses = net->getSynapses();

	//remove protected synapses
	{
		QList<Synapse*> allSynapses = net->getSynapses();
		for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
			Synapse *synapse = i.next();

			if(synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
				|| synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECT_EXISTENCE)) 
			{
				synapses.removeAll(synapse);
				continue;
			}
			else if(synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
				synapses.removeAll(synapse);
				continue;
			}
			else if(synapse->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE)) {
				synapses.removeAll(synapse);
				continue;
			}
		}
	}

	double probability = mRemoveProbability->get();
	int maxNumberOfRemovedSynapses = mMaxNumberOfSynapsesToRemove->get();

	for(int i = 0; i < maxNumberOfRemovedSynapses && !synapses.empty(); ++i) {
		
		if(Random::nextDouble() >= probability) {
			continue;
		}

		Synapse *synapse = synapses.value(Random::nextInt(synapses.size()));

		if(synapse == 0) {
			continue;
		}

		synapses.removeAll(synapse);

		QList<NeuralNetworkElement*> deletedElements = net->savelyRemoveNetworkElement(synapse);
		while(!deletedElements.empty()) {
			NeuralNetworkElement *elem = deletedElements.at(0);
			deletedElements.removeAll(elem);
			delete elem;
		}

		QString currentGenString = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());
	}

	return true;
}


}


