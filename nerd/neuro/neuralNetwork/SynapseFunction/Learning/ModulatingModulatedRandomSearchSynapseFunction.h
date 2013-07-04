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


#ifndef NERDModulatingModulatedRandomSearchSynapseFunction_H
#define NERDModulatingModulatedRandomSearchSynapseFunction_H

#include "SynapseFunction/NeuroModulatorSynapseFunction.h"
#include "Value/RangeValue.h"
#include "Value/CodeValue.h"

namespace nerd {
	
	/**
	 * Contains the parameters and local variables for the configuration of a single modulator
	 * type description. ModulatingModulatorRandomSearchSynapseFunction supports the definition
	 * of several modulator type descriptions in the same functions, so that each synapse can
	 * react on different types of neuromodulation in a distinct way.
	 * 
	 * Parameters:
	 * - mType: the neuromodulator type to react to
	 * - mChangeProbability: the probability of (weight) changes caused by the given modulator type.
	 * - mDisableProbability: the probability to disable or reenable a synapse (controls the active flag of the SF)
	 * - mDesiredConnectivityDensity: the preferred connectivity density of the target neuron.
	 * - mValueList: the (optional) fixed value set to choose weights from.
	 * - mCurrentValueListIndex: the current position within the weight set vector.
	 * 
	 * Helper variables:
	 * - mObservable: The observable object corresponding to the measured modulator concentration of this parameter set.
	 */
	struct ModulatingModulatedRandomSearchParameters {
	public:
		ModulatingModulatedRandomSearchParameters() : mType(0), mChangeProbability(0), mMaximalIndexChange(1), 
			mNoiseStrength(0.0), mDisableProbability(0), mDesiredConnectivityDensity(-1.0), mDensityRange(0.0),
			mMode(0), mCurrentValueListIndex(-1), mObservable(0) {}
			
		int mType;
		double mChangeProbability;
		int mMaximalIndexChange;
		double mNoiseStrength;
		double mDisableProbability;
		double mDesiredConnectivityDensity;
		double mDensityRange;
		int mMode;
		QList<double> mValueList;
		int mCurrentValueListIndex;
		QList<double> mParams;
		DoubleValue *mObservable;
	};
	
	/**
	* ModulatingModulatedRandomSearchSynapseFunction.
	*
	* The ModulatingModulatedRandomSearchSynapseFunction does random changes to its weight
	* as long as the given set of NeuroModulators is present.
	*/
	class ModulatingModulatedRandomSearchSynapseFunction : public NeuroModulatorSynapseFunction {
	public:
		ModulatingModulatedRandomSearchSynapseFunction();
		ModulatingModulatedRandomSearchSynapseFunction(const ModulatingModulatedRandomSearchSynapseFunction &other);
		virtual ~ModulatingModulatedRandomSearchSynapseFunction();

		virtual SynapseFunction* createCopy() const;

		virtual void valueChanged(Value *value);

		virtual void reset(Synapse *owner);
		virtual double calculate(Synapse *owner);
		
		bool isActive() const;

		bool equals(SynapseFunction *synapseFunction) const;

	protected:
		virtual void updateSettings(bool resetCurrentIndices);
		
		virtual void randomSearchModeMultiVariant(Synapse *owner, ModulatingModulatedRandomSearchParameters &params);
		
		virtual double updateConcentrationLevel(Synapse *owner, ModulatingModulatedRandomSearchParameters &params);
		virtual bool isChangeTriggered(Synapse *owner, ModulatingModulatedRandomSearchParameters &params);
		virtual void performTopologySearch(Synapse *owner, ModulatingModulatedRandomSearchParameters &params);
		virtual void performRandomWalkWeightChanges(Synapse *owner, ModulatingModulatedRandomSearchParameters &params);
		virtual void performReducedRandomWalkWeightChanges(Synapse *owner, ModulatingModulatedRandomSearchParameters &params);
		virtual void performBacktracking(Synapse *owner, ModulatingModulatedRandomSearchParameters &params);
		
		int incrementDisableChangeCounter(Synapse *owner);
		int incrementWeightChangeCounter(Synapse *owner);
		
		void enableWeight(Synapse *owner, bool enable = true);

	private:
		Synapse *mOwner;
		
		QList<DoubleValue*> mObservables;
		QList<ModulatingModulatedRandomSearchParameters> mParameters;
		
		NeuralNetwork *mCurrentNetwork;
		NeuralNetworkManager *mNetworkManager;
		
		bool mNotifiedErrors;
		
		//general parameters
		CodeValue *mTypeParameters;
		DoubleValue *mProbabilityForChange;
		
		//topology search parameters
		BoolValue *mActive;
		DoubleValue *mActivationObservable;
		
		//parameters for backtracking
		QList<double> mKnownPersistentSettings;
		QList<bool> mKnownPersistentActiveStates;
		int mNumberOfStepsWithoutModulators;
		
		
	};
	
}

#endif


