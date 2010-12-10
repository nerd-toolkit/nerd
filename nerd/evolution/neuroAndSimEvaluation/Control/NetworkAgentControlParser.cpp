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

#include "NetworkAgentControlParser.h"
#include <QStringList>
#include "Core/Core.h"
#include "Network/NeuralNetwork.h"
#include "IO/NeuralNetworkIO.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include "Network/Neuro.h"
#include "Fitness/Fitness.h"
#include <iostream>
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"

using namespace std;

namespace nerd {

NetworkAgentControlParser::NetworkAgentControlParser()
{
	mNetLoaderArgument = new CommandLineArgument("loadNet", "net", 
			"<agent> <network> [<fitness>]", "Loads a neural network from file "
			"\n     <network> and attaches it as controller to the SimObjectGroup <agent>. "
			"\n     If no <network> is given, then an empty default network is used. ",
			0, 2, true);

	Neuro::install();
	Core::getInstance()->addSystemObject(this);
}

NetworkAgentControlParser::~NetworkAgentControlParser() {
}

QString NetworkAgentControlParser::getName() const {
	return "NetworkAgentControlParser";
}


void NetworkAgentControlParser::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mPhysicsEnvironmentChangedEvent) {
		connectNetworksToInterfaces();
	}
}

bool NetworkAgentControlParser::init() {
	bool ok = true;

	return ok;
}


bool NetworkAgentControlParser::bind() {
	bool ok = true;

	mPhysicsEnvironmentChangedEvent = 
					Physics::getPhysicsManager()->getPhysicsEnvironmentChangedEvent();
	mPhysicsEnvironmentChangedEvent->addEventListener(this);
	
	connectNetworksToInterfaces();

	return ok;
}


bool NetworkAgentControlParser::cleanUp() {
	return true;
}


void NetworkAgentControlParser::connectNetworksToInterfaces() {
	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();
	bool addedNetwork = false;

	QStringList params = mNetLoaderArgument->getParameters();

	int numberOfEntries = mNetLoaderArgument->getNumberOfEntries();

	for(int i = 0; i < numberOfEntries; ++i) {
		QStringList paramSet;
		if(params.size() > i) {
			paramSet = params.at(i).split(" ");
		}
		if(paramSet.size() > 2) {
			Core::log("NetworkAgentControlParser: Could not interpret command line argument. ");
			continue;
		}
		QString agentName = "";
		if(paramSet.size() > 0) {
			agentName = paramSet.at(0);
		}
		
		QString networkFile = "";
		if(paramSet.size() > 1) {
			networkFile = paramSet.at(1);
		}
		
		PhysicsManager *pm = Physics::getPhysicsManager();

		SimObjectGroup *agent = 0;
		if(agentName == "" && !pm->getSimObjectGroups().empty()) {
			agent = pm->getSimObjectGroups().at(0);
		}
		else {
			agent = pm->getSimObjectGroup(agentName);
		}

		if(agent == 0) {
			Core::log(QString("NetworkAgentControlParser: Could not find an agent with name [")
					.append(agentName).append("]! [Skipping]"));
			continue;
		}
		
		QString errorMessage;
		QList<QString> messages;

		NeuralNetwork *net = 0;

		//check if there is already a network that can be reused.
		QList<NeuralNetwork*> networks = nnm->getNeuralNetworks();
		if(networks.size() > i) {
			net = networks.at(i);
		}

		if(net == 0) {
			if(networkFile == "") {
				//create standard network with matching number of input/output neurons.
	
				net = new ModularNeuralNetwork();
				int numberOfInputs = agent->getOutputValues().size();
				int numberOfOutputs = agent->getInputValues().size();
	
				for(int i = 0; i < numberOfInputs; ++i) {
					Neuron *neuron = new Neuron("Neuron" + QString::number(i),
											*net->getDefaultTransferFunction(),
											*net->getDefaultActivationFunction());
					neuron->setProperty(Neuron::NEURON_TYPE_INPUT);
					net->addNeuron(neuron);
				}
				for(int i = 0; i < numberOfOutputs; ++i) {
					Neuron *neuron = new Neuron("Neuron" + QString::number(i),
											*net->getDefaultTransferFunction(),
											*net->getDefaultActivationFunction());
					neuron->setProperty(Neuron::NEURON_TYPE_OUTPUT);
					net->addNeuron(neuron);
				}
		
			}
			else {
				net = NeuralNetworkIO::createNetworkFromFile(
								networkFile, &errorMessage, &messages);
			}
		}

		if(!messages.empty()) {
			Core::log(QString("NetworkAgentControlParser: ").append(errorMessage));
			for(int i = 0; i < messages.size(); ++i) {
				Core::log(QString("NetworkAgentControlParser: ").append(messages.at(i)));
			}
		}

		if(net == 0) {
			Core::log(QString("NetworkAgentControlParser: Could not load network from file [")
					.append(networkFile).append("]! [Skipping]"));
			continue;
		}

		agent->setController(net);
		
		
		nnm->addNeuralNetwork(net); 
		addedNetwork = true;

		Core::log(QString("NetworkAgentControlParser: Loaded network from file [")
					.append(networkFile).append("] to control agent [")
					.append(agentName).append("]!"));
	}

	if(addedNetwork) {
		nnm->triggerCurrentNetworksReplacedEvent();
	}
}


}


