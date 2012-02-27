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


#ifndef NERD_ReportLogger_H
#define NERD_ReportLogger_H

#include "Core/SystemObject.h"
#include "Value/ValueChangedListener.h"

#include "NerdConstants.h"

namespace nerd { class StringValue; }

namespace nerd {

  /**
   * ReportLogger.
   * 
   * Komponent to save reports into a file. 
   */
  class ReportLogger : public virtual SystemObject, public virtual ValueChangedListener
  {
  public:
    ReportLogger(const QString &configValuesPath);   
    
    ReportLogger(const QString &configValuesPath,
                 const QString &filePath);   
    
    ~ReportLogger();

    virtual QString getName() const;

    virtual bool init();
    
    virtual bool bind();
    
    virtual bool cleanUp();
    
    virtual bool setFilePath(const QString &filePath);
    
    virtual StringValue* getFilePathValue();
    
    virtual bool addMessage(const QString &reportMessage);
    
    virtual StringValue* getReportMessageValue();
    
    virtual void valueChanged(Value *value);
    
  protected:
    
    /**
     * Path to the configuration values of the ReportLogger.
     */
    QString mConfigValuePath;
    
    /**
     * Path of the Reportfile which is defined at the constructor of the
     * ReportLogger and used during the init() as first value for the 
     * mReportFilePath. 
     */
    QString mInitialFilePath;
        
    /**
     * Path of the Reportfile in which the report messages are saved.
     */
    StringValue *mReportFilePath;
    
    /**
     * ReportMessage which should be saved to the Reportfile.
     */
    StringValue *mReportMessage;
    
    bool mFileSetupCompleted;
    
    bool mLoggingEnabled;
    
    virtual bool createReportFile();
    
    virtual bool saveReportMessageToFile();
  };

}

#endif


