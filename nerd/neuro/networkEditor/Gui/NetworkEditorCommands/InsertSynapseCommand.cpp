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



#include "InsertSynapseCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new InsertSynapseCommand.
 */
InsertSynapseCommand::InsertSynapseCommand(NetworkVisualization *context, Synapse *synapse, 
						Neuron *sourceNeuron, SynapseTarget *target, QPointF position)
	: Command("Insert Synapse"), mVisualizationContext(context)
{
	mNewSynapses.append(synapse);
	mSourceNeurons.append(sourceNeuron);
	mTargets.append(target);
	mInsertPositions.append(Vector3D(position.x(), position.y(), 0.0));
}

InsertSynapseCommand::InsertSynapseCommand(NetworkVisualization *context, QList<Synapse*> synapses, 
					 QList<Neuron*> sourceNeurons, QList<SynapseTarget*> targets, 
					 QList<Vector3D> positions)
	: Command("Insert Synapses"), mVisualizationContext(context), mNewSynapses(synapses),
		mSourceNeurons(sourceNeurons), mTargets(targets), mInsertPositions(positions)
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

	if(mSourceNeurons.empty() 
		|| mTargets.empty() 
		|| mNewSynapses.empty() 
		|| mSourceNeurons.size() != mTargets.size()
		|| mTargets.size() != mNewSynapses.size()
		|| mVisualizationContext == 0) 
	{
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
	
	bool ok = true;
	for(int i = 0; i < mNewSynapses.size(); ++i) {
		
		Synapse *synapse = mNewSynapses.at(i);
		Neuron *source = mSourceNeurons.at(i);
		SynapseTarget *target = mTargets.at(i);

		if(network->getSynapses().contains(synapse)) {
			ok = false;
			continue;
		}
		if(!network->getNeurons().contains(source) 
			|| (!network->getSynapses().contains(dynamic_cast<Synapse*>(target))
				&& (!network->getNeurons().contains(dynamic_cast<Neuron*>(target)))))
		{
			ok = false;
			continue;
		}

		//check for position
		Vector3D position;
		if(i < mInsertPositions.size()) {
			position = mInsertPositions.at(i);
		}
		else {
			//create default
			Vector3D sourcePos = source->getPosition();
			Vector3D targetPos = target->getPosition();
			QPointF newPos = Math::centerOfLine(QPointF(sourcePos.getX(), sourcePos.getY()), 
												QPointF(targetPos.getX(), targetPos.getY()));
			position = Vector3D(newPos.x(), newPos.y(), 0.0);
		}

		NeuralNetworkUtil::setNetworkElementLocationProperty(
						synapse, position.getX(), position.getY(), position.getZ());
		
		synapse->setSource(source);
		synapse->setTarget(target);

		if(!target->addSynapse(synapse)) {
			ok = false;
			continue;
		}
	}

	//handler->rebuildView();
	QList<NeuralNetworkElement*> elems;
	for(int i = 0; i < mNewSynapses.size(); ++i) {
		elems.append(mNewSynapses.at(i));
	}
	handler->setSelectionHint(elems);

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();

	return ok;
}


bool InsertSynapseCommand::undoCommand() {
	
	if(mSourceNeurons.empty() 
		|| mTargets.empty() 
		|| mNewSynapses.empty() 
		|| mSourceNeurons.size() != mTargets.size()
		|| mTargets.size() != mNewSynapses.size()
		|| mVisualizationContext == 0) 
	{
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());
	
	mInsertPositions.clear();
	
	bool ok = true;

	//go from back to front to support the addition of higher order synapse assemblies.
	for(int i = mNewSynapses.size() - 1; i >= 0; --i) {
		
		Synapse *synapse = mNewSynapses.at(i);
		Neuron *source = mSourceNeurons.at(i);
		SynapseTarget *target = mTargets.at(i);

		mInsertPositions.prepend(synapse->getPosition());

		NeuralNetwork *network = handler->getNeuralNetwork();

		if(network == 0) {
			return false;
		}
		if(!network->getSynapses().contains(synapse)) {
			ok = false;
			continue;
		}
		if(!network->getNeurons().contains(source) 
			|| (!network->getSynapses().contains(dynamic_cast<Synapse*>(target))
				&& (!network->getNeurons().contains(dynamic_cast<Neuron*>(target)))))
		{
			ok = false;
			continue;
		}

		//remove synapse
		if(!target->removeSynapse(synapse)) {
			return false;
		}
		synapse->setSource(0);
		synapse->setTarget(0);
		
	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	
	return ok;
}





}




