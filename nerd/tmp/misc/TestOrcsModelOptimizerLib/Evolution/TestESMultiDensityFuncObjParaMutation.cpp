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

#include "TestESMultiDensityFuncObjParaMutation.h"

#include "Evolution/ESInformation.h"
#include "Evolution/ESMultiDensityFuncObjParaMutation.h"
#include "Value/OptimizationDouble.h"
#include "Math/Math.h"

#include <cstdlib>

using namespace std;
using namespace nerd;


void TestESMultiDensityFuncObjParaMutation::testMultiDensityFuncObjParaMutation()
{
	ESInformation info;
	info.currentGeneration = 0;
	info.rho = 0;
	info.numberOfStrategyParameters = 5;
	info.numberOfObjectParameters = 5;
	info.numberOfLastSuccessfulIndividuals = 0;
	info.mu = 0;
	info.lambda = 0;
	
	ESMultiDensityFuncObjParaMutation *mutator = new ESMultiDensityFuncObjParaMutation();
	
	srand(1);
	double gaussianOne = Math::calculateGaussian(0.0,1.0);
	double gaussianTwo = Math::calculateGaussian(0.0,1.0);
	double gaussianThree = Math::calculateGaussian(0.0,1.0);
	
	QList<OptimizationDouble> objParas;
	objParas.append(1.0);
	objParas.append(2.0);
	objParas.append(3.0);
	
	OptimizationDouble minViolate(-100.0, 0.0, 1.0);
	OptimizationDouble maxViolate(100.0, 0.0, 1.0);
	objParas.append(minViolate);
	objParas.append(maxViolate);

	
	QList<OptimizationDouble> stratParas;
	stratParas.append(0.5);
	stratParas.append(1.0);
	stratParas.append(1.5);
	stratParas.append(1.0);
	stratParas.append(1.0);
	
	srand(1);
	QList<OptimizationDouble> newObjParas = mutator->doMutation(objParas, stratParas, info);
	
	// check number of new paras
	QCOMPARE(newObjParas.size(), objParas.size());
	
	// check new paras
	QCOMPARE(newObjParas.at(0).value, 1.0 + (gaussianOne * 0.5));
	QCOMPARE(newObjParas.at(1).value, 2.0 + (gaussianTwo * 1.0));
	QCOMPARE(newObjParas.at(2).value, 3.0 + (gaussianThree * 1.5));
	
	minViolate.value = 0.0;
	maxViolate.value = 1.0;
	QCOMPARE(newObjParas.at(3), minViolate);
	QCOMPARE(newObjParas.at(4), maxViolate);

	
	delete mutator;
}

