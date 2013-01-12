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

#include "CalcErrorFunction.h"
#include "OrcsModelOptimizerConstants.h"

#include "Core/Core.h"
#include "Optimization/TrainSet.h"
#include "Optimization/MotionData.h"
#include "Optimization/SimulationData.h"
#include "Optimization/PhysicalData.h"
#include "Optimization/PhysicalDataSource.h"
#include "Optimization/Tools.h"
#include "Optimization/StatisticTools.h"
#include "Optimization/ReportLogger.h"
#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include "Value/BoolValue.h"
#include "Value/IntValue.h"
#include "QuadraticErrorFunction.h"

#include <QDir>


namespace nerd 
{

CalcErrorFunction::CalcErrorFunction(const QString &model, TrainSet *trainSet) 
 : 	ModelOptimizer::ModelOptimizer(model, trainSet),
	 	mValueFileDir(0),
		mSaveMotionCourse(0),
		mValueFileIndex(0),
		mErrorFunction(0)																					 
{ }
   
   
QString CalcErrorFunction::getName() const{
  return "CalcErrorFunction";
}


bool CalcErrorFunction::init() 
{
  bool ok = ModelOptimizer::init();
  
  ValueManager *vm = Core::getInstance()->getValueManager();
  
  QStringList configPaths;
  configPaths += mModelConfigPath;
  configPaths += OrcsModelOptimizerConstants::VM_BASE_PATH 
                 + OrcsModelOptimizerConstants::VM_OPTIMIZER_PATH;
  
  // load configuration values
	mValueFileDir = Tools::getStringValue(vm, 
                                configPaths,
																OrcsModelOptimizerConstants::VM_VALUEFILEDIRECTORY_VALUE,
                                Tools::EM_ERROR,
                                getName(),
                                ok);
	
	mSaveMotionCourse = Tools::getOrAddBoolValue(vm, 
                                configPaths,
																OrcsModelOptimizerConstants::VM_SAVEMOTIONCOURSE_VALUE,
                                false,
                                getName(),
                                ok,
																true);
	
	
	// generate list with value file names
	if(ok == true)
	{	
		QDir valFileDir( mValueFileDir->get() );
		
		QString valueFileStr = "*" +
				OrcsModelOptimizerConstants::FILE_ENDING_VALUE_PARAMETER;
	
		// set namefilter to search value parameter file
		valFileDir.setNameFilters(QStringList(valueFileStr));
		valFileDir.refresh();
	
		if( valFileDir.count() >= 1 )
		{
			mValueFiles.resize(valFileDir.count());
	
			for(uint i = 0; i < valFileDir.count(); i++)
			{
				mValueFiles[i] = valFileDir.filePath(valFileDir[i]);
			}
		}
	}
	
	
	
		// create errorfunction
	mErrorFunction = new QuadraticErrorFunction(mTrainSet,
																							mToOptimizeParameters);
	
	
  return ok;
}


bool CalcErrorFunction::optimizationStart()
{
  bool ok = ModelOptimizer::optimizationStart();
	
		// create name of result file
	
	mResultFileName = getOptimizationRunOutputDirectory()
			+ QDir::separator()
			+ getFileNameBegin()
			+ "_Results.log";	
	
	// write start entry for result file
	QFile file(mResultFileName);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
		Core::log(QString("%1: ERROR - Could not create the file [%2]!").arg(
							getName(), mResultFileName));
		file.close();
		return false;
	}

	QTextStream out(&file);

	out << QString("# ValueFile\n");
	out << QString("# Error\n");
	out << "\n";
	
	file.close();
  
  return ok;
}

bool CalcErrorFunction::doNextOptimizationStep()
{
	bool finished = false;
	
	// load next valuefile for which the error should be calculated
	ValueManager * vm = Core::getInstance()->getValueManager();
	if(!vm->loadValues(mValueFiles.at(mValueFileIndex), false))
	{
		finished = true;
	}	
	
	QString currentFileName = mValueFiles.at(mValueFileIndex);
	currentFileName = mValueFiles.at(mValueFileIndex).split(QDir::separator()).last();

	
	double error = mErrorFunction->calculate(getParametersAsDoubleVector());
	
	
	// write result in file
	QFile file(mResultFileName);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append)){
		Core::log(QString("%1: ERROR - Could not create the file [%2]!").arg(
							getName(), mResultFileName));
		file.close();
		return false;
	}
	QTextStream out(&file);
	out << QString("%1;%2\n").arg(currentFileName,QString::number(error, 'g', 15));
	file.close();
	
	
	//////////////////////////////////////
	// create traindatastatistic if wanted
	//////////////////////////////////////

	if(mSaveMotionCourse->get() == true)
	{
		if(!saveTraindataStatistic(currentFileName))
		{
			finished = true;
		}
	}
	
	mValueFileIndex++;
	
	if(mValueFileIndex >= mValueFiles.count()){
		finished = true;
	}
	
  return finished; 
}

bool CalcErrorFunction::optimizationEnd()
{
  bool ok = ModelOptimizer::optimizationEnd(); 
  
  return ok;
}

bool CalcErrorFunction::writeReportStartEntry()
{
  return true;
}
    
bool CalcErrorFunction::writeReportEndEntry()
{
  return true;
}


QVector<double> CalcErrorFunction::getParametersAsDoubleVector()
{
	QVector<double> doubleVec (mToOptimizeParameters.size(), 0.0);
  
	for(int i = 0; i < mToOptimizeParameters.size(); i++)
	{
		doubleVec[i] = ((DoubleValue*)mToOptimizeParameters.at(i)->value)->get();
	}
  
	return doubleVec;
} 


} // namespace nerd

