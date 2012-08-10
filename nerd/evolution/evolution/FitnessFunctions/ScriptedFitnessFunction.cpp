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



#include "ScriptedFitnessFunction.h"
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
#include "PlugIns/CommandLineArgument.h"
#include "PlugIns/PlugInManager.h"
#include <QFile>
#include <QTextStream>
#include "EvolutionConstants.h"
#include "Value/MultiPartValue.h"
#include "Util/Tracer.h"
#include <QDir>
#include <QTextStream>
#include "NerdConstants.h"


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;
 
namespace nerd {


/**
 * Constructs a new ScriptedFitnessFunction.
 *
 * @pram name the name of the new FitnessFunction.
 */
ScriptedFitnessFunction::ScriptedFitnessFunction(const QString &name)
	: ScriptingContext(name), ControllerFitnessFunction(name), 
		mScriptFileNameArg(0), mSimEnvironmentChangedEvent(0)
{
	PlugInManager *pm = Core::getInstance()->getPlugInManager();

	mScriptFileNameArg = pm->getCommandLineArgument("fitnessScriptFile");
	if(mScriptFileNameArg == 0) {
		mScriptFileNameArg = new CommandLineArgument("fitnessScriptFile", "fsf", 
				"<fitnessName> <fileName>",
				"Loads a fitness function script from file <fileName>\n   and attaches it "
				"to the ScriptedFitnessFunction\n   with name <fitnessName>.",
				2, 0, true, true);
	}

	//script code is initialized with a simple but valid fitness script, just counting the steps.
	mScriptCode->setValueFromString(QString("") 
					+ "function reset() {/**/};/**//**/function calc()"
					+ " {/**/  return fitness + 1;/**/};");
	
	mScriptCode->setErrorValue(mErrorState);

	addParameter("Code", mScriptCode);
	addParameter("FileName", mScriptFileName);
}


/**
 * Copy constructor. 
 * 
 * @param other the ScriptedFitnessFunction object to copy.
 */
ScriptedFitnessFunction::ScriptedFitnessFunction(const ScriptedFitnessFunction &other) 
	: ValueChangedListener(), ScriptingContext(other),
	  ControllerFitnessFunction(other), mSimEnvironmentChangedEvent(0)
{
	mScriptFileNameArg = other.mScriptFileNameArg;

	delete mScriptCode;
	delete mScriptFileName;

	mScriptCode = dynamic_cast<CodeValue*>(getParameter("Code"));
	mScriptCode->addValueChangedListener(this);
	mScriptFileName = dynamic_cast<FileNameValue*>(getParameter("FileName"));
	//mScriptFileName->useAsFileName(true);
	
	mScriptCode->setErrorValue(mErrorState);

	mBindPhaseEvent = Core::getInstance()->getEventManager()->registerForEvent(
						NerdConstants::EVENT_NERD_SYSTEM_BIND, this);
}

/**
 * Destructor.
 */
ScriptedFitnessFunction::~ScriptedFitnessFunction() {
}


/**
 * Creates a new instance as copy of this.
 *
 * @return a copy of this FitnessFunction.
 */
FitnessFunction* ScriptedFitnessFunction::createCopy() const {
	return new ScriptedFitnessFunction(*this);
}



QString ScriptedFitnessFunction::getName() const {
	return ControllerFitnessFunction::getName();
}


/**
 * Called when the FitnessFunction is attached to the System. 
 */
void ScriptedFitnessFunction::attachToSystem() {
	TRACE("ScriptedFitnessFunction::attachToSystem");

	ControllerFitnessFunction::attachToSystem();

	if(mScriptFileNameArg != 0) {
		int numberOfEntries = mScriptFileNameArg->getNumberOfEntries();
		for(int i = 0; i < numberOfEntries; ++i) {
			QStringList args = mScriptFileNameArg->getEntryParameters(i);
			if(args.size() != 2) {
				continue;
			}
			if(args.at(0) == getName()) {
				mScriptFileName->set(args.at(1));

				if(!loadScriptCode(false)) {
					continue;
				}
				break;
			}
		}
	}

	mScriptFileName->addValueChangedListener(this);

	EventManager *em = Core::getInstance()->getEventManager();
	mSimEnvironmentChangedEvent = em->registerForEvent(
					EvolutionConstants::EVENT_PHYSICS_ENVIRONMENT_CHANGED,
					this);
}


/**
 * Called when the FitnessFunction is detached to the System. 
 */
void ScriptedFitnessFunction::detachFromSystem() {
	TRACE("ScriptedFitnessFunction::detachFromSystem");

	ControllerFitnessFunction::detachFromSystem();

	mScriptFileName->removeValueChangedListener(this);

	if(mSimEnvironmentChangedEvent != 0) {
		mSimEnvironmentChangedEvent->removeEventListener(this);
	}
}

void ScriptedFitnessFunction::setControlInterface(ControlInterface *controlInterface) {
	ControllerFitnessFunction::setControlInterface(controlInterface);
}


/**
 * Used to react on changes of the scriptFileName to reload the fitness script.
 */
void ScriptedFitnessFunction::valueChanged(Value *value) {
	TRACE("ScriptedFitnessFunction::valueChanged");

	ScriptingContext::valueChanged(value);
	if(value == 0) {
		return;
	}
}

void ScriptedFitnessFunction::eventOccured(Event *event) {
	TRACE("ScriptedFitnessFunction::eventOccured");

	ScriptingContext::eventOccured(event);
	if(event == 0) {
		return;
	}
	else if(event == mSimEnvironmentChangedEvent) {
		resetScriptContext();
	}
	else if(event == mBindPhaseEvent) {
		//make sure scripts are reloaded during bind phase if created before 
		//(so that all values and events are available).
		resetScriptContext();
	}
}


/**
 * Calculates the current Fitness by calling the script method calc():
 *
 * @return the current fitness oft he evaluated individual.
 */
double ScriptedFitnessFunction::calculateCurrentFitness() {
	TRACE("ScriptedFitnessFunction::calculateCurrentFitness");

	if(mScript == 0) {
		return 0.0;
	}
	executeScriptFunction("_calculateFitness();");

	return mFitnessBuffer;
}


/**
 * Called to reset the FitnessFunction. 
 * This destroys the previous script engine and provides a clean one. 
 * This script engine provides a number of global variables and functions 
 * that can be used in the fitness script. 
 */
void ScriptedFitnessFunction::resetScriptContext() {

	if(!Core::getInstance()->isMainExecutionThread()) {
		return;
	}

	ControllerFitnessFunction::reset();

	mErrorState->set("");
	mFitnessBuffer = 0.0;

	ScriptingContext::resetScriptContext();
}


/**
 * Calls the prepareTry() function in the fitness script.
 */
void ScriptedFitnessFunction::prepareNextTry() {
	TRACE("ScriptedFitnessFunction::prepareNextTry");

	mFitnessBuffer = 0.0;

	executeScriptFunction("reset()");

// 	if(mScript != 0) {
// 		importVariables();
// 
// 		QScriptValue error = mScript->evaluate("reset();");
// 		if(mScript->hasUncaughtException()) {
// 			reportError(QString("Error while calling function reset(). Not implemented? ")
// 						+ error.toString());
// 		}
// 
// 		exportVariables();
// 	}
}



/**
 * This is a helper slot to enable try termination from within the fitness script.
 * This method just forwards execution to terminateTry().
 */
void ScriptedFitnessFunction::terminateCurrentTry() {
	TRACE("ScriptedFitnessFunction::terminateCurrentTry");

	terminateTry();
}


/**
 * This is a helper slot to enable reading of double values from within the scripts.
 * This method just forwards execution to getValue().
 */
double ScriptedFitnessFunction::getDoubleValue(const QString &valueName) {
	return getValue(valueName);
}


/**
 * This is a helper slot to enable reading of arbitrary values as Strings from 
 * within the scripts. This method forwards execution to getValueAsString().
 */
QString ScriptedFitnessFunction::getStringValue(const QString &valueName) {
	return getValueAsString(valueName);
}


/**
 * Sets the value of the fitnessBuffer. This slot can be invoked by scripts.
 * 
 * @param currentFitness the fitness to set.
 */
void ScriptedFitnessFunction::setFitnessBuffer(double currentFitness) {
	mFitnessBuffer = currentFitness;
}


/**
 * Returns the current value of the FitnessBuffer. The FitnessBuffer is used to communicate
 * the current fitness with the scripts.
 *
 * @return the current fitness buffer.
 */
double ScriptedFitnessFunction::getFitnessBuffer() const {
	return mFitnessBuffer;
}



/**
 * Writes a log message to the core logger and to the mErrorState parameter Value.
 *
 * @param message the message to log.
 */
void ScriptedFitnessFunction::reportError(const QString &message) {
	QString msg = message;
	mErrorState->set(msg.replace("\n", " | "));
	Core::log(QString("ScriptFitness [") + getName() + "]: " + message);
}




void ScriptedFitnessFunction::addCustomScriptContextStructures() {

	ScriptingContext::addCustomScriptContextStructures();
	
	QScriptValue error = mScript->evaluate(QString("function terminateTry() {")
					  + "  nerd.terminateCurrentTry() };");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add defVar function. ") 
						+ error.toString());
		return;
	}

	error = mScript->evaluate("var fitness = nerd.fitness;");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add fitness variable. ") 
						+ error.toString());
		return;
	}

	defineVariable("tryCount", EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES, false);
	defineVariable("stepCount", EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS, false);

	error = mScript->evaluate(
			"function _calculateFitness() { fitness = nerd.fitness; nerd.fitness = this.calc(); };");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add _calculateFitness() method. Naming conflict? ")
					+ error.toString());
		return;
	}
}


}



