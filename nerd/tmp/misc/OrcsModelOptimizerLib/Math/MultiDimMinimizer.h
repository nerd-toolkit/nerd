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
 
#ifndef NERD_MATH_MULTIDIMMINIMIZER_H
#define NERD_MATH_MULTIDIMMINIMIZER_H

#include "Core/Object.h"

#include <QVector>
#include "MultiDimFunctionPoint.h"


namespace nerd{ class MultiDimFunction; }

namespace nerd
{

/**
 * MultiDimMinizer
 * 
 * Interface for a multi dimensional function minimizer.
 */
class MultiDimMinimizer : public Object
{
  public:
    MultiDimMinimizer();
    
    MultiDimFunctionPoint minimize(MultiDimFunction *function,
                                   QVector<double> startPoint,
                                   int maxIterations = 100);
    
    MultiDimFunctionPoint getMinimumPoint();  
    
    virtual QString getName() const = 0;
    
    double getTolerance() const;
    void setTolerance( double tolerance );
    
    virtual bool minimizationStart(MultiDimFunction *function, 
                                   QVector<double> startPoint) = 0;
    
    virtual bool doMinimizationStep() = 0;
    
    virtual bool minimizationEnd() = 0;
    
  protected:
    
    /**
     * Minimum point which was found by the last minimization step.
     */
    MultiDimFunctionPoint mMinimumPoint;
    
    double mTolerance;
};

}

#endif

