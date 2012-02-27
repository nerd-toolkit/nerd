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

using namespace std;

namespace nerd {


/**
 * Constructs a new ScriptedNetworkManipulator.
 */
ScriptedNetworkManipulator::ScriptedNetworkManipulator()
	: mNetwork(0), mClearNetworkModificationStacks(0)
{
	mClearNetworkModificationStacks = Core::getInstance()->getEventManager()->getEvent(
			NeuralNetworkConstants::EVENT_NETWORK_CLEAR_MODIFICATION_STACKS, true);
}

/**
 * Destructor.
 */
ScriptedNetworkManipulator::~ScriptedNetworkManipulator() {
}

void ScriptedNetworkManipulator::setNeuralNetwork(ModularNeuralNetwork *network) {
	mNetwork = network;
}


/**
 * Returns the id of the first neuron with the given name.
 */
qlonglong ScriptedNetworkManipulator::getNeuron(const QString &name) {
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
qlonglong ScriptedNetworkManipulator::getNeuroModule(const QString &name) {
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
qlonglong ScriptedNetworkManipulator::getNeuronGroup(const QString &name) {
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
 * Returns a list with the ids of all neurons of a given NeuronGroup.. If allEnclosed is true,
 * then all neurons of the group and all of its direct or indirect submodules are returned.
 * If false, then only the direct member neurons of the group are returned.
 * 
 * If the groupId is 0, then all neurons of the entire network are returned.
 * 
 * The allEnclosed parameter can be ommited. It defaults to false.
 * The groupId can also be ommited. It defaults to 0. 
 */
QVariantList ScriptedNetworkManipulator::getNeurons(qlonglong groupId, bool allEnclosed) {
	QVariantList neuronIds;

	if(mNetwork == 0) {
		return neuronIds;
	}

	QList<Neuron*> neurons;
	if(groupId == 0) {
		neurons = mNetwork->getNeurons();
		
	}
	else {
		NeuronGroup *group = ModularNeuralNetwork::selectNeuronGroupById(groupId, mNetwork->getNeuronGroups());
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
QVariantList ScriptedNetworkManipulator::getSubModules(qlonglong groupId) {
	QVariantList moduleIds;

	if(mNetwork == 0) {
		return moduleIds;
	}

	NeuronGroup *mainGroup = ModularNeuralNetwork::selectNeuronGroupById(groupId, mNetwork->getNeuronGroups());
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
qlonglong ScriptedNetworkManipulator::getParentModule(qlonglong elementId) {
	if(mNetwork == 0) {
		return 0;
	}
	NeuroModule *module = ModularNeuralNetwork::selectNeuroModuleById(elementId, mNetwork->getNeuroModules());
	if(module != 0) {
		if(module->getParentModule() == 0) {
			return 0;
		}
		return module->getParentModule()->getId();
	}
	Neuron *neuron = NeuralNetwork::selectNeuronById(elementId, mNetwork->getNeurons());
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
QVariantList ScriptedNetworkManipulator::getInSynapses(qlonglong neuronId) {
	QVariantList synapseIds;

	if(mNetwork == 0) {
		return synapseIds;
	}

	Neuron *neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
	if(neuron == 0) {
		return synapseIds;
	}

	QList<Synapse*> synapses = neuron->getSynapses();
	for(int i = 0; i < synapses.size(); i++) {
		synapseIds.append(synapses.at(i)->getId());
	}
	return synapseIds;
}

/**
 * Returns a list with the ids of all synapses going out of the neuron specified by its neuronId.
 * Will return an empty list in case of failure.
 */
QVariantList ScriptedNetworkManipulator::getOutSynapses(qlonglong neuronId) {
	QVariantList synapseIds;

	if(mNetwork == 0) {
		return synapseIds;
	}

	Neuron *neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
	if(neuron == 0) {
		return synapseIds;
	}

	QList<Synapse*> synapses = neuron->getOutgoingSynapses();
	for(int i = 0; i < synapses.size(); i++) {
		synapseIds.append(synapses.at(i)->getId());
	}
	return synapseIds;
}

/**
 * Returns the id of the source neuron of the synapse specified by its synapseId.
 * Will return 0 in case of failure.
 */
qlonglong ScriptedNetworkManipulator::getSource(qlonglong synapseId) {
	if(mNetwork == 0) {
		return 0;
	}
	Synapse *synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
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
qlonglong ScriptedNetworkManipulator::getTarget(qlonglong synapseId) {
	if(mNetwork == 0) {
		return 0;
	}
	Synapse *synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
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
double ScriptedNetworkManipulator::getBias(qlonglong neuronId) {
	if(mNetwork == 0) {
		return 0;
	}
	Neuron *neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
	if(neuron == 0) {
		return 0;
	}
	return neuron->getBiasValue().get();
}

/**
 * Returns the weight of the synapse with the given synapseId.
 * Will return 0 in case of failure.
 */
double ScriptedNetworkManipulator::getWeight(qlonglong synapseId) {
	if(mNetwork == 0) {
		return 0;
	}
	Synapse *synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
	if(synapse == 0) {
		return 0;
	}
	return synapse->getStrengthValue().get();
}

/**
 * Returns the current activation of a neuron.
 * Will return 0 in case of failure.
 */
double ScriptedNetworkManipulator::getActivation(qlonglong neuronId) {
	if(mNetwork == 0) {
		return 0;
	}
	Neuron *neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
	if(neuron == 0) {
		return 0;
	}
	return neuron->getActivationValue().get();
}

/**
 * Returns the current output of a neuron.
 * Will return 0 in case of failure.
 */
double ScriptedNetworkManipulator::getOutput(qlonglong neuronId) {
	if(mNetwork == 0) {
		return 0;
	}
	Neuron *neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
	if(neuron == 0) {
		return 0;
	}
	return neuron->getOutputActivationValue().get();
}

/**
 * Returns a 3-dimensional list with the x,y,z coordinates of the network object with the given objectId.
 * Will return an empty list in case of failure.
 */
QVariantList ScriptedNetworkManipulator::getPosition(qlonglong objectId) {
	QVariantList position;

	if(mNetwork == 0) {
		return position;
	}
	QList<NeuralNetworkElement*> networkElements;
	mNetwork->getNetworkElements(networkElements);
	NeuralNetworkElement *object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
	
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
QVariantList ScriptedNetworkManipulator::getProperties(qlonglong objectId) {
	QVariantList properties;

	if(mNetwork == 0) {
		return properties;
	}
	QList<NeuralNetworkElement*> networkElements;
	mNetwork->getNetworkElements(networkElements);
	NeuralNetworkElement *object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
	
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
QString ScriptedNetworkManipulator::getProperty(qlonglong objectId, const QString &name) {

	if(mNetwork == 0) {
		return "~";
	}
	QList<NeuralNetworkElement*> networkElements;
	mNetwork->getNetworkElements(networkElements);
	NeuralNetworkElement *object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
	
	Properties *props = dynamic_cast<Properties*>(object);

	if(props == 0) {
		return "~";
	}
	if(props->hasProperty(name)) {
		return props->getProperty(name);
	}
	return "~";
}

QString ScriptedNetworkManipulator::getName(qlonglong neuronId) {
	if(mNetwork == 0) {
		return "Unknown";
	}
	Neuron *neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
	if(neuron == 0) {
		return "Unknown";
	}
	return neuron->getNameValue().get();
}


/**
 * Sets the bias of the neuron with the given neuronId.
 * Returns false in case of failure, otherwise true.
 */
bool ScriptedNetworkManipulator::setBias(qlonglong neuronId, double bias) {
	if(mNetwork == 0) {
		return false;
	}
	Neuron *neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
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
bool ScriptedNetworkManipulator::setOutput(qlonglong neuronId, double output) {
	if(mNetwork == 0) {
		return false;
	}
	Neuron *neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
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
bool ScriptedNetworkManipulator::setActivation(qlonglong neuronId, double activation) {
	if(mNetwork == 0) {
		return false;
	}
	Neuron *neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());
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
bool ScriptedNetworkManipulator::setWeight(qlonglong synapseId, double weight) {
	if(mNetwork == 0) {
		return false;
	}
	Synapse *synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());
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
bool ScriptedNetworkManipulator::setPosition(qlonglong objectId, double x, double y, double z) {
	QVariantList position;

	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());

	QList<NeuralNetworkElement*> networkElements;
	mNetwork->getNetworkElements(networkElements);
	NeuralNetworkElement *object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
	
	if(object == 0) {
		return false;
	}
	object->setPosition(Vector3D(x, y, z));
	
	nmm->triggerNetworkStructureChangedEvent();
	return true;
}

/**
 * Sets the property "propName" of the network element "objectId" to the new "content". 
 * The content may also be ommited, then it is replaced by "".
 * Will return false in case of failure, otherwise true.
 */
bool ScriptedNetworkManipulator::setProperty(qlonglong objectId, const QString &propName, const QString &content) {
	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());

	QList<NeuralNetworkElement*> networkElements;
	mNetwork->getNetworkElements(networkElements);
	NeuralNetworkElement *object = NeuralNetwork::selectNetworkElementById(objectId, networkElements);
	
	Properties *props = dynamic_cast<Properties*>(object);

	if(props == 0) {
		return false;
	}
	props->setProperty(propName, content);
	
	nmm->triggerNetworkStructureChangedEvent();
	return true;
}

/**
 * Adds a network element (Neuron or NeuroModule) to the group specified by the groupId.
 * Will return false in case of failure, otherwise true.
 */
bool ScriptedNetworkManipulator::addToGroup(qlonglong elementId, qlonglong groupId) {
	if(mNetwork == 0) {
		return 0;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());

	NeuronGroup *group = ModularNeuralNetwork::selectNeuronGroupById(groupId, mNetwork->getNeuronGroups());
	if(group == 0) {
		return false;
	}
	
	NeuroModule *module = ModularNeuralNetwork::selectNeuroModuleById(elementId, mNetwork->getNeuroModules());
	if(module != 0) {
		bool ok = group->addSubModule(module);
		if(ok) {
			nmm->triggerNetworkStructureChangedEvent();
		}
		return ok;
	}
	Neuron *neuron = NeuralNetwork::selectNeuronById(elementId, mNetwork->getNeurons());
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
bool ScriptedNetworkManipulator::removeFromGroup(qlonglong elementId, qlonglong groupId) {
	if(mNetwork == 0) {
		return 0;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());

	NeuronGroup *group = ModularNeuralNetwork::selectNeuronGroupById(groupId, mNetwork->getNeuronGroups());
	if(group == 0) {
		return false;
	}
	
	NeuroModule *module = ModularNeuralNetwork::selectNeuroModuleById(elementId, mNetwork->getNeuroModules());
	if(module != 0) {
		bool ok = group->removeSubModule(module);
		if(ok) {
			nmm->triggerNetworkStructureChangedEvent();
		}
		return ok;
	}
	Neuron *neuron = NeuralNetwork::selectNeuronById(elementId, mNetwork->getNeurons());
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
qlonglong ScriptedNetworkManipulator::createNeuron(const QString &name) {
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
qlonglong ScriptedNetworkManipulator::createSynapse(qlonglong sourceId, qlonglong targetId, double weight) {
	if(mNetwork == 0) {
		return 0;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());

	QList<Neuron*> neurons = mNetwork->getNeurons();
	QList<Synapse*> synapses = mNetwork->getSynapses();

	Neuron *source = NeuralNetwork::selectNeuronById(sourceId, neurons);
	SynapseTarget *target = NeuralNetwork::selectNeuronById(targetId, neurons);

	if(target == 0) {
		target = NeuralNetwork::selectSynapseById(targetId, synapses);
	}
	if(source == 0 || target == 0) {
		return 0;
	}

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
qlonglong ScriptedNetworkManipulator::createNeuroModule(const QString &name) {
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
qlonglong ScriptedNetworkManipulator::createNeuronGroup(const QString &name) {
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


bool ScriptedNetworkManipulator::removeNeuron(qlonglong neuronId) {
	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	Neuron *neuron = NeuralNetwork::selectNeuronById(neuronId, mNetwork->getNeurons());

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


bool ScriptedNetworkManipulator::removeSynapse(qlonglong synapseId) {
	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	Synapse *synapse = NeuralNetwork::selectSynapseById(synapseId, mNetwork->getSynapses());

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



bool ScriptedNetworkManipulator::removeNeuronGroup(qlonglong groupId) {
	if(mNetwork == 0) {
		return false;
	}
	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker g(nmm->getNetworkExecutionMutex());
	NeuronGroup *group = ModularNeuralNetwork::selectNeuronGroupById(groupId, mNetwork->getNeuronGroups());

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



}



