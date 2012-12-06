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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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

#include "PhysicsManager.h"
#include "PhysicalSimulationAlgorithm.h"
#include "Collision/CollisionManager.h"
#include "Core/Core.h"
#include "Value/InterfaceValue.h"
#include "Value/ValueManager.h"
#include <QTime>
#include "Physics/Physics.h"
#include "SimulationConstants.h"
#include "NerdConstants.h"
#include <QListIterator>
#include <iostream>
#include <QMutexLocker>
#include <QRegExp>
#include "Util/Tracer.h"

#include "Physics/SimBody.h"
#include "Physics/SimActuator.h"
#include "Physics/SimSensor.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);
#include <PlugIns/CommandLineArgument.h>
#include <Command/Command.h>


using namespace std;


namespace nerd {


/**
 * Standard constructor.
 */
PhysicsManager::PhysicsManager() : mPhysicalSimulationAlgorithm(0), mCollisionManager(0),
		mSynchronizeObjectsWithPhysicalModel(true), mNextStepEvent(0),
		mCompletedNextStepEvent(0),
		mResetEvent(0), mResetFinalizedEvent(0), mResetSettingsEvent(0), 
		mResetSettingsTerminatedEvent(0), mInitialResetDone(false), mCurrentSimulationTime(0),
		mCurrentRealTime(0),  mCurrentStep(0), mResetDuration(0), mStepExecutionDuration(0),
		mPhysicalStepDuration(0), mSynchronizationDuration(0), mPostStepDuration(0),
		mCollisionHandlingDuration(0), mDisablePhysics(0), mResetMutex(QMutex::Recursive)
{
	EventManager *em = Core::getInstance()->getEventManager();

	//make sure these events are created right at the beginning (required for morphology evolution)
	em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_SETTINGS, true);
	em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_SETTINGS_COMPLETED, true);
	mPhysicsEnvironmentChangedEvent = em->createEvent(
					SimulationConstants::EVENT_PHYSICS_ENVIRONMENT_CHANGED);
	
	CommandLineArgument *switchXZArg = new CommandLineArgument("switchYZAxis", "switchYZAxis", "",
						"The y-axis now means height, so that the position of a robot in the plane is (x,z).",
						0, 0, true, false);
	bool switchXZ = switchXZArg->getNumberOfEntries() > 0 ? false : true;
	
	mSwitchYZAxes = new BoolValue(switchXZ);
	mSwitchYZAxes->setDescription("Changes the XYZ space to a XZY space, so that X and Y are in the plain.");
	Core::getInstance()->getValueManager()->addValue(SimulationConstants::VALUE_SWITCH_YZ_AXES, mSwitchYZAxes);
	
	mDisablePhysics = new BoolValue(false);
	mDisablePhysics->setDescription("If true, then the physical simulation is skipped and no update of the physics takes place.");
	Core::getInstance()->getValueManager()->addValue(SimulationConstants::VALUE_DISABLE_PHYSICS, mDisablePhysics);
}


/**
 * Destructor.
 */
PhysicsManager::~PhysicsManager()
{
	delete mPhysicalSimulationAlgorithm;
	destroySimObjects();
	destroyPrototypes();
	destroySimObjectGroups();

	//clear Physics helper class.
	Physics::reset();
}


/**
 * Initializes the PhysicsManager.
 * @return false if no collision manager could be found.
 */
bool PhysicsManager::init() {

	bool ok = true;

	mCollisionManager = Physics::getCollisionManager();
	if(mCollisionManager == 0) {
		Core::log("PhysicsManager: Could not find a CollisionManager.");
		ok = false;
	}

	mCurrentSimulationTime = new DoubleValue(0.0);
	mCurrentRealTime = new DoubleValue(0.0);

	mResetDuration = new IntValue(0);
	mStepExecutionDuration = new IntValue(0);
	mPhysicalStepDuration = new IntValue(0);
	mSynchronizationDuration = new IntValue(0);
	mPostStepDuration = new IntValue(0);
	mCollisionHandlingDuration = new IntValue(0);

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/Simulation/SimulationTime", mCurrentSimulationTime);
	vm->addValue("/Simulation/RealTime", mCurrentRealTime);
	vm->addValue("/ExecutionTime/Physics/Reset", mResetDuration);
	vm->addValue("/ExecutionTime/Physics/EntireStep", mStepExecutionDuration);
	vm->addValue("/ExecutionTime/Physics/Physics", mPhysicalStepDuration);
	vm->addValue("/ExecutionTime/Physics/Synchronization", mSynchronizationDuration);
	vm->addValue("/ExecutionTime/Physics/PostStep", mPostStepDuration);
	vm->addValue("/ExecutionTime/Physics/CollisionHandling", mCollisionHandlingDuration);

	EventManager *em = Core::getInstance()->getEventManager();

	mResetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET, true);
	mResetFinalizedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED, this);
	mResetSettingsEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_SETTINGS, true);
	mResetSettingsTerminatedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_SETTINGS_COMPLETED, true);

	return ok;
}


/**
 * Executed during the binding phase.
 * Returns false if one of the following Events have not been found:
 * - /Control/NextStep
 * - /Control/NextStepCompleted
 * - /Control/ResetSimulation
 * - /Control/ResetSimulationFinalized
 * - /Control/ResetSimulationSettings
 *
 * @return true if successful, otherwise false.
 */
bool PhysicsManager::bind() {

	EventManager *em = Core::getInstance()->getEventManager();
	mNextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP, true);
	mCompletedNextStepEvent = em->getEvent(
		NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, true);
	
	mCurrentStep = Core::getInstance()->getValueManager()->getIntValue(SimulationConstants::VALUE_EXECUTION_CURRENT_STEP);

	if(mNextStepEvent == 0
		|| mCompletedNextStepEvent == 0
		|| mResetEvent == 0
		|| mResetSettingsEvent == 0
		|| mResetSettingsTerminatedEvent == 0
		|| mResetFinalizedEvent == 0)
	{
		Core::log("PhysicsManager: Required Events could not be found.");
		return false;
	}
	else {
		mResetEvent->addEventListener(this);
		mNextStepEvent->addEventListener(this);
	}
	return true;
}


/**
 * Called during the shutdown phase.
 */
bool PhysicsManager::cleanUp() {

	ValueManager *vm = Core::getInstance()->getValueManager();

	//remove all parameter values from the valuemanager.
	for(QListIterator<SimObject*> i(mSimObjects); i.hasNext();) {
		SimObject *obj = i.next();
		obj->clear();

		//TODO speed up! Iterate over hashtable instead of name and getParameter.
		vm->removeValues(obj->getParameters());
		
	}
	return true;
}


/**
 * Resets the simulation to the initial conditions.
 * This especially involves the PhysicalSimulationAlgorithm and updating
 * the CollisionManager.
 *
 * @return true if successful.
 */
bool PhysicsManager::resetSimulation() {
	TRACE("PhysicsManager::resetSimulation");

	bool measurePerformance = Core::getInstance()->isPerformanceMeasuringEnabled();

	QTime time;

	if(measurePerformance) {
		time.start();
	}

	bool resetOk = true;
	if(mPhysicalSimulationAlgorithm == 0) {
		Core::log("PhyiscsManager: PhysicalSimulationAlgorithm is NULL.");
		return false;
	}

	QMutexLocker resetMutexLocker(&mResetMutex);

	clearPhysics();

	mResetSettingsEvent->trigger();
	mResetSettingsTerminatedEvent->trigger();

	mPhysicalSimulationAlgorithm->resetPhysics();
	for(int i = 0; i < mSimObjects.size(); i++) {
		mSimObjects.at(i)->setup();
	}

	if(mPhysicalSimulationAlgorithm->finalizeSetup() == false) {
		Core::log("PhysicsManager: Problems finalizing setup in physical"
			" simulation algorithm.");
		resetOk = false;
	}

	Physics::getCollisionManager()->updateCollisionModel();

	if(resetOk) {
		mInitialResetDone = true;
	}

	if(measurePerformance) {
		mResetDuration->set(time.elapsed());
		mCurrentRealTime->set(0.0);
	}
	mCurrentSimulationTime->set(0.0);

	mStopwatch.start();

	return resetOk;
}

bool PhysicsManager::executeSimulationStep() {

	if(mDisablePhysics->get()) {
		//still update the simulation time!
		if(mCurrentStep != 0) {
			//calculate the simulation time according to the current step.
			mCurrentSimulationTime->set(mCurrentStep->get()
					* mPhysicalSimulationAlgorithm->getTimeStepSize()
					* mPhysicalSimulationAlgorithm->getIterationsPerStep());
		}
		
		return true;
	}
	
	QTime stepTime;
	QTime time;

	bool measurePerformance = Core::getInstance()->isPerformanceMeasuringEnabled();

	if(measurePerformance) {
		time.start();
		stepTime.start();
	}

	bool executeStepOk = true;
	if(mPhysicalSimulationAlgorithm != 0) {
		mPhysicalSimulationAlgorithm->executeSimulationStep(this);

		if(measurePerformance) {
			mPhysicalStepDuration->set(time.restart());
		}

		//synchronize all SimObjects with the current state of the physical model.
		if(mSynchronizeObjectsWithPhysicalModel) {
			for(QList<SimObject*>::iterator i = mSimObjects.begin(); 
				i != mSimObjects.end(); i++) 
			{
				(*i)->synchronizeWithPhysicalModel(mPhysicalSimulationAlgorithm);
			}
		}

		if(measurePerformance) {
			mSynchronizationDuration->set(time.restart());
		}

		if(mCollisionManager !=  0 ) {
			mCollisionManager->updateCollisionRules();
		}

		if(measurePerformance) {
			mCollisionHandlingDuration->set(time.restart());
		}
		if(mCurrentStep != 0) {
			//calculate the simulation time according to the current step.
			mCurrentSimulationTime->set(mCurrentStep->get()
					* mPhysicalSimulationAlgorithm->getTimeStepSize()
					* mPhysicalSimulationAlgorithm->getIterationsPerStep());
		}
		else {
			mCurrentSimulationTime->set(mCurrentSimulationTime->get()
					+ mPhysicalSimulationAlgorithm->getTimeStepSize()
					* mPhysicalSimulationAlgorithm->getIterationsPerStep());
		}
	}
	else {
		Core::log("PhysicsManager: Can not execute simulation step due to missing algorithm.");
		executeStepOk = false;
		mCurrentSimulationTime->set(0.0);
	}
	if(measurePerformance) {
		mPostStepDuration->set(time.restart());
		mStepExecutionDuration->set(stepTime.elapsed());
	}

	mCurrentRealTime->set(((double) mStopwatch.elapsed()) / 1000.0);

	return executeStepOk;
}

void PhysicsManager::clearPhysics() {
	//lock mutex
	QMutexLocker resetMutexLocker(&mResetMutex);

	for(QListIterator<SimObject*> i(mSimObjects); i.hasNext();) {
		i.next()->clear();
	}
}


QList<SimBody*> PhysicsManager::getSimBodies() const {
	return mBodyObjects;
}


/**
 * Returns the SimBody whose absoluteName() equals the given name.
 * Note that absolute names always start with a /. If name does not 
 * start with a /, then one is added automatically.
 *
 * @param name the name of the object
 * @return the object with the given absoluteName(). NULL if not found.
 */
SimBody* PhysicsManager::getSimBody(const QString &name) {
	QString objectName = name;
	if(!objectName.startsWith("/")) {
		objectName.prepend("/");
	}
	for(int i = 0 ; i < mBodyObjects.size(); i++) {
		if(mBodyObjects.at(i)->getAbsoluteName().compare(objectName) == 0) {
			return mBodyObjects.at(i);
		}
	}
	return 0;
}


QList<SimJoint*> PhysicsManager::getSimJoints() const {
	return mJointObjects;
}


/**
 * Returns the SimJoint whose absoluteName() equals the given name.
 * Note that absolute names always start with a /. If name does not 
 * start with a /, then one is added automatically.
 *
 * @param name the name of the object
 * @return the object with the given absoluteName(). NULL if not found.
 */
SimJoint* PhysicsManager::getSimJoint(const QString &name) {
	QString objectName = name;
	if(!objectName.startsWith("/")) {
		objectName.prepend("/");
	}
	for(int i = 0 ; i < mJointObjects.size(); i++) {
		if(mJointObjects.at(i)->getAbsoluteName().compare(objectName) == 0) {
			return mJointObjects.at(i);
		}
	}
	return 0;
}


/**
 * Adds a SimObject to the PhysicsManager. All parameters of the SimObject
 * are registered at the ValueManager, their names starting with /sim/.
 * 
 * Child object of the SimObject are automatically added to the PhysicsManager.
 */
bool PhysicsManager::addSimObject(SimObject *object) {
	if(object == 0 || mSimObjects.contains(object)) {
		return false;
	}
	mSimObjects.append(object);
	SimBody *body = dynamic_cast<SimBody*>(object);
	if(body != 0) {
		mBodyObjects.append(body);
	}
	SimJoint *joint = dynamic_cast<SimJoint*>(object);
	if(joint != 0) {
		mJointObjects.append(joint);
	}
	SimSensor *sensor = dynamic_cast<SimSensor*>(object);
	if(sensor != 0) {
		mSensorObjects.append(sensor);
	}
	SimActuator *actuator = dynamic_cast<SimActuator*>(object);
	if(actuator != 0) {
		mActuatorObjects.append(actuator);
	}

	//Add the SimObject parameter to the ValueManager.
	ValueManager *vm = Core::getInstance()->getValueManager();
	QList<QString> parameterNames = object->getParameterNames();
	QString objectName = QString("").append(object->getAbsoluteName()).append("/");
	for(int j = 0; j < parameterNames.size(); j++) {
		QString paramName = parameterNames.at(j);
		Value *param = object->getParameter(paramName);
		if(param == 0) {
			continue;
		}
		paramName = paramName.prepend(objectName);
		if(vm->getValue(paramName) == 0) {
			QString fullName = QString("/Sim").append(paramName);
			vm->addValue(fullName, param);
		}
	}

	//add child objects
	QList<SimObject*> children = object->getChildObjects();
	for(QListIterator<SimObject*> i(children); i.hasNext();) {
		addSimObject(i.next());
	}
	return true;
}


/**
 * Removes a single SimObject from the PhysicsManager.
 * Hereby the object parameters are also removed from the ValueManager.
 * The object is removed from all SimObjectGroups to keep the system valid. If it is not 
 * desired to remove the object from a SimObjectGroup, this group has to be removed from 
 * the PhysicsManager before this method is called for its member objects.
 * Child objects of object are NOT removed. This has to be done manually.
 */
bool PhysicsManager::removeSimObject(SimObject *object) {
	if(object == 0 || !mSimObjects.contains(object)) {
		return false;
	}
	mSimObjects.removeAll(object);
	SimBody *body = dynamic_cast<SimBody*>(object);
	if(body != 0) {
		mBodyObjects.removeAll(body);
	}
	SimJoint *joint = dynamic_cast<SimJoint*>(object);
	if(joint != 0) {
		mJointObjects.removeAll(joint);
	}
	SimSensor *sensor = dynamic_cast<SimSensor*>(object);
	if(sensor != 0) {
		mSensorObjects.removeAll(sensor);
	}
	SimActuator *actuator = dynamic_cast<SimActuator*>(object);
	if(actuator != 0) {
		mActuatorObjects.removeAll(actuator);
	}

	//remove parameters of object.
	ValueManager *vm = Core::getInstance()->getValueManager();
	QList<Value*> parameters = object->getParameters();
	for(QListIterator<Value*> i(parameters); i.hasNext();) {
		Value *value = i.next();
		vm->removeValue(value);
	}

	//remove obj from all SimObjectGroups.
	for(QListIterator<SimObjectGroup*> j(mSimObjectGroups); j.hasNext();) {
		SimObjectGroup *group = j.next();
		group->removeObject(object);
	}
	return true;
}


QList<SimObject*> PhysicsManager::getSimObjects() const {
	return mSimObjects;
}


/**
 * Returns the SimObjects whose absoluteName() equals the given name.
 * Note that absolute names always start with a /. If name does not 
 * start with a /, then one is added automatically.
 *
 * @param name the name of the object
 * @return the object with the given absoluteName(). NULL if not found.
 */
SimObject* PhysicsManager::getSimObject(const QString &name) {
	QString objectName = name;
	if(!objectName.startsWith("/")) {
		objectName.prepend("/");
	}
	for(int i = 0 ; i < mSimObjects.size(); i++) {
		if(mSimObjects.at(i)->getAbsoluteName().compare(objectName) == 0) {
			return mSimObjects.at(i);
		}
	}
	return 0;
}


/**
 * Returns all objects matching the regular expression namePattern.
 * The regular expression is case sensitive.
 *
 * @param namePattern the regular expression to match object names.
 */
QList<SimObject*> PhysicsManager::getSimObjects(const QString &namePattern) {
	QList<SimObject*> objects;
	QRegExp expr(namePattern);

	for(QListIterator<SimObject*> i(mSimObjects); i.hasNext();) {
		SimObject *simObject = i.next();
		if(expr.exactMatch(simObject->getAbsoluteName())) {
			objects.append(simObject);
		}
	}

	return objects;
}


bool PhysicsManager::addSimObjectGroup(SimObjectGroup *group) {
	if(group == 0 || mSimObjectGroups.contains(group)) {
		return false;
	}
	mSimObjectGroups.append(group);

	QList<SimObject*> objects = group->getObjects();
	for(QListIterator<SimObject*> i(objects); i.hasNext();) {
		addSimObject(i.next());
	}
	return true;
}


bool PhysicsManager::removeSimObjectGroup(SimObjectGroup *group) {
	if(group == 0 || !mSimObjectGroups.contains(group)) {
		return false;
	}
	mSimObjectGroups.removeAll(group);
	return true;
}


QList<SimObjectGroup*> PhysicsManager::getSimObjectGroups() const {
	return mSimObjectGroups;
}

SimObjectGroup* PhysicsManager::getSimObjectGroup(const QString &name) const {
	for(QListIterator<SimObjectGroup*> i(mSimObjectGroups); i.hasNext();) {
		SimObjectGroup *group = i.next();
		if(group->getName() == name) {
			return group;
		}
	}
	return 0;
}


void PhysicsManager::destroySimObjectGroups() {
	while(!mSimObjectGroups.empty()) {
		SimObjectGroup *group = mSimObjectGroups.front();
		mSimObjectGroups.removeAll(group);
		delete group;
	}
	mSimObjectGroups.clear();
}



/**
 * Sets the physical simulation algorithm used for the simulation.
 *
 * @param algorithm the physical simulation algorithm to use.
 */
void PhysicsManager::setPhysicalSimulationAlgorithm(PhysicalSimulationAlgorithm *algorithm) {
	mPhysicalSimulationAlgorithm = algorithm;
}


/**
 * Returns the currently used physical simulation algorithm.
 *
 * @return the current simulation algorithm.
 */
PhysicalSimulationAlgorithm* PhysicsManager::getPhysicalSimulationAlgorithm() const {
	return mPhysicalSimulationAlgorithm;
}



bool PhysicsManager::addPrototype(const QString &name, SimObject *prototype) {
	if(prototype == 0) {
		return false;
	}
	if(mPrototypes.keys().contains(name)) {
		Core::log(QString("PhysicsManager: There was a name conflict with prototype [")
			.append(name).append("] [IGNORING PROTOTYPE]"));
		return false;
	}
	mPrototypes.insert(name, prototype);
	return true;
}

SimObject* PhysicsManager::getPrototype(const QString &name) const {
	return mPrototypes.value(name);
}


QList<QString> PhysicsManager::getPrototypeNames() const {
	return mPrototypes.keys();
}

QList<SimObject*> PhysicsManager::getPrototypes() const {
	return mPrototypes.values();
}


SimBody* PhysicsManager::getBodyPrototype(const QString &name) const {
	return dynamic_cast<SimBody*>(mPrototypes.value(name));
}


SimJoint* PhysicsManager::getJointPrototype(const QString &name) const {
	return dynamic_cast<SimJoint*>(mPrototypes.value(name));
}


/**
 * Clears all prototypes of all types. PhysicsManager also destroys the prototype
 * objects, so make sure that they are not destroyed a second time somewhere else.
 */
void PhysicsManager::destroyPrototypes() {
	QList<SimObject*> prototypes = mPrototypes.values();
	mPrototypes.clear();

	while(!prototypes.empty()) {
		SimObject *obj = prototypes.front();
		prototypes.removeAll(obj);
		delete obj;
	}
}

/**
 * Clears and destroys all SimObjects except of the prototypes.
 */
void PhysicsManager::destroySimObjects() {

	while(!mSimObjects.empty()) {
		SimObject *object = mSimObjects.front();
		mSimObjects.removeAll(object);
		delete object;
	}
	mSimObjects.clear();
	mBodyObjects.clear();
	mJointObjects.clear();

}


void PhysicsManager::eventOccured(Event *e) {
	if(e == 0) {
		return;
	}
	else if(Core::getInstance()->isShuttingDown()) {
		return;
	}
	else if(e == mNextStepEvent) {
		
		if(mInitialResetDone == false) {
			//this should ensure that reset was executed at least once before a
			//step is executed.
			//TODO: This might not be sufficient or save. Check!
			resetSimulation();
		}

		executeSimulationStep();
	}
	else if(e == mResetEvent) {
		resetSimulation();
	}
}


QString PhysicsManager::getName() const {
	return "PhysicsManager";
}


QList<ControlInterface*> PhysicsManager::getControllerInterfaces() const {
	QList<ControlInterface*> controller;
	for(int i = 0; i < mSimObjectGroups.size(); i++) {
		controller.push_back(mSimObjectGroups.at(i));
	}
	return controller;
}

ControlInterface* PhysicsManager::getControllerInterface(const QString &controllerName) const {
	return getSimObjectGroup(controllerName);
}

void PhysicsManager::updateSensors() {
	for(int i = 0; i < mSensorObjects.size(); i++) {
		mSensorObjects.at(i)->updateSensorValues();
	}
}

void PhysicsManager::updateActuators() {
	for(int i = 0; i < mActuatorObjects.size(); i++) {
		mActuatorObjects.at(i)->updateActuators();
	}
}

QMutex* PhysicsManager::getResetMutex() {
	return &mResetMutex;
}

Event* PhysicsManager::getResetEvent() const {
	return mResetEvent;
}


Event* PhysicsManager::getPhysicsEnvironmentChangedEvent() const {
	return mPhysicsEnvironmentChangedEvent;
}


/**
 * Note: For performance reasons this event is NOT triggered automatically when a SimObject or 
 * SimObject group was added or removed. This event has to be triggered manually when the modifications
 * on the environment are completed. Otherwise too many event calls would be the result.
 *
 * Never forget to call this method after removing a SimObject or SimObjectGroup. Otherwise the 
 * system may be in an invalid state and pointers to removed SimObjects may still be in use.
 */
void PhysicsManager::triggerPhysicsEnvironmentChangedEvent() {
	mPhysicsEnvironmentChangedEvent->trigger();
}


}
