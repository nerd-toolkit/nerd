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

#define PARAM(type, paraObj, paraName) \
	(dynamic_cast<type*>(paraObj->getParameter(paraName)))
#include "SimulationLoop.h"
#include "Value/ValueManager.h"
#include <QCoreApplication>
#include "Physics/Physics.h"
#include "NerdConstants.h"
#include "SimulationConstants.h"
#include <iostream>

using namespace std;

namespace nerd {

SimulationLoop::SimulationLoop() : mSimulationDelay(0), mPauseSimulation(0),
		mNextStepEvent(0), mCompletedStepEvent(0), mShutDownEvent(0), mResetEvent(0), 
		mResetFinalizedEvent(0), mDoShutDown(false), mSimulationState(false),
		mUseRealtimeValue(0), mTimeStepSizeValue(0)
{
	Core::getInstance()->addGlobalObject("SimulationLoop", this);
	ValueManager *manager = Core::getInstance()->getValueManager();

	mSimulationDelay = new IntValue(0);
	manager->addValue("/Simulation/Delay", mSimulationDelay);

	mUseRealtimeValue = new BoolValue(false);
	mUseRealtimeValue->addValueChangedListener(this);
	manager->addValue("/Simulation/RunAtRealtime", mUseRealtimeValue);

	mRealTimeTrigger.moveToThread(QCoreApplication::instance()->thread());
	connect(&mRealTimeTrigger, SIGNAL(timeout()), this, SLOT(realTimeStepExpired()));
	connect(this, SIGNAL(startRealtime(int)), &mRealTimeTrigger, SLOT(start(int)), 
		Qt::QueuedConnection);
	connect(this, SIGNAL(stopRealtime()), &mRealTimeTrigger, SLOT(stop()), 
		Qt::QueuedConnection);

	connect(this, SIGNAL(quitMainApplication()), QCoreApplication::instance(), 
		SLOT(quit()));
}

SimulationLoop::~SimulationLoop(){
}

void SimulationLoop::eventOccured(Event *event){
	if(event == mShutDownEvent) {
		mDoShutDown = true;
	}
}

void SimulationLoop::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mUseRealtimeValue) {
		if(mUseRealtimeValue->get()) {
			emit startRealtime((int) (mTimeStepSizeValue->get() * 1000.0));
		}
		else {
			emit stopRealtime();
		}
	}
	else if(value == mTimeStepSizeValue) {
		if(mUseRealtimeValue->get()) {
			emit startRealtime((int) (mTimeStepSizeValue->get() * 1000.0));
		}
	}
}

QString SimulationLoop::getName() const{
	return "/Execution/SimulationLoop";
}

void SimulationLoop::run() {

	//set the running thread as main execution thread.
	Core::getInstance()->setMainExecutionThread();

	// Initializing Simulation.
	Physics::getPhysicsManager()->resetSimulation();

	while(!mDoShutDown) {
		while(mPauseSimulation->get() && !mDoShutDown) {
			Core::getInstance()->executePendingTasks();
			msleep(100);
		}
		if(mDoShutDown) {
			break;
		}
		if(mUseRealtimeValue->get()) {
			while(!mRealTimeStepExpired && mUseRealtimeValue->get() && !mDoShutDown) {
				mRealTimeMutex.lock();
				mRealTimeWaitCondition.wait(&mRealTimeMutex, 10);
				mRealTimeMutex.unlock();
				Core::getInstance()->executePendingTasks();
			}
			if(mDoShutDown) {
				break;
			}
			mRealTimeStepExpired = false;
			mNextStepEvent->trigger();
			mCompletedStepEvent->trigger();
			Core::getInstance()->executePendingTasks();
		}
		else {
			mNextStepEvent->trigger();
			mCompletedStepEvent->trigger();
			Core::getInstance()->executePendingTasks();
			if(mDoShutDown) {
				break;
			}
			//wait delay ms (in small intercals of at max 100 ms to preserve reactivity.
			int waitedDelayTime = 0;
			int pendingDelay = mSimulationDelay->get();
			if(pendingDelay > 100) {
				pendingDelay = 100;
			}
			while(pendingDelay > 0 && !mDoShutDown) {
				msleep(pendingDelay);
				Core::getInstance()->executePendingTasks();

				waitedDelayTime += pendingDelay;
				pendingDelay = mSimulationDelay->get() - waitedDelayTime;
				if(pendingDelay > 100) {
					pendingDelay = 100;
				}
			}
			
			msleep(mSimulationDelay->get());
		}
		
	}

	Core::getInstance()->clearMainExecutionThread();

	emit stopRealtime();
// 	emit quitMainApplication();
	//QCoreApplication::instance()->quit();
}


bool SimulationLoop::init() {
	
	Core::getInstance()->registerThread(this);

	ValueManager *vm = Core::getInstance()->getValueManager();
	EventManager *eManager = Core::getInstance()->getEventManager();
	mNextStepEvent = eManager->createEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP,
					 "Update the System.");

	mCompletedStepEvent = eManager->createEvent(
		NerdConstants::EVENT_EXECUTION_STEP_COMPLETED,
		"Triggerd AFTER the physic was updated. Can be used by Sensor to calculate "
		"the new sensor value.");
	
	mPauseSimulation = new BoolValue(false);
	if(!vm->addValue(SimulationConstants::VALUE_EXECUTION_PAUSE, mPauseSimulation)) {
		Core::log("SimulationLoop: Required value could not be created ["
				+ SimulationConstants::VALUE_EXECUTION_PAUSE + "]");
		return false;
	}

	return true;
}

bool SimulationLoop::bind() {
	bool bindOk = true;
	ValueManager *vm = Core::getInstance()->getValueManager();
	EventManager *em = Core::getInstance()->getEventManager();

	mNextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	mCompletedStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);
	mShutDownEvent = em->registerForEvent(NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this);
	mResetFinalizedEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED, this);
	mResetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET);

	if(mShutDownEvent == 0 )  {
		Core::log("SimulationLoop: Required Event [" + NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN
				+ "] could not be found.");
		bindOk = false;
	}
	if(mNextStepEvent == 0)  {
		Core::log("SimulationLoop: Required Event [" + NerdConstants::EVENT_EXECUTION_NEXT_STEP
				+ "] could not be found.");
		bindOk = false;
	}
	if(mCompletedStepEvent == 0) {
		Core::log("SimulationLoop: Required Event [" + NerdConstants::EVENT_EXECUTION_STEP_COMPLETED
				+ "] could not be found.");
		bindOk = false;	
	}
	if(mResetEvent == 0) {
		Core::log("SimulationLoop: Required Event [" + NerdConstants::EVENT_EXECUTION_RESET
				+ "] could not be found.");
		bindOk = false;
	}
	if(mResetFinalizedEvent == 0) {
		Core::log("SimulationLoop: Required Event [" + NerdConstants::EVENT_EXECUTION_RESET_COMPLETED
				+ "] could not be found.");	
		bindOk = false;
	}

	mTimeStepSizeValue = vm->getDoubleValue(SimulationConstants::VALUE_TIME_STEP_SIZE);
	if(mTimeStepSizeValue == 0) 
	{
		Core::log("SimulationLoop: Required Value [" + SimulationConstants::VALUE_TIME_STEP_SIZE
				+ "] could not be found.");
		bindOk = false;
	}
	else {
		mTimeStepSizeValue->addValueChangedListener(this);
	}

	return bindOk;
}

bool SimulationLoop::cleanUp() {
	mUseRealtimeValue->removeValueChangedListener(this);
	if(mTimeStepSizeValue != 0) {
		mTimeStepSizeValue->removeValueChangedListener(this);
	}
	
	return true;
}


void SimulationLoop::setSimulationDelay(int mSec) {
	mSimulationDelay->set(mSec);
}

void SimulationLoop::realTimeStepExpired() {
	mRealTimeStepExpired = true;
	mRealTimeWaitCondition.wakeAll();
}

}
