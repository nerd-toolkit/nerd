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

#ifndef NERD_XMLCONFIGLOADER_H
#define NERD_XMLCONFIGLOADER_H

#include "Core/Object.h"
#include <QHash>

class QString;
class QDomElement;

namespace nerd { class ValueManager; }
namespace nerd { class XMLConfigDocument; }

namespace nerd {

class XMLConfigLoader : public virtual Object {
  public:
    XMLConfigLoader(const QString &configFile, 
                    const QString &configDescrFile);
    
    XMLConfigLoader(const QString &configFile, 
                    const QString &configDescrFile,
                    const QHash<QString,QString> &stringTemplates);
    
    ~XMLConfigLoader();

    virtual bool init();
    
    virtual QString getName() const;

    virtual bool loadFromXML();

  protected:
    
    enum ElementType {DOUBLE, INT, STRING, BOOL, PATH, LIST};
    enum MissingAction {ERROR, WARNING, NOTHING};
    
    static const QString TYPE_ATTRIBUTE;
    static const QString ORCS_NAME_ATTRIBUTE;
    static const QString MISSING_ATTRIBUTE;  
    static const QString OVERRIDE_ATTRIBUTE; 
    
    XMLConfigDocument * mConfigFile;
    XMLConfigDocument * mConfigDescrFile;
    
    ValueManager * mValueManager;
    
    QHash<QString,QString> mStringTemplates;
    
    virtual bool loadElement(QDomElement descrElement, 
                             QDomElement configElement, 
                             const QString &orcsPath,
                             const bool parentIsOverride = true);
    
    virtual bool addOrSetValue(const QString &name, 
                             const ElementType type, 
                             const QString &val,
                             const bool isOverride = true);
        
    virtual bool loadConfigParameter( QDomElement configElement, 
                                      const QString &attributeName,
                                      const QString &orcsName, 
                                      const ElementType type, 
                                      const MissingAction missingAction = NOTHING,
                                      const bool isOverride = true);
    
    // Helper methods
    ElementType convertToElementType( const QString &typeStr );
    MissingAction convertToMissingAction( const QString &missingActionStr );
    QString getFullPath(const QDomElement &element);
};

} // namespace nerd
#endif
