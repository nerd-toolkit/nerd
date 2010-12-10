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



#include "InsertSubnetworkCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new InsertSubnetworkCommand.
 */
InsertSubnetworkCommand::InsertSubnetworkCommand(NetworkVisualization *context, 
							ModularNeuralNetwork *subNet, QPointF position, bool makeIdsunique)
	: Command("Insert Subnet"), mVisualizationContext(context), mInsertionPosition(position),
	  mTargetNetwork(0), mParentModule(0)
{	
	initialize(subNet, position, makeIdsunique);
}


InsertSubnetworkCommand::InsertSubnetworkCommand(ModularNeuralNetwork *targetNetwork, 
								ModularNeuralNetwork *subNet, NeuroModule *parentModule,
								QPointF position, bool makeIdsunique)
	: Command("Insert Subnet"), mVisualizationContext(0), mInsertionPosition(position),
	  mTargetNetwork(targetNetwork), mParentModule(parentModule)
{
	initialize(subNet, position, makeIdsunique);
}


/**
 * Destructor.
 */
InsertSubnetworkCommand::~InsertSubnetworkCommand() {
}


bool InsertSubnetworkCommand::isUndoable() const {
	return true;
}


bool InsertSubnetworkCommand::doCommand() {
	SimpleNetworkVisualizationHandler *handler = 0;
	ModularNeuralNetwork *network = 0;
	if(mVisualizationContext != 0) {
	
		handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
				mVisualizationContext->getVisualizationHander());
	
		if(handler == 0) {
			return false;
		}

		QMutexLocker guard(mVisualizationContext->getSelectionMutex());
		QMutexLocker executionGuard(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

		network = handler->getNeuralNetwork();
	}
	else if(mTargetNetwork != 0) {
		network = mTargetNetwork;
	}

	if(network == 0) {
		return false;
	}


	QList<NeuralNetworkElement*> selectedItems;

	//move neurons to currentNetwork.
	for(QListIterator<Neuron*> i(mNewNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		network->addNeuron(neuron);
		selectedItems.append(neuron);

		QList<Synapse*> synapses = neuron->getSynapses();
		for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
			Synapse *synapse = j.next();
			if(mNewNeurons.contains(synapse->getSource())) {
				selectedItems.append(synapse);
			}
		}
	}	
	//move neuronModules to network (including constraints)
	for(QListIterator<NeuroModule*> i(mNewNeuroModules); i.hasNext();) {
		NeuroModule *module = i.next();
		network->addNeuronGroup(module);
		selectedItems.append(module);
	}
	//move neuronGroups to network (including constraints)
	for(QListIterator<NeuronGroup*> i(mNewNeuronGroups); i.hasNext();) {
		NeuronGroup *group = i.next();
		network->addNeuronGroup(group);
		selectedItems.append(group);
	}

	if(mParentModule != 0) {
		for(QListIterator<Neuron*> i(mRootNeurons); i.hasNext();) {
			mParentModule->addNeuron(i.next());
		}
		for(QListIterator<NeuroModule*> i(mRootModles); i.hasNext();) {
			mParentModule->addSubModule(i.next());
		}
	}

	if(handler != 0) {
		//handler->rebuildView();
		handler->setSelectionHint(selectedItems);

		Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
		//mVisualizationContext->notifyNeuralNetworkModified();
	
	
		//selectet items
		//handler->markElementsAsSelected(selectedItems);
	}

	

	return true;
}


bool InsertSubnetworkCommand::undoCommand() {
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
	

	if(network == 0) {
		return false;
	}

	//remove neurons 
	for(QListIterator<Neuron*> i(mNewNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		network->removeNeuron(neuron);
	}	
	//remove neuronModules 
	for(QListIterator<NeuroModule*> i(mNewNeuroModules); i.hasNext();) {
		NeuroModule *module = i.next();
		network->removeNeuronGroup(module);
	}
	//remove neuronGroups 
	for(QListIterator<NeuronGroup*> i(mNewNeuronGroups); i.hasNext();) {
		NeuronGroup *group = i.next();
		network->removeNeuronGroup(group);
	}

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


void InsertSubnetworkCommand::initialize(ModularNeuralNetwork *subNet, 
							QPointF, bool makeIdsunique) 
{
	if(subNet != 0) {
		ModularNeuralNetwork *copy = dynamic_cast<ModularNeuralNetwork*>(subNet->createCopy());

		mRootNeurons = copy->getRootNeurons();
		mRootModles = copy->getRootModules();

		QList<NeuralNetworkElement*> elements;
		copy->getNetworkElements(elements);
		elements.removeAll(copy->getDefaultNeuronGroup());

		//align position
		if(!elements.empty()) {
			Properties *elem = dynamic_cast<Properties*>(elements.at(0));
			if(elem != 0 && elem->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION)) {
				bool ok = true;
				QPointF pos = NeuralNetworkUtil::getPosition(
							elem->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION), &ok);
				if(ok) {
					QPointF offset = mInsertionPosition - pos;
					for(QListIterator<NeuralNetworkElement*> i(elements); i.hasNext();) {
						NeuralNetworkElement *nne = i.next();
						Properties *prop = dynamic_cast<Properties*>(nne);
						if(nne != 0 && prop != 0 && prop->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION)) {
							QPointF localPos = offset + NeuralNetworkUtil::getPosition(
									prop->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION), &ok);
							if(ok) {
								NeuralNetworkUtil::setNetworkElementLocationProperty(
									nne, localPos.x(), localPos.y(), 0.0);
							}
						}
					}
				}
			}
			
		}


		QHash<qulonglong, qulonglong> changedIds;

		for(QListIterator<NeuralNetworkElement*> i(elements); i.hasNext();) {
			NeuralNetworkElement *elem = i.next();
			if(makeIdsunique) {
				qulonglong newId = NeuralNetwork::generateNextId();
				changedIds.insert(elem->getId(), newId);
				elem->setId(newId);
			}
			Neuron *neuron = dynamic_cast<Neuron*>(elem);
			if(neuron != 0) {
				mNewNeurons.append(neuron);
				continue;
			}
			NeuroModule *module = dynamic_cast<NeuroModule*>(elem);
			if(module != 0) {
				mNewNeuroModules.append(module);
				continue;
			}
			NeuronGroup *group = dynamic_cast<NeuronGroup*>(elem);
			if(group != 0 && group != copy->getDefaultNeuronGroup()) {
				mNewNeuronGroups.append(group);
			}
		}
		if(!changedIds.empty()) {
			copy->notifyMemberIdsChanged(changedIds);
		}
		copy->freeElements(false);
		delete copy;
	}
}

}



