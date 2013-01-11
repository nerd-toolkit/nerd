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

#include "ModularNeuralNetwork.h"
#include <QListIterator>
#include <iostream>
#include "Constraints/GroupConstraint.h"
#include "Core/Core.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Math/Math.h"
#include "Util/Tracer.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "Util/Util.h"
#include "NeuralNetworkConstants.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);


using namespace std;

namespace nerd {


ModularNeuralNetwork::ModularNeuralNetwork(const ActivationFunction &defaultActivationFunction,
					  const TransferFunction &defaultTransferFunction,
					  const SynapseFunction &defaultSynapseFunction)
	: NeuralNetwork(defaultActivationFunction, defaultTransferFunction, defaultSynapseFunction)
{
	TRACE("ModularNeuralNetwork::ModularNeuralNetwork");

	mDefaultNeuronGroup = new NeuronGroup("Default");
	addNeuronGroup(mDefaultNeuronGroup);
}

ModularNeuralNetwork::ModularNeuralNetwork(const ModularNeuralNetwork &other) 
	: Controller(), Object(), NeuralNetwork(other), mDefaultNeuronGroup(0)
{
	TRACE("ModularNeuralNetwork::ModularNeuralNetworkCopy");


	QList<NeuroModule*> newModules;	
	QList<NeuronGroup*> newGroups;

	mDefaultNeuronGroup = other.getDefaultNeuronGroup()->createCopy();

	newGroups.append(mDefaultNeuronGroup);

	//copy all groups
	{	
		for(QListIterator<NeuronGroup*> i(other.mNeuronGroups); i.hasNext();) {

			NeuronGroup *otherGroup = i.next();

			if(otherGroup == 0) {
				Core::log("ModularNeuralNetwork: Three was a NULL group in the network!");
				continue;
			}

			if(newGroups.contains(otherGroup) || otherGroup == other.getDefaultNeuronGroup()) {
				continue;
			}

			NeuroModule *module = dynamic_cast<NeuroModule*>(otherGroup);
			if(module != 0 && other.getSubModuleOwner(module) != 0) {
				//this is a submodule, so ignore it.
				continue;
			}

			if(ModularNeuralNetwork::selectNeuronGroupById(
						otherGroup->getId(), newGroups)	!= 0) 
			{
				//this module is already contained (e.g. as submodule)
				continue;
			}

			NeuronGroup *ng = otherGroup->createCopy();
			newGroups.append(ng);

			NeuroModule *nm = dynamic_cast<NeuroModule*>(ng);
			if(nm != 0 && !newModules.contains(nm)) {
				newModules.append(nm);

				//add submodules only for NeuroModules (because NeuronGroups still have pointers to the other networks modules).
				QList<NeuroModule*> subModules = ng->getAllEnclosedModules();
				for(QListIterator<NeuroModule*> k(subModules); k.hasNext();) {
					NeuroModule *module = k.next();
					if(!newGroups.contains(module)) {
						newGroups.append(module);
					}
					if(!newModules.contains(module)) {
						newModules.append(module);
					}
				}
			}
		}

		//verify and update submodule pointers
		for(QListIterator<NeuronGroup*> i(newGroups); i.hasNext();) {
			i.next()->verifySubModulePointers(this, newModules);
		}

		//move groups to this network.
		for(QListIterator<NeuronGroup*> i(newGroups); i.hasNext();) {
			NeuronGroup *group = i.next();
			group->moveGroupToNetworkConst(&other, this);
		}
	}
}

ModularNeuralNetwork::~ModularNeuralNetwork() {
	TRACE("ModularNeuralNetwork::~ModularNeuralNetwork");

	while(!mNeuronGroups.empty()) {
		NeuronGroup *g = mNeuronGroups.at(0);
// 		cerr << ">Deleting NeuronGroup " << g->getName().toStdString().c_str() << " " << g << endl;
		mNeuronGroups.removeAll(g);
		delete g;
	}
}


NeuralNetwork* ModularNeuralNetwork::createCopy() {
	TRACE("ModularNeuralNetwork::createCopy");

	return new ModularNeuralNetwork(*this);
}


void ModularNeuralNetwork::reset() {
	TRACE("ModularNeuralNetwork::reset");

	NeuralNetwork::reset();
}


bool ModularNeuralNetwork::equals(NeuralNetwork *network) {
	TRACE("ModularNeuralNetwork::equals");

	if(NeuralNetwork::equals(network) == false) {
		return false;
	}
	ModularNeuralNetwork *mnn = dynamic_cast<ModularNeuralNetwork*>(network);

	if(mnn == 0) {
		return false;
	}
	QList<NeuronGroup*> otherGroups = mnn->mNeuronGroups;

	if(mNeuronGroups.size() != otherGroups.size()) {
		return false;
	}

	for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
		NeuronGroup *group = i.next();
		NeuronGroup *otherGroup = ModularNeuralNetwork::selectNeuronGroupById(
									group->getId(), otherGroups);

		if(otherGroup == 0) {
			return false;
		}
		if(group->equals(otherGroup) == false) {
			return false;
		}	
	}
	return true;
}


bool ModularNeuralNetwork::addNeuron(Neuron *neuron) {
	TRACE("ModularNeuralNetwork::addNeuron");

	if(NeuralNetwork::addNeuron(neuron)) {
		mDefaultNeuronGroup->addNeuron(neuron);
		return true;
	}
	return false;
}


bool ModularNeuralNetwork::removeNeuron(Neuron *neuron) {
	TRACE("ModularNeuralNetwork::removeNeuron");

	bool ok = NeuralNetwork::removeNeuron(neuron);
	mDefaultNeuronGroup->removeNeuron(neuron);
	for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
		NeuronGroup *group = i.next();
		if(group->getNeurons().contains(neuron)) {
			group->removeNeuron(neuron);
		}
	}
	return ok;
}


bool ModularNeuralNetwork::addNeuronGroup(NeuronGroup *group) {
	TRACE("ModularNeuralNetwork::addNeuronGroup");

	if(group == 0 || mNeuronGroups.contains(group)) {
		return false;
	}
	if(group->setOwnerNetwork(this)) {
		mNeuronGroups.append(group);

		//add all neurons of group, that are not already part of the network
		QList<Neuron*> groupNeurons = group->getNeurons();
		for(QListIterator<Neuron*> i(groupNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
			if(!mNeurons.contains(neuron)) {
				addNeuron(neuron);
			}
		}
	
		//add all submodules
		QList<NeuroModule*> modules = group->getSubModules();
		for(QListIterator<NeuroModule*> i(modules); i.hasNext();) {
			NeuroModule *mod = i.next();
			if(!mNeuronGroups.contains(mod)) {
				addNeuronGroup(mod);
			}
		}
		return true;
	}
	else {
		Core::log(QString("ModularNeuralNetwork: Could not add NeuronGroup [")
				.append(group->getName()).append("]!"));
	}
	return false;
}


/**
 * The DefaultNeuronGroup can not be destroyed.
 */
bool ModularNeuralNetwork::removeNeuronGroup(NeuronGroup *group) {
	TRACE("ModularNeuralNetwork::removeNeuronGroup");

	if(group == 0 || !mNeuronGroups.contains(group) || group == mDefaultNeuronGroup) {
		return false;
	}
	mNeuronGroups.removeAll(group);
	group->setOwnerNetwork(0);

	//remove neurons and submodules if group was a module
	NeuroModule *module = dynamic_cast<NeuroModule*>(group);
	if(module != 0) {
		QList<Neuron*> neurons = module->getNeurons();
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			removeNeuron(i.next());
		}
		QList<NeuroModule*> modules = module->getSubModules();
		for(QListIterator<NeuroModule*> i(modules); i.hasNext();) {
			removeNeuronGroup(i.next());
		}
		for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
			NeuronGroup *group = i.next();

			//check if this module was part of a NeuronGroup, if so, remove it from the group
			if(group->getSubModules().contains(module)) {
				group->removeSubModule(module);
			}
			//remove module from parent module if this module was a submodule.
			NeuroModule *mod = dynamic_cast<NeuroModule*>(group);
			if(mod != 0) {
				if(mod->getSubModules().contains(module)) {
					mod->removeSubModule(module);
					break;
				}
			}
			
		}
		
	}
	
	
	return true;
}

NeuronGroup* ModularNeuralNetwork::getNeuronGroup(const QString &name) const {
	TRACE("ModularNeuralNetwork::getNeuronGroup");

	for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
		NeuronGroup *group = i.next();
		if(group->getName() == name) {
			return group;
		}
	}
	return 0;
}

NeuronGroup* ModularNeuralNetwork::getDefaultNeuronGroup() const {
	TRACE("ModularNeuralNetwork::getDefaultNeuronGroup");

	return mDefaultNeuronGroup;
}


QList<NeuronGroup*> ModularNeuralNetwork::getNeuronGroups() const {
	TRACE("ModularNeuralNetwork::getNeuronGroups");

	return mNeuronGroups;
}


QList<NeuroModule*> ModularNeuralNetwork::getNeuroModules() const {
	TRACE("ModularNeuralNetwork::getNeuroModules");

	QList<NeuroModule*> modules;
	for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
		NeuroModule *module = dynamic_cast<NeuroModule*>(i.next());
		if(module != 0) {
			modules.append(module);
		}
	}	
	return modules;
}

NeuroModule* ModularNeuralNetwork::getSubModuleOwner(NeuroModule *module) const {
	TRACE("ModularNeuralNetwork::getSubModuleOwner");

	for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
		NeuroModule *m = dynamic_cast<NeuroModule*>(i.next());
		if(m != 0 && m->getSubModules().contains(module)) {
			return m;
		}
	}	
	return 0;
}


QList<NeuralNetworkElement*> ModularNeuralNetwork::savelyRemoveNetworkElement(
					NeuralNetworkElement *element) 
{

	QList<NeuralNetworkElement*> removedElements = NeuralNetwork::savelyRemoveNetworkElement(element);

	Neuron *neuron = dynamic_cast<Neuron*>(element);
	if(neuron != 0) {
		//make sure that neurons are also removed from all groups
		QList<NeuronGroup*> groups = mNeuronGroups;
		for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
			NeuronGroup *group = i.next();
			if(group->getNeurons().contains(neuron)) {
				group->removeNeuron(neuron);
			}
		}
	}
	NeuroModule *module = dynamic_cast<NeuroModule*>(element);
	if(module != 0) {
		if(!mNeuronGroups.contains(module)) {
			return removedElements;
		}
		QList<NeuroModule*> subModules = module->getSubModules();
		for(QListIterator<NeuroModule*> i(subModules); i.hasNext();) {
			NeuroModule *subModule = i.next();
			Util::addWithoutDuplicates(removedElements, savelyRemoveNetworkElement(subModule));
		}
		QList<Neuron*> neurons = module->getNeurons();
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *neuron = i.next();
			Util::addWithoutDuplicates(removedElements, savelyRemoveNetworkElement(neuron));
		}
		if(module->getParentModule() != 0) {
			module->getParentModule()->removeSubModule(module);
		}
		//remove from all groups
		QList<NeuronGroup*> groups = mNeuronGroups;
		for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
			NeuronGroup *group = i.next();
			if(group->getSubModules().contains(module)) {
				group->removeSubModule(module);
			}
		}
	}
	NeuronGroup *group = dynamic_cast<NeuronGroup*>(element);
	if(group != 0) {
		if(removeNeuronGroup(group)) {
			removedElements.append(group);
		}
	}

	return removedElements;
}

NeuroModule* ModularNeuralNetwork::getOwnerModule(Neuron *neuron) const {
	for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
		NeuroModule *module = dynamic_cast<NeuroModule*>(i.next());
		if(module != 0) {
			if(module->getNeurons().contains(neuron)) {
				return module;
			}
		}
	}
	return 0;
}


QList<NeuroModule*> ModularNeuralNetwork::getRootModules() const {
	QList<NeuroModule*> modules;
	for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
		NeuroModule *module = dynamic_cast<NeuroModule*>(i.next());
		if(module != 0 && module->getParentModule() == 0) {
			modules.append(module);
		}
	}
	return modules;
}


QList<Neuron*> ModularNeuralNetwork::getRootNeurons() const {
	QList<Neuron*> neurons = mNeurons;
	QList<NeuroModule*> rootModules = getRootModules();
	for(QListIterator<NeuroModule*> i(rootModules); i.hasNext();) {
		NeuroModule *module = i.next();
		QListIterator<Neuron*> allNeurons = module->getAllEnclosedNeurons();
		for(QListIterator<Neuron*> j(allNeurons); j.hasNext();) {
			neurons.removeAll(j.next());
		}
	}
	return neurons;
}

bool ModularNeuralNetwork::notifyMemberIdsChanged(QHash<qulonglong, qulonglong> changedIds) {
	bool ok = true;
	for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
		if(!i.next()->notifyMemberIdsChanged(changedIds)) {
			ok = false;
		}
	}
	
	//change own synaptic pathway property
	if(hasProperty(NeuralNetworkConstants::TAG_MODULE_PERMITTED_PATHWAYS)) {
		QString newPairValue;	
	
		QStringList pairStrings = getProperty(NeuralNetworkConstants::TAG_MODULE_PERMITTED_PATHWAYS).split("|");
		for(QListIterator<QString> i(pairStrings); i.hasNext();) {
			QStringList pair = i.next().split(",");
			if(pair.size() != 2) {
				continue;
			}
			bool ok = true;
			qulonglong referenceId = pair.at(0).toULongLong(&ok);
			if(!ok) {
				continue;
			}
			qulonglong ownerId = pair.at(1).toULongLong(&ok);
			if(!ok) {
				continue;
			}
			if(changedIds.keys().contains(referenceId)) {
				referenceId = changedIds.value(referenceId);
			}
			if(changedIds.keys().contains(ownerId)) {
				ownerId = changedIds.value(ownerId);
			}
			newPairValue = newPairValue + QString::number(referenceId) + "," + QString::number(ownerId);
			if(i.hasNext()) {
				newPairValue.append("|");
			}
		}
		setProperty(NeuralNetworkConstants::TAG_MODULE_PERMITTED_PATHWAYS, newPairValue);
	}
	
	return ok;
}


void ModularNeuralNetwork::getNetworkElements(QList<NeuralNetworkElement*> &elementList) const {
	TRACE("ModularNeuralNetwork::getNetworkElements");

	NeuralNetwork::getNetworkElements(elementList);
	for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
		elementList.append(i.next());
	}
}


void ModularNeuralNetwork::adjustIdCounter() {
	TRACE("ModularNeuralNetwork::adjustIdCounter");

	NeuralNetwork::adjustIdCounter();

	qulonglong maxId = 1;

	for(QListIterator<NeuronGroup*> i(mNeuronGroups); i.hasNext();) {
		NeuronGroup *group = i.next();
		
		maxId = Math::max(maxId, group->getId());

		QList<GroupConstraint*> constraints = group->getConstraints();
		for(QListIterator<GroupConstraint*> j(constraints); j.hasNext();) {
			maxId = Math::max(maxId, j.next()->getId());
		}
	}
	NeuralNetwork::resetIdCounter(Math::max(NeuralNetwork::generateNextId(), maxId));
}

/**
 * The network will release all netowrk elements (except the default functions 
 * and the default NeuronGroup. The released elements will not be destroyed if 
 * destroyElements is false.
 * The caller of this method is responsible for their destruction.
 */
void ModularNeuralNetwork::freeElements(bool destroyElements) {
	TRACE("ModularNeuralNetwork::freeElements");

	NeuralNetwork::freeElements(destroyElements);

	if(destroyElements) {
		while(!mNeuronGroups.empty()) {
			NeuronGroup *g = mNeuronGroups.at(0);
			mNeuronGroups.removeAll(g);
			g->removeAllNeurons();
			if(dynamic_cast<NeuroModule*>(g) != 0) {
				dynamic_cast<NeuroModule*>(g)->removeAllSubModules();
			}
			if(g != mDefaultNeuronGroup) {
				delete g;
			}
		}
	}
	mNeuronGroups.clear();
	QList<GroupConstraint*> constraints = mDefaultNeuronGroup->getConstraints();
	mDefaultNeuronGroup->removeAllConstraints();
	mDefaultNeuronGroup->removeAllNeurons();
	if(destroyElements) {
		for(QListIterator<GroupConstraint*> i(constraints); i.hasNext();) {
			delete i.next();
		}
	}
	mNeuronGroups.append(mDefaultNeuronGroup);
}


NeuronGroup* ModularNeuralNetwork::selectNeuronGroupById(qulonglong id, 
								QList<NeuronGroup*> groups) 
{
	TRACE("ModularNeuralNetwork::selectNeuronGroupById");

	for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
		NeuronGroup *g = i.next();
		if(g->getId() == id) {
			return g;
		}
	}
	return 0;
}

NeuroModule* ModularNeuralNetwork::selectNeuroModuleById(qulonglong id, 
								QList<NeuroModule*> modules) 
{
	TRACE("ModularNeuralNetwork::selectNeuroModuleById");

	for(QListIterator<NeuroModule*> i(modules); i.hasNext();) {
		NeuroModule *m = i.next();
		if(m->getId() == id) {
			return m;
		}
	}
	return 0;
}

GroupConstraint* ModularNeuralNetwork::selectConstraintById(qulonglong id, 
								QList<GroupConstraint*> constraints) 
{
	TRACE("ModularNeuralNetwork::selectConstraintById");

	for(QListIterator<GroupConstraint*> i(constraints); i.hasNext();) {
		GroupConstraint *g = i.next();
		if(g->getId() == id) {
			return g;
		}
	}
	return 0;
}

}



