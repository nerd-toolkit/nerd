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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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

#include "MultiCoreEvaluationRunner.h"
#include <QCoreApplication>
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "NerdConstants.h"
#include <iostream>
#include "Value/ValueManager.h"
#include <QCoreApplication>
#include "PlugIns/CommandLineArgument.h"
#include <Command/Command.h>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QThread>
#include "Math/Math.h"

using namespace std;

namespace nerd {
	
	MultiCoreEvaluationRunner::MultiCoreEvaluationRunner()
	: mDoShutDown(false), mShutDownEvent(0), mMessageValue(0), mMinIndex(-1), mMaxIndex(-1)
	{
		connect(this, SIGNAL(quitMainApplication()),
				QCoreApplication::instance(), SLOT(quit()));
		
		mMessageValue = new StringValue("");
		mMessageValue->setNotifyAllSetAttempts(true);
		Core::getInstance()->getValueManager()->addValue(
			"/Evaluation/Messages", mMessageValue);
		
		mNumberOfProcesses = new IntValue(4);
		Core::getInstance()->getValueManager()->addValue(
			"/Evaluation/NumberOfProcesses", mNumberOfProcesses);
		
		
		CommandLineArgument *jobFileNameArg = new CommandLineArgument("scriptFile", "scriptFile", 
									 "<absoluteFileName>", "The name of the main evaluation script",
									 1, 0, true, true);
		
		if(jobFileNameArg->getEntries().size() > 0) {
			QStringList jobScriptName = jobFileNameArg->getEntryParameters(0);
			if(jobScriptName.size() > 0) {
				mJobFile = jobScriptName.at(0);
			}
		}
		
		CommandLineArgument *indicesArg = new CommandLineArgument("index", "index",
									"<minIndex> [<maxIndex]", "The working directory indices to evaluate.\n"
									"If maxIndex is ommited, then only a single evaluation takes place.",
									1, 1, true, true);
		
		if(indicesArg->getEntries().size() > 0) {
			QStringList indicesList = indicesArg->getEntryParameters(0);
			if(indicesList.size() > 0) {
				bool ok = true;
				int index = indicesList.at(0).toInt(&ok);
				if(ok && index >= 0) {
					mMinIndex = index;
				}
			}
			if(indicesList.size() > 1) {
				bool ok = true;
				int index = indicesList.at(1).toInt(&ok);
				if(ok && index >= 0 && index >= mMinIndex) {
					mMaxIndex = index;
				}
			}
			else {
				mMaxIndex = mMinIndex;
			}
		}
		
		
		Core::getInstance()->addSystemObject(this);
	}
	
	MultiCoreEvaluationRunner::~MultiCoreEvaluationRunner() {	
		Core::getInstance()->deregisterThread(this);
	}
	
	
	bool MultiCoreEvaluationRunner::init() {
		bool ok = true;
		
		Core::getInstance()->registerThread(this);
		
		
		
		
		if(mMinIndex < 0 || mMaxIndex < 0 || mMinIndex > mMaxIndex) {
			Core::log("MultiCoreEvaluationRunner: Indices for evaluations have not been valid: min="
						+ QString::number(mMinIndex) + " max=" + QString::number(mMaxIndex) + "!", true);
			return false;
		}
		
		QFile jobFile(mJobFile);
		if(!jobFile.exists()) {
			Core::log("MultiCoreEvaluationRunner: Could not access job file [" + mJobFile + "]!", true);
			return false;
		}
		
		
		Core::log("MultiCoreEvaluationRunner: FileScript: " + mJobFile + " MinIndex: " 
					+ QString::number(mMinIndex) + " MaxIndex: " + QString::number(mMaxIndex), true);
		
		
		
		
		return ok;
	}
	
	
	bool MultiCoreEvaluationRunner::bind() {
		bool ok = true;
		
		EventManager *em = Core::getInstance()->getEventManager();
		
		mShutDownEvent = em->registerForEvent(NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this);
		
		if(mShutDownEvent == 0) 
		{
			Core::log("SimulationLoop: Required Events could not be found.");
			ok = false;
		}
		
		return ok;
	}
	
	
	bool MultiCoreEvaluationRunner::cleanUp() {
		bool ok = true;
		
		return ok;
	}
	
	
	void MultiCoreEvaluationRunner::eventOccured(Event *event) {
		if(event == 0) {
			return;
		}
		else if(event == mShutDownEvent) {
			mDoShutDown = true;
		}
	}
	
	
	
	
	QString MultiCoreEvaluationRunner::getName() const {
		return "MultiCoreEvaluationRunner";
	}
	
	void MultiCoreEvaluationRunner::run() {
		//set the running thread as main execution thread.
		Core::getInstance()->setMainExecutionThread();
		
		//load config file
		ValueManager *vm = Core::getInstance()->getValueManager();
		vm->loadValues("evalConf.val");
		vm->saveValues("evalConf.val", vm->getValueNamesMatchingPattern(".*/NumberOfProcesses"), "");
		
		
		//execute evaluations
		int numberOfProcesses = Math::min(16, mNumberOfProcesses->get());
		
		int currentIndex = mMinIndex;
		
		while((!mActiveProcesses.empty() || (mMinIndex <= mMaxIndex)) && !mDoShutDown) {
			
			while(mActiveProcesses.size() < numberOfProcesses && currentIndex <= mMaxIndex) {
				//create a new process.
				
				Core::log("Evaluating index: " + QString::number(currentIndex), true);
				
				QStringList parameter;
				parameter << mJobFile;
				parameter << QString::number(currentIndex);
				currentIndex++;
				
				QProcess *proc = new QProcess();
				proc->start("/bin/bash", parameter);
				
				mActiveProcesses.append(proc);
				
				//Core::log("size: " + QString::number(mActiveProcesses.size()) + " at " + QString::number(currentIndex) + " of max " + QString::number(mMaxIndex) + " procs: " + QString::number(numberOfProcesses), true);
			}

			if(mActiveProcesses.empty()) {
				break;
			}
			
			while(true) {
				Core::getInstance()->executePendingTasks();
				
				QList<QProcess*> finishedProcesses;
				
				for(int i =  0; i < mActiveProcesses.size(); ++i) {
					QProcess *proc = mActiveProcesses.at(i);
					if(proc->waitForFinished(100)) {
						finishedProcesses.append(proc);
					}
					Core::getInstance()->executePendingTasks();
				}

				if(!finishedProcesses.empty()) {
					while(!finishedProcesses.empty()) {
						QProcess *proc = finishedProcesses.at(0);
						finishedProcesses.removeAll(proc);
						mActiveProcesses.removeAll(proc);
						
						if(proc->exitCode() != 0) {
							Core::log("MultiCoreEvaluationRunner: Problem with evaluation process. "
									  "Exit code was " + QString::number(proc->exitCode()), true);
						}
						else {
							Core::log("MultiCoreEvaluationRunner: An evaluation was finished...", true);
						}
						delete proc;
					}
					break;
				}
			}

		}
		
		Core::log("Multi-core evaluation was completed", true);
		
		if(!mDoShutDown) {
			if(Core::getInstance()->scheduleTask(new ShutDownTask())) {
			}
			Core::getInstance()->executePendingTasks();
		}
		while(!mDoShutDown) {
			Core::getInstance()->executePendingTasks();
		}
		
		Core::getInstance()->clearMainExecutionThread();
		
		//emit quitMainApplication();
	}
	
	
	StringValue* MultiCoreEvaluationRunner::getMessageValue() const {
		return mMessageValue;
	}
	
}


