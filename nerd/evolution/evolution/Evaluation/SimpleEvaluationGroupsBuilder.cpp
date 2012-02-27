/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
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

#include "SimpleEvaluationGroupsBuilder.h"
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/Population.h"
#include "Evolution/World.h"
#include "Math/Math.h"

namespace nerd {

SimpleEvaluationGroupsBuilder::SimpleEvaluationGroupsBuilder() {
}

SimpleEvaluationGroupsBuilder::~SimpleEvaluationGroupsBuilder() {
}

void SimpleEvaluationGroupsBuilder::createGroups() {
	mEvaluationGroups.clear();

	World *world = mEvaluationMethod->getOwnerWorld();
	QList<Population*> populations = world->getPopulations();
	
	int maxIndividualNumber = 0;
	for(int i = 0; i < populations.size(); i++) {
		maxIndividualNumber = Math::max(maxIndividualNumber, populations.at(i)->
			getPopulationSizeValue()->get());
	}

	for(int i = 0; i < maxIndividualNumber; i++) {
		QList<Individual*> newGroup;
		for(int j = 0; j < populations.size(); j++) {
			Population *population = populations.at(j);
			if(population->getIndividuals().size() > 0) {
				newGroup.push_back(
					population->getIndividuals().at(i % population->getIndividuals().size()));	
			}
		}
		mEvaluationGroups.push_back(newGroup);
	}

}


QString SimpleEvaluationGroupsBuilder::getName() const {
	return "SimpleEvaluationGroupsBuilder";
}

}
