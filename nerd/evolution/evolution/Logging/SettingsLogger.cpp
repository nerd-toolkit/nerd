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

#include "SettingsLogger.h"
#include "Value/ValueManager.h"
#include "Event/EventManager.h"
#include "EvolutionConstants.h"
#include "Core/Core.h"
#include <QVector>
#include <algorithm>
#include "Value/ValueManager.h"
#include "Value/StringValue.h"
#include <QTextStream>
#include <QDate>
#include <QTime>
#include <iostream>

using namespace std;

namespace nerd {

/**
 * The logger tries to register as global objefct automatically.
 */
SettingsLogger::SettingsLogger(bool activateStaticLogger, bool activateIncrementalLogger)
	: mEvolutionCompletedEvent(0), mCurrentGeneration(0), mWorkingDirectory(0),
	  mIncrementalLogFileEnabled(activateIncrementalLogger), mStaticLogFileEnabled(activateStaticLogger),
	  mIncrementalFileName(""), mNumberOfSteps(0), mNumberOfTrys(0), mFirstLogEntry(true)
{
	Core::getInstance()->addGlobalObject(EvolutionConstants::OBJECT_SETTINGS_LOGGER, this);

	mCommentValue = new StringValue("");
	mCommentValue->addValueChangedListener(this);

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue(EvolutionConstants::VALUE_INCREMENTAL_LOGGER_COMMENT, mCommentValue);

	mLoggerCommentHistoryUpdatedEvent = Core::getInstance()->getEventManager()
										->createEvent("LoggerCommenHistoryUpdated");
}


SettingsLogger::~SettingsLogger() {
}


bool SettingsLogger::init() {
	bool ok = true;

	return ok;
}


bool SettingsLogger::bind() {
	bool ok = true;

	ValueManager *vm = Core::getInstance()->getValueManager();

	mWorkingDirectory = vm->getStringValue(
				EvolutionConstants::VALUE_EVO_WORKING_DIRECTORY);
	mWorkingDirectory->useAsFileName(true);
	mCurrentGeneration = vm->getIntValue(
				EvolutionConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER);

	if(mWorkingDirectory == 0) {
		Core::log("SettingsLogger: Could not find the Value with the current "
				  "working directory.! [DISABLING FUNCTION] ");
	}
	if(mCurrentGeneration == 0) {
		Core::log("SettingsLogger: Could not find the Value with the current "
				  "working directory.! [DISABLING FUNCTION] ");
	}

	EventManager *em = Core::getInstance()->getEventManager();

	mEvolutionCompletedEvent = em->registerForEvent(
			EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_COMPLETED, this);

	if(mEvolutionCompletedEvent == 0) {
		Core::log(QString("SettingsLogger: Could not find the Event [")
				.append(EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_COMPLETED)
				.append("working directory.! [DISABLING FUNCTION] "));
	}

	if(mIncrementalLogFileEnabled && mWorkingDirectory != 0 && mCurrentGeneration != 0) {
		mIncrementalFileName = mWorkingDirectory->get().append("Evolution.log");
		Core::getInstance()->enforceDirectoryPath(mWorkingDirectory->get());
	}

	QList<QString> numberOfIndividualsNames = vm->getValueNamesMatchingPattern(".*/Evo/.*/PopulationSize");
	for(QListIterator<QString> i(numberOfIndividualsNames); i.hasNext();) {
		IntValue *value = vm->getIntValue(i.next());
		if(value != 0) {
			mNumberOfIndividualsValues.append(value);
		}
	}
	QList<QString> bestFitnessNames = vm->getValueNamesMatchingPattern(".*/Fitness/.*/Fitness/MaxFitness");
	for(QListIterator<QString> i(bestFitnessNames); i.hasNext();) {
		DoubleValue *value = vm->getDoubleValue(i.next());
		if(value != 0) {
			mBestFitnessValues.append(value);
		}
	}

	mNumberOfSteps = vm->getIntValue(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS);
	mNumberOfTrys = vm->getIntValue(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES);

	return ok;
}


bool SettingsLogger::cleanUp() {
	return true;
}


QString SettingsLogger::getName() const {
	return "SettingsLogger";
}


void SettingsLogger::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mEvolutionCompletedEvent) {
		if(mStaticLogFileEnabled) {
			writeSettingsLogFile();
		}
		if(mIncrementalLogFileEnabled) {
			writeIncrementalLogFile();
		}
	}
}

void SettingsLogger::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	if(value == mCommentValue && mIncrementalLogFileEnabled) {
		Core::getInstance()->scheduleTask(new LogCommentTask(this, mCommentValue->get()));
	}
}


void SettingsLogger::addValues(const QString &regularExpression) {
	if(mValuesToStore.contains(regularExpression)) {
		return;
	}
	mValuesToStore.append(regularExpression);
}


QList<QString> SettingsLogger::getValuesToStore() const {
	return mValuesToStore;
}

QList<QString> SettingsLogger::getCommentHistory() const {
	return mCommentHistory;
}

bool SettingsLogger::writeSettingsLogFile() {
	if(mEvolutionCompletedEvent == 0 || mCurrentGeneration == 0 || mWorkingDirectory == 0) {
		return false;
	}

	QString path = mWorkingDirectory->get()
			.append(EvolutionConstants::GENERATION_DIRECTORY_PREFIX)
			.append(QString::number(mCurrentGeneration->get()))
			.append("/");

	Core::getInstance()->enforceDirectoryPath(path);
	
	QString fileName = path.append(EvolutionConstants::GENERATION_SETTINGS_LOG_FILE);


	ValueManager *vm = Core::getInstance()->getValueManager();

	QList<QString> valueNames;

	for(QListIterator<QString> i(mValuesToStore); i.hasNext();) {
		valueNames << vm->getValueNamesMatchingPattern(i.next(), false);
		//TODO make unique
	}


	return vm->saveValues(fileName, valueNames, "Automated settings log of NERD evolution.");
}

bool SettingsLogger::writeIncrementalLogFile() {	

	QMap<Value*, QString> changedValues;
	ValueManager *vm = Core::getInstance()->getValueManager();

	if(mObservedValues.empty()) {
		//collect all observed values
		QList<QString> valueNames;
		for(QListIterator<QString> i(mValuesToStore); i.hasNext();) {
			valueNames << vm->getValueNamesMatchingPattern(i.next(), false);
			//TODO make unique
		}
		for(QListIterator<QString> i(valueNames); i.hasNext();) {
			QString name = i.next();
			Value *value = vm->getValue(name);
			if(mObservedValues.contains(value)) {
				continue;
			}
			mObservedValues.insert(value, name);
			changedValues.insert(value, name);
			mVariableMemory.insert(value, value->getValueAsString());
		}
	}
	else {
		QHash<Value*, QString> variableMemory = mVariableMemory;
		for(QHashIterator<Value*, QString> i(variableMemory); i.hasNext();) {
			i.next();
			if(i.key()->getValueAsString() != i.value()) {
				changedValues.insert(i.key(), mObservedValues.value(i.key()));
				mVariableMemory.insert(i.key(), i.key()->getValueAsString());
			}
		}
	}

	QFile file(mIncrementalFileName);

	if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
		Core::log(QString("Could not open file ").append(mIncrementalFileName).append(" to write incremental log."));
		file.close();
		return false;
	}

	QTextStream output(&file);

	if(mFirstLogEntry) {
		mFirstLogEntry = false;
		output << "#NERD Evolution Log (Version " << Core::getInstance()->getVersionString() 
				<< ")" << endl;
	}

	output << "#" << endl
		   << "#-------------------------------------------------------------------------------------------" << endl
		   << "#!! Generation: " << mCurrentGeneration->get() 
		   << "\t\tSize:";
	for(QListIterator<IntValue*> i(mNumberOfIndividualsValues); i.hasNext();) {
		output << i.next()->getValueAsString();
		if(i.hasNext()) {
			output << ",";
		}
	}
	output << "  Steps:" << mNumberOfSteps->getValueAsString() << "  Trys:" << mNumberOfTrys->getValueAsString();
	output  << "  Best:";
	for(QListIterator<DoubleValue*> i(mBestFitnessValues); i.hasNext();) {
		output << i.next()->getValueAsString();
		if(i.hasNext()) {
			output << ",";
		}
	}
	output << endl;
	output << "#-------------------------------------------------------------------------------------------" << endl
		   << "# Date: " << QDate::currentDate().toString("dd.MM.yyyy")
					<< "  " << QTime::currentTime().toString("hh:mm:ss") << endl;
	output << "#" << endl;

	for(QMapIterator<Value*, QString> i(changedValues); i.hasNext();) {
		i.next();
		output << i.value() << "=" << i.key()->getValueAsString() << "\n";
	}
	file.close();

	return true;
	
}

bool SettingsLogger::addCommentToFile(const QString &comment) {
	
	QFile file(mIncrementalFileName);

	if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
		Core::log(QString("Could not open file ").append(mIncrementalFileName).append(" to write incremental log."));
		file.close();
		return false;
	}

	QString correctedComment = "# " + comment;
	correctedComment.replace("\n", "\n# ");

	QString message = QString("#\n")
		   + "#**********************************************************\n"
		   + "# Comment: " + " at " + QDate::currentDate().toString("dd.MM.yyyy")
		   + "  " + QTime::currentTime().toString("hh:mm:ss") + "\n"
		   + "# \n" + correctedComment +
		   + "\n#**********************************************************\n";

	mCommentHistory.append(message);

	QTextStream output(&file);
	output << message;
// 	output << "#" << endl
// 		   << "#**********************************************************" << endl
// 		   << "#>> Comment: " << " at " << QDate::currentDate().toString("dd.MM.yyyy")
// 		   << "  " << QTime::currentTime().toString("hh:mm:ss") << endl
// 		   << "# " << endl << correctedComment << endl
// 		   << "#**********************************************************" << endl;

	mLoggerCommentHistoryUpdatedEvent->trigger();

	return true;
}


Event* SettingsLogger::getCommentHistoryUpdatedEvent() const {
	return mLoggerCommentHistoryUpdatedEvent;
}

}


