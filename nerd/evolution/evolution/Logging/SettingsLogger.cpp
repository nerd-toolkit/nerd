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

namespace nerd {

/**
 * The logger tries to register as global objefct automatically.
 */
SettingsLogger::SettingsLogger()
	: mEvolutionCompletedEvent(0), mCurrentGeneration(0), mWorkingDirectory(0)
{
	Core::getInstance()->addGlobalObject(EvolutionConstants::OBJECT_SETTINGS_LOGGER, this);
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
		writeSettingsLogFile();
	}
}



void SettingsLogger::addValues(const QString &regularExpression) {
	if(mValuesToStore.contains(regularExpression)) {
		return;
	}
	mValuesToStore.append(regularExpression);
}


const QList<QString>& SettingsLogger::getValuesToStore() const {
	return mValuesToStore;
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


}


