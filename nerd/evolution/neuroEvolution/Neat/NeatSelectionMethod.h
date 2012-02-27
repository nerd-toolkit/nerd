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



#ifndef NERDNeatSelectionMethod_H
#define NERDNeatSelectionMethod_H

#include <QString>
#include <QHash>
#include "SelectionMethod/SelectionMethod.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "Fitness/FitnessFunction.h"
#include "Value/NormalizedDoubleValue.h"

namespace nerd {

	struct NeatSpecies {
		NeatSpecies();

		int mId;
		int mAge;
		int mAgeOfLastImprovement;
		double mMaxFitness;
		double mMeanFitness;
		double mMaxFitnessEver;
		int mNumberOfChildren;
		QList<Individual*> mMembers;

		static int mIdCounter;
	};

	class NeatFitness : public FitnessFunction {
	public:
		NeatFitness() : FitnessFunction("NeatFitness") {}
		NeatFitness(const NeatFitness &other) : FitnessFunction(other) {}
		
		virtual FitnessFunction* createCopy() const { return new NeatFitness(*this); }
		virtual double calculateCurrentFitness() { return 0.0; }
		virtual void prepareNextTry() {}
	};

	/**
	 * NeatSelectionMethod.
	 *
	 */
	class NeatSelectionMethod : public SelectionMethod {
	public:
		NeatSelectionMethod();
		NeatSelectionMethod(const NeatSelectionMethod &other);
		virtual ~NeatSelectionMethod();

		virtual SelectionMethod* createCopy() const;

		virtual QList<Individual*> createSeed(QList<Individual*> currentGeneration,
									int numberOfIndividuals, 
									int numberOfPreservedParents,
									int numberOfParentsPerIndividual);

		void adjustFitness(NeatSpecies *species);
		QList<Individual*> getSurvivers(NeatSpecies *species);
		NeatSpecies* getMatchingSpecies(Individual *ind);

	private:
		NeatFitness *mFitnessMarker;
		QList<NeatSpecies*> mSpecies;
		QHash<NeatSpecies*, Individual*> mSpeciesRepresentatives;
		IntValue *mDropOffAge;
		DoubleValue *mDropOffFactor;
		IntValue *mAgeBonusThreshold;
		DoubleValue *mAgeBonusFactor;
		DoubleValue *mSurvivalRate;
		DoubleValue *mGenomeCompatibilityThreshold;
		IntValue *mDesiredNumberOfSpecies;
		DoubleValue *mCompatibilityAutoAdjustIncrement;
		NormalizedDoubleValue *mInterspeciesMatingRate;
		
	};

}

#endif



