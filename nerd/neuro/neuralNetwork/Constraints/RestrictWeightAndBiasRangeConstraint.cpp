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



#include "RestrictWeightAndBiasRangeConstraint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Util/Util.h"
#include "Util/NeuralNetworkUtil.h"
#include "NeuralNetworkConstants.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <Math/Math.h>

using namespace std;

namespace nerd {


/**
 * Constructs a new RestrictWeightAndBiasRangeConstraint.
 * 
 * Supported modes: sbiog
 * w (weight) or s, b (bias) or n
 * i (include incomming synapses), o (include outgoing synapses), g (include group internal synapses).
 * w and g are default, if nothing is specified. 
 * Instead of 
 * 
 */
RestrictWeightAndBiasRangeConstraint::RestrictWeightAndBiasRangeConstraint()
	: GroupConstraint("RestrictBiasAndWeightRange")
{
	mMode = new StringValue("s");
	mModeParams = new StringValue("");
	mMin = new DoubleValue(-1.0);
	mMax = new DoubleValue(1.0);
	mRecursive = new BoolValue(false);
	
	addParameter("Mode", mMode);
	addParameter("ModeParams", mModeParams);
	addParameter("Min", mMin);
	addParameter("Max", mMax);
	addParameter("Recursive", mRecursive);
}


/**
 * Copy constructor. 
 * 
 * @param other the RestrictWeightAndBiasRangeConstraint object to copy.
 */
RestrictWeightAndBiasRangeConstraint::RestrictWeightAndBiasRangeConstraint(const RestrictWeightAndBiasRangeConstraint &other)
	: Object(), ValueChangedListener(), GroupConstraint(other) 
{
	mMode = dynamic_cast<StringValue*>(getParameter("Mode"));
	mModeParams = dynamic_cast<StringValue*>(getParameter("ModeParams"));
	mMin = dynamic_cast<DoubleValue*>(getParameter("Min"));
	mMax = dynamic_cast<DoubleValue*>(getParameter("Max"));
	mRecursive = dynamic_cast<BoolValue*>(getParameter("Recursive"));
}

/**
 * Destructor.
 */
RestrictWeightAndBiasRangeConstraint::~RestrictWeightAndBiasRangeConstraint() {
}

GroupConstraint* RestrictWeightAndBiasRangeConstraint::createCopy() const {
	return new RestrictWeightAndBiasRangeConstraint(*this);
}

bool RestrictWeightAndBiasRangeConstraint::isValid(NeuronGroup*) {
	return true;
}


bool RestrictWeightAndBiasRangeConstraint::applyConstraint(NeuronGroup *owner, 
									CommandExecutor*, 
									QList<NeuralNetworkElement*> &trashcan)
{
	if(owner == 0 || owner->getOwnerNetwork() == 0) {
		//do not do anything.
		return true;
	}

	ModularNeuralNetwork *net = owner->getOwnerNetwork();

	bool inputMode = mMode->get().toLower().contains("i") ? true : false;
	bool outputMode = mMode->get().toLower().contains("o") ? true : false;
	bool groupMode = mMode->get().toLower().contains("g") ? true : false;
	bool synapseMode = (mMode->get().toLower().contains("s") 
						|| mMode->get().toLower().contains("w")) ? true : false;
	bool biasMode = (mMode->get().toLower().contains("n") 
						|| mMode->get().toLower().contains("b")) ? true : false;
	
	if(!synapseMode && !biasMode) {
		//use synapse mode as default, if not specified.
		synapseMode = true;
	}
	if(!inputMode && !outputMode && !groupMode) {
		//use group mode as default.
		groupMode = true;
	}
	
	QList<Neuron*> validExternalNeurons;
	QList<NeuronGroup*> validGroups = NeuralNetworkUtil::getGroupsFromIdList(net, mModeParams->get());
	for(QListIterator<NeuronGroup*> i(validGroups); i.hasNext();) {
		Util::addWithoutDuplicates<Neuron>(validExternalNeurons, i.next()->getAllEnclosedNeurons());
	}

	QList<Neuron*> neurons;
	QList<Synapse*> innerSynapses;
	QList<Synapse*> incommingSynapses;
	QList<Synapse*> outgoingSynapses;
	
	if(mRecursive->get()) {
		neurons = owner->getAllEnclosedNeurons();
	}
	else {
		neurons = owner->getNeurons();
	}
	
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();

		QList<Synapse*> inSynapses = neuron->getSynapses();
		for(QListIterator<Synapse*> i(inSynapses); i.hasNext();) {
			Synapse *s = i.next();
			if(neurons.contains(s->getSource())) {
				innerSynapses.append(s);
			}
			else {
				if(validExternalNeurons.empty() || validExternalNeurons.contains(s->getSource())) {
					incommingSynapses.append(s);
				}
			}
		}
		QList<Synapse*> outSynapses = neuron->getOutgoingSynapses();
		for(QListIterator<Synapse*> i(outSynapses); i.hasNext();) {
			Synapse *s = i.next();
			if(!neurons.contains(dynamic_cast<Neuron*>(s->getTarget()))) {
				if(validExternalNeurons.empty() 
						|| validExternalNeurons.contains(dynamic_cast<Neuron*>(s->getTarget()))) 
				{
					outgoingSynapses.append(s);
				}
			}
		}
	}
	

	double min = mMin->get();
	double max = mMax->get();
	
	bool networkChanged = false;
	
	if(synapseMode) {
		if(groupMode) {
			if(checkSynapses(innerSynapses, min, max)) {
				networkChanged = true;
			}
		}
		if(inputMode) {
			if(checkSynapses(incommingSynapses, min, max)) {
				networkChanged = true;
			}
		}
		if(outputMode) {
			if(checkSynapses(outgoingSynapses, min, max)) {
				networkChanged = true;
			}
		}
	}
	
	if(biasMode) {
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *n = i.next();
			double bias = n->getBiasValue().get();
			
			//change only neurons that already have a non-zero bias value.
			if(bias != 0.0) {
				if(bias >= min && bias <= max) {
					continue;
				}
				if(-bias >= min && -bias <= max) {
					bias = -bias;
				}
				else {
					bias = Math::min(max, Math::max(bias, min));
				}
				n->getBiasValue().set(bias);
				networkChanged = true;
			}
		}
	}

	return !networkChanged;
}
		
bool RestrictWeightAndBiasRangeConstraint::equals(GroupConstraint *constraint) {
	if(!GroupConstraint::equals(constraint)) {
		return false;
	}
	RestrictWeightAndBiasRangeConstraint *cc = 
					dynamic_cast<RestrictWeightAndBiasRangeConstraint*>(constraint);

	if(cc == 0) {
		return false;
	}
	return true;
}

bool RestrictWeightAndBiasRangeConstraint::checkSynapses(QList<Synapse*> &synapses, double min, double max) {
	bool networkChanged = false;
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *s = i.next();
		double weight = s->getStrengthValue().get();
		if(weight >= min && weight <= max) {
			continue;
		}
		if(-weight >= min && -weight <= max) {
			weight = -weight;
		}
		else {
			weight = Math::min(max, Math::max(weight, min));
		}
		s->getStrengthValue().set(weight);
		networkChanged = true;
	}
	return networkChanged;
}


}



