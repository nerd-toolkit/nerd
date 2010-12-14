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



#include "ScriptingContext.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QScriptEngine>
#include <QStringList>
#include <QHashIterator>
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "Value/BoolValue.h"
#include "Value/StringValue.h"
#include <QFile>
#include <QTextStream>
#include "Value/MultiPartValue.h"
#include "Util/Tracer.h"
#include <QDir>
#include <QTextStream>
#include "Math/Random.h"
#include <iostream>

using namespace std;

namespace nerd {


/**
 * Constructs a new ScriptingContext.
 */
ScriptingContext::ScriptingContext(const QString &name, const QString &mainContextName)
	: mName(name), mScript(0), mScriptCode(0), mScriptFileName(0), mMainContextName(mainContextName),
	  mHasUnresolvedValueDefinitions(false), mHasUnresolvedEventDefinitions(false),
	  mMaxNumberOfTriesToResolveDefinitions(5)
{
	mScriptCode = new StringValue();
	mScriptCode->addValueChangedListener(this);

	mScriptFileName = new StringValue("");
	mScriptFileName->addValueChangedListener(this);
	mScriptFileName->setNotifyAllSetAttempts(true);
	mScriptFileName->useAsFileName(true);
}


/**
 * Copy constructor. 
 * 
 * @param other the ScriptingContext object to copy.
 */
ScriptingContext::ScriptingContext(const ScriptingContext &other) 
	: QObject(), mName(other.mName), mScript(0), mScriptCode(0), mScriptFileName(0), 
	  mMainContextName(other.mMainContextName),
	  mHasUnresolvedValueDefinitions(false), mHasUnresolvedEventDefinitions(false),
	  mMaxNumberOfTriesToResolveDefinitions(other.mMaxNumberOfTriesToResolveDefinitions)
{
	mScriptCode = new StringValue(other.mScriptCode->get());
	mScriptCode->addValueChangedListener(this);

	mScriptFileName = new StringValue(other.mScriptFileName->get());
	mScriptFileName->addValueChangedListener(this);
	mScriptFileName->setNotifyAllSetAttempts(true);
	mScriptFileName->useAsFileName(true);

	//copy persitent variables
	for(QHashIterator<QString, QString> i(other.mPersistentParameters); i.hasNext();) {
		i.next();
		mPersistentParameters.insert(i.key(), i.value());
	}
}

/**
 * Destructor.
 */
ScriptingContext::~ScriptingContext() {
	if(mScript != 0) {
		delete mScript;
	}
}

QString ScriptingContext::getName() const {
	return mName;
}

void ScriptingContext::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mScriptCode) {
		resetScriptContext();
	}
	else if(value == mScriptFileName) {
		if(mScriptFileName->get().trimmed() != "") {
			loadScriptCode(true);
			resetScriptContext();
		}
	}
}

void ScriptingContext::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else {
		for(QHash<Event*, QString>::iterator i = mEventNames.begin(); i != mEventNames.end(); i++) {
			Event *e = i.key();
			if(event == e) {
				mEventOccurences[i.value()] = true;
			}
		}
	}
}

/**
 * The scripting context can only be executed from the mainExecutionThread!
 */
void ScriptingContext::resetScriptContext() {
	if(!Core::getInstance()->isMainExecutionThread()) {
		return;
	}
	mMaxNumberOfTriesToResolveDefinitions = 5;
	mHasUnresolvedValueDefinitions = false;
	mHasUnresolvedEventDefinitions = false;

	//remove all known events
	for(QHash<Event*, QString>::iterator i = mEventNames.begin(); i != mEventNames.end(); i++) {
		Event *e = i.key();
		if(e != 0) {
			e->removeEventListener(this);
		}
	}
	mEventNames.clear();
	mEventOccurences.clear();

	if(mScript != 0) {
		delete mScript;
		mScript = 0;
	}

	mScript = new QScriptEngine();

	//make global objects available
	QScriptValue nerdScriptObj = mScript->newQObject(this, 
							QScriptEngine::QtOwnership,
			 				//QScriptEngine::ExcludeSuperClassMethods |
							//QScriptEngine::ExcludeSuperClassProperties |
							QScriptEngine::ExcludeChildObjects);
	mScript->globalObject().setProperty(mMainContextName, nerdScriptObj);
	mScript->globalObject().setProperty("orcs", nerdScriptObj); //for backwards compatibility with ORCS 2.5

	//add function wrappers
	QScriptValue error = mScript->evaluate(QString("function defVar(varName, valueName) {")
					  + "  if(arguments.length != 2"
					  + "    || (typeof arguments[0] != 'string')"
					  + "    || (typeof arguments[1] != 'string')) "
					  + "  { "
					  + "    print('defVar: Invalid use of parameters!');"
					  + "    throw TypeError('defVar: requires two string parameters!');"
					  + "  }"
					  + "  " + mMainContextName + ".defineVariable(varName, valueName);"
					  + "}");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add defVar function. ") 
						+ error.toString());
		return;
	}

	error = mScript->evaluate(QString("function defVarR(varName, valueName) {")
					  + "  if(arguments.length != 2"
					  + "    || (typeof arguments[0] != 'string')"
					  + "    || (typeof arguments[1] != 'string')) "
					  + "  { "
					  + "    print('defVarR: Invalid use of parameters!');"
					  + "    throw TypeError('defVarR: requires two string parameters!');"
					  + "  }"
					  + "  " + mMainContextName + ".defineReadOnlyVariable(varName, valueName);"
					  + "}");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add defVarR function. ") 
						+ error.toString());
		return;
	}

	error = mScript->evaluate(QString("function defStatic(varName, initValue) {")
					  + "  if(arguments.length != 2"
					  + "    || (typeof arguments[0] != 'string')"
					  + "    || (typeof arguments[1] != 'string')) "
					  + "  { "
					  + "    print('defStatic: Invalid use of parameters!');"
					  + "    throw TypeError('defStatic: requires two string parameters!');"
					  + "  }"
					  + "  " + mMainContextName + ".definePersistentParameter(varName, initValue);"
					  + "}");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add defVar function. ") 
						+ error.toString());
		return;
	}

	error = mScript->evaluate(QString("function defEvent(varName, eventName) {")
					  + "  if(arguments.length != 2"
					  + "    || (typeof arguments[0] != 'string')"
					  + "    || (typeof arguments[1] != 'string')) "
					  + "  { "
					  + "    print('defEvent: Invalid use of parameters!');"
					  + "    throw TypeError('defEvent: requires two string parameters!');"
					  + "  }"
					  + "  " + mMainContextName + ".defineEvent(varName, eventName); "
					  + "}");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add defEvent function. ") 
						+ error.toString());
		return;
	}

	//allow subclasses to add custom structures to the scripting context.
	addCustomScriptContextStructures();

	QString programCode = getScriptCode();
	if(!mScript->canEvaluate(programCode)) {
		Core::log(QString("~ScriptingContext [") + getName() + 
					"]: Script contains syntax errors.");
		reportError("Syntax error detected in script!");
		return;
	}

	//evaluate basic programCode to make functions and global variables useable.
	error = mScript->evaluate(programCode);

	if(mScript->hasUncaughtException()) {
		QStringList backtrace = mScript->uncaughtExceptionBacktrace();
		QString errorMessage = QString("~ScriptingContext [") + getName() + 
					"]: Error in line " + 
					QString::number(mScript->uncaughtExceptionLineNumber()) +
					":\n";
		errorMessage += error.toString() + "\n";

		for(QListIterator<QString> i(backtrace); i.hasNext();) {
			errorMessage += QString("\n").append(i.next());
		}
		reportError(errorMessage);

		delete mScript;
		mScript = 0;

		return;
	}
	else {
		//define Value variables
		for(QHash<QString, Value*>::iterator i = mReadVariables.begin(); 
			i != mReadVariables.end(); i++) 
		{
			Value *v = i.value();
			if(dynamic_cast<MultiPartValue*>(v) != 0) {
				MultiPartValue *mpv = dynamic_cast<MultiPartValue*>(v);

				QString defCode = QString("function _value_") + i.key() + "() { ";
				for(int j = 0; j < mpv->getNumberOfValueParts(); ++j) {
					defCode += "this." + mpv->getValuePartName(j) + " = 0; ";
				}
				defCode += "}; ";
				defCode += "var " + i.key() + " = new _value_" + i.key() + "();";
				error = mScript->evaluate(defCode);
			}
			else {
				error = mScript->evaluate(QString("var ") + i.key() + " = 0;");
			}
			if(mScript->hasUncaughtException()) {
				reportError(QString("Could not add variable with name [") 
						+ i.key() + "]: " + error.toString());
				//ignore error and go on.
			}
		}
	}
}

/**
 * The scripting context can only be executed from the mainExecutionThread!
 */
void ScriptingContext::executeScriptFunction(const QString &functionName) {
	if(mScript == 0 || !Core::getInstance()->isMainExecutionThread()) {
		return;
	}
	if((mMaxNumberOfTriesToResolveDefinitions > 0) && (mHasUnresolvedValueDefinitions || mHasUnresolvedEventDefinitions)) {
		reloadEventAndValueDefinitions();
	}

	importVariables();
	QScriptValue error = mScript->evaluate(functionName);

	if(mScript->hasUncaughtException()) {
		reportError(QString("There was an error executing [" + functionName + "] ") 
						+ error.toString());
	}
	exportVariables();
}

void ScriptingContext::reloadEventAndValueDefinitions() {

	mMaxNumberOfTriesToResolveDefinitions--;

	mHasUnresolvedEventDefinitions = false;
	mHasUnresolvedValueDefinitions = false;

	QString code = mScriptCode->get();
	QStringList linesOfCode = code.split("/**/");
	
	for(int i = 0; i < linesOfCode.size(); ++i) {
		QString line = linesOfCode.at(i);
		if(line.contains("defVar(") || line.contains("defEvent(") || line.contains("defVarR(")) {
			if(mScript != 0) {
				mScript->evaluate(line);
			}
		}
	}

	if(mHasUnresolvedValueDefinitions || mHasUnresolvedEventDefinitions) {
		if(Core::getInstance()->isInitialized()) {
			Core::log("Scripting Context [" + getName() + "]: "
					  "ReloadEventAndValueDefinitions: Unresolved defVar or defEvent.", true);
		}
	}

}


/**
 * Sets the QString with the entire new script.
 * The string is placed in Value 'ScriptCode', hereby replacing all newline characters
 * by a neutral char sequence to fit the entire code into one line.
 *
 * To get the string back with all its newline chars, use method getScriptCode().
 *
 * @param code the new string.
 */
bool ScriptingContext::setScriptCode(const QString &code) {
	QString singleLineCode = code;
	singleLineCode.replace("\n", "/**/");
	mScriptCode->set(singleLineCode);
	return true;
}

/**
 * Returns the current script code. This code is obtained from parameter Value 
 * 'Code', so the code might have changed by the user or other parts of the program.
 * The received code may contain newline characters.
 *
 * @return the current script code.
 */
QString ScriptingContext::getScriptCode() const {
	QString code = mScriptCode->get();
	code.replace("/**/", "\n");
	return code;
}

/**
 * Loads script code from a the file specified in parameter Value 
 * 'FileName'. If replaceExistingCode is true, then the old script code is
 * overwritten by the new one. Otherwise if there is already script code
 * available, then the code from the file is ignored.
 * The loaded code is put into Value 'Code' via method setScriptCode(), 
 * which might change the code a bit, like removing newline characters.
 * 
 * @param replaceExistingCode flag to determine whether to protect existing code.
 * @return true if successful, otherwise false.
 */
bool ScriptingContext::loadScriptCode(bool replaceExistingCode) {
	if(mScriptFileName->get() == "") {
		if(replaceExistingCode) {
			mScriptCode->set("");
		}
	}

	if(!replaceExistingCode && mScriptCode->get().trimmed() != "") {
		//TODO check if this should return true.
		return false;
	}

	mWrittenVariables.clear();
	mReadVariables.clear();
	mEventNames.clear();
	mEventOccurences.clear();
	mPersistentParameters.clear();

	QFile file(mScriptFileName->get());

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		Core::log(QString("Could not load fitness script file [")
				+ mScriptFileName->get() + "] to ScriptingContext ["
				+ getName() + "]");
		file.close();
		return false;
	}

	QTextStream fileStream(&file);
	setScriptCode(fileStream.readAll());
	file.close();

	Core::log(QString("ScriptingContext: Loaded script code from file [")
			  + mScriptFileName->get() + "].");

	return true;
}

void ScriptingContext::setMainContextName(const QString &contextName) {
	mMainContextName = contextName;
}


QString ScriptingContext::getMainContextName() const {
	return mMainContextName;
}

void ScriptingContext::setStringBuffer(const QString &string) {
	mVariableBuffer = string;
}

QString ScriptingContext::getStringBuffer() const {
	return mVariableBuffer;
}

/**
 * Defines a script variable named name that is set to the content of Value valueName at the beginning
 * of each step. 
 * Flag reportMissingValues indicates whether warnings are logged when a Value with the given
 * name could not be found. This is the default for variable definitions from within scripts. 
 * In some cases it is not an error if a Value is missing. In these cases set the flag to false.
 * The variables of missing Values will still be defined in script scope, but will always be set
 * to 0.
 * 
 * @param name the name of the script variable
 * @param valueName the name of the Value serving as source of the variable content.
 * @param reportMissingValues flag to determine whether warnings are logged in case of missing Values.
 */
void ScriptingContext::defineVariable(const QString &name, const QString &valueName, 
						bool reportMissingValues) 
{
	Value *value = Core::getInstance()->getValueManager()->getMultiPartValue(valueName);

	if(value == 0 && reportMissingValues) {
		if(Core::getInstance()->isInitialized()) {
			reportError(QString("ScriptingContext [") + getName() + "]: "
					+ "Could not define string variable [" + name 
					+ "] for value [" + valueName + "]! Value name not found!");
		}
		mHasUnresolvedValueDefinitions = true;
		return;
	}

	//Add as read and written variables
	mWrittenVariables.insert(name, value);
	mReadVariables.insert(name, value);
}

void ScriptingContext::defineReadOnlyVariable(const QString &name, const QString &valueName, 
						bool reportMissingValues)
{
	Value *value = Core::getInstance()->getValueManager()->getMultiPartValue(valueName);

	if(value == 0 && reportMissingValues) {
		if(Core::getInstance()->isInitialized()) {
			reportError(QString("ScriptingContext [") + getName() + "]: "
					+ "Could not define string variable [" + name 
					+ "] for value [" + valueName + "]! Value name not found!");
		}
		mHasUnresolvedValueDefinitions = true;
		return;
	}

	//Add only as read variables (do not write changed variables back
	mReadVariables.insert(name, value);
}

/**
 * Defines a script variable named name that is true when the Event with the given eventName 
 * was present during the current simulation step.
 * Flag reportMissingValues indicates whether warnings are logged when an Event with the given
 * name could not be found. This is the default for variable definitions from within scripts. 
 * In some cases it is not an error if an Event is missing. In these cases set the flag to false.
 * The variables of missing Events will still be defined in script scope, but will always be set
 * to 0.
 * 
 * @param name the name of the script variable
 * @param valueName the name of the Event that is observed.
 * @param reportMissingValues flag to determine whether warnings are logged in case of missing Events.
 */
void ScriptingContext::defineEvent(const QString &name, const QString &eventName, 
						bool reportMissingEvents) 
{
	EventManager *em = Core::getInstance()->getEventManager();

	Event *event = em->registerForEvent(eventName, this);
	
	if(event == 0 && reportMissingEvents) {
		if(Core::getInstance()->isInitialized()) {
			reportError(QString("ScriptingContext [") + getName() + "]: "
					+ "Could not define event variable [" + name 
					+ "] for event [" + eventName + "]! Event name not found!");
		}
		mHasUnresolvedEventDefinitions = true;
		return;
	}
	mEventNames.insert(event, name);
	mEventOccurences.insert(name, false);
}

void ScriptingContext::definePersistentParameter(const QString &name, 
						const QString &initialValue) 
{
	if(mPersistentParameters.keys().contains(name)) {
		return;
	}
	mPersistentParameters.insert(name, initialValue);
}

bool ScriptingContext::setProperty(const QString &fullPropertyName, const QString &value) {
	ValueManager *vm = Core::getInstance()->getValueManager();

	QString propNameRegExp = fullPropertyName;
	propNameRegExp = propNameRegExp.replace("**", ".*");
	QList<QString> matchingNames = vm->getValueNamesMatchingPattern(propNameRegExp, true);

	if(matchingNames.empty()) {
		reportError("Could not find global property [" 
				+ fullPropertyName + "] to set to [" + value + "]");
		return false;
	}

	bool ok = true;
	for(QListIterator<QString> i(matchingNames); i.hasNext();) {
		Value *v = vm->getValue(i.next());
		if(v == 0) {
			reportError("Could not find global property [" 
					+ fullPropertyName + "] to set to [" + value + "]");
			ok = false;
			continue;
		}
		if(!v->setValueFromString(value)) {
			reportError("Could not set global property [" 
					+ fullPropertyName + "] to [" + value + "]");
			ok = false;
			continue;
		}
	}
	return ok;
}

QString ScriptingContext::getProperty(const QString &fullPropertyName) {
	ValueManager *vm = Core::getInstance()->getValueManager();

	QString propNameRegExp = fullPropertyName;
	propNameRegExp = propNameRegExp.replace("**", ".*");
	QList<QString> matchingNames = vm->getValueNamesMatchingPattern(propNameRegExp, true);

	if(matchingNames.empty()) {
		reportError("Could not find global property [" 
				+ fullPropertyName + "]");
		return "";
	}

	for(QListIterator<QString> i(matchingNames); i.hasNext();) {
		Value *v = vm->getValue(i.next());
		if(v == 0) {
			reportError("Could not find global property [" 
					+ fullPropertyName + "]");
			continue;
		}
		return v->getValueAsString();
	}
	return "";
}

QScriptValue ScriptingContext::getPropertyAsArray(const QString fullPropertyName) {
	QScriptValue array = mScript->newArray(3);
	QString vector3DString = getProperty(fullPropertyName);
	if(!vector3DString.startsWith("(") || !vector3DString.endsWith(")")) {
		Core::log("ScriptedModel::getVectorElement: Could not parse [" + vector3DString + "]", true);
		return array;
	}
	QString content = vector3DString;
	QStringList elements = content.mid(1, content.length() - 2).split(",");
	if(elements.size() <= 0 || elements.size() > 3) {
		Core::log("ScriptedModel::getVectorElement: Property was not a valid Vetor3D.", true);
		return array;
	}
	for(int i = 0; i < elements.size(); ++i) {
		array.setProperty(i, elements.at(i).toDouble());
	}
	return array;
}

bool ScriptingContext::createGlobalStringProperty(const QString &propertyName, const QString &content) {
	ValueManager *vm = Core::getInstance()->getValueManager();
	if(vm->nameExists(propertyName)) {
		return false;
	}
	StringValue *value = new StringValue(content);
	return vm->addValue(propertyName, value);
}

bool ScriptingContext::createGlobalDoubleProperty(const QString &propertyName, double content) {
	ValueManager *vm = Core::getInstance()->getValueManager();
	if(vm->nameExists(propertyName)) {
		return false;
	}
	DoubleValue *value = new DoubleValue(content);
	return vm->addValue(propertyName, value);
}

void ScriptingContext::setValueFromString(const QString &valueName, const QString &content, 
						bool warnIfValueMissing) 
{
	Value *value = Core::getInstance()->getValueManager()->getValue(valueName);
	if(value == 0) {
		if(warnIfValueMissing) {
			reportError(QString("ScriptingContext [") + getName() + "]: "
					+ "Could not find value [" + valueName + "]!");
		}
		return;
	}
	if(!value->setValueFromString(content) && warnIfValueMissing) {
		reportError(QString("ScriptingContext [") + getName() + "]: "
				+ "Could not set content [" + content + "] to value [" + valueName + "]!");
	}
}

/**
 * Triggers any event from the scripting context.
 * Note: This method is dangerous and can harm the application flow if used improperly.
 *
 * @param eventName the name of the Event to trigger
 * @param createIfNotAvailable if true, then a new event is created if there is no matching event.
 */
void ScriptingContext::triggerEvent(const QString &eventName, bool createIfNotAvailable) {
	EventManager *em = Core::getInstance()->getEventManager();
	Event *event = em->getEvent(eventName, createIfNotAvailable);
	if(event != 0) {
		event->trigger();
	}
}


/**
 * Loads the content of a file and returns its entire content as a single string.
 * This string can be parsed and interpreted in the scripting context.
 * This method is only defined for existing, character based files.
 * If the specified file does not exist, then the returned string is empty.
 *
 * @param fileName the name of the character based file to load.
 * @return the content of the entire file in a single string.
 */
QString ScriptingContext::loadFileToString(const QString &fileName) {
	QDir dir(fileName);
	if(dir.exists()) {
		Core::log("ScriptingContext [" + getName() + "]:  Could not load directory [" 
				+ fileName + "]", true);
		return "";
	}

	QFile file(fileName);
	if(!file.exists()) {
		Core::log("ScriptingContext [" + getName() + "]: Could not load file [" 
				+ fileName + "]", true);
		return "";
	}

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		Core::log(QString("ScriptingContext [" + getName() + "]: Error while loading file [") 
				+ fileName + "]", true);
		file.close();
		return "";
	}
	QTextStream input(&file);
	return input.readAll();
}

double ScriptingContext::random() {
	return Random::nextDouble();
}


int ScriptingContext::randomInt(int max) {
	return Random::nextInt(max);
}


QString ScriptingContext::toVector3DString(double x, double y, double z) {
	return QString("(") + QString::number(x) + "," + QString::number(y) + "," + QString::number(z) + ")";
}


QString ScriptingContext::toColorString(double r, double g, double b, double t) {
	return QString("(") + QString::number(r) + "," + QString::number(g) 
			+ "," + QString::number(b) + "," + QString::number(t) + ")";
}

double ScriptingContext::getVectorElement(const QString &vector3DString, int index) {
	if(!vector3DString.startsWith("(") || !vector3DString.endsWith(")")) {
		Core::log("ScriptedModel::getVectorElement: Could not parse [" + vector3DString + "]", true);
		return 0.0;
	}
	QString content = vector3DString;
	QStringList elements = content.mid(1, content.length() - 2).split(",");
	if(elements.size() <= 0 || elements.size() >= index) {
		Core::log("ScriptedModel::getVectorElement: Index out of range ["
				+ QString::number(index) + "]", true);
		return 0.0;
	}
	return elements.at(index).toDouble();
}

bool ScriptingContext::loadValues(const QString &fileName) {
	return Core::getInstance()->getValueManager()->loadValues(fileName);
}

/**
 * Writes a log message to the core logger.
 * Can be overwritten by subclasses to report the errors somewhere else.
 *
 * @param message the message to log.
 */
void ScriptingContext::reportError(const QString &message) {
	QString msg = message;
	Core::log(QString("ScriptingContext [") + getName() + "]: " + message, true);
}

/**
 * Imports the content of all Values aded with defineVariables to the scripting context.
 * IntValues and DoubleValues are set as native ints and doubles. 
 * BoolValues will be translated to real bools (true / false).
 * All other Value types will be set as Strings with enclosing parantheses.
 */
void ScriptingContext::importVariables() {
	if(mScript != 0) {

		//update variables
		for(QHash<QString, Value*>::iterator i = mReadVariables.begin(); 
			i != mReadVariables.end(); i++) 
		{
			Value *v = i.value();
			if(v == 0) {
				continue;
			}

			if(dynamic_cast<DoubleValue*>(v) != 0	
				|| dynamic_cast<IntValue*>(v) != 0)
			{
				mScript->evaluate(i.key() + " = " + i.value()->getValueAsString() + ";");
			}
			else if(dynamic_cast<BoolValue*>(v) != 0) {
				QString state = (dynamic_cast<BoolValue*>(v)->get() == true) ? "true" : "false";
				mScript->evaluate(i.key() + " = " + state + ";");
			}
			else if(dynamic_cast<MultiPartValue*>(v) != 0) {
				MultiPartValue *mpv = dynamic_cast<MultiPartValue*>(v);
				for(int j = 0; j < mpv->getNumberOfValueParts(); ++j) {
					Value *part = mpv->getValuePart(j);
					QString content = "";
					if(dynamic_cast<IntValue*>(part) != 0 || dynamic_cast<DoubleValue*>(part) != 0) {
						content = part->getValueAsString();
					}
					else if(dynamic_cast<BoolValue*>(part) != 0) {
						content = (dynamic_cast<BoolValue*>(v)->get() == true) ? "true" : "false";
					}
					else {
						content = QString("\"") + part->getValueAsString() + "\"";
					}
					QScriptValue error = mScript->evaluate(i.key() + "." + mpv->getValuePartName(j) + " = " 
										+ content + ";");
					
					if(mScript->hasUncaughtException()) {
						reportError(QString("Could not import value part.") 
										+ error.toString());
					}
				}
			}
			else {
				mScript->evaluate(i.key() + " = '" + i.value()->getValueAsString() + "';");
			}
		}

		//update events
		for(QHash<QString, bool>::iterator i = mEventOccurences.begin(); i != mEventOccurences.end(); i++) {
			QString state = (i.value() == true) ? "true" : "false";
			mScript->evaluate(i.key() + " = " + state + ";");
			//reset occurence marker
			i.value() = false;
		}

		//update persistent parameters
		for(QHash<QString, QString>::iterator i = mPersistentParameters.begin(); 
			i != mPersistentParameters.end(); i++) 
		{
			mScript->evaluate(i.key() + " = " + i.value() + ";");
		}
	}
}

void ScriptingContext::exportVariables() {
	if(mScript != 0) {

		//update variables
		for(QHash<QString, Value*>::iterator i = mWrittenVariables.begin(); 
			i != mWrittenVariables.end(); i++) 
		{
			Value *v = i.value();
			if(v == 0) {
				continue;
			}

			if(dynamic_cast<DoubleValue*>(v) != 0	
				|| dynamic_cast<IntValue*>(v) != 0)
			{
				mScript->evaluate(mMainContextName + ".stringBuffer = " + i.key() + ".toString();");
				v->setValueFromString(mVariableBuffer);
			}
			else if(dynamic_cast<BoolValue*>(v) != 0) {
				mScript->evaluate(mMainContextName + ".stringBuffer = " + i.key() + ".toString();");
				if(mVariableBuffer == "true") {
					v->setValueFromString("true");
				}
				else {
					v->setValueFromString("false");
				}
			}
			else if(dynamic_cast<MultiPartValue*>(v) != 0) {
				MultiPartValue *mpv = dynamic_cast<MultiPartValue*>(v);
				for(int j = 0; j < mpv->getNumberOfValueParts(); ++j) {
					Value *part = mpv->getValuePart(j);
			
					QScriptValue error = mScript->evaluate(mMainContextName+ ".stringBuffer = " 
											+ i.key() + "." + mpv->getValuePartName(j) + ".toString();");

					part->setValueFromString(mVariableBuffer);
					
					if(mScript->hasUncaughtException()) {
						reportError(QString("Could not import value part.") 
										+ error.toString());
					}
				}
			}
			else {
				mScript->evaluate(mMainContextName + ".stringBuffer = " + i.key() + ".toString();");
				v->setValueFromString(mVariableBuffer);
			}
		}
	
		//export persistent data
		for(QHash<QString, QString>::iterator i = mPersistentParameters.begin(); 
			i != mPersistentParameters.end(); i++) 
		{
			mScript->evaluate(mMainContextName + ".stringBuffer = " + i.key() + ".toString();");
			mPersistentParameters.insert(i.key(), mVariableBuffer);
		}

	}
}

void ScriptingContext::addCustomScriptContextStructures() {
	
}



}



