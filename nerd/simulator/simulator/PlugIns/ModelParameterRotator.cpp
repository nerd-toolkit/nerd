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



#include "ModelParameterRotator.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Event/EventManager.h"
#include "NerdConstants.h"
#include <QDir>
#include <QFile>
#include "SimulationConstants.h"
#include "Physics/Physics.h"
#include "Physics/SimulationEnvironmentManager.h"
#include "Physics/PhysicsManager.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ModelParameterRotator.
 */
ModelParameterRotator::ModelParameterRotator()
	: SystemObject(), EventListener(), mTriggerEvent(0), mCurrentTrial(0)
{
	mEnableRotation = new BoolValue(false);
	mParameterDirectory = new StringValue(Core::getInstance()->getConfigDirectoryPath() + "/"
											+ "modelParameters");
	mCurrentSet = new StringValue();
	mReloadParameterSets = new BoolValue(true);

	ValueManager *vm = Core::getInstance()->getValueManager();

	vm->addValue("/Control/ParameterRotation/Enable", mEnableRotation);
	vm->addValue("/Control/ParameterRotation/ParameterDirectory", mParameterDirectory);
	vm->addValue("/Control/ParameterRotation/CurrentSet", mCurrentSet);
	vm->addValue("/Control/ParameterRotation/ReloadSets", mReloadParameterSets);

	if(!Core::getInstance()->isUsingReducedFileWriting()) {
		Core::getInstance()->enforceDirectoryPath(mParameterDirectory->get());
	}

	Core::getInstance()->addSystemObject(this);
}




/**
 * Destructor.
 */
ModelParameterRotator::~ModelParameterRotator() {
}

QString ModelParameterRotator::getName() const {
	return "ModelParameterRotator";
}


bool ModelParameterRotator::init() {
	bool ok = true;

	return ok;
}


bool ModelParameterRotator::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();
	mTriggerEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_NEXT_TRY, this);

	ValueManager *vm = Core::getInstance()->getValueManager();
	mCurrentTrial = vm->getIntValue(SimulationConstants::VALUE_EXECUTION_CURRENT_TRY);

	//if no trigger event or trial value was found, then just ignore motor model rotation (no error).

	return ok;
}


bool ModelParameterRotator::cleanUp() {
	return true;
}


void ModelParameterRotator::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mTriggerEvent && mEnableRotation->get() && mCurrentTrial != 0) {
		if(mReloadParameterSets->get()) {
			reloadParameterSets();
		}
		applyParameterSet(mCurrentTrial->get());
	}
}

void ModelParameterRotator::reloadParameterSets() {
	while(!mParameterSets.empty()) {
		delete mParameterSets.takeFirst();
	}
	mParameterSetNames.clear();
	mParameterSets.clear();

	mReloadParameterSets->set(false);

	QDir dir(mParameterDirectory->get());
	if(!dir.exists()) {
		Core::log(QString("ModelParameterRotator: Could not find directory [")
					.append(mParameterDirectory->get())
					.append("] to load model parameters."), true);
		return;
	}
	QStringList files = dir.entryList();
	for(QListIterator<QString> i(files); i.hasNext();) {
		QString file = i.next();
		if(file.endsWith(".val")) {
			Properties *props = new Properties();
			if(props->loadFromFile(dir.absoluteFilePath(file)) == true) {
				mParameterSets.append(props);
				mParameterSetNames.append(file);
			}
		}
	}
}

void ModelParameterRotator::applyParameterSet(int index) {
	if(mParameterSets.empty()) {
		mCurrentSet->set("");
		return;
	}

	int setIndex = index % mParameterSets.size();

	Properties *props = mParameterSets.at(setIndex);
	QString setName = mParameterSetNames.at(setIndex);

	ValueManager *vm = Core::getInstance()->getValueManager();
	SimulationEnvironmentManager *sem = Physics::getSimulationEnvironmentManager();

	QList<QString> propNames = props->getPropertyNames();
	for(QListIterator<QString> i(propNames); i.hasNext();) {
		QString targetName = i.next();
		QString newValue = props->getProperty(targetName);
		
		Value *targetValue = vm->getValue(targetName);

		if(targetValue == 0) {
			//TODO add a warning.
			continue;
		}

		if(targetName.startsWith("/Sim/")) {
			
			PhysicsManager *pm = Physics::getPhysicsManager();
	
			const QList<SimObject*> &simObjects = pm->getSimObjects();
			for(QListIterator<SimObject*> i(simObjects); i.hasNext();) {
				SimObject *obj = i.next();
			
				const QList<Value*> &params = obj->getParameters();

				bool parameterFound = false;

				for(QListIterator<Value*> j(params); j.hasNext();) {
					Value *value = j.next();
					if(value == targetValue) {
						
						//get parameter name
						QList<QString> parameterNames = obj->getParameterNames();
						for(QListIterator<QString> k(parameterNames); k.hasNext();) {
							QString name = k.next();
							if(obj->getParameter(name) == targetValue) {
								sem->storeParameter(obj, name, newValue);
								parameterFound = true;
								break;
							}
						}
					}
					if(parameterFound) {
						break;
					}
				}
			}
		}
		targetValue->setValueFromString(newValue);
	}

	mCurrentSet->set(QString("[") + QString::number(setIndex) + "] " + setName);
}


}



