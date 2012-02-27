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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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

#include "XMLConfigLoader.h"

#include <QString>

#include <QDomElement>
#include <QDomNodeList>

#include "Core/Core.h"
#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include "Value/BoolValue.h"
#include "Value/IntValue.h"

#include "XMLConfigDocument.h"

namespace nerd 
{
  
const QString XMLConfigLoader::TYPE_ATTRIBUTE = "type";
const QString XMLConfigLoader::ORCS_NAME_ATTRIBUTE = "orcs";
const QString XMLConfigLoader::MISSING_ATTRIBUTE = "missing";   
const QString XMLConfigLoader::OVERRIDE_ATTRIBUTE = "override";  

XMLConfigLoader::XMLConfigLoader(const QString &configFile, 
                                 const QString &configDescrFile)
{
  mConfigFile = new XMLConfigDocument(configFile);
  mConfigDescrFile = new XMLConfigDocument(configDescrFile);
  
  mValueManager = Core::getInstance()->getValueManager();
}



XMLConfigLoader::XMLConfigLoader(const QString &configFile, 
                                 const QString &configDescrFile,
                                 const QHash<QString,QString> &stringTemplates)
{
  mConfigFile = new XMLConfigDocument(configFile, stringTemplates);
  mConfigDescrFile = new XMLConfigDocument(configDescrFile, stringTemplates);
  mStringTemplates = QHash<QString,QString>(stringTemplates);
  
  mValueManager = Core::getInstance()->getValueManager();
}

XMLConfigLoader::~XMLConfigLoader() 
{
  delete mConfigFile;
  delete mConfigDescrFile;
}

QString XMLConfigLoader::getName() const{
  return "XMLConfigLoader";
}

/**
* Initializes the XMLConfigLoader. Opens the configuration file and creates
* the QDomDocument.
*
* @return TRUE if the initialization was successfully, otherwhise FALSE.
*/
bool XMLConfigLoader::init() 
{ 
  bool ok = true;
   
  ok &= mConfigFile->init();
  ok &= mConfigDescrFile->init();
  
  return ok;
}

/**
 * Generates new Values in the ValueManager for configuration parameters in the configuration 
 * file if they do not exist or change their value if they exist. 
 *
 * @return TRUE if the load process was successfully, otherwhise FALSE.
 */
bool XMLConfigLoader::loadFromXML() 
{ 
  QDomElement descrStartElem  = mConfigDescrFile->getDocumentElement().firstChildElement();
  QDomElement configStartElem = mConfigFile->getDocumentElement();
  
  if(descrStartElem.isNull()){
    Core::log( "XMLConfigLoader: ERROR - In file config description file [" + mConfigDescrFile->getFileName() + "]. " +
        "No elements found!" ); 
    return false;
  }
    
  if(configStartElem.isNull()){
    Core::log( "XMLConfigLoader: ERROR - In file config file [" + mConfigFile->getFileName() + 
        "]. " + "No elements found!" ); 
    return false;
  }
  
  QString orcsPath = "";
  
  return loadElement(descrStartElem, configStartElem, orcsPath, true);
}

bool XMLConfigLoader::loadElement(QDomElement descrElement, 
                                  QDomElement configElement, 
                                  const QString &orcsPath,
                                  const bool parentIsOverride)
{
  bool ok = true;
  
  // Get Description Element Information
  QString descrElemName = descrElement.tagName();
  
  // read attributes
  ElementType type = this->convertToElementType(
      mConfigDescrFile->readAttribute(descrElement, TYPE_ATTRIBUTE));
  
  QString orcsName = mConfigDescrFile->readAttribute(descrElement, ORCS_NAME_ATTRIBUTE);
  
  MissingAction missingAction = this->convertToMissingAction(
      mConfigDescrFile->readAttribute(descrElement, MISSING_ATTRIBUTE));
  
  bool isOverride = true;
  QString isOverrideStr = mConfigDescrFile->readAttribute(descrElement, OVERRIDE_ATTRIBUTE);
  if(isOverrideStr.isNull() == true){
    isOverride = parentIsOverride;
  }
  else {
    
    BoolValue *converter = new BoolValue();
    
    if(!converter->setValueFromString(isOverrideStr)){
      ok = false;
      Core::log( "XMLConfigLoader: ERROR - In file config description file [" 
          + mConfigDescrFile->getFileName() + "]. " +
          "The value \"" + isOverrideStr + "\" for the attribute [" + 
          OVERRIDE_ATTRIBUTE + "] is in a wrong format!"); 
    }
    
    isOverride = converter->get();
    delete converter;
  }
  
  // stop if a type or missing action can not be interpreted
  if(type < 0 || missingAction < 0)
    return false;
  
  QString newOrcsPath = orcsPath + orcsName;
   
  if(type == LIST || type == PATH)
  {
    ////////////////////////////////////////////////////////////////////////////////////
    // BEGIN: iteration over CONFIGURATION elements
     
    // get the configuration element which is represents the path or list element
    QDomElement configPathElem = configElement.firstChildElement(descrElement.tagName());
  
    // check if in the configuration file exists a element
    if(configPathElem.isNull())
    { 
      if(missingAction != NOTHING)
      {
        QString message;
        if(missingAction == WARNING)
          message = "Warning";
        else {
          message = "ERROR";
          ok = false;
        }
        Core::log("XMLConfigLoader: " + message + " - The parameter [" + 
            getFullPath(descrElement) + "] is missing in the configuration file [" +
            mConfigFile->getFileName() + "].");
      }
    }
    else
    {
      int i = 1;
      while(!configPathElem.isNull())
      {  
        ////////////////////////////////////////////////////////////////////////////////////
        // BEGIN: iteration over configuration DESCRIPTION elements
 
        QDomElement descrPathChildElem = descrElement.firstChildElement();
        
        // stop if a path or list description element has no children
        if(descrPathChildElem.isNull()){
          Core::log( "XMLConfigLoader: ERROR - In file config description file [" +
              mConfigDescrFile->getFileName() + "]. " +
              "No child elements for the path or list element [" + 
              getFullPath(descrElement) + "]." ); 
          return false;
        }
        
        while(!descrPathChildElem.isNull())
        {
          QString tmp = (type == LIST ? newOrcsPath.arg(i) : newOrcsPath);
          
          ok &= this->loadElement(descrPathChildElem, 
                                  configPathElem, 
                                  tmp,
                                  isOverride);
          descrPathChildElem = descrPathChildElem.nextSiblingElement(); 
        }
        
        // END: iteration over configuration DESCRIPTION elements
        //////////////////////////////////////////////////////////////////////////////////// 
        
        
        configPathElem = configPathElem.nextSiblingElement(configPathElem.tagName());
        i++;
        
        // stop if the element is a path
        if(type == PATH)
          break;
      }    
    }
    
    // END: iteration over CONFIGURATION elements
    //////////////////////////////////////////////////////////////////////////////////// 
  }
  else
    ok &= loadConfigParameter(configElement, 
                              descrElemName, 
                              newOrcsPath, 
                              type, 
                              missingAction, 
                              isOverride);
  
  return ok;
}

XMLConfigLoader::ElementType XMLConfigLoader::convertToElementType( const QString & typeStr )
{
  if(typeStr == "path")
    return PATH;
  if(typeStr == "list")
    return LIST;
  if(typeStr == "string")
    return STRING;
  if(typeStr == "double")
    return DOUBLE;
  if(typeStr == "int")
    return INT;
  if(typeStr == "bool")
    return BOOL;
  
    // if not found --> error
  Core::log( "XMLConfigLoader: ERROR - In file config description file [" 
      + mConfigDescrFile->getFileName() + "]. " +
      "Do not know " + TYPE_ATTRIBUTE + " attribute [" + typeStr + "]!" ); 
  
  return (XMLConfigLoader::ElementType)-1;
}

XMLConfigLoader::MissingAction XMLConfigLoader::convertToMissingAction( const QString & missingActionStr )
{ 
  if(missingActionStr == "error")
    return ERROR;
  if(missingActionStr == "warning")
    return WARNING;
  if(missingActionStr == "nothing")
    return NOTHING;
  
  // if not found --> error
  Core::log( "XMLConfigLoader: ERROR - In file config description file [" 
      + mConfigDescrFile->getFileName() + "]. " +
      "Do not know value \"" + missingActionStr + "\" for the attribute [" + 
      MISSING_ATTRIBUTE + "]!"); 
  
  return (XMLConfigLoader::MissingAction)-1;
}

bool XMLConfigLoader::loadConfigParameter(QDomElement configElement, 
                                           const QString & attributeName,
                                           const QString & orcsName,
                                           const ElementType type, 
                                           const MissingAction missingAction,
                                           const bool isOverride)
{
  bool ok = true;
  
  QString val = mConfigFile->readAttribute(configElement,attributeName);
  
  if(val.isNull())
  {
    if(missingAction != NOTHING)
    {
      QString message;
      if(missingAction == WARNING)
        message = "Warning";
      else
      {
        message = "ERROR";
        ok = false;
      }
       
      Core::log("XMLConfigLoader: " + message + " - The parameter [" + 
          getFullPath(configElement) + "/" + attributeName + 
          "] is missing in the configuration file [" + mConfigFile->getFileName() + "].");
    }
  }
  else
  {
    if(!this->addOrSetValue(orcsName, type, val, isOverride))
     {
       ok = false;
       Core::log( "XMLConfigLoader: ERROR - Can not set Value [" + orcsName + "] to " +
                  "\"" + val + "\"! Please check if the valuetype is correct." );       
     }
  }
  
  return ok;
}

bool XMLConfigLoader::addOrSetValue(const QString & name, 
                                    const ElementType type, 
                                    const QString & val,
                                    const bool isOverride)
{  
  bool ok = true;
  
  Value * valueRef = mValueManager->getValue(name);
  
  if(valueRef == 0)
  {
    switch(type)
    {
      case STRING:  valueRef = new StringValue();
                    break;
      case DOUBLE:  valueRef = new DoubleValue();
                    break;
      case BOOL:    valueRef = new BoolValue();
                    break;
      case INT:     valueRef = new IntValue();
                    break;      
      default:
      {
        Core::log("XMLConfigLoader: Do not know ValueType [" + QString::number(type) + 
                  "] for value " + name);
        return false;
      }
    }
    
    ok &= mValueManager->addValue(name, valueRef);
    ok &= valueRef->setValueFromString(val);
  }
  else
  {
    if(isOverride == true){
      ok &= valueRef->setValueFromString(val);
    }
  }
  
  return ok;
  
}

/**
 * Returns the path to the given XML Element in the document,
 * Example:
 *  <elem1>
 *    <elem2 />
 *  </elem1>
 *  
 *  Input: elem2
 *  Output: "elem1/elem2"
 * 
 * @return QString with the path to to the QDomElement.
 */
QString XMLConfigLoader::getFullPath(const QDomElement &element)
{
  QString path = element.tagName();
  
  QDomNode parent = element.parentNode();
  while(!parent.isNull())
  {
    if(parent.isElement() && !parent.parentNode().isDocument())
    {
      path = path.prepend(parent.toElement().tagName() + "/");
    }
    parent = parent.parentNode();
  }
  
  return path;
}

} // namespace nerd
