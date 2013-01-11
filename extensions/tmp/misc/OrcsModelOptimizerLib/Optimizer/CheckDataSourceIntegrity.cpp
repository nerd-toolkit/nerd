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

#include "CheckDataSourceIntegrity.h"
#include "OrcsModelOptimizerConstants.h"

#include "Core/Core.h"
#include "Optimization/TrainSet.h"
#include "Optimization/MotionData.h"
#include "Optimization/SimulationData.h"
#include "Optimization/PhysicalData.h"
#include "Optimization/PhysicalDataSource.h"
#include "Optimization/Tools.h"
#include "Optimization/ReportLogger.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"

#include "Math/Math.h"

#include <QStringList>
#include <QMap>
#include <QDir>

#include <cmath>

namespace nerd 
{

CheckDataSourceIntegrity::CheckDataSourceIntegrity(const QString &model, TrainSet *trainSet) 
 : ModelOptimizer::ModelOptimizer(model, trainSet),
   mTolerance(0)
{ }
   
CheckDataSourceIntegrity::~CheckDataSourceIntegrity()
{

}
   
   
QString CheckDataSourceIntegrity::getName() const{
  return "CheckDataSourceIntegrity";
}


bool CheckDataSourceIntegrity::init() 
{
  bool ok = ModelOptimizer::init();
  
  ValueManager *vm = Core::getInstance()->getValueManager();
  
  QStringList configPaths;
  configPaths += mModelConfigPath;
  configPaths += OrcsModelOptimizerConstants::VM_BASE_PATH 
                 + OrcsModelOptimizerConstants::VM_OPTIMIZER_PATH;
  
  // load configuration values
  mTolerance = Tools::getOrAddDoubleValue(vm, 
                                configPaths,
                                OrcsModelOptimizerConstants::VM_TOLERANCE_VALUE,
                                false,
                                getName(),
                                ok,
                                0.0);
  
  mTimeTolerance = Tools::getOrAddIntValue(vm, 
                                configPaths,
                                OrcsModelOptimizerConstants::VM_TIMETOLERANCE_VALUE,
                                false,
                                getName(),
                                ok,
                                0);
  
  return ok;
}


bool CheckDataSourceIntegrity::optimizationStart()
{
  bool ok = ModelOptimizer::optimizationStart();
  

  
  return ok;
}

bool CheckDataSourceIntegrity::doNextOptimizationStep()
{
  mTrainSet->collectNewSimData();
  
  QList<MotionDescr> incoherentData;

  // search for incoherant data
  for(int motion = 0; motion < mTrainSet->getMotionCount(); motion++)
  {
    MotionData *md = mTrainSet->getMotionData(motion);
    
    MotionDescr motionDescr;
    motionDescr.id = motion;
    motionDescr.name = md->getMotionName();
    
    for(int source = 0; source < md->getPhysicalData()->getPhysicalDataSourceCount(); source++)
    {     
      PhysicalDataSource *pds = md->getPhysicalData()->getPhysicalDataSource(source);
       
      SourceDescr sourceDescr;
      sourceDescr.id = source;
      sourceDescr.name = pds->getSourceName();
           
      for(int channel = 0; channel < md->getChannelCount(); channel++)
      {
        bool error = false;
        int errorTime = 0;
        
        for(int simstep = 0; simstep < md->getMotionLength(); simstep++)
        {
          double simData = md->getSimulationData()->getData(channel, simstep);
          double phyData = pds->getData(channel, simstep);
          double diff = Math::abs(simData - phyData);
          
          if(diff > mTolerance->get()){
            errorTime++;
          }
          else {
            errorTime = 0;
          }
          
          if(errorTime > mTimeTolerance->get())
          {
            error = true;
            break;
          }
        }
        
        if(error == true)
        {
          ChannelDescr channelDescr;
          channelDescr.id = channel;
          channelDescr.name = md->getChannelName(channel);
              
          sourceDescr.channels.push_back(channelDescr);
        }
      }
      
      if(!sourceDescr.channels.empty()) {
        motionDescr.sources.push_back(sourceDescr);
      }
    }
     
    if(!motionDescr.sources.empty()){
      incoherentData.push_back(motionDescr); 
    }
  }
  
  
  // print report entry
  if(incoherentData.empty() == true)
  {
    mReportLogger->addMessage("\nFound no incoherent data.");  
  }
  else
  {
    QString message;
    
    message += "\nFound incoherent data!\n";
    message += "\n";
    message += "The following files are incoherent for the listed channels:\n";
    message += "\n";
    
    for(int motion = 0; motion < incoherentData.size(); motion++)
    {
      MotionDescr motionDescr = incoherentData.at(motion);
      
      message += " - " + motionDescr.name + ":\n\n";
      
      if(motionDescr.sources.size() ==
         mTrainSet->getMotionData(motionDescr.id)->getPhysicalData()->getPhysicalDataSourceCount())
      {
        message += "   All Datasources are incoherent!\n\n";
      }
      
      for(int source = 0; source < motionDescr.sources.size(); source++)
      {
        SourceDescr sourceDescr = motionDescr.sources.at(source);
        
        message += "    - " + sourceDescr.name + ":\n";
        
        for(int channel = 0; channel < sourceDescr.channels.size(); channel++)
        {
          message += "        - " + sourceDescr.channels.at(channel).name + "\n";   
        }
        
        message += "\n";
      }
      message += "\n\n";      
    }
    
    mReportLogger->addMessage(message);      
  } 
  
  // create error statistics
  saveErrorStatistic(incoherentData);
  
  return true; 
}

bool CheckDataSourceIntegrity::optimizationEnd()
{
  bool ok = ModelOptimizer::optimizationEnd(); 
  
  
  return ok;
}

bool CheckDataSourceIntegrity::writeReportStartEntry()
{
  return true;
}
    
bool CheckDataSourceIntegrity::writeReportEndEntry()
{
  return true;
}

bool CheckDataSourceIntegrity::saveErrorStatistic(QList<MotionDescr> &incoherentData)
{
  if(incoherentData.empty()){
    return true;
  }
  
  // create folder which stores the files 
  QString directory = getOptimizationRunOutputDirectory() 
      + QDir::separator()
      + "IncoherentDataStatistics";
  
  Core::getInstance()->enforceDirectoryPath(directory);
  
  for(int motion = 0; motion < incoherentData.size(); motion++)
  {
    MotionDescr motionDescr = incoherentData.at(motion);
 
    MotionData *motionData = mTrainSet->getMotionData(motionDescr.id);
    
    for(int source = 0; source < motionDescr.sources.size(); source++)
    {
      SourceDescr sourceDescr = motionDescr.sources.at(source);
 
      QString filePath = directory 
          + QDir::separator()
          + sourceDescr.name.split(QDir::separator()).last()
          + ".stat";
      
      QFile file(filePath);
      if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        Core::log(QString("%1: ERROR - Could not create the file [%2]!").arg(
                  getName(), filePath));
		file.close();
		return false;
      }
        
      QTextStream out(&file);
          
      for(int channel = 0; channel < sourceDescr.channels.size(); channel++)
      {
        out << QString("# Simulation Data - %1\n").arg(sourceDescr.channels.at(channel).name);
        out << QString("# Physical Data - %1\n").arg(sourceDescr.channels.at(channel).name);
      }
      out << "\n";
        
      for(int i = 0; i < motionData->getMotionLength(); i++ )
      {
        for(int channel = 0; channel < sourceDescr.channels.size(); channel++)
        {
          if(channel > 0)
            out << ";";
          
          out << QString::number(motionData->getSimulationData()->getData(sourceDescr.channels.at(channel).id,i));
          out << ";";
          out << QString::number(motionData->getPhysicalData()->getPhysicalDataSource(sourceDescr.id)->getData(sourceDescr.channels.at(channel).id,i));
        }
        out << "\n";
      }
      
      file.close();
    }
  }
  
  return true;
}


} // namespace nerd

