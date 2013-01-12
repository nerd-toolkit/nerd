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



#include "InsertSubnetworkIntoNetwork.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new InsertSubnetworkIntoNetwork.
 */
InsertSubnetworkIntoNetwork::InsertSubnetworkIntoNetwork()
{
}


/**
 * Destructor.
 */
InsertSubnetworkIntoNetwork::~InsertSubnetworkIntoNetwork() {
}

QList<NeuralNetworkElement*> InsertSubnetworkIntoNetwork::insertSubnetIntoNetwork
				(ModularNeuralNetwork *network, ModularNeuralNetwork *subnet) 
{

	QList<NeuralNetworkElement*> newElements;

	QList<Neuron*> newNeurons = subnet->getNeurons();
	QList<NeuronGroup*> newGroups = subnet->getNeuronGroups();

	//NeuronGroup *newDefaultGroup = subnet->getDefaultNeuronGroup();
	//NeuronGroup *oldDefaultGroup = network->getDefaultNeuronGroup();

	//update all ids
	QHash<qulonglong, qulonglong> changedIds;

	QList<NeuralNetworkElement*> elements;
	subnet->getNetworkElements(elements);
	for(QListIterator<NeuralNetworkElement*> j(elements); j.hasNext();) {
		NeuralNetworkElement *elem = j.next();
		if(elem != subnet->getDefaultNeuronGroup()) {
			qulonglong newId = NeuralNetwork::generateNextId();
			changedIds.insert(elem->getId(), newId);
			elem->setId(newId);
			newElements.append(elem);
		}
	}
	//allow group constraintes to adapt to new ids.
	subnet->notifyMemberIdsChanged(changedIds);

	//free (but do not delete) all elements in newNet to prevent it from 
	//destroying its elements when newNet is deleted.
	subnet->freeElements(false);

	//move neurons from newNet to currentNetwork.
	for(QListIterator<Neuron*> i(newNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		network->addNeuron(neuron);
	}	
	//move neuronGroups from subnet to network (including constraints)
	for(QListIterator<NeuronGroup*> i(newGroups); i.hasNext();) {
		NeuronGroup *group = i.next();
		if(group != subnet->getDefaultNeuronGroup()) {
			network->addNeuronGroup(group);
		}
	}
	network->validateSynapseConnections();

	return newElements;
}


}



