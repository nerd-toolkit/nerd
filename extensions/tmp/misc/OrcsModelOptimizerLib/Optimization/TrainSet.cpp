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

#include "TrainSet.h"

#include "OrcsModelOptimizerConstants.h"

#include <QString>
#include <QMap>
#include <QPair>
#include <QHash>
#include <QDir>

#include "Core/Core.h"
#include "Value/StringValue.h"
#include "PlugIns/KeyFramePlayer.h"
#include "SimulationData.h"
#include "PhysicalData.h"
#include "XMLConfigLoader.h"
#include "Tools.h"
#include "MotionData.h"


namespace nerd
{

TrainSet::TrainSet(EvaluationLoop *evaluationLoop)
  : mEvaluationLoop(evaluationLoop)
{
}

/**
 * Destructor
 *
 * Delete all SimulationData and PhysicalData objects.
 */
TrainSet::~TrainSet()
{
  for(int i = 0; i < mMotionDataVector.size(); i++)
  {
    if(mMotionDataVector.at(i) != 0){
      delete mMotionDataVector[i];
    }
  }
  mMotionDataVector.clear();

}

QString TrainSet::getName() const {
  return "TrainSet";
}

/**
 * Initializes the TrainSet. Creates for every configured motion a
 * SimulationData and a PhysicalData object and calls their init() method.
 *
 * @return TRUE if successfully, otherwise false.
 */
bool TrainSet::init()
{
  bool ok = true;

  ValueManager * vm = Core::getInstance()->getValueManager();

  // map which holds the created simulationdata and the maximal length of all motions
  // which are using the same simulationdata object
  QMap<QString, QPair<SimulationData*, uint > > createdSimulationData;

  ///////////////////////////////////////////////////////
  // Load Values to build path to motion
  // configuration files
  ///////////////////////////////////////////////////////

  StringValue * scenarioPath = Tools::getStringValue(vm,
                                        OrcsModelOptimizerConstants::VM_BASE_PATH +
                                          OrcsModelOptimizerConstants::VM_INPUTDATA_PATH +
                                          OrcsModelOptimizerConstants::VM_SCENARIO_VALUE,
                                        Tools::EM_ERROR,
                                        this->getName(),
                                        ok);

  // the scenario path is needed to generate filepaths
  // --> if no value defined: stop initialization
  if(ok == false)
    return ok;

  // create string to access motions
  QString motionPathTemplate =
      OrcsModelOptimizerConstants::VM_BASE_PATH +
      OrcsModelOptimizerConstants::VM_INPUTDATA_PATH +
      OrcsModelOptimizerConstants::VM_MOTION_LIST;

  // iterate over motions by checking if the name value for a motion exists
  int i = 1;
  QString motionPath = motionPathTemplate.arg(i);
  StringValue * motionNameVal = vm->getStringValue(motionPath
      + OrcsModelOptimizerConstants::VM_NAME_VALUE);

  while(motionNameVal != 0)
  {
    ///////////////////////////////////////////////////////
    // Load Motion Configuration if the file exist
    ///////////////////////////////////////////////////////

    bool currentMotionOK = true;

    QDir motionDir( Tools::toPathString(scenarioPath->get()) +
                    Tools::toPathString(motionNameVal->get()) );

    if(!motionDir.exists()){
      Core::log(QString("TrainSet: ERROR - Can not find motion folder [%1]!").arg(
                motionDir.path()));
      currentMotionOK = false;
    }
    else
    {
      QString motionConfigFileStr = "*" +
          OrcsModelOptimizerConstants::FILE_ENDING_MOTION_CONFIG;

      // set namefilter to search motion configuration file
      motionDir.setNameFilters(QStringList(motionConfigFileStr));
      motionDir.refresh();

      if(motionDir.count() > 1)
      {
        Core::log(QString("TrainSet: ERROR - Too many motion configuration files (\"%1\") in the motion folder [%2]!").arg(motionConfigFileStr, motionDir.path()));

        currentMotionOK = false;
      }
      else if( motionDir.count() == 1)
      {
        QHash<QString, QString> templates;
        templates.insert(OrcsModelOptimizerConstants::TEMPLATE_MOTION_PATH,
                         motionPath);

        XMLConfigLoader *loader = new XMLConfigLoader(motionDir.filePath(motionDir[0]) ,
            OrcsModelOptimizerConstants::CONFIG_FOLDER +
                OrcsModelOptimizerConstants::FILE_MOTION_CONFIG_DESCRIPTION,
            templates);

        currentMotionOK &= loader->init();

        if(currentMotionOK == true){
          currentMotionOK &= loader->loadFromXML();
        }
      }
    }

    if(currentMotionOK == true)
    {
      ///////////////////////////////////////////////////////
      // Initialize the MotionData
      ///////////////////////////////////////////////////////

      MotionData *md = new MotionData(motionPath,
                                      mEvaluationLoop);

      currentMotionOK &= md->init();

      if(currentMotionOK == true){
        currentMotionOK &= md->readPhysicalData();
      }

      mMotionDataVector.push_back(md);
    }

    ok &= currentMotionOK;

    // try to get next motion
    i++;
    motionPath = motionPathTemplate.arg(i);
    motionNameVal = vm->getStringValue(motionPath
        + OrcsModelOptimizerConstants::VM_NAME_VALUE);
  }

  return ok;
}


void TrainSet::setInitialSnaphot(QHash<SimObject*, QHash<Value*, QString> > initialSnapshot)
{
  for(int i = 0; i < mMotionDataVector.size(); i++)
  {
    mMotionDataVector.at(i)->getSimulationData()->setInitialSnapshot(initialSnapshot);
  }
}


int TrainSet::getMotionCount() {
  return mMotionDataVector.size();
}

MotionData* TrainSet::getMotionData(int motion)
{
  return mMotionDataVector.at(motion);
}


/**
 * Collects new data from the simulation for all motions by calling the corresponding
 * SimulationData.collectNewData().
 *
 * @return TRUE if succesfully, otherwise FALSE.
 */
bool TrainSet::collectNewSimData()
{
  bool ok = true;

  for(int i = 0; i < mMotionDataVector.size(); i++)
  {
    ok &= mMotionDataVector.at(i)->collectNewSimData();

    if(ok == false){
      break;
    }
  }

  return ok;
}

/**
 * Collects new data from the simulation for a certain motion by calling the corresponding
 * SimulationData.collectNewData().
 *
 * @return TRUE if succesfully, otherwise FALSE.
 */
bool TrainSet::collectNewSimData(int motion)
{
  // check if motion exists
  if(motion > mMotionDataVector.size()) {
    Core::log("TrainSet: ERROR - Could not access motion \"" + QString::number(motion) + "\". " +
        "Only " + QString::number(mMotionDataVector.size() + 1) + " motions stored!");
    return false;
  }

  return mMotionDataVector.at(motion)->collectNewSimData();
}


} // namespace nerd
