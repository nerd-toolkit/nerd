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

#include "MuSlashRhoLambdaESOptimizer.h"
#include "OrcsModelOptimizerConstants.h"

#include "Core/Core.h"
#include "Optimization/TrainSet.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"

#include "ErrorFunction.h"
#include "QuadraticErrorFunction.h"
#include "ESSingleClientFitnessEvaluation.h"
#include "Physics/Physics.h"
#include "Optimization/Tools.h"
#include "Optimization/ReportLogger.h"

#include "Evolution/ESCommaSelection.h"
#include "Evolution/ESDiscreteRecombination.h"
#include "Evolution/ESFitnessEvaluation.h"
#include "Evolution/ESInitialize.h"
#include "Evolution/ESIntermediateRecombination.h"
#include "Evolution/ESMarriage.h"
#include "Evolution/ESMultiDensityFuncObjParaMutation.h"
#include "Evolution/ESMultiDensityFuncStratParaMutation.h"
#include "Evolution/ESObjectParametersMutation.h"
#include "Evolution/ESOneFifthRuleStratParaMutation.h"
#include "Evolution/ESPlusSelection.h"
#include "Evolution/ESRandomInitialize.h"
#include "Evolution/ESRecombination.h"
#include "Evolution/ESSelection.h"
#include "Evolution/ESSingleDensityFuncObjParaMutation.h"
#include "Evolution/ESSingleDensityFuncStratParaMutation.h"
#include "Evolution/ESStrategyParametersMutation.h"
#include "Evolution/MuSlashRhoLambdaES.h"
#include "Evolution/ESInitializeValue.h"
#include "Evolution/ESInformation.h"
#include "Evolution/ESIndividual.h"
#include "Value/OptimizationDouble.h"

#include <QStringList>
#include <QDir>
#include <QFile>
#include <QtAlgorithms>
#include <QPair>


namespace nerd 
{

MuSlashRhoLambdaESOptimizer::MuSlashRhoLambdaESOptimizer(const QString &model, TrainSet *trainSet) 
 : 	ModelOptimizer::ModelOptimizer(model, trainSet),
	 	mMu(0),
	 	mRho(0),
		mLambda(0),
		mSelectionType(0),
		mStrategyParaRecombinationType(0),
		mObjectParaRecombinationType(0),
		mParameterMutationsType(0),
		mLearnRateKoeff(0),
		mCheckInterval(0),
		mInitStrategyParameter(0),
		mNumberOfResultFiles(0),			
		mESMarriage(0),
		mESObjectParametersRecombination(0),
		mESStrategyParametersRecombination(0),
		mESObjectParametersMutation(0),
		mESStrategyParametersMutation(0),
		mESSelection(0),
		mESFitnessEvaluation(0),
		mESInitialize(0),
		mMuSlashRhoLambdaES(0),
		mErrorFunction(0),
		mSmallestError(0),
		mHighestError(0),
		mMeanError(0),
		mErrorSTD(0)
{
	mSmallestError = new DoubleValue(0.0);
	mHighestError = new DoubleValue(0.0);
	mMeanError = new DoubleValue(0.0);
	mErrorSTD = new DoubleValue(0.0); 
}
   
MuSlashRhoLambdaESOptimizer::~MuSlashRhoLambdaESOptimizer()
{
	if(mSmallestError != 0) {
		delete mSmallestError;
	}
	
	if(mHighestError != 0) {
		delete mHighestError;
	}
	
	if(mMeanError != 0) {
		delete mMeanError;
	}
	
	if(mErrorSTD != 0) {
		delete mErrorSTD;
	}
	
	if(mESMarriage != 0) {
		delete mESMarriage;
	}
		
	if(mESObjectParametersRecombination != 0) {
		delete mESObjectParametersRecombination;
	}
	
	if(mESStrategyParametersRecombination != 0) {
		delete mESStrategyParametersRecombination;
	}
	
	if(mESObjectParametersMutation != 0) {
		delete mESObjectParametersMutation;
	}
	
	if(mESStrategyParametersMutation != 0) {
		delete mESStrategyParametersMutation;
	}
	
	if(mESSelection != 0) {
		delete mESSelection;
	}
	
	if(mESFitnessEvaluation != 0) {
		delete mESFitnessEvaluation;
	}
	
	if(mESInitialize != 0) {
		delete mESInitialize;
	}
	
	if(mMuSlashRhoLambdaES != 0) {
		delete mMuSlashRhoLambdaES;
	}
	
	if(mErrorFunction != 0) {
		delete mErrorFunction;
	}	
}
   
   
QString MuSlashRhoLambdaESOptimizer::getName() const{
  return "MuSlashRhoLambdaESOptimizer";
}

bool MuSlashRhoLambdaESOptimizer::init() 
{
  bool ok = ModelOptimizer::init();
  
  ValueManager *vm = Core::getInstance()->getValueManager();
  
  QStringList configPaths;
  configPaths += mModelConfigPath;
  configPaths += OrcsModelOptimizerConstants::VM_BASE_PATH 
                 + OrcsModelOptimizerConstants::VM_OPTIMIZER_PATH;
  
  // load configuration values	
	mMu = Tools::getIntValue(	vm, 
														configPaths,
														OrcsModelOptimizerConstants::VM_MU_VALUE,
														Tools::EM_ERROR,
														getName(),
														ok);

	mRho = Tools::getIntValue(vm, 
														configPaths,
														OrcsModelOptimizerConstants::VM_RHO_VALUE,
														Tools::EM_ERROR,
														getName(),
														ok);
                         
	mLambda = Tools::getIntValue(	vm, 
																configPaths,
																OrcsModelOptimizerConstants::VM_LAMBDA_VALUE,
																Tools::EM_ERROR,
																getName(),
																ok);
	
	mSelectionType = Tools::getStringValue(vm, 
																			configPaths,
																			OrcsModelOptimizerConstants::VM_SELECTION_VALUE,
																			Tools::EM_ERROR,
																			getName(),
																			ok);
	
	mStrategyParaRecombinationType = Tools::getStringValue(vm, 
																			configPaths,
																			OrcsModelOptimizerConstants::VM_STRATPARARECOMB_VALUE,
																			Tools::EM_ERROR,
																			getName(),
																			ok);
	
	mObjectParaRecombinationType = Tools::getStringValue(vm, 
																			configPaths,
																			OrcsModelOptimizerConstants::VM_OBJPARARECOMB_VALUE,
																			Tools::EM_ERROR,
																			getName(),
																			ok);
	
	mParameterMutationsType = Tools::getStringValue(vm, 
																			configPaths,
									 										OrcsModelOptimizerConstants::VM_PARAMUTATION_VALUE,
																			Tools::EM_ERROR,
																			getName(),
																			ok);
	
	// parameters for strategy recombination
	mLearnRateKoeff = Tools::getDoubleValue(vm, 
																			configPaths,
																			OrcsModelOptimizerConstants::VM_LEARNRATEKOEFF_VALUE,
																			Tools::EM_NOERROR,
																			getName(),
																			ok);
	
	mCheckInterval = Tools::getIntValue(vm, 
																			configPaths,
																			OrcsModelOptimizerConstants::VM_CHECKINTERVAL_VALUE,
																			Tools::EM_NOERROR,
																			getName(),
																			ok);
	////
		
	mInitStrategyParameter = Tools::getOrAddDoubleValue(vm, 
																			configPaths,
									 										OrcsModelOptimizerConstants::VM_INITSTRATPARA_VALUE,
																			false,
																			getName(),
																			ok,
									 										1.0);
	
	mNumberOfResultFiles = Tools::getOrAddIntValue(vm, 
																			configPaths,
									 										OrcsModelOptimizerConstants::VM_NUMBEROFRESULTSFILES_VALUE,
																			false,
																			getName(),
																			ok,
									 										1);
	
	
  // initialize optimization routines
  if(ok == true)
  {
		ok &= initMuSlashRhoLambdaESComponents();	
  }
	
	// initialize statistics
	ok &= appendStatistic(mSmallestError, "SmallestError");
	ok &= appendStatistic(mHighestError, "HighestError");
	ok &= appendStatistic(mMeanError, "MeanError");
	ok &= appendStatistic(mErrorSTD, "ErrorSTD");
  
  return ok;
}

bool MuSlashRhoLambdaESOptimizer::initMuSlashRhoLambdaESComponents()
{
	bool ok = true;
	
	if(mMuSlashRhoLambdaES != 0) {
		delete mMuSlashRhoLambdaES;
		mMuSlashRhoLambdaES = 0;
	}
	
	// initialize components from config
	ok &= initESStrategyParaRecombinationFromConfig();
	ok &= initESObjectParaRecombinationFromConfig();
	ok &= initESParameterMutationsFromConfig();
	ok &= initESSelectionFromConfig();
	ok &= initESInitializeFromConfig();
	
	if(ok == true)
	{
		if(mESMarriage != 0) {
			delete mESMarriage;
			mESMarriage = 0;
		}
		
		if(mErrorFunction != 0) {
			delete mErrorFunction;
			mErrorFunction = 0;
		}	
		
		if(mESFitnessEvaluation != 0) {
			delete mESFitnessEvaluation;
			mESFitnessEvaluation = 0;
		}	
		
		mESMarriage = new ESMarriage();
		
		mErrorFunction = new QuadraticErrorFunction(mTrainSet, mToOptimizeParameters);
		mErrorFunction->setSimulationRunsCounter(mSimulationRuns);
		mESFitnessEvaluation = new ESSingleClientFitnessEvaluation(mErrorFunction);
		
		mMuSlashRhoLambdaES = new MuSlashRhoLambdaES(	mESMarriage,
																								 	mESStrategyParametersRecombination,
																								 	mESObjectParametersRecombination,
																								 	mESStrategyParametersMutation,
																								 	mESObjectParametersMutation,
																								 	mESSelection,
																									mESFitnessEvaluation );
	}
	
	return ok;
}



bool MuSlashRhoLambdaESOptimizer::initESSelectionFromConfig()
{
	if(mSelectionType == 0){
		Core::log(QString("%1: ERROR - Value [mSelectionType] is not initialized!").arg(getName()));
		return false;
	}
	
	if(mESSelection != 0){
		delete mESSelection;
		mESSelection = 0;
	}
	
	QString typeStr = mSelectionType->get().toLower().trimmed();
	
	if(typeStr == "plus") {
		mESSelection = new ESPlusSelection();
		return true;
	}
	
	if(typeStr == "comma") {
		mESSelection = new ESCommaSelection();
		return true;
	}
	
	Core::log(QString("%1: ERROR - The type \"%2\" for [%3] is unknown! The following types are possible: \"Plus\", \"Comma\".").arg(	
						getName(),
						mSelectionType->get(),
						OrcsModelOptimizerConstants::VM_SELECTION_VALUE));
	return false;
}

bool MuSlashRhoLambdaESOptimizer::initESStrategyParaRecombinationFromConfig()
{
	if(mStrategyParaRecombinationType == 0){
		Core::log(QString("%1: ERROR - Value [mStrategyParaRecombinationType] is not initialized!").arg(getName()));
		return false;
	}
	
	if(mESStrategyParametersRecombination != 0){
		delete mESStrategyParametersRecombination;
		mESStrategyParametersRecombination = 0;
	}
	
	QString typeStr = mStrategyParaRecombinationType->get().toLower().trimmed();
	
	if(typeStr == "discrete") {
		mESStrategyParametersRecombination = new ESDiscreteRecombination();
		return true;
	}
	
	if(typeStr == "intermediate") {
		mESStrategyParametersRecombination = new ESIntermediateRecombination();
		return true;
	}
	
	Core::log(QString("%1: ERROR - The type \"%2\" for [%3] is unknown! The following types are possible: \"Discrete\", \"Intermediate\".").arg(	
						getName(),
						mStrategyParaRecombinationType->get(),
						OrcsModelOptimizerConstants::VM_STRATPARARECOMB_VALUE));
	return false;
}	
	
bool MuSlashRhoLambdaESOptimizer::initESObjectParaRecombinationFromConfig()
{
	if(mObjectParaRecombinationType == 0){
		Core::log(QString("%1: ERROR - Value [mObjectParaRecombinationType] is not initialized!").arg(getName()));
		return false;
	}
	
	if(mESObjectParametersRecombination != 0){
		delete mESObjectParametersRecombination;
		mESObjectParametersRecombination = 0;
	}
	
	QString typeStr = mObjectParaRecombinationType->get().toLower().trimmed();
	
	if(typeStr == "discrete") {
		mESObjectParametersRecombination = new ESDiscreteRecombination();
		return true;
	}
	
	if(typeStr == "intermediate") {
		mESObjectParametersRecombination = new ESIntermediateRecombination();
		return true;
	}
	
	Core::log(QString("%1: ERROR - The type \"%2\" for [%3] is unknown! The following types are possible: \"Discrete\", \"Intermediate\".").arg(	
						getName(),
						mObjectParaRecombinationType->get(),
						OrcsModelOptimizerConstants::VM_OBJPARARECOMB_VALUE));
	return false;
}
		
bool MuSlashRhoLambdaESOptimizer::initESParameterMutationsFromConfig()
{
	if(mParameterMutationsType == 0){
		Core::log(QString("%1: ERROR - Value [mParameterMutationsType] is not initialized!").arg(getName()));
		return false;
	}
	
	if(mESStrategyParametersMutation != 0){
		delete mESStrategyParametersMutation;
		mESStrategyParametersMutation = 0;
	}
	
	if(mESObjectParametersMutation != 0){
		delete mESObjectParametersMutation;
		mESObjectParametersMutation = 0;
	}
	
	if(mLearnRateKoeff == 0){
		Core::log(QString("%1: ERROR - Attribute [%2] is not defined!").arg(
							getName(),
							OrcsModelOptimizerConstants::VM_LEARNRATEKOEFF_VALUE));
		return false;
	}
	
	QString typeStr = mParameterMutationsType->get().toLower().trimmed();
	
	if(typeStr == "singlestd") {
				
		mESStrategyParametersMutation = 
				new ESSingleDensityFuncStratParaMutation(mLearnRateKoeff->get());
		
		mESObjectParametersMutation = new ESSingleDensityFuncObjParaMutation();
		return true;
	}
	
	if(typeStr == "multistd") {
		mESStrategyParametersMutation = 
				new ESMultiDensityFuncStratParaMutation(mLearnRateKoeff->get());
		
		mESObjectParametersMutation = new ESMultiDensityFuncObjParaMutation();

		return true;
	}
	
	if(typeStr == "1/5rule") 
	{
		if(mCheckInterval == 0){
			Core::log(QString("%1: ERROR - Attribute [%2] is not defined!").arg(
								getName(),
								OrcsModelOptimizerConstants::VM_CHECKINTERVAL_VALUE));
			return false;
		}
		
		mESStrategyParametersMutation = 
				new ESOneFifthRuleStratParaMutation(mCheckInterval->get(),
																						mLearnRateKoeff->get());
		
		mESObjectParametersMutation = new ESSingleDensityFuncObjParaMutation();

		return true;
	}
	
	Core::log(QString("%1: ERROR - The type \"%2\" for [%3] is unknown! The following types are possible: \"SingleSTD\", \"MultiSTD\", \"1/5Rule\".").arg(	
						getName(),
						mParameterMutationsType->get(),
						OrcsModelOptimizerConstants::VM_PARAMUTATION_VALUE));
	return false;
}

bool MuSlashRhoLambdaESOptimizer::initESInitializeFromConfig()
{
	bool ok = true;
	
	if(mESInitialize != 0){
		delete mESInitialize;
		mESInitialize = 0;
	}
	
	//////////////////////////////////////////
	// ObjectParameters
	//////////////////////////////////////////
	
	QList<ESInitializeValue>	objectParaRanges;
		
	for(int i = 0; i < mToOptimizeParameters.size(); i++ )
	{
		// check if a random value should be used or a certain value
		if(mToOptimizeParameters.at(i)->init->get() == "random" )
		{
			ESInitializeValue range(mToOptimizeParameters.at(i)->min->get(), 
											 				mToOptimizeParameters.at(i)->max->get());
											 
			objectParaRanges.append( range );
		}
		else
		{
			ESInitializeValue range(((DoubleValue*)mToOptimizeParameters.at(i)->value)->get(),
																mToOptimizeParameters.at(i)->min->get(),
																mToOptimizeParameters.at(i)->max->get());
											 
			objectParaRanges.append( range );			
		}
	}
	
	//////////////////////////////////////////
	// StrategyParameters
	//////////////////////////////////////////
	QList<ESInitializeValue>	strategyParaRanges;
	
	if(mParameterMutationsType == 0){
		Core::log(QString("%1: ERROR - Value [mParameterMutationsType] is not initialized!").arg(getName()));
		return false;
	}
	
	if(mInitStrategyParameter == 0){
		Core::log(QString("%1: ERROR - Value [mInitStrategyParameter] is not initialized!").arg(getName()));
		return false;
	}
	
	QString typeStr = mParameterMutationsType->get().toLower().trimmed();
	
	// check if one strategyparameter or for every objectparameter one is needed
	if(typeStr == "singlestd" || typeStr == "1/5rule") 
	{
		ESInitializeValue range(mInitStrategyParameter->get());
		strategyParaRanges.append(range);
	} 
	else if(typeStr == "multistd")
	{
		for(int i = 0; i < mToOptimizeParameters.size(); i++)
		{
			ESInitializeValue range(mInitStrategyParameter->get());
			strategyParaRanges.append(range);			
		}
	}
	else
	{
		Core::log(QString("%1: ERROR - The type \"%2\" for [%3] is unknown! The following types are possible: \"SingleSTD\", \"MultiSTD\", \"1/5Rule\".").arg(	
					getName(),
					mParameterMutationsType->get(),
					OrcsModelOptimizerConstants::VM_PARAMUTATION_VALUE));
		return false;
	}
	
	///////////////////////////////////
	
	mESInitialize = new ESRandomInitialize(objectParaRanges, strategyParaRanges); 
	
	return ok;
}



bool MuSlashRhoLambdaESOptimizer::writeReportStartEntry()
{
  bool ok = ModelOptimizer::writeReportStartEntry();
  
  QString message;
  
  // continue with configuration
	message += " -  Mu: " + 
			mMu->getValueAsString() + "\n";
  
	message += " -  Rho: " + 
			mRho->getValueAsString() + "\n";
	
	message += " -  Lambda: " + 
			mLambda->getValueAsString() + "\n";

	message += " -  StrategyParameterRecombination: " + 
			mStrategyParaRecombinationType->getValueAsString() + "\n";

	message += " -  ObjectParameterRecombination: " + 
			mObjectParaRecombinationType->getValueAsString() + "\n";
  
	message += " -  ParameterMutation: " + 
			mParameterMutationsType->getValueAsString() + "\n";

	if(mLearnRateKoeff != 0)
	{
		message += "    -  LearnRateCoefficient: " + 
				mLearnRateKoeff->getValueAsString() + "\n";
	}

	if(mCheckInterval != 0)
	{
		message += "    -  CheckInterval: " + 
				mCheckInterval->getValueAsString() + "\n";
	}
	
	message += " -  Selection: " + 
			mSelectionType->getValueAsString() + "\n";
	
	message += " -  InitialStrategyParameter: " + 
			mInitStrategyParameter->getValueAsString() + "\n";
	
  ok &= mReportLogger->addMessage(message);
  
  return ok;
}

bool MuSlashRhoLambdaESOptimizer::optimizationStart()
{
  bool ok = ModelOptimizer::optimizationStart();
	
	ok &= mMuSlashRhoLambdaES->startEvolutionRun(	mMu->get(), 
																								mRho->get(), 
																								mLambda->get(), 
																								mESInitialize);
	
	updateFitnessValues();
	saveBestIndividuals();
  
  return ok;
}

bool MuSlashRhoLambdaESOptimizer::doNextOptimizationStep()
{
  bool finished = false;
	
	mMuSlashRhoLambdaES->doNextGeneration();
	
	updateFitnessValues();
	saveBestIndividuals();
   
  return finished; 
}

bool MuSlashRhoLambdaESOptimizer::optimizationEnd()
{
  bool ok = ModelOptimizer::optimizationEnd(); 
  
  return ok;
}

QVector<double> MuSlashRhoLambdaESOptimizer::getParametersAsDoubleVector()
{
  QVector<double> doubleVec (mToOptimizeParameters.size(), 0.0);
  
  for(int i = 0; i < mToOptimizeParameters.size(); i++)
  {
    doubleVec[i] = ((DoubleValue*)mToOptimizeParameters.at(i)->value)->get();
  }
  
  return doubleVec;
}                                              
                                     
void MuSlashRhoLambdaESOptimizer::setParametersFromDoubleVector(const QVector<double> &vector)
{
  for(int i = 0; i < mToOptimizeParameters.size(); i++)
  {
    ((DoubleValue*)mToOptimizeParameters.at(i)->value)->set(vector.at(i));
  }
}

void MuSlashRhoLambdaESOptimizer::updateFitnessValues()
{
	const ESInformation &info = mMuSlashRhoLambdaES->getInformation();
	
	mSmallestError->set(-1.0 * info.bestFitness);
	mHighestError->set(-1.0 * info.worstFitness);
	mMeanError->set(-1.0 * info.meanFitness);
	mErrorSTD->set(info.fitnessSTD);
}

bool MuSlashRhoLambdaESOptimizer::saveBestIndividuals()
{
	bool ok = true;
	
	QString path = getOptimizationRunOutputDirectory() + "/BestIndividuals/";
	
	const QList<ESIndividual*> &population = mMuSlashRhoLambdaES->getPopulation();
	
	Core::getInstance()->enforceDirectoryPath(path);
	
	////////////////////////////////////////////////////////////
	// read filenames of existing files and there error
	// and sort them according to their error
	////////////////////////////////////////////////////////////
	
	QDir folder(path);
	QStringList namefilter;
	namefilter += "*" + OrcsModelOptimizerConstants::FILE_ENDING_VALUE_PARAMETER;
	
	QStringList fileList = folder.entryList(namefilter, 
																					QDir::Files,
																					QDir::Name );
	QList< QPair<double, QString> > errorFilePairList; 
	
	for(int i = 0; i < fileList.size(); i++)
	{
		double error = fileList.at(i).split("_").at(3).toDouble(&ok);
		
		if(ok == false){
			Core::log(QString("%1 - saveBestIndividuals: ERROR - Could not convert the error \"%2\" of the result filename \"%3\" in to a double!").arg(	
								getName(),
								fileList.at(i).split("_").at(3),
								fileList.at(i)));
			return false;
		}
		
		QPair<double, QString> pair;
		pair.first = error;
		pair.second = fileList.at(i);
		
		errorFilePairList.append(pair);
	}
	
	qStableSort(errorFilePairList);
	
	//////////////////////////////////////////////////////
	// save N best individuals and delete the other old 
	// individuals
	///////////////////////////////////////////////////// 
	
	QList<QString> toHoldFiles; 	// list with filenames which should not be deleted
	QList<ESIndividual*> toSaveIndividuals; // list with individuals which should be saved to a file
	
	int populationIndex = 0;
	int fileIndex = 0;
	
	// check which new individuals have to be saved
	for(int i = 0; i < mNumberOfResultFiles->get(); i++)
	{
		// stop if all individuals are checked
		if(populationIndex >= population.size())
		{	
			if(fileIndex >= errorFilePairList.size()){
				break;
			}
			
			toHoldFiles.append(errorFilePairList.at(fileIndex).second);
			fileIndex++;
			continue;
		}
		
		if(fileIndex >= errorFilePairList.size())
		{
			toSaveIndividuals.append(population.at(populationIndex));
			populationIndex++;
			continue;
		}
		
		ESIndividual *curNewIndividual = population.at(populationIndex);
		
		// if current new individual and file are the same individuals
		// yes: dont store new individual, because it already exists
			
		unsigned long oldIndividualID =
				errorFilePairList.at(fileIndex).second.split("_").at(4).split(".").at(0).toULong(&ok);
		
		if(ok == false){
			Core::log(QString("%1 - saveBestIndividuals: ERROR - Could not convert the id \"%2\" of the result filename \"%3\" in to a ulong!").arg(	
								getName(),
								errorFilePairList.at(fileIndex).second.split("_").at(4),
								errorFilePairList.at(fileIndex).second));
			return false;
		}
		
		if(curNewIndividual->getID() == oldIndividualID)
		{
			toHoldFiles.append(errorFilePairList.at(fileIndex).second);
			populationIndex++;
			fileIndex++;
			continue;
		}
		
		// check if new individual is better than current file
		// yes: add new individual to create file list
		// no: hold current file 
		
		double newIndividualFitness = -1.0 * curNewIndividual->getFitness();
		double oldIndividualFitness = errorFilePairList.at(fileIndex).first;
		
		// is individual in population better than in file stored?
		if(newIndividualFitness < oldIndividualFitness)
		{
			toSaveIndividuals.append(curNewIndividual);
			populationIndex++;
		}
		else
		{
			toHoldFiles.append(errorFilePairList.at(fileIndex).second);
			fileIndex++;
		}
	}
	
	// save new Individuals
	for(int i = 0; i < toSaveIndividuals.size(); i++)
	{
		ESIndividual *individual = toSaveIndividuals.at(i);
		
		ok &= saveIndividual(individual,
												 folder.absoluteFilePath(
												 getFileNameBegin() 
														 + "_" 
														 + QString::number(-1.0 * individual->getFitness())
														 + "_"
														 + QString::number(individual->getID())
														 + OrcsModelOptimizerConstants::FILE_ENDING_VALUE_PARAMETER));	
	}
	
	// delete old files
	for(int i = 0 ; i < fileList.size() ; i++)
	{
		if(!toHoldFiles.contains(fileList.at(i)))
		{
			QFile::remove(folder.absoluteFilePath(fileList.at(i)));
		}
	}
	
	return ok;
}

bool MuSlashRhoLambdaESOptimizer::saveIndividual(	ESIndividual* individual, 
																									const QString &filename)
{
	const QList<OptimizationDouble> &parameters = individual->getObjectParameters();
	
	for(int i = 0; i < mToOptimizeParameters.size(); i++)
	{
		((DoubleValue*)mToOptimizeParameters.at(i)->value)->set(parameters.at(i).value);
	}
	
	return saveToOptimizeParameters(filename);
}

} // namespace nerd

