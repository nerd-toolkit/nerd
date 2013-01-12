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

#include "TestESOneFifthRuleStratParaMutation.h"

#include "Evolution/ESInformation.h"
#include "Evolution/ESOneFifthRuleStratParaMutation.h"
#include "Value/OptimizationDouble.h"
#include "Math/Math.h"

#include <cstdlib>
#include <cmath>

using namespace std;
using namespace nerd;


void TestESOneFifthRuleStratParaMutation::testOneFifthRuleStratParaMutation()
{
	ESInformation info;
	info.rho = 0;
	info.mu = 0;
	info.numberOfStrategyParameters = 0;
	info.numberOfObjectParameters = 0;
	info.lambda = 5;
	info.currentGeneration = 0;
	info.numberOfLastSuccessfulIndividuals = 0;
	
	ESOneFifthRuleStratParaMutation *mutator = new ESOneFifthRuleStratParaMutation(	2,
																																									0.5);			
	QList<OptimizationDouble> stratParas;
	stratParas.append(1.25);
	
	// initial generation
	info.currentGeneration = 0;
	info.numberOfLastSuccessfulIndividuals = 0;
	QList<OptimizationDouble> newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));

	
	//////////////////////////////////////////////////////////////////////
	// First case: P_s > 1/5
	//////////////////////////////////////////////////////////////////////

	// check generation
	info.currentGeneration = 1;
	info.numberOfLastSuccessfulIndividuals = 3;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));


		// check generation
	info.currentGeneration = 2;
	info.numberOfLastSuccessfulIndividuals = 3;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0).value, stratParas.at(0).value / 0.5 );
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0).value, stratParas.at(0).value / 0.5 );


	//////////////////////////////////////////////////////////////////////
	// Second case: P_s < 1/5
	//////////////////////////////////////////////////////////////////////
	
		// check generation
	info.currentGeneration = 3;
	info.numberOfLastSuccessfulIndividuals = 0;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));


	// check generation
	info.currentGeneration = 4;
	info.numberOfLastSuccessfulIndividuals = 1;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0).value, stratParas.at(0).value * 0.5);
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0).value, stratParas.at(0).value * 0.5);

	//////////////////////////////////////////////////////////////////////
	// Third case: P_s == 1/5
	//////////////////////////////////////////////////////////////////////
	
		// check generation
	info.currentGeneration = 5;
	info.numberOfLastSuccessfulIndividuals = 1;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));

	// check generation
	info.currentGeneration = 6;
	info.numberOfLastSuccessfulIndividuals = 1;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));



	delete mutator;
}

void TestESOneFifthRuleStratParaMutation::testOneFifthRuleStratParaMutationMinViolation()
{
	ESInformation info;
	info.rho = 0;
	info.mu = 0;
	info.numberOfStrategyParameters = 1;
	info.numberOfObjectParameters = 0;
	info.lambda = 5;
	info.currentGeneration = 0;
	info.numberOfLastSuccessfulIndividuals = 0;
	
	ESOneFifthRuleStratParaMutation *mutator = new ESOneFifthRuleStratParaMutation(	2,
			0.5);			
	QList<OptimizationDouble> stratParas;
	
	OptimizationDouble sp(1.25, 1.0, 10.0);
	stratParas.append(sp);
	
	// initial generation
	info.currentGeneration = 0;
	info.numberOfLastSuccessfulIndividuals = 0;
	QList<OptimizationDouble> newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));

	
	//////////////////////////////////////////////////////////////////////
	// First case: P_s > 1/5
	//////////////////////////////////////////////////////////////////////

	// check generation
	info.currentGeneration = 1;
	info.numberOfLastSuccessfulIndividuals = 3;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));


		// check generation
	info.currentGeneration = 2;
	info.numberOfLastSuccessfulIndividuals = 3;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());
		
	sp.setValueInsideRange(2.5);
	QCOMPARE(newStratParas.at(0), sp );
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), sp );


	//////////////////////////////////////////////////////////////////////
	// Second case: P_s < 1/5
	//////////////////////////////////////////////////////////////////////
	
		// check generation
	info.currentGeneration = 3;
	info.numberOfLastSuccessfulIndividuals = 0;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));


	// check generation
	info.currentGeneration = 4;
	info.numberOfLastSuccessfulIndividuals = 1;
	newStratParas = mutator->doMutation(stratParas, info);
	
	sp.setValueInsideRange(1.0);
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), sp);
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), sp);

	//////////////////////////////////////////////////////////////////////
	// Third case: P_s == 1/5
	//////////////////////////////////////////////////////////////////////
	
		// check generation
	info.currentGeneration = 5;
	info.numberOfLastSuccessfulIndividuals = 1;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));

	// check generation
	info.currentGeneration = 6;
	info.numberOfLastSuccessfulIndividuals = 1;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));



	delete mutator;
}

void TestESOneFifthRuleStratParaMutation::testOneFifthRuleStratParaMutationMaxViolation()
{
	ESInformation info;
	info.rho = 0;
	info.mu = 0;
	info.numberOfStrategyParameters = 1;
	info.numberOfObjectParameters = 0;
	info.lambda = 5;
	info.currentGeneration = 0;
	info.numberOfLastSuccessfulIndividuals = 0;
	
	ESOneFifthRuleStratParaMutation *mutator = new ESOneFifthRuleStratParaMutation(	2,
			0.5);			
	QList<OptimizationDouble> stratParas;
	
	OptimizationDouble sp(1.25, 0.0, 2.0);
	stratParas.append(sp);
	
	// initial generation
	info.currentGeneration = 0;
	info.numberOfLastSuccessfulIndividuals = 0;
	QList<OptimizationDouble> newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));

	
	//////////////////////////////////////////////////////////////////////
	// First case: P_s > 1/5
	//////////////////////////////////////////////////////////////////////

	// check generation
	info.currentGeneration = 1;
	info.numberOfLastSuccessfulIndividuals = 3;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));


		// check generation
	info.currentGeneration = 2;
	info.numberOfLastSuccessfulIndividuals = 3;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());
		
	sp.setValueInsideRange(2.0);
	QCOMPARE(newStratParas.at(0), sp );
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), sp );


	//////////////////////////////////////////////////////////////////////
	// Second case: P_s < 1/5
	//////////////////////////////////////////////////////////////////////
	
		// check generation
	info.currentGeneration = 3;
	info.numberOfLastSuccessfulIndividuals = 0;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));


	// check generation
	info.currentGeneration = 4;
	info.numberOfLastSuccessfulIndividuals = 1;
	newStratParas = mutator->doMutation(stratParas, info);
	
	sp.setValueInsideRange(1.25 * 0.5);
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), sp);
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), sp);

	//////////////////////////////////////////////////////////////////////
	// Third case: P_s == 1/5
	//////////////////////////////////////////////////////////////////////
	
		// check generation
	info.currentGeneration = 5;
	info.numberOfLastSuccessfulIndividuals = 1;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));

	// check generation
	info.currentGeneration = 6;
	info.numberOfLastSuccessfulIndividuals = 1;
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));
	
	newStratParas = mutator->doMutation(stratParas, info);
	
	QCOMPARE(newStratParas.size(), stratParas.size());	
	QCOMPARE(newStratParas.at(0), stratParas.at(0));



	delete mutator;
}
