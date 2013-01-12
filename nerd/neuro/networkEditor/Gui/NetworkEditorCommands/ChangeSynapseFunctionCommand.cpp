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



#include "ChangeSynapseFunctionCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuro.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ChangeSynapseFunctionCommand.
 */
ChangeSynapseFunctionCommand::ChangeSynapseFunctionCommand(NetworkVisualization *context,
				 QList<Synapse*> synapses, SynapseFunction *newSynapseFunction)
	: Command("Change SynapseFunction"), mVisualizationContext(context), mSynapses(synapses),
	  mNewSynapseFunction(newSynapseFunction)
{
}


/**
 * Destructor.
 */
ChangeSynapseFunctionCommand::~ChangeSynapseFunctionCommand() {
	while(!mOldSynapseFunctions.empty()) {
		SynapseFunction *sf = mOldSynapseFunctions.front();
		mOldSynapseFunctions.removeAll(sf);
		delete sf;
	}
	while(!mNewSynapseFunctions.empty()) {
		SynapseFunction *sf = mNewSynapseFunctions.front();
		mNewSynapseFunctions.removeAll(sf);
		delete sf;
	}
	delete mNewSynapseFunction;
}

bool ChangeSynapseFunctionCommand::isUndoable() const {
	return true;
}


bool ChangeSynapseFunctionCommand::doCommand() {

	if(mNewSynapseFunction == 0 || mVisualizationContext == 0) {
		return false;
	}	
	
	bool allowUndo = true;

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	if(!mNewSynapseFunctions.empty() && mNewSynapseFunctions.size() == mSynapses.size()) {
		for(int i = 0; i < mSynapses.size() && i < mNewSynapseFunctions.size(); ++i) {
			Synapse *synapse = mSynapses.at(i);
			SynapseFunction *sf = mNewSynapseFunctions.at(i);
			if(synapse == 0 || sf == 0) {
				allowUndo = false;
				continue;
			}
			mOldSynapseFunctions.append(synapse->getSynapseFunction());
			synapse->setSynapseFunction(sf);
		}
		mNewSynapseFunctions.clear();
	}
	else {

		if(!mNewSynapseFunctions.empty()) {
			while(!mNewSynapseFunctions.empty()) {
				SynapseFunction *sf = mNewSynapseFunctions.front();
				mNewSynapseFunctions.removeAll(sf);
				delete sf;
			}
			allowUndo = false;
		}

		for(QListIterator<Synapse*> i(mSynapses); i.hasNext();) {
			Synapse *synapse = i.next();
			if(synapse == 0) {
				continue;
			}
			mOldSynapseFunctions.append(synapse->getSynapseFunction());
			synapse->setSynapseFunction(mNewSynapseFunction->createCopy());
		}
		mNewSynapseFunctions.clear();
	}
	
// 	while(!mOldSynapseFunctions.empty()) {
// 		SynapseFunction *sf = mOldSynapseFunctions.front();
// 		mOldSynapseFunctions.removeAll(sf);
// 		delete sf;
// 	}
// 
// 	for(QListIterator<Synapse*> i(mSynapses); i.hasNext();) {
// 		Synapse *synapse = i.next();
// 		if(synapse == 0) {
// 			continue;
// 		}
// 		mOldSynapseFunctions.append(synapse->getSynapseFunction()->createCopy());
// 		synapse->setSynapseFunction(*mNewSynapseFunction);
// 
// 	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return allowUndo;
}


bool ChangeSynapseFunctionCommand::undoCommand() {
	if(mNewSynapseFunction == 0 || mVisualizationContext == 0) {
		return false;
	}	
	
	bool allowUndo = true;

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	for(int i = 0; i < mSynapses.size() && i < mOldSynapseFunctions.size(); ++i) {
		Synapse *synapse = mSynapses.at(i);
		SynapseFunction *sf = mOldSynapseFunctions.at(i);
		if(synapse == 0 || sf == 0) {
			allowUndo = false;
			continue;
		}
		mNewSynapseFunctions.append(synapse->getSynapseFunction());
		synapse->setSynapseFunction(sf);
	}
	
	mOldSynapseFunctions.clear();
	
// 	for(int i = 0; i < mSynapses.size() && i < mOldSynapseFunctions.size(); ++i) {
// 		Synapse *synapse = mSynapses.at(i);
// 		SynapseFunction *sf = mOldSynapseFunctions.at(i);
// 		if(synapse == 0 || sf == 0) {
// 			continue;
// 		}
// 		synapse->setSynapseFunction(*sf);
// 	}
		
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return allowUndo;
}



}




