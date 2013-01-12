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
#ifndef NERD_TRIGGER_H
#define NERD_TRIGGER_H


#include "Core/Object.h"
#include <QStringList>
namespace nerd{ class StringValue; }
namespace nerd{ class DoubleValue; }
namespace nerd{ class Normalization; }

namespace nerd {

/**
* Trigger
*/
class Trigger : public Object {
  public:
    
		Trigger();
		
    Trigger(const QStringList &triggerConfigPaths);
    
    ~Trigger();

    virtual bool init();
    
    virtual QString getName() const;
    
    QString getTriggerValueName() const;
        
    virtual bool checkTrigger(double checkValue);

  protected:
    enum modus {UNKNOWN, BIGGER_THEN, SMALLER_THEN, BIGGER_EQUAL_THEN, SMALLER_EQUAL_THEN};
    
    static const QString MODUS_BIGGER_THEN;
    static const QString MODUS_SMALLER_THEN;
    static const QString MODUS_BIGGER_EQUAL_THEN;
    static const QString MODUS_SMALLER_EQUAL_THEN;
    
    static const QString PARAVALTYPE_NORMALIZED;
    static const QString PARAVALTYPE_SIMULATION;
    static const QString PARAVALTYPE_PHYSICAL;
		
		bool mIsActive;
    
    /**
     * Paths to the values which contain the cofiguration of the trigger.
     */
    QStringList mTriggerConfigPaths;
    
    /**
     * Name of the modus of the trigger which is defined in the configuration.
     */
    StringValue *mModusName;
    
    /**
     * Modus of the trigger which defines when the trigger triggers.
     */
    modus mModus;
    
    /**
     * String of the configured parameter value type for the parameter.
     * This defines how the parameter gets normalized.
     * Standard is "Normalized".
     */
    StringValue *mParameterValueType;
    
    /**
     * Normalization object which is used to normalize the parameter value.
     */
    Normalization *mParameterNormalization;
    
    /**
     * Name of the value from which the values are used to check if the 
     * gets active.
     */
    StringValue *mTriggerValueName;
    
    /**
     * Value which is used to check if the values from mTriggerValueName
     * full fill the configured modus.
     */
    DoubleValue *mParameter;
    
    
    bool readModusFromConfig();
    
    bool readParameterNormalizationFromConfig();
    
};

} // namespace nerd
#endif
