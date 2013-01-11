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

#include "Individual.h"
#include "Fitness/FitnessFunction.h"
namespace nerd {

int Individual::mIdCounter = 0;


/**
 * Constructs a new Individual with a pseudo-unique id.
 */
Individual::Individual()
	: Properties(), mGenome(0), mPhenotype(0), mProtectGenome(false)
{
	mId = mIdCounter;
	mIdCounter++;
	if(mIdCounter > 200000000) {
		mIdCounter = 0;
	}
	setProperty("ID", QString::number(mId));
}


/**
 * Constructs a new Individual with a given id.
 *
 * @param id the id of the Individual.
 */
Individual::Individual(int id) 
	: Properties(), mGenome(0), mPhenotype(0), mProtectGenome(false)
{
	mId = id;
	setProperty("ID", QString::number(mId));
}

/**
 * Destructor, destroys the attached genome and the phenotype!
 */
Individual::~Individual() {
	
	if(mGenome != mPhenotype) {
		delete mPhenotype;
	}
	delete mGenome;
}


/**
 * Returns the id of this Individual.
 * Usually individuals have different ids, but this is not guaranteed.
 *
 * @return the id of the Individual.
 */
int Individual::getId() const {
	return mId;
}


/**
 * Returns the genome of the Individual. This can be any subclass of Object.
 *
 * @return the gnome of the Individual.
 */
Object* Individual::getGenome() const {
	return mGenome;
}


/**
 * Sets the genome.
 * The individual will be responsible for the genome's destruction.
 *
 * @param genome the new genome of the Indivdual. 
 */
void Individual::setGenome(Object *genome) {
	mGenome = genome;
}


/**
 * Returns the Phenotype of the Indivudal.
 * This can be any subclass of Object.
 * 
 * @return the phenotype of the Individual.
 */
Object* Individual::getPhenotype() const {
	return mPhenotype;
}


/**
 * Sets the phenotype of this Individual.
 * The individual will be responsible for the phenotype's destruction.
 *
 * @param phenotype the new phenotype of the Individual. 
 */
void Individual::setPhenotype(Object *phenotype) {
	mPhenotype = phenotype;
}


/**
 * Returns the QList storing the direct parents of this Individual.
 * Usually this list is filled by the SelectionMethods.
 * After the execution of the EvolutionAlgorithm the list is cleared and the parents
 * are destroyed.
 *
 * @return the list with all direct parents.
 */
QList<Individual*>& Individual::getParents() {
	return mParents;
}

/**
 * Returns the fitness associated with FitnessFunction key.
 *
 * @param key the FitnessFunction that calculated the fitness.
 * @return the fitness calculated by FitnessFunction key.
 */
double Individual::getFitness(FitnessFunction *key) const {
	if(key == 0 || !mFitness.contains(key)) {
		return 0.0;
	}
	return mFitness.value(key);
}


/**
 * Sets the fitness calculated by FitnessFunction key.
 *
 * @param key the fitnessFunction to associate the fitness with.
 * @param fitness the fitness
 */
void Individual::setFitness(FitnessFunction *key, double fitness) {
	if(key == 0) {
		return;
	}
	mFitness.insert(key, fitness);
	
	setProperty("Fit - \"" + key->getName() + "\"", QString::number(fitness));
}


/**
 * Clears the fitness memory.
 */
void Individual::clearFitness() {
	mFitness.clear();
}


/**
 * Returns all FitnessFunctions that have associated fitness values.
 */
QList<FitnessFunction*> Individual::getFitnessFunctions() const {
	return mFitness.keys();
}



/**
 * Switches protection mode on or off. If protected an Individual usually is not 
 * modified during evolution. However this flag is only a hint that can be ignored
 * by EvolutionAlgorithms.
 * 
 * This flag usually is set by the Selection method to protect parents that survive
 * to the next generation without alteration.
 *
 * @param protect indicates whether to protect the genome or not.
 */
void Individual::protectGenome(bool protect) {
	mProtectGenome = protect;
}


/**
 * Returns true if the genome is protected.
 *
 * @return true if the genome is protected.
 */
bool Individual::isGenomeProtected() const {
	return mProtectGenome;
}


}


