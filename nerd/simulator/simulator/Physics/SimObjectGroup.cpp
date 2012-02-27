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



#include "SimObjectGroup.h"
#include <iostream>

using namespace std;

namespace nerd {

SimObjectGroup::SimObjectGroup(const QString &name, const QString &type) 
			: mName(name), mType(type) 
{
}

SimObjectGroup::SimObjectGroup(const SimObjectGroup &other) 
	: mName(other.mName), mType(other.mType)
{
	for(QListIterator<SimObject*> i(other.mObjectGroup); i.hasNext();) {
		SimObject *obj = i.next()->createCopy();
		mObjectGroup.append(obj);
		obj->setObjectGroup(this);
	}
}

SimObjectGroup::~SimObjectGroup() {
}


const QString& SimObjectGroup::getName() const {
	return mName;
}

void SimObjectGroup::setName(const QString &name) {
	mName = name;
}


const QString& SimObjectGroup::getType() const {
	return mType;
}


void SimObjectGroup::clearGroup() {
	mObjectGroup.clear();
}


bool SimObjectGroup::addObject(SimObject *object) {
	if(object == 0 || mObjectGroup.contains(object)) {
		return false;
	}
	mObjectGroup.append(object);
	object->setObjectGroup(this);

	//-Add child objects
	QList<SimObject*> children = object->getChildObjects();
	for(QListIterator<SimObject*> i(children); i.hasNext();) {
		SimObject *obj = i.next();
		addObject(obj);
	}
	return true;
}


bool SimObjectGroup::removeObject(SimObject *object) {
	if(object == 0 || !mObjectGroup.contains(object)) {
		return false;
	}
	mObjectGroup.removeAll(object);
	if(object->getObjectGroup() == this) {
		object->setObjectGroup(0);
	}

	//-Remove child objects
	QList<SimObject*> children = object->getChildObjects();
	for(QListIterator<SimObject*> i(children); i.hasNext();) {
		SimObject *obj = i.next();
		removeObject(obj);
	}

	return true;
}

		
QList<SimObject*> SimObjectGroup::getObjects() const {
	return mObjectGroup;
}


QList<InterfaceValue*> SimObjectGroup::getInputValues() const {
	QList<SimObject*> objects(mObjectGroup);
	QList<InterfaceValue*> values;
	
	for(QListIterator<SimObject*> i(objects); i.hasNext();) {
		values += i.next()->getInputValues();
	}
	return values;
}


QList<InterfaceValue*> SimObjectGroup::getOutputValues() const {
	QList<SimObject*> objects(mObjectGroup);
	QList<InterfaceValue*> values;
	
	for(int i = 0; i < objects.size(); ++i) {
		values += objects.at(i)->getOutputValues();
	}
	
	return values;
}


QList<InterfaceValue*> SimObjectGroup::getInfoValues() const {
	QList<SimObject*> objects(mObjectGroup);
	QList<InterfaceValue*> values;
	
	for(int i = 0; i < objects.size(); ++i) {
		values += objects.at(i)->getInfoValues();
	}
	
	return values;
}

 

}





