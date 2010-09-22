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



#include "ReplaceModuleCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"
#include "Util/ReplaceModuleHandler.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ReplaceModuleCommand.
 */
ReplaceModuleCommand::ReplaceModuleCommand(NetworkVisualization *context, NeuroModule *oldModule, 
						NeuroModule *newModule)
	: Command("Replace Module"), mVisualizationContext(context), 
	  mOldModule(oldModule), mNewModule(newModule), mOldModuleId(0)
{
	if(mOldModule != 0) {
		mOldModuleId = mOldModule->getId();
	}
}


/**
 * Destructor.
 */
ReplaceModuleCommand::~ReplaceModuleCommand() {
}

bool ReplaceModuleCommand::isUndoable() const {
	return false;
}


bool ReplaceModuleCommand::doCommand() {

	if(mOldModule == 0 || mNewModule == 0 || mVisualizationContext == 0) {
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

	if(!network->getNeuroModules().contains(mOldModule)) {
		return false;
	}


	QList<NeuralNetworkElement*> trashcan;
	QString errorMessage;

	if(!ReplaceModuleHandler::canBeReplaced(mOldModule, mNewModule, &errorMessage)) {
		mMessage = "Replace Module Check Failed:";
		mMessage.append("\nError Message: ").append(errorMessage).append("\n");
		
		cerr << mMessage.toStdString().c_str() << endl;
		return false;
	}

	errorMessage = "";

	bool ok = ReplaceModuleHandler::replaceModule(mOldModule, mNewModule, trashcan, &errorMessage);

	mMessage = "Replace Module (";
	if(ok) {
		mMessage.append("ok)\n");
	}
	else {
		mMessage.append("failed)\n");
	}
	mMessage.append("\nError Message: ").append(errorMessage).append("\n");
	
	cerr << mMessage.toStdString().c_str() << endl;
	cerr << "Trash: " << trashcan.size() << endl;

	handler->rebuildView();

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	while(!trashcan.empty()) {
		NeuralNetworkElement *elem = trashcan.at(0);
		trashcan.removeAll(elem);
		delete elem;
	}

	return false;
}


bool ReplaceModuleCommand::undoCommand() {
// 	if(mSourceNeuron == 0 || mTarget == 0 || mNewSynapse == 0 || mVisualizationContext == 0) {
// 		return false;
// 	}	
// 	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
// 			mVisualizationContext->getVisualizationHander());
// 
// 	if(handler == 0) {
// 		return false;
// 	}
// 	QMutexLocker guard(mVisualizationContext->getSelectionMutex());
// 
// 	SynapseItem *synapseItem = handler->getSynapseItem(mNewSynapse);
// 	if(synapseItem == 0) {
// 		return false;
// 	}
// 	mInsertPosition = synapseItem->getGlobalPosition();
// 
// 	NeuralNetwork *network = handler->getNeuralNetwork();
// 
// 	if(network == 0) {
// 		return false;
// 	}
	

// 	handler->rebuildView();
// 
// 	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
// 	mVisualizationContext->notifyNeuralNetworkModified();

	return false;
}


QString ReplaceModuleCommand::getMessage() const {
	return mMessage;
}


}



