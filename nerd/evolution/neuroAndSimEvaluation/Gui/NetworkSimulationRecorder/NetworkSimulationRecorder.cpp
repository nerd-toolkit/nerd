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
#include <IO/NeuralNetworkIO.h>
#include <Util/NeuralNetworkUtil.h>

using namespace std;

namespace nerd {
	

	
	

NetworkSimulationRecorder::NetworkSimulationRecorder()
	: SimulationRecorder(), mDisablePlasticityValue(0), mNetworksChanged(true)
{
	
	mNetworkManager = Neuro::getNeuralNetworkManager();
	
	mIncludeNetworks = new BoolValue(true);	
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
	
	QStringList entries = mObservedValues->get().split("\n");
	
	QMutexLocker locker(mNetworkManager->getNetworkExecutionMutex());
	
	QList<NeuralNetwork*> nets = mNetworkManager->getNeuralNetworks();
	if(nets.empty()) {
		return;
	}
	
	for(int i = 0; i < nets.size(); ++i) {
		NeuralNetwork *net = nets.at(i);
		if(! NeuralNetworkIO::createFileFromNetwork(
					mFile->fileName().append("_net" + QString::number(i) + ".onn"), net, 0, 0)) 
		{
			Core::log("NetworkSimulationRecorder: Could not save the current network to a file!", true);
		}
	}

	for(QStringListIterator i(entries); i.hasNext();) {
		QString entry = i.next();

		if(entry.trimmed().startsWith("[Neurons,O]")) { //Neuron Output
			QStringList majorParts = entry.trimmed().mid(11).split(":");
			if(majorParts.size() != 2) {
				Core::log("NetworkSimulationRecorder: Could not read line: ["
						+ entry.trimmed() + "]. Format: [Neurons,O]netId:neuronId,...", true);
				continue;
			}
			QList<Neuron*> neurons = getNeurons(getNetworkByIdString(nets, majorParts.at(0)), majorParts.at(1));
			for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
				Neuron *neuron = j.next();
				mRecordedValues.append(&neuron->getOutputActivationValue());
				mRecordedValueNames.append("[NO]" + majorParts.at(0) + ":" 
									+ QString::number(neuron->getId()) + "|" 
									+ neuron->getNameValue().get());
			}
		}
		else if(entry.trimmed().startsWith("[Neurons,A]")) { //Neuron Activation
			QStringList majorParts = entry.trimmed().mid(11).split(":");
			if(majorParts.size() != 2) {
				Core::log("NetworkSimulationRecorder: Could not read line: ["
						+ entry.trimmed() + "]. Format: [Neurons,A]netId:neuronId,...", true);
				continue;
			}
			QList<Neuron*> neurons = getNeurons(getNetworkByIdString(nets, majorParts.at(0)), majorParts.at(1));
			for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
				Neuron *neuron = j.next();
				mRecordedValues.append(&neuron->getActivationValue());
				mRecordedValueNames.append("[NA]" + majorParts.at(0) + ":" 
									+ QString::number(neuron->getId()) + "|" 
									+ neuron->getNameValue().get());
			}
		}
		else if(entry.trimmed().startsWith("[Neurons,AF]")) { //Neuron ActivationFunction Observables
			QStringList majorParts = entry.trimmed().mid(12).split(":");
			if(majorParts.size() != 3) {
				Core::log("NetworkSimulationRecorder: Could not read line: ["
						+ entry.trimmed() + "]. Format: [Neurons,AF]netId:observableNames:neuronId,...", true);
				continue;
			}
			QStringList observableNames = majorParts.at(1).trimmed().split("|");
			QList<Neuron*> neurons = getNeurons(getNetworkByIdString(nets, majorParts.at(0)), majorParts.at(2));
			for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
				Neuron *neuron = j.next();
				ObservableNetworkElement *one = dynamic_cast<ObservableNetworkElement*>(neuron->getActivationFunction());
				if(one != 0) {
					QList<QString> oneNames = one->getObservableOutputNames();
					for(QListIterator<QString> j(oneNames); j.hasNext();) {
						QString oneName = j.next();
						if(observableNames.contains("*") || observableNames.contains(oneName)) {
							mRecordedValues.append(one->getObservableOutput(oneName));
							mRecordedValueNames.append("[NAF]" + majorParts.at(0) + ":" 
										+ QString::number(neuron->getId()) + "|" + oneName 
										+ "|" + neuron->getNameValue().get());
						}
					}
				}
			}
		}
		else if(entry.trimmed().startsWith("[Neurons,TF]")) { //Neuron TransferFunction Observables
			QStringList majorParts = entry.trimmed().mid(12).split(":");
			if(majorParts.size() != 3) {
				Core::log("NetworkSimulationRecorder: Could not read line: ["
						+ entry.trimmed() + "]. Format: [Neurons,TF]netId:observableNames:neuronId,...", true);
				continue;
			}
			QStringList observableNames = majorParts.at(1).trimmed().split("|");
			QList<Neuron*> neurons = getNeurons(getNetworkByIdString(nets, majorParts.at(0)), majorParts.at(2));
			for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
				Neuron *neuron = j.next();
				ObservableNetworkElement *one = dynamic_cast<ObservableNetworkElement*>(neuron->getTransferFunction());
				if(one != 0) {
					QList<QString> oneNames = one->getObservableOutputNames();
					for(QListIterator<QString> j(oneNames); j.hasNext();) {
						QString oneName = j.next();
						if(observableNames.contains("*") || observableNames.contains(oneName)) {
							mRecordedValues.append(one->getObservableOutput(oneName));
							mRecordedValueNames.append("[NTF]" + majorParts.at(0) + ":" 
										+ QString::number(neuron->getId()) + "|" + oneName 
										+ "|" + neuron->getNameValue().get());
						}
					}
				}
			}
		}
		else if(entry.trimmed().startsWith("[Synapses,W]")) { //Synapse Weights
			QStringList majorParts = entry.trimmed().mid(12).split(":");
			if(majorParts.size() != 2) {
				Core::log("NetworkSimulationRecorder: Could not read line: ["
						+ entry.trimmed() + "]. Format: [Synapses,W]netId:synapseId,...", true);
				continue;
			}
			QList<Synapse*> synapses = getSynapses(getNetworkByIdString(nets, majorParts.at(0)), majorParts.at(1));
			for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
				Synapse *synapse = j.next();
				mRecordedValues.append(&synapse->getStrengthValue());
				mRecordedValueNames.append("[SW]" + majorParts.at(0) + ":" + QString::number(synapse->getId()));
			}
		}
		else if(entry.trimmed().startsWith("[Synapses,SF]")) { //Synapse Function Observables
			QStringList majorParts = entry.trimmed().mid(13).split(":");
			if(majorParts.size() != 3 || majorParts.at(1).size() < 1) {
				Core::log("NetworkSimulationRecorder: Could not read line: ["
						+ entry.trimmed() + "]. Format: [Synapses,SF]netId:observableNames:synapseId,...", true);
				continue;
			}
			QStringList observableNames = majorParts.at(1).trimmed().split("|");
			QList<Synapse*> synapses = getSynapses(getNetworkByIdString(nets, majorParts.at(0)), majorParts.at(2));
			for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
				Synapse *synapse = j.next();
				ObservableNetworkElement *one = dynamic_cast<ObservableNetworkElement*>(synapse->getSynapseFunction());
				if(one != 0) {
					QList<QString> oneNames = one->getObservableOutputNames();
					for(QListIterator<QString> j(oneNames); j.hasNext();) {
						QString oneName = j.next();
						if(observableNames.contains("*") || observableNames.contains(oneName)) {
							mRecordedValues.append(one->getObservableOutput(oneName));
							mRecordedValueNames.append("[SSF]" + majorParts.at(0) + ":" 
										+ QString::number(synapse->getId()) + "|" + oneName);
						}
					}
				}
			}
		}
		
	}

	mNetworksChanged = false;
}


void NetworkSimulationRecorder::syncWithListOfRecordedValues() {
	
	SimulationRecorder::syncWithListOfRecordedValues();
	
	QFile *infoFile = new QFile(mPlaybackFile->get() + "_info.txt");
	
	if(!infoFile->open(QIODevice::ReadOnly)) {
		Core::log(QString("Could not open file ").append(mFile->fileName()).append(" to record the simulation data."), true);
		infoFile->close();
		delete infoFile;
		infoFile = 0;
		return;
	}	
	
	QMutexLocker locker(mNetworkManager->getNetworkExecutionMutex());
	
	if(!mNetworkManager->getNeuralNetworks().empty()) {
		
		QList<NeuralNetwork*> nets = mNetworkManager->getNeuralNetworks();
	
		QTextStream dataStream(infoFile);
		bool ok = true;
		
		while(!dataStream.atEnd()) {
			
			QString line = dataStream.readLine().trimmed();

			if(line.startsWith("[NO]")) {
				QStringList majorParts = line.mid(4).split(":");
				if(majorParts.size() != 2) {
					Core::log("NetworkSimulationRecorder: Could not read line: ["
							+ line.trimmed() + "]. Format: [NO]netId:neuronId", true);
					continue;
				}
				
				NeuralNetwork *net = getNetworkByIdString(nets, majorParts.at(0));
				if(net == 0) {
					Core::log("NetworkSimulationRecorder: Could not find network id in [" + line + "]", true);
					continue;
				}
				
				QStringList parts = majorParts.at(1).split("|");
				if(parts.size() < 1) {
					Core::log("NetworkSimulationRecorder: Could not read neuron id [" + line + "]", true);
					continue;
				}
				qulonglong id = parts.at(0).toULongLong(&ok);
				if(!ok) {
					Core::log("NetworkSimulationRecorder: Could not parse neuron id [" + line + "]", true);
					continue;
				}
				
				Neuron *neuron = net->selectNeuronById(id, net->getNeurons());
				if(neuron == 0) {
					Core::log("NetworkSimulationRecorder: Could not find neuron with id [" + QString::number(id) + "]", true);
					continue;
				}
				mRecordedValues.append(&neuron->getOutputActivationValue());
				mRecordedValueNames.append("[NO]" + majorParts.at(0) + ":" + QString::number(id) + "|" 
									+ neuron->getNameValue().get());
			}
			else if(line.startsWith("[NA]")) {
				QStringList majorParts = line.mid(4).split(":");
				if(majorParts.size() != 2) {
					Core::log("NetworkSimulationRecorder: Could not read line: ["
							+ line.trimmed() + "]. Format: [NA]netId:neuronId", true);
					continue;
				}
				
				NeuralNetwork *net = getNetworkByIdString(nets, majorParts.at(0));
				if(net == 0) {
					Core::log("NetworkSimulationRecorder: Could not find network id in [" + line + "]", true);
					continue;
				}
				
				QStringList parts = majorParts.at(1).split("|");
				if(parts.size() < 1) {
					Core::log("NetworkSimulationRecorder: Could not read neuron id [" + line + "]", true);
					continue;
				}
				qulonglong id = parts.at(0).toULongLong(&ok);
				if(!ok) {
					Core::log("NetworkSimulationRecorder: Could not parse neuron id [" + line + "]", true);
					continue;
				}
				
				Neuron *neuron = net->selectNeuronById(id, net->getNeurons());
				if(neuron == 0) {
					Core::log("NetworkSimulationRecorder: Could not find neuron with id [" + QString::number(id) + "]", true);
					continue;
				}
				mRecordedValues.append(&neuron->getActivationValue());
				mRecordedValueNames.append("[NA]" + majorParts.at(0) + ":" + QString::number(id) + "|" 
									+ neuron->getNameValue().get());
			}
			else if(line.startsWith("[NAF]")) {
				QStringList majorParts = line.mid(5).split(":");
				if(majorParts.size() != 2) {
					Core::log("NetworkSimulationRecorder: Could not read line: ["
							+ line.trimmed() + "]. Format: [NAF]netId:neuronId|observableName", true);
					continue;
				}
				
				NeuralNetwork *net = getNetworkByIdString(nets, majorParts.at(0));
				if(net == 0) {
					Core::log("NetworkSimulationRecorder: Could not find network id in [" + line + "]", true);
					continue;
				}
				
				QStringList parts = majorParts.at(1).split("|");
				if(parts.size() < 2) {
					Core::log("NetworkSimulationRecorder: Could not read neuron id [" + line + "]", true);
					continue;
				}
				qulonglong id = parts.at(0).toULongLong(&ok);

				if(!ok) {
					Core::log("NetworkSimulationRecorder: Could not parse neuron id [" + line + "]", true);
					continue;
				}
				Neuron *neuron = net->selectNeuronById(id, net->getNeurons());
				if(neuron == 0 || neuron->getActivationFunction() == 0) {
					Core::log("NetworkSimulationRecorder: Could not find neuron with id [" + QString::number(id) + "]", true);
					continue;
				}
				Value *obsParam = neuron->getActivationFunction()->getObservableOutput(parts.at(1));
				if(obsParam != 0) {
					mRecordedValues.append(obsParam);
					mRecordedValueNames.append("[NAF]" + majorParts.at(0) + ":" + QString::number(id) 
									+ "|" + parts.at(1) + "|" + neuron->getNameValue().get());
				}
			}
			else if(line.startsWith("[NTF]")) {
				QStringList majorParts = line.mid(5).split(":");
				if(majorParts.size() != 2) {
					Core::log("NetworkSimulationRecorder: Could not read line: ["
							+ line.trimmed() + "]. Format: [NTF]netId:neuronId|observableName", true);
					continue;
				}
				
				NeuralNetwork *net = getNetworkByIdString(nets, majorParts.at(0));
				if(net == 0) {
					Core::log("NetworkSimulationRecorder: Could not find network id in [" + line + "]", true);
					continue;
				}
				
				QStringList parts = majorParts.at(1).split("|");
				if(parts.size() < 2) {
					Core::log("NetworkSimulationRecorder: Could not read neuron id [" + line + "]", true);
					continue;
				}
				qulonglong id = parts.at(0).toULongLong(&ok);

				if(!ok) {
					Core::log("NetworkSimulationRecorder: Could not parse neuron id [" + line + "]", true);
					continue;
				}
				Neuron *neuron = net->selectNeuronById(id, net->getNeurons());
				if(neuron == 0 || neuron->getTransferFunction() == 0) {
					Core::log("NetworkSimulationRecorder: Could not find neuron with id [" + QString::number(id) + "]", true);
					continue;
				}
				Value *obsParam = neuron->getTransferFunction()->getObservableOutput(parts.at(1));
				if(obsParam != 0) {
					mRecordedValues.append(obsParam);
					mRecordedValueNames.append("[NTF]" + majorParts.at(0) + ":" + QString::number(id) 
									+ "|" + parts.at(1) + "|" + neuron->getNameValue().get());
				}
			}
			else if(line.startsWith("[SW]")) {
				QStringList majorParts = line.mid(4).split(":");
				if(majorParts.size() != 2) {
					Core::log("NetworkSimulationRecorder: Could not read line: ["
							+ line.trimmed() + "]. Format: [SW]netId:synapseId", true);
					continue;
				}
				
				NeuralNetwork *net = getNetworkByIdString(nets, majorParts.at(0));
				if(net == 0) {
					Core::log("NetworkSimulationRecorder: Could not find network id in [" + line + "]", true);
					continue;
				}
				
				QStringList parts = majorParts.at(1).split("|");
				if(parts.size() < 1) {
					Core::log("NetworkSimulationRecorder: Could not read synapse id [" + line + "]", true);
					continue;
				}
				qulonglong id = parts.at(0).toULongLong(&ok);
				if(!ok) {
					Core::log("NetworkSimulationRecorder: Could not parse synapse id [" + line + "]", true);
					continue;
				}
				Synapse *synapse = net->selectSynapseById(id, net->getSynapses());
				if(synapse == 0) {
					Core::log("NetworkSimulationRecorder: Could not find synapse with id [" + QString::number(id) + "]", true);
					continue;
				}
				mRecordedValues.append(&synapse->getStrengthValue());
				mRecordedValueNames.append("[SW]" + majorParts.at(0) + ":" + QString::number(id));
			}
			else if(line.startsWith("[SSF]")) {
				QStringList majorParts = line.mid(5).split(":");
				if(majorParts.size() != 2) {
					Core::log("NetworkSimulationRecorder: Could not read line: ["
							+ line.trimmed() + "]. Format: [SSF]netId:synapseId|observableName", true);
					continue;
				}
				
				NeuralNetwork *net = getNetworkByIdString(nets, majorParts.at(0));
				if(net == 0) {
					Core::log("NetworkSimulationRecorder: Could not find network id in [" + line + "]", true);
					continue;
				}
				
				QStringList parts = majorParts.at(1).split("|");
				if(parts.size() < 2) {
					Core::log("NetworkSimulationRecorder: Could not read synapse id [" + line + "]", true);
					continue;
				}
				qulonglong id = parts.at(0).toULongLong(&ok);
				if(!ok) {
					Core::log("NetworkSimulationRecorder: Could not parse synapse id [" + line + "]", true);
					continue;
				}
				Synapse *synapse = net->selectSynapseById(id,  net->getSynapses());
				if(synapse == 0 || synapse->getSynapseFunction() == 0) {
					Core::log("NetworkSimulationRecorder: Could not find synapse with id [" + QString::number(id) + "]", true);
					continue;
				}
				Value *obsParam = synapse->getSynapseFunction()->getObservableOutput(parts.at(1));
				if(obsParam != 0) {
					mRecordedValues.append(obsParam);
					mRecordedValueNames.append("[SSF]" + majorParts.at(0) + ":" 
									+  QString::number(id) + "|" + parts.at(1));
				}
			}
		}
	}
	infoFile->close();
	delete infoFile;
	
	mNetworksChanged = false;
}


void NetworkSimulationRecorder::updateRecordedData(QDataStream &dataStream) {
	
	
	if(mNetworksChanged) {
		//TODO check for missing neurons from required set!
		updateListOfRecordedValues();
	}
	
	SimulationRecorder::updateRecordedData(dataStream);
	/*
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
	*/
	
}


void NetworkSimulationRecorder::updatePlaybackData(QDataStream &dataStream) {
	
	if(mNetworksChanged) {
		//TODO check for missing neurons etc.
		syncWithListOfRecordedValues();
	}
	
	SimulationRecorder::updatePlaybackData(dataStream);
	
	/*
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
	*/
	
}

NeuralNetwork* NetworkSimulationRecorder::getNetworkByIdString(QList<NeuralNetwork*> networks, const QString &netId) {
	bool ok = true;
	int id = netId.toInt(&ok);
	if(!ok || id < 0 || id >= networks.size()) {
		Core::log("NetworkSimulationRecorder: Could not find network: ["
				+ netId + "]. Format: [Neurons,o]netId:neuronId,...", true);
		return 0;
	}
	return networks.at(id);
}


QList<Neuron*> NetworkSimulationRecorder::getNeurons(NeuralNetwork *net, const QString &neuronIds) {
	
	QList<Neuron*> neurons;
	if(net == 0) {
		return neurons;
	}
// 	//get network
// 	NeuralNetwork *net = 0;
// 	bool ok = true;
// 	int id = netId.toInt(&ok);
// 	if(!ok || id < 0 || id >= networks.size()) {
// 		Core::log("NetworkSimulationRecorder: Could not find network: ["
// 				+ netId + "]. Format: [Neurons,o]netId:neuronId,...", true);
// 		return neurons;
// 	}
// 	net = networks.at(id);

	//get neurons
	if(neuronIds.startsWith("*")) {
		neurons = net->getNeurons();
	}
	else {
		neurons = NeuralNetworkUtil::getNeuronsFromIdList(net, neuronIds);
	}
	return neurons;
}

QList<Synapse*> NetworkSimulationRecorder::getSynapses(NeuralNetwork *net, const QString &synapseIds) {
	QList<Synapse*> synapses;
	if(net == 0) {
		return synapses;
	}
	
// 	//get network
// 	bool ok = true;
// 	NeuralNetwork *net = 0;
// 	int id = netId.toInt(&ok);
// 	if(!ok || id < 0 || id >= networks.size()) {
// 		Core::log("NetworkSimulationRecorder: Could not find network: ["
// 				+ netId + "]. Format: [Neurons,o]netId:neuronId,...", true);
// 		return synapses;
// 	}
// 	net = networks.at(id);

	//get synapses
	if(synapseIds.startsWith("*")) {
		synapses = net->getSynapses();
	}
	else {
		synapses = NeuralNetworkUtil::getSynapsesFromIdList(net, synapseIds);
	}
	return synapses;
}

}


