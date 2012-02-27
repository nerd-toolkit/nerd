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

#include "ENS3EvolutionAlgorithm.h"
#include "NeuralNetworkManipulationChain/NeuralNetworkManipulationChainAlgorithm.h"
#include "NeuralNetworkManipulationChain/CloneNetworkOperator.h"
#include "NeuralNetworkManipulationChain/CreateNetworkOperator.h"
#include "NeuralNetworkManipulationChain/InsertNeuronOperator.h"
#include "NeuralNetworkManipulationChain/InsertSynapseOperator.h"
#include "NeuralNetworkManipulationChain/InitializeSynapsesOperator.h"
#include "NeuralNetworkManipulationChain/InitializeBiasOperator.h"
#include "NeuralNetworkManipulationChain/ChangeBiasOperator.h"
#include "NeuralNetworkManipulationChain/ChangeSynapseStrengthOperator.h"
#include "NeuralNetworkManipulationChain/RemoveNeuronOperator.h"
#include "NeuralNetworkManipulationChain/RemoveSynapseOperator.h"
#include "NeuralNetworkManipulationChain/ResolveConstraintsOperator.h"
#include "NeuralNetworkManipulationChain/ConnectNeuronClassesFilter.h"
#include "NeuralNetworkManipulationChain/EnableSynapseOperator.h"
#include "SelectionMethod/TournamentSelectionMethod.h"
#include "Evolution/Population.h"
#include "Evolution/World.h"
#include "SelectionMethod/PoissonDistributionRanking.h"
#include "Phenotype/IdentityGenotypePhenotypeMapper.h"
#include "NeuralNetworkManipulationChain/InsertBiasOperator.h"
#include "NeuralNetworkManipulationChain/RemoveBiasOperator.h"
#include "NeuralNetworkManipulationChain/InitializeBiasOperator.h"
#include "NeuralNetworkManipulationChain/InitializeSynapsesOperator.h"


namespace nerd {


/**
 * Note. This constructore assumes that world is fully configured.
 * The correct number of populations has already to be set.. 
 * Also a FitnessFunction has to be set for each population before this constructor
 * is called. The first FitnessFunction in the list of FitnessFunctions
 * of each population is used as responsible FitnessFunction for the SelectionMethods.
 *
 * ENS3 consists of:
 * - the ManipulationChainAlgorithm with a number of mutation operators.
 * - the IdentityPhenotypeGenotypeMapper.
 * - a number of compatible SelectionMethods with the PoissionDistribution as default.
 */
ENS3EvolutionAlgorithm::ENS3EvolutionAlgorithm(World *world)
{
	NeuralNetworkManipulationChainAlgorithm *evo = 
			new NeuralNetworkManipulationChainAlgorithm("ENS3");

	if(world != 0) {
		world->setEvolutionAlgorithm(evo);
	}
	
	CreateNetworkOperator *createNetOp = 
				new CreateNetworkOperator("CreateInitNetwork");
	CloneNetworkOperator *cloneNetOp = 
				new CloneNetworkOperator("CloneParent");
	RemoveNeuronOperator *removeNeuronOp = 	
				new RemoveNeuronOperator("RemoveNeurons");
	RemoveSynapseOperator *removeSynapseOp =
				new RemoveSynapseOperator("RemoveSynapse");
	RemoveBiasOperator *removeBiasOp =
				new RemoveBiasOperator("RemoveBias");
	InsertNeuronOperator *insertNeuronOp = 
				new InsertNeuronOperator("InsertNeuron");
	InsertSynapseOperator *insertSynapseOp = 
				new InsertSynapseOperator("InsertSynapse");
	InsertBiasOperator *insertBiasOp = 
				new InsertBiasOperator("InsertBias");
	InitializeSynapsesOperator *initSynapsesOp = 
				new InitializeSynapsesOperator("InitSynapses");
	InitializeBiasOperator *initBiasOp =
				new InitializeBiasOperator("InitBias");
	ChangeBiasOperator *changeBias =
				new ChangeBiasOperator("ChangeBias");
	ChangeSynapseStrengthOperator *changeSynapseStrengthOp =
				new ChangeSynapseStrengthOperator("ChangeSynapseStrength");	
// 	ConnectNeuronClassesFilter *connectNeuronClassFilter =
// 				new ConnectNeuronClassesFilter("ConnectNeuronClasses");
	EnableSynapseOperator *enableSynapseOperator =
				new EnableSynapseOperator("EnableSynapse", true);
	EnableSynapseOperator *disableSynapseOperator =
				new EnableSynapseOperator("DisableSynapse", false);
	

	evo->addOperator(createNetOp);
	evo->addOperator(cloneNetOp);
	evo->addOperator(removeNeuronOp);
	evo->addOperator(removeSynapseOp);
	evo->addOperator(removeBiasOp);
	evo->addOperator(insertNeuronOp);
	evo->addOperator(insertSynapseOp);
	evo->addOperator(insertBiasOp);
	evo->addOperator(initSynapsesOp);
	evo->addOperator(enableSynapseOperator);
	evo->addOperator(disableSynapseOperator);
	evo->addOperator(initBiasOp);
	evo->addOperator(changeBias);
	evo->addOperator(changeSynapseStrengthOp);
// 	evo->addOperator(connectNeuronClassFilter);

	createNetOp->getOperatorIndexValue()->set(0);
	cloneNetOp->getOperatorIndexValue()->set(5);
	removeNeuronOp->getOperatorIndexValue()->set(10);
	removeSynapseOp->getOperatorIndexValue()->set(15);
	removeBiasOp->getOperatorIndexValue()->set(17);
	insertNeuronOp->getOperatorIndexValue()->set(20);
	insertSynapseOp->getOperatorIndexValue()->set(30);
	insertBiasOp->getOperatorIndexValue()->set(40);
	initSynapsesOp->getOperatorIndexValue()->set(50);
	initBiasOp->getOperatorIndexValue()->set(52);
	disableSynapseOperator->getOperatorIndexValue()->set(54);
	enableSynapseOperator->getOperatorIndexValue()->set(55);
	changeBias->getOperatorIndexValue()->set(60);
	changeSynapseStrengthOp->getOperatorIndexValue()->set(70);
// 	connectNeuronClassFilter->getOperatorIndexValue()->set(1100);


	//TODO remove (is not ENS3)
// 	ResolveConstraintsOperator *constraintResolver =
// 				new ResolveConstraintsOperator("ConstraintResolver");
// 	constraintResolver->getOperatorIndexValue()->set(1000);
// 	evo->addOperator(constraintResolver);


	//Add Selection Methods and genotypePhenotypeMapper
	QList<Population*> populations = world->getPopulations();
	for(QListIterator<Population*> i(populations); i.hasNext();) {
		Population *population = i.next();

		//Add Selection Methods.
		TournamentSelectionMethod *tournament = new TournamentSelectionMethod(5);
		PoissonDistributionRanking *poissonDistribution = new PoissonDistributionRanking();
		population->addSelectionMethod(tournament);
		population->addSelectionMethod(poissonDistribution);
		tournament->getPopulationProportion()->set(0.0);
		poissonDistribution->getPopulationProportion()->set(1.0);

		tournament->setResponsibleFitnessFunction(population->getFitnessFunctions().at(0));
		poissonDistribution->setResponsibleFitnessFunction(population->getFitnessFunctions().at(0));

		//Add GenotypePhenotypeMapper.
		IdentityGenotypePhenotypeMapper *mapper = new IdentityGenotypePhenotypeMapper();
		population->setGenotypePhenotypeMapper(mapper);
	}

	
	
	
}

ENS3EvolutionAlgorithm::~ENS3EvolutionAlgorithm() {
}

}


