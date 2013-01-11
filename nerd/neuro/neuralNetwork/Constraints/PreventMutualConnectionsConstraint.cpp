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


#include "PreventMutualConnectionsConstraint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Util/Util.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new PreventMutualConnectionsConstraint.
 */
PreventMutualConnectionsConstraint::PreventMutualConnectionsConstraint()
	: GroupConstraint("PreventMutualConnections")
{
}


/**
 * Copy constructor. 
 * 
 * @param other the PreventMutualConnectionsConstraint object to copy.
 */
PreventMutualConnectionsConstraint::PreventMutualConnectionsConstraint(
						const PreventMutualConnectionsConstraint &other) 
	: Object(), ValueChangedListener(), GroupConstraint(other)
{
}

/**
 * Destructor.
 */
PreventMutualConnectionsConstraint::~PreventMutualConnectionsConstraint() {
}

GroupConstraint* PreventMutualConnectionsConstraint::createCopy() const {
	return new PreventMutualConnectionsConstraint(*this);
}

bool PreventMutualConnectionsConstraint::isValid(NeuronGroup*) {
	return true;
}


bool PreventMutualConnectionsConstraint::applyConstraint(NeuronGroup *owner, 
									CommandExecutor*, 
									QList<NeuralNetworkElement*> &trashcan)
{
	if(owner == 0) {
		//do not do anything.
		return true;
	}

	bool networkChanged = false;

	ModularNeuralNetwork *net = owner->getOwnerNetwork();

	QList<Neuron*> neurons = owner->getNeurons();
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		
		QList<Synapse*> synapses = neuron->getSynapses();
		for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
			Synapse *synapse = j.next();
			if(neurons.contains(synapse->getSource())) {
				//remove synapse(s)
				Util::addWithoutDuplicates(trashcan, net->savelyRemoveNetworkElement(synapse));
				networkChanged = true;
			}
		}
	}

	if(owner->getOwnerNetwork() != 0) {
		owner->getOwnerNetwork()->validateSynapseConnections();
	}
	return !networkChanged;
}
		
bool PreventMutualConnectionsConstraint::equals(GroupConstraint *constraint) const {
	if(!GroupConstraint::equals(constraint)) {
		return false;
	}
	PreventMutualConnectionsConstraint *pmcc = 
					dynamic_cast<PreventMutualConnectionsConstraint*>(constraint);

	if(pmcc == 0) {
		return false;
	}
	return true;
}


}



