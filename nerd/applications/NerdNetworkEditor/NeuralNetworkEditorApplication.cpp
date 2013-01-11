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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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



#include "NeuralNetworkEditorApplication.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Collections/StandardNeuralNetworkFunctions.h"
#include "PlugIns/CommandLineArgument.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "IO/NeuralNetworkIO.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Collections/StandardConstraintCollection.h"
#include "Gui/NetworkEditorTools/NeuralNetworkToolbox.h"
#include "Network/Neuro.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "SynapseFunction/SynapseFunction.h"
#include "Gui/NetworkEditor/StandAloneNetworkEditor.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "PlugIns/NeuralNetworkAttributes.h"
#include "Collections/StandardTagCollection.h"
#include <NeuroModulation/NeuroModulatorManager.h>

using namespace std;

namespace nerd {


/**
 * Constructs a new NeuralNetworkEditorApplication.
 */
NeuralNetworkEditorApplication::NeuralNetworkEditorApplication()
	: mEditor(0)
{
	mName = "NeuralNetworkApplication";
}



/**
 * Destructor.
 */
NeuralNetworkEditorApplication::~NeuralNetworkEditorApplication() {
}



bool NeuralNetworkEditorApplication::setupGui() {

	//Have to  be present before the GUI is constructed.
	StandardNeuralNetworkFunctions();
	StandardConstraintCollection();


	mEditor = new StandAloneNetworkEditor(0);
	mEditor->resize(600, 600);
	mEditor->initialize();

	//create tools
// 	new ShowInputOutputNeuronsTool(mEditor);
// 	new ShowBiasTool(mEditor);
	new NeuralNetworkToolbox(mEditor);

	//create default tab
	mEditor->addNetworkVisualization("Default");

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
				cerr << "Error while loading net: " << errorMessage.toStdString().c_str() << endl;
			}
			if(!warnings.empty()) {
				for(QListIterator<QString> j(warnings); j.hasNext();) {
					cerr << "Warning: " << j.next().toStdString().c_str() << endl;
				}
			}

			if(dynamic_cast<ModularNeuralNetwork*>(net) != 0) {
				NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
				if(visu != 0) {
					visu->setNeuralNetwork(dynamic_cast<ModularNeuralNetwork*>(net)); 
				}
			}
			
		}
	}
	else {
		NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
		if(visu != 0) {
			visu->setNeuralNetwork(new ModularNeuralNetwork(
										AdditiveTimeDiscreteActivationFunction(),
										TransferFunctionTanh(),
										SimpleSynapseFunction())); 
		}
	}

	mEditor->triggerEnableNetworkModification(true);


	connect(this, SIGNAL(showGui()), mEditor, SLOT(show()));

	return true;
}

bool NeuralNetworkEditorApplication::setupApplication() {
	bool ok = true;

	//install network characterizer
	new NeuralNetworkAttributes();
	StandardTagCollection();
	
	//Enable NeuroModulation
	NeuroModulatorManager::getInstance();

	return ok;
}


bool NeuralNetworkEditorApplication::runApplication() {
	bool ok = true;


	return ok; 
}




}



