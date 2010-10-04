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


#include "ParameterizedObject.h"
#include <QList>
#include "Core/Core.h"
#include <QListIterator>
#include <iostream>

using namespace std;

namespace nerd {

/**
 * Constructs a ParameterizedObject with the given name.
 * The value prefix will be an empty string.
 *
 * @param name the name of the ParameterizedObject.
 */
ParameterizedObject::ParameterizedObject(const QString &name)
					: mName(name), mValuePrefix(""), mValueManager(0)
{
	mValueManager = Core::getInstance()->getValueManager();
}

/**
 * Constructs a ParameterizedObject with the given name. The value prefix
 * is set to the given prefix.
 *
 * @param name the name of the ParameterizedObject.
 * @param valuePrefix the prefix used to publsh parameters on global scope.
 */
ParameterizedObject::ParameterizedObject(const QString &name, const QString &valuePrefix)
					: mName(name), mValuePrefix(valuePrefix), mValueManager(0)
{
	mValueManager = Core::getInstance()->getValueManager();
}


/**
 * Copy constructor.
 * Parameters are copied from the other ParameterizedObject. Parameters of the other
 * ParameterizedObject, that have been published at the ValueManager, are only copied
 * locally. The copies are NOT published!
 *
 * @param object the ParameterizedObject to copy.
 */
ParameterizedObject::ParameterizedObject(const ParameterizedObject &object) 
		: Object(), ValueChangedListener(), mValueManager(0)
{
	mName = object.mName;
	mValuePrefix = object.mValuePrefix;
	mValueManager = Core::getInstance()->getValueManager();

	//Copy parameters, but do not publish any of them.
	QList<QString> parameterNames = object.mParameters.keys();
	while(!parameterNames.empty()) {
		QString name = parameterNames.front();
		parameterNames.pop_front();
		Value *value = object.mParameters.value(name);
		if(value != 0) {
			Value *copy = value->createCopy();
			addParameter(name, copy);
		}
	}
}


/**
 * Destructor.
 * Does nothing.
 */
ParameterizedObject::~ParameterizedObject()
{
	removeParameters();
}


/**
 * Adds a parameter to the ParameterizedObject. The name is a local name, that can be
 * used to retrieve the parameter later with getParameter(name). The parameter is used
 * only at local scope and is not published at the ValueManager.
 *
 * All added parameter Values are automatically observed as ValueChangedListener.
 *
 * @param name the local name of the parameter.
 * @param value the value to add as parameter.
 */
bool ParameterizedObject::addParameter(const QString &name, Value *value) {
	return addParameter(name, value, false);
}


/**
 * Adds a parameter to the ParameterizedObject. The name is a local name, that can be
 * used to retrieve the parameter later with getParameter(name). If the publish flag
 * is true, then the Value is additionally published in the global ValueManager. In this
 * case the global name is the prefix of the ParameterizedObject, followed by the local
 * name.
 *
 * All added parameter Values are automatically observed as ValueChangedListener.
 *
 * @param name the local name of the parameter.
 * @param value the value to add as parameter.
 * @param publish a flag that determines whether to publish the parameter globally.
 */
bool ParameterizedObject::addParameter(const QString &name, Value *value, bool publish) {
	if(value == 0 || mParameters.contains(name)) {
		return false;
	}
	mParameters.insert(name, value);
	value->addValueChangedListener(this);

	if(publish && mValueManager != 0) {
		mValueManager->addValue(QString("").append(mValuePrefix).append(name), value);
	}

	return true;
}


/**
 * Returns the parameter with the given local name.
 * If there is no such Value, NULL is returned.
 *
 * @param name the name of the desired parameter Value.
 * @return the parameter identified with the given name if available, otherwise NULL.
 */
Value* ParameterizedObject::getParameter(const QString &name) const {
	if(!mParameters.contains(name)) {
		return 0;
	}
	return mParameters.value(name);
}


bool ParameterizedObject::publishParameter(Value *value, bool publish) {

	if(value == 0 || mValueManager == 0) {
		return false;
	}
	
	for(QListIterator<QString> i(mParameters.keys()); i.hasNext();) {
		QString name = i.next();
		Value *value = mParameters.value(name);
		if(value == value) {

			if(publish) {
				mValueManager->addValue(QString("").append(mValuePrefix).append(name), value);
			}
			else {
				mValueManager->removeValue(QString("").append(mValuePrefix).append(name));
			}
		}
	}
	return true;	
}


/**
 * Removes all parameters and destroys them.
 * Values that have been published at the ValueManager are removed.
 */
void ParameterizedObject::removeParameters() {

	//Erase all values from the ValueManager that have eventually been published.
	QList<Value*> parameters = mParameters.values();
	mValueManager->removeValues(parameters);

	//Destroy all parameters.
	while(!parameters.empty()) {
		Value* value = parameters.front();
		parameters.pop_front();
		if(value != 0) {
			value->removeValueChangedListener(this);
			//make sure that there is no value twice registered.
			parameters.removeAll(value);
			delete value;
		}
	}
	mParameters.clear();
}


/**
 * Returns a QList with all available parameter Values.
 * @return all available parameter Values.
 */
QList<Value*> ParameterizedObject::getParameters() const {
	return mParameters.values();
}


/**
 * Returns the names of all available parameter Values.
 * These names do NOT contain the prefix and are local names.
 * @param the names of all parameters.
 */
QList<QString> ParameterizedObject::getParameterNames() const {
	return mParameters.keys();
}


/**
 * Publishes all parameters at the ValueManager. This method is useful if the 
 * ParameterizedObject is a prototype that wants to provide a copy that publishes
 * its parameters. In this case the copy can use this method to publish all parameters.
 */
void ParameterizedObject::publishAllParameters() {

	QList<QString> names = mParameters.keys();
	for(QListIterator<QString> i(names); i.hasNext();) {
		QString name = i.next();
		Value *value = mParameters.value(name);
		if(value != 0) {
			mValueManager->addValue(QString("").append(mValuePrefix).append(name), value);
		}
	}
}


/**
 * Withdraws all parameters from the ValueManager. If at least on parameter was removed,
 * the variable repository changed event is triggered. Thus unpublishing values
 * is computationally expensive and should not be done frequently.
 */
void ParameterizedObject::unpublishAllParameters() {

	mValueManager->removeValues(mParameters.values());
}


/**
 * Called when observed Values changed.
 */
void ParameterizedObject::valueChanged(Value*) {
}


/**
 * Returns the name of this ParameterizedObject.
 * @return the name.
 */
QString ParameterizedObject::getName() const {
	return mName;
}


/**
 * Sets the name of this ParameterizedObject.
 * The name will usually be the used to identify the object in the Core.
 * @param name the name of the ParameterizedObject;
 */
void ParameterizedObject::setName(const QString &name) {
	mName = name;
}




/**
 * Returns the prefix used to distinquish local values at global scope when
 * published in the ValueManager. <br>
 * Example: Having a Prefix of "/Prefix/", then calling addParameter("Param1", param, true)
 * will publish the Value param with the global name "/Prefix/Param1".
 *
 * @return the currently used prefix.
 */
QString ParameterizedObject::getPrefix() const {
	return mValuePrefix;
}


/**
 * Sets the prefix of this ParameterizedObject. The prefix is added to value names
 * published at global scope to avoid conflicts with other global parameters.
 *
 * @param prefix the new prefix to use for published values.
 */
void ParameterizedObject::setPrefix(const QString &prefix) {
	mValuePrefix = prefix;
}


bool ParameterizedObject::equals(const ParameterizedObject *obj) const {
	if(obj == 0) {
		return false;
	}
	if(obj->mName != mName) {
		return false;
	}
	if(obj->mValuePrefix != mValuePrefix) {
		return false;
	}

	QList<QString> paramNames = getParameterNames();
	QList<QString> otherParamNames = obj->getParameterNames();

	if(paramNames.size() != otherParamNames.size()) {
		return false;
	}

	for(int i = 0; i < paramNames.size(); ++i) {
		QString name = paramNames.at(i);

		if(!otherParamNames.contains(name)) {
			return false;
		}
		Value *value = getParameter(name);
		Value *otherValue = obj->getParameter(name);

		if(value == 0 || otherValue == 0 || !value->equals(otherValue)) {
			return false;
		}
	}

	return true;
}


}