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

#include "ChangeBiasOperator.h"
#include "Network/NeuralNetwork.h"
#include "Core/Core.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Math/Random.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include "Math/Math.h"
#include <iostream>
#include "NeuroEvolutionConstants.h"
#include "Network/NeuroTagManager.h"
#include "Util/NeuroEvolutionUtil.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"

using namespace std;

namespace nerd {

ChangeBiasOperator::ChangeBiasOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mChangeProbability(0),
	  mToggleBiasEnableProbability(0),
	  mMaxBias(0), mMinBias(0), mDeviation(0), mUseGaussDistribution(0)
{
	mChangeProbability = new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0);
	mToggleBiasEnableProbability = new NormalizedDoubleValue(0.0, 0.0, 1.0, 0.0, 1.0);
	mMaxBias = new DoubleValue(10.0);
	mMinBias = new DoubleValue(-10.0);
	mDeviation = new DoubleValue(1.0);
	mUseGaussDistribution = new BoolValue(true);
	mReinitializeBiasProbability = new NormalizedDoubleValue(0.0, 0.0, 1.0, 0.0, 1.0);

	mChangeProbability->setDescription("Probability per neuron to change the bias value if"
										 " the neuron already as a bias.");

	addParameter("ChangeProbability", mChangeProbability);
	addParameter("ToggleDisableProbability", mToggleBiasEnableProbability);
	addParameter("MaxBias", mMaxBias);
	addParameter("MinBias", mMinBias);
	addParameter("Deviation", mDeviation);
	addParameter("UseGaussDistribution", mUseGaussDistribution);
	addParameter("ReInitProbability", mReinitializeBiasProbability);

}

ChangeBiasOperator::ChangeBiasOperator(const ChangeBiasOperator &other)
	: NeuralNetworkManipulationOperator(other)
{
	mChangeProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("ChangeProbability"));
	mToggleBiasEnableProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("ToggleDisableProbability"));
	mMinBias = dynamic_cast<DoubleValue*>(
			getParameter("MinBias"));
	mMaxBias = dynamic_cast<DoubleValue*>(
			getParameter("MaxBias"));
	mDeviation = dynamic_cast<DoubleValue*>(
			getParameter("Deviation"));
	mUseGaussDistribution = dynamic_cast<BoolValue*>(
			getParameter("UseGaussDistribution"));
	mReinitializeBiasProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("ReInitProbability"));
}

ChangeBiasOperator::~ChangeBiasOperator() {
}

NeuralNetworkManipulationOperator* ChangeBiasOperator::createCopy() const
{
	return new ChangeBiasOperator(*this);
}


bool ChangeBiasOperator::applyOperator(Individual *individual, CommandExecutor*) {

	ModularNeuralNetwork *net = dynamic_cast<ModularNeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("ChangeBiasOperator: Could not apply operator because individual did not "
				  "provide a ModularNeuralNetwork as genome!");
		return false;
	}

	QList<Neuron*> neurons = net->getNeurons();
	
	QList<Neuron*> allNeurons(neurons);
	for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
		Neuron *neuron = i.next();

		//TODO remove all unchangeable neurons
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
			|| neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_PROTECT_BIAS)
			|| neuron->hasProperty(Neuron::NEURON_TYPE_INPUT)) 
		{
			neurons.removeAll(neuron);
			continue;
		}
		//remove all neurons without a bias
		if(neuron->getBiasValue().get() == 0.0
			&& !neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_PREVIOUS_BIAS)) 
		{
			neurons.removeAll(neuron);
			continue;
		}
	}

	double probability = mChangeProbability->get();
	double toggleEnabledProbability = mToggleBiasEnableProbability->get();
	bool useGaussDistribution = mUseGaussDistribution->get();
	double deviation = mDeviation->get();
	double min = mMinBias->get();
	double max = mMaxBias->get();
	double reinitProbability = mReinitializeBiasProbability->get();

	//change the bias of some neurons 
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();

		double localChangeProb = probability;

		{
			QString changeProbString = neuron->getProperty(
						NeuralNetworkConstants::TAG_ELEMENT_CHANGE_PROBABILITY);

			bool ok = true;
			localChangeProb = NeuroEvolutionUtil::getLocalNetworkSetting(changeProbString, 
							localChangeProb, net, &ok);
			if(!ok) {
				Core::log("ChangeBiasOperator: Could not interpret "
					" tag [" + NeuralNetworkConstants::TAG_ELEMENT_CHANGE_PROBABILITY
					+ "] with content [" + changeProbString + "]", true);
			}
		}

		if(Random::nextDouble() >= localChangeProb) {
			continue;
		}

		bool disabled = neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_PREVIOUS_BIAS);

		double bias = neuron->getBiasValue().get();
		if(bias != 0.0) {
			disabled = false;
		}

		if(disabled) {
			if(Random::nextDouble() < toggleEnabledProbability) {
				bool ok = false;
				bias = neuron->getProperty(
							NeuralNetworkConstants::TAG_NEURON_PREVIOUS_BIAS).toDouble(&ok);
				neuron->removeProperty(NeuralNetworkConstants::TAG_NEURON_PREVIOUS_BIAS);
				
				if(!ok) {
					continue;
				}	
			}
			else {
				continue;
			}
		}
		else {
		
			if(Random::nextDouble() < toggleEnabledProbability) {
				neuron->setProperty(NeuralNetworkConstants::TAG_NEURON_PREVIOUS_BIAS,
						neuron->getBiasValue().getValueAsString());
				neuron->getBiasValue().set(0.0);

				//mark as modified.
				neuron->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
				continue;
			}
			else {
				if(useGaussDistribution) {
					bias += Math::getNextGaussianDistributedValue(deviation);
				}
				else {
					bias += Math::getNextUniformlyDistributedValue(deviation);
				}
			}
		}
		if(Random::nextDouble() < reinitProbability) {
			neuron->setProperty(NeuralNetworkConstants::TAG_NEURON_REINIT_BIAS);
		}


		{
			QString minString = neuron->getProperty(
						NeuralNetworkConstants::TAG_NEURON_MIN_BIAS);

			bool ok = true;
			min = NeuroEvolutionUtil::getLocalNetworkSetting(minString, 
							min, net, &ok);
			if(!ok) {
				Core::log("ChangeBiasOperator: Could not interpret "
					" tag [" + NeuralNetworkConstants::TAG_NEURON_MIN_BIAS
					+ "] with content [" + minString + "]", true);
			}
		}
		{
			QString maxString = neuron->getProperty(
						NeuralNetworkConstants::TAG_NEURON_MAX_BIAS);

			bool ok = true;
			max = NeuroEvolutionUtil::getLocalNetworkSetting(maxString, 
							max, net, &ok);
			if(!ok) {
				Core::log("ChangeBiasOperator: Could not interpret "
					" tag [" + NeuralNetworkConstants::TAG_NEURON_MAX_BIAS
					+ "] with content [" + maxString + "]", true);
			}
		}
	
// 		if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_MIN_BIAS)) {
// 			bool ok = false;
// 			double m = neuron->getProperty(NeuralNetworkConstants::TAG_NEURON_MIN_BIAS)
// 									.toDouble(&ok);
// 			if(ok) {
// 				min = m;
// 			}
// 		}
// 		if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_MAX_BIAS)) {
// 			bool ok = false;
// 			double m = neuron->getProperty(NeuralNetworkConstants::TAG_NEURON_MAX_BIAS)
// 									.toDouble(&ok);
// 			if(ok) {
// 				max = m;
// 			}
// 		}

		//fit bias to range and set to neuron.
		neuron->getBiasValue().set(Math::min(max, Math::max(min, bias)));
		neuron->removeProperty(NeuralNetworkConstants::TAG_NEURON_PREVIOUS_BIAS);

		//mark as modified.
		neuron->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
	}

	return true;
}


}


