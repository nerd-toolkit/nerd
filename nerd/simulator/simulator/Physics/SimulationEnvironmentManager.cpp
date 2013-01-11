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


#include "SimulationEnvironmentManager.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Collision/CollisionManager.h"
#include "Physics/SimObject.h"
#include "Value/Value.h"
#include "Physics/Physics.h"
#include "NerdConstants.h"
#include "Math/Random.h"
#include <iostream>
#include <Math/Math.h>

using namespace std;

namespace nerd {

/**
 * Constructor
 */
SimulationEnvironmentManager::SimulationEnvironmentManager() : mResetSettingsEvent(0), 
		mCurrentTry(0), mCurrentSimulationSeed(0) 
{
	ValueManager *vManager = Core::getInstance()->getValueManager();
	
	mCurrentSimulationSeed = new IntValue(Random::nextInt());
	vManager->addValue(SimulationConstants::VALUE_RANDOMIZATION_SIMULATION_SEED,
		mCurrentSimulationSeed);
	
	mRandomizeSeedAtReset = new BoolValue(false);
	vManager->addValue(SimulationConstants::VALUE_RANDOMIZE_SEED_AT_RESET, mRandomizeSeedAtReset);

	mRandomizeEnvironmentEvent = Core::getInstance()->getEventManager()
			->createEvent(SimulationConstants::EVENT_RANDOMIZE_ENVIRONMENT);
}

/**
 * Destructor
 */
SimulationEnvironmentManager::~SimulationEnvironmentManager() {
	while(!mSimulationRandomizer.empty()) {
		Randomizer *rand = mSimulationRandomizer.front();
		mSimulationRandomizer.removeAll(rand);
		delete rand;
	}
}

/**
 * 
 * @return 
 */
bool SimulationEnvironmentManager::init() {
	

	 return true;
}

/**
 * 
 * @return 
 */
bool SimulationEnvironmentManager::bind() {
	bool bindOk = true;
	EventManager *eManager = Core::getInstance()->getEventManager();
	mResetSettingsEvent = 
		eManager->registerForEvent(NerdConstants::EVENT_EXECUTION_RESET_SETTINGS, this);
	if(mResetSettingsEvent == 0) {
		bindOk = false;
		Core::log(QString("SimulationEnvironmentManager: Required Event [")
			.append(NerdConstants::EVENT_EXECUTION_RESET_SETTINGS)
			.append("] could not be found."));
	}
	ValueManager *vManager = Core::getInstance()->getValueManager();
	mCurrentTry = vManager->getIntValue(SimulationConstants::VALUE_EXECUTION_CURRENT_TRY);
	if(mCurrentTry == 0) {
		Core::log("SimulationEnvironmentManager: Could not find required Value. "
			"Manager will work without tries.");
	}

	return bindOk;
}

/**
 * 
 * @return 
 */
bool SimulationEnvironmentManager::cleanUp() {
	return Core::getInstance()->getEventManager()
			->deregisterFromEvent(NerdConstants::EVENT_EXECUTION_RESET_SETTINGS, this);
}

/**
 * 
 * @param event 
 */
void SimulationEnvironmentManager::eventOccured(Event *event) {
	if(event == mResetSettingsEvent) {
		if(mRandomizeSeedAtReset->get()) {
			mCurrentSimulationSeed->set(Random::nextInt());
			performRandomization();
		}
		resetStartConditions();	
	}
}


/**
 * Implementation of Object::getName(). Returns a name for SimulationEnvironmentManager 
 * objects.
 * @return the object-class name
 */
QString SimulationEnvironmentManager::getName() const {
	return "SimulationEnvironmentManager";
}

/**
 * Adds a parameter value of a SystemObject to the list of stored settings. Values of 
 * already saved parameter values will be replaced by the new value automatically.
 * @param object the SimObjet to which the parameter to be stored belongs
 * @param paramValue the name of the parameter to be stored as a QString.
 * @param valueAsString the value of the parameter to be stored as a QString.
 */
void SimulationEnvironmentManager::storeParameter(SimObject *object,const QString &paramValueName, 
												  const QString &valueAsString)
{
	Value *objectParameter = object->getParameter(paramValueName);	
	if(objectParameter == 0) {
		Core::log(QString("SimulationEnvironmentManager: The SimObject \"")
			.append(object->getName()).append("\" has no parameter named: \"")
			.append(paramValueName).append("\""));
		return;
	}
	if(mParameterSettings.find(object) == mParameterSettings.end()) {
		QHash<Value*, QString> objectSettingsHash;
		objectSettingsHash[objectParameter] = valueAsString;
		mParameterSettings[object] = objectSettingsHash;
	}
	else {
		QHash<Value*, QString> savedSettings = mParameterSettings.value(object);
		savedSettings[objectParameter] = valueAsString;
		mParameterSettings[object] = savedSettings;
	}
}

void SimulationEnvironmentManager::clearParameter(SimObject *object, const QString &paramValueName) {
	QHash<Value*, QString> objectHash;
	Value* valuetoRemove = object->getParameter(paramValueName);
	if(mParameterSettings.find(object) != mParameterSettings.end() 
		&& valuetoRemove != 0) 
	{
		objectHash = mParameterSettings.value(object);
		objectHash.remove(valuetoRemove);
		mParameterSettings[object] = objectHash;
	}
}

void SimulationEnvironmentManager::clearParameters(SimObject *object) {
	mParameterSettings.remove(object);
}

void SimulationEnvironmentManager::clearAllParameters() {
	mParameterSettings.clear();
}

/**
 * Changes all registered settings of the simulation to the saved values.
 * Is called after the event "/Control/ResetSimulationSettings" was triggered. 
 */
void SimulationEnvironmentManager::resetStartConditions() {

	for(int i = 0; i < mParameterSettings.size(); i++) {
		QHash<Value*, QString> parameters = mParameterSettings.value(
			mParameterSettings.keys().at(i));
		QHash<Value*, QString> buffer = parameters;

		//TODO test this carefully!

		for(int j = 0; j < parameters.size(); j++) {
			Value *value = parameters.keys().at(j);
			if(dynamic_cast<QuaternionValue*>(value) != 0) {
				//restore quaternions first to prevent orientation values to change...
				QString parameterValue = parameters.value(value);
				value->setValueFromString(parameterValue);
				buffer.remove(value);
			}
		}

		//restore all remaining values.
		for(int j = 0; j < buffer.size(); j++) {
			Value *value = buffer.keys().at(j);
			QString parameterValue = buffer.value(value);
			value->setValueFromString(parameterValue);
		}
	}
	performRandomization();
}


/**
 * Collects and stores all parameters of the SimObject objects that are currently 
 * registered at the PhysicsManager.
 */
void SimulationEnvironmentManager::createSnapshot() {

	QList<SimObject*> simObjects = Physics::getPhysicsManager()->getSimObjects();

	SimObject *current = 0;
	for(int i = 0; i < simObjects.size(); i++) {
		current = simObjects.at(i);
		for(int j = 0; j < current->getParameterNames().size(); j++) {
			QString paramName = current->getParameterNames().at(j);
			storeParameter(current, paramName, current->getParameter(paramName)->getValueAsString()); 
		}
	}	
}

bool SimulationEnvironmentManager::addRandomizer(Randomizer *randomizer) {
	if(randomizer != 0 && !mSimulationRandomizer.contains(randomizer)) {
		mSimulationRandomizer.push_back(randomizer);
		return true;
	}
	return false;
}

bool SimulationEnvironmentManager::removeRandomizer(Randomizer *randomizer) {
	if(randomizer != 0 && mSimulationRandomizer.contains(randomizer)) {
		int index = mSimulationRandomizer.indexOf(randomizer);
		mSimulationRandomizer.removeAt(index);
		return true;
	}
	return false;
}


QHash<SimObject*, QHash<Value*, QString> > SimulationEnvironmentManager::getSnapshot() const {
	return mParameterSettings;
}


void SimulationEnvironmentManager::setSnapshot(QHash<SimObject*, QHash<Value*, QString> > snapshot) 
{
	mParameterSettings = snapshot;
}


void SimulationEnvironmentManager::performRandomization() {

	if(mCurrentSimulationSeed != 0) {
	
		Random::setSeed(mCurrentSimulationSeed->get());
		int randomizationSeed = 0;
		if(mCurrentTry != 0) {
			randomizationSeed = Random::nextInt();	
			for(int i = 0; i < mCurrentTry->get(); i++) {
				randomizationSeed = Random::nextInt();	
			}
		}
		else {
			randomizationSeed = Random::nextInt();	
		}
		if(mCurrentTry != 0) {
		}
		Random::setSeed(randomizationSeed);
		for(int i = 0; i < mSimulationRandomizer.size(); i++) {
			mSimulationRandomizer.at(i)->applyRandomization();
		}
		mRandomizeEnvironmentEvent->trigger();
		Random::init();
		Core::getInstance()->executePendingTasks();
	}
}

}


