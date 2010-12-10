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



#include "MultiObjectiveTournamentSelection.h"
#include "Math/Random.h"
#include "Evolution/Population.h"
#include "Math/Math.h"
#include <QListIterator>
#include "Fitness/FitnessFunction.h"
#include "Core/Core.h"
#include <iostream>
#include "Math/IndividualSorter.h"
#include "Util/Tracer.h"
#include <QStringList>

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {


/**
 * Constructs a new MultiObjectiveTournamentSelection.
 */
MultiObjectiveTournamentSelection::MultiObjectiveTournamentSelection(int tournamentSize)
	: SelectionMethod("MultiObjectiveTournamentSelection"), mTournamentSize(0)
{
	mTournamentSize = new IntValue(tournamentSize);
	addParameter("TournamentSize", mTournamentSize);

	mFitnessWeights = new StringValue("Controllers/Script,1");
	addParameter("FitnessWeights", mFitnessWeights);
}


/**
 * Copy constructor. 
 * 
 * @param other the MultiObjectiveTournamentSelection object to copy.
 */
MultiObjectiveTournamentSelection::MultiObjectiveTournamentSelection(
										const MultiObjectiveTournamentSelection &other) 
	: Object(), ValueChangedListener(), SelectionMethod(other)
{
	mTournamentSize = dynamic_cast<IntValue*>(getParameter("TournamentSize"));
	mFitnessWeights = dynamic_cast<StringValue*>(getParameter("FitnessWeights"));
}

/**
 * Destructor.
 */
MultiObjectiveTournamentSelection::~MultiObjectiveTournamentSelection() {
}

/**
 * Creates a copy of this TournamentSelectionMethod. The copy will live in the given population.
 *
 * @param ownerPopulation the population the copy will live in (will be added to the population).
 * @return a copy of this SelectionMethod.
 */
SelectionMethod* MultiObjectiveTournamentSelection::createCopy() const {
	return new MultiObjectiveTournamentSelection(*this);
}


/**
 * Creates the given number of individuals as part of a new generation.
 * 
 * @param numberOfIndividuals the number of individuals to create.
 * @param numberOfPreservedParents hint to preserve up to this number of parents (unchanged parents)
 * @param numberOfParentsPerIndividual the number of parents per new individudal.
 */
QList<Individual*> MultiObjectiveTournamentSelection::createSeed(
			QList<Individual*> currentGeneration, int numberOfIndividuals, 
			int numberOfPreservedParents, int numberOfParentsPerIndividual)
{
	TRACE("MultiObjectiveTournamentSelection::createSeed");

	if(mOwnerPopulation == 0) {
		return QList<Individual*>();
	}

	QList<Individual*> parentGeneration(currentGeneration);
	QList<Individual*> newGeneration;
	
	//preserve best parents
	parentGeneration = IndividualSorter::sortByFitness(
			parentGeneration, mResponsibleFitnessFunction);

	for(int i = 0; i < parentGeneration.size() && i < numberOfPreservedParents; ++i) {
		Individual *ind = parentGeneration.at(i);
		ind->protectGenome(true);
		newGeneration.append(ind);
	}

	if(mOwnerPopulation == 0 || mPopulationProportionValue == 0 
		|| mTournamentSize == 0 || mResponsibleFitnessFunction == 0) 
	{
		return newGeneration;
	}

	//collect fitness weights and fitness function indicies
	mWeightsPerFitnessIndex.clear();
	QStringList fitnessEntries = mFitnessWeights->get().split(";");

	for(QListIterator<QString> i(fitnessEntries); i.hasNext();) {
		QString entry = i.next();

		QStringList entryList = entry.split(",");
		if(entryList.size() != 2) {
			Core::log("MultiObjectiveTournamentSelection: Could not parse entry part [" 
						+ entry + "]", true);
			continue;
		}
		else {
			FitnessFunction *fitnessFunction = mOwnerPopulation->getFitnessFunction(entryList.at(0));
			bool okDouble = false;
			double weight = entryList.at(1).toDouble(&okDouble);
			if(fitnessFunction == 0 || !okDouble) {
				Core::log("MultiObjectiveTournamentSelection: Could not parse entry part [" 
						+ entry + "]." + QString::number((long) fitnessFunction), true);
				continue;
			}
			if(mWeightsPerFitnessIndex.keys().contains(fitnessFunction)) {
				Core::log("MultiObjectiveTournamentSelection: Fitness index [" 
						+ entryList.at(0) + "] was specified more than once!", true);
				continue;
			}
			else {
				mWeightsPerFitnessIndex.insert(fitnessFunction, weight);
			}
		}
	}



	int numberOfRivals = Math::max(2, mTournamentSize->get()); 

	for(int i = 0; i < numberOfIndividuals - numberOfPreservedParents; ++i) {

		Individual *newIndividual = new Individual();
	
		if(!parentGeneration.empty()) {

			//determin the required number of parents per individual.
			for(int k = 0; k < numberOfParentsPerIndividual; ++k) {	
				//select rivals.
				QList<Individual*> rivals;
				rivals.append(parentGeneration.at(Random::nextInt(parentGeneration.size())));

				for(int j = 0; j < numberOfRivals - 1; ++j) {

					Individual *nextRival = rivals.at(0);
					while(nextRival != 0 
							&& rivals.contains(nextRival) 
							&& rivals.size() < parentGeneration.size()) 
					{
						nextRival = parentGeneration.at(Random::nextInt(parentGeneration.size()));
					}
					rivals.append(nextRival);
				}

				Individual *bestIndividual = getBestIndividual(rivals);

				if(bestIndividual != 0) {
					//add the best rival as parent to the new individual.
					newIndividual->getParents().append(bestIndividual);
				}
			}
		}
		newGeneration.append(newIndividual);
	}
	return newGeneration;
}


Individual* MultiObjectiveTournamentSelection::getBestIndividual(QList<Individual*> candidates) {

	Individual *bestIndividual = 0;
	double bestDominanceValues = -9999999;

	for(QListIterator<Individual*> i(candidates); i.hasNext();) {
		Individual *individual = i.next();

		double dominanceValue = 0.0;

		for(QHashIterator<FitnessFunction*, double> k(mWeightsPerFitnessIndex); k.hasNext();) {
			k.next();

			FitnessFunction *fitnessFunction = k.key();
			double weight = k.value();

			double fitness = individual->getFitness(fitnessFunction);

			//calculate dominance value
			for(QListIterator<Individual*> j(candidates); j.hasNext();) {
				Individual *candidate = j.next();
				double candiateFitness = candidate->getFitness(fitnessFunction);
				if(candiateFitness > fitness) {
					dominanceValue -= weight;
				}
				else if(candiateFitness < fitness) {
					dominanceValue += weight;
				}
			}
		}
		if(dominanceValue > bestDominanceValues) {
			bestIndividual = individual;
			bestDominanceValues = dominanceValue;
		}
	}
	return bestIndividual;
}

}



