/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   till.faber@uni-osnabrueck.de
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
#include <QTimer>



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
	connect(op, SIGNAL(dataPrepared(QString, MatrixValue*, QString, QString)), opw, SLOT(printData(QString, MatrixValue*, QString, QString)));


	timer = new QTimer();
// 	timer->start(500);
	timer->setInterval(500);
	connect(timer, SIGNAL(timeout()), opw, SLOT(updateData()));
	connect(opw, SIGNAL(timerStart()), timer, SLOT(start()));
	connect(op, SIGNAL(startProcessing()), opw, SLOT(processing()));
	connect(op, SIGNAL(finishedProcessing()), timer, SLOT(stop()));
	connect(op, SIGNAL(finishedProcessing()), opw, SLOT(finishedProcessing()));
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



