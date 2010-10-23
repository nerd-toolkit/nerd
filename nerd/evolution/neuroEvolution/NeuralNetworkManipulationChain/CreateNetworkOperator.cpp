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

#include "CreateNetworkOperator.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Network/Neuro.h"
#include "Network/Neuron.h"
#include "Network/NeuralNetworkManager.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "SynapseFunction/SynapseFunction.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include <iostream>
#include "IO/NeuralNetworkIO.h"
#include "Core/Core.h"
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include <QDir>
#include "EvolutionConstants.h"
#include "Evolution/EvolutionManager.h"
#include "Event/EventManager.h"

using namespace std;

namespace nerd {

CreateNetworkOperator::CreateNetworkOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mEvolutionRestartedEvent(), 
	  mNetworkIndex(0), mFirstNetworkCycle(true)
{
	mNumberOfInputNeurons = new IntValue(45);
	mNumberOfOutputNeurons = new IntValue(45);
	mTransferFunctionName = new StringValue("ASeriesTanh");
	mActivationFunctionName = new StringValue("ASeries");
	mSynapseFunctionName = new StringValue("ASeries");
	mInitialNetworkXML = new StringValue("");
	mInitialNetworkXML->useAsFileName(true);
	mPreserveInitialNetworks = new BoolValue(true);

	addParameter("InputNeurons", mNumberOfInputNeurons);
	addParameter("OutputNeurons", mNumberOfOutputNeurons);
	addParameter("TransferFunction", mTransferFunctionName);
	addParameter("ActivationFunction", mActivationFunctionName);
	addParameter("SynapseFunction", mSynapseFunctionName);
	addParameter("InitialNetworkXML", mInitialNetworkXML);
	addParameter("PreserveInitialNetworks", mPreserveInitialNetworks);

	getMaximalNumberOfApplicationsValue()->set(1);
	getOperatorIndexValue()->set(0);

	EventManager *em = Core::getInstance()->getEventManager();
	mEvolutionRestartedEvent = em->registerForEvent(EvolutionConstants::EVENT_EVO_EVOLUTION_RESTARTED, this, true);


	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QList<TransferFunction*> tfs = nmm->getTransferFunctionPrototypes();
	for(QListIterator<TransferFunction*> i(tfs); i.hasNext();) {
        mTransferFunctionName->getOptionList().append(i.next()->getName());
	}

	QList<ActivationFunction*> afs = nmm->getActivationFunctionPrototypes();
	for(QListIterator<ActivationFunction*> i(afs); i.hasNext();) {
        mActivationFunctionName->getOptionList().append(i.next()->getName());
	}

	QList<SynapseFunction*> sfs = nmm->getSynapseFunctionPrototypes();
	for(QListIterator<SynapseFunction*> i(sfs); i.hasNext();) {
        mSynapseFunctionName->getOptionList().append(i.next()->getName());
	}

}

CreateNetworkOperator::CreateNetworkOperator(const CreateNetworkOperator &other)
	: Object(), ValueChangedListener(), NeuralNetworkManipulationOperator(other),
	  mNetworkIndex(0), mFirstNetworkCycle(true)
{
	mNumberOfInputNeurons = dynamic_cast<IntValue*>(
			getParameter("InputNeurons"));
	mNumberOfOutputNeurons = dynamic_cast<IntValue*>(
			getParameter("OutputNeurons"));
	mTransferFunctionName = dynamic_cast<StringValue*>(
			getParameter("TransferFunction"));
	mActivationFunctionName = dynamic_cast<StringValue*>(
			getParameter("ActivationFunction"));
	mSynapseFunctionName = dynamic_cast<StringValue*>(
			getParameter("SynapseFunction"));
	mInitialNetworkXML = dynamic_cast<StringValue*>(
			getParameter("InitialNetworkXML"));
	mPreserveInitialNetworks = dynamic_cast<BoolValue*>(
			getParameter("PreserveInitialNetworks"));

	EventManager *em = Core::getInstance()->getEventManager();
	mEvolutionRestartedEvent = em->registerForEvent(EvolutionConstants::EVENT_EVO_EVOLUTION_RESTARTED, this, true);
}

CreateNetworkOperator::~CreateNetworkOperator() {
	while(!mInitialNetworks.empty()) {
		NeuralNetwork *net = mInitialNetworks.at(0);
		mInitialNetworks.removeAll(net);
		delete net;
	}
}

NeuralNetworkManipulationOperator* CreateNetworkOperator::createCopy() const
{
	return new CreateNetworkOperator(*this);
}

bool CreateNetworkOperator::applyOperator(Individual *individual, CommandExecutor*) {

	if(individual == 0) {
		return false;
	}

	if(!individual->getParents().empty()) {
		//don't change an individual's genome if it has parents.
		return true;
	}

	if(individual->getGenome() != 0) {
		//don't change an individual if it already has a genome.
		return true;
	}

	QString currentGenString = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());

	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();

	NeuralNetwork *net = 0;

	//Check if there is already an initial network as blueprint.
	if(!mInitialNetworks.empty()) {
		if(mNetworkIndex >= mInitialNetworks.size()) {
			mNetworkIndex = 0;
			mFirstNetworkCycle = false;
		}
		net = mInitialNetworks.at(mNetworkIndex)->createCopy();
		mNetworkIndex++;

		individual->setGenome(net);
		if(mFirstNetworkCycle && mPreserveInitialNetworks->get()) {
			individual->protectGenome(true);
		}

		return true;
	}

	if(net == 0 && mInitialNetworkXML->get().trimmed() != "") {

		QDir networkDirectory(mInitialNetworkXML->get());

		if(networkDirectory.exists()) {

			QStringList filters;
     		filters << "*.onn" << "*.smb";

			QStringList networkFiles = networkDirectory.entryList(filters);

			for(QListIterator<QString> j(networkFiles); j.hasNext();) {

				QString fileName = networkDirectory.absoluteFilePath(j.next());

				NeuralNetwork *net = loadNetwork(fileName);

				if(net != 0) {
					mInitialNetworks.append(net);
				}
			}

			if(!mInitialNetworks.empty()) {

				individual->setGenome(mInitialNetworks.at(0)->createCopy());
				individual->protectGenome(true);
				mNetworkIndex++;

				return true;
			}

		}
		else {

			NeuralNetwork *net = loadNetwork(mInitialNetworkXML->get());

			if(net != 0) {

				mInitialNetworks.append(net);
				mFirstNetworkCycle = false;

				individual->setGenome(net->createCopy());

				if(mPreserveInitialNetworks->get()) {
					individual->protectGenome(true);
				}

				return true;
			}
		}
	}

	//if no file was loaded, then create a network interface from scratch.

	//create default function instances (at the stack, not at the heap)
	AdditiveTimeDiscreteActivationFunction additiveTd;
	TransferFunctionTanh tanh;
	SimpleSynapseFunction simpleSynapseFunction;

	ActivationFunction *activationFunction = &additiveTd;
	TransferFunction *transferFunction = &tanh;
	SynapseFunction *synapseFunction = &simpleSynapseFunction;


	//Check if there is a default function for each type matching the parameter values.

	const QList<ActivationFunction*> &activationFunctions = nnm->getActivationFunctionPrototypes();
	for(QListIterator<ActivationFunction*> i(activationFunctions); i.hasNext();) {
		ActivationFunction *af = i.next();
		if(af->getName() == mActivationFunctionName->get()) {
			activationFunction = af;
			break;
		}
	}

	const QList<TransferFunction*> &transferFunctions = nnm->getTransferFunctionPrototypes();
	for(QListIterator<TransferFunction*> i(transferFunctions); i.hasNext();) {
		TransferFunction *tf = i.next();
		if(tf->getName() == mTransferFunctionName->get()) {
			transferFunction = tf;
			break;
		}
	}

	const QList<SynapseFunction*> &synapseFunctions = nnm->getSynapseFunctionPrototypes();
	for(QListIterator<SynapseFunction*> i(synapseFunctions); i.hasNext();) {
		SynapseFunction *sf = i.next();
		if(sf->getName() == mSynapseFunctionName->get()) {
			synapseFunction = sf;
			break;
		}
	}

	//Create a neural network as initial network with the given
	//number of input/output neurons.
	net = new ModularNeuralNetwork(*activationFunction, *transferFunction, *synapseFunction);

	//Add input/output neurons
	for(int i = 0; i < mNumberOfInputNeurons->get(); ++i) {
		Neuron *neuron = new Neuron(QString("Input").append(QString::number(i)), *transferFunction,
									*activationFunction);
		neuron->setProperty(Neuron::NEURON_TYPE_INPUT);
		neuron->setProperty(EvolutionConstants::TAG_EVO_CREATION_DATE, currentGenString);
		net->addNeuron(neuron);
	}
	for(int i = 0; i < mNumberOfOutputNeurons->get(); ++i) {
		Neuron *neuron = new Neuron(QString("Output").append(QString::number(i)), *transferFunction,
									*activationFunction);

		neuron->setProperty(Neuron::NEURON_TYPE_OUTPUT);
		neuron->setProperty(EvolutionConstants::TAG_EVO_CREATION_DATE, currentGenString);
		net->addNeuron(neuron);
	}

	mInitialNetworks.append(net);
	individual->setGenome(net->createCopy());
	individual->protectGenome(true);
	mFirstNetworkCycle = false;

	return true;
}


QString CreateNetworkOperator::getName() const {
	return NeuralNetworkManipulationOperator::getName();
}


void CreateNetworkOperator::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mEvolutionRestartedEvent) {
		//7cerr << "CreateNetworkOp: destroy templates" << endl;
		//destroy network templates
		while(!mInitialNetworks.empty()) {
			NeuralNetwork *net = mInitialNetworks.first();
			mInitialNetworks.removeAll(net);
			delete net;
		}
	}
}


NeuralNetwork* CreateNetworkOperator::loadNetwork(const QString fileName) {


	//check if there is a given initial network xml
	QString errorMsg;
	QList<QString> warnings;
	NeuralNetwork *net = NeuralNetworkIO::createNetworkFromFile(
				fileName, &errorMsg, &warnings);


	QString currentGenString = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());

	if(net == 0) {
		Core::log(QString("CreateNetworkOperator: Could not create network from file [")
				.append(mInitialNetworkXML->get()).append("]! "));

		Core::log(QString("CreateNetworkOperator: Error: ").append(errorMsg));
	}
	else {

		//increment network id counter
		qulonglong maxId = 0;

		//set creategeneration dates
		const QList<Neuron*> &neurons = net->getNeurons();
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *n = i.next();
			n->setProperty(EvolutionConstants::TAG_EVO_CREATION_DATE, currentGenString);
			if(n->getId() > maxId) {
				maxId = n->getId();
			}
		}
		QList<Synapse*> synapses = net->getSynapses();
		for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
			Synapse *s = i.next();
			s->setProperty(EvolutionConstants::TAG_EVO_CREATION_DATE, currentGenString);
			if(s->getId() > maxId) {
				maxId = s->getId();
			}
		}
		ModularNeuralNetwork *modularNet = dynamic_cast<ModularNeuralNetwork*>(net);
		if(modularNet != 0) {
			const QList<NeuronGroup*> &groups = modularNet->getNeuronGroups();
			for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
				NeuronGroup *g = i.next();
				g->setProperty(EvolutionConstants::TAG_EVO_CREATION_DATE, currentGenString);
				if(g->getId() > maxId) {
					maxId = g->getId();
				}
			}
		}

		if(NeuralNetwork::generateNextId() <= maxId) {
			NeuralNetwork::resetIdCounter(maxId + 1);
		}

		for(QListIterator<QString> i(warnings); i.hasNext();) {
			Core::log(QString("CreateNetworkOperator: Warning: ").append(i.next()));
		}
	}

	return net;
}

}


