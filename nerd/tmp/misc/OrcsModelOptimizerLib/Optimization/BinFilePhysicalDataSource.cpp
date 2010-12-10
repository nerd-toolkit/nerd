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


#include "BinFilePhysicalDataSource.h"

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

const uint BinFilePhysicalDataSource::DATATYPE_LENGTH = 2; 
      
  
BinFilePhysicalDataSource::BinFilePhysicalDataSource(
			Trigger *trigger,
			Normalization *triggerNormalization,
			QVector<StringValue*> trainDataValueNames,
			QVector<Normalization*> trainDataValueNormalizations,
			const QString &configDocumentPath, 
			const QString &dataFilePath)
  : PhysicalDataSource(	trigger,
						triggerNormalization,
						trainDataValueNames,
						trainDataValueNormalizations,
						configDocumentPath, 
						dataFilePath),
	mDataFile(0), 
	mDataStream(0), 
	mFrameLength(0),
	mFrameTimeLength(0), 
	mTimePos(0), 
	mTimeChangeIntervall(0)
{
  
}

BinFilePhysicalDataSource::~BinFilePhysicalDataSource()
{ 
  for(int i = 0; i < mData.size(); i++){
    delete mData[i];
  }
}

QString BinFilePhysicalDataSource::getName() const{
  return "BinFilePhysicalDataSource";
}

bool BinFilePhysicalDataSource::init()
{
  // initialize at first the base
  //   --> needed for further initialization 
  bool ok = PhysicalDataSource::init();
  
  // base has to be correctly initialized
  if(ok == false)
    return false;
  
  ///////////////////////////////////////////////
  // Read needed values from configuration file
  ///////////////////////////////////////////////

  QString dataFormatElemStr = OrcsModelOptimizerConstants::PDS_DESCR_BASE_ELEMENT +
      XMLConfigDocument::SEPARATOR +
      OrcsModelOptimizerConstants::PDS_DESCR_DATAFORMAT_ELEMENT;
  
  // read  mTimePos
  QString timePosStr = mConfigDocument->readConfigParameter(dataFormatElemStr,
                                       OrcsModelOptimizerConstants::PDS_DESCR_TIMEPOS_VALUE);  
  
  mTimePos = timePosStr.toUInt(&ok);
  
  if(ok == false){
    Core::log(QString("BinFilePhysicalDataSource: ERROR - In the configuration file [%1] is no value defined for the parameter [%2] or the parameter is in a wrong format!").arg(
               mConfigDocument->getFileName(), 
               dataFormatElemStr.replace(XMLConfigDocument::SEPARATOR, "/") + "/" +
                   OrcsModelOptimizerConstants::PDS_DESCR_TIMEPOS_VALUE));
  }
  
  // read  mFrameLength
  QString frameLengthStr = mConfigDocument->readConfigParameter(
      dataFormatElemStr,
      OrcsModelOptimizerConstants::PDS_DESCR_FRAMELENGTH_VALUE);  
   
  mFrameLength = frameLengthStr.toUInt(&ok);
  
  if(ok == false){
    Core::log(QString("BinFilePhysicalDataSource: ERROR - In the configuration file [%1] is no value defined for the parameter [%2] or the parameter is in a wrong format!").arg(
              mConfigDocument->getFileName(), 
              dataFormatElemStr.replace(XMLConfigDocument::SEPARATOR, "/") + "/" +
              OrcsModelOptimizerConstants::PDS_DESCR_FRAMELENGTH_VALUE));
  }
  
    // read  frameTimeLength to calculate mMotionLengthInFrames
  QString frameTimeLengthStr = mConfigDocument->readConfigParameter(
      dataFormatElemStr,
      OrcsModelOptimizerConstants::PDS_DESCR_FRAMETIMELENGTH_VALUE);  
   
  mFrameTimeLength = frameTimeLengthStr.toUInt(&ok);
  
  if(ok == false){
    Core::log(QString("BinFilePhysicalDataSource: ERROR - In the configuration file [%1] is no value defined for the parameter [%2] or the parameter is in a wrong format!").arg(
              mConfigDocument->getFileName(), 
              dataFormatElemStr.replace(XMLConfigDocument::SEPARATOR, "/") + "/" +
              OrcsModelOptimizerConstants::PDS_DESCR_FRAMETIMELENGTH_VALUE));
  }
  
  // read value for sync value 
  mSyncValue = mConfigDocument->readConfigParameter(
      dataFormatElemStr,
      OrcsModelOptimizerConstants::PDS_DESCR_SYNCVALUE_VALUE);
    
  if(mSyncValue.isNull() || mSyncValue.isEmpty()) {
    Core::log(QString("BinFilePhysicalDataSource: ERROR - In the configuration file [%1] is no value defined for the parameter [%2]!").arg(
              mConfigDocument->getFileName(), 
              dataFormatElemStr.replace(XMLConfigDocument::SEPARATOR, "/") + "/" +
              OrcsModelOptimizerConstants::PDS_DESCR_SYNCVALUE_VALUE));
  }
  
  // read timeChangeIntervall
  QString timeChangeIntervall = mConfigDocument->readConfigParameter(
      dataFormatElemStr,
      OrcsModelOptimizerConstants::PDS_DESCR_TIMECHANGEINTERVALL_VALUE);
    
  mTimeChangeIntervall = timeChangeIntervall.toUInt(&ok);
  
  if(ok == false){
    Core::log(QString("BinFilePhysicalDataSource: ERROR - In the configuration file [%1] is no value defined for the parameter [%2] or the parameter is in a wrong format!").arg(
              mConfigDocument->getFileName(), 
              dataFormatElemStr.replace(XMLConfigDocument::SEPARATOR, "/") + "/" +
                  OrcsModelOptimizerConstants::PDS_DESCR_TIMECHANGEINTERVALL_VALUE));
  }
  
  
  ///////////////////////////////////////////////
  // Open Datafile and DataStream
  ///////////////////////////////////////////////
  
  mDataFile = new QFile(mDataFilePath);
  
  if(!mDataFile->exists()){
    Core::log(QString("BinFilePhysicalDataSource: Data file [%1] does not exist!").arg(mDataFilePath));
    return false;
  }
  
  if(!mDataFile->open(QIODevice::ReadOnly))
  {
    Core::log(QString("BinFilePhysicalDataSource: Could not open data file [%1]! Maybe no read rights?").arg(mDataFilePath));
    return false;
  }
  
  mDataStream = new QDataStream(mDataFile);
  mDataStream->setByteOrder(QDataStream::LittleEndian);
  
  return ok;
}

double BinFilePhysicalDataSource::getData(int channel, int simulationStep)
{ 
  // 1) calculate the time point of the wanted data in milliseconds 
  double simulationStepInMsec = Tools::convertSimStepsToMsec(simulationStep);
      
  // 2) calculate the point of the wanted data in the mData vector
  int dataStep = simulationStepInMsec / mFrameTimeLength;
  
  return mData.at(channel)->at(dataStep);
}


int BinFilePhysicalDataSource::readData()
{
  return readDataFrames( -1 );
}

bool BinFilePhysicalDataSource::readData(int motionLengthInSimSteps)
{
  bool ok = true;
  
  if(readDataFrames( motionLengthInSimSteps ) < 0){
    return false;
  }
  
  return ok;
}

/**
 * Reads the data from the binary file into the mData. 
 * Checks the Trigger, thus only data after the trigger occurs are written into the mData.
 * Normalizes the data.
 * Reads data till mMotionLengthInFrames
 * 
 * @return TRUE if successfully, otherwise FALSE.
 */
int BinFilePhysicalDataSource::readDataFrames(int motionLengthInSimSteps)
{  
  int readMotionLengthInSimSteps = 0;
  
  int motionLengthInFrames = -1;
  
  if(motionLengthInSimSteps >= 0){
    motionLengthInFrames = Tools::convertSimStepsToMsec(motionLengthInSimSteps) 
                           / mFrameTimeLength;
    
    if(motionLengthInFrames < 0){
      return -1;
    }
  }
  
  PositionValueMap posValMap;
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
  // add position of timestamp for integrity check
  posValMap.insert(mTimePos, 0.0);
  
  // iterate over values which should be loaded and create the map to store their values for
  // each frame
  for(int i = 0; i < mTrainDataValueNames.count(); i++)
  {
    // get the position in the datafile for the value from the config file
    int valuePos = this->getValuePosition(mTrainDataValueNames[i]->get());
    
    if(valuePos < 0){
      return -1;
    }
    
    // store the position in a vector with the same sorting like the mTrainDataValueNames 
    posVector.push_back( valuePos ); 
    
    // store the position in a sorted map which will be filled by the readNextFrame() method
    // with the values for one frame
    posValMap.insert(valuePos, 0.0);
    
    // create data vector which holds all the data
    if(motionLengthInFrames > 0){
      mData.push_back( new QVector<double>( motionLengthInFrames ) ); 
    }
    else {
      mData.push_back( new QVector<double>() ); 
    }
  }
  
  // iterate through frames in the data file and store the needed values
  bool isTriggered = false;
  int framesSinceTriggered = 0;
  int frameCount = 1; // frame count is used for error messages
  int framesSinceTimeStampChange = 0;
  double oldTimestamp = -1.0;
  bool firstTimeChangeOccured = false;
  error lastReadError = NOERROR;
  
  while(!mDataStream->atEnd() &&  // check if the data ends
         (motionLengthInFrames < 0 ||  // check if enough frames are read or if all frames should 
         framesSinceTriggered < motionLengthInFrames) ) // be read (mMotionLengthInFrames == -1)
  {
    bool ok = true;
    bool skipUncompleteData = false;
       
    // read values from next frame, but skip uncomplete data before the first frame
    if(frameCount <= 1) {
      skipUncompleteData = true;
    }
    
    lastReadError = readNextFrame(&posValMap, skipUncompleteData);
    
    // stop if the frame is incomplete, thus not all data from the frame could be
    // read
    if(lastReadError == FRAMEINCOMPLETE){
      break;
    }
    
    if(lastReadError == NOSYNCVALUE) 
    {
      Core::log(QString("BinFilePhysicalDataSource: ERROR - An error occured while reading the %1th frame of the data file [%2]!").arg(QString::number(frameCount),mDataFilePath)); 
      return -1;
    }
    
    // check if the timestamp changes normally
    
    // check if timestamp changed
    // --> yes: check if correct number of frames per timestamp was read
    //          + check if timestamp is increased by one
    //          (the first timestamp change will be ignored, because the 
    //           data file can start recording during a frame periode)
    // --> no: check if too many frames per timestamp
    if(oldTimestamp != posValMap.value(mTimePos)){
      if(firstTimeChangeOccured == true &&
          (framesSinceTimeStampChange != mTimeChangeIntervall ||
           posValMap.value(mTimePos) != (oldTimestamp + 1.0))) {
        Core::log(QString("BinFilePhysicalDataSource: ERROR - The %1th frame of the data file [%2] has a wrong timestamp!").arg(QString::number(frameCount),mDataFilePath)); 
        return -1;
      }
      else {
        oldTimestamp = posValMap.value(mTimePos);
        framesSinceTimeStampChange = 1;
        
        if(frameCount > 1){
          firstTimeChangeOccured = true;
        }
      }
    }
    else {
      framesSinceTimeStampChange++;
      
      // error if too many frames per frame
      if(framesSinceTimeStampChange > mTimeChangeIntervall){
        Core::log(QString("BinFilePhysicalDataSource: ERROR - The %1th frame of the data file [%2] uses the same timestamp like the %3 frames before, but maximal %3 frames can use one timestamp!").arg(
                  QString::number(frameCount),
                  mDataFilePath,
                  QString::number(mTimeChangeIntervall))); 
        return -1;
      }
    }
    
    // check if trigger was already fullfilled 
    //  --> if not: check if trigger is now fullfilled
    if(!isTriggered)
		{
			if(mTrigger != 0){
				isTriggered = mTrigger->checkTrigger( 
                    		mTriggerNormalization->normalize( posValMap.value(triggerPos)));
			}
			else{
				isTriggered = true;
			}	
    }
    
    // store values if the trigger was fullfilled in normalized form
    if(isTriggered)
    {
      for(int i = 0; i < posVector.count(); i++)
      {
        if(motionLengthInFrames > 0) {
          mData.at(i)->replace( framesSinceTriggered,
                              mNormalizations.at(i)->normalize(posValMap.value(posVector.at(i))));       
        }
        else {
          mData.at(i)->append(mNormalizations.at(i)->normalize(posValMap.value(posVector.at(i))));
        }
      }
      
      framesSinceTriggered++;
    }
    
    frameCount++;
  }
  
  if(framesSinceTriggered <= 0)
  {
    Core::log(QString("BinFilePhysicalDataSource: ERROR - Found not trigger condition in the data file [%1]!").arg(
              mDataFilePath));
    
    return -1;
  }
  
  if( motionLengthInFrames >= 0 &&
      framesSinceTriggered < motionLengthInFrames) {
    Core::log(QString("BinFilePhysicalDataSource: ERROR - Not enough data in the data file [%1] for a motion analysis with length of %2 msec! The data file provides only a length of %3 msec with the used trigger.").arg(
      mDataFilePath,
      QString::number(Tools::convertSimStepsToMsec(motionLengthInSimSteps)),
      QString::number(framesSinceTriggered * mFrameTimeLength)));
    
    return -1;
  }
  
  return  Tools::convertMsecToSimSteps( framesSinceTriggered * mFrameTimeLength );
}


/**
 * Reads the data from the next frame in the data file. The data is defined 
 * by the keys in the PositionValueMap which correspond to positions in the
 * frame. The values are directly inserted in to the PositionValueMap.
 * 
 * @param posValMap PositionValueMap where the keys are the positions which sould be read
 * in the frame and the values are the read values.
 * @return NOERROR if no error occured, NOSYNCVALUE if the first value in the frame is
 *         not the configured mSyncValue, FRAMEINCOMPLETE if the frame was not complete.
 */
BinFilePhysicalDataSource::error
BinFilePhysicalDataSource::readNextFrame(PositionValueMap *posValMap, 
                                              bool skipDataBeforeFrame)
{ 
  // check sync value at first position
  qint16 syncValue;
  
  if(skipDataBeforeFrame == true)
  {
    bool foundFirstByte = false;
    bool foundSecondByte = false;
    qint8 first = mSyncValue.toInt() & 0xFF;
    qint8 second = mSyncValue.toInt() >> 8;
    
    qint8 curByte = 0;
    
    while(!mDataStream->atEnd() 
          && !foundSecondByte)
    {
      *mDataStream >> curByte;
      
      // if first byte is found and the current byte is the second
      //  --> stop because the mSyncValue is found
      if(foundFirstByte == true && curByte == second) {
        break;
      }
      
      // check if byte is first byte of the mSyncValue
      if(curByte == first){
        foundFirstByte = true;
      }
      else {
        foundFirstByte = false;
      }
    }
    
    if(mDataStream->atEnd()){
      Core::log(QString("BinFilePhysicalDataSource: ERROR - Could not find the configured Sync Value of \"%1\" in the data file!").arg(mSyncValue));
      return NOSYNCVALUE;
    }
  }
  else
  {
    *mDataStream >> syncValue;
    
    if(syncValue != mSyncValue.toInt()){
      Core::log(QString("BinFilePhysicalDataSource: ERROR - Sync Value in frame is not equal the configured Sync Value of \"%1\"!").arg(mSyncValue));
      return NOSYNCVALUE;
    }
  }
   
  // go through map and read the data defined by the key with the position
  PositionValueMap::const_iterator i = posValMap->constBegin();
  
  // start after the sync value to read
  uint curStreamPosition = 1 * DATATYPE_LENGTH;
  while (i != posValMap->constEnd()) 
  {
    // skip data to get to position which should be readed 
    mDataStream->skipRawData(DATATYPE_LENGTH * i.key() - curStreamPosition);
      
    if(mDataStream->atEnd()){
      //Core::log(QString("BinFilePhysicalDataSource: ERROR - The data file [%1] is incomplete! Please check if the data file is correct.").arg(mDataFilePath));
      return FRAMEINCOMPLETE;
    }
    
    // read value
    qint16 curData;
    *mDataStream >> curData; 
    
    posValMap->insert(i.key(),curData);
    
    // the new current position is the currently readed position plus DATATYPE_LENGTH,
    // because the reading process on the stream sets the pointer behind the readed position
    curStreamPosition = (i.key() * DATATYPE_LENGTH) + DATATYPE_LENGTH;
    
    // get next position to read
    i++;
  }
  
  // skip rest of the frame to begin next time at the next frame
  mDataStream->skipRawData(DATATYPE_LENGTH * mFrameLength - curStreamPosition);
 
  return NOERROR;
}

/**
 * Reads the position from a Orcs Value in the data file from the configuration
 * file.
 * 
 * @param valueName Name of the Orcs Value.
 * @return Position of the Orcs Value in the data file.
 */
int BinFilePhysicalDataSource::getValuePosition(const QString &valueName)
{
  int pos = -1;
  
  QDomElement curValueElem = mConfigDocument->readElement(
      OrcsModelOptimizerConstants::PDS_DESCR_BASE_ELEMENT 
      + XMLConfigDocument::SEPARATOR 
      + OrcsModelOptimizerConstants::PDS_DESCR_VALUE_ELEMENT);
  
  while(!curValueElem.isNull())
  {
    QString curOrcsNameStr = curValueElem.attribute(
       OrcsModelOptimizerConstants::PDS_DESCR_ORCSNAME_VALUE);
    
    if(curOrcsNameStr == valueName)
    {
      QString posStr = curValueElem.attribute(
        OrcsModelOptimizerConstants::PDS_DESCR_BINARYPOS_VALUE);
      
      bool ok = true;
      pos = posStr.toInt(&ok);
      
      if(ok == false) {
        Core::log(QString("BinFilePhysicalDataSource: ERROR - In the configuration file [%1] is no value defined for the parameter [%2] (%3 = %4) or the parameter is in a wrong format!").arg(
                  mConfigDocument->getFileName(), 
                  OrcsModelOptimizerConstants::PDS_DESCR_VALUE_ELEMENT + "/" +
                    OrcsModelOptimizerConstants::PDS_DESCR_BINARYPOS_VALUE,
                  OrcsModelOptimizerConstants::PDS_DESCR_ORCSNAME_VALUE,
                  curOrcsNameStr));
      }
      
      break;
    }
    
    curValueElem = mConfigDocument->readNextEqualElement(curValueElem);
  }
  
  if(pos == -1){
    Core::log(QString("BinFilePhysicalDataSource: ERROR - Could not find a configuration for the value [%1] in the configuration file [%2]!").arg(
              valueName,
              mConfigDocument->getFileName()));
  }
  
  return pos;
}

} // namespace nerd
