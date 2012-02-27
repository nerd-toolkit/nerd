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

#include "PowellModelOptimizer.h"
#include "OrcsModelOptimizerConstants.h"

#include "Core/Core.h"
#include "Optimization/TrainSet.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "QuadraticErrorFunction.h"
#include "ReducedChiSquareErrorFunction.h"
#include "Math/PowellMultiDimMinimizer.h"
#include "Math/BrentOneDimMinimizer.h"
#include "Math/ParabolicBracketFinder.h"
#include "ErrorFunction.h"
#include "Physics/Physics.h"
#include "Optimization/Tools.h"
#include "Optimization/ReportLogger.h"

#include <QStringList>
#include <QDir>


namespace nerd 
{

PowellModelOptimizer::PowellModelOptimizer(const QString &model, TrainSet *trainSet) 
 : 	ModelOptimizer::ModelOptimizer(model, trainSet),
   	mErrorFunction(0), 
   	mBracketFinder(0), 
   	mOneDimMinimizer(0),
   	mMultiDimMinimizer(0),
   	mMultiDimTolerance(0),
   	mOneDimMaxIterations(0),
   	mOneDimTolerance(0),
		mModelError(0)
{
	mModelError = new DoubleValue(0.0); 
}
   
PowellModelOptimizer::~PowellModelOptimizer()
{
	if(mModelError != 0){
		delete mModelError;
		mModelError = 0;
	}
	
  if(mErrorFunction != 0){
    delete mErrorFunction;
  }
  
  if(mBracketFinder != 0){
    delete mBracketFinder;
  }
  
  if(mOneDimMinimizer != 0){
    delete mOneDimMinimizer;
  }
  
  if(mMultiDimMinimizer != 0){
    delete mMultiDimMinimizer;
  }
}
   
   
QString PowellModelOptimizer::getName() const{
  return "PowellModelOptimizer";
}

/**
 * Initializes the mBracketFinder, mOneDimMinimizer and mMultiDimMinimizer.
 * Calls the ModelOptimizer::init() method.
 * 
 * @return TRUE if successfully, otherwise FALSE.
 */
bool PowellModelOptimizer::init() 
{
  bool ok = ModelOptimizer::init();
  
  ValueManager *vm = Core::getInstance()->getValueManager();
  
  QStringList configPaths;
  configPaths += mModelConfigPath;
  configPaths += OrcsModelOptimizerConstants::VM_BASE_PATH 
                 + OrcsModelOptimizerConstants::VM_OPTIMIZER_PATH;
  
  // load configuration values
  mMultiDimTolerance = Tools::getOrAddDoubleValue(vm, 
                                configPaths,
                                OrcsModelOptimizerConstants::VM_MULTIDIMTOLERANCE_VALUE,
                                false,
                                getName(),
                                ok,
                                3.0E-5);

  mOneDimTolerance = Tools::getOrAddDoubleValue(vm, 
                                configPaths,
                                OrcsModelOptimizerConstants::VM_ONEDIMTOLERANCE_VALUE,
                                false,
                                getName(),
                                ok,
                                1.0E-5);

  mOneDimMaxIterations = Tools::getOrAddIntValue(vm, 
                                configPaths,
                                OrcsModelOptimizerConstants::VM_ONEDIMMAXITERATIONS_VALUE,
                                false,
                                getName(),
                                ok,
                                100);
  
                         
  // initialize optimization routines
  if(ok == true)
  {
    mErrorFunction = new QuadraticErrorFunction(mTrainSet,
                                                mToOptimizeParameters);
    
    mErrorFunction->setSimulationRunsCounter(mSimulationRuns);
    
    mBracketFinder = new ParabolicBracketFinder();
    
    mOneDimMinimizer = new BrentOneDimMinimizer(mBracketFinder);
    mOneDimMinimizer->setMaxIterations(mOneDimMaxIterations->get());
    mOneDimMinimizer->setTolerance(mOneDimTolerance->get());
    
    mMultiDimMinimizer = new PowellMultiDimMinimizer(mOneDimMinimizer);
    mMultiDimMinimizer->setTolerance(mMultiDimTolerance->get());
  }
	
	
	// initialize statistics
	ok &= appendStatistic(mModelError,"ModelError");
	ok &= appendStatistic(mToOptimizeParameters);
  
  return ok;
}

bool PowellModelOptimizer::writeReportStartEntry()
{
  bool ok = ModelOptimizer::writeReportStartEntry();
  
  QString message;
  
  // continue with configuration
  message += " -  LineMinimizer MaxIterations: " + 
      mOneDimMaxIterations->getValueAsString() + "\n";
  
  message += " -  LineMinimizer Tolerance:     " + 
      mOneDimTolerance->getValueAsString() + "\n";
  
  message += " -  MultiDimMinimizer Tolerance: " + 
      mMultiDimTolerance->getValueAsString() + "\n";
  
  ok &= mReportLogger->addMessage(message);
	
  return ok;
}

bool PowellModelOptimizer::writeReportEndEntry()
{ 
	bool ok = ModelOptimizer::writeReportEndEntry();
	
	QString message;
	message += "  - ModelError: " + mModelError->getValueAsString() + "\n";
	
	ok &= mReportLogger->addMessage(message);
	
	return ok;
}

bool PowellModelOptimizer::optimizationStart()
{
  bool ok = ModelOptimizer::optimizationStart();
  
  ok &= mMultiDimMinimizer->minimizationStart(mErrorFunction,
                                              getParametersAsDoubleVector()); 
  
  Physics::getSimulationEnvironmentManager()->resetStartConditions();
  
  // save model error of start values
  mModelError->set( mMultiDimMinimizer->getMinimumPoint().y );
  
  return ok;
}

bool PowellModelOptimizer::doNextOptimizationStep()
{
  bool finished = false;
  
  finished = mMultiDimMinimizer->doMinimizationStep();
  
	if(finished == true){
		mReportLogger->addMessage("\nStopped optimization: Found minimum.");
	}
	
  mModelError->set( mMultiDimMinimizer->getMinimumPoint().y );
 
	QString filename = getOptimizationRunOutputDirectory() 
			+ QDir::separator() 
			+ getFileNameBegin() 
			+ "_Parameter.val";
	
	
	setParametersFromDoubleVector(mMultiDimMinimizer->getMinimumPoint().x);
	bool ok = saveToOptimizeParameters(filename);
	
	if(ok == false){
		finished = true;
		mReportLogger->addMessage("\nStopped optimization: Could not save parameter file!");
	}
  
  return finished; 
}

bool PowellModelOptimizer::optimizationEnd()
{
  bool ok = ModelOptimizer::optimizationEnd(); 
  
  ok &= mMultiDimMinimizer->minimizationEnd();
  
  return ok;
}

QVector<double> PowellModelOptimizer::getParametersAsDoubleVector()
{
  QVector<double> doubleVec (mToOptimizeParameters.size(), 0.0);
  
  for(int i = 0; i < mToOptimizeParameters.size(); i++)
  {
    doubleVec[i] = ((DoubleValue*)mToOptimizeParameters.at(i)->value)->get();
  }
  
  return doubleVec;
}    
                                          
void PowellModelOptimizer::setParametersFromDoubleVector(QVector<double> parameters)
{
	// set model parameter to given parameters
	for(int i = 0; i < parameters.size(); i++){
		((DoubleValue*)mToOptimizeParameters.at(i)->value)->set(parameters.at(i));
	}
}

} // namespace nerd

