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



#include "NeuroEvolutionSelector.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/World.h"
#include "Evolution/Population.h"
#include "Fitness/Fitness.h"
#include "FitnessFunctions/SurvivalTimeFitnessFunction.h"
#include "Phenotype/IdentityGenotypePhenotypeMapper.h"
#include "SelectionMethod/TournamentSelectionMethod.h"
#include "Collections/ENS3EvolutionAlgorithm.h"
#include "Collections/StandardIndividualStatistics.h"
#include "Collections/NeuralNetworkIndividualStatistics.h"
#include "Fitness/FitnessManager.h"
#include "PlugIns/CommandLineArgument.h"
#include "PlugIns/PlugInManager.h"
#include "Statistics/Statistics.h"
#include "Statistics/BasicNeuralNetworkStatistics.h"
#include "Collections/NeatAlgorithm.h"
#include "Collections/ModularNeuroEvolution1.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NeuroEvolutionSelector.
 */
NeuroEvolutionSelector::NeuroEvolutionSelector(EvaluationMethod *evaluationMethod)
{

	//TODO Currently only a single world with a single population is supported.

	//Select evolution algorithm
	CommandLineArgument *evoAlgorithmArg = new CommandLineArgument(
		"evolutionAlgorithm", "evo", "<algorithmName> <controlledAgent>", 
		"Uses the evolution algorithm with the given <algorithmName> "
		"to evolve controllers for agent <controlledAgent>.", 1, 1, true);

	CommandLineArgument *fitnessFunctionArg = new CommandLineArgument(
			"fitness", "fit", "<prototypeName> <fitnessFunctionName> <population>", 
			"Creates a copy of <prototypeName> and uses it "
			" with the given <fitnessFunctionName>. The <population>"
			" is a number and specifies the population in which the fitness function is used.", 
			2, 1, true);

	//ensure that there is at least on world, even if not specified via command line.
	bool createDefaultWorld = false;
	if(evoAlgorithmArg->getNumberOfEntries() == 0) {
		createDefaultWorld = true;
	}
	
	int scriptFitnessCounter = 0;

	for(int evoCount = 0; evoCount < evoAlgorithmArg->getNumberOfEntries() || createDefaultWorld; ++evoCount) {

		QString worldName = "Main";
		if(evoCount > 0) {
			worldName = "Pop" + QString::number(evoCount);
		}
		World *world = new World(worldName);
		QString popName = "Controllers";
		if(evoCount > 0) {
			popName = "Controllers" + QString::number(evoCount);
		}
		Population *population = new Population(popName, world);
		world->addPopulation(population);
	
		if(evoCount == 0) {
			world->setEvaluationMethod(evaluationMethod);
		}
		else {
			world->setEvaluationMethod(0);
		}
	
		Evolution::getEvolutionManager()->addEvolutionWorld(world);
	
	
		//Install Fitness
		FitnessFunction *fitness = 0;

	
		for(int i = 0; i < fitnessFunctionArg->getNumberOfEntries(); ++i) {
			QList<QString> fitArguments = fitnessFunctionArg->getEntryParameters(i);
			if((evoCount == 0 && fitArguments.size() <= 2)
			  || (fitArguments.size() > 2 
					&& fitArguments.at(2).toInt() == evoCount))
			{
				if(fitArguments.size() > 1) {
					fitness = Fitness::getFitnessManager()
						->createFitnessFunctionFromPrototype(fitArguments.at(0), fitArguments.at(1));
			
					if(fitness == 0) {
						cerr << "Failed to create FitnessFunction prototype ["
							<< fitArguments.at(0).toStdString().c_str() << endl;
					}
				}
				if(fitness != 0) {
					population->addFitnessFunction(fitness);
				}
			}
		}
		if(fitness == 0) {
			QString scriptName = "Script";
			if(scriptFitnessCounter > 0) {
				scriptName += QString::number(scriptFitnessCounter);
			}
			scriptFitnessCounter++;
		
			fitness = Fitness::getFitnessManager()
				->createFitnessFunctionFromPrototype("Script", scriptName);
			population->addFitnessFunction(fitness);
		}
	
		QList<QString> evoArguments = evoAlgorithmArg->getEntryParameters(evoCount);
		if(evoArguments.size() >= 1 && evoArguments.at(0).trimmed() == "ens3")
		{
			ENS3EvolutionAlgorithm evo(world);
		}
		else if(evoArguments.size() >= 1 && evoArguments.at(0).trimmed() == "neat") {
			NeatAlgorithm evo(world);
		}
		else if(evoArguments.size() < 1 || (evoArguments.size() >= 1 
				&& (evoArguments.at(0).trimmed() == "mens" || evoArguments.at(0).trimmed() == "icone")))
		{
			ModularNeuroEvolution1 evo(world);
		}
		if(evoArguments.size() >= 2) {
			world->getControlleAgentValue()->set(evoArguments.at(1));
		}

		//add individual statistics
		StandardIndividualStatistics indStatistics(population); 
		NeuralNetworkIndividualStatistics neuroStatistics(population);
		
		//Add neuralNetworkStatustic calculator.
		Statistics::getStatisticsManager()->addGenerationStatistics(
			new BasicNeuralNetworkStatistics(*population));

		createDefaultWorld = false;
	}

}


/**
 * Destructor.
 */
NeuroEvolutionSelector::~NeuroEvolutionSelector() {
}




}



