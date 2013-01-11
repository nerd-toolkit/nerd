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

#include "Population.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Fitness/FitnessManager.h"
#include "Fitness/Fitness.h"
#include "SelectionMethod/SelectionMethod.h"
#include "Phenotype/GenotypePhenotypeMapper.h"
#include "Core/Core.h"
#include "Core/SystemObject.h"
#include <iostream>
#include "Evolution/World.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new Population with the given name.
 * Parameters are published with a prefix /Evolution/Populations/name.
 *
 * @param name the name of this Population.
 */
Population::Population(const QString &name, World *ownerWorld)
	: ParameterizedObject(name), mGenotypePhenotypeMapper(0), mOwnerWorld(ownerWorld)
{
	QString prefix = "/Evo/Pop/";
	if(ownerWorld != 0) {
		prefix = "/Evo/" + ownerWorld->getName() + "/Pop/";
	}
	setPrefix(prefix.append(name).append("/"));

	mPopulationSizeValue = new IntValue(0);
	mDesiredPopulationSizeValue = new IntValue(1);
	mNumberOfPreservedParentsValue = new IntValue(1);

	addParameter("PopulationSize", mPopulationSizeValue, true);
	addParameter("DesiredPopulationSize", mDesiredPopulationSizeValue, true);
	addParameter("NumberOfPreservedParents", mNumberOfPreservedParentsValue, true);
}

/**
 * Destructor. Destroys all individuals, the GenotypePhenotypeMapper and all
 * SelectionMethods.
 */
Population::~Population() {
	while(!mIndividuals.empty()) {
		Individual *ind = mIndividuals.at(0);
		mIndividuals.removeAll(ind);
		delete ind;
	}
	if(mGenotypePhenotypeMapper != 0) {
		SystemObject *s = dynamic_cast<SystemObject*>(mGenotypePhenotypeMapper);
		if(s == 0) {
			delete mGenotypePhenotypeMapper;
		}
	}
	while(!mSelectionMethods.empty()) {
		SelectionMethod *selection = mSelectionMethods.at(0);
		mSelectionMethods.removeAll(selection);

		SystemObject *s = dynamic_cast<SystemObject*>(selection);
		if(s == 0) {
			delete selection;
		}
	}
}


/**
 * Returns a list with all Individuals of this Population.
 * This list is a non-constant reference to the internal data structure,
 * so this list can be used to add or remove individuals.
 *
 * @return all individuals of this Poplation.
 */
QList<Individual*>& Population::getIndividuals() {
	return mIndividuals;
}


/**
 * Adds a SelectionMethod to the Population.
 *
 * @param selectionMethod the SelectionMethod to add.
 * @return true if successful, false if NULL or already contained.
 */
bool Population::addSelectionMethod(SelectionMethod *selectionMethod) {
	if(selectionMethod == 0 || mSelectionMethods.contains(selectionMethod)) {
		return false;
	}
	mSelectionMethods.append(selectionMethod);
	selectionMethod->setPrefix(getPrefix().append("Selection/")
			.append(selectionMethod->getName()).append("/"));
	selectionMethod->setOwnerPopulation(this);
	selectionMethod->publishAllParameters();
	
	return true;
}


/**
 * Removes a SelectionMethod from the Population. 
 * 
 * @param selectionMethod the SelectionMethod to remove.
 * @return true is successful, false if seletionMethod was NULL or is not contained.
 */
bool Population::removeSelectionMethod(SelectionMethod *selectionMethod) {
	if(selectionMethod == 0 || !mSelectionMethods.contains(selectionMethod)) {
		return false;
	}
	mSelectionMethods.removeAll(selectionMethod);
	selectionMethod->setOwnerPopulation(0);
	selectionMethod->unpublishAllParameters();
	return true;
}


/**
 * Returns a list with all available SelectionMethods. 
 * 
 * @return all available SelectionMethods.
 */
QList<SelectionMethod*> Population::getSelectionMethods() const {
	return mSelectionMethods;
}



bool Population::addFitnessFunction(FitnessFunction *fitnessFunction) {
	if(fitnessFunction == 0 || mFitnessFunctions.contains(fitnessFunction)) {
		return false;
	}
	mFitnessFunctions.append(fitnessFunction);
	fitnessFunction->setPrefix(getPrefix().append("Fitness/")
			.append(fitnessFunction->getName()).append("/"));
	Fitness::getFitnessManager()->addFitnessFunction(fitnessFunction);
	fitnessFunction->attachToSystem();
	return true;
}		


bool Population::removeFitnessFunction(FitnessFunction *fitnessFunction) {
	if(fitnessFunction == 0 || !mFitnessFunctions.contains(fitnessFunction)) {
		return false;
	}
	mFitnessFunctions.removeAll(fitnessFunction);
	fitnessFunction->detachFromSystem();
	return true;
}


QList<FitnessFunction*> Population::getFitnessFunctions() const {
	return mFitnessFunctions;
}

/**
 * Sets the GenotypePhenotypeMapper used for the genomes of this population.
 * 
 * @param mapper the GenotypePhenotypeMapper to use.
 */
void Population::setGenotypePhenotypeMapper(GenotypePhenotypeMapper *mapper) {
	mGenotypePhenotypeMapper = mapper;
}

/**
 * Returns the GenotypePhenotypeMapper used for the genomes of this population. 
 * 
 * @return the used GenotypePhenotypeMapper.
 */
GenotypePhenotypeMapper* Population::getGenotypePhenotypeMapper() const {
	return mGenotypePhenotypeMapper;
}


/**
 * Returns the IntValue holding the actual size of the Population.
 * This actual size can differ from getDesiredPopulationSizeValue() depending
 * on the selection methods and the evolution algorithm.
 *
 * This Value is also available at the ValueManager as published parameter.
 * 
 * @return the IntValue holding the desired population size.
 */
IntValue* Population::getPopulationSizeValue() const {
	return mPopulationSizeValue;
}


/**
 * Returns the IntValue holding the desired population size of the Population.
 * This Value is also available at the ValueManager as published parameter.
 * 
 * @return the IntValue holding the desired population size.
 */
IntValue* Population::getDesiredPopulationSizeValue() const {
	return mDesiredPopulationSizeValue;
}

/**
 * Returns the IntValue holding the desired number of protected parents of the Population.
 * This Value is also available at the ValueManager as published parameter.
 * 
 * @return the IntValue holding the desired number of protected parents.
 */
IntValue* Population::getNumberOfPreservedParentsValue() const {
	return mNumberOfPreservedParentsValue;
}

World* Population::getOwnerWorld() const {
	return mOwnerWorld;
}

FitnessFunction* Population::getFitnessFunction(const QString &name) const {
	FitnessFunction *fitness = 0;
	for(int i = 0; i < mFitnessFunctions.size(); i++) {	
		if(mFitnessFunctions.at(i)->getName().compare(name) == 0) {
			fitness = mFitnessFunctions.at(i);
			break;
		}
	}
	return fitness;
}

}


