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

#include "NeuralNetworkIOSimbaV3Xml.h"
#include "Network/Neuro.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "NeuralNetworkIO.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "SynapseFunction/SynapseFunction.h"
#include "TransferFunction/TransferFunctionASeriesTanh.h"
#include "ActivationFunction/ASeriesActivationFunction.h"
#include "SynapseFunction/ASeriesSynapseFunction.h"
#include "Network/Neuron.h"
#include "Core/Core.h"
#include "Value/Value.h"
#include "NeuralNetworkIOBytecode.h"
#include <QDebug>
#include <QDir>
#include <QTextStream>

namespace nerd {


/**
 * Try to export a modular A-Series network to a Simba File so that it is importable by Simba and NERD again.
 * 
 * @param fileName the Simba file to write to.
 * @param modNet the modular A-Series network.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkIOSimbaV3Xml::createASeriesFileFromNetwork(QString fileName, ModularNeuralNetwork *modNet, QString *errorMsg) {
	// Make a copy of the net because the names will be changed
	ModularNeuralNetwork *modNetCopy = dynamic_cast<ModularNeuralNetwork*>(modNet->createCopy());

	// First check if network is a valid A-Series network which means that it is exportable to the A-Series Bytecode
	if(!NeuralNetworkIOBytecode::checkIfNetworkIsExportable(modNetCopy, errorMsg)) {
		errorMsg->prepend("Network to export is not a valid A-Series network because it is not exportable to A-Series bytecode, error:\n");
		delete modNetCopy;
		return false;
	}

	// Now map the Input and Output Neurons from their SpinalChord-Address to the Simba names
	mapInputAndOutputNeuronsToSimbaNames(modNetCopy);

	// Assign a name to all unnamed hidden neurons and prepend the Accelboard-Module name to all hidden neurons (if this is not already the case)
	prepareHiddenNeuronNames(modNetCopy);

	// Copy all neurons and synapses from modNetCopy to a new neural network so that the neurons have the 
	//right order for Simba to NERD UDP communication.
	NeuralNetwork *net = copyNeuronsAndSynapsesToNewNet(modNetCopy, errorMsg);
	if(net == NULL) {
		errorMsg->prepend("Error during copy of synapses and neurons. Coding error: ");
		delete modNetCopy;
		return false;
	}

	// Save net to file
	bool ret = createFileFromNetwork(fileName, net, errorMsg);

	delete modNetCopy;
	return ret;	
}

/**
 * Creates a new neural net and copies all neurons and synapses from a given modular A-Series network 
 * to the new network so that the neurons habe the right order for Simba to NERD UDP communication.
 * 
 * @param modNet the modular A-Series network.
 * @return the new net.
 */
NeuralNetwork* NeuralNetworkIOSimbaV3Xml::copyNeuronsAndSynapsesToNewNet(ModularNeuralNetwork *modNet, QString *errorMsg) {
	// Create the new net.
	NeuralNetwork *net = new NeuralNetwork(ASeriesActivationFunction(), TransferFunctionASeriesTanh(), ASeriesSynapseFunction());

	// Create a list with the right order of the neurons.
	QList<QString> rightOrderNeuronNames;
	rightOrderNeuronNames << "ABSR/Waist/Angle" << "ABAR/Right/ShoulderYaw/Angle" << "ABAL/Left/ShoulderYaw/Angle" << "ABAR/Right/ShoulderPitch/Angle" << "ABAL/Left/ShoulderPitch/Angle" << "ABAR/Right/Elbow/Angle" << "ABAL/Left/Elbow/Angle" << "ABHR/Right/HipRoll/Angle" << "ABHL/Left/HipRoll/Angle" << "ABHR/Right/HipPitch/Angle" << "ABHL/Left/HipPitch/Angle" << "ABHR/Right/HipYaw/Angle" << "ABHL/Left/HipYaw/Angle" << "ABFR/Right/Knee/Angle" << "ABFL/Left/Knee/Angle" << "ABFR/Right/AnkleYaw/Angle" << "ABFL/Left/AnkleYaw/Angle" << "ABFR/Right/AnklePitch/Angle" << "ABFL/Left/AnklePitch/Angle" << "ABSR/Head/Pan/Angle" << "ABSR/Head/Tilt/Angle" << "ABFR/Right/Foot/AccelX" << "ABFR/Right/Foot/AccelY" << "ABHR/Right/Leg/AccelX" << "ABHR/Right/Leg/AccelY" << "ABFR/Right/Foot/AccelX" << "ABFR/Right/Foot/AccelY" << "ABHR/Right/Leg/AccelX" << "ABHR/Right/Leg/AccelY" << "ABSR/Right/Shoulder/AccelX" << "ABSR/Right/Shoulder/AccelY" << "ABAR/Right/Arm/AccelX" << "ABAR/Right/Arm/AccelY" << "ABFL/Left/Foot/AccelX" << "ABFL/Left/Foot/AccelY" << "ABHL/Left/Leg/AccelX" << "ABHL/Left/Leg/AccelY" << "ABML/Left/Shoulder/AccelX" << "ABML/Left/Shoulder/AccelY" << "ABAL/Left/Arm/AccelX" << "ABAL/Left/Arm/AccelY";
	rightOrderNeuronNames << "ABSR/Waist/DesiredAngle" << "ABSR/Waist/DesiredTorque" << "ABAR/Right/ShoulderYaw/DesiredAngle" << "ABAR/Right/ShoulderYaw/DesiredTorque" << "ABAL/Left/ShoulderYaw/DesiredAngle" << "ABAL/Left/ShoulderYaw/DesiredTorque" << "ABAR/Right/ShoulderPitch/DesiredAngle" << "ABAR/Right/ShoulderPitch/DesiredTorque" << "ABAL/Left/ShoulderPitch/DesiredAngle" << "ABAL/Left/ShoulderPitch/DesiredTorque" << "ABAR/Right/Elbow/DesiredAngle" << "ABAR/Right/Elbow/DesiredTorque" << "ABAL/Left/Elbow/DesiredAngle" << "ABAL/Left/Elbow/DesiredTorque" << "ABHR/Right/HipRoll/DesiredAngle" << "ABHR/Right/HipRoll/DesiredTorque" << "ABHL/Left/HipRoll/DesiredAngle" << "ABHL/Left/HipRoll/DesiredTorque" << "ABHR/Right/HipPitch/DesiredAngle" << "ABHR/Right/HipPitch/DesiredTorque" << "ABHL/Left/HipPitch/DesiredAngle" << "ABHL/Left/HipPitch/DesiredTorque" <<  "ABHR/Right/HipYaw/DesiredAngle" << "ABHR/Right/HipYaw/DesiredTorque" << "ABHL/Left/HipYaw/DesiredAngle" << "ABHL/Left/HipYaw/DesiredTorque" << "ABFR/Right/Knee/DesiredAngle" << "ABFR/Right/Knee/DesiredTorque" << "ABFL/Left/Knee/DesiredAngle" << "ABFL/Left/Knee/DesiredTorque" << "ABFR/Right/AnkleYaw/DesiredAngle" << "ABFR/Right/AnkleYaw/DesiredTorque" << "ABFL/Left/AnkleYaw/DesiredAngle" << "ABFL/Left/AnkleYaw/DesiredTorque" << "ABFR/Right/AnklePitch/DesiredAngle" << "ABFR/Right/AnklePitch/DesiredTorque" << "ABFL/Left/AnklePitch/DesiredAngle" << "ABFL/Left/AnklePitch/DesiredTorque" << "ABSR/Head/Pan/DesiredAngle" << "ABSR/Head/Pan/DesiredTorque" << "ABSR/Head/Tilt/DesiredAngle" << "ABSR/Head/Tilt/DesiredTorque";

	// Get modNetNeurons
	QList<Neuron*> modNetNeurons = modNet->getNeurons();
	QListIterator<Neuron*> modNetNeuronIt(modNetNeurons);

	// Iterate over all rightOrderNeuronNames and put neuron with that name from modNet to net
	QMap<Neuron*,Neuron*> modNetToNetNeuronMap;
	while(!rightOrderNeuronNames.isEmpty()) {
		QString name = rightOrderNeuronNames.first();
		// Get neuron with that name and put it to new net
		modNetNeuronIt.toFront();
		bool found = false;
		while(modNetNeuronIt.hasNext()) {
			Neuron *modNetNeuron = modNetNeuronIt.next();
			if(modNetNeuron->hasProperty(Neuron::NEURON_TYPE_INPUT) || modNetNeuron->hasProperty(Neuron::NEURON_TYPE_OUTPUT)) {
				if(modNetNeuron->getNameValue().get() == name) {
					Neuron *newNeuron = modNetNeuron->createCopy();
					net->addNeuron(newNeuron);
					modNetToNetNeuronMap[modNetNeuron] = newNeuron;
					modNetNeurons.removeAll(modNetNeuron);
					found = true;
					break;
				}
			}
		}
		if(!found) {
			delete net;
			*errorMsg = QString("Input or output neuron with name %1 not found in original net!").arg(name);
			return NULL;
		}
		rightOrderNeuronNames.removeAll(name);
	}
	// Transfer all hidden neurons
	while(!modNetNeurons.isEmpty()) {
		Neuron *modNetNeuron = modNetNeurons.first();
		if(modNetNeuron->hasProperty(Neuron::NEURON_TYPE_INPUT) || modNetNeuron->hasProperty(Neuron::NEURON_TYPE_OUTPUT)) {
			*errorMsg = QString("Input or output neuron with name %1 left after copy of neurons").arg(modNetNeuron->getNameValue().get());
			delete net;
			return NULL;
		}
		Neuron *newNeuron = modNetNeuron->createCopy();
		net->addNeuron(newNeuron);
		modNetToNetNeuronMap[modNetNeuron] = newNeuron;
		modNetNeurons.removeAll(modNetNeuron);
	}

	// Transfer synapses from modNet to net
	QList<Synapse*> modNetSynapses = modNet->getSynapses();
	QListIterator<Synapse*> modNetSynapseIt(modNetSynapses);
	while(modNetSynapseIt.hasNext()) {
		Synapse *modNetSynapse = modNetSynapseIt.next();
		Neuron *modNetSource = modNetSynapse->getSource();
		Neuron *modNetTarget = dynamic_cast<Neuron*>(modNetSynapse->getTarget());
		if(modNetTarget == NULL) {
			*errorMsg = "Synapse with a synapse as target found!";
			delete net;
			return NULL;
		}
		Neuron *newSource = modNetToNetNeuronMap[modNetSource];
		Neuron *newTarget = modNetToNetNeuronMap[modNetTarget];
		Synapse::createSynapse(newSource, newTarget, modNetSynapse->getStrengthValue().get(), ASeriesSynapseFunction());
	}

	return net;
}

/**
 * Assign a name to all unnamed hidden neurons of a given modular A-Series network and prepend the Accelboard module name to all hidden neurons (if this is not already the case).
 * 
 * @param modNet the modular A-Series network.
 */
void NeuralNetworkIOSimbaV3Xml::prepareHiddenNeuronNames(ModularNeuralNetwork *modNet) {
	// Iterate over all modules
	QList<NeuroModule*> modules = modNet->getNeuroModules();
	QListIterator<NeuroModule*> moduleIt(modules);
	while(moduleIt.hasNext()) {
		NeuroModule *module = moduleIt.next();
		QString accelboardPrefix = module->getProperty("Accelboard").append("/");
		// Iterate over all module neurons
		QList<Neuron*> moduleNeurons = module->getAllEnclosedNeurons();
		QListIterator<Neuron*> moduleNeuronIt(moduleNeurons);
		while(moduleNeuronIt.hasNext()) {
			int hiddenNeuronIndex = 0;
			Neuron *moduleNeuron = moduleNeuronIt.next();
			if(!(moduleNeuron->hasProperty(Neuron::NEURON_TYPE_INPUT) || moduleNeuron->hasProperty(Neuron::NEURON_TYPE_OUTPUT))) {
				QString moduleNeuronName = moduleNeuron->getNameValue().get();
				if(moduleNeuronName.isEmpty()) {
					QString newName = accelboardPrefix.append(QString("Hidden%1").arg(hiddenNeuronIndex));
					moduleNeuron->getNameValue().set(newName);
					hiddenNeuronIndex++;
				} else {
					if(!moduleNeuronName.startsWith(accelboardPrefix)) {
						QString newName = accelboardPrefix + moduleNeuronName;
						moduleNeuron->getNameValue().set(newName);
					}
				}
			}
		}
	}
}

/**
 * Maps the SpinalChordAddress from the input and output neurons of the given modular A-Series network to the Simba A-Series neuron names.
 * 
 * @param modNet the modular A-Series network
 */
void NeuralNetworkIOSimbaV3Xml::mapInputAndOutputNeuronsToSimbaNames(ModularNeuralNetwork *modNet) {
	QMap<QString, QString> neuronAddressToNameMap;

	neuronAddressToNameMap["26"] = "ABSR/Waist/Angle";
	neuronAddressToNameMap["58"] = "ABAR/Right/ShoulderYaw/Angle";
	neuronAddressToNameMap["42"] = "ABAL/Left/ShoulderYaw/Angle";
	neuronAddressToNameMap["59"] = "ABAR/Right/ShoulderPitch/Angle";
	neuronAddressToNameMap["43"] = "ABAL/Left/ShoulderPitch/Angle";
	neuronAddressToNameMap["60"] = "ABAR/Right/Elbow/Angle";
	neuronAddressToNameMap["44"] = "ABAL/Left/Elbow/Angle";
	neuronAddressToNameMap["90"] = "ABHR/Right/HipRoll/Angle";
	neuronAddressToNameMap["74"] = "ABHL/Left/HipRoll/Angle";
	neuronAddressToNameMap["92"] = "ABHR/Right/HipYaw/Angle";
	neuronAddressToNameMap["76"] = "ABHL/Left/HipYaw/Angle";
	neuronAddressToNameMap["91"] = "ABHR/Right/HipPitch/Angle";
	neuronAddressToNameMap["75"] = "ABHL/Left/HipPitch/Angle";
	neuronAddressToNameMap["122"] = "ABFR/Right/Knee/Angle";
	neuronAddressToNameMap["106"] = "ABFL/Left/Knee/Angle";
	neuronAddressToNameMap["124"] = "ABFR/Right/AnklePitch/Angle";
	neuronAddressToNameMap["108"] = "ABFL/Left/AnklePitch/Angle";
	neuronAddressToNameMap["123"] = "ABFR/Right/AnkleYaw/Angle";
	neuronAddressToNameMap["107"] = "ABFL/Left/AnkleYaw/Angle";
	neuronAddressToNameMap["27"] = "ABSR/Head/Pan/Angle";
	neuronAddressToNameMap["28"] = "ABSR/Head/Tilt/Angle";
	neuronAddressToNameMap["3"] = "ABML/Left/Shoulder/AccelX";
	neuronAddressToNameMap["4"] = "ABML/Left/Shoulder/AccelY";
	neuronAddressToNameMap["19"] = "ABSR/Right/Shoulder/AccelX";
	neuronAddressToNameMap["20"] = "ABSR/Right/Shoulder/AccelY";
	neuronAddressToNameMap["35"] = "ABAL/Left/Arm/AccelX";
	neuronAddressToNameMap["36"] = "ABAL/Left/Arm/AccelY";
	neuronAddressToNameMap["51"] = "ABAR/Right/Arm/AccelX";
	neuronAddressToNameMap["52"] = "ABAR/Right/Arm/AccelY";
	neuronAddressToNameMap["67"] = "ABHL/Left/Leg/AccelX";
	neuronAddressToNameMap["68"] = "ABHL/Left/Leg/AccelY";
	neuronAddressToNameMap["83"] = "ABHR/Right/Leg/AccelX";
	neuronAddressToNameMap["84"] = "ABHR/Right/Leg/AccelY";
	neuronAddressToNameMap["99"] = "ABFL/Left/Foot/AccelX";
	neuronAddressToNameMap["100"] = "ABFL/Left/Foot/AccelY";
	neuronAddressToNameMap["115"] = "ABFR/Right/Foot/AccelX";
	neuronAddressToNameMap["116"] = "ABFR/Right/Foot/AccelY";
	neuronAddressToNameMap["29"] = "ABSR/Waist/DesiredAngle";
	neuronAddressToNameMap["16"] = "ABSR/Waist/DesiredTorque";
	neuronAddressToNameMap["61"] = "ABAR/Right/ShoulderYaw/DesiredAngle";
	neuronAddressToNameMap["48"] = "ABAR/Right/ShoulderYaw/DesiredTorque";
	neuronAddressToNameMap["45"] = "ABAL/Left/ShoulderYaw/DesiredAngle";
	neuronAddressToNameMap["32"] = "ABAL/Left/ShoulderYaw/DesiredTorque";
	neuronAddressToNameMap["62"] = "ABAR/Right/ShoulderPitch/DesiredAngle";
	neuronAddressToNameMap["49"] = "ABAR/Right/ShoulderPitch/DesiredTorque";
	neuronAddressToNameMap["46"] = "ABAL/Left/ShoulderPitch/DesiredAngle";
	neuronAddressToNameMap["33"] = "ABAL/Left/ShoulderPitch/DesiredTorque";
	neuronAddressToNameMap["63"] = "ABAR/Right/Elbow/DesiredAngle";
	neuronAddressToNameMap["50"] = "ABAR/Right/Elbow/DesiredTorque";
	neuronAddressToNameMap["47"] = "ABAL/Left/Elbow/DesiredAngle";
	neuronAddressToNameMap["34"] = "ABAL/Left/Elbow/DesiredTorque";
	neuronAddressToNameMap["93"] = "ABHR/Right/HipRoll/DesiredAngle";
	neuronAddressToNameMap["80"] = "ABHR/Right/HipRoll/DesiredTorque";
	neuronAddressToNameMap["77"] = "ABHL/Left/HipRoll/DesiredAngle";
	neuronAddressToNameMap["64"] = "ABHL/Left/HipRoll/DesiredTorque";
	neuronAddressToNameMap["95"] = "ABHR/Right/HipYaw/DesiredAngle";
	neuronAddressToNameMap["82"] = "ABHR/Right/HipYaw/DesiredTorque";
	neuronAddressToNameMap["79"] = "ABHL/Left/HipYaw/DesiredAngle";
	neuronAddressToNameMap["66"] = "ABHL/Left/HipYaw/DesiredTorque";
	neuronAddressToNameMap["94"] = "ABHR/Right/HipPitch/DesiredAngle";
	neuronAddressToNameMap["81"] = "ABHR/Right/HipPitch/DesiredTorque";
	neuronAddressToNameMap["78"] = "ABHL/Left/HipPitch/DesiredAngle";
	neuronAddressToNameMap["65"] = "ABHL/Left/HipPitch/DesiredTorque";
	neuronAddressToNameMap["125"] = "ABFR/Right/Knee/DesiredAngle";
	neuronAddressToNameMap["112"] = "ABFR/Right/Knee/DesiredTorque";
	neuronAddressToNameMap["109"] = "ABFL/Left/Knee/DesiredAngle";
	neuronAddressToNameMap["96"] = "ABFL/Left/Knee/DesiredTorque";
	neuronAddressToNameMap["127"] = "ABFR/Right/AnklePitch/DesiredAngle";
	neuronAddressToNameMap["114"] = "ABFR/Right/AnklePitch/DesiredTorque";
	neuronAddressToNameMap["111"] = "ABFL/Left/AnklePitch/DesiredAngle";
	neuronAddressToNameMap["98"] = "ABFL/Left/AnklePitch/DesiredTorque";
	neuronAddressToNameMap["126"] = "ABFR/Right/AnkleYaw/DesiredAngle";
	neuronAddressToNameMap["113"] = "ABFR/Right/AnkleYaw/DesiredTorque";
	neuronAddressToNameMap["110"] = "ABFL/Left/AnkleYaw/DesiredAngle";
	neuronAddressToNameMap["97"] = "ABFL/Left/AnkleYaw/DesiredTorque";
	neuronAddressToNameMap["30"] = "ABSR/Head/Pan/DesiredAngle";
	neuronAddressToNameMap["17"] = "ABSR/Head/Pan/DesiredTorque";
	neuronAddressToNameMap["31"] = "ABSR/Head/Tilt/DesiredAngle";
	neuronAddressToNameMap["18"] = "ABSR/Head/Tilt/DesiredTorque";

	QList<Neuron*> inputAndOutputNeurons = modNet->getInputNeurons() + modNet->getOutputNeurons();
	QListIterator<Neuron*> inputAndOutputNeuronsIt(inputAndOutputNeurons);
	while(inputAndOutputNeuronsIt.hasNext()) {
		Neuron *neuron = inputAndOutputNeuronsIt.next();
		QString neuronAddress = neuron->getProperty(NeuralNetworkIOBytecode::SPINAL_CORD_ADDRESS_PROPERTY);
		QString newName = neuronAddressToNameMap[neuronAddress];
		// Set new name
		neuron->getNameValue().set(newName);
	}
}

/**
 * Try to load an A-Series Simba network into NERD. The Input and Output Neurons of the Simba net 
 * need to have appropriate names to assign them to the appropriate NERD Modules. The Hidden Neurons 
 * are assigned to the Modules by the beginning of their name (e.g. ABAL/Hidden1).
 * 
 * @param fileName the Simba file to read from.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @param warnings returns warnings if the ModularNeuralNetwork was created with some restrictions.
 * @return the created ModularNeuralNetwork or NULL if the function fails.
 */
ModularNeuralNetwork* NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile(QString fileName, QString *errorMsg, QList<QString> *warnings) {
	// Load the net as non modular network
	NeuralNetwork *net = createNetFromFile(fileName, errorMsg, warnings);
	if(net == NULL) {
		return NULL;
	}

	// Create a modular A-Series network from the old network.
	if(net->getDefaultTransferFunction()->getName() != "tanh") {
		warnings->append(QString("DefaultTransferFunction of the net is not supported by the ASeries. Changed to tanh!"));
	}
	if(net->getDefaultActivationFunction()->getName() != "AdditiveTD") {
		warnings->append(QString("DefaultUpdateFunction of the net is not supported by the ASeries. Changed to AdditiveTD!"));
	}
	if(net->getDefaultSynapseFunction()->getName() != "SimpleUpdateFunction") {
		warnings->append(QString("DefaultSynapseUpdateFunction of the net is not supported by the ASeries. Changed to SimpleUpdateFunction!"));
	}
	ModularNeuralNetwork *modNet = new ModularNeuralNetwork(ASeriesActivationFunction(), TransferFunctionASeriesTanh(), ASeriesSynapseFunction());

	// Transfer the neurons and synapses from net to modNet.
	if(!transferNeuronsAndSynapses(net, modNet, errorMsg, warnings)) {
		delete net;
		delete modNet;
		return NULL;
	}
	delete net;

	// Create the Accelboard modules and assign the neurons to the modules according to the beginning of their name (e.g. /ABAL/Hidden1).
	if(!createAccelboardModulesAndAssignNeurons(modNet, errorMsg)) {
		delete modNet;
		return NULL;
	}

	// Assign the SpinalChordAddresses to the net neurons according to their name.
	if(!assignSpinalChordAddressesToNeurons(modNet, errorMsg)) {
		delete modNet;
		return NULL;
	}

	// Check if Modular Neural Network is exportable to the A-Series
	if(!NeuralNetworkIOBytecode::checkIfNetworkIsExportable(modNet, errorMsg)) {
		errorMsg->prepend("Imported network is not exportable to A-Series bytecode, error:\n");
		delete modNet;
		return NULL;
	}

	// Map Simba neuron names of the given modular A-Series net to NERD neuron names.
	mapNeuronNames(modNet);

	return modNet;
}

/**
 * Transfers the neurons and synapses from a neural net to the new A-Series modular neural net.
 * 
 * @param net the neural net.
 * @param modNet the new A-Series modular neural net.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @param warnings returns warnings if there are some restrictions.
 * @return true if transfer was successful, otherwise false.
 */
bool NeuralNetworkIOSimbaV3Xml::transferNeuronsAndSynapses(NeuralNetwork *net, ModularNeuralNetwork *modNet, QString *errorMsg, QList<QString> *warnings) {
	// Transfer the neurons of the old network to the new one
	QMap<Neuron*, Neuron*> netNeuronToModNetNeuronMap;
	bool neuronTfFault = false;
	bool neuronAfFault = false;
	QList<Neuron*> netNeurons = net->getNeurons();
	QListIterator<Neuron*> netNeuronIt(netNeurons);
	while(netNeuronIt.hasNext()) {
		Neuron *netNeuron = netNeuronIt.next();
		if(netNeuron->getTransferFunction()->getName() != "tanh") {
			neuronTfFault = true;
		}
		if(netNeuron->getActivationFunction()->getName() != "AdditiveTD") {
			neuronAfFault = true;
		}
		Neuron *modNetNeuron = netNeuron->createCopy();
		modNetNeuron->setActivationFunction(ASeriesActivationFunction());
		modNetNeuron->setTransferFunction(TransferFunctionASeriesTanh());
		modNet->addNeuron(modNetNeuron);
		netNeuronToModNetNeuronMap[netNeuron] = modNetNeuron;
	}
	if(neuronTfFault) {
		warnings->append(QString("At least one neuron of the net has a TransferFunction not supported by the ASeries. Changed to tanh!"));
	}
	if(neuronAfFault) {
		warnings->append(QString("At least one neuron of the net has an UpdateFunction not supported by the ASeries. Changed to AdditiveTD!"));
	}

	// Transfer the synapses of the old network to the new one
	bool sfFault = false;
	QList<Synapse*> netSynapses = net->getSynapses();
	QListIterator<Synapse*> netSynapsesIt(netSynapses);
	while(netSynapsesIt.hasNext()) {
		Synapse *netSynapse = netSynapsesIt.next();
		Neuron *netTargetNeuron = dynamic_cast<Neuron*>(netSynapse->getTarget());
		if(netTargetNeuron == NULL) {
			*errorMsg = QString("There is a synapse with a synapse as target in the net!");
			return false;
		}
		Neuron *netSourceNeuron = netSynapse->getSource();
		if(netSynapse->getSynapseFunction()->getName() != "SimpleUpdateFunction") {
			sfFault = true;
		}
		Neuron *modNetTargetNeuron = netNeuronToModNetNeuronMap[netTargetNeuron];
		Neuron *modNetSourceNeuron = netNeuronToModNetNeuronMap[netSourceNeuron];
		Synapse::createSynapse(modNetSourceNeuron, modNetTargetNeuron, netSynapse->getStrengthValue().get(), ASeriesSynapseFunction());
	}
	if(sfFault) {
		warnings->append(QString("At least one synapse of the net has an SynapseUpdateFunction not supported by the ASeries. Changed to SimpleUpdateFunction!"));
	}
	return true;
}


/**
 * Create the A-Series accelboard modules in the given A-Series modular neural net and assign the neurons of the net to these modules according to the beginning of their names.
 * 
 * @param modNet the A-Series modular neural net.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkIOSimbaV3Xml::createAccelboardModulesAndAssignNeurons(ModularNeuralNetwork *modNet, QString *errorMsg) {
	// Create accelboard modules
	QMap<QString, NeuroModule*> nameToModuleMap;
	QList<QString> accelboardNames;
	accelboardNames << "ABML" << "ABSR" << "ABAL" << "ABAR" << "ABHL" << "ABHR" << "ABFL" << "ABFR";
	QListIterator<QString> accelboardNamesIt(accelboardNames);
	while(accelboardNamesIt.hasNext()) {
		QString accelboardName = accelboardNamesIt.next();
		NeuroModule *module = new NeuroModule(accelboardName);
		nameToModuleMap[accelboardName] = module;
		module->setProperty("Accelboard", accelboardName);
		modNet->addNeuronGroup(module);
	}

	// Assign neurons to modules
	QList<Neuron*> neurons = modNet->getNeurons();
	QListIterator<Neuron*> neuronIt(neurons);
	while(neuronIt.hasNext()) {
		Neuron *neuron = neuronIt.next();
		QString neuronName = neuron->getNameValue().get();
		// Extract accelboard name from neuron name
		QString neuronAccelboardName = neuronName.left(neuronName.indexOf("/"));
		if(!accelboardNames.contains(neuronAccelboardName)) {
			*errorMsg = QString("Every neuron name in the net has to start with an accelboard name (e.g. ABAL/Hidden1). There is a neuron in the net with the name %1!").arg(neuronName);
			return false;
		}
		// Assign neuron to module with neuronAccelboardName
		NeuroModule *module = nameToModuleMap[neuronAccelboardName];
		module->addNeuron(neuron);
	}

	return true;
}

/**
 * Assigns the A-Series SpinalChordAddresses to the input and output neurons of the modular A-Series net according to their name. Checks if all input and output neurons are valid and only exist once.
 * 
 * @param modNet the modular A-Series net.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkIOSimbaV3Xml::assignSpinalChordAddressesToNeurons(ModularNeuralNetwork *modNet, QString *errorMsg) {
	// Make a map of all expected input and output neurons and their corresponding A-Series-SpinalChord-Address
	QMap<QString, QString> inputNeuronAddressMap;
	QMap<QString, QString> outputNeuronAddressMap;
	
	inputNeuronAddressMap["ABSR/Waist/Angle"] = "26";
	inputNeuronAddressMap["ABAR/Right/ShoulderYaw/Angle"] = "58";
	inputNeuronAddressMap["ABAL/Left/ShoulderYaw/Angle"] = "42";
	inputNeuronAddressMap["ABAR/Right/ShoulderPitch/Angle"] = "59";
	inputNeuronAddressMap["ABAL/Left/ShoulderPitch/Angle"] = "43";
	inputNeuronAddressMap["ABAR/Right/Elbow/Angle"] = "60";
	inputNeuronAddressMap["ABAL/Left/Elbow/Angle"] = "44";
	inputNeuronAddressMap["ABHR/Right/HipRoll/Angle"] = "90";
	inputNeuronAddressMap["ABHL/Left/HipRoll/Angle"] = "74";
	inputNeuronAddressMap["ABHR/Right/HipYaw/Angle"] = "92";
	inputNeuronAddressMap["ABHL/Left/HipYaw/Angle"] = "76";
	inputNeuronAddressMap["ABHR/Right/HipPitch/Angle"] = "91";
	inputNeuronAddressMap["ABHL/Left/HipPitch/Angle"] = "75";
	inputNeuronAddressMap["ABFR/Right/Knee/Angle"] = "122";
	inputNeuronAddressMap["ABFL/Left/Knee/Angle"] = "106";
	inputNeuronAddressMap["ABFR/Right/AnklePitch/Angle"] = "124";
	inputNeuronAddressMap["ABFL/Left/AnklePitch/Angle"] = "108";
	inputNeuronAddressMap["ABFR/Right/AnkleYaw/Angle"] = "123";
	inputNeuronAddressMap["ABFL/Left/AnkleYaw/Angle"] = "107";
	inputNeuronAddressMap["ABSR/Head/Pan/Angle"] = "27";
	inputNeuronAddressMap["ABSR/Head/Tilt/Angle"] = "28";
	inputNeuronAddressMap["ABML/Left/Shoulder/AccelX"] = "3";
	inputNeuronAddressMap["ABML/Left/Shoulder/AccelY"] = "4";
	inputNeuronAddressMap["ABSR/Right/Shoulder/AccelX"] = "19";
	inputNeuronAddressMap["ABSR/Right/Shoulder/AccelY"] = "20";
	inputNeuronAddressMap["ABAL/Left/Arm/AccelX"] = "35";
	inputNeuronAddressMap["ABAL/Left/Arm/AccelY"] = "36";
	inputNeuronAddressMap["ABAR/Right/Arm/AccelX"] = "51";
	inputNeuronAddressMap["ABAR/Right/Arm/AccelY"] = "52";
	inputNeuronAddressMap["ABHL/Left/Leg/AccelX"] = "67";
	inputNeuronAddressMap["ABHL/Left/Leg/AccelY"] = "68";
	inputNeuronAddressMap["ABHR/Right/Leg/AccelX"] = "83";
	inputNeuronAddressMap["ABHR/Right/Leg/AccelY"] = "84";
	inputNeuronAddressMap["ABFL/Left/Foot/AccelX"] = "99";
	inputNeuronAddressMap["ABFL/Left/Foot/AccelY"] = "100";
	inputNeuronAddressMap["ABFR/Right/Foot/AccelX"] = "115";
	inputNeuronAddressMap["ABFR/Right/Foot/AccelY"] = "116";

	outputNeuronAddressMap["ABSR/Waist/DesiredAngle"] = "29";
	outputNeuronAddressMap["ABSR/Waist/DesiredTorque"] = "16";
	outputNeuronAddressMap["ABAR/Right/ShoulderYaw/DesiredAngle"] = "61";
	outputNeuronAddressMap["ABAR/Right/ShoulderYaw/DesiredTorque"] = "48";
	outputNeuronAddressMap["ABAL/Left/ShoulderYaw/DesiredAngle"] = "45";
	outputNeuronAddressMap["ABAL/Left/ShoulderYaw/DesiredTorque"] = "32";
	outputNeuronAddressMap["ABAR/Right/ShoulderPitch/DesiredAngle"] = "62";
	outputNeuronAddressMap["ABAR/Right/ShoulderPitch/DesiredTorque"] = "49";
	outputNeuronAddressMap["ABAL/Left/ShoulderPitch/DesiredAngle"] = "46";
	outputNeuronAddressMap["ABAL/Left/ShoulderPitch/DesiredTorque"] = "33";
	outputNeuronAddressMap["ABAR/Right/Elbow/DesiredAngle"] = "63";
	outputNeuronAddressMap["ABAR/Right/Elbow/DesiredTorque"] = "50";
	outputNeuronAddressMap["ABAL/Left/Elbow/DesiredAngle"] = "47";
	outputNeuronAddressMap["ABAL/Left/Elbow/DesiredTorque"] = "34";
	outputNeuronAddressMap["ABHR/Right/HipRoll/DesiredAngle"] = "93";
	outputNeuronAddressMap["ABHR/Right/HipRoll/DesiredTorque"] = "80";
	outputNeuronAddressMap["ABHL/Left/HipRoll/DesiredAngle"] = "77";
	outputNeuronAddressMap["ABHL/Left/HipRoll/DesiredTorque"] = "64";
	outputNeuronAddressMap["ABHR/Right/HipYaw/DesiredAngle"] = "95";
	outputNeuronAddressMap["ABHR/Right/HipYaw/DesiredTorque"] = "82";
	outputNeuronAddressMap["ABHL/Left/HipYaw/DesiredAngle"] = "79";
	outputNeuronAddressMap["ABHL/Left/HipYaw/DesiredTorque"] = "66";
	outputNeuronAddressMap["ABHR/Right/HipPitch/DesiredAngle"] = "94";
	outputNeuronAddressMap["ABHR/Right/HipPitch/DesiredTorque"] = "81";
	outputNeuronAddressMap["ABHL/Left/HipPitch/DesiredAngle"] = "78";
	outputNeuronAddressMap["ABHL/Left/HipPitch/DesiredTorque"] = "65";
	outputNeuronAddressMap["ABFR/Right/Knee/DesiredAngle"] = "125";
	outputNeuronAddressMap["ABFR/Right/Knee/DesiredTorque"] = "112";
	outputNeuronAddressMap["ABFL/Left/Knee/DesiredAngle"] = "109";
	outputNeuronAddressMap["ABFL/Left/Knee/DesiredTorque"] = "96";
	outputNeuronAddressMap["ABFR/Right/AnklePitch/DesiredAngle"] = "127";
	outputNeuronAddressMap["ABFR/Right/AnklePitch/DesiredTorque"] = "114";
	outputNeuronAddressMap["ABFL/Left/AnklePitch/DesiredAngle"] = "111";
	outputNeuronAddressMap["ABFL/Left/AnklePitch/DesiredTorque"] = "98";
	outputNeuronAddressMap["ABFR/Right/AnkleYaw/DesiredAngle"] = "126";
	outputNeuronAddressMap["ABFR/Right/AnkleYaw/DesiredTorque"] = "113";
	outputNeuronAddressMap["ABFL/Left/AnkleYaw/DesiredAngle"] = "110";
	outputNeuronAddressMap["ABFL/Left/AnkleYaw/DesiredTorque"] = "97";
	outputNeuronAddressMap["ABSR/Head/Pan/DesiredAngle"] = "30";
	outputNeuronAddressMap["ABSR/Head/Pan/DesiredTorque"] = "17";
	outputNeuronAddressMap["ABSR/Head/Tilt/DesiredAngle"] = "31";
	outputNeuronAddressMap["ABSR/Head/Tilt/DesiredTorque"] = "18";

	// Assign all addresses to input neurons
	{
		// Get all input neurons
		QList<Neuron*> inputNeurons = modNet->getInputNeurons();
		QListIterator<Neuron*> inputNeuronIt(inputNeurons);
		// Assign addresses
		QMapIterator<QString, QString> inputNeuronAddressMapIt(inputNeuronAddressMap);
		while(inputNeuronAddressMapIt.hasNext()) {
			inputNeuronAddressMapIt.next();
			QString inputNeuronName = inputNeuronAddressMapIt.key();
			QString inputNeuronAddress = inputNeuronAddressMapIt.value();
			// Find input neuron
			bool inputNeuronFound = false;
			inputNeuronIt.toFront();
			while(inputNeuronIt.hasNext()) {
				Neuron *inputNeuron = inputNeuronIt.next();
				if(inputNeuronName == inputNeuron->getNameValue().get()) {
					// Assign address
					inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CORD_ADDRESS_PROPERTY, inputNeuronAddress);
					inputNeuronFound = true;
					break;
				}
			}
			if(!inputNeuronFound) {
				*errorMsg = QString("At least one input neuron missing. Input neuron with name %1 not found!").arg(inputNeuronName);
				return false;
			}
		}
		// Check if all input neurons have an assigned address
		inputNeuronIt.toFront();
		while(inputNeuronIt.hasNext()) {
			Neuron *inputNeuron = inputNeuronIt.next();
			if(!inputNeuron->hasProperty(NeuralNetworkIOBytecode::SPINAL_CORD_ADDRESS_PROPERTY)) {
				*errorMsg = QString("Input neuron has an unknown name or name is assigned twice: %1!").arg(inputNeuron->getNameValue().get());
				return false;
			}
		}
	}

	// Assign all addresses to output neurons
	{
		// Get all output neurons
		QList<Neuron*> outputNeurons = modNet->getOutputNeurons();
		QListIterator<Neuron*> outputNeuronIt(outputNeurons);
		// Assign addresses
		QMapIterator<QString, QString> outputNeuronAddressMapIt(outputNeuronAddressMap);
		while(outputNeuronAddressMapIt.hasNext()) {
			outputNeuronAddressMapIt.next();
			QString outputNeuronName = outputNeuronAddressMapIt.key();
			QString outputNeuronAddress = outputNeuronAddressMapIt.value();
			// Find output neuron
			bool outputNeuronFound = false;
			outputNeuronIt.toFront();
			while(outputNeuronIt.hasNext()) {
				Neuron *outputNeuron = outputNeuronIt.next();
				if(outputNeuronName == outputNeuron->getNameValue().get()) {
					// Assign address
					outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CORD_ADDRESS_PROPERTY, outputNeuronAddress);
					outputNeuronFound = true;
					break;
				}
			}
			if(!outputNeuronFound) {
				*errorMsg = QString("At least one output neuron missing. Output neuron with name %1 not found!").arg(outputNeuronName);
				return false;
			}
		}
		// Check if all output neurons have an assigned address
		outputNeuronIt.toFront();
		while(outputNeuronIt.hasNext()) {
			Neuron *outputNeuron = outputNeuronIt.next();
			if(!outputNeuron->hasProperty(NeuralNetworkIOBytecode::SPINAL_CORD_ADDRESS_PROPERTY)) {
				*errorMsg = QString("Output neuron has an unknown name or name is assigned twice: %1!").arg(outputNeuron->getNameValue().get());
				return false;
			}
		}
	}

	return true;
}

/**
 * Map Simba neuron names of the given modular A-Series net to NERD neuron names. Actually all 
 * leading accelboard names and the / are cut off.
 * 
 * @param modNet the modular A-Series net.
 */
void NeuralNetworkIOSimbaV3Xml::mapNeuronNames(ModularNeuralNetwork *modNet) {
	QList<Neuron*> neurons = modNet->getNeurons();
	QListIterator<Neuron*> neuronIt(neurons);
	while(neuronIt.hasNext()) {
		Neuron *neuron = neuronIt.next();
		QString neuronName = neuron->getNameValue().get();
		// Cut off the accelboard name and the /
		QString newNeuronName = neuronName.right(neuronName.size() - neuronName.indexOf("/") - 1);
		neuron->getNameValue().set(newNeuronName);
	}
}







/**
 * Try to load a Simba network into NERD.
 * 
 * @param fileName the Simba file to read from.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @param warnings returns warnings if the ModularNeuralNetwork was created with some restrictions.
 * @return the created NeuralNetwork or NULL if the function fails.
 */
NeuralNetwork* NeuralNetworkIOSimbaV3Xml::createNetFromFile(QString fileName, QString *errorMsg, QList<QString> *warnings) {
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)) {
		if(errorMsg != NULL) {
			*errorMsg = QString("Cannot open file ").append(fileName).append(".");
		}
		file.close();
		return NULL;
	}

	QDomDocument document;
	QString errorStr;
	int errorLine, errorColumn;
	if(!document.setContent(&file, &errorStr, &errorLine, &errorColumn)) {
		if(errorMsg != NULL) {
			*errorMsg = QString("Parse error at line %1, column %2:\n%3")
								.arg(errorLine)
								.arg(errorColumn)
								.arg(errorStr);
		}
		file.close();
		return NULL;
	}

	file.close();

	QDomNodeList nodeList = document.elementsByTagName("net");
	QDomElement netElement;
	for(uint ui = 0; ui < nodeList.length(); ++ui) {
		QDomNode node = nodeList.item(ui);
		if(node.isElement()) {
			netElement = node.toElement();
			break;
		}
	}
	if(netElement.isNull()) {
		if(errorMsg != NULL) {
			*errorMsg = "No element <net> found!";
		}
		return NULL;
	}

	return createNetFromNetElement(netElement, errorMsg, warnings);

	return 0;
}

NeuralNetwork* NeuralNetworkIOSimbaV3Xml::createNetFromNetElement(const QDomElement &netElement, QString *errorMsg, QList<QString> *warnings) {
	QString version = netElement.attribute("version");
	if(version != "Simba XML V3.0") {
		if(errorMsg != NULL) {
			*errorMsg = "Version number not Simba XML V3.0!";
		}
		return NULL;
	}

	NeuralNetworkManager* neuralNetworkManager = Neuro::getNeuralNetworkManager();
	if(neuralNetworkManager == NULL) {
		Core::log("NeuralNetworkIOSimbaV3Xml::read : Failed to get NeuralNetworkManager!");
		return NULL;
	}

	NeuralNetwork *net = new ModularNeuralNetwork();

	addDefaultTransferFunctionToNetwork(net, netElement, neuralNetworkManager, warnings);
	addDefaultActivationFunctionToNetwork(net, netElement, neuralNetworkManager, warnings);
	addDefaultSynapseFunctionToNetwork(net, netElement, neuralNetworkManager, warnings);
	addNeuronsAndSynapsesToNetwork(net, netElement, neuralNetworkManager, warnings);

	net->adjustIdCounter();

	return net;	
}

void NeuralNetworkIOSimbaV3Xml::addDefaultTransferFunctionToNetwork(NeuralNetwork *net, const QDomElement &netElement, NeuralNetworkManager *neuralNetworkManager, QList<QString> *warnings) {

	QDomNodeList tfList = netElement.elementsByTagName("defaultTransferFunction");
	QDomElement tfElem;
	for(uint ui = 0; ui < tfList.length(); ++ui) {
		QDomNode node = tfList.item(ui);
		if(node.isElement()) {
			tfElem = node.toElement();
			break;
		}
	}

	if(!tfElem.isNull()) {
		TransferFunction *defaultTransferFunction = NULL;
		//look up correct transfer function.
		QString simbaV3Name = tfElem.attribute("name");
		QString nerdName = NeuralNetworkIO::getNERDTransferFunctionNameFromSimbaV3(simbaV3Name);

		QList<TransferFunction*> tfPrototypes = neuralNetworkManager->getTransferFunctionPrototypes();
		QListIterator<TransferFunction*> tfPrototypesIterator(tfPrototypes);
		while(tfPrototypesIterator.hasNext()) {
			TransferFunction *tfPrototype = tfPrototypesIterator.next();
			if(tfPrototype->getName() == nerdName) {
				defaultTransferFunction = tfPrototype->createCopy();
				break;
			}
		}
		if(defaultTransferFunction == NULL) {
			if(warnings != NULL) {
				warnings->append(QString("Unknown name \"%1\" of <defaultTransferFunction>. DefaultTransferFunction set to \"%2\"!").arg(simbaV3Name).arg(net->getDefaultTransferFunction()->getName()));
			}
			return;
		}
		//check for parameters.
		QDomNodeList paramList = tfElem.elementsByTagName("param");
		for(uint ui = 0; ui < paramList.length(); ++ui) {
			QDomNode node = paramList.item(ui);
			if(node.isElement()) {
				QDomElement param = node.toElement();
				QString name = param.attribute("name");
				if(name.isEmpty()) {
					if(warnings != NULL) {
						warnings->append(QString("Attribute \"name\" of parameter %1 of element <defaultTransferFunction> not found. Parameter ignored!").arg(ui + 1));
					}
					continue;
				}
				QString value = param.attribute("value");
				if(value.isEmpty()) {
					if(warnings != NULL) {
						warnings->append(QString("Attribute \"value\" of parameter %1 of element <defaultTransferFunction> not found. Parameter ignored!").arg(ui + 1));
					}
					continue;
				}
				Value *parameter = defaultTransferFunction->getParameter(name);
				if(parameter == NULL) {
					if(warnings != NULL) {
						warnings->append(QString("Unknown parameter \"%1\" of element <defaultTransferFunction>. Parameter ignored!").arg(name));
					}
					continue;
				}
				parameter->setValueFromString(value);
			}
		}
		net->setDefaultTransferFunction(*defaultTransferFunction);
		delete defaultTransferFunction;
	} 
	else {
		if(warnings != NULL) {
			warnings->append(QString("No element <defaultTransferFunction> found. "
									 "DefaultTransferFunction set to \"%1\"!")
							 .arg(net->getDefaultTransferFunction()->getName()));
		}
	}
}

void NeuralNetworkIOSimbaV3Xml::addDefaultActivationFunctionToNetwork(NeuralNetwork *net, const QDomElement &netElement, NeuralNetworkManager *neuralNetworkManager, QList<QString> *warnings) {

	QDomNodeList afList = netElement.elementsByTagName("defaultUpdateFunction");
	QDomElement afElem;
	for(uint ui = 0; ui < afList.length(); ++ui) {
		QDomNode node = afList.item(ui);
		if(node.isElement()) {
			afElem = node.toElement();
			break;
		}
	}

	if(!afElem.isNull()) {
		ActivationFunction *defaultActivationFunction = NULL;
		//look up correct transfer function.
		QString simbaV3Name = afElem.attribute("name");
		QString nerdName = NeuralNetworkIO::getNERDActivationFunctionNameFromSimbaV3(simbaV3Name);

		QList<ActivationFunction*> afPrototypes = neuralNetworkManager->getActivationFunctionPrototypes();
		QListIterator<ActivationFunction*> afPrototypesIterator(afPrototypes);
		while(afPrototypesIterator.hasNext()) {
			ActivationFunction *afPrototype = afPrototypesIterator.next();
			if(afPrototype->getName() == nerdName) {
				defaultActivationFunction = afPrototype->createCopy();
				break;
			}
		}
		if(defaultActivationFunction == NULL) {
			if(warnings != NULL) {
				warnings->append(QString("Unknown name \"%1\" of <defaultActivationFunction>. DefaultActivationFunction set to \"%2\"!").arg(simbaV3Name).arg(net->getDefaultActivationFunction()->getName()));
			}
			return;
		}

		//check for parameters.
		QDomNodeList paramList = afElem.elementsByTagName("param");
		for(uint ui = 0; ui < paramList.length(); ++ui) {
			QDomNode node = paramList.item(ui);
			if(node.isElement()) {
				QDomElement param = node.toElement();
				QString name = param.attribute("name");
				if(name.isEmpty()) {
					if(warnings != NULL) {
						warnings->append(QString("Attribute \"name\" of parameter %1 of element <defaultActivationFunction> not found. Parameter ignored!").arg(ui + 1));
					}
					continue;
				}
				QString value = param.attribute("value");
				if(value.isEmpty()) {
					if(warnings != NULL) {
						warnings->append(QString("Attribute \"value\" of parameter %1 of element <defaultActivationFunction> not found. Parameter ignored!").arg(ui + 1));
					}
					continue;
				}
				Value *parameter = defaultActivationFunction->getParameter(name);
				if(parameter == NULL) {
					if(warnings != NULL) {
						warnings->append(QString("Unknown parameter \"%1\" of element <defaultActivationFunction>. Parameter ignored!").arg(name));
					}
					continue;
				}
				parameter->setValueFromString(value);
			}
		}
		net->setDefaultActivationFunction(*defaultActivationFunction);
		delete defaultActivationFunction;
	} 
	else {
		if(warnings != NULL) {
			warnings->append(QString("No element <defaultActivationFunction> found. DefaultActivationFunction set to \"%1\"!").arg(net->getDefaultActivationFunction()->getName()));
		}
	}
}

void NeuralNetworkIOSimbaV3Xml::addDefaultSynapseFunctionToNetwork(NeuralNetwork *net, const QDomElement &netElement, NeuralNetworkManager *neuralNetworkManager, QList<QString> *warnings) {

	QDomNodeList sfList = netElement.elementsByTagName("defaultSynapseUpdateFunction");
	QDomElement sfElem;
	for(uint ui = 0; ui < sfList.length(); ++ui) {
		QDomNode node = sfList.item(ui);
		if(node.isElement()) {
			sfElem = node.toElement();
			break;
		}
	}

	if(!sfElem.isNull()) {
		SynapseFunction *defaultSynapseFunction = NULL;
		//look up correct transfer function.
		QString simbaV3Name = sfElem.attribute("name");
		QString nerdName = NeuralNetworkIO::getNERDSynapseFunctionNameFromSimbaV3(simbaV3Name);

		QList<SynapseFunction*> sfPrototypes = neuralNetworkManager->getSynapseFunctionPrototypes();
		QListIterator<SynapseFunction*> sfPrototypesIterator(sfPrototypes);
		while(sfPrototypesIterator.hasNext()) {
			SynapseFunction *sfPrototype = sfPrototypesIterator.next();
			if(sfPrototype->getName() == nerdName) {
				defaultSynapseFunction = sfPrototype->createCopy();
				break;
			}
		}
		if(defaultSynapseFunction == NULL) {
			if(warnings != NULL) {
				warnings->append(QString("Unknown name \"%1\" of <defaultSynapseUpdateFunction>. DefaultSynapseFunction set to \"%2\"!").arg(simbaV3Name).arg(net->getDefaultSynapseFunction()->getName()));
			}
			return;
		}

		//check for parameters.
		QDomNodeList paramList = sfElem.elementsByTagName("param");
		for(uint ui = 0; ui < paramList.length(); ++ui) {
			QDomNode node = paramList.item(ui);
			if(node.isElement()) {
				QDomElement param = node.toElement();
				QString name = param.attribute("name");
				if(name.isEmpty()) {
					if(warnings != NULL) {
						warnings->append(QString("Attribute \"name\" of parameter %1 of element <defaultSynapseUpdateFunction> not found. Parameter ignored!").arg(ui + 1));
					}
					continue;
				}
				QString value = param.attribute("value");
				if(value.isEmpty()) {
					if(warnings != NULL) {
						warnings->append(QString("Attribute \"value\" of parameter %1 of element <defaultSynapseUpdateFunction> not found. Parameter ignored!").arg(ui + 1));
					}
					continue;
				}
				Value *parameter = defaultSynapseFunction->getParameter(name);
				if(parameter == NULL) {
					if(warnings != NULL) {
						warnings->append(QString("Unknown parameter \"%1\" of element <defaultSynapseUpdateFunction>. Parameter ignored!").arg(name));
					}
					continue;
				}
				parameter->setValueFromString(value);
			}
		}
		net->setDefaultSynapseFunction(*defaultSynapseFunction);
		delete defaultSynapseFunction;
	} 
	else {
		if(warnings != NULL) {
			warnings->append(QString("No element <defaultSynapseUpdateFunction> found. DefaultSynapseFunction set to \"%1\"!").arg(net->getDefaultSynapseFunction()->getName()));
		}
	}
}

void NeuralNetworkIOSimbaV3Xml::addNeuronsAndSynapsesToNetwork(NeuralNetwork *net, const QDomElement &netElement, NeuralNetworkManager *neuralNetworkManager, QList<QString> *warnings) {
	//create neurons
	QDomNodeList neurons = netElement.elementsByTagName("neuron");
	QHash<Neuron*, QDomElement> mNeuronElementTable;
	QVector<Neuron*> neuronBuffer;
	QHash<Neuron*, QDomNodeList> synapseBuffer;
		
	for(uint ui = 0; ui < neurons.length(); ++ui) {
		QDomElement neuronElem;
		QDomNode node = neurons.item(ui);
		if(node.isElement()) {
			neuronElem = node.toElement();
				
			QString name = neuronElem.attribute("name");
			QString layer = neuronElem.attribute("layer");
			QString bias = neuronElem.attribute("bias");
			QString protection = neuronElem.attribute("protection");
			QString maxIn = neuronElem.attribute("maxIn");
			QString maxOut = neuronElem.attribute("maxOut");
				
			if(layer.isEmpty() || bias.isEmpty() || name.isEmpty() || protection.isEmpty() || maxIn.isEmpty() || maxOut.isEmpty()) {
				if(warnings != NULL) {
					warnings->append(QString("One of the attributes \"name\", \"layer\", \"bias\", \"protection\", \"maxIn\" and \"maxOut\" not found in neuron from line %1. Neuron ignored!").arg(node.lineNumber()));
				}
				continue;
			}
				
			//set neuron transfer function.
			TransferFunction *neuronTf = NULL;	
			{
				QDomElement neuronTfElem;
				QDomNodeList tfList = neuronElem.elementsByTagName("transferFunction");
				for(uint uj = 0; uj < tfList.length(); ++uj) {
					QDomNode tfNode = tfList.item(uj);
					if(tfNode.isElement()) {
						neuronTfElem = tfNode.toElement();
						break;
					}
				}

				if(!neuronTfElem.isNull()) {
					QString simbaV3Name = neuronTfElem.attribute("name");
					QString nerdName = NeuralNetworkIO::getNERDTransferFunctionNameFromSimbaV3(simbaV3Name);

					//look up correct transfer function.
					QList<TransferFunction*> tfPrototypes = neuralNetworkManager->getTransferFunctionPrototypes();
					QListIterator<TransferFunction*> tfPrototypesIterator(tfPrototypes);
					while(tfPrototypesIterator.hasNext()) {
						TransferFunction *tfPrototype = tfPrototypesIterator.next();
						if(tfPrototype->getName() == nerdName) {
							neuronTf = tfPrototype->createCopy();
							break;
						}
					}
					if(neuronTf == NULL) {
						if(warnings != NULL) {
							warnings->append(QString("Unknown name \"%1\" of <transferFunction> in neuron from line %2. Neuron ignored!").arg(simbaV3Name).arg(node.lineNumber()));
						}
						continue;
					}

					//check for parameters.
					QDomNodeList paramList = neuronTfElem.elementsByTagName("param");
					for(uint uj = 0; uj < paramList.length(); ++uj) {
						QDomNode node = paramList.item(uj);
						if(node.isElement()) {
							QDomElement param = node.toElement();
							QString name = param.attribute("name");
							if(name.isEmpty()) {
								if(warnings != NULL) {
									warnings->append(QString("Attribute \"name\" of parameter %1 of element <transferFunction> not found in neuron from line %2. Parameter ignored!").arg(uj + 1).arg(node.lineNumber()));
								}
								continue;
							}
							QString value = param.attribute("value");
							if(value.isEmpty()) {
								if(warnings != NULL) {
									warnings->append(QString("Attribute \"value\" of parameter %1 of element <transferFunction> not found in neuron from line %2. Parameter ignored!").arg(uj + 1).arg(node.lineNumber()));
								}
								continue;
							}
							Value *parameter = neuronTf->getParameter(name);
							if(parameter == NULL) {
								if(warnings != NULL) {
									warnings->append(QString("Unknown parameter \"%1\" of element <transferFunction> in neuron from line %2. Parameter ignored!").arg(name).arg(node.lineNumber()));
								}
								continue;
							}
							parameter->setValueFromString(value);
						}
					}
				} 
				else {
					if(warnings != NULL) {
						warnings->append(QString("No element <transferFunction> found in neuron from line %1. Neuron ignored!").arg(node.lineNumber()));
					}
					continue;
				}
			}

			//set neuron activation function.
			ActivationFunction *neuronAf = NULL;
			{
				QDomElement neuronAfElem;
				QDomNodeList afList = neuronElem.elementsByTagName("updateFunction");
				for(uint uj = 0; uj < afList.length(); ++uj) {
					QDomNode afNode = afList.item(uj);
					if(afNode.isElement()) {
						neuronAfElem = afNode.toElement();
						break;
					}
				}

				if(!neuronAfElem.isNull()) {
					QString simbaV3Name = neuronAfElem.attribute("name");
					QString nerdName = NeuralNetworkIO::getNERDActivationFunctionNameFromSimbaV3(simbaV3Name);

					//look up correct transfer function.
					QList<ActivationFunction*> afPrototypes = neuralNetworkManager->getActivationFunctionPrototypes();
					QListIterator<ActivationFunction*> afPrototypesIterator(afPrototypes);
					while(afPrototypesIterator.hasNext()) {
						ActivationFunction *afPrototype = afPrototypesIterator.next();
						if(afPrototype->getName() == nerdName) {
							neuronAf = afPrototype->createCopy();
							break;
						}
					}

					if(neuronAf == NULL) {
						if(warnings != NULL) {
							warnings->append(QString("Unknown name \"%1\" of <updateFunction> in neuron from line %2. Neuron ignored!").arg(simbaV3Name).arg(node.lineNumber()));
						}
						delete neuronTf;
						continue;
					}

					//check for parameters.
					QDomNodeList paramList = neuronAfElem.elementsByTagName("param");
					for(uint uj = 0; uj < paramList.length(); ++uj) {
						QDomNode node = paramList.item(uj);
						if(node.isElement()) {
							QDomElement param = node.toElement();
							QString name = param.attribute("name");
							if(name.isEmpty()) {
								if(warnings != NULL) {
									warnings->append(QString("Attribute \"name\" of parameter %1 of element <updateFunction> not found in neuron from line %2. Parameter ignored!").arg(uj + 1).arg(node.lineNumber()));
								}
								continue;
							}
							QString value = param.attribute("value");
							if(value.isEmpty()) {
								if(warnings != NULL) {
									warnings->append(QString("Attribute \"value\" of parameter %1 of element <updateFunction> not found in neuron from line %2. Parameter ignored!").arg(uj + 1).arg(node.lineNumber()));
								}
								continue;
							}
							Value *parameter = neuronTf->getParameter(name);
							if(parameter == NULL) {
								if(warnings != NULL) {
									warnings->append(QString("Unknown parameter \"%1\" of element <updateFunction> in neuron from line %2. Parameter ignored!").arg(name).arg(node.lineNumber()));
								}
								continue;
							}
							parameter->setValueFromString(value);
						}
					}
				} 
				else {
					if(warnings != NULL) {
						warnings->append(QString("No element <updateFunction> found in neuron from line %1. Neuron ignored!").arg(node.lineNumber()));
					}
					delete neuronTf;
					continue;
				}
			}

			// Create Neuron
			Neuron *neuron = new Neuron(name, *neuronTf, *neuronAf);
			delete neuronTf;
			delete neuronAf;

			//Set layer, protection mode, bias, maxIn, maxOut.
			//TODO: protection mode, maxIn, maxOut
			neuron->getBiasValue().set(bias.toDouble());
			if(layer == "input") {
				neuron->setProperty(Neuron::NEURON_TYPE_INPUT);
			}
			if(layer == "output") {
				neuron->setProperty(Neuron::NEURON_TYPE_OUTPUT);
			}

			net->addNeuron(neuron);
			neuronBuffer.append(neuron);
			mNeuronElementTable.insert(neuron, neuronElem);

			//Memorize synapses until all neurons have been read (required to determine source neurons).
			QDomNodeList synapseList = neuronElem.elementsByTagName("synapse");
			synapseBuffer.insert(neuron, synapseList);
		}
	}

	//add synapses
	{
		QList<Neuron*> neurons = synapseBuffer.keys();
		QListIterator<Neuron*> neuronIterator(neurons);
		while(neuronIterator.hasNext()) {
			Neuron *neuron = neuronIterator.next();
			QDomElement neuronElement = mNeuronElementTable.value(neuron);
			QDomNodeList synapseList = synapseBuffer.value(neuron);

			addSynapsesToNetwork(neuronBuffer, neuron, synapseList, neuronElement, neuralNetworkManager, warnings);
		}
	}
}

void NeuralNetworkIOSimbaV3Xml::addSynapsesToNetwork(QVector<Neuron*> neurons, SynapseTarget *target, const QDomNodeList &synapseList, const QDomNode &currentNode, NeuralNetworkManager *neuralNetworkManager, QList<QString> *warnings) {

	QList<SynapseFunction*> sfPrototypes = neuralNetworkManager->getSynapseFunctionPrototypes();

	for(uint ui = 0; ui < synapseList.length(); ++ui) {
		QDomNode node = synapseList.item(ui);
		if(node.isElement() && (node.parentNode() == currentNode)) {
			QDomElement synapseElem = node.toElement();
			QString strength = synapseElem.attribute("strength");
			QString source = synapseElem.attribute("source");
			QString protection = synapseElem.attribute("protection");

			//TODO: Protection Mode

			if(strength.isEmpty() || source.isEmpty() || protection.isEmpty()) {
				if(warnings != NULL) {
					warnings->append(QString("One of the attributes \"strength\", \"source\" and \"protection\" not found in synapse from line %1. Synapse ignored!").arg(node.lineNumber()));
				}
				continue;
			}

			qlonglong sourceID = source.toULongLong();
			Synapse *synapse = NULL;
			if((sourceID < 0) || (sourceID >= neurons.size())) {
				if(warnings != NULL) {
					warnings->append(QString("SourceID from synapse in line %1 does not match any neuron. Synapse ignored!").arg(node.lineNumber()));
				}
				continue;
			}

			// read synapse function
			SynapseFunction *synapseFunction = NULL;
			{
				QDomElement sfElem;
				QDomNodeList sfList = synapseElem.elementsByTagName("synapseUpdateFunction");
				for(uint uj = 0; uj < sfList.length(); uj++) {
					QDomNode node = sfList.item(uj);
					if(node.isElement() && (node.parentNode() == synapseElem)) {
						sfElem = node.toElement();
						break;
					}
				}
				if(sfElem.isNull()) {
					if(warnings != NULL) {
						warnings->append(QString("No element <synapseUpdateFunction> found in synapse from line %1. Synapse ignored!").arg(node.lineNumber()));
					}
					continue;
				}

				// look up correct synapse function
				QString simbaV3Name = sfElem.attribute("name");
				QString nerdName = NeuralNetworkIO::getNERDSynapseFunctionNameFromSimbaV3(simbaV3Name);

				QList<SynapseFunction*> sfPrototypes = neuralNetworkManager->getSynapseFunctionPrototypes();
				QListIterator<SynapseFunction*> sfPrototypesIterator(sfPrototypes);
				while(sfPrototypesIterator.hasNext()) {
					SynapseFunction *sfPrototype = sfPrototypesIterator.next();
					if(sfPrototype->getName() == nerdName) {
						synapseFunction = sfPrototype->createCopy();
						break;
					}
				}
				if(synapseFunction == NULL) {
					if(warnings != NULL) {
						warnings->append(QString("Unknown name \"%1\" of <synapseFunction> in synapse from line %2. Synapse ignored!").arg(simbaV3Name).arg(node.lineNumber()));
					}
					continue;
				}

				//check for parameters.
				QDomNodeList paramList = sfElem.elementsByTagName("param");
				for(uint uj = 0; uj < paramList.length(); ++uj) {
					QDomNode node = paramList.item(uj);
					if(node.isElement()) {
						QDomElement param = node.toElement();
						QString name = param.attribute("name");
						if(name.isEmpty()) {
							if(warnings != NULL) {
								warnings->append(QString("Attribute \"name\" of parameter %1 of element <synapseFunction> not found in synapse from line %2. Parameter ignored!").arg(uj + 1).arg(node.lineNumber()));
							}
							continue;
						}
						QString value = param.attribute("value");
						if(value.isEmpty()) {
							if(warnings != NULL) {
								warnings->append(QString("Attribute \"value\" of parameter %1 of element <synapseFunction> not found in synapse from line %2. Parameter ignored!").arg(uj + 1).arg(node.lineNumber()));
							}
							continue;
						}
						Value *parameter = synapseFunction->getParameter(name);
						if(parameter == NULL) {
							if(warnings != NULL) {
								warnings->append(QString("Unknown parameter \"%1\" of element <synapseFunction> in synapse from line %2. Parameter ignored!").arg(name).arg(node.lineNumber()));
							}
							continue;
						}
						parameter->setValueFromString(value);
					}
				}
			}

			// create and add synapse
			synapse = new Synapse(neurons.at(sourceID), NULL, strength.toDouble(), *synapseFunction);
			delete synapseFunction;
			target->addSynapse(synapse);

			//check for more synapses.
			QDomNodeList nextSynapseList = synapseElem.elementsByTagName("synapse");
			addSynapsesToNetwork(neurons, synapse, nextSynapseList, synapseElem, neuralNetworkManager, warnings);
		}
	}
}

bool NeuralNetworkIOSimbaV3Xml::createFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg) {
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		if(errorMsg != 0) {
			*errorMsg = QString("Cannot create file ").append(fileName).append(".");
		}
		file.close();
		return false;
	}

	QDomDocument doc;

	QDomProcessingInstruction header = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
	doc.appendChild(header); 

	QDomElement netRoot = doc.createElement("net");
	netRoot.setAttribute("version", "Simba XML V3.0");
	doc.appendChild(netRoot);

	//store default TransferFunction and its parameters.
	TransferFunction *defaultTf = net->getDefaultTransferFunction();
	QDomElement defaultTfElem = doc.createElement("defaultTransferFunction");
	QString nerdDefaultTfName = defaultTf->getName();
	QString simbaV3DefaultTfName = NeuralNetworkIO::getSimbaV3TransferFunctionNameFromNERD(nerdDefaultTfName);
	defaultTfElem.setAttribute("name", simbaV3DefaultTfName);
	QList<Value*> defaultTfParameters = defaultTf->getParameters();
	QList<QString> defaultTfParameterNames = defaultTf->getParameterNames();
	for(int i = 0; i < defaultTfParameters.size(); ++i) {
		Value* value = defaultTfParameters.at(i);
		QString name = defaultTfParameterNames.at(i);
		QDomElement param = doc.createElement("param");
		param.setAttribute("name", name);
		param.setAttribute("value", value->getValueAsString());
		defaultTfElem.appendChild(param);
	}
	netRoot.appendChild(defaultTfElem);

	//store default ActivationFunction and its parameters.
	ActivationFunction *defaultAf = net->getDefaultActivationFunction();
	QDomElement defaultAfElem = doc.createElement("defaultUpdateFunction");
	QString nerdDefaultAfName = defaultAf->getName();
	QString simbaV3DefaultAfName = NeuralNetworkIO::getSimbaV3ActivationFunctionNameFromNERD(nerdDefaultAfName);
	defaultAfElem.setAttribute("name", simbaV3DefaultAfName);
	QList<Value*> defaultAfParameters = defaultAf->getParameters();
	QList<QString> defaultAfParameterNames = defaultAf->getParameterNames();
	for(int i = 0; i < defaultAfParameters.size(); ++i) {
		Value* value = defaultAfParameters.at(i);
		QString name = defaultAfParameterNames.at(i);
		QDomElement param = doc.createElement("param");
		param.setAttribute("name", name);
		param.setAttribute("value", value->getValueAsString());
		defaultAfElem.appendChild(param);
	}
	netRoot.appendChild(defaultAfElem);

	//store default SynapseFunction and its parameters.
	SynapseFunction *defaultSf = net->getDefaultSynapseFunction();
	QDomElement defaultSfElem = doc.createElement("defaultSynapseUpdateFunction");
	QString nerdDefaultSfName = defaultSf->getName();
	QString simbaV3DefaultSfName = NeuralNetworkIO::getSimbaV3SynapseFunctionNameFromNERD(nerdDefaultSfName);
	defaultSfElem.setAttribute("name", simbaV3DefaultSfName);
	QList<Value*> defaultSfParameters = defaultSf->getParameters();
	QList<QString> defaultSfParameterNames = defaultSf->getParameterNames();
	for(int i = 0; i < defaultSfParameters.size(); ++i) {
		Value* value = defaultSfParameters.at(i);
		QString name = defaultSfParameterNames.at(i);
		QDomElement param = doc.createElement("param");
		param.setAttribute("name", name);
		param.setAttribute("value", value->getValueAsString());
		defaultSfElem.appendChild(param);
	}
	netRoot.appendChild(defaultSfElem);

	//store neurons
	QList<Neuron*> neurons = net->getNeurons();
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron* neuron = i.next();

		QString type = "hidden";

		if(neuron->hasProperty(Neuron::NEURON_TYPE_INPUT)) {
			type = "input";
		} 
		else if(neuron->hasProperty(Neuron::NEURON_TYPE_OUTPUT)) {
			type = "output";
		}

		//TODO: protection, maxIn, maxOut
		QString protection = "dynamic";
				
		QDomElement neuronElem = doc.createElement("neuron");
		netRoot.appendChild(neuronElem);
		neuronElem.setAttribute("bias", neuron->getBiasValue().getValueAsString());
		neuronElem.setAttribute("layer", type);
		neuronElem.setAttribute("maxIn", -1);
		neuronElem.setAttribute("maxOut", -1);
		neuronElem.setAttribute("name", neuron->getNameValue().getValueAsString());
		neuronElem.setAttribute("protection", protection);
				
		//store current TransferFunction of the neuron and its parameters.
		QDomElement tfElem = doc.createElement("transferFunction");
		neuronElem.appendChild(tfElem);
		TransferFunction *tf = neuron->getTransferFunction();
		QString nerdTfName = tf->getName();
		QString simbaV3TfName = NeuralNetworkIO::getSimbaV3TransferFunctionNameFromNERD(nerdTfName); 
		tfElem.setAttribute("name", simbaV3TfName);
		QList<Value*> tfParameters = tf->getParameters();
		QList<QString> tfParameterNames = tf->getParameterNames();
		for(int j = 0; j < tfParameters.size(); ++j) {
			Value *value = tfParameters.at(j);
			QString name = tfParameterNames.at(j);
			QDomElement param = doc.createElement("param");
			param.setAttribute("name", name);
			param.setAttribute("value", value->getValueAsString());
			tfElem.appendChild(param);
		}

		//store current ActivationFunction of the neuron and its parameters.
		QDomElement afElem = doc.createElement("updateFunction");
		neuronElem.appendChild(afElem);
		ActivationFunction *af = neuron->getActivationFunction();
		QString nerdAfName = af->getName();
		QString simbaV3AfName = NeuralNetworkIO::getSimbaV3ActivationFunctionNameFromNERD(nerdAfName); 
		afElem.setAttribute("name", simbaV3AfName);
		QList<Value*> afParameters = af->getParameters();
		QList<QString> afParameterNames = af->getParameterNames();
		for(int j = 0; j < afParameters.size(); ++j) {
			Value *value = afParameters.at(j);
			QString name = afParameterNames.at(j);
			QDomElement param = doc.createElement("param");
			param.setAttribute("name", name);
			param.setAttribute("value", value->getValueAsString());
			afElem.appendChild(param);
		}
		
		QList<Synapse*> synapses = neuron->getSynapses();
		for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
			Synapse *synapse = j.next();
			addSynapseToFile(doc, net, synapse, neuronElem);
		}
	}

	// write QDomDocument to file
	QTextStream outStream(&file);
	doc.save(outStream, 4);

	file.close();
	return true;
}

void NeuralNetworkIOSimbaV3Xml::addSynapseToFile(QDomDocument &doc, NeuralNetwork *net, Synapse *synapse, QDomElement &node) {
	//TODO: protection, maxIn, maxOut
	QString protection = "dynamic";
		
	QDomElement synapseElem = doc.createElement("synapse");
	node.appendChild(synapseElem);

	synapseElem.setAttribute("protection", protection);
	synapseElem.setAttribute("source", QString::number(NeuralNetworkIO::getNeuronNumber(net, synapse->getSource())));
	synapseElem.setAttribute("strength", synapse->getStrengthValue().getValueAsString());
		
	//store current SynapseFunction of the neuron and its parameters.
	QDomElement sfElem = doc.createElement("synapseUpdateFunction");
	synapseElem.appendChild(sfElem);
	SynapseFunction *sf = synapse->getSynapseFunction();
	QString nerdSfName = sf->getName();
	QString simbaV3SfName = NeuralNetworkIO::getSimbaV3SynapseFunctionNameFromNERD(nerdSfName); 
	sfElem.setAttribute("name", simbaV3SfName);
	QList<Value*> sfParameters = sf->getParameters();
	QList<QString> sfParameterNames = sf->getParameterNames();
	for(int j = 0; j < sfParameters.size(); ++j) {
		Value *value = sfParameters.at(j);
		QString name = sfParameterNames.at(j);
		QDomElement param = doc.createElement("param");
		param.setAttribute("name", name);
		param.setAttribute("value", value->getValueAsString());
		sfElem.appendChild(param);
	}
	
	QList<Synapse*> synapses = synapse->getSynapses();
	for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
		Synapse *synapse = j.next();
		addSynapseToFile(doc, net, synapse, synapseElem);
	}
}

}
