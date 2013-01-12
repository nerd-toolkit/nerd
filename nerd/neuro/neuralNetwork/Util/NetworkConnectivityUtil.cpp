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
#include "Util/Util.h"
#include <limits>


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
	
	ModularNeuralNetwork *mnn = dynamic_cast<ModularNeuralNetwork*>(network);
	
	bool ignoreInterfaces = (modus & NetworkConnectivityUtil::MODUS_IGNORE_INTERFACES) != 0;
	bool ignoreProperties = (modus & NetworkConnectivityUtil::MODUS_IGNORE_PROPERTIES) != 0;
	
	for(QListIterator<Neuron*> i(sources); i.hasNext();) {
		Neuron *source = i.next();
		
		if(source == 0) {
			continue;
		}
		
		if(!ignoreProperties) {
			if(source->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
				|| source->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_SOURCE)) 
			{
				continue;
			}
			QString maxOutputSyn = source->getProperty(
						NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_OUT_SYNAPSES);
			bool ok = true;
			int numberOfSyn = maxOutputSyn.toInt(&ok);
			if(ok && (source->getOutgoingSynapses().size() >= numberOfSyn)) {
				continue;
			}
		}

		QList<Neuron*> validTargets = mnn->getNeurons();
		
		if(!ignoreInterfaces) {
			validTargets = NetworkConnectivityUtil::getValidTargetNeurons(source, mnn);
		}
		
		for(QListIterator<SynapseTarget*> j(targets); j.hasNext();) {
			SynapseTarget *target = j.next();
			
			if(target == 0) {
				continue;
			}
			
			if(!ignoreProperties) {
				
				if(target->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
					|| target->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_TARGET)) 
				{
					continue;
				}
				QString maxInputSyn = source->getProperty(
							NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_IN_SYNAPSES);
				bool ok = true;
				int numberOfSyn = maxInputSyn.toInt(&ok);
				if(ok && (target->getSynapses().size() >= numberOfSyn)) {
					continue;
				}
			}
			
			if(!ignoreInterfaces) {
				
				Neuron *targetNeuron = dynamic_cast<Neuron*>(target);
				
				//check for interface neurons.
				if(targetNeuron != 0 && !validTargets.contains(targetNeuron)) {
					continue;
				}
				if(targetNeuron != 0) {
					QList<Neuron*> validSources = NetworkConnectivityUtil::getValidSourceNeurons(targetNeuron, mnn);
					if(!validSources.contains(source)) {
						continue;
					}
				}
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


QList<Neuron*> NetworkConnectivityUtil::getValidSourceNeurons(Neuron *target, ModularNeuralNetwork *net)
{
	QList<Neuron*> allOutputModuleNeurons;

	if(target == 0 || net == 0) {
		return allOutputModuleNeurons;
	}

	int interfaceLevel = getInterfaceLevel(target, NeuralNetworkConstants::TAG_MODULE_INPUT);
	bool isInterface = interfaceLevel > 0 ? true : false;

	//crawl down into parent modules as deep as the interface level allows.
	NeuroModule *parentModule = net->getOwnerModule(target);
	//NeuroModule *previousModule = parentModule;
	int k = 0;
	for(; k <= interfaceLevel; ++k) {

		if(parentModule != 0) {
			Util::addWithoutDuplicates(allOutputModuleNeurons, parentModule->getNeurons());

			for(QListIterator<NeuroModule*> j(parentModule->getSubModules()); j.hasNext();) {
				Util::addWithoutDuplicates(allOutputModuleNeurons, j.next()->getOutputNeurons());
			}
			if(isInterface) {
				//previousModule = parentModule;
				parentModule = parentModule->getParentModule();
			}
			else {
				break;
			}
		}
		else {
			Util::addWithoutDuplicates(allOutputModuleNeurons, net->getRootNeurons());
			QList<NeuroModule*> rootModules = net->getRootModules();
			for(QListIterator<NeuroModule*> i(rootModules); i.hasNext();) {
				Util::addWithoutDuplicates(allOutputModuleNeurons, i.next()->getOutputNeurons());
			}
			break;
		}
	}

	return allOutputModuleNeurons;
}


QList<Neuron*> NetworkConnectivityUtil::getValidTargetNeurons(Neuron *source, ModularNeuralNetwork *net)
{
	QList<Neuron*> allInputModuleNeurons;

	if(source == 0 || net == 0) {
		return allInputModuleNeurons;
	}

	int interfaceLevel = getInterfaceLevel(source, NeuralNetworkConstants::TAG_MODULE_OUTPUT);
	bool isInterface = interfaceLevel > 0 ? true : false;

	//crawl down into parent modules as deep as the interface level allows.
	NeuroModule *parentModule = net->getOwnerModule(source);
	//NeuroModule *previousModule = parentModule;
	int k = 0;
	for(; k <= interfaceLevel; ++k) {

		if(parentModule != 0) {
			Util::addWithoutDuplicates(allInputModuleNeurons, parentModule->getNeurons());

			for(QListIterator<NeuroModule*> j(parentModule->getSubModules()); j.hasNext();) {
				Util::addWithoutDuplicates(allInputModuleNeurons, j.next()->getInputNeurons());
			}
			if(isInterface) {
				//previousModule = parentModule;
				parentModule = parentModule->getParentModule();
			}
			else {
				break;
			}
		}
		else {
			Util::addWithoutDuplicates(allInputModuleNeurons, net->getRootNeurons());
			QList<NeuroModule*> rootModules = net->getRootModules();
			for(QListIterator<NeuroModule*> i(rootModules); i.hasNext();) {
				Util::addWithoutDuplicates(allInputModuleNeurons, i.next()->getInputNeurons());
			}
			break;
		}
	}

	return allInputModuleNeurons;
}

int NetworkConnectivityUtil::getInterfaceLevel(Neuron *neuron, const QString &moduleInterfaceType) {
	
	if(moduleInterfaceType == "") {
		return 0;
	}
	bool neuronIsInterfaceNeuron = neuron->hasProperty(moduleInterfaceType);
	bool neuronIsExtendedInterfaceNeuron = 
			neuron->hasProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	QString levelString = neuron->getProperty(moduleInterfaceType);
	if(neuronIsExtendedInterfaceNeuron) {
		levelString = neuron->getProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	}

	int interfaceLevel = 0;
	if(neuronIsInterfaceNeuron) {
		interfaceLevel = 1;

		if(neuronIsExtendedInterfaceNeuron && levelString.trimmed() == "") {
			interfaceLevel = numeric_limits<int>::max();
		}
		else {
			bool ok = true;
			int level = levelString.toInt(&ok);
			if(ok) {
				interfaceLevel =  level + 1; //TODO adapt neuroModule::getInput/Outputs
			}
		}
	}
	return interfaceLevel;
}

}



