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
 
#include "MotionData.h"

#include "OrcsModelOptimizerConstants.h"
#include "SimulationConstants.h"

#include <QString>
#include <QStringList>

#include "Core/Core.h"
#include "SimulationData.h"
#include "PhysicalData.h"
#include "Tools.h"
#include "Trigger.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"
#include "Value/Value.h"



namespace nerd 
{
  
  
MotionData::MotionData(const QString &motionConfigPath,
                       EvaluationLoop *evaluationLoop)
  : mMotionConfigPath(motionConfigPath),
    mEvaluationLoop(evaluationLoop),
    mMotionLengthInMsec(0),
    mMotionLengthInSimSteps(0),
    mTrigger(0),
    mSimulationData(0),
    mPhysicalData(0)
{

}
  
MotionData::~MotionData()
{  
  if(mPhysicalData != 0){
    delete mPhysicalData;
    mPhysicalData = 0;
  }
  
  if(mSimulationData != 0){
    delete mSimulationData;
    mSimulationData = 0;
  }
  
  if(mTrigger != 0){
    delete mTrigger;
    mTrigger = 0;
  }
}
    
bool MotionData::init()
{
  bool ok = true;
  
  ValueManager * vm = Core::getInstance()->getValueManager();
  
  /////////////////////////////////////////
  // Read motion length config
  /////////////////////////////////////////
  mMotionLengthInMsec = Tools::getOrAddIntValue(vm, 
                                           mMotionConfigPath + 
                                               OrcsModelOptimizerConstants::VM_LENGTH_VALUE,
                                           false,
                                           getName(),
                                           ok,
                                           -1);
  
  if(ok == true) {
    mMotionLengthInSimSteps = Tools::convertMsecToSimSteps(mMotionLengthInMsec->get());
  }
  
  /////////////////////////////////////////
  // Read trigger config
  /////////////////////////////////////////
   
  // there are 2 possible paths were the trigger values are defined
  QStringList triggerPathList;
  
  // 1) at the motion config
  triggerPathList += mMotionConfigPath + 
      OrcsModelOptimizerConstants::VM_TRIGGER_PATH;
  
  // 2) at the general input data config
  triggerPathList += OrcsModelOptimizerConstants::VM_BASE_PATH +
      OrcsModelOptimizerConstants::VM_INPUTDATA_PATH + 
      OrcsModelOptimizerConstants::VM_TRIGGER_PATH;
  
  mTrigger = new Trigger( triggerPathList );

  ok &= mTrigger->init();
  
  /////////////////////////////////////////////
  // Initialize simulationdata and pysical data
  /////////////////////////////////////////////
  
  if(ok == true)
  {

    mSimulationData = new SimulationData(mMotionConfigPath, 
                                          mEvaluationLoop,
                                          mTrigger);
    
    ok &= mSimulationData->init();

    
    mPhysicalData = new PhysicalData(mMotionConfigPath, mTrigger);
    
    ok &= mPhysicalData->init(); 
  }

  return ok;
}

QString MotionData::getName() const {
  return "MotionData";
}
  
int MotionData::getMotionLength() const
{
  return mMotionLengthInSimSteps;
}
  
int MotionData::getChannelCount() const
{
  return getSimulationData()->getChannelCount();
}

QString MotionData::getMotionName() const
{
  return getSimulationData()->getMotionName();
}
    
QString MotionData::getChannelName(int channel) const
{
  return getSimulationData()->getChannelName( channel );
}

bool MotionData::collectNewSimData()
{
  return mSimulationData->collectNewData( this->getMotionLength() );
}

bool MotionData::readPhysicalData()
{
  bool ok = true;
  
  // check if a motion length is configured
  // --> no: choose the motion length of the shortest physical data source
  // --> yes: use the configured motion length
  if(this->getMotionLength() < 0)
  {
    mMotionLengthInSimSteps = mPhysicalData->readData();
    
    mMotionLengthInMsec->set( Tools::convertSimStepsToMsec(mMotionLengthInSimSteps) );

    ok &= mMotionLengthInSimSteps < 0 ? false : true;
  }
  else
  {
    ok &= mPhysicalData->readData( getMotionLength() );
  }
  
  return ok;
}

SimulationData* MotionData::getSimulationData() const{
  return mSimulationData;
}

PhysicalData* MotionData::getPhysicalData() const{
  return mPhysicalData;
}

} // namespace nerd
