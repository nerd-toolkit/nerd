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

#include "ESOneFifthRuleStratParaMutation.h"

#include "ESInformation.h"


namespace nerd {

ESOneFifthRuleStratParaMutation::ESOneFifthRuleStratParaMutation(int checkInterval, double a)
	: mCheckInterval(checkInterval),
	  mA(a),
		mSucessfulOffspringsInInterval(0),
		mNextGenerationOffspingCount(0)
{}
		
QList<OptimizationDouble> ESOneFifthRuleStratParaMutation::doMutation(	
													QList<OptimizationDouble> strategyParameters,
													const ESInformation &information)
{
	QList<OptimizationDouble> mutatedStratParas;
		
	// check if one mutation strength parameter exists
	if(strategyParameters.size() != 1){
		return mutatedStratParas;
	}
	
	// count number of succesful offsprings during the current interval
	if(mNextGenerationOffspingCount == information.currentGeneration) 
	{
		// reset count for next intervall
		if((information.currentGeneration - 1) % mCheckInterval == 0){
			mSucessfulOffspringsInInterval = 0;
		}
		
		mSucessfulOffspringsInInterval += information.numberOfLastSuccessfulIndividuals;
		mNextGenerationOffspingCount++;
	}
	
	double newStdDeviation = strategyParameters.at(0).value;
	
	if(information.currentGeneration > 0 &&
		 information.currentGeneration % mCheckInterval == 0)
	{
		// calculate rate of successful to all offsprings during the interval
		double Ps = (double)mSucessfulOffspringsInInterval / ((double)information.lambda * (double)mCheckInterval);
		
		if(Ps > 0.2) {
			newStdDeviation = strategyParameters.at(0).value / mA;
		}
		else if(Ps < 0.2) {
			newStdDeviation = strategyParameters.at(0).value * mA;
		}
	}
	
	OptimizationDouble newParameter = strategyParameters.at(0);
	newParameter.setValueInsideRange(newStdDeviation);
		
	mutatedStratParas.append(newParameter);
		
	return mutatedStratParas;
}


} // namespace nerd
