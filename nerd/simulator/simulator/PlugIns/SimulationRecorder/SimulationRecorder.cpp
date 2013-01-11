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



#include "SimulationRecorder.h"
#include "Event/EventManager.h"
#include "Core/Core.h"
#include <iostream>
#include "Value/ValueManager.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include <QStringList>
#include "NerdConstants.h"
#include <QFile>
#include <QDir>


using namespace std;

namespace nerd {

SimulationRecorder::SimulationRecorder()
	: mResetEvent(0), mStepCompletedEvent(0), mPhysicsWasDisabled(false), mFile(0), mDataStream(0), mExecutionMode(0)
{
	Core::getInstance()->addSystemObject(this);

	mActivateRecording = new BoolValue(false);
	mActivatePlayback = new BoolValue(false);
	mRecordingDirectory = new FileNameValue(Core::getInstance()->getConfigDirectoryPath() + "/" + "dataRecording/");
	mFileNamePrefix = new StringValue("rec");
	mPlaybackFile = new FileNameValue("");
	mIncludeSimulation = new BoolValue(true);
	mRecordingInterval = new IntValue(1);
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/DataRecorder/ActivateRecording", mActivateRecording);
	vm->addValue("/DataRecorder/ActivatePlayback", mActivatePlayback);
	vm->addValue("/DataRecorder/FileName", mFileNamePrefix);
	vm->addValue("/DataRecorder/PlaybackFile", mPlaybackFile);
	//vm->addValue("/DataRecorder/IncludeSimulation", mIncludeSimulation);
	vm->addValue("/DataRecorder/RecordingDirectory", mRecordingDirectory);
	vm->addValue("/DataRecorder/RecordingInterval", mRecordingInterval);
	
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
	
	mData.reserve(5000);
	mFileDataStream.setDevice(mFile);
	
	//write data file format version number
	mFileDataStream << ((uint) 1);
	
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
	
	mFileDataStream << mCurrentStep->get();
	
	updateRecordedData(*mDataStream);
	
	//cerr << "writing bytes: " << mData.size() << endl;
	
	mFileDataStream << ((uint) mData.size());
	mFileDataStream.writeBytes(mData.data(), mData.size());
	//mFile->write((const char*) mFrameMarker.data(), mFrameMarker.size() * sizeof(int));
// 	mFile->write((const char*) &length, sizeof(int));
// 	mFile->write((const char*) mData.data(), mData.size() * sizeof(char));

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
	
	mPhysicsWasDisabled = mPhysicsDisabled->get();
	mPhysicsDisabled->set(true);
	
	//get all values to record...
	updateListOfRecordedValues();
	
	mExecutionMode = -1;
	
	mReachedAndOfFile = false;
	
	//read out data file format version number
	uint version = 0;
	mFileDataStream >> version;
	Core::log("Starting data playback from file [" + mFile->fileName() + "]! Data Format: " + QString::number(version), true);
	
	mFirstPlaybackStep = true;
	readStepNumber = false;
	
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
		uint version = 0;
		mFileDataStream >> version;
		readStepNumber = false;
	}
	
	//check if the new frame should be applied...
	if(!readStepNumber) {
		mFileDataStream >> mStepCounter;
		readStepNumber = true;
	}
	
	if(!mFirstPlaybackStep && (mStepCounter > mCurrentStep->get())) {
		return;
	}
	mFirstPlaybackStep = false;
	readStepNumber = false;
	
	mCurrentStep->set(mStepCounter);
	
	uint size = 0;
	mFileDataStream >> size;
	char *content = 0;
	
	mFileDataStream.readBytes(content, size);
	
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



void SimulationRecorder::updateListOfRecordedValues() {
	
	
	mRecordedValues.clear();
	
	PhysicsManager *pm = Physics::getPhysicsManager();
	QList<SimObject*> objects = pm->getSimObjects();
	
	for(QListIterator<SimObject*> i(objects); i.hasNext();) {
		SimObject *obj = i.next();
		
		//collect positions and orientations of all bodies.
		SimBody *body = dynamic_cast<SimBody*>(obj);
		if(body != 0 && body->getDynamicValue()->get()) {
			for(int j = 0; j < 3; ++j) {
				DoubleValue *val = dynamic_cast<DoubleValue*>(body->getPositionValue()->getValuePart(j));
				if(val != 0) {
					mRecordedValues.append(val);
				}
			}
			for(int j = 0; j < 4; ++j) {
				DoubleValue *val = dynamic_cast<DoubleValue*>(body->getQuaternionOrientationValue()->getValuePart(j));
				if(val != 0) {
					mRecordedValues.append(val);
				}
			}
			//Core::log("Recording: " + body->getName() + " now. " + QString::number(mRecordedValues.size()), true);
		}
		
		QList<InterfaceValue*> outputs = obj->getOutputValues();
		QList<InterfaceValue*> inputs = obj->getInputValues();
		
		for(QListIterator<InterfaceValue*> j(outputs); j.hasNext();) {
			mRecordedValues.append(j.next());
		}
		for(QListIterator<InterfaceValue*> j(inputs); j.hasNext();) {
			mRecordedValues.append(j.next());
		}
	}
	
	
}


void SimulationRecorder::updateRecordedData(QDataStream &dataStream) {
	
	dataStream << ((int) mRecordedValues.size());
	
	//cerr << "record: " << mCurrentStep->get() << " with # " << mRecordedValues.size() << endl;
	
	QString s;
	
	for(QListIterator<DoubleValue*> i(mRecordedValues); i.hasNext();) {
		double value = i.next()->get();
		dataStream << value;
		
		//s += QString::number(value) + ",";
	}
	
	//Core::log("Wrote: " + s, true);
}


void SimulationRecorder::updatePlaybackData(QDataStream &dataStream) {
	
	int numberOfValues = 0;
	dataStream >> numberOfValues;
	
	//cerr << "step: " << step << endl;
	
	//QString s;
	
	double val = 0.0;
	int counter = 0;
	for(QListIterator<DoubleValue*> i(mRecordedValues); i.hasNext() && (counter < numberOfValues);) {
		dataStream >> val;
		i.next()->set(val);
		counter++;
		
		//s += QString::number(val) + ",";
	}
	//make sure that the network information can be read afterwards, even when objects are missing.
	for(; counter < numberOfValues; ++counter) {
		dataStream >> val;
	}
	
	//Core::log("Read: " + s, true);
}


}


