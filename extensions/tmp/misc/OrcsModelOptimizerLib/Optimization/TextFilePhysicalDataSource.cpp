/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   OrcsModelOptimizer by Chris Reinke (creinke@uni-osnabrueck.de         *
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


#include "TextFilePhysicalDataSource.h"

#include "OrcsModelOptimizerConstants.h"

#include <QString>
#include <QDomElement>
#include <QFile>

#include "Core/Core.h"
#include "Value/StringValue.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "Tools.h"

#include "Trigger.h"
#include "Normalization.h"
#include "XMLConfigDocument.h"

namespace nerd 
{

TextFilePhysicalDataSource::TextFilePhysicalDataSource(	Trigger *trigger,
														Normalization *triggerNormalization,
														const QVector<StringValue*> &trainDataValueNames,
														const QVector<Normalization*> &trainDataValueNormalizations,
														const QString &configDocumentPath, 
														const QString &dataFilePath)
	: PhysicalDataSource(	trigger,
 							triggerNormalization,
 							trainDataValueNames,
 							trainDataValueNormalizations,
 							configDocumentPath, 
 							dataFilePath),
		mDataFile(0), mTextStream(0), mFrameTimeLength(0) {
}

TextFilePhysicalDataSource::~TextFilePhysicalDataSource() {
	for(int i = 0; i < mData.size(); i++){
		delete mData[i];
	}
}

QString TextFilePhysicalDataSource::getName() const{
	return "TextFilePhysicalDataSource";
}

bool TextFilePhysicalDataSource::init()
{
	// initialize the base first
	//   --> needed for further initialization 
	bool ok = PhysicalDataSource::init();

	// base has to be initialized correctly 
	if(!ok) {
		return false;
	}

	///////////////////////////////////////////////
	// Read needed values from configuration file
	///////////////////////////////////////////////

	QString dataFormatElemStr = OrcsModelOptimizerConstants::PDS_DESCR_BASE_ELEMENT +
		XMLConfigDocument::SEPARATOR +
		OrcsModelOptimizerConstants::PDS_DESCR_DATAFORMAT_ELEMENT;

	// read  frameTimeLength to calculate mMotionLengthInFrames
	QString frameTimeLengthStr = mConfigDocument->readConfigParameter(
		dataFormatElemStr,
		OrcsModelOptimizerConstants::PDS_DESCR_FRAMETIMELENGTH_VALUE);

	mFrameTimeLength = frameTimeLengthStr.toUInt(&ok);

	if(ok == false){
    	Core::log(QString("TextFilePhysicalDataSource: ERROR - In the configuration file [%1] no value is defined for the parameter [%2] or the parameter is in a wrong format!").arg(
			mConfigDocument->getFileName(), 
			dataFormatElemStr.replace(XMLConfigDocument::SEPARATOR, "/") + "/" +
			OrcsModelOptimizerConstants::PDS_DESCR_FRAMETIMELENGTH_VALUE));
	}

	///////////////////////////////////////////////
	// Open Datafile and DataStream
	///////////////////////////////////////////////

	mDataFile = new QFile(mDataFilePath);

	if(!mDataFile->exists()){
		Core::log(QString("TextFilePhysicalDataSource: Data file [%1] does not exist!").arg(mDataFilePath));
		return false;
	}

	if(!mDataFile->open(QIODevice::ReadOnly)) {
    	Core::log(QString("TextFilePhysicalDataSource: Could not open data file [%1]! Maybe no read rights?").arg(mDataFilePath));
    	return false;
	}

	mTextStream = new QTextStream(mDataFile);

	return ok;
}

double TextFilePhysicalDataSource::getData(int channel, int simulationStep)
{ 
	// 1) calculate the time point of the wanted data in milliseconds 
	double simulationStepInMsec = Tools::convertSimStepsToMsec(simulationStep);

	// 2) calculate the point of the wanted data in the mData vector
	int dataStep = simulationStepInMsec / mFrameTimeLength;

	return mData.at(channel)->at(dataStep);
}

int TextFilePhysicalDataSource::readData()
{
	return readDataFrames(-1);
}

bool TextFilePhysicalDataSource::readData(int motionLengthInSimSteps)
{
	bool ok = true;

	if(readDataFrames(motionLengthInSimSteps) < 0) {
		return false;
	}

	return ok;
}

/**
 * Reads the data from the text file into the mData. 
 * Checks the Trigger, thus only data after the trigger occurs are written into the mData.
 * Normalizes the data.
 * Reads data till mMotionLengthInFrames
 * 
 * @return TRUE if successfully, otherwise FALSE.
 */
int TextFilePhysicalDataSource::readDataFrames(int motionLengthInSimSteps) {

	int motionLengthInFrames = -1;

	if(motionLengthInSimSteps >= 0) {
		motionLengthInFrames = Tools::convertSimStepsToMsec(motionLengthInSimSteps) / mFrameTimeLength;

		if(motionLengthInFrames < 0) {
			return -1;
		}
	}

	PositionDoubleValueMap posValMap;
	QVector<int> posVector;

	// get the position of the trigger value in the data file
	
	int triggerPos = -1;
	
	if(mTrigger != 0)
	{
		triggerPos = this->getValuePosition(mTrigger->getTriggerValueName());
		
		if(triggerPos < 0){
			return -1;
		}
	
		posValMap.insert(triggerPos, 0.0);
	}

	// iterate over values which should be loaded and create the map to store their values for
	// each frame
	for(int i = 0; i < mTrainDataValueNames.count(); i++) {

		// get the position in the datafile for the value from the config file
		int valuePos = this->getValuePosition(mTrainDataValueNames[i]->get());

		if(valuePos < 0) {
			return -1;
		}

		// store the position in a vector with the same sorting like the mTrainDataValueNames 
		posVector.push_back(valuePos);

		// store the position in a sorted map which will be filled by the readNextFrame() method
		// with the values for one frame
		posValMap.insert(valuePos, 0.0);

		// create data vector which holds all the data
		if(motionLengthInFrames > 0) {
			mData.push_back(new QVector<double>(motionLengthInFrames));
		} else {
			mData.push_back(new QVector<double>());
		}
	}

	// iterate through frames in the data file and store the needed values
	bool isTriggered = false;
	int framesSinceTriggered = 0;
	int frameCount = 1; // frame count is used for error messages
	error lastReadError = NOERROR;

	while(!mTextStream->atEnd() &&  // check if the data ends
			(motionLengthInFrames < 0 ||  // check if enough frames are read or if all frames should 
			framesSinceTriggered < motionLengthInFrames)) { // be read (mMotionLengthInFrames == -1)

		lastReadError = readNextFrame(&posValMap);

		// stop if the frame is incomplete, thus not all data from the frame could be
		// read
		if(lastReadError == FRAMEINCOMPLETE){
			break;
		}

		if(lastReadError == READERROR) {
			Core::log(QString("TextFilePhysicalDataSource: ERROR - An error occured while reading the %1th frame of the data file [%2]!").arg(QString::number(frameCount), mDataFilePath)); 
			return -1;
		}

		// check if trigger was already fullfilled 
		//  --> if not: check if trigger is now fullfilled
		if(!isTriggered) {
			if(mTrigger != 0) {
				isTriggered = mTrigger->checkTrigger( 
                	    		mTriggerNormalization->normalize(posValMap.value(triggerPos)));
			} else {
				isTriggered = true;
			}	
		}

		// store values if the trigger was fullfilled in normalized form
		if(isTriggered) {
			for(int i = 0; i < posVector.count(); i++) {
				if(motionLengthInFrames > 0) {
					mData.at(i)->replace(framesSinceTriggered,
						mNormalizations.at(i)->normalize(posValMap.value(posVector.at(i))));
				} else {
					mData.at(i)->append(mNormalizations.at(i)->normalize(posValMap.value(posVector.at(i))));
				}
			}
			framesSinceTriggered++;
		}

		frameCount++;
	}

	if(framesSinceTriggered <= 0) {
		Core::log(QString("TextFilePhysicalDataSource: ERROR - Found no trigger condition in the data file [%1]!").arg(mDataFilePath));
		return -1;
	}

	if(motionLengthInFrames >= 0 &&
		framesSinceTriggered < motionLengthInFrames) {
		Core::log(QString("TextFilePhysicalDataSource: ERROR - Not enough data in the data file [%1] for a motion analysis with length of %2 msec! The data file provides only a length of %3 msec with the used trigger.").arg(
			mDataFilePath, QString::number(Tools::convertSimStepsToMsec(motionLengthInSimSteps)),
			QString::number(framesSinceTriggered * mFrameTimeLength)));
		return -1;
	}

	return Tools::convertMsecToSimSteps(framesSinceTriggered * mFrameTimeLength);
}


/**
 * Reads the data from the next frame in the data file. The data is defined 
 * by the keys in the PositionDoubleValueMap which correspond to positions in the
 * frame. The values are directly inserted in to the PositionDoubleValueMap.
 * 
 * @param posValMap PositionDoubleValueMap where the keys are the positions which sould be read
 * in the frame and the values are the read values.
 * @return NOERROR if no error occured, FRAMEINCOMPLETE if the frame was not complete.
 */
TextFilePhysicalDataSource::error
TextFilePhysicalDataSource::readNextFrame(PositionDoubleValueMap *posValMap) {
	// go through map and read the data defined by the key with the position
	PositionDoubleValueMap::const_iterator i = posValMap->constBegin();
	QString line = mTextStream->readLine();
	QStringList lineList = line.split(QRegExp("\\s+"));

	while (i != posValMap->constEnd()) {
		if(i.key() >= lineList.size()) {
			return FRAMEINCOMPLETE;
		}
		QString stringValue = lineList.at(i.key());

		bool ok;
		double curData = stringValue.toDouble(&ok);
		if(!ok) {
			return READERROR;
		}

		posValMap->insert(i.key(), curData);

		i++;
	}

	return NOERROR;
}

/**
 * Reads the position from an Orcs Value in the data file from the configuration
 * file.
 * 
 * @param valueName Name of the Orcs Value.
 * @return Position of the Orcs Value in the data file.
 */
int TextFilePhysicalDataSource::getValuePosition(const QString &valueName)
{
	int pos = -1;

	QDomElement curValueElem = mConfigDocument->readElement(
		OrcsModelOptimizerConstants::PDS_DESCR_BASE_ELEMENT 
		+ XMLConfigDocument::SEPARATOR 
		+ OrcsModelOptimizerConstants::PDS_DESCR_VALUE_ELEMENT);

	while(!curValueElem.isNull()) {
		QString curOrcsNameStr = curValueElem.attribute(
			OrcsModelOptimizerConstants::PDS_DESCR_ORCSNAME_VALUE);

		if(curOrcsNameStr == valueName) {
			QString posStr = curValueElem.attribute(
			OrcsModelOptimizerConstants::PDS_DESCR_COLUMNPOS_VALUE);

			bool ok = true;
			pos = posStr.toInt(&ok);

			if(ok == false) {
				Core::log(QString("TextFilePhysicalDataSource: ERROR - In the configuration file [%1] there is no value defined for the parameter [%2] (%3 = %4) or the parameter is in a wrong format!").arg(
					mConfigDocument->getFileName(), 
					OrcsModelOptimizerConstants::PDS_DESCR_VALUE_ELEMENT + "/" +
					OrcsModelOptimizerConstants::PDS_DESCR_COLUMNPOS_VALUE,
					OrcsModelOptimizerConstants::PDS_DESCR_ORCSNAME_VALUE,
					curOrcsNameStr));
			}

			break;
		}

		curValueElem = mConfigDocument->readNextEqualElement(curValueElem);
	}

	if(pos == -1) {
		Core::log(QString("TextFilePhysicalDataSource: ERROR - Could not find a configuration for the value [%1] in the configuration file [%2]!").arg(
				valueName, mConfigDocument->getFileName()));
	}

	return pos;
}

} // namespace nerd
