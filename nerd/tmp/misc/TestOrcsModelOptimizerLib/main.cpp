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



#include "Util/UnitTestMacros.h"

#include "Math/TestParabolicBracketFinder.h"
#include "Math/TestBrentOneDimMinimizer.h"
#include "Math/TestMultiToOneDimFunction.h"
#include "Math/TestPowellMultiDimMinimizer.h"

#include "Evolution/TestESMarriage.h"
#include "Evolution/TestESDiscreteRecombination.h"
#include "Evolution/TestESIntermediateRecombination.h"
#include "Evolution/TestESPlusSelection.h"
#include "Evolution/TestESCommaSelection.h"
#include "Evolution/TestESSingleDensityFuncObjParaMutation.h"
#include "Evolution/TestESMultiDensityFuncObjParaMutation.h"
#include "Evolution/TestESSingleDensityFuncStratParaMutation.h"
#include "Evolution/TestESMultiDensityFuncStratParaMutation.h"
#include "Evolution/TestESOneFifthRuleStratParaMutation.h"
#include "Evolution/TestESRandomInitialize.h"
#include "Evolution/TestMuSlashRhoLambdaES.h"

#include "Optimization/TestXMLConfigDocument.h"

TEST_START("TestOrcsModelOptimizer", 1, -1, 17);

	// Test general minimization methods
	TEST(TestParabolicBracketFinder);
	TEST(TestBrentOneDimMinimizer);
	TEST(TestMultiToOneDimFunction);
	TEST(TestPowellMultiDimMinimizer);
	// Test EvolutionStrategie
	TEST(TestESMarriage);
	TEST(TestESDiscreteRecombination);
	TEST(TestESIntermediateRecombination);
	TEST(TestESPlusSelection);
	TEST(TestESCommaSelection);
	TEST(TestESSingleDensityFuncObjParaMutation);
	TEST(TestESMultiDensityFuncObjParaMutation);
	TEST(TestESSingleDensityFuncStratParaMutation);
	TEST(TestESMultiDensityFuncStratParaMutation);
	TEST(TestESOneFifthRuleStratParaMutation);
	TEST(TestESRandomInitialize);
	TEST(TestMuSlashRhoLambdaES);
	
	TEST(TestXMLConfigDocument);
	
TEST_END;


