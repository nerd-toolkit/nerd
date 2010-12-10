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


#include "ESDiscreteRecombination.h"

#include "ESIndividual.h"
#include "Math/Random.h"

namespace nerd {

ESDiscreteRecombination::ESDiscreteRecombination()
{}

OptimizationDouble 
ESDiscreteRecombination::recombineParameter(const QList<ESIndividual*> &parents,
																						const parameterType type,
																						const ESInformation &information,
																						const int parameterIndex)
{
	if(parents.size() <= 0){
		OptimizationDouble retval;
		retval.value = 0.0;
		retval.min = 0.0;
		retval.max = 0.0;
		
		return retval;
	}
	
	if(parents.size() == 1){
		return getParentParameters(parents.at(0), type).at(parameterIndex);
	}
	
	int dominantParentIndex = Random::nextInt(parents.size());
	
	const QList<OptimizationDouble> &dominantParentParameters =
			 getParentParameters(parents.at(dominantParentIndex), type);
	
	return dominantParentParameters.at(parameterIndex);	
}


} // namespace nerd
