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

#include "TestMuSlashRhoLambdaES.h"


#include <QList>
#include "Value/ValueRange.h"
#include "Evolution/ESIndividual.h"
#include "Evolution/ESMarriage.h"
#include "Evolution/ESRecombination.h"
#include "Evolution/ESDiscreteRecombination.h"
#include "Evolution/ESIntermediateRecombination.h"
#include "Evolution/ESStrategyParametersMutation.h"
#include "Evolution/ESOneFifthRuleStratParaMutation.h"
#include "Evolution/ESObjectParametersMutation.h"
#include "Evolution/ESSingleDensityFuncObjParaMutation.h"
#include "Evolution/ESSelection.h"
#include "Evolution/ESPlusSelection.h"
#include "Evolution/ESCommaSelection.h"
#include "Evolution/ESInitialize.h"
#include "Evolution/ESRandomInitialize.h"
#include "Evolution/MuSlashRhoLambdaES.h"
#include "Evolution/ESFitnessEvaluation.h"
#include "Evolution/ESInformation.h"
#include "ESTestFitnessEvaluation.h"
#include "Evolution/ESInitializeValue.h"

using namespace std;
using namespace nerd;


void TestMuSlashRhoLambdaES::testPlusES()
{		
	ESMarriage *marriage = new ESMarriage();
 	ESRecombination *strategyRecombination = new ESDiscreteRecombination();
	ESRecombination *objRecombination = new ESIntermediateRecombination();
	ESStrategyParametersMutation *strategyMutation = new ESOneFifthRuleStratParaMutation(2,0.9);
	ESObjectParametersMutation *objMutation = new ESSingleDensityFuncObjParaMutation();
	ESSelection *plusSelection = new ESPlusSelection();
	
	ESInitializeValue range(0.0,2.0);
	ESInitialize *init = new ESRandomInitialize(range,2,range,1);
	
	ESTestFitnessEvaluation *eval = new ESTestFitnessEvaluation(1.0);
	
	MuSlashRhoLambdaES *es = new MuSlashRhoLambdaES(marriage, 
																									strategyRecombination, 
																									objRecombination, 
																									strategyMutation, 
																									objMutation, 
						 																			plusSelection,
						 																			eval); 
	
	bool ok = es->startEvolutionRun(10,2,100,init);
	QCOMPARE(ok, true);
	
	checkEvolutionInteration(es, 
													 10, 
													 0, 
													 1.0, 
													 0,
													 1.0,
													 1.0,
													 1.0,
													 0.0);

	eval->setFitness(0.5);
	ok = es->doNextGeneration();
	QCOMPARE(ok, true);
	
	checkEvolutionInteration(es, 10, 1, 1.0, 0,
													 1.0,
													 1.0,
													 1.0,
													 0.0);
	
	eval->setFitness(1.5);
	ok = es->doNextGeneration();
	QCOMPARE(ok, true);
	
	checkEvolutionInteration(es, 10, 2, 1.5, 100,
													 1.5,
													 1.5,
													 1.5,
													 0.0);
	
	delete marriage;
	delete strategyRecombination;
	delete objRecombination;
	delete strategyMutation;
	delete objMutation;
	delete plusSelection;
	delete eval;
	delete init;

}

void TestMuSlashRhoLambdaES::testCommaES()
{		
	ESMarriage *marriage = new ESMarriage();
	ESRecombination *strategyRecombination = new ESDiscreteRecombination();
	ESRecombination *objRecombination = new ESIntermediateRecombination();
	ESStrategyParametersMutation *strategyMutation = new ESOneFifthRuleStratParaMutation(2,0.9);
	ESObjectParametersMutation *objMutation = new ESSingleDensityFuncObjParaMutation();
	
	ESSelection *commaSelection = new ESCommaSelection();
	
	ESInitializeValue range(0.0,2.0);
	ESInitialize *init = new ESRandomInitialize(range,2,range,1);
	
	ESTestFitnessEvaluation *eval = new ESTestFitnessEvaluation(1.0);
	
	MuSlashRhoLambdaES *es = new MuSlashRhoLambdaES(marriage, 
																									strategyRecombination, 
																									objRecombination, 
																									strategyMutation, 
																									objMutation, 
																									commaSelection,
																									eval); 
	
	bool ok = es->startEvolutionRun(10,3,50,init);
	QCOMPARE(ok, true);
	
	checkEvolutionInteration(es, 10, 0, 1.0, 0,
													 1.0,
													 1.0,
													 1.0,
													 0.0);

	eval->setFitness(0.5);
	ok = es->doNextGeneration();
	QCOMPARE(ok, true);
	
	checkEvolutionInteration(es, 10, 1, 0.5, 0,
													 0.5,
													 0.5,
													 0.5,
													 0.0);
	
	eval->setFitness(1.5);
	ok = es->doNextGeneration();
	QCOMPARE(ok, true);
	
	checkEvolutionInteration(es, 10, 2, 1.5, 50,
													 1.5,
													 1.5,
													 1.5,
													 0.0);
	
	delete marriage;
	delete strategyRecombination;
	delete objRecombination;
	delete strategyMutation;
	delete objMutation;
	delete commaSelection;
	delete eval;
	delete init;

}



void TestMuSlashRhoLambdaES::checkEvolutionInteration(MuSlashRhoLambdaES *es, 
																											int populationSize, 
																											int generationCount, 
																											double fitness,
																										  int lastSuccessfulOffsprings,
																										 	double bestFitness,
																										 	double worstFitness,
																										  double meanFitness,
																										 	double fitnessSTD)
{
	QCOMPARE(es->getPopulation().size(), populationSize);
	QCOMPARE(es->getGenerationCount(), generationCount);
	
	for(int i = 0; i < es->getPopulation().size(); i++ )
	{
		QCOMPARE(es->getPopulation().at(i)->getFitness(), fitness);
	}
	
	QCOMPARE(es->getInformation().numberOfLastSuccessfulIndividuals, lastSuccessfulOffsprings);
	
	QCOMPARE(es->getInformation().bestFitness, bestFitness);
	QCOMPARE(es->getInformation().worstFitness, worstFitness);
	QCOMPARE(es->getInformation().meanFitness, meanFitness);
	QCOMPARE(es->getInformation().fitnessSTD, fitnessSTD);
	
}

