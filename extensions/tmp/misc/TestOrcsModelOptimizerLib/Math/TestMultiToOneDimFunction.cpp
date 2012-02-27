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

#include "TestMultiToOneDimFunction.h"
#include "TwoDimQuadraticFunction.h"
#include "Math/MultiToOneDimFunction.h"


using namespace std;
using namespace nerd;

void TestMultiToOneDimFunction::testTwoDimQuadraticFunction() 
{
  TwoDimQuadraticFunction *f = new TwoDimQuadraticFunction(); 
  
  MultiToOneDimFunction *mtof = new MultiToOneDimFunction(f);
  
  QVector<double> startPoint (2, 0.0);
  QVector<double> direction (2, 0.0);
  
  direction[0] = 1.0;
  
  mtof->setStartPoint(startPoint);
  mtof->setDirection(direction);
  
  // check if startPoint is correctly represented
  QCOMPARE(mtof->calculate(0), f->calculate(startPoint));
  
  startPoint[0] = 1.0;
  startPoint[1] = 0.0;
  QCOMPARE(mtof->calculate(1.0), f->calculate(startPoint));
  
  direction[0] = 1.0;
  direction[1] = 1.0;
  mtof->setDirection(direction);
  
  startPoint[0] = 1.0;
  startPoint[1] = 1.0;
  QCOMPARE(mtof->calculate(1.0), f->calculate(startPoint));
  
  delete f;
  delete mtof;
	
}
