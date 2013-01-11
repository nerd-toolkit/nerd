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


#include "ESPlusSelection.h"

#include "MuSlashRhoLambdaES.h"
#include "ESIndividual.h"

namespace nerd {
	
	
ESPlusSelection::ESPlusSelection() {
}


QList<ESIndividual*> ESPlusSelection::doSelection(const QList<ESIndividual*> &parents, 
																						 			const QList<ESIndividual*> &offsprings, 
											 											 			const int mu,
																					  			const ESInformation &information )
{
	QList<ESIndividual*> selectedIndividuals;
	
	// check if enough individuals are existing
	if(parents.size() + offsprings.size() < mu){
		return selectedIndividuals;
	}
	
	int parentsIndex = 0;
	int offspringsIndex = 0;
	for(int i = 0; i < mu; i++ )
	{
		if(parentsIndex >= parents.size())
		{
			selectedIndividuals.append(offsprings.at(offspringsIndex));
			offspringsIndex++;
			continue;
		}
		
		if(offspringsIndex >= offsprings.size())
		{
			selectedIndividuals.append(parents.at(parentsIndex));
			parentsIndex++;
			continue;
		}
		
		if(parents.at(parentsIndex)->getFitness() < offsprings.at(offspringsIndex)->getFitness())
		{
			selectedIndividuals.append(offsprings.at(offspringsIndex));
			offspringsIndex++;
		}
		else
		{
			selectedIndividuals.append(parents.at(parentsIndex));
			parentsIndex++;			
		}
	}
	
	return selectedIndividuals;
}



} // namespace nerd
