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

#include "TestESIntermediateRecombination.h"

#include "Evolution/ESIndividual.h"
#include "Evolution/ESInformation.h"
#include "Evolution/ESIntermediateRecombination.h"


using namespace std;
using namespace nerd;


void TestESIntermediateRecombination::testIntermediateRecombination()
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
	ESIntermediateRecombination *recomb = new ESIntermediateRecombination();	
	
	// first test parent
	QList<OptimizationDouble> objParas1;
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
	
	QList<OptimizationDouble> objResults1;
	objResults1.append(p1);
	objResults1.append(2.0);
	objResults1.append(3.0);
	checkRecombination(recombinedObjParas1,objResults1);
	
		// test if recombination of object parameters works with 1 parent
	QList<OptimizationDouble> recombinedStratParas1 = recomb->doRecombination(parents,
			 																													ESRecombination::STRATEGY, 
																																info);
	
	QList<OptimizationDouble> stratResults1;
	stratResults1.append(10.0);
	stratResults1.append(20.0);
	stratResults1.append(30.0);
	stratResults1.append(40.0);
	checkRecombination(recombinedStratParas1,stratResults1);
	
	
	
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
	
	QList<OptimizationDouble> objResults2;
	OptimizationDouble r2((1.0 + 4.0) / 2.0, -10.0, 10.0);
	
	objResults2.append(r2);
	objResults2.append((2.0 + 5.0) / 2.0);
	objResults2.append((3.0 + 6.0) / 2.0);
	checkRecombination(recombinedObjParas2,objResults2);
	
	// test if recombination of object parameters works with 2 parents
	QList<OptimizationDouble> recombinedStratParas2 = recomb->doRecombination(parents,
			ESRecombination::STRATEGY, 
			info);
	
	QList<OptimizationDouble> stratResults2;
	stratResults2.append((10.0 + 50.0) / 2.0);
	stratResults2.append((20.0 + 60.0) / 2.0);
	stratResults2.append((30.0 + 70.0) / 2.0);
	stratResults2.append((40.0 + 80.0) / 2.0);
	checkRecombination(recombinedStratParas2,stratResults2);

	
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
	
	QList<OptimizationDouble> objResults3;
	OptimizationDouble r3((1.0 + 4.0 + 7.0) / 3.0, -10.0, 10.0);
	
	objResults3.append(r3);
	objResults3.append((2.0 + 5.0 + 8.0) / 3.0);
	objResults3.append((3.0 + 6.0 + 9.0) / 3.0);
	checkRecombination(recombinedObjParas3,objResults3);
	
	// test if recombination of object parameters works with 2 parents
	QList<OptimizationDouble> recombinedStratParas3 = recomb->doRecombination(parents,
			ESRecombination::STRATEGY, 
			info);

	QList<OptimizationDouble> stratResults3;
	stratResults3.append((10.0 + 50.0 + 90.0) / 3.0);
	stratResults3.append((20.0 + 60.0 + 100.0) / 3.0);
	stratResults3.append((30.0 + 70.0 + 110.0) / 3.0);
	stratResults3.append((40.0 + 80.0 + 120.0) / 3.0);
	checkRecombination(recombinedStratParas3,stratResults3);


	deleteTestPopulation(parents);
	delete recomb;

}

void TestESIntermediateRecombination::checkRecombination(	
		const QList<OptimizationDouble> recombinedParas,
		const QList<OptimizationDouble> results)
{
	QVERIFY(recombinedParas.size() == results.size());		
	
	// check elements of the lists
	for(int i = 0; i < recombinedParas.size(); i++) {
		QCOMPARE(recombinedParas.at(i), results.at(i));
	}	
}


void TestESIntermediateRecombination::deleteTestPopulation(QList<ESIndividual*> &population)
{
	for(int i = 0; i < population.size();i++)
	{
		if(population.at(i) != 0){
			delete population.at(i);
		}
	}
	population.clear();
}

