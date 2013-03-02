/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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
 *   The NERD Toolkit is part of the EU project ALEAR                      *
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
 *   clearly by citing the NERD homepage and the NERD overview paper.      *  
 ***************************************************************************/



#include "SimulationRecorder.h"
#include "Event/EventManager.h"
#include "Core/Core.h"
#include <iostream>
#include "Value/ValueManager.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include <QStringList>
#include "NerdConstants.h"
#include <Math/Math.h>
#include <QFile>
#include <QDir>
#include <QString>


using namespace std;

namespace nerd {

SimulationRecorder::SimulationRecorder()
	: mResetEvent(0), mStepCompletedEvent(0), mPhysicsWasDisabled(false), mStartEndFrameRange(0, 0), 
		mNumberOfFrames(0), mFrameNumber(0), mFile(0), mDataStream(0), mExecutionMode(0)
		
{
	Core::getInstance()->addSystemObject(this);

	mActivateRecording = new BoolValue(false);
	mActivatePlayback = new BoolValue(false);
	mPlaybackSafeMode = new BoolValue(false);
	mRecordingDirectory = new FileNameValue(Core::getInstance()->getConfigDirectoryPath() + "/" + "dataRecording/");
	mFileNamePrefix = new StringValue("rec");
	mPlaybackFile = new FileNameValue("");
	mRecordingInterval = new IntValue(1);
	mObservedValues = new CodeValue("[Interfaces]\n/Sim/**/Position\n/Sim/**/OrientationQuaternion");
	mRecordedValueNameList = new CodeValue();
	mStartEndFrameValue = new RangeValue(0, 0);
	mNumberOfFramesValue = new IntValue(0);
	mDesiredFrameValue = new IntValue(-1);
	mCurrentFrameValue = new IntValue(0);
	
	
	mPlaybackSafeMode->setDescription("Is slower, but works with corrupt files, e.g. after "
										"a crash of if the harddrive was full.");
	mRecordingDirectory->setDescription("The directory in which all files are stored");
	mFileNamePrefix->setDescription("The file name prefix. The recorder makes sure that the "
										"final file name is unique, using numbers as postfix.");
	mPlaybackFile->setDescription("The DATA file to play back. This is the file without .onn, .js or _info.txt");
	mRecordingInterval->setDescription("The interval at which frames are recorded. "
										"\nOnly every nth simulation step is recorded."
										"\nThis is important to reduce the file size");
	mObservedValues->setDescription("Describes the values to record in terms of regular expressions."
										"\nPredefined values are: "
										"\n- [Interfaces] All interface values (motors and sensors) of the simluation.");
	mRecordedValueNameList->setDescription("Do not change this data. Gives an overview on the actually recorded values"
										"\nafter applying the regular expressions of RecordedValues.");
	mStartEndFrameValue->setDescription("The range of simulation steps covered by the currently playing data file.");
	mNumberOfFramesValue->setDescription("The number of data frames contained in the playing data file.");
	mDesiredFrameValue->setDescription("Can be used to jump to a desired frame number during playback."
										"\nFrames < 0 and > NumberOfFrames have no effect. Note that seeking over large"
										"\ndistances can take some time!");
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/DataRecorder/ActivateRecording", mActivateRecording);
	vm->addValue("/DataRecorder/ActivatePlayback", mActivatePlayback);
	vm->addValue("/DataRecorder/FileName", mFileNamePrefix);
	vm->addValue("/DataRecorder/PlaybackFile", mPlaybackFile);
	vm->addValue("/DataRecorder/RecordingDirectory", mRecordingDirectory);
	vm->addValue("/DataRecorder/RecordingInterval", mRecordingInterval);
	vm->addValue("/DataRecorder/RecordedValues", mObservedValues);
	vm->addValue("/DataRecorder/Record/RecordedValueNameList", mRecordedValueNameList);
	vm->addValue("/DataRecorder/Playback/SafeMode", mPlaybackSafeMode);
	vm->addValue("/DataRecorder/Playback/Range", mStartEndFrameValue);
	vm->addValue("/DataRecorder/Playback/NumberOfFrames", mNumberOfFramesValue);
	vm->addValue("/DataRecorder/Playback/DesiredFrame", mDesiredFrameValue);
	vm->addValue("/DataRecorder/Playback/CurrentFrame", mCurrentFrameValue);
	
	mActivateRecording->addValueChangedListener(this);
	mActivatePlayback->addValueChangedListener(this);
	
	
}

SimulationRecorder::~SimulationRecorder() {
}


QString SimulationRecorder::getName() const {
	return "SimulationRecorder";
}


bool SimulationRecorder::init() {
	return true;
}


bool SimulationRecorder::bind() {
	bool ok = true;
	
	mRecordingDirectory->set(Core::getInstance()->getConfigDirectoryPath() + "/" + "dataRecording/");
	
	EventManager *em = Core::getInstance()->getEventManager();

	mResetEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_RESET, this);
	Event *nextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP, this);
	mStepStartedEvent = em->createEvent("TriggerPlaybackEvent", "Triggered right before a new step is executed...");
	if(nextStepEvent != 0) {
		nextStepEvent->addUpstreamEvent(mStepStartedEvent);
	}
	
	mStepCompletedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, this);

	ValueManager *vm = Core::getInstance()->getValueManager();
	
	mCurrentStep = vm->getIntValue(SimulationConstants::VALUE_EXECUTION_CURRENT_STEP);
	mPhysicsDisabled = vm->getBoolValue(SimulationConstants::VALUE_DISABLE_PHYSICS);

	if(mResetEvent == 0) {
		Core::log("SimulationRecorder: Could not find Event [" + NerdConstants::EVENT_EXECUTION_RESET + "]", true);
		ok = false;
	}
	if(mStepCompletedEvent == 0) {
		Core::log("SimulationRecorder: Could not find Event [" + NerdConstants::EVENT_EXECUTION_STEP_COMPLETED + "]", true);
		ok = false;
	}
	if(mCurrentStep == 0) {
		Core::log("SimulationRecorder: Could not find Value [" + SimulationConstants::VALUE_EXECUTION_CURRENT_STEP  + "]", true);
	}
	if(mPhysicsDisabled == 0) {
		Core::log("SimulationRecorder: Could not find Value [" + SimulationConstants::VALUE_DISABLE_PHYSICS  + "]", true);
	}

	return ok;
}


bool SimulationRecorder::cleanUp() {
	if(mExecutionMode == 1) {
		stopRecording();
	}
	else if(mExecutionMode == -1) {
		stopPlayback();
	}
	mActivateRecording->removeValueChangedListener(this);
	mActivatePlayback->removeValueChangedListener(this);
	return true;
}


void SimulationRecorder::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mResetEvent) {
		recordData(true);
	}
	else if(event == mStepCompletedEvent) {
		recordData();
	}
	else if(event == mStepStartedEvent) {
		playbackData();
	}
}

void SimulationRecorder::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mActivateRecording) {
		if(mActivateRecording->get() == true) {
			startRecording();
		}
		else {
			stopRecording();
		}
	}
	else if(value == mActivatePlayback) {
		if(mActivatePlayback->get() == true) {
			startPlayback();
		}
		else {
			stopPlayback();
		}
	}
}


void SimulationRecorder::startRecording() {

	if(mExecutionMode == -1) {
		stopPlayback();
	}
	if(mStepCompletedEvent == 0) {
		Core::log("SimulationRecorder: Cannot record without Event [" + NerdConstants::EVENT_EXECUTION_STEP_COMPLETED + "]", true);
		return;
	}
	mStepCompletedEvent->addEventListener(this);
	
	if(mFile != 0) {
		stopRecording();
	}
	
	mFrameNumber = 0;
	
	Core::getInstance()->enforceDirectoryPath(mRecordingDirectory->get());
	
	//create file
	int counter = 1;
	QFile file;
	do {
		file.setFileName(mRecordingDirectory->get() + mFileNamePrefix->get() + "_" + QString::number(counter) + ".data");
		++counter;
	} while(file.exists());
	
	mFile = new QFile(file.fileName());
	
	if(!mFile->open(QIODevice::WriteOnly)) {
		Core::log(QString("Could not open file ").append(mFile->fileName()).append(" to record the simulation data."), true);
		mFile->close();
		delete mFile;
		mFile = 0;
		return;
	}
	
	mPlaybackFile->set(mFile->fileName());
	
	Core::log("Starting data recording to file [" + mFile->fileName() + "]! ", true);
	
	//get all values to record...
	updateListOfRecordedValues();
	
	//update the string list of value names.
	updateRecordedValueNameList();
	
	QFile infoFile(file.fileName().append("_info.txt"));
	if(!infoFile.open(QIODevice::WriteOnly)) {
		Core::log(QString("Could not open infor file ").append(infoFile.fileName()).append("."), true);
		infoFile.close();
		return;
	}
	
	{
		QTextStream infoStream(&infoFile);
		writeInfoFile(infoStream);
	}
	infoFile.close();
	
	
	mData.reserve(5000);
	mFileDataStream.setDevice(mFile);
	
	//write data file format version number
	mFileDataStream << ((uint) 2);
	
	mExecutionMode = 1;
	mStepCounter = 0;
	
}

void SimulationRecorder::stopRecording() {
	if(mStepCompletedEvent == 0 || mFile == 0) {
		return;
	}
	
	if(mFile != 0) {
		Core::log("Stopping data recording! ", true);
	}
	
	mStepCompletedEvent->removeEventListener(this);
	
	mFileDataStream.setDevice(0);
	
	if(mFile != 0) {
		mFile->flush();
		mFile->close();
		delete mFile;
		mFile = 0;
	}
	
	if(mActivateRecording->get()) {
		mActivateRecording->set(false);
	}
	
	mExecutionMode = 0;
}


/**
 * Records a full data frame of the simulation. If forceRecording is true, then the frame is definively recorded. 
 * Otherwise, a recodring takes only place according to the recording policy, e.g. every nth time frame.
 */
void SimulationRecorder::recordData(bool forceRecording) {
	if(mExecutionMode != 1) {
		return;
	}
	
	if(mFile == 0 || !mFile->isOpen()) {
		return;
	}
	
	++mStepCounter;
	if(!forceRecording && mStepCounter < mRecordingInterval->get()) {
		return;
	}
	mStepCounter = 0;
	
	mData.clear();
	mDataStream = new QDataStream(&mData, QIODevice::WriteOnly);
	
	
	
	updateRecordedData(*mDataStream);
	
	//write the number of bytes before and after the block to allow a backseeking.
	mFileDataStream << ((uint) mFrameNumber);
	mFileDataStream << ((qint32) mCurrentStep->get());
	mFileDataStream << ((uint) mData.size());
	mFileDataStream.writeRawData(mData.data(), mData.size());
	mFileDataStream << ((uint) mData.size());
	
	++mFrameNumber;

	delete mDataStream;
	mDataStream = 0;
}



bool SimulationRecorder::startPlayback() {
	if(mExecutionMode == 1) {
		stopRecording();
	}
	
	if(mPhysicsDisabled == 0) {
		return false;
	}
	
	mStepStartedEvent->addEventListener(this);
	
	mFile = new QFile(mPlaybackFile->get());
	
	if(!mFile->open(QIODevice::ReadOnly)) {
		Core::log(QString("Could not open file ").append(mFile->fileName()).append(" to record the simulation data."), true);
		mFile->close();
		delete mFile;
		mFile = 0;
		return false;
	}
	
	mFileDataStream.setDevice(mFile);
	
	//determine start, end and number of frames.
	mNumberOfFrames = 0;
	if(mFileDataStream.atEnd()) {
		stopPlayback();
		return false;
	}
	uint version = 0;
	mFileDataStream >> version;
	if(mFileDataStream.atEnd() || version != 2) {
		stopPlayback();
		return false;
	}
	uint frameNumber = 0;
	mFileDataStream >> frameNumber;
	qint32 start = -1;
	mFileDataStream >> start;
	qint32 end = start;
	
	if(!mPlaybackSafeMode->get()) {
		uint size = 0;
		qint64 fileSize = mFile->size();
		mFile->seek(fileSize - sizeof(uint));
		mFileDataStream >> size;
		mFile->seek(mFile->pos() - size - (2 * sizeof(uint)) - (2 * sizeof(qint32)));
		mFileDataStream >> frameNumber;
		mFileDataStream >> end;
		
		mNumberOfFrames = frameNumber + 1;
		
		mStartEndFrameValue->set(start, end);
		mStartEndFrameRange.set(start, end);
		mNumberOfFramesValue->set(mNumberOfFrames);
	}
	else {
		//this is much slower, but it also works with corrupt files that 
		//may occur when no space is left on a defice during writing.
		uint size = 0;
		while(!mFileDataStream.atEnd()) {
			++mNumberOfFrames;
			
			mFileDataStream >> size;
			mFileDataStream.skipRawData(size);
			mFileDataStream >> size;
			if(!mFileDataStream.atEnd()) {
				mFileDataStream >> frameNumber;
			}
			if(!mFileDataStream.atEnd()) {
				mFileDataStream >> end;
			}
		}

		mStartEndFrameValue->set(start, end);
		mStartEndFrameRange.set(start, end);
		mNumberOfFramesValue->set(mNumberOfFrames);
	}
	
	mFrameNumber = 0;
	
	
	mPhysicsWasDisabled = mPhysicsDisabled->get();
	mPhysicsDisabled->set(true);
	
	//get all values to record...
	syncWithListOfRecordedValues();
	
	updateRecordedValueNameList();
	
	mExecutionMode = -1;
	
	mReachedAndOfFile = true;
	
	mFirstPlaybackStep = true;
	mReadStepNumber = false;
	
	return true;
}


bool SimulationRecorder::stopPlayback() {
	
	mStepStartedEvent->removeEventListener(this);
	
	if(mFile != 0) {
		Core::log("Stopping data playback! ", true);
	}
	
	if(mFile != 0) {
		mFile->close();
		delete mFile;
		mFile = 0;
	}
	if(mActivatePlayback->get()) {
		mActivatePlayback->set(false);
	}
	
	if(mPhysicsDisabled->get() != mPhysicsWasDisabled) {
		mPhysicsDisabled->set(mPhysicsWasDisabled);
	}
	
	mFileDataStream.setDevice(0);
	
	mExecutionMode = 0;
	
	return true;
}


void SimulationRecorder::playbackData() {
	if(mExecutionMode != -1) {
		return;
	}
	
	if(mFile == 0 || !mFile->isOpen()) {
		return;
	}
	
	//restart if at end of file.
	if(mReachedAndOfFile) {
		mFileDataStream.setDevice(0);
		mFile->reset();
		mFileDataStream.setDevice(mFile);
		mReachedAndOfFile = false;
		
		//read out data file format version number
		mVersion = 0;
		mFileDataStream >> mVersion;
		
		if(mVersion != 2) {
			Core::log("SimulationRecorder: Wrong data version. This NERD release only accepts data recordings of data format 2", true);
			stopPlayback();
			return;
		}
		mFrameNumber = 0;
		mReadStepNumber = false;
	}
	
	if(mDesiredFrameValue->get() >= mNumberOfFrames || mDesiredFrameValue->get() == ((int) mFrameNumber)) {
		mDesiredFrameValue->set(-1);
	}
	
	//If a special frame is seeked, then try to find the matching frame in the file.
	if(mDesiredFrameValue->get() >= 0) {
		
		bool fwd = true;
		if(mDesiredFrameValue->get() < ((int)  mFrameNumber)) {
			fwd = false;
		}
		
		qint32 step = 0;
		qint32 size = 0;
		uint frameNumber = 0;
		
		//determine where to start to speed up seeking
		if(!mPlaybackSafeMode->get() && fwd && (mDesiredFrameValue->get() - ((int) mFrameNumber) > (mNumberOfFrames - mDesiredFrameValue->get()))) {
			fwd = false;
			mFile->seek(mFile->size() - sizeof(uint));
			mFileDataStream >> size;
			mFile->seek(mFile->pos() - size - (2 * sizeof(uint)) - (2 * sizeof(qint32)));
			mReadStepNumber = false;
		}
		else if(!fwd && (((int) mFrameNumber - mDesiredFrameValue->get()) > mDesiredFrameValue->get())) {
			uint version = 0;
			fwd = true;
			mFile->reset();
			mFileDataStream >> version;
			mReadStepNumber = false;
		}
		
		if(fwd) {
			if(!mReadStepNumber) {
				mFileDataStream >> frameNumber;
				mFileDataStream >> step; //step number
			}
			while(((int) frameNumber) != mDesiredFrameValue->get() && !mFileDataStream.atEnd()) {
				mFileDataStream >> size; //size
				mFileDataStream.skipRawData(size);
				mFileDataStream >> size; //second size
				mFileDataStream >> frameNumber;
				mFileDataStream >> step; //step number
			}
		}
		else {
			if(!mReadStepNumber) {
				mFileDataStream >> frameNumber;
				mFileDataStream >> step; //step number
			}
			do {
				mFile->seek(mFile->pos() - sizeof(size) - sizeof(step) - sizeof(frameNumber));
				mFileDataStream >> size;
				cerr << "Size: " << size << " | ";
				mFile->seek(mFile->pos() - size - sizeof(step) - (2 * sizeof(size)) - sizeof(frameNumber));
				mFileDataStream >> frameNumber;
				mFileDataStream >> step; //step number
				cerr << "FrameNo: " << frameNumber << " step " << step << endl;
				
			} while(((int) frameNumber) != mDesiredFrameValue->get() && !mFileDataStream.atEnd());
		}
		
		mDesiredFrameValue->set(-1);
		
		if(mFileDataStream.atEnd()) {
			mReachedAndOfFile = true;
			Core::log("Seeked frame is the end of the data stream!", true);
			return;
		}
		mReadStepNumber = true;
		mStepCounter = step;
		mFrameNumber = frameNumber;
		mCurrentStep->set(mStepCounter);
		
// 		mFile->reset();
// 		uint tokenUInt = 0;
// 		qint32 step = 0;
// 		qint32 size = 0;
// 		mFileDataStream >> tokenUInt;
// 		
// 		uint frameNumber = 0;
// 		mFileDataStream >> frameNumber;
// 		mFileDataStream >> step; //step number
// 		while(((int) frameNumber) != mDesiredFrameValue->get() && !mFileDataStream.atEnd()) {
// 			mFileDataStream >> size; //size
// 			mFileDataStream.skipRawData(size);
// 			mFileDataStream >> size; //second size
// 			mFileDataStream >> frameNumber;
// 			mFileDataStream >> step; //step number
// 		}
// 		
// 		mDesiredFrameValue->set(-1);
// 		
// 		if(mFileDataStream.atEnd()) {
// 			mReachedAndOfFile = true;
// 			Core::log("Seeked frame is the end of the data stream!", true);
// 			return;
// 		}
// 		mReadStepNumber = true;
// 		mStepCounter = step;
// 		mFrameNumber = frameNumber;
// 		mCurrentStep->set(mStepCounter);
	}
	
	//check if the new frame should be applied...
	if(!mReadStepNumber) {
		mFileDataStream >> mFrameNumber;
		
		qint32 step = 0;
		mFileDataStream >> step;
		mStepCounter = step;
		mReadStepNumber = true;
	}
	
	if(!mFirstPlaybackStep && (mStepCounter > mCurrentStep->get())) {
		return;
	}
	mFirstPlaybackStep = false;
	mReadStepNumber = false;
	
	mCurrentStep->set(mStepCounter);
	mCurrentFrameValue->set(mFrameNumber);
	
	uint size = 0;
	mFileDataStream >> size;
	char *content = new char[size];
	
	mFileDataStream.readRawData(content, size);
	
	uint size2 = 0;
	mFileDataStream >> size2;
	
	if(mFile->atEnd()) {
		mReachedAndOfFile = true;
	}
	
	QByteArray data(content, size);;
	mDataStream = new QDataStream(&data, QIODevice::ReadOnly);
	
	
	
	updatePlaybackData(*mDataStream);
	
	delete mDataStream;
	mDataStream = 0;
	delete content;
	
}


void SimulationRecorder::updateRecordedValueNameList() {
	mRecordedValueNameList->set("Number of Values: " 
					+ QString::number(mRecordedValueNames.size()) 
					+ "\n\n" + mRecordedValueNames.join("\n"));
}



void SimulationRecorder::updateListOfRecordedValues() {
	
	
	
	mRecordedValues.clear();
	mRecordedValueNames.clear();
	
	ValueManager *vm = Core::getInstance()->getValueManager();

	QStringList entries = mObservedValues->get().split("\n");
	
	for(QStringListIterator i(entries); i.hasNext();) {
		QString entry = i.next();
		
		
		//Add all input and output neurons (Interface Values) if requested.
		if(entry.trimmed() == "[Interfaces]") {
			
			QList<Value*> allValues = vm->getValuesMatchingPattern("/Sim/.*");
			for(QListIterator<Value*> i(allValues); i.hasNext();) {
				InterfaceValue *iv = dynamic_cast<InterfaceValue*>(i.next());
				if(iv != 0) {
					QString name = vm->getNamesOfValue(iv).first();
					if(!mRecordedValues.contains(iv)) {
						mRecordedValues.append(iv);
						mRecordedValueNames.append("[P]" + name);
					}
				}
			}
			continue;
		}
		
		
		entry = entry.replace("**", ".*");
		QList<QString> valueNames = vm->getValueNamesMatchingPattern(entry);

		for(QListIterator<QString> j(valueNames); j.hasNext();) {
			QString name = j.next();
			Value* v = vm->getValue(name);
			
			MultiPartValue *mv = dynamic_cast<MultiPartValue*>(v);
			if(mv != 0) {
				for(int k = 0; k < mv->getNumberOfValueParts(); ++k) {
					Value *mpv = mv->getValuePart(k);
					if(!mRecordedValues.contains(mpv)) {
						mRecordedValues.append(mpv);
						mRecordedValueNames.append("[M]" + name + "[" + mv->getValuePartName(k) + "]");
					}
				}
			}
			else {
				if(!mRecordedValues.contains(v)) {
					mRecordedValues.append(v);
					mRecordedValueNames.append("[P]" + name);
				}
			}
		}
	}
	
}


void SimulationRecorder::syncWithListOfRecordedValues() {
	
	mRecordedValueNames.clear();
	mRecordedValues.clear();
	
	QFile *infoFile = new QFile(mPlaybackFile->get() + "_info.txt");

	if(!infoFile->open(QIODevice::ReadOnly)) {
		Core::log(QString("Could not open file ").append(mFile->fileName()).append(" to record the simulation data."), true);
		infoFile->close();
		delete infoFile;
		infoFile = 0;
		return;
	}	

	ValueManager *vm = Core::getInstance()->getValueManager();
	
	QTextStream dataStream(infoFile);
	while(!dataStream.atEnd()) {
		
		QString line = dataStream.readLine().trimmed();

		if(line.startsWith("[P]")) {
			QString name = line.mid(3);
			Value *v = vm->getValue(name);
			
			if(dynamic_cast<DoubleValue*>(v) != 0 || dynamic_cast<IntValue*>(v) != 0) {
				mRecordedValues.append(v);
				mRecordedValueNames.append("[P]" + name);
			}
		}
		else if(line.startsWith("[M]") && line.endsWith("]")) {
			QString name = line.mid(3);
			int s = name.lastIndexOf("[");
			if(s <= 0) {
				continue;
			}
			QString postfix = name.mid(s + 1, name.size() - 2 - s);
			name = name.mid(0, s);
			
			MultiPartValue *v = dynamic_cast<MultiPartValue*>(vm->getValue(name));
			
			if(v != 0) {
				int index = 0;
				for(; index < v->getNumberOfValueParts(); ++index) {
					if(v->getValuePartName(index) == postfix) {
						Value *mpv = v->getValuePart(index);
						
						mRecordedValues.append(mpv);
						mRecordedValueNames.append("[M]" + name + "[" + postfix + "]");
					}
				}
			}
		}
	}
	infoFile->close();
	delete infoFile;
	
}


void SimulationRecorder::updateRecordedData(QDataStream &dataStream) {
	
	dataStream << ((qint32) mRecordedValues.size());

	QString s;
	
	for(QListIterator<Value*> i(mRecordedValues); i.hasNext();) {
		Value *v = i.next();
		if(dynamic_cast<DoubleValue*>(v) != 0) {
			dataStream << dynamic_cast<DoubleValue*>(v)->get();
		}
		else if(dynamic_cast<IntValue*>(v) != 0) {
			dataStream << ((double) dynamic_cast<IntValue*>(v)->get());
		}
	}
}


void SimulationRecorder::updatePlaybackData(QDataStream &dataStream) {
	
	qint32 numberOfValues = 0;
	dataStream >> numberOfValues;
	
	int counter = 0;
	for(QListIterator<Value*> i(mRecordedValues); i.hasNext() && (counter < numberOfValues);) {
		Value *v = i.next();
		if(dynamic_cast<DoubleValue*>(v) != 0) {
			double val = 0;
			dataStream >> val;
			dynamic_cast<DoubleValue*>(v)->set(val);
		}
		else if(dynamic_cast<IntValue*>(v) != 0) {
			double val = 0;
			dataStream >> val;
			dynamic_cast<IntValue*>(v)->set((int) (val + (Math::sign(val) * 0.5)));
		}
		counter++;
	}
	//make sure that the network information can be read afterwards, even when objects are missing.
	double val = 0;
	for(; counter < numberOfValues; ++counter) {
		dataStream >> val;
	}
}

void SimulationRecorder::writeInfoFile(QTextStream &dataStream) {
	
	dataStream << QString("Date: " + QDate::currentDate().toString("yy-MM-dd") + " at "
			+ QTime::currentTime().toString("hh-mm-ss") + "\n\n");
	dataStream << QString("Data File: " + mPlaybackFile->get() + "\n\n");	
	dataStream << QString("Parameter Search Pattern:\n\n" + mObservedValues->get() + "\n\n");
	dataStream << QString("Parameters: \n\n[Params]\n" + mRecordedValueNameList->get() + "\n[/Params]\n\n");
}


}


