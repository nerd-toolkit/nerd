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



#include "InsertGroupCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new InsertGroupCommand.
 */
InsertGroupCommand::InsertGroupCommand(NetworkVisualization *context, NeuronGroup *newGroup, 
										 QPointF position)
	: Command("Insert Group"), mVisualizationContext(context), 
	  mNewGroup(newGroup), mInsertPosition(position)
{
}



/**
 * Destructor.
 */
InsertGroupCommand::~InsertGroupCommand() {
}

bool InsertGroupCommand::isUndoable() const {
	return true;
}


bool InsertGroupCommand::doCommand() {
	if(mNewGroup == 0 || mVisualizationContext == 0) {
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

	NeuralNetworkUtil::setNetworkElementLocationProperty(mNewGroup, 
						mInsertPosition.x(), mInsertPosition.y(), 0.0);

	if(!network->addNeuronGroup(mNewGroup)) {
		return false;
	}

	//handler->rebuildView();
	QList<NeuralNetworkElement*> elems;
	elems.append(mNewGroup);
	handler->setSelectionHint(elems);

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	return true;
}


bool InsertGroupCommand::undoCommand() {
	if(mNewGroup == 0 || mVisualizationContext == 0) {
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

	if(!network->removeNeuronGroup(mNewGroup)) {
		return false;
	}

	GroupItem *groupItem = handler->getGroupItem(mNewGroup);
	if(groupItem == 0) {
		return false;
	}

	mInsertPosition = groupItem->getLocalPosition();

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


}




