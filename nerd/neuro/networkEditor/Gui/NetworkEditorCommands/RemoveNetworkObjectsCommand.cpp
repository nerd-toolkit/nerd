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



#include "RemoveNetworkObjectsCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new RemoveNetworkObjectsCommand.
 */
RemoveNetworkObjectsCommand::RemoveNetworkObjectsCommand(NetworkVisualization *context, 
							QList<PaintItem*> items)
	: Command("Delete Objects"), mVisualizationContext(context)
{
	//extract neural network elements.
	for(QListIterator<PaintItem*> i(items); i.hasNext();) {
		PaintItem *item = i.next();
		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
		if(neuronItem != 0) {
			mRemovedNeurons.insert(neuronItem->getNeuron(), 0);
			continue;
		}
		SynapseItem *synapseItem = dynamic_cast<SynapseItem*>(item);
		if(synapseItem != 0) {
			mRemovedSynapses.append(synapseItem->getSynapse());
			continue;
		}
		ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
		if(moduleItem != 0) {
			mRemovedNeuronGroups.insert(moduleItem->getNeuroModule(), 0);
			continue;
		}
		GroupItem *groupItem = dynamic_cast<GroupItem*>(item);
		if(groupItem != 0) {
			mRemovedNeuronGroups.insert(groupItem->getNeuronGroup(), 0);
			continue;
		}
	}
}


RemoveNetworkObjectsCommand::RemoveNetworkObjectsCommand(NetworkVisualization *context, 
						QList<NeuralNetworkElement*> networkElements)
	: Command("Delete Objects"), mVisualizationContext(context)
{
	//extract neural network elements.
	for(QListIterator<NeuralNetworkElement*> i(networkElements); i.hasNext();) {
		NeuralNetworkElement *item = i.next();
		Neuron *neuronItem = dynamic_cast<Neuron*>(item);
		if(neuronItem != 0) {
			mRemovedNeurons.insert(neuronItem, 0);
			continue;
		}
		Synapse *synapseItem = dynamic_cast<Synapse*>(item);
		if(synapseItem != 0) {
			mRemovedSynapses.append(synapseItem);
			continue;
		}
		NeuroModule *moduleItem = dynamic_cast<NeuroModule*>(item);
		if(moduleItem != 0) {
			mRemovedNeuronGroups.insert(moduleItem, 0);
			continue;
		}
		NeuronGroup *groupItem = dynamic_cast<NeuronGroup*>(item);
		if(groupItem != 0) {
			mRemovedNeuronGroups.insert(groupItem, 0);
			continue;
		}
	}
}


/**
 * Destructor.
 */
RemoveNetworkObjectsCommand::~RemoveNetworkObjectsCommand() {
	//TODO
}


bool RemoveNetworkObjectsCommand::isUndoable() const {
	return true;
}



bool RemoveNetworkObjectsCommand::doCommand() {
	if(mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	ModularNeuralNetwork *network = handler->getNeuralNetwork();

	if(network == 0) {
		return false;
	}

	QList<NeuronGroup*> neuronGroups = network->getNeuronGroups();

	QList<Synapse*> allSynapses = network->getSynapses();
	QList<NeuroModule*> allModules = network->getNeuroModules();
	QList<Synapse*> removedSynapses = mRemovedSynapses;
	QList<Neuron*> removedNeurons = mRemovedNeurons.keys();
	QList<NeuronGroup*> removedGroups = mRemovedNeuronGroups.keys();
	mRemovedSynapses.clear();
	mRemovedNeurons.clear();
	mRemovedNeuronGroups.clear();

	//collect all NeuronGroups that can be savely removed and mark their neurons for deletion.
	while(!removedGroups.empty()) {
		NeuronGroup *group = removedGroups.takeFirst();

		NeuroModule *ownerModule = 0;

		NeuroModule *module = dynamic_cast<NeuroModule*>(group);
		if(module != 0) {
			QList<Neuron*> neurons = group->getAllEnclosedNeurons();
			bool containsInterfaceNeurons = false;
			for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
				Neuron *neuron = j.next();
				if(neuron->hasProperty(Neuron::NEURON_TYPE_INPUT)
					|| neuron->hasProperty(Neuron::NEURON_TYPE_OUTPUT))
				{
					containsInterfaceNeurons = true;
					break;
				}
			}
			if(containsInterfaceNeurons) {
				continue;
			}
	
			for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
				Neuron *neuron = j.next();
				if(!removedNeurons.contains(neuron)) {
					removedNeurons.append(neuron);
				}
			}

			QList<NeuroModule*> submodules = module->getSubModules();
			for(QListIterator<NeuroModule*> j(submodules); j.hasNext();) {
				NeuroModule *mod = j.next();
				if(!mRemovedNeuronGroups.contains(mod) && !removedGroups.contains(mod)) {
					removedGroups.append(mod);
				}
			}

			//Check if this module is a submodule
			for(QListIterator<NeuroModule*> j(allModules); j.hasNext();) {
				NeuroModule *mod = j.next();
				if(mod->getSubModules().contains(module)) {
					ownerModule = mod;
					break;
				}
			}
		}

		mRemovedNeuronGroups.insert(group, ownerModule);
		
	}

	//check for all neurons whether they can be deleted and add their synapses.
	for(QListIterator<Neuron*> i(removedNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		if(neuron->hasProperty(Neuron::NEURON_TYPE_INPUT)
			|| neuron->hasProperty(Neuron::NEURON_TYPE_OUTPUT))
		{
			continue;
		}
		for(QListIterator<Synapse*> s(allSynapses); s.hasNext();) {
			Synapse *synapse = s.next();
			if(synapse->getSource() == neuron || synapse->getTarget() == neuron) {
				if(!removedSynapses.contains(synapse)) {
					removedSynapses.append(synapse);
				}
			}
		}
		NeuroModule *ownerModule = 0;
		for(QListIterator<NeuroModule*> j(allModules); j.hasNext();) {
			NeuroModule *mod = j.next();
			if(mod->getNeurons().contains(neuron)) {
				ownerModule = mod;
				break;
			}
		}
		mRemovedNeurons.insert(neuron, ownerModule);
	}

	//add all synapses that have targets in the list of removed synapses.
	while(!removedSynapses.empty()) {
		Synapse *synapse = removedSynapses.takeFirst();
		for(QListIterator<Synapse*> s(allSynapses); s.hasNext();) {
			Synapse *other = s.next();
			if(other->getTarget() == synapse) {
				removedSynapses.append(other);
			}
		}
		mRemovedSynapses.append(synapse);
	}

	for(QListIterator<Synapse*> i(mRemovedSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		SynapseTarget *target = synapse->getTarget();
		if(target != 0) {
			target->removeSynapse(synapse);
			if(synapse->getSource() != 0) {
				synapse->getSource()->deregisterOutgoingSynapse(synapse);
			}
			synapse->setTarget(target);
		}
	}
	for(QListIterator<Neuron*> i(mRemovedNeurons.keys()); i.hasNext();) {
		network->removeNeuron(i.next());
	}
	for(QListIterator<NeuronGroup*> i(mRemovedNeuronGroups.keys()); i.hasNext();) {
		network->removeNeuronGroup(i.next());
	}
	

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	return true;
}



bool RemoveNetworkObjectsCommand::undoCommand() {
	if(mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	ModularNeuralNetwork *network = handler->getNeuralNetwork();

	if(network == 0) {
		return false;
	}

	//Add modules and add as submodule if required.
	QList<NeuronGroup*> groupsToAdd = mRemovedNeuronGroups.keys();
	for(QListIterator<NeuronGroup*> i(groupsToAdd); i.hasNext();) {	
		NeuronGroup *group = i.next();
		network->addNeuronGroup(group);
		NeuroModule *module = dynamic_cast<NeuroModule*>(group);
		if(module != 0) {
			NeuronGroup *owner = mRemovedNeuronGroups.value(module);
			if(owner != 0) {
				owner->addSubModule(module);
			}
		}
	}
	//Add neurons
	QList<Neuron*> neuronsToAdd = mRemovedNeurons.keys();
	for(QListIterator<Neuron*> i(neuronsToAdd); i.hasNext();) {
		Neuron *neuron = i.next();
		network->addNeuron(neuron);
		NeuroModule *module = mRemovedNeurons.value(neuron);
		if(module != 0) {
			module->addNeuron(neuron);
		}
	}
	//Add synapses and add to their targets.
	for(QListIterator<Synapse*> i(mRemovedSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		SynapseTarget *target = synapse->getTarget();
		if(target != 0) {
			target->addSynapse(synapse);
		}
		else {
			Core::log("RemoveNetworkObjectsCommand: Could not find synapse target!");
		}
	}

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	return true;
}




}




