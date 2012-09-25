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


#include "OrcsEvaluationApplication.h"
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
#include "PlugIns/FitnessLogger.h"
#include "Fitness/Fitness.h"
#include "Fitness/FitnessManager.h"
#include "Collections/NeuroFitnessPrototypes.h"
#include "Collections/TerminateTryCollisionRule.h"
#include "Collections/StandardConstraintCollection.h"
#include "Gui/NetworkEditor/EvaluationNetworkEditor.h"
#include "Gui/NetworkEditorTools/NeuralNetworkToolbox.h"
#include "Collections/NetworkEditorCollection.h"
#include "Statistics/Statistics.h"
#include "PlugIns/ActivationFrequencyCalculator.h"
#include "Physics/EvolvableParameter.h"
#include "Collections/EvolutionSimulationPrototypes.h"
#include "Collections/NeuroModuleCollection.h"
#include "Collections/YarsCommunication.h"
#include "PlugIns/NeuralNetworkAttributes.h"
#include "PlugIns/FramesPerSecondCounter.h"
#include "Collections/StandardTagCollection.h"
#include "PlugIns/ModelParameterRotator.h"
#include "Gui/ScriptedFitnessEditor/MultipleScriptedFitnessEditor.h"
#include "Gui/AutomaticPictureSeries/PictureSeriesCreator.h"
#include "Collections/UniversalNeuroScriptLoader.h"

using namespace std;

using namespace nerd;

OrcsEvaluationApplication::OrcsEvaluationApplication()
		: NeuroAndSimEvaluationStandardGuiApplication(true, true)
{

}

OrcsEvaluationApplication::~OrcsEvaluationApplication()
{
}

QString OrcsEvaluationApplication::getName() const
{
	return "OrcsEvaluationApplication";
}

bool OrcsEvaluationApplication::setupGui() {
	NeuroAndSimEvaluationStandardGuiApplication::setupGui();

	//Have to  be present before the GUI is constructed.
	StandardNeuralNetworkFunctions();
	StandardConstraintCollection();


	if(mEnableGui && mGuiMainWindow != 0) {
		new NetworkEditorCollection(mGuiMainWindow->getMenu("Tools"), "Network &Editor");

		//Add Fitness Editor
		QWidget *widget = new MultipleScriptedFitnessEditor(".*Fitness.*/Code", false);
		widget->resize(500,600);
		QAction *showFitnessEditorAction = mGuiMainWindow->getMenu("Tools")->addAction("Fitness Editor");
		showFitnessEditorAction->setShortcut(Qt::Key_F5);
		connect(showFitnessEditorAction, SIGNAL(triggered()),
				widget, SLOT(showWidget()));

		//Add PictureSeriesCreator
		new PictureSeriesCreator();
	}

	return true;
}

bool OrcsEvaluationApplication::setupApplication()
{
// 	StandardConstraintCollection();
// 	StandardNeuralNetworkFunctions();
	NeuroAndSimEvaluationStandardGuiApplication::setupApplication();

	//Install network characterization
	new NeuralNetworkAttributes();

	//Choose Physics Engine (or external Yars simulator)
	CommandLineArgument *physicsArg = new CommandLineArgument("physics", "p", "<physicsLibrary>",
			"Uses <physicsLibrary> as physics engine. "
			"Currently there are [ode, yars].", 1,0, true);
	if(physicsArg->getNumberOfEntries() != 0 && !physicsArg->getEntryParameters(0).empty()
			&& physicsArg->getEntryParameters(0).at(0).trimmed() == "yars")
	{
		YarsCommunication();
	}
	else {
		//install ODE PhysicsLayer
		ODE_Physics();
	}
	
	new FramesPerSecondCounter();


	EvolutionSimulationPrototypes();

	//install file parser
	new SimpleObjectFileParser();
	//install neural network control parser.
	new NetworkAgentControlParser();

	new ActivationFrequencyCalculator("A");

	//add motor model rotation
	new ModelParameterRotator();

	//Install neuro modules
	NeuroModuleCollection();
	StandardTagCollection();
	
	Fitness::install();
	new FitnessLogger();
	NeuroFitnessPrototypes();
	Statistics::getStatisticsManager();

	UniversalNeuroScriptLoader();

	return true;
}

bool OrcsEvaluationApplication::buildSimulationModel()
{
	Physics::getSimulationEnvironmentManager()->createSnapshot();

	return true;
}




