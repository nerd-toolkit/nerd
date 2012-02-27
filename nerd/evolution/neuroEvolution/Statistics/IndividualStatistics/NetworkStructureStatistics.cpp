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



#include "NetworkStructureStatistics.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Evolution/Individual.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Core/Core.h"
#include "EvolutionConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NetworkStructureStatistics.
 */
NetworkStructureStatistics::NetworkStructureStatistics(Population *population) 
		: IndividualStatistic(population, EvolutionConstants::EVENT_EVO_EVALUATION_COMPLETED) 
{
	mName = "HiddenNeuronStatistic";
}


/**
 * Destructor.
 */
NetworkStructureStatistics::~NetworkStructureStatistics() {
}

void NetworkStructureStatistics::updateStatistic(Individual *individual) {

	if(mMapper == 0) {	
		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
			" GenotypePhenotypeMapper could not be found."));
		return;
	}
	if(!mMapper->createPhenotype(individual)) {
		Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
			" Could not apply GenotypePhenotypeMapper [")
			.append(mMapper->getName()).append("]! [SKIPPING INDIVIDUAL]"));
		return;
	}
	ModularNeuralNetwork *network =  dynamic_cast<ModularNeuralNetwork*>(individual->getPhenotype());
	if(network == 0) {
		return;
	}
	int numberOfNeurons = network->getNeurons().size();
	int numberOfModules = network->getNeuroModules().size();
	int numberOfGroups = network->getNeuronGroups().size();
	int numberOfSynapses = network->getSynapses().size();
	int highestIteration = network->getHighestRequiredIterationNumber();

	individual->setProperty("Neurons", QString::number(numberOfNeurons));
	individual->setProperty("Modules", QString::number(numberOfModules));
	individual->setProperty("Groups", QString::number(numberOfGroups));
	individual->setProperty("Synapses", QString::number(numberOfSynapses));
	individual->setProperty("MaxIterations", QString::number(highestIteration));

}

bool NetworkStructureStatistics::bind() {
	bool ok = IndividualStatistic::bind();
	if(mPopulation == 0) {
		return false;
	}
	mMapper = mPopulation->getGenotypePhenotypeMapper();
	return ok;
}



}



