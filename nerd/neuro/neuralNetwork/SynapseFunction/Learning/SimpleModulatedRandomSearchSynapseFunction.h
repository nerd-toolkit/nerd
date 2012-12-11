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


#ifndef NERDSimpleModulatedRandomSearchSynapseFunction_H
#define NERDSimpleModulatedRandomSearchSynapseFunction_H

#include "SynapseFunction/NeuroModulatorSynapseFunction.h"
#include "Value/RangeValue.h"

namespace nerd {
	
	struct SimpleModulatedRandomSearchParameters {
	public:
		SimpleModulatedRandomSearchParameters() : mType(0), mChangeProbability(0),
			mDisableProbability(0), mMode(0), mObservable(0) {}
			
		int mType;
		double mChangeProbability;
		double mDisableProbability;
		int mMode;
		QList<double> mParams;
		DoubleValue *mObservable;
	};
	
	/**
	* SimpleModulatedRandomSearchSynapseFunction.
	*
	* The SimpleModulatedRandomSearchSynapseFunction does random changes to its weight
	* as long as the given set of NeuroModulators is present.
	*/
	class SimpleModulatedRandomSearchSynapseFunction : public NeuroModulatorSynapseFunction {
	public:
		SimpleModulatedRandomSearchSynapseFunction();
		SimpleModulatedRandomSearchSynapseFunction(const SimpleModulatedRandomSearchSynapseFunction &other);
		virtual ~SimpleModulatedRandomSearchSynapseFunction();

		virtual SynapseFunction* createCopy() const;

		virtual void valueChanged(Value *value);

		virtual void reset(Synapse *owner);
		virtual double calculate(Synapse *owner);

		bool equals(SynapseFunction *synapseFunction) const;

	protected:
		virtual void updateSettings();
		
		virtual void randomSearchModeSimpleRandom(Synapse *owner, SimpleModulatedRandomSearchParameters &params);
		virtual void randomSearchModeBacktracking(Synapse *owner, SimpleModulatedRandomSearchParameters &params);
		
		int incrementDisableChangeCounter(Synapse *owner);
		int incrementWeightChangeCounter(Synapse *owner);

	private:
		StringValue *mTypeParameters;
		DoubleValue *mProbabilityForChange;
		BoolValue *mInactive;

		QList<DoubleValue*> mObservables;
		QList<SimpleModulatedRandomSearchParameters> mParameters;

		
		NeuralNetwork *mCurrentNetwork;
		NeuralNetworkManager *mNetworkManager;
		
		bool mNotifiedErrors;
		
		QList<double> mKnownPersistentSettings;
		QList<bool> mKnownPersistentDisableStates;
		int mNumberOfStepsWithoutModulators;
	};
	
}

#endif


