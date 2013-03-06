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
#include <Util/Util.h>
#include <QFile>
#include <QDir>
#include <QString>
#include <qcoreapplication.h>




using namespace std;

namespace nerd {
	
//Helper class to start recording via task object.

class StartRecordingTask : public Task {
	public:
		StartRecordingTask(SimulationRecorder *recorder, bool record, bool start) 
					: mRecorder(recorder), mRecord(record), mStart(start) {}
		
		virtual bool runTask() { 
			if(mRecorder == 0) { 
				return true; 
			}
			else if(mRecord) {
				if(mStart) {
					mRecorder->startRecording();
				}
				else {
					mRecorder->stopRecording();
				}
			}
			else {
				if(mStart) {
					mRecorder->startPlayback();
				}
				else {
					mRecorder->stopPlayback();
				}
			}
			return true; 
		};
	
	private:
		SimulationRecorder *mRecorder;
		bool mRecord;
		bool mStart;
};



SimulationRecorder::SimulationRecorder()
	: mResetEvent(0), mStepCompletedEvent(0), mPhysicsWasDisabled(false), mStartEndFrameRange(0, 0), 
		mNumberOfFrames(0), mFrameNumber(0), mWasPaused(false),  mFile(0), mDataStream(0), mExecutionMode(SIMREC_OFF)
		
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
	
	mDesiredFrameValue->addValueChangedListener(this);
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/DataRecorder/Control/ActivateRecording", mActivateRecording);
	vm->addValue("/DataRecorder/Control/ActivatePlayback", mActivatePlayback);
	vm->addValue("/DataRecorder/Recording/FileName", mFileNamePrefix);
	vm->addValue("/DataRecorder/Recording/RecordingDirectory", mRecordingDirectory);
	vm->addValue("/DataRecorder/Recording/RecordingInterval", mRecordingInterval);
	vm->addValue("/DataRecorder/Recording/RecordedValues", mObservedValues);
	vm->addValue("/DataRecorder/Recording/RecordedValueNameList", mRecordedValueNameList);
	vm->addValue("/DataRecorder/Playback/PlaybackFile", mPlaybackFile);
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
	
	//mRecordingDirectory->set(Core::getInstance()->getConfigDirectoryPath() + "/" + "dataRecording/");
	
	EventManager *em = Core::getInstance()->getEventManager();

	mResetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET);
	Event *nextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	mStepStartedEvent = em->createEvent("TriggerPlaybackEvent", "Triggered right before a new step is executed...");
	if(nextStepEvent != 0) {
		nextStepEvent->addUpstreamEvent(mStepStartedEvent);
	}
	
	mStepCompletedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);

	ValueManager *vm = Core::getInstance()->getValueManager();
	
	mCurrentStep = vm->getIntValue(SimulationConstants::VALUE_EXECUTION_CURRENT_STEP);
	mPhysicsDisabled = vm->getBoolValue(SimulationConstants::VALUE_DISABLE_PHYSICS);
	mSimulationPaused = vm->getBoolValue(SimulationConstants::VALUE_EXECUTION_PAUSE);

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
	if(mSimulationPaused == 0) {
		Core::log("SimulationRecorder: Could not find Value [" + SimulationConstants::VALUE_EXECUTION_PAUSE  + "]", true);
	}
	
	

	return ok;
}


bool SimulationRecorder::cleanUp() {
	if(mExecutionMode == SIMREC_RECORDING) {
		stopRecording();
	}
	else if(mExecutionMode == SIMREC_PLAYBACK) {
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
		
		//If the step was executed after a one-step trigger during pause mode, then recover pause mode.
		if(mSimulationPaused != 0 && !mSimulationPaused->get() && mWasPaused) {
			mSimulationPaused->set(mWasPaused);

			if(mExecutionMode == SIMREC_PLAYBACK) {
				mStepCompletedEvent->removeEventListener(this);
			}
			
			mWasPaused = false;
		}
		
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
		if(Core::getInstance()->isInitialized()) {
			if(mActivateRecording->get() == true) {
				//startRecording();
				Core::getInstance()->scheduleTask(new StartRecordingTask(this, true, true));
			}
			else {
				//stopRecording();
				Core::getInstance()->scheduleTask(new StartRecordingTask(this, true, false));
			}
		}
	}
	else if(value == mActivatePlayback) {
		if(mActivatePlayback->get() == true) {
			//startPlayback();
			Core::getInstance()->scheduleTask(new StartRecordingTask(this, false, true));
		}
		else {
			//stopPlayback();
			Core::getInstance()->scheduleTask(new StartRecordingTask(this, false, false));
		}
	}
	else if(value == mDesiredFrameValue && mDesiredFrameValue->get() > 0) {
		
		//If the simulation is paused, execute a single step only to apply the settings
		//of the recovered data frame (see eventOccured() for pause recovery).
		
		if((mExecutionMode == SIMREC_PLAYBACK) && mSimulationPaused != 0) {
			mWasPaused = mWasPaused | mSimulationPaused->get();
			mSimulationPaused->set(false);
			mStepCompletedEvent->addEventListener(this);
		}
	}
}


/**
 * Prepares the recorder for recording.
 * If the recorder is in playback mode, then playback is stopped.
 * Prior to the recording, several files are stored to allow a recovery of the recording conditions.
 * This includes the environment files, the executed network, etc.
 * Also, a _info file is created that contains information about the content of the data file. 
 * DO NOT CHANGE THAT FILE if you're not sure what you're doing as it is also used to assign values during playback.
 * For the data, a new, unique file is opened and initialized with a version id. You can only load data files with 
 * the proper version number (so you need a matching NERD release with the correct data format. This may change over time!
 * 
 * The actual recording is then done with recordData() at every simulation step.
 */
void SimulationRecorder::startRecording() {

	if(mExecutionMode == SIMREC_PLAYBACK) {
		stopPlayback();
	}
	if(mStepCompletedEvent == 0) {
		Core::log("SimulationRecorder: Cannot record without Event [" + NerdConstants::EVENT_EXECUTION_STEP_COMPLETED + "]", true);
		return;
	}
	mStepCompletedEvent->addEventListener(this);
	mResetEvent->addEventListener(this);
	
	if(mFile != 0) {
		stopRecording();
	}
	
	mFrameNumber = 0;
	
	Core::getInstance()->enforceDirectoryPath(mRecordingDirectory->get());
	
	//create file (find a unique name with the desired prefix)
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
		stopRecording();
		return;
	}
	
	mPlaybackFile->set(mFile->fileName());
	
	Core::log("Starting data recording to file [" + mFile->fileName() + "]! ", true);
	
	//get all values to record...
	updateListOfRecordedValues();
	
	//update the string list of value names.
	updateRecordedValueNameList();
	
	//write the info file that contains the information on the value mapping!
	QFile infoFile(file.fileName().append("_info.txt"));
	if(!infoFile.open(QIODevice::WriteOnly)) {
		Core::log(QString("Could not open infor file ").append(infoFile.fileName()).append("."), true);
		infoFile.close();
		stopRecording();
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
	
	mExecutionMode = SIMREC_RECORDING;
	mStepCounter = 0;
	
}


/**
 * Stops the recording and finalizes the data file.
 */
void SimulationRecorder::stopRecording() {
	if(mStepCompletedEvent == 0 || mFile == 0) {
		return;
	}
	
	if(mFile != 0) {
		Core::log("Stopping data recording! ", true);
	}
	
	mStepCompletedEvent->removeEventListener(this);
	mResetEvent->removeEventListener(this);
	
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
	
	mExecutionMode = SIMREC_OFF;
}


/**
 * Records a full data frame of the simulation. If forceRecording is true, then the frame is definively recorded. 
 * Otherwise, a recodring takes only place according to the recording policy, e.g. every nth time frame.
 * Data format: frameNumber | currentStep | frameSize | data... | frameSize
 * See the dataformat description for the full format.
 */
void SimulationRecorder::recordData(bool forceRecording) {
	if(mExecutionMode != SIMREC_RECORDING) {
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
	
	//write the number of bytes before and after the block to allow seeking backwards.
	mFileDataStream << ((uint) mFrameNumber);
	mFileDataStream << ((qint32) mCurrentStep->get());
	mFileDataStream << ((uint) mData.size());
	mFileDataStream.writeRawData(mData.data(), mData.size());
	mFileDataStream << ((uint) mData.size());
	
	++mFrameNumber;

	delete mDataStream;
	mDataStream = 0;
}


/**
 * Starts to playback a data file.
 * If the recorder is still in recording mode, then that mode is switched off first.
 * 
 * To playback a data file, you need the exact same configuration of the NERD experiment
 * (network, environment, scripts, etc.) and you require the _info file next to the data file.
 * The needed objects in the system are listed in _info. 
 * 
 * Before showing the first frame, the number of frames and the range of the simulation time
 * are summarized. If this makes problems, try to use the slower PlaybackSafeMode (for currupt files).
 */
bool SimulationRecorder::startPlayback() {
	if(mExecutionMode == SIMREC_RECORDING) {
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
	
	mExecutionMode = SIMREC_PLAYBACK;
	
	mReachedAndOfFile = true;
	
	mFirstPlaybackStep = true;
	mReadStepNumber = false;
	
	return true;
}

/**
 * Stops the playback mode. 
 * The simulation continues as before, continuing with the last simulation step count of the playback.
 */
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
	
	mExecutionMode = SIMREC_OFF;
	
	return true;
}


/**
 * Plays back a single data frame.
 * To allow simulations in real-time, the 'missing' simulation steps between two recorded data frames
 * are still executed as normal, hereby NOT updating the data until the next valid frame is required.
 */
void SimulationRecorder::playbackData() {
	if(mExecutionMode != SIMREC_PLAYBACK) {
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
		
		//seek forwards or backwards to find the correct frame.
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
				mFile->seek(mFile->pos() - size - sizeof(step) - (2 * sizeof(size)) - sizeof(frameNumber));
				mFileDataStream >> frameNumber;
				mFileDataStream >> step; //step number
				
			} while(((int) frameNumber) != mDesiredFrameValue->get() && !mFileDataStream.atEnd());
		}
		
		mDesiredFrameValue->set(-1);
		
		if(mFileDataStream.atEnd()) {
			mReachedAndOfFile = true;
			Core::log("Seeked frame is the end of the data stream!", true);
			return;
		}
		
		//mark this frame / step as already read.
		mReadStepNumber = true;
		mStepCounter = step;
		mFrameNumber = frameNumber;
		mCurrentStep->set(mStepCounter);
	}
	
	//check if the new frame should be applied...
	if(!mReadStepNumber) {
		mFileDataStream >> mFrameNumber;
		
		qint32 step = 0;
		mFileDataStream >> step;
		mStepCounter = step;
		mReadStepNumber = true;
	}
	
	//ckeck if we have to wait for the currect step to come...
	if(!mFirstPlaybackStep && (mStepCounter > mCurrentStep->get())) {
		return;
	}
	
	
	//Read and apply the next frame
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
	
	
	//recover the data of the single frame
	updatePlaybackData(*mDataStream);
	
	delete mDataStream;
	mDataStream = 0;
	delete content;
	
}


/**
 * Creates the a list with all matching (found) values to record and stores this
 * in mRecordedValueNameList, that can be viewed in the graphical PropertyPanel.
 * */
void SimulationRecorder::updateRecordedValueNameList() {
	mRecordedValueNameList->set("Number of Values: " 
					+ QString::number(mRecordedValueNames.size()) 
					+ "\n\n" + mRecordedValueNames.join("\n"));
}


/**
 * Interprets the regular expressions and keywords in mObservedValues and derives all
 * matching values from this info. Each subclass of SimulationRecorder can contribute
 * own keywords and descriptions to specify the values to record.
 * The SimulationRecorder handles all values that can be represented in the GVR (PropertyPanel).
 * 
 * In addition to classical regular expressions, this method also accepts then following keywords:
 * - [Interfaces] Selects all inputs and outputs of all AgentInterfaces (motors and sensors).
 * 
 * If a value is a MultiPartValue, then each part of the value is added separately.
 */
void SimulationRecorder::updateListOfRecordedValues() {
	
	//Store environment files.
	StringValue *args = Core::getInstance()->getValueManager()->getStringValue("/CommandLineArguments/installScriptedModel");
	QStringList environmentFiles = Util::getItemsFromCurlyBraceSeparatedString(args->get());
	for(int i = 0; i < environmentFiles.size(); ++i) {
		QFile envFile("./" + environmentFiles.at(i));
		envFile.copy(mFile->fileName().append("_").append(environmentFiles.at(i)));
	}
	
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


/**
 * Matches the value names given in the _info file with the available data values.
 * This function is used during playback to map the recovered data to actual values.
 * 
 * Subclasses of SimulationRecorder have to extend this when they also extend method
 * updateListOfRecordedValues() to allow a proper identification of the values found in 
 * that method.
 */
void SimulationRecorder::syncWithListOfRecordedValues() {
	
	mRecordedValueNames.clear();
	mRecordedValues.clear();
	
	
	//Open the _info file
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
	
	//Interpret all lines that start with a [P] (property) or [M] (multi-part value)
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


/**
 * Write the data for the current data frame to a file.
 * Dataformat: numberOfDoubleValues | data...
 * 
 * Subclasses of the SimulationRecorder can extend this with custom data acquisitions.
 * 
 * See the dataformat description for the full format.
 */
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

/**
 * Recovers the data from the current data frame.
 * The format and values have to correspond to the data stored in updateRecordedData().
 * 
 * Subclasses of the SimulationRecorder have to provide a matching mapping to
 * fit the added data in updateRecordedData().
 */
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


/**
 * Writes the current _info file, containing information on the recorded data.
 * Most importantly, this file contains a list with all recorded value objects
 * with markers of their type.
 */
void SimulationRecorder::writeInfoFile(QTextStream &dataStream) {
	
	dataStream << QString("Date: " + QDate::currentDate().toString("yy-MM-dd") + " at "
			+ QTime::currentTime().toString("hh-mm-ss") + "\n\n");
	dataStream << QString("Data File: " + mPlaybackFile->get() + "\n\n");	
	dataStream << QString("Command: " + QCoreApplication::arguments().join(" ") + "\n\n");
	dataStream << QString("Parameter Search Pattern:\n\n" + mObservedValues->get() + "\n\n");
	dataStream << QString("Parameters: \n\n[Params]\n" + mRecordedValueNameList->get() + "\n[/Params]\n\n");
}


}


