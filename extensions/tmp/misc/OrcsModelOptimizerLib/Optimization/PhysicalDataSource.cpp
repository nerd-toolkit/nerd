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


#include "PhysicalDataSource.h"

#include "OrcsModelOptimizerConstants.h"
#include "SimulationConstants.h"

#include <QString>
#include <QStringList>
#include <QDomElement>
#include <QHash>

#include "Core/Core.h"
#include "Value/StringValue.h"

#include "Trigger.h"
#include "Normalization.h"
#include "XMLConfigDocument.h"
#include "XMLConfigLoader.h"
#include "BinFilePhysicalDataSource.h"
#include "TextFilePhysicalDataSource.h"
#include "Tools.h"



namespace nerd {

PhysicalDataSource* PhysicalDataSource::createPhysicalDataSource(
																			Trigger *trigger,
																			Normalization *triggerNormalization,
																			const QVector<StringValue*> &trainDataValueNames,
																			const QVector<Normalization*> &trainDataValueNormalizations,
																			const QString &configDocumentPath, 
																			const QString &dataFilePath)
{
	XMLConfigDocument *configDocument = new XMLConfigDocument(configDocumentPath);
	
	if(!configDocument->init()){
		return 0;
	}
	
	QString dataSourceType = configDocument->readConfigParameter(
																			"physicalDataSourceDescription",
																			"name");
	
	delete configDocument;
	
	if(dataSourceType.isNull() == true){
		Core::log(QString("PhysicalDataSource: ERROR - Could not find attribute [physicalDataSourceDescription/name] in the configuration file [%1]!" ).arg(configDocumentPath));
		return 0;
	}
	
	if(dataSourceType == OrcsModelOptimizerConstants::PDS_BINARY) {
		return new BinFilePhysicalDataSource(	trigger,
												triggerNormalization,
												trainDataValueNames,
												trainDataValueNormalizations,
												configDocumentPath, 
												dataFilePath);
	}
	if(dataSourceType == OrcsModelOptimizerConstants::PDS_TEXT) {
		return new TextFilePhysicalDataSource(	trigger,
												triggerNormalization,
												trainDataValueNames,
												trainDataValueNormalizations,
												configDocumentPath, 
												dataFilePath);
	}

  // error if the type is unknown
  Core::log(QString("PhysicalDataSource: ERROR - Do not know PhysicalDataSource type \"%1\"]. Possible types are: %2").arg(
						dataSourceType,
            OrcsModelOptimizerConstants::PDS_BINARY));
  return 0;
}




PhysicalDataSource::PhysicalDataSource(	Trigger *trigger,
																			Normalization *triggerNormalization,
																			const QVector<StringValue*> &trainDataValueNames,
																			const QVector<Normalization*> &trainDataValueNormalizations,
																			const QString &configDocumentPath, 
																			const QString &dataFilePath)
	:	mDataFilePath(dataFilePath),
    mTrigger(trigger),
    mTriggerNormalization(triggerNormalization),
    mTrainDataValueNames(trainDataValueNames),
		mNormalizations(trainDataValueNormalizations)
{
	mConfigDocument = new XMLConfigDocument(configDocumentPath);
}

bool PhysicalDataSource::init()
{
  bool ok = true;
	
	ok &= mConfigDocument->init();

  return ok;
}

/**
 * Returns the name of the source.
 * This corresponds to the file path of the source data file.
 *
 * @return Name of source.
 */
QString PhysicalDataSource::getSourceName() const
{
  return mDataFilePath;
}



} // namespace nerd
