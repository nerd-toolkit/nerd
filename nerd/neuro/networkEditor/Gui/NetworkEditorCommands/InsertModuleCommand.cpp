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

#include "InsertModuleCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new InsertModuleCommand.
 */
InsertModuleCommand::InsertModuleCommand(NetworkVisualization *context, NeuroModule *newModule, 
										 QPointF position, NeuroModule *parent, QString name)
	: Command("Insert Module"), mVisualizationContext(context), 
	  mNewModule(newModule), mInsertPosition(position), mModuleSize(50.0, 50.0),
	  mParentModule(parent), mName(name)
{
	if(name != "") {
		newModule->setProperty("ModuleName", name);
	}
}



/**
 * Destructor.
 */
InsertModuleCommand::~InsertModuleCommand() {
}

bool InsertModuleCommand::isUndoable() const {
	return true;
}


bool InsertModuleCommand::doCommand() {
	if(mNewModule == 0 || mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHandler());

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

	if(!network->addNeuronGroup(mNewModule)) {
		return false;
	}
	if(mParentModule != 0) {
		if(!mParentModule->addSubModule(mNewModule)) {
			network->removeNeuronGroup(mNewModule);
			return false;
		}
	}
	
	mNewModule->setPosition(Vector3D(mInsertPosition.x(), mInsertPosition.y(), 0.0));
	mNewModule->setSize(QSizeF(mModuleSize.width(), mModuleSize.height()));
	
// 	NeuralNetworkUtil::setNetworkElementLocationProperty(mNewModule, mInsertPosition.x(),
// 								mInsertPosition.y(), 0.0);
// 	NeuralNetworkUtil::setNeuroModuleLocationSizeProperty(mNewModule, mModuleSize.width(),
// 								mModuleSize.height());


	//handler->rebuildView();
	QList<NeuralNetworkElement*> elems;
	elems.append(mNewModule);
	handler->setSelectionHint(elems);

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	return true;
}


bool InsertModuleCommand::undoCommand() {
	if(mNewModule == 0 || mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHandler());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	ModuleItem *moduleItem = handler->getModuleItem(mNewModule);
	if(moduleItem == 0) {
		return false;
	}

	ModularNeuralNetwork *network = handler->getNeuralNetwork();

	if(network == 0) {
		return false;
	}

	if(!network->removeNeuronGroup(mNewModule)) {
		return false;
	}

	mInsertPosition = moduleItem->getGlobalPosition();
	mModuleSize = moduleItem->getSize();
	
// 	if(mParentModule != 0) {
// 		ModuleItem *item = handler->getModuleItem(mParentModule);
// 		if(item != 0) {
// 			item->removeMemberItem(moduleItem);
// 		}
// 		mParentModule->removeSubModule(mNewModule);
// 	}
// 
// 	if(handler->removeModuleItem(moduleItem)) {
// 		delete moduleItem;
// 		Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
// 		mVisualizationContext->notifyNeuralNetworkModified();
// 		return true;
// 	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	return true;
}



}




