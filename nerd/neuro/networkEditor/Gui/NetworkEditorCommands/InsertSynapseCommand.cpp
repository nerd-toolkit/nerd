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



#include "InsertSynapseCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new InsertSynapseCommand.
 */
InsertSynapseCommand::InsertSynapseCommand(NetworkVisualization *context, Synapse *synapse, 
						Neuron *sourceNeuron, SynapseTarget *target, QPointF position)
	: Command("Insert Synapse"), mVisualizationContext(context), 
	  mNewSynapse(synapse), mSourceNeuron(sourceNeuron),
	  mTarget(target), mInsertPosition(position)
{
}



/**
 * Destructor.
 */
InsertSynapseCommand::~InsertSynapseCommand() {
}


bool InsertSynapseCommand::isUndoable() const {
	return true;
}


bool InsertSynapseCommand::doCommand() {

	if(mSourceNeuron == 0 || mTarget == 0 || mNewSynapse == 0 || mVisualizationContext == 0) {
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

	if(network->getSynapses().contains(mNewSynapse)) {
		return false;
	}
	if(!network->getNeurons().contains(mSourceNeuron) 
		|| (!network->getSynapses().contains(dynamic_cast<Synapse*>(mTarget))
			&& (!network->getNeurons().contains(dynamic_cast<Neuron*>(mTarget)))))
	{
		return false;
	}


	NeuralNetworkUtil::setNetworkElementLocationProperty(mNewSynapse, mInsertPosition.x(),
														 mInsertPosition.y(), 0.0);
	mNewSynapse->setSource(mSourceNeuron);
	mNewSynapse->setTarget(mTarget);

	if(!mTarget->addSynapse(mNewSynapse)) {
		return false;
	}

	//handler->rebuildView();
	QList<NeuralNetworkElement*> elems;
	elems.append(mNewSynapse);
	handler->setSelectionHint(elems);

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


bool InsertSynapseCommand::undoCommand() {
	if(mSourceNeuron == 0 || mTarget == 0 || mNewSynapse == 0 || mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	SynapseItem *synapseItem = handler->getSynapseItem(mNewSynapse);
	if(synapseItem == 0) {
		return false;
	}
	mInsertPosition = synapseItem->getGlobalPosition();

	NeuralNetwork *network = handler->getNeuralNetwork();

	if(network == 0) {
		return false;
	}
	if(!network->getSynapses().contains(mNewSynapse)) {
		return false;
	}
	if(!network->getNeurons().contains(mSourceNeuron) 
		|| (!network->getSynapses().contains(dynamic_cast<Synapse*>(mTarget))
			&& (!network->getNeurons().contains(dynamic_cast<Neuron*>(mTarget)))))
	{
		return false;
	}

	//remove synapse
	if(!mTarget->removeSynapse(mNewSynapse)) {
		return false;
	}
	mNewSynapse->setSource(0);
	mNewSynapse->setTarget(0);

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	
	return true;
}





}




