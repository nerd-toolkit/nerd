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



#include "CollisionManager.h" 
#include "CollisionObject.h"
#include "CollisionHandler.h"
#include "CollisionRule.h"
#include "Collision/EventCollisionRule.h"
#include "Contact.h"
#include "MaterialProperties.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include <QListIterator>
#include "PlugIns/CommandLineArgument.h"
#include <iostream>

using namespace std;

namespace nerd {

CollisionManager::CollisionManager() : mCollisionHandler(0) {
	mMaterialProperties = new MaterialProperties();
	if(!addCollisionRulePrototype("EventCollisionRule",	new EventCollisionRule("EventCollisionRule"))) 
	{
		Core::log("CollisionManager: Error while adding collision rule prototype!");
	}
}

CollisionManager::~CollisionManager() {
	if(mCollisionHandler != 0) {
		delete mCollisionHandler;
	}

	while(!mCollisionRules.empty()) {
		CollisionRule *tmp = mCollisionRules.front();
		mCollisionRules.removeAll(tmp);
		delete tmp;
	}
	
	while(!mAllowCollisionPairs.empty()) {
		Contact *tmp = mAllowCollisionPairs.front();
		mAllowCollisionPairs.removeAll(tmp);
		delete tmp;
	}
	delete mMaterialProperties;	
}

QString CollisionManager::getName() const {
	return "CollisionManager";
}

bool CollisionManager::init() {
	for(int i = 0; i < mCollisionRules.size(); i++) {
		mCollisionRules.at(i)->init();
	}
	return true;
}

bool CollisionManager::bind() {

	CommandLineArgument *printCollisionRulePrototypesArg = new CommandLineArgument(
			"collisionRulePrototypes", "colp", "", 
			"Dispalys a list with all available collion rule prototypes",
			0, 0, false, false);
	CommandLineArgument *printCollisionRulesArg = new CommandLineArgument(
			"collisionRules", "col", "", 
			"Dispalys a list with all available collion rules registered until the binding phase.",
			0, 0, false, false);

	if(printCollisionRulePrototypesArg->getNumberOfEntries() > 0) {
		printCollisionRulePrototypes();
	}
	if(printCollisionRulesArg->getNumberOfEntries() > 0) {
		printCollisionRules();
	}

	return true;
}

bool CollisionManager::cleanUp() {
	return true;
}

QList<Contact> CollisionManager::getCollisions() const {
	return mCollisions;
}

/**
 * @return All CollisionObjects that are listed in a CollisionRule registered at the CollisionManager.
 */
QList<CollisionObject*> CollisionManager::getCollisionObjectsIncludedInRule() const {
	return mCollisionObjects;
}

QList<CollisionRule*> CollisionManager::getCollisionRules() const {
	return mCollisionRules;
}

CollisionRule* CollisionManager::getCollisionRule(const QString &name) const {
	for(QListIterator<CollisionRule*> i(mCollisionRules); i.hasNext();) {
		CollisionRule *rule = i.next();
		if(rule->getName() == name) {
			return rule;
		}
	}
	return 0;
}

void CollisionManager::updateCollisionModel() {
	
	if(mCollisionHandler == 0) {
		Core::log("CollisionManager: There is no CollisionHandler specified for this manager.");
		return;
	}
	for(int i = 0; i < mAllowCollisionPairs.size(); i++) {
		mCollisionHandler->disableCollisions(mAllowCollisionPairs.at(i)->getFirstCollisionObject(), 
			mAllowCollisionPairs.at(i)->getSecondCollisionObject(), true);
	}	

	mCollisionObjects.clear();
	CollisionRule *rule;
	QList<CollisionObject*> allCollisionObjects;
	for(int i = 0; i < mCollisionRules.size(); i++) {
		rule = mCollisionRules.at(i);
		allCollisionObjects += rule->getSourceGroup();
		allCollisionObjects += rule->getTargetGroup();
	}
	for(int i = 0 ; i < allCollisionObjects.size(); i++) {
		if(mCollisionObjects.indexOf(allCollisionObjects.at(i)) == -1){
			mCollisionObjects.push_back(allCollisionObjects.at(i));
		}
	}

	mCollisionHandler->updateCollisionHandler(this);
}

/**
 * Sets the physics specific CollisionHandler that will be used internally. 
 * This method has do be called before the simulation is started.
 *
 * @param handler the CollisionHandler that should be used during the simulation.
 */
void CollisionManager::setCollisionHandler(CollisionHandler *handler) {
	mCollisionHandler = handler;
}

CollisionHandler* CollisionManager::getCollisionHandler() const {
	return mCollisionHandler;
}


void CollisionManager::addCollisionRule(CollisionRule *rule, bool enforceUniqueNames) {
	for(int i = 0; i< mCollisionRules.size(); i++) {
		if(enforceUniqueNames 
			&& mCollisionRules.at(i)->getName().compare(rule->getName()) == 0) 
		{
			Core::log(QString("CollisionManager: There was a name conflict with rule [")
						.append(rule->getName()).append("] [IGNORING RULE]"));
			return;
		}
	}
	rule->setPrefix("/CollisionRules/" + rule->getName() + "/");
	rule->publishAllParameters();
	mCollisionRules.push_back(rule);
}

void CollisionManager::removeCollisionRule(CollisionRule *rule) {
	if(rule == 0) {
		return;
	}
	rule->unpublishAllParameters();
	mCollisionRules.removeAll(rule);
}

void CollisionManager::removeCollisionRule(const QString &ruleName) {
	for(int i = 0; i < mCollisionRules.size(); i++) {
		if(mCollisionRules.at(i)->getName().compare(ruleName) == 0) {
			removeCollisionRule(mCollisionRules.at(i));
			return;
		}
	}
}

/**
 * Is executed after every simulation step. All occured collisions are 
 * checked and the registered events are triggered.
 */
void CollisionManager::updateCollisionRules() {
	if(mCollisionHandler == 0) {
		Core::log("CollisionManager: There is no CollisionHandler specified for this manager.");
		return;
	}
	mCollisions = mCollisionHandler->getContacts();
	for(int j = 0; j < mCollisionRules.size(); j++) {
		for(int i = 0; i < mCollisions.size(); i++) {	
			mCollisionRules.at(j)->handleContact(mCollisions.at(i));
		}
	}
}

/**
 * Sets the collision proterty of two SimBody objects.
 * @param first the first SimBody object
 * @param second the second SimBody object
 * @param disable determines how contacts between the two objects will be treated. 
 * TRUE: The two bodies can intersect with each other. 
 * FALSE: the contact between the two bodies will be treated as a collision 
 * and is resolved by the physics engine.
 */
void CollisionManager::disableCollisions(CollisionObject *first, CollisionObject *second,
		bool disable)
{
	if(first == 0 || second == 0 || first == second) {
		return;
	}

	if(mCollisionHandler == 0) {
		Core::log("CollisionManager Warning: Could not find a CollisionHandler object!"
			" [IGNORING]");
		return;
	}
	bool contained = false;
	int index = -1;
	for(int i = 0; i < mAllowCollisionPairs.size(); i++) {
		if((mAllowCollisionPairs.at(i)->getFirstCollisionObject() == first 
			&& mAllowCollisionPairs.at(i)->getSecondCollisionObject() == second) 
			|| (mAllowCollisionPairs.at(i)->getFirstCollisionObject() == second
			&& mAllowCollisionPairs.at(i)->getSecondCollisionObject() == first))
		{
			contained = true;
			index = i;
			break;
		}
	}

	if(contained && !disable) {
		Contact *tmp = mAllowCollisionPairs.at(index);
		mAllowCollisionPairs.removeAt(index);
		delete tmp;
		mCollisionHandler->disableCollisions(first, second, disable);
	}
	if(!contained && disable) {
		mAllowCollisionPairs.push_back(new Contact(first, second));
		mCollisionHandler->disableCollisions(first, second, disable);
	}
}


/**
 * sets the collision proteries between two groups of SimBody objects.
 * @param firstGroup 
 * @param secondGroup 
 * @param disable TRUE: All contacts between bodies of the first 
 * group and bodies of the second group will not be resolved. The bodies 
 * therefore can intersect each other.
 */
void CollisionManager::disableCollisions(QList<CollisionObject*> firstGroup, 
		QList<CollisionObject*> secondGroup, bool disable) 
{
	for(int i = 0; i < firstGroup.size(); i++) {
		for(int j = 0; j < secondGroup.size(); j++) {
			this->disableCollisions(firstGroup.at(i), secondGroup.at(j), disable);
		}
	}
}

/**
 * 
 * @param collisionObject 
 * @param disable 
 */
void CollisionManager::disableCollisions(CollisionObject* collisionObject, bool disable) 
{
	collisionObject->disableCollisions(disable);
}

void CollisionManager::disableCollisions(CollisionObject* collisionObject, 
	QList<CollisionObject*> objectGroup, bool disable) 
{
	for(int i = 0; i < objectGroup.size(); i++) {
		this->disableCollisions(objectGroup.at(i), collisionObject, disable);
	}
}


MaterialProperties* CollisionManager::getMaterialProperties() const {
	return mMaterialProperties;
}

bool CollisionManager::addCollisionRulePrototype(const QString &name, CollisionRule *prototype) {
	if(prototype == 0) {
		return false;
	}
	if(mCollisionRulePrototypes.keys().contains(name)) {
		return false;
	}
	else {
		mCollisionRulePrototypes[name] = prototype;
		return true;
	}
}

CollisionRule* CollisionManager::getCollisionRulePrototype(const QString &prototypeName) {
	return mCollisionRulePrototypes.value(prototypeName);
}

QHash<QString, CollisionRule*> CollisionManager::getCollisionRulePrototypes() const {
	return mCollisionRulePrototypes;
}


void CollisionManager::printCollisionRulePrototypes() const {
	cerr << endl << "CollisionRule Prototpyes:" << endl << "----------------------------" << endl;
	for(QListIterator<QString> i(mCollisionRulePrototypes.keys()); i.hasNext();) {
		cerr << "  " << i.next().toStdString().c_str() << endl;
	}
}


void CollisionManager::printCollisionRules() const {
	cerr << endl << "Active CollisionRules:" << endl << "----------------------------" << endl;
	for(QListIterator<CollisionRule*> i(mCollisionRules); i.hasNext();) {
		CollisionRule *rule = i.next();
		if(rule != 0) {
			cerr << "  " << rule->getName().toStdString().c_str() << endl;
		}
	}
}

}
