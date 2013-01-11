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

#include "Core/Core.h"
#include "Physics/Physics.h"
#include "PlugIns/SimpleObjectFileParser.h"
#include "Evaluation/EvaluationLoop.h"
#include "Event/Event.h"
#include "Player/Player.h"


#include "OptimizationManager.h"
#include "OrcsModelOptimizerConstants.h"
#include "NerdConstants.h"
#include "Tools.h"
#include "XMLConfigLoader.h"
#include "Optimizer/ModelOptimizer.h"
#include "TrainSet.h"

#include <QDir>
#include <QCoreApplication>

using namespace std;

namespace nerd 
{ 
  
/**
 * Creates a new OptimizatioManager which is configured with the given configuration file.
 *
 * @param xmlConfgFile Path and filename of the XML-Configuation file.
 */
OptimizationManager::OptimizationManager(const QString &xmlConfigFile) 
  : mXMLConfigLoader(0),
    mEvaluationLoop(0),
		mIsConfigError(false),
		mDoShutDown(false),
		mShutDownEvent(0),
    mScenarioVal(0),
		mRunOptimizationValue(0),
		mPauseOptimizationValue(0),
		mShutDownAfterOptimization(0),
		mTrainSet(0)

{
  connect(this, SIGNAL(quitMainApplication()),
  QCoreApplication::instance(), SLOT(quit()));
  
  mXMLConfigLoader = new XMLConfigLoader(xmlConfigFile,
                                         OrcsModelOptimizerConstants::CONFIG_FOLDER +
                                         OrcsModelOptimizerConstants::FILE_CONFIG_DESCRIPTION);
   
  
  // initialize the values from the configuration file
  if(!mXMLConfigLoader->init()){
    mIsConfigError = true;
    return;
  }
  
  if(!mXMLConfigLoader->loadFromXML()) {
    mIsConfigError = true;
    return;
  }
  
  // load needed values from the configuration
  ValueManager * vm = Core::getInstance()->getValueManager();
  
	
  //QString agentPath = OrcsModelOptimizerConstants::VM_BASE_PATH +
  //                    OrcsModelOptimizerConstants::VM_AGENT_VALUE;
  QString scenarioPath = OrcsModelOptimizerConstants::VM_BASE_PATH + 
                         OrcsModelOptimizerConstants::VM_INPUTDATA_PATH + 
                         OrcsModelOptimizerConstants::VM_SCENARIO_VALUE;
  
  //mAgentVal = vm->getStringValue( agentPath );
  mScenarioVal = vm->getStringValue( scenarioPath );
	 
  if(mScenarioVal == 0) {
    Core::log("OptimizationManager: ERROR - Could not find required Value \"" + 
        scenarioPath + "\"!");
    mIsConfigError = true;
    return;
  }
	
	
  
  // create needed components which can not be loaded at init - phase, because
  // they are SystemObjects which must exist at the init - phase
	
	
	QString playerPath = 	OrcsModelOptimizerConstants::VM_BASE_PATH +
												OrcsModelOptimizerConstants::VM_PLAYER_PATH;
	
	Player* player = Player::createPlayer( playerPath );
	
	if(player == 0){
		mIsConfigError = true;
	}
  
	
  mEvaluationLoop = new EvaluationLoop(); 
  
  //generate file parser and set environment xml which should be loaded
  new SimpleObjectFileParser();
  QString loadEnvironmentPath = "/CommandLineArguments/loadEnvironment";
  StringValue * loadEnvironmentVal = vm->getStringValue(loadEnvironmentPath);
  if(loadEnvironmentVal == 0) {
    Core::log("OptimizationManager: ERROR - Could not find required Value \"" + 
        loadEnvironmentPath + "\" to set the environment XML file! " + 
        "Maybe the SimpleObjectFileParser was changed.");
    mIsConfigError = true;
    return;
  }
  
  QDir scenarioDir( Tools::toPathString(mScenarioVal->get()) );
  
  if(!scenarioDir.exists()){
    Core::log(QString("OptimizationManager: ERROR - Can not find scenario folder [%1]!").arg(
              Tools::toPathString(mScenarioVal->get())));
    mIsConfigError = true;
  }
  else
  {
    if(loadEnvironmentVal->get() == "")
    {
      QString environmentFileStr = "*" + 
          OrcsModelOptimizerConstants::FILE_ENDING_ENVIRONMENT;
      
      // set namefilter to search environment file
      scenarioDir.setNameFilters(QStringList(environmentFileStr));
  
      if( scenarioDir.count() != 1 ){
        if(scenarioDir.count() > 1) {
          Core::log(QString("OptimizationManager: ERROR - Too many environment files (\"%1\") in the scenario folder [%2]!").arg(environmentFileStr, scenarioDir.path()));
        }
        else {
          Core::log(QString("OptimizationManager: ERROR - No environment file (\"%1\") in the scenario folder [%2]!").arg(environmentFileStr, scenarioDir.path()));
        }
        mIsConfigError = true;
      }
      else
      {
        loadEnvironmentVal->set( "{" + scenarioDir.filePath(scenarioDir[0]) + "}");
      }
    }
  }
  

  
  Core::getInstance()->addSystemObject(this);
}



OptimizationManager::~OptimizationManager()
{
  Core::getInstance()->deregisterThread(this);
}

QString OptimizationManager::getName() const 
{
  return "OptimizationManager";
}

/**
 * Checks if the initiaization in the constructor of the OptimizationManager
 * was successfully. 
 * Further more it initializes the RunOptimization and PauseOptimization Value
 * if they do not exist with default values that the optimization starts directly 
 * after the start() or run() method is called. 
 * Defaultvalues: RunOptimization = true; PauseOptimization= false.
 * init
 * Note: Automatically called by the Core.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */  
bool OptimizationManager::init() 
{ 
  bool ok = true;
  
  if(mIsConfigError == true) {
    return false;
  }
  
  ///////////////////////////////////////////////////////////
  // Initialize the Control Values
  ///////////////////////////////////////////////////////////

  ValueManager * vm = Core::getInstance()->getValueManager();
  
  mRunOptimizationValue = Tools::getOrAddBoolValue(vm, 
                                        OrcsModelOptimizerConstants::VM_RUNOPTIMIZATION_VALUE,
                                        false,
                                        this->getName(),
                                        ok,
                                        true);
    
  mPauseOptimizationValue = Tools::getOrAddBoolValue(vm, 
                                        OrcsModelOptimizerConstants::VM_PAUSEOPTIMIZATION_VALUE,
                                        false,
                                        this->getName(),
                                        ok,
                                        false);
  
  mShutDownAfterOptimization = Tools::getOrAddBoolValue(vm, 
                                        OrcsModelOptimizerConstants::VM_SHUTDOWNAFTEROPTIMIZATION_VALUE,
                                        false,
                                        this->getName(),
                                        ok,
                                        false);
  
  return ok;
}

/**
 * Checks if the initiaization in the constructor of te OptimizationManager
 * was successfully.
 * Registers furthermore for the mShutDownEvent event with the name
 * OrcsConstans::EVENT_NERD_SYSTEM_SHUTDOWN.
 * 
 * Note: Automatically called by the Core.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool OptimizationManager::bind() 
{
  bool ok = true;
  
  if(mIsConfigError == true) {
    return false; 
  }
  
  EventManager *em = Core::getInstance()->getEventManager();

  mShutDownEvent = em->registerForEvent(NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this);

  if(mShutDownEvent == 0) {
    Core::log(QString("OptimizationManager: ERROR - Required Event [%1] could not be found!").arg(NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN));
    ok = false;
  }
  
  // get intial snapshot of the simulator which will be loaded everytime before
  // a new optimization run is started
  mInitialSnapshot = Tools::saveSnapshot();
  
  return ok;
}

/**
 * Initializes the TrainSet and the ModelOptimizer. Loads furhtermore the 
 * value parameter files from the scenario folder.
 * 
 * Deletes old componentes if they exist. 
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool OptimizationManager::initializeOptimizationRun()
{
  bool ok = true;
    
  if(mIsConfigError == true)
    return false;
  
  ValueManager * vm = Core::getInstance()->getValueManager();
  
  // destroy at first old components if they exist
  this->destroyOptimizationComponents();
    
  // reset simulation to initial configuration
  Tools::loadSnapshot(mInitialSnapshot);
  
  ///////////////////////////////////////////////////////////
  // Load the value parameter files from the scenario folder
  /////////////////////////////////////////////////////////// 
  
  QDir scenarioDir(Tools::toPathString( mScenarioVal->get() ));
  
  QString valueFileStr = "*" +
      OrcsModelOptimizerConstants::FILE_ENDING_VALUE_PARAMETER;
    
  // set namefilter to search value parameter file
  scenarioDir.setNameFilters(QStringList(valueFileStr));
  scenarioDir.refresh();
          
  for(uint i = 0; i < scenarioDir.count(); i++)
  {
    ok &= vm->loadValues(scenarioDir.filePath(scenarioDir[i]), false);
  }
  
  mOptimizationRunSnapshot = Tools::saveSnapshot();
  
  ///////////////////////////////////////////////////////////
  // Initialize the TrainSet
  ///////////////////////////////////////////////////////////
      
  mTrainSet = new TrainSet(mEvaluationLoop);
  
  if(!mTrainSet->init())
    return false;
  
  ///////////////////////////////////////////////////////////
  // Initialize the ModelOptimizers
  ///////////////////////////////////////////////////////////
     
  // read the name of the optimizer
  QString optimizerNamePath = OrcsModelOptimizerConstants::VM_BASE_PATH +
      OrcsModelOptimizerConstants::VM_OPTIMIZER_PATH +
      OrcsModelOptimizerConstants::VM_NAME_VALUE;
  
  StringValue * optimizerNameVal = vm->getStringValue( optimizerNamePath );
   
  if(optimizerNameVal == 0) {
    Core::log("OptimizationManager: ERROR - Could not find required Value \"" + 
        optimizerNamePath + "\"!");
    return false;
  }
      
  // create string to access models
  QString modelPathTemplate =
      OrcsModelOptimizerConstants::VM_BASE_PATH +
      OrcsModelOptimizerConstants::VM_OPTIMIZER_PATH + 
      OrcsModelOptimizerConstants::VM_MODEL_LIST;    
  
  // iterate over models by checking if the name value for a model exists
  int i = 1;
  QString modelPath = modelPathTemplate.arg(i);
  StringValue * modelNameVal = vm->getStringValue(modelPath 
      + OrcsModelOptimizerConstants::VM_NAME_VALUE);
  
  while(modelNameVal != 0)
  {
     ModelOptimizer *optimizer = ModelOptimizer::createModelOptimizer(optimizerNameVal->get(),
        modelPath, 
        mTrainSet);
    
    // break up if the optimizer could not be created
    if(optimizer == 0){
      ok = false;
      break;
    }
    
    ok &= optimizer->init();
    
    mModelOptimizerVector.push_back( optimizer ); 
    
    // try to get next model
    i++;
    modelPath = modelPathTemplate.arg(i);
    modelNameVal = vm->getStringValue(modelPath + OrcsModelOptimizerConstants::VM_NAME_VALUE);
  }

  return ok;
}

void OptimizationManager::destroyOptimizationComponents()
{
  for(int i = 0; i < mModelOptimizerVector.count(); i++)
  {
    if(mModelOptimizerVector.at(i) != 0) {
      delete mModelOptimizerVector.at(i);
    }
    mModelOptimizerVector.clear();
  }
  
  if(mTrainSet != 0) {
    delete mTrainSet;
    mTrainSet = 0;
  }
}

/**
 * Called by the ORCS System if an event occured.
 * Checks if the event NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN is triggered
 * and sets the value mDoShutDown to true if that happens.
 * 
 * @param event Event which was triggered.
 */
void OptimizationManager::eventOccured(Event *event)
{
  if(event == 0) {
    return;
  }
  else if(event == mShutDownEvent) {
    mDoShutDown = true;
  }
}

/**
 * Delete all loaded components by calling destroyOptimizationComponents().
 * 
 * Note: Automatically called by the Core.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool OptimizationManager::cleanUp() 
{
  this->destroyOptimizationComponents();
  
  if(mXMLConfigLoader != 0) {
    delete mXMLConfigLoader;
    mXMLConfigLoader = 0;
  }

  
  return true;
}

void OptimizationManager::run() 
{ 
  Core::getInstance()->setMainExecutionThread();
  
  while(!mDoShutDown)
  {
    // wait till optimization should run
    while(mRunOptimizationValue == 0 || (!mRunOptimizationValue->get() && !mDoShutDown)) 
    {
      QCoreApplication::instance()->thread()->wait(150);
      Core::getInstance()->executePendingTasks();
    }
    
    if(mDoShutDown) { break; }
    
    // initialize new optimization run
    bool initializeOk = this->initializeOptimizationRun();
    
    if(initializeOk == false){
      mDoShutDown = true;
    }
    
    // start optimizationNerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN
    while(mRunOptimizationValue->get() == true && !mDoShutDown)
    {
      // iterate over the initialized optimizer objects 
      for(int i = 0; i < mModelOptimizerVector.count(); i++)
      {
                
        // load the mOptimizationRunSnapshot to undo changes from the last ModelOptimizer
        Tools::loadSnapshot(mOptimizationRunSnapshot);

        // inform Optimization process about start
        bool isCorrectStart = mModelOptimizerVector.at(i)->callOptimizationStart();
        
        // do optimization with current optimizer
        // till it is finished
        while(isCorrectStart &&
              !mModelOptimizerVector.at(i)->callNextOptimizationStep() && 
              mRunOptimizationValue->get() == true &&
              !mDoShutDown )
        {
          // if pause --> wait
          while(mPauseOptimizationValue->get() == true && 
                mRunOptimizationValue->get() == true &&
                !mDoShutDown)
          {
            QCoreApplication::instance()->thread()->wait(150);
            Core::getInstance()->executePendingTasks();
          }
        }
        
        // inform Optimization process about end
        mModelOptimizerVector.at(i)->callOptimizationEnd();
        
        if(mDoShutDown || !mRunOptimizationValue->get()) { break; }
      }
      
      // stop optimization after all optimizers finished
      mRunOptimizationValue->set(false);
    }
    
    if(mShutDownAfterOptimization->get() == true){
      mDoShutDown = true;
    }
  }
  
  Core::getInstance()->scheduleTask(new ShutDownTask());
  Core::getInstance()->executePendingTasks();
  
  Core::getInstance()->clearMainExecutionThread();
}


} // namespace nerd
