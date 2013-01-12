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

#include "FitnessManager.h"
#include "EvolutionConstants.h"
#include "NerdConstants.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Core/Core.h"
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QDate>
#include <QTime>
#include <iostream>
#include "PlugIns/CommandLineArgument.h"


using namespace std;

namespace nerd {


/**
 * Constructor.
 */
FitnessManager::FitnessManager() : mNextTryEvent(0), mNextIndividualEvent(0), 
		mStepCompletedEvent(0), mTryCompletedEvent(0), mIndividualCompletedEvent(0),
		mGenerationCompletedEvent(0), mResetEvent(0), mNumberOfSteps(0), mNumberOfTries(0),
		mTerminateTryEvent(0), mEnableFitnessFunctionExecution(true)
{
}


/**
 * Destructor. 
 * Destroys all registered FitnessFunctions and FitnessFunction prototypes.
 */
FitnessManager::~FitnessManager() {
	while(!mFitnessFunctions.empty()) {
		FitnessFunction* ff = mFitnessFunctions.begin().value();
		mFitnessFunctions.erase(mFitnessFunctions.begin());
		delete ff;
	}
	while(!mFitnessFunctionPrototypes.empty()) {
		FitnessFunction *fPrototype = mFitnessFunctionPrototypes.begin().value();
		mFitnessFunctionPrototypes.erase(mFitnessFunctionPrototypes.begin());
		delete fPrototype;
	}
}

/**
 * Initializes the FitnessManager.
 */
bool FitnessManager::init() {
	bool initOk = true;

	//print names of FitnessFunctionPrototypes if required
	CommandLineArgument *printFitnessFunctionsArgument = new CommandLineArgument("fitnessPrototypes", 
							"fitnessPrototypes", "", 
							"Prints the names of all available FitnessFunction prototypes.",
							0, 0, false, false);
	printFitnessFunctionsArgument->setProperty("debug");

	if(printFitnessFunctionsArgument->getNumberOfEntries() > 0) {
		cerr << "FitnessFunction Prototypes: " << endl
			 << "-----------------------------" << endl;
		QList<FitnessFunction*> prototypes = mFitnessFunctionPrototypes.values();
		for(QListIterator<FitnessFunction*> i(prototypes); i.hasNext();) {
			FitnessFunction *ff = i.next();
			cerr << " > " << ff->getName().toStdString().c_str() << endl;
		}
	}

	return initOk;
}

/** 
 * Establishes connections to Events and Values.
 */
bool FitnessManager::bind() {
	
	bool bindOk = true;

	EventManager *eManager = Core::getInstance()->getEventManager();

	mNextTryEvent = eManager->registerForEvent(
		EvolutionConstants::EVENT_EXECUTION_NEXT_TRY, this);
	mNextIndividualEvent = eManager->registerForEvent(
		EvolutionConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL, this);
	mStepCompletedEvent = eManager->registerForEvent(
		NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, this);
	mTryCompletedEvent = eManager->registerForEvent(
		EvolutionConstants::EVENT_EXECUTION_TRY_COMPLETED, this);
	mIndividualCompletedEvent = eManager->registerForEvent(
		EvolutionConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED, this);
	mGenerationCompletedEvent = eManager->registerForEvent(
		EvolutionConstants::EVENT_EXECUTION_GENERATION_COMPLETED, this, false);
	mTerminateTryEvent = eManager->getEvent(
		EvolutionConstants::EVENT_EXECUTION_TERMINATE_TRY, false);
	mResetEvent = eManager->registerForEvent(
		NerdConstants::EVENT_EXECUTION_RESET, this);

	if(mNextTryEvent == 0) {
		Core::log(QString("FitnessManager: Could not find Event [")
			.append(EvolutionConstants::EVENT_EXECUTION_NEXT_TRY)
			.append("]! [TERMINATING]"));
		bindOk = false;
	}
	if(mNextIndividualEvent == 0) {
		Core::log(QString("FitnessManager: Could not find Event [")
			.append(EvolutionConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL)
			.append("]! [TERMINATING]"));
		bindOk = false;
	}
	if(mStepCompletedEvent == 0) {
		Core::log(QString("FitnessManager: Could not find Event [")
			.append(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED)
			.append("]! [TERMINATING]"));
		bindOk = false;
	}
	if(mTryCompletedEvent == 0) {
		Core::log(QString("FitnessManager: Could not find Event [")
			.append(EvolutionConstants::EVENT_EXECUTION_TRY_COMPLETED)
			.append("]! [TERMINATING]"));
		bindOk = false;
	}
	if(mIndividualCompletedEvent == 0) {
		Core::log(QString("FitnessManager: Could not find Event [")
			.append(EvolutionConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED)
			.append("]! [TERMINATING]"));
		bindOk = false;
	}
	if(mResetEvent == 0) {
		Core::log(QString("FitnessManager: Could not find Event [")
			.append(NerdConstants::EVENT_EXECUTION_RESET)
			.append("]! [TERMINATING]"));
		bindOk = false;
	}

	ValueManager *vm = Core::getInstance()->getValueManager();

	mNumberOfSteps = vm->getIntValue(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS);
	mNumberOfTries = vm->getIntValue(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES);

	return bindOk;
}

bool FitnessManager::cleanUp() {
	bool clearOk = true;
	return clearOk;
}

QString FitnessManager::getName() const {
	return "FitnessManager";
}

void FitnessManager::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if((event == mNextTryEvent || event == mResetEvent) && mEnableFitnessFunctionExecution) {
		QHash<QString, FitnessFunction*>::const_iterator i = mFitnessFunctions.constBegin();
		while(i != mFitnessFunctions.constEnd()) {
			i.value()->resetTry();
			++i;
		}
	}
	else if(event == mNextIndividualEvent && mEnableFitnessFunctionExecution) {
		QHash<QString, FitnessFunction*>::const_iterator i = mFitnessFunctions.constBegin();
		while(i != mFitnessFunctions.constEnd()) {
			i.value()->reset();
			++i;
		}
	}
	else if(event == mStepCompletedEvent && mEnableFitnessFunctionExecution) {	
		QHash<QString, FitnessFunction*>::const_iterator i = mFitnessFunctions.constBegin();
		while(i != mFitnessFunctions.constEnd()) {
			i.value()->update();
			++i;
		}
	}
	else if(event == mTryCompletedEvent && mEnableFitnessFunctionExecution) {	
		QHash<QString, FitnessFunction*>::const_iterator i = mFitnessFunctions.constBegin();
		while(i != mFitnessFunctions.constEnd()) {
			i.value()->finishTry();
			++i;
		}
	}
	else if(event == mIndividualCompletedEvent) {	
		QHash<QString, FitnessFunction*>::const_iterator i = mFitnessFunctions.constBegin();
		while(i != mFitnessFunctions.constEnd()) {
			i.value()->finishIndividual();
			++i;
		}
	}
	if(event == mGenerationCompletedEvent) {	
		QHash<QString, FitnessFunction*>::const_iterator i = mFitnessFunctions.constBegin();
		while(i != mFitnessFunctions.constEnd()) {
			i.value()->finishGeneration();
			++i;
		}
	}
}


/**
 * Adds a fitnessfunction to the fitness manager. After successfully adding the fitness 
 * function, this function will be called after the according events occurred 
 * (individual completed, try completed...).
 *
 * @param fitnessFunction 
 * @return 
 */
bool FitnessManager::addFitnessFunction(FitnessFunction *fitnessFunction) {
	if(fitnessFunction == 0 
		|| mFitnessFunctions.keys().contains(fitnessFunction->getName()) 
		|| mFitnessFunctions.values().contains(fitnessFunction)) 
	{
		return false;
	}
	mFitnessFunctions[fitnessFunction->getName()] = fitnessFunction;
	return true;
}

/**
 * Remove the fitness function with the given name from the list of managed fitness 
 * functions. Thereby, this function will not be updated during the evolution process.
 * 
 * @param fitnessFunctionName 
 * @return 
 */
bool FitnessManager::removeFitnessFunction(const QString &fitnessFunctionName) {
	if(!mFitnessFunctions.keys().contains(fitnessFunctionName)) {
		return false;
	}
	mFitnessFunctions.remove(fitnessFunctionName);
	return true;
}

/**
 * Remove a fitness function from the list of managed fitness functions. Thereby, this function will not be updated during the evolution process.
 * @param fitnessFunction 
 * @return 
 */
bool FitnessManager::removeFitnessFunction(FitnessFunction *fitnessFunction) {
	if(!mFitnessFunctions.values().contains(fitnessFunction) 
		|| mFitnessFunctions.value(fitnessFunction->getName()) != fitnessFunction) 
	{
		return false;
	}
	mFitnessFunctions.remove(fitnessFunction->getName());
	return true;
}

QList<FitnessFunction*> FitnessManager::getFitnessFunctions() {
	return mFitnessFunctions.values();
}

QList<QString> FitnessManager::getFitnessFunctionNames() {	
	return mFitnessFunctions.keys();
}

/**
 * Adds a prototype of a fitness function to the list of prototypes. The prototype name must be unique to allow a name-based identification of the prototype to be used.
 * @param name The name to be used as prototype name.
 * @param fitnessFunction The fitness function to be used as prototype of the given name.
 * @return 
 */
bool FitnessManager::addFitnessFunctionPrototype(const QString &name, FitnessFunction* fitnessFunction) {
	if(name == 0 
		|| fitnessFunction == 0 
		|| mFitnessFunctionPrototypes.keys().contains(name) 
		|| mFitnessFunctionPrototypes.values().contains(fitnessFunction)) 
	{
		return false;
	}
	mFitnessFunctionPrototypes[name] = fitnessFunction;
	return true;
}

bool FitnessManager::removeFitnessFunctionPrototype(const QString &name) {
	if(name == 0 || !mFitnessFunctionPrototypes.keys().contains(name)) {
		return false;
	}
	mFitnessFunctionPrototypes.remove(name);
	return true;
}

QHash<QString, FitnessFunction*> FitnessManager::getFitnessFunctionPrototypes() {
	return mFitnessFunctionPrototypes;
}

/**
 * Create a fitness function from a prototype. This method creates a copy of the fitnessfunction that is stored as prototype of the given name.
 * @param prototypeName The name of the prototype to be used
 * @param fitnessFunctionName The name to be used for the fitnessfunction.
 * @return 
 */
FitnessFunction* FitnessManager::createFitnessFunctionFromPrototype(const QString &prototypeName, 
										const QString &fitnessFunctionName) 
{
	if(prototypeName == 0 
		|| !mFitnessFunctionPrototypes.keys().contains(prototypeName) 
		|| mFitnessFunctionPrototypes.value(prototypeName) == 0) 
	{
		return 0;
	}
	FitnessFunction* fitnessFunction = mFitnessFunctionPrototypes.value(prototypeName)->createCopy();
	if(fitnessFunction == 0) {
		return 0;
	}
	if(fitnessFunctionName != "") {
		fitnessFunction->setName(fitnessFunctionName);
	} 
	else {
		fitnessFunction->setName(prototypeName);
	}
	fitnessFunction->setPrototypeName(prototypeName);
	return fitnessFunction;
}

bool FitnessManager::writeFitnessValuesToFile(const QString &fileName, 
		const QString &comment) 
{
	QString fullPath = fileName;
	QFile file(fullPath);
	
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		std::cerr << "Could not open file " << fullPath.toStdString().c_str()
			<< " to write Values." << std::endl;
		file.close();
		return false;
	}

	QString correctedComment = comment;
	correctedComment.replace("\n", "\n#");

	QTextStream output(&file);
	output << "#NERD FitnessFunctions" << "\n";
	output << "#Saved at: " << QDate::currentDate().toString("dd.MM.yyyy")
		<< "  " << QTime::currentTime().toString("hh:mm:ss") << "\n";
	output << "#\n#" << correctedComment << "#\n";

 	QHash<QString, FitnessFunction*>::iterator i;
 	for (i = mFitnessFunctions.begin(); i != mFitnessFunctions.end(); ++i) {
		QString fitnessName = i.key();
		FitnessFunction *fitness = i.value();
		double fitnessValue = fitness->getFitness();
		output << fitnessName << "=" << fitnessValue << "\n";
	}

	file.close();
	return true;

}


void FitnessManager::suggestTryTermination() {
	if(mTerminateTryEvent != 0) {
		mTerminateTryEvent->trigger();
	}
}


int FitnessManager::getNumberOfSteps() {
	if(mNumberOfSteps != 0) {
		return mNumberOfSteps->get();
	}
	return 1;
}


int FitnessManager::getNumberOfTries() {
	if(mNumberOfTries != 0) {
		return mNumberOfTries->get();
	}
	return 1;
}


/**
 * Enables or disables FitnessFunction execution.
 *
 * If disabled then the FitnessFunctions are not executed, which is useful if the FitnessManager
 * is only used to manage and organize the FitnessFunctions for an evolution application.
 * In such applications the evaluation (including fitness calculation) is done in a separate application.
 * Enabled FitnessFunctions could lead to warnings and error messages because the 
 * simulation environment and other required parts of the evaluation might not be present.
 *
 * @param enable determins whether to switch execution on or off.
 */
void FitnessManager::enableFitnessFunctionExecution(bool enable) {
	mEnableFitnessFunctionExecution = enable;
}

/**
 * Returns true if FitnessFunctions execution is enabled.
 */
bool FitnessManager::isFitnessFunctionExecutionEnabled() const {
	return mEnableFitnessFunctionExecution;
}


}
