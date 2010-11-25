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

#include "ChangeSynapseStrengthOperator.h"
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
#include "Network/NeuroTagManager.h"

//TODO allow overwriting with factors, fixed values, variables and the like. (auch bei bias)

using namespace std;

namespace nerd {

ChangeSynapseStrengthOperator::ChangeSynapseStrengthOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mStrengthChangeProbability(0),
	  mDirectionChangeProbability(0), mSignChangeProbability(0),
	  mMaxBias(0), mMinBias(0), mDeviation(0), mUseGaussDistribution(0),
	  mAllowDynamicSignChanges(0)
{
	mStrengthChangeProbability = new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0);
	mDirectionChangeProbability = new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0);
	mSignChangeProbability = new NormalizedDoubleValue(0.01, 0.0, 1.0, 0.0, 1.0);
	mMaxBias = new DoubleValue(10.0);
	mMinBias = new DoubleValue(-10.0);
	mDeviation = new DoubleValue(1.0);
	mUseGaussDistribution = new BoolValue(true);
	mAllowDynamicSignChanges = new BoolValue(true);
	mReinitializeStrengthProbability = new NormalizedDoubleValue(0.0, 0.0, 1.0, 0.0, 1.0);

	addParameter("WeightChangeProbability", mStrengthChangeProbability);
	addParameter("ChangeDirectionToggleProbability", mDirectionChangeProbability);
	addParameter("SignChangeProbability", mSignChangeProbability);
	addParameter("MaxStrength", mMaxBias);
	addParameter("MinStrength", mMinBias);
	addParameter("Deviation", mDeviation);
	addParameter("UseGaussDistribution", mUseGaussDistribution);
	addParameter("AllowDynamicSignChanges", mAllowDynamicSignChanges);
	addParameter("ReInitStrengthProbability", mReinitializeStrengthProbability);

}

ChangeSynapseStrengthOperator::ChangeSynapseStrengthOperator(const ChangeSynapseStrengthOperator &other)
	: NeuralNetworkManipulationOperator(other)
{
	mStrengthChangeProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("WeightChangeProbability"));
	mDirectionChangeProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("ChangeDirectionToggleProbability"));
	mSignChangeProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("SignChangeProbability"));
	mMinBias = dynamic_cast<DoubleValue*>(
			getParameter("MinStrength"));
	mMaxBias = dynamic_cast<DoubleValue*>(
			getParameter("MaxStrength"));
	mDeviation = dynamic_cast<DoubleValue*>(
			getParameter("Deviation"));
	mUseGaussDistribution = dynamic_cast<BoolValue*>(
			getParameter("UseGaussDistribution"));
	mAllowDynamicSignChanges = dynamic_cast<BoolValue*>(
			getParameter("AllowDynamicSignChanges"));
	mReinitializeStrengthProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("ReInitStrengthProbability"));
}

ChangeSynapseStrengthOperator::~ChangeSynapseStrengthOperator() {
}

NeuralNetworkManipulationOperator* ChangeSynapseStrengthOperator::createCopy() const
{
	return new ChangeSynapseStrengthOperator(*this);
}


bool ChangeSynapseStrengthOperator::applyOperator(Individual *individual, 
												CommandExecutor*) 
{

	NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("ChangeSynapseStrengthOperator: Could not apply "
				  "operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	QList<Synapse*> synapses = net->getSynapses();
	
	QList<Synapse*> allSynapses(synapses);
	for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
		Synapse *synapse = i.next();

		//TODO remove all unchangeable synapses
		if(synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
			|| synapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_PROTECT_STRENGTH)) 
		{
			synapses.removeAll(synapse);
			continue;
		}
	}

	double changeProbability = mStrengthChangeProbability->get();	
	double directionChangeProbability = mDirectionChangeProbability->get();
	double signChangeProbability = mSignChangeProbability->get();
	bool useGaussDistribution = mUseGaussDistribution->get();
	double deviation = mDeviation->get();
	double min = mMinBias->get();
	double max = mMaxBias->get();
	bool allowDynamicSignChanges = mAllowDynamicSignChanges->get();
	double reinitStrengthProbability = mReinitializeStrengthProbability->get();

	//change the synapse strength of some synapses 
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();

		double localChangeProb = changeProbability;

		QString changeProbString = synapse->getProperty(
						NeuralNetworkConstants::TAG_SYNAPSE_CHANGE_PROBABILITY);

		if(changeProbString != "") {
			localChangeProb = changeProbString.toDouble();
		}
		
		if(Random::nextDouble() >= localChangeProb) {
			continue;
		}


		if(Random::nextDouble() < reinitStrengthProbability) {
			// this triggers reinitialization if the initialization operator is on.
			synapse->getStrengthValue().set(0.0);
// 			synapse->setProperty(NeuralNetworkConstants::TAG_SYNAPSE_REINIT_STRENGTH);
			continue;
		}

		double strength = synapse->getStrengthValue().get();
		bool excitatory = strength >= 0.0 ? true : false;
		double previousStrength = strength;
		if(synapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_PREVIOUS_STRENGTH)) {
			previousStrength = synapse->getProperty(NeuralNetworkConstants::TAG_SYNAPSE_PREVIOUS_STRENGTH)
							.toDouble();
		}
		double difference = strength - previousStrength;

		double change = 0.0;
		double localDeviation = deviation;
		
		QString deviationString = synapse->getProperty(NeuralNetworkConstants::TAG_SYNAPSE_CHANGE_DEVIATION);
		if(deviationString != "") {
			localDeviation = deviationString.toDouble();
		}

		if(useGaussDistribution) {
			change = Math::abs(Math::getNextGaussianDistributedValue(localDeviation));
		}
		else {
			change = Math::abs(Math::getNextUniformlyDistributedValue(localDeviation));
		}
		if(difference == 0.0) {
			difference = Random::nextSign();
		}

		if(difference < 0.0) {
			change = -change;
		}

		if(Random::nextDouble() < directionChangeProbability) {
			change = -change;
		}

		double newStrength = 0.0;
		bool changeSign = Random::nextDouble() < signChangeProbability;

		bool ok = false;
		while(!ok) {
			newStrength = strength + change;
			ok = true;

			if(changeSign) {
				if((newStrength < 0.0 && strength < 0.0)
					|| (newStrength >= 0.0 && strength >= 0.0))
				{
					newStrength = -newStrength;
					excitatory = !excitatory;
				}
			}
			
			if(!allowDynamicSignChanges) {
				if(excitatory && newStrength <= 0.0) {
					change = change / 2.0;
				}
				else if(!excitatory && newStrength >= 0.0) {
					change = change / 2.0;
				}
				else {
					break;
				}
	
				if(change != 0.0) {
					ok = false;
				}
			}
		}

		//fit bias to range and set to neuron.
		double maxStrength = max;
		double minStrength = min;

		//check if the neuron strength is forced to a spefic range.
		if(synapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_MAX_STRENGTH)) {
			bool ok = false;
			double m = synapse->getProperty(NeuralNetworkConstants::TAG_SYNAPSE_MAX_STRENGTH)
									.toDouble(&ok);
			if(ok) {
				maxStrength = m;
			}
		}
		if(synapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_MIN_STRENGTH)) {
			bool ok = false;
			double m = synapse->getProperty(NeuralNetworkConstants::TAG_SYNAPSE_MIN_STRENGTH)
									.toDouble(&ok);
			if(ok) {
				minStrength = m;
			}
		}

		//set strength
		synapse->getStrengthValue().set(Math::min(maxStrength, Math::max(minStrength, newStrength)));
		synapse->setProperty(NeuralNetworkConstants::TAG_SYNAPSE_PREVIOUS_STRENGTH,
							QString::number(strength));

		//mark as modified.
		synapse->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
	}

	return true;
}


}


