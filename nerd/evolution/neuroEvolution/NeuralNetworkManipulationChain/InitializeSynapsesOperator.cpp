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

#include "InitializeSynapsesOperator.h"
#include "Network/NeuralNetwork.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Core/Core.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Math/Random.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <iostream>
#include <QListIterator>
#include "Util/NeuroEvolutionUtil.h"

using namespace std;

namespace nerd {

InitializeSynapsesOperator::InitializeSynapsesOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mMinStrength(0)
{
	mMinStrength = new DoubleValue(-10.0);
	mMaxStrength = new DoubleValue(10.0);
	addParameter("MinStrength", mMinStrength);
	addParameter("MaxStrength", mMaxStrength);
}

InitializeSynapsesOperator::InitializeSynapsesOperator(const InitializeSynapsesOperator &other)
	: NeuralNetworkManipulationOperator(other)
{
	mMinStrength = dynamic_cast<DoubleValue*>(
			getParameter("MinStrength"));
	mMaxStrength = dynamic_cast<DoubleValue*>(
			getParameter("MaxStrength"));
}

InitializeSynapsesOperator::~InitializeSynapsesOperator() {
}

NeuralNetworkManipulationOperator* InitializeSynapsesOperator::createCopy() const
{
	return new InitializeSynapsesOperator(*this);
}


bool InitializeSynapsesOperator::applyOperator(Individual *individual, 	
												CommandExecutor*) 
{

	ModularNeuralNetwork *net = dynamic_cast<ModularNeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("InitializeSynapsesOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	QList<Synapse*> synapses = net->getSynapses();

	QList<Synapse*> allSynapses(synapses);
	for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
		Synapse *synapse = i.next();

		if(synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
			|| synapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_PROTECT_STRENGTH)) 
		{
			synapses.removeAll(synapse);
			continue;
		}
	}

	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();
		if(synapse->getStrengthValue().get() == 0.0)
// 			|| synapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_REINIT_STRENGTH)) 
		{
			double min = mMinStrength->get();
			double max = mMaxStrength->get();
			
			//check if the neuron strength is forced to a spefic range.
			{
				bool ok = true;
				QString minString = synapse->getProperty(
							NeuralNetworkConstants::TAG_SYNAPSE_MIN_STRENGTH);
				
				min = NeuroEvolutionUtil::getLocalNetworkSetting(minString, 
								min, net, &ok);
				if(!ok) {
					Core::log("InitializeSynapsesOperator: Could not interpret "
						" tag [" + NeuralNetworkConstants::TAG_SYNAPSE_MIN_STRENGTH
						+ "] with content [" + minString + "]", true);
				}
			}
			{
				bool ok = true;
				QString maxString = synapse->getProperty(
							NeuralNetworkConstants::TAG_SYNAPSE_MAX_STRENGTH);
				
				max = NeuroEvolutionUtil::getLocalNetworkSetting(maxString, 
								max, net, &ok);
				if(!ok) {
					Core::log("InitializeSynapsesOperator: Could not interpret "
						" tag [" + NeuralNetworkConstants::TAG_SYNAPSE_MAX_STRENGTH
						+ "] with content [" + maxString + "]", true);
				}
			}
			
			
			if(max < min) {
				Core::log("InitializeSynapsesOperator: Inconsistent range found for synapse ["
					+ QString::number(synapse->getId()) + ": "
					+ "[" + QString::number(min) + "," + QString::number(max)
					+ "]. Set to min!", true);
				
				max = min;
			}
			double strength = min + (Random::nextDouble() * (max - min));
			synapse->getStrengthValue().set(strength);

			//mark as modified.
			synapse->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
// 			synapse->removeProperty(NeuralNetworkConstants::TAG_SYNAPSE_REINIT_STRENGTH);
		}
	}
	return true;
}


}


