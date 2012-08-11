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
		mRandomizationHistorySize(0),
		mOneShotRandomization(0), mLastOneShotCount(-1), mLastSeed(0)
{
	mGlobalBiasRange = new RangeValue(-0.1, 0.1);
	mGlobalOutputRange = new RangeValue(-1, 1);
	mGlobalActivationRange = new RangeValue(0, 0);
	mGlobalWeightRange = new RangeValue(-0.1, 0.1);
	//mIndividualRanges = new StringValue();
	
	mStoreRandomizedValuesInNetwork = new BoolValue(true);
	mStoreRandomizedValuesInNetwork->setDescription("If true, then the seed for the randomization is stored "
						"in the neuron-group to allow a replication of the randomization. ");
	mApplyStoredValuesFromNetwork = new BoolValue(false);
	mApplyStoredValuesFromNetwork->setDescription("If true, then no real randomization takes place. "
						"Instead, the last stored randomization seed is used to set the randomized "
						"values. ");
	mRandomizationHistorySize = new IntValue(10);
	mRandomizationHistorySize->setDescription("The number of past randomization seeds to store when "
						"seed storing is enabled. ");
	
	
	addParameter("GlobalBiasRange", mGlobalBiasRange);
	addParameter("GlobalOutputRange", mGlobalOutputRange);
	addParameter("GlobalActivationRange", mGlobalActivationRange);
	addParameter("GlobalWeightRange", mGlobalWeightRange);
	//addParameter("IndividualRanges", mIndividualRanges);
	addParameter("StoreValuesInNetwork", mStoreRandomizedValuesInNetwork);
	addParameter("RestoreValuesFromNetwork", mApplyStoredValuesFromNetwork);
	addParameter("HistorySize", mRandomizationHistorySize);
	
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
		mOneShotRandomization(0), mLastOneShotCount(-1), mLastSeed(other.mLastSeed)
{
	mGlobalBiasRange = dynamic_cast<RangeValue*>(getParameter("GlobalBiasRange"));
	mGlobalOutputRange = dynamic_cast<RangeValue*>(getParameter("GlobalOutputRange"));
	mGlobalActivationRange = dynamic_cast<RangeValue*>(getParameter("GlobalActivationRange"));
	mGlobalWeightRange = dynamic_cast<RangeValue*>(getParameter("GlobalWeightRange"));
	//mIndividualRanges = dynamic_cast<StringValue*>(getParameter("IndividualRanges"));
	mStoreRandomizedValuesInNetwork = dynamic_cast<BoolValue*>(getParameter("StoreValuesInNetwork"));
	mApplyStoredValuesFromNetwork = dynamic_cast<BoolValue*>(getParameter("RestoreValuesFromNetwork"));
	mOneShotRandomization = dynamic_cast<BoolValue*>(getParameter("SingleShot"));
	mRandomizationHistorySize = dynamic_cast<IntValue*>(getParameter("HistorySize"));
	
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
	
	//make sure that the randomization constraint is run only once per resolver run
	//to allow other constraints to resolve potentially induced conflicts.
	
	QString executionMarker = NeuralNetworkConstants::PROP_PREFIX_CONSTRAINT_TEMP
								+ "RandRun";
	
	if(owner->hasProperty(executionMarker)) {
		//has already been executed during this resolver run.
		return true;
	}
	owner->setProperty(executionMarker);
	
	
	//check if this is a single-shot randomization
	//that is executed only once per analyzer run (NetworkDynamicsAnalyzerApplication).
	//This is a special case that only works in the analyzer (or anywhere where a 
	//counter for runs is given.
	
	bool keepPreviousRandomization = false;
	
	if(mAnalyzerRunCounter != 0 
		&& mOneShotRandomization != 0 
		&& mOneShotRandomization->get()) 
	{
		//check if the randomizer has already been executed.
		if(mAnalyzerRunCounter->get() == mLastOneShotCount) {
			
			keepPreviousRandomization = true;
		}
		
		//update run counter
		mLastOneShotCount = mAnalyzerRunCounter->get();
	}
	

	//ModularNeuralNetwork *net = owner->getOwnerNetwork();
	
	
	//Set the randomization seed:
	//1) if single shot and this is not the first run, then stick to the previous seed.
	//2) else if a previously stored seed should be used, then try to get that from the network
	//3) else use a random seed
	
	Random random;
	int seed = Random::nextInt();  //take seed from global randomization pool
	
	bool restoredSeed = false;
	
	if(keepPreviousRandomization) {
		//at single shot randomization, use the seed of the first run for all other runs.
		seed = mLastSeed;
	}
	else if(mApplyStoredValuesFromNetwork->get()) {
		
		//try to restore a randomization that was stored as seed in the owner group.
		
		if(!owner->hasProperty("RANDOMIZATION_SEED")) {
			//do not change anything!
			mWarningMessage = QString("Could not find a stored randomization seed in neuron group [")
								+ QString::number(owner->getId()) + "]. Skipping Randomization.";
			return true;
		}
		QString seedRawString = owner->getProperty("RANDOMIZATION_SEED");
		QStringList seedRawList = seedRawString.split(",", QString::SkipEmptyParts);
		
		if(seedRawList.empty()) {
			mWarningMessage = QString("Could not parse stored randomization seed in neuron group [")
				+ QString::number(owner->getId()) + "]. Skipping Randomization.";
			return true;
		}
		bool ok = true;
		int newSeed = seedRawList.at(0).toInt(&ok);
		if(!ok) {
			//do not change anything!
			mWarningMessage = QString("Could not parse stored randomization seed in neuron group [")
								+ QString::number(owner->getId()) + "]. Skipping Randomization.";
			return true;
		}
		seed = newSeed;
		restoredSeed = true;
	}
	mLastSeed = seed;
	random.mSetSeed(seed);
	
	if(!restoredSeed && mStoreRandomizedValuesInNetwork->get()) {
		
		//store seed and keep older seeds up to history size.
		
		QString seedString = owner->getProperty("RANDOMIZATION_SEED");
		QStringList separateSeeds = seedString.split(",", QString::SkipEmptyParts);
		
		QString newSeedString = QString::number(seed);
		
		for(int i = 0; i < separateSeeds.size() && i < mRandomizationHistorySize->get() - 1; ++i) {
			newSeedString = newSeedString + "," + separateSeeds.at(i);
		}
		
		owner->setProperty("RANDOMIZATION_SEED", newSeedString);
	}
	
	
	
	//Collect all neurons
	//In modules, this includes all neurons of all submodules.
	
	QList<Neuron*> neurons = owner->getNeurons();
	
	NeuroModule *ownerModule = dynamic_cast<NeuroModule*>(owner);
	if(ownerModule != 0) {
		neurons = ownerModule->getAllEnclosedNeurons();
	}

	
	//Randomize all parameters of the neurons and synapses.
	

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
		
		//TODO here allow to choose which synapses to randomize (in, out, mutual)
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

	return true;
}


bool RandomizationConstraint::equals(GroupConstraint *constraint) const {
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


