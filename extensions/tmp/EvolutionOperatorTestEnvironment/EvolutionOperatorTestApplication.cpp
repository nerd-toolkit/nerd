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


#include "EvolutionOperatorTestApplication.h"
#include "Core/Core.h"
#include "Value/InterfaceValue.h"
#include "Physics/SimObjectGroup.h"
#include "Physics/PhysicsManager.h"
#include "Collections/ODE_Physics.h"
#include <iostream>
#include "Physics/SimulationEnvironmentManager.h"
#include "PlugIns/SimpleObjectFileParser.h"
#include "Physics/Physics.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Control/NetworkAgentControlParser.h"
#include "Collections/StandardNeuralNetworkFunctions.h"
#include "Statistics/Statistics.h"
#include "Statistics/BasicNeuralNetworkStatistics.h"
#include "Collections/StandardIndividualStatistics.h"
#include "Collections/NeuralNetworkIndividualStatistics.h"
#include "SimulationConstants.h"
#include "Collections/NeuroFitnessPrototypes.h"
#include "Fitness/FitnessManager.h"
#include "Logging/SettingsLogger.h"
#include "Statistics/StatisticsLogger.h"
#include <QDir>
#include "PlugIns/FramesPerSecondCounter.h"
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
#include "Collections/EvolutionSimulationPrototypes.h"
#include "Collections/NeuroModuleCollection.h"
#include "Collections/NeuroEvolutionSelector.h"
#include "Collections/YarsCommunication.h"
#include "PlugIns/NeuralNetworkAttributes.h"
#include "Evolution/Evolution.h"
#include "Evaluation/LocalNetworkInSimulationEvaluationMethod.h"
#include "FirstParentsTestSelectionMethod.h"



using namespace std;
using namespace nerd;


EvolutionOperatorTestApplication::EvolutionOperatorTestApplication()
		: BaseApplication(), mEnableGui(true)
{
}

EvolutionOperatorTestApplication::~EvolutionOperatorTestApplication()
{
}

QString EvolutionOperatorTestApplication::getName() const
{
	return "EvolutionOperatorTestApplication";
}

bool EvolutionOperatorTestApplication::runApplication() {
	Physics::getSimulationEnvironmentManager()->createSnapshot();
	Physics::getPhysicsManager()->resetSimulation();

	mRunner->start();

	return true;
}

bool  EvolutionOperatorTestApplication::setupGui() {
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


bool EvolutionOperatorTestApplication::setupApplication()
{
	bool ok = true;
	

	//Choose Physics Engine (or external Yars simulator)
	CommandLineArgument *physicsArg = new CommandLineArgument("physics", "p", "<physicsLibrary>",
			"Uses <physicsLibrary> as physics engine.\n"
			"     Currently there are [ode, yars].", 1,0, true);
	if(physicsArg->getNumberOfEntries() != 0 && !physicsArg->getEntryParameters(0).empty()
			&& physicsArg->getEntryParameters(0).at(0).trimmed() == "yars")
	{
		YarsCommunication();
	}
	else {
		//install ODE PhysicsLayer
		ODE_Physics();
	}


	EvolutionSimulationPrototypes();

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

	//Install neuro modules
	NeuroModuleCollection();

	//Install network attribute characterizer
	new NeuralNetworkAttributes();

	//Install statistics logger to save the current statistics to a file.
	new StatisticsLogger(Evolution::getEvolutionManager()->getEvolutionWorkingDirectory());

	//Install and initialize the settings logger (logging of settings history).
	SettingsLogger *settingsLogger = new SettingsLogger();
	settingsLogger->addValues(".*Evo/.*Algorithm.*");
	settingsLogger->addValues(".*Evo/.*Selection.*");
	settingsLogger->addValues(".*Evo/.*Fitness.*");

	new FramesPerSecondCounter();
	new SimObjectGroupPrinter();
	new SaveBestNetworksHandler(3); 


	if(!buildSimulationModel()) {
		Core::log("LocalNNSimulatorEvolutionApplication: "
				  "Problem while building the simulation model.");
		ok = false;
	}

	if(!buildEvolutionSystem()) {
		Core::log("LocalNNSimulatorEvolutionApplication: "
				  "Problem while building the evolution system.");
		ok = false;
	}

	mRunner = new EvolutionRunner();

	return ok;

}

bool EvolutionOperatorTestApplication::buildSimulationModel()
{
	Physics::getSimulationEnvironmentManager()->createSnapshot();

	return true;
}

bool EvolutionOperatorTestApplication::buildEvolutionSystem() {

	Evolution::install();

	//install evolution system with local simulator evolution.
	new NeuroEvolutionSelector(new LocalNetworkInSimulationEvaluationMethod());
	
	if(!Evolution::getEvolutionManager()->getEvolutionWorlds().empty()) {
		World *world = Evolution::getEvolutionManager()->getEvolutionWorlds().at(0);
		const QList<Population*> &populations = world->getPopulations();

		for(QListIterator<Population*> i(populations); i.hasNext();) {
			Population *population = i.next();	

			//Add Selection Methods.
			FirstParentsTestSelectionMethod *firstParentSelection = new FirstParentsTestSelectionMethod();
			population->addSelectionMethod(firstParentSelection);
			firstParentSelection->getPopulationProportion()->set(0.0);
	
			firstParentSelection->setResponsibleFitnessFunction(population->getFitnessFunctions().at(0));
		}
	}

	return true;
}



