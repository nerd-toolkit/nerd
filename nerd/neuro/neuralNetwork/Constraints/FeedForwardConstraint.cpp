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



#include "FeedForwardConstraint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Util/Util.h"
#include "Util/NeuralNetworkUtil.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new FeedForwardConstraint.
 */
FeedForwardConstraint::FeedForwardConstraint()
	: GroupConstraint("FeedForward")
{
}


/**
 * Copy constructor. 
 * 
 * @param other the FeedForwardConstraint object to copy.
 */
FeedForwardConstraint::FeedForwardConstraint(const FeedForwardConstraint &other)
	: Object(), ValueChangedListener(), GroupConstraint(other) 
{
}

/**
 * Destructor.
 */
FeedForwardConstraint::~FeedForwardConstraint() {
}

GroupConstraint* FeedForwardConstraint::createCopy() const {
	return new FeedForwardConstraint(*this);
}

bool FeedForwardConstraint::isValid(NeuronGroup*) {
	return true;
}


bool FeedForwardConstraint::applyConstraint(NeuronGroup *owner, 
									CommandExecutor*, 
									QList<NeuralNetworkElement*> &trashcan)
{
	mErrorMessage = "";
	
	if(owner == 0) {
		//do not do anything.
		return true;
	}

	bool networkChanged = false;

	ModularNeuralNetwork *net = owner->getOwnerNetwork();

	IntValue *currentGenerationValue = Core::getInstance()->getValueManager()->getIntValue(
								NeuralNetworkConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER);
	int currentGeneration = 0;
	if(currentGenerationValue != 0) {
		currentGeneration = currentGenerationValue->get();
	}

	QList<Neuron*> neurons = owner->getNeurons();
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		
		bool retry = true;
		while(retry) {

			retry = false;
			
			QList<Synapse*> recurrentChain = NeuralNetworkUtil::getRecurrenceChain(neuron, neuron, neurons);

			if(!recurrentChain.empty()) {
				
				//find newest synapse and delete that one, then recheck if the loop is resolved.
				//heuristic: newest synapse is the one with the largest unique id.
				Synapse *newest = 0;
				for(QListIterator<Synapse*> j(recurrentChain); j.hasNext();) {
					Synapse *synapse = j.next();
					if(synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
						|| synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECT_EXISTENCE))
					{
						continue;
					}
					if(newest == 0 || newest->getId() < synapse->getId()) {
						newest = synapse;
					}
				}
				if(newest != 0) {
					trashcan <<  net->savelyRemoveNetworkElement(newest);
					retry = true;
				}
				mErrorMessage += QString("There was a loop of size ") 
								+ QString::number(recurrentChain.size()) 
								+ QString(" detected!\n");
				networkChanged = true;
			}
		}
	}

	return !networkChanged;
}
		
bool FeedForwardConstraint::equals(GroupConstraint *constraint) {
	if(!GroupConstraint::equals(constraint)) {
		return false;
	}
	FeedForwardConstraint *cc = 
					dynamic_cast<FeedForwardConstraint*>(constraint);

	if(cc == 0) {
		return false;
	}
	return true;
}


}



