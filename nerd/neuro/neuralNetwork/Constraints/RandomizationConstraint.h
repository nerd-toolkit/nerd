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


#ifndef NERDRandomizationConstraint_H
#define NERDRandomizationConstraint_H


#include "Constraints/GroupConstraint.h"
#include "Value/IntValue.h"
#include "Value/StringValue.h"
#include "Value/RangeValue.h"
#include "Value/BoolValue.h"


namespace nerd {
	

	/**
	 * RandomizationConstraint.
	 * 
	 * This constraint allows to randomize parameters of the module members, such as
	 * neuron bias, activation, output, synapse weight, etc.
	 * 
	 * The constraint is only executed once per constraint resolver run to allow other
	 * constraints to resolve inconsistencies introduced by the randomization.
	 * 
	 * The constraint can store the used seed in the module as property. With this stored 
	 * property, the randomized settings can be replicated. For this, the constraint can be 
	 * configured to use such stored seeds instead of generating a new random seed.
	 * 
	 * Special case: the randomize constraint is often used with the NetworkAnalyzer tool.
	 * When the constraint detects a value called /Plotter/AnalyzerRunCounter then a new
	 * property appears, that allows to enable its sigle-shot mode. In that mode, the 
	 * constraint uses a new random seed for the first execution and then that same seed
	 * for all other executions throughout the same analyer run. If the counter of the 
	 * value changes, then a new analyzer run is assumed and a new seed is generated
	 * during the next constraint run.
	 * 
	 * TODO: the constraint should allow more control over the parameter settings and
	 * should also include attributes of TransferFuncitons, ActivationFunctions and
	 * SynapseFunctions.
	 */
	class RandomizationConstraint : public GroupConstraint {
	public:
		RandomizationConstraint();
		RandomizationConstraint(const RandomizationConstraint &other);
		virtual ~RandomizationConstraint();

		virtual GroupConstraint* createCopy() const;

		virtual bool isValid(NeuronGroup *owner);
		virtual bool applyConstraint(NeuronGroup *owner, CommandExecutor *executor,
									 QList<NeuralNetworkElement*> &trashcan);
		
		virtual bool equals(GroupConstraint *constraint) const;

	private:
		RangeValue *mGlobalBiasRange;
		RangeValue *mGlobalOutputRange;
		RangeValue *mGlobalActivationRange;
		RangeValue *mGlobalWeightRange;
		StringValue *mIndividualRanges;
		BoolValue *mStoreRandomizedValuesInNetwork;
		BoolValue *mApplyStoredValuesFromNetwork;
		IntValue *mRandomizationHistorySize;
		
		BoolValue *mOneShotRandomization;
		IntValue *mAnalyzerRunCounter;
		int mLastOneShotCount;
		int mLastSeed;
	};

}

#endif


