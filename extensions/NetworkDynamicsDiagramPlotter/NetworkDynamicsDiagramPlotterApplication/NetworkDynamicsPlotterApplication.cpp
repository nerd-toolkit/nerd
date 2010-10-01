

/***************************************************************************
 *   NERD - Open Robot Control and Simulation Library                      *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Copyright (C) 2008 by Christian Rempis, Verena Thomas                 *
 *   christian.rempis@uni-osnabrueck.de +                                  *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD library is part of the EU project ALEAR                      *
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
 ***************************************************************************/



#include "NetworkDynamicsPlotterApplication.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Collections/StandardNeuralNetworkFunctions.h"
#include "PlugIns/CommandLineArgument.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Collections/StandardConstraintCollection.h"
#include "Network/Neuro.h"
#include "PlugIns/NeuralNetworkAttributes.h"
#include "Collections/StandardTagCollection.h"
#include "Gui/DynamicsPlotterMainWindow.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include "IO/NeuralNetworkIO.h"
#include "Collections/NetworkEditorCollection.h"
#include "EvolutionConstants.h"
#include "Value/BoolValue.h"
#include "Collections/NeuroModuleCollection.h"
#include "Collections/DynamicsPlotCollection.h"
#include "Gui/Control/BoolValueSwitcherAction.h"
#include "DynamicsPlotConstants.h"




using namespace std;

namespace nerd {


/**
 * Constructs a new NeuralNetworkEditorApplication.
 */
NetworkDynamicsPlotterApplication::NetworkDynamicsPlotterApplication()
	: mExecutionLoop(0)
{
	mName = "NeuralNetworkApplication";

	//Add statis mode value to allow the plotters to prevent network modifications in the editor
	//while the plots are calculated.
	BoolValue *stasisModeValue = new BoolValue(true);
	Core::getInstance()->getValueManager()->addValue(
				EvolutionConstants::VALUE_EVO_STASIS_MODE, stasisModeValue);

}



/**
 * Destructor.
 */
NetworkDynamicsPlotterApplication::~NetworkDynamicsPlotterApplication() {
}



bool NetworkDynamicsPlotterApplication::setupGui() {

	//Have to  be present before the GUI is constructed.
	StandardNeuralNetworkFunctions();
	StandardConstraintCollection();

	mMainWindow = new DynamicsPlotterMainWindow(true);

	if(mMainWindow != 0) {
		new NetworkEditorCollection(mMainWindow->getMenu("Tools"), "Network &Editor", false);

		BoolValueSwitcherAction *runPlotterButton = new BoolValueSwitcherAction("&Run Plotters",
					DynamicsPlotConstants::VALUE_PLOTTER_EXECUTE);
		runPlotterButton->setShortcut(tr("Ctrl+r"));
		mMainWindow->getMenu("Control")->addAction(runPlotterButton);
	}

	connect(this, SIGNAL(showGui()), mMainWindow, SLOT(show()));

	//****Till****//
	op = new OnlinePlotter();
	opw = new OnlinePlotterWindow();
	connect(op, SIGNAL(dataPrepared(QString, MatrixValue*)), opw, SLOT(printData(QString, MatrixValue*)));


	//***/Till****//
	
	return true;
}

bool NetworkDynamicsPlotterApplication::setupApplication() {
	bool ok = true;

	//install network characterizer
	new NeuralNetworkAttributes();
	StandardTagCollection();
	NeuroModuleCollection();

	DynamicsPlotCollection();


	//load a network if given.
	CommandLineArgument *netArg = new CommandLineArgument("loadNetwork", "net", "<networkFile",
								"Loads a NeuralNetwork to the NetworkEditor", 
								1, 0, false, true);

	if(netArg->getNumberOfEntries() > 0) {
		QStringList files = netArg->getEntryParameters(0);
		if(files.size() > 0) {
			QString errorMessage;
			QList<QString> warnings;
			NeuralNetwork *net = NeuralNetworkIO::createNetworkFromFile(files.at(0), 
							&errorMessage, &warnings);

			if(errorMessage != "") {
				Core::log("Error while loading net: " + errorMessage, true);
			}
			if(!warnings.empty()) {
				for(QListIterator<QString> j(warnings); j.hasNext();) {
					Core::log("Warning: " + j.next(), true);
				}
			}

			if(dynamic_cast<ModularNeuralNetwork*>(net) != 0) {
				Neuro::getNeuralNetworkManager()->addNeuralNetwork(net);
			}
			
		}
	}
	else {
		Neuro::getNeuralNetworkManager()->addNeuralNetwork(new ModularNeuralNetwork(
										AdditiveTimeDiscreteActivationFunction(),
										TransferFunctionTanh(),
										SimpleSynapseFunction())); 
	}

	mExecutionLoop = new PlotterExecutionLoop();

	//****Till****//
	exporter = new Exporter();
	//***/Till****//
	return ok;
}


bool NetworkDynamicsPlotterApplication::runApplication() {
	bool ok = true;

	mExecutionLoop->start();

	return ok; 
}




}



