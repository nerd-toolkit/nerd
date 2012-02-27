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

#include "FitnessLogger.h"
#include "Core/Core.h"
#include "Value/StringValue.h"
#include "PlugIns/CommandLineParser.h"
#include "PlugIns/PlugInManager.h"
#include "PlugIns/CommandLineArgument.h"

#include "EvolutionConstants.h"
#include "Fitness/Fitness.h"
#include "Fitness/FitnessManager.h"
#include <QCoreApplication>
#include <qfile.h>
#include <qdir.h>

namespace nerd {

FitnessLogger::FitnessLogger() : ParameterizedObject("FitnessLogger"), mFileName(0),
		mLoggerArgumentDescription(0)
{
	Core::getInstance()->addSystemObject(this);
	mMaxRetries = 10;
	mFileName = new StringValue("");
	mFileName->useAsFileName(true);
	mIsLogging = new BoolValue(false);
	addParameter("/FitnessLogger/FileName", mFileName, true);
	addParameter("/FitnessLogger/IsLogging", mIsLogging, true);
	mLoggerArgumentDescription = new CommandLineArgument(
		"logFitness", "logFit", "<FileName>",
		"Saves the fitness results of each individual.",
		1, 0, true);
}

FitnessLogger::~FitnessLogger() {
}

bool FitnessLogger::init() {
	if(mLoggerArgumentDescription->getParameterValue()->get() != "") {
		QStringList fileNames = mLoggerArgumentDescription->getEntries();
		for(int i = 0; i < fileNames.size(); ++i) {
			mFileName->set(fileNames.at(i));
		}
		mIsLogging->set(true);
	}

	return true;
}

void FitnessLogger::eventOccured(Event *event) {
	if(event == mIndividualCompletedEvent && mIsLogging->get()) {
		saveFitnessResults();
	}
}

bool FitnessLogger::bind() {	
	mIndividualCompletedEvent = Core::getInstance()->getEventManager()->getEvent(
		EvolutionConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED);
	if(mIndividualCompletedEvent == 0) {
		Core::log("FitnessLogger: Could not find required Event.");
		return false;
	}
	mIndividualCompletedEvent->addEventListener(this);
	return true;
}


bool FitnessLogger::cleanUp() {
	return true;
}

void FitnessLogger::saveFitnessResults() {
	if(Fitness::getFitnessManager() != 0) {	
		int nrOfRetries = 0;
		while(!Fitness::getFitnessManager()->writeFitnessValuesToFile(mFileName->get()) 
			&& nrOfRetries < mMaxRetries) 
		{
			QCoreApplication::instance()->thread()->wait(250);
			nrOfRetries++;
		}
	}
	else {
		Core::log("EvaluationLoop: Could not find the FitnessManager.");
	}
}

QString FitnessLogger::getName() const {
	return "Fitness/Logger";
}

}
