/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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



#include "NetworkSimulationRecorder.h"
#include "Event/EventManager.h"
#include "Core/Core.h"
#include <iostream>
#include "Value/ValueManager.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include <QStringList>
#include "NerdConstants.h"
#include <NeuralNetworkConstants.h>
#include <Network/Neuro.h>
#include <NeuroModulation/NeuroModulator.h>
#include <QFile>
#include <QDir>
#include "ActivationFunction/NeuroModulatorActivationFunction.h"


using namespace std;

namespace nerd {

NetworkSimulationRecorder::NetworkSimulationRecorder()
	: SimulationRecorder(), mDisablePlasticityValue(0), mNetworksChanged(true)
{
	
	mNetworkManager = Neuro::getNeuralNetworkManager();
	
	mIncludeNetworks = new BoolValue(true);
	
	//Core::getInstance()->getValueManager()->addValue("/DataRecorder/IncludeNetworks", mIncludeNetworks);
	
}

NetworkSimulationRecorder::~NetworkSimulationRecorder() {
}


QString NetworkSimulationRecorder::getName() const {
	return "NetworkSimulationRecorder";
}


bool NetworkSimulationRecorder::init() {
	return SimulationRecorder::init();
}


bool NetworkSimulationRecorder::bind() {
	bool ok = SimulationRecorder::bind();
	
	EventManager *em = Core::getInstance()->getEventManager();
	
	mNetworksReplacedEvent = em->registerForEvent(NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED, this);
	mNetworkStructureChangedEvent = em->registerForEvent(NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED, this);

	ValueManager *vm = Core::getInstance()->getValueManager();
	
	mDisablePlasticityValue = vm->getBoolValue(NeuralNetworkConstants::VALUE_DISABLE_NEURAL_PLASTICITY);
	mDisableNetworkUpdateValue = vm->getBoolValue(NeuralNetworkConstants::VALUE_DISABLE_NETWORK_UPDATE);
	
	if(mNetworksReplacedEvent == 0 || mNetworkStructureChangedEvent == 0 
		|| mDisablePlasticityValue == 0 || mDisableNetworkUpdateValue == 0) 
	{
		ok = false;
		Core::log("NetworkSimulationRecorder: Could not find all required events and values.");
	}
	
	return ok;
}


bool NetworkSimulationRecorder::cleanUp() {
	SimulationRecorder::cleanUp();
	return true;
}


void NetworkSimulationRecorder::eventOccured(Event *event) {
	SimulationRecorder::eventOccured(event);
	if(event == 0) {
		return;
	}
	else if(event == mNetworksReplacedEvent
		|| event == mNetworkStructureChangedEvent) 
	{
		mNetworksChanged = true;
	}
}

void NetworkSimulationRecorder::valueChanged(Value *value) {
	SimulationRecorder::valueChanged(value);
	if(value == 0) {
		return;
	}
}


bool NetworkSimulationRecorder::startPlayback() {
	bool ok = SimulationRecorder::startPlayback();
	
	mPlasticityWasDisabled = mDisablePlasticityValue->get();
	mNetworkUpdateWasDisabled = mDisableNetworkUpdateValue->get();
	mDisablePlasticityValue->set(true);
	mDisableNetworkUpdateValue->set(true);
	
	return ok;
}


bool NetworkSimulationRecorder::stopPlayback() {
	bool ok = SimulationRecorder::stopPlayback();
	
	mDisablePlasticityValue->set(mPlasticityWasDisabled);
	mDisableNetworkUpdateValue->set(mNetworkUpdateWasDisabled);
	
	return ok;
}



//TODO entire mechanism: make sure that networks with similar ids can be used together!
//This will not work in the current implementation.
void NetworkSimulationRecorder::updateListOfRecordedValues() {
	SimulationRecorder::updateListOfRecordedValues();
	
	QMutexLocker locker(mNetworkManager->getNetworkExecutionMutex());
	
	mNeurons.clear();
	mSynapses.clear();
	
	QList<NeuralNetwork*> networks = mNetworkManager->getNeuralNetworks();
	for(QListIterator<NeuralNetwork*> i(networks); i.hasNext();) {
		NeuralNetwork *network = i.next();
		
		mNeurons << network->getNeurons();
		mSynapses << network->getSynapses();
	}
	
	mNetworksChanged = false;
	
}


void NetworkSimulationRecorder::updateRecordedData(QDataStream &dataStream) {
	
	
	if(mNetworksChanged) {
		//TODO check for missing neurons from required set!
		updateListOfRecordedValues();
	}
	
	SimulationRecorder::updateRecordedData(dataStream);
	
	if(mIncludeNetworks->get() == false) {
		return;
	}
	
	dataStream << ((int) mNeurons.size());
	
	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		
		dataStream << neuron->getId() 
					<< neuron->getBiasValue().get()
					<< neuron->getOutputActivationValue().get() 
					<< neuron->getActivationValue().get();
		
		NeuroModulatorActivationFunction *af = 
					dynamic_cast<NeuroModulatorActivationFunction*>(neuron->getActivationFunction());
		
		if(af != 0) {
			NeuroModulator *mod = af->getNeuroModulator();		
			
			//TODO Solve this in a more generic way as soon as there is more time!
			QList<int> modulatorTypes = mod->getModulatorTypes();
			
			dataStream << ((int) modulatorTypes.size());
			
			for(int j = 0; j < modulatorTypes.size(); ++j) {
				int type = modulatorTypes.at(j);
				
				dataStream << type;
				dataStream << ((double) (mod->getConcentration(type, neuron)));
				dataStream << ((bool) (mod->isCircularArea(type)));
				QRectF rect = mod->getLocalRect(type);
				//cerr << "Writing: " << neuron->getId() << " T " << type  << " " << rect.x() << " " << (mod->getLocalRect(2).x()) << " and " << rect.y() << " con: " << ((double) (mod->getConcentration(type, neuron))) << endl;
				dataStream << ((double) rect.width());
				dataStream << ((double) rect.height());
			}
		}
		else {
			dataStream << ((int) 0);
		}
	}
	
	dataStream << ((int) mSynapses.size());
	
	for(QListIterator<Synapse*> i(mSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		
		dataStream << synapse->getId() << synapse->getStrengthValue().get();
	}
	
	
}


void NetworkSimulationRecorder::updatePlaybackData(QDataStream &dataStream) {
	
	if(mNetworksChanged) {
		//TODO check for missing neurons etc.
		updateListOfRecordedValues();
	}
	
	SimulationRecorder::updatePlaybackData(dataStream);
	
	if(mIncludeNetworks->get() == false) {
		return;
	}
	
	int numberOfNeurons = 0;
	dataStream >> numberOfNeurons;
	
	qulonglong id = 0;
	double bias = 0.0;
	double output = 0.0;
	double activation = 0.0;
	int numberOfNMs = 0;
	
	for(int i = 0; i < numberOfNeurons; ++i) {
		dataStream >> id >> bias >> output >> activation;
		
		dataStream >> numberOfNMs; 
		
		Neuron *neuron = NeuralNetwork::selectNeuronById(id, mNeurons);
		
		if(neuron == 0) {
			Core::log("NetworkSimulationRecorder: Warning: Could not find "
						"neuron with id [" + QString::number(id) + "]", true);
			continue;
		}
		neuron->getBiasValue().set(bias);
		neuron->getActivationValue().set(activation);
		neuron->getOutputActivationValue().set(output);

		NeuroModulatorActivationFunction *af = 
					dynamic_cast<NeuroModulatorActivationFunction*>(neuron->getActivationFunction());
		
		int type = 0;
		double concentration = 0.0;
		bool circular = true;
		double width = 0.0;
		double height = 0.0;
		
		for(int j = 0; j < numberOfNMs; ++j) {
			dataStream >> type;
			dataStream >> concentration;
			dataStream >> circular;
			dataStream >> width;
			dataStream >> height;

			if(af != 0 && af->getNeuroModulator() != 0) {
				NeuroModulator *mod = af->getNeuroModulator();
				mod->setConcentration(type, concentration, neuron);
				mod->setLocalAreaRect(type, width, height, Vector3D(), circular);
				
				//cerr << "Got: " << type << " " << width << " " << height << " str: " << concentration << " C: " << circular << endl;
			}
		}
	}
	
	int numberOfSynapses = 0;
	dataStream >> numberOfSynapses;
		
	double weight = 0.0;
	
	for(int i = 0; i < numberOfSynapses; ++i) {
		dataStream >> id;
		dataStream >> weight;
		
		Synapse *synapse = NeuralNetwork::selectSynapseById(id, mSynapses);
		
		if(synapse == 0) {
			Core::log("NetworkSimulationRecorder: Warning: Could not find "
						"synapse with id [" + QString::number(id) + "]", true);
			continue;
		}
		synapse->getStrengthValue().set(weight);
	}
	
}


}


