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

#include "SimpleFitnessLogger.h"
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
#include "NerdConstants.h"
#include <Fitness/FitnessManager.h>
#include <Fitness/Fitness.h>
#include "Value/BoolValue.h"
#include <qdir.h>

using namespace std;

namespace nerd {

/**
 * The logger tries to register as global objefct automatically.
 */
SimpleFitnessLogger::SimpleFitnessLogger(const QString &fileName)
	: mFileName(fileName), mCurrentTry(0)
{
	Core::getInstance()->addGlobalObject(EvolutionConstants::OBJECT_SIMPLE_FITNESS_LOGGER, this);
	
	if(!mFileName.endsWith(".txt") && !mFileName.endsWith(".log")) {
		mFileName.append(".txt");
	}
	
	mLogFile.setFileName(mFileName);
	if(!mLogFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		Core::log(QString("SimpleFitnessLogger: Could not open logfile file ")
							.append(mFileName), true);
		mLogFile.close();
	}
	mWriter.setDevice(&mLogFile);
	mWriter << "Simple Fitness Logger" << endl << endl;
	QDir dir(".");
	mWriter << "Path: " << dir.absolutePath() << endl << endl;
}


SimpleFitnessLogger::~SimpleFitnessLogger() {
}


bool SimpleFitnessLogger::init() {
	bool ok = true;

	return ok;
}


bool SimpleFitnessLogger::bind() {
	bool ok = true;
	
	//HACK MODE remove
	if(NerdConstants::HackMode) {
		BoolValue *p = Core::getInstance()->getValueManager()->getBoolValue(NerdConstants::VALUE_EXECUTION_PAUSE);
		if(p != 0) {
			//p->set(true);
		}
	}

	EventManager *em = Core::getInstance()->getEventManager();

	mResetEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_RESET, this);
	mTryCompletedEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_TRY_COMPLETED, this);
	mTryStartedEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_NEXT_TRY, this);
	
	if(mResetEvent == 0 || mTryCompletedEvent == 0 || mTryStartedEvent == 0) {
		Core::log("SimpleFitnessLogger: Could not find all events. Functionality is limited!");
	}
	
	return ok;
}


bool SimpleFitnessLogger::cleanUp() {
	
	if(mLogFile.isOpen()) {
		mLogFile.close();
	}
	
	return true;
}


QString SimpleFitnessLogger::getName() const {
	return "SimpleFitnessLogger";
}


void SimpleFitnessLogger::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(mLogFile.isOpen()) {
		FitnessManager *fm = Fitness::getFitnessManager();
		
		QList<FitnessFunction*> fitnessFunctions = fm->getFitnessFunctions();
	
// 		if(event == mResetEvent) {
// 			mWriter << "Reset: " << endl;
// 			for(QListIterator<FitnessFunction*> i(fitnessFunctions); i.hasNext();) {
// 				FitnessFunction *ff = i.next();
// 				mWriter << "Name: " << ff->getName() << endl;
// 				mWriter << "Fitness:        " << ff->getFitness() << endl;
// 				mWriter << "Current Fitness " << ff->getCurrentFitness() << endl;
// 				mWriter << "Mean Fitness    " << ff->getMeanFitnessValue()->get() << endl;
// 				mWriter << "Max Fitness     " << ff->getMaxFitnessValue()->get() << endl;
// 				mWriter << "Min Fitness     " << ff->getMinFitnessValue()->get() << endl;
// 				mWriter << endl;
// 			}
// 		}
		if(event == mTryStartedEvent) {
			if(mCurrentTry == 0) {
				mCurrentTry++;
			}
			else {
				mWriter << "Next Step: " << mCurrentTry << endl;
				mCurrentTry++;
				for(QListIterator<FitnessFunction*> i(fitnessFunctions); i.hasNext();) {
					FitnessFunction *ff = i.next();
					mWriter << "Name: " << ff->getName() << endl;
					mWriter << "Current Fitness: " << ff->getCurrentFitness() << endl;
					mWriter << "Overall Fitness: " << ff->getFitness() << endl;
					mWriter << endl;
				}
			}
		}
// 		else if(event == mTryCompletedEvent) {
// 			mWriter << "TryCompleted: " << endl;
// 			for(QListIterator<FitnessFunction*> i(fitnessFunctions); i.hasNext();) {
// 				FitnessFunction *ff = i.next();
// 				mWriter << "Name: " << ff->getName() << endl;
// 				mWriter << "Fitness:        " << ff->getFitness() << endl;
// 				mWriter << "Current Fitness " << ff->getCurrentFitness() << endl;
// 				mWriter << "Mean Fitness    " << ff->getMeanFitnessValue()->get() << endl;
// 				mWriter << "Max Fitness     " << ff->getMaxFitnessValue()->get() << endl;
// 				mWriter << "Min Fitness     " << ff->getMinFitnessValue()->get() << endl;
// 				mWriter << endl;
// 			}
// 		}
	}
}





}


