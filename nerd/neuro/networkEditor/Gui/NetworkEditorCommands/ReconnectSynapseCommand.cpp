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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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



#include "ReconnectSynapseCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new ReconnectSynapseCommand.
 */
ReconnectSynapseCommand::ReconnectSynapseCommand(NetworkVisualization *context, Synapse *synapse, 
						NeuralNetworkElement *previous, NeuralNetworkElement *current)
	: Command("Reconnect Synapse"), mVisualizationContext(context), 
		mSynapse(synapse), mPreviousElement(previous), mCurrentElement(current)
{
}

/**
 * Destructor.
 */
ReconnectSynapseCommand::~ReconnectSynapseCommand() {
}


bool ReconnectSynapseCommand::isUndoable() const {
	return true;
}


bool ReconnectSynapseCommand::doCommand() {
	if(mSynapse == 0 || mPreviousElement == 0 || mCurrentElement == 0 || mVisualizationContext == 0) {
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

	QList<Synapse*> allSynapses = network->getSynapses();

	if(mSynapse->getSource() == mPreviousElement) {
		for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
			Synapse *synapse = i.next();
			if(synapse->getSource() == mCurrentElement 
					&& synapse->getTarget() == mSynapse->getTarget()) 
			{
				Core::log("ReconnectSynapseCommand: There was already such a synapse.");
				return false;
			}
		}
		Neuron *neuron = dynamic_cast<Neuron*>(mCurrentElement);
		if(neuron == 0) {
			Core::log("ReconnectSynapseCommand: Could not find source neuron.");
			return false;
		}
		mSynapse->setSource(neuron);
	}
	else if(mSynapse->getTarget() == mPreviousElement) {
		for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
			Synapse *synapse = i.next();
			if(synapse->getSource() == mSynapse->getSource() 
					&& synapse->getTarget() == mCurrentElement) 
			{
				Core::log("ReconnectSynapseCommand: There was already such a synapse.");
				return false;
			}
		}
		SynapseTarget *previousTarget = dynamic_cast<SynapseTarget*>(mPreviousElement);
		SynapseTarget *currentTarget = dynamic_cast<SynapseTarget*>(mCurrentElement);
		if(previousTarget == 0 || currentTarget == 0) {
			Core::log("ReconnectSynapseCommand: Could not find one of the targets.");
			return false;
		}
		previousTarget->removeSynapse(mSynapse);
		currentTarget->addSynapse(mSynapse);
	}
	else {
		Core::log("ReconnectSynapseCommand: Could not find source of target.");
		return false;
	}

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


bool ReconnectSynapseCommand::undoCommand() {
	if(mSynapse == 0 || mPreviousElement == 0 || mCurrentElement == 0 || mVisualizationContext == 0) {
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

	QList<Synapse*> allSynapses = network->getSynapses();

	if(mSynapse->getSource() == mCurrentElement) {
		for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
			Synapse *synapse = i.next();
			if(synapse->getSource() == mPreviousElement 
					&& synapse->getTarget() == mSynapse->getTarget()) 
			{
				Core::log("ReconnectSynapseCommand: There was already such a synapse.");
				return false;
			}
		}
		Neuron *neuron = dynamic_cast<Neuron*>(mPreviousElement);
		if(neuron == 0) {
			Core::log("ReconnectSynapseCommand: Could not find source neuron.");
			return false;
		}
		mSynapse->setSource(neuron);
	}
	else if(mSynapse->getTarget() == mCurrentElement) {
		for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
			Synapse *synapse = i.next();
			if(synapse->getSource() == mSynapse->getSource() 
					&& synapse->getTarget() == mPreviousElement) 
			{
				Core::log("ReconnectSynapseCommand: There was already such a synapse.");
				return false;
			}
		}
		SynapseTarget *previousTarget = dynamic_cast<SynapseTarget*>(mPreviousElement);
		SynapseTarget *currentTarget = dynamic_cast<SynapseTarget*>(mCurrentElement);
		if(previousTarget == 0 || currentTarget == 0) {
			Core::log("ReconnectSynapseCommand: Could not find one of the targets.");
			return false;
		}
		currentTarget->removeSynapse(mSynapse);
		previousTarget->addSynapse(mSynapse);
	}
	else {
		Core::log("ReconnectSynapseCommand: Could not find source of target.");
		return false;
	}

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}




}



