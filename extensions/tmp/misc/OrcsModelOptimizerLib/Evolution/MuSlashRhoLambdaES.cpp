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


#include "MuSlashRhoLambdaES.h"

#include "ESIndividual.h"
#include "ESMarriage.h"
#include "ESRecombination.h"
#include "ESObjectParametersMutation.h"
#include "ESStrategyParametersMutation.h"
#include "ESSelection.h"
#include "ESFitnessEvaluation.h"
#include "ESInitialize.h"
#include "Value/OptimizationDouble.h"
#include "Optimization/StatisticTools.h"

#include <QtAlgorithms>
#include <QString>


//#include <QFile>
//#include <QTextStream>


namespace nerd {
	
bool MuSlashRhoLambdaES::descendingOrder(const ESIndividual *individual1,
																			 	 const ESIndividual *individual2)
{
	return individual1->getFitness() > individual2->getFitness();
}
	
MuSlashRhoLambdaES::MuSlashRhoLambdaES(	ESMarriage *marriage,
												ESRecombination *strategyParametersRecombination,
												ESRecombination *objectParametersRecombination,
												ESStrategyParametersMutation *strategyParametersMutation,
												ESObjectParametersMutation *objectParametersMutation,
												ESSelection *selection,
												ESFitnessEvaluation *fitnessEvaluation) 
	:	mMarriage(marriage),
 		mStrategyParametersRecombination(strategyParametersRecombination),
 		mObjectParametersRecombination(objectParametersRecombination),
 		mStrategyParametersMutation(strategyParametersMutation),
 		mObjectParametersMutation(objectParametersMutation),
 		mSelection(selection),
 		mFitnessEvaluation(fitnessEvaluation)											
{
	mInformation.currentGeneration = 0;
	mInformation.rho = 0;
	mInformation.mu = 0;
	mInformation.lambda = 0;
	mInformation.numberOfStrategyParameters = 0;
	mInformation.numberOfObjectParameters = 0;
	mInformation.numberOfLastSuccessfulIndividuals = 0;
}

MuSlashRhoLambdaES::~MuSlashRhoLambdaES() 
{
	deletePopulation();
}

bool MuSlashRhoLambdaES::startEvolutionRun(	int mu, 
																					 	int rho, 
																					 	int lambda, 
																					 	ESInitialize *init) 
{
	deletePopulation();
	
	// set information struct to values
	mInformation.currentGeneration = 0;
	mInformation.mu = mu;
	mInformation.rho = rho;
	mInformation.lambda = lambda;
	mInformation.numberOfLastSuccessfulIndividuals = 0;
	
	if(mInformation.mu <= 0 || mInformation.rho <= 0 || mInformation.lambda <= 0){
		return false;
	}
	
	mPopulation = init->createPopulation(mInformation.mu);
	
	if(mPopulation.size() <= 0){
		return false;
	}
	
	mInformation.numberOfStrategyParameters = mPopulation.first()->getStrategyParameters().size();
	mInformation.numberOfObjectParameters = mPopulation.first()->getObjectParameters().size();
	
	//calculate fitness values of population
	bool ok = mFitnessEvaluation->doEvaluation(mPopulation,
																				 mInformation);
	
	if(ok == false){
		return false;
	}
	
	qSort(mPopulation.begin(), 
				mPopulation.end(),
				MuSlashRhoLambdaES::descendingOrder);
	
	calculateStatistics();
	
	return true;
}

bool MuSlashRhoLambdaES::doNextGeneration() 
{
	/*
	QFile file ("evolution.log");
	file.open(QIODevice::WriteOnly | QIODevice::Append);
      
	QTextStream out(&file);
	
	
	out << "\n\n--------------------------\nNew generation:\n";
	*/
	
	bool ok = true;
	
	QList<ESIndividual*> offsprings;
	QList<double> bestParentsFitness;
	
	// generate offsprings
	for(int i = 0; i < mInformation.lambda; i++)
	{
		// select parents which should be used to generate current offspring
		QList<ESIndividual*> curParents = mMarriage->doMarriage(getPopulation(), 
																													  mInformation.rho,
																													  mInformation);
		
		// check if an error occured
		if(curParents.size() <= 0){
			return false;
		}
		
		// get fitness of the best parent
		qSort(curParents.begin(), 
					curParents.end(),
					MuSlashRhoLambdaES::descendingOrder);
		bestParentsFitness.append(curParents.first()->getFitness());
		
		/*
		out << "\tParents:\n";
		for(int parentIndex = 0; parentIndex < curParents.size(); parentIndex++)
		{
			out << "\t\t" << curParents.at(parentIndex)->toString() << "\n";
		}
		*/
		
		// recombine offspring strategy parameters from parents
		QList<OptimizationDouble> recombinedStratParas 
				= mStrategyParametersRecombination->doRecombination(
																						curParents,
																						ESRecombination::STRATEGY,
																						mInformation);
				
		// check if an error occured
		if(recombinedStratParas.size() <= 0){
			return false;
		}
		
		// recombine offspring object parameters from parents
		QList<OptimizationDouble> recombinedObjParas 
				= mObjectParametersRecombination->doRecombination(
																						curParents,
																						ESRecombination::OBJECT,
																						mInformation);
		
		// check if an error occured
		if(recombinedObjParas.size() <= 0){
			return false;
		}
		// mutate the offspring strategy parameters
		QList<OptimizationDouble> mutatedStratParas = mStrategyParametersMutation->doMutation(
																										recombinedStratParas,
																										mInformation);
		
		// check if an error occured
		if(mutatedStratParas.size() <= 0){
			return false;
		}
		// mutate the offspring object parameters with help of is mutated strategy parameters
		QList<OptimizationDouble> mutatedObjParas = mObjectParametersMutation->doMutation(
																									recombinedObjParas,
																									mutatedStratParas,
																									mInformation);
		
		// check if an error occured
		if(mutatedObjParas.size() <= 0){
			return false;
		}
		
		// create new Individual and add it to offspring list
		offsprings.append( new ESIndividual(mutatedObjParas, mutatedStratParas, 0.0) );
		
		/*
		out << "\t--> Offspring:\n";
		out << "\t\t" << offsprings.last()->toString();
		out << "\n\n";
		*/
	}	
	
	//calculate fitness values of offsprings
	ok &= mFitnessEvaluation->doEvaluation(offsprings,
																				 mInformation);
	
	if(ok == false){
		return false;
	}
	
	// calculate sum of offsprings which have a better fitness then their best parent
	mInformation.numberOfLastSuccessfulIndividuals = 0;
	for(int i = 0; i < offsprings.size(); i++)
	{
		if(offsprings.at(i)->getFitness() > bestParentsFitness.at(i)){
			mInformation.numberOfLastSuccessfulIndividuals++;
		}
	}
	bestParentsFitness.clear();
	
	// sort offspring accordingly their fitness
	qSort(offsprings.begin(), 
				offsprings.end(),
				MuSlashRhoLambdaES::descendingOrder);
	
	// get list with new population
	QList<ESIndividual*> selectionList = mSelection->doSelection(mPopulation, 
																															 offsprings, 
																															 mInformation.mu,
																															 mInformation);
	
	// check if an error occured
	if(selectionList.size() <= 0){
		return false;
	}
	
	// delete old parents which are not in the selection
	deletePopulation(selectionList);
	
	// use selection as new population
	mPopulation = selectionList;
	
	// update statisticvalues in mInformation
	calculateStatistics();

	mInformation.currentGeneration++;
	
	/*
	file.flush();
	file.close();
	*/
	
	return ok;
}

const QList<ESIndividual*> & MuSlashRhoLambdaES::getPopulation() const {
	return mPopulation;
}

ESIndividual* MuSlashRhoLambdaES::getBestIndividual() const
{
	if(mPopulation.size() > 0){
		return mPopulation.first();
	}
	else {
		return 0;
	}
}

int MuSlashRhoLambdaES::getGenerationCount() {
	return mInformation.currentGeneration;
}

const ESInformation & MuSlashRhoLambdaES::getInformation() {
	return mInformation;
}

void MuSlashRhoLambdaES::deletePopulation()
{
	for(int i = 0; i < mPopulation.size(); i++ )
	{
		if(mPopulation.at(i) != 0) {
			delete mPopulation.at(i);
		}
	}
	mPopulation.clear();
}


void MuSlashRhoLambdaES::deletePopulation(QList<ESIndividual*> whiteList)
{
	for(int i = 0; i < mPopulation.size(); i++ )
	{
		if(mPopulation.at(i) != 0 && !whiteList.contains(mPopulation.at(i))) {
			delete mPopulation.at(i);
		}
	}
	mPopulation.clear();
}


void MuSlashRhoLambdaES::calculateStatistics()
{
	if(mPopulation.size() <= 0){
		return;
	}
	
	mInformation.bestFitness = mPopulation.first()->getFitness();
	mInformation.worstFitness = mPopulation.last()->getFitness();
	
	// calculate mean and std
	QVector<double> fitnessList(mPopulation.size());
	
	for(int i = 0; i < mPopulation.size(); i++) {
		fitnessList.replace(i,mPopulation.at(i)->getFitness());
	}
	
	mInformation.meanFitness = StatisticTools::calculateMean(fitnessList);
	mInformation.fitnessSTD = 
			StatisticTools::calculateStandardDeviation(	fitnessList,
																									mInformation.meanFitness);
																									
	fitnessList.clear();
}


} // namespace nerd
