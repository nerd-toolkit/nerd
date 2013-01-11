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

#ifndef NERD_MOTIONDATA_H
#define NERD_MOTIONDATA_H

#include "Core/Object.h"

#include <QHash>

class QString;
namespace nerd { class SimulationData; }
namespace nerd { class PhysicalData; }
namespace nerd { class DoubleValue; }
namespace nerd { class IntValue; }
namespace nerd { class KeyFramePlayer; }
namespace nerd { class EvaluationLoop; }
namespace nerd { class SimObject; }
namespace nerd { class Trigger; }
namespace nerd { class Value; }

namespace nerd {

/**
 * MotionData
 * 
 * Holds the SimulationData and PhysicalData for one certain Motion.
 * Used by the TrainSet to hold the data for all used motions.
 */
  class MotionData : public Object
{
  public:
    
    MotionData(const QString &motionConfigPath,
               EvaluationLoop *evaluationLoop);
    
    ~MotionData();
    
    virtual QString getName() const; 
    
    bool init();
 
    int getMotionLength() const;
    
    int getChannelCount() const;
    
    QString getMotionName() const;
    
    QString getChannelName(int channel) const;
    
    bool collectNewSimData();
    
    bool readPhysicalData();
    
    SimulationData* getSimulationData() const;
    
    PhysicalData* getPhysicalData() const;
    

    
  protected:
    
    /**
     * Path to the Values which contain the configuration for the motion. 
     */
    QString mMotionConfigPath;
      
    /** 
     * Needed to create SimulationData. 
     */
    EvaluationLoop *mEvaluationLoop;
    
    /** 
     * Length of the simulated motion after the trigger occurs in milliseconds.
     * This is the value which the users defines in the configuration file. 
     */
    IntValue *mMotionLengthInMsec;
    
    /** 
     * Length of the simulated motion after the trigger occurs in simulationsteps.
     * This value is calculated with help of the mMotionLengthInMsec and
     * mSimTimeStepSize. 
     */ 
    int mMotionLengthInSimSteps;
    
    /**
     * Trigger which is used to bring the recorded simulation and physical data
     * into phase.
     */
    Trigger *mTrigger;
    
    SimulationData *mSimulationData;
    
    PhysicalData *mPhysicalData;
};

} // namespace nerd
#endif
