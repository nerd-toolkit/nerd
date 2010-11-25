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


#include "ValueManager.h"
#include "Value.h"
#include "ValueChangedListener.h"
#include "IntValue.h"
#include "BoolValue.h"
#include <QString>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QDate>
#include <QTime>
#include <QListIterator>

#include <QMutexLocker>
#include "Value/StringValue.h"
#include "Value/DoubleValue.h"
#include "NerdConstants.h"
#include "Core/Core.h"
#include "Value/ColorValue.h"
#include "Value/InterfaceValue.h"
#include "Value/NormalizedDoubleValue.h"
#include "Value/QuaternionValue.h"
#include "Value/Vector3DValue.h"
#include "Value/Matrix3x3Value.h"
#include "Value/ULongLongValue.h"
#include "NerdConstants.h"
#include "Util/FileLocker.h"
#include <iostream>
#include <QDir>

using namespace std;

namespace nerd {



/**
 * Constructs a new ValueManager. Usually this is NOT done manually.
 * Users should use the ValueManager provided by the Core with method
 * Core::getInstance()->getValueManager().
 *
 * The constructor automatically adds prototypes for all common value types.
 * Additional prototypes can be added manually with method addPrototype().
 */
ValueManager::ValueManager(EventManager *eventManager) : mRepositoryChangedCounter(0)
{
	mRepositoryChangedEvent = eventManager
		->createEvent(NerdConstants::EVENT_VALUE_REPOSITORY_CHANGED);

	mRepositoryChangedCounter = new IntValue(0);
	addValue(NerdConstants::VALUE_NERD_REPOSITORY_CHANGED_COUNTER, 
			 mRepositoryChangedCounter);

	//add default value prototypes
	addPrototype(new IntValue());
	addPrototype(new DoubleValue());
	addPrototype(new BoolValue());
	addPrototype(new StringValue());
	addPrototype(new ColorValue());
	addPrototype(new InterfaceValue("", ""));
	addPrototype(new NormalizedDoubleValue());
	addPrototype(new QuaternionValue());
	addPrototype(new Vector3DValue());
	addPrototype(new Matrix3x3Value());
	addPrototype(new ULongLongValue());

}


/**
 * Destroys the ValueManager and hereby destroys all registered Values and Prototypes.
 */
ValueManager::~ValueManager() {
	mNotificationStack.clear();
	QList<Value*> values = mValues.values();

	//delete repository changed counter
	values.removeAll(mRepositoryChangedCounter);
	delete mRepositoryChangedCounter;

	while(!values.empty()) {
		Value* tmp = values.front();
		values.removeAll(tmp);
		delete tmp; 
	}
	while(!mPrototypes.empty()) {
		Value *tmp = mPrototypes.front();
		mPrototypes.removeAll(tmp);
		delete tmp;
	}
}


/**
 * Returns true if the given name is already used to indentify a Value.
 *
 * @param name the name to check for.
 * @return true if there is already a value registered with that name, otherwise false.
 */
bool ValueManager::nameExists(const QString &name) {
	QMap<QString, Value*>::iterator index = mValues.find(name);
	if(index == mValues.end()) {
		return false;
	}
	return true;
}


/**
 * Adds a Value to the ValueManager and associates it with the given name. 
 * If the name is already in use by another value, then false is returned and the Value
 * is not registered. 
 *
 * Changing the collection of Values in the ValueManager causes the RepositoryChangedListeners
 * to be notified. Also a specific event is triggered so that other objects can react on this change.
 *
 *
 * @param name the name to associate the value with.
 * @param value the value to add.
 * @return true if successful, otherwise false.
  */
bool ValueManager::addValue(const QString &name, Value *value) {
	if(value == 0) {
		return false;
	}

	QMap<QString, Value*>::iterator i = mValues.find(name);
	if(i != mValues.end()) {
		Core::log(QString("ValueManager::addValue: Could not add value with name [")
			.append(name).append("] du to a name conflict. The name is already in use!"));
		return false;
	}
	mValues[name] = value;
	notifyRepositoryChangedListeners();
	return true;
}


/**
 * Removes a value based on its name. If there is no value with that name then this method has no 
 * effect. If a value was found, it is removed and all RepositoryChangedListeners are notified.
 *
 * @param name the name of the Value to remove.
 * @return true if a Value was successfully removed, otherwise false.
 */
bool ValueManager::removeValue(const QString &name) {

	if(mValues.remove(name) > 0) {
		notifyRepositoryChangedListeners();
		return true;
	}
	return false;
}

/**
 * Remove all occurences of value from the ValueManager. Thus if the Value was registered with
 * different names, then all names are released. 
 *
 * @param value the value to remove.
 * @return true if successful, otherwise false.
 */
bool ValueManager::removeValue(Value *value) {
	if(value == 0) {
		return false;
	}

	QList<QString> names = mValues.keys(value);

	if(names.empty()) {
		return false;
	}
	for(int i = 0; i < names.size(); ++i) {
		removeValue(names.at(i));
	}

	return true;
}


/**
 * Removes all Values in the list from the ValueManager. 
 * Hereby all ValueChangedListeners are removed. 
 * This method should be used to finally remove Values, e.g. during shutdown.
 * This version is much more efficient than deleting Values with the other removeValue
 * methods. Calling this method will trigger the RepositoryChangedChangedListeners 
 * a single time and only if a value was really removed.
 *
 * @param values the list with the values to remove.
 * @return true.
 */
bool ValueManager::removeValues(const QList<Value*> &values) {
	bool changed = false;
	QMap<QString, Value*>::iterator i = mValues.begin();
	while(i != mValues.end()) {
		if(values.contains(i.value())) {
			if(i.value() != 0) {
				i.value()->removeValueChangedListeners();
				i = mValues.erase(i);
				changed = true;
			}
		} 
		else {
			++i;
		}
	}
	if(changed) {
		notifyRepositoryChangedListeners();
	}
	return true;
}


/**
 * Returns a Value associated with the given name. 
 * If there is no matching Value, NULL is returned.
 *
 * @param name the name of the desired Value. 
 * @return the associated Value of NULL if no such Value exists.
 */
Value* ValueManager::getValue(const QString &name) {
	QMap<QString, Value*>::iterator index = mValues.find(name);
	if(index != mValues.end()) {
		return index.value();
	}
	return 0;
}


/**
 * This method returns the MultiPartValue part corresponding to a MultiPartValue. The MultiPartValue
 * is specified by parameter 'name'. The desired value part is specified either by parameter 
 * valuePart or by leaving this parameter emptry and giving the name as part of parameter 'name'
 * in the following format: 'fullNameOfTheMultiPartValue:valuePart'.
 *
 * Note: If neither valuePart is given, nor a colon is used as separator in the value name, then 
 * this method does exactly the same as getValue(). Thus also values not beeing a subclass of
 * MulitPartValue can be found with this method. If a valuePart is specified, but there was no
 * such valuePart found in the MultiPartValue (or the value was not a subclass of MultiPartValue),
 * then 0 is returned.
 * 
 * @param name the name of the (MultiPart)Value.
 * @param valuePart the valuePart of the above specified MultiPartValue.
 * @return the MultiPartValue part specified with valuePart or the Value itself if no valuePart was given. 
 *         NULL is returned in case of failures.
 */
Value* ValueManager::getMultiPartValue(const QString &name, const QString &valuePart) {
	QString valueName = name;
	QString partName = valuePart;
	if(partName == "") {

		//if there is a value with the given name, then return it, 
		//otherwise try to split it into value and multipart strings.
		Value *value = getValue(valueName);
		if(value != 0) {
			return value;
		}

		int colonIndex = valueName.lastIndexOf(":");
		if(colonIndex > 0) {
			valueName = name.mid(0, colonIndex);
			partName = name.mid(colonIndex + 1);
		}
	}
	Value *value = getValue(valueName);
	if(value == 0) {
		return 0;
	}
	if(partName != "") {
		//get multi part value
		MultiPartValue *mpv = dynamic_cast<MultiPartValue*>(value);
		if(mpv == 0) {
			return 0;
		}
		for(int i = 0; i < mpv->getNumberOfValueParts(); ++i) {
			if(mpv->getValuePartName(i) == partName) {
				return mpv->getValuePart(i);
			}
		}
		return 0;
	}
	return value;
}


/**
 * Returns an IntValue associated with the given name.
 * If there is no such IntValue (e.g. because the name is not associated with a Value 
 * or the associated Value is not of type IntValue), then false is returend.
 +
 * @param name the name of the desired value. 
 * @return the associated IntValue if found, otherwise NULL.
 */
IntValue* ValueManager::getIntValue(const QString &name) {
	QMap<QString, Value*>::iterator index = mValues.find(name);
	if(index != mValues.end()) {
		return dynamic_cast<IntValue*>(index.value());
	}
	return 0;
}



/**
 * Returns an BoolValue associated with the given name.
 * If there is no such BoolValue (e.g. because the name is not associated with a Value 
 * or the associated Value is not of type BoolValue), then false is returend.
 +
 * @param name the name of the desired value. 
 * @return the associated BoolValue if found, otherwise NULL.
 */
BoolValue* ValueManager::getBoolValue(const QString &name) {
	QMap<QString, Value*>::iterator index = mValues.find(name);
	if(index != mValues.end()) {
		return dynamic_cast<BoolValue*>(index.value());
	}
	return 0;
}


/**
 * Returns an DoubleValue associated with the given name.
 * If there is no such DoubleValue (e.g. because the name is not associated with a Value 
 * or the associated Value is not of type DoubleValue), then false is returned.
 +
 * @param name the name of the desired value. 
 * @return the associated DoubleValue if found, otherwise NULL.
 */
DoubleValue* ValueManager::getDoubleValue(const QString &name) {
	QMap<QString, Value*>::iterator index = mValues.find(name);
	if(index != mValues.end()) {
		return dynamic_cast<DoubleValue*>(index.value());
	}
	return 0;
}


/**
 * Returns an StringValue associated with the given name.
 * If there is no such StringValue (e.g. because the name is not associated with a Value 
 * or the associated Value is not of type StringValue), then false is returned.
 +
 * @param name the name of the desired value. 
 * @return the associated StringValue if found, otherwise NULL.
 */
StringValue* ValueManager::getStringValue(const QString &name) {
	QMap<QString, Value*>::iterator index = mValues.find(name);
	if(index != mValues.end()) {
		return dynamic_cast<StringValue*>(index.value());
	}
	return 0;
}


/**
 * Returns a container with all currently known Values. 
 *
 * @return a container with all known Values.
 */
QList<Value*> ValueManager::getValues() {
	return mValues.values();
}


/**
 * Returns a list with all Value objects that are registered with names matching
 * the given pattern. The pattern may be a full name of a regular expression (like .*test.*stuff).
 *
 * @param pattern the regular expression to select the Values.
 * @return a list with all matching Values.
 */
QList<Value*> ValueManager::getValuesMatchingPattern(const QString &pattern, bool caseSensitive) {
	QList<Value*> values;
	QRegExp expr(pattern);
	if(caseSensitive) {
		expr.setCaseSensitivity(Qt::CaseSensitive);
	}
	else {
		expr.setCaseSensitivity(Qt::CaseInsensitive);
	}

	QMap<QString, Value*>::iterator index;
	for(index = mValues.begin(); index != mValues.end(); index++) {
		if(expr.exactMatch(index.key())) {
			if(!values.contains(index.value())) {
				values.append(index.value());
			}
		}
	}
	return values;
}


/**
 * Retuns a list with all value names matching the given regular expression.
 *
 * @param pattern the regular expression to describe the desired value names.
 * @param caseSensitive if true then the regular expression is interpreted case sensitive.
 * @return a list with all value names matching the regular expression.
 */
QList<QString> ValueManager::getValueNamesMatchingPattern(const QString &pattern, bool caseSensitive) {
	QList<QString> values;
	QRegExp expr(pattern);

	if(!caseSensitive) {
		expr.setCaseSensitivity(Qt::CaseInsensitive);
	}

	QMap<QString, Value*>::iterator index;
	for(index = mValues.begin(); index != mValues.end(); index++) {
		if(expr.exactMatch(index.key())) {
			values.append(index.key());
		}
	}
	return values;
}


/**
 * Returns a list with all names that are associated with the given value. As a value can be
 * registered with different names, this method can be used to determine which other names 
 * are used to reference this Value.
 *
 * @param value the value to get the names for.
 * @return a list with all names associated with the given value.
 */
QList<QString> ValueManager::getNamesOfValue(const Value *value) {
	QList<QString> values;
	QMap<QString, Value*>::iterator index;
	for(index = mValues.begin(); index != mValues.end(); index++) {
		if(index.value() == value) {
			values.append(index.key());
		}
	}
	return values;
}


/**
 * Returns all names used for associations with values. 
 * 
 * @return a list with all know value names.
 */
QList<QString> ValueManager::getValueNames() {
	QList<QString> values;
	QMap<QString, Value*>::iterator index;
	for(index = mValues.begin(); index != mValues.end(); index++) {
		values.append(index.key());
	}
	return values;
}


/**
 * Removes all Values from the ValueManager. 
 * Note: the removed Value files will not be deleted automatically. This has to be done
 * manualle in the owner classes or by calling delete on all values obtained with getValues();
 */
void ValueManager::removeAllValues() {
	QList<Value*> values = mValues.values();
	while(!values.empty()) {
		Value* tmp = values.front();
		values.removeAll(tmp);
// 		delete tmp;
	}
	mValues.clear();
	notifyRepositoryChangedListeners();
}


/**
 * Just for debugging purposes. Is obsolete and will be removed in future releases.
 */
void ValueManager::notifyRepositoryChangedListeners() {
	mRepositoryChangedCounter->set(mRepositoryChangedCounter->get() + 1);
	mRepositoryChangedEvent->trigger();
}




/**
 * Removes all EventListeners from the ReporistoryChangedEvent and all 
 * ValueChangedListeners from all Values.
 * 
 * This method is useful during shutdown to prevent notifications while the system is destroyed.
 */
void ValueManager::removeAllListeners() {
	const QList<EventListener*> &repoListeners = 
			mRepositoryChangedEvent->getEventListeners();
	for(QListIterator<EventListener*> i(repoListeners); i.hasNext();) {
		mRepositoryChangedEvent->removeEventListener(i.next());
	}

	QList<Value*> values = mValues.values();
	for(QListIterator<Value*> i(values); i.hasNext();) {
		Value *value = i.next();
		QList<ValueChangedListener*> listeners = value->getValueChangedListeners();
		for(QListIterator<ValueChangedListener*> j(listeners); j.hasNext();) {
			value->removeValueChangedListener(j.next());
		}
	}
}




/**
 * Just for debugging purposes. Is obsolete and will be removed in future releases.
 */
void ValueManager::pushToNotificationStack(Object *stackedObject) {
	mNotificationStack.push_back(stackedObject);
}

/**
 * Just for debugging purposes. Is obsolete and will be removed in future releases.
 */
void ValueManager::popFromNotificationStack() {
	mNotificationStack.pop_back();
}


/**
 * Just for debugging purposes. Is obsolete and will be removed in future releases.
 */
QVector<Object*> ValueManager::getNotificationStack() const {
	return mNotificationStack;
}



/**
 * Loads values from a value file, specified with the full fileName. 
 * If values could not be found log messages are reported to the debug log.
 * Currently the UNIX file locker is automatically used to prevent write/read conflicts.
 * On Windows this has no effect. 
 * 
 * TODO: file locking and error reporting should be simplified either through this
 *       method or by putting the load and save functionality to an own class.
 * 
 * @param fileName the name of the value file to load.
 * @return true if there was no error, otherwise false.
 */
bool ValueManager::loadValues(const QString &fileName, bool useFileLocking) {

	Core::log(QString("Loading value file ").append(fileName));

	if(mFileNameBuffer.contains(fileName)) {
		//avoid infinite loops.
		return true;
	}

	QDir dir(fileName);
	if(dir.exists()) {
		Core::log(QString("ValueManager: Tried to load a directory instead of a file: [")
					.append(fileName).append("]"));
		return false;
	}

	QFile file(fileName);

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		Core::log(QString("ValueManager: Could not load file ").append(fileName), true);
		file.close();
		return false;
	}

	if(useFileLocking) {
		if(!FileLocker::lockFileShared(file)) {
			Core::log(QString("Could not lock file %1 shared").arg(fileName));
			file.close();
			return false;
		}
	}

	mFileNameBuffer.append(fileName);

	QTextStream input(&file);
	while (!input.atEnd()) {
		QString line = input.readLine();
		line = line.trimmed();

		if(line.startsWith("#")) {
			if(line.startsWith("#include")) {
				QString additionalFileName = line.mid(9);
				cerr << "Additional File:" << additionalFileName.toStdString().c_str() << endl;
		
				//load the values from fileName first, then continue with the current file.
				//subfiles are only supported without file locking!
				loadValues(additionalFileName, false);
			}
			continue;
		}
		int sepIndex = line.indexOf("=");
		if(sepIndex == -1) {
			continue;
		}
		QString name = line.left(sepIndex);
		QString valueContent = line.right(line.length() - sepIndex - 1);

		Value *value = getValue(name);
		if(value == 0) {
			Core::log(QString("ValueManager::loadValues(): Could not find value [")
				.append(name).append("]."));
			continue;
		}
		if(!value->setValueFromString(valueContent)) {
			Core::log(QString("ValueManager::loadValues(): Could not load setting for "
				"value: [").append(name).append("]."));
		}
	}
	Core::log(QString("Loaded file ").append(fileName));

	if(useFileLocking) {
		FileLocker::unlockFile(file);
	}
	file.close();

	mFileNameBuffer.removeLast();

	return true;
}


/**
 * Saves a set of Values to a file. The Values are identified by a list of value names. 
 * The comment will be added to the file in the header so that the user can determine later
 * what kind of value file this is. 
 * If there is already a file with the given name, then the new file will overwrite the existing one.
 * Errors are logged to the debug log.
 * 
 * @param fileName the name of the file to save the values to. 
 * @param valuesToSave a list with the names of all Values that should be saved.
 * @param comment the comment contains useful information about the type of value file.
 * @return true if no error occurred, otherwise false.
 */
bool ValueManager::saveValues(const QString &fileName, 
							  const QList<QString> valuesToSave,
							  const QString &comment, 
							  bool useFileLocking)
{

	QFile file(fileName);

	int dirIndex = fileName.lastIndexOf("/");
	if(dirIndex > 0) {
		Core::getInstance()->enforceDirectoryPath(file.fileName().mid(0, dirIndex));
	}

	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		Core::log(QString("Could not open file ").append(fileName).append(" to write Values."));
		file.close();
		return false;
	}

	if(useFileLocking) {
		if(!FileLocker::lockFileExclusively(file)) {
			Core::log(QString("Could not lock file %1 exclusively").arg(fileName));
			file.close();
			return false;
		}
	}

	QString correctedComment = comment;
	correctedComment.replace("\n", "\n#");

	QTextStream output(&file);
	output << "#NERD Values" << "\n";
	output << "#Saved at: " << QDate::currentDate().toString("dd.MM.yyyy")
					<< "  " << QTime::currentTime().toString("hh:mm:ss") << "\n";
	output << "#\n#" << correctedComment << "#\n";

	for(int i = 0; i < valuesToSave.size(); ++i) {
		QString valueName = valuesToSave.at(i);
		Value *value = getValue(valueName);

		if(value == 0) {
			output << "#Not found: " << valueName << "\n";
		}
		else {
			output << valueName << "=" << value->getValueAsString() << "\n";
		}
	}

	if(useFileLocking) {
		FileLocker::unlockFile(file);
	}
	file.close();

	return true;
}


/**
 * Adds a value prototype to the ValueManager. 
 * Prototypes are required to create typed Values (like BoolValue, StringValue,...) from string 
 * representations. Therefore all types of Value should be represented by a suitable Prototype. 
 * 
 * The prototype servers as a template that is used to create a copy based on a value type name.
 * Note: Prototypes can not be removed at the moment.
 *
 * @param prototype the value prototype to add.
 * @return true if successful, false if already available or NULL.
 */
bool ValueManager::addPrototype(Value *prototype) {
	if(prototype == 0 || mPrototypes.contains(prototype)) {
		return false;
	}
	for(int i = 0; i < mPrototypes.size(); i++) {
		if(prototype->getTypeName().compare(mPrototypes.at(i)->getTypeName()) == 0) {
			return false;
		}
	}
	mPrototypes.append(prototype);
	return true;
}


/**
 * Returns a list of all available value prototypes. 
 *
 * @return a list of all prototypes.
 */
const QList<Value*>& ValueManager::getPrototypes() const {
	return mPrototypes;
}


/**
 * Creates a copy of a prototype given its type name (e.g. Integer, Boolean).
 *
 * @param typeName the type name of the desired value type.
 * @return a copy of the corresponding prototype or NULL if no such prototype could be found.
 */
Value* ValueManager::createCopyOfPrototype(const QString &typeName) const {
	for(QListIterator<Value*> i(mPrototypes); i.hasNext();) {
		Value *value = i.next();
		if(value->getTypeName() == typeName) {
			return value->createCopy();
		}
	}
	return 0;
}


/**
 * For debug use only.
 *
 * Prints all known values with their names and content (obtained with value->getValueAsString())
 * to the error stream. 
 */
void ValueManager::printValues(const QString &valueNamePattern) {
	QList<QString> valueNames = getValueNamesMatchingPattern(valueNamePattern, true);
	for(int i = 0; i < valueNames.size(); ++i) {
		cerr << "Value: " << valueNames.at(i).toStdString().c_str() 
			 << " = " << getValue(valueNames.at(i))->getValueAsString().toStdString().c_str() 
			 << endl;
	}
}


}


