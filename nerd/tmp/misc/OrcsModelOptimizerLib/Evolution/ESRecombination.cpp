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


#include "ESRecombination.h"
#include "ESIndividual.h"

namespace nerd {

	QList<OptimizationDouble> ESRecombination::doRecombination(QList<ESIndividual*> parents,
															const parameterType type,
															const ESInformation &information)
{
	QList<OptimizationDouble> recombinedParas;
	
	// return empty list if no parents given
	if(parents.size() <= 0){
		return recombinedParas;
	}
	
	// if only one parent is given --> return its parameters
	if(parents.size() == 1){
		recombinedParas = QList<OptimizationDouble>(getParentParameters(parents.at(0), type));
		return recombinedParas;
	}
	
	// create discrete list of parameters of the parents
	int parameterSize = getParentParameters(parents.at(0), type).size();
	for(int paraIndex = 0; paraIndex < parameterSize; paraIndex++)
	{
		recombinedParas.append(recombineParameter(parents,
																						  type,
																							information,
																						 	paraIndex));
	}
	
	return recombinedParas;
}

QList<OptimizationDouble>
ESRecombination::getParentParameters(ESIndividual *parent,
									const parameterType type)
{
	if(type == STRATEGY){
		return parent->getStrategyParameters();
	}
	else {
		return parent->getObjectParameters();
	}
}

} // namespace nerd
