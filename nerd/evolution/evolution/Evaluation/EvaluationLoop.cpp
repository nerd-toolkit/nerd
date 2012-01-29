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
#include "EvaluationLoop.h"
#include "Value/ValueManager.h"
#include <QCoreApplication>
#include "NerdConstants.h"
#include "Value/DoubleValue.h"
#include "Core/Core.h"
#include "PlugIns/PlugInManager.h"
#include "Fitness/Fitness.h"
#include "Fitness/FitnessManager.h"
#include "EvolutionConstants.h"
#include "Math/Math.h"
#include "Util/Tracer.h"
#include <iostream>
#include "EvolutionConstants.h"


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {

EvaluationLoop::EvaluationLoop(int numberOfSteps, int numberOfTries) 
	: mNextTryEvent(0), mTryCompletedEvent(0), mNextStepEvent(0),
		mStepCompletedEvent(0), mShutDownEvent(0), mResetEvent(0), mResetFinalizedEvent(0), 
		mCurrentStep(0), mPauseSimulation(0), mNumberOfTries(0), mNumberOfSteps(0), 
		mDoShutDown(false), mTerminateTry(false), mSimulationState(false), mCurrentTry(0), mRunInRealTime(0),
		mIsEvolutionMode(true), mInitialNumberOfTries(numberOfTries),
		mInitialNumberOfSteps(numberOfSteps)
{
	Core::getInstance()->addGlobalObject("EvaluationLoop", this);
	mIsEvolutionModeArgument = new CommandLineArgument(
		"testMode", "test", "",
		"Switches the mode of the EvaluationLoop to test mode. That means that setting" 
		" tries and steps to -1 will result in infinit simulation.",
		0, 0, true);
}

EvaluationLoop::~EvaluationLoop(){
}
		
QString EvaluationLoop::getName() const{
	return "/Execution/EvaluationLoop";
}

bool EvaluationLoop::init() {
	TRACE("EvaluationLoop::init");

	if(mIsEvolutionModeArgument->getParameterValue()->get() != "") {
		mIsEvolutionMode = false;
	}
	ValueManager *vm = Core::getInstance()->getValueManager();
	EventManager *em = Core::getInstance()->getEventManager();

	mNextStepEvent = em->createEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP, 
		"Triggers the next execution step (updating physics, neuralnetworks, "
		"fitnessfunctions etc.");
	mStepCompletedEvent = em->createEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, 
		"Triggered AFTER all Listeners of the next step event did react.");

	Event *preStepCompletedEvent = em->createEvent(
		NerdConstants::EVENT_EXECUTION_PRE_STEP_COMPLETED,
		"Triggered shortly before the StepCompleted Event is triggered.");
	mStepCompletedEvent->addUpstreamEvent(preStepCompletedEvent);

	mNextTryEvent = em->createEvent(EvolutionConstants::EVENT_EXECUTION_NEXT_TRY, 
		"Triggers the next try of the evaluation.");

	mTryCompletedEvent = em->createEvent(EvolutionConstants::EVENT_EXECUTION_TRY_COMPLETED, 
		"Triggered AFTER the current try of the evaluation was finished.");
	
	mTerminateTryEvent = em->getEvent(
		EvolutionConstants::EVENT_EXECUTION_TERMINATE_TRY, true);

	if(mNextTryEvent == 0 || mTryCompletedEvent == 0 || mTerminateTryEvent == 0) {
		Core::log("EvaluationLoop: Required Events could not be found.");
	}

	mNumberOfTriesValue = new IntValue(mInitialNumberOfTries);
	mNumberOfStepsValue = new IntValue(mInitialNumberOfSteps);
	mCurrentTry = new IntValue(0);
	mCurrentStep = new IntValue(0);
	mPauseSimulation = new BoolValue(false);
	
	if(mIsEvolutionMode) {
		mNumberOfTries =  Math::max(mNumberOfTriesValue->get(), 1);
		mNumberOfSteps = Math::max(mNumberOfStepsValue->get(), 1);
	}
	else {
		mNumberOfTries =  -1;
		mNumberOfSteps = mNumberOfStepsValue->get();
	}
	
	if(!vm->addValue(EvolutionConstants::VALUE_EXECUTION_PAUSE, mPauseSimulation)) {
		mPauseSimulation = vm->getBoolValue(EvolutionConstants::VALUE_EXECUTION_PAUSE);
	}
	if(mPauseSimulation == 0) {
		Core::log("EvaluationLoop: Required Value could not be found.");
	}

	if(vm->addValue(
		EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES, mNumberOfTriesValue) 
		== false) 
	{
		Core::log(QString("EvaluationLoop: Required Value [")
			.append(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES)
			.append("] could not be added to the ValueManager."));
	}
	
	if(vm->addValue(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS, mNumberOfStepsValue) == false) {
		Core::log(QString("EvaluationLoop: Required Value [")
			.append(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS)
			.append("] could not be added to the ValueManager."));
	}

	vm->addValue(EvolutionConstants::VALUE_EXECUTION_CURRENT_TRY, mCurrentTry);
	vm->addValue(EvolutionConstants::VALUE_EXECUTION_CURRENT_STEP, mCurrentStep);
	
	return true;
}

bool EvaluationLoop::bind() {
	TRACE("EvaluationLoop::bind");


	bool bindOk = true;
	EventManager *em = Core::getInstance()->getEventManager();
	mShutDownEvent = em->registerForEvent(NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this);
	mResetFinalizedEvent = em->registerForEvent(
		NerdConstants::EVENT_EXECUTION_RESET_COMPLETED, this);
	mResetEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_RESET, this);
	mTerminateTryEvent =  em->registerForEvent(
		EvolutionConstants::EVENT_EXECUTION_TERMINATE_TRY, this);

	if(mShutDownEvent == 0 
		|| mNextStepEvent == 0 
		|| mStepCompletedEvent == 0
		|| mResetEvent == 0 
		|| mResetFinalizedEvent == 0
		|| mTerminateTryEvent == 0) 
	{
		Core::log("EvaluationLoop: Required Events could not be found.");
		bindOk = false;
	}

	mEvaluationStartedEvent = em->registerForEvent(
		EvolutionConstants::EVENT_EVO_EVALUATION_STARTED, this, false);

	if(mEvaluationStartedEvent == 0) {
		mNumberOfTriesValue->addValueChangedListener(this);
		mNumberOfStepsValue->addValueChangedListener(this);
		if(mIsEvolutionMode) {
			mNumberOfTries =  Math::max(mNumberOfTriesValue->get(), 1);
			mNumberOfSteps = Math::max(mNumberOfStepsValue->get(), 1);
		}
		else {
			mNumberOfTries =  -1;
			mNumberOfSteps = mNumberOfStepsValue->get();
		}
	}
	
	mNextTryEvent->addEventListener(this);
	mTryCompletedEvent->addEventListener(this);
	mStepCompletedEvent->addEventListener(this);

	mRunInRealTime = Core::getInstance()->getValueManager()->getBoolValue(
				EvolutionConstants::VALUE_RUN_SIMULATION_IN_REALTIME);
	
	if(!mIsEvolutionMode && mRunInRealTime != 0) {
		mRunInRealTime->set(true);
		
		
		//TODO hack, undo later!
		if(NerdConstants::HackMode) {
			mRunInRealTime->set(false);
		}
	}

	return bindOk;
}

bool EvaluationLoop::cleanUp() {
	TRACE("EvaluationLoop::cleanUp");

	mNextTryEvent->removeEventListener(this);
	mTryCompletedEvent->removeEventListener(this);
	mStepCompletedEvent->removeEventListener(this);

	if(mEvaluationStartedEvent == 0) {
		mNumberOfTriesValue->removeValueChangedListener(this);
		mNumberOfStepsValue->removeValueChangedListener(this);
	}
	
	return true;
}

void EvaluationLoop::eventOccured(Event *event){
	TRACE("EvaluationLoop::eventOccured");

	if(event == 0) {
		return;
	}
	else if(event == mShutDownEvent) {
		mDoShutDown = true;
	}
	else if(event == mTerminateTryEvent) {
		mTerminateTry = true;
	}
	else if(event == mEvaluationStartedEvent) {
		if(mIsEvolutionMode) {
			mNumberOfTries =  Math::max(mNumberOfTriesValue->get(), 1);
			mNumberOfSteps = Math::max(mNumberOfStepsValue->get(), 1);
		}
		else {
			mNumberOfTries =  -1;
			mNumberOfSteps = mNumberOfStepsValue->get();
		}
	}
	else if(event == mResetEvent && mCurrentStep != 0) {
		
		//TODO hack, remove later!
		if(NerdConstants::HackMode) {
			mNumberOfStepsValue->set(99999999);
		}
		
		mCurrentStep->set(0);
	}
}

void EvaluationLoop::valueChanged(Value *value) {
	TRACE("EvaluationLoop::valueChanged");

	if(value == 0) {
		return;
	}
	if(mEvaluationStartedEvent == 0 && value != 0) {
		if(value == mNumberOfTriesValue) {
			if(mIsEvolutionMode) {
				mNumberOfTries =  Math::max(mNumberOfTriesValue->get(), 1);
			}
			else {
				mNumberOfTries = -1;
			}
		}
		if(value == mNumberOfStepsValue) {
			mNumberOfSteps = Math::max(mNumberOfStepsValue->get(), 1);
		}
	}
}


void EvaluationLoop::executeEvaluationLoop() {
	TRACE("EvaluationLoop::executeEvaluationLoop");

	bool unboundedNumberOfTries = false;
	if(mNumberOfTries == -1) {
		unboundedNumberOfTries = true;
	}

	bool unboundedNumberOfSteps = false;
	if(mNumberOfSteps <= 0) {
		unboundedNumberOfSteps = true;
	}	

	int currentTry = 0;
	mCurrentTry->set(0);

	while((currentTry < mNumberOfTries || unboundedNumberOfTries) && !mDoShutDown) {

		mCurrentTry->set(currentTry++);

		mCurrentStep->set(0);

		Core::getInstance()->executePendingTasks();
		mNextTryEvent->trigger();

		mTerminateTry = false;

		Core::getInstance()->executePendingTasks();

		mResetEvent->trigger();

		Core::getInstance()->executePendingTasks();
		mResetFinalizedEvent->trigger();

		while((mCurrentStep->get() < mNumberOfSteps || unboundedNumberOfSteps) && !mDoShutDown) {

			while(mPauseSimulation->get() && !mDoShutDown) {
				Core::getInstance()->executePendingTasks();
				pause();
			}
			if(mDoShutDown) {
				break;
			}
			mCurrentStep->set(mCurrentStep->get() + 1);

			mNextStepEvent->trigger();
			Core::getInstance()->executePendingTasks();

			if(mDoShutDown) {
				break;
			}

			mStepCompletedEvent->trigger();
			Core::getInstance()->executePendingTasks();

			performWait();	

			if(mDoShutDown || mTerminateTry) {
				break;
			}
		}

		if(mDoShutDown) {
				break;
		}
		mTryCompletedEvent->trigger();

	}
}

void EvaluationLoop::setClusterMode(bool isClusterMode) {
	mIsEvolutionMode = isClusterMode;
}

void EvaluationLoop::performWait() {
	//QCoreApplication::instance()->thread()->wait(100);
}


void EvaluationLoop::pause() {
	QCoreApplication::instance()->thread()->wait(100);
}

}
