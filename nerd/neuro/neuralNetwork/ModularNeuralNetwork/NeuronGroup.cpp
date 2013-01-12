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

#include "NeuronGroup.h"
#include <QListIterator>
#include "Network/NeuralNetwork.h"
#include "Constraints/GroupConstraint.h"
#include <iostream>
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Core/Core.h"
#include "Util/Tracer.h"
#include "Util/Util.h"


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;


namespace nerd {

NeuronGroup::NeuronGroup(const QString &name, qulonglong id)
	: ParameterizedObject(name), mId(id), mOwnerNetwork(0)
{	
	TRACE("NeuronGroup::NeuronGroup");

	if(mId == 0) {
		mId = NeuralNetwork::generateNextId();
	}
}

NeuronGroup::NeuronGroup(const NeuronGroup &other) 
	: Object(), ValueChangedListener(), ParameterizedObject(other), NeuralNetworkElement(other),
	  mId(other.mId), mOwnerNetwork(0)
{
	TRACE("NeuronGroup::NeuronGroupCopy");

	//copy the neuron pointers
	mMemberNeurons << other.mMemberNeurons;

	//copy the module pointers
	for(QListIterator<NeuroModule*> i(other.mSubModules); i.hasNext();) {
		addSubModule(i.next());
	}

	for(QListIterator<GroupConstraint*> i(other.mConstraints); i.hasNext();) {
		GroupConstraint *constraint = i.next();
		GroupConstraint *copy = constraint->createCopy();
		mConstraints.append(copy);
	}
}


/**
 * Destroys all registered GroupConstraints. Thus a GroupConstraint must not be 
 * added to more then one NeuronGroup at the same time!
 */
NeuronGroup::~NeuronGroup() {
	TRACE("NeuronGroup::~NeuronGroup");

	for(QListIterator<GroupConstraint*> i(mConstraints); i.hasNext();) {
		delete i.next();
	}
}


/**
 * This method is used to set the owner network and to check whether the
 * owner network is compatible with the neuron group.
 *
 * @param network the new owner of the group.
 * @return true if the group is compatible, otherwise false.
 */
bool NeuronGroup::setOwnerNetwork(ModularNeuralNetwork *network) {
	TRACE("NeuronGroup::setOwnerNetwork");

	mOwnerNetwork = network;
	return true;
}

ModularNeuralNetwork* NeuronGroup::getOwnerNetwork() const {
	TRACE("NeuronGroup::getOwnerNetwork");

	return mOwnerNetwork;
}

NeuronGroup* NeuronGroup::createCopy() const {
	TRACE("NeuronGroup::createCopy");

	return new NeuronGroup(*this);
}


qulonglong NeuronGroup::getId() const {
	TRACE("NeuronGroup::getId");

	return mId;
}


void NeuronGroup::setId(qulonglong id) {
	TRACE("NeuronGroup::setId");

	mId = id;
}


bool NeuronGroup::addNeuron(Neuron *neuron) {
	TRACE("NeuronGroup::addNeuron");

	if(neuron == 0 || mMemberNeurons.contains(neuron)) {
		return false;
	}
	mMemberNeurons.append(neuron);
	if(mOwnerNetwork != 0) {
		mOwnerNetwork->addNeuron(neuron);
	}
	return true;
}


bool NeuronGroup::removeNeuron(Neuron *neuron) {
	TRACE("NeuronGroup::removeNeuron");

	if(neuron == 0 || !mMemberNeurons.contains(neuron)) {
		return false;
	}
	mMemberNeurons.removeAll(neuron);
	return true;
}


QList<Neuron*> NeuronGroup::getNeurons() const {
	TRACE("NeuronGroup::getNeurons");

	return mMemberNeurons;
}


void NeuronGroup::removeAllNeurons() {
	TRACE("NeuronGroup::removeAllNeurons");

	mMemberNeurons.clear();
}

bool NeuronGroup::addSubModule(NeuroModule *module) {
	TRACE("NeuronGroup::addSubModule");

	if(module == 0 || mSubModules.contains(module) || module == this) {
		return false;
	}
	mSubModules.append(module);
	if(mOwnerNetwork != 0) {
		mOwnerNetwork->addNeuronGroup(module);
	}
	return true;
}


bool NeuronGroup::removeSubModule(NeuroModule *module) {
	TRACE("NeuronGroup::removeSubModule");

	if(module == 0 || !mSubModules.contains(module)) {
		return false;
	}
	mSubModules.removeAll(module);
	return true;
}

void NeuronGroup::removeAllSubModules() {
	TRACE("NeuronGroup::removeAllSubModules");

	mSubModules.clear();
}


QList<NeuroModule*> NeuronGroup::getSubModules() const {
	TRACE("NeuronGroup::getSubModules");

	return mSubModules;
}

QList<Neuron*> NeuronGroup::getAllEnclosedNeurons() const {
	QList<Neuron*> neurons = getNeurons();
	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
		NeuroModule *module = i.next();
		Util::addWithoutDuplicates(neurons, module->getAllEnclosedNeurons());
	}
	return neurons;
}


QList<NeuroModule*> NeuronGroup::getAllEnclosedModules() const {
	TRACE("NeuronGroup::getAllEnclosedModules");

	QList<NeuroModule*> modules = mSubModules;

	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
		NeuroModule *module = i.next();
		Util::addWithoutDuplicates(modules, module->getAllEnclosedModules());
	}
	return modules;
}


/**
 * Adds a GroupConstraint to this NeuronGroup. If the constraint is already contained in 
 * this NeuronGroup or if the GroupConstraint could be found in another NeuronGroup of this
 * ModularNeuralNetwork, then false is returned and the GroupConstraint is not added.
 * 
 * @param constraint the GroupConstraint to add.
 * @return true if successful.
 */
bool NeuronGroup::addConstraint(GroupConstraint *constraint) {
	TRACE("NeuronGroup::addConstraint");

	if(constraint == 0 || mConstraints.contains(constraint)) {
		return false;
	}
	if(mOwnerNetwork != 0) {
		QList<NeuronGroup*> groups = mOwnerNetwork->getNeuronGroups();
		for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
			if(i.next()->getConstraints().contains(constraint)) {
				return false;
			}
		}
	}
	if(constraint->attachToGroup(this)) {
		mConstraints.append(constraint);
		return true;
	}
	return false;
}


bool NeuronGroup::removeConstraint(GroupConstraint *constraint) {
	TRACE("NeuronGroup::removeConstraint");

	if(constraint == 0 || !mConstraints.contains(constraint)) {
		return false;
	}
	mConstraints.removeAll(constraint);
	constraint->detachFromGroup(this);
	return true;
}

/**
 * Does not destroy the removed constraints.
 */
void NeuronGroup::removeAllConstraints() {
	TRACE("NeuronGroup::removeAllConstraints");

	for(QListIterator<GroupConstraint*> i(mConstraints); i.hasNext();) {
		i.next()->detachFromGroup(this);
	}

	mConstraints.clear();
}


QList<GroupConstraint*> NeuronGroup::getConstraints() const {
	TRACE("NeuronGroup::getConstraints");

	return mConstraints;
}


bool NeuronGroup::notifyMemberIdsChanged(QHash<qulonglong, qulonglong> changedIds) {
	bool ok = true;
	for(QListIterator<GroupConstraint*> i(mConstraints); i.hasNext();) {
		if(!i.next()->groupIdsChanged(changedIds)) {
			ok = false;
		}
	}
	return ok;
}


/**
 * Method does NOT remove the NeuronGroup from the oldNetwork (is const).
 */
bool NeuronGroup::moveGroupToNetworkConst(ModularNeuralNetwork const *oldNetwork, 
									 ModularNeuralNetwork *newNetwork)
{
	TRACE("NeuronGroup::moveGroupToNetworkConst");

	if(oldNetwork == 0 || newNetwork == 0) {
		return false;
	}
	if(mOwnerNetwork == newNetwork) {
		return true;
	}

	bool ok = true;

	//add submodules first to ensure that the neurons are adapted before the parent module is added.
	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
		if(!i.next()->moveGroupToNetworkConst(oldNetwork, newNetwork)) {
			ok = false;
		}
	}
	
	QList<Neuron*> oldGroupNeurons = mMemberNeurons;

	mMemberNeurons.clear();

	for(QListIterator<Neuron*> i(oldGroupNeurons); i.hasNext();) {
		Neuron *neuron = i.next();	

		if(neuron == 0) {
			Core::log("NeuronGroup: Warning: There was a NULL neuron in the group.");
			return false;
		}

		Neuron *newNeuron = newNetwork->getNeuronById(neuron->getId());
		
		if(newNeuron == 0) {
			Core::log(QString("NeuronGroup::moveGroupToNetwork: Could not find required neuron [")
						.append(QString::number(neuron->getId())).append("]."));
			return false;
		}

		mMemberNeurons.append(newNeuron);

		for(QListIterator<GroupConstraint*> j(mConstraints); j.hasNext();) {
			GroupConstraint *constraint = j.next();
			constraint->networkElementIdChanged(this, neuron->getId(), newNeuron->getId());
		}
	}

	newNetwork->addNeuronGroup(this);
	mOwnerNetwork = newNetwork;

	//NOTE: Validate has to be executed manually (otherwise recursive overhead is too high)
	// 	newNetwork->validateSynapseConnections();

	return ok;
}

bool NeuronGroup::moveGroupToNetwork(ModularNeuralNetwork *oldNetwork, 
										ModularNeuralNetwork *newNetwork)
{
	TRACE("NeuronGroup::moveGroupToNetwork");

	if(oldNetwork == 0 || newNetwork == 0) {
		return false;
	}
	oldNetwork->removeNeuronGroup(this);
	return moveGroupToNetworkConst(oldNetwork, newNetwork);
}


/**
 * Renews the pointers to submodules according to their id.
 */
bool NeuronGroup::verifySubModulePointers(ModularNeuralNetwork *net, QList<NeuroModule*> availableModules) {
	if(net == 0) {
		return false;
	}

	QList<NeuroModule*> oldModulePointers = mSubModules;
	mSubModules.clear();

	bool ok = true;

	for(QListIterator<NeuroModule*> i(oldModulePointers); i.hasNext();) {
		NeuroModule *oldPointer = i.next();
		NeuroModule *currentModule = 
			ModularNeuralNetwork::selectNeuroModuleById(oldPointer->getId(), availableModules);
		if(currentModule != 0) {
			mSubModules.append(currentModule);
		}
		else {
			ok = false;
		}
	}
	return ok;
}


/**
 * To be equal, the ownerNetwork does NOT has to be the same object.
 */
bool NeuronGroup::equals(NeuronGroup *group) {
	TRACE("NeuronGroup::equals");

	if(group == 0) {
		return false;
	}
	if(ParameterizedObject::equals(group) == false) {
		return false;
	}
	if(Properties::equals(group) == false) {
		return false;
	}
	if(mId != group->mId) {
		return false;
	}

	QList<Neuron*> otherNeurons = group->mMemberNeurons;

	if(mMemberNeurons.size() != otherNeurons.size()) {
		return false;
	}
	for(QListIterator<Neuron*> i(mMemberNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		Neuron *otherNeuron = NeuralNetwork::selectNeuronById(neuron->getId(), otherNeurons);
		
		if(otherNeuron == 0) {
			return false;
		}
		if(neuron->equals(otherNeuron) == false) {
			return false;
		}
	}

	QList<NeuroModule*> otherSubModules = group->mSubModules;
	if(mSubModules.size() != otherSubModules.size()) {
		return false;
	}
	for(QListIterator<NeuroModule*> i(mSubModules); i.hasNext();) {
		NeuroModule *module = i.next();
		NeuroModule *otherModule = ModularNeuralNetwork::selectNeuroModuleById(
										module->getId(), otherSubModules);

		if(otherModule == 0) {
			return false;
		}
		if(module->equals(otherModule) == false) {
			return false;
		}
	}

	QList<GroupConstraint*> otherConstraints = group->mConstraints;

	if(mConstraints.size() != otherConstraints.size()) {
		return false;
	}

	for(QListIterator<GroupConstraint*> i(mConstraints); i.hasNext();) {
		GroupConstraint *constraint = i.next();
		GroupConstraint *otherConstraint = 
				ModularNeuralNetwork::selectConstraintById(constraint->getId(), otherConstraints);
		
		if(otherConstraint == 0) {
			return false;
		}

		if(constraint->equals(otherConstraint) == false) {
			return false;
		}
	}


	return true;
}

}


