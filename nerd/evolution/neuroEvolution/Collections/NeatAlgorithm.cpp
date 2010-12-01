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



#include "NeatAlgorithm.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Neat/NeatEvolutionAlgorithm.h"
#include "Neat/NeatSelectionMethod.h"
#include "Neat/NeatGenotypePhenotypeMapper.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NeatAlgorithm.
 */
NeatAlgorithm::NeatAlgorithm(World *world)
{

	if(world == 0) {
		return;
	}

	NeatEvolutionAlgorithm *neatAlg = new NeatEvolutionAlgorithm();

	world->setEvolutionAlgorithm(neatAlg);


	//Add Selection Methods and genotypePhenotypeMapper
	QList<Population*> populations = world->getPopulations();
	for(QListIterator<Population*> i(populations); i.hasNext();) {
		Population *population = i.next();

		//Add Selection Methods.
		NeatSelectionMethod *selection = new NeatSelectionMethod();
		population->addSelectionMethod(selection);
		selection->getPopulationProportion()->set(1.0);

		selection->setResponsibleFitnessFunction(population->getFitnessFunctions().at(0));

		//Add GenotypePhenotypeMapper.
		NeatGenotypePhenotypeMapper *mapper = new NeatGenotypePhenotypeMapper();
		population->setGenotypePhenotypeMapper(mapper);
	}
}





}



