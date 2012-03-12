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



#include "NerdMultiCoreNeuroEvoApplication.h"
#include "Core/Core.h"
#include "Value/InterfaceValue.h"
#include <iostream>
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Control/NetworkAgentControlParser.h"
#include "Collections/StandardNeuralNetworkFunctions.h"
#include "Collections/StandardIndividualStatistics.h"
#include "Collections/NeuralNetworkIndividualStatistics.h"
#include "Fitness/FitnessManager.h"
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/World.h"
#include "Evolution/Population.h"
#include "Fitness/Fitness.h"
#include "Phenotype/IdentityGenotypePhenotypeMapper.h"
#include "SelectionMethod/TournamentSelectionMethod.h"
#include "Collections/ENS3EvolutionAlgorithm.h"
#include "ClusterEvaluation/ClusterNetworkInSimEvaluationMethod.h"
#include "Evaluation/SimpleEvaluationGroupsBuilder.h"
#include "EvolutionConstants.h"
#include "NerdConstants.h"
#include "Collections/NeuroFitnessPrototypes.h"
#include "Collections/LoadAndStoreValueManagerPerGeneration.h"
#include "Statistics/Statistics.h"
#include "PlugIns/CommandLineArgument.h"
#include <QApplication>
#include "Statistics/Statistics.h"
#include "Statistics/BasicNeuralNetworkStatistics.h"
#include "Statistics/StatisticsLogger.h"
#include "Gui/FitnessPlotter/OnlineFitnessPlotter.h"
#include "Collections/StandardConstraintCollection.h"
#include "SelectionMethod/PoissonDistributionRanking.h"
#include "Collections/NeuroModuleCollection.h"
#include "Collections/NeuroEvolutionSelector.h"
#include "PlugIns/SaveBestNetworksHandler.h"
#include "PlugIns/WorkspaceCleaner.h"
#include "PlugIns/EvolutionTerminationTrigger.h"
#include "Collections/EvolutionPropertyPanelCollection.h"
#include "Logging/SettingsLogger.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NerdMultiCoreNeuroEvoApplication.
 */
NerdMultiCoreNeuroEvoApplication::NerdMultiCoreNeuroEvoApplication()
	: BaseApplication(), mRunner(0), mMainGui(0)
{
}


/**
 * Destructor.
 */
NerdMultiCoreNeuroEvoApplication::~NerdMultiCoreNeuroEvoApplication() {
}

QString NerdMultiCoreNeuroEvoApplication::getName() const {
	return "NerdMultiCoreNeuroEvoApplication";
}

bool NerdMultiCoreNeuroEvoApplication::setupGui() {
	bool enableGui = true;

	CommandLineArgument *guiArgument = 
			new CommandLineArgument(
				"enableGui", "gui", "",
				"Starts the application with graphical user interface.",
				0, 0,
				true);
	CommandLineArgument *noGuiArgument = 
			new CommandLineArgument(
				"disableGui", "nogui", "",
				"Starts the application without graphical user interface.",
				0, 0,
				true);

	if(noGuiArgument->getParameterValue()->get() != "") {
		enableGui = false;
	}
	if(guiArgument->getParameterValue()->get() != "") {
		enableGui = true;
	}

	if(enableGui) {
		mMainGui = new SimpleEvolutionMainWindow(true);
		OnlineFitnessPlotter *plotterButton = new OnlineFitnessPlotter();
		mMainGui->getMenu("Evolution")->addAction(plotterButton->getAction());

		new EvolutionPropertyPanelCollection(mMainGui->getMenu("Evolution"), 
											"Evolution Parameters", true);

		connect(this, SIGNAL(showGui()), mMainGui, SLOT(show()));
	}
	return true;
}

bool NerdMultiCoreNeuroEvoApplication::setupApplication() {
	bool ok = true;
	StandardNeuralNetworkFunctions();

	LoadAndStoreValueManagerPerGeneration();
	Fitness::install(); 
	
	//disable fitness function execution. Fitness is evaluated only in the evaluation application.
	Fitness::getFitnessManager()->enableFitnessFunctionExecution(false); 

	//Install neuro modules
	NeuroModuleCollection();

	//Install constraint manager.
	StandardConstraintCollection();

	NeuroFitnessPrototypes();

// 	Statistics::install();
	SettingsLogger *settingsLogger = new SettingsLogger();
// 	settingsLogger->addValues("(?!.*/Evo/.*/Fitness/.*/Fitness/.*)(?!.*/Performance/.*)/Evo/Algorithm/.*");
// 	settingsLogger->addValues("(?!.*/Evo/.*/Fitness/.*/Fitness/.*)(?!.*/Performance/.*)/Evo/Pop/.*");
	settingsLogger->addValues("(?!.*/Evo/.*/Fitness/.*/Fitness/.*)(?!.*/Performance/.*)/Evo/.*/Algorithm/.*");
	settingsLogger->addValues("(?!.*/Evo/.*/Fitness/.*/Fitness/.*)(?!.*/Performance/.*)/Evo/.*/Pop/.*");
	settingsLogger->addValues("/Evo/.*/Fitness/.*/Fitness/CalculationMode");
	settingsLogger->addValues("/Control/NumberOfSteps");
	settingsLogger->addValues("/Control/NumberOfTries");

	//Install statistics logger to save the current statistics to a file.
	new StatisticsLogger(Evolution::getEvolutionManager()->getEvolutionWorkingDirectoryValue());

	if(!buildEvolutionSystem()) {
		Core::log("NerdClusterNeuroEvoApplication: Problem while "
			"building the 	evolution system.");
		ok = false;
	}

	mRunner = new EvolutionRunner();

	new SaveBestNetworksHandler(2);
	//new WorkspaceCleaner(EvolutionConstants::EVENT_EVO_CLUSTER_JOB_SUBMITTED);
	new WorkspaceCleaner(EvolutionConstants::EVENT_EVO_EVALUATION_STARTED);
	new EvolutionTerminationTrigger();

	//enable toggle mode for preview windows.
	new CommandLineArgument("toggle", "toggle", "", 
						"Makes the OpenGL windows toggle.", 0, 0, true, false);

	return ok;
}

bool NerdMultiCoreNeuroEvoApplication::runApplication() {
	mRunner->start();

	return true;
}

bool NerdMultiCoreNeuroEvoApplication::buildEvolutionSystem() {
	Evolution::install();

	EvaluationMethod *evalMethod = new ClusterNetworkInSimEvaluationMethod("/Evaluation");
	evalMethod->setEvaluationGroupsBuilder(new SimpleEvaluationGroupsBuilder());

	new NeuroEvolutionSelector(evalMethod);

	return true;
}



}



