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



#include "ModularNeuroEvolution1.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
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
#include "SelectionMethod/StochasticUniversalSamplingSelection.h"
#include "SelectionMethod/MultiObjectiveTournamentSelection.h"
#include "Phenotype/IdentityGenotypePhenotypeMapper.h"
#include "NeuralNetworkManipulationChain/ModuleCrossOverOperator.h"
#include "NeuralNetworkManipulationChain/InsertSynapseModularOperator.h"
#include "NeuralNetworkManipulationChain/InsertNeuroModuleOperator.h"
#include "NeuralNetworkManipulationChain/InsertBiasOperator.h"
#include "NeuralNetworkManipulationChain/RemoveBiasOperator.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new ModularNeuroEvolution1.
 */
ModularNeuroEvolution1::ModularNeuroEvolution1(World *world)
{
	NeuralNetworkManipulationChainAlgorithm *evo = 
			new NeuralNetworkManipulationChainAlgorithm("ICONE");
	evo->setRequiredNumberOfParentsPerIndividual(2);

	if(world != 0) {
		world->setEvolutionAlgorithm(evo);
	}
	
	CreateNetworkOperator *createNetOp = 
				new CreateNetworkOperator("CreateInitNetwork");
	ModuleCrossOverOperator *modularCrossover =
				new ModuleCrossOverOperator("ModularCrossover");
	CloneNetworkOperator *cloneNetOp = 
				new CloneNetworkOperator("CloneParent");
	RemoveNeuronOperator *removeNeuronOp = 	
				new RemoveNeuronOperator("RemoveNeurons");
	RemoveSynapseOperator *removeSynapseOp =
				new RemoveSynapseOperator("RemoveSynapse");
	RemoveBiasOperator *removeBiasOp =
				new RemoveBiasOperator("RemoveBias");
	InsertNeuroModuleOperator *insertNeuroModule = 
				new InsertNeuroModuleOperator("InsertNeuroModule");
	InsertNeuronOperator *insertNeuronOp = 
				new InsertNeuronOperator("InsertNeuron");
	InsertSynapseModularOperator *insertSynapseOp = 	
				new InsertSynapseModularOperator("InsertSynapse");
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
	ResolveConstraintsOperator *constraintResolver =
				new ResolveConstraintsOperator("ConstraintResolver");


	evo->addOperator(createNetOp);
	evo->addOperator(modularCrossover);
	evo->addOperator(cloneNetOp);
	evo->addOperator(removeNeuronOp);
	evo->addOperator(removeSynapseOp);
	evo->addOperator(removeBiasOp);
	evo->addOperator(insertNeuroModule);
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
	modularCrossover->getOperatorIndexValue()->set(10);
	cloneNetOp->getOperatorIndexValue()->set(20);
	removeNeuronOp->getOperatorIndexValue()->set(30);
	removeSynapseOp->getOperatorIndexValue()->set(40);
	removeBiasOp->getOperatorIndexValue()->set(50);
	insertNeuroModule->getOperatorIndexValue()->set(60);
	insertNeuronOp->getOperatorIndexValue()->set(70);
	insertSynapseOp->getOperatorIndexValue()->set(80);
	insertBiasOp->getOperatorIndexValue()->set(90);
	initSynapsesOp->getOperatorIndexValue()->set(100);
	initBiasOp->getOperatorIndexValue()->set(120);
	disableSynapseOperator->getOperatorIndexValue()->set(140);
	enableSynapseOperator->getOperatorIndexValue()->set(160);
	changeBias->getOperatorIndexValue()->set(180);
	changeSynapseStrengthOp->getOperatorIndexValue()->set(200);
// 	connectNeuronClassFilter->getOperatorIndexValue()->set(1100);
	constraintResolver->getOperatorIndexValue()->set(1000);
	evo->addOperator(constraintResolver);


	//Add Selection Methods and genotypePhenotypeMapper
	QList<Population*> populations = world->getPopulations();
	for(QListIterator<Population*> i(populations); i.hasNext();) {
		Population *population = i.next();

		//Add Selection Methods.
		TournamentSelectionMethod *tournament = new TournamentSelectionMethod(5);
		StochasticUniversalSamplingSelection *universalSampling = new StochasticUniversalSamplingSelection();
		MultiObjectiveTournamentSelection *multiObjectivTournament = new MultiObjectiveTournamentSelection(5);
		population->addSelectionMethod(tournament);
		population->addSelectionMethod(universalSampling);
		population->addSelectionMethod(multiObjectivTournament);
		tournament->getPopulationProportion()->set(1.0);
		universalSampling->getPopulationProportion()->set(0.0);
		multiObjectivTournament->getPopulationProportion()->set(0.0);

		tournament->setResponsibleFitnessFunction(population->getFitnessFunctions().at(0));
		universalSampling->setResponsibleFitnessFunction(population->getFitnessFunctions().at(0));
		multiObjectivTournament->setResponsibleFitnessFunction(population->getFitnessFunctions().at(0));

		//Add GenotypePhenotypeMapper.
		IdentityGenotypePhenotypeMapper *mapper = new IdentityGenotypePhenotypeMapper();
		population->setGenotypePhenotypeMapper(mapper);
	}
}





}



