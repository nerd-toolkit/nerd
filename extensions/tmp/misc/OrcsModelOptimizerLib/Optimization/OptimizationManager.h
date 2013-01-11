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

#ifndef NERD_OPTIMIZATIONMANAGER_H
#define NERD_OPTIMIZATIONMANAGER_H


#include <QThread>
#include <QVector>
#include <QHash>
#include "Core/SystemObject.h"
#include "Event/EventListener.h"


class QString; 
namespace nerd { class ModelOptimizer; } 
namespace nerd { class TrainSet; } 
namespace nerd { class XMLConfigLoader; } 
namespace nerd { class KeyFramePlayer; } 
namespace nerd { class EvaluationLoop; } 
namespace nerd { class Value; } 
namespace nerd { class StringValue; } 
namespace nerd { class BoolValue; }
namespace nerd { class Event; } 
namespace nerd { class SimObject; } 

namespace nerd {

/**
* OptimizationManager
* Mangages the optimization process. Creates and holds the TrainSet and the instances of the
* ModelOptimizer which is used.
*/

class OptimizationManager : public QThread, public virtual SystemObject, 
                            public virtual EventListener 
  
{
  Q_OBJECT 
  public:       
    OptimizationManager(const QString &xmlConfigFile);
    
    ~OptimizationManager();

    virtual QString getName() const;

    virtual bool init();

    virtual bool bind();

    virtual bool cleanUp();
    
    virtual void run();
        
    virtual void eventOccured(Event *event);
    
  signals:
    void quitMainApplication();

  protected:
       
		XMLConfigLoader *mXMLConfigLoader;
    
		EvaluationLoop *mEvaluationLoop;
		
		bool mIsConfigError;
		
		bool mDoShutDown;
		
    Event *mShutDownEvent;
    
		StringValue *mScenarioVal;
		
    BoolValue *mRunOptimizationValue;
    BoolValue *mPauseOptimizationValue;
    BoolValue *mShutDownAfterOptimization;
   
    /**
     * TrainSet which is used for all ModelOptimizer in mModelOptimizerVector
     */
    TrainSet *mTrainSet;
    
    /**
     * List of ModelOptimizer which are used during the optimization process.
     * The list is ordered like the entries in the configuration file.
     */
    QVector< ModelOptimizer* > mModelOptimizerVector;
		    
    /**
     * Snapshot which will be created at begin before any value files are loaded.
     * Is loaded everytime before a new optimization process starts to undo the changes
     * to the simulation values during the optimization process.
     */
    QHash<SimObject*, QHash<Value*, QString> > mInitialSnapshot;
        
    /**
     * Snapshot which will be created after the scenario value files are loaded.
     * Is loaded before a ModelOptimizer starts its optimization process to undo the changes
     * to the simulation values done by previous ModelOptimizer runs.
     */    
    QHash<SimObject*, QHash<Value*, QString> > mOptimizationRunSnapshot;
    
    virtual bool initializeOptimizationRun();
    virtual void destroyOptimizationComponents();
};

} // namespace nerd
#endif
