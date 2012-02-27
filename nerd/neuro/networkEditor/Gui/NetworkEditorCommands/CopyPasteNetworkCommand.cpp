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



#include "CopyPasteNetworkCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new CopyPasteNetworkCommand.
 */
CopyPasteNetworkCommand::CopyPasteNetworkCommand(NetworkVisualization *context, 
				QList<Neuron*> neurons, 
				QList<Synapse*> synapses, QList<NeuroModule*> modules, 
				QList<NeuronGroup*> groups)
	: Command("Copy Objects"), mVisualizationContext(context), mNetwork(0)
{
	if(context != 0) {
		mNetwork = context->getNeuralNetwork();
	}

	mCopiedNeurons = neurons;
	mCopiedSynapses = synapses;
	mCopiedNeuronGroups = groups;
	mCopiedNeuroModules = modules;

}



/**
 * Destructor.
 */
CopyPasteNetworkCommand::~CopyPasteNetworkCommand() {
}


bool CopyPasteNetworkCommand::isUndoable() const {
	return true;
}


bool CopyPasteNetworkCommand::doCommand() {

	if(mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());
	QMutexLocker executionGuard(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	ModularNeuralNetwork *network = handler->getNeuralNetwork();
	

	if(network == 0 || network != mNetwork) {
		return false;
	}


	
	ModularNeuralNetwork *netCopy = 0;

	QHash<qulonglong, qulonglong> changedIds;

	if(mNewSynapses.empty() && mNewNeuroModules.empty() && mNewNeurons.empty()) {
		netCopy = dynamic_cast<ModularNeuralNetwork*>(network->createCopy());

		QList<NeuronGroup*> neuronGroups = network->getNeuronGroups();
	
		QList<Synapse*> allSynapses = network->getSynapses();
		QList<NeuroModule*> allModules = network->getNeuroModules();
		QList<Synapse*> synapses = mCopiedSynapses;
		QList<Neuron*> neurons = mCopiedNeurons;
		QList<NeuronGroup*> groups = mCopiedNeuronGroups;
		QList<NeuroModule*> modules = mCopiedNeuroModules;
		mCopiedSynapses.clear();
		mCopiedNeurons.clear();
		mCopiedNeuronGroups.clear();
		mCopiedNeuroModules.clear();


// 		for(int p = 0; p < netCopy->getNeuronGroups().size(); ++p) { //TODO what was this for?
// 			NeuronGroup *g = netCopy->getNeuronGroups().at(p);
// 			for(int q = 0; q < g->getSubModules().size(); q++) {
// 				NeuroModule *m = g->getSubModules().at(q);
// 			}
// 		}
	

		//collect all NeuronGroups that can be savely removed and mark their neurons for deletion.
		while(!modules.empty()) {
			NeuroModule *module = modules.takeFirst();

			//make sure that no submodule is copied without its parent module.
			bool skipModule = false;
			for(QListIterator<NeuroModule*> i(modules); i.hasNext();) {
				NeuroModule *m = i.next();
				if(m->getAllEnclosedModules().contains(module)) {
					skipModule = true;
					break;
				}
			}	
			if(skipModule) {
				//module is skipped and handled later (but triggered by its parent)
				continue;
			}
	
			QList<Neuron*> enclosedNeurons = module->getAllEnclosedNeurons();
	
			for(QListIterator<Neuron*> j(enclosedNeurons); j.hasNext();) {
				Neuron *neuron = j.next();
				if(!neurons.contains(neuron)) {
					neurons.append(neuron);
				}
			}
	
			QList<NeuroModule*> submodules = module->getSubModules();
			for(QListIterator<NeuroModule*> j(submodules); j.hasNext();) {
				NeuroModule *mod = j.next();
				if(!mCopiedNeuroModules.contains(mod) && !modules.contains(mod)) {
					modules.append(mod);
				}
			}
	
			NeuroModule *moduleCopy = ModularNeuralNetwork::selectNeuroModuleById(
									module->getId(), netCopy->getNeuroModules());
			if(moduleCopy == 0) {
				Core::log("CopyPasteNetworkCommand: Could not find copied module");
				return false;
			}
	
			mCopiedNeuroModules.append(module);
			qulonglong newId = NeuralNetwork::generateNextId();
			changedIds.insert(moduleCopy->getId(), newId);
			moduleCopy->setId(newId);
			mNewNeuroModules.append(moduleCopy);
		}
	
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *neuron = i.next();
	
			for(QListIterator<Synapse*> s(allSynapses); s.hasNext();) {
				Synapse *synapse = s.next();
				Neuron *targetAsNeuron = dynamic_cast<Neuron*>(synapse->getTarget());
				if(neurons.contains(synapse->getSource()) && neurons.contains(targetAsNeuron)) {
					if(!synapses.contains(synapse)) {
						synapses.append(synapse);
					}
				}
			}
	
			Neuron *neuronCopy = NeuralNetwork::selectNeuronById(neuron->getId(), netCopy->getNeurons());
			
			if(neuronCopy == 0) {
				Core::log("CopyPasteNetworkCommand: Could not find copied neuron");
				return false;
			}
	
			mCopiedNeurons.append(neuron);
			qulonglong newId = NeuralNetwork::generateNextId();
			changedIds.insert(neuronCopy->getId(), newId);
			neuronCopy->setId(newId);
			mNewNeurons.append(neuronCopy);
		}
	
		//add all synapses that have targets in the list of added synapses.
		while(!synapses.empty()) {
			Synapse *synapse = synapses.takeFirst();
			QList<Synapse*> incommingSynapses = synapse->getSynapses();
			for(QListIterator<Synapse*> s(incommingSynapses); s.hasNext();) {
				Synapse *other = s.next();
				if(mCopiedNeurons.contains(other->getSource())
					&& !synapses.contains(other)
					&& !mCopiedSynapses.contains(other)) 
				{
					synapses.append(other);
				}
			}
			Synapse *synapseCopy = NeuralNetwork::selectSynapseById(
						synapse->getId(), netCopy->getSynapses());
			
			if(synapseCopy == 0) {
				Core::log("CopyPasteNetworkCommand: Could not find copied synapse");
				return false;
			}
	
			mCopiedSynapses.append(synapse);
			qulonglong newId = NeuralNetwork::generateNextId();
			changedIds.insert(synapseCopy->getId(), newId);
			synapseCopy->setId(newId);
			mNewSynapses.insert(synapseCopy, 0);
		}
	}

	//change ids of the constraints of the new modules (do not apply to entire network).
	for(QListIterator<NeuroModule*> i(mNewNeuroModules); i.hasNext();) {
		i.next()->notifyMemberIdsChanged(changedIds);
	}

	//transfer new neurons and modules to old network
	for(QListIterator<NeuroModule*> i(mNewNeuroModules); i.hasNext();) {
		NeuroModule *module = i.next();
		if(netCopy != 0) {
			netCopy->removeNeuronGroup(module);
		}
		network->addNeuronGroup(module);
	}

	QList<Synapse*> newSynapses = mNewSynapses.keys();
	for(QListIterator<Neuron*> i(mNewNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		neuron->removeProperty(Neuron::NEURON_TYPE_INPUT);
		neuron->removeProperty(Neuron::NEURON_TYPE_OUTPUT);
		if(netCopy != 0) {
			netCopy->removeNeuron(neuron);
		}
		QList<Synapse*> incomingSynapses = neuron->getSynapses();
		for(QListIterator<Synapse*> j(incomingSynapses); j.hasNext();) {
			Synapse *synapse = j.next();
			if(!newSynapses.contains(synapse)) {
				synapse->setSource(0);
				neuron->removeSynapse(synapse);
			}
		}
		QList<Synapse*> outgoingSynapses = neuron->getOutgoingSynapses();
		for(QListIterator<Synapse*> j(outgoingSynapses); j.hasNext();) {
			Synapse *synapse = j.next();
			if(!newSynapses.contains(synapse)) {
				synapse->getSource()->removeSynapse(synapse);
				synapse->setSource(0);
			}
		}
		network->addNeuron(neuron);
	}
	for(QListIterator<Synapse*> i(newSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		SynapseTarget *synapseTarget = mNewSynapses.value(synapse);
		
		if(synapseTarget != 0) {
			synapseTarget->addSynapse(synapse);
		}
		else {
			SynapseTarget *target = synapse->getTarget();
			Neuron *targetAsNeuron = dynamic_cast<Neuron*>(target);
			if(targetAsNeuron != 0) {
				if(mNewNeurons.contains(targetAsNeuron)) {
					targetAsNeuron->addSynapse(synapse);
				}
				else {
					Neuron *origNeuron = NeuralNetwork::selectNeuronById(
								targetAsNeuron->getId(), network->getNeurons());
					if(origNeuron != 0) {
						targetAsNeuron->removeSynapse(synapse);
						origNeuron->addSynapse(synapse);
					}
				}
			}
			Synapse *targetAsSynapse = dynamic_cast<Synapse*>(target);
			if(targetAsSynapse != 0) {
				if(newSynapses.contains(targetAsSynapse)) {
					targetAsSynapse->addSynapse(synapse);
				}
				else {
					Synapse *origSynapse = NeuralNetwork::selectSynapseById(
								targetAsSynapse->getId(), network->getSynapses());
					if(origSynapse != 0) {
						targetAsSynapse->removeSynapse(synapse);
						origSynapse->addSynapse(synapse);
					}
				}
			}
		}
		Neuron *source = synapse->getSource();
		if(!mNewNeurons.contains(source)) {
			Neuron *origNeuron = NeuralNetwork::selectNeuronById(
							source->getId(), network->getNeurons());
			synapse->setSource(origNeuron);
		}
		else {
			//renew outgoing synapse pointers.
			synapse->setSource(synapse->getSource());
		}
		mNewSynapses.insert(synapse, synapse->getTarget());
	}
	
	if(netCopy != 0) {
		//TODO undo comment
		delete netCopy;
	}
	

	


	//get selected items
	QList<NeuralNetworkElement*> selectedItems;

	for(QListIterator<Neuron*> i(mNewNeurons); i.hasNext();) {
		selectedItems.append(i.next());
	}
	for(QListIterator<Synapse*> i(newSynapses); i.hasNext();) {
		selectedItems.append(i.next());
	}
	for(QListIterator<NeuroModule*> i(mNewNeuroModules); i.hasNext();) {
		selectedItems.append(i.next());
	}

	//handler->markElementsAsSelected(selectedItems);
	//handler->rebuildView();

	handler->setSelectionHint(selectedItems);

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


bool CopyPasteNetworkCommand::undoCommand() {
	if(mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());
	QMutexLocker executionGuard(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	ModularNeuralNetwork *network = handler->getNeuralNetwork();
	

	if(network == 0 || network != mNetwork) {
		return false;
	}

	QList<NeuronGroup*> allNeuronGroups = network->getNeuronGroups();
	QList<NeuroModule*> allNeuroModules = network->getNeuroModules();

	QList<Synapse*> newSynapses = mNewSynapses.keys();
	for(QListIterator<Synapse*> i(newSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		SynapseTarget *target = synapse->getTarget();
		if(target != 0 
			&& !mNewNeurons.contains(dynamic_cast<Neuron*>(target)) 
			&& !mNewSynapses.contains(dynamic_cast<Synapse*>(target))) 
		{
			target->removeSynapse(synapse);
			synapse->setSource(0);
		}
	}
	
	for(QListIterator<NeuroModule*> i(mNewNeuroModules); i.hasNext();) {
		NeuroModule *module = i.next();
		for(QListIterator<NeuronGroup*> j(allNeuronGroups); j.hasNext();) {
			NeuroModule *g = dynamic_cast<NeuroModule*>(j.next());
			if(g == 0) {
				continue;
			}
			if(g->getSubModules().contains(module) 
					&& !mNewNeuroModules.contains(g)) 
			{
				g->removeSubModule(module);
			}
		}
		network->removeNeuronGroup(module);
	}

	for(QListIterator<Neuron*> i(mNewNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		for(QListIterator<NeuronGroup*> j(allNeuronGroups); j.hasNext();) {
			NeuroModule *group = dynamic_cast<NeuroModule*>(j.next());
			if(group == 0) {
				continue;
			}
			if(group->getNeurons().contains(neuron) 
				&& !mNewNeuroModules.contains(group)) 
			{
				group->removeNeuron(neuron);
			}
		}
		network->removeNeuron(neuron);
	}

	

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();


// 	while(!mNewSynapses.empty()) {
// 		Synapse *synapse = mNewSynapses.takeFirst();
// 		mNewSynapses.removeAll(synapse);
// 		delete synapse;
// 	}
// 	while(!mNewNeurons.empty()) {
// 		Neuron *neuron = mNewNeurons.takeFirst();
// 		mNewNeurons.removeAll(neuron);
// 		delete neuron;
// 	}
// 	while(!mNewNeuroModules.empty()) {
// 		NeuroModule *module = mNewNeuroModules.takeFirst();
// 		mNewNeuroModules.removeAll(module);
// 		delete module;
// 	}

	return true;
}



}



