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


#include "ESSingleDensityFuncObjParaMutation.h"
#include "Math/Math.h"

namespace nerd {

ESSingleDensityFuncObjParaMutation::ESSingleDensityFuncObjParaMutation()
{}

QList<OptimizationDouble> ESSingleDensityFuncObjParaMutation::doMutation(	
					QList<OptimizationDouble> objectParameters, 
					QList<OptimizationDouble> strategyParameters,
					const ESInformation &information)
{
	QList<OptimizationDouble> mutatedObjParas;
	
	if(objectParameters.size() <= 0){
		return mutatedObjParas;
	}
	
	// check if one mutation strength parameter exists
	if(strategyParameters.size() != 1){
		return mutatedObjParas;
	}
	
	double mutationStrength = strategyParameters.at(0).value;
	
	for(int i = 0; i < objectParameters.size(); i++)
	{
		double randomSample = Math::calculateGaussian(0.0, 1.0);	
		double newValue = objectParameters.at(i).value + (mutationStrength * randomSample);
		
		OptimizationDouble newParameter = objectParameters.at(i);
		newParameter.setValueInsideRange(newValue);
		
		mutatedObjParas.append(newParameter);
	}

	return mutatedObjParas;
}



} // namespace nerd
