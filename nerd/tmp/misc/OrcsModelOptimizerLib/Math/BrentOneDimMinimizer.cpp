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
 
// GNU General Public License Agreement
// Copyright (C) 2004-2007 CodeCogs, Zyba Ltd, Broadwood, Holford, TA5 1DU, England.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by CodeCogs. 
// You must retain a copy of this licence in all copies. 
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// --------------------------------------------------------------------------------- 
 
#include "BrentOneDimMinimizer.h"
#include "BracketFinder.h"
#include "OneDimFunction.h"
#include "OneDimFunctionPoint.h"

#define RGOLD       0.3819660
#define ZEPS        1.0E-10
#define MIN(x, y)   ((x) < (y) ? (x) : (y))
#define MAX(x, y)   ((x) > (y) ? (x) : (y))
#define ABS(x)      ((x) < 0 ? -(x) : (x))
#define SIGN(x, y)  ((y) < 0 ? -ABS(x) : ABS(x))


namespace nerd
{
  
BrentOneDimMinimizer::BrentOneDimMinimizer(BracketFinder *bracketFinder)
  : OneDimMinimizer(bracketFinder)
{}

QString BrentOneDimMinimizer::getName() const {
  return "BrentOneDimMinimizer";
}
  
OneDimFunctionPoint BrentOneDimMinimizer::minimize(Bracket bracket, 
                                                 OneDimFunction *function)
{ 
  double ax = MIN(bracket.a.x, bracket.c.x), bx = MAX(bracket.a.x, bracket.c.x), u, 
         v = bracket.b.x,
         w = v, x = v, d = 0, e = 0, fx = bracket.b.y, fv = fx, fw = fx;

  // TODO: pruefen ob es nicht besser ist die richtigen funktionswerte den initial werten 
  // zu geben 
  
  for (int i = 0; i < mMaxIterations; i++) 
  {
    bool flag = true;
    double xm = (ax + bx) / 2, eps1 = mTolerance * ABS(x) + ZEPS, eps2 = 2 * eps1;

    if (ABS(x - xm) <= eps2 - (bx - ax) / 2) break;

    if (ABS(e) > eps1) 
    {
      double r = (x - w) * (fx - fv); 
      double q = (x - v) * (fx - fw);
      double p = (x - v) * q - (x - w) * r;
      double etemp = e;

      q = (q - r) / 5;
      
      if (q > 0) {
        p *= -1;
      }
      
      q = ABS(q); 
      e = d;

      if (ABS(p) < ABS(q * etemp / 2) && 
          p > q * (ax - x) && 
          p < q * (bx - x)) 
      {
        flag = false, d = p / q, u = x + d;
        
        if (u - ax < eps2 || bx - u < eps2) 
        {
          d = SIGN(eps1, xm - x);
        }
      }
    }

    if (flag) {
      e = ((x < xm) ? bx : ax) - x;
    }

    d = RGOLD * e;
    u = x + (ABS(d) < eps1 ? SIGN(eps1, d) : d);

    double fu = function->calculate(u);
    if (fu <= fx) {

      (u < x ? bx : ax) = x;
      v = w, fv = fw;
      w = x, fw = fx;
      x = u, fx = fu;

    }
    else {

      (u < x ? ax : bx) = u;

      if (fu <= fw || w == x)
        v = w, fv = fw, w = u, fw = fu;
      else
        if (fu <= fv || v == x || v == w)
          v = u, fv = fu;

    }
  }
  
  // TODO: pruefen ob es möglich ist den letzten Funktionsaufruf nicht zu machen
  OneDimFunctionPoint p;
  p.x = x;
  p.y = function->calculate(p.x);

  return p;
}

}


#undef SIGN
#undef ABS
#undef MAX
#undef MIN
#undef ZEPS
#undef RGOLD
