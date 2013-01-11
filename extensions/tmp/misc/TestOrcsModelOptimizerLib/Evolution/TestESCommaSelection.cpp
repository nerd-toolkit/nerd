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

#include "TestESCommaSelection.h"

#include "Evolution/ESIndividual.h"
#include "Evolution/ESInformation.h"
#include "Evolution/ESCommaSelection.h"
#include "Evolution/MuSlashRhoLambdaES.h"
#include "Value/OptimizationDouble.h"

#include <QtAlgorithms>

using namespace std;
using namespace nerd;


void TestESCommaSelection::testCommaSelection()
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
	QList<ESIndividual*> offsprings;
	
	ESCommaSelection *selector = new ESCommaSelection;	
	
	QList<OptimizationDouble> tmp;
	
	// create test individuals
	ESIndividual *parentOne = new ESIndividual(tmp,tmp, 10.0); 
	ESIndividual *parentTwo = new ESIndividual(tmp,tmp, 4.0);
	ESIndividual *offspringOne = new ESIndividual(tmp,tmp, 8.0);
	ESIndividual *offspringTwo = new ESIndividual(tmp,tmp, 6.0);
	ESIndividual *offspringThree = new ESIndividual(tmp,tmp, 4.0);
	ESIndividual *offspringFour = new ESIndividual(tmp,tmp, 2.0);

	// add individuals unsorted to populations
	parents.append(parentTwo);
	parents.append(parentOne);
	offsprings.append(offspringFour);
	offsprings.append(offspringTwo);
	offsprings.append(offspringOne);
	offsprings.append(offspringThree);
	
	qSort(parents.begin(), 
				parents.end(),
				MuSlashRhoLambdaES::descendingOrder);
	
	qSort(offsprings.begin(), 
				offsprings.end(),
				MuSlashRhoLambdaES::descendingOrder);
	
	// check selection of all and sorting
	QList<ESIndividual*> selectionOne = selector->doSelection(parents, offsprings, 4, info);
	QCOMPARE(selectionOne.size(), 4);
	QCOMPARE(selectionOne.at(0), offspringOne);
	QCOMPARE(selectionOne.at(1), offspringTwo);
	QCOMPARE(selectionOne.at(2), offspringThree);
	QCOMPARE(selectionOne.at(3), offspringFour);
	
	
	// check selection of half and sorting
	QList<ESIndividual*> selectionTwo = selector->doSelection(parents, offsprings, 2, info);
	QCOMPARE(selectionTwo.size(), 2);
	QCOMPARE(selectionTwo.at(0), offspringOne);
	QCOMPARE(selectionTwo.at(1), offspringTwo);
	
	delete selector;
	deleteTestPopulation(parents);
	deleteTestPopulation(offsprings);
}


void TestESCommaSelection::deleteTestPopulation(QList<ESIndividual*> &population)
{
	for(int i = 0; i < population.size();i++)
	{
		if(population.at(i) != 0){
			delete population.at(i);
		}
	}
	population.clear();
}

