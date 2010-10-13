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



#include "ScriptedModel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Physics/SimObject.h"
#include "Collision/CollisionManager.h"
#include "Physics/SimObjectGroup.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ScriptedModel.
 */
ScriptedModel::ScriptedModel(const QString &name, const QString &script)
	: ScriptingContext(name, "model"), ModelInterface(name), mPrototypeName(""), mIdCounter(1), mAgent(0),
		mCurrentSimObject(0), mEnvironmentMode(false)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ScriptedModel object to copy.
 */
ScriptedModel::ScriptedModel(const ScriptedModel &other) 
	: ScriptingContext(other), ModelInterface(other), mPrototypeName(other.mPrototypeName), 
		mIdCounter(other.mIdCounter), mAgent(0), mCurrentSimObject(0), mEnvironmentMode(false)
{
	for(QHashIterator<StringValue*, QString> i(other.mPrototypeParameters); i.hasNext();) {
		i.next();
		StringValue *value = dynamic_cast<StringValue*>(getParameter(i.value()));
		if(value != 0) {
			mPrototypeParameters.insert(value, i.value());
		}
	}
}

/**
 * Destructor.
 */
ScriptedModel::~ScriptedModel() {
}

QString ScriptedModel::getName() const {
	return ModelInterface::getName();
}

void ScriptedModel::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	ModelInterface::valueChanged(value);
	ScriptingContext::valueChanged(value);
}

void ScriptedModel::eventOccured(Event *event) {
	ModelInterface::eventOccured(event);
	ScriptingContext::eventOccured(event);
}

void ScriptedModel::loadScriptFromFile(const QString &fileName) {
	mScriptFileName->set(fileName);
}

SimObject* ScriptedModel::createCopy() const {
	return new ScriptedModel(*this);
}

void ScriptedModel::createModel() {
	mIdCounter = 1;
	resetScriptContext();
	mAgent = new SimObjectGroup(getName(), "Agent");
	mCurrentSimObject = 0;

	executeScriptFunction("createModel();");
	
	QList<SimObject*> objects = mSimObjectsLookup.values();
	for(QListIterator<SimObject*> i(objects); i.hasNext();) {
		SimObject *obj = i.next();
		mAgent->addObject(obj);
		mSimObjects.append(obj);
	}
	Physics::getPhysicsManager()->addSimObjectGroup(mAgent);
}

void ScriptedModel::layoutObjects() {
	mCurrentSimObject = 0;

	executeScriptFunction("layoutModel();") ;
}

void ScriptedModel::createEnvironment() {
	mCurrentSimObject = 0;
	mEnvironmentMode = true;
	executeScriptFunction("createEnvironment();");
	mEnvironmentMode = false;

	PhysicsManager *pm = Physics::getPhysicsManager();

	QList<SimObject*> objects = mEnvironmentObjectLookup.values();
	for(QListIterator<SimObject*> i(objects); i.hasNext();) {
		SimObject *obj = i.next();
		ModelInterface *model = dynamic_cast<ModelInterface*>(obj);
		if(model != 0) {
			model->setup();
		}
		pm->addSimObject(i.next());
	}
}


int ScriptedModel::createObject(const QString &prototypeName, const QString &name) {
	SimObject *obj = Physics::getPhysicsManager()->getPrototype("Prototypes/" + prototypeName);
	if(obj == 0) {
		return 0;
	}
	SimObject *newObject = obj->createCopy();
	int id = mIdCounter++;
	if(mEnvironmentMode) {
		mEnvironmentObjectLookup.insert(id, newObject);
	}
	else {
		mSimObjectsLookup.insert(id, newObject);
	}
	newObject->setName(name);
	return id;
}

int ScriptedModel::copyObject(int objectId, const QString &name) {
	SimObject *obj = 0;
	if(mEnvironmentMode) {
		obj = mEnvironmentObjectLookup.value(objectId);
	}
	else {
		obj = mSimObjectsLookup.value(objectId);
	}
	if(obj == 0) {
		return 0;
	}
	else {
		SimObject *newObject = obj->createCopy();
		int id = mIdCounter++;
		if(mEnvironmentMode) {
			mEnvironmentObjectLookup.insert(id, newObject);
		}
		else {
			mSimObjectsLookup.insert(id, newObject);
		}
		newObject->setName(name);
		return id;
	}
}

bool ScriptedModel::setProperty(int objectId, const QString &propertyName, const QString &value) {
	SimObject *obj = 0;
	if(mEnvironmentMode) {
		obj = mEnvironmentObjectLookup.value(objectId);
	}
	else {
		obj = mSimObjectsLookup.value(objectId);
	}
	if(obj == 0) {
		return false;
	}
	Value *prop = obj->getParameter(propertyName);
	if(prop == 0) {
		return false;
	}
	return prop->setValueFromString(value);
}

bool ScriptedModel::hasProperty(int objectId, const QString &propertyName) {
	SimObject *obj = 0;
	if(mEnvironmentMode) {
		obj = mEnvironmentObjectLookup.value(objectId);
	}
	else {
		obj = mSimObjectsLookup.value(objectId);
	}
	if(obj == 0) {
		return false;
	}
	Value *prop = obj->getParameter(propertyName);
	if(prop == 0) {
		return false;
	}
	return true;
}

QString ScriptedModel::getProperty(int objectId, const QString &propertyName) {
	SimObject *obj = 0;
	if(mEnvironmentMode) {
		obj = mEnvironmentObjectLookup.value(objectId);
	}
	else {
		obj = mSimObjectsLookup.value(objectId);
	}
	if(obj == 0) {
		return "";
	}
	Value *prop = obj->getParameter(propertyName);
	if(prop == 0) {
		return "";
	}
	return prop->getValueAsString();
}


bool ScriptedModel::makeCurrent(int objectId) {
	SimObject *obj = 0;
	if(mEnvironmentMode) {
		obj = mEnvironmentObjectLookup.value(objectId);
	}
	else {
		obj = mSimObjectsLookup.value(objectId);
	}
	if(obj == 0) {
		mCurrentSimObject = 0;
		return false;
	}
	mCurrentSimObject = obj;
	return true;
}

int ScriptedModel::getCurrent() {
	if(mCurrentSimObject == 0) {
		return 0;
	}
	if(mEnvironmentMode) {
		for(QHashIterator<int, SimObject*> i(mEnvironmentObjectLookup); i.hasNext();) {
			i.next();
			if(i.value() == mCurrentSimObject) {
				return i.key();
			}
		}
	}
	else {
		for(QHashIterator<int, SimObject*> i(mSimObjectsLookup); i.hasNext();) {
			i.next();
			if(i.value() == mCurrentSimObject) {
				return i.key();
			}
		}
	}
	return 0;
}

bool ScriptedModel::setP(const QString &propertyName, const QString &value) {
	if(mCurrentSimObject == 0) {
		return false;
	}
	Value *prop = mCurrentSimObject->getParameter(propertyName);
	if(prop == 0) {
		return false;
	}
	return prop->setValueFromString(value);
}


bool ScriptedModel::allowCollisions(int objectId1, int objectId2, bool allow) {

	SimBody *obj1 = 0;
	SimBody *obj2 = 0;

	if(mEnvironmentMode) {
		obj1 = dynamic_cast<SimBody*>(mEnvironmentObjectLookup.value(objectId1));
		obj2 = dynamic_cast<SimBody*>(mEnvironmentObjectLookup.value(objectId2));
	}
	else {
		obj1 = dynamic_cast<SimBody*>(mSimObjectsLookup.value(objectId1));
		obj2 = dynamic_cast<SimBody*>(mSimObjectsLookup.value(objectId2));
	}
	

	if(obj1 == 0 || obj2 == 0) {
		Core::log("ScriptedModel::allowCollisions: Could not find required bodies ["
				+ QString::number(objectId1) + ", " + QString::number(objectId2) + "]");
		return false;
	}
	
	CollisionManager *cm = Physics::getCollisionManager();
	cm->disableCollisions(obj1->getCollisionObjects(), obj2->getCollisionObjects(), allow);
	return true;
}


QString ScriptedModel::toVector3DString(double x, double y, double z) {
	return QString("(") + QString::number(x) + "," + QString::number(y) + "," + QString::number(z) + ")";
}


QString ScriptedModel::toColorString(double r, double g, double b, double t) {
	return QString("(") + QString::number(r) + "," + QString::number(g) 
			+ "," + QString::number(b) + "," + QString::number(t) + ")";
}

bool ScriptedModel::hasEnvironmentSection() {
	if(mScript != 0) {
		mScript->evaluate("createEnvironment.toString();");
		if(mScript->hasUncaughtException()) {
			return false;
		}
		return true;
	}
	return false;
}


bool ScriptedModel::hasModelSection() {
	if(mScript != 0) {
		mScript->evaluate("createModel.toString();");
		if(mScript->hasUncaughtException()) {
			return false;
		}
		mScript->evaluate("layoutModel.toString();");
		if(mScript->hasUncaughtException()) {
			return false;
		}
		return true;
	}
	return false;
}


/**
 * The prototype name can only be set once!.
 * Copies of a prototype can not overwrite the prototype name inhererited from their parents.
 */
void ScriptedModel::setPrototypeName(const QString &name) {
	if(mPrototypeName == "") {
		mPrototypeName = "Prototypes/" + name;
		setName(mPrototypeName);
	}
}

QString ScriptedModel::getPrototypeName() const {
	return mPrototypeName;
}

void ScriptedModel::setModelName(const QString &name) {
	//do not allow changes here.
	//the model name must not be changed by the script.
}

QString ScriptedModel::getModelName() const {
	return getName();
}

void ScriptedModel::definePrototypeParameter(const QString &name, const QString &initialValue) {	
	if(!mPersistentParameters.contains(name) && !mPrototypeParameters.values().contains(name)) {

		if(getParameter(name) != 0) {
			Core::log(QString("ScriptedModel::definePrototypeParameter: Could not add prototpye parameter because ")
					+ "there was already a parameter with name [" + name + "]");
			return;
		}
		StringValue *param = new StringValue(initialValue);
		//param->addValueChangedListener(this);
		mPrototypeParameters.insert(param, name);
		addParameter(name, param);
	}

}

void ScriptedModel::reportError(const QString &message) {
	Core::log(QString("ScriptedModel [") + getName() + "]: " + message, true);
}

void ScriptedModel::importVariables() {
	ScriptingContext::importVariables();

	//update persistent parameters
	for(QHashIterator<StringValue*, QString> i(mPrototypeParameters); i.hasNext();) {
		i.next();
		mScript->evaluate(i.value() + " = \"" + i.key()->get() + "\";");
	}
}

void ScriptedModel::addCustomScriptContextStructures() {
	ScriptingContext::addCustomScriptContextStructures();

	QScriptValue error = mScript->evaluate(QString("function makeCurrent(obj) {")
					  + mMainContextName + ".makeCurrent(obj) };");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add makeCurrent function. ") 
						+ error.toString());
		return;
	}

	error = mScript->evaluate(QString("function setP(name, content) {")
					  + mMainContextName + ".setP(name, content) };");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add setP function. ") 
						+ error.toString());
		return;
	}

	
}

}


