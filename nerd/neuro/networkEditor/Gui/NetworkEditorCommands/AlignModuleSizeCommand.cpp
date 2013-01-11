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



#include "AlignModuleSizeCommand.h"
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
 * Constructs a new AlignModuleSizeCommand.
 * 
 */
AlignModuleSizeCommand::AlignModuleSizeCommand(int mode, NetworkVisualization *context, 
										 QList<NeuroModule*> modules)
	: Command(mode == HORIZONTAL ? "AlignHorizontal" : "AlignVertical"), 
	  mMode(mode), mVisualizationContext(context), 
	  mModulesToAlign(modules)
{
}


/**
 * Destructor.
 */
AlignModuleSizeCommand::~AlignModuleSizeCommand() {
}

bool AlignModuleSizeCommand::isUndoable() const {
	return true;
}


bool AlignModuleSizeCommand::doCommand() {

	if(mModulesToAlign.size() < 2 || mVisualizationContext == 0) {
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

	QSizeF refSize = mModulesToAlign.last()->getSize();

	for(int i = 0; i < mModulesToAlign.size(); ++i) {
		NeuroModule *module = mModulesToAlign.at(i);

		if(module == 0) {
			continue;
		}

		mPreviousDimensions.append(module->getSize());
		QSizeF size = module->getSize();
		if(mMode == HORIZONTAL) {
			size.setWidth(refSize.width());
		}
		else {
			size.setHeight(refSize.height());
		}
		module->setSize(size);
	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();

	return true;
}


bool AlignModuleSizeCommand::undoCommand() {
	if(mModulesToAlign.empty() || mVisualizationContext == 0) {
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

	
	for(int i = 0; i < mModulesToAlign.size() && i < mPreviousDimensions.size(); ++i) {
		NeuroModule *module = mModulesToAlign.at(i);
		QSizeF size = mPreviousDimensions.at(i);

		if(module == 0) {
			continue;
		}
		module->setSize(size);
	}
	
	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}



}



