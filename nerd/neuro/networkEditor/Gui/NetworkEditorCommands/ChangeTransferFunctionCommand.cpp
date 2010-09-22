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



#include "ChangeTransferFunctionCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuro.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new ChangeTransferFunctionCommand.
 */
ChangeTransferFunctionCommand::ChangeTransferFunctionCommand(NetworkVisualization *context, 
					QList<Neuron*> neurons, TransferFunction *newTransferFunction)
	: Command("Change TransferFunction"), mVisualizationContext(context), mNeurons(neurons),
	  mNewTransferFunction(newTransferFunction)
{
}



/**
 * Destructor.
 */
ChangeTransferFunctionCommand::~ChangeTransferFunctionCommand() {
	while(!mOldTransferFunctions.empty()) {
		TransferFunction *tf = mOldTransferFunctions.front();
		mOldTransferFunctions.removeAll(tf);
		delete tf;
	}
	delete mNewTransferFunction;
}

bool ChangeTransferFunctionCommand::isUndoable() const {
	return true;
}


bool ChangeTransferFunctionCommand::doCommand() {

	if( mNewTransferFunction == 0 || mVisualizationContext == 0) {
		return false;
	}	
	
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	while(!mOldTransferFunctions.empty()) {
		TransferFunction *tf = mOldTransferFunctions.front();
		mOldTransferFunctions.removeAll(tf);
		delete tf;
	}

	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		if(neuron == 0) {
			continue;
		}
		mOldTransferFunctions.append(neuron->getTransferFunction()->createCopy());
		neuron->setTransferFunction(*mNewTransferFunction);

	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


bool ChangeTransferFunctionCommand::undoCommand() {
	if(mNewTransferFunction == 0 || mVisualizationContext == 0) {
		return false;
	}	

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	for(int i = 0; i < mNeurons.size() && i < mOldTransferFunctions.size(); ++i) {
		Neuron *neuron = mNeurons.at(i);
		TransferFunction *tf = mOldTransferFunctions.at(i);
		if(neuron == 0 || tf == 0) {
			continue;
		}
		neuron->setTransferFunction(*tf);
	}
	
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


}




