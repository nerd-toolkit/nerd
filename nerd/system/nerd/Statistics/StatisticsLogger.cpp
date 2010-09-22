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

#include "StatisticsLogger.h"
#include "Statistics/Statistics.h"
#include "Statistics/StatisticCalculator.h"
#include "Statistics/StatisticsManager.h"
#include "Core/Core.h"
#include "NerdConstants.h"
#include <QTextStream>
#include <iostream>

#include <QDir>

using namespace std;

namespace nerd {


StatisticsLogger::StatisticsLogger(const QString &loggerPath) 
	: mLoggerPath(""),
		mLoggerPathValue(0),
		mFileName("Statistics.log"),
		mLogEventName(NerdConstants::EVENT_GENERATION_STATISTICS_CALCULATED),
		mLogEvent(0),
		mLoggingEnabled(false), 
		mFileSetupCompleted(false),
		mStatisticsStepEvent("")
{
	setLoggerPath(loggerPath);

	Core::getInstance()->addSystemObject(this);
} 

StatisticsLogger::StatisticsLogger(StringValue *loggerPathValue)	
	: mLoggerPath(""),
		mLoggerPathValue(loggerPathValue),
		mFileName("Statistics.log"),
		mLogEventName(NerdConstants::EVENT_GENERATION_STATISTICS_CALCULATED),
		mLogEvent(0),
		mLoggingEnabled(false), 
		mFileSetupCompleted(false),
		mStatisticsStepEvent("")
{
	if(mLoggerPathValue != 0) {
		setLoggerPath(loggerPathValue->get());
		mLoggerPathValue->addValueChangedListener(this);
	}

	Core::getInstance()->addSystemObject(this);
} 

StatisticsLogger::StatisticsLogger(const QString &loggerPath,
                                   const QString &fileName,
                                   const QString &logEventName,
                                   const QString &statisticsStepEvent)
  : mLoggerPath(""),
    mFileName(fileName),
    mLogEventName(logEventName),
    mLogEvent(0),
    mLoggingEnabled(false), 
    mFileSetupCompleted(false),
    mStatisticsStepEvent(statisticsStepEvent)
{
	setLoggerPath(loggerPath);

	Core::getInstance()->addSystemObject(this);
}


StatisticsLogger::~StatisticsLogger() {
}


QString StatisticsLogger::getName() const {
	return "StatisticsLogger";
}


bool StatisticsLogger::init() {
	return true;
}


bool StatisticsLogger::bind() 
{
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mLogEvent = em->registerForEvent(mLogEventName, this);

	if(mLogEvent == 0) {
		Core::log(QString("StatisticsLogger: Could not register for Event [")
			.append(mLogEventName)
			.append("]! [FUNCTION DISABLED]"));
	}

	return ok;
}


bool StatisticsLogger::cleanUp() {
	return true;
}

void StatisticsLogger::setLoggerPath(const QString &loggerPath)
{
	if(loggerPath.endsWith(QDir::separator())) {
		mLoggerPath = loggerPath;
	}
	else {
		mLoggerPath = loggerPath + QDir::separator();
	}

	mFileSetupCompleted = false;
	mLoggingEnabled = false;
}


QString StatisticsLogger::getLoggerPath()
{
	return mLoggerPath;
}


void StatisticsLogger::setFileName(const QString &fileName)
{
	mFileName = fileName;
	mFileSetupCompleted = false;
	mLoggingEnabled = false;
}


QString StatisticsLogger::getFileName()
{
	return mFileName;
}


void StatisticsLogger::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mLogEvent) {
		saveStatistic();
	}
}

void StatisticsLogger::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mLoggerPathValue) {
		setLoggerPath(mLoggerPathValue->get());
		mFileSetupCompleted = false;
	}
}

bool StatisticsLogger::storeGenerationStatistics() {
	return saveStatistic();
}

bool StatisticsLogger::saveStatistic() {

	StatisticsManager *sm = Statistics::getStatisticsManager();

	if(sm == 0)  {
		return false;
	}

	QList<StatisticCalculator*> calculators;
	if(mStatisticsStepEvent.isNull() || mStatisticsStepEvent.isEmpty())	{ 
		// use standard
		calculators = sm->getGenerationStatistics();
	}
	else { 
		// use defined
		calculators = sm->getStatistics(mStatisticsStepEvent);
	}

	//At the first time try to create the logger file.
	if(!mFileSetupCompleted) {
		mFileSetupCompleted = true;

		Core::getInstance()->enforceDirectoryPath(mLoggerPath);
		QString fileName = QString(mLoggerPath).append(mFileName);

		QFile file(fileName);
		if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			Core::log(QString("StatisticsLogger: Could not open file [")
				.append(fileName).append("] to write statistics."));
			file.close();
			return false;
		}

		QTextStream output(&file);

		for(QListIterator<StatisticCalculator*> i(calculators); i.hasNext();) {
			StatisticCalculator *calc = i.next();
			output << "# " << calc->getName() << endl;
		}

		file.close();

		mLoggingEnabled = true;

	}

	//if a logger file was successfully created, start loggin. Otherwise stop.
	if(!mLoggingEnabled) {
		return false;
	}

	QString fileName = QString(mLoggerPath).append(mFileName);

	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
		Core::log(QString("StatisticsLogger: ERROR - Could not open file [")
			.append(fileName).append("] to write statistics!"));
		file.close();
		return false;
	}

	QTextStream output(&file);

	for(int i = 0; i < calculators.size(); ++i) {
		output << QString::number(calculators.at(i)->getStatistics().last(), 'g', 15);

		if(i < calculators.size() - 1) {
			output << ";";
		}
	}

	output << "\n";

	file.close();

	return true;
}


}


