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

#include "TournamentSelectionMethod.h"
#include "Math/Random.h"
#include "Evolution/Population.h"
#include "Math/Math.h"
#include <QListIterator>
#include "Fitness/FitnessFunction.h"
#include "Core/Core.h"
#include <iostream>
#include "Math/IndividualSorter.h"
#include "Util/Tracer.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {


/**
 * Creates a new TournamentSelectionMethod living in the given population. 
 *
 * @param population the population to add this TournamentSelectionMethod to. 
 * @param tournamentSize the number of rivals to choose a winner from during tournaments.
 */
TournamentSelectionMethod::TournamentSelectionMethod(int tournamentSize)
	: SelectionMethod("TournamentSelection"), mTournamentSize(0)
{
	TRACE("TournamentSelectionMethod::TournamentSelectionMethod");

	mTournamentSize = new IntValue(tournamentSize);
	addParameter("TournamentSize", mTournamentSize);
}



/**
 * Creates a new TournamentSelection as copy of another TournamentSelection.
 * The copy will live in (be added to) the given population.
 *
 * @param other the TournementSelection to copy.
 * @param population the population the copy will live in.
 */
TournamentSelectionMethod::TournamentSelectionMethod(const TournamentSelectionMethod &other)
	: Object(), ValueChangedListener(), SelectionMethod(other)
{	
	TRACE("TournamentSelectionMethod::TournamentSelectionMethodCopy");

	mTournamentSize = dynamic_cast<IntValue*>(getParameter("TournamentSize"));
}


/**
 * Destructor.
 */
TournamentSelectionMethod::~TournamentSelectionMethod() {
	TRACE("TournamentSelectionMethod::~TournamentSelectionMethod");
}


/**
 * Creates a copy of this TournamentSelectionMethod. The copy will live in the given population.
 *
 * @param ownerPopulation the population the copy will live in (will be added to the population).
 * @return a copy of this SelectionMethod.
 */
SelectionMethod* TournamentSelectionMethod::createCopy() const {
	TRACE("TournamentSelectionMethod::createCopy");

	return new TournamentSelectionMethod(*this);
}


/**
 * Creates the given number of individuals as part of a new generation.
 * 
 * @param numberOfIndividuals the number of individuals to create.
 * @param numberOfPreservedParents hint to preserve up to this number of parents (unchanged parents)
 * @param numberOfParentsPerIndividual the number of parents per new individudal.
 */
QList<Individual*> TournamentSelectionMethod::createSeed(
			QList<Individual*> currentGeneration, int numberOfIndividuals, 
			int numberOfPreservedParents, int numberOfParentsPerIndividual)
{
	TRACE("TournamentSelectionMethod::createSeed");

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

	int numberOfRivals = Math::max(2, mTournamentSize->get()); 
	FitnessFunction *fitnessFunction = mResponsibleFitnessFunction;

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

				//determine best rival of the selection
				Individual *bestIndividual = rivals.at(0);
		
				if(bestIndividual == 0) {
					Core::log("TournamentSelectionMethod: Could not find suitable rivals.");
					continue;
				}

				for(QListIterator<Individual*> j(rivals); j.hasNext();) {
					Individual *ind = j.next();
					if(ind->getFitness(fitnessFunction) 
							> bestIndividual->getFitness(fitnessFunction)) 
					{
						bestIndividual = ind;
					}
				}

				//add the best rival as parent to the new individual.
				newIndividual->getParents().append(bestIndividual);
			}
		}
		newGeneration.append(newIndividual);
	}
	return newGeneration;
}


}


