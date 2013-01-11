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

#ifndef NERD_MODELOPTIMIZER_H
#define NERD_MODELOPTIMIZER_H


#include "Core/Object.h"

#include <QVector>
#include <QList>
#include <QDateTime>

class QString;
namespace nerd { class TrainSet; } 
namespace nerd { class Value; } 
namespace nerd { class StringValue; } 
namespace nerd { class DoubleValue; }
namespace nerd { class BoolValue; }
namespace nerd { class IntValue; }  
namespace nerd { class Event; } 
namespace nerd { class StatisticsLogger; }
namespace nerd { class ReportLogger; }
namespace nerd { class StatisticCalculator; }


namespace nerd {

/**
 * ParameterDescription
 * 
 * Holds the Value and the configuration of a parameter which should be optimized by a
 * ModelOptimizer.
 */  
struct ParameterDescription {
  Value *value;
  
  StringValue *name;
  
  DoubleValue *min;
  
  DoubleValue *max;
  
  StringValue *init;
  
};  
  
 /**
 * ModelOptimizer
 * 
 * Baseclass for all classes which implement a model optimizer algorithm. 
 * The optimization algorithm is started via the method startOptmization() which 
 * must be overloaded. A TrainSet is provided which holds the data from the physical 
 * robot and the data from the simulation.
 * 
 * Furthermore provides the baseclass the static method createModelOptimizer which
 * is used by the OptimizationManager to create certain objects of inheritances of
 * the ModelOptimizer class. For every new model optimizer must also the optimizer
 * their registered.  
 */
class ModelOptimizer : public Object {
  public:
    static ModelOptimizer* createModelOptimizer(  const QString &optimizer, 
                                                  const QString &modelConfigPath,
                                                  TrainSet *trainSet);
    
    ModelOptimizer(const QString &modelConfigPath, 
                   TrainSet *trainSet);
    
    ~ModelOptimizer();

    virtual bool init();
    
    virtual QString getName() const = 0;
    
    bool callOptimizationStart();
    
    bool callNextOptimizationStep();
    
    bool callOptimizationEnd();
        
  protected:
    /**
     * Path of the Values which configr
     */
    QString mModelConfigPath;
    
    /**
    * TrainSet which provides the data to compare the model with the real system. 
    */
    TrainSet *mTrainSet;
    
    /**
     * Path to the directory for the output files.
     */
    StringValue *mOutputDirectory;
    
    /**
     * Name of the model which should be optimized. 
     * Used for the filename of the output files.
     */
    StringValue *mModelName;
    
    /**
     * Defines if the statistic files which compares the simulation data and the physical
     * data should be printed.
     */
    BoolValue *mIsPrintTraindataStatistics;
 
    /**
     * Parameters from the model which should be optimized.
     */
    QVector<ParameterDescription*> mToOptimizeParameters;
    
    /**
     * Paths of the model and parameter values which should be saved
     * in the result file after the optimization.
     */
    QList<QString> mToSaveModelValues;
    
    /**
     * Contains either values or file paths to value files which should be loaded 
     * before the optimization process starts.
     * The entries have certain format to differentiate values and files:
     *  values: "value:<valuepath>:<value of value>" Example: "value:/Simulation/Steps:400"
     *  files: "file:<filepath>" Example: "file:./Optimization_init.val"
     */
    QVector<QString> mInitialValues;
    
    
    Event *mOptimizationStepCompleted;
    
    Event *mOptimizationRunCompleted;
       
    StatisticsLogger *mStatisticsLogger;
    
    ReportLogger *mReportLogger;
            
    IntValue *mSimulationRuns;
    
    IntValue *mMaxSimulationRuns;
    
    IntValue *mMaxIterations;
    
    IntValue *mIterationCount;
    
    QString mStepCompletedEventName;
  
    QString mOptimizationCompletedEventName;
  
    QString mOptimizationStatisticCompletedEventName;
    
    /**
     * Time when optimization starts (callOptimizationStart() - method is called). 
     * Used for filenames and for informations in the report file.
     */
    QDateTime mOptimizationStartTime;
    
    bool readToOptimizeParameters();
    bool readToSaveModelValues();
    bool readInitialValues();
    
    virtual ParameterDescription* createParameterDescription(
        const QString &parameterConfigPath,
        bool *ok);
    
    virtual bool optimizationStart();
    
    /**
     * Does one optimization step.
     * 
     * Important: The mSimulationRun value should be updated during the
     * optimization step.
     */
    virtual bool doNextOptimizationStep() = 0;
    
    virtual bool optimizationEnd();
    
    virtual bool writeReportStartEntry();
    
    virtual bool writeReportEndEntry();
    
    virtual QString getFileNameBegin();
    
    virtual QString getOptimizationRunOutputDirectory();
    
    virtual bool saveTraindataStatistic(const QString &fileName);
    
    virtual bool saveToOptimizeParameters(const QString &filePath);
		
		bool appendStatistic(StatisticCalculator *childStatistic);
		bool appendStatistic(Value *statisticValue,const QString &name);
		bool appendStatistic(QVector<ParameterDescription*> mToOptimizeParameters);
};

} // namespace nerd
#endif
