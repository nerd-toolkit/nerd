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

#include "EvaluationLoopExecutor.h"
#include "Value/ValueManager.h"
#include <iostream>
#include <QCoreApplication>
#include "NerdConstants.h"
#include "EvolutionConstants.h"
#include "Value/DoubleValue.h"
#include "Core/Core.h"
#include "PlugIns/PlugInManager.h"
#include "Fitness/Fitness.h"
#include "Fitness/FitnessManager.h"
#include "EvolutionConstants.h"

using namespace std;

namespace nerd{


EvaluationLoopExecutor::EvaluationLoopExecutor() : EvaluationLoop(), mRealTimeSupport(true), 
				mSimulationDelay(0), mUseRealtimeValue(0), mTimeStepSizeValue(0), 
				mNextIndividualEvent(0), mIndividualCompletedEvent(0), mCurrentIndividual(0) 
{

	ValueManager *manager = Core::getInstance()->getValueManager();

	mSimulationDelay = new IntValue(0);
	manager->addValue("/Simulation/Delay", mSimulationDelay);

	mUseRealtimeValue = new BoolValue(false);
	mUseRealtimeValue->addValueChangedListener(this);
	manager->addValue(EvolutionConstants::VALUE_RUN_SIMULATION_IN_REALTIME, mUseRealtimeValue);

	mRealTimeTrigger.moveToThread(QCoreApplication::instance()->thread());
	connect(&mRealTimeTrigger, SIGNAL(timeout()),
			this, SLOT(realTimeStepExpired()));
	connect(this, SIGNAL(startRealtime(int)),
			&mRealTimeTrigger, SLOT(start(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(stopRealtime()),
			&mRealTimeTrigger, SLOT(stop()), Qt::QueuedConnection);

	connect(this, SIGNAL(quitMainApplication()),
			QCoreApplication::instance(), SLOT(quit()));
}

EvaluationLoopExecutor::~EvaluationLoopExecutor(){
	Core::getInstance()->deregisterThread(this);
}
		
void EvaluationLoopExecutor::eventOccured(Event *event){
	EvaluationLoop::eventOccured(event);	
}

void EvaluationLoopExecutor::valueChanged(Value *value) {
	EvaluationLoop::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mUseRealtimeValue && mRealTimeSupport) {
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

QString EvaluationLoopExecutor::getName() const{
	return "/Execution/EvaluationLoopExecutor";
}

void EvaluationLoopExecutor::run() {
	//set the running thread as main execution thread.
	Core::getInstance()->setMainExecutionThread();

	int numberOfIndividuals = mNumberOfIndividuals->get();
	bool unboundedNumberOfIndividuals = false;
	if(numberOfIndividuals == -1) {
		unboundedNumberOfIndividuals = true;
	}

	int currentIndividual = 0;
	mCurrentIndividual->set(0);
	
// 	TODO: make numberOfIndividuals etc. classvariables and update their value on reset
	while((currentIndividual < numberOfIndividuals || unboundedNumberOfIndividuals) 
			&& !mDoShutDown) 
	{
		Core::getInstance()->executePendingTasks();
		mCurrentIndividual->set(currentIndividual++);
		mCurrentTry->set(0);
		mNextIndividualEvent->trigger();
		executeEvaluationLoop();
		mIndividualCompletedEvent->trigger();
	}

	if( !mDoShutDown) {
		if(Core::getInstance()->scheduleTask(new ShutDownTask())) {
		}
		Core::getInstance()->executePendingTasks();
	}
	while(!mDoShutDown) {
		Core::getInstance()->executePendingTasks();
	}

	emit stopRealtime();
// 	emit quitMainApplication();
	//QCoreApplication::instance()->quit();
	Core::getInstance()->clearMainExecutionThread();
}


bool EvaluationLoopExecutor::init() {
	bool ok =  EvaluationLoop::init();

	EventManager *em = Core::getInstance()->getEventManager();
	ValueManager *vm = Core::getInstance()->getValueManager();

	mNextIndividualEvent = em->getEvent(EvolutionConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL, false);
	if(mNextIndividualEvent == 0) {
		mNextIndividualEvent = 
			em->createEvent(EvolutionConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL, 
			"Triggers the start of the evaluation of a new individual.");
	}	

	mIndividualCompletedEvent = 
				em->getEvent(EvolutionConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED);
	if(mIndividualCompletedEvent == 0) {	
		mIndividualCompletedEvent = 
			em->createEvent(EvolutionConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED, 
			"Triggerd after the evaluation of the current individual is finished.");
	}

	if(mNextIndividualEvent == 0) {
		Core::log("EvaluationLoop: Required Event [" 
				+ EvolutionConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL
				+ "] could not be found.");
		ok = false;
	}
	if(mIndividualCompletedEvent == 0) {
		Core::log("EvaluationLoop: Required Event ["
				+ EvolutionConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED
				+ "] could not be found.");
		ok = false;
	}

	mNumberOfIndividuals = new IntValue(1);
	mCurrentIndividual = new IntValue(0);

	if(!vm->addValue(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_INDIVIDUALS, mNumberOfIndividuals)) {
		Core::log("EvaluationLoop: Required Value [" 
			+ EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_INDIVIDUALS 
			+ "] could not be added to the ValueManager.");
		ok = false;
	}
	if(!vm->addValue(EvolutionConstants::VALUE_EXECUTION_CURRENT_INDIVIDUAL, mCurrentIndividual)) {
		Core::log("EvaluationLoop: Required Value [" 
			+ EvolutionConstants::VALUE_EXECUTION_CURRENT_INDIVIDUAL 
			+ "] could not be added to the ValueManager.");
		ok = false;
	}


	Core::getInstance()->registerThread(this);

	return ok;
}

bool EvaluationLoopExecutor::bind() {
	bool bindOk = EvaluationLoop::bind();

	ValueManager *vm = Core::getInstance()->getValueManager();

	mTimeStepSizeValue = vm->getDoubleValue(EvolutionConstants::VALUE_TIME_STEP_SIZE);
	if(mTimeStepSizeValue == 0) {
		mRealTimeSupport = false;
	}
	else {
		mTimeStepSizeValue->addValueChangedListener(this);
	}
	mNextIndividualEvent->addEventListener(this);
	mIndividualCompletedEvent->addEventListener(this);

	return bindOk;
}

bool EvaluationLoopExecutor::cleanUp() {
	EvaluationLoop::cleanUp();

	mUseRealtimeValue->removeValueChangedListener(this);
	if(mTimeStepSizeValue != 0) {
		mTimeStepSizeValue->removeValueChangedListener(this);
	}
	mNextIndividualEvent->removeEventListener(this);
	mIndividualCompletedEvent->removeEventListener(this);

	return true;
}


void EvaluationLoopExecutor::setSimulationDelay(int uSec) {
	mSimulationDelay->set(uSec);
}

void EvaluationLoopExecutor::realTimeStepExpired() {
	mRealTimeStepExpired = true;
	mRealTimeWaitCondition.wakeAll();
}

void EvaluationLoopExecutor::pause() {
	msleep(100);
}

void EvaluationLoopExecutor::performWait() {
	if(mUseRealtimeValue->get() && mRealTimeSupport) {
		while(!mRealTimeStepExpired && mUseRealtimeValue->get() && !mDoShutDown) {
			mRealTimeMutex.lock();
			mRealTimeWaitCondition.wait(&mRealTimeMutex, 10);
			mRealTimeMutex.unlock();
			Core::getInstance()->executePendingTasks();
		}
		mRealTimeStepExpired = false;
	}
	else {
		//wait delay ms (in small intercals of at max 100 ms to preserve reactivity.
		int waitedDelayTime = 0;
		int pendingDelay = mSimulationDelay->get();
		if(pendingDelay > 10000) {
			pendingDelay = 10000;
		}
		while(pendingDelay > 0 && !mDoShutDown) {

			usleep(pendingDelay);
			Core::getInstance()->executePendingTasks();

			waitedDelayTime += pendingDelay;
			pendingDelay = mSimulationDelay->get() - waitedDelayTime;
			if(pendingDelay > 10000) {
				pendingDelay = 10000;
			}
		}
		
		//msleep(mSimulationDelay->get());
	}

}

}
