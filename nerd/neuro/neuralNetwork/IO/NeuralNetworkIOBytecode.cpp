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

#include "NeuralNetworkIOBytecode.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Core/Core.h"
#include <QFile>
#include <QList>
#include <QDebug>
#include "Math/ASeriesFunctions.h"

namespace nerd {

const char* NeuralNetworkIOBytecode::A_SERIES_ACCELBOARD_NAMES[8] = {"ABML", "ABSR", "ABAL", "ABAR", "ABHL", "ABHR", "ABFL", "ABFR"};

const char* NeuralNetworkIOBytecode::SPINAL_CORD_ADDRESS_PROPERTY = "SPINAL_CORD_ADDRESS";
const char* NeuralNetworkIOBytecode::REMOVE_SPINAL_CORD_ADDRESS_PROPERTY = "REMOVE_SPINAL_CORD_ADDRESS";
const char* NeuralNetworkIOBytecode::MEMORY_NUMBER_PROPERTY = "MEMORY_NUMBER";

/**
 * Try to export a neural network to the ASeries Bytecode.
 * 
 * @param fileName the file to write to.
 * @param net the net to export.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkIOBytecode::createFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg) {
	// try to open or create file
	QString code = NeuralNetworkIOBytecode::createByteCodeFromNetwork(net, errorMsg);

	if(code == "") {
		return false;
	}

	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		if(errorMsg != NULL) {
			*errorMsg = QString("Cannot create file ").append(fileName).append(".");
		}
		file.close();
		return false;
	}
	file.write(code.toUtf8());
	file.close();

	return true;
}

QString NeuralNetworkIOBytecode::createByteCodeFromNetwork(NeuralNetwork *net, QString *errorMsg) {
	QString code;
	QTextStream output(&code);
	NeuralNetworkIOBytecode::checkOrExportNetwork(output, net, errorMsg);
	return code;
}

bool NeuralNetworkIOBytecode::createMotionEditorFileFromNetwork(QString fileName, 
												NeuralNetwork *net, QString *errorMsg) 
{
	QString byteCodeFileName = fileName;
	byteCodeFileName += ".hyb";
	if(!createFileFromNetwork(byteCodeFileName, net, errorMsg)) {
		return false;
	}
	

	//create motion editor file
	QString motionFile = "mef4\r\n";
	motionFile += "kf|1|358|172||512|False|512|False|512|False|512|False|512|False|512|False|512|False";
	motionFile += "|512|False|512|False|512|False|512|False|512|False|512|False|512|False|512|False|";
	motionFile += "512|False|512|False|512|False|512|False|\r\n";
	motionFile += "co||1|1|100|False|NeuralNet|600|600|600|600|600|600|600|600|600";
	motionFile += "|600|600|600|600|600|600|600|600|600|600|600|600|600|600|600|600|600";
	motionFile += "|600|600|600|600|600|600|600|600|600|600|600|600|\r\n";
	motionFile += "mns|robots||\r\n";
	motionFile += "mns|authors||\r\n";
	motionFile += "mns|date||\r\n";
	motionFile += "mns|desc||\r\n";

	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		if(errorMsg != NULL) {
			*errorMsg = QString("Cannot create file ").append(fileName).append(".");
		}
		file.close();
		return false;
	}
	file.write(motionFile.toUtf8());
	file.close();
	return true;
}

/**
 * Checks, if a neural network is exportable to the ASeries Bytecode.
 *
 * @param net the net to check.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkIOBytecode::checkIfNetworkIsExportable(NeuralNetwork *net, QString *errorMsg) {
	QString code;
	QTextStream output(&code);
	return NeuralNetworkIOBytecode::checkOrExportNetwork(output, net, errorMsg);
}

/**
 * Checks, if a neural network is exportable to the ASeries Bytecode if the check parameter is true. If the check parameter is false the network is tried to exported to a given fileName.
 * @param fileName the file to write to if check is false.
 * @param net the net to check or to export.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @param check see function description.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkIOBytecode::checkOrExportNetwork(QTextStream &output, NeuralNetwork *net, 
												  QString *errorMsg) {
	// try to cast Network to ModularNeuralNetwork
	ModularNeuralNetwork *modNet = dynamic_cast<ModularNeuralNetwork*>(net);
	if(modNet == NULL) {
		*errorMsg = QString("Network has to be modular!");
		return false;
	}

	// First get all Accelboard modules.
	// Check, if all Accelboard modules exist once and if all modules with Property "Accelboard" have a valid value.
	QMap<QString, NeuroModule*> accelboardModules;	// Map Accelboard-Name to Module
	QList<NeuroModule*> modules = modNet->getNeuroModules();
	QListIterator<NeuroModule*> modulesIt(modules);
	while(modulesIt.hasNext()) {
		NeuroModule* module = modulesIt.next();
		if(module != 0) {
			if(module->hasProperty("Accelboard")) {
				QString accelboardName = module->getProperty("Accelboard");
				// Ensure that no Accelboard module exist twice.
				if(accelboardModules.contains(accelboardName)) {
					*errorMsg = QString("There are at least two modules with property Accelboard = %1!").arg(accelboardName);
					return false;
				}
				// Ensure that every module with property Accelboard has a valid property value
				if(!isValidAccelboardName(accelboardName)) {
					*errorMsg = QString("There is a module with property Accelboard = %1. This is not a valid accelboard name!").arg(accelboardName);
					return false;
				}
				accelboardModules[accelboardName] = module;
			}
		}
	}
	// Check if all accelboard modules were found.
	for(int i = 0; i < 8; i++) {
		QString accelboardName = QString(A_SERIES_ACCELBOARD_NAMES[i]);
		if(!accelboardModules.contains(accelboardName)) {
			*errorMsg = QString("Module with property Accelboard = %1 not found!").arg(accelboardName);
			return false;
		}
	}

	// Make a map with all neurons and their corresponding accelboard Module
	QMap<Neuron*, NeuroModule*> neuronModMap;
	QListIterator<NeuroModule*> accelModIt(accelboardModules.values());
	while(accelModIt.hasNext()) {
		NeuroModule* module = accelModIt.next();
		QList<Neuron*> neurons = module->getAllEnclosedNeurons();
		QListIterator<Neuron*> neuronIt(neurons);
		while(neuronIt.hasNext()) {
			Neuron* neuron = neuronIt.next();
			neuronModMap[neuron] = module;
		}
	}

	// Check if all network neurons belong to accelboard modules
	QList<Neuron*> netNeurons = net->getNeurons();
	QListIterator<Neuron*> netNeuronIt(netNeurons);
	while(netNeuronIt.hasNext()) {
		Neuron *netNeuron = netNeuronIt.next();
		if(!neuronModMap.contains(netNeuron)) {
			*errorMsg = QString("There is at least one neuron which doesn't belong to an accelboard module!");
			return false;
		}
	}

	// Check if all accelboard modules have the right input and output neurons with corresponding SpinalChordAddress.
	accelModIt.toFront();
	while(accelModIt.hasNext()) {
		NeuroModule* module = accelModIt.next();
		if(!accelboardModuleHasRightInputOutputNeurons(module, errorMsg)) {
			return false;
		}
	}

	// Check that there are no enabled synapses with a synapse as target. This is assumed later.
	QList<Synapse*> synapses = net->getSynapses();
	QListIterator<Synapse*> synapseIt(synapses);
	while(synapseIt.hasNext()) {
		Synapse* synapse = synapseIt.next();
		if(synapse->getEnabledValue().get()) {
			SynapseTarget* target = synapse->getTarget();
			Neuron* targetNeuron = dynamic_cast<Neuron*>(target);
			if(targetNeuron == 0) {
				*errorMsg = QString("There is an enabled synapse with a synapse as target in the net!");
				return false;
			}
		}
	}
	
	// Check if all accelboard modules don't exceed the maximum number of hidden neurons and the maximum number of outgoing hidden neurons. Also check if an hidden neuron has an invalid SpinalChordAddress.
	accelModIt.toFront();
	while(accelModIt.hasNext()) {
		NeuroModule* module = accelModIt.next();
		if(!accelboardModuleHiddenNeuronCheck(net, module, errorMsg, true)) {
			return false;
		}
	}

	// Now assign the SpinalChordAddresses and MemoryNumbers to the HiddenNeurons
	accelModIt.toFront();
	while(accelModIt.hasNext()) {
		NeuroModule* module = accelModIt.next();
		accelboardModuleHiddenNeuronCheck(net, module, errorMsg, false);
	}

	// Now write the Bytecode
	writeBytecode(accelboardModules, output);

	// Remove all added SpinalChordAddresses and Memory Numbers
	removeSpinalChordAddressesAndMemoryNumbers(net);

	return true;
}

/**
 * Removes all automatically added SpinalChord-Addresses and Memory Numbers from the net.
 * 
 * @param net the net.
 */
void NeuralNetworkIOBytecode::removeSpinalChordAddressesAndMemoryNumbers(NeuralNetwork *net) {
	// Get all network neurons
	QList<Neuron*> neurons = net->getNeurons();
	// Iterate over the neurons
	QListIterator<Neuron*> neuronIt(neurons);
	while(neuronIt.hasNext()) {
		Neuron *neuron = neuronIt.next();
		// If neuron is a hidden neuron and neuron has a Memory Number or added SpinalChordAddress assinged, remove this number or SpinalChordAddress.
		if(!(neuron->hasProperty(Neuron::NEURON_TYPE_INPUT) || neuron->hasProperty(Neuron::NEURON_TYPE_OUTPUT))) {
			if(neuron->hasProperty(MEMORY_NUMBER_PROPERTY)) {
				neuron->removeProperty(MEMORY_NUMBER_PROPERTY);
			}
			if(neuron->hasProperty(REMOVE_SPINAL_CORD_ADDRESS_PROPERTY)) {
				neuron->removeProperty(REMOVE_SPINAL_CORD_ADDRESS_PROPERTY);
				neuron->removeProperty(SPINAL_CORD_ADDRESS_PROPERTY);
			}
		}
	}
}

/**
 * Writes the Bytecode for all Accelboard Modules.
 * 
 * @param accelboardModules the Accelboard Name and Module Map.
 * @param file the file to write to.
 */
void NeuralNetworkIOBytecode::writeBytecode(QMap<QString, NeuroModule*> &accelboardModules, QTextStream &out) {

	// write net name
	out << "NeuralNet:\r\n\r\n";

	// Iterate over all Accelboard Modules
	QMapIterator<QString, NeuroModule*> accelModIt(accelboardModules);
	while(accelModIt.hasNext()) {
		accelModIt.next();
		NeuroModule *accelMod = accelModIt.value();
		// Get accelboard name
		QString accelboardName = accelModIt.key();
		// Write accelboard beginning keyword
		out << "_" << accelboardName << ":\r\n\r\n";
		// Get Module neurons
		QList<Neuron*> neurons = accelMod->getAllEnclosedNeurons();
		// Iterate over all neurons
		QListIterator<Neuron*> neuronIt(neurons);
		while(neuronIt.hasNext()) {
			Neuron* neuron = neuronIt.next();
			// If neuron is not an Input Neuron, write Bytecode
			if(!neuron->hasProperty(Neuron::NEURON_TYPE_INPUT)) {
				// Get bias value
				double bias = neuron->getBiasValue().get();
				// If SourceNeuron has FlipActivity property, flip bias
				if(neuron->isActivationFlipped()) {
					bias = -bias;
				}
				// Write bias value
				out << "weight " << bias << "\r\nbias\r\n";
				// Write enabled incoming synapses
				QList<Synapse*> synapses = neuron->getSynapses();
				QListIterator<Synapse*> synapseIt(synapses);
				while(synapseIt.hasNext()) {
					Synapse* synapse = synapseIt.next();
					if(synapse->getEnabledValue().get()) {
						Neuron *sourceNeuron = synapse->getSource();
						double strength = synapse->getStrengthValue().get();
						// If SourceNeuron has FlipActivity property, flip strength
						if(sourceNeuron->isActivationFlipped()) {
							strength = -strength;
						}
						// If TargetNeuron has FlipActivity property and TargetNeuron is not SourceNeuron, 
						// flip strength
						if(neuron->isActivationFlipped() && (neuron != sourceNeuron)) {
							strength = -strength;
						}
						bool addSynapses = true;
						while(addSynapses) {
							double w = strength;
							if(strength > ASeriesFunctions::sMaxDoubleFixedPoint_4_9) {
								strength -= ASeriesFunctions::sMaxDoubleFixedPoint_4_9;
								w = ASeriesFunctions::sMaxDoubleFixedPoint_4_9;
							}
							else if(strength < ASeriesFunctions::sMinDoubleFixedPoint_4_9) {
								strength -= ASeriesFunctions::sMinDoubleFixedPoint_4_9;
								w = ASeriesFunctions::sMinDoubleFixedPoint_4_9;
							}
							else {
								addSynapses = false;
							}
							out << "weight " << w << "\r\n";
							if(sourceNeuron->hasProperty(SPINAL_CORD_ADDRESS_PROPERTY)) {
								out << "read " << sourceNeuron->getProperty(SPINAL_CORD_ADDRESS_PROPERTY) 
									<< "\r\n";
							} else {
								out << "read " << sourceNeuron->getProperty(MEMORY_NUMBER_PROPERTY) 
									<< "\r\n";
							}
							out << "mac\r\n";
						}
					}
				}
				// Write transfer function
				out << "tanh\r\n";
				// Write result to MemoryNumber or SpinalChordAddress
				if(neuron->hasProperty(SPINAL_CORD_ADDRESS_PROPERTY)) {
					out << "res " << neuron->getProperty(SPINAL_CORD_ADDRESS_PROPERTY) << "\r\n\r\n";
				} else {
					out << "res " << neuron->getProperty(MEMORY_NUMBER_PROPERTY) << "\r\n\r\n";
				}
			}
		}
	}
}

/**
 * Checks if every hidden neuron with an assigned SpinalChordAddress has a valid SpinalChordAddress and that no SpinalChordAddresses are assigned twice.
 * Checks if there is a free SpinalChordAddress for every hidden neuron with outgoing connections and if there is a free internal memory number or free SpinalChordAddress for every hidden neuron without an outgoing connection.
 * 
 * @param net the neural net.
 * @param module the module to check.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @param check if check is false, the function doesn't check the things but assumes that everything is ok and assigns SpinaChordAddresses and Memory Numbers to the hidden neurons. The added SpinalChordAddresses are provided with a remark to remove them later and to distinguish them from hand added SpinalChordAddresses from the user.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkIOBytecode::accelboardModuleHiddenNeuronCheck(NeuralNetwork *net, NeuroModule *module, QString *errorMsg, bool check) {
	QString accelboardName = module->getProperty("Accelboard");

	// Get a list of hidden neurons of the module
	QList<Neuron*> hiddenNeurons;
	QList<Neuron*> neurons = module->getAllEnclosedNeurons();
	QListIterator<Neuron*> neuronIt(neurons);
	while(neuronIt.hasNext()) {
		Neuron *neuron = neuronIt.next();
		if(!(neuron->hasProperty(Neuron::NEURON_TYPE_INPUT) || neuron->hasProperty(Neuron::NEURON_TYPE_OUTPUT))) {
			hiddenNeurons << neuron;
		}
	}

	// Generate the free SpinalChordAddresses depending on the accelboard name
	QList<QString> freeAddresses;
	if(accelboardName == "ABML") {
		freeAddresses << "7" << "8" << "9" << "10" << "11" << "12" << "13";
	} else if(accelboardName == "ABSR") {
		freeAddresses << "22" << "23" << "24" << "25";
	} else if(accelboardName == "ABAL") {
		freeAddresses << "38" << "39" << "40" << "41";
	} else if(accelboardName == "ABAR") {
		freeAddresses << "54" << "55" << "56" << "57";
	} else if(accelboardName == "ABHL") {
		freeAddresses << "70" << "71" << "72" << "73";
	} else if(accelboardName == "ABHR") {
		freeAddresses << "86" << "87" << "88" << "89";
	} else if(accelboardName == "ABFL") {
		freeAddresses << "102" << "103" << "104" << "105";
	} else if(accelboardName == "ABFR") {
		freeAddresses << "118" << "119" << "120" << "121";
	}

	// Check if every hidden neuron with an assigned SpinalChordAddress has a valid SpinalChordAddress and that no SpinalChordAddresses are assigned twice.
	QListIterator<Neuron*> hiddenIt(hiddenNeurons);
	while(hiddenIt.hasNext()) {
		Neuron* hiddenNeuron = hiddenIt.next();
		if(hiddenNeuron->hasProperty(SPINAL_CORD_ADDRESS_PROPERTY)) {
			QString address = hiddenNeuron->getProperty(SPINAL_CORD_ADDRESS_PROPERTY);
			if(check && !freeAddresses.contains(address)) {
				*errorMsg = QString("Hidden neuron in accelboard module %1 has invalid SpinalCordAddress or SpinalCordAddress is assigned twice: %2 = %3!").arg(accelboardName).arg(SPINAL_CORD_ADDRESS_PROPERTY).arg(address);
				return false;
			}
			freeAddresses.removeAll(address);
		}
	}

	// Check if there is a free SpinalChordAddress for every hidden neuron with enabled outgoing connections and if there is a free internal memory number or free SpinalChordAddress for every hidden neuron without an outgoing enabled connection.
	int freeInternalMemory = 32;
	hiddenIt.toFront();
	while(hiddenIt.hasNext()) {
		Neuron* hiddenNeuron = hiddenIt.next();
		if(!hiddenNeuron->hasProperty(SPINAL_CORD_ADDRESS_PROPERTY)) {
			if(isNeuronWithOutgoingConnections(net, module, hiddenNeuron)) {
				// If neuron has outgoing connections, check if there are free SpinalChordAddresses left.
				if(check && freeAddresses.isEmpty()) {
					*errorMsg = QString("No more free SpinalChordAddresses left for hidden neuron with outgoing connections in accelboard module %1!").arg(accelboardName);
					return false;
				}
				// If not to check, assign the first free SpinalChordAddress to the neuron with the remove later property.
				if(!check) {
					hiddenNeuron->setProperty(SPINAL_CORD_ADDRESS_PROPERTY, freeAddresses.first());
					hiddenNeuron->setProperty(REMOVE_SPINAL_CORD_ADDRESS_PROPERTY, "");
				}
				freeAddresses.removeFirst();
			} else {
				// If neuron has no outgoing connection first check if there is InternalMemory left.
				if(freeInternalMemory > 0) {
					// If not to check, assign the first free InternalMemoryNumber
					if(!check) {
						hiddenNeuron->setProperty(MEMORY_NUMBER_PROPERTY, QString("N%1").arg(32 - freeInternalMemory));
					}
					freeInternalMemory--;
				} else {
					// Otherwise look for free SpinalChordAddresses left.
					if(check && freeAddresses.isEmpty()) {
						*errorMsg = QString("No more free Internal Memory and no more free SpinalChordAddresses left for hidden neuron without outgoing connections in accelboard module %1!").arg(accelboardName);
						return false;
					}
					// If not to check, assign a free SpinalChordAddress with the remove later property.
					if(!check) {
						hiddenNeuron->setProperty(SPINAL_CORD_ADDRESS_PROPERTY, freeAddresses.first());
						hiddenNeuron->setProperty(REMOVE_SPINAL_CORD_ADDRESS_PROPERTY, "");
					}
					freeAddresses.removeFirst();
				}
			}
		}
	}
	return true;
}

/**
 * Checks if a given neuron has enabled connections to other modules than the owner-module.
 * 
 * @param net the neural net.
 * @param module ther owner-module.
 * @param neuron the neuron to check.
 * @return returns true if neuron has outgoing connections, otherwise false.
 */
bool NeuralNetworkIOBytecode::isNeuronWithOutgoingConnections(NeuralNetwork *net, NeuroModule *module, Neuron *neuron) {
	QString accelboardName = module->getProperty("Accelboard");
	QList<Neuron*> neurons = module->getAllEnclosedNeurons();
	QList<Synapse*> synapses = net->getSynapses();
	QListIterator<Synapse*> synapseIt(synapses);
	while(synapseIt.hasNext()) {
		Synapse* synapse = synapseIt.next();
		if(synapse->getEnabledValue().get()) {
			SynapseTarget* target = synapse->getTarget();
			Neuron* targetNeuron = dynamic_cast<Neuron*>(target);
			// Check neuron for outgoing connections
			if(synapse->getSource() == neuron) {
				if(!neurons.contains(targetNeuron)) {
					return true;
				}
			}
		}
	}
	return false;
}

/**
 * Check if a given accelboard module has the right Input and Output neurons with the right SpinalChordAddresses.
 * 
 * @param module the accelboard module.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkIOBytecode::accelboardModuleHasRightInputOutputNeurons(NeuroModule *module, QString *errorMsg) {
	QString accelboardName = module->getProperty("Accelboard");

	// Get a list of all Input Neuron SpinalChordAddresses and Output Neuron SpinalChordAddresses
	QList<QString> inputNeuronAddresses;
	QList<QString> outputNeuronAddresses;
	
	QList<Neuron*> neurons = module->getAllEnclosedNeurons();
	QListIterator<Neuron*> neuronIt(neurons);
	while(neuronIt.hasNext()) {
		Neuron *neuron = neuronIt.next();
		if(neuron->hasProperty(Neuron::NEURON_TYPE_INPUT)) {
			if(!neuron->hasProperty(SPINAL_CORD_ADDRESS_PROPERTY)) {
				*errorMsg = QString("There is an input neuron in accelboard module %1 which has no property %2!").arg(accelboardName).arg(SPINAL_CORD_ADDRESS_PROPERTY);
				return false;
			}
			inputNeuronAddresses << neuron->getProperty(SPINAL_CORD_ADDRESS_PROPERTY);
		} else if(neuron->hasProperty(Neuron::NEURON_TYPE_OUTPUT)) {
			if(!neuron->hasProperty(SPINAL_CORD_ADDRESS_PROPERTY)) {
				*errorMsg = QString("There is an output neuron in accelboard module %1 which has no property %2!").arg(accelboardName).arg(SPINAL_CORD_ADDRESS_PROPERTY);
				return false;
			}
			outputNeuronAddresses << neuron->getProperty(SPINAL_CORD_ADDRESS_PROPERTY);
		}
	}

	// Create a list of correct SpinalChordAddresses for input and output neurons
	QList<QString> inputAddresses;
	QList<QString> outputAddresses;

	if(accelboardName == "ABML") {
		inputAddresses << "3" << "4";
		// no output neurons
	} else if(accelboardName == "ABSR") {
		inputAddresses << "19" << "20" << "26" << "27" << "28";
		outputAddresses << "16" << "17" << "18" << "29" << "30" << "31";
	} else if(accelboardName == "ABAL") {
		inputAddresses << "35" << "36" << "42" << "43" << "44";
		outputAddresses << "32" << "33" << "34" << "45" << "46" << "47";
	} else if(accelboardName == "ABAR") {
		inputAddresses << "51" << "52" << "58" << "59" << "60";
		outputAddresses << "48" << "49" << "50" << "61" << "62" << "63";
	} else if(accelboardName == "ABHL") {
		inputAddresses << "67" << "68" << "74" << "75" << "76";
		outputAddresses << "64" << "65" << "66" << "77" << "78" << "79";
	} else if(accelboardName == "ABHR") {
		inputAddresses << "83" << "84" << "90" << "91" << "92";
		outputAddresses << "80" << "81" << "82" << "93" << "94" << "95";
	} else if(accelboardName == "ABFL") {
		inputAddresses << "99" << "100" << "106" << "107" << "108";
		outputAddresses << "96" << "97" << "98" << "109" << "110" << "111";
	} else if(accelboardName == "ABFR") {
		inputAddresses << "115" << "116" << "122" << "123" << "124";
		outputAddresses << "112" << "113" << "114" << "125" << "126" << "127";
	}

	// Check if all SpinalChordAddresses are valid and only exist once
	// First the input neurons
	QListIterator<QString> inputNeuronAddressesIt(inputNeuronAddresses);
	while(inputNeuronAddressesIt.hasNext()) {
		QString inputNeuronAddress = inputNeuronAddressesIt.next();
		if(!inputAddresses.contains(inputNeuronAddress)) {
			*errorMsg = QString("Input neuron in accelboard module %1 has invalid SpinalCordAddress or SpinalCordAddress is assigned twice: %2 = %3!").arg(accelboardName).arg(SPINAL_CORD_ADDRESS_PROPERTY).arg(inputNeuronAddress);
			return false;
		}
		inputAddresses.removeAll(inputNeuronAddress);
	}
	// Check if there are SpinalChordAddresses left
	if(!inputAddresses.empty()) {
		*errorMsg = QString("At least one input neuron in accelboard module %1 missing. SpinalChordAddress %2 not assigned!").arg(accelboardName).arg(inputAddresses[0]);
		return false;
	}

	// Now the output neurons
	QListIterator<QString> outputNeuronAddressesIt(outputNeuronAddresses);
	while(outputNeuronAddressesIt.hasNext()) {
		QString outputNeuronAddress = outputNeuronAddressesIt.next();
		if(!outputAddresses.contains(outputNeuronAddress)) {
			*errorMsg = QString("Output neuron in accelboard module %1 has invalid SpinalCordAddress or SpinalCordAddress is assigned twice: %2 = %3!").arg(accelboardName).arg(SPINAL_CORD_ADDRESS_PROPERTY).arg(outputNeuronAddress);
			return false;
		}
		outputAddresses.removeAll(outputNeuronAddress);
	}
	// Check if there are SpinalChordAddresses left
	if(!outputAddresses.empty()) {
		*errorMsg = QString("At least one output neuron in accelboard module %1 missing. SpinalChordAddress %2 not assigned!").arg(accelboardName).arg(outputAddresses[0]);
		return false;
	}

	return true;
}

/**
 * Check, if a given name is a valid A-Series-Accelboard-Name.
 * 
 * @param name the name to check.
 * @return 
 */
bool NeuralNetworkIOBytecode::isValidAccelboardName(QString name) {
	for(int i = 0; i < 8; i++) {
		if(name == A_SERIES_ACCELBOARD_NAMES[i]) {
			return true;
		}
	}
	return false;
}


/*NeuralNetwork* NeuralNetworkIOBytecode::createNetFromFile(QString fileName, QString *errorMsg, QList<QString> *warnings) {
	// TODO
}
*/
}
