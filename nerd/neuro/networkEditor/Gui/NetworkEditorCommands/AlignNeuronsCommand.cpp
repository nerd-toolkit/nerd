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



#include "AlignNeuronsCommand.h"
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
 * Constructs a new AlignNeuronsCommand.
 * 
 * TODO: Temporariliy added modules and other network items here, but did not rename
 * 			the neuron related variable names!
 */
AlignNeuronsCommand::AlignNeuronsCommand(int mode, bool adjustDistance, NetworkVisualization *context, 
										 QList<NeuralNetworkElement*> neurons)
	: Command(mode == HORIZONTAL ? "AlignHorizontal" : "AlignVertical"), 
	  mMode(mode), mAdjustDistance(adjustDistance), mVisualizationContext(context), 
	  mNeuronsToAlign(neurons)
{
}


/**
 * Destructor.
 */
AlignNeuronsCommand::~AlignNeuronsCommand() {
}

bool AlignNeuronsCommand::isUndoable() const {
	return true;
}


bool AlignNeuronsCommand::doCommand() {

	if(mNeuronsToAlign.size() < 2 || mVisualizationContext == 0) {
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

	//calculate desired distance based on the positions of the upperleft elements.
	double distance = 1.0;
	double startingPos = 0.0;

	QList<NeuralNetworkElement*> mSortedNeurons;
	for(QListIterator<NeuralNetworkElement*> i(mNeuronsToAlign); i.hasNext();) {
		NeuralNetworkElement *neuron = i.next();
		Vector3D pos = neuron->getPosition();
		
		bool added = false;
		for(int j = 0; j < mSortedNeurons.size(); ++j) {
			Vector3D currentPos = mSortedNeurons.at(j)->getPosition();
			if((mMode == HORIZONTAL && currentPos.getX() > pos.getX())
				|| (mMode != HORIZONTAL && currentPos.getY() > pos.getY())) 
			{
				mSortedNeurons.insert(j, neuron);
				added = true;
				break;
			}
		}
		if(!added) {
			mSortedNeurons.append(neuron);
		}
	}

	if(mSortedNeurons.size() < 2) {
		return true;
	}

	if(mMode == HORIZONTAL) {
		if(mAdjustDistance) {
			startingPos = mSortedNeurons.at(0)->getPosition().getX();
			distance = mSortedNeurons.at(1)->getPosition().getX() - startingPos;
		}
		else {
			startingPos = mSortedNeurons.at(0)->getPosition().getY();
		}
	}
	else {
		if(mAdjustDistance) {
			startingPos = mSortedNeurons.at(0)->getPosition().getY();
			distance = mSortedNeurons.at(1)->getPosition().getY() - startingPos;
		}
		else {
			startingPos = mSortedNeurons.at(0)->getPosition().getX();
		}
	}

	mNeuronsToAlign = mSortedNeurons;

	for(int i = 0; i < mNeuronsToAlign.size(); ++i) {
		NeuralNetworkElement *neuron = mNeuronsToAlign.at(i);

		if(neuron == 0) {
			continue;
		}

		mPreviousPositions.append(neuron->getPosition());
		Vector3D pos = neuron->getPosition();
		if(mMode == HORIZONTAL) {
			if(mAdjustDistance) {
				pos.setX(startingPos + (i * distance));
			}
			else {
				pos.setY(startingPos);
			}
		}
		else {
			if(mAdjustDistance) {
				pos.setY(startingPos + (i * distance));
			}
			else {
				pos.setX(startingPos);
			}
		}
		if(dynamic_cast<NeuroModule*>(neuron) != 0) {
			NeuralNetworkUtil::moveNeuroModuleTo(dynamic_cast<NeuroModule*>(neuron),
												 pos.getX(), pos.getY(), pos.getZ());
		}
		else {
			neuron->setPosition(pos);
		}
	}

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


bool AlignNeuronsCommand::undoCommand() {
	if(mNeuronsToAlign.empty() || mVisualizationContext == 0) {
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

	
	for(int i = 0; i < mNeuronsToAlign.size() && i < mPreviousPositions.size(); ++i) {
		NeuralNetworkElement *neuron = mNeuronsToAlign.at(i);
		Vector3D pos = mPreviousPositions.at(i);

		if(neuron == 0) {
			continue;
		}
		if(dynamic_cast<NeuroModule*>(neuron) != 0) {
			NeuralNetworkUtil::moveNeuroModuleTo(dynamic_cast<NeuroModule*>(neuron),
												 pos.getX(), pos.getY(), pos.getZ());
		}
		else {
			neuron->setPosition(pos);
		}
	}
	
	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}



}



