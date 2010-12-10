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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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


#include "PhysicalData.h"
#include "PhysicalDataSource.h"
#include "Core/Core.h"
#include "Value/StringValue.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"
#include "XMLConfigLoader.h"
#include "Tools.h"
#include "StatisticTools.h"
#include "OrcsModelOptimizerConstants.h"
#include "SimulationConstants.h"
#include "Normalization.h"

#include <QString>
#include <QDir>
#include <QStringList>
#include <QHash>


namespace nerd {

PhysicalData::PhysicalData(const QString &motionConfigPath, Trigger *trigger)
  : mMotionConfigPath(motionConfigPath),
    mTrigger(trigger),
		mTriggerNormalization(0),
    mMotionName(0),
    mScenarioPath(0),
		//mDataSourceType(0),
    mCurMotionLengthInSimSteps(0)
{
}

PhysicalData::~PhysicalData()
{
  this->deleteData();

  if(mPhysicalDataSources.count() > 0) {
    for(int i = 0; i < mPhysicalDataSources.count(); i++){
      delete mPhysicalDataSources.at(i);
    }
    mPhysicalDataSources.clear();
  }
	
	// delete Trigger and TrainDataValue Normalizations
	if(mTriggerNormalization != 0){
		delete mTriggerNormalization;
	}
	
	for(int i = 0; i < mNormalizations.size(); i++ )
	{
		if(mNormalizations.at(i) != 0){
			delete mNormalizations.at(i);
		}
	}
	mNormalizations.clear();
}


QString PhysicalData::getName() const {
  return "PhysicalData";
}

bool PhysicalData::init()
{
  bool ok = true;

  ValueManager * vm = Core::getInstance()->getValueManager();

  ///////////////////////////////////////////////
  // Load general Values
  ///////////////////////////////////////////////

  mMotionName = Tools::getStringValue(vm,
                                      mMotionConfigPath +
                                          OrcsModelOptimizerConstants::VM_NAME_VALUE,
                                      Tools::EM_ERROR,
                                      getName(),
                                      ok);

  mScenarioPath = Tools::getStringValue(vm,
                                        OrcsModelOptimizerConstants::VM_BASE_PATH +
                                            OrcsModelOptimizerConstants::VM_INPUTDATA_PATH +
                                            OrcsModelOptimizerConstants::VM_SCENARIO_VALUE,
                                        Tools::EM_ERROR,
                                        getName(),
                                        ok);
	
	QStringList formatPaths;
	formatPaths += 	mMotionConfigPath;
	formatPaths += 	OrcsModelOptimizerConstants::VM_BASE_PATH +
									OrcsModelOptimizerConstants::VM_INPUTDATA_PATH;
	
	StringValue *formatFile = Tools::getStringValue(	vm,
																				formatPaths,
																				OrcsModelOptimizerConstants::VM_FORMAT_VALUE,
                                        Tools::EM_ERROR,
                                        getName(),
                                        ok);
	
	// the motion name and scenario path are needed to generate filepaths
	// and the format to read the the format config file
  // --> if no values defined: stop initialization
	if(ok == false){
		return ok;
	}
	
	///////////////////////////////////////////////////////////////////
  // Load SourceType and Normalization information from Format
  // configuration file
	///////////////////////////////////////////////////////////////////

	mFormatFilepath = OrcsModelOptimizerConstants::CONFIG_FOLDER 
										+ formatFile->get();
	
	QHash<QString, QString> templates;
	templates.insert(OrcsModelOptimizerConstants::TEMPLATE_MOTION_PATH,
									 mMotionConfigPath);

	XMLConfigLoader *configLoader = new XMLConfigLoader(
											mFormatFilepath,
											OrcsModelOptimizerConstants::CONFIG_FOLDER +
												OrcsModelOptimizerConstants::FILE_PHYSICALDATASOURCE_CONFIG_DESCRIPTION,
											templates);

	if(!configLoader->init()) {
		return false;
	}

	if(!configLoader->loadFromXML()) {
		return false;
	}

	delete configLoader;
	
	/////////////////////////////////////////////////////////
  // Load list of traindata values and their normalization
	/////////////////////////////////////////////////////////
	
	QString trainDataValuePathTemplate =  OrcsModelOptimizerConstants::VM_BASE_PATH +
			OrcsModelOptimizerConstants::VM_TRAINDATA_PATH +
			OrcsModelOptimizerConstants::VM_VALUE_LIST;

	QString standartNormalizationPath = OrcsModelOptimizerConstants::VM_BASE_PATH +
			OrcsModelOptimizerConstants::VM_TRAINDATA_PATH +
			OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
			OrcsModelOptimizerConstants::VM_PHYSICALDATA_PATH;

	QString specialNormalizationPath = trainDataValuePathTemplate +
			OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
			OrcsModelOptimizerConstants::VM_PHYSICALDATA_PATH;

	int i = 1;
	StringValue * trainDataValueNameVal =
			vm->getStringValue(trainDataValuePathTemplate.arg(i) +
			OrcsModelOptimizerConstants::VM_NAME_VALUE);
	while(trainDataValueNameVal != 0)
	{
		QStringList normalizationPathList;

		normalizationPathList += specialNormalizationPath.arg(i);
		normalizationPathList += standartNormalizationPath;

		Normalization *normalization = new Normalization(normalizationPathList);

		ok &= normalization->init();

		mTrainDataValueNames.append(trainDataValueNameVal);
		mNormalizations.append(normalization);


    // try to get next value
		i++;
		trainDataValueNameVal = vm->getStringValue(trainDataValuePathTemplate.arg(i) +
				OrcsModelOptimizerConstants::VM_NAME_VALUE);
	}

		
	///////////////////////////////////////////////
  // Normalization for the trigger value
	///////////////////////////////////////////////

  // there are 3 options where the normalization is defined
	QStringList triggerNormalizationPaths;

  // 1) by the special trigger for the current motion
	triggerNormalizationPaths += mMotionConfigPath +
			OrcsModelOptimizerConstants::VM_TRIGGER_PATH +
			OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
			OrcsModelOptimizerConstants::VM_PHYSICALDATA_PATH;

  // 2) by the standard trigger
	triggerNormalizationPaths += OrcsModelOptimizerConstants::VM_BASE_PATH +
			OrcsModelOptimizerConstants::VM_TRAINDATA_PATH +
			OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
			OrcsModelOptimizerConstants::VM_PHYSICALDATA_PATH;

  // 3) by the standard normalization for simulation data
	triggerNormalizationPaths += standartNormalizationPath;

	mTriggerNormalization = new Normalization(triggerNormalizationPaths);

	ok &= mTriggerNormalization->init();
	
	
		//////////////////////////////////////////////////////////
	// Load data source type which is defined in format file
	//////////////////////////////////////////////////////////
	/*
	mDataSourceType = Tools::getStringValue(vm,
																					mMotionConfigPath +
																						OrcsModelOptimizerConstants::VM_SOURCETYPE_VALUE,
																					Tools::EM_ERROR,
																					getName(),
																					ok);
	*/
  ///////////////////////////////////////////////
  // Generate Filepaths to data files
  ///////////////////////////////////////////////

  // get directory for data files
  QDir motionDir( Tools::toPathString(mScenarioPath->get()) +
                  Tools::toPathString(mMotionName->get()));

  // TODO: angeben das keine Hiddenfiles benutzt werden sollen
  motionDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
  motionDir.refresh();

  if(!motionDir.exists()) {
    Core::log(QString("PhysicalData: ERROR - Can not find motion directory [%1]!").arg(
              motionDir.absolutePath()));
    ok = false;
  }

  // Read list of files which should be used from the folder.
  // If no files are defined --> use all files except the motiondescription and
  // the value parameter file

  QStringList filters;

  QString fileTemplate =  mMotionConfigPath + OrcsModelOptimizerConstants::VM_FILENAME_LIST;
  i = 1;
  StringValue * fileNameVal = vm->getStringValue(fileTemplate.arg(i));
  while(fileNameVal != 0)
  {
    QStringList cur_filter;
    cur_filter.push_back(fileNameVal->get());
    motionDir.setNameFilters(cur_filter);
    if(motionDir.count() <= 0){
      Core::log(QString("PhysicalData: ERROR - Can not find data file [%1]!").arg(
                fileNameVal->get()));
      ok = false;
    }
    else
    {
      filters.push_back(fileNameVal->get());
    }

    // try to get next value
    i++;
    fileNameVal = vm->getStringValue(fileTemplate.arg(i));
  }

  if(filters.count() > 0)
    motionDir.setNameFilters(filters);

  // stop initialization if not all configured files are found
  if(ok == false)
    return false;

  // initialize the PhysicalDataSources with the data files in the motion directory

  QString mdName = OrcsModelOptimizerConstants::FILE_ENDING_MOTION_DESCRIPTION_FILE;
  QString valName =  OrcsModelOptimizerConstants::FILE_ENDING_VALUE_PARAMETER;
  QString infoName = OrcsModelOptimizerConstants::FILE_ENDING_MOTION_CONFIG;

  for(uint i = 0; i < motionDir.count(); i++)
  {
    // check if current file is not the motion description or parameter value or information file
    // or a hidden file
    if(!motionDir[i].endsWith(mdName) &&
       !motionDir[i].endsWith(valName) &&
       !motionDir[i].endsWith(infoName) &&
       !motionDir[i].endsWith("~"))
    {
			QString dataSourceFile = Tools::toPathString(motionDir.absolutePath()) + motionDir[i];
			
      PhysicalDataSource *source = 
					PhysicalDataSource::createPhysicalDataSource(	/*mDataSourceType->get(),*/
          																							mTrigger,
																 												mTriggerNormalization,
														 														mTrainDataValueNames,
														 														mNormalizations,
														 														mFormatFilepath,
														 														dataSourceFile);

      if(source == 0){
        ok = false;
      }
      else
      {
        mPhysicalDataSources.push_back(source);
        ok &= source->init();
      }
    }
  }

  if(mPhysicalDataSources.count() <= 0){
    Core::log(QString("PhysicalData: ERROR - Could not find data files in the motion directory [%1] !").arg(mMotionName->get()));
    ok = false;
  }

  return ok;
}

int PhysicalData::getMotionLength() const
{
  return mCurMotionLengthInSimSteps;
}

double PhysicalData::getMeanData(int channel, int simulationStep)
{
  return mMeanData.at(channel)->at(simulationStep);
}

double PhysicalData::getStandartDeviation(int channel, int simulationStep)
{
  return mStandartDeviation.at(channel)->at(simulationStep);
}

bool PhysicalData::deleteData()
{
  // delete old data if they exist
  if(mMeanData.count() > 0) {
    for(int i = 0; i < mMeanData.count(); i++){
      delete mMeanData.at(i);
    }
    mMeanData.clear();
  }

  if(mStandartDeviation.count() > 0) {
    for(int i = 0; i < mStandartDeviation.count(); i++){
      delete mStandartDeviation.at(i);
    }
    mStandartDeviation.clear();
  }

  return true;
}

bool PhysicalData::readData(int lengthInSimSteps, bool alreadyReadData)
{
  bool ok = true;

  // delete old data if they exist
  if(!this->deleteData()){
    return false;
  }

  if(alreadyReadData == false)
  {
    // read data from datasources
    for(int i = 0; i < mPhysicalDataSources.count(); i++)
    {
      ok &= mPhysicalDataSources.at(i)->readData(lengthInSimSteps);
    }
  }

  if(ok == false){
    return false;
  }

  mCurMotionLengthInSimSteps = lengthInSimSteps;

  // buffer to hold the data of all data sources for one timepoint for mean
  // and standard deviation calculation
  QVector<double> timepointData(mPhysicalDataSources.count());
  double mean = 0.0;
  double standardDeviation = 0.0;

  // iterate over channels and fill the data for each channel
  for(int channel = 0; channel < mTrainDataValueNames.count(); channel++)
  {
    // initialize the vectors for the current channel
    mMeanData.push_back( new QVector<double>(this->getMotionLength()) );
    mStandartDeviation.push_back( new QVector<double>(this->getMotionLength()) );

    // iterate over the timepoints of the motion and compute the meandata for
    // each timepoint by collecting all data from the data sources
    for(int timepoint = 0; timepoint < this->getMotionLength(); timepoint++)
    {
      // get data from all datasources from current timepoint
      for(int datasource = 0; datasource < mPhysicalDataSources.count(); datasource++)
      {
        timepointData[datasource] =
           mPhysicalDataSources.at(datasource)->getData(channel, timepoint);
      }

      // calculate mean and standard deviation

      mean = StatisticTools::calculateMean(timepointData);
      standardDeviation = StatisticTools::calculateStandardDeviation(timepointData,mean);

      mMeanData.at(channel)->replace(timepoint,mean);
      mStandartDeviation.at(channel)->replace(timepoint,standardDeviation);
    }
  }

  return ok;
}

int PhysicalData::readData()
{
  bool ok = true;

  int shortesMotionLengthInSteps = mPhysicalDataSources.at(0)->readData();

  // read data from datasources and detect shortest motion length
  for(int i = 1; i < mPhysicalDataSources.count(); i++)
  {
    int curMotionLengthInSteps = mPhysicalDataSources.at(i)->readData();

    if(shortesMotionLengthInSteps > curMotionLengthInSteps){
      shortesMotionLengthInSteps = curMotionLengthInSteps;
    }
  }

  if(shortesMotionLengthInSteps < 0) {
    ok = false;
  }
  else
  {
    // save data to mMeanData and mStandartDeviation
    ok &= readData( shortesMotionLengthInSteps , true);
  }

  if(ok == false){
    shortesMotionLengthInSteps = -1;
  }

  return shortesMotionLengthInSteps;
}

/**
 * Number of single physical datasources which are used to compute
 * the mean data and the standarddeviation.
 *
 * @return Number of PhysicalDataSources.
 */
int PhysicalData::getPhysicalDataSourceCount()
{
  return mPhysicalDataSources.size();
}

/**
 * Returns the reference to the PhysicalDataSources with the given index.
 *
 * @param source Indexof the PhysicalDataSource.
 * @return Reference to the PhysicalDataSource.
 */
PhysicalDataSource* PhysicalData::getPhysicalDataSource(int source)
{
  return mPhysicalDataSources.at(source);
}


} // namespace nerd
