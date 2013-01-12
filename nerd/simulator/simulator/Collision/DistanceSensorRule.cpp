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

#include "Collision/CollisionManager.h"
#include "Physics/Physics.h"
#include "Physics/RayGeom.h"
#include "DistanceSensorRule.h"
#include "Core/Core.h"
#include <iostream>

using namespace std;

namespace nerd {

DistanceSensorRule::DistanceSensorRule(const QString &name) : CollisionRule(name)
{
	Physics::getCollisionManager()->addCollisionRule(this, false);
}

//TODO: implement a copy constructor.
DistanceSensorRule::DistanceSensorRule(const DistanceSensorRule &rule)
		: Object(), ValueChangedListener(), CollisionRule(rule) 
{
	Core::log("DistanceSensorRule: DistanceSensorRule copy constructor called but is not implemented yet.");
}


DistanceSensorRule::~DistanceSensorRule() {
	QList<QList<Vector3D>*> lists = mHash.values();
	mHash.clear();
	while(!lists.empty()) {
		QList<Vector3D> *list = lists.front();
		lists.removeAll(list);
		delete list;
	}
	
}

CollisionRule* DistanceSensorRule::createCopy() const {
	return new DistanceSensorRule(*this);
}

bool DistanceSensorRule::addToSourceGroup(CollisionObject *obj) {
	if(CollisionRule::addToSourceGroup(obj)) {
		mHash.insert(obj, new QList<Vector3D>);
		return true;
	}
	return false;
}

bool DistanceSensorRule::removeFromSourceGroup(CollisionObject *obj) {
	if(CollisionRule::removeFromSourceGroup(obj)) {
		QList<Vector3D> *list = mHash.value(obj);
		mHash.remove(obj);
		if(list != 0) {
			delete list;
		}
		return true;
	}
	return false;
}

bool DistanceSensorRule::handleContact(const Contact &contact) {

	if(mSourceGroup.contains(contact.getFirstCollisionObject())
			&& mTargetGroup.contains(contact.getSecondCollisionObject())) 
	{
		(*mHash.value(contact.getFirstCollisionObject())) << contact.getContactPoints();
		return true;
	}
	else if(mSourceGroup.contains(contact.getSecondCollisionObject())
			&& mTargetGroup.contains(contact.getFirstCollisionObject())) 
	{
		(*mHash.value(contact.getSecondCollisionObject())) << contact.getContactPoints();
		return true;
	}
	return false;
}

QList<Vector3D>* DistanceSensorRule::getCollisionPoints(CollisionObject *obj) const {
	return mHash.value(obj);
}

}

