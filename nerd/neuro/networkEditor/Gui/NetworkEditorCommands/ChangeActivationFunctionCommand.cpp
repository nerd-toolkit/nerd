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



#include "ChangeActivationFunctionCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuro.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ChangeActivationFunctionCommand.
 */
ChangeActivationFunctionCommand::ChangeActivationFunctionCommand(NetworkVisualization *context, 
					QList<Neuron*> neurons, ActivationFunction *newActivationFunction)
	: Command("Change ActivationFunction"), mVisualizationContext(context), mNeurons(neurons),
	  mNewActivationFunction(newActivationFunction)
{
}



/**
 * Destructor.
 */
ChangeActivationFunctionCommand::~ChangeActivationFunctionCommand() {
	while(!mOldActivationFunctions.empty()) {
		ActivationFunction *af = mOldActivationFunctions.front();
		mOldActivationFunctions.removeAll(af);
		delete af;
	}
	while(!mNewActivationFunctions.empty()) {
		ActivationFunction *af = mNewActivationFunctions.front();
		mNewActivationFunctions.removeAll(af);
		delete af;
	}
	delete mNewActivationFunction;
}

bool ChangeActivationFunctionCommand::isUndoable() const {
	return true;
}


bool ChangeActivationFunctionCommand::doCommand() {

	if(mNewActivationFunction == 0 || mVisualizationContext == 0) {
		return false;
	}	

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());
	
	bool allowUndo = true;
	
	if(!mNewActivationFunctions.empty() && mNewActivationFunctions.size() == mNeurons.size()) {
		for(int i = 0; i < mNeurons.size() && i < mNewActivationFunctions.size(); ++i) {
			Neuron *neuron = mNeurons.at(i);
			ActivationFunction *af = mNewActivationFunctions.at(i);
			
			if(neuron == 0 || af == 0) {
				allowUndo = false;
				continue;
			}
			mOldActivationFunctions.append(neuron->getActivationFunction());
			neuron->setActivationFunction(af);
		}
		mNewActivationFunctions.clear();
	}
	else {

		if(!mNewActivationFunctions.empty()) {
			while(!mNewActivationFunctions.empty()) {
				ActivationFunction *af = mNewActivationFunctions.front();
				mNewActivationFunctions.removeAll(af);
				delete af;
			}
			allowUndo = false;
		}

		for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
			if(neuron == 0) {
				continue;
			}
			mOldActivationFunctions.append(neuron->getActivationFunction());
			neuron->setActivationFunction(mNewActivationFunction->createCopy());
		}
		mNewActivationFunctions.clear();
	}

// 	while(!mOldActivationFunctions.empty()) {
// 		ActivationFunction *af = mOldActivationFunctions.front();
// 		mOldActivationFunctions.removeAll(af);
// 		delete af;
// 	}
// 
// 	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
// 		Neuron *neuron = i.next();
// 		if(neuron == 0) {
// 			continue;
// 		}
// 
// 		mOldActivationFunctions.append(neuron->getActivationFunction()->createCopy());
// 		neuron->setActivationFunction(*mNewActivationFunction);
// 	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return allowUndo;
}


bool ChangeActivationFunctionCommand::undoCommand() {
	if(mNewActivationFunction == 0 || mVisualizationContext == 0) 
	{
		return false;
	}	
	
	bool allowUndo = true;

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	for(int i = 0; i < mNeurons.size() && i < mOldActivationFunctions.size(); ++i) {
		Neuron *neuron = mNeurons.at(i);
		ActivationFunction *af = mOldActivationFunctions.at(i);
		if(neuron == 0 || af == 0) {
			allowUndo = false;
			continue;
		}
		mNewActivationFunctions.append(neuron->getActivationFunction());
		neuron->setActivationFunction(af);
	}
	
	mOldActivationFunctions.clear();
	
// 	for(int i = 0; i < mNeurons.size() && i < mOldActivationFunctions.size(); ++i) {
// 		Neuron *neuron = mNeurons.at(i);
// 		ActivationFunction *af = mOldActivationFunctions.at(i);
// 		if(neuron == 0 || af == 0) {
// 			continue;
// 		}
// 		neuron->setActivationFunction(*af);
// 	}
	
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return allowUndo;
}



}




