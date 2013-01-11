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

#include "NeuroModule.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <iostream>
#include "Util/Tracer.h"
#include "NeuralNetworkConstants.h"
#include "Util/NeuralNetworkUtil.h"
#include "Core/Core.h"
#include <limits>

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {

NeuroModule::NeuroModule(const QString &name, qulonglong id)
	: NeuronGroup(name, id), mParentModule(0)
{
	TRACE("NeuroModule::NeuroModule");
	QString moduleName = name;
	//setProperty("ModuleName", moduleName.append("_").append(QString::number(mId)));
	setProperty(NeuralNetworkConstants::TAG_MODULE_TYPE, "Default");
	setProperty(NeuralNetworkConstants::TAG_MODULE_IDENTIFIER, QString::number(mId));
}

NeuroModule::NeuroModule(const NeuroModule &other) 
	: Object(), ValueChangedListener(), NeuronGroup(other), 
	  mParentModule(0)
{
	TRACE("NeuroModule::NeuroModuleCopy");
	
	//copy submodules, ignore the module pointers copied in GroupModule.
	mSubModules.clear();
	for(QListIterator<NeuroModule*> i(other.mSubModules); i.hasNext();) {
		NeuroModule *module = dynamic_cast<NeuroModule*>(i.next()->createCopy());
		addSubModule(module);
	}
}

NeuroModule::~NeuroModule() {
	TRACE("NeuroModule::~NeuroModule");
}

NeuronGroup* NeuroModule::createCopy() const {
	TRACE("NeuroModule::createCopy");

	return new NeuroModule(*this);
}

NeuroModule* NeuroModule::createDeepCopy() const {
	
	NeuroModule *copy = new NeuroModule(*this);
	
	QList<NeuroModule*> allModules;
	allModules.append(copy),
	allModules << copy->getAllEnclosedModules();

	for(QListIterator<NeuroModule*> i(allModules); i.hasNext();) {
		NeuroModule *module = i.next();
		QList<Neuron*> neurons = module->getNeurons();
		module->removeAllNeurons();
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *neuron = i.next();
			module->addNeuron(neuron->createCopy());
		}
	}

	QList<Neuron*> allNeurons = getAllEnclosedNeurons();

	QList<Synapse*> newSynapses;
	
	QList<SynapseTarget*> targets;
	for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
		targets.append(i.next());
	}

	//copy local synapses (but not synapses to the outside world)
	while(!targets.empty()) {
		SynapseTarget *target = targets.takeFirst();

		QList<Synapse*> synapses = target->getSynapses();
		for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
			Synapse *synapse = i.next();

			bool valid = true;
			QList<Neuron*> neurons = NeuralNetworkUtil::getConnectedNeurons(synapse);
			for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
				if(!allNeurons.contains(j.next())) {
					valid = false;
					break;
				}
			}
			if(!valid) {
				continue;
			}

			//copy synapse
			Synapse *synapseCopy = synapse->createCopy();
			newSynapses.append(synapseCopy);

			targets.append(synapse);
			
		}
	}

	allNeurons = copy->getAllEnclosedNeurons();

	//reconnect synapses to the new neurons of the copied module.
	for(QListIterator<Synapse*> i(newSynapses); i.hasNext();) {
		Synapse *synapse = i.next();

		Neuron *source = NeuralNetwork::selectNeuronById(
							synapse->getSource()->getId(), allNeurons);
		SynapseTarget *target = NeuralNetwork::selectNeuronById(
							synapse->getTarget()->getId(), allNeurons);
		if(target == 0) {
			target = NeuralNetwork::selectSynapseById(synapse->getTarget()->getId(), newSynapses);
		}

		if(source == 0 || target == 0) {
			Core::log("NeuroModule: Problem copying module. Synapses did not match.");
			continue;
		}

		synapse->setSource(source);
		target->addSynapse(synapse);
	}

	copy->renewIds();

	return copy;
}


void NeuroModule::setId(qulonglong id) {
	NeuronGroup::setId(id);
	setProperty(NeuralNetworkConstants::TAG_MODULE_IDENTIFIER, QString::number(mId));
}


bool NeuroModule::setOwnerNetwork(ModularNeuralNetwork *network) {
	TRACE("NeuroModule::setOwnerNetwork");

	if(network == 0) {
		mOwnerNetwork = 0;
		return true;
	}
	//check if there are other NeuroModules that contain the same neurons as this one.
	//if there is another NeuroModule containing one of the neurons of this module,
	//then this method returns false to indicate incompatibility. Neurons can only
	//live in one NeuroModule at a time.
	QList<NeuronGroup*> groups = network->getNeuronGroups();
	for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
		NeuroModule *module = dynamic_cast<NeuroModule*>(i.next());
		if(module != 0 && module != this) {
			QList<Neuron*> neurons = module->getNeurons();
			for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
				if(mMemberNeurons.contains(j.next())) {
					mOwnerNetwork = 0;
					return false;
				}
			}
		}
	}
	mOwnerNetwork = network;
	return true;
}

bool NeuroModule::addInputNeuron(Neuron *neuron) {
	TRACE("NeuroModule::addInputNeuron");

	if(neuron == 0) {
		return false;
	}
	addNeuron(neuron);

	neuron->setProperty(NeuralNetworkConstants::TAG_MODULE_INPUT);
	return true;
}


bool NeuroModule::removeInputNeuron(Neuron *neuron) {
	TRACE("NeuroModule::removeInputNeuron");

	if(neuron == 0) {
		return false;
	}

	neuron->removeProperty(NeuralNetworkConstants::TAG_MODULE_INPUT);
	return true;
}

//TODO not tested yet
QList<Neuron*> NeuroModule::getInputNeurons(int startLevel) const {
	TRACE("NeuroModule::getInputNeurons");

	QList<Neuron*> neurons;
	for(QListIterator<Neuron*> i(mMemberNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_MODULE_INPUT)) {
			neurons.append(neuron);
		}
	}
	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
		NeuroModule *module = i.next();
		QList<Neuron*> inputNeurons = module->getInputNeurons(startLevel + 1);
		for(QListIterator<Neuron*> j(inputNeurons); j.hasNext();) {
			Neuron *neuron = j.next();
			
			int level = 0;
			bool isExtendedInterface = false;
			QString levelString = neuron->getProperty(NeuralNetworkConstants::TAG_MODULE_INPUT);

			if(neuron->hasProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE)) {
				levelString = neuron->getProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
				isExtendedInterface = true;
			}
					
			if(levelString.trimmed() == "") {
				if(isExtendedInterface) {
					level = numeric_limits<int>::max();
				}
			}
			else {
				bool ok = true;
				int interfaceLevel = levelString.toInt(&ok);
				if(ok) {
					level = interfaceLevel;
				}
			}
			if(level > startLevel) {
				neurons.append(neuron);
			}
// 			if(neuron->hasProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE)) {
// 				int level = 1;
// 				QString levelString = 
// 						neuron->getProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
// 				if(levelString.trimmed() == "") {
// 					level = numeric_limits<int>::max();
// 				}
// 				else {
// 					level = levelString.toInt();
// 				}
// 				if(level > startLevel) {
// 					neurons.append(neuron);
// 				}
// 			}
		}
	}

	return neurons;
}



//TODO not tested yet
bool NeuroModule::addOutputNeuron(Neuron *neuron) {
	TRACE("NeuroModule::addOutputNeuron");

	if(neuron == 0) {
		return false;
	}
	addNeuron(neuron);

	neuron->setProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT);
	return true;
	
}


bool NeuroModule::removeOutputNeuron(Neuron *neuron) {
	TRACE("NeuroModule::removeOutputNeuron");

	if(neuron == 0) {
		return false;
	}

	neuron->removeProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT);
	return true;
}


QList<Neuron*> NeuroModule::getOutputNeurons(int startLevel) const {
	TRACE("NeuroModule::getOutputNeurons");

	QList<Neuron*> neurons;
	for(QListIterator<Neuron*> i(mMemberNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT)) {
			neurons.append(neuron);
		}
	}
	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
		NeuroModule *module = i.next();
		QList<Neuron*> outputNeurons = module->getOutputNeurons(startLevel + 1);
		for(QListIterator<Neuron*> j(outputNeurons); j.hasNext();) {
			Neuron *neuron = j.next();

			int level = 0;
			bool isExtendedInterface = false;
			QString levelString = neuron->getProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT);

			if(neuron->hasProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE)) {
				levelString = neuron->getProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
				isExtendedInterface = true;
			}
					
			if(levelString.trimmed() == "") {
				if(isExtendedInterface) {
					level = numeric_limits<int>::max();
				}
			}
			else {
				bool ok = true;
				int interfaceLevel = levelString.toInt(&ok);
				if(ok) {
					level = interfaceLevel;
				}
			}
			if(level > startLevel) {
				neurons.append(neuron);
			}
// 			int level = 1;
// 			if(neuron->hasProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE)) {
// 				QString levelString = 
// 						neuron->getProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
// 				if(levelString.trimmed() == "") {
// 					level = numeric_limits<int>::max();
// 				}
// 				else {
// 					level = levelString.toInt();
// 				}
// 				if(level > startLevel) {
// 					neurons.append(neuron);
// 				}
// 			}
		}
	}

	return neurons;
}

bool NeuroModule::addSubModule(NeuroModule *module) {
	TRACE("NeuroModule::addSubModule");

	if(module == 0 || mSubModules.contains(module) || module == this) {
		return false;
	}

	//prevent submodule from beeing added when the module is already used somewhere
	if(getAllEnclosedModules().contains(module)) {
		return false;
	}

	//allow only adding modules that are not already submodules somewhere
	if(mOwnerNetwork != 0 && mOwnerNetwork->getNeuroModules().contains(module)) {
		QList<NeuroModule*> modules = mOwnerNetwork->getNeuroModules();
		for(QListIterator<NeuroModule*> i(modules); i.hasNext();) {
			NeuroModule *m = i.next();
			if(m == module) {
				continue;
			}
			if(m->getAllEnclosedModules().contains(module)) {
				return false;
			}
		}
	}

	//prevent submodule from being added when one of its neurons is part of 
	//this or another module.
	QList<Neuron*> subNeurons = module->getNeurons();
	for(QListIterator<Neuron*> i(subNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		if(getAllEnclosedNeurons().contains(neuron)) {
			return false;
		}
		if(mOwnerNetwork != 0) {
			QList<NeuroModule*> allModules = mOwnerNetwork->getNeuroModules();
			for(QListIterator<NeuroModule*> j(allModules); j.hasNext();) {
				NeuroModule *mod = j.next();
				if(mod != module && mod->getNeurons().contains(neuron)) {
					return false;
				}
			}
		}
	}
	mSubModules.append(module);
	module->mParentModule = this;
	if(mOwnerNetwork != 0) {
		mOwnerNetwork->addNeuronGroup(module);
	}
	return true;
}


bool NeuroModule::removeSubModule(NeuroModule *module) {
	TRACE("NeuroModule::removeSubModule");

	if(module == 0 || !mSubModules.contains(module)) {
		return false;
	}
	mSubModules.removeAll(module);
	module->mParentModule = 0;
	return true;
}

void NeuroModule::removeAllSubModules() {
	TRACE("NeuroModule::removeAllSubModules");

	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
		i.next()->mParentModule = 0;
	}
	mSubModules.clear();
}


QList<NeuroModule*> NeuroModule::getSubModules() const {
	TRACE("NeuroModule::getSubModules");

	return mSubModules;
}

NeuroModule* NeuroModule::getParentModule() const {
	return mParentModule;
}

// QList<Neuron*> NeuroModule::getAllEnclosedNeurons() const {
// 	TRACE("NeuroModule::getAllEnclosedNeurons");
// 
// 	QList<Neuron*> neurons = getNeurons();
// 	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
// 		NeuroModule *module = i.next();
// 		neurons << module->getAllEnclosedNeurons();
// 	}
// 	return neurons;
// }


// QList<NeuroModule*> NeuroModule::getAllEnclosedModules() const {
// 	TRACE("NeuroModule::getAllEnclosedModules");
// 
// 	QList<NeuroModule*> modules = mSubModules;
// 
// 	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
// 		NeuroModule *module = i.next();
// 		modules << module->getAllEnclosedModules();
// 	}
// 	return modules;
// }


QList<Synapse*> NeuroModule::getAllEnclosedSynapses() const {
	QList<Synapse*> allSynapses;

		
	QList<Neuron*> allNeurons = getAllEnclosedNeurons();
	QList<SynapseTarget*> targets;
	for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
		targets.append(i.next());
	}

	while(!targets.empty()) {
		SynapseTarget *target = targets.takeFirst();

		QList<Synapse*> synapses = target->getSynapses();
		for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
			Synapse *synapse = i.next();

			bool valid = true;
			QList<Neuron*> neurons = NeuralNetworkUtil::getConnectedNeurons(synapse);
			for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
				if(!allNeurons.contains(j.next())) {
					valid = false;
					break;
				}
			}
			if(!valid) {
				continue;
			}
			targets.append(synapse);
			allSynapses.append(synapse);
		}
	}
	return allSynapses;
}


// bool NeuroModule::moveGroupToNetworkConst(ModularNeuralNetwork const *oldNetwork, 
// 										ModularNeuralNetwork *newNetwork)
// {
// 	TRACE("NeuroModule::moveGroupToNetworkConst");
// 
// 	bool ok = true;
// 	//add submodules first to ensure that the neurons are adapted before the parent module is added.
// 	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
// 		if(!i.next()->moveGroupToNetworkConst(oldNetwork, newNetwork)) {
// 			ok = false;
// 		}
// 	}
// 	if(!NeuronGroup::moveGroupToNetworkConst(oldNetwork, newNetwork)) {
// 		ok = false;
// 	}
// 
// 	return ok;
// }


/**
 * Currently z is ignored.
 */
bool NeuroModule::moveGroupToLocation(double x, double y, double) {
	QPointF pos = NeuralNetworkUtil::getPosition(this);
	QPointF newPos(x, y);

	QPointF shift = newPos - pos;

	NeuralNetworkUtil::setNetworkElementLocationProperty(this, x, y, 0.0);
	
	QList<Neuron*> allNeurons = getAllEnclosedNeurons();
	QList<Synapse*> allSynapses = getAllEnclosedSynapses();
	QList<NeuroModule*> allModules = getAllEnclosedModules();

	for(QListIterator<NeuroModule*> i(allModules); i.hasNext();) {
		NeuroModule *module = i.next();
		QPointF modulePos = NeuralNetworkUtil::getPosition(module) + shift;
		NeuralNetworkUtil::setNetworkElementLocationProperty(module, modulePos.x(), modulePos.y(), 0.0);
	}
	for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		QPointF neuronPos = NeuralNetworkUtil::getPosition(neuron) + shift;
		NeuralNetworkUtil::setNetworkElementLocationProperty(neuron, neuronPos.x(), neuronPos.y(), 0.0);
	}
	for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		QPointF synapsePos = NeuralNetworkUtil::getPosition(synapse) + shift;
		NeuralNetworkUtil::setNetworkElementLocationProperty(synapse, synapsePos.x(), synapsePos.y(), 0.0);
	}
	
	return true;
}


bool NeuroModule::replaceNeuron(Neuron *oldNeuron, Neuron *newNeuron) {
	if(oldNeuron == 0 || newNeuron == 0) {
		return false;
	}
	if(oldNeuron == newNeuron) {
		return true;
	}
	if(getAllEnclosedNeurons().contains(newNeuron)) {
		return false;
	}
	if(mMemberNeurons.contains(oldNeuron)) {
		int index = mMemberNeurons.indexOf(oldNeuron);
		mMemberNeurons.insert(index, newNeuron);
		mMemberNeurons.removeAll(oldNeuron);
		
		//reconnect neuron
		QList<Synapse*> synapses = oldNeuron->getSynapses();
		synapses << oldNeuron->getOutgoingSynapses();
		while(!synapses.empty()) {
			Synapse *synapse = synapses.at(0);
			synapses.removeAll(synapse);
			if(synapse->getSource() == oldNeuron) {
				synapse->setSource(newNeuron);
			}
			if(synapse->getTarget() == oldNeuron) {
				oldNeuron->removeSynapse(synapse);
				newNeuron->addSynapse(synapse);
			}
		}
		return true;
	}
	else {
		QList<NeuroModule*> allModules = getAllEnclosedModules();
		for(QListIterator<NeuroModule*> i(allModules); i.hasNext();) {
			NeuroModule *module = i.next();
			if(module->getNeurons().contains(oldNeuron)) {
				return module->replaceNeuron(oldNeuron, newNeuron);
			}
		}
	}
	return false;
}



void NeuroModule::setSize(const QSizeF &size) {
	NeuralNetworkUtil::setNeuroModuleLocationSizeProperty(this, size.width(), size.height());
}


QSizeF NeuroModule::getSize() const {
	return NeuralNetworkUtil::getModuleSize(this);
}


bool NeuroModule::addNeuron(Neuron *neuron) {
	TRACE("NeuroModule::addNeuron");

	if(neuron == 0) {
		return false;
	}
	if(mOwnerNetwork == 0) {
		return NeuronGroup::addNeuron(neuron);
	}
	QList<NeuronGroup*> groups = mOwnerNetwork->getNeuronGroups();
	for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
		NeuroModule *module = dynamic_cast<NeuroModule*>(i.next());
		if(module != 0) {
			if(module->getNeurons().contains(neuron)) {
				return false;
			}
		}
	}
	return NeuronGroup::addNeuron(neuron);
}

bool NeuroModule::removeNeuron(Neuron *neuron) {
	TRACE("NeuroModule::removeNeuron");

	if(neuron == 0) {
		return false;
	}
	bool ok = NeuronGroup::removeNeuron(neuron);
	
	//do not remove from owner network

	return ok;
}



void  NeuroModule::removeAllNeurons() {
	TRACE("NeuroModule::removeAllNeurons");

	NeuronGroup::removeAllNeurons();
}



bool NeuroModule::equals(NeuronGroup *group) {
	TRACE("NeuroModule::equals");

	if(NeuronGroup::equals(group) == false) {
		return false;
	}
	NeuroModule *nm = dynamic_cast<NeuroModule*>(group);
	if(nm == 0) {
		return false;
	}

	
	
// 	QList<NeuroModule*> otherSubModules = nm->mSubModules;
// 	if(mSubModules.size() != otherSubModules.size()) {
// 		return false;
// 	}
// 	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
// 		NeuroModule *module = i.next();
// 		NeuroModule *otherModule = ModularNeuralNetwork::selectNeuroModuleById(
// 										module->getId(), otherSubModules);
// 
// 		if(otherModule == 0) {
// 			return false;
// 		}
// 		if(module->equals(otherModule) == false) {
// 			return false;
// 		}
// 	}

	return true;
}


void NeuroModule::renewIds() {

	QList<Neuron*> allNeurons = getAllEnclosedNeurons();
	QList<Synapse*> allSynapses = getAllEnclosedSynapses();
	QList<NeuroModule*> allModules = getAllEnclosedModules();

	QHash<qulonglong, qulonglong> changedIds;
		
	qulonglong newId = NeuralNetwork::generateNextId();
	changedIds.insert(getId(), newId);
	setId(newId);

	for(QListIterator<NeuroModule*> i(allModules); i.hasNext();) {
		NeuralNetworkElement *elem = i.next();
		qulonglong newId = NeuralNetwork::generateNextId();
		changedIds.insert(elem->getId(), newId);
		elem->setId(newId);
	}
	for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
		NeuralNetworkElement *elem = i.next();
		qulonglong newId = NeuralNetwork::generateNextId();
		changedIds.insert(elem->getId(), newId);
		elem->setId(newId);
	}
	for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
		NeuralNetworkElement *elem = i.next();
		qulonglong newId = NeuralNetwork::generateNextId();
		changedIds.insert(elem->getId(), newId);
		elem->setId(newId);
	}

	notifyMemberIdsChanged(changedIds);
}

QList<NeuralNetworkElement*> NeuroModule::getAllEnclosedNetworkElements() const {
	QList<NeuralNetworkElement*> elements;

	QList<Neuron*> allNeurons = getAllEnclosedNeurons();
	QList<Synapse*> allSynapses = getAllEnclosedSynapses();
	QList<NeuroModule*> allModules = getAllEnclosedModules();

	for(QListIterator<NeuroModule*> i(allModules); i.hasNext();) {
		elements.append(i.next());
	}
	for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
		elements.append(i.next());
	}
	for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
		elements.append(i.next());
	}

	return elements;
}

}


