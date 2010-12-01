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


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {

LocalNetworkInSimulationEvaluationMethod::LocalNetworkInSimulationEvaluationMethod()
	: EvaluationMethod("LocalNetworkInSimulationEvaluation"),
	  mControllerName(0), 
	  mEvaluationLoop(0), mIndividualStartedEvent(0), mIndividualCompletedEvent(0), 
	  mShutDownEvent(0), mSimEnvironmentChangedEvent(0), 
	  mDoShutDown(false), mCurrentIndividualValue(0),
	  mRestartIndividual(false), mRandomizeSeed(0), mDesiredSeed(0), mSimulationSeed(0),
	  mPopulation(0)
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
	
	mControllerName = new StringValue("Default");
	mCurrentIndividualValue = new IntValue(0);
	mRunStasisMode = new BoolValue(false);
	mRunStasisMode->addValueChangedListener(this);
	mRandomizeSeed = new BoolValue(true);
	mDesiredSeed = new IntValue(Random::nextInt());

	Core::getInstance()->addSystemObject(this);

	addParameter("CurrentIndividual", mCurrentIndividualValue);
	addParameter("Stasis", mRunStasisMode);
	addParameter("ControllerName", mControllerName);
	addParameter("RandomizeSeed", mRandomizeSeed);
	addParameter("DesiredSeed", mDesiredSeed);

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
	  EvaluationMethod(other), mControllerName(0), 
	  mEvaluationLoop(0), mIndividualStartedEvent(0), mIndividualCompletedEvent(0), 
	  mShutDownEvent(0), mSimEnvironmentChangedEvent(0), 
	  mDoShutDown(false), mCurrentIndividualValue(0),
	  mRestartIndividual(false), mRandomizeSeed(0), mDesiredSeed(0), mSimulationSeed(0),
	  mPopulation(0)
{
	mEvaluationLoop = other.mEvaluationLoop;
	Core::getInstance()->addSystemObject(this);

	mCurrentIndividualValue = dynamic_cast<IntValue*>(
			getParameter("CurrentIndividual"));
	mRunStasisMode = dynamic_cast<BoolValue*>(
			getParameter(EvolutionConstants::VALUE_EVO_STASIS_MODE));
	mControllerName = dynamic_cast<StringValue*>(
			getParameter("ControllerName"));
	mRandomizeSeed = dynamic_cast<BoolValue*>(
			getParameter("RandomizeSeed"));
	mDesiredSeed = dynamic_cast<IntValue*>(
			getParameter("DesiredSeed"));

	if(mRunStasisMode != 0) {
		mRunStasisMode->addValueChangedListener(this);
	}
}

LocalNetworkInSimulationEvaluationMethod::~LocalNetworkInSimulationEvaluationMethod() {
}

EvaluationMethod* LocalNetworkInSimulationEvaluationMethod::createCopy() {
	return new LocalNetworkInSimulationEvaluationMethod(*this);
}


bool LocalNetworkInSimulationEvaluationMethod::init() {
	bool ok = true;


	return ok;
}


bool LocalNetworkInSimulationEvaluationMethod::bind() {
	TRACE("LocalNetworkInSimulationEvaluationMethod::bind");

	bool ok = true;

	mShutDownEvent = Core::getInstance()->getEventManager()->registerForEvent(
			NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this);
	mSimEnvironmentChangedEvent = Core::getInstance()->getEventManager()->registerForEvent(
			EvolutionConstants::EVENT_PHYSICS_ENVIRONMENT_CHANGED, this);

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

	//TODO remove, just for test purposes
// 	mSimulationSeed = new IntValue(0);

	if(mSimulationSeed == 0) {
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

	mPopulation = mOwnerWorld->getPopulations().at(0);

	if(mPopulation == 0) {
		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Could not find a population in World [")
				.append(mOwnerWorld->getName()).append("]"));
		return false;
	}

	GenotypePhenotypeMapper *mapper = mPopulation->getGenotypePhenotypeMapper();

	if(mapper == 0) {
		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Could not find a GenotypePhenotypeMapper for population [")
				.append(mPopulation->getName()).append("]"));
		return false;
	}

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

	//execute all individuals one after the other.
	QList<Individual*> individuals = mPopulation->getIndividuals();

	NeuralNetworkManager *networkManager = Neuro::getNeuralNetworkManager();

	QList<FitnessFunction*> fitnessFunctions = mPopulation->getFitnessFunctions();

	int currentIndividual = 0;
	for(QListIterator<Individual*> i(individuals); i.hasNext();) {

		Core::getInstance()->executePendingTasks();
	
		if(mStopEvaluation || mDoShutDown || evoManager->getRestartGenerationValue()->get()) {
			break;
		}

		evoManager->setCurrentNumberOfCompletedIndividualsDuringEvaluation(currentIndividual);

		Individual *ind = i.next();

		//create phenotype
		if(!mapper->createPhenotype(ind)) {
			Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Could not apply GenotypePhenotypeMapper [")
				.append(mapper->getName()).append("]! [SKIPPING INDIVIDUAL]"));
			continue;
		}

		//check if phenotype is a regular neural network
		NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(ind->getPhenotype());
		if(net == 0) {
			Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Phenotype of Individual was not a NeuralNetwork! ")
				.append("]! [SKIPPING INDIVIDUAL]"));
			continue;
		}

		//enable nonetexec
		
		//add neural network to the ControlInterface and execute the individual
		if(mControlInterface == 0) {
			Core::log("LocalNetworkInSimulationEvaluationMethod: Controller lost.", true);
			return false;
		}

		mControlInterface->setController(net);
		net->setControlInterface(mControlInterface);

		networkManager->addNeuralNetwork(net);

		mCurrentIndividualValue->set(currentIndividual++);

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

		networkManager->removeNeuralNetwork(net);

		networkManager->triggerCurrentNetworksReplacedEvent();

		//save fitness values
		for(QListIterator<FitnessFunction*> i(fitnessFunctions); i.hasNext();) {
			FitnessFunction *ff = i.next();
			if(ff != 0) {
				ind->setFitness(ff, ff->getFitness());
			}
		}

		//clear ControlInterface
		mControlInterface = net->getControlInterface(); //for the case the interface was changed.
		net->setControlInterface(0);
		if(mControlInterface != 0) {
			mControlInterface->setController(0);
		}

		//allow changes made in phenotype to be transferred to the genome.
		mapper->updateGenotype(ind);

	
		if(mDoShutDown) {
			return true;
		}
	}
	evoManager->setCurrentNumberOfCompletedIndividualsDuringEvaluation(individuals.size());

	if(mDoShutDown) {
		return true;
	}
	
	//detach fitness functions from ControlInterface and FitnessManager.
	for(QListIterator<FitnessFunction*> i(fitnessFunctions); i.hasNext();) {
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
	mControlInterface = 0;
	if(mControllerName->get() == "Default" 
			&& !Physics::getPhysicsManager()->getSimObjectGroups().empty()) 
	{
		mControlInterface = Physics::getPhysicsManager()->getSimObjectGroups().at(0);
	}
	else {
		mControlInterface = Physics::getPhysicsManager()
								->getSimObjectGroup(mControllerName->get());
	}

	if(mControlInterface == 0) {
		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				" Could not find controller [")
				.append(mControllerName->get()).append("]! [IGNORING]"));

		return false;
	}
	return true;
}

void LocalNetworkInSimulationEvaluationMethod::setupFitnessFunctions() {
	//attach fitness functions to ControlInterface.
	Fitness::getFitnessManager();

	if(mPopulation == 0) {
		Core::log("LocalNetworkInSimulationEvaluationMethod: No population found", true);
		return;
	}

	QList<FitnessFunction*> fitnessFunctions = mPopulation->getFitnessFunctions();

	for(QListIterator<FitnessFunction*> i(fitnessFunctions); i.hasNext();) {
		ControllerFitnessFunction *ff = dynamic_cast<ControllerFitnessFunction*>(i.next());
		if(ff != 0) {
			ff->setControlInterface(mControlInterface);
		}
	}	
}

}


