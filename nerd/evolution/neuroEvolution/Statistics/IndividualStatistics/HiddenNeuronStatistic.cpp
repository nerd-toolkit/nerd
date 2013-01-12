/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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

#include "HiddenNeuronStatistic.h"
#include "Evolution/Individual.h"
#include "Network/NeuralNetwork.h"
#include "Core/Core.h"
#include "EvolutionConstants.h"


namespace nerd {


HiddenNeuronStatistic::HiddenNeuronStatistic(Population *population) 
		: IndividualStatistic(population, EvolutionConstants::EVENT_EVO_EVALUATION_COMPLETED) 
{
	mName = "HiddenNeuronStatistic";
}


HiddenNeuronStatistic::~HiddenNeuronStatistic() {
}

void HiddenNeuronStatistic::updateStatistic(Individual *individual) {

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
	NeuralNetwork *network =  dynamic_cast<NeuralNetwork*>(individual->getPhenotype());
	if(network == 0) {
		return;
	}
	int nrOfNeurons = network->getNeurons().size();
	nrOfNeurons -= network->getOutputNeurons().size();
	nrOfNeurons -= network->getInputNeurons().size();
	individual->setProperty("HiddenNeurons", QString::number(nrOfNeurons));

}

bool HiddenNeuronStatistic::bind() {
	bool ok = IndividualStatistic::bind();
	if(mPopulation == 0) {
		return false;
	}
	mMapper = mPopulation->getGenotypePhenotypeMapper();
	return ok;
}

}
