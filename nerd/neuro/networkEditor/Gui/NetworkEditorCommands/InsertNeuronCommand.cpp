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



#include "InsertNeuronCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new InsertNeuronCommand.
 */
InsertNeuronCommand::InsertNeuronCommand(NetworkVisualization *context, Neuron *neuron, 
										 QPointF position, NeuroModule *parent)
	: Command("Insert Neuron"), mVisualizationContext(context), 
	  mNeuron(neuron), mInsertPosition(position), mParentModule(parent)
{
}


/**
 * Destructor.
 */
InsertNeuronCommand::~InsertNeuronCommand() {
}


bool InsertNeuronCommand::isUndoable() const {
	return true;
}


bool InsertNeuronCommand::doCommand() {
	if(mNeuron == 0 || mVisualizationContext == 0) {
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

	if(mParentModule != 0 && !network->getNeuroModules().contains(mParentModule)) {
		return false;
	}

	if(!network->addNeuron(mNeuron)) {
		return false;
	}
	if(mParentModule != 0) {
		if(!mParentModule->addNeuron(mNeuron)) {
			network->removeNeuron(mNeuron);
			return false;
		}
	}

	NeuralNetworkUtil::setNetworkElementLocationProperty(
					mNeuron, mInsertPosition.x(), mInsertPosition.y(), 0.0);


	//handler->rebuildView();

	QList<NeuralNetworkElement*> elems;
	elems.append(mNeuron);
	handler->setSelectionHint(elems);

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	
	return true;
}


bool InsertNeuronCommand::undoCommand() {
	if(mNeuron == 0 || mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	NeuronItem *neuronItem = handler->getNeuronItem(mNeuron);
	if(neuronItem == 0) {
		return false;
	}
	mInsertPosition = neuronItem->getGlobalPosition();

	NeuralNetwork *network = handler->getNeuralNetwork();

	if(network == 0) {
		return false;
	}

	if(!network->removeNeuron(mNeuron)) {
		return false;
	}

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	return true;
}



}




