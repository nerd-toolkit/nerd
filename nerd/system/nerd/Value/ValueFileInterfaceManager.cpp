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

#include "ValueFileInterfaceManager.h"
#include "Value/ValueManager.h"
#include "Core/Core.h"
#include "Event/EventManager.h"
#include <QFile>
#include "NerdConstants.h"
#include <QDate>
#include <QTime>
#include "Value/IntValue.h"
#include "Util/FileLocker.h"

namespace nerd {

ValueFileInterfaceManager::ValueFileInterfaceManager(const QString &name, 
				const QString &ownParamPath,
				const QString &loadTriggerEventName, const QString &loadFileName,
				const QString &storeTriggerEventName, const QString &storeFileName,
				const QString &storePattern)
	: ParameterizedObject(name, ownParamPath), mLoadFileTriggerEvent(0), 
	  mStoreToFileTriggerEvent(0), mRepositoryChangedEvent(0),
	  mAutoLoadValueFile(0), mAutoStoreValueFile(0), mAutoStoreValuePattern(0),
	  mValueTypeFileName(0), mLoadTriggerEventName(loadTriggerEventName), 
	  mStoreTriggerEventName(storeTriggerEventName)
{
	mAutoLoadValueFile = new StringValue(loadFileName);
	mAutoStoreValueFile = new StringValue(storeFileName);
	mAutoStoreValuePattern = new StringValue(storePattern);
	mValueTypeFileName = new StringValue();
	mEnableManager = new BoolValue(false);

	addParameter("FileToLoad", mAutoLoadValueFile, true);
	addParameter("FileToStore", mAutoStoreValueFile, true);
	addParameter("StorePattern", mAutoStoreValuePattern, true);
	addParameter("FileToStoreTypes", mValueTypeFileName, true);
	addParameter("EnableValueInterface", mEnableManager, true);

	Core::getInstance()->addSystemObject(this);
}

ValueFileInterfaceManager::~ValueFileInterfaceManager() {
}


QString ValueFileInterfaceManager::getName() const {
	return ParameterizedObject::getName();
}


bool ValueFileInterfaceManager::init() {
	bool ok = true;

	return ok;
}


bool ValueFileInterfaceManager::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mLoadFileTriggerEvent = em->registerForEvent(mLoadTriggerEventName, this);
	mStoreToFileTriggerEvent = em->registerForEvent(mStoreTriggerEventName, this);
	
	if(mLoadFileTriggerEvent == 0) {
		Core::log(QString("ValueFileInterfaceManager [")
				  .append(getName()).append("]: Could not find trigger event [")
				  .append(mLoadTriggerEventName)
				  .append("] for automated value loading. [IGNORING]" ));
	}
	if(mStoreToFileTriggerEvent == 0) {
		Core::log(QString("ValueFileInterfaceManager [")
				  .append(getName()).append("]: Could not find trigger event [")
				  .append(mStoreTriggerEventName)
				  .append("] for automated value storing. [IGNORING]" ));
	}

	//Check whether to store the type information of the values.
	if(mValueTypeFileName->get() != "") {
		mRepositoryChangedEvent = em->registerForEvent(
				NerdConstants::EVENT_VALUE_REPOSITORY_CHANGED, this);

		if(mRepositoryChangedEvent == 0) {
			Core::log(QString("ValueFileInterfaceManager [")
				  .append(getName()).append("]: Could not find repository changed event [")
				  .append(NerdConstants::EVENT_VALUE_REPOSITORY_CHANGED)
				  .append("] to react on repository changes. [IGNORING]" ));
		}

		storeValueTypeInformation();
	}

	return ok;
}


bool ValueFileInterfaceManager::cleanUp() {
	return true;
}

void ValueFileInterfaceManager::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(!mEnableManager->get()) {
		return;
	}

	if(event == mLoadFileTriggerEvent) {

		//load all values from a given value file.
		//afterwards erase the value file.

		ValueManager *vm = Core::getInstance()->getValueManager();

		QFile file(mAutoLoadValueFile->get());
		if(file.exists()) {
			if(!vm->loadValues(mAutoLoadValueFile->get()), true) {
				Core::log(QString("ValueFileInterfaceManager [")
					.append(getName()).append("]: Could not load value file [")
					.append(mAutoLoadValueFile->get())
					.append("]. [IGNORING]" ));
			}
			file.remove();
		}
		
	}
	else if(event == mStoreToFileTriggerEvent) {

		//Save all Values of the ValueManager with the given name pattern.		

		ValueManager *vm = Core::getInstance()->getValueManager();
	
		if(!vm->saveValues(mAutoStoreValueFile->get(), vm->getValueNamesMatchingPattern(
						mAutoStoreValuePattern->get()), 
						QString("Autosave ValueManager. Pattern: [")
							.append(mAutoStoreValuePattern->get()).append("]")), 
						true)
		{
			Core::log(QString("ValueFileInterfaceManager [")
				  .append(getName()).append("]: Could not save value file [")
				  .append(mAutoStoreValueFile->get())
				  .append("]. [IGNORING]" ));
		}
	}
	else if(event == mRepositoryChangedEvent) {
		storeValueTypeInformation();
	}
}

StringValue& ValueFileInterfaceManager::getTypeFileNameValue() const {
	return *mValueTypeFileName;
}


void ValueFileInterfaceManager::storeValueTypeInformation() {

	ValueManager *vm = Core::getInstance()->getValueManager();

	QList<QString> valuesToSave = 
		vm->getValueNamesMatchingPattern(mAutoStoreValuePattern->get(), true);

	QString fileName = mValueTypeFileName->get();

	QFile file(fileName);

	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		Core::log(QString("ValueFileInterfaceManager: Could not open file ")
				.append(fileName).append(" to write value type information."));
		file.close();
		return;
	}

	if(!FileLocker::lockFileExclusively(file)) {
		Core::log(QString("ValueFileInterfaceManager: Could not lock file %1 exclusively").arg(fileName));
		file.close();
		return;
	}

	QTextStream output(&file);
	output << "#NERD Value Type Information" << "\n";
	output << "#Saved at: " << QDate::currentDate().toString("dd.MM.yyyy")
					<< "  " << QTime::currentTime().toString("hh:mm:ss") << "\n";

	for(int i = 0; i < valuesToSave.size(); ++i) {
		QString valueName = valuesToSave.at(i);
		Value *value = vm->getValue(valueName);

		if(value == 0) {
			output << "#Not found: " << valueName << "\n";
		}
		else {
			output << valueName << "=" << value->getDescription() << "=" 
				   << value->getProperties() << "\n";
		}
	}

	FileLocker::unlockFile(file);
	file.close();
}



}


