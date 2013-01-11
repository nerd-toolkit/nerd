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

#include "XMLConfigDocument.h"

#include <QString>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>

#include "Core/Core.h"

namespace nerd {
  
const char XMLConfigDocument::SEPARATOR
    = ':';

XMLConfigDocument::XMLConfigDocument(const QString &filePath)
  : mFileName(filePath), mXmlDom(0) 
{}


XMLConfigDocument::XMLConfigDocument(const QString &filePath, 
                                     const QHash<QString,QString> &stringTemplates)
  : mFileName(filePath), mXmlDom(0) 
{
  mStringTemplates = QHash<QString,QString>(stringTemplates);

}

XMLConfigDocument::~XMLConfigDocument() 
{
  delete mXmlDom;
}

QString XMLConfigDocument::getName() const
{
  return "XMLConfigDocument";
}

/**
* Initializes the XMLConfigDocument. Opens the configuration file and creates
* the QDomDocument.
*
* @return TRUE if the initialization was successfully, otherwhise FALSE.
*/
bool XMLConfigDocument::init() 
{  
  this->mXmlDom = new QDomDocument("MyXMLConfigDocument");
  
  // Open config file  
  QFile file(mFileName.trimmed());
  if (!file.open(QIODevice::ReadOnly)) {
    Core::log("XMLConfigDocument: Configuration file \""+ mFileName + "\" could not be opened!");
    file.close();
    return false;
  }
  
  // Read xml in
  if(!this->mXmlDom->setContent(&file)) {
    file.close();
    Core::log("XMLConfigDocument: Error while parsing the xml-file \""+ mFileName + "\"! " +
              "Please check if the XML Syntax is correct.");
    return false;
  }
  
  file.close();
  
  return true;
}

QString XMLConfigDocument::getFileName() const {
  return mFileName;
}

QDomElement XMLConfigDocument::getDocumentElement() const
{
  return mXmlDom->documentElement();
}

QDomElement XMLConfigDocument::readElement(const QString &name) const
{
  return readElement(this->getDocumentElement(), name);
} 

QDomElement XMLConfigDocument::readElement(const QDomElement &baseElem, const QString &name) const
{
  QDomElement result;
  QString curElemName;
  
  int separator_count = name.count(SEPARATOR);
  
  if( separator_count <= 0)
    curElemName = name;
  else
    curElemName = name.section(SEPARATOR, 0, 0);
  
  QDomNode n = baseElem.firstChildElement(curElemName);
  if(!n.isNull()) 
  {
    result = n.toElement();
    
    if( separator_count > 0 )
    {
      QString changedName = name.section(SEPARATOR, 1, name.count(SEPARATOR));
   
      result = readElement(result, changedName);
    }
  }
  
  return result;
}
 
QDomElement XMLConfigDocument::readNextEqualElement(const QDomElement &currentElement) const
{
  return currentElement.nextSiblingElement(currentElement.tagName()); 
}

QString XMLConfigDocument::readConfigParameter(const QString &name, const QString &attributeName) const
{
  return this->readConfigParameter(this->getDocumentElement(), name, attributeName);
}

QString XMLConfigDocument::readConfigParameter(const QDomElement &baseElem, 
                                               const QString &elemName,
                                               const QString &attributeName) const
{
  QDomElement elem;
  
  if(elemName.isNull() || elemName.isEmpty()) {
    elem = baseElem;
  }
  else {
    elem = this->readElement(baseElem,elemName);
  }
  
  if(elem.isNull())
    return QString::null;
  else
    return this->readAttribute(elem, attributeName);  
}

/**
 * Reads the string value from an attribute. 
 * Checks furthermore if the string value contains a string template
 * and replaces it with the corresponding string.  
 * 
 * @param elem Element which contains the attribute.
 * @param attributeName Name of the attribute.
 * @return String value from the attribute.
 */
QString XMLConfigDocument::readAttribute(const QDomElement &elem, 
                                         const QString &attributeName) const
{
  QString val = elem.attribute(attributeName);
  
  QHash<QString, QString>::const_iterator i = mStringTemplates.constBegin();
  while(i != mStringTemplates.constEnd()) 
  {
    val = val.replace(i.key(), i.value());
    i++;
  }
    
  return val;
}


} // namespace nerd
