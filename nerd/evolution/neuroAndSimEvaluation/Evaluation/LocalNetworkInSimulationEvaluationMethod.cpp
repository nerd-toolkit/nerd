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

#include "LocalNetworkInSimulationEvaluationMethod.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include "Physics/SimObjectGroup.h"
#include "Control/ControlInterface.h"
#include "Core/Core.h"
#include "Evolution/Population.h"
#include "Evolution/World.h"
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include <QListIterator>
#include "Network/NeuralNetwork.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include "Fitness/Fitness.h"
#include "Fitness/FitnessManager.h"
#include <iostream>
#include "EvolutionConstants.h"
#include "NerdConstants.h"
#include "Fitness/Fitness.h"
#include "EvolutionConstants.h"
#include "Value/ValueManager.h"
#include "Math/Random.h"
#include "Util/Tracer.h"
#include "Fitness/ControllerFitnessFunction.h"
#include "Math/Math.h"
#include "Math/Random.h"


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {

LocalNetworkInSimulationEvaluationMethod::LocalNetworkInSimulationEvaluationMethod()
	: EvaluationMethod("LocalNetworkInSimulationEvaluation"),
	  mEvaluationLoop(0), mIndividualStartedEvent(0), mIndividualCompletedEvent(0), 
	  mShutDownEvent(0), mSimEnvironmentChangedEvent(0), 
	  mDoShutDown(false), mCurrentIndividualValue(0),
	  mRestartIndividual(false), mRandomizeSeed(0), mDesiredSeed(0), mSimulationSeed(0)
{
	mEvaluationLoop = new EvaluationLoop(1000, 1);

	EventManager *em = Core::getInstance()->getEventManager();

	mIndividualStartedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL);
	mIndividualCompletedEvent =
			em->createEvent(EvolutionConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED);

	if(mIndividualStartedEvent == 0) {
		Core::log("LocalNetworkInSimulationEvaluationMethod: Could not create individual started event");
	}
	if(mIndividualCompletedEvent == 0) {
		Core::log("LocalNetworkInSimulationEvaluationMethod: Could not create individual completed event");
	}
	
	mCurrentIndividualValue = new IntValue(0);
	mRunStasisMode = new BoolValue(false);
	mRunStasisMode->addValueChangedListener(this);
	mRandomizeSeed = new BoolValue(true);
	mDesiredSeed = new IntValue(Random::nextInt());
	mMaximumNumberOfPartnersPerEvaluation = new IntValue(1);

	Core::getInstance()->addSystemObject(this);

	addParameter("CurrentIndividual", mCurrentIndividualValue, true);
	addParameter("Stasis", mRunStasisMode, true);
	addParameter("RandomizeSeed", mRandomizeSeed, true);
	addParameter("DesiredSeed", mDesiredSeed, true);
	addParameter("MaxNumberOfPartnersPerEval", mMaximumNumberOfPartnersPerEvaluation, true);

	//add statis mode parameter also on global path
	Core::getInstance()->getValueManager()->addValue(
				EvolutionConstants::VALUE_EVO_STASIS_MODE, mRunStasisMode);
	Core::getInstance()->getValueManager()->addValue(
				EvolutionConstants::VALUE_EXECUTION_CURRENT_INDIVIDUAL, mCurrentIndividualValue);

	Fitness::install();
}

/**
 * Copy constructor. All EvaluationMethod copies use the same EvaluationMethod.
 * TODO: check if the EvaluationLoop should be independent of the LocalNetworkInSimEvalMethod.
 * TODO: check if LocalNetworkInSimEvalM. should be a SystemObject.
 */
LocalNetworkInSimulationEvaluationMethod::LocalNetworkInSimulationEvaluationMethod(
				const LocalNetworkInSimulationEvaluationMethod &other)
	: Object(), ValueChangedListener(), EventListener(), SystemObject(), 
	  EvaluationMethod(other),
	  mEvaluationLoop(0), mIndividualStartedEvent(0), mIndividualCompletedEvent(0), 
	  mShutDownEvent(0), mSimEnvironmentChangedEvent(0), 
	  mDoShutDown(false), mCurrentIndividualValue(0),
	  mRestartIndividual(false), mRandomizeSeed(0), mDesiredSeed(0), mSimulationSeed(0)
{
	mEvaluationLoop = other.mEvaluationLoop;
	Core::getInstance()->addSystemObject(this);

	mCurrentIndividualValue = dynamic_cast<IntValue*>(
			getParameter("CurrentIndividual"));
	mRunStasisMode = dynamic_cast<BoolValue*>(
			getParameter(EvolutionConstants::VALUE_EVO_STASIS_MODE));
	mRandomizeSeed = dynamic_cast<BoolValue*>(
			getParameter("RandomizeSeed"));
	mDesiredSeed = dynamic_cast<IntValue*>(
			getParameter("DesiredSeed"));
	mMaximumNumberOfPartnersPerEvaluation = dynamic_cast<IntValue*>(
			getParameter("maxNumberOfPartnersPerEval"));

	if(mRunStasisMode != 0) {
		mRunStasisMode->addValueChangedListener(this);
	}
	Core::log("LocalNetworkInSimulationEvaluationMethod: "
				"COPY Constructor is not fully supported yet!", true);
}

LocalNetworkInSimulationEvaluationMethod::~LocalNetworkInSimulationEvaluationMethod() {
}

EvaluationMethod* LocalNetworkInSimulationEvaluationMethod::createCopy() {
	return new LocalNetworkInSimulationEvaluationMethod(*this);
}


bool LocalNetworkInSimulationEvaluationMethod::init() {
	bool ok = true;

	EvolutionManager *em = Evolution::getEvolutionManager();
	QList<World*> worlds = em->getEvolutionWorlds();

	if(mControllerNames.empty()) {
		for(int i = 0; i < worlds.size(); ++i) {
			World *world = worlds.at(i);
			mControllerNames.append(world->getControlleAgentValue());
		}
	}

	return ok;
}


bool LocalNetworkInSimulationEvaluationMethod::bind() {
	TRACE("LocalNetworkInSimulationEvaluationMethod::bind");

	bool ok = true;

	mShutDownEvent = Core::getInstance()->getEventManager()->registerForEvent(
			NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this);
	mSimEnvironmentChangedEvent = Core::getInstance()->getEventManager()->registerForEvent(
			EvolutionConstants::EVENT_PHYSICS_ENVIRONMENT_CHANGED, this);

	mRandomizeEnvironmentEvent = Core::getInstance()->getEventManager()->registerForEvent(
			SimulationConstants::EVENT_RANDOMIZE_ENVIRONMENT, this);

	if(mShutDownEvent == 0) {
		ok = false;
		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: Could not find  Event [")
					.append(NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN).append("]"));
	}
	if(mSimEnvironmentChangedEvent == 0) {
		Core::log("LocalNetworkInSimulationEvaluationMethod: Will not react on environment changes");
	}

	ValueManager *vm = Core::getInstance()->getValueManager();

	mSimulationSeed = vm->getIntValue(SimulationConstants::VALUE_RANDOMIZATION_SIMULATION_SEED);
	mCurrentTryValue = vm->getIntValue(SimulationConstants::VALUE_EXECUTION_CURRENT_TRY);

	//TODO remove, just for test purposes
// 	mSimulationSeed = new IntValue(0);

	if(mSimulationSeed == 0 || mCurrentTryValue == 0) {
		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: Could not find "
					"required Value [")
					.append(SimulationConstants::VALUE_RANDOMIZATION_SIMULATION_SEED)
					.append("]! [TERMINATING]"));
		ok = false;
	}

	return ok;
}


bool LocalNetworkInSimulationEvaluationMethod::cleanUp() {
	return true;
}

void LocalNetworkInSimulationEvaluationMethod::valueChanged(Value *value) {
	TRACE("LocalNetworkInSimulationEvaluationMethod::valueChanged");

	EvaluationMethod::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mRunStasisMode && mRunStasisMode->get()) {
		mRestartIndividual = true;
	}
}


void LocalNetworkInSimulationEvaluationMethod::eventOccured(Event *event) {
	TRACE("LocalNetworkInSimulationEvaluationMethod::eventOccured");

	if(event == 0) {
		return;
	}
	else if(event == mShutDownEvent) {
		mDoShutDown = true;
	}
	else if(event == mSimEnvironmentChangedEvent) {
		setupController();
		setupFitnessFunctions();
	}
	else if(event == mRandomizeEnvironmentEvent) {
		if(mCurrentTryValue->get() > 0) { //TODO check if this should be done here in every try!
			setNetworksForNextTry(mCurrentTryValue->get());
		}
	}
}

QString LocalNetworkInSimulationEvaluationMethod::getName() const {
	return EvaluationMethod::getName();
}

bool LocalNetworkInSimulationEvaluationMethod::evaluateIndividuals() {
	TRACE("LocalNetworkInSimulationEvaluationMethod::evaluateIndividuals");

	EvolutionManager *evoManager = Evolution::getEvolutionManager();

	if(mDoShutDown) {
		return true;
	}

	mStopEvaluation = false;

	//check if all required objects are available.
	if(mOwnerWorld == 0) {
		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Could not find an owner World "));
		return false;
	}

	if(mOwnerWorld->getPopulations().empty()) {
		return false;
	}

	mPopulations.clear();
	QList<World*> worlds = evoManager->getEvolutionWorlds();
	for(int i = 0; i < worlds.size(); ++i) {
		World *world = worlds.at(i);
		QList<Population*> populations = world->getPopulations();
		if(!populations.empty()) {
			mPopulations.append(populations.at(0));
		}
		else {
			Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Could not find a population in World [")
				.append(mOwnerWorld->getName()).append("]"), true);
			return false;
		}
	}

	if(mPopulations.empty()) {
		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Could not find any population in entire evolution scenario!"));
		return false;
	}

	//TODO multi pop
// 	Population *pop = mPopulations.at(0);
// 
// 	GenotypePhenotypeMapper *mapper = pop->getGenotypePhenotypeMapper();

// 	if(mapper == 0) {
// 		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
// 				" Could not find a GenotypePhenotypeMapper for population [")
// 				.append(pop->getName()).append("]"));
// 		return false;
// 	}

	//set generation fitness
	if(mRandomizeSeed->get()) {
		Random::init();
		mDesiredSeed->set(Random::nextInt());
	}
	mSimulationSeed->set(mDesiredSeed->get());

	if(!setupController()) {
		Core::log("LocalNetworkInSimulationEvaluationMethod: Could not find a controller.", true);
		return false;
	}

	Core::getInstance()->executePendingTasks();

	setupFitnessFunctions();

	setupEvaluationPairs();

	//execute all individuals one after the other. 
// 	QList<Individual*> individuals = pop->getIndividuals();

	NeuralNetworkManager *networkManager = Neuro::getNeuralNetworkManager();

// 	QList<FitnessFunction*> fitnessFunctions = pop->getFitnessFunctions();

	int currentIndividual = 0;
	for(QListIterator<QList<QList<Individual*> > > i(mEvaluationPairs); i.hasNext();) {

		Core::getInstance()->executePendingTasks();
	
		if(mStopEvaluation || mDoShutDown || evoManager->getRestartGenerationValue()->get()) {
			break;
		}

		evoManager->setCurrentNumberOfCompletedIndividualsDuringEvaluation(currentIndividual);

		QList<QList<Individual*> > evalPairs = i.next();

		mCurrentPermutation = evalPairs;

		if(mCurrentPermutation.empty()) {
			continue;
		}
		
		setNetworksForNextTry(0);
	
		mCurrentIndividualValue->set(currentIndividual);
	
		networkManager->triggerCurrentNetworksReplacedEvent();


		//execute individual
		mRestartIndividual = true;
		while(mRestartIndividual) {
			mRestartIndividual = false;

			Core::getInstance()->executePendingTasks();

			mIndividualStartedEvent->trigger();
			mEvaluationLoop->executeEvaluationLoop();
			mIndividualCompletedEvent->trigger();

			if(mRunStasisMode->get() && !mStopEvaluation && !mDoShutDown) {
				mRestartIndividual = true;
			}
		}

		Core::getInstance()->executePendingTasks();

		for(int k = 0; k < mCurrentNeuralNetworks.size(); ++k) {
			networkManager->removeNeuralNetwork(mCurrentNeuralNetworks[k]);
		}
		networkManager->triggerCurrentNetworksReplacedEvent();

		QList<Individual*> individuals = mCurrentPermutation[mCurrentTryValue->get() % mCurrentPermutation.size()];

		//save fitness values
		for(int k = 0; k < individuals.size(); ++k) {

			Population *pop = mPopulations[k];

			if(pop == 0) {
				continue;
			}

			QList<FitnessFunction*> fitnessFunctions = pop->getFitnessFunctions();

			for(QListIterator<FitnessFunction*> i(fitnessFunctions); i.hasNext();) {
				FitnessFunction *ff = i.next();
				if(ff != 0) {
					individuals[k]->setFitness(ff, ff->getFitness());
				}
			}

			NeuralNetwork *net = mCurrentNeuralNetworks[k];

			//clear ControlInterface
			mControlInterfaces[k] = net->getControlInterface(); //in case the interface was changed.
			net->setControlInterface(0);
			if(mControlInterfaces[k] != 0) {
				mControlInterfaces[k]->setController(0);
			}

	
			//allow changes made in phenotype to be transferred to the genome.
			pop->getGenotypePhenotypeMapper()->updateGenotype(individuals[k]);
		}
		currentIndividual++;
	
		if(mDoShutDown) {
			return true;
		}
	}
	evoManager->setCurrentNumberOfCompletedIndividualsDuringEvaluation(mEvaluationPairs.size());

	if(mDoShutDown) {
		return true;
	}
	
	//detach fitness functions from ControlInterface and FitnessManager.
	for(QListIterator<FitnessFunction*> i(mCurrentFitnessFunctions); i.hasNext();) {
		ControllerFitnessFunction *ff = dynamic_cast<ControllerFitnessFunction*>(i.next());
		if(ff != 0) {
			ff->setControlInterface(0);
		}
	}
	Core::getInstance()->executePendingTasks();

	return true;


}


bool LocalNetworkInSimulationEvaluationMethod::reset() {
	return true;
}

void LocalNetworkInSimulationEvaluationMethod::stopEvaluation() {
	mStopEvaluation = true;
}


bool LocalNetworkInSimulationEvaluationMethod::setupController() {
	//find controlInterface to attach fitness and neural network to.
	mControlInterfaces.clear();	
	for(int i = 0; i < mControllerNames.size(); ++i) {
		if(mControllerNames[i]->get() == "Default" 
				&& !Physics::getPhysicsManager()->getSimObjectGroups().empty()) 
		{
			bool foundController = false;
			QList<SimObjectGroup*> groups = Physics::getPhysicsManager()->getSimObjectGroups();
			for(int j = 0; j < groups.size(); ++j) {
				SimObjectGroup *group = groups.at(j);
				if(!mControlInterfaces.contains(group)) {
					mControlInterfaces.append(group);
					foundController = true;
					break;
				}
			}
			if(!foundController) {
				Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
						" Could not find controller [")
						.append(mControllerNames[i]->get()).append("]! [IGNORING]"), true);
		
				return false;
			}
		}
		else {
			SimObjectGroup *group = Physics::getPhysicsManager()
									->getSimObjectGroup(mControllerNames[i]->get());
			if(group != 0) {
				mControlInterfaces.append(group);
			}
			else {
				Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
						" Could not find controller [")
						.append(mControllerNames[i]->get()).append("]! [IGNORING]"), true);
		
				return false;
			}
		}
	}

	if(mControlInterfaces.size() != mControllerNames.size()) {
		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Could not find all required controllers! [IGNORING]"), true);

		return false;
	}
	return true;
}

void LocalNetworkInSimulationEvaluationMethod::setupFitnessFunctions() {
	//attach fitness functions to ControlInterface.
	Fitness::getFitnessManager();

	mCurrentFitnessFunctions.clear();

	if(mPopulations.empty()) {
		Core::log("LocalNetworkInSimulationEvaluationMethod: No population found", true);
		return;
	}

	if(mPopulations.size() != mControlInterfaces.size()) {
		Core::log("LocalNetworkInSimulationEvaluationMethod: "
					"Number of populations and controllers does not match!", true);
		return;
	}
	for(int i = 0; i < mPopulations.size(); ++i) {
		Population *population = mPopulations.at(i);
		ControlInterface *controller = mControlInterfaces.at(i);

		QList<FitnessFunction*> fitnessFunctions = population->getFitnessFunctions();
	
		for(QListIterator<FitnessFunction*> i(fitnessFunctions); i.hasNext();) {
			ControllerFitnessFunction *ff = dynamic_cast<ControllerFitnessFunction*>(i.next());
			if(ff != 0) {
				ff->setControlInterface(controller);
			}
		}
		mCurrentFitnessFunctions.append(fitnessFunctions);
	}
}

void LocalNetworkInSimulationEvaluationMethod::setupEvaluationPairs() {
	//TODO put this into EvaluatonGroupsBuilder subclass when the framework is refactored.

	mEvaluationPairs.clear();

	if(mPopulations.empty()) {
		return;
	}

	int maxPermutations = Math::max(1, mMaximumNumberOfPartnersPerEvaluation->get());

	QList<QList<Individual*> > allIndividuals;

	for(int i = 1; i < mPopulations.size(); ++i) {
		allIndividuals.append(mPopulations.at(i)->getIndividuals());
	}
	QList<QList<Individual*> > workingBuffer = allIndividuals;

	QList<Individual*> mainPopulation = mPopulations.at(0)->getIndividuals();

	for(int i = 0; i < mainPopulation.size(); ++i) {
		Individual *ind = mainPopulation[i];

		QList<QList<Individual*> > permutations;
		
		ind->removeProperty("Permutation");

		for(int j = 0; j < maxPermutations; ++j) {
			QList<Individual*> evalGroup;
			evalGroup.append(ind);

			for(int k = 0; k < workingBuffer.size(); ++k) {
				if(workingBuffer[k].empty()) {
					workingBuffer[k] = allIndividuals[k];
				}
				Individual *ind_k = workingBuffer[k].takeAt(Random::nextInt(workingBuffer[k].size()));
				ind_k->removeProperty("Permutation");
				evalGroup.append(ind_k);
			}	
			permutations.append(evalGroup);
		}
		mEvaluationPairs.append(permutations);
	}
}

void LocalNetworkInSimulationEvaluationMethod::setNetworksForNextTry(int currentTry) {

	if(mCurrentPermutation.empty()) {
		return;
	}

	NeuralNetworkManager *networkManager = Neuro::getNeuralNetworkManager();

	if(currentTry > 0) {
		QList<Individual*> individuals = mCurrentPermutation[(currentTry - 1) % mCurrentPermutation.size()];

		for(int k = 0; k < individuals.size(); ++k) {
			Population *pop = mPopulations[k];
			if(pop == 0) {
				continue;
			}
			NeuralNetwork *net = mCurrentNeuralNetworks[k];

			//clear ControlInterface
			mControlInterfaces[k] = net->getControlInterface(); //in case the interface was changed.
			net->setControlInterface(0);
			if(mControlInterfaces[k] != 0) {
				mControlInterfaces[k]->setController(0);
			}
			//allow changes made in phenotype to be transferred to the genome.
			pop->getGenotypePhenotypeMapper()->updateGenotype(individuals[k]);
			networkManager->removeNeuralNetwork(net);
		}
	}


	mCurrentNeuralNetworks.clear();

	QList<Individual*> individuals = mCurrentPermutation[currentTry % mCurrentPermutation.size()];

	QString permutationString = "(";
	for(int i = 0; i < individuals.size(); ++i) {
		permutationString += QString::number(individuals[i]->getId());
		if(i < (individuals.size() - 1)) {
			permutationString += ",";
		}
	}
	permutationString += ")";

	for(int ind_index = 0; ind_index < individuals.size(); ++ind_index) {
		Individual *ind = individuals[ind_index];
		ControlInterface *controller = mControlInterfaces[ind_index];
		Population *pop = mPopulations[ind_index];

		ind->setProperty("Permutations", ind->getProperty("Permutations") + permutationString);
		
		if(ind == 0 || controller == 0 || pop == 0  || pop->getGenotypePhenotypeMapper() == 0) {
			Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Could not find GenotypePhenotypeMapper or other required objects ["
				"]! [SKIPPING INDIVIDUAL]"), true);
			continue;
		}
		GenotypePhenotypeMapper *mapper = pop->getGenotypePhenotypeMapper();

		//create phenotype
		if(!mapper->createPhenotype(ind)) {
			Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Could not apply GenotypePhenotypeMapper [")
				.append(mapper->getName()).append("]! [SKIPPING INDIVIDUAL]"), true);
			continue;
		}

		//check if phenotype is a regular neural network
		NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(ind->getPhenotype());
		if(net == 0) {
			Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Phenotype of Individual was not a NeuralNetwork! ")
				.append("]! [SKIPPING INDIVIDUAL]"), true);
			continue;
		}

		//enable nonetexec
		
		//add neural network to the ControlInterface and execute the individual
		if(controller == 0) {
			Core::log("LocalNetworkInSimulationEvaluationMethod: Controller lost.", true);
			return;
		}

		controller->setController(net);
		net->setControlInterface(controller);

		mCurrentNeuralNetworks.append(net);
		networkManager->addNeuralNetwork(net);
	}
}

}


