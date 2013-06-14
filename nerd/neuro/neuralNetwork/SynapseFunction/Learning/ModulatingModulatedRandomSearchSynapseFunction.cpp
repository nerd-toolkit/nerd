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

#include "ModulatingModulatedRandomSearchSynapseFunction.h"
#include <iostream>
#include <Math/Math.h>
#include <Math/Random.h>
#include "Network/Neuron.h"
#include <Network/Neuro.h>
#include <NeuralNetworkConstants.h>
#include <QStringList>
#include <Core/Core.h>

using namespace std;

namespace nerd {
	
	
	/**
	* Constructor.
	*/
	ModulatingModulatedRandomSearchSynapseFunction::ModulatingModulatedRandomSearchSynapseFunction()
		: NeuroModulatorSynapseFunction("MRS2"), mOwner(0), 
		  mCurrentNetwork(0), mNumberOfStepsWithoutModulators(0)
	{
		mTypeParameters = new StringValue("1, 1.0, 0.2, 0, 3.0, -1.5, 1.5");
		mTypeParameters->setDescription("Parameter Settings: NM-type, change probability, "
						"disable probability, mode, opt1, opt2, opt3, ...|<second entry>|<third>|..."
						"\nModes: 0 pure random"
						"\n       1 backtracking random");

		mProbabilityForChange = new DoubleValue(0.5);
		mProbabilityForChange->setDescription("Reference probability for changes.");
		
		mActive = new BoolValue(true);
		mActive->setDescription("If false, then the synapse has no effect and does also not change.");
		
		mActivationObservable = new DoubleValue(0);
		mActivationObservable->addValueChangedListener(this);
		
		addObservableOutput("Active", mActivationObservable);

		addParameter("Settings", mTypeParameters);
		addParameter("Probability", mProbabilityForChange);
		addParameter("Active", mActive);
		
		mNetworkManager = Neuro::getNeuralNetworkManager();
		
		updateSettings();
	}


	/**
	* Copy constructor.
	*/
	ModulatingModulatedRandomSearchSynapseFunction::ModulatingModulatedRandomSearchSynapseFunction(
											const ModulatingModulatedRandomSearchSynapseFunction &other)
		: Object(), ValueChangedListener(), NeuroModulatorElement(other), NeuroModulatorSynapseFunction(other), 
		  mOwner(0), mCurrentNetwork(0), mNumberOfStepsWithoutModulators(0)
	{
		mObservables.clear();
		
		mTypeParameters = dynamic_cast<StringValue*>(getParameter("Settings"));
		mProbabilityForChange = dynamic_cast<DoubleValue*>(getParameter("Probability"));
		mActive = dynamic_cast<BoolValue*>(getParameter("Active"));
		
		mActivationObservable = new DoubleValue(mActive->get() ? 1.0 : 0.0);
		mActivationObservable->addValueChangedListener(this);
		addObservableOutput("Active", mActivationObservable);

		for(QHashIterator<QString, Value*> i(other.mObservableOutputs); i.hasNext();) {
			i.next();
			if(i.key() != "Active") {
				DoubleValue *obs = dynamic_cast<DoubleValue*>(i.value()->createCopy());  
				addObservableOutput(i.key(), obs);
				mObservables.append(obs);
			}
		}
		
		mNetworkManager = Neuro::getNeuralNetworkManager();
		
		updateSettings();
	}


	/**
	* Destructor.
	*/
	ModulatingModulatedRandomSearchSynapseFunction::~ModulatingModulatedRandomSearchSynapseFunction() {
		mParameters.clear();
		mObservableOutputs.clear();
		while(!mObservables.empty()) {
			DoubleValue *observable = mObservables.front();
			mObservables.removeAll(observable);
			delete observable;
		}
		delete mActivationObservable;
	}


	/**
	 * Creates a copy of this SynapseFunction.
	 */
	SynapseFunction* ModulatingModulatedRandomSearchSynapseFunction::createCopy() const {
			return new ModulatingModulatedRandomSearchSynapseFunction(*this);
	}


	/**
	 * If the main parameters have been changed, then the internal settings are updated
	 * to fit the new settings.
	 */
	void ModulatingModulatedRandomSearchSynapseFunction::valueChanged(Value *value) {
		NeuroModulatorSynapseFunction::valueChanged(value);
		if(value == 0) {
			return;
		}
		else if(value == mTypeParameters) {
			updateSettings();
		}
		else if(value == mActive) {
			//enableWeight(mOwner, !mActive->get());
			mActivationObservable->set(mActive->get() ? 1.0 : 0.0);
		}
		else if(value == mActivationObservable) {
			enableWeight(mOwner, mActivationObservable->get() > 0.5);
		}
	}


	/**
	* Updates the parameter settings by parsing the parameter string anew.
	*/
	void ModulatingModulatedRandomSearchSynapseFunction::reset(Synapse *synapse) {
		mOwner = synapse;
		mCurrentNetwork = 0;
		updateSettings();
		mActivationObservable->set(mActive->get() ? 1.0 : 0.0);
		enableWeight(mOwner, mActive->get());
	}


	/**
	* Returns the strength of the owner synapse. Before that, the random learning is triggered,
	* so the weight may be changed before the weight is returned.
	* 
	* @param owner the owner of this SynapseFunction.
	* @return the strength of the owner.
	*/
	double ModulatingModulatedRandomSearchSynapseFunction::calculate(Synapse *owner) {
		mOwner = owner;
		mCurrentNetwork = 0;
		if(owner == 0 || owner->getSource() == 0) {
			return 0.0;
		}
		
		mCurrentNetwork = owner->getSource()->getOwnerNetwork();
		if(mCurrentNetwork == 0) {
			return owner->getStrengthValue().get();
		}
		
		
		if(mActivationObservable->get() < 0.5 && mActive->get()) {
			mActive->set(false);
		}
		else if(mActivationObservable->get() > 0.5 && !mActive->get()) {
			mActive->set(true);
		}
		
		
		//only execute modulation if the global plasticity hint allows it.
		if(mNetworkManager->getDisablePlasticityValue()->get() == false) {
			for(int i = 0; i < mParameters.size(); ++i) {
			
				ModulatingModulatedRandomSearchParameters &params = mParameters[i];

				switch(params.mMode) {
					case 0:
					default:
						randomSearchModeMultiVariant(owner, params);
				}
			}
		}
		
		if(!mActive->get()) {
			return 0.0;
		}
		
		return owner->getStrengthValue().get();
	}


	/**
	 * TODO: not complete yet, but no function is using it right now.
	 */
	bool ModulatingModulatedRandomSearchSynapseFunction::equals(SynapseFunction *synapseFunction) const {
		if(NeuroModulatorSynapseFunction::equals(synapseFunction) == false) {
			return false;
		}

		ModulatingModulatedRandomSearchSynapseFunction *sf = 
					dynamic_cast<ModulatingModulatedRandomSearchSynapseFunction*>(synapseFunction);

		if(sf == 0) {
			return false;
		}
		if(!sf->mTypeParameters->equals(mTypeParameters)) {
			return false;
		}
		if(!sf->mProbabilityForChange->equals(mProbabilityForChange)) {
			return false;
		}
		return true;
	}


	/**
	 * Called whenever the parameter settings string has to be parsed to get the
	 * configuration of the random learning rule.
	 * 
	 * This method parses the configuration string according to the following format:
	 * type, changeProb, disableProb, mode, opt1, op2, ...|type2, changeProb2, ...
	 * 
	 * So, multiple parameter sets can be configured to allow the reaction to different
	 * neuro-modulators with different modes and parameter settings.
	 * 
	 * The first four parameters are mandatory, all others (arbitrarily long list of double params)
	 * depend on the chosen mode. 
	 * 
	 * There will be error messages to the console and the log file if parsing fails
	 * or later if the number of optional parameters does not fit to the chosen mode.
	 * 
	 * Each new setting is associated with an own observable to allow an observation of the 
	 * detected related modulator concentration. If the number of settings is reduced, then 
	 * the created observables will NOT be deleted to prevent missing objects in plotters 
	 * and loggers. Also, the role of the observables can change, if the order of settings is changed.
	 * This has to be considered when observing observables after setting changes.
	 */
	void ModulatingModulatedRandomSearchSynapseFunction::updateSettings() {
		

		//enable one-shot error messages to prevent a flooding of the console / log file.
		mNotifiedErrors = false; 
		
		for(QListIterator<DoubleValue*> i(mObservables); i.hasNext();) {
			i.next()->set(0.0);
		}
		mParameters.clear();
		
		QStringList entries = mTypeParameters->get().split("|");
		
		for(int i = 0; i < entries.size(); ++i) {
			QString entry = entries.at(i);
			QStringList paramStrings = entry.split(",");
			
			if(paramStrings.length() < 4) {
				Core::log("ModulatingModulatedRandomSearchSynapseFunction: Too few "
						  "parameters in [" + entry + "]", true);
				continue;
			}
			
			bool ok = true;
			
			ModulatingModulatedRandomSearchParameters params;
			
			params.mType = paramStrings.at(0).toInt(&ok);
			if(!ok) {
				Core::log("ModulatingModulatedRandomSearchSynapseFunction: Could not parse "
						  "type (1) in [" + entry + "]", true);
				continue;
			}
			params.mChangeProbability = paramStrings.at(1).toDouble(&ok);
			if(!ok) {
				Core::log("ModulatingModulatedRandomSearchSynapseFunction: Could not parse "
						  "change probability (2) in [" + entry + "]", true);
				continue;
			}
			params.mDisableProbability = paramStrings.at(2).toDouble(&ok);
			if(!ok) {
				Core::log("ModulatingModulatedRandomSearchSynapseFunction: Could not parse "
						  "disable probability (3) in [" + entry + "]", true);
				continue;
			}
			params.mMode = paramStrings.at(3).toInt(&ok);
			if(!ok) {
				Core::log("ModulatingModulatedRandomSearchSynapseFunction: Could not parse "
						  "mode (4) in [" + entry + "]", true);
				continue;
			}
			
			//get the optional parameters (all have to be of type double)
			for(int j = 4; j < paramStrings.size(); ++j) {
				double param = paramStrings.at(j).toDouble(&ok);
				if(!ok) {
					Core::log("ModulatingModulatedRandomSearchSynapseFunction: Could not parse optional parameter " 
							+ QString::number(j - 3) + "  (" + QString::number(j) + ") in ["
							+ entry + "]", true);
							continue;
				}
				else {
					params.mParams.append(param);
				}
			}
			
			
			//(re-)assign observables to the settings.
			if(i < mObservables.size()) {
				params.mObservable = mObservables.at(i);
			}
			else {
				DoubleValue *newObservable = new DoubleValue();
				params.mObservable = newObservable;
				
				addObservableOutput("Observable" + QString::number(i) + "_T" + QString::number(params.mType), newObservable);
				
				mObservables.append(newObservable);
			}
			mParameters.append(params);
		}
	}
	

	/**
	 * MultiVariant Mode.
	 * 
	 * This mode does combineseveral variants:
	 * 
	 * 
	 * Random Walk:
	 * ----------------------------------------------
	 * 
	 * Reduced Value Set (with noise):
	 * ----------------------------------------------
	 * 
	 * Topology Search:
	 * ----------------------------------------------
	 * 
	 * Connectivity Density Control:
	 * ----------------------------------------------
	 * 
	 * Local Modulation with Types: inclusive
	 * ----------------------------------------------
	 * 
	 * Synapse Protection (refraction phases):
	 * ----------------------------------------------
	 * 
	 * Activity Dependent Change Probabilities:
	 * ----------------------------------------------
	 * 
	 * Refinement with Backtracking:
	 * ----------------------------------------------
	 * 
	 * 1) if ALL other neuro-modulators are not present for a given number of steps. 
	 *   In this case, the current weight setting and disable/enable state are considerd
	 *   well working and are stored for a later backtracking.
	 * 
	 * 2) if the given neuro-modulator type is active and there is backtracking information
	 *   available, then that information is used to change the weight back towards its 
	 *   previously known (well working?) way.
	 * 
	 * In case (2) the way the backtracking is performed can be adjusted with the backtracking-mode 
	 * parameter of this operation. Currently, there are the following modes:
	 * 
	 * 0: If backtracking becomes active, the last known state is directly and completely restored.
	 */

	//TODO Add an option to occassionally flip back to previous value (local search). Probability for 
	//     flip-back should decrease with higher level, with a full backtracking above a threshold.
	void ModulatingModulatedRandomSearchSynapseFunction::randomSearchModeMultiVariant(
		Synapse *owner, ModulatingModulatedRandomSearchParameters &params) 
	{
		if(owner == 0) {
			return;
		}
		
		//check if the number of parameters is valid.
		if(params.mParams.size() != 2) {
			if(mNotifiedErrors == false) {
				mNotifiedErrors = true;
				
				Core::log(QString("ModulatingModulatedRandomSearchSynapseFunction: ")
				+ "Mode 1 requires exactly 2 optional parameters: " 
				  + "stepsBeforeMemo, backtracking_mode. Found " 
					+ QString::number(params.mParams.size()) 
					+ " instead!", true);
			}
			return;
		}
		
		//check if there is modulator of any relevant type:
		bool noModulatorDetected = true;
		for(int i = 0; i < mParameters.size(); ++i) {
			
			ModulatingModulatedRandomSearchParameters &paramSet = mParameters[i];
			if(paramSet.mMode == 1) {
				//this modulator is of a backtracking type... not a regular modulation.
				continue;
			}
			
			double concentration = NeuroModulator::getConcentrationInNetworkAt(
					paramSet.mType, owner->getPosition(), mCurrentNetwork);
			
			if(concentration > 0.0) {
				noModulatorDetected = false;
				break;
			}
		}
		
		//udpate current modulator concentration.
		double concentration = NeuroModulator::getConcentrationInNetworkAt(
			params.mType, owner->getPosition(), mCurrentNetwork);
		
		if(params.mObservable != 0) {
			params.mObservable->set(concentration);
		}
		
		
		if(noModulatorDetected) {
			//check if we should memorize the setting as known working solution (in the broader context at least).
			++mNumberOfStepsWithoutModulators;
			
			int numberOfWorkingStepsBeforeMemorization = params.mParams.at(0);
			
			if(mNumberOfStepsWithoutModulators >= numberOfWorkingStepsBeforeMemorization) {
				mKnownPersistentActiveStates.clear();
				mKnownPersistentActiveStates.append(mActive->get());
				
				mKnownPersistentSettings.clear();
				mKnownPersistentSettings.append(owner->getStrengthValue().get());
				
				mNumberOfStepsWithoutModulators = 0;
				
				owner->setProperty("Memory:", QString::number(mKnownPersistentSettings.last()) 
								+ "," + QString::number((int) mKnownPersistentActiveStates.last()));
			}
		}
		else {
			mNumberOfStepsWithoutModulators = 0;
			
			if(mKnownPersistentActiveStates.empty()) {
				//cannot perform a backtracking if there are no previously memorized settings.
				return;
			}
			
			double changeProbability = Math::max(0.0, Math::min(1.0, 
																concentration * params.mChangeProbability));
			
			int backtrackingMode = (int) params.mParams.at(1); //backtracking mode
			
			//cerr << "Got: " << disableProbability << " and " << changeProbability << " of " << params.mType << endl;
			
			if(Random::nextDouble() < changeProbability) {
				
				//simple, full backtracking to the last known state.
				if(backtrackingMode == 0) {
					//make sure the synapse is in the same enable/disable state as in the last known working configuration.
					if(mActive->get() != mKnownPersistentActiveStates.last()) {
						mActive->set(mKnownPersistentActiveStates.last());
						
						//don't do it twice. Changing mActive alreay calls this method!
						//enableWeight(owner, !mActive->get());
						
						incrementDisableChangeCounter(owner);
					}
					if(owner->getStrengthValue().get() != mKnownPersistentSettings.last()) {
						owner->getStrengthValue().set(mKnownPersistentSettings.last());
					}
				}
			}
		}
	}
	
	void ModulatingModulatedRandomSearchSynapseFunction::performTopologySearch(
						Synapse *owner, ModulatingModulatedRandomSearchParameters &params)
	{
		
		
	}
	
	void ModulatingModulatedRandomSearchSynapseFunction::performRandomWalkWeightChanges(
						Synapse *owner, ModulatingModulatedRandomSearchParameters &params)
	{
		
	}
	
	
	void ModulatingModulatedRandomSearchSynapseFunction::performReducedRandomWalkWeightChanges(
						Synapse *owner, ModulatingModulatedRandomSearchParameters &params)
	{
		
	}
	
	
	void ModulatingModulatedRandomSearchSynapseFunction::performBacktracking(
						Synapse *owner, ModulatingModulatedRandomSearchParameters &params)
	{
		
	}
	
	/*
	void ModulatingModulatedRandomSearchSynapseFunction::ramdonizeWeightAndActiveState(
						Synapse *owner, double concentration, double variance, double minValue, 
						double maxValue, bool useValueSet, double maxConcentration) 
	{
		if(owner == 0) {
			return;
		}
		if(params.mParams.size() < 3 && params.mParams.size() > 4) {
			if(mNotifiedErrors == false) {
				mNotifiedErrors = true;
				
				Core::log(QString("SimpleModulatedRandomSearchSynapseFunction: ")
						  + "Mode 0 requires 3 or 4 optional parameters: " 
						  + "variance, min, max, (maxLevel). Found " 
						  + QString::number(params.mParams.size()) 
						  + " instead!", true);
			}
			return;
		}
		
		double concentration = NeuroModulator::getConcentrationInNetworkAt(
						params.mType, owner->getPosition(), mCurrentNetwork);
		
		//If the maxConcentration parameter is given, cut the concentration level!
		if(params.mParams.size() > 3) {
			concentration = Math::min(params.mParams.at(3), concentration);
		}
			
		if(params.mObservable != 0) {
			params.mObservable->set(concentration); 
		}
		
		double disableProbability = Math::max(0.0, Math::min(1.0, 
						concentration * params.mDisableProbability * mProbabilityForChange->get()));
		
		double changeProbability = Math::max(0.0, Math::min(1.0, 
						concentration * params.mChangeProbability * mProbabilityForChange->get()));
		
		if(Random::nextDouble() < disableProbability) {
			mInactive->set(!mInactive->get());
			
			//don't do it twice!
			//enableWeight(owner, !mInactive->get());
			
			incrementDisableChangeCounter(owner);
		}
		
		//change the weight by chance or when the synapse is set to 0.0.
		if(!mInactive->get() && ((Random::nextDouble() < changeProbability) || (owner->getStrengthValue().get() == 0.0))) {
			
			double variance = params.mParams.at(0);
			double min = params.mParams.at(1);
			double max = params.mParams.at(2);

// 			double newWeight = Math::max(min, Math::min(max, 
// 								owner->getStrengthValue().get() 
// 									+ (Random::nextDoubleBetween(-variance, variance))));
			
			//if the new weight is out of range, then do not saturate, but flip to the other side
			//of the range. This should avoid that weights tend to be saturated at the max and min values.
			double newWeight = owner->getStrengthValue().get() + (Random::nextDoubleBetween(-variance, variance));
			
			while(newWeight > max) {
				newWeight -= (max - min);
			}
			while(newWeight < min) {
				newWeight += (max - min);
			}
			
			owner->getStrengthValue().set(newWeight);
			
			incrementWeightChangeCounter(owner);
		}
	}
	*/
	
	
	
	/**
	 * Increments the property of the synapse that holds the number of disable/enable changes.
	 * This property is used for debugging and network analysis to detect the more and less
	 * plastic synapses.
	 */
	int ModulatingModulatedRandomSearchSynapseFunction::incrementDisableChangeCounter(Synapse *owner) {
		if(owner == 0) {
			return 0;
		}
		
		bool ok = true;
		int countDisables = owner->getProperty("MRS-D").toInt(&ok);
		if(!ok) {
			countDisables = 0;
		}
		countDisables++;
		owner->setProperty("MRS-D", QString::number(countDisables));
		
		return countDisables;
	}
	
	
	/**
	 * Increments the property of the synapse that holds the number of weight changes.
	 * This property is used for debugging and network analysis to detect the more and less
	 * plastic synapses.
	 */
	int ModulatingModulatedRandomSearchSynapseFunction::incrementWeightChangeCounter(Synapse *owner) {
		if(owner == 0) {
			return 0;
		}
		
		bool ok = true;
		int countChanges = owner->getProperty("MRS-M").toInt(&ok);
		if(!ok) {
			countChanges = 0;
		}
		countChanges++;
		owner->setProperty("MRS-M", QString::number(countChanges));
		
		return countChanges;
	}
	
	
	/**
	 * When a synapse is disabled, then its weigth is set to 0.0 (to allow cloning synapses to disable as well)
	 * and stored as property. When the synapse is enabled again, then that stored value is recovered (if present).
	 * 
	 * Note: Because of the conversions, the accuracy of the disabled synapse may change. But this is usually not
	 * a problem. 
	 */
	void ModulatingModulatedRandomSearchSynapseFunction::enableWeight(Synapse *owner, bool enable) {
		if(owner == 0) {
			return ;
		}
		
		
		if(enable) {
			bool ok = true;
			
			double oldWeight = owner->getProperty("_SMRS_w").toDouble(&ok);
			
			if(ok) {
				owner->getStrengthValue().set(oldWeight);
			}
			owner->removeProperty(NeuralNetworkConstants::TAG_ELEMENT_DRAW_AS_DISABLED);
			owner->removeProperty("_SMRS_w");
		}
		else {
			if(!owner->hasProperty("_SMRS_w")) {
				double currentWeight = owner->getStrengthValue().get();
				owner->getStrengthValue().set(0.0);
				owner->setProperty("_SMRS_w", QString::number(currentWeight));
			}
			owner->setProperty(NeuralNetworkConstants::TAG_ELEMENT_DRAW_AS_DISABLED);
		}
		mActive->set(enable);
		
	}
	
	
}


