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

#include "ReducedChiSquareErrorFunction.h"

#include "Optimization/TrainSet.h"
#include "Optimization/MotionData.h"
#include "Optimization/StatisticTools.h"
#include "Value/DoubleValue.h"
#include "ModelOptimizer.h"

#include <QString>
#include <limits>

namespace nerd
{

ReducedChiSquareErrorFunction::ReducedChiSquareErrorFunction(TrainSet *trainSet, 
  const QVector<ParameterDescription*> &modelParameter)
  : mTrainSet(trainSet), mModelParameter(modelParameter)
{ }
    
QString ReducedChiSquareErrorFunction::getName() const{
  return "ReducedChiSquareErrorFunction";
}
    
double ReducedChiSquareErrorFunction::calculate(QVector<double> x)
{
  double reducedChiSquare = 0.0;
    
  // set model parameter to new values
  for(int i = 0; i < x.size(); i++){
    ((DoubleValue*)mModelParameter.at(i)->value)->set(x.at(i));    
  }
	
	// check if one of the parameters exceeds the boundarys
	// -> if yes: return maximum error
	for(int i = 0; i < mModelParameter.size(); i++)
	{
		if(	x.at(i) < mModelParameter.at(i)->min->get() 
					|| x.at(i) > mModelParameter.at(i)->max->get())
		{
			return std::numeric_limits<double>::max();
		}
	}
  
  // collect new simulation data
  mTrainSet->collectNewSimData();
  
  double v = 0.0;
  
  for(int motion = 0; motion < mTrainSet->getMotionCount(); motion++)
  {
    MotionData *data = mTrainSet->getMotionData(motion);
    v += data->getChannelCount() * data->getMotionLength();
  }
  v -= mModelParameter.size();
  
  if(v > 0.0)
  {
    double chiSquare = StatisticTools::calculateChiSquareError(mTrainSet);
    
    reducedChiSquare = chiSquare / v;
  }
  
  // return the quadratic error of the data
  return reducedChiSquare;
}

}

