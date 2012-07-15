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

#include "RandomizationConstraint.h"
#include "Math/Random.h"
#include <Core/Core.h>
#include "Network/Neuron.h"
#include "Network/Synapse.h"
#include "ActivationFunction/ActivationFunction.h"
#include "TransferFunction/TransferFunction.h"
#include "SynapseFunction/SynapseFunction.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <iostream>
#include "NeuralNetworkConstants.h"
#include <QTime>


using namespace std;

namespace nerd {


RandomizationConstraint::RandomizationConstraint(int minNumberOfNeurons, int maxNumberOfNeurons) 
	: GroupConstraint("Randomization"), mGlobalBiasRange(0), mGlobalOutputRange(0),
		mGlobalActivationRange(0), mGlobalWeightRange(0), mIndividualRanges(0),
		mStoreRandomizedValuesInNetwork(0), mApplyStoredValuesFromNetwork(0),
		mOneShotRandomization(0), mLastOneShotCount(-1)
{
	mGlobalBiasRange = new RangeValue(-0.1, 0.1);
	mGlobalOutputRange = new RangeValue(-1, 1);
	mGlobalActivationRange = new RangeValue(0, 0);
	mGlobalWeightRange = new RangeValue(-0.1, 0.1);
	//mIndividualRanges = new StringValue();
	mStoreRandomizedValuesInNetwork = new BoolValue(true);
	mApplyStoredValuesFromNetwork = new BoolValue(false);
	
	addParameter("GlobalBiasRange", mGlobalBiasRange);
	addParameter("GlobalOutputRange", mGlobalOutputRange);
	addParameter("GlobalActivationRange", mGlobalActivationRange);
	addParameter("GlobalWeightRange", mGlobalWeightRange);
	//addParameter("IndividualRanges", mIndividualRanges);
	addParameter("StoreValuesInNetwork", mStoreRandomizedValuesInNetwork);
	addParameter("RestoreValuesFromNetwork", mApplyStoredValuesFromNetwork);
	
	//only if an analyzer counter variable was found!
	mAnalyzerRunCounter = Core::getInstance()->getValueManager()->getIntValue(
							NeuralNetworkConstants::VALUE_ANALYZER_RUN_COUNTER);
	
	if(mAnalyzerRunCounter != 0) {
		
		mOneShotRandomization = new BoolValue(true);
		mOneShotRandomization->setDescription("If true, then this constraint is only run the first "
					"time it is called for each analyzer run.");
		addParameter("SingleShot", mOneShotRandomization);
	}
}


RandomizationConstraint::RandomizationConstraint(const RandomizationConstraint &other)
	: Object(), ValueChangedListener(), GroupConstraint(other), mGlobalBiasRange(0), 
		mGlobalOutputRange(0), 		mGlobalActivationRange(0), mGlobalWeightRange(0), 
		mIndividualRanges(0), mStoreRandomizedValuesInNetwork(0), mApplyStoredValuesFromNetwork(0),
		mOneShotRandomization(0), mLastOneShotCount(-1)
{
	mGlobalBiasRange = dynamic_cast<RangeValue*>(getParameter("GlobalBiasRange"));
	mGlobalOutputRange = dynamic_cast<RangeValue*>(getParameter("GlobalOutputRange"));
	mGlobalActivationRange = dynamic_cast<RangeValue*>(getParameter("GlobalActivationRange"));
	mGlobalWeightRange = dynamic_cast<RangeValue*>(getParameter("GlobalWeightRange"));
	//mIndividualRanges = dynamic_cast<StringValue*>(getParameter("IndividualRanges"));
	mStoreRandomizedValuesInNetwork = dynamic_cast<BoolValue*>(getParameter("StoreValuesInNetwork"));
	mApplyStoredValuesFromNetwork = dynamic_cast<BoolValue*>(getParameter("RestoreValuesFromNetwork"));
	mOneShotRandomization = dynamic_cast<BoolValue*>(getParameter("SingleShot"));
	
	//only if an analyzer counter variable was found!
	mAnalyzerRunCounter = Core::getInstance()->getValueManager()->getIntValue(
							NeuralNetworkConstants::VALUE_ANALYZER_RUN_COUNTER);
	
	if(mAnalyzerRunCounter != 0) {
		if(mOneShotRandomization == 0) {
			mOneShotRandomization = new BoolValue(true);
			addParameter("SingleShot", mOneShotRandomization);
		}
	}
}

RandomizationConstraint::~RandomizationConstraint() {
}

GroupConstraint* RandomizationConstraint::createCopy() const {
	return new RandomizationConstraint(*this);
}


bool RandomizationConstraint::isValid(NeuronGroup*) {
	return true;
}


bool RandomizationConstraint::applyConstraint(NeuronGroup *owner, CommandExecutor*,
												QList<NeuralNetworkElement*> &trashcan) 
{

	if(owner == 0 || owner->getOwnerNetwork() == 0) {
		mErrorMessage = "Owner ModularNeuralNetwork or owner NeuronGroup has been NULL";
		return false;
	}
	
	QString executionMarker = NeuralNetworkConstants::PROP_PREFIX_CONSTRAINT_TEMP
								+ "RandRun";
	
	if(owner->hasProperty(executionMarker)) {
		//has already been executed during this resolver run.
		return true;
	}
	owner->setProperty(executionMarker);
	
	if(mAnalyzerRunCounter != 0 
		&& mOneShotRandomization != 0 
		&& mOneShotRandomization->get()) 
	{
		//check if the randomizer has already been executed.
		if(mAnalyzerRunCounter->get() == mLastOneShotCount) {
			
			//do nothing.
			return true;
		}
		
		//update run counter
		mLastOneShotCount = mAnalyzerRunCounter->get();
	}
	

	ModularNeuralNetwork *net = owner->getOwnerNetwork();
	
	Random random;
	int seed = Random::nextInt();  //take seed from global randomization pool
	if(mApplyStoredValuesFromNetwork->get()) {
		if(!owner->hasProperty("RANDOMIZATION_SEED")) {
			//do not change anything!
			mWarningMessage = QString("Could not find a stored randomization seed in neuron group [")
								+ QString::number(owner->getId()) + "]. Skipping Randomization.";
			return true;
		}
		QString seedString = owner->getProperty("RANDOMIZATION_SEED");
		bool ok = true;
		int newSeed = seedString.toInt(&ok);
		if(!ok) {
			//do not change anything!
			mWarningMessage = QString("Could not parse stored randomization seed in neuron group [")
								+ QString::number(owner->getId()) + "]. Skipping Randomization.";
			return true;
		}
		seed = newSeed;
	}
	random.mSetSeed(seed);
	
	if(mStoreRandomizedValuesInNetwork->get()) {
		owner->setProperty("RANDOMIZATION_SEED", QString::number(seed));
	}
	
	
	QList<Neuron*> neurons = owner->getNeurons();
	
	NeuroModule *ownerModule = dynamic_cast<NeuroModule*>(owner);
	if(ownerModule != 0) {
		neurons = ownerModule->getAllEnclosedNeurons();
	}


	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		
		if(mGlobalActivationRange->getMin() != 0.0 
			&& mGlobalActivationRange->getMax() != 0.0) 
		{
			neuron->getActivationValue().set(
				random.mNextDoubleBetween(mGlobalActivationRange->getMin(), 
										    mGlobalActivationRange->getMax()));
		}
		if(mGlobalOutputRange->getMin() != 0.0 
			&& mGlobalOutputRange->getMax() != 0.0) 
		{
			neuron->getOutputActivationValue().set(
				random.mNextDoubleBetween(mGlobalOutputRange->getMin(), 
										    mGlobalOutputRange->getMax()));
		}
		if(mGlobalBiasRange->getMin() != 0.0 
			&& mGlobalBiasRange->getMax() != 0.0) 
		{
			neuron->getBiasValue().set(
				random.mNextDoubleBetween(mGlobalBiasRange->getMin(), 
										    mGlobalBiasRange->getMax()));
		}
		if(mGlobalWeightRange->getMin() != 0.0 
			&& mGlobalWeightRange->getMax() != 0.0) 
		{
			QList<Synapse*> synapses = neuron->getSynapses();
			
			for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
				
				j.next()->getStrengthValue().set(
					random.mNextDoubleBetween(mGlobalWeightRange->getMin(), 
											  mGlobalWeightRange->getMax()));
				
			}
		}
	}
	
	/*
	if(neurons.size() > max) {
		if(autoAdapt) {
			while(neurons.size() > max && max >= 0) {
				Neuron *newestNeuron = 0;
				int newestDate = -1;
				for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
					Neuron *neuron = i.next();
					if(newestNeuron == 0) {
						newestNeuron = neuron;
					}
					QString dateString = 
							neuron->getProperty(NeuralNetworkConstants::TAG_CREATION_DATE);
					if(dateString != "") {
						int date = dateString.toInt();
						if(date < newestDate) {
							continue;
						}
						newestDate = date;
						newestNeuron = neuron;
					}
					else if(newestDate == -1) {
						newestNeuron = neuron;
					}
				}
				if(newestNeuron == 0) {
					return false;
				}
				trashcan << net->savelyRemoveNetworkElement(newestNeuron);
				neurons.removeAll(newestNeuron);
			}
			return false;
		}
		else {
			mErrorMessage = "Too many neurons found in this group.";
			return false;
		}
	}

	if(neurons.size() < min) {
		if(autoAdapt) {

			ActivationFunction *af = net->getDefaultActivationFunction();
			TransferFunction *tf = net->getDefaultTransferFunction();

			while(neurons.size() < min) {
				Neuron *neuron = new Neuron("", *tf, *af);
				net->addNeuron(neuron);
				owner->addNeuron(neuron);
				neurons.append(neuron);
			}
			return true;
		}
		else {
			mErrorMessage = "Too few neurons found in this group.";
			return false;
		}
	}*/

	return true;
}


bool RandomizationConstraint::equals(GroupConstraint *constraint) {
	if(GroupConstraint::equals(constraint) == false) {
		return false;
	}
	RandomizationConstraint *c = dynamic_cast<RandomizationConstraint*>(constraint);
	if(c == 0) {
		return false;
	}
	if(!mGlobalBiasRange->equals(c->mGlobalBiasRange)) {
		return false;
	}
	if(!mGlobalActivationRange->equals(c->mGlobalActivationRange)) {
		return false;
	}
	if(!mGlobalOutputRange->equals(c->mGlobalOutputRange)) {
		return false;
	}
	if(!mGlobalWeightRange->equals(c->mGlobalWeightRange)) {
		return false;
	}
	if(!mIndividualRanges->equals(c->mIndividualRanges)) {
		return false;
	}
	if(!mStoreRandomizedValuesInNetwork->equals(c->mStoreRandomizedValuesInNetwork)) {
		return false;
	}
	if(!mApplyStoredValuesFromNetwork->equals(c->mApplyStoredValuesFromNetwork)) {
		return false;
	}


	return true;
}


}


