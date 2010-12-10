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



#include "PoissonDistributionRanking.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Random.h"
#include "Evolution/Population.h"
#include "Math/Math.h"
#include <QListIterator>
#include "Fitness/FitnessFunction.h"
#include "Core/Core.h"
#include <iostream>
#include "Math/IndividualSorter.h"
#include <math.h>

using namespace std;

namespace nerd {


/**
 * Constructs a new PoissonDistributionRanking.
 */
PoissonDistributionRanking::PoissonDistributionRanking()
	: SelectionMethod("PoissonDistribution")
{
	mSelectionPressure = new DoubleValue(1.0);
	addParameter("Pressure", mSelectionPressure);
}


/**
 * Copy constructor. 
 * 
 * @param other the PoissonDistributionRanking object to copy.
 */
PoissonDistributionRanking::PoissonDistributionRanking(const PoissonDistributionRanking &other)
	: Object(), ValueChangedListener(), SelectionMethod(other) 
{
	mSelectionPressure = dynamic_cast<DoubleValue*>(getParameter("Pressure"));
}

/**
 * Destructor.
 */
PoissonDistributionRanking::~PoissonDistributionRanking() {
}


/**
 * Creates a copy of this TournamentSelectionMethod. The copy will live in the given population.
 *
 * @param ownerPopulation the population the copy will live in (will be added to the population).
 * @return a copy of this SelectionMethod.
 */
SelectionMethod* PoissonDistributionRanking::createCopy() const {
	return new PoissonDistributionRanking(*this);
}


/**
 * Creates the given number of individuals as part of a new generation.
 * 
 * @param numberOfIndividuals the number of individuals to create.
 * @param numberOfPreservedParents hint to preserve up to this number of parents (unchanged parents)
 * @param numberOfParentsPerIndividual the number of parents per new individudal.
 */
QList<Individual*> PoissonDistributionRanking::createSeed(
				QList<Individual*> currentGeneration,
				int numberOfIndividuals, int numberOfPreservedParents, 
				int)
{

// 	cerr << "poisson" << endl;

	if(mOwnerPopulation == 0) {
		return QList<Individual*>();
	}

	QList<Individual*> parentGeneration(currentGeneration);
	QList<Individual*> newGeneration;
	QList<Individual*> newParents;
	
	//preserve best parents
	parentGeneration = IndividualSorter::sortByFitness(
			parentGeneration, mResponsibleFitnessFunction);

	for(int i = 0; i < parentGeneration.size() && i < numberOfPreservedParents; ++i) {
		Individual *ind = parentGeneration.at(i);
		ind->protectGenome(true);
		newParents.append(ind);
	}

	//don't proceed with generations smaller than 2.
	if(parentGeneration.size() == 1) {	
		fillUpGeneration(newParents, parentGeneration.front(), numberOfIndividuals);
	}

	if(parentGeneration.size() <= 1) {
		return newParents;
	}

	if(mOwnerPopulation == 0 || mPopulationProportionValue == 0 
		|| mSelectionPressure == 0 || mResponsibleFitnessFunction == 0) 
	{
		return newParents;
	}

	FitnessFunction *fitnessFunction = mResponsibleFitnessFunction;


	//get all required terms for poisson process
	double gamma = mSelectionPressure->get();
	double fitnessSum = 0.0;
	double maxFitness = -100000000.0;
	double minFitness = 0.0;
	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {
		double fitness = i.next()->getFitness(fitnessFunction);
		minFitness = Math::min(minFitness, fitness);
		maxFitness = Math::max(maxFitness, fitness);
	}
	minFitness = -minFitness;
	maxFitness += minFitness;

	if(maxFitness == minFitness) {
		Core::log("PoissonDistributionRanking: Fitness is always the same!");

		newGeneration << newParents;
		fillUpGeneration(newGeneration, parentGeneration.front(), numberOfIndividuals);

		return newGeneration;
	}

	double normFactor = 1.0 / maxFitness;

	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {
		double fitness = (minFitness + i.next()->getFitness(fitnessFunction)) * normFactor;
		fitnessSum += (minFitness * normFactor) + fitness;
	}

	

	double mean = fitnessSum / ((double) parentGeneration.size());

	double deviationSum = 0.0;
	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {
		double d = (mean - ((minFitness + i.next()->getFitness(fitnessFunction)) * normFactor));
		deviationSum += (d * d);
	}

	double average = deviationSum / ((double) (parentGeneration.size() - 1));


	if(average == 0) {
		Core::log("PoissonDistributionRanking: Fitness average was 0!");

		newGeneration << newParents;
		fillUpGeneration(newGeneration, parentGeneration.front(), numberOfIndividuals);

		return newGeneration;
	}

	double offspringProbabilitySum = 0.0;
	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {
		offspringProbabilitySum += 
				exp((-gamma * (((maxFitness - minFitness) - (
					(minFitness + i.next()->getFitness(fitnessFunction)))) * normFactor)) 
					/ average);
	}

	if(offspringProbabilitySum == 0) {
		Core::log("PoissonDistributionRanking: OffspringProbabilitySum was 0!");

		newGeneration << newParents;
		fillUpGeneration(newGeneration, parentGeneration.front(), numberOfIndividuals);

		return newGeneration;
	}

	//normalize probabilitySum according to desired population size.
	double normalizedOffspringProbabilitySum = 
				Math::max(0.0, ((double) (numberOfIndividuals - numberOfPreservedParents)))
				/ offspringProbabilitySum;

	double boost = 1.0;
	while(numberOfIndividuals > (newGeneration.size() + newParents.size())) {
		for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {
	
			Individual *parent = i.next();
			
			//double birthRate = exp(-gamma * ((maxFitness - parent->getFitness(fitnessFunction)) 
			//					/ average)) * normalizedOffspringProbabilitySum;
			double birthRate = exp((-gamma * (((maxFitness - minFitness) - 
						(minFitness + parent->getFitness(fitnessFunction))) * normFactor)) 
					/ average) * normalizedOffspringProbabilitySum;
	
			int numberOfChildren = -1;
			double limit = exp(-1.0 * Math::abs(birthRate)) * boost;
			double product = 1.0;

// 			cerr << "inner: " << (((maxFitness - 
// 						(minFitness + parent->getFitness(fitnessFunction))) * normFactor)) << endl;
// 
// 			cerr << "maxfit: " << maxFitness << " parentfit: " << parent->getFitness(fitnessFunction)
// 				<< " normalized: " << ((minFitness + parent->getFitness(fitnessFunction)) * normFactor)
// 				<< "  gamma: " << gamma << endl;
// 			cerr << "limit: " << limit << " birthrate: " << birthRate <<  " abs: " << Math::abs(birthRate) << " average: " << average << "  normOffspringSum: " << normalizedOffspringProbabilitySum << endl;

			do {
				++numberOfChildren;
				product *= Random::nextDouble();
			} while(product > limit);
	
			if(numberOfChildren > 0) {
				if(!newParents.contains(parent)) {
					newParents.append(parent);
				}
			}
			for(int i = 0; i < numberOfChildren; ++i) {
				Individual *newIndividual = new Individual();
				newIndividual->getParents().append(parent);
				newGeneration.append(newIndividual);
			}
		}
		boost = boost * 0.9;
	}

	newGeneration = newParents + newGeneration;

	return newGeneration;
}



void PoissonDistributionRanking::fillUpGeneration(QList<Individual*> &generation, 
											Individual *parent, int desiredSize) 
{
	while(generation.size() < desiredSize) {
		Individual *newIndividual = new Individual();
		newIndividual->getParents().append(parent);
		generation.append(newIndividual);
	}
}


}



