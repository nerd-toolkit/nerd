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
 

#include "ParabolicBracketFinder.h"
#include "OneDimFunctionPoint.h"
#include "OneDimFunction.h"


#define MAX(x, y)   ((x) > (y) ? (x) : (y))
#define ABS(x)      ((x) < 0 ? -(x) : (x))
#define SIGN(x, y)  ((y) < 0 ? -ABS(x) : ABS(x))

namespace nerd
{

ParabolicBracketFinder::ParabolicBracketFinder()
{}
  
QString ParabolicBracketFinder::getName() const{
  return "ParabolicBracketFinder";
}


    
Bracket ParabolicBracketFinder::findBracket(double a, double b, OneDimFunction *function)
{
  double ax = 0.0,bx = 0.0,cx = 0.0,fa = 0.0,fb = 0.0,fc = 0.0;
  const double GOLD=1.618034,GLIMIT=100.0,TINY=1.0e-20;
  ax=a; 
  bx=b;
  double fu;
  fa=function->calculate(ax);
  fb=function->calculate(bx);
  
  if (fb > fa) 
  { 
   double tmp = 0.0;
    
   tmp =ax;
   ax = bx;
   bx = tmp;
    
   tmp =fb;
   fb = fa;
   fa = tmp;
  }
              
  cx=bx+GOLD*(bx-ax);
  fc=function->calculate(cx);
                  
  while (fb > fc) { 
                    
    double r=(bx-ax)*(fb-fc); 
    
    double q=(bx-cx)*(fb-fa);
                            
    double u=bx-((bx-cx)*q-(bx-ax)*r)/ (2.0*SIGN(MAX(ABS(q-r),TINY),q-r));
                            
    double ulim=bx+GLIMIT*(cx-bx);
    
    if ((bx-u)*(u-cx) > 0.0) 
    { 
      fu=function->calculate(u);
      if (fu < fc) 
      { 
        ax=bx;
        bx=u;
        fa=fb;
        fb=fu;
        break;
      } else if (fu > fb) 
      {
        cx=u;
        fc=fu;
        break;
      }
  
      u=cx+GOLD*(cx-bx); 
      fu=function->calculate(u);
      
    } else if ((cx-u)*(u-ulim) > 0.0) { 
      fu=function->calculate(u);
      
      if (fu < fc) 
      {
        shft3(bx,cx,u,u+GOLD*(u-cx));
        shft3(fb,fc,fu,function->calculate(u));
      }
    } else if ((u-ulim)*(ulim-cx) >= 0.0) 
    { 
      u=ulim;
      fu=function->calculate(u);
    } else { 
      u=cx+GOLD*(cx-bx);
      fu=function->calculate(u);
    }
    
    shft3(ax,bx,cx,u); 
    shft3(fa,fb,fc,fu);
    
  }
  Bracket retval;
  retval.a.x = ax;
  retval.b.x = bx;
  retval.c.x = cx;
  retval.a.y = fa;
  retval.b.y = fb;
  retval.c.y = fc;
  
  return retval;
}

inline void ParabolicBracketFinder::shft2(double &a, double &b, const double c)
{
  a=b;
  b=c;
}

inline void ParabolicBracketFinder::shft3(double &a, double &b, double &c, const double d)
{
  a=b;
  b=c;
  c=d;
}

inline void ParabolicBracketFinder::mov3(double &a, 
                                         double &b, 
                                         double &c, 
                                         const double d, 
                                         const double e,
                                         const double f)
{
  a=d; 
  b=e; 
  c=f;
}

}

#undef SIGN
#undef MAX
#undef ABS
