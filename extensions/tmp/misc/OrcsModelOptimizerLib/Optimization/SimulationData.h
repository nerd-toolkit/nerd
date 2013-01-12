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
 
#ifndef NERD_SIMULATIONDATA_H
#define NERD_SIMULATIONDATA_H


#include "Core/Object.h"
#include "Event/EventListener.h"
#include <QVector>
#include <QHash>

namespace nerd { class StringValue; } 
namespace nerd { class DoubleValue; }
namespace nerd { class IntValue; }
namespace nerd { class Trigger; } 
namespace nerd { class Normalization; } 
namespace nerd { class EvaluationLoop; } 
namespace nerd { class Event; } 
namespace nerd { class Value; } 
namespace nerd { class SimObject; } 
namespace nerd { class Player; }

namespace nerd {

/**
* SimulationData
* Used as an interface to the simulation for a certain motion. Collects data from the simulation
* and stores it. The recorded data is is normalized through a Normalization object. A Trigger 
* defines the startingpoint of the recording.
* The motion is defined by a MotionDescription file (used for the KeyFramePlayer) and 
* by a value file (used by the ValueManager).  
* 
*/

// TODO: Maybe better to implement a own simulationloop which can be controlled better
class SimulationData : public virtual Object, public virtual EventListener 
  {
  public:
    SimulationData(const QString &motionConfigPath, 
                   EvaluationLoop *evaluationLoop,
                   Trigger *mTrigger);

    ~SimulationData();

    virtual QString getName() const;
    
    virtual bool init();
    
    virtual void eventOccured(Event *event);

    virtual bool collectNewData(int lengthInSimSteps);

    virtual double getData(int channel, int simulationStep);

    virtual int getMotionLength() const;
    
    virtual int getChannelCount() const;
    
    virtual QString getMotionName() const;
    
    virtual QString getChannelName(int channel) const;
    
    virtual void setInitialSnapshot(
               QHash<SimObject*, QHash<Value*, QString> > mInitialSnapshot);

  protected:
    
    /** 
     * ORCS Value path of the configuration of the motion which is simulated
     */
    QString mMotionConfigPath;
    
    /** 
     * Name of the motion which is simulated
     */ 
    StringValue *mMotionName;
    
    /** 
     * Path to the scenario folder which contains the motion folders 
     */
    StringValue *mScenarioPath;
                    
    QVector< DoubleValue* > mDataValues;    
    /** 
     * Normalization objects for the different channels.
     * Index corresponds to the channel index.
     */
    QVector< Normalization* > mNormalizations;
    
		Trigger *mTrigger;
		
    /** 
     * Normalization object which is used to normalize the trigger value.
     */
    Normalization *mTriggerNormalization;
    
		bool mIsTriggered;
		
		DoubleValue *mTriggerValue;
    
		Player *mPlayer;
		
		StringValue *mPlayerSource;

		EvaluationLoop *mEvaluationLoop;
  
		IntValue *mSimNumberOfTries;
		IntValue *mSimNumberOfSteps;
		
		Event *mStepCompletedEvent;
		Event *mTryCompletedEvent;
		Event *mNextTryEvent;
		Event *mTerminateTryEvent;
		
		int mStepsFromTriggerOnset;	
		
		bool mIsFirstSimStep;
		
    QVector< QString > mValueParameterFiles;
  
    QHash<SimObject*, QHash<Value*, QString> > mInitialSnapshot;
		
		/** 
		 * Stores the number which simulationsteps which should be recorded after 
		 * the trigger onset. Is set by the lengthInSimSteps parameter from the
		 * collectNewSimData() method.  
		*/
		int mCurMotionLengthInSimSteps;

		
		QVector< QVector<double>* > mData;
    
};

} // namespace nerd
#endif
