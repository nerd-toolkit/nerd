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
	createModel(false);
}

void CustomModel::createModel(bool ignorePhysicsManager) {

	QString pathName = "/";
	pathName = pathName.append(getName());

	for(QListIterator<SimObject*> i(mSimObjects); i.hasNext();) {
		SimObject *object = i.next();
		object->setName(object->getName().prepend("/"));

		if(!ignorePhysicsManager) {
			QList<Value*> parameters = object->getParameters();
			for(QListIterator<Value*> j(parameters); j.hasNext();) {
				StringValue *stringValue = dynamic_cast<StringValue*>(j.next());
				if(stringValue != 0) {
					if(stringValue->get().contains("$name$")) {
						QString replacedString = stringValue->get().replace("$name$", pathName);
						stringValue->set(replacedString);
					}
				}
			}
		}
		mAgent->addObject(object);
	}

	if(!ignorePhysicsManager) {
		Physics::getPhysicsManager()->addSimObjectGroup(mAgent);
	}
	else {
		delete mAgent;
	}
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


void CustomModel::addSimObjectPrefix(const QString &prefix) {

	QList<StringValue*> stringValues;

	QHash<QString, QString> nameLookup;
	for(QListIterator<SimObject*> i(mSimObjects); i.hasNext();) {
		SimObject *object = i.next();
		QString name = object->getName();
		object->setName(prefix + name);
		Core::log("Renaming: " + name + " with " + prefix + " to " + object->getName(), true);
		nameLookup.insert(name, object->getName());

		QList<Value*> objectParameters = object->getParameters();
		for(QListIterator<Value*> j(objectParameters); j.hasNext();) {
			StringValue *sv = dynamic_cast<StringValue*>(j.next());
			if(sv != 0) {
				stringValues.append(sv);
			}
		}
	}
	//check if there are strings to change.
	for(QListIterator<StringValue*> i(stringValues); i.hasNext();) {
		StringValue *sv = i.next();
		QString value = sv->get();
		bool hasNameMacro = false;
		if(value.startsWith("$name$")) {
			hasNameMacro = true;
			value = value.mid(6);
		}
		if(nameLookup.contains(value)) {
			QString prefix = "";
			if(hasNameMacro) {
				prefix = "$name$/";
			}
			sv->set(prefix + nameLookup.value(value));
		}
	}
	//TODO change variable names
}

bool CustomModel::addVariable(CustomModelVariable *variable) {
	if(variable == 0 || mVariables.contains(variable) || mVariableLookUp.contains(variable->mName)) {
		return false;
	}
	mVariables.append(variable);
	mVariableLookUp.insert(variable->mName, variable);
	return true;
}

void CustomModel::addVariablePrefix(const QString &prefix) {
	for(QListIterator<CustomModelVariable*> i(mVariables); i.hasNext();) {
		CustomModelVariable *var = i.next();
		var->mName = var->mName.prepend(prefix + "/");
	}
}

QList<CustomModelVariable*> CustomModel::getVariables() const {
	return mVariables;
}



}



