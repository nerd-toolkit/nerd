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


#include "ModelOptimizer.h"
#include "OrcsModelOptimizerConstants.h"
#include "Optimization/Tools.h"
#include "Optimization/ReportLogger.h"
#include "Optimization/TrainSet.h"
#include "Optimization/MotionData.h"
#include "Optimization/SimulationData.h"
#include "Optimization/PhysicalData.h"
#include "Core/Core.h"
#include "Value/Value.h"
#include "Value/StringValue.h"
#include "Value/DoubleValue.h"
#include "Value/BoolValue.h"
#include "Value/IntValue.h"
#include "Physics/Physics.h"
#include "Statistics/Statistics.h"
#include "Statistics/StatisticsManager.h"
#include "Statistics/StatisticsLogger.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Math/Random.h"


#include <QString>
#include <QStringList>
#include <QtAlgorithms>
#include <QDir>
#include <QSet>
#include <limits>


// includes of all known optimizers
#include "TestModelOptimizer.h"
#include "PrintModelOptimizer.h"
#include "PowellModelOptimizer.h"
#include "CheckDataSourceIntegrity.h"
#include "MuSlashRhoLambdaESOptimizer.h"
#include "CalcErrorFunction.h"

namespace nerd {

/**
   * Creates a new certain model optimizer object. The model optimizer is choosen by
   * his name.
   *
   * @param optimizer Name of the optimizer which should be created.
   * @param model ORCS value path to the model configuration which should be optimized.
   * @param trainSet TrainSet with data which should be used from the optimizer to optmize the
   * model.
   * @return Reference to the new created model optimizer.
 */
ModelOptimizer* ModelOptimizer::createModelOptimizer( const QString &optimizer,
                                                      const QString &modelConfigPath,
                                                      TrainSet *trainSet)
{
	QString typeDef = optimizer.trimmed().toLower();
	
	if(typeDef == OrcsModelOptimizerConstants::MO_TEST_OPTIMIZER.toLower())
    return new TestModelOptimizer(modelConfigPath, trainSet);

	if(typeDef == OrcsModelOptimizerConstants::MO_PRINT_OPTIMIZER.toLower())
    return new PrintModelOptimizer(modelConfigPath, trainSet);

	if(typeDef == OrcsModelOptimizerConstants::MO_POWELL_OPTIMIZER.toLower())
    return new PowellModelOptimizer(modelConfigPath, trainSet);

	if(typeDef == OrcsModelOptimizerConstants::MO_CHECKDATASOURCEINTEGRITY.toLower())
    return new CheckDataSourceIntegrity(modelConfigPath, trainSet);

	if(typeDef == OrcsModelOptimizerConstants::MO_MUSLASHRHOLAMBDAES.toLower())
		return new MuSlashRhoLambdaESOptimizer(modelConfigPath, trainSet);
	
	if(typeDef == OrcsModelOptimizerConstants::MO_CALCERRORFUNCTION.toLower())
		return new CalcErrorFunction(modelConfigPath, trainSet);

  // error if the optimizer is unknown
	Core::log(QString("ModelOptimizer: ERROR - Unknown ModelOptimizer \"%1\"! The following ModelOptimizer exists: \"%2\", \"%3\", \"%4\", \"%5\", \"%6\", \"%7\".").arg(
                 optimizer,
                 OrcsModelOptimizerConstants::MO_TEST_OPTIMIZER,
                 OrcsModelOptimizerConstants::MO_PRINT_OPTIMIZER,
                 OrcsModelOptimizerConstants::MO_POWELL_OPTIMIZER,
                 OrcsModelOptimizerConstants::MO_CHECKDATASOURCEINTEGRITY,
								 OrcsModelOptimizerConstants::MO_MUSLASHRHOLAMBDAES,
				 				 OrcsModelOptimizerConstants::MO_CALCERRORFUNCTION));
  return 0;
}

/**
 * Creates a new ModelOptimizer object.
 *
 * @param model ORCS value path to the model configuration which should be optimized.
 * @param trainSet TrainSet with data which should be used from the optimizer to optmize the model.
 * @param outputDirectory Path to the directory for the output files.
 */
ModelOptimizer::ModelOptimizer(const QString &modelConfigPath,
                               TrainSet *trainSet)
  : mModelConfigPath(modelConfigPath),
    mTrainSet(trainSet),
    mOutputDirectory(0),
    mModelName(0),
    mIsPrintTraindataStatistics(0),
    mOptimizationStepCompleted(0),
    mOptimizationRunCompleted(0),
    mStatisticsLogger(0),
    mReportLogger(0),
    mSimulationRuns(0),
    mMaxSimulationRuns(0),
    mMaxIterations(0),
    mIterationCount(0)
{
  mSimulationRuns = new IntValue(0);
  mIterationCount = new IntValue(0);
}

ModelOptimizer::~ModelOptimizer()
{
  for(int i = 0; i < mToOptimizeParameters.size(); i++)
  {
    if(mToOptimizeParameters.at(i) != 0){
      delete mToOptimizeParameters.at(i);
    }
  }

  if(mSimulationRuns != 0){
    delete mSimulationRuns;
    mSimulationRuns = 0;
  }

  if(mIterationCount != 0){
    delete mIterationCount;
    mIterationCount = 0;
  }
}

/**
 * Initializes the ModelOptimizer by reading the parameters which should be optimized.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool ModelOptimizer::init()
{
  bool ok = true;

  ValueManager *vm = Core::getInstance()->getValueManager();

  // read modelname
  mModelName = Tools::getStringValue(vm,
                                     mModelConfigPath
                                      + OrcsModelOptimizerConstants::VM_NAME_VALUE,
                                     Tools::EM_ERROR,
                                     getName(),
                                     ok);

  mStepCompletedEventName =
      OrcsModelOptimizerConstants::EVENT_MODELOPTIMIZATIONSTEP_COMPLETED.arg(mModelName->get());

  mOptimizationCompletedEventName =
      OrcsModelOptimizerConstants::EVENT_MODELOPTIMIZATION_COMPLETED.arg(mModelName->get());

  mOptimizationStatisticCompletedEventName =
      OrcsModelOptimizerConstants::EVENT_MODELOPTIMIZATION_STATISTIC_COMPLETED.arg(mModelName->get());


  // read outputdirectory or use standart
  mOutputDirectory = Tools::getOrAddStringValue(vm,
                                  OrcsModelOptimizerConstants::VM_BASE_PATH
                                  + OrcsModelOptimizerConstants::VM_OUTPUTDIR_VALUE,
                                  false,
                                  getName(),
                                  ok,
                                  QDir::currentPath() + QDir::separator() + "Results");

  QStringList configPaths;
  configPaths += mModelConfigPath;
  configPaths += OrcsModelOptimizerConstants::VM_BASE_PATH
      + OrcsModelOptimizerConstants::VM_OPTIMIZER_PATH;

  mMaxIterations = Tools::getOrAddIntValue(vm,
                                configPaths,
                                OrcsModelOptimizerConstants::VM_MAXITERATIONS_VALUE,
                                false,
                                getName(),
                                ok,
                                std::numeric_limits<int>::max());

  mMaxSimulationRuns = Tools::getOrAddIntValue(vm,
                                configPaths,
                                OrcsModelOptimizerConstants::VM_MAXSIMULATIONRUNS_VALUE,
                                false,
                                getName(),
                                ok,
                                std::numeric_limits<int>::max());

  mIsPrintTraindataStatistics = Tools::getOrAddBoolValue(vm,
                                configPaths,
                                OrcsModelOptimizerConstants::VM_PRINTTRAINDATASTATISTICS_VALUE,
                                false,
                                getName(),
                                ok,
                                false);

  // Read parameters which should be optimized
  ok &= readToOptimizeParameters();

  // Read initial values and valuefiles
  ok &= readInitialValues();

  // Read values which should be saved if the
  // optimization parameters should be saved
  ok &= readToSaveModelValues();

  //////////////////////////////////////////////
  // Initialize statistic
  /////////////////////////////////////////////

  EventManager *em = Core::getInstance()->getEventManager();
 
  // create Events
  mOptimizationStepCompleted = em->createEvent(mStepCompletedEventName);
  if(mOptimizationStepCompleted == 0){
    ok = false;
  }

  mOptimizationRunCompleted = em->createEvent(mOptimizationCompletedEventName);
  if(mOptimizationRunCompleted == 0){
    ok = false;
  }

  // create statistics
	ok &= appendStatistic(mSimulationRuns, "SimulationRuns");
	ok &= appendStatistic(mIterationCount, "IterationCount");

  mStatisticsLogger = new StatisticsLogger(mOutputDirectory->get(),
                                           getFileNameBegin() + "_Statistics.log",
                                           mOptimizationStatisticCompletedEventName,
                                           mStepCompletedEventName);
  ok &= mStatisticsLogger->init();
  ok &= mStatisticsLogger->bind();


  //////////////////////////////////////////////
  // Initialize report logger
  /////////////////////////////////////////////

  mReportLogger = new ReportLogger(mModelConfigPath + "ReportLogger/");

  ok &= mReportLogger->init();
  ok &= mReportLogger->bind();

  return ok;
}

/**
 * Reads the parameter Values which should be optimized from the configuration
 * and saves them in the mToOptimizeParameters.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool ModelOptimizer::readToOptimizeParameters()
{
  bool ok = true;

  // delete old values
  mToOptimizeParameters.clear();

  ValueManager *vm = Core::getInstance()->getValueManager();

  // iterate over parameters by checking if the name value for a parameter exists
  QString parameterPathTemplate = mModelConfigPath
      + OrcsModelOptimizerConstants::VM_PARAMETER_LIST;

  int i = 1;
  QString parameterPath = parameterPathTemplate.arg(i);
  StringValue *parameterNameVal = vm->getStringValue(parameterPath
      + OrcsModelOptimizerConstants::VM_NAME_VALUE);

  while(parameterNameVal != 0)
  {
    ParameterDescription *paraDescr = createParameterDescription(parameterPath,
                                                                 &ok);
    paraDescr->value = 0;
    paraDescr->name = 0;
    paraDescr->min = 0;
    paraDescr->max = 0;
    paraDescr->init = 0;

    // read the name of the parameter and create a reference to it
    StringValue *curParameterName = Tools::getStringValue(vm,
        parameterPath
            + OrcsModelOptimizerConstants::VM_NAME_VALUE,
            Tools::EM_ERROR,
            getName(),
            ok);

    if(ok == true)
    {
      Value *curParameter = Tools::getValue(vm,
                                            curParameterName->get(),
                                            Tools::EM_ERROR,
                                            getName(),
                                            ok);

      paraDescr->value = curParameter;
      paraDescr->name = curParameterName;
    }

		paraDescr->min = Tools::getOrAddDoubleValue(vm,
        		parameterPath
            + OrcsModelOptimizerConstants::VM_MIN_VALUE,
            false,
            getName(),
            ok,
						std::numeric_limits<double>::min());

    paraDescr->max = Tools::getOrAddDoubleValue(vm,
        		parameterPath
            + OrcsModelOptimizerConstants::VM_MAX_VALUE,
            false,
            getName(),
            ok,
						std::numeric_limits<double>::max());
		
		// check if min and max are correctly set
		if(paraDescr->max->get() < paraDescr->min->get()){
			Core::log(QString("%1: ERROR - The attribute [%2] is smaller than the attribute [%3] for the parameter [%4]!").arg(
																getName(),
																OrcsModelOptimizerConstants::VM_MAX_VALUE,
																OrcsModelOptimizerConstants::VM_MIN_VALUE,
																paraDescr->name->get()));
			ok = false;
		}


    paraDescr->init = Tools::getOrAddStringValue(vm,
        		parameterPath
            + OrcsModelOptimizerConstants::VM_INIT_VALUE,
            false,
            getName(),
            ok,
					 	"default");

    mToOptimizeParameters.push_back(paraDescr);

    // try to get next parameter
    i++;
    parameterPath = parameterPathTemplate.arg(i);
    parameterNameVal = vm->getStringValue(parameterPath
        + OrcsModelOptimizerConstants::VM_NAME_VALUE);
  }

  if(mToOptimizeParameters.size() <= 0){
    Core::log(QString("%1: ERROR - No parameters defined under the configuration path [%2]!").arg(getName(), mModelConfigPath));
    ok = false;
  }

  return ok;
}

/**
 * Reads the initial values from the configuration which should be loaded at the beginning of
 * an optimization process.
 * Stores them in the mInitialValues.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool ModelOptimizer::readInitialValues()
{
  bool ok = true;

  ValueManager *vm = Core::getInstance()->getValueManager();

  // delete old values
  mInitialValues.clear();

  // iterate over InitialValues by checking if the name or file value for an InitialValue exists
  QString initialValuesPathTemplate = mModelConfigPath
      + OrcsModelOptimizerConstants::VM_INITIALVALUES_PATH
      + OrcsModelOptimizerConstants::VM_VALUE_LIST;

  int i = 1;
  QString initialValuePath = initialValuesPathTemplate.arg(i);
  StringValue *initialValueNameVal = vm->getStringValue(initialValuePath
      + OrcsModelOptimizerConstants::VM_NAME_VALUE);

  StringValue *initialValueFileVal = vm->getStringValue(initialValuePath
      + OrcsModelOptimizerConstants::VM_FILE_VALUE);

  while(initialValueNameVal != 0 || initialValueFileVal != 0)
  {
    if(initialValueNameVal != 0 && initialValueFileVal != 0){
      Core::log(QString("%1: ERROR - A File and Name Parameter is configured at [%2]. Only one of these is allowed!").arg(getName(),
                                          initialValuePath));
      ok = false;
    }
    else
    {
      if(initialValueNameVal != 0)
      { // case if a value is configured

        //load value for the Value
        StringValue *valueVal = Tools::getStringValue(vm,
            initialValuePath +
                OrcsModelOptimizerConstants::VM_VALUE_VALUE,
            Tools::EM_ERROR,
            getName(),
            ok);

        if(valueVal != 0)
        {
          //check if the value exists
          Value *value = Tools::getStringValue(vm,
                                          initialValueNameVal->get(),
                                          Tools::EM_ERROR,
                                          getName(),
                                          ok,
                                          "The value is configured as initial value.");

          if(value != 0)
          {
            // add value to mInitialvalues
            mInitialValues.append(QString("value:%1:%2").arg(initialValueNameVal->get(),
                                                            valueVal->get()));
          }
        }

      }
      else
      { // case if a value file is configured

        //check if the value file exists
        QFile file(initialValueFileVal->get());

        if(!file.exists()){
          Core::log(QString("%1: ERROR - The initial value file [%2] does not exist!").arg(getName(),
                    initialValueFileVal->get()));
          ok = false;
        }
        else
        {
          // add file to mInitialvalues
          mInitialValues.append(QString("file:%1").arg(initialValueFileVal->get()));
        }
      }
    }


    // try to get next initialValue
    i++;
    initialValuePath = initialValuesPathTemplate.arg(i);
    initialValueNameVal = vm->getStringValue(initialValuePath
        + OrcsModelOptimizerConstants::VM_NAME_VALUE);
    initialValueFileVal = vm->getStringValue(initialValuePath
        + OrcsModelOptimizerConstants::VM_FILE_VALUE);
  }

  return ok;
}


/**
 * Reads the ModelValues which should be saved from the configuration
 * and from the mToOptimizeParameters and saves them in the mToSaveModelValues.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool ModelOptimizer::readToSaveModelValues()
{
  ValueManager *vm = Core::getInstance()->getValueManager();

  QList<QString> valueNames;
  QList<QString> excludeValueNames;

  // iterate over modelValues by checking if the name value for a modelValue exists
  QString modelValuesPathTemplate = mModelConfigPath
      + OrcsModelOptimizerConstants::VM_MODELVALUES_PATH
      + OrcsModelOptimizerConstants::VM_VALUE_LIST;

  int i = 1;
  QString modelValuePath = modelValuesPathTemplate.arg(i);
  StringValue *modelValueNameVal = vm->getStringValue(modelValuePath
      + OrcsModelOptimizerConstants::VM_NAME_VALUE);

  while(modelValueNameVal != 0)
  {
    QString temp = modelValueNameVal->get();

    temp.replace("**", ".*");
    if(temp.startsWith("^")) {
      temp = temp.mid(1);
    }
    else {
      temp.prepend(".*");
    }
    if(temp.endsWith("$")) {
      temp = temp.mid(0, temp.size() - 1);
    }
    else {
      temp.append(".*");
    }

    // check if the current values should be excluded
    BoolValue *excludeVal = vm->getBoolValue(modelValuePath
        + OrcsModelOptimizerConstants::VM_EXCLUDE_VALUE);

    bool exclude = excludeVal == 0 ? false : excludeVal->get();

    // load value names
    if(exclude == true) {
      excludeValueNames << vm->getValueNamesMatchingPattern(temp, false);
    }
    else {
      valueNames << vm->getValueNamesMatchingPattern(temp, false);
    }

    // try to get next modelValue
    i++;
    modelValuePath = modelValuesPathTemplate.arg(i);
    modelValueNameVal = vm->getStringValue(modelValuePath
        + OrcsModelOptimizerConstants::VM_NAME_VALUE);
  }

  // add Parameters which should be optimized to the list
  for(int i = 0; i < mToOptimizeParameters.size(); i++){
    valueNames.append(mToOptimizeParameters.at(i)->name->get());
  }

  // delete possible valuename reapeats in the valueList and the to exclude
  // values
  valueNames = valueNames.toSet().subtract(excludeValueNames.toSet()).toList();
  qSort(valueNames);

  mToSaveModelValues = valueNames;

  return true;
}

/**
 * Writes the start entry into the report file. Called before the optimizationStart() -
 * method is called.
 * Can be overridden to add further information to the report file, but this method should
 * be called at first.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool ModelOptimizer::writeReportStartEntry()
{
  bool ok = true;

  QString message = "";

  message += "Optimization Report\n";
  message += "===================\n";
  message += "\n";
  message += "Date:      " + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm") + "\n";
  message += "Model:     " + mModelName->get() + "\n";
  message += "Optimizer: " + getName() + "\n";
  message += "\n";
  message += "Optimization Parameters: \n";

  for(int i = 0; i < mToOptimizeParameters.size(); i++)
  {
    message += "  - " + mToOptimizeParameters.at(i)->name->get() + " ";
    message += "(" + mToOptimizeParameters.at(i)->value->getValueAsString() + ")\n";
  }

  message += "\n";
  message += "Initial Parameters:\n";
  // add initial values to report
  for(int i = 0; i < mInitialValues.size(); i++)
  {
    QString currentValString = mInitialValues.at(i);

    if(currentValString.startsWith("value:"))
    { // load value

      int indexFirstSep = currentValString.indexOf(":");
      int indexSecondSep = currentValString.indexOf(":",indexFirstSep + 1);

      QString valueName = currentValString.mid(indexFirstSep + 1,
          indexSecondSep - 1 - indexFirstSep);
      QString valueStr = currentValString.mid(indexSecondSep + 1);

      message += "  - " + valueName + " (" + valueStr + ")\n";

    }
    else if(currentValString.startsWith("file:"))
    {
    // load value file
      int indexFirstSep = currentValString.indexOf(":");
      QString fileName = currentValString.mid(indexFirstSep + 1);

      message += "  - " + fileName + "\n";
    }
    else {
      Core::log(QString("ModelOptimizer::writeReportStartEntry(): ERROR - Unknow format for string [%1] in the mInitialValues!").arg(currentValString));
      ok = false;
    }
  }
	
	
	// print used motions and lengths
	
	message += "\n";
	message += "Used Motions:\n";
  // add initial values to report
	for(int i = 0; i < mTrainSet->getMotionCount(); i++)
	{
		MotionData *currentMotion = mTrainSet->getMotionData(i);
		
		message += 	"  - " + currentMotion->getMotionName() 
				+ " (Length: " + QString::number(currentMotion->getMotionLength() + 1) + " simsteps)\n";
	}
	
	
	//print configuration

  message += "\n";
  message += "Configuration:\n";
  message += "  - MaxIterations:  " + mMaxIterations->getValueAsString() + "\n";
  message += "  - MaxSimulations: " + mMaxSimulationRuns->getValueAsString() + "\n";

  ok &= mReportLogger->addMessage(message);

  return ok;
}

/**
 * Writes the end entry into the report file. Called after the optimizationEnd() -
 * method is called.
 * Can be overridden to add further information to the report file, but this method should
 * be called at first.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool ModelOptimizer::writeReportEndEntry()
{
  bool ok = true;

  QString message;

  message += "\nOptimization Results\n";
  message += "--------------------\n";
  message += "\n";

	/*
  // add optimized parameters
  message += "Optimized Parameters:\n";

  for(int i = 0; i < mToOptimizeParameters.size(); i++)
  {
    message += "  - " + mToOptimizeParameters.at(i)->name->get() + " ";
    message += "--> " + mToOptimizeParameters.at(i)->value->getValueAsString() + "\n";
  }

  message += "\n";
	*/
	
	
  // add statistical values
  message += "Statistics:\n";
  message += "  - Simulation Runs: " + mSimulationRuns->getValueAsString() + "\n";
  message += "  - Optimization Iterations: " + mIterationCount->getValueAsString() + "\n";

  // calculate optimization duration
  int rawSec = mOptimizationStartTime.secsTo(QDateTime::currentDateTime());
  int sec = rawSec % 60;
  int min = (rawSec / 60) % 60;
  int hours = rawSec / 60 / 60;

  QString timeStr;
  if(hours > 0) {
    timeStr = QString::number(hours) + "h " + QString::number(min) + "m ";
  }
  else if(min > 0) {
    timeStr = QString::number(min) + "m ";
  }
  timeStr += QString::number(sec) + "s";

  message += "  - Optimization Duration: " + timeStr + "\n";

  ok &= mReportLogger->addMessage(message);

  return ok;
}

/**
 * Creates a new ParameterDescription object. The configuration values for the parameter
 * are defined under the parameterConfigPath. The method is called by the init method.
 *
 * This method can be overidden from ModelOptimizers to add their specific configuration
 * to a ParameterDescription.
 *
 * Note: The value property of the ParameterDescription is read out after this method is
 * called from the ModelOptimizer::init() method.
 *
 * @param parameterConfigPath Path to the configuration values of the parameter.
 * @param ok Set to FALSE if an error occured.
 * @return Reference to the new ParameterDescription.
 */
ParameterDescription* ModelOptimizer::createParameterDescription(
    const QString &parameterConfigPath, bool *ok)
{
  return new ParameterDescription();
}


/**
 * Called by the OptimizationManager to trigger the optimization start.
 *
 * 1. Resets the following variables: mOptimizationStartTime, mSimulationRuns,
 * mIterationCount.
 *
 * 2. Loads the configured initial values.
 *
 * 3. Prepares the ReportLogger and the StatisticsLogger.
 *
 * 4. Calls the writeReportStartEntry() Method.
 *
 * 5. Calls the optimizationStart() Method.
 *
 * 5. Calls the mOptimizationStepCompleted Event to save the statistic the first time.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool ModelOptimizer::callOptimizationStart()
{
  bool ok = true;

  // save current time and change name of filepaths
  mOptimizationStartTime = QDateTime::currentDateTime();

  // create folder for output files
  Core::getInstance()->enforceDirectoryPath(getOptimizationRunOutputDirectory());

  mSimulationRuns->set(0);
  mIterationCount->set(0);

  //////////////////////////////////////////
  // load initial values
  //////////////////////////////////////////

  ValueManager *vm = Core::getInstance()->getValueManager();

  for(int i = 0; i < mInitialValues.size(); i++)
  {
    QString currentValString = mInitialValues.at(i);

    if(currentValString.startsWith("value:"))
    { // load value

      int indexFirstSep = currentValString.indexOf(":");
      int indexSecondSep = currentValString.indexOf(":",indexFirstSep + 1);

      QString valueName = currentValString.mid(indexFirstSep + 1,
                                               indexSecondSep - 1 - indexFirstSep);
      QString valueStr = currentValString.mid(indexSecondSep + 1);

      Value *value = Tools::getStringValue(vm,
                                           valueName,
                                           Tools::EM_ERROR,
                                           getName(),
                                           ok,
                                           "The value is configured as initial value.");

      if(value != 0){
        ok &= value->setValueFromString(valueStr);
      }

    }
    else if(currentValString.startsWith("file:"))
    {
      // load value file
      int indexFirstSep = currentValString.indexOf(":");
      QString fileName = currentValString.mid(indexFirstSep + 1);

      ok &= vm->loadValues(fileName,false);
    }
    else {
      Core::log(QString("ModelOptimizer::optimizationStart(): ERROR - Unknow format for string [%1] in the mInitialValues!").arg(currentValString));
      ok = false;
    }
  }

  mTrainSet->setInitialSnaphot(Tools::saveSnapshot());


	////////////////////////////////////////////////
  // set initial values of to optimize parameters
	////////////////////////////////////////////////

	for(int i = 0; i < mToOptimizeParameters.size(); i++)
	{
		ParameterDescription *paraDescr = mToOptimizeParameters.at(i);

		// if default is defined --> use value which is set currently in simulator
		if(paraDescr->init->get().trimmed().toLower() != "default")
		{
			// check if a random value should be used
			if(paraDescr->init->get().trimmed().toLower() == "random")
			{				
				double random = Random::nextDoubleBetween(paraDescr->min->get(),
																							 		paraDescr->max->get());

				((DoubleValue*)paraDescr->value)->set(random);
			}
			else
			{
				bool convertOK = paraDescr->value->setValueFromString(paraDescr->init->get());
				
				if(convertOK == false){
					Core::log(QString("%1: ERROR - The initial value \"%2\" for the parameter [%3] has a wrong format!").arg(
									 	getName(),
										paraDescr->init->get(),
										paraDescr->name->get()));
					ok = false;
				}
			}
		}
	}

	////////////////////////////////////////////////


  // Prepare Report file
  mReportLogger->setFilePath(getOptimizationRunOutputDirectory() + QDir::separator() + getFileNameBegin() + "_Report.log");

  // Prepare Statistic file
  mStatisticsLogger->setFileName(getFileNameBegin() + "_Statistics.log");
  mStatisticsLogger->setLoggerPath(getOptimizationRunOutputDirectory());

  ok &= writeReportStartEntry();

  QString message;
  message += "\nOptimization Run\n";
  message += "----------------\n";
	message += "\n";
  message += "Start optimization ...";
  ok &= mReportLogger->addMessage(message);

  if(mIsPrintTraindataStatistics->get() == true) {
    ok &= saveTraindataStatistic("BeforeOptimization");
  }

	if(ok == true) {
  	ok &= optimizationStart();
	}

  mOptimizationStepCompleted->trigger();

  return ok;
}


/**
 * Called before the optimization process starts and the doNextOptimizationStep
 * is called.
 * Can be overridden to initialize the optimization process.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool ModelOptimizer::optimizationStart()
{
  return true;
}



bool ModelOptimizer::callNextOptimizationStep()
{
  bool finished = true;

  if(mIterationCount->get() < mMaxIterations->get() &&
     mSimulationRuns->get() < mMaxSimulationRuns->get() )
  {
    finished = doNextOptimizationStep();

    mIterationCount->set(mIterationCount->get() + 1);

    mOptimizationStepCompleted->trigger();
  }
  else
  {
    QString message;

    if(mIterationCount->get() >= mMaxIterations->get()) {
      message += "   Maximal iterations of " + QString::number(mMaxIterations->get()) + " reached.";
    }

    if(mSimulationRuns->get() >= mMaxSimulationRuns->get())
    {
      if(message.size() > 0) {
        message += "\n";
      }
      message += "   Maximal simulation runs of " + QString::number(mMaxSimulationRuns->get()) + " reached.";
    }

    mReportLogger->addMessage("\nStopped optimization:\n" + message);

  }

  return finished;
}


bool ModelOptimizer::callOptimizationEnd()
{
  bool ok = optimizationEnd();

  mOptimizationRunCompleted->trigger();

  ok &= writeReportEndEntry();

  if(mIsPrintTraindataStatistics->get() == true) {
    ok &= saveTraindataStatistic("AfterOptimization");
  }

  return ok;
}

/**
 * Called after the optimization process ends (doNextOptimizationStep returns false).
 * Can be overridden to do actions after the optimization process.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool ModelOptimizer::optimizationEnd()
{
  return true;
}


/**
 * Saves the configured ModelValues and Parameters in the given file.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool ModelOptimizer::saveToOptimizeParameters(const QString &filePath)
{
  ValueManager *vm = Core::getInstance()->getValueManager();

  return vm->saveValues(filePath,mToSaveModelValues,"",false);
}

bool ModelOptimizer::saveTraindataStatistic(const QString &fileName)
{
  // create folder which stores the files
  QString directory = getOptimizationRunOutputDirectory()
                      + QDir::separator()
                      + "TraindataStatistics";

  Core::getInstance()->enforceDirectoryPath(directory);

  // collect data
  Physics::getSimulationEnvironmentManager()->createSnapshot();
  bool ok = mTrainSet->collectNewSimData();
	
	if(ok == false){
		return false;
	}
		
  Physics::getSimulationEnvironmentManager()->resetStartConditions();

  for(int motion = 0; motion < mTrainSet->getMotionCount(); motion++)
  {
    MotionData *motionData = mTrainSet->getMotionData(motion);

    QString filePath = directory
                       + QDir::separator()
                       + getFileNameBegin()
                       + "_"
                       + motionData->getMotionName()
                       + "_"
                       + fileName
                       + ".stat";

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
      Core::log(QString("%1: ERROR - Could not create the file [%2]!").arg(
                                        getName(),
                                        filePath));
      file.close();
      return false;
    }

    QTextStream out(&file);

    for(int channel = 0; channel < motionData->getChannelCount(); channel++)
    {
      out << QString("# Simulation Data - %1\n").arg(motionData->getChannelName(channel));
      out << QString("# Physical Data - %1\n").arg(motionData->getChannelName(channel));
      out << QString("# Physical Data - %1 - STD\n").arg(motionData->getChannelName(channel));
    }
    out << "\n";

    for(int i = 0; i < motionData->getMotionLength(); i++ )
    {
      for(int channel = 0; channel < motionData->getChannelCount(); channel++)
      {
        if(channel > 0)
          out << ";";
        out << QString::number(motionData->getSimulationData()->getData(channel,i));
        out << ";";
        out << QString::number(motionData->getPhysicalData()->getMeanData(channel,i));
        out << ";";
        out << QString::number(motionData->getPhysicalData()->getStandartDeviation(channel,i));
      }
      out << "\n";
    }

    file.close();
  }

  return true;
}


/**
 * Returns the begin of all files which are created for results.
 * Format: "<ModelName>_<Date and Time of optimization starts>"
 *
 * @return String which is used as begin for all files which are created.
 */
QString ModelOptimizer::getFileNameBegin()
{
  return  mModelName->get() + "_" + mOptimizationStartTime.toString("yyyyMMdd_hhmm");
}

/**
 * Returns the path to the directory which holds the results files
 * of the current optimization run.
 *
 * @return Path to the results directory.
 */
QString ModelOptimizer::getOptimizationRunOutputDirectory()
{
  QString directory = mOutputDirectory->get();

  if(!directory.endsWith(QDir::separator())) {
    directory += QDir::separator();
  }

  directory += mModelName->get() + "_" + mOptimizationStartTime.toString("yyyyMMdd_hhmm");

  return directory;
}



bool ModelOptimizer::appendStatistic(StatisticCalculator *statistic)
{
	bool ok = true;
	
	StatisticsManager *sm = Statistics::getStatisticsManager();

	ok &= sm->addStatistics(statistic,
													mStepCompletedEventName,
						 							mOptimizationCompletedEventName,
			 										mOptimizationStatisticCompletedEventName);
	
	return ok;
}

bool ModelOptimizer::appendStatistic(Value *statisticValue, const QString &name)
{
	bool ok = true;

	QString basePath = mModelConfigPath + "Statistics/";
	
	StatisticCalculator *statistic = new StatisticCalculator(	name, 
																														basePath);
	statistic->setObservedValue(statisticValue);
	ok &= appendStatistic(statistic);
	
	return ok;
}

bool ModelOptimizer::appendStatistic(
		QVector<ParameterDescription*> mToOptimizeParameters)
{
	bool ok = true;

	for(int i = 0; i < mToOptimizeParameters.size(); i++ )
	{
		// use only last part of path as name
		int index = mToOptimizeParameters.at(i)->name->get().lastIndexOf("/");
		index = index < 0 ? 0 : index + 1;
		QString name = "Parameters/" + mToOptimizeParameters.at(i)->name->get().mid(index);
    
		ok &= appendStatistic(mToOptimizeParameters.at(i)->value, 
													name);
	}
	
	return ok;
}

} // namespace nerd
