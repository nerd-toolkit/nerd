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

#include "TestESRandomInitialize.h"

#include "Evolution/ESIndividual.h"
#include "Evolution/ESRandomInitialize.h"
#include "Evolution/ESInitializeValue.h"
#include "Value/ValueRange.h"
#include "Value/OptimizationDouble.h"

#include "Math/Random.h"
#include <limits>

using namespace std;
using namespace nerd;


void TestESRandomInitialize::testRandomInitialize()
{
	Random::init();
	Random::setSeed(1);
	
	double randomOne = Random::nextDoubleBetween(0.0, 1.0);
	double randomTwo = Random::nextDoubleBetween(0.0, 1.0);
	double randomThree = Random::nextDoubleBetween(-2.0, 2.0);
	double randomFour = Random::nextDoubleBetween(-2.0, 2.0);
	double randomFive = Random::nextDoubleBetween(-100.0, 100.0);
	
	ESInitializeValue rangeOne(0.0,1.0);
		
	ESInitializeValue rangeTwo(-2.0, 2.0);
	
	ESInitializeValue rangeThree(-100.0, 100.0);
		
	ESInitializeValue definedInit(2.1);
	
	QList<ESInitializeValue> objRanges;
	objRanges.append(rangeOne);
	objRanges.append(rangeOne);
	objRanges.append(definedInit);
	
	QList<ESInitializeValue> stratRanges;
	stratRanges.append(rangeTwo);
	stratRanges.append(rangeTwo);
	stratRanges.append(rangeThree);
	
	OptimizationDouble valueOne(randomOne, 0.0, 1.0);
	OptimizationDouble valueTwo(randomTwo, 0.0, 1.0);
	OptimizationDouble valueThree(randomThree, -2.0, 2.0);
	OptimizationDouble valueFour(randomFour, -2.0, 2.0);
	OptimizationDouble valueFive(randomFive, -100.0, 100.0);
	
	OptimizationDouble definedValue(2.1,
																	numeric_limits<double>::min() ,
																	numeric_limits<double>::max() );
	
	
	
	///////////////////////////
	// Test first constructor
	///////////////////////////
	
	Random::setSeed(1);
	ESRandomInitialize *initOne = new ESRandomInitialize(objRanges, stratRanges);
	QList<ESIndividual*> populationOne = initOne->createPopulation(1);
	
	QCOMPARE(populationOne.size(), 1);
	QCOMPARE(populationOne.at(0)->getObjectParameters().size(), objRanges.size());
	QCOMPARE(populationOne.at(0)->getObjectParameters().at(0), valueOne);
	QCOMPARE(populationOne.at(0)->getObjectParameters().at(1), valueTwo);
	QCOMPARE(populationOne.at(0)->getObjectParameters().at(2), definedValue);
	
	QCOMPARE(populationOne.at(0)->getStrategyParameters().size(), stratRanges.size());
	QCOMPARE(populationOne.at(0)->getStrategyParameters().at(0), valueThree);
	QCOMPARE(populationOne.at(0)->getStrategyParameters().at(1), valueFour);
	QCOMPARE(populationOne.at(0)->getStrategyParameters().at(2), valueFive);

	deleteTestPopulation(populationOne);
	delete initOne;
	
	
	///////////////////////////
	// Test second constructor
	///////////////////////////
	
	Random::setSeed(1);
	ESRandomInitialize *initTwo = new ESRandomInitialize(objRanges, rangeTwo, 2);
	QList<ESIndividual*> populationTwo = initTwo->createPopulation(1);
	
	QCOMPARE(populationTwo.size(), 1);
	QCOMPARE(populationTwo.at(0)->getObjectParameters().size(), objRanges.size());
	QCOMPARE(populationTwo.at(0)->getObjectParameters().at(0), valueOne);
	QCOMPARE(populationTwo.at(0)->getObjectParameters().at(1), valueTwo);
	QCOMPARE(populationTwo.at(0)->getObjectParameters().at(2), definedValue);
	
	QCOMPARE(populationTwo.at(0)->getStrategyParameters().size(), 2);
	QCOMPARE(populationTwo.at(0)->getStrategyParameters().at(0), valueThree);
	QCOMPARE(populationTwo.at(0)->getStrategyParameters().at(1), valueFour);

	deleteTestPopulation(populationTwo);
	delete initTwo;
	
	///////////////////////////
	// Test third constructor
	///////////////////////////
	
	Random::setSeed(1);
	ESRandomInitialize *initThree = new ESRandomInitialize(rangeOne, 2, stratRanges);
	QList<ESIndividual*> populationThree = initThree->createPopulation(1);
	
	QCOMPARE(populationThree.size(), 1);
	QCOMPARE(populationThree.at(0)->getObjectParameters().size(), 2);
	QCOMPARE(populationThree.at(0)->getObjectParameters().at(0), valueOne);
	QCOMPARE(populationThree.at(0)->getObjectParameters().at(1), valueTwo);
	
	QCOMPARE(populationThree.at(0)->getStrategyParameters().size(), stratRanges.size());
	QCOMPARE(populationThree.at(0)->getStrategyParameters().at(0), valueThree);
	QCOMPARE(populationThree.at(0)->getStrategyParameters().at(1), valueFour);
	QCOMPARE(populationThree.at(0)->getStrategyParameters().at(2), valueFive);

	deleteTestPopulation(populationThree);
	delete initThree;
	
	///////////////////////////
	// Test fourth constructor
	///////////////////////////
	
	Random::setSeed(1);
	ESRandomInitialize *initFour = new ESRandomInitialize(rangeOne, 2, rangeTwo, 2);
	QList<ESIndividual*> populationFour = initFour->createPopulation(1);
	
	QCOMPARE(populationFour.size(), 1);
	QCOMPARE(populationFour.at(0)->getObjectParameters().size(), 2);
	QCOMPARE(populationFour.at(0)->getObjectParameters().at(0), valueOne);
	QCOMPARE(populationFour.at(0)->getObjectParameters().at(1), valueTwo);
	
	QCOMPARE(populationFour.at(0)->getStrategyParameters().size(), 2);
	QCOMPARE(populationFour.at(0)->getStrategyParameters().at(0), valueThree);
	QCOMPARE(populationFour.at(0)->getStrategyParameters().at(1), valueFour);

	deleteTestPopulation(populationFour);
	delete initFour;
	
}


void TestESRandomInitialize::deleteTestPopulation(QList<ESIndividual*> &population)
{
	for(int i = 0; i < population.size();i++)
	{
		if(population.at(i) != 0){
			delete population.at(i);
		}
	}
	population.clear();
}

