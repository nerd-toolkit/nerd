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

#include "Tools.h"

#include <QString>
#include <QStringList>
#include "Core/Core.h"
#include "Value/Value.h"
#include "Value/StringValue.h"
#include "Value/IntValue.h"
#include "Value/BoolValue.h"
#include "Value/DoubleValue.h"
#include "Value/DoubleValue.h"
#include "Physics/Physics.h"

using namespace nerd;

/**
 * Converts a given string into a path string by trimming it and adding a "/" at the
 * end if necessary.
 *
 * Example: " ./folder1/folder2   " --> "./folder1/folder2/"
 *
 * @param path String which should be converted into a path string.
 * @return Converted path string.
 */
QString Tools::toPathString(const QString &path)
{
  QString trimmedPath = path.trimmed();

  if(!trimmedPath.endsWith('/'))
    return trimmedPath.append('/');
  else
    return trimmedPath;
}

/**
 * Loads the given Snapshot via the SimulationEnvironmentManager.
 *
 * @param snapsot Snapsot which sould be loaded.
 */
void Tools::loadSnapshot(QHash<SimObject*, QHash<Value*, QString> >  &snapshot)
{
  Physics::getSimulationEnvironmentManager()->setSnapshot(snapshot);
  Physics::getSimulationEnvironmentManager()->resetStartConditions();
}

/**
 * Saves the current values in the smulation into a snapshot by calling
 * SimulationEnvironmentManager()->createSnapshot() and returns the saved values.
 *
 * @return Snapshot with saved values.
 */
QHash<SimObject*, QHash<Value*, QString> > Tools::saveSnapshot()
{
  Physics::getSimulationEnvironmentManager()->createSnapshot();
  return QHash<SimObject*, QHash<Value*, QString> >(
           Physics::getSimulationEnvironmentManager()->getSnapshot());
}




/**
 * Converts the given length in Milliseconds to SimulationSteps.
 *
 * @param msec Length in milliseconds.
 * @return Length in Simulation steps.
 */
int Tools::convertMsecToSimSteps(int msec)
{
  if(msec < 0){
    return -1;
  }

  ValueManager *vm = Core::getInstance()->getValueManager();

  bool ok = true;
  // get length of one step in the simulation in seconds
  DoubleValue *mSimTimeStepSize = Tools::getDoubleValue(vm,
                                           SimulationConstants::VALUE_TIME_STEP_SIZE,
                                           Tools::EM_ERROR,
                                           "Tools::convertMsecInSimSteps()",
                                           ok);

  if(!ok) {
    return -1;
  }

  if(mSimTimeStepSize->get() == 0) {
    return -1;
  }

  return msec / (mSimTimeStepSize->get() * 1000.0);
}

/**
 * Converts the given length in SimulationSteps to Milliseconds.
 *
 * @param msec Length in milliseconds.
 * @return Length in Simulation steps.
 */
int Tools::convertSimStepsToMsec(int simSteps)
{
  if(simSteps < 0){
    return -1;
  }

  ValueManager *vm = Core::getInstance()->getValueManager();

  bool ok = true;

  // get length of one step in the simulation in seconds
  DoubleValue *mSimTimeStepSize = Tools::getDoubleValue(vm,
      SimulationConstants::VALUE_TIME_STEP_SIZE,
      Tools::EM_ERROR,
      "Tools::convertMsecInSimSteps()",
      ok);

  if(!ok) {
    return -1;
  }

  if(mSimTimeStepSize->get() == 0) {
    return -1;
  }

  return simSteps * (1000.0 * mSimTimeStepSize->get());
}






Value* Tools::getValue(ValueManager *valueManager,
                       const QString &name,
                       ErrorModus modus,
                       const QString &componentName,
                       bool &isNoError,
                       const QString &additionalMessage)
{
  Value *val = valueManager->getValue(name);

  if(val == 0 && modus != Tools::EM_NOERROR)
  {
    QString errorStr("");

    switch(modus)
    {
      case Tools::EM_ERROR:
      {
        errorStr = "ERROR";
        isNoError = false;
        break;
      }
      case Tools::EM_WARNING: {
        errorStr = "Warning";
        break;
      }
    }

    Core::log(componentName + ": " + errorStr + " - Could not find required value [" +
        name + "]! " + additionalMessage);
  }

  return val;
}


StringValue* Tools::getStringValue(ValueManager *valueManager,
                                   const QString &name,
                                   ErrorModus modus,
                                   const QString &componentName,
                                   bool &isNoError,
                                   const QString &additionalMessage)
{
  return (StringValue*) Tools::getValue(valueManager,
                                       name,
                                       modus,
                                       componentName,
                                       isNoError,
                                       additionalMessage);
}

IntValue* Tools::getIntValue(ValueManager *valueManager,
                                   const QString &name,
                                   ErrorModus modus,
                                   const QString &componentName,
                                   bool &isNoError,
                                   const QString &additionalMessage)
{
  return (IntValue*) Tools::getValue(valueManager,
          name,
          modus,
          componentName,
          isNoError,
          additionalMessage);
}

DoubleValue* Tools::getDoubleValue(ValueManager *valueManager,
                             const QString &name,
                             ErrorModus modus,
                             const QString &componentName,
                             bool &isNoError,
                             const QString &additionalMessage)
{
  return (DoubleValue*) Tools::getValue(valueManager,
          name,
          modus,
          componentName,
          isNoError,
          additionalMessage);
}

Value* Tools::getOrAddValue(ValueManager *valueManager,
                            const QString &name,
                            bool isPrintWarning,
                            const QString &componentName,
                            bool &isNoError,
                            Value *defaultVal,
                            const QString &additionalMessage)
{
  Value *val = valueManager->getValue(name);

  if(val == 0)
  {
    val = defaultVal;

    if(isPrintWarning) {
      Core::log(QString(componentName + ": Warning - Required Value [" + name +
          "] was not created. Add Value to ValueManager with default value \"" +
          defaultVal->getValueAsString() + "\"." + additionalMessage));
    }

    if(valueManager->addValue(name, val) == false)
    {
      Core::log(QString(componentName + ": ERROR - Required Value [" + name +
          "] could not be added to the ValueManager!"));

      isNoError = false;
    }
  }

  return val;
}

StringValue* Tools::getOrAddStringValue(ValueManager *valueManager,
                                        const QString &name,
                                        bool isPrintWarning,
                                        const QString &componentName,
                                        bool &isNoError,
                                        const QString &defaultVal,
                                        const QString &additionalMessage)
{
  StringValue *defaultStringValue = new StringValue(defaultVal);

  return (StringValue*) Tools::getOrAddValue(valueManager,
                                            name,
                                            isPrintWarning,
                                            componentName,
                                            isNoError,
                                            defaultStringValue,
                                            additionalMessage);
}


IntValue* Tools::getOrAddIntValue(ValueManager *valueManager,
                                  const QString &name,
                                  bool isPrintWarning,
                                  const QString &componentName,
                                  bool &isNoError,
                                  int defaultVal,
                                  const QString &additionalMessage)
{
  IntValue *defaultIntValue = new IntValue(defaultVal);

  return (IntValue*) Tools::getOrAddValue(valueManager,
                                          name,
                                          isPrintWarning,
                                          componentName,
                                          isNoError,
                                          defaultIntValue,
                                          additionalMessage);
}

BoolValue* Tools::getOrAddBoolValue(ValueManager *valueManager,
                                  const QString &name,
                                  bool isPrintWarning,
                                  const QString &componentName,
                                  bool &isNoError,
                                  bool defaultVal,
                                  const QString &additionalMessage)
{
  BoolValue *defaultBoolValue = new BoolValue(defaultVal);

  return (BoolValue*) Tools::getOrAddValue(valueManager,
          name,
          isPrintWarning,
          componentName,
          isNoError,
          defaultBoolValue,
          additionalMessage);
}

DoubleValue* Tools::getOrAddDoubleValue(ValueManager *valueManager,
                                    const QString &name,
                                    bool isPrintWarning,
                                    const QString &componentName,
                                    bool &isNoError,
                                    double defaultVal,
                                    const QString &additionalMessage)
{
  DoubleValue *defaultBoolValue = new DoubleValue(defaultVal);

  return (DoubleValue*) Tools::getOrAddValue(valueManager,
          name,
          isPrintWarning,
          componentName,
          isNoError,
          defaultBoolValue,
          additionalMessage);
}


/////////////////////////////////////////////////////
// getValue Methods for different value occurences
/////////////////////////////////////////////////////

Value* Tools::getValue(ValueManager *valueManager,
                        const QStringList &basePathList,
                        const QString &name,
                        ErrorModus modus,
                        const QString &componentName,
                        bool &isNoError,
                        const QString &additionalMessage)
{
  Value *retVal = 0;

  for(int i = 0; i < basePathList.size(); i++)
  {
    bool ok = true;

    retVal = Tools::getValue( valueManager,
                              basePathList.at(i) + name,
                              Tools::EM_NOERROR,
                              componentName,
                              ok);

    if(retVal != 0){
      break;
    }
  }

  if(retVal == 0 && basePathList.size() > 0 && modus != Tools::EM_NOERROR)
  {
    QString errorStr("");

    switch(modus)
    {
      case Tools::EM_ERROR:
      {
        errorStr = "ERROR";
        isNoError = false;
        break;
      }
      case Tools::EM_WARNING: {
        errorStr = "Warning";
        break;
      }
    }

    QString occurenceList = "";
    for(int i = 0; i < basePathList.size(); i++)
    {
      if(i > 0) {
        occurenceList += ", ";
      }

      occurenceList += "[" + basePathList.at(i) + name +"]";
    }

    Core::log(componentName + ": " + errorStr + " - Could not find required value at one of the following locations:" + occurenceList + "! " + additionalMessage);
  }

  return retVal;
}

StringValue* Tools::getStringValue(ValueManager *valueManager,
                                   const QStringList &basePathList,
                                   const QString &name,
                                   ErrorModus modus,
                                   const QString &componentName,
                                   bool &isNoError,
                                   const QString &additionalMessage)
{
  return (StringValue*) Tools::getValue(valueManager,
          basePathList,
          name,
          modus,
          componentName,
          isNoError,
          additionalMessage);
}

IntValue* Tools::getIntValue(ValueManager *valueManager,
                             const QStringList &basePathList,
                             const QString &name,
                             ErrorModus modus,
                             const QString &componentName,
                             bool &isNoError,
                             const QString &additionalMessage)
{
  return (IntValue*) Tools::getValue(valueManager,
          basePathList,
          name,
          modus,
          componentName,
          isNoError,
          additionalMessage);
}

DoubleValue* Tools::getDoubleValue(ValueManager *valueManager,
                                   const QStringList &basePathList,
                                   const QString &name,
                                   ErrorModus modus,
                                   const QString &componentName,
                                   bool &isNoError,
                                   const QString &additionalMessage)
{
  return (DoubleValue*) Tools::getValue(valueManager,
          basePathList,
          name,
          modus,
          componentName,
          isNoError,
          additionalMessage);
}

/////////////////////////////////////////////////////
// getOrAddValue Methods for different value occurences
/////////////////////////////////////////////////////

Value* Tools::getOrAddValue(ValueManager *valueManager,
                            const QStringList &basePathList,
                            const QString &name,
                            bool isPrintWarning,
                            const QString &componentName,
                            bool &isNoError,
                            Value *defaultVal,
                            const QString &additionalMessage)
{
  if(basePathList.size() <= 0){
    return 0;
  }

  Value *val = Tools::getValue(valueManager,
                               basePathList,
                               name,
                               Tools::EM_NOERROR,
                               componentName,
                               isNoError);

  if(val == 0)
  {
    val = defaultVal;
    QString valueName = basePathList.last() + name;

    if(isPrintWarning) {
      Core::log(QString(componentName + ": Warning - Required Value [" + valueName +
          "] was not created. Add Value to ValueManager with default value \"" +
          defaultVal->getValueAsString() + "\"." + additionalMessage));
    }

    if(valueManager->addValue(valueName, val) == false)
    {
      Core::log(QString(componentName + ": ERROR - Required Value [" + valueName +
          "] could not be added to the ValueManager!"));

      isNoError = false;
    }
  }

  return val;
}

StringValue* Tools::getOrAddStringValue(ValueManager *valueManager,
                                        const QStringList &basePathList,
                                        const QString &name,
                                        bool isPrintWarning,
                                        const QString &componentName,
                                        bool &isNoError,
                                        const QString &defaultVal,
                                        const QString &additionalMessage)
{
  StringValue *defaultStringValue = new StringValue(defaultVal);

  return (StringValue*) Tools::getOrAddValue(valueManager,
          basePathList,
          name,
          isPrintWarning,
          componentName,
          isNoError,
          defaultStringValue,
          additionalMessage);
}


IntValue* Tools::getOrAddIntValue(ValueManager *valueManager,
                                  const QStringList &basePathList,
                                  const QString &name,
                                  bool isPrintWarning,
                                  const QString &componentName,
                                  bool &isNoError,
                                  int defaultVal,
                                  const QString &additionalMessage)
{
  IntValue *defaultIntValue = new IntValue(defaultVal);

  return (IntValue*) Tools::getOrAddValue(valueManager,
          basePathList,
          name,
          isPrintWarning,
          componentName,
          isNoError,
          defaultIntValue,
          additionalMessage);
}

BoolValue* Tools::getOrAddBoolValue(ValueManager *valueManager,
                                    const QStringList &basePathList,
                                    const QString &name,
                                    bool isPrintWarning,
                                    const QString &componentName,
                                    bool &isNoError,
                                    bool defaultVal,
                                    const QString &additionalMessage)
{
  BoolValue *defaultBoolValue = new BoolValue(defaultVal);

  return (BoolValue*) Tools::getOrAddValue(valueManager,
          basePathList,
          name,
          isPrintWarning,
          componentName,
          isNoError,
          defaultBoolValue,
          additionalMessage);
}

DoubleValue* Tools::getOrAddDoubleValue(ValueManager *valueManager,
                                    const QStringList &basePathList,
                                    const QString &name,
                                    bool isPrintWarning,
                                    const QString &componentName,
                                    bool &isNoError,
                                    double defaultVal,
                                    const QString &additionalMessage)
{
  DoubleValue *defaultBoolValue = new DoubleValue(defaultVal);

  return (DoubleValue*) Tools::getOrAddValue(valueManager,
          basePathList,
          name,
          isPrintWarning,
          componentName,
          isNoError,
          defaultBoolValue,
          additionalMessage);
}
