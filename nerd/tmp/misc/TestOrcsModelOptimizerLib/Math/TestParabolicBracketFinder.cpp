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

#include "TestParabolicBracketFinder.h"
#include "QuadraticFunction.h"
#include "Math/ParabolicBracketFinder.h"


using namespace std;
using namespace nerd;


//Chris
void TestParabolicBracketFinder::testBracketing() 
{
  QuadraticFunction *f = new QuadraticFunction(); 
  
  ParabolicBracketFinder *bf = new ParabolicBracketFinder();
  
  Bracket bracket = bf->findBracket(0.0, 0.1, f);
  
  // Test order of x-points
  QVERIFY(bracket.a.x <  bracket.b.x);
  QVERIFY(bracket.b.x <  bracket.c.x);
  
  // Test functions values
  QVERIFY(bracket.a.y >=  bracket.b.y);
  QVERIFY(bracket.c.y >=  bracket.b.y);
  
  QCOMPARE(bracket.a.y, f->calculate(bracket.a.x));
  QCOMPARE(bracket.b.y, f->calculate(bracket.b.x));
  QCOMPARE(bracket.c.y, f->calculate(bracket.c.x));
  
  
  
  bracket = bf->findBracket(0.0, 2.0, f);
  
  // Test order of x-points
  QVERIFY(bracket.a.x <  bracket.b.x);
  QVERIFY(bracket.b.x <  bracket.c.x);
  
  // Test functions values
  QVERIFY(bracket.a.y >=  bracket.b.y);
  QVERIFY(bracket.c.y >=  bracket.b.y);
  
  QCOMPARE(bracket.a.y, f->calculate(bracket.a.x));
  QCOMPARE(bracket.b.y, f->calculate(bracket.b.x));
  QCOMPARE(bracket.c.y, f->calculate(bracket.c.x));
  
  bracket = bf->findBracket(5.0, 6.1, f);
  
  // Test order of x-points
  QVERIFY(bracket.a.x >  bracket.b.x);
  QVERIFY(bracket.b.x >  bracket.c.x);
  
  // Test functions values
  QVERIFY(bracket.a.y >=  bracket.b.y);
  QVERIFY(bracket.c.y >=  bracket.b.y);
  
  QCOMPARE(bracket.a.y, f->calculate(bracket.a.x));
  QCOMPARE(bracket.b.y, f->calculate(bracket.b.x));
  QCOMPARE(bracket.c.y, f->calculate(bracket.c.x));
  
  delete f;
  delete bf;
	
}
