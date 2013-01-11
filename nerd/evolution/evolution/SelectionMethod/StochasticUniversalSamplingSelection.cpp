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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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



#include "StochasticUniversalSamplingSelection.h"
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
#include "Util/Tracer.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {


/**
 * Constructs a new StochasticUniversalSamplingSelection.
 */
StochasticUniversalSamplingSelection::StochasticUniversalSamplingSelection()
	: SelectionMethod("StochasticUniversalSampling")
{
	mBestPercentage = new NormalizedDoubleValue(1.0, 0.1, 1.0, 0.1, 1.0);
	addParameter("mBestPercentage", mBestPercentage);
}


/**
 * Copy constructor. 
 * 
 * @param other the StochasticUniversalSamplingSelection object to copy.
 */
StochasticUniversalSamplingSelection::StochasticUniversalSamplingSelection(
							const StochasticUniversalSamplingSelection &other)
	: Object(), ValueChangedListener(), SelectionMethod(other)
{
	mBestPercentage = dynamic_cast<NormalizedDoubleValue*>(
						getParameter("mBestPercentage"));
}

/**
 * Destructor.
 */
StochasticUniversalSamplingSelection::~StochasticUniversalSamplingSelection() {
}


SelectionMethod* StochasticUniversalSamplingSelection::createCopy() const {
	return new StochasticUniversalSamplingSelection();
}

QList<Individual*> StochasticUniversalSamplingSelection::createSeed(
									QList<Individual*> currentGeneration,
									int numberOfIndividuals, 
									int numberOfPreservedParents,
									int numberOfParentsPerIndividual)
{
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
		|| mResponsibleFitnessFunction == 0) 
	{
		return newGeneration;
	}

	//calculate fitness sum
	double minFitness = 0.0;

	QList<double> fitnessValeus;
	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {
		Individual *ind = i.next();
		double fit = ind->getFitness(mResponsibleFitnessFunction);
		minFitness = Math::min(minFitness, fit);
		fitnessValeus.append(fit);
	}
	double fitnessSum = 0.0;
	for(QListIterator<double> i(fitnessValeus); i.hasNext();) {
		fitnessSum += (-minFitness + i.next());
	}

	int numberOfChildren = numberOfIndividuals - newGeneration.size();
	double bestPercentage = mBestPercentage->get();

	if(numberOfChildren <= 0 || bestPercentage == 0.0) {
		return newGeneration;
	}

	QList<Individual*> parents;
	
	double fraction = fitnessSum / ((double) numberOfChildren);

	double sum = 0.0;
	int childCount = 1;
	for(QListIterator<Individual*> i(parentGeneration); i.hasNext();) {

		if(parents.size() >= numberOfChildren) {
			break;
		}

		Individual *ind = i.next();
		double fit = ind->getFitness(mResponsibleFitnessFunction);
		sum += (fit / bestPercentage);

		double pos = fraction * ((double) childCount);
		while(pos <= sum && parents.size() < numberOfChildren) {
			parents.append(ind);
			++childCount;
			pos = fraction * ((double) childCount);
		}
	}
	
	
	QList<QList<Individual*>*> additionalParents;
	for(int i = 0; i < numberOfParentsPerIndividual; ++i) {
		additionalParents.append(new QList<Individual*>(parents));
	}

	for(QListIterator<Individual*> i(parents); i.hasNext();) {
		Individual *mainParent = i.next();

		Individual *child = new Individual();
		child->getParents().append(mainParent);

		for(int j = 0; j < numberOfParentsPerIndividual; ++j) {
			QList<Individual*> *parents = additionalParents.at(j);
			Individual *parent = parents->takeAt(Random::nextInt(parents->size()));
			child->getParents().append(parent);
		}
		newGeneration.append(child);
	}

	for(QListIterator<QList<Individual*>*> i(additionalParents); i.hasNext();) {
		delete i.next();
	}

	return newGeneration;
}


}



