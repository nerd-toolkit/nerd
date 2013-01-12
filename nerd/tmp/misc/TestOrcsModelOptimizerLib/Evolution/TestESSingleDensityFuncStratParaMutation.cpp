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

#include "TestESSingleDensityFuncStratParaMutation.h"

#include "Evolution/ESInformation.h"
#include "Evolution/ESSingleDensityFuncStratParaMutation.h"
#include "Value/OptimizationDouble.h"
#include "Math/Math.h"

#include <cstdlib>
#include <cmath>

using namespace std;
using namespace nerd;


void TestESSingleDensityFuncStratParaMutation::testSingleDensityFuncStratParaMutation()
{
	ESInformation info;
	info.currentGeneration = 0;
	info.rho = 0;
	info.numberOfStrategyParameters = 1;
	info.numberOfObjectParameters = 3;
	info.numberOfLastSuccessfulIndividuals = 0;
	info.mu = 0;
	info.lambda = 0;
	
	ESSingleDensityFuncStratParaMutation *mutator = new ESSingleDensityFuncStratParaMutation(2.0);
	
	srand(1);
	double gaussianOne = Math::calculateGaussian(0.0,1.0);
		
	QList<OptimizationDouble> stratParas;
	stratParas.append(0.5);
	
	srand(1);
	QList<OptimizationDouble> newStratParas = mutator->doMutation(stratParas, info);
	
	// check number of new paras
	QCOMPARE(newStratParas.size(), stratParas.size());
	
	// check new paras
	QCOMPARE(newStratParas.at(0).value, 0.5 * exp( (1.0/pow(2.0 * 3.0, 0.5)) * gaussianOne));
	
	/////////////////////////////////////////////////////////
	// Check min violation
	/////////////////////////////////////////////////////////
	
	QList<OptimizationDouble> stratParas2;
	OptimizationDouble minViolation(-100.0, 0.0, 1.0);
	stratParas2.append(minViolation);
	
	srand(1);
	QList<OptimizationDouble> newStratParas2 = mutator->doMutation(stratParas2, info);
	
	// check number of new paras
	QCOMPARE(newStratParas2.size(), stratParas2.size());
	
	minViolation.value = 0.0;
	// check new paras
	QCOMPARE(newStratParas2.at(0), minViolation);
	
	/////////////////////////////////////////////////////////
	// Check max violation
	/////////////////////////////////////////////////////////
	
	QList<OptimizationDouble> stratParas3;
	OptimizationDouble maxViolation(100.0, 0.0, 1.0);
	stratParas3.append(maxViolation);
	
	srand(1);
	QList<OptimizationDouble> newStratParas3 = mutator->doMutation(stratParas3, info);
	
	// check number of new paras
	QCOMPARE(newStratParas3.size(), stratParas3.size());
	
	maxViolation.value = 1.0;
	// check new paras
	QCOMPARE(newStratParas3.at(0), maxViolation);

	
	delete mutator;
}
