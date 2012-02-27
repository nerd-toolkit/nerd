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
#include "TestESMarriage.h"

#include "Evolution/ESIndividual.h"
#include "Evolution/ESInformation.h"
#include "Evolution/ESMarriage.h"
#include "Value/OptimizationDouble.h"


using namespace std;
using namespace nerd;


void TestESMarriage::testMarriage()
{
	ESInformation info;
	info.currentGeneration = 0;
	info.rho = 0;
	info.numberOfStrategyParameters = 0;
	info.numberOfObjectParameters = 0;
	info.numberOfLastSuccessfulIndividuals = 0;
	info.mu = 0;
	info.lambda = 0;
	
	QList<ESIndividual*> population = createTestPopulation(10);
		
	ESMarriage *marriage = new ESMarriage();
	
	QList<ESIndividual*> married = marriage->doMarriage(population, 0, info);
	QCOMPARE( married.size(), 0);

	
	married = marriage->doMarriage(population, 1, info);
	QCOMPARE( married.size(), 1);
	QVERIFY( population.contains( married.at(0) ) );
	
	
	married = marriage->doMarriage(population, 5, info);
	QCOMPARE( married.size(), 5);
	for(int i = 0; i < married.size(); i++)
	{ 
		QVERIFY( population.contains( married.at(i) ) );
		// check if the item occure only once
		QCOMPARE( married.count(married.at(i)), 1);
	}
	
	married = marriage->doMarriage(population, population.size(), info);
	QCOMPARE( married.size(), population.size());
	for(int i = 0; i < married.size(); i++)
	{ 
		QVERIFY( population.contains( married.at(i) ) );
		// check if the item occure only once
		QCOMPARE( married.count(married.at(i)), 1 );
	}

	deleteTestPopulation(population);
	delete marriage;
}


QList<ESIndividual*> TestESMarriage::createTestPopulation(int size)
{
	QList<ESIndividual*> population;
	QList<OptimizationDouble> tmp;
	for(int i = 0; i < size; i++) {
		population.append( new ESIndividual(tmp, tmp, 0.0) );
	}
	
	return population;
}

void TestESMarriage::deleteTestPopulation(QList<ESIndividual*> &population)
{
	for(int i = 0; i < population.size();i++)
	{
		if(population.at(i) != 0){
			delete population.at(i);
		}
	}
	population.clear();
}

