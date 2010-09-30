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

#include "EvolutionManager.h"
#include "EvolutionConstants.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Event/EventManager.h"
#include <QListIterator>
#include "Evolution/World.h"
#include "Evolution/Population.h"
#include "Phenotype/GenotypePhenotypeMapper.h"
#include "SelectionMethod/SelectionMethod.h"
#include "Evolution/EvolutionAlgorithm.h"
#include "Evaluation/EvaluationMethod.h"
#include "Value/ValueManager.h"
#include <QTime>
#include <iostream>
#include "Math/Math.h"
#include <QDir>
#include <QDate>
#include <QTime>
#include "Util/Tracer.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);


using namespace std;

namespace nerd {

EvolutionManager::EvolutionManager()
	: mInitialized(false), mGenerationDuration(0), mEvolutionDuration(0), 
	  mSelectionDuration(0), mEvolutionAlgorithmDuration(0), mEvaluationDuration(0),
	  mRestartGenerationValue(0), mRestartGeneration(false), mEvolutionWorkingDirectory(0),
	  mDefaultEvaluationMethod(0)
{
	ValueManager *vm = Core::getInstance()->getValueManager();

	mCurrentGenerationNumberValue = new IntValue(0);
	mRestartGenerationValue = new BoolValue(false);
	mRestartGenerationValue->addValueChangedListener(this);
	mEvolutionWorkingDirectory = new StringValue(
		QDir::currentPath().append("/")
			.append(QDate::currentDate().toString("yyMMdd")).append("_")
			.append(QTime::currentTime().toString("hhmmss")).append("/"));
	mEvolutionWorkingDirectory->useAsFileName(true);
	mEvoIndividualsCompletedCounter = new IntValue(0);
	mEvalIndividualsCompletedCounter = new IntValue(0);

	vm->addValue(EvolutionConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER, 
					mCurrentGenerationNumberValue);
	vm->addValue(EvolutionConstants::VALUE_EVO_RESTART_GENERATION, 
					mRestartGenerationValue);
	vm->addValue(EvolutionConstants::VALUE_EVO_WORKING_DIRECTORY,
					mEvolutionWorkingDirectory);
	vm->addValue(EvolutionConstants::VALUE_NUMBER_OF_COMPLETED_INDIVIDUALS_EVO,
					mEvoIndividualsCompletedCounter);
	vm->addValue(EvolutionConstants::VALUE_NUMBER_OF_COMPLETED_INDIVIDUALS_EVAL,
					mEvalIndividualsCompletedCounter);

	EventManager *em = Core::getInstance()->getEventManager();
	
	mTriggerNextGenerationEvent =
 			em->createEvent(EvolutionConstants::EVENT_EVO_TRIGGER_NEXT_GENERATION);
	if(mTriggerNextGenerationEvent != 0) {
		mTriggerNextGenerationEvent->addEventListener(this);
	}

	mGenerationStartedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EVO_GENERATION_STARTED);
	mNextGenerationCompletedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EVO_GENERATION_COMPLETED);
	mEvaluationStartedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EVO_EVALUATION_STARTED);
	mEvaluationCompletedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EVO_EVALUATION_COMPLETED);
	mSelectionStartedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EVO_SELECTION_STARTED);
	mSelectionCompletedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EVO_SELECTION_COMPLETED);
	mEvolutionAlgorithmStartedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_STARTED);
	mEvolutionAlgorithmCompletedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_COMPLETED);
	mEvolutionTerminatedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EVO_EVOLUTION_TERMINATED);
	mEvolutionRestartedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EVO_EVOLUTION_RESTARTED);

	mGenerationDuration = new IntValue(0);
	mEvolutionDuration = new IntValue(0);
	mSelectionDuration = new IntValue(0);
	mEvolutionAlgorithmDuration = new IntValue(0);
	mEvaluationDuration = new IntValue(0);

	vm->addValue("/ExecutionTime/Evolution/EntireGeneration", mGenerationDuration);
	vm->addValue("/ExecutionTime/Evolution/EntireEvolution", mEvolutionDuration);
	vm->addValue("/ExecutionTime/Evolution/Selection", mSelectionDuration);
	vm->addValue("/ExecutionTime/Evolution/EvolutionAlgorithm", mEvolutionAlgorithmDuration);
	vm->addValue("/ExecutionTime/Evolution/Evaluation", mEvaluationDuration);
}

EvolutionManager::~EvolutionManager() {

	while(!mEvolutionWorlds.empty()) {
		World *world = mEvolutionWorlds.at(0);
		mEvolutionWorlds.removeAll(world);
		delete world;
	}

	//destroy own values if not registered at ValueManager.
	ValueManager *vm = Core::getInstance()->getValueManager();
	QList<Value*> valuesToRemove;
	valuesToRemove.append(mCurrentGenerationNumberValue);
	valuesToRemove.append(mRestartGenerationValue);
	valuesToRemove.append(mEvolutionWorkingDirectory);
	valuesToRemove.append(mGenerationDuration);
	valuesToRemove.append(mEvolutionDuration);
	valuesToRemove.append(mSelectionDuration);
	valuesToRemove.append(mEvolutionAlgorithmDuration);
	valuesToRemove.append(mEvaluationDuration);

	vm->removeValues(valuesToRemove);

	delete mCurrentGenerationNumberValue;
	delete mRestartGenerationValue;
	delete mEvolutionWorkingDirectory;
	delete mGenerationDuration;
	delete mEvolutionDuration;
	delete mSelectionDuration;
	delete mEvolutionAlgorithmDuration;
	delete mEvaluationDuration;
	delete mDefaultEvaluationMethod;
}

QString EvolutionManager::getName() const {
	return "EvolutionManager";
}

bool EvolutionManager::init() {
	bool ok = true;

	if(mTriggerNextGenerationEvent == 0
		|| mGenerationStartedEvent == 0
		|| mNextGenerationCompletedEvent == 0
		|| mEvaluationStartedEvent == 0
		|| mEvaluationCompletedEvent == 0
		|| mSelectionStartedEvent == 0
		|| mSelectionCompletedEvent == 0
		|| mEvolutionAlgorithmStartedEvent == 0
		|| mEvolutionAlgorithmCompletedEvent == 0)
	{
		Core::log("EvolutionManager: Could not create all required Events! [TERMINATING]");
		ok = false;
	}
	mInitialized = ok;

	return ok;
}


bool EvolutionManager::bind() {
	bool ok = true;

	return ok;
}


bool EvolutionManager::cleanUp() {
	return true;
}

void EvolutionManager::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mTriggerNextGenerationEvent) {
		processNextGeneration();
	}
}

void EvolutionManager::valueChanged(Value *value) {
	TRACE("EvolutionManager::valueChanged");

	if(value == 0) {
		return;
	}
	else if(value == mRestartGenerationValue && mRestartGenerationValue->get() == true) {
		//stop current evaluations.
		for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
			EvaluationMethod *evaluation = i.next()->getEvaluationMethod();
			if(evaluation != 0) {
				evaluation->stopEvaluation();
			}
		}
		mRestartGeneration = true;
	}
}

bool EvolutionManager::registerAsGlobalObject() {
	Core::getInstance()->addGlobalObject(
		EvolutionConstants::OBJECT_EVOLUTION_MANAGER, this);

	return Core::getInstance()->getGlobalObject(
		EvolutionConstants::OBJECT_EVOLUTION_MANAGER) == this;
}


bool EvolutionManager::addEvolutionWorld(World *world) {
	if(world == 0 || mEvolutionWorlds.contains(world)) {
		return false;
	}
	mEvolutionWorlds.append(world);
	return true;
}


bool EvolutionManager::removeEvolutionWorld(World *world) {
	if(world == 0 || !mEvolutionWorlds.contains(world)) {
		return false;
	}	
	mEvolutionWorlds.removeAll(world);
	return true;
}


const QList<World*>& EvolutionManager::getEvolutionWorlds() const {
	return mEvolutionWorlds;
}


/**
 * The default EvaluationMethod can be used to control the evaluation of several worlds together.
 */
void EvolutionManager::setDefaultEvaluationMethod(EvaluationMethod *evaluationMethod) {
	mDefaultEvaluationMethod = evaluationMethod;
}


EvaluationMethod* EvolutionManager::getDefaultEvaluationMethod() const {
	return mDefaultEvaluationMethod;
}


/**
 * Processes the next generation.
 * First the selection methods are called to create the individuals for the next
 * generation. Then the EvolutionAlgorithm is called to fill the newly created
 * individuals with suitable genomes. In the last step the EvaluationMethod is called
 * to evaluate the new generation.
 * 
 * All individuals of the old generation that are not part of the new generation any more
 * will be deleted automatically. This will happen after the EvolutionAlgorithm phase right
 * before the EvaluationStartedEvent is triggered. Parents therefore can be used savely
 * during the EvolutionAlgorithm phase.
 */
bool EvolutionManager::processNextGeneration() {
	TRACE("EvolutionManager::processNextGeneration");

	if(!mInitialized) {
		return false;
	}

	bool verbose = false;

	bool ok = true;

	Core *core = Core::getInstance();

	if(verbose) {
		cerr << "\nStarting generation of next generation" << endl;
	}

	//start timers for performance measurements
	QTime entireGenerationTime;
	QTime entireEvolutionTime;
	QTime time;
	entireGenerationTime.start();
	entireEvolutionTime.start();
	time.start();

	//increment current generation
	int generationId = mCurrentGenerationNumberValue->get() + 1;
	mCurrentGenerationNumberValue->set(generationId);
	mGenerationStartedEvent->trigger();

	//memorize individuals for destruction
	QList<Individual*> trashcan;
	core->executePendingTasks();

	if(core->isShuttingDown()) {
		return true;
	}

	//Use selection method to create the children stubs for the next generations.
	mSelectionStartedEvent->trigger();
	core->executePendingTasks();

	if(core->isShuttingDown()) {
		return true;
	}

	if(verbose) {
		cerr << "Selecting new individuals" << endl;
	}

	QHash<Population*, QList<Individual*> > newGenerations;

	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
		World *world = i.next();

		if(world->getEvolutionAlgorithm() == 0) {
			Core::log(QString("EvolutionManager: Could not find an EvolutionAlgorithm "
							  "for World [").append(world->getName()).append("]! {SKIPPING]"));
			mInitialized = false;
			return false;
		}


		const QList<Population*> &populations = world->getPopulations();
		for(QListIterator<Population*> j(populations); j.hasNext();) {
			Population *pop = j.next();
			trashcan << pop->getIndividuals();

			//remove genome protection from all individuals
			for(QListIterator<Individual*> i(pop->getIndividuals()); i.hasNext();) {
				i.next()->protectGenome(false);
			}

			//enforce at least one individual per population (to avoid uncontrollable evolutions).
			//TODO check how to do this better.
			int desiredPopulationSize = Math::max(pop->getDesiredPopulationSizeValue()->get(), 1);

			const QList<SelectionMethod*> &selectionMethods = pop->getSelectionMethods();
			QList<Individual*> newGeneration;
			double sum = 0.0;

			for(QListIterator<SelectionMethod*> k(selectionMethods); k.hasNext();) {
				sum += k.next()->getPopulationProportion()->get();
			}

			//avoid division by zero. //TODO check if this is ok
			if(sum == 0.0) {
				sum = 0.0001;
			}
			for(QListIterator<SelectionMethod*> k(selectionMethods); k.hasNext();) {
				SelectionMethod *selection = k.next();
				if(selection->getPopulationProportion()->get() <= 0.0) {
					continue;
				}
				QList<Individual*> individuals = selection->createSeed(pop->getIndividuals(),
						Math::abs((int) (((double) desiredPopulationSize) *
							(selection->getPopulationProportion()->get() / sum))),
						pop->getNumberOfPreservedParentsValue()->get(),
						world->getEvolutionAlgorithm()
							->getRequiredNumberOfParentsPerIndividual());

				for(QListIterator<Individual*> i(individuals); i.hasNext();) {
					Individual *ind = i.next();
					if(!newGeneration.contains(ind)) {
						newGeneration.append(ind);
					}

				}
			}


			//fill up population to the desired size.
			if(newGeneration.size() < desiredPopulationSize) {
				for(int i = newGeneration.size(); i < desiredPopulationSize; ++i) {
					newGeneration.append(new Individual());
				}
			}

			//pop->getIndividuals().clear();
			//pop->getIndividuals() << newGeneration;
			newGenerations.insert(pop, newGeneration);

			//remove all individuals from the trashcan that are still in use
			for(QListIterator<Individual*> newInd(newGeneration); newInd.hasNext();) {
				trashcan.removeAll(newInd.next());
			}

			core->executePendingTasks();

			if(core->isShuttingDown()) {
				return true;
			}
		}
	}

	if(verbose) {
		cerr << "Set parent properties" << endl;
	}

	//update parent property of old individuals 
	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
		World *world = i.next();
		const QList<Population*> &populations = world->getPopulations();
		for(QListIterator<Population*> j(populations); j.hasNext();) {
			Population *pop = j.next();
			if(pop == 0) {
				continue;
			}
			QList<Individual*> newGeneration = newGenerations.value(pop);
			QList<Individual*> oldGeneration = pop->getIndividuals();
			for(QListIterator<Individual*> k(oldGeneration); k.hasNext();) {
				Individual *parent = k.next();
				int firstParent = 0;
				int additionalParent = 0;
				for(QListIterator<Individual*> l(newGeneration); l.hasNext();) {
					Individual *ind = l.next();
					if(!ind->getParents().empty() && ind->getParents().first() == parent) {
						firstParent++;
					}
					if(ind->getParents().contains(parent)) {
						additionalParent++;
					}
				}		
				parent->setProperty("Offspring", QString::number(firstParent));
				parent->setProperty("ParentOf", QString::number(additionalParent));
			}
		}
	}

	if(verbose) {
		cerr << "Selection completed" << endl;
	}

	mSelectionCompletedEvent->trigger();
	mSelectionDuration->set(time.restart());
	core->executePendingTasks();

	if(core->isShuttingDown()) {
		return true;
	}

	if(verbose) {
		cerr << "update populations" << endl;
	}

	//update populations (set individuals of new generation)
	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
		World *world = i.next();
		const QList<Population*> &populations = world->getPopulations();
		for(QListIterator<Population*> j(populations); j.hasNext();) {
			Population *pop = j.next();
			if(pop == 0) {
				continue;
			}
			QList<Individual*> newGeneration = newGenerations.value(pop);
			pop->getIndividuals().clear();

			for(QListIterator<Individual*> k(newGeneration); k.hasNext();) {
				Individual *ind = k.next();
				pop->getIndividuals().append(ind);
				
				//remove marker for significant genome changes.
				ind->removeProperty(EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE);
			}

			//update actual population size
			pop->getPopulationSizeValue()->set(pop->getIndividuals().size());
		}
	}

	if(verbose) {
		cerr << "Apply EvolutionAlgorithm" << endl;
	}

	//Apply EvolutionAlgorithm to create the next generations
	mEvolutionAlgorithmStartedEvent->trigger();
	core->executePendingTasks();

	if(core->isShuttingDown()) {
		return true;
	}

	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
		World *world = i.next();
		if(world->getEvolutionAlgorithm() != 0) {
			//TODO automatically remove all individuals in trash from all populations.
			world->getEvolutionAlgorithm()->createNextGeneration(trashcan);
		}
		core->executePendingTasks();

		if(core->isShuttingDown()) {
			return true;
		}
	}

	if(verbose) {
		cerr << "EA completed" << endl;
	}

	mEvolutionAlgorithmCompletedEvent->trigger();
	mEvolutionAlgorithmDuration->set(time.restart());;
	mEvolutionDuration->set(entireEvolutionTime.elapsed());
	core->executePendingTasks();

	if(core->isShuttingDown()) {
		return true;
	}

// 	//Apply GenotypePhenotypeMappers to create the Phenotypes of the new individuals
// 	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
// 		World *world = i.next();
// 		if(world->getEvaluationMethod() != 0) {
// 			const QList<Population*> &populations = i.next()->getPopulations();
// 			for(QListIterator<Population*> j(populations); j.hasNext();) {
// 				Population *pop = j.next();
// 				GenotypePhenotypeMapper *mapper = pop->getGenotypePhenotypeMapper();
// 				if(mapper != 0) {
// 					QList<Individual*> &individuals = pop->getIndividuals();
// 					for(QListIterator<Individual*> k(individuals); k.hasNext();) {
// 						mapper->createPhenotype(k.next());
// 					}
// 				}
// 			}
// 		}
// 	}

	if(verbose) {
		cerr << "Remove parent pointers from children" << endl;
	}

	//delete parent pointers (because parents may be deleted hereafter, 
	//so danging pointers have to be avoided.
	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
		World *world = i.next();
		const QList<Population*> &populations = world->getPopulations();
		for(QListIterator<Population*> j(populations); j.hasNext();) {
			Population *pop = j.next();
			const QList<Individual*> &individuals = pop->getIndividuals();
			for(QListIterator<Individual*> i(individuals); i.hasNext();) {
				i.next()->getParents().clear();
			}
		}
		core->executePendingTasks();

		if(core->isShuttingDown()) {
			return true;
		}
	}

	if(verbose) {
		cerr << "Destroy dead individuals" << endl;
	}

	//destroy all individuals in the trash can.
	while(!trashcan.empty()) {
		Individual *ind = trashcan.at(0);
		trashcan.removeAll(ind);
		delete ind;
	}

	if(verbose) {
		cerr << "Start Evaluation" << endl;
	}

	mRestartGeneration = true;
	while(mRestartGeneration) {
		//Evaluate individuals and apply FitnessFunctions
		mEvaluationStartedEvent->trigger();  //Note:: this event can occure multiple times pre gen.
		core->executePendingTasks();

		if(core->isShuttingDown()) {
			return true;
		}

		mRestartGenerationValue->set(false);
		mRestartGeneration = false;
		
		for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
			World *world = i.next();
	
			if(world->getEvaluationMethod() != 0) {
				world->getEvaluationMethod()->evaluateIndividuals();
			}
			core->executePendingTasks();

			if(core->isShuttingDown()) {
				return true;
			}
		}	

		//check if there is a default evaluation method
		if(mDefaultEvaluationMethod != 0) {
			mDefaultEvaluationMethod->evaluateIndividuals();
			core->executePendingTasks();

			if(core->isShuttingDown()) {
				return true;
			}
		}

		if(mRestartGenerationValue->get()) {
			mRestartGeneration = true;
		}
	}

	if(verbose) {
		cerr << "Eval done" << endl;
	}

	mEvaluationCompletedEvent->trigger();
	mEvaluationDuration->set(time.restart());
	core->executePendingTasks();

	if(core->isShuttingDown()) {
		return true;
	}

	if(verbose) {
		cerr << "Generation done" << endl;
	}

	mNextGenerationCompletedEvent->trigger();
	mGenerationDuration->set(entireGenerationTime.elapsed());
	core->executePendingTasks();

	if(core->isShuttingDown()) {
		return true;
	}

	if(verbose) {
		cerr << "EvolutionManager::processNextGeneration finished." << endl;
	}

	return ok;
}


bool EvolutionManager::restartEvolution() {

	if(!mInitialized) {
		return false;
	}

	bool verbose = false;

	Core *core = Core::getInstance();

	if(verbose) {
		cerr << "\nRestarting evolution!" << endl;
	}

	//notify evolution terminated
	mEvolutionTerminatedEvent->trigger();
	core->executePendingTasks();

	if(core->isShuttingDown()) {
		return true;
	}

	//reset generation number
	mCurrentGenerationNumberValue->set(0);

	//memorize all individuals for destruction
	QList<Individual*> trashcan;
	core->executePendingTasks();

	if(core->isShuttingDown()) {
		return true;
	}


	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
		World *world = i.next();

		if(world->getEvolutionAlgorithm() == 0) {
			Core::log(QString("EvolutionManager: Could not find an EvolutionAlgorithm "
							  "for World [").append(world->getName()).append("]! {SKIPPING]"));
			mInitialized = false;
			return false;
		}

		if(verbose) {
			cerr << "Resetting selection methods and populations." << endl;
		}

		const QList<Population*> &populations = world->getPopulations();
		for(QListIterator<Population*> j(populations); j.hasNext();) {
			Population *pop = j.next();

			//discard individuals
			trashcan << pop->getIndividuals();
			pop->getIndividuals().clear();

			//reset selection methods
			const QList<SelectionMethod*> &selectionMethods = pop->getSelectionMethods();
			for(QListIterator<SelectionMethod*> k(selectionMethods); k.hasNext();) {
				SelectionMethod *selection = k.next();
				selection->reset();
			}

			core->executePendingTasks();
			if(core->isShuttingDown()) {
				return true;
			}
		}
	}

	core->executePendingTasks();
	if(core->isShuttingDown()) {
		return true;
	}

	if(verbose) {
		cerr << "Reset EvolutionAlgorithms and EvaluationAlgorithms." << endl;
	}

	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
		World *world = i.next();
		if(world->getEvolutionAlgorithm() != 0) {
			world->getEvolutionAlgorithm()->reset();
		}
		if(world->getEvaluationMethod() != 0) {
			world->getEvaluationMethod()->reset();
		}

		core->executePendingTasks();
		if(core->isShuttingDown()) {
			return true;
		}
	}

	if(verbose) {
		cerr << "Destroy all individuals." << endl;
	}

	//destroy all individuals in the trash can.
	while(!trashcan.empty()) {
		Individual *ind = trashcan.at(0);
		trashcan.removeAll(ind);
		delete ind;
	}

	core->executePendingTasks();
	if(core->isShuttingDown()) {
		return true;
	}

	if(verbose) {
		cerr << "Restarting evolution completed." << endl;
	}

	initEvolution();

	mEvolutionRestartedEvent->trigger();

// 	return;
// 
// 	mCurrentGenerationNumberValue->set(0);
// 
// 	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
// 		World *world = i.next();
// 		if(world->getEvaluationMethod() != 0) {
// 			world->getEvaluationMethod()->reset();
// 		}
// 		if(world->getEvolutionAlgorithm() != 0) {
// 			world->getEvolutionAlgorithm()->reset();
// 		}
// 	}

	return true;
}


/**
 * Returns the Event used to trigger the processing of the next generation.
 * This Event can be used to start method processNextGeneration() instead of
 * a direct call of that method.
 *
 * @return the Event to trigger the processing of the next generation.
 */
Event* EvolutionManager::getNextGenerationTriggerEvent() const {
	return mTriggerNextGenerationEvent;
}
 


IntValue* EvolutionManager::getCurrentGenerationValue() const {
	return mCurrentGenerationNumberValue;
}



QString EvolutionManager::getEvolutionWorkingDirectory() const {
	return mEvolutionWorkingDirectory->get();
}

StringValue* EvolutionManager::getEvolutionWorkingDirectoryValue() const {
	return mEvolutionWorkingDirectory;
}

BoolValue* EvolutionManager::getRestartGenerationValue() const {
	return mRestartGenerationValue;
}

void EvolutionManager::setCurrentNumberOfCompletedIndividualsDuringEvolution(int count) {
	mEvoIndividualsCompletedCounter->set(count);
}


void EvolutionManager::setCurrentNumberOfCompletedIndividualsDuringEvaluation(int count) {
	mEvalIndividualsCompletedCounter->set(count);
}

void EvolutionManager::initEvolution() {
	mEvolutionWorkingDirectory->set(
		QDir::currentPath().append("/")
			.append(QDate::currentDate().toString("yyMMdd")).append("_")
			.append(QTime::currentTime().toString("hhmmss")).append("/"));
	mEvoIndividualsCompletedCounter->set(0);
	mEvalIndividualsCompletedCounter->set(0);
}


// void EvolutionManager::restartEvolution() {
// 	if(!mInitialized) {
// 		return;
// 	}
// 
// 	bool verbose = false;
// 
// 	Core *core = Core::getInstance();
// 
// 	if(verbose) {
// 		cerr << "\nRestarting evolution!" << endl;
// 	}
// 
// 	//notify evolution terminated
// 	mEvolutionTerminatedEvent->trigger();
// 	core->executePendingTasks();
// 
// 	if(core->isShuttingDown()) {
// 		return;
// 	}
// 
// 	//reset generation number
// 	mCurrentGenerationNumberValue->set(0);
// 
// 	//memorize all individuals for destruction
// 	QList<Individual*> trashcan;
// 	core->executePendingTasks();
// 
// 	if(core->isShuttingDown()) {
// 		return;
// 	}
// 
// 
// 	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
// 		World *world = i.next();
// 
// 		if(world->getEvolutionAlgorithm() == 0) {
// 			Core::log(QString("EvolutionManager: Could not find an EvolutionAlgorithm "
// 							  "for World [").append(world->getName()).append("]! {SKIPPING]"));
// 			mInitialized = false;
// 			return;
// 		}
// 
// 		if(verbose) {
// 			cerr << "Resetting selection methods and populations." << endl;
// 		}
// 
// 		const QList<Population*> &populations = world->getPopulations();
// 		for(QListIterator<Population*> j(populations); j.hasNext();) {
// 			Population *pop = j.next();
// 
// 			//discard individuals
// 			trashcan << pop->getIndividuals();
// 			pop->getIndividuals().clear();
// 
// 			//reset selection methods
// 			const QList<SelectionMethod*> &selectionMethods = pop->getSelectionMethods();
// 			for(QListIterator<SelectionMethod*> k(selectionMethods); k.hasNext();) {
// 				SelectionMethod *selection = k.next();
// 				selection->reset();
// 			}
// 
// 			core->executePendingTasks();
// 			if(core->isShuttingDown()) {
// 				return;
// 			}
// 		}
// 	}
// 
// 	
// 
// 	if(verbose) {
// 		cerr << "update populations" << endl;
// 	}
// 
// 	//update populations (set individuals of new generation)
// 	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
// 		World *world = i.next();
// 		const QList<Population*> &populations = world->getPopulations();
// 		for(QListIterator<Population*> j(populations); j.hasNext();) {
// 			Population *pop = j.next();
// 			if(pop == 0) {
// 				continue;
// 			}
// 			QList<Individual*> newGeneration = newGenerations.value(pop);
// 			pop->getIndividuals().clear();
// 
// 			for(QListIterator<Individual*> k(newGeneration); k.hasNext();) {
// 				Individual *ind = k.next();
// 				pop->getIndividuals().append(ind);
// 				
// 				//remove marker for significant genome changes.
// 				ind->removeProperty(EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE);
// 			}
// 
// 			//update actual population size
// 			pop->getPopulationSizeValue()->set(pop->getIndividuals().size());
// 		}
// 	}
// 
// 	if(verbose) {
// 		cerr << "Apply EvolutionAlgorithm" << endl;
// 	}
// 
// 	core->executePendingTasks();
// 	if(core->isShuttingDown()) {
// 		return;
// 	}
// 
// 	if(verbose) {
// 		cerr << "Reset EvolutionAlgorithms." << endl;
// 	}
// 
// 	for(QListIterator<World*> i(mEvolutionWorlds); i.hasNext();) {
// 		World *world = i.next();
// 		if(world->getEvolutionAlgorithm() != 0) {
// 			world->getEvolutionAlgorithm()->reset();
// 		}
// 
// 		core->executePendingTasks();
// 		if(core->isShuttingDown()) {
// 			return;
// 		}
// 	}
// 
// 	if(verbose) {
// 		cerr << "Destroy all individuals." << endl;
// 	}
// 
// 	//destroy all individuals in the trash can.
// 	while(!trashcan.empty()) {
// 		Individual *ind = trashcan.at(0);
// 		trashcan.removeAll(ind);
// 		delete ind;
// 	}
// 
// 	core->executePendingTasks();
// 	if(core->isShuttingDown()) {
// 		return;
// 	}
// 
// 	if(verbose) {
// 		cerr << "Restarting evolution completed." << endl;
// 	}
// 
// 	return;
// }

}


