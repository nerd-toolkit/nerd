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



#include "AlignSynapseCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/SynapseTarget.h"
#include "Network/Neuron.h"
#include "Network/Synapse.h"
#include "Math/Math.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new AlignSynapseCommand.
 */
AlignSynapseCommand::AlignSynapseCommand(NetworkVisualization *context, QList<Synapse*> synapses)
	: Command("Align Synapses"), mVisualizationContext(context), 
	  mSynapsesToAlign(synapses)
{
}



/**
 * Destructor.
 */
AlignSynapseCommand::~AlignSynapseCommand() {
}


bool AlignSynapseCommand::isUndoable() const {
	return true;
}


bool AlignSynapseCommand::doCommand() {

	if(mSynapsesToAlign.empty() || mVisualizationContext == 0) {
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

	mPreviousPositions.clear();

	for(QListIterator<Synapse*> i(mSynapsesToAlign); i.hasNext();) {
		Synapse *synapse = i.next();

		if(synapse == 0) {
			continue;
		}

		mPreviousPositions.append(synapse->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));

		Neuron *source = synapse->getSource();
		Properties *target = dynamic_cast<Properties*>(synapse->getTarget());

		if(source == 0 || target == 0) {
			cerr << "Error1" << endl;
			return false;
		}

		bool sourceOk = false;
		bool targetOk = false;
		QPointF sourcePosition = NeuralNetworkUtil::getPosition(
						source->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION), &sourceOk);
		QPointF targetPosition = NeuralNetworkUtil::getPosition(
						target->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION), &targetOk);

		if(sourceOk == false || targetOk == false) {
			return false;
		}

		//check if there are synapses in both directions (requires an offset)
		double offset = 0.0;
		for(QListIterator<Synapse*> j(mSynapsesToAlign); j.hasNext();) {
			Synapse *s = j.next();
			if(s == 0) {
				continue;
			}
			if(s->getSource() == synapse->getTarget() 
				&& s->getTarget() == synapse->getSource())
			{
				offset = 20.0;
				break;
			}
		}

		QPointF pos = Math::centerOfLine(sourcePosition, 
										 targetPosition, offset);

		//check if this synapse is a self connection
		if(source == target) {
			pos = sourcePosition + QPointF(0.0, -70.0);
		}

		NeuralNetworkUtil::setNetworkElementLocationProperty(synapse, pos.x(), pos.y(), 0.0);
	}

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


bool AlignSynapseCommand::undoCommand() {
	if(mSynapsesToAlign.empty() || mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	NeuralNetwork *network = handler->getNeuralNetwork();

	if(network == 0) {
		return false;
	}

	
	for(int i = 0; i < mSynapsesToAlign.size() && i < mPreviousPositions.size(); ++i) {
		Synapse *synapse = mSynapsesToAlign.at(i);
		QString pos = mPreviousPositions.at(i);

		if(synapse == 0) {
			continue;
		}

		synapse->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, pos);
	}
	
	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


}



