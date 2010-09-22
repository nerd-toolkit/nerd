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



#include "MoveMembersToModuleCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Network/Neuro.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new MoveMembersToModuleCommand.
 */
MoveMembersToModuleCommand::MoveMembersToModuleCommand(NetworkVisualization *context, 
				QList<PaintItem*> itemsToMove, NeuronGroup *sourceGroup, NeuronGroup *targetGroup)
	: Command("Move to Group"), mVisualizationContext(context), mItemsToMove(itemsToMove),
	  mSourceGroup(sourceGroup), mTargetGroup(targetGroup)
{
	for(QListIterator<PaintItem*> i(itemsToMove); i.hasNext();) {
		PaintItem *item = i.next();
		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
		if(neuronItem != 0) {
			Neuron *neuron = neuronItem->getNeuron();
			if(sourceGroup == 0 || sourceGroup->getNeurons().contains(neuron)) {
				mMovedNeurons.append(neuron);
			}
			continue;
		}
		ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
		if(moduleItem != 0) {
			NeuroModule *module = moduleItem->getNeuroModule();
			NeuroModule *owner = dynamic_cast<NeuroModule*>(sourceGroup);
			if(owner == 0 || (owner != 0 && owner->getSubModules().contains(module))) {
				mMovedModules.append(module);
			}
			continue;
		}
	}

}



/**
 * Destructor.
 */
MoveMembersToModuleCommand::~MoveMembersToModuleCommand() {
}

bool MoveMembersToModuleCommand::isUndoable() const {
	return true;
}


bool MoveMembersToModuleCommand::doCommand() {

	if(mItemsToMove.empty() || mSourceGroup == mTargetGroup || mVisualizationContext == 0) {
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


	for(QListIterator<Neuron*> i(mMovedNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		if(mSourceGroup != 0) {
			mSourceGroup->removeNeuron(neuron);
		}
		if(mTargetGroup != 0) {
			mTargetGroup->addNeuron(neuron);
		}
	}
	for(QListIterator<NeuroModule*> i(mMovedModules); i.hasNext();) {
		NeuroModule *module = i.next();
		if(mSourceGroup != 0) {
			mSourceGroup->removeSubModule(module);
		}
		if(mTargetGroup != 0) {
			mTargetGroup->addSubModule(module);
		}
	}

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	return true;
}




bool MoveMembersToModuleCommand::undoCommand() {
	if(mItemsToMove.empty() || mSourceGroup == mTargetGroup || mVisualizationContext == 0) {
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

	for(QListIterator<Neuron*> i(mMovedNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		if(mTargetGroup != 0) {
			mTargetGroup->removeNeuron(neuron);
		}
		if(mSourceGroup != 0) {
			mSourceGroup->addNeuron(neuron);
		}
	}
	for(QListIterator<NeuroModule*> i(mMovedModules); i.hasNext();) {
		NeuroModule *module = i.next();
		if(mTargetGroup != 0) {
			mTargetGroup->removeSubModule(module);
		}
		if(mSourceGroup != 0) {
			mSourceGroup->addSubModule(module);
		}
	}

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	return true;
}


}




