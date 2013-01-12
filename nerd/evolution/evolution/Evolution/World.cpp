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

#include "World.h"
#include <iostream>
#include "Core/Core.h"
#include "Core/SystemObject.h"
#include "Evolution/EvolutionAlgorithm.h"
#include "Evaluation/EvaluationMethod.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new World with the given name.
 * Parmameters are published with prefix /Evolution/Worlds/name.
 * 
 * @param name the name of this evolution World.
 */
World::World(const QString &name)
	: ParameterizedObject(name), mEvolutionAlgorithm(0), mEvaluationMethod(0),
	  mDeleteEvolutionAlgorithm(false), mDeleteEvaluationAlgorithm(false)
{
	setPrefix(QString("/Evo/Worlds/").append(name).append("/"));

	mControlledAgents = new StringValue("Default");
	addParameter("ControlledAgents", mControlledAgents, true);
}


/**
 * Destroys all Populations, the EvolutionAlgorithm and the EvaluationMethod.
 */
World::~World() {
	while(!mPopulations.empty()) {
		Population *pop = mPopulations.at(0);
		mPopulations.removeAll(pop);
		delete pop;
	}
	if(mEvolutionAlgorithm != 0 && mDeleteEvolutionAlgorithm) {
		mEvolutionAlgorithm->unpublishAllParameters();
		delete mEvolutionAlgorithm;	
	}
	if(mEvaluationMethod != 0 && mDeleteEvaluationAlgorithm) {
		mEvaluationMethod->unpublishAllParameters();
		delete mEvaluationMethod;
	}
}


/**
 * Adds a population to this world. All populations of a world are destroyed in the 
 * destructor of the World. 
 * 
 * @param population the population to add.
 * @return true if successful, false if population was NULL or if it was already added.
 */
bool World::addPopulation(Population *population) {
	if(population == 0 || mPopulations.contains(population)) {
		return false;
	}
	mPopulations.append(population);
	return true;
}


/**
 * Removes a population from this world.
 *
 * @param population the population to remove.
 * @return true if successful, false if population was NULL or it was not part of this world.
 */
bool World::removePopulation(Population *population) {
	if(population == 0 || !mPopulations.contains(population)) {
		return false;
	}
	mPopulations.removeAll(population);
	return true;
}


/**
 * Returns the list with all populations. 
 *
 * @return all populations of this world.
 */
QList<Population*> World::getPopulations() const {
	return mPopulations;
}


/**
 * Sets the EvolutionAlgorithm used for all populations of this world. 
 * The world will destroy the EvolutionAlgorithm in the destructor.
 * 
 * @param evolutionAlgorithm the new EvolutionAlgorithm.
 */
void World::setEvolutionAlgorithm(EvolutionAlgorithm *evolutionAlgorithm) {
	if(mEvolutionAlgorithm != evolutionAlgorithm && mEvolutionAlgorithm != 0) {
		mEvolutionAlgorithm->unpublishAllParameters();
	}
	mEvolutionAlgorithm = evolutionAlgorithm;
	if(mEvolutionAlgorithm != 0) {
		QString prefix("/Evo/" + getName() + "/Algorithm/");
		prefix.append(mEvolutionAlgorithm->getName()).append("/");
		mEvolutionAlgorithm->setPrefix(prefix);

		mEvolutionAlgorithm->publishAllParameters();

		mEvolutionAlgorithm->setOwnerWorld(this);
		
		//check if the EvolutionAlgorithm is a SystemObject (and likely is registered at the core)
		if(dynamic_cast<SystemObject*>(mEvolutionAlgorithm) == 0) {
			mDeleteEvolutionAlgorithm = true;
		}
		else {
			mDeleteEvolutionAlgorithm = false;
		}
	}
	else {
		mDeleteEvolutionAlgorithm = false;
	}
}


/**
 * Returns the EvolutionAlgorithm used for this World.
 * 
 * @rethr the EvolutionAlgorithm.
 */
EvolutionAlgorithm* World::getEvolutionAlgorithm() const {
	return mEvolutionAlgorithm;
}


/**
 * Sets the EvaluationMethod used for all populations of this world.
 * The world will destroy this EvaluationMethod in the destructor.
 * 
 * @param evaluationMethod the new EvaulationMethod.
 */
void World::setEvaluationMethod(EvaluationMethod *evaluationMethod) {
	if(mEvaluationMethod != evaluationMethod && mEvaluationMethod != 0) {
		mEvaluationMethod->unpublishAllParameters();
	}
	mEvaluationMethod = evaluationMethod;
	if(mEvaluationMethod != 0) {
		QString prefix("/Evo/Evaluation/");
		prefix.append(getName()).append("/")
			  .append(mEvaluationMethod->getName()).append("/");
		mEvaluationMethod->setPrefix(prefix);

		mEvaluationMethod->publishAllParameters();

		mEvaluationMethod->setOwnerWorld(this);

		//check if the EvaluationMethod is a SystemObject (and likely is registered at the core)
		if(dynamic_cast<SystemObject*>(mEvaluationMethod) == 0) {
			mDeleteEvaluationAlgorithm = true;
		}
		else {
			mDeleteEvaluationAlgorithm = false;
		}
	}
	else {
		mDeleteEvaluationAlgorithm = false;
	}
}


/**
 * Returns the EvaluationMethod responsible for the Evaluation in this world.
 * 
 * @return the EvaluationMethod obeject.
 */
EvaluationMethod* World::getEvaluationMethod() const {
	return mEvaluationMethod;
}

StringValue* World::getControlleAgentValue() const {
	return mControlledAgents;
}

}


