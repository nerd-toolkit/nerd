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
#include <QStringList>
#include "Physics/BoxBody.h"
#include "Physics/CapsuleBody.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new ScriptedModel.
 */
ScriptedModel::ScriptedModel(const QString &name, const QString &script)
	: ScriptingContext(name, "model"), ModelInterface(name), mPrototypeName(""), mIdCounter(1),
		mCurrentSimObject(0), mEnvironmentMode(false), mSetupEnvironmentMode(false), 
		mRandomizationMode(false), mMorphologyBase(0)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ScriptedModel object to copy.
 */
ScriptedModel::ScriptedModel(const ScriptedModel &other) 
	: ScriptingContext(other), ModelInterface(other), mPrototypeName(other.mPrototypeName), 
		mIdCounter(other.mIdCounter), mCurrentSimObject(0), mEnvironmentMode(false),
		mSetupEnvironmentMode(false), mRandomizationMode(false), mMorphologyBase(0)
{
	for(QHashIterator<StringValue*, QString> i(other.mPrototypeParameters); i.hasNext();) {
		i.next();
		StringValue *value = dynamic_cast<StringValue*>(getParameter(i.value()));
		if(value != 0) {
			mPrototypeParameters.insert(value, i.value());
		}
	}
	resetScriptContext();
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

	if(!hasModelSection()) {
		return;
	}
	//resetScriptContext();
	

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

	if(mScript == 0) {
		return;
	}

	mScript->evaluate("layoutModel.toString();");
	if(mScript->hasUncaughtException()) {
		mScript->evaluate("function layoutModel() { };");
	}
	executeScriptFunction("layoutModel();") ;
}

void ScriptedModel::randomizeObjects() {
	if(mScript == 0) {
		return;
	}

	mScript->evaluate("randomizeEnvironment.toString();");
	if(mScript->hasUncaughtException()) {
		mScript->evaluate("function randomizeEnvironment() { };");
	}
	mRandomizationMode = true;
	executeScriptFunction("randomizeEnvironment();") ;
	mRandomizationMode = false;
}

void ScriptedModel::createEnvironment() {

	if(mScript == 0) {
		return;
	}

	mCurrentSimObject = 0;
	mEnvironmentMode = true;
	executeScriptFunction("createEnvironment();");
	mEnvironmentMode = false;

	PhysicsManager *pm = Physics::getPhysicsManager();

	QList<SimObject*> objects = mEnvironmentObjectLookup.values();
	for(QListIterator<SimObject*> i(objects); i.hasNext();) {
		SimObject *obj = i.next();
		pm->addSimObject(obj);

		ModelInterface *model = dynamic_cast<ModelInterface*>(obj);
		if(model != 0) {
			model->setup();
		}
	}
	mSetupEnvironmentMode = true;
	mScript->evaluate("setupEnvironment.toString();");
	if(!mScript->hasUncaughtException()) {
		executeScriptFunction("setupEnvironment();");
	}
	mSetupEnvironmentMode = false;
}


int ScriptedModel::createObject(const QString &prototypeName, const QString &name) {
	SimObject *obj = Physics::getPhysicsManager()->getPrototype("Prototypes/" + prototypeName);
	if(obj == 0) {
		reportError("Could not find prototype [" + prototypeName + "]");
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

	//by default the new object is the current object.
	mCurrentSimObject = newObject;

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
		mCurrentSimObject = newObject;
		return id;
	}
}

bool ScriptedModel::setProperty(int objectId, const QString &propertyName, const QString &value) {
	ParameterizedObject *obj = 0;
	if(mEnvironmentMode || mRandomizationMode) {
		obj = mEnvironmentObjectLookup.value(objectId);
	}
	else if(mSetupEnvironmentMode) {
		obj = mCollisionRulesLookup.value(objectId);
	}
	else {
		obj = mSimObjectsLookup.value(objectId);
	}
	if(obj == 0) {
		reportError("Could not find object [" 
				+ QString::number(objectId) + "] to set property [" + propertyName + "]");
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
	if(mEnvironmentMode || mRandomizationMode) {
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
	if(mEnvironmentMode || mRandomizationMode) {
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
	else if(mRandomizationMode) {
		//search in both lookup tables
		obj = mEnvironmentObjectLookup.value(objectId);
		if(obj == 0) {
			obj = mSimObjectsLookup.value(objectId);
		}
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


bool ScriptedModel::addToModel(const QString modelName, int objectId) {
	SimObject *obj = 0;
	if(mEnvironmentMode) {
		obj = mEnvironmentObjectLookup.value(objectId);
	}
	else { //TODO check if this should be allowed at all?
		obj = mSimObjectsLookup.value(objectId);
	}
	if(obj == 0) {
		reportError("Could not find object [" 
				+ QString::number(objectId) + "] to add to model [" + modelName + "]");
		return false;
	}
	SimObjectGroup *group = Physics::getPhysicsManager()->getSimObjectGroup(modelName);
	if(group == 0) {
		reportError("Could not find the model with name [" + modelName + "]");
		return false;
	}
	return group->addObject(obj);
}

bool ScriptedModel::addToModel(int modelId, int objectId) {
	SimObject *obj = 0;
	SimObjectGroup *model = 0;
	if(mEnvironmentMode) {
		obj = mEnvironmentObjectLookup.value(objectId);
		ModelInterface *modelInterface = dynamic_cast<ModelInterface*>(mEnvironmentObjectLookup.value(modelId));
		if(modelInterface != 0) {
			model = modelInterface->getAgentInterface();
		}
	}
	if(obj == 0) {
		reportError("Could not find object [" 
				+ QString::number(objectId) + "] to add to model [" + QString::number(modelId) + "]");
		return false;
	}
	if(model == 0) {
		reportError("Could not find the model with id [" + QString::number(modelId) + "]");
		return false;
	}
	return model->addObject(obj);
}


int ScriptedModel::createCollisionRule(const QString &name, const QString &prototypeName) {
	if(!mSetupEnvironmentMode) {
		reportError("Called createCollisionRule outside of setupEnvironment() function!");
		return 0;
	}
	
	CollisionManager *cm = Physics::getCollisionManager();

	CollisionRule *prototype = cm->getCollisionRulePrototype(prototypeName);

	if(prototype == 0) {
		reportError("Could not find collision rule prototype [" 
				+ prototypeName + "]");
		return 0;
	}
	CollisionRule *rule = prototype->createCopy();
	rule->setName(name);
	cm->addCollisionRule(rule, false);
	int id = mIdCounter++;
	mCollisionRulesLookup.insert(id, rule);
	return id;
}


bool ScriptedModel::crAddSource(int collisionRule, int bodyId) {
	if(!mSetupEnvironmentMode) {
		reportError("Called crAddSource() outside of setupEnvironment() function!");
		return false;
	}
	CollisionRule *rule = mCollisionRulesLookup.value(collisionRule);
	if(rule == 0) {
		reportError("Could not reference collision rule with id [" 
				+ QString::number(collisionRule) + "]");
		return false;
	}
	SimBody *body = dynamic_cast<SimBody*>(mSimObjectsLookup.value(bodyId));
	if(body == 0) {
		body = dynamic_cast<SimBody*>(mEnvironmentObjectLookup.value(bodyId));
	}
	if(body == 0) {
		reportError("Could not reference body with id [" 
				+ QString::number(bodyId) + "]");
		return false;
	}
	for(int i = 0; i < body->getCollisionObjects().size(); i++) {
		rule->addToSourceGroup(body->getCollisionObjects().at(i));
	}
	return true;
}

bool ScriptedModel::crAddSource(int collisionRule, const QString &bodyRegExp) {
	if(!mSetupEnvironmentMode) {
		reportError("Called crAddSource() outside of setupEnvironment() function!");
		return false;
	}

	CollisionRule *rule = mCollisionRulesLookup.value(collisionRule);
	if(rule == 0) {
		reportError("Could not reference collision rule with id [" 
				+ QString::number(collisionRule) + "]");
		return false;
	}

	StringValue *sourceList = rule->getSourceList();
	QString listContent = sourceList->get();
	if(listContent.trimmed() != "") {
		listContent = listContent.trimmed().append(",");
	}
	listContent = listContent.append(bodyRegExp);
	sourceList->set(listContent);

// 	QString sourceName = bodyRegExp;
// 	sourceName.replace("**", ".*");
// 
// 	if(!sourceName.startsWith("/")) {
// 		sourceName.prepend("/");
// 	}
// 
// 	QList<SimObject*> matchingObjects = Physics::getPhysicsManager()
// 					->getSimObjects(sourceName);
// 	for(QListIterator<SimObject*> j(matchingObjects); j.hasNext();) {
// 		SimBody *sourceObject = dynamic_cast<SimBody*>(j.next());
// 		if(sourceObject != 0) {
// 			for(int i = 0; i < sourceObject->getCollisionObjects().size(); i++) {
// 				rule->addToSourceGroup(sourceObject->getCollisionObjects().at(i));
// 			}
// 		}
// 	}
	return true;
}


bool ScriptedModel::crAddTarget(int collisionRule, int bodyId) {
	if(!mSetupEnvironmentMode) {
		reportError("Called crAddTarget() outside of setupEnvironment() function!");
		return false;
	}

	CollisionRule *rule = mCollisionRulesLookup.value(collisionRule);
	if(rule == 0) {
		reportError("Could not reference collision rule with id [" 
				+ QString::number(collisionRule) + "]");
		return false;
	}
	SimBody *body = dynamic_cast<SimBody*>(mSimObjectsLookup.value(bodyId));
	if(body == 0) {
		body = dynamic_cast<SimBody*>(mEnvironmentObjectLookup.value(bodyId));
	}
	if(body == 0) {
		reportError("Could not reference body with id [" 
				+ QString::number(bodyId) + "]");
		return false;
	}
	for(int i = 0; i < body->getCollisionObjects().size(); i++) {
		rule->addToTargetGroup(body->getCollisionObjects().at(i));
	}
	return true;
}

bool ScriptedModel::crAddTarget(int collisionRule, const QString &bodyRegExp) {
	if(!mSetupEnvironmentMode) {
		reportError("Called crAddTarget() outside of setupEnvironment() function!");
		return false;
	}

	CollisionRule *rule = mCollisionRulesLookup.value(collisionRule);
	if(rule == 0) {
		reportError("Could not reference collision rule with id [" 
				+ QString::number(collisionRule) + "]");
		return false;
	}

	StringValue *targetList = rule->getTargetList();
	QString listContent = targetList->get();
	if(listContent.trimmed() != "") {
		listContent = listContent.trimmed().append(",");
	}
	listContent = listContent.append(bodyRegExp);
	targetList->set(listContent);

// 	QString targetName = bodyRegExp;
// 	targetName.replace("**", ".*");
// 
// 	if(!targetName.startsWith("/")) {
// 		targetName.prepend("/");
// 	}
// 
// 	QList<SimObject*> matchingObjects = Physics::getPhysicsManager()
// 					->getSimObjects(targetName);
// 	for(QListIterator<SimObject*> j(matchingObjects); j.hasNext();) {
// 		SimBody *targetObject = dynamic_cast<SimBody*>(j.next());
// 		if(targetObject != 0) {
// 			for(int i = 0; i < targetObject->getCollisionObjects().size(); i++) {
// 				rule->addToTargetGroup(targetObject->getCollisionObjects().at(i));
// 			}
// 		}
// 	}
	return true;
}

void ScriptedModel::crNegateRule(int collisionRule, bool negate) {
	if(!mSetupEnvironmentMode) {
		reportError("Called crNegateRule() outside of setupEnvironment() function!");
		return;
	}

	CollisionRule *rule = mCollisionRulesLookup.value(collisionRule);
	if(rule == 0) {
		reportError("Could not reference collision rule with id [" 
				+ QString::number(collisionRule) + "]");
		return;
	}
	rule->negateRule(negate);
}


bool ScriptedModel::crIsNegated(int collisionRule) {
	if(!mSetupEnvironmentMode) {
		reportError("Called crIsNegated() outside of setupEnvironment() function!");
		return false;
	}

	CollisionRule *rule = mCollisionRulesLookup.value(collisionRule);
	if(rule == 0) {
		reportError("Could not reference collision rule with id [" 
				+ QString::number(collisionRule) + "]");
		return false;
	}
	return rule->isNegated();
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
		reportError("Could not find required bodies ["
				+ QString::number(objectId1) + ", " + QString::number(objectId2) + "]");
		return false;
	}
	
	CollisionManager *cm = Physics::getCollisionManager();
	cm->disableCollisions(obj1->getCollisionObjects(), obj2->getCollisionObjects(), allow);
	return true;
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
		return true;
	}
	return false;
}

bool ScriptedModel::setMorphologyBase(int bodyId) {
	mMorphologyBase = dynamic_cast<SimBody*>(mSimObjectsLookup.value(bodyId));
	return (mMorphologyBase != 0);
}


bool ScriptedModel::setMorphologyProperty(int bodyId, const QString &propertyName, 
											double content) 
{
	//TODO NOT fully implemented yet!

	if(mMorphologyBase == 0) {
		return false;
	}
	SimObject *obj = dynamic_cast<SimBody*>(mSimObjectsLookup.value(bodyId));
	if(obj == 0) {
		return false;
	}
	if(dynamic_cast<BoxBody*>(obj) != 0) {
		BoxBody *body = dynamic_cast<BoxBody*>(obj);
		Vector3DValue *pos = body->getPositionValue();
		Vector3DValue *orientation = body->getOrientationValue();

		QList<QList<SimObject*> > tree = getMorphologyTree(body);

		if(propertyName == "Width") {
			DoubleValue *value = dynamic_cast<DoubleValue*>(body->getParameter("Width"));
			double diff = content - value->get();
			value->set(content);

			for(QListIterator<QList<SimObject*> > i(tree); i.hasNext();) {
				QList<SimObject*> chain = i.next();
				if(chain.contains(mMorphologyBase)) {
					continue;
				}
				QList<SimObject*> left;
				QList<SimObject*> right;
				for(QListIterator<SimObject*> k(chain); k.hasNext();) {
					SimObject *obj = k.next();
					//TODO
				}
				Physics::translateSimObjects(chain, pos->get() * -1);
				Physics::rotateSimObjects(chain, orientation->get() * -1);
				Physics::translateSimObjects(chain, Vector3D(diff, 0.0, 0.0));
				Physics::rotateSimObjects(chain, orientation->get());
				pos->set(pos->getX() + diff, pos->getY(), pos->getZ());
				Physics::translateSimObjects(chain, pos->get());
				
			}
		}
		else if(propertyName == "Height") {

		}
		else if(propertyName == "Depth") {

		}
	}
	else if(dynamic_cast<CapsuleBody*>(obj) != 0) {
		CapsuleBody *body = dynamic_cast<CapsuleBody*>(obj);
		if(propertyName == "Radius") {

		}
		else if(propertyName == "Length") {

		}
	}
}


bool ScriptedModel::rotateObjects(QScriptValue objectIds, QScriptValue origin, QScriptValue vector3DAngles) {
	
	bool ok = true;
	if(!objectIds.isArray() || !vector3DAngles.isArray() || !origin.isArray()) {
		return false;
	}
	int modArrayLength = vector3DAngles.property("length").toInteger();
	int objectArrayLength = objectIds.property("length").toInteger();
	int originLength = origin.property("length").toInteger();
	if(modArrayLength != 3 || objectArrayLength <= 0 || originLength != 3 || mScript == 0) {
		return false;
	}
	QScriptValue minusOrigin = mScript->newArray(3);
	minusOrigin.setProperty(0, origin.property(0).toNumber() * -1.0);
	minusOrigin.setProperty(1, origin.property(1).toNumber() * -1.0);
	minusOrigin.setProperty(2, origin.property(2).toNumber() * -1.0);

	rotateOrTranslateObjects(objectIds, minusOrigin, true);
	rotateOrTranslateObjects(objectIds, vector3DAngles, false);
	rotateOrTranslateObjects(objectIds, origin, true);
}


bool ScriptedModel::translateObjects(QScriptValue objectIds, QScriptValue vector3DTranslation) {
	return rotateOrTranslateObjects(objectIds, vector3DTranslation, true);
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
			reportError("definePrototypeParameter: Could not add prototpye parameter because "
					"there was already a parameter with name [" + name + "]");
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
	error = mScript->evaluate(QString("function set(name, content) {")
					  + mMainContextName + ".setP(name, content) };");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add setP function. ") 
						+ error.toString());
		return;
	}
	
}

QList<QList<SimObject*> > ScriptedModel::getMorphologyTree(SimObject *localBase) {
	QList<QList<SimObject*> > tree;

	PhysicsManager *pm = Physics::getPhysicsManager();
	
	QList<SimJoint*> joints;
	for(QListIterator<SimObject*> i(mSimObjectsLookup.values()); i.hasNext();) {
		SimJoint *joint = dynamic_cast<SimJoint*>(i.next());
		if(joint != 0) {
			joints.append(joint);
		}
	}

	QList<SimJoint*> remainingJoints = joints;
	QList<SimBody*> directNeighbors;

	for(QListIterator<SimJoint*> i(joints); i.hasNext();) {
		SimJoint *joint = i.next();
		SimBody *firstBody = pm->getSimBody(joint->getFirstBodyName()->get());
		SimBody *secondBody = pm->getSimBody(joint->getSecondBodyName()->get());

		if(firstBody == 0 && secondBody == 0) {
			remainingJoints.removeAll(joint);
			continue;
		}
		if(firstBody == localBase) {
			remainingJoints.removeAll(joint);
			directNeighbors.append(firstBody);
			continue;
		}
		else if(secondBody == localBase) {
			remainingJoints.removeAll(joint);
			directNeighbors.append(secondBody);
		}
	}
	for(QListIterator<SimBody*> i(directNeighbors); i.hasNext();) {
		QList<SimObject*> chain = getMorphologyChainRecursively(i.next(), remainingJoints);
		if(!chain.empty()) {
			tree.append(chain);
		}
	}
	return tree;
}

QList<SimObject*> ScriptedModel::getMorphologyChainRecursively(SimObject *localBase, 
							QList<SimJoint*> remainingJoints) 
{
	QList<SimObject*> chain;

	PhysicsManager *pm = Physics::getPhysicsManager();
	
	QList<SimJoint*> joints = remainingJoints;
	for(QListIterator<SimJoint*> i(joints); i.hasNext();) {
		SimJoint *joint = i.next();
		SimBody *firstBody = pm->getSimBody(joint->getFirstBodyName()->get());
		SimBody *secondBody = pm->getSimBody(joint->getSecondBodyName()->get());

		if(firstBody == 0 && secondBody == 0) {
			remainingJoints.removeAll(joint);
			continue;
		}
		if(firstBody == localBase) {
			remainingJoints.removeAll(joint);
			chain.append(getMorphologyChainRecursively(firstBody, remainingJoints));
			continue;
		}
		else if(secondBody == localBase) {
			remainingJoints.removeAll(joint);
			chain.append(getMorphologyChainRecursively(secondBody, remainingJoints));
		}
	}
	return chain;
}


bool ScriptedModel::rotateOrTranslateObjects(QScriptValue objectIds, 
					QScriptValue vector3DMod, bool translate) 
{
	if(!objectIds.isArray() || !vector3DMod.isArray()) {
		return false;
	}
	int modArrayLength = vector3DMod.property("length").toInteger();
	int objectArrayLength = objectIds.property("length").toInteger();
	if(modArrayLength != 3 || objectArrayLength <= 0) {
		return false;
	}
	Vector3D modification(vector3DMod.property(0).toNumber(), 
						vector3DMod.property(1).toNumber(), 
						vector3DMod.property(2).toNumber());
	
	QList<SimObject*> objects;

	for(int i = 0; i < objectArrayLength; ++i) {
		int id = objectIds.property(i).toInteger();
		SimObject *obj = mSimObjectsLookup.value(id);
		if(obj != 0 && !objects.contains(obj)) {
			objects.append(obj);
		}
	}
	if(translate) {
		Physics::translateSimObjects(objects, modification);
	}
	else {
		Physics::rotateSimObjects(objects, modification);
	}

	return true;
}

}



