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



#include "ReplaceModuleHandler.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "NeuralNetworkConstants.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Util/NeuralNetworkUtil.h"
#include <QStringList>
#include "Util/Util.h"


using namespace std;

namespace nerd {



bool ReplaceModuleHandler::canBeReplaced(NeuroModule *oldModule, NeuroModule *newModule, 
											QString *errorMessage)
{
	if(oldModule == 0 || newModule == 0) {
		if(errorMessage != 0) {
			errorMessage->append("Some of the required module pointers were NULL.\n");
		}
		return false;
	}
	ModularNeuralNetwork *ownerNetwork = oldModule->getOwnerNetwork();

	if(ownerNetwork == 0) {
		if(errorMessage != 0) {
			errorMessage->append("OldModule does not have an owner network.\n");
		}
		return false;
	}

	//Check if the module input/output interface is compatible.
	if(newModule->getInputNeurons().size() != oldModule->getInputNeurons().size()) {
		if(errorMessage != 0) {
			errorMessage->append("New and old module do not have the same numer of input neurons!");
		}
		return false;
	} 
	if(newModule->getOutputNeurons().size() != oldModule->getOutputNeurons().size()) {
		if(errorMessage != 0) {
			errorMessage->append("New and old module do not have the same numer of output neurons!");
		}
		return false;
	}

	if(oldModule->hasProperty(NeuralNetworkConstants::TAG_MODULE_PREVENT_REPLACEMENT)
		|| newModule->hasProperty(NeuralNetworkConstants::TAG_MODULE_PREVENT_REPLACEMENT)) 
	{
		if(errorMessage != 0) {
			errorMessage->append("One of the modules prevents replacement via tag.");	
		}
		return false;
	}

	QString moduleType = oldModule->getProperty(NeuralNetworkConstants::TAG_MODULE_TYPE);

	//Check if the type of the modules is compatible.
	if(newModule->hasProperty(NeuralNetworkConstants::TAG_MODULE_COMPATIBLE_TYPES)) {
		QString compatibilityList = 
					newModule->getProperty(NeuralNetworkConstants::TAG_MODULE_COMPATIBLE_TYPES);
		QStringList compatibleTypes = compatibilityList.split(",");
		bool compatible = false;
		for(QListIterator<QString> k(compatibleTypes); k.hasNext();) {
			if(k.next().trimmed() == moduleType) {
				compatible = true;
				break;
			}
		}
		if(!compatible) {	
			if(errorMessage != 0) {
				errorMessage->append("New module is not compatible with the old module type!");
			}
			return false; 
		}
	}

	
	//check if the neurons are also compatible with the global input / output neurons
	QList<Neuron*> globalInputNeuronsOfOldModule;
	QList<Neuron*> globalOutputNeuronsOfOldModule;
	int countOldModuleInputs = 0;
	int countOldModuleOutputs = 0;
	QList<Neuron*> tmpOldInputNeurons = oldModule->getInputNeurons();
	QList<Neuron*> tmpOldOutputNeurons = oldModule->getOutputNeurons();
	QList<Neuron*> oldModuleDirectIndirectNeurons = oldModule->getAllEnclosedNeurons();
	for(QListIterator<Neuron*> i(oldModuleDirectIndirectNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		bool interfaceNeuron = false;
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_INPUT_NEURON)) {
			globalInputNeuronsOfOldModule.append(neuron);
			interfaceNeuron = true;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_OUTPUT_NEURON)) {
			globalOutputNeuronsOfOldModule.append(neuron);
			interfaceNeuron = true;
		}
		if(interfaceNeuron) {
			if(tmpOldInputNeurons.contains(neuron)) {
				++countOldModuleInputs;
			}
			if(tmpOldOutputNeurons.contains(neuron)) {
				++countOldModuleOutputs;
			}
		}
	}
	QList<Neuron*> globalInputNeuronsOfNewModule;
	QList<Neuron*> globalOutputNeuronsOfNewModule;
	int countNewModuleInputs = 0;
	int countNewModuleOutputs = 0;
	QList<Neuron*> tmpNewInputNeurons = oldModule->getInputNeurons();
	QList<Neuron*> tmpNewOutputNeurons = oldModule->getOutputNeurons();
	QList<Neuron*> newModuleDirectIndirectNeurons = oldModule->getAllEnclosedNeurons();
	for(QListIterator<Neuron*> i(newModuleDirectIndirectNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		bool interfaceNeuron = false;
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_INPUT_NEURON)) {
			globalInputNeuronsOfNewModule.append(neuron);
			interfaceNeuron = true;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_OUTPUT_NEURON)) {
			globalOutputNeuronsOfNewModule.append(neuron);
			interfaceNeuron = true;
		}
		if(interfaceNeuron) {
			if(tmpNewInputNeurons.contains(neuron)) {
				++countNewModuleInputs;
			}
			if(tmpNewOutputNeurons.contains(neuron)) {
				++countNewModuleOutputs;
			}
		}
	}
	if(globalInputNeuronsOfOldModule.size() != globalInputNeuronsOfNewModule.size()) {
		if(errorMessage != 0) {
			errorMessage->append("New and old module do not have the same numer of global input neurons!");
		}
		return false;
	}
	if(globalOutputNeuronsOfOldModule.size() != globalOutputNeuronsOfNewModule.size()) {
		if(errorMessage != 0) {
			errorMessage->append("New and old module do not have the same numer of global output neurons!");
		}
		return false;
	}
	if(countOldModuleInputs != countNewModuleInputs || countOldModuleOutputs != countNewModuleOutputs) {
		if(errorMessage != 0) {
			errorMessage->append("New and old module do not have the same numer of "
				"local interface neurons among its global interface neurons.!");
		}
		return false;
	}
	

	return true;
}

/**
 * Will not destroy or modify newModule. The inserted module will be a deep copy of newModule.
 */
bool ReplaceModuleHandler::replaceModule(NeuroModule *oldModule, NeuroModule *newModule, 
							QList<NeuralNetworkElement*> &trashcan, QString *errorMessage)
{
	if(oldModule == 0 || newModule == 0) {
		if(errorMessage != 0) {
			errorMessage->append("Some of the required module pointers were NULL.\n");
		}
		return false;
	}
	ModularNeuralNetwork *ownerNetwork = oldModule->getOwnerNetwork();

	if(ownerNetwork == 0) {
		if(errorMessage != 0) {
			errorMessage->append("OldModule does not have an owner network.\n");
		}
		return false;
	}


	NeuroModule *replacement = dynamic_cast<NeuroModule*>(newModule->createDeepCopy());

	
	//check if the neurons are also compatible with the global input / output neurons
	QList<Neuron*> globalInputNeuronsOfOldModule;
	QList<Neuron*> globalOutputNeuronsOfOldModule;
	int countOldModuleInputs = 0;
	int countOldModuleOutputs = 0;
	QList<Neuron*> tmpOldInputNeurons = oldModule->getInputNeurons();
	QList<Neuron*> tmpOldOutputNeurons = oldModule->getOutputNeurons();
	QList<Neuron*> oldModuleDirectIndirectNeurons = oldModule->getAllEnclosedNeurons();
	for(QListIterator<Neuron*> i(oldModuleDirectIndirectNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		bool interfaceNeuron = false;
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_INPUT_NEURON)) {
			globalInputNeuronsOfOldModule.append(neuron);
			interfaceNeuron = true;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_OUTPUT_NEURON)) {
			globalOutputNeuronsOfOldModule.append(neuron);
			interfaceNeuron = true;
		}
		if(interfaceNeuron) {
			if(tmpOldInputNeurons.contains(neuron)) {
				++countOldModuleInputs;
			}
			if(tmpOldOutputNeurons.contains(neuron)) {
				++countOldModuleOutputs;
			}
		}
	}
	QList<Neuron*> globalInputNeuronsOfNewModule;
	QList<Neuron*> globalOutputNeuronsOfNewModule;
	int countNewModuleInputs = 0;
	int countNewModuleOutputs = 0;
	QList<Neuron*> tmpNewInputNeurons = replacement->getInputNeurons();
	QList<Neuron*> tmpNewOutputNeurons = replacement->getOutputNeurons();
	QList<Neuron*> newModuleDirectIndirectNeurons = replacement->getAllEnclosedNeurons();
	for(QListIterator<Neuron*> i(newModuleDirectIndirectNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		bool interfaceNeuron = false;
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_INPUT_NEURON)) {
			globalInputNeuronsOfNewModule.append(neuron);
			interfaceNeuron = true;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_OUTPUT_NEURON)) {
			globalOutputNeuronsOfNewModule.append(neuron);
			interfaceNeuron = true;
		}
		if(interfaceNeuron) {
			if(tmpNewInputNeurons.contains(neuron)) {
				++countNewModuleInputs;
			}
			if(tmpNewOutputNeurons.contains(neuron)) {
				++countNewModuleOutputs;
			}
		}
	}
	if(globalInputNeuronsOfOldModule.size() != globalInputNeuronsOfNewModule.size()) {
		errorMessage->append("New and old module do not have the same numer of global input neurons!");
		return false;
	}
	if(globalOutputNeuronsOfOldModule.size() != globalOutputNeuronsOfNewModule.size()) {
		errorMessage->append("New and old module do not have the same numer of global output neurons!");
		return false;
	}
	if(countOldModuleInputs != countNewModuleInputs || countOldModuleOutputs != countNewModuleOutputs) {
		errorMessage->append("New and old module do not have the same numer of "
				"local interface neurons among its global interface neurons.!");
		return false;
	}
	

	if(oldModule->getInputNeurons().size() != replacement->getInputNeurons().size() 
		|| oldModule->getOutputNeurons().size() != replacement->getOutputNeurons().size()) 
	{
		if(errorMessage != 0) {
			errorMessage->append("Interface of modules does not match.\n");
		}
		return false;
	}

	//replace global interface neurons
	Neuron dummy("Dummy", TransferFunctionTanh(), AdditiveTimeDiscreteActivationFunction(), 0);
	for(int i = 0; i < globalInputNeuronsOfOldModule.size(); ++i) {
		Neuron *oldNeuron = globalInputNeuronsOfOldModule.at(i);
		Neuron *newNeuron = globalInputNeuronsOfNewModule.at(i);
		oldModule->replaceNeuron(oldNeuron, &dummy);
		replacement->replaceNeuron(newNeuron, oldNeuron);
		oldModule->replaceNeuron(&dummy, newNeuron);
		NeuralNetworkUtil::setNetworkElementLocationProperty(oldNeuron,
				NeuralNetworkUtil::getPosition(newNeuron));
	}
	for(int i = 0; i < globalOutputNeuronsOfOldModule.size(); ++i) {
		Neuron *oldNeuron = globalOutputNeuronsOfOldModule.at(i);
		Neuron *newNeuron = globalOutputNeuronsOfNewModule.at(i);
		oldModule->replaceNeuron(oldNeuron, &dummy);
		replacement->replaceNeuron(newNeuron, oldNeuron);
		oldModule->replaceNeuron(&dummy, newNeuron);
		NeuralNetworkUtil::setNetworkElementLocationProperty(oldNeuron,
				NeuralNetworkUtil::getPosition(newNeuron));
	}


	//rewire modules

	QList<Neuron*> oldModuleNeurons = oldModule->getAllEnclosedNeurons();
	QList<Neuron*> newModuleNeurons = replacement->getAllEnclosedNeurons();
	QList<Neuron*> oldInputNeurons = oldModule->getInputNeurons();
	QList<Neuron*> oldOutputNeurons = oldModule->getOutputNeurons();
	QList<Neuron*> newInputNeurons = replacement->getInputNeurons();
	QList<Neuron*> newOutputNeurons = replacement->getOutputNeurons();
 
	Util::addWithoutDuplicates<Neuron>(oldModuleNeurons, oldInputNeurons);
	Util::addWithoutDuplicates<Neuron>(oldModuleNeurons, oldOutputNeurons);
	Util::addWithoutDuplicates<Neuron>(newModuleNeurons, newInputNeurons);
	Util::addWithoutDuplicates<Neuron>(newModuleNeurons, newOutputNeurons);


	//disconnect all synapses from outside of the new module.
	for(QListIterator<Neuron*> i(newModuleNeurons); i.hasNext();) {
		Neuron *newNeuron = i.next();
		QList<Synapse*> synapses = newNeuron->getSynapses();
		for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
			Synapse *synapse = j.next();
			if(!newModuleNeurons.contains(synapse->getSource())) {
				newNeuron->removeSynapse(synapse);
				synapse->setSource(0);
				trashcan.append(synapse);
			}
		}
		QList<Synapse*> outgoingSynapses = newNeuron->getOutgoingSynapses();
		for(QListIterator<Synapse*> j(outgoingSynapses); j.hasNext();) {
			Synapse *synapse = j.next();
			QList<Neuron*> connectedNeurons = NeuralNetworkUtil::getConnectedNeurons(synapse);
			for(QListIterator<Neuron*> k(connectedNeurons); k.hasNext();) {
				if(!newModuleNeurons.contains(k.next())) {
					synapse->getTarget()->removeSynapse(synapse);
					synapse->setSource(0);
					trashcan.append(synapse);
				}
			}
		}
	}



	for(QListIterator<Neuron*> i(oldModuleNeurons); i.hasNext();) {
		Neuron *neuron = i.next();

		bool isInputNeuron = false;
		bool isOutputNeuron = false;

		if(oldInputNeurons.contains(neuron)) {

			isInputNeuron = true;
			int inputIndex = oldInputNeurons.indexOf(neuron);

			//select matching input neuron from new module.
			if(inputIndex == -1 || inputIndex >= newInputNeurons.size()) {
				if(errorMessage != 0) {
					errorMessage->append("Problem matching input neuron.\n");
				}
				isInputNeuron = false;
				continue; //TODO remove neuron later
			}
			
			Neuron *newNeuron = newInputNeurons.at(inputIndex);

			if(newNeuron == 0) {
				if(errorMessage != 0) {
					errorMessage->append("Could not find matching input neuron.\n");
				}
				continue;
			}

			//reconnect incomming synapses from old to new input neuron.
			QList<Synapse*> inputSynapses = neuron->getSynapses();
			for(QListIterator<Synapse*> k(inputSynapses); k.hasNext();) {
				Synapse *synapse = k.next();

				if(oldModuleNeurons.contains(synapse->getSource())) {
					continue;
				}

				neuron->removeSynapse(synapse);
				newNeuron->addSynapse(synapse);
			}
		}
		if(oldOutputNeurons.contains(neuron)) {
			isOutputNeuron = true;

			int outputIndex = oldOutputNeurons.indexOf(neuron);

			//select matching input neuron from new module.
			if(outputIndex == -1 || outputIndex >= newOutputNeurons.size()) {
				if(errorMessage != 0) {
					errorMessage->append("Problem matching output neuron.\n");
				}
				isOutputNeuron = false;
				continue; //TODO remove neuron later
			}
			
			Neuron *newNeuron = newOutputNeurons.at(outputIndex);

			if(newNeuron == 0) {
				if(errorMessage != 0) {
					errorMessage->append("Could not find matching output neuron.\n");
				}
				continue;
			}

			//reconnect outgoing synapses from old to new output neuron.
			QList<Synapse*> outputSynapses = neuron->getOutgoingSynapses();
			for(QListIterator<Synapse*> k(outputSynapses); k.hasNext();) {
				Synapse *synapse = k.next();

				QList<Neuron*> connectedNeurons = NeuralNetworkUtil::getConnectedNeurons(synapse);
				bool internal = true;
				for(QListIterator<Neuron*> j(connectedNeurons); j.hasNext();) {
					if(!oldModuleNeurons.contains(j.next())) {
						internal = false;
						break;
					}
				}
				if(internal) {
					continue;
				}
				synapse->setSource(newNeuron);
			}
		}

		if(!isInputNeuron || !isOutputNeuron) {
			//remove all outsider synapses from / to this neuron.
	
			QList<Synapse*> synapses;
			if(!isInputNeuron) {
				synapses << neuron->getSynapses();
			}
			if(!isOutputNeuron) {
				synapses << neuron->getOutgoingSynapses();
			}

			while(!synapses.empty()) {
				Synapse *synapse = synapses.at(0);
				synapses.removeAll(synapse);

				synapses << synapse->getSynapses();

				//check if this synapse is NOT a internal module synapse (which would be preserved).
				bool remove = false;
				if(!oldModuleNeurons.contains(synapse->getSource())) {
					remove = true;
				}

				if(!remove) {
					QList<Neuron*> connectedNeurons = NeuralNetworkUtil::getConnectedNeurons(synapse);
					for(QListIterator<Neuron*> j(connectedNeurons); j.hasNext();) {
						if(!oldModuleNeurons.contains(j.next())) {
							remove = true;
							break;
						}
					}
				}

				if(remove) {
					synapse->setSource(0);
					SynapseTarget *target = synapse->getTarget();
					if(target != 0) {
						target->removeSynapse(synapse);
					}
					trashcan.append(synapse);
				}
			}
		}
	}


	QPointF pos = NeuralNetworkUtil::getPosition(oldModule->getProperty(
							NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
	replacement->moveGroupToLocation(pos.x(), pos.y(), 0.0);


	NeuroModule *owner = oldModule->getParentModule();

	ownerNetwork->removeNeuronGroup(oldModule);
	ownerNetwork->addNeuronGroup(replacement);

	//set the type (role) of this module.
	replacement->setProperty(NeuralNetworkConstants::TAG_MODULE_TYPE, 
			oldModule->getProperty(NeuralNetworkConstants::TAG_MODULE_TYPE));
	//set the identifier id
	if(oldModule->hasProperty(NeuralNetworkConstants::TAG_MODULE_IDENTIFIER)) {
		QString idString = oldModule->getProperty(NeuralNetworkConstants::TAG_MODULE_IDENTIFIER);
		replacement->setProperty(NeuralNetworkConstants::TAG_MODULE_IDENTIFIER, idString);

		//make sure that the replacement gets the same id as the old module 
		//(to ensure that constraints still work)
		bool ok = true;
		qulonglong id = idString.toULongLong(&ok);
		if(ok) {
			replacement->setId(id);
		}
	}

	if(owner != 0) {
		owner->removeSubModule(oldModule);
		owner->addSubModule(replacement);

		//exchange pointers in all neuronGroups (ignore modules)
		for(QListIterator<NeuronGroup*> k(ownerNetwork->getNeuronGroups()); k.hasNext();) {
			NeuronGroup *ng = k.next();
			NeuroModule *nm = dynamic_cast<NeuroModule*>(ng);
			if(nm == 0 && ng != 0 && ng->getSubModules().contains(oldModule)) {
				ng->removeSubModule(oldModule);
				ng->addSubModule(replacement);
			}
		}
	}

	ownerNetwork->validateSynapseConnections();

	//move entire old module into trashcan (make sure that synapses are not double deleted 
	//by neurons / synapses).
	//TODO use savely remove?
	//TODO notify constraints about id changes.
	QList<NeuralNetworkElement*> elems(oldModule->getAllEnclosedNetworkElements());
	for(QListIterator<NeuralNetworkElement*> i(elems); i.hasNext();) {
		SynapseTarget *target = dynamic_cast<SynapseTarget*>(i.next());
		if(target != 0) {
			QList<Synapse*> synapses = target->getSynapses();
			for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
				Synapse *synapse = j.next();
				target->removeSynapse(synapse);
				synapse->setSource(0);
			}	
		}
	}
	trashcan << elems;
	trashcan.append(oldModule);

	return true;
}




}



