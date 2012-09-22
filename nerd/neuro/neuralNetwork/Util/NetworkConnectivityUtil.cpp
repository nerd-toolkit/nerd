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



#include "NetworkConnectivityUtil.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include <QStringList>
#include "NeuralNetworkConstants.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Math/Math.h"
#include "SynapseFunction/SimpleSynapseFunction.h"

using namespace std;

namespace nerd {
	

// SynapseSet NetworkConnectivityUtil::fullyConnectGroups(NeuronGroup *sourceGroup, NeuronGroup *targetGroup, int modus) {
// 	return NetworkConnectivityUtil::fullyConnectElements(sourceGroup->getNeurons(), targetGroup->getNeurons(), modus);
// }

SynapseSet NetworkConnectivityUtil::fullyConnectElements(NeuralNetwork *network, QList<Neuron*> sources, 
														 QList<SynapseTarget*> targets, int modus) 
{
	SynapseSet set = connectElementsUnidirectional(network, sources, targets, modus);
	
	if((modus & MODUS_BIDIRECTIONAL) != 0) {
		QList<Neuron*> biDirSources;
		for(QListIterator<SynapseTarget*> i(targets); i.hasNext();) {
			Neuron *neuron = dynamic_cast<Neuron*>(i.next());
			if(neuron != 0) {
				biDirSources.append(neuron);
			}	
		}
		QList<SynapseTarget*> biDirTargets;
		for(QListIterator<Neuron*> i(sources); i.hasNext();) {
			biDirTargets.append(i.next());
		}
		
		SynapseSet set2 = connectElementsUnidirectional(network, biDirSources, biDirTargets, modus);
		
		cerr << "Got set: " << set2.mSynapses.size() << endl;
		
		set.mSynapses << set2.mSynapses;
		set.mSources << set2.mSources;
		set.mTargets << set2.mTargets;
	}
	return set;
}

/**
 * Connects all neurons in the group. 
 */
// SynapseSet NetworkConnectivityUtil::fullyConnectGroup(NeuronGroup *group, int modus) {
// 	return NetworkConnectivityUtil::fullyConnectElements(group->getNeurons(), group->getNeurons(), modus);
// }

SynapseSet NetworkConnectivityUtil::connectElementsUnidirectional(NeuralNetwork *network, 
			 QList<Neuron*> sources, QList<SynapseTarget*> targets, int modus,
			 double defaultWeight, SynapseFunction *defaultSynapseFunction)
{
	SynapseSet set;
	
	SynapseFunction *defaultSF = new SimpleSynapseFunction();
	
	for(QListIterator<Neuron*> i(sources); i.hasNext();) {
		Neuron *source = i.next();
		
		if(source == 0) {
			continue;
		}
		
		for(QListIterator<SynapseTarget*> j(targets); j.hasNext();) {
			SynapseTarget *target = j.next();
			
			if(target == 0) {
				continue;
			}
			
			bool hasSynapse = false;
			
			QList<Synapse*> synapses = target->getSynapses();
			for(QListIterator<Synapse*> s(synapses); s.hasNext();) {
				Synapse *synapse = s.next();
				if(synapse->getSource() == source) {
					hasSynapse = true;
					break;
				}
			}
			
			SynapseFunction *sf = defaultSynapseFunction;
			if(sf == 0) {
				if(network != 0) {
					sf = network->getDefaultSynapseFunction();
				}
				if(sf == 0) {
					sf = defaultSF;
				}
			}
			
			if(!hasSynapse) {
				Synapse *newSynapse = new Synapse(0, 0, defaultWeight, *sf);
				set.mSynapses.append(newSynapse);
				set.mSources.append(source);
				set.mTargets.append(target);
				
				Vector3D pos = Math::centerOfLine(source->getPosition(), target->getPosition());
				if(source == target) {
					pos = source->getPosition();
					pos.setY(pos.getY() - 70.0);
				}
				set.mPositions.append(pos);
			}
		}
	}
	//clean up
	delete defaultSF;
	
	return set;
}



}



