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



#include "ChangeNetworkFunctionCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuro.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ChangeNetworkFunctionCommand.
 */
ChangeNetworkFunctionCommand::ChangeNetworkFunctionCommand(NetworkVisualization *context, 
							NeuralNetwork *network, TransferFunction *newTransferFunction)
	: Command("Change DefaultSynapseFunction"), mVisualizationContext(context), mNetwork(network),
	  mNewTransferFunction(newTransferFunction), mOldTransferFunction(0), mNewActivationFunction(0), 
	  mOldActivationFunction(0), mNewSynapseFunction(0), mOldSynapseFunction(0)
{

}


ChangeNetworkFunctionCommand::ChangeNetworkFunctionCommand(NetworkVisualization *context, 
							NeuralNetwork *network, ActivationFunction *newActivationFunction)
	: Command("Change DefaultActivationFunction"), mVisualizationContext(context), mNetwork(network),
	  mNewTransferFunction(0), mOldTransferFunction(0), mNewActivationFunction(newActivationFunction), 
	  mOldActivationFunction(0), mNewSynapseFunction(0), mOldSynapseFunction(0)
{
}


ChangeNetworkFunctionCommand::ChangeNetworkFunctionCommand(NetworkVisualization *context, 
							NeuralNetwork *network, SynapseFunction *newSynapseFunction)	
: Command("Change DefaultSynapseFunction"), mVisualizationContext(context), mNetwork(network),
	  mNewTransferFunction(0), mOldTransferFunction(0), mNewActivationFunction(0), 
	  mOldActivationFunction(0), mNewSynapseFunction(newSynapseFunction), mOldSynapseFunction(0)
{
}



ChangeNetworkFunctionCommand::~ChangeNetworkFunctionCommand() {
	delete mNewTransferFunction;
	delete mNewActivationFunction;
	delete mNewSynapseFunction;
}


bool ChangeNetworkFunctionCommand::isUndoable() const {
	return true;
}

bool ChangeNetworkFunctionCommand::doCommand() {
	if((mNewSynapseFunction == 0 && mNewTransferFunction == 0 && mNewActivationFunction == 0)
		|| mVisualizationContext == 0 || mNetwork == 0) 
	{
		return false;
	}	

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	if(mNewSynapseFunction != 0) {
		mOldSynapseFunction = mNetwork->getDefaultSynapseFunction()->createCopy();
		mNetwork->setDefaultSynapseFunction(*mNewSynapseFunction);
	}
	else if(mNewTransferFunction != 0) {
		mOldTransferFunction = mNetwork->getDefaultTransferFunction()->createCopy();
		mNetwork->setDefaultTransferFunction(*mNewTransferFunction);
	}
	else if(mNewActivationFunction != 0) {
		mOldActivationFunction = mNetwork->getDefaultActivationFunction()->createCopy();
		mNetwork->setDefaultActivationFunction(*mNewActivationFunction);
	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


bool ChangeNetworkFunctionCommand:: undoCommand() {
	if((mNewSynapseFunction == 0 && mNewTransferFunction == 0 && mNewActivationFunction == 0)
		|| mVisualizationContext == 0 || mNetwork == 0) 
	{
		return false;
	}	

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	if(mOldSynapseFunction != 0) {
		mNetwork->setDefaultSynapseFunction(*mOldSynapseFunction);
		delete mOldSynapseFunction;
		mOldSynapseFunction = 0;
	}
	else if(mOldTransferFunction != 0) {
		mNetwork->setDefaultTransferFunction(*mOldTransferFunction);
		delete mOldTransferFunction;
		mOldTransferFunction = 0;
	}
	else if(mOldActivationFunction != 0) {
		mNetwork->setDefaultActivationFunction(*mOldActivationFunction);
		delete mOldActivationFunction;
		mOldActivationFunction = 0;
	}
	
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}



}




