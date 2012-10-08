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



#include "ScriptedNetworkManipulator.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <QMutexLocker>
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "NeuralNetworkConstants.h"
#include <Math/Math.h>
#include "NeuroModulation/NeuroModulatorElement.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Constraints/GroupConstraint.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new ScriptedNetworkManipulator.
 */
ScriptedNetworkManipulator::ScriptedNetworkManipulator()
	: mNetwork(0), mClearNetworkModificationStacks(0), mOwner(0)
{
	mClearNetworkModificationStacks = Core::getInstance()->getEventManager()->getEvent(
			NeuralNetworkConstants::EVENT_NETWORK_CLEAR_MODIFICATION_STACKS, true);
}

/**
 * Destructor.
 */
ScriptedNetworkManipulator::~ScriptedNetworkManipulator() {
}


/**
 * Sets the neural network on which all operations will be performed. 
 */
void ScriptedNetworkManipulator::setNeuralNetwork(ModularNeuralNetwork *network) {
	mNetwork = network;
}


/**
 * The owner hint speeds up manipulations of parameters that belong to a scripted NeuralNetworkElement
 * if there is any. All methods that use the id to address the target object FIRST check if the 
 * id belongs to the owner element. This heuristics avoids a time-consuming search for that 
 * network element. Since most operations are local changes at the scripting object itself, 
 * this considerably speeds up operations.
 * Therefore, whenever possible, a suitable owner hint should be given!
 */
void ScriptedNetworkManipulator::setOwnerHint(NeuralNetworkElement *owner) {
	mOwner = owner;
}

NeuralNetworkElement* ScriptedNetworkManipulator::getOwnerHint() const {
	return mOwner;
}


/**
 * Returns the id of the first neuron with the given name.
 */
qulonglong ScriptedNetworkManipulator::getNeuron(const QString &name) {
	if(mNetwork == 0) {
		return 0;
	}

	QList<Neuron*> neurons = mNetwork->getNeurons();
	for(int i = 0; i < neurons.size(); i++) {
		Neuron *neuron = neurons.at(i);
		if(neuron->getNameValue().get() == name) {
			return neuron->getId();
		}
	}
	return 0;
}

/**
 * Returns the id of the first NeuroModule with the given name.
 */
qulonglong ScriptedNetworkManipulator::getNeuroModule(const QString &name) {
	if(mNetwork == 0) {
		return 0;
	}

	QList<NeuroModule*> modules = mNetwork->getNeuroModules();
	for(int i = 0; i < modules.size(); i++) {
		NeuroModule *module = modules.at(i);
		if(module->getName() == name) {
			return module->getId();
		}
	}
	return 0;
}


/**
 * Returns the id of the first NeuronGroup with the given name.
 */
qulonglong ScriptedNetworkManipulator::getNeuronGroup(const QString &name) {
	if(mNetwork == 0) {
		return 0;
	}

	QList<NeuronGroup*> groups = mNetwork->getNeuronGroups();
	for(int i = 0; i < groups.size(); i++) {
		NeuronGroup *group = groups.at(i);
		if(group->getName() == name) {
			return group->getId();
		}
	}
	return 0;
}

/**
 * Returns a list with the ids of all neurons of a given NeuronGroup. If allEnclosed is true,
 * then all neurons of the group and all of its direct or indirect submodules are returned.
 * If false, then only the direct member neurons of the group are returned.
 * 
 * If the groupId is 0, then all neurons of the entire network are returned.
 * 
 * The allEnclosed parameter can be ommited. It defaults to false.
 * The groupId can also be ommited. It defaults to 0. 
 */
QVariantList ScriptedNetworkManipulator::getNeurons(qulonglong groupId, bool allEnclosed) {
	QVariantList neuronIds;

	if(mNetwork == 0) {
		return neuronIds;
	}

	QList<Neuron*> neurons;
	if(groupId == 0) {
		neurons = mNetwork->getNeurons();
		
	}
	else {
		NeuronGroup *group = 0;
		{
			//check owner hint to speed up things.
			NeuronGroup *owner = dynamic_cast<NeuronGroup*>(mOwner);
			if(owner != 0 && owner->getId() == groupId) {
				group = owner;
			}
			else {
				group = ModularNeuralNetwork::selectNeuronGroupById(groupId, mNetwork->getNeuronGroups());
			}
		}
		if(group == 0) {
			return neuronIds;
		}
		if(allEnclosed) {
			neurons = group->getAllEnclosedNeurons();
		}
		else {
			neurons = group->getNeurons();
		}
	}
	
	for(int i = 0; i < neurons.size(); i++) {
		Neuron *neuron = neurons.at(i);
		neuronIds.append(neuron->getId());
	}
	return neuronIds;
}

/**
 * Returns a list with the ids of all synapses of the network.
 */
QVariantList ScriptedNetworkManipulator::getSynapses() {
	QVariantList synapseIds;

	if(mNetwork == 0) {
		return synapseIds;
	}

	QList<Synapse*> synapses = mNetwork->getSynapses();
	for(int i = 0; i < synapses.size(); i++) {
		Synapse *synapse = synapses.at(i);
		synapseIds.append(synapse->getId());
	}
	return synapseIds;
}

/**
 * Returns a list with all ids of all NeuronGroups and NeuroModules.
 */
QVariantList ScriptedNetworkManipulator::getGroupsAndModules() {
	QVariantList groupIds;

	if(mNetwork == 0) {
		return groupIds;
	}

	QList<NeuronGroup*> groups = mNetwork->getNeuronGroups();
	for(int i = 0; i < groups.size(); i++) {
		NeuronGroup *group = groups.at(i);
		groupIds.append(group->getId());
	}
	return groupIds;
}

/**
 * Returns a list with all ids of all NeuronGroups not including NeuroModules.
 */
QVariantList ScriptedNetworkManipulator::getGroups() {
	QVariantList groupIds;

	if(mNetwork == 0) {
		return groupIds;
	}

	QList<NeuronGroup*> groups = mNetwork->getNeuronGroups();
	QList<NeuroModule*> modules = mNetwork->getNeuroModules();
	for(QListIterator<NeuroModule*> i(modules); i.hasNext();) {
		groups.removeAll(i.next());
	}

	for(int i = 0; i < groups.size(); i++) {
		NeuronGroup *group = groups.at(i);
		groupIds.append(group->getId());
	}
	return groupIds;
}

/**
 * Returns a list with all ids of all NeuroModules.
 */
QVariantList ScriptedNetworkManipulator::getModules() {
	QVariantList moduleIds;

	if(mNetwork == 0) {
		return moduleIds;
	}

	QList<NeuroModule*> modules = mNetwork->getNeuroModules();
	for(int i = 0; i < modules.size(); i++) {
		NeuroModule *module = modules.at(i);
		moduleIds.append(module->getId());
	}
	return moduleIds;
}

/**
 * Returns a list with the ids of all submodules that are contained in the group/modele with the given id.
 */
QVariantList ScriptedNetworkManipulator::getSubModules(qulonglong groupId) {
	QVariantList moduleIds;

	if(mNetwork == 0) {
		return moduleIds;
	}
	
	NeuronGroup *mainGroup = 0;
	{
		//check owner hint to speed up things.
		NeuronGroup *owner = dynamic_cast<NeuronGroup*>(mOwner);
		if(owner != 0 && owner->getId() == groupId) {
			mainGroup = owner;
		}
		else {
			mainGroup = ModularNeuralNetwork::selectNeuronGroupById(groupId, mNetwork->getNeuronGroups());
		}
	}
	if(mainGroup == 0) {
		return moduleIds;
	}

	QList<NeuroModule*> modules = mainGroup->getSubModules();
	for(int i = 0; i < modules.size(); i++) {
		NeuroModule *module = modules.at(i);
		moduleIds.append(module->getId());
	}
	return moduleIds;
}

/**
 * Returns the id of the parent module for a Neuron or NeuroModule.
 * Will return 0 in case of failure.
 */
qulonglong ScriptedNetworkManipulator::getParentModule(qulonglong elementId) {
	if(mNetwork == 0) {
		return 0;
	}
	Neuron *neuron = 0;
	NeuroModule *module = 0;
	{
		//check owner hint to speed up things.
		NeuroModule *ownerModule = dynamic_cast<NeuroModule*>(mOwner);
		Neuron *ownerNeuron = dynamic_cast<Neuron*>(mOwner);
		if(ownerModule != 0 && ownerModule->getId() == elementId) {
			module = ownerModule;
		}
		else if(ownerNeuron != 0 && ownerNeuron->getId() == elementId) {
			neuron = ownerNeuron;
		}
	}
	if(module == 0 && neuron == 0) {
		module = ModularNeuralNetwork::selectNeuroModuleById(elementId, mNetwork->getNeuroModules());
	}
	if(module != 0) {
		if(module->getParentModule() == 0) {
			return 0;
		}
		return module->getParentModule()->getId();
	}
	
	
	if(neuron == 0) {
		neuron = NeuralNetwork::selectNeuronById(elementId, mNetwork->getNeurons());
	}
	if(neuron != 0) {
		NeuroModule *m = mNetwork->getOwnerModule(neuron);
		if(m == 0) {
			return 0;
		}
		return m->getId();
	}
	return 0;
}

/**
 * Returns a list with the ids of all synapses going into the neuron specified by its neuronId.
 * Will return an empty list in case of failure.
 */
QVariantList ScriptedNetworkManipulator::getInSynapses(qulonglong targetId, bool excludeDisabledSynapses) {
	QVariantList synapseIds;

	if(mNetwork == 0) {
		return synapseIds;
	}
	
	SynapseTarget *target = 0;
	{
		//check owner hint to speed up things.
		SynapseTarget *owner = dynamic_cast<SynapseTarget*>(mOwner);
		if(owner != 0 && owner->getId() == targetId) {
			target = owner;
		}
		if(target == 0) {
			target = NeuralNetwork::selectNeuronById(targetId, mNetwork->getNeurons());
		}
		if(target == 0) {
			target = NeuralNetwork::selectSynapseById(targetId, mNetwork->getSynapses());
		}
	}

	if(target == 0) {
		return synapseIds;
	}

	QList<Synapse*> synapses = target->getSynapses();
	for(int i = 0; i < synapses.size(); i++) {
		Synapse *synapse = synapses.at(i);
		if(!excludeDisabledSynapses || synapse->getEnabledValue().get()) {
			synapseIds.append(synapses.at(i)->getId());
		}
	}
	return synapseIds;
}

/**
 * Returns a list with the ids of all synapses going out of the neuron specified by its neuronId.
 * Will return an empty list in case of failure.
 */
QVariantList ScriptedNetworkManipulator::getOutSynapses(qulonglong neuronId, bool excludeDisabledSynapses) {
	QVariantList synapseIds;

	if(mNetwork == 0) {
		return synapseIds;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0) {
		return synapseIds;
	}

	QList<Synapse*> synapses = neuron->getOutgoingSynapses();
	for(int i = 0; i < synapses.size(); i++) {
		Synapse *synapse = synapses.at(i);
		if(!excludeDisabledSynapses || synapse->getEnabledValue().get()) {
			synapseIds.append(synapses.at(i)->getId());
		}
	}
	return synapseIds;
}


QVariantList ScriptedNetworkManipulator::getSynapses(QVariantList targetIds, bool includeFromExternals, 
								 bool includeToExternals, bool excludeDisabledSynapses)
{
	QVariantList synapseIds;

	if(mNetwork == 0) {
		return synapseIds;
	}
	
	QList<Synapse*> allSynapses = mNetwork->getSynapses();
	
	QList<SynapseTarget*> targets;
	
	bool ok = false;
	for(QListIterator<QVariant> i(targetIds); i.hasNext();) {
		
		qulonglong id = i.next().toLongLong(&ok);
		if(!ok) {
			//TODO warning?
			continue;
		}
		SynapseTarget *target = 0;
		{
			//check owner hint to speed up things.
			SynapseTarget *owner = dynamic_cast<SynapseTarget*>(mOwner);
			if(owner != 0 && owner->getId() == id) {
				target = owner;
			}
			if(target == 0) {
				target = NeuralNetwork::selectNeuronById(id, mNetwork->getNeurons());
			}
			if(target == 0) {
				target = NeuralNetwork::selectSynapseById(id, allSynapses);
			}
		}
		
		if(target == 0) {
			continue;
		}
		
		if(!targets.contains(target)) {
			targets.append(target);
		}
	}
	
	for(QListIterator<SynapseTarget*> i(targets); i.hasNext();) {
		SynapseTarget *target = i.next();
		
		QList<Synapse*> synapses = target->getSynapses();
		for(int j = 0; j < synapses.size(); j++) {
			Synapse *synapse = synapses.at(j);
			if(synapse != 0 && (!excludeDisabledSynapses || synapse->getEnabledValue().get())) {
				if(includeFromExternals || targets.contains(synapse->getSource())) {
					if(!synapseIds.contains(synapse->getId())) {
						synapseIds.append(synapse->getId());
					}
				}
			}
		}
		Neuron *neuron = dynamic_cast<Neuron*>(target);
		if(neuron != 0) {
			QList<Synapse*> synapses = neuron->getOutgoingSynapses();
			for(int j = 0; j < synapses.size(); j++) {
				Synapse *synapse = synapses.at(j);
				if(synapse != 0 && (!excludeDisabledSynapses || synapse->getEnabledValue().get())) {
					if(includeToExternals || targets.contains(synapse->getTarget())) {
						if(!synapseIds.contains(synapse->getId())) {
							synapseIds.append(synapse->getId());
						}
					}
				}
			}
		}
	}
	return synapseIds;
}

QVariantList ScriptedNetworkManipulator::getInSynapses(QVariantList targetIds, 
				bool includeFromExternals, bool excludeDisabledSynapses)
{
	QVariantList synapseIds;

	if(mNetwork == 0) {
		return synapseIds;
	}
	
	QList<Synapse*> allSynapses = mNetwork->getSynapses();
	
	QList<SynapseTarget*> targets;
	
	bool ok = false;
	for(QListIterator<QVariant> i(targetIds); i.hasNext();) {
		
		qulonglong id = i.next().toLongLong(&ok);
		if(!ok) {
			//TODO warning?
			continue;
		}
		SynapseTarget *target = 0;
		{
			//check owner hint to speed up things.
			SynapseTarget *owner = dynamic_cast<SynapseTarget*>(mOwner);
			if(owner != 0 && owner->getId() == id) {
				target = owner;
			}
			if(target == 0) {
				target = NeuralNetwork::selectNeuronById(id, mNetwork->getNeurons());
			}
			if(target == 0) {
				target = NeuralNetwork::selectSynapseById(id, allSynapses);
			}
		}
		
		if(target != 0 && !targets.contains(target)) {
			targets.append(target);
		}
	}
	
	for(QListIterator<SynapseTarget*> i(targets); i.hasNext();) {
		SynapseTarget *target = i.next();
		
		QList<Synapse*> synapses = target->getSynapses();
		for(int j = 0; j < synapses.size(); j++) {
			Synapse *synapse = synapses.at(j);
			if(synapse != 0 && (!excludeDisabledSynapses || synapse->getEnabledValue().get())) {
				if(includeFromExternals || targets.contains(synapse->getSource())) {
					synapseIds.append(synapse->getId());
				}
			}
		}
	}
	return synapseIds;
}


QVariantList ScriptedNetworkManipulator::getOutSynapses(QVariantList neuronIds, 
				bool includeToExternals, bool excludeDisabledSynapses)
{
	QVariantList synapseIds;

	if(mNetwork == 0) {
		return synapseIds;
	}
	
	QList<Synapse*> allSynapses = mNetwork->getSynapses();
	
	QList<Neuron*> neurons;
	
	bool ok = false;
	for(QListIterator<QVariant> i(neuronIds); i.hasNext();) {
		
		qulonglong id = i.next().toLongLong(&ok);
		if(!ok) {
			//TODO warning?
			continue;
		}
		Neuron *source = 0;
		{
			//check owner hint to speed up things.
			Neuron *owner = dynamic_cast<Neuron*>(mOwner);
			if(owner != 0 && owner->getId() == id) {
				source = owner;
			}
			if(source == 0) {
				source = NeuralNetwork::selectNeuronById(id, mNetwork->getNeurons());
			}
		}
		if(source != 0 && !neurons.contains(source)) {
			neurons.append(source);
		}
	}
	
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		
		QList<Synapse*> synapses = neuron->getOutgoingSynapses();
		for(int j = 0; j < synapses.size(); j++) {
			Synapse *synapse = synapses.at(j);
			if(synapse != 0 && (!excludeDisabledSynapses || synapse->getEnabledValue().get())) {
				if(includeToExternals || neurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))) {
					synapseIds.append(synapse->getId());
				}
			}
		}
	}
	return synapseIds;
}

/**
 * Returns the id of the source neuron of the synapse specified by its synapseId.
 * Will return 0 in case of failure.
 */
qulonglong ScriptedNetworkManipulator::getSource(qulonglong synapseId) {
	if(mNetwork == 0) {
		return 0;
	}
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}
	
	if(synapse == 0) {
		return 0;
	}
	Neuron *neuron = synapse->getSource();
	if(neuron == 0) {
		return 0;
	}
	return neuron->getId();
}

/**
 * Returns the id of the synapse target (neuron or synpse) of the synapse specified by its synapseId.
 * Will return 0 in case of failure.
 */
qulonglong ScriptedNetworkManipulator::getTarget(qulonglong synapseId) {
	if(mNetwork == 0) {
		return 0;
	}
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}
	
	if(synapse == 0) {
		return 0;
	}
	SynapseTarget *target = synapse->getTarget();
	if(target == 0) {
		return 0;
	}
	return target->getId();
}


/**
 * Returns the bias of the neuron with the given neuronId.
 */
double ScriptedNetworkManipulator::getBias(qulonglong neuronId) {
	if(mNetwork == 0) {
		return 0;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}
	
	if(neuron == 0) {
		return 0;
	}
	return neuron->getBiasValue().get();
}

/**
 * Returns the weight of the synapse with the given synapseId.
 * Will return 0 in case of failure.
 */
double ScriptedNetworkManipulator::getWeight(qulonglong synapseId) {
	if(mNetwork == 0) {
		return 0;
	}
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}
	
	if(synapse == 0) {
		return 0;
	}
	return synapse->getStrengthValue().get();
}


/**
 * Executes the calculateActivation() funciton of a synapses and returns its result.
 * Will return 0 in case of a failure...
 */
double ScriptedNetworkManipulator::getSynapseOutput(qulonglong synapseId) {
	if(mNetwork == 0) {
		return 0;
	}
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}

	if(synapse == 0) {
		return 0;
	}
	return synapse->calculateActivation();
}


/**
 * Executes the synapse function without calling the calcualteActivation of the synapse.
 * The returned value usually is the (modulated) weight of the synapse.
 */
double ScriptedNetworkManipulator::execSynapseFunction(qulonglong synapseId) {
	if(mNetwork == 0) {
		return 0.0;
	}
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}

	if(synapse == 0) {
		return 0.0;
	}
	SynapseFunction *sf = synapse->getSynapseFunction();
	if(sf == 0) {
		return 0.0;
	}
	return sf->calculate(synapse);
}

/**
 * Returns the current activation of a neuron.
 * Will return 0 in case of failure.
 */
double ScriptedNetworkManipulator::getActivation(qulonglong neuronId) {
	if(mNetwork == 0) {
		return 0;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}
	
	if(neuron == 0) {
		return 0;
	}
	return neuron->getActivationValue().get();
}


/**
 * Returns the last stored activation of a neuron.
 * Will return 0 in case of failure.
 */
double ScriptedNetworkManipulator::getLastActivation(qulonglong neuronId) {
	if(mNetwork == 0) {
		return 0;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0) {
		return 0;
	}
	return neuron->getLastActivation();
}



/**
 * Returns the current output of a neuron.
 * Will return 0 in case of failure.
 */
double ScriptedNetworkManipulator::getOutput(qulonglong neuronId) {
	if(mNetwork == 0) {
		return 0;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0) {
		return 0;
	}
	return neuron->getOutputActivationValue().get();
}


/**
 * Returns the last stored output of a neuron.
 * Will return 0 in case of failure.
 */
double ScriptedNetworkManipulator::getLastOutput(qulonglong neuronId) {
	if(mNetwork == 0) {
		return 0;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0) {
		return 0;
	}
	return neuron->getLastOutputActivation();
}


/**
 * Returns a 3-dimensional list with the x,y,z coordinates of the network object with the given objectId.
 * Will return an empty list in case of failure.
 */
QVariantList ScriptedNetworkManipulator::getPosition(qulonglong objectId) {
	QVariantList position;

	if(mNetwork == 0) {
		return position;
	}
	
	NeuralNetworkElement *object = 0;
	{
		//check owner hint to speed up things.
		NeuralNetworkElement *owner = dynamic_cast<NeuralNetworkElement*>(mOwner);
		if(owner != 0 && owner->getId() == objectId) {
			object = owner;
		}
		if(object == 0) {
			QList<NeuralNetworkElement*> networkElements;
			mNetwork->getNetworkElements(networkElements);
			object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
		}
	}
	
	if(object == 0) {
		return position;
	}

	Vector3D pos = object->getPosition();
	position.append(pos.getX());
	position.append(pos.getY());
	position.append(pos.getZ());

	return position;
}

/**
 * Returns a list with the property names available of the network element with the given objectId.
 * Will return an empty list in case of failure.
 */
QVariantList ScriptedNetworkManipulator::getProperties(qulonglong objectId) {
	QVariantList properties;

	if(mNetwork == 0) {
		return properties;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	
	NeuralNetworkElement *object = 0;
	{
		//check owner hint to speed up things.
		NeuralNetworkElement *owner = dynamic_cast<NeuralNetworkElement*>(mOwner);
		if(owner != 0 && owner->getId() == objectId) {
			object = owner;
		}
		if(object == 0) {
			QList<NeuralNetworkElement*> networkElements;
			mNetwork->getNetworkElements(networkElements);
			object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
		}
	}
	
	Properties *props = dynamic_cast<Properties*>(object);

	if(props == 0) {
		return properties;
	}

	QList<QString> propNames = props->getPropertyNames();
	for(int i = 0; i < propNames.size(); ++i) {
		properties.append(propNames.at(i));
	}

	return properties;
}

/**
 * Returns the content of the specified property (name) of the network element with the given objectId. 
 * In case of failure or in case there is no such property, the string "~" is returned.
 */
QString ScriptedNetworkManipulator::getProperty(qulonglong objectId, const QString &name) {

	if(mNetwork == 0) {
		return "~";
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	
	NeuralNetworkElement *object = 0;
	{
		//check owner hint to speed up things.
		NeuralNetworkElement *owner = dynamic_cast<NeuralNetworkElement*>(mOwner);
		if(owner != 0 && owner->getId() == objectId) {
			object = owner;
		}
		if(object == 0) {
			QList<NeuralNetworkElement*> networkElements;
			mNetwork->getNetworkElements(networkElements);
			object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
		}
	}
	
	Properties *props = dynamic_cast<Properties*>(object);

	if(props == 0) {
		return "~";
	}
	if(props->hasProperty(name)) {
		return props->getProperty(name);
	}
	return "~";
}

QString ScriptedNetworkManipulator::getName(qulonglong neuronId) {
	if(mNetwork == 0) {
		return "Unknown";
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0) {
		return "Unknown";
	}
	return neuron->getNameValue().get();
}


/**
 * Sets the bias of the neuron with the given neuronId.
 * Returns false in case of failure, otherwise true.
 */
bool ScriptedNetworkManipulator::setBias(qulonglong neuronId, double bias) {
	if(mNetwork == 0) {
		return false;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0) {
		return false;
	}
	neuron->getBiasValue().set(bias);
	return true;
}

/**
 * Sets the output value of the neuron with the given neuronId. 
 * The output value will be changed as if the output was produced in the current step. 
 * Note that the visualization may not show this correctly, because the output value is set 
 * after the network calculation, so the editor may show the result of the calculation instead 
 * of the changed neuron.
 *
 * Returns false in case of failure, otherwise true.
 */
bool ScriptedNetworkManipulator::setOutput(qulonglong neuronId, double output) {
	if(mNetwork == 0) {
		return false;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}
	
	if(neuron == 0) {
		return false;
	}
	neuron->getOutputActivationValue().set(output);
	neuron->prepare();
	return true;
}

/**
 * Sets the activation of the neuron with the given neuronId. 
 * This may not have an effect on the network, because the activation of the next step is calculated
 * at the beginning of the next step and overwrites the previous activation. Changing the activation
 * only makes sense if the activation function is taking the previous activation value into account, 
 * for instance in leaky integrator models.
 * 
 * Will return false in case of failure, otherwise true.
 */
bool ScriptedNetworkManipulator::setActivation(qulonglong neuronId, double activation) {
	if(mNetwork == 0) {
		return false;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}
	
	if(neuron == 0) {
		return false;
	}
	neuron->getActivationValue().set(activation);
	neuron->prepare();
	return true;
}


/**
 * Sets the weight of the synapse specified with the synapseId.
 * Will return false in case of failure.
 */
bool ScriptedNetworkManipulator::setWeight(qulonglong synapseId, double weight) {
	if(mNetwork == 0) {
		return false;
	}
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}
	
	if(synapse == 0) {
		return false;
	}
	synapse->getStrengthValue().set(weight);
	return true;
}

/**
 * Sets the new position of the network element specified by the objectId. 
 * Will return false in case of failure, otherwise true.
 */
bool ScriptedNetworkManipulator::setPosition(qulonglong objectId, double x, double y, double z) {
	QVariantList position;

	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());

	NeuralNetworkElement *object = 0;
	{
		//check owner hint to speed up things.
		NeuralNetworkElement *owner = dynamic_cast<NeuralNetworkElement*>(mOwner);
		if(owner != 0 && owner->getId() == objectId) {
			object = owner;
		}
		if(object == 0) {
			QList<NeuralNetworkElement*> networkElements;
			mNetwork->getNetworkElements(networkElements);
			object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
		}
	}
	
	if(object == 0) {
		return false;
	}
	object->setPosition(Vector3D(x, y, z));
	
	nmm->triggerNetworkParametersChangedEvent();
	return true;
}

/**
 * Sets the property "propName" of the network element "objectId" to the new "content". 
 * The content may also be ommited, then it is replaced by "".
 * Will return false in case of failure, otherwise true.
 */
bool ScriptedNetworkManipulator::setProperty(qulonglong objectId, const QString &propName, const QString &content, bool severeChange) {
	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());

	NeuralNetworkElement *object = 0;
	{
		//check owner hint to speed up things.
		NeuralNetworkElement *owner = dynamic_cast<NeuralNetworkElement*>(mOwner);
		if(owner != 0 && owner->getId() == objectId) {
			object = owner;
		}
		if(object == 0) {
			QList<NeuralNetworkElement*> networkElements;
			mNetwork->getNetworkElements(networkElements);
			object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
		}
	}
	
	Properties *props = dynamic_cast<Properties*>(object);

	if(props == 0) {
		return false;
	}
	props->setProperty(propName, content);
	
	if(severeChange) {
		nmm->triggerNetworkStructureChangedEvent();
	}
	else {
		nmm->triggerNetworkParametersChangedEvent();
	}
	return true;
}


bool ScriptedNetworkManipulator::setName(qulonglong neuronId, const QString &name) {
	if(mNetwork == 0) {
		return false;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0) {
		return false;
	}
	neuron->getNameValue().set(name);
	return true;
}


/**
 * Adds a network element (Neuron or NeuroModule) to the group specified by the groupId.
 * Will return false in case of failure, otherwise true.
 */
bool ScriptedNetworkManipulator::addToGroup(qulonglong elementId, qulonglong groupId) {
	if(mNetwork == 0) {
		return 0;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());

	//get target group to which the element should be added
	NeuronGroup *group = 0;
	{
		//check owner hint to speed up things.
		NeuronGroup *owner = dynamic_cast<NeuronGroup*>(mOwner);
		if(owner != 0 && owner->getId() == groupId) {
			group = owner;
		}
		if(group == 0) {
			group = ModularNeuralNetwork::selectNeuronGroupById(groupId, mNetwork->getNeuronGroups());
		}
	}

	if(group == 0) {
		return false;
	}
	
	//check if the element to be added is a module 
	NeuroModule *module = 0;
	{
		//check owner hint to speed up things.
		NeuroModule *owner = dynamic_cast<NeuroModule*>(mOwner);
		if(owner != 0 && owner->getId() == groupId) {
			module = owner;
		}
		if(module == 0) {
			module = ModularNeuralNetwork::selectNeuroModuleById(elementId, mNetwork->getNeuroModules());
		}
	}
	
	if(module != 0) {
		bool ok = group->addSubModule(module);
		if(ok) {
			nmm->triggerNetworkStructureChangedEvent();
		}
		return ok;
	}
	
	//check if the element to be added is a neuron 
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == groupId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(elementId, mNetwork->getNeurons());
		}
	}

	if(neuron != 0) {
		bool ok = group->addNeuron(neuron);
		if(ok) {
			nmm->triggerNetworkStructureChangedEvent();
		}
		return ok;
	}
	return false;
}

/**
 * Removes a network element (Neuron or NeuroModule) from a NeuronGroup / NeuroModule specified by groupId.
 * Will return false in case of failure, otherwise true.
 */
bool ScriptedNetworkManipulator::removeFromGroup(qulonglong elementId, qulonglong groupId) {
	if(mNetwork == 0) {
		return 0;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());

	NeuronGroup *group = 0;
	{
		//check owner hint to speed up things.
		NeuronGroup *owner = dynamic_cast<NeuronGroup*>(mOwner);
		if(owner != 0 && owner->getId() == groupId) {
			group = owner;
		}
		if(group == 0) {
			group = ModularNeuralNetwork::selectNeuronGroupById(groupId, mNetwork->getNeuronGroups());
		}
	}
	
	if(group == 0) {
		return false;
	}
	
	//check if the element to be added is a module 
	NeuroModule *module = 0;
	{
		//check owner hint to speed up things.
		NeuroModule *owner = dynamic_cast<NeuroModule*>(mOwner);
		if(owner != 0 && owner->getId() == groupId) {
			module = owner;
		}
		if(module == 0) {
			module = ModularNeuralNetwork::selectNeuroModuleById(elementId, mNetwork->getNeuroModules());
		}
	}

	if(module != 0) {
		bool ok = group->removeSubModule(module);
		if(ok) {
			nmm->triggerNetworkStructureChangedEvent();
		}
		return ok;
	}
	
	//check if the element to be added is a neuron 
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == groupId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(elementId, mNetwork->getNeurons());
		}
	}
	
	if(neuron != 0) {
		bool ok = group->removeNeuron(neuron);
		if(ok) {
			nmm->triggerNetworkStructureChangedEvent();
		}
		return ok;
	}
	return false;
}

/**
 * Creates a new neuron and adds it to the current network. The optional name can be used to set the name
 * of the neuron.
 * The id of the neuron is returned if successful, otherwise 0 is returned.
 */
qulonglong ScriptedNetworkManipulator::createNeuron(const QString &name) {
	if(mNetwork == 0) {
		return 0;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	
	Neuron *neuron = new Neuron(name, *(mNetwork->getDefaultTransferFunction()), 
								*(mNetwork->getDefaultActivationFunction()));
	mNetwork->addNeuron(neuron);

	nmm->triggerNetworkStructureChangedEvent();
	return neuron->getId();
}

/**
 * Creates a new synapse, going from neuron sourceId to target targetId. 
 * Returns the id of the new synapse in case of success, otherwise returns 0.
 */
qulonglong ScriptedNetworkManipulator::createSynapse(qulonglong sourceId, qulonglong targetId, double weight) {
	if(mNetwork == 0) {
		return 0;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());

	QList<Neuron*> neurons = mNetwork->getNeurons();
	QList<Synapse*> synapses = mNetwork->getSynapses();

	Neuron *source = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == sourceId) {
			source = owner;
		}
		if(source == 0) {
			source = NeuralNetwork::selectNeuronById(sourceId, neurons);
		}
	}
	
	SynapseTarget *target = 0;
	{
		//check owner hint to speed up things.
		SynapseTarget *owner = dynamic_cast<SynapseTarget*>(mOwner);
		if(owner != 0 && owner->getId() == targetId) {
			target = owner;
		}
		if(target == 0) {
			target = NeuralNetwork::selectNeuronById(targetId, neurons);
		}
		if(target == 0) {
			target = NeuralNetwork::selectSynapseById(targetId, synapses);
		}
	}

	if(source == 0 || target == 0) {
		return 0;
	}

	//check if there is already a synaspe between the two elements.
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();
		if(synapse->getSource() == source && synapse->getTarget() == target) {
			return 0;
		}
	}
	Synapse *s = Synapse::createSynapse(source, target, weight, *(mNetwork->getDefaultSynapseFunction()));

	nmm->triggerNetworkStructureChangedEvent();
	return s->getId();

}


/**
 * Creates a new NeuroModule with the given name and returns its id.
 * Returns 0 in case of failure.
 */
qulonglong ScriptedNetworkManipulator::createNeuroModule(const QString &name) {
	if(mNetwork == 0) {
		return 0;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	
	NeuroModule *module = new NeuroModule(name);
	mNetwork->addNeuronGroup(module);

	nmm->triggerNetworkStructureChangedEvent();
	return module->getId();
}

/**
 * Creates a new NeuronGroup with the given name and returns its id.
 * Returns 0 in case of failure.
 */
qulonglong ScriptedNetworkManipulator::createNeuronGroup(const QString &name) {
	if(mNetwork == 0) {
		return 0;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	
	NeuronGroup *group = new NeuronGroup(name);
	mNetwork->addNeuronGroup(group);

	nmm->triggerNetworkStructureChangedEvent();
	return group->getId();
}


bool ScriptedNetworkManipulator::removeNeuron(qulonglong neuronId) {
	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0) {
		return false;
	}
	bool ok = mNetwork->removeNeuron(neuron);

	mClearNetworkModificationStacks->trigger();
	nmm->triggerNetworkStructureChangedEvent();

	if(ok) {
		//delete neuron;
	}
	return ok;
}


bool ScriptedNetworkManipulator::removeSynapse(qulonglong synapseId) {
	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}

	if(synapse == 0 || synapse->getTarget() == 0) { 
		return false;
	}
	
	bool ok = synapse->getTarget()->removeSynapse(synapse);

	mClearNetworkModificationStacks->trigger();
	nmm->triggerNetworkStructureChangedEvent();

	if(ok) {
		//delete synapse;
	}
	return ok;
}



bool ScriptedNetworkManipulator::removeNeuronGroup(qulonglong groupId) {
	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	
	NeuronGroup *group = 0;
	{
		//check owner hint to speed up things.
		NeuronGroup *owner = dynamic_cast<NeuronGroup*>(mOwner);
		if(owner != 0 && owner->getId() == groupId) {
			group = owner;
		}
		if(group == 0) {
			group = ModularNeuralNetwork::selectNeuronGroupById(groupId, mNetwork->getNeuronGroups());
		}
	}

	if(group == 0) { 
		return false;
	}
	bool ok = mNetwork->removeNeuronGroup(group);

	mClearNetworkModificationStacks->trigger();
	nmm->triggerNetworkStructureChangedEvent();

	if(ok) {
		//delete group;
	}
	return ok;
}


bool ScriptedNetworkManipulator::removeProperty(qulonglong objectId, const QString &propertyName, bool severeChange) {
	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	
	NeuralNetworkElement *object = 0;
	{
		//check owner hint to speed up things.
		NeuralNetworkElement *owner = dynamic_cast<NeuralNetworkElement*>(mOwner);
		if(owner != 0 && owner->getId() == objectId) {
			object = owner;
		}
		if(object == 0) {
			QList<NeuralNetworkElement*> networkElements;
			mNetwork->getNetworkElements(networkElements);
			object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
		}
	}
	
	Properties *props = dynamic_cast<Properties*>(object);
	
	if(props == 0) {
		return false;
	}
	if(!props->hasProperty(propertyName)) {
		return true;
	}
	props->removeProperty(propertyName);
	
	if(severeChange) {
		nmm->triggerNetworkStructureChangedEvent();
	}
	else {
		nmm->triggerNetworkParametersChangedEvent();
	}
	return true;
}


bool ScriptedNetworkManipulator::setTransferFunction(qulonglong neuronId, const QString &transferFunctionName) {
	if(mNetwork == 0) {
		return false;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0) {
		return false;
	}
	QList<TransferFunction*> transferFunctions = 
				Neuro::getNeuralNetworkManager()->getTransferFunctionPrototypes();
	for(QListIterator<TransferFunction*> i(transferFunctions); i.hasNext();) {
		TransferFunction *tf = i.next();
		if(tf->getName() == transferFunctionName) {
			neuron->setTransferFunction(*tf);
			return true;
		}
	}
	return false;
}


QString ScriptedNetworkManipulator::getTransferFunctionName(qulonglong neuronId) {
	if(mNetwork == 0) {
		return "";
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0 || neuron->getTransferFunction() == 0) {
		return "";
	}
	return neuron->getTransferFunction()->getName();
}


bool ScriptedNetworkManipulator::setActivationFunction(qulonglong neuronId, const QString &activationFunctionName) {
	if(mNetwork == 0) {
		return false;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0) {
		return false;
	}
	QList<ActivationFunction*> activationFunctions = 
				Neuro::getNeuralNetworkManager()->getActivationFunctionPrototypes();
	for(QListIterator<ActivationFunction*> i(activationFunctions); i.hasNext();) {
		ActivationFunction *af = i.next();
		if(af->getName() == activationFunctionName) {
			neuron->setActivationFunction(*af);
			return true;
		}
	}
	return false;
}


QString ScriptedNetworkManipulator::getActivationFunctionName(qulonglong neuronId) {
	if(mNetwork == 0) {
		return "";
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0 || neuron->getActivationFunction() == 0) {
		return "";
	}
	return neuron->getActivationFunction()->getName();
}


bool ScriptedNetworkManipulator::setSynapseFunction(qulonglong synapseId, const QString &synapseFunctionName) {
	if(mNetwork == 0) {
		return false;
	}
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}
	
	if(synapse == 0) {
		return false;
	}
	//check if there is a prototype with the matching name
	QList<SynapseFunction*> synapseFunctions = 
				Neuro::getNeuralNetworkManager()->getSynapseFunctionPrototypes();
	for(QListIterator<SynapseFunction*> i(synapseFunctions); i.hasNext();) {
		SynapseFunction *sf = i.next();
		if(sf->getName() == synapseFunctionName) {
			synapse->setSynapseFunction(*sf);
			return true;
		}
	}
	return false;
}


QString ScriptedNetworkManipulator::getSynapseFunctionName(qulonglong synapseId) {
	if(mNetwork == 0) {
		return "";
	}
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}
	
	if(synapse == 0 || synapse->getSynapseFunction() == 0) {
		return "";
	}
	return synapse->getSynapseFunction()->getName();
}


bool ScriptedNetworkManipulator::setTransferFunctionParameter(
				qulonglong neuronId, const QString &parameterName, const QString &value)
{
	if(mNetwork == 0) {
		return "";
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}
	
	if(neuron == 0 || neuron->getTransferFunction() == 0) {
		return "";
	}
	Value *param = neuron->getTransferFunction()->getParameter(parameterName);
	if(param == 0) {
		return "";
	}
	return param->setValueFromString(value);
}


QString ScriptedNetworkManipulator::getTransferFunctionParameter(
				qulonglong neuronId, const QString &parameterName)
{
	if(mNetwork == 0) {
		return "";
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}
	
	if(neuron == 0 || neuron->getTransferFunction() == 0) {
		return "";
	}
	Value *param = neuron->getTransferFunction()->getParameter(parameterName);
	if(param == 0) {
		return "";
	}
	return param->getValueAsString();
}


bool ScriptedNetworkManipulator::setActivationFunctionParameter(
				qulonglong neuronId, const QString &parameterName, const QString &value)
{
	if(mNetwork == 0) {
		return false;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}
	
	if(neuron == 0 || neuron->getActivationFunction() == 0) {
		return false;
	}
	Value *param = neuron->getActivationFunction()->getParameter(parameterName);
	if(param == 0) {
		return false;
	}
	return param->setValueFromString(value);
}


QString ScriptedNetworkManipulator::getActivationFunctionParameter(
				qulonglong neuronId, const QString &parameterName)
{
	if(mNetwork == 0) {
		return "";
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}

	if(neuron == 0 || neuron->getActivationFunction() == 0) {
		return "";
	}
	Value *param = neuron->getActivationFunction()->getParameter(parameterName);
	if(param == 0) {
		return "";
	}
	return param->getValueAsString();
}


bool ScriptedNetworkManipulator::setSynapseFunctionParameter(
				qulonglong synapseId, const QString &parameterName, const QString &value)
{
	if(mNetwork == 0) {
		return "";
	}
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}

	if(synapse == 0 || synapse->getSynapseFunction() == 0) {
		return "";
	}
	Value *param = synapse->getSynapseFunction()->getParameter(parameterName);
	if(param == 0) {
		return false;
	}
	return param->setValueFromString(value);
}


QString ScriptedNetworkManipulator::getSynapseFunctionParameter(
				qulonglong synapseId, const QString &parameterName)
{
	if(mNetwork == 0) {
		return "";
	}
	
	Synapse *synapse = 0;
	{
		//check owner hint to speed up things.
		Synapse *owner = dynamic_cast<Synapse*>(mOwner);
		if(owner != 0 && owner->getId() == synapseId) {
			synapse = owner;
		}
		if(synapse == 0) {
			synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
		}
	}

	if(synapse == 0 || synapse->getSynapseFunction() == 0) {
		return "";
	}
	Value *param = synapse->getSynapseFunction()->getParameter(parameterName);
	if(param == 0) {
		return "";
	}
	return param->getValueAsString();
}

double ScriptedNetworkManipulator::applyTransferFunction(qulonglong neuronId, double activity) {
	if(mNetwork == 0) {
		return 0.0;
	}
	
	Neuron *neuron = 0;
	{
		//check owner hint to speed up things.
		Neuron *owner = dynamic_cast<Neuron*>(mOwner);
		if(owner != 0 && owner->getId() == neuronId) {
			neuron = owner;
		}
		if(neuron == 0) {
			neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
		}
	}
	
	if(neuron == 0 || neuron->getTransferFunction() == 0) {
		return 0.0;
	}
	return neuron->getTransferFunction()->transferActivation(activity, neuron);
}

double ScriptedNetworkManipulator::tf(qulonglong neuronId, double activity) {
	return applyTransferFunction(neuronId, activity);
}

double ScriptedNetworkManipulator::getDistance(QVariantList pos1, QVariantList pos2) {
	if(pos1.length() == 2 && pos2.length() == 2) {
		QPointF point1(pos1.at(0).toDouble(), pos1.at(1).toDouble());
		QPointF point2(pos2.at(0).toDouble(), pos2.at(1).toDouble());
		
		return Math::distance(point1, point2);
	}
	else if(pos1.length() == 3 && pos2.length() == 3) {
		Vector3D point1(pos1.at(0).toDouble(), pos1.at(1).toDouble(), pos1.at(2).toDouble());
		Vector3D point2(pos2.at(0).toDouble(), pos2.at(1).toDouble(), pos2.at(2).toDouble());
		
		return Math::distance(point1, point2);
	}
	return 0.0;
}



/**
 * Enables the neuromodulators for a neuromodulator element.
 * If the element is not a neuromodulator element, then nothing will happen.
 */
void ScriptedNetworkManipulator::enableNeuroModulators(qulonglong elementId, bool enable) {
	if(mNetwork == 0) {
		return;
	}

	NeuralNetworkElement *object = 0;
	NeuroModulatorElement *nModElem = 0;
	getNeuroModulator(elementId, &object, &nModElem);
	
	if(object == 0 || nModElem == 0) {
		return;
	}
	
	if(enable) {
		if(nModElem->getNeuroModulator() == 0) {
			nModElem->setNeuroModulator();
		}
		//don't change the EXISTING modulator object, if enable == true and no modulator given
	}
	else {
		//diable modulators.
		nModElem->setNeuroModulator(0);
	}
}



/**
 * Sets the current concentration level of a specific neuromodulator type spread by the given element.
 * If the element is not an active neuromodulator element, then there is no effect.
 */
void ScriptedNetworkManipulator::setModulatorArea(qulonglong elementId, int type, double width, 
												  double height, bool circle) 
{
	if(mNetwork == 0) {
		return;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		modulator->setLocalAreaRect(type, width, height, Vector3D(0.0, 0.0, 0.0), circle);
	}
}

/**
 * Sets a reference module use by the NeuroModulators to determin the area of influence.
 * With a reference module, that area is the area of the module (including submodules)
 * 
 */
void ScriptedNetworkManipulator::setModulatorAreaReferenceModule(qulonglong elementId, int type, 
																 qulonglong moduleId) 
{
	if(mNetwork == 0) {
		return;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		
		NeuroModule *module = 0;
		{
			//check owner hint to speed up things.
			NeuroModule *owner = dynamic_cast<NeuroModule*>(mOwner);
			if(owner != 0 && owner->getId() == moduleId) {
				module = owner;
			}
			if(module == 0) {
				module = ModularNeuralNetwork::selectNeuroModuleById(moduleId, mNetwork->getNeuroModules());
			}
		}
		modulator->setAreaReferenceModule(type, module);
	}
}

/**
 * Returns the rectangle of the area of a specific neuromodulator type spread by the given element.
 * The first two numbers in the returned list are the width and height of the area.
 * The third number specifies, if this is a circle (1) or not (0).
 * The last two numbers give an (optional local offset).
 * 
 * If the element is not an active neuromodulator element, then 0.0 is returned.
 */
QVariantList ScriptedNetworkManipulator::getModulatorArea(qulonglong elementId, int type) {
	QVariantList area;
	
	if(mNetwork == 0) {
		return area;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		QRectF rect = modulator->getLocalRect(type);
		bool isCircle = modulator->isCircularArea(type);
		
		area.append(rect.width());
		area.append(rect.height());
		area.append(isCircle ? 1.0 : 0.0);
		area.append(rect.x());
		area.append(rect.y());
	}
	return area;
}




/**
 * Sets the current concentration level of a specific neuromodulator type spread by the given element.
 * If the element is not an active neuromodulator element, then there is no effect.
 */
void ScriptedNetworkManipulator::setModulatorConcentration(qulonglong elementId, int type, double concentration) {
	if(mNetwork == 0) {
		return;
	}
	
	NeuralNetworkElement *object = 0;
	NeuroModulator *modulator = getNeuroModulator(elementId, &object);
	if(modulator != 0) {
		modulator->setConcentration(type, concentration, object);
	}
}

/**
 * Returns the current concentration level of a specific neuromodulator type spread by the given element.
 * If the element is not an active neuromodulator element, then 0.0 is returned.
 */
double ScriptedNetworkManipulator::getModulatorConcentration(qulonglong elementId, int type) {
	if(mNetwork == 0) {
		return 0.0;
	}
	
	NeuralNetworkElement *object = 0;
	NeuroModulator *modulator = getNeuroModulator(elementId, &object);
	if(modulator != 0) {
		return modulator->getConcentration(type, object);
	}
	return 0.0;
}

/**
 * Returns the current concentration level of a specific neuromodulator type 
 * at a certain global positinon in a network that is spread by the given element.
 * This will only consider the concentration level caused by the specified element.
 * 
 * If the element is not an active neuromodulator element, then 0.0 is returned.
 */
double ScriptedNetworkManipulator::getModulatorConcentrationAt(qulonglong elementId, 
											int type, double x, double y, double z) 
{
	if(mNetwork == 0) {
		return 0.0;
	}
	
	NeuralNetworkElement *object = 0;
	NeuroModulator *modulator = getNeuroModulator(elementId, &object);
	if(modulator != 0) {
		return modulator->getConcentrationAt(type, Vector3D(x, y, z), object);
	}
	return 0.0;
}

/**
 * Returns the global concentration level of a specific neuromodulator type 
 * at a certain global positinon in the entire network. 
 * This includes all concentration levels caused by all elements in the network 
 * of that type!
 */
double ScriptedNetworkManipulator::getModulatorConcentrationAt(int type, double x, double y, double z) {
	if(mNetwork == 0) {
		return 0.0;
	}
	
	return NeuroModulator::getConcentrationInNetworkAt(type, Vector3D(x, y, z), mNetwork);
}

/**
 * Sets the concentration calculation modus for a specific type of the given element.
 * If the element is not an active neuromodulator element, then there is no effect.
 */
void ScriptedNetworkManipulator::setModulatorDistributionModus(qulonglong elementId, int type, int modus) {
	if(mNetwork == 0) {
		return;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		modulator->setDistributionModus(type, modus);
	}
}

/**
 * Returns the currently used modulator distribution modus of the given modulator type.
 */
int ScriptedNetworkManipulator::getModulatorDistributionModus(qulonglong elementId, int type) {
	if(mNetwork == 0) {
		return -1;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		return modulator->getDistributionModus(type);
	}
	return -1;
}

/**
 * Sets the update modus for a specific modulator type of the given element.
 * If the element is not an active neuromodulator element, then there is no effect.
 */
void ScriptedNetworkManipulator::setModulatorUpdateModus(qulonglong elementId, int type, int modus) {
	if(mNetwork == 0) {
		return;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		modulator->setUpdateModus(type, modus);
	}
}

/**
 * Returns the currently used modulator update modus of the given modulator type.
 */
int ScriptedNetworkManipulator::getModulatorUpdateModus(qulonglong elementId, int type) {
	if(mNetwork == 0) {
		return -1;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		return modulator->getUpdateModus(type);
	}
	return -1;
}

bool ScriptedNetworkManipulator::setModulatorUpdateParameters(qulonglong elementId, int modus, QVariantList params) {
	if(mNetwork == 0) {
		return false;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		
		QList<double> parameters;
		for(int i = 0; i < params.size(); ++i) {
			parameters.append(params.at(i).toDouble());
		}
		
		return modulator->setUpdateModusParameters(modus, parameters);
	}
	return false;
}


QVariantList ScriptedNetworkManipulator::getModulatorUpdateParameters(qulonglong elementId, int type) {
	
	QVariantList params;
	
	if(mNetwork == 0) {
		return params;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		QList<double> parameters = modulator->getUpdateModusParameters(type);
		for(QListIterator<double> i(parameters); i.hasNext();) {
			params.append(i.next());
		}
	}
	
	return params;
}

QVariantList ScriptedNetworkManipulator::getModulatorUpdateParameterNames(qulonglong elementId, int type) {
	
	QVariantList names;
	
	if(mNetwork == 0) {
		return names;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		QList<QString> parameters = modulator->getUpdateModusParameterNames(type);
		for(int i = 0; i < parameters.size(); ++i) {
			names.append(QString::number(i) + ": " + parameters.at(i));
		}
	}
	
	return names;
}

QVariantList ScriptedNetworkManipulator::getModulatorUpdateVariables(qulonglong elementId, int type) {
	QVariantList variables;
	
	if(mNetwork == 0) {
		return variables;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		QList<double> parameters = modulator->getUpdateModusVariables(type);
		for(QListIterator<double> i(parameters); i.hasNext();) {
			variables.append(i.next());
		}
	}
	
	return variables;
}


QVariantList ScriptedNetworkManipulator::getModulatorUpdateVariableNames(qulonglong elementId, int type) {
	QVariantList names;
	
	if(mNetwork == 0) {
		return names;
	}
	
	NeuroModulator *modulator = getNeuroModulator(elementId);
	if(modulator != 0) {
		QList<QString> parameters = modulator->getUpdateModusVariableNames(type);
		for(int i = 0; i < parameters.size(); ++i) {
			names.append(QString::number(i) + ": " + parameters.at(i));
		}
	}
	
	return names;
}



NeuroModulator* ScriptedNetworkManipulator::getNeuroModulator(qulonglong elementId, 
						NeuralNetworkElement **element, NeuroModulatorElement **modElem) 
{ 
	if(mNetwork == 0) {
		return 0;
	}
	
	NeuralNetworkElement *object = 0;
	{
		//check owner hint to speed up things.
		NeuralNetworkElement *owner = dynamic_cast<NeuralNetworkElement*>(mOwner);
		if(owner != 0 && owner->getId() == elementId) {
			object = owner;
		}
		if(object == 0) {
			QList<NeuralNetworkElement*> networkElements;
			mNetwork->getNetworkElements(networkElements);
			object = NeuralNetwork::selectNetworkElementById(elementId, networkElements);
		}
	}
	if(object == 0) {
		return 0;
	}
	NeuroModulatorElement *nModElem = 0;
	
	Neuron *neuron = dynamic_cast<Neuron*>(object);
	Synapse *synapse = dynamic_cast<Synapse*>(object);
	NeuroModule *module = dynamic_cast<NeuroModule*>(object);
	if(neuron != 0) {
		nModElem = dynamic_cast<NeuroModulatorElement*>(dynamic_cast<Neuron*>(object)->getActivationFunction());
	}
	else if(synapse != 0) {
		nModElem = dynamic_cast<NeuroModulatorElement*>(dynamic_cast<Synapse*>(object)->getSynapseFunction());
	}
	else if(module != 0) {
		//use the first NeuroModulator constaint (ignore all others)
		QList<GroupConstraint*> constraints = dynamic_cast<NeuroModule*>(object)->getConstraints();
		for(QListIterator<GroupConstraint*> j(constraints); j.hasNext();) {
			GroupConstraint *group = j.next();
			nModElem = dynamic_cast<NeuroModulatorElement*>(group);
			
			if(nModElem != 0) {
				break;
			}
		}
	}
	
	if(nModElem == 0) {
		return 0;
	}
	
	if(element != 0) {
		(*element) = object;
	}
	if(modElem != 0) {
		(*modElem) = nModElem;
	}
	return nModElem->getNeuroModulator();
}


}



