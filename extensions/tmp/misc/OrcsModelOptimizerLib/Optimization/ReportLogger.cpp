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

#include "ReportLogger.h"

#include "OrcsModelOptimizerConstants.h"
#include "Core/Core.h"
#include "Value/StringValue.h"
#include "Optimization/Tools.h"

#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <iostream>

using namespace std;

namespace nerd {

  
ReportLogger::ReportLogger(const QString &configValuesPath) 
  : mConfigValuePath(configValuesPath),
    mInitialFilePath("Report.log"),
    mReportFilePath(0),
    mReportMessage(0),
    mFileSetupCompleted(false),
    mLoggingEnabled(false)
{ 
  Core::getInstance()->addSystemObject(this);
} 
  
ReportLogger::ReportLogger(const QString &configValuesPath,
                           const QString &filePath)
  : mConfigValuePath(configValuesPath),
    mInitialFilePath(filePath),
    mReportFilePath(0),
    mReportMessage(0),
    mFileSetupCompleted(false),
    mLoggingEnabled(false)
{ 
  Core::getInstance()->addSystemObject(this);
} 


ReportLogger::~ReportLogger() 
{
  
}


QString ReportLogger::getName() const {
  return "ReportLogger";
}


bool ReportLogger::init() 
{
  bool ok = true;
  
  if(!mConfigValuePath.endsWith("/")){
    mConfigValuePath.append("/");
  }
  
  ValueManager *vm = Core::getInstance()->getValueManager();
      
  mReportFilePath = Tools::getOrAddStringValue(vm, 
                           mConfigValuePath
                           + OrcsModelOptimizerConstants::VM_REPORT_FILEPATH_VALUE,
                           false,
                           getName(),
                           ok,
                           mInitialFilePath);
  
  mReportMessage = Tools::getOrAddStringValue(vm, 
                           mConfigValuePath
                           + OrcsModelOptimizerConstants::VM_REPORT_MESSAGE_VALUE,
                           false,
                           getName(),
                           ok,
                           "");
  return ok;
}


bool ReportLogger::bind() 
{
  bool ok = true;
  
  if(mReportFilePath != 0){
    mReportFilePath->addValueChangedListener(this);
  }
  else {
    Core::log(QString("%1 - bind(): ERROR - The Value [mReportFilePath] is not initialized!").arg(
              getName()));
    ok = false;
  }
    
  
  if(mReportMessage != 0){
    mReportMessage->addValueChangedListener(this);
  }
  else {
    Core::log(QString("%1 - bind(): ERROR - The Value [mReportMessage] is not initialized!").arg(
              getName()));
    ok = false;
  }
  
  return ok;
}


bool ReportLogger::cleanUp() {
  return true;
}

bool ReportLogger::setFilePath(const QString &filePath)
{
  if(mReportFilePath == 0){
    Core::log(QString("%1 - setFilePath(): Could not change file path to\"%2\", because the value [mReportFilePath] is not initialized!").arg(
              getName(),
              filePath));
    return false;
  }

  mReportFilePath->set(filePath);

  return true;   
}


StringValue* ReportLogger::getFilePathValue()
{
  return mReportFilePath;
}


bool ReportLogger::addMessage(const QString &reportMessage)
{
  if(mReportMessage == 0){
    Core::log(QString("%1 - addMessage(): Could not save report message \"%2\", because the value [mReportMessage] is not initialized!").arg(
            getName(),
            reportMessage));
    return false;
  }
  
  mReportMessage->set(reportMessage);
  
  return true;
}
    
StringValue* ReportLogger::getReportMessageValue()
{
  return mReportMessage;
}

void ReportLogger::valueChanged(Value *value)
{
  if(value == 0){
    return;
  }
  
  if(value == mReportMessage){
    saveReportMessageToFile();  
  }
  
  if(value == mReportFilePath){
    mFileSetupCompleted  = false;
    mLoggingEnabled = false;
  }
}


bool ReportLogger::createReportFile()
{
  mFileSetupCompleted = true;
  
  QFile file(mReportFilePath->get());
  QFileInfo fileInfo(file);
  
  Core::getInstance()->enforceDirectoryPath(fileInfo.absolutePath());
  
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    Core::log(QString("%1: ERROR - Could not create file [%2] to write report!").arg(
                 getName(),
                 mReportFilePath->get()));
    file.close();
    return false;
  }

  file.close();

  mLoggingEnabled = true;
  
  return true;
}
    
bool ReportLogger::saveReportMessageToFile()
{
  bool ok = true;
  
  QFile file(mReportFilePath->get());
  
  if(!file.exists() && mFileSetupCompleted == false)
  {
    ok &= createReportFile();
    
    if(ok == false){
      return false;
    }    
  }
  
  if(mLoggingEnabled == true)
  {  
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
      Core::log(QString("%1: ERROR - Could not open file [%2] to write report!").arg(
                  getName(),
                  mReportFilePath->get()));
      file.close();
      return false;
    }
  
    QTextStream output(&file);
  
    output << mReportMessage->get();
    output << "\n";
  
    file.close();
  }
    
  return ok;
}

}


