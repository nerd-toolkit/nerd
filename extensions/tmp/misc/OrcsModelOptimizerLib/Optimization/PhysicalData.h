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

#ifndef NERD_PHYSICALDATA_H
#define NERD_PHYSICALDATA_H


#include "Core/Object.h"
#include <QVector>

class QString;
namespace nerd { class PhysicalDataSource; } 
namespace nerd { class StringValue; } 
namespace nerd { class IntValue; }
namespace nerd { class DoubleValue; }
namespace nerd { class Trigger; }
namespace nerd { class Normalization;}

namespace nerd {

class PhysicalData : public Object {
  public:
    PhysicalData(const QString &motionConfigPath, Trigger *trigger);

    ~PhysicalData();

    virtual QString getName() const;
    
    virtual bool init();

    virtual int readData();
    
    virtual bool readData(int lengthInSimSteps, bool alreadyReadData = false);
    
    virtual int getMotionLength() const;

    virtual double getMeanData(int channel, int simulationStep);

    virtual double getStandartDeviation(int channel, int simulationStep);
    
    virtual int getPhysicalDataSourceCount();
    
    virtual PhysicalDataSource* getPhysicalDataSource(int source);
    
  protected:
    
    QString mMotionConfigPath;
    
    /** 
     * Used trigger to check for the start point of data recording.
     * Is given to the PhysicalDataSources.
     */ 
    Trigger *mTrigger;
		
		/**
		 * Normalization of the trigger value which is used by the 
		 * PhysicalDataSources. 
		 */
		Normalization *mTriggerNormalization;
    
    /** 
     * Name of the motion which is simulated
     */ 
    StringValue *mMotionName;
    
    /** 
     * Path to the scenario folder which contains the motion folders 
     */
    StringValue *mScenarioPath;
		
		/** 
		 * Path to the format file with specification for the PhysicalDatasources.
		 * For example: "./config/ASerieBinDescription.xml"
		 */
		QString mFormatFilepath;
		
		/** 
		 * Source type of the PhysicalDataSources.
		 * For example: "Binary"
		 */
		//StringValue *mDataSourceType;
    
    int mCurMotionLengthInSimSteps;
    
    QVector<PhysicalDataSource*> mPhysicalDataSources;
    
    QVector< StringValue* > mTrainDataValueNames; 
		
		/**
		 * Normalizations of the TrainDataValues which are used by 
		 * the PhysicalDataSources. The order is the same as in the 
		 * mTrainDataValueNames.
		 */
		QVector< Normalization* > mNormalizations;
    
    QVector< QVector<double>* > mMeanData;

    QVector< QVector<double>* > mStandartDeviation;
    
    
    
    virtual bool deleteData();

};

} // namespace nerd
#endif
