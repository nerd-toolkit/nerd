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

#include "Value.h"
#include "Core/Core.h"
#include "ValueChangedListener.h"
#include <iostream>

using namespace std;

#include <QMutexLocker>

namespace nerd {


Value::Value() : Object() {
	createValue("", false );
}

Value::Value(const QString &description, bool notifyAllSetAttempts) : Object() {
	createValue(description, notifyAllSetAttempts);
}

/**
 * Copy constructor.
 * This constructor does NOT copy the registered ValueListeners.
 * Therefore the new Value will have no ValueListeners attached.
 */
Value::Value(const Value &value) : Object() {
	createValue(value.mDescription, value.mNotifyAllSetAttempts);
	mDocumentation = value.mDocumentation;
	mOptionList = value.mOptionList;
}


/**
 * If there are still ValueChangedListeners registered, then
 * Value calls their forceListenerDeregistration() method.
 * This method can be overwritten by listeners to handle this event.
 */
Value::~Value() {
	if(mValueChangedListeners.size() > 0) {
		for(int i = 0; i < mValueChangedListeners.size(); ++i) {
			mValueChangedListeners.at(i)->forceListenerDeregistration(this);
		}
	}
}


/**
 * Creates a new simple Value object with the given description and standard settings.
 * Since this value does NOT encapsulate any data, it is only usefule for test purposes.
 *
 * @param description the type description of the Value, e.g. Integer.
 * @param notifyAllSetAttempty if true, then the ValueChangedListeners are notified at all calls of set(),
 *        even if the value is not changed by the method call.
 */
void Value::createValue(const QString &description, bool notifyAllSetAttempts) {
	mDescription = description;
	mNotifyCount = 0;
	mValueManager = Core::getInstance()->getValueManager();
	mMaintainNotificationStack = false;
	mNotifyAllSetAttempts = notifyAllSetAttempts;
}



/**
 * Creates a new copy of the Value.
 *
 * @return a new copy.
 */
Value* Value::createCopy() {
	return new Value(mDescription, mNotifyAllSetAttempts);
}


/**
 * Sets the notifyallSetAttempts flag of the Value. This causes the Value to notify its
 * ValueChangedListeners at any set attempt, independently of whether the conten is
 * really changed.
 *
 * @see isNotifyingAllSetAttempts
 * @param notify true to switch the flag on, false to switch it off.
 */
void Value::setNotifyAllSetAttempts(bool notify) {
	mNotifyAllSetAttempts = notify;
}


/**
 * Returns true if the Value is configured to notify its ValueChangedListeners at any set attempt
 * (set(), setValueFromString(), specific set methods, etc.), even if the call of these methods
 * does not effectively change the Value content.
 *
 * @return true if notifyAllSetAttempty flag is set.
 */
bool Value::isNotifyingAllSetAttempts() const {
	return mNotifyAllSetAttempts;
}


/**
 * Returns the type description of the Value.
 *
 * @return the description.
 */
QString Value::getDescription() const {
	return mDescription;
}


/**
 * Sets the description of the value type, like Integer, Double, etc.
 *
 * @param description the value type description.
 */
void Value::setDescription(const QString &description) {
	mDescription = description;
}

void Value::setDocumentation(const QString &documentation) {
    mDocumentation = documentation;
}


QString Value::getDocumentation() const {
    return mDocumentation;
}


QList<QString>& Value::getOptionList() {
    return mOptionList;
}


/**
 * Sets the value content from a string representation.
 * This method does nothing and has to be overwritten by subclasses
 * in a suitable way. The method should return true if the string
 * could be successfully parsed to set the content of the Value.
 *
 * This implementation always returns false.
 *
 * @param string the string representation of the desired value.
 * @return true if successful, otherwise false.
 */
bool Value::setValueFromString(const QString&) {
	return false;
}


/**
 * Returns a string representation of the value content.
 * This method returns "" and has to be overwritten by subclasses
 * in a suitable way. The method should return true if the string
 * could be successfully parsed to set the content of the Value.
 *
 * @return the value content as string.
 */
QString Value::getValueAsString() const {
	return "";
}


/**
 * Adds a ValueChangedListener that gets notified when the value changes.
 *
 * @param listener the listener to add.
 * @return true if successful, false if NULL or already contained.
 */
bool Value::addValueChangedListener(ValueChangedListener *listener) {
	QMutexLocker guard(&mMutex);
	if(listener == 0 || mValueChangedListeners.contains(listener)) {
		return false;
	}
	mValueChangedListeners.append(listener);
	return true;
}


/**
 * Removes a ValueChangedListener.
 *
 * @param listener the listener to remove.
 * @return true if successful. False if already contained or NULL.
 */
bool Value::removeValueChangedListener(ValueChangedListener *listener) {
	QMutexLocker guard(&mMutex);
	if(listener == 0 || !mValueChangedListeners.contains(listener)) {
		return false;
	}
	mValueChangedListeners.removeAll(listener);
	return true;
}


/**
 * Removes all ValueChangedListeners at once.
 */
void Value::removeValueChangedListeners() {
	QMutexLocker guard(&mMutex);
	mValueChangedListeners.clear();
}


/**
 * Returns a list with all registered ValueChangedListeners.
 *
 * @return a list with all ValueChangedListeners.
 */
const QList<ValueChangedListener*> Value::getValueChangedListeners() const {
	return mValueChangedListeners;
}


/**
 * Notifies all registered ValueChangedListeners that the Value changed.
 */
void Value::notifyValueChanged() {
	if(mNotifyCount > 0) {
		Core::log("Warning: Value was notified while notification is running!");
		return;
	}

	QMutexLocker guard(&mMutex);

	if(mNotifyCount <= 0) {
		mChangedListenerBufferVector.clear();
		mChangedListenerBufferVector = mValueChangedListeners;
		mNotifyCount = 0;
	}
	if(mMaintainNotificationStack) {
		mValueManager->pushToNotificationStack(this);
	}

	mNotifyCount++;
	if(mMaintainNotificationStack) {
		for(int i = 0; i < mChangedListenerBufferVector.size(); i++) {
			mValueManager->pushToNotificationStack(mChangedListenerBufferVector.at(i));
			mChangedListenerBufferVector.at(i)->valueChanged(this);
			mValueManager->popFromNotificationStack();
		}
	}
	else {
		for(int i = 0; i < mChangedListenerBufferVector.size(); i++) {
			mChangedListenerBufferVector.at(i)->valueChanged(this);
		}
	}
	mNotifyCount--;
	if(mMaintainNotificationStack) {
		mValueManager->popFromNotificationStack();
	}
}


/**
 * By default this method returns the same as getDescription().
 * Subclasses may overwrite this method.
 */
QString Value::getName() const {
	return mDescription;
}


/**
 * Sets the properties for this Value in a string representation.
 * Does nothing by default.
 *
 * @return true;
 */
bool Value::setProperties(const QString&) {
	return true;
}


/**
 * Returns the properties of this Value in a string representation.
 * Returns an empty string by default.
 *
 * @return the properties (default is the empty string).
 */
QString Value::getProperties() const {
	return "";
}


/**
 * Returns true if both values are the same.
 */
bool Value::equals(const Value *value) const {
	if(value == 0) {
		return false;
	}
	if(mNotifyAllSetAttempts != value->mNotifyAllSetAttempts) {
		return false;
	}
	if(mDescription != value->mDescription) {
		return false;
	}
	if(getName() != value->getName()) {
		return false;
	}
	return true;
}

}
