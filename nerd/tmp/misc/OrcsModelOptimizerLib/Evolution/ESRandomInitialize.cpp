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


#include "ESRandomInitialize.h"

#include "Value/OptimizationDouble.h"
#include "Math/Random.h"
#include "ESIndividual.h"


namespace nerd {

ESRandomInitialize::ESRandomInitialize(	const QList<ESInitializeValue> &objectParametersRanges, 
																				const QList<ESInitializeValue> &strategyParametersRanges)
	: mObjectParametersRanges(objectParametersRanges),
		mStrategyParametersRanges(strategyParametersRanges)
{}   
		
ESRandomInitialize::ESRandomInitialize(	ESInitializeValue objectParametersRanges,
										 										int numberOfObjectParameters, 
					 															const QList<ESInitializeValue> &strategyParametersRanges)
	: mStrategyParametersRanges(strategyParametersRanges)
{
	mObjectParametersRanges = createValueRangeList(	objectParametersRanges, 
																									numberOfObjectParameters);
} 

ESRandomInitialize::ESRandomInitialize(	const QList<ESInitializeValue> &objectParametersRanges, 
																				ESInitializeValue strategyParametersRanges,
					 															int numberOfStrategyParameters)
	: mObjectParametersRanges(objectParametersRanges)
{
	mStrategyParametersRanges = createValueRangeList(	strategyParametersRanges, 
																										numberOfStrategyParameters);
} 

ESRandomInitialize::ESRandomInitialize(	ESInitializeValue objectParametersRanges,
										 										int numberOfObjectParameters, 
					 															ESInitializeValue strategyParametersRanges,
																				int numberOfStrategyParameters)
{
	mObjectParametersRanges = createValueRangeList(	objectParametersRanges, 
																									numberOfObjectParameters);
	
	mStrategyParametersRanges = createValueRangeList(	strategyParametersRanges, 
																										numberOfStrategyParameters);
}
		
QList<ESInitializeValue> 
ESRandomInitialize::createValueRangeList(	ESInitializeValue parametersRanges,
																					int numberOfParameters)
{
	QList<ESInitializeValue> list;
	
	for(int i = 0 ; i < numberOfParameters; i++) {
		list.append(parametersRanges);
	}
	
	return list;
}		
		
		
QList<ESIndividual*> ESRandomInitialize::createPopulation(int size)
{
	QList<ESIndividual*> population;
			
	for(int i = 0; i < size; i++)
	{
		QList<OptimizationDouble> objectParameters;
		for(int objIndex = 0; objIndex < mObjectParametersRanges.size(); objIndex++)
		{
			OptimizationDouble randomVal;
			randomVal.min = mObjectParametersRanges.at(objIndex).min;
			randomVal.max = mObjectParametersRanges.at(objIndex).max;

			if(mObjectParametersRanges.at(objIndex).type == ESInitializeValue::DEFINED )
			{			
				randomVal.value = mObjectParametersRanges.at(objIndex).value;
			}
			else if(mObjectParametersRanges.at(objIndex).type == ESInitializeValue::RANDOM)
			{
				randomVal.value = Random::nextDoubleBetween(mObjectParametersRanges.at(objIndex).min,
																										mObjectParametersRanges.at(objIndex).max);
			}
			else {
				population.clear();
				return population;
			}
			
			objectParameters.append(randomVal);
		}
		
		QList<OptimizationDouble> strategyParameters;
		for(int stratIndex = 0; stratIndex < mStrategyParametersRanges.size(); stratIndex++)
		{
			OptimizationDouble randomVal;
			randomVal.min = mStrategyParametersRanges.at(stratIndex).min;
			randomVal.max = mStrategyParametersRanges.at(stratIndex).max;

			if(mStrategyParametersRanges.at(stratIndex).type == ESInitializeValue::DEFINED )
			{			
				randomVal.value = mStrategyParametersRanges.at(stratIndex).value;
			}
			else if(mStrategyParametersRanges.at(stratIndex).type == ESInitializeValue::RANDOM)
			{
				randomVal.value = Random::nextDoubleBetween(mStrategyParametersRanges.at(stratIndex).min,
						mStrategyParametersRanges.at(stratIndex).max);
			}
			else {
				population.clear();
				return population;
			}
			
			strategyParameters.append(randomVal);
		}
		
		population.append( new ESIndividual(objectParameters, strategyParameters, 0.0) );
	}
	
	return population;
}


} // namespace nerd
