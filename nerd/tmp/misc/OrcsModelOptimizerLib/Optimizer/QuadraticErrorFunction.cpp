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

#include "QuadraticErrorFunction.h"

#include "Optimization/TrainSet.h"
#include "Optimization/StatisticTools.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "ModelOptimizer.h"

#include <limits>
#include <QString>

namespace nerd
{

QuadraticErrorFunction::QuadraticErrorFunction(TrainSet *trainSet, 
  QVector<ParameterDescription*> modelParameter)
  : mTrainSet(trainSet), mModelParameter(modelParameter)
{ }
    
QString QuadraticErrorFunction::getName() const{
  return "QuadraticErrorFunction";
}
    
double QuadraticErrorFunction::calculate(QVector<double> x)
{  
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
	if(mSimulationRunsCounter != 0)
	{
  	mSimulationRunsCounter->set(mSimulationRunsCounter->get() + 1);
	}
  
  double error = StatisticTools::calculateQuadraticError(mTrainSet);
  
  // return the quadratic error of the data
  return error;
}

}

