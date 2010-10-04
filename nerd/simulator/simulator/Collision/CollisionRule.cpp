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

#include "CollisionRule.h"
#include "Core/Core.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Physics/SimBody.h"
#include "Physics/Physics.h"

namespace nerd {

/**
 * Constructor. The second parameter defines, whether this rule registers at the core as SystemObject. If so, the rule deregisters itself during cleanUp from the CollisionManager.
 * @param ruleName Name of the CollisionRule
 * @param addAsSystemObject If true, the rule registers at the core as SystemObject.
 * @param negateRule If true, the CollisionRule is negated.
 */
CollisionRule::CollisionRule(const QString &ruleName, bool negateRule) 
		: ParameterizedObject(ruleName), mNegateRule(negateRule) 
{

}

CollisionRule::CollisionRule(const CollisionRule &rule) : Object(), ValueChangedListener(), 
		ParameterizedObject(rule), mNegateRule(rule.mNegateRule)
{
	mSourceGroup = rule.getSourceGroup();
	mTargetGroup = rule.getTargetGroup();
}

CollisionRule::~CollisionRule() {
}


void CollisionRule::set(const QList<CollisionObject*> &sourceGroup, 
		const QList<CollisionObject*> &targetGroup)
{
	mSourceGroup.clear();
	mTargetGroup.clear();
	mSourceGroup = sourceGroup;
	mTargetGroup = targetGroup;
}

bool CollisionRule::addToSourceGroup(CollisionObject *collisionObject) {
	if(collisionObject == 0 || mSourceGroup.contains(collisionObject)) {
		return false;
	}
	mSourceGroup.append(collisionObject);
	return true;
}


bool CollisionRule::addToTargetGroup(CollisionObject *collisionObject) {
	if(collisionObject == 0 || mTargetGroup.contains(collisionObject)) {
		return false;
	}
	mTargetGroup.append(collisionObject);
	return true;
}


bool CollisionRule::removeFromSourceGroup(CollisionObject *collisionObject) {
	if(collisionObject == 0 || !mSourceGroup.contains(collisionObject)) {
		return false;
	}
	mSourceGroup.removeAll(collisionObject);
	return true;
}


bool CollisionRule::removeFromTargetGroup(CollisionObject *collisionObject) {
	if(collisionObject == 0 || !mTargetGroup.contains(collisionObject)) {
		return false;
	}
	mTargetGroup.removeAll(collisionObject);
	return true;
}


void CollisionRule::negateRule(bool negate)  {
	mNegateRule = negate;
}

bool CollisionRule::isNegated() const {
	return mNegateRule;
}


QList<CollisionObject*> CollisionRule::getSourceGroup() const {
	return mSourceGroup;
}

void CollisionRule::setSourceGroup(const QList<CollisionObject*> &sourceGroup){
	mSourceGroup = sourceGroup;
}

QList<CollisionObject*> CollisionRule::getTargetGroup() const {
	return mTargetGroup;
}

void CollisionRule::setTargetGroup(const QList<CollisionObject*> &targetGroup) {
	mTargetGroup = targetGroup;
}

bool CollisionRule::init() {
	bool ok = true;
	return ok;
}

bool CollisionRule::bind() {
	bool ok = true;
	return ok;
}

bool CollisionRule::cleanUp() {
	bool ok = true;
	Physics::getCollisionManager()->removeCollisionRule(this);
	return ok;
}

}