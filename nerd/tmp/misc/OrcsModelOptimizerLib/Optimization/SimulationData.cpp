/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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

#include "SimulationData.h"


#include <QString>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QList>

#include "OrcsModelOptimizerConstants.h"
#include "NerdConstants.h"
#include "SimulationConstants.h"
#include "Core/Core.h"
#include "Value/StringValue.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"
#include "Physics/Physics.h"
#include "PlugIns/KeyFramePlayer.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Evaluation/EvaluationLoop.h"
#include "EvolutionConstants.h"
#include "Player/Player.h"

#include "Trigger.h"
#include "Normalization.h"
#include "Tools.h"


namespace nerd
{

SimulationData::SimulationData(const QString &motionConfigPath,
                                 EvaluationLoop *evaluationLoop,
                                 Trigger *trigger)
  : mMotionConfigPath(motionConfigPath),
 		mMotionName(0),
    mScenarioPath(0),
    mTrigger(trigger),
		mTriggerNormalization(0),
    mIsTriggered(false),
		mTriggerValue(0),
    mPlayer(0),
		mPlayerSource(0),
		mEvaluationLoop(evaluationLoop),
		mSimNumberOfTries(0),
		mSimNumberOfSteps(0),
		mStepCompletedEvent(0),
		mTryCompletedEvent(0),
		mNextTryEvent(0),
		mTerminateTryEvent(0),
		mStepsFromTriggerOnset(0),
		mIsFirstSimStep(true),
		mCurMotionLengthInSimSteps(0)
{
}

SimulationData::~SimulationData()
{
  for(int i = 0; i < mData.size(); i++){
    delete mData[i];
  }
  mData.clear();

  for(int i = 0; i < mNormalizations.count(); i++)
  {
    if(mNormalizations.at(i) != 0)
      delete mNormalizations.at(i);
  }
  mNormalizations.clear();

  if(mTriggerNormalization != 0){
    delete mTriggerNormalization;
    mTriggerNormalization = 0;
  }
}

/**
 * Initializes the SimulationData object.
 * Reads the needed information from the associated motion via the ValueManager init()
 * and checks if alle needed files (motion description, environment parameter) exist.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool SimulationData::init()
{
  bool ok = true;

  ValueManager * vm = Core::getInstance()->getValueManager();

  ///////////////////////////////////////////////
  // Load Values
  ///////////////////////////////////////////////

  // load the filenames of the motion description file and the environment value file
  mMotionName = Tools::getStringValue(vm,
                                      mMotionConfigPath +
                                          OrcsModelOptimizerConstants::VM_NAME_VALUE,
                                      Tools::EM_ERROR,
                                      getName(),
                                      ok);

  mScenarioPath = Tools::getStringValue(vm,
                                      OrcsModelOptimizerConstants::VM_BASE_PATH +
                                          OrcsModelOptimizerConstants::VM_INPUTDATA_PATH +
                                          OrcsModelOptimizerConstants::VM_SCENARIO_VALUE,
                                      Tools::EM_ERROR,
                                      getName(),
                                      ok);

  // the motion name and scenario path are needed to generate filepaths
  // --> if no values defined: stop initialization
  if(ok == false)
    return ok;
	
	mPlayerSource =  Tools::getStringValue(	vm,
																				 	mMotionConfigPath +
                                          	OrcsModelOptimizerConstants::VM_PLAYERSOURCE_VALUE,
                                     			Tools::EM_ERROR,
                                      		getName(),
                                      		ok);


  mSimNumberOfTries = Tools::getIntValue(vm,
                                     EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES,
                                     Tools::EM_ERROR,
                                     getName(),
                                     ok);

  mSimNumberOfSteps = Tools::getIntValue(vm,
                                         EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS,
                                         Tools::EM_ERROR,
                                         getName(),
                                         ok);

  // load list of traindata values
  QString trainDataValuePathTemplate =  OrcsModelOptimizerConstants::VM_BASE_PATH +
                                        OrcsModelOptimizerConstants::VM_TRAINDATA_PATH +
                                        OrcsModelOptimizerConstants::VM_VALUE_LIST;

  QString standartNormalizationPath = OrcsModelOptimizerConstants::VM_BASE_PATH +
      OrcsModelOptimizerConstants::VM_TRAINDATA_PATH +
      OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
      OrcsModelOptimizerConstants::VM_SIMULATIONDATA_PATH;

  QString specialNormalizationPath = trainDataValuePathTemplate +
      OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
      OrcsModelOptimizerConstants::VM_SIMULATIONDATA_PATH;

  int i = 1;
  StringValue * trainDataValueNameVal =
      vm->getStringValue(trainDataValuePathTemplate.arg(i) +
                         OrcsModelOptimizerConstants::VM_NAME_VALUE);
  while(trainDataValueNameVal != 0)
  {
    DoubleValue *dataValue = Tools::getDoubleValue(vm,
                                              trainDataValueNameVal->get(),
                                              Tools::EM_ERROR,
																							getName(),
                                              ok,
                                              "This value is as TrainData value configured.");

    QStringList normalizationPathList;

    normalizationPathList  += specialNormalizationPath.arg(i);
    normalizationPathList  += standartNormalizationPath;

    Normalization *normalization = new Normalization( normalizationPathList );

    ok &= normalization->init();

    mDataValues.push_back(dataValue);
    mData.push_back(new QVector<double>());
    mNormalizations.push_back(normalization);


    // try to get next value
    i++;
    trainDataValueNameVal = vm->getStringValue(trainDataValuePathTemplate.arg(i) +
                                               OrcsModelOptimizerConstants::VM_NAME_VALUE);
  }

  ///////////////////////////////////////////////
  // Generate Filepaths environment parameters
  ///////////////////////////////////////////////

  QDir motionDir( Tools::toPathString(mScenarioPath->get()) +
                  Tools::toPathString(mMotionName->get()));

  if(!motionDir.exists()){
    Core::log(QString("SimulationData: ERROR - Can not find motion folder [%1]!").arg(
              Tools::toPathString(mScenarioPath->get()) +
              Tools::toPathString(mMotionName->get())));
    ok = false;
  }
  else
  {
		
    QString valueFileStr = "*" +
                     OrcsModelOptimizerConstants::FILE_ENDING_VALUE_PARAMETER;

    // set namefilter to search value parameter file
    motionDir.setNameFilters(QStringList(valueFileStr));
    motionDir.refresh();

    if( motionDir.count() >= 1 )
    {
      mValueParameterFiles.resize(motionDir.count());

      for(uint i = 0; i < motionDir.count(); i++)
      {
        mValueParameterFiles[i] = motionDir.filePath(motionDir[i]);
      }
    }
  }

  ///////////////////////////////////////////////
  // Get needed Events for the simulation
  ///////////////////////////////////////////////

  EventManager *em = Core::getInstance()->getEventManager();

  mStepCompletedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);
  if(mStepCompletedEvent == 0) {
		Core::log("SimulationData: ERROR - Could not find required Event [EVENT_EXECUTION_STEP_COMPLETED].");
    ok = false;
  }

  mNextTryEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_TRY);
  if(mNextTryEvent == 0) {
		Core::log("SimulationData: ERROR - Could not find required Event [EVENT_EXECUTION_NEXT_TRY].");
    ok = false;
  }

  mTryCompletedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_TRY_COMPLETED);
  if(mTryCompletedEvent == 0) {
		Core::log("SimulationData: ERROR - Could not find required Event [EVENT_EXECUTION_TRY_COMPLETED].");
    ok = false;
  }

  mTerminateTryEvent = em->getEvent(EvolutionConstants::EVENT_EXECUTION_TERMINATE_TRY);
  if(mTerminateTryEvent == 0) {
    Core::log("SimulationData: ERROR - Could not find required Event [EVENT_EXECUTION_TERMINATE_TRY].");
    ok = false;
  }

  ///////////////////////////////////////////////
  // Initialize Trigger value
  ///////////////////////////////////////////////

  mTriggerValue = Tools::getDoubleValue(vm,
                                        mTrigger->getTriggerValueName(),
                                        Tools::EM_ERROR,
                                        getName(),
                                        ok,
                                        "This value is as trigger value configured.");

  ///////////////////////////////////////////////
  // Normalization for the trigger value
  ///////////////////////////////////////////////

  // there are 3 options where the normalization is defined
  QStringList triggerNormalizationPaths;

  // 1) by the special trigger for the current motion
  triggerNormalizationPaths += mMotionConfigPath +
      OrcsModelOptimizerConstants::VM_TRIGGER_PATH +
      OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
      OrcsModelOptimizerConstants::VM_SIMULATIONDATA_PATH;

  // 2) by the standard trigger
  triggerNormalizationPaths += OrcsModelOptimizerConstants::VM_BASE_PATH +
      OrcsModelOptimizerConstants::VM_TRAINDATA_PATH +
      OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
      OrcsModelOptimizerConstants::VM_SIMULATIONDATA_PATH;

  // 3) by the standard normalization for simulation data
  triggerNormalizationPaths += standartNormalizationPath;

  mTriggerNormalization = new Normalization(triggerNormalizationPaths);

  ok &= mTriggerNormalization->init();
	
	///////////////////////////////////////////////
  // Search Player
	///////////////////////////////////////////////
	
	
	QList<SystemObject*> sysObjs = Core::getInstance()->getSystemObjects();
	
	for(int i = 0; i < sysObjs.size(); i++)
	{
		Player *player = dynamic_cast<Player*>(sysObjs.at(i));
		
		if(player != 0) {
			mPlayer = player;
			break;
		}
	}
	
	if(mPlayer == 0){
		Core::log(QString("%1: ERROR - Could not find a player object!").arg(getName()));
		ok = false;
	}

  return ok;
}


QString SimulationData::getName() const {
  return "SimulationData";
}

void SimulationData::setInitialSnapshot(QHash<SimObject*, QHash<Value*, QString> > initialSnapshot)
{
  mInitialSnapshot = initialSnapshot;
}

/**
 * Returns the length of the motion in steps.
 * The value corresponds to the number of data which is collected.
 *
 * @return Length of simulated motion in steps.
 */
int SimulationData::getMotionLength() const
{
  return mCurMotionLengthInSimSteps;
}

/**
 * Returns the number of data channel (orcs values) which are recorded.
 *	if(configPath.right(1) == "/"){
		mConfigPath = configPath;
}
	else {
		mConfigPath = configPath + "/";
}
 * @return Number of recorded channels.
 */
int SimulationData::getChannelCount() const
{
  return mData.count();
}

QString SimulationData::getMotionName() const
{
  return mMotionName->get();
}

QString SimulationData::getChannelName(int channel) const
{
  ValueManager *vm = Core::getInstance()->getValueManager();

  return vm->getNamesOfValue(mDataValues.at(channel)).at(0);
}


bool SimulationData::collectNewData(int lengthInSimSteps)
{
  bool ok = true;

  // set the length of data recording such that the eventOccured() method
  // can access the information
  mCurMotionLengthInSimSteps = lengthInSimSteps;

  // load value parameter files

  // load initial set of values
  Tools::loadSnapshot(mInitialSnapshot);

	
  // load the values defined by the user for the current motion
  ValueManager * vm = Core::getInstance()->getValueManager();
  for(int i = 0; i < mValueParameterFiles.count(); i++)
  {
    ok &= vm->loadValues(mValueParameterFiles.at(i), false);
  }
	
  // fix the loaded values, because the evaluationloop will call a reset
  // for the SimulationEnvironment
  Physics::getSimulationEnvironmentManager()->createSnapshot();

	
    // load motion description file
	ok &= mPlayer->setSource(	Tools::toPathString(mScenarioPath->get())
														+ Tools::toPathString(mMotionName->get())
														+ mPlayerSource->get());
	
	if(ok == false){
		return false;
	}

  // Prepare for simulation run
  mIsTriggered = false;
  mStepsFromTriggerOnset = 0;
  mIsFirstSimStep = true;

  // resize the data vectors
  for(int i = 0; i < mDataValues.size(); i++) {
    mData[i]->resize(mCurMotionLengthInSimSteps);
  }

  // Prepare EvaluationLoop
  mSimNumberOfTries->set(1);

  // number of maximal steps
  mSimNumberOfSteps->set(
    Tools::convertMsecToSimSteps(OrcsModelOptimizerConstants::STANDARD_MAX_MOTION_TRIGGER_ONSET)
    + mCurMotionLengthInSimSteps
                        );

  // register important event
  mStepCompletedEvent->addEventListener(this);
  mTryCompletedEvent->addEventListener(this);
  mNextTryEvent->addEventListener(this);

  // execute simulation
  //  --> the simulation calls then the eventOccured() method
  mEvaluationLoop->executeEvaluationLoop();
	
  // TODO: muss pruefen ob shutdown getriggered wurde --> dannach muss
  // alles abbrechen!!

  // deregister important events
  mStepCompletedEvent->removeEventListener(this);
  mTryCompletedEvent->removeEventListener(this);
  mNextTryEvent->removeEventListener(this);

  //check if the data recording was successfully
  if(!mIsTriggered) {
    Core::log("SimulationData: ERROR - Trigger was not detected for motion ["+
        mMotionName->get() + "]!");
    return false;
  }
  if(mStepsFromTriggerOnset - 1 != this->getMotionLength()) {
    Core::log("SimulationData: ERROR - The number of recorded data is wrong! #" +
        QString::number(this->getMotionLength()) + " should be recorded, but #" +
        QString::number(mStepsFromTriggerOnset) + " is recorded.");
    return false;
  }
	
  return ok;
}

void SimulationData::eventOccured(Event *event)
{
  if(event == mStepCompletedEvent)
  {
    // do not use first step, because key frameplayer inputs at first at the second step
    // key frames
    if(mIsFirstSimStep)
    {
      mIsFirstSimStep = false;
    }
    else
    {
      if(!mIsTriggered){
        mIsTriggered = mTrigger->checkTrigger(
                          mTriggerNormalization->normalize( mTriggerValue->get())
                                              );
      }

      if(mIsTriggered)
      {
        mStepsFromTriggerOnset++;

        if(mStepsFromTriggerOnset <= mCurMotionLengthInSimSteps)
        {
          for(int i = 0; i < mDataValues.size(); i++)
          {
            mData[i]->replace(mStepsFromTriggerOnset - 1,
                              mNormalizations.at(i)->normalize( mDataValues.at(i)->get() ) );
          }
        }
        else {
          mTerminateTryEvent->trigger();
        }
      }
    }
  }
  else if(event == mNextTryEvent)
  {
    //TODO: react on NextTryEvent
  }
  else if(event == mTryCompletedEvent)
  {
    //TODO: react on TryCompletedEvent
  }	
}

double SimulationData::getData(int channel, int simulationStep)
{
  return mData.at(channel)->at(simulationStep);
}


} // namespace nerd
