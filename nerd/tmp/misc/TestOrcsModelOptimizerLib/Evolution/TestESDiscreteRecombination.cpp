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

#include "TestESDiscreteRecombination.h"

#include "Evolution/ESIndividual.h"
#include "Evolution/ESInformation.h"
#include "Evolution/ESDiscreteRecombination.h"


using namespace std;
using namespace nerd;


void TestESDiscreteRecombination::testDiscreteRecombination()
{
	ESInformation info;
	info.currentGeneration = 0;
	info.rho = 0;
	info.numberOfStrategyParameters = 0;
	info.numberOfObjectParameters = 0;
	info.numberOfLastSuccessfulIndividuals = 0;
	info.mu = 0;
	info.lambda = 0;
	
	QList<ESIndividual*> parents;
	ESDiscreteRecombination *recomb = new ESDiscreteRecombination();	
	
	// first test parent
	QList<OptimizationDouble> objParas1;
	// check if ranes are set corectly by recombination
	OptimizationDouble p1(1.0, -10.0, 10.0);
	objParas1.append(p1);
	objParas1.append(2.0);
	objParas1.append(3.0);
	
	QList<OptimizationDouble> strategyParas1;
	strategyParas1.append(10.0);
	strategyParas1.append(20.0);
	strategyParas1.append(30.0);
	strategyParas1.append(40.0);
	
	ESIndividual *parent1 = new ESIndividual(objParas1, strategyParas1, 0.0);
	parents.append( parent1 );

	// test if recombination of object parameters works with 1 parent
	QList<OptimizationDouble> recombinedObjParas1 = recomb->doRecombination(parents,
			ESRecombination::OBJECT, 
			info);
	
	checkRecombination(recombinedObjParas1,parents, ESRecombination::OBJECT);
	
		// test if recombination of object parameters works with 1 parent
	QList<OptimizationDouble> recombinedStratParas1 = recomb->doRecombination(parents,
			ESRecombination::STRATEGY, 
			info);
	
	checkRecombination(recombinedStratParas1,parents, ESRecombination::STRATEGY);
	
	
	
	////////////////////////////////////////////////////////////////
	// Test with 2 parents
	
	// second test parent
	QList<OptimizationDouble> objParas2;
	OptimizationDouble p2(4.0, -10.0, 10.0);
	objParas2.append(p2);
	objParas2.append(5.0);
	objParas2.append(6.0);
	
	QList<OptimizationDouble> strategyParas2;
	strategyParas2.append(50.0);
	strategyParas2.append(60.0);
	strategyParas2.append(70.0);
	strategyParas2.append(80.0);
	
	ESIndividual *parent2 = new ESIndividual(objParas2, strategyParas2, 0.0);
	parents.append( parent2 );

	// test if recombination of object parameters works with 2 parents
	QList<OptimizationDouble> recombinedObjParas2 = recomb->doRecombination(parents,
			ESRecombination::OBJECT, 
			info);
	
	checkRecombination(recombinedObjParas2,parents, ESRecombination::OBJECT);
	
	// test if recombination of object parameters works with 2 parents
	QList<OptimizationDouble> recombinedStratParas2 = recomb->doRecombination(parents,
			ESRecombination::STRATEGY, 
			info);
	
	checkRecombination(recombinedStratParas2,parents, ESRecombination::STRATEGY);

	
	////////////////////////////////////////////////////////////////
	// Test with 3 parents

	// second test parent
	QList<OptimizationDouble> objParas3;
	OptimizationDouble p3(7.0, -10.0, 10.0);

	objParas3.append(p3);
	objParas3.append(8.0);
	objParas3.append(9.0);
	
	QList<OptimizationDouble> strategyParas3;
	strategyParas3.append(90.0);
	strategyParas3.append(100.0);
	strategyParas3.append(110.0);
	strategyParas3.append(120.0);
	
	ESIndividual *parent3 = new ESIndividual(objParas3, strategyParas3, 0.0);
	parents.append( parent3 );

	// test if recombination of object parameters works with 2 parents
	QList<OptimizationDouble> recombinedObjParas3 = recomb->doRecombination(parents,
			ESRecombination::OBJECT, 
			info);
	
	checkRecombination(recombinedObjParas3, parents, ESRecombination::OBJECT);
	
	// test if recombination of object parameters works with 2 parents
	QList<OptimizationDouble> recombinedStratParas3 = recomb->doRecombination(parents,
			ESRecombination::STRATEGY, 
			info);

	checkRecombination(recombinedStratParas3, parents, ESRecombination::STRATEGY);


	deleteTestPopulation(parents);
	delete recomb;
}

void TestESDiscreteRecombination::checkRecombination(
									QList<OptimizationDouble> recombinedParas,
									QList<ESIndividual*> parents,
									const ESRecombination::parameterType type)
{
	// check size of lists
	if(type == ESRecombination::OBJECT) {
		QVERIFY(recombinedParas.size() == parents.at(0)->getObjectParameters().size());		
	}
	else {
		QVERIFY(recombinedParas.size() == parents.at(0)->getStrategyParameters().size());
	}
	
	// check elements of the lists
	for(int i = 0; i < recombinedParas.size(); i++)
	{
		bool foundCorrectValue = false;
		
		for(int j = 0; j < parents.size(); j++)
		{
			if(type == ESRecombination::OBJECT) {
				OptimizationDouble currentParentPara = parents.at(j)->getObjectParameters().at(i);
						
				if(currentParentPara == recombinedParas.at(i)){
					foundCorrectValue = true;
					break;
				}
			}
			else {
				OptimizationDouble currentParentPara = parents.at(j)->getStrategyParameters().at(i);
						
				if(currentParentPara == recombinedParas.at(i)){
					foundCorrectValue = true;
					break;
				}
			}
		}
		
		QVERIFY2(foundCorrectValue, "The current parameter was wrongly recombined!");
	}
}


void TestESDiscreteRecombination::deleteTestPopulation(QList<ESIndividual*> &population)
{
	for(int i = 0; i < population.size();i++)
	{
		if(population.at(i) != 0){
			delete population.at(i);
		}
	}
	population.clear();
}

