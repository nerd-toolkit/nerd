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

#include "TestPowellMultiDimMinimizer.h"
#include "TwoDimQuadraticFunctionWithBounds.h"
#include "TwoDimQuadraticFunction.h"
#include "Math/ParabolicBracketFinder.h"
#include "Math/OneDimMinimizer.h"
#include "Math/BrentOneDimMinimizer.h"
#include "Math/MultiDimFunctionPoint.h"
#include "Math/PowellMultiDimMinimizer.h"

#include <cmath>

using namespace std;
using namespace nerd;


//Chris
void TestPowellMultiDimMinimizer::testTwoDimQuadraticFunction() 
{
  const double DELTA = 1E-5;
  
  TwoDimQuadraticFunction *f = new TwoDimQuadraticFunction(); 
  
  ParabolicBracketFinder *bf = new ParabolicBracketFinder();
  
  OneDimMinimizer *blm = new BrentOneDimMinimizer(bf);
   
  PowellMultiDimMinimizer *po = new PowellMultiDimMinimizer(blm);
  
  
  QVector<double> startPoint(2,0.0);
  
  // Test minimize function

  MultiDimFunctionPoint p = po->minimize(f, startPoint);
  
	QVERIFY( fabs(p.x[0] - f->minAx) <= DELTA);
  QVERIFY( fabs(p.x[1] - f->minBx) <= DELTA);
	QVERIFY( fabs(p.y - f->minY) <= po->getTolerance());
	QVERIFY( f->calculate(p.x) == p.y );
  
  // Test if iteration steps get better and better
  QVERIFY( po->minimizationStart(f, startPoint) );
  
  double lastY = po->getMinimumPoint().y;
  while(!po->doMinimizationStep())
  {
    QVERIFY( po->getMinimumPoint().y <= lastY);
    lastY = po->getMinimumPoint().y;
  }
  
  
  delete f;
  delete bf;
  delete blm;
  delete po;  
}

void TestPowellMultiDimMinimizer::testTwoDimQuadraticFunctionWithBounds()
{
	const double DELTA = 1E-5;
  
	TwoDimQuadraticFunctionWithBounds *f = new TwoDimQuadraticFunctionWithBounds(); 
  
	ParabolicBracketFinder *bf = new ParabolicBracketFinder();
  
	OneDimMinimizer *blm = new BrentOneDimMinimizer(bf);
   
	PowellMultiDimMinimizer *po = new PowellMultiDimMinimizer(blm);
  
  
	QVector<double> startPoint(2,-2.0);
  
  // Test minimize function

	MultiDimFunctionPoint p = po->minimize(f, startPoint);
  
	QVERIFY( abs(p.x[0] - f->minAx) <= DELTA);
	QVERIFY( abs(p.x[1] - f->minBx) <= DELTA);
	QVERIFY( abs(p.y - f->minY) <= po->getTolerance());
	QVERIFY( f->calculate(p.x) == p.y );
  
  // Test if iteration steps get better and better
	QVERIFY( po->minimizationStart(f, startPoint) );
  
	double lastY = po->getMinimumPoint().y;
	while(!po->doMinimizationStep())
	{
		QVERIFY( po->getMinimumPoint().y <= lastY);
		lastY = po->getMinimumPoint().y;
	}
  
  
	delete f;
	delete bf;
	delete blm;
	delete po; 
}


void TestPowellMultiDimMinimizer::testToleranceSetting()
{
  
	TwoDimQuadraticFunction *f = new TwoDimQuadraticFunction(); 
  
	ParabolicBracketFinder *bf = new ParabolicBracketFinder();
  
	OneDimMinimizer *blm = new BrentOneDimMinimizer(bf);
   
	PowellMultiDimMinimizer *po = new PowellMultiDimMinimizer(blm);
  
	QVector<double> startPoint(2.0,0.0);
	
	
	po->setTolerance(10.0);
  
  // Test if iteration steps get better and better
	QVERIFY( po->minimizationStart(f, startPoint) );
  
	int loopNumber = 0;
	
	double lastY = po->getMinimumPoint().y;
	while(!po->doMinimizationStep())
	{
		lastY = po->getMinimumPoint().y;
		
		loopNumber++;
	}
	
	QCOMPARE(loopNumber,2);
	
	
	po->setTolerance(1.0);
  
  // Test if iteration steps get better and better
	QVERIFY( po->minimizationStart(f, startPoint) );
  
	loopNumber = 0;
	lastY = po->getMinimumPoint().y;
	while(!po->doMinimizationStep())
	{
		lastY = po->getMinimumPoint().y;
		
		loopNumber++;
	}
	
	QCOMPARE(loopNumber,5);

  
	delete f;
	delete bf;
	delete blm;
	delete po;  
}
