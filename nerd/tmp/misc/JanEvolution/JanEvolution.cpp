/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
 *                                                                         *
 *   Nehring library by Jan Nehring (Diploma Thesis)                       *
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


#include "JanEvolution.h"
#include "Core/Core.h"
#include "Value/InterfaceValue.h"
#include "Physics/SimObjectGroup.h"
#include "Physics/PhysicsManager.h"
#include "Collections/ODE_Physics.h"
#include <iostream>
#include "Physics/SimulationEnvironmentManager.h"
#include "PlugIns/SimpleObjectFileParser.h"
#include "PlugIns/KeyFramePlayer.h"
#include "Physics/Physics.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Control/NetworkAgentControlParser.h"
#include "Collections/StandardNeuralNetworkFunctions.h"

#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/World.h"
#include "Evolution/Population.h"
#include "Fitness/Fitness.h"
#include "FitnessFunctions/SurvivalTimeFitnessFunction.h"
#include "Phenotype/IdentityGenotypePhenotypeMapper.h"
#include "SelectionMethod/TournamentSelectionMethod.h"
#include "Collections/ENS3EvolutionAlgorithm.h"
#include "Evaluation/LocalNetworkInSimulationEvaluationMethod.h"
#include "Collections/LoadAndStoreValueManagerPerGeneration.h"
#include "Statistics/Statistics.h"
#include "Statistics/BasicNeuralNetworkStatistics.h"
#include "Collections/StandardIndividualStatistics.h"
#include "Collections/NeuralNetworkIndividualStatistics.h"
#include "SimulationConstants.h"
#include "Collections/NeuroFitnessPrototypes.h" 
#include "Fitness/FitnessManager.h"
#include "Models/Volksbot.h"
#include "Logging/SettingsLogger.h"
#include "Statistics/StatisticsLogger.h"
#include <QDir>
#include "PlugIns/StepsPerSecondCounter.h"
#include "Gui/FitnessPlotter/OnlineFitnessPlotter.h"
#include "Gui/PopulationOverview/MultiplePopulationOverviewWindowWidget.h"
#include "Gui/Control/BoolValueSwitcherAction.h"
#include <QMenu>
#include "EvolutionConstants.h"
#include "Collections/StandardConstraintCollection.h"
#include "PlugIns/CommandLineArgument.h"
#include "PlugIns/PlugInManager.h"
#include "PlugIns/SimObjectGroupPrinter.h"
#include "Collections/TerminateTryCollisionRule.h"
#include "Collections/NetworkEditorCollection.h"
#include "SelectionMethod/PoissonDistributionRanking.h"
#include "PlugIns/SaveBestNetworksHandler.h"
#include "Collections/JanPrototypes.h"


using namespace std;
using namespace nerd;


JanEvolution::JanEvolution()
		: BaseApplication(), mEnableGui(true)
{
}

JanEvolution::~JanEvolution()
{
}

QString JanEvolution::getName() const
{
	return "JanEvolution";
}

bool JanEvolution::runApplication() {
	Physics::getSimulationEnvironmentManager()->createSnapshot();
	Physics::getPhysicsManager()->resetSimulation();

	mRunner->start();

	return true;
}

bool  JanEvolution::setupGui() {
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
		mEnableGui = false;
	}
	if(guiArgument->getParameterValue()->get() != "") {
		mEnableGui = true;
	}

	if(mEnableGui) {
		mGuiMainWindow = new GuiMainWindow(true, true);
		connect(this, SIGNAL(showGui()), mGuiMainWindow, SLOT(showWindow()));

		OnlineFitnessPlotter *plotterButton = new OnlineFitnessPlotter(); 
		mGuiMainWindow->getMenu("Evolution")->addAction(plotterButton->getAction());
		

		MultiplePopulationOverviewWindowWidget *overview = new MultiplePopulationOverviewWindowWidget();
		overview->getAction()->setShortcut(tr("Ctrl+Shift+p"));
		mGuiMainWindow->getMenu("Evolution")->addAction(overview->getAction());
		
		BoolValueSwitcherAction *runEvolutionButton = new BoolValueSwitcherAction("&Run Evolution",
					EvolutionConstants::VALUE_EVO_RUN_EVOLUTION);
		runEvolutionButton->setShortcut(tr("Ctrl+e"));
		mGuiMainWindow->getMenu("Tools")->addAction(runEvolutionButton);

		new NetworkEditorCollection(mGuiMainWindow->getMenu("Tools"), "Network Editor");
	}
	return true;
}


bool JanEvolution::setupApplication()
{
	bool ok = true;
	//install ODE PhysicsLayer
	ODE_Physics();

	//install own prototypes
	JanPrototypes();

	//install file parser
	new SimpleObjectFileParser();

	//Install file interface for HTTPS communication.
	//new LoadAndStoreValueManagerPerGeneration();

	//Install fitness prototypes
	NeuroFitnessPrototypes();
	
	//Install standard NN functions and objects.
	StandardNeuralNetworkFunctions();

	//Install constraint manager.
	StandardConstraintCollection();

	//Install statistics logger to save the current statistics to a file.
	new StatisticsLogger(Evolution::getEvolutionManager()->getEvolutionWorkingDirectory());

	//Install and initialize the settings logger (logging of settings history).
	SettingsLogger *settingsLogger = new SettingsLogger();
	settingsLogger->addValues("(?!.*/Evo/.*/Fitness/.*/Fitness/.*)(?!.*/Performance/.*)/Evo/.*/Algorithm/.*");
	settingsLogger->addValues("(?!.*/Evo/.*/Fitness/.*/Fitness/.*)(?!.*/Performance/.*)/Evo/.*/Pop/.*");
	settingsLogger->addValues("/Evo/.*/Fitness/.*/Fitness/CalculationMode");
	settingsLogger->addValues("/Control/NumberOfSteps");
	settingsLogger->addValues("/Control/NumberOfTries");

	new StepsPerSecondCounter();
	new SimObjectGroupPrinter();
	new SaveBestNetworksHandler(3); 


	if(!buildSimulationModel()) {
		Core::log("JanEvolution: "
				  "Problem while building the simulation model.");
		ok = false;
	}

	if(!buildEvolutionSystem()) {
		Core::log("JanEvolution: "
				  "Problem while building the evolution system.");
		ok = false;
	}

	mRunner = new EvolutionRunner();

	return ok;

}

bool JanEvolution::buildSimulationModel()
{
	Physics::getSimulationEnvironmentManager()->createSnapshot();

	return true;
}

bool JanEvolution::buildEvolutionSystem() {

	World *world = new World("Main");
	Population *population = new Population("HumanoidControllers");
	world->addPopulation(population);

	IdentityGenotypePhenotypeMapper *mapper = new IdentityGenotypePhenotypeMapper();
	population->setGenotypePhenotypeMapper(mapper);

	new ENS3EvolutionAlgorithm(world);

	world->setEvaluationMethod(new LocalNetworkInSimulationEvaluationMethod());

	Evolution::getEvolutionManager()->addEvolutionWorld(world);


	//Add statistics calculator.
	Statistics::getStatisticsManager()->addGenerationStatistics(
		new BasicNeuralNetworkStatistics(*population));



	FitnessFunction *fitness = 0;

	CommandLineArgument *fitnessFunctionArg = new CommandLineArgument(
		"fitness", "fit", "<prototypeName> <fitnessFunctionName>", 
		"Creates a copy of <prototypeName> and uses it for the population"
		" with the given <fitnessFunctionName>.", 2, 0, true);

	QList<QString> fitArguments = fitnessFunctionArg->getEntryParameters(0);
	if(fitArguments.size() > 1) {
		fitness = Fitness::getFitnessManager()
			->createFitnessFunctionFromPrototype(fitArguments.at(0), fitArguments.at(1));

		if(fitness == 0) {
			cerr << "Failed to create FitnessFunction prototype ["
				 << fitArguments.at(0).toStdString().c_str() << endl;
		}
	}

	if(fitness == 0) {
		fitness = Fitness::getFitnessManager()
			->createFitnessFunctionFromPrototype("Script", "Script");
	}

	population->addFitnessFunction(fitness);

	TournamentSelectionMethod *tournament = new TournamentSelectionMethod(5);
	PoissonDistributionRanking *poissonDistribution = new PoissonDistributionRanking();
	population->addSelectionMethod(tournament);
	population->addSelectionMethod(poissonDistribution);
	tournament->setResponsibleFitnessFunction(fitness);
	poissonDistribution->setResponsibleFitnessFunction(fitness);
	poissonDistribution->getPopulationProportion()->set(0.0);

	//add individual statistics
	StandardIndividualStatistics indStatistics(population); 
	NeuralNetworkIndividualStatistics neuroStatistics(population);

	return true;
}


