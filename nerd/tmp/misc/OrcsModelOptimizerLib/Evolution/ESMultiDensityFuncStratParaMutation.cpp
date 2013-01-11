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

#include "ESMultiDensityFuncStratParaMutation.h"

#include "ESInformation.h"
#include "Math/Math.h"
#include <cmath>


namespace nerd {

ESMultiDensityFuncStratParaMutation::ESMultiDensityFuncStratParaMutation(double c)
	: mC(c)
{}
		
QList<OptimizationDouble> ESMultiDensityFuncStratParaMutation::doMutation(	
											QList<OptimizationDouble> strategyParameters,
											const ESInformation &information)
{
	QList<OptimizationDouble> mutatedStratParas;
		
	// check if for every object parameter exists one mutation strength
	if(strategyParameters.size() != information.numberOfObjectParameters){
		return mutatedStratParas;
	}
	
	// calculate learing rates
	double tauZero = mC / pow(2.0 * (double)information.numberOfObjectParameters, 0.5);
	double tau = mC / pow(2.0 * pow((double)information.numberOfObjectParameters,0.5), 0.5);
	
	double randomSample = Math::calculateGaussian(0.0, 1.0);	
	double allKoeff = exp(tauZero * randomSample);
				
	for(int i = 0; i < strategyParameters.size(); i++ )
	{
		randomSample = Math::calculateGaussian(0.0, 1.0);	
		
		double newValue = allKoeff * (strategyParameters.at(i).value * exp(tau * randomSample));
		
		OptimizationDouble newParameter = strategyParameters.at(i);
		newParameter.setValueInsideRange(newValue);
	
		mutatedStratParas.append(newParameter);
	}
	
	return mutatedStratParas;
}


} // namespace nerd
