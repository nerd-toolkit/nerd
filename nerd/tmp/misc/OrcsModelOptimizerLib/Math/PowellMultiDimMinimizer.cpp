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
 

#include "PowellMultiDimMinimizer.h"

#include "MultiDimFunction.h"
#include "MultiToOneDimFunction.h"
#include "OneDimFunctionPoint.h"
#include "MultiDimFunctionPoint.h"
#include "OneDimMinimizer.h"

#include <cmath>
    
namespace nerd 
{    
    
PowellMultiDimMinimizer::PowellMultiDimMinimizer(OneDimMinimizer *oneDimMinimizer)
  : mOneDimMinimizer (oneDimMinimizer),
    mMultiDimFunction(0),
    mOneDimFunction(0),
    mDim(0),
    mBiggestFuncDiff(0.0),
    mBiggestFuncDiffDirection(0),
    mLastFuncVal(0.0),
    mCurrentDirection(0)
{}
    
QString PowellMultiDimMinimizer::getName() const{
  return "PowellMultiDimMinimizer";
}     
    
bool PowellMultiDimMinimizer::minimizationStart(MultiDimFunction *multiDimFunction,
                                                QVector<double> startPoint)
{
  mMultiDimFunction = multiDimFunction;
  mOneDimFunction = new MultiToOneDimFunction(mMultiDimFunction);
  
  mDim = startPoint.size();
  
  mPointLastIteration.x = QVector<double> (startPoint);
  mPointLastIteration.y = mMultiDimFunction->calculate(mPointLastIteration.x);
  
  mMinimumPoint = mPointLastIteration;
  
  // matrix with conjugate search mDirections
  mDirections = QVector< QVector<double> >(mDim,
                                           QVector<double>(mDim, 0.0));
  
  // initialize mDirections as basis vectors
  for(int i = 0; i < mDirections.size(); i++){
    mDirections[i][i] = 1.0;
  }
   
  // initialize variable for extrapolated point
  mPointExtrapolated.x = QVector<double>(mDim,0.0);
  mPointExtrapolated.y = 0.0;
  
  // initialize variable for the current minimized point
  // and use the startpoint as first value
  mPointCurrentMinimized.x = QVector<double>(mPointLastIteration.x);
  mPointCurrentMinimized.y = mPointLastIteration.y;
  
  // initialize variable for the direction of the mimization of one iteration
  mMinimizeDirection = QVector<double>(mDim, 0.0);
  
  mCurrentDirection = 0;

  return true;
}
    
bool PowellMultiDimMinimizer::doMinimizationStep()
{ 
  if(mCurrentDirection == 0)
  {
    mBiggestFuncDiff = 0.0;
    mBiggestFuncDiffDirection = 0;
    mLastFuncVal =mPointLastIteration.y;
  }
  
  // search optimum for every search direction  
  if(mCurrentDirection < mDim)
  {
    // set search direction to current direction and the start point to the last found minimum
    mOneDimFunction->setDirection(mDirections.at(mCurrentDirection));
    mOneDimFunction->setStartPoint(mPointCurrentMinimized.x);
    
    // minimize the function in the defined direction and store the y and x value
    OneDimFunctionPoint pLine = mOneDimMinimizer->minimize(mOneDimFunction);
    mPointCurrentMinimized.x = mOneDimFunction->getMultiDimPoint( pLine.x );
    mPointCurrentMinimized.y = pLine.y;
    
    // get the direction with the biggest function decrease
    if(mLastFuncVal - mPointCurrentMinimized.y > mBiggestFuncDiff)
    {
      mBiggestFuncDiff = mLastFuncVal - mPointCurrentMinimized.y;
      mBiggestFuncDiffDirection = mCurrentDirection;
    }
    
    mLastFuncVal = mPointCurrentMinimized.y;
    
    mMinimumPoint = mPointCurrentMinimized;
    
    mCurrentDirection++;
  }
  else
  {
    // restart next time with the search for the minimum in every conjungate dimension
    mCurrentDirection = 0;
    
    // stop criterion
    // compare the the minized point the last iteration with the minimized point of the current
    // iteration
		
		if(fabs(mPointLastIteration.y - mLastFuncVal) <= mTolerance)
    {
      mMinimumPoint = mPointCurrentMinimized;
      return true;
    }
    
    // create extrapolated point 
    // and direction of current line minimizations
    // and store the point of the current iteration
    for(int i = 0; i < mDim; i++)
    {
      mPointExtrapolated.x[i] = 2.0*mPointCurrentMinimized.x.at(i) - mPointLastIteration.x.at(i);
      mMinimizeDirection[i] = mPointCurrentMinimized.x.at(i) - mPointLastIteration.x.at(i);
    }
    
    // get the value of the extrapolated point
    mPointExtrapolated.y = mMultiDimFunction->calculate(mPointExtrapolated.x);
    
    // change set of conjugate mDirections of necessary
    if(mPointExtrapolated.y < mPointLastIteration.y)
    {
      double t = 2.0 * (mPointLastIteration.y - 2.0 * mPointCurrentMinimized.y + mPointExtrapolated.y)
          * pow(mPointLastIteration.y - mPointCurrentMinimized.y - mBiggestFuncDiff,2.0)
          - mBiggestFuncDiff * pow(mPointLastIteration.y - mPointExtrapolated.y,2.0);
      
      if(t < 0.0)
      {
        mOneDimFunction->setStartPoint(mPointCurrentMinimized.x);
        mOneDimFunction->setDirection(mMinimizeDirection);
        
        OneDimFunctionPoint pLine = mOneDimMinimizer->minimize(mOneDimFunction);
        
        mPointCurrentMinimized.x = mOneDimFunction->getMultiDimPoint( pLine.x );
        mPointCurrentMinimized.y = pLine.y;
        
        mDirections[mBiggestFuncDiffDirection] = mMinimizeDirection;
      }
    }
    
    mPointLastIteration.x = mPointCurrentMinimized.x;
    mPointLastIteration.y = mPointCurrentMinimized.y;
    
    mMinimumPoint = mPointCurrentMinimized;
  }

  return false;
}   

bool PowellMultiDimMinimizer::minimizationEnd()
{
  if(mOneDimFunction != 0)
  {
    delete mOneDimFunction;
    mOneDimFunction = 0;
  }
	
	return true;
}


}
