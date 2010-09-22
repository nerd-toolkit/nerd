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



#include "NeatSelectionMethod.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/IndividualSorter.h"
#include <math.h>
#include "Neat/NeatSpeciesOrganism.h"
#include "Evolution/Population.h"
#include "Math/Random.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {

int NeatSpecies::mIdCounter = 0;

NeatSpecies::NeatSpecies() 
	: mId(mIdCounter++), mAge(0), mAgeOfLastImprovement(0), mMaxFitness(0.0), mMeanFitness(0.0),
	  mMaxFitnessEver(0.0), mNumberOfChildren(0)
{}

/**
 * Constructs a new NeatSelectionMethod.
 */
NeatSelectionMethod::NeatSelectionMethod()
	: SelectionMethod("NeatSelection"), mFitnessMarker(0)
{
	mSurvivalRate = new DoubleValue(0.1);
	mDropOffAge = new IntValue(100);
	mDropOffFactor = new DoubleValue(0.01);
	mAgeBonusThreshold = new IntValue(10);
	mAgeBonusFactor = new DoubleValue(1.5);
	mGenomeCompatibilityThreshold = new DoubleValue(0.5);
	mInterspeciesMatingRate = new NormalizedDoubleValue(0.05, 0.0, 1.0);
	mDesiredNumberOfSpecies = new IntValue(-1);
	mCompatibilityAutoAdjustIncrement = new DoubleValue(0.3);
	
	addParameter("SurvivalRate", mSurvivalRate);
	addParameter("DropOffAge", mDropOffAge);
	addParameter("DropOffFactor", mDropOffFactor);
	addParameter("AgeBonusThreshold", mAgeBonusThreshold);
	addParameter("AgeBonusFactor", mAgeBonusFactor);
	addParameter("GenomeCompatibiltyThreshold", mGenomeCompatibilityThreshold);
	addParameter("InterspeciesMatingRate", mInterspeciesMatingRate);
	addParameter("DesiredNumberOfSpecies", mDesiredNumberOfSpecies);
	addParameter("CompatibilityAutoAdjustIncrement", mCompatibilityAutoAdjustIncrement);

	mFitnessMarker = new NeatFitness();
}


/**
 * Copy constructor. 
 * 
 * @param other the NeatSelectionMethod object to copy.
 */
NeatSelectionMethod::NeatSelectionMethod(const NeatSelectionMethod &other) 
	: SelectionMethod(other)
{
	mSurvivalRate = dynamic_cast<DoubleValue*>(getParameter("SurvivalRate"));
	mDropOffAge = dynamic_cast<IntValue*>(getParameter("DropOffAge"));
	mDropOffFactor = dynamic_cast<DoubleValue*>(getParameter("DropOffFactor"));
	mAgeBonusThreshold = dynamic_cast<IntValue*>(getParameter("AgeBonusThreshold"));
	mAgeBonusFactor = dynamic_cast<DoubleValue*>(getParameter("AgeBonusFactor"));
	mGenomeCompatibilityThreshold = dynamic_cast<DoubleValue*>(
									getParameter("GenomeCompatibiltyThreshold"));
	mInterspeciesMatingRate = dynamic_cast<NormalizedDoubleValue*>(
									getParameter("InterspeciesMatingRate"));
	mDesiredNumberOfSpecies = dynamic_cast<IntValue*>(
									getParameter("DesiredNumberOfSpecies"));	
	mCompatibilityAutoAdjustIncrement = dynamic_cast<DoubleValue*>(
									getParameter("CompatibilityAutoAdjustIncrement"));

	mFitnessMarker = new NeatFitness();
}

/**
 * Destructor.
 */
NeatSelectionMethod::~NeatSelectionMethod() {
	delete mFitnessMarker;
}


SelectionMethod* NeatSelectionMethod::createCopy() const {
	return new NeatSelectionMethod(*this);
}


QList<Individual*> NeatSelectionMethod::createSeed(const QList<Individual*> &currentGeneration,
									int numberOfIndividuals, 
									int, int)
{

	if(mOwnerPopulation == 0) {
		return QList<Individual*>();
	}

	QList<Individual*> parentGeneration(currentGeneration);
	QList<Individual*> newGeneration;

	if(parentGeneration.empty()) {
		return newGeneration;
	}

	//update compatibility threshold if required
	if(mDesiredNumberOfSpecies->get() > 0) {
		if(mSpecies.size() < mDesiredNumberOfSpecies->get()) {
			mGenomeCompatibilityThreshold->set(mGenomeCompatibilityThreshold->get()
						- mCompatibilityAutoAdjustIncrement->get());
		}
		else if(mSpecies.size() > mDesiredNumberOfSpecies->get()) {
			mGenomeCompatibilityThreshold->set(mGenomeCompatibilityThreshold->get()
						+ mCompatibilityAutoAdjustIncrement->get());
		}
	}

	//update species
	//put individuals to the correct species
	//create and destroy new and obsolte species.
	mSpeciesRepresentatives.clear();
	for(QListIterator<NeatSpecies*> i(mSpecies); i.hasNext();) {
		NeatSpecies *species = i.next();
		if(species != 0 && !species->mMembers.empty()) {
			mSpeciesRepresentatives.insert(species, species->mMembers.first());
			species->mMembers.clear();
		}
	}

	QList<NeatSpecies*> usedSpecies;
	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {
		Individual *ind = i.next();
		NeatSpecies *matchingSpecies = getMatchingSpecies(ind);
		if(matchingSpecies != 0) {
			ind->setProperty("Species", QString::number(matchingSpecies->mId));
			matchingSpecies->mMembers.append(ind);
			if(!usedSpecies.contains(matchingSpecies)) {
				usedSpecies.append(matchingSpecies);
			}
			if(!mSpecies.contains(matchingSpecies)) {
				mSpecies.append(matchingSpecies);
			}
			if(!mSpeciesRepresentatives.keys().contains(matchingSpecies)) {
				mSpeciesRepresentatives.insert(matchingSpecies, ind);
			}
		}
	}

	QList<NeatSpecies*> oldSpecies = mSpecies;
	for(QListIterator<NeatSpecies*> i(oldSpecies); i.hasNext();) {
			NeatSpecies *species = i.next();
			if(!usedSpecies.contains(species)) {
				mSpecies.removeAll(species);
				delete species;
			}
	}
// 	cerr << "s2: species " << mSpecies.size() << " " << mSpeciesRepresentatives.size() << " " << usedSpecies.size() << endl;

	//preserve best individuals
	for(QListIterator<NeatSpecies*> i(mSpecies); i.hasNext();) {
		NeatSpecies *species = i.next();
		adjustFitness(species);
		IndividualSorter::sortByFitness(species->mMembers, mFitnessMarker);
		//add survivers to next generation
		newGeneration << getSurvivers(species);
	}

	double maxFitness = 0.0;
	double sumFitness = 0.0;
	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {
		double fitness = i.next()->getFitness(mFitnessMarker);
		if(fitness > maxFitness) {
			maxFitness = fitness;
		}
		sumFitness += fitness;
	}
	double averageFitness = sumFitness / ((double) parentGeneration.size());

	//protect all unchanged parents
	for(QListIterator<Individual*> i(newGeneration); i.hasNext();) {
		i.next()->protectGenome(true);
	}

	if(averageFitness == 0.0) {
		return newGeneration;
	}

	//check for stagnation => delta encoding

	int desiredPopSize = newGeneration.size();

	//Count desired  number of children.
	for(QListIterator<NeatSpecies*> i(mSpecies); i.hasNext();) {
		NeatSpecies *species = i.next();

		//QList<Individual*> &individuals = species->mMembers;
		
		int numberOfChildrenInSpecies = 0;
		for(QListIterator<Individual*> j(species->mMembers); j.hasNext();) {
			Individual *ind = j.next();
			int numberOfChildren = ind->getFitness(mFitnessMarker) / averageFitness;
			numberOfChildrenInSpecies += numberOfChildren;
		}
		species->mNumberOfChildren = numberOfChildrenInSpecies;
		desiredPopSize += numberOfChildrenInSpecies;
		
	}
	
	if(desiredPopSize == 0) {
		return newGeneration;
	}

	//create new individuals
	for(QListIterator<NeatSpecies*> i(mSpecies); i.hasNext();) {
		NeatSpecies *species = i.next();

		QList<Individual*> &individuals = species->mMembers;

		int numberOfChildrenInSpecies = (int) (floor(((double) species->mNumberOfChildren) 
										* (((double) numberOfIndividuals) / ((double) desiredPopSize))) + 1.0);

		for(int i = 0; i < numberOfChildrenInSpecies; ++i) {
			QList<Individual*> potentialParents = individuals;
			//get parents
			Individual *parent1 = potentialParents.at(Random::nextInt(potentialParents.size()));
			Individual *parent2 = 0;
			potentialParents.removeAll(parent1);
			
			//check if parent2 is from the same or from distinct species
			if(Random::nextDouble() < mInterspeciesMatingRate->get()) {
				//get parent from another species
				QList<NeatSpecies*> potentialSpecies = mSpecies;
			
				//Choose a random species tending towards better species
				double randMult = Math::getNextGaussianDistributedValue(0.0) / 4.0;
				if(randMult > 1.0) {
					randMult = 1.0;
				}
				//This tends to select better species
				int randSpeciesNum = (int) floor((randMult * (potentialSpecies.size() - 1.0)) + 0.5);

				NeatSpecies *parentSpecies = potentialSpecies.at(randSpeciesNum);

				if(parentSpecies != 0 && !parentSpecies->mMembers.empty()) {
					parent2 = parentSpecies->mMembers.first();
				}
				if(parent2 == 0 && !potentialParents.empty()) {
					parent2 = potentialParents.at(Random::nextInt(potentialParents.size()));
				}
			}
			else {
				//get parent2 from the same species
				if(!potentialParents.empty()) {
					parent2 = potentialParents.at(Random::nextInt(potentialParents.size()));
				}
			}
			Individual *newIndividual = new Individual();

			//add parents (best parent comes first).
			//if both parents have equal fitness, then the smaller one comes first.
			if(parent1 != 0 && parent2 != 0) {
	
				double fitnessP1 = parent1->getFitness(mResponsibleFitnessFunction);
				double fitnessP2 = parent2->getFitness(mResponsibleFitnessFunction);
				if(fitnessP1 >= fitnessP2) {
					newIndividual->getParents().append(parent1);
					newIndividual->getParents().append(parent2);
					if(fitnessP1 == fitnessP2) {
						newIndividual->setProperty("ParentsEqual");
					}
				}
				else {
					newIndividual->getParents().append(parent2);	
					newIndividual->getParents().append(parent1);
				}
			}
			else if(parent1 != 0) {
				newIndividual->getParents().append(parent1);
			}
			else if(parent2 != 0) {
				newIndividual->getParents().append(parent2);
			}
			newGeneration.append(newIndividual);
		}
	}


	return newGeneration;
}



void NeatSelectionMethod::adjustFitness(NeatSpecies *species) {
	if(species == 0) {
		return;
	}
	//increment species age
	species->mAge++;

	species->mMaxFitness = 0.0;

	int ageDebt = species->mAge - species->mAgeOfLastImprovement + 1 - mDropOffAge->get();
	double bestFitness = 0.0;

	QList<Individual*> &individuals = species->mMembers;
	for(QListIterator<Individual*> i(individuals); i.hasNext();) {
		Individual *ind = i.next();
		
		//reduce fitness for individuals if species is too old.
		double fitness = ind->getFitness(mResponsibleFitnessFunction);

		if(fitness > species->mMaxFitness) {
			species->mMaxFitness = fitness;
		}

		if(ageDebt > 0) {
			fitness = fitness * mDropOffFactor->get();
		}
		//increase fitness for very young species
		if(species->mAge < mAgeBonusThreshold->get()) {
			fitness = fitness * mAgeBonusFactor->get();
		}
		//do not allow negative fitness
		if(fitness < 0.0) {
			fitness = 0.0;
		}
		//share fitness with species
		fitness = fitness / ((double) individuals.size());

		if(fitness > bestFitness) {
			bestFitness = fitness;
		}

		ind->setFitness(mFitnessMarker, fitness);
	}

	//Update age of last improvement
	if(bestFitness > species->mMaxFitnessEver) {
		species->mMaxFitnessEver = bestFitness;
		species->mAgeOfLastImprovement = species->mAge;
	}
}


QList<Individual*> NeatSelectionMethod::getSurvivers(NeatSpecies *species) {
	if(species == 0) {
		return QList<Individual*>();
	}
	
	QList<Individual*> &individuals = species->mMembers;
	IndividualSorter::sortByFitness(individuals, mFitnessMarker);

	int numberOfSurvivors = (int) (floor(mSurvivalRate->get() * ((double) individuals.size()))) + 1.0;

	QList<Individual*> survivors;
	for(int i = 0; i < numberOfSurvivors && i < individuals.size(); ++i) {
		survivors.append(individuals.at(i));
	}
	return survivors;
}


NeatSpecies* NeatSelectionMethod::getMatchingSpecies(Individual *ind) {

	if(ind == 0) {
		return 0;
	}
	NeatSpeciesOrganism *organism = dynamic_cast<NeatSpeciesOrganism*>(ind->getGenome());
	if(organism == 0) {
		return 0;
	}

	cerr << "Checking for matching species: " << mSpecies.size() << endl;
	for(QListIterator<NeatSpecies*> i(mSpecies); i.hasNext();) {
		NeatSpecies *species = i.next();
		Individual *best = mSpeciesRepresentatives.value(species);
		if(best == 0) {
			continue;
		}
		NeatSpeciesOrganism *keyOrganism = dynamic_cast<NeatSpeciesOrganism*>(best->getGenome());

		if(keyOrganism == 0) {
			Core::log("NeatSelectionMethod: Individual genome was not a NeatSpeciesOrganism!");
			continue;
		}
		cerr << "Check 1" << endl;
		if(keyOrganism->isCompatible(organism, mGenomeCompatibilityThreshold->get())) {
			cerr << "Check 2" << endl;
			return species;
		}
	}
	//could not find a matching species, so create a new one.
	cerr << "Create new species" << endl;
	return new NeatSpecies();
}


}



