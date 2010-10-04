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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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
 
#ifndef TEST_TwoDimQuadraticFunctionWithBounds_H
#define TEST_TwoDimQuadraticFunctionWithBounds_H

#include "Math/MultiDimFunction.h"

#include <QVector>
#include <limits>

namespace nerd
{

/**
 * Two Dimensional QuadraticFunction.
 */
class TwoDimQuadraticFunctionWithBounds : public MultiDimFunction
{
  public:
    
    const double minAx; 
    const double minBx;
    const double minY;
    
    TwoDimQuadraticFunctionWithBounds() : 
				minAx(1.1), 
				minBx(0.0), 
				minY(0.25) {}
    
    virtual QString getName() const{
      return "TwoDimQuadraticFunctionWithBounds";
    }
    
    virtual double calculate(QVector<double> x)
    { 
			if(x[0] < -3.0 || x[0] > 3.0){
				return std::numeric_limits<double>::max();
			}
			
			if(x[1] < -3.0 || x[1] > 0.0){
				return std::numeric_limits<double>::max();	
			}
			
      return (x[0]-1.1)*(x[0]-1.1) + (x[1]-0.5)*(x[1]-0.5);
    }
};

}

#endif
