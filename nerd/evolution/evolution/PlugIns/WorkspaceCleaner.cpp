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



#include "WorkspaceCleaner.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "Evolution/Evolution.h"
#include <QProcess>
#include "Value/ValueManager.h"
#include <QTime>


using namespace std;

namespace nerd {


/**
 * Constructs a new WorkspaceCleaner.
 */
WorkspaceCleaner::WorkspaceCleaner(const QString &triggerEventName)
	: mTriggerEventName(triggerEventName)
{
	mCleanUpTime = new IntValue(0);

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/ExecutionTime/EvoDirectoryCleanUp", mCleanUpTime);

	Core::getInstance()->addSystemObject(this);
}


/**
 * Destructor.
 */
WorkspaceCleaner::~WorkspaceCleaner() {
}


QString WorkspaceCleaner::getName() const {
	return "WorkspaceCleaner";
}

bool WorkspaceCleaner::init() {
	return true;
}


bool WorkspaceCleaner::bind() {

	EventManager *em = Core::getInstance()->getEventManager();

	mTriggerEvent = em->registerForEvent(mTriggerEventName, this);

	if(mTriggerEvent == 0) {
		Core::log(QString("WorkspaceCleaner: Could not find trigger Event [") + mTriggerEventName + "]");
	}

	return true;
}


bool WorkspaceCleaner::cleanUp() {
	return true;
}


void WorkspaceCleaner::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(event == mTriggerEvent) {
		cleanUpWorkspace();
	}
}


void WorkspaceCleaner::cleanUpWorkspace() {

	QTime time;

	bool measurePerformance = Core::getInstance()->isPerformanceMeasuringEnabled();

	if(measurePerformance) {
		time.start();
	}

	EvolutionManager *em = Evolution::getEvolutionManager();

	QString workingDirectory = em->getEvolutionWorkingDirectory();
	int currentGeneration = em->getCurrentGenerationValue()->get();

	int stepSizeIncrement = 2;
	int stepSize = stepSizeIncrement;

	//zip previous directory.
	{
		if(currentGeneration > 1) {
			Core::log("Trying to zip " + QString::number(currentGeneration - 1), true);
			QString currentFileName = "gen" + QString::number(currentGeneration - 1);
			QProcess zipProc;
			zipProc.setWorkingDirectory(workingDirectory);
			QStringList args;
			args << "-rm" << currentFileName + ".zip" << QString("./") + currentFileName;
			zipProc.start("zip", args);
			zipProc.waitForFinished(600000);
		}
	}


	int startGeneration = currentGeneration - (currentGeneration % stepSizeIncrement) - 1;
	for(int i = startGeneration; i > 0; --i) {
		//keep generation in certain intervals	
		if(i == 1) {
			continue;
		}
		if(i % stepSize == 0) {
			stepSize *= 2;
			continue;
		}
		QString dirName = "gen" + QString::number(i);
		QString fileName = QString(workingDirectory) + dirName + ".zip";
// 		QString fileName = dirName + "/eval";

		//deleteFilesRecursively(fileName);
		//removeFilesFromZipRecursively(fileName, "eval");

		//rename directory to make it easier to find the complete directories.
		QFile file(fileName);
		if(file.exists()) {

			//remove all eval files from the zip file.
			QProcess delFromZipProc;
			delFromZipProc.setWorkingDirectory(workingDirectory);
			QStringList args;
			args << "-d" << fileName << (dirName + "/eval*");
			delFromZipProc.start("zip", args);
			delFromZipProc.waitForFinished(600000);

			QString newFileName = "_gen" + QString::number(i) + ".zip";
			file.rename(QString(workingDirectory) + newFileName);
// 			QProcess zipProc;
// 			zipProc.setWorkingDirectory(workingDirectory);
// 			QStringList args;
// 			args << "-rm" << newFileName + ".zip" << QString("./") + newFileName;
// 			zipProc.start("zip", args);
// 			zipProc.waitForFinished();
		}
	}

	if(measurePerformance) {
		mCleanUpTime->set(time.elapsed());
	}
	else {
		mCleanUpTime->set(0);
	}
}

void WorkspaceCleaner::deleteFilesRecursively(const QString &fileName) {

	if(fileName.endsWith(".")) {
		return;
	}

	QDir dir(fileName);

	if(dir.exists()) {
		dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::Dirs);

		QStringList fileNames = dir.entryList();

		for(QListIterator<QString> i(fileNames); i.hasNext();) {
			deleteFilesRecursively(dir.absolutePath() + QDir::separator() + i.next());
		}

		dir.cdUp();
		if(dir.exists()) {
			dir.rmdir(fileName);
		}
	}
	else {
		QFile file(fileName);
		if(file.exists()) {
			file.remove();
		}
	}
}

// void WorkspaceCleaner::removeFilesFromZipRecursively(const QString &fileName, const QString removedDir) {
// 	
// 	
// }

}



