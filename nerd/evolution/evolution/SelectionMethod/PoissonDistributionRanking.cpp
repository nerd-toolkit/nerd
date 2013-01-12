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
	
	//prepare for poisson function (but check how to cope with large n! (lookup table?)
	mUseSoftMax = new BoolValue(true);
	//addParameter("UseSoftMax", mUseSoftMax);
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
	//mUseSoftMax = dynamic_cast<BoolValue*>(getParameter("UseSoftMax"));
	mUseSoftMax = new BoolValue(true);
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
		Core::log("PoissonDistributionRanking: All indiduals have a similar fitness.!", true);

		//TODO check if this should still be a stochastic process...
		newGeneration << newParents;
		fillUpGeneration(newGeneration, parentGeneration.front(), numberOfIndividuals);

		return newGeneration;
	}

	double normFactor = 1.0 / maxFitness;

	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {
		double fitness = (minFitness + i.next()->getFitness(fitnessFunction)) * normFactor;
		//fitnessSum += (minFitness * normFactor) + fitness; //Cr
		fitnessSum += fitness;
	}

	

	double mean = fitnessSum / ((double) parentGeneration.size());

	double deviationSum = 0.0;
	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {
		double d = (mean - ((minFitness + i.next()->getFitness(fitnessFunction)) * normFactor));
		deviationSum += (d * d);
	}

	double average = deviationSum / ((double) (parentGeneration.size() - 1));


	if(average == 0) {
		Core::log("PoissonDistributionRanking: Fitness average was 0!", true);

		newGeneration << newParents;
		fillUpGeneration(newGeneration, parentGeneration.front(), numberOfIndividuals);

		return newGeneration;
	}

	double offspringProbabilitySum = 0.0;
	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {

		//required later for the denumerator of individual offspring o_i.
		offspringProbabilitySum += 
				exp((-gamma * (1 - ((minFitness + i.next()->getFitness(fitnessFunction)) * normFactor))) 
					/ average);
	}

	if(offspringProbabilitySum == 0) {
		Core::log("PoissonDistributionRanking: OffspringProbabilitySum was 0!", true);

		newGeneration << newParents;
		fillUpGeneration(newGeneration, parentGeneration.front(), numberOfIndividuals);

		return newGeneration;
	}
	
	int desiredPopulationSize = Math::max(1, numberOfIndividuals - numberOfPreservedParents);

	double boost = 1.0;
	while(numberOfIndividuals > (newGeneration.size() + newParents.size())) {
		if(boost < 1.0) {
			Core::log(QString("PoissonDistribution: Doing a second sweep with ") + QString::number(boost), true);
		}
		for(int i = 0; i < parentGeneration.size(); ++i) {
	
			Individual *parent = parentGeneration.at(i);
			
			//b_i = z * o_i (z == desiredPopulationSize)
			//o_i = e^(-gamma (p_max - p_i) / sigma^2) / sum[j=1_N](e^(-gamma (p_max - p_j) / sigma^2) (p_i fitness of individual i)
			
			double birthRate = 1.0;
			if(mUseSoftMax->get()) {
			
				birthRate = ((double) desiredPopulationSize) * 
							exp(-gamma * (1 - ((minFitness + parent->getFitness(fitnessFunction)) * normFactor)) 
						/ average) / offspringProbabilitySum;
			}
	
			int numberOfChildren = -1;
			double limit = exp(-1.0 * Math::abs(birthRate)) * boost;
			double product = 1.0;

			
			// algorithm poisson random number (Knuth):
			//     init:
			//          Let L ← e^(−λ), k ← 0 and p ← 1.
			//     do:
			//          k ← k + 1.
			//          Generate uniform random number u in [0,1] and let p ← p × u.
			//     while p > L.
			//     return k − 1.
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



