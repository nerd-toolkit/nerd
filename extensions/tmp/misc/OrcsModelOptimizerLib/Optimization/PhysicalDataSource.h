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

#ifndef NERD_PHYSICALDATASOURCE_H
#define NERD_PHYSICALDATASOURCE_H


#include "Core/Object.h"
#include <QVector>

class QString;
namespace nerd { class Trigger; } 
namespace nerd { class Normalization; } 
namespace nerd { class XMLConfigDocument; }
namespace nerd { class StringValue; }
namespace nerd { class DoubleValue; }
namespace nerd { class IntValue; }

namespace nerd {

class PhysicalDataSource : public Object {
  public:
		static PhysicalDataSource* createPhysicalDataSource(
																			Trigger *trigger,
																			Normalization *triggerNormalization,
																			const QVector<StringValue*> &trainDataValueNames,
																			const QVector<Normalization*> &trainDataValueNormalizations,
																			const QString &configDocumentPath, 
																			const QString &dataFilePath);
		
		PhysicalDataSource(	Trigger *trigger,
												Normalization *triggerNormalization,
												const QVector<StringValue*> &trainDataValueNames,
												const QVector<Normalization*> &trainDataValueNormalizations,
												const QString &configDocumentPath, 
												const QString &dataFilePath);
		

    virtual bool init();

    virtual QString getName() const = 0;
    
    virtual QString getSourceName() const;

    virtual int readData() = 0;
    
    virtual bool readData(int motionLengthInSteps) = 0;
    
    virtual double getData(int channel, int simulationStep) = 0;


  protected:
    
    QString mDataFilePath;
		
		XMLConfigDocument *mConfigDocument;
    
    QString mMotionConfigPath;
    
		Trigger *mTrigger;
		
		Normalization *mTriggerNormalization;
		
    QVector< StringValue* > mTrainDataValueNames;
    
    QVector< Normalization* > mNormalizations;    
   
};

} // namespace nerd
#endif
