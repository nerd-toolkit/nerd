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

#ifndef NERD_XMLCONFIGDOCUMENT_H
#define NERD_XMLCONFIGDOCUMENT_H

#include "Core/Object.h"
#include <QHash>

class QString;
class QDomDocument;
class QDomElement;

namespace nerd {

class XMLConfigDocument : public virtual Object {
  public:
    
    static const char SEPARATOR; 
    
    XMLConfigDocument(const QString &filePath);
    XMLConfigDocument(const QString &filePath,
                      const QHash<QString,QString> &stringTemplates);

    ~XMLConfigDocument();

    virtual bool init();
    
    virtual QString getName() const;
    
    QString getFileName() const;
    
    QDomElement getDocumentElement() const;
    QDomElement readElement(const QString &name) const;
    QDomElement readElement(const QDomElement &baseElem, const QString &name) const;  
  
    QDomElement readNextEqualElement(const QDomElement &currentElement) const;
  
    QString readConfigParameter(const QString &name, const QString &attributeName) const;
    QString readConfigParameter(const QDomElement &baseElem, const QString &elemName, 
                                const QString &attributeName) const;
      
    QString readAttribute(const QDomElement &baseElem, const QString &attributeName) const;
  protected:
  QString mFileName;
  QDomDocument * mXmlDom;  
  QHash<QString,QString> mStringTemplates;
};

} // namespace nerd
#endif
