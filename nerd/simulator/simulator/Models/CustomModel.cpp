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



#include "CustomModel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QListIterator>
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new CustomModel.
 */
CustomModel::CustomModel(const QString &name)
	: ModelInterface("Prototypes/" + name)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the CustomModel object to copy.
 */
CustomModel::CustomModel(const CustomModel &other) 
	: ModelInterface(other)
{
	mSimObjects.clear();
	for(QListIterator<SimObject*> i(other.mSimObjects); i.hasNext();) {
		mSimObjects.append(i.next()->createCopy());
	}
}


/**
 * Destructor.
 */
CustomModel::~CustomModel() {
}


SimObject* CustomModel::createCopy() const {
	return new CustomModel(*this);
}


void CustomModel::createModel() {

	mAgent = new SimObjectGroup(getName(), "Agent");
	QString pathName = "/";
	pathName = pathName.append(getName());

	for(QListIterator<SimObject*> i(mSimObjects); i.hasNext();) {
		SimObject *object = i.next();
		object->setName(object->getName().prepend("/"));

		QList<Value*> parameters = object->getParameters();
		for(QListIterator<Value*> j(parameters); j.hasNext();) {
			StringValue *stringValue = dynamic_cast<StringValue*>(j.next());
			if(stringValue != 0) {
				Core::log("Found: " + stringValue->get() + "  ", true);
				if(stringValue->get().contains("$name$")) {
					QString replacedString = stringValue->get().replace("$name$", pathName);
					Core::log("Replaced: " + stringValue->get() + " " +  replacedString, true);
					stringValue->set(replacedString);
				}
			}
		}

		mAgent->addObject(object);
	}

	Physics::getPhysicsManager()->addSimObjectGroup(mAgent);
}

bool CustomModel::addSimObject(SimObject *object) {
	if(object == 0 || mSimObjects.contains(object)) {
		return false;
	}
	for(QListIterator<SimObject*> i(mSimObjects); i.hasNext();) {
		if(i.next()->getName() == object->getName()) {
			return false;
		}
	}
	mSimObjects.append(object);
	return true;
}

QList<SimObject*> CustomModel::getSimObjects() const {
	return mSimObjects;
}


void CustomModel::layoutObjects() {
	ModelInterface::layoutObjects();
}


bool CustomModel::addVariable(CustomModelVariable *variable) {
	if(variable == 0 || mVariables.contains(variable) || mVariableLookUp.contains(variable->mName)) {
		return false;
	}
	mVariables.append(variable);
	mVariableLookUp.insert(variable->mName, variable);
	return true;
}


QList<CustomModelVariable*> CustomModel::getVariables() const {
	return mVariables;
}



}



