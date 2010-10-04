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
	delete mNewSynapseFunction;
}

bool ChangeSynapseFunctionCommand::isUndoable() const {
	return true;
}


bool ChangeSynapseFunctionCommand::doCommand() {

	if(mNewSynapseFunction == 0 || mVisualizationContext == 0) {
		return false;
	}	

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	while(!mOldSynapseFunctions.empty()) {
		SynapseFunction *sf = mOldSynapseFunctions.front();
		mOldSynapseFunctions.removeAll(sf);
		delete sf;
	}

	for(QListIterator<Synapse*> i(mSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		if(synapse == 0) {
			continue;
		}
		mOldSynapseFunctions.append(synapse->getSynapseFunction()->createCopy());
		synapse->setSynapseFunction(*mNewSynapseFunction);

	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


bool ChangeSynapseFunctionCommand::undoCommand() {
	if(mNewSynapseFunction == 0 || mVisualizationContext == 0) {
		return false;
	}	

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	for(int i = 0; i < mSynapses.size() && i < mOldSynapseFunctions.size(); ++i) {
		Synapse *synapse = mSynapses.at(i);
		SynapseFunction *sf = mOldSynapseFunctions.at(i);
		if(synapse == 0 || sf == 0) {
			continue;
		}
		synapse->setSynapseFunction(*sf);
	}
		
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}



}



