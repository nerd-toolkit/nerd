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



#include "CopyPasteNetworkTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/CopyPasteNetworkCommand.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>

using namespace std;

namespace nerd {


/**
 * Constructs a new CopyPasteNetworkTool.
 */
CopyPasteNetworkTool::CopyPasteNetworkTool(QObject *owner)
	: NetworkManipulationTool(owner)
{
}


/**
 * Destructor.
 */
CopyPasteNetworkTool::~CopyPasteNetworkTool() {
}


void CopyPasteNetworkTool::clear() {
	NetworkManipulationTool::clear();
}


void CopyPasteNetworkTool::activate(NetworkVisualization *visu) {

	NetworkManipulationTool::activate(visu);

	if(mVisuContext == 0) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualizationHandler *handler = mVisuContext->getVisualizationHander();
	if(handler == 0) {
		return;
	}

	QList<Neuron*> neurons;
	QList<Synapse*> synapses;
	QList<NeuronGroup*> groups;
	QList<NeuroModule*> modules;

	QMutexLocker guard(mVisuContext->getSelectionMutex());
	QList<PaintItem*> items = mVisuContext->getSelectedItems();

	for(QListIterator<PaintItem*> i(items); i.hasNext();) {
		PaintItem *item = i.next();
		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
		if(neuronItem != 0) {
			if(neuronItem->getNeuron() != 0) {
				neurons.append(neuronItem->getNeuron());
			}
		}
		SynapseItem *synapseItem = dynamic_cast<SynapseItem*>(item);
		if(synapseItem != 0) {
			if(synapseItem->getSynapse() != 0) {
				synapses.append(synapseItem->getSynapse());
			}
		}
		ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
		if(moduleItem != 0) {
			if(moduleItem->getNeuroModule() != 0) {
				modules.append(moduleItem->getNeuroModule());
			}
		}
		GroupItem *groupItem = dynamic_cast<GroupItem*>(item);
		if(groupItem != 0) {
			if(groupItem->getNeuronGroup() != 0) {
				groups.append(groupItem->getNeuronGroup());
			}
		}
	}

	if(!neurons.empty() || !synapses.empty() || !groups.empty() || !modules.empty()) {
		CopyPasteNetworkCommand *command = 
				new CopyPasteNetworkCommand(mVisuContext, neurons, synapses, modules, groups);
		mVisuContext->getCommandExecutor()->executeCommand(command);
	}
	emit done();
}


QString CopyPasteNetworkTool::getStatusMessage() {
	return "Copy/Paste";
}



}



