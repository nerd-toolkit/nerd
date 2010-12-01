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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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

#include "SymmetryConstraint.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <QStringList>
#include <QListIterator>
#include "Core/Core.h"
#include <iostream>
#include <QMutexLocker>
#include "Network/Neuro.h"
#include "Util/Util.h"
#include "Math/Math.h"
#include "NeuralNetworkConstants.h"

using namespace std;


namespace nerd {

const int SymmetryConstraint::FLIP_MODE_NORMAL = 0;
const int SymmetryConstraint::FLIP_MODE_HORIZONTAL = 1;
const int SymmetryConstraint::FLIP_MODE_VERTICAL = 2;
const int SymmetryConstraint::CONNECTION_MODE_INPUT = 1;
const int SymmetryConstraint::CONNECTION_MODE_OUTPUT = 2;
const int SymmetryConstraint::CONNECTION_MODE_MUTUAL = 4;
const int SymmetryConstraint::CONNECTION_MODE_INPUT_AS = 8;
const int SymmetryConstraint::CONNECTION_MODE_OUTPUT_AS = 16;
const int SymmetryConstraint::CONNECTION_MODE_MUTUAL_AS = 32;
const int SymmetryConstraint::CONNECTION_MODE_STRUCTURAL = 64;

SymmetryConstraint::SymmetryConstraint(qulonglong id)
	: GroupConstraint("Symmetry", id), mGroup1(0), mGroup2(0)
{
	mTargetGroupId = new ULongLongValue(0);
	addParameter("TargetId", mTargetGroupId);

	mNetworkElementPairValue = new StringValue("");
	addParameter("References", mNetworkElementPairValue);

	mLayoutValue = new StringValue("");
	addParameter("Layout", mLayoutValue);

	mConnectionModeValue = new StringValue("");
	addParameter("Mode", mConnectionModeValue);

}


SymmetryConstraint::SymmetryConstraint(const SymmetryConstraint &other) 
	: Object(), ValueChangedListener(), GroupConstraint(other),
	  mGroup1(0), mGroup2(0)
{
	mTargetGroupId = dynamic_cast<ULongLongValue*>(getParameter("TargetId"));
	mNetworkElementPairValue = dynamic_cast<StringValue*>(getParameter("References"));
	mLayoutValue = dynamic_cast<StringValue*>(getParameter("Layout"));
	mConnectionModeValue = dynamic_cast<StringValue*>(getParameter("Mode"));
}


SymmetryConstraint::~SymmetryConstraint() {
}


GroupConstraint* SymmetryConstraint::createCopy() const {
	return new SymmetryConstraint(*this);
}



bool SymmetryConstraint::attachToGroup(NeuronGroup *group) {
	mGroup2 = group;
	setSlaveMark(true);
	return GroupConstraint::attachToGroup(group);
}


bool SymmetryConstraint::detachFromGroup(NeuronGroup *group) {
	setSlaveMark(false);
	return GroupConstraint::detachFromGroup(group);
}

bool SymmetryConstraint::setRequiredElements(QList<NeuralNetworkElement*> elements) {

	if(elements.size() != 2) {
		mErrorMessage = QString("Too few elements set. Got ")
					.append(QString::number(elements.size()))
					.append(" but expected 2");
		return false;
	}

	NeuronGroup *group1 = dynamic_cast<NeuronGroup*>(elements.at(0));
	NeuronGroup *group2 = dynamic_cast<NeuronGroup*>(elements.at(1));
	
	if(group1 == 0 || group2 == 0) {
		mErrorMessage = "Both elements have to be NeuronGroups! [Type error]";
		return false;
	}

	mGroup1 = group1;
	mGroup2 = group2;
	
	return true;
}


bool SymmetryConstraint::isValid(NeuronGroup *owner) {

	mErrorMessage = "";
	
	ModularNeuralNetwork *net = owner->getOwnerNetwork();
	
	if(net == 0) {
		mErrorMessage = "Could not find a ModularNeuralNetwork owner of NeuronGroup.";
		return false;
	}

	mGroup1 = ModularNeuralNetwork::selectNeuronGroupById(mTargetGroupId->get(),
									 net->getNeuronGroups());
	mGroup2 = owner;

	if(mGroup1 == 0) {
		mErrorMessage = "The target group was NULL.";
		return false;
	}
	if(mGroup2 == 0) {
		mErrorMessage = QString("Could not find the owner group with id [")
						+ mTargetGroupId->getValueAsString() + "]";
		return false;
	}

	if(!updateNetworkElementPairs(mGroup1, mGroup2)) {
		return false;
	}

	return true;
}

bool SymmetryConstraint::applyConstraint(NeuronGroup *owner, CommandExecutor*,
										QList<NeuralNetworkElement*> &trashcan) 
{

	if(!isValid(owner)) {
		mErrorMessage = "Constraint was not valid!";
		return false;
	}

	ModularNeuralNetwork *net = owner->getOwnerNetwork();
	
	if(net == 0) {
		mErrorMessage = "Could not find a ModularNeuralNetwork owner of NeuronGroup.";
		return false;
	}

	mGroup1 = ModularNeuralNetwork::selectNeuronGroupById(mTargetGroupId->get(),
									 net->getNeuronGroups());
	mGroup2 = owner;

	updateFlipMode();

	if(mGroup1 == 0) {
		mErrorMessage = "The owner group was NULL.";
		return false;
	}

	
	int connectionMode = 0;
	if(mConnectionModeValue->get().contains("i", Qt::CaseInsensitive)) {
		connectionMode = connectionMode | CONNECTION_MODE_INPUT;
	}
	if(mConnectionModeValue->get().contains("I")) {
		connectionMode = connectionMode | CONNECTION_MODE_INPUT_AS;
	}
	if(mConnectionModeValue->get().contains("o", Qt::CaseInsensitive)) {
		connectionMode = connectionMode | CONNECTION_MODE_OUTPUT;
	}
	if(mConnectionModeValue->get().contains("O")) {
		connectionMode = connectionMode | CONNECTION_MODE_OUTPUT_AS;
	}
	if(mConnectionModeValue->get().contains("m", Qt::CaseInsensitive)) {
		connectionMode = connectionMode | CONNECTION_MODE_MUTUAL;
	}
	if(mConnectionModeValue->get().contains("M")) {
		connectionMode = connectionMode | CONNECTION_MODE_MUTUAL_AS;
	}
	if(mConnectionModeValue->get().contains("s", Qt::CaseInsensitive)) {
		connectionMode = connectionMode | CONNECTION_MODE_STRUCTURAL;
	}

	bool networkWasModified = false;

	//align main module sizes (if the groups are modules)
	NeuroModule *module1 = dynamic_cast<NeuroModule*>(mGroup1);
	NeuroModule *module2 = dynamic_cast<NeuroModule*>(mGroup2);

	Vector3D group1Origin;
	Vector3D group2Origin;
	if(!getGroupOrigins(group1Origin, group2Origin)) {
		mErrorMessage = "Could not determin origins of groups!";
		return false;
	}

	mModulePairs.clear();

	QList<NetworkElementPair> elementPairs;

	//check all available modules
	for(int i = 0; i < mNetworkElementPairs.size(); ++i) {
		NetworkElementPair pair = mNetworkElementPairs.at(i);

		if(pair.mOwnerElement == 0 && pair.mReferenceElement == 0) {
			continue;
		}

		if((dynamic_cast<NeuroModule*>(pair.mOwnerElement) != 0
					|| dynamic_cast<NeuroModule*>(pair.mReferenceElement) != 0))
		{

			NeuroModule *oModule = dynamic_cast<NeuroModule*>(pair.mOwnerElement);
			NeuroModule *rModule = dynamic_cast<NeuroModule*>(pair.mReferenceElement);

			if(oModule == 0) { //delete reference object
				if(net->getNeuroModules().contains(rModule)) {
					trashcan << net->savelyRemoveNetworkElement(rModule);
				}				
				pair.mOwnerElement = 0;
				pair.mReferenceElement = 0;
				pair.mOwnerId = 0;
				pair.mReferenceId = 0;

				networkWasModified = true;
			}
			else if(rModule == 0) { //create a new reference object
				rModule = new NeuroModule(oModule->getName());
				pair.mReferenceElement = rModule;
				pair.mReferenceId = rModule->getId();
				
				mModulePairs.append(pair);

				networkWasModified = true;
			}
			else if(oModule != 0 && rModule != 0) {
				mModulePairs.append(pair);
			}
			if(pair.mOwnerElement != 0 && pair.mReferenceElement != 0) {
				elementPairs.append(pair);
			}
		}
	}

	QList<NeuroModule*> allOwnerSubmodules = mGroup1->getAllEnclosedModules();
	if(module1 != 0 && module2 != 0) {
		allOwnerSubmodules.append(module1);
	}

	for(QListIterator<NetworkElementPair> i(mModulePairs); i.hasNext();) {
		NetworkElementPair pair = i.next();

		NeuroModule *oModule = dynamic_cast<NeuroModule*>(pair.mOwnerElement);
		NeuroModule *rModule = dynamic_cast<NeuroModule*>(pair.mReferenceElement);

		if(!allOwnerSubmodules.contains(oModule)) {
			continue;
		}

		if(oModule != 0 && rModule != 0) {
			if(module1 != 0 && module2 != 0 && oModule->getParentModule() == module1) {
				if(rModule->getParentModule() != module2) {
					if(rModule->getParentModule() != 0) {
						rModule->getParentModule()->removeSubModule(rModule);
					}
					module2->addSubModule(rModule);

					networkWasModified = true;
				}
			}
			else {
				if(oModule->getParentModule() == 0) {
					if(rModule->getParentModule() != 0) {
						rModule->getParentModule()->removeSubModule(rModule);

						networkWasModified = true;
					}
				}
				else {
					NeuroModule *rParent = getMatchingModule(oModule->getParentModule());
					if(rParent == 0) {
						//problem
						continue;
					}
					if(rModule->getParentModule() != rParent && rModule->getParentModule() != 0) {
						rModule->getParentModule()->removeSubModule(rModule);
						networkWasModified = true;
					}
					if(!rParent->getSubModules().contains(rModule)) {
						rParent->addSubModule(rModule);
						networkWasModified = true;
					}
				}
			}
			//check for marked (+) properties (also in structure mode)
			QList<QString> props = oModule->getPropertyNames();
			for(QListIterator<QString> kk(props); kk.hasNext();) {
				QString prop = kk.next();
				if(prop.startsWith("+")) {
					QString content = oModule->getProperty(prop);
					if(!rModule->hasProperty(prop) || rModule->getProperty(prop) != content) {
						rModule->setProperty(prop, content);
						networkWasModified = true;
					}
				}
			}
		}
		if(pair.mOwnerElement != 0 && pair.mReferenceElement != 0) {
			elementPairs.append(pair);
		}
	}
	if(module1 != 0 && module2 != 0) {
		if(setModulePositionsRecursively(module2, group1Origin, group2Origin)) {
			networkWasModified = true;
		}
	}
	else {
		QList<NeuroModule*> group2Modules = mGroup2->getSubModules();
		for(QListIterator<NeuroModule*> k(group2Modules); k.hasNext();) {
			NeuroModule *group2Module = k.next();
			if(setModulePositionsRecursively(group2Module, group1Origin, group2Origin)) {
				networkWasModified = true;
			}
		}
	}

	//update origins
	getGroupOrigins(group1Origin, group2Origin);


	//Complete other element pairs.
	for(int i = 0; i < mNetworkElementPairs.size(); ++i) {
		NetworkElementPair pair = mNetworkElementPairs.at(i);

		if(pair.mOwnerElement == 0 && pair.mReferenceElement == 0) {
			continue;
		}

		if(dynamic_cast<Neuron*>(pair.mOwnerElement) != 0
			|| dynamic_cast<Neuron*>(pair.mReferenceElement) != 0)
		{
			//update neurons
			Neuron *oNeuron = dynamic_cast<Neuron*>(pair.mOwnerElement);
			Neuron *rNeuron = dynamic_cast<Neuron*>(pair.mReferenceElement);

			if(oNeuron == 0) { //delete reference object
				if(net->getNeurons().contains(rNeuron)) {
					NeuronGroup *ownerGroup = getNeuronOwnerGroup(rNeuron, mGroup2);
					if(ownerGroup != 0) {
						ownerGroup->removeNeuron(rNeuron);
					}
					//do not delete interface neurons (here only remove neuron from its owner group).
					if(!rNeuron->hasProperty(NeuralNetworkConstants::TAG_INPUT_NEURON)
						&& !rNeuron->hasProperty(NeuralNetworkConstants::TAG_OUTPUT_NEURON))
					{
						trashcan << net->savelyRemoveNetworkElement(rNeuron);
					}
				}				
				pair.mOwnerElement = 0;
				pair.mReferenceElement = 0;
				pair.mOwnerId = 0;
				pair.mReferenceId = 0;

				networkWasModified = true;
			}
			else if(rNeuron == 0) { //create a new reference object
				rNeuron = oNeuron->createCopy();
				rNeuron->setId(NeuralNetwork::generateNextId());

				//make sure the new neuron is NOT an interface neuron
				rNeuron->removeProperty(NeuralNetworkConstants::TAG_INPUT_NEURON);
				rNeuron->removeProperty(NeuralNetworkConstants::TAG_OUTPUT_NEURON);
				pair.mReferenceElement = rNeuron;
				pair.mReferenceId = rNeuron->getId();
				net->addNeuron(rNeuron);
				NeuroModule *ownerModule = dynamic_cast<NeuroModule*>(
											getNeuronOwnerGroup(oNeuron, mGroup1));
				if(ownerModule != 0) {
					NeuroModule *refModule = getMatchingModule(ownerModule);
					if(refModule != 0) {
						refModule->addNeuron(rNeuron);
					}
				}
				else {
					mGroup2->addNeuron(rNeuron);
				}

				//set position
				setElementPosition(rNeuron, group2Origin, oNeuron->getPosition() - group1Origin);

				networkWasModified = true;
			}
			else if(oNeuron != 0 && rNeuron != 0) {

				//adjust neuron attributes if the connection mode is NOT structure only mode.
				if((connectionMode & CONNECTION_MODE_STRUCTURAL) == 0) {

					//do not symmetrize the attributes of sensor neurons, because their attributes are used to shift
					//the sensor output to a certain, non-symmetric range (template).

					//check for bias
					if(!rNeuron->hasProperty(NeuralNetworkConstants::TAG_INPUT_NEURON)) {
						if(rNeuron->getBiasValue().get() != oNeuron->getBiasValue().get()) {
							rNeuron->getBiasValue().set(oNeuron->getBiasValue().get());
							networkWasModified = true;
						}
	
						//check for transferFunction
						if(!rNeuron->getTransferFunction()->equals(oNeuron->getTransferFunction())) {
							rNeuron->setTransferFunction(*oNeuron->getTransferFunction());
							networkWasModified = true;
						}
			
						//check for activationFunction
						if(!rNeuron->getActivationFunction()->equals(oNeuron->getActivationFunction())) {
							rNeuron->setActivationFunction(*oNeuron->getActivationFunction());
							networkWasModified = true;
						}
					}
				}
				//do not check for flipped state.

				//check for marked (+) properties (also in structure mode)
				QList<QString> props = oNeuron->getPropertyNames();
				for(QListIterator<QString> kk(props); kk.hasNext();) {
					QString prop = kk.next();
					if(prop.startsWith("+")) {
						QString content = oNeuron->getProperty(prop);
						if(!rNeuron->hasProperty(prop) || rNeuron->getProperty(prop) != content) {
							rNeuron->setProperty(prop, content);
							networkWasModified = true;
						}
					}
				}
// 				//remove all marked (+) properties not in the original neuron //TODO is currently deactivated.
// 				QList<QString> rProps = rNeuron->getPropertyNames();
// 				for(QListIterator<QString> kk(rProps); kk.hasNext();) {
// 					QString prop = kk.next();
// 					if(prop.startsWith("+")) {
// 						if(!props.contains(prop)) {
// 							rNeuron->removeProperty(prop);
// 						}
// 					}
// 				}

				//check if neurons are in the correct modules / groups
				NeuroModule *ownerModule = dynamic_cast<NeuroModule*>(
									getNeuronOwnerGroup(oNeuron, mGroup1));
				NeuronGroup *refGroup = getNeuronOwnerGroup(rNeuron, mGroup2);
				if(ownerModule != 0) {
					NeuroModule *refModule = getMatchingModule(ownerModule);
					if(refModule != 0 && refGroup != refModule) {
						if(refGroup != 0) {
							refGroup->removeNeuron(rNeuron);
						}
						refModule->addNeuron(rNeuron);
						networkWasModified = true;
					}
				}
				if(setElementPosition(rNeuron, group2Origin, oNeuron->getPosition() - group1Origin)) {
					networkWasModified = true;
				}
			}
			if(pair.mOwnerElement != 0 && pair.mReferenceElement != 0) {
				elementPairs.append(pair);
			}
		}
	}

	mNetworkElementPairs = elementPairs;

	//check for synapses
	QList<Synapse*> allSynapses;
	QList<Neuron*> allNeurons = mGroup1->getAllEnclosedNeurons();
	QList<Neuron*> allRefNeurons = mGroup2->getAllEnclosedNeurons();
	{
		for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
			Util::addWithoutDuplicates(allSynapses, getSynapsesOfSynapseTarget(neuron));
			Util::addWithoutDuplicates(allSynapses, neuron->getOutgoingSynapses());
		}
	}

	QList<Synapse*> referencedSynapses;

	bool switchedInput = (connectionMode & CONNECTION_MODE_INPUT_AS) != 0;
	bool switchedOutput = (connectionMode & CONNECTION_MODE_OUTPUT_AS) != 0;
	bool switchedMutual = (connectionMode & CONNECTION_MODE_MUTUAL_AS) != 0;


	for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
		Synapse *synapse = i.next();

		
		if(allRefNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))
			&& allNeurons.contains(synapse->getSource()))
		{
			continue;
		} 
		Synapse *refSynapse = getMatchingSynapse(synapse, connectionMode);
		if(refSynapse == 0) {
			continue;
		}

		referencedSynapses.append(synapse);
		referencedSynapses.append(refSynapse);

		if((connectionMode & CONNECTION_MODE_STRUCTURAL) == 0) {
		
			if(refSynapse->getEnabledValue().get() != synapse->getEnabledValue().get()) {
				refSynapse->getEnabledValue().set(synapse->getEnabledValue().get());
				networkWasModified = true;
			}
			if(!refSynapse->getSynapseFunction()->equals(synapse->getSynapseFunction())) {
				refSynapse->setSynapseFunction(*synapse->getSynapseFunction());
				networkWasModified = true;
			}
			double strength = synapse->getStrengthValue().get();
			if(switchedInput 
				&& !allNeurons.contains(refSynapse->getSource())
				&& !allRefNeurons.contains(refSynapse->getSource())
				&& allRefNeurons.contains(dynamic_cast<Neuron*>(refSynapse->getTarget())))
			{
				strength = -strength;
			}
			if(switchedOutput
				&& !allNeurons.contains(dynamic_cast<Neuron*>(refSynapse->getTarget()))
				&& !allRefNeurons.contains(dynamic_cast<Neuron*>(refSynapse->getTarget()))
				&& allRefNeurons.contains(refSynapse->getSource()) )
			{
				strength = -strength;
			}
			if(switchedMutual
				&& ((allNeurons.contains(refSynapse->getSource()) 
						&& allRefNeurons.contains(dynamic_cast<Neuron*>(refSynapse->getTarget())))
					|| (allRefNeurons.contains(refSynapse->getSource())
						&& allNeurons.contains(dynamic_cast<Neuron*>(refSynapse->getTarget())))))
			{
				strength = -strength;
			}
			if(refSynapse->getStrengthValue().get() != strength) {
				refSynapse->getStrengthValue().set(strength);
				networkWasModified = true;
			}
		}

		//check for marked (+) properties (also in structure mode)
		QList<QString> props = synapse->getPropertyNames();
		for(QListIterator<QString> kk(props); kk.hasNext();) {
			QString prop = kk.next();
			if(prop.startsWith("+")) {
				QString content = synapse->getProperty(prop);
				if(!refSynapse->hasProperty(prop) || refSynapse->getProperty(prop) != content) {
					refSynapse->setProperty(prop, content);
					networkWasModified = true;
				}
			}
		}
		for(QListIterator<QString> kk(props); kk.hasNext();) {
			QString prop = kk.next();
			if(prop.startsWith("+")) {
				refSynapse->setProperty(prop, synapse->getProperty(prop));
			}
		}
		//remove all marked (+) properties not in the original neuron //TODO is currently deactivated.
// 		QList<QString> rProps = synapse->getPropertyNames();
// 		for(QListIterator<QString> kk(rProps); kk.hasNext();) {
// 			QString prop = kk.next();
// 			if(prop.startsWith("+")) {
// 				if(!props.contains(prop)) {
// 					synapse->removeProperty(prop);
// 				}
// 			}
// 		}

		if(allNeurons.contains(synapse->getSource()) 
			&& (allNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget())) 
					|| allSynapses.contains(dynamic_cast<Synapse*>(synapse->getTarget()))))
		{
			//align synapse position
			if(setElementPosition(refSynapse, group2Origin, synapse->getPosition() - group1Origin)) {
				networkWasModified = true;
			}
		}
		else {
			if(refSynapse->hasProperty("__new__")) {
				refSynapse->centerPosition();
			}
		}
		if(refSynapse->hasProperty("__new__")) {
			//TODO make sure that this here happens AFTER higher-order synapses are handled
			//TODO when higher order synapses are supported.
			refSynapse->setId(NeuralNetwork::generateNextId());

			refSynapse->removeProperty("__new__");
			networkWasModified = true;
		}
		
	}

	QList<Synapse*> allRefSynapses;
	{
		for(QListIterator<Neuron*> i(allRefNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
			Util::addWithoutDuplicates(allRefSynapses, getSynapsesOfSynapseTarget(neuron));
			Util::addWithoutDuplicates(allRefSynapses, neuron->getOutgoingSynapses());
		}
	}


	//remove all superfluous synapses.
	QList<Synapse*> allRefSynapsesCopy = allRefSynapses;
	while(!allRefSynapses.empty()) {
		Synapse *synapse = allRefSynapses.at(0);
		allRefSynapses.removeAll(synapse);

		if(!(connectionMode & CONNECTION_MODE_MUTUAL)) {
			Neuron *targetNeuron = dynamic_cast<Neuron*>(synapse->getTarget());
			if((mGroup1->getAllEnclosedNeurons().contains(targetNeuron)
					&& mGroup2->getAllEnclosedNeurons().contains(synapse->getSource()))
				|| (mGroup1->getAllEnclosedNeurons().contains(synapse->getSource()) 
					&& mGroup2->getAllEnclosedNeurons().contains(targetNeuron)))
			{
				//do not change anything, mutual symmetry is switched off.
				continue;
			}
		}

		if((connectionMode & CONNECTION_MODE_INPUT) == 0) {
			if(!allRefNeurons.contains(synapse->getSource())
				&& !allNeurons.contains(synapse->getSource())) 
			{
				continue;
			}
		}
		if((connectionMode & CONNECTION_MODE_OUTPUT) == 0) {
			if((!allRefNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))
				&& !allRefSynapsesCopy.contains(dynamic_cast<Synapse*>(synapse->getTarget())))
				|| allNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))
				|| allSynapses.contains(dynamic_cast<Synapse*>(synapse->getTarget())))
			{
				continue;
			}
		}
		if(!referencedSynapses.contains(synapse)) {
			trashcan << net->savelyRemoveNetworkElement(synapse);
			networkWasModified = true;
		}
	}

	updateNetworkElementPairValue();

	if((connectionMode & CONNECTION_MODE_STRUCTURAL) != 0) {
		//if structure is symmetrized only, then the elements are no slaves and can be changed.
		setSlaveMark(false); 
	}
	else {
		setSlaveMark(true);
	}

	return !networkWasModified;
}


/** 
 * Called if the group ids of controlled member elements was changed somewhere.
 * This method allows to realign the new ids with the old obsolete ids.
 */
bool SymmetryConstraint::groupIdsChanged(QHash<qulonglong, qulonglong> changedIds) {
	GroupConstraint::groupIdsChanged(changedIds);

	if(changedIds.keys().contains(mTargetGroupId->get())) {
		mTargetGroupId->set(changedIds.value(mTargetGroupId->get()));
	}
	QString newPairValue;	

	QStringList pairStrings = mNetworkElementPairValue->get().split("|");
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
	mNetworkElementPairValue->set(newPairValue);

	return true;
}

bool SymmetryConstraint::equals(GroupConstraint *constraint) {
	//TODO
	constraint = constraint; //Just to get rid of the compile warning until this method is implemented.

	return false;
}


bool SymmetryConstraint::getGroupOrigins(Vector3D &origin1, Vector3D &origin2)
{
	if(mGroup1 == 0 || mGroup2 == 0) {
		return false;
	}

	NeuroModule *module1 = dynamic_cast<NeuroModule*>(mGroup1);
	NeuroModule *module2 = dynamic_cast<NeuroModule*>(mGroup2);

	if(module1 != 0) {
		origin1 = module1->getPosition();
	}
	if(module2 != 0) {
		origin2 = module2->getPosition();
	}
	if(module1 != 0 && module2 != 0) {
		return true;
	}

	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
		NetworkElementPair pair = i.next();
		if(pair.mReferenceElement != 0 && pair.mOwnerElement != 0) {
			origin1 = pair.mOwnerElement->getPosition();
			origin2 = pair.mReferenceElement->getPosition();
			return true;
		}
	}
	return false;
}


bool SymmetryConstraint::setElementPosition(NeuralNetworkElement *element, 
							const Vector3D &origin, const Vector3D &pos)
{
	if(element == 0) {
		return false;
	}
	NeuroModule *module = dynamic_cast<NeuroModule*>(mGroup2);
	NeuroModule *moduleElement = dynamic_cast<NeuroModule*>(element);

	Vector3D newElementPosition;

	if(mFlipMode == FLIP_MODE_NORMAL) {
		newElementPosition.set(origin + pos);
	}
	else if(mFlipMode == FLIP_MODE_VERTICAL) {
		Vector3D newOrigin(origin);
		if(module != 0) {
			newOrigin.setY(newOrigin.getY() + module->getSize().height());
		}
		if(moduleElement != 0) {
			newOrigin.setY(newOrigin.getY() - moduleElement->getSize().height());
		}
		newElementPosition.set(newOrigin + Vector3D(pos.getX(), pos.getY() * -1.0, pos.getZ()));
	}
	else if(mFlipMode == FLIP_MODE_HORIZONTAL) {
		Vector3D newOrigin(origin);
		if(module != 0) {
			newOrigin.setX(newOrigin.getX() + module->getSize().width());
		}
		if(moduleElement != 0) {
			newOrigin.setX(newOrigin.getX() - moduleElement->getSize().width());
		}
		newElementPosition.set(newOrigin + Vector3D(pos.getX() * -1.0, pos.getY(), pos.getZ()));
	}
	else if(mFlipMode == (FLIP_MODE_VERTICAL | FLIP_MODE_HORIZONTAL)) {
		Vector3D newOrigin(origin);
		if(module != 0) {
			QSizeF size = module->getSize();
			newOrigin.setX(newOrigin.getX() + size.width());
			newOrigin.setY(newOrigin.getY() + size.height());
		}
		if(moduleElement != 0) {
			QSizeF size = moduleElement->getSize();
			newOrigin.setX(newOrigin.getX() - size.width());
			newOrigin.setY(newOrigin.getY() - size.height());
		}
		newElementPosition.set(newOrigin + Vector3D(pos.getX() * -1.0, 
							   pos.getY() * -1.0, pos.getZ()));
	}

	bool positionChanged = false;
	Vector3D currentPos = element->getPosition();
	if(Math::compareDoubles(currentPos.getX(), newElementPosition.getX(), 0.1) == false
		|| Math::compareDoubles(currentPos.getY(), newElementPosition.getY(), 0.1) == false
		|| Math::compareDoubles(currentPos.getZ(), newElementPosition.getZ(), 0.1) == false) 
	{
		positionChanged = true;
	}
	element->setPosition(newElementPosition);

	return positionChanged;
}


bool SymmetryConstraint::setModulePositionsRecursively(NeuroModule *module, 
					Vector3D referenceOrigin, Vector3D newOrigin) 
{
	bool positionsChanged = false;
	if(module == 0) {
		return false;
	}

	NeuroModule *reference = getMatchingModule(module);
	if(reference == 0) {
		return false;
	}
	//do not move mGroup2 
	
	QSizeF currentModuleSize = module->getSize();
	QSizeF referenceSize = reference->getSize();
	if(Math::compareDoubles(currentModuleSize.width(), referenceSize.width(), 0.1) == false
		|| Math::compareDoubles(currentModuleSize.height(), referenceSize.height(), 0.1) == false) 
	{
		positionsChanged = true;
	}
	module->setSize(referenceSize);
	if(module != mGroup2) {
		if(setElementPosition(module, newOrigin, reference->getPosition() - referenceOrigin)) {
			positionsChanged = true;
		}
	}

	QList<NeuroModule*> submodules = module->getSubModules();
	for(QListIterator<NeuroModule*> i(submodules); i.hasNext();) {
		if(setModulePositionsRecursively(i.next(), referenceOrigin, newOrigin)) {
			positionsChanged = true;
		}
	}
	return positionsChanged;
}

NeuroModule* SymmetryConstraint::getMatchingModule(NeuroModule *module) {
	if(module == 0) {
		return 0;
	}
	if(module == mGroup1) {
		return dynamic_cast<NeuroModule*>(mGroup2);
	}
	if(module == mGroup2) {
		return dynamic_cast<NeuroModule*>(mGroup1);
	}
	for(QListIterator<NetworkElementPair> i(mModulePairs); i.hasNext();) {
		NetworkElementPair pair = i.next();
		if(pair.mOwnerElement == module) {
			return dynamic_cast<NeuroModule*>(pair.mReferenceElement);
		}
		if(pair.mReferenceElement == module) {
			return dynamic_cast<NeuroModule*>(pair.mOwnerElement);
		}
	}
	//check the "normal" network element pairs (should usually not be necessary).
	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
		NetworkElementPair pair = i.next();
		if(pair.mOwnerElement == module) {
			return dynamic_cast<NeuroModule*>(pair.mReferenceElement);
		}
		if(pair.mReferenceElement == module) {
			return dynamic_cast<NeuroModule*>(pair.mOwnerElement);
		}
	}
	return 0;
}

Neuron* SymmetryConstraint::getMatchingNeuron(Neuron *neuron) {
	if(neuron == 0) {
		return 0;
	}
	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
		NetworkElementPair pair = i.next();	
		if(pair.mOwnerElement == neuron) {
			return dynamic_cast<Neuron*>(pair.mReferenceElement);
		}
		if(pair.mReferenceElement == neuron) {
			return dynamic_cast<Neuron*>(pair.mOwnerElement);
		}
	}
	return 0;
}


Synapse* SymmetryConstraint::getMatchingSynapse(Synapse *synapse, int connectionMode) {
	if(synapse == 0) {
		return 0;
	}	

	Neuron *refSource = getMatchingNeuron(synapse->getSource());
	Neuron *targetNeuron = dynamic_cast<Neuron*>(synapse->getTarget());

	//TODO support higher order synapses
	//Synapse *targetSynapse = dynamic_cast<Synapse*>(synapse->getTarget());
	

	if(!(connectionMode & CONNECTION_MODE_MUTUAL)) {
		if((mGroup1->getAllEnclosedNeurons().contains(targetNeuron) 
				&& mGroup2->getAllEnclosedNeurons().contains(synapse->getSource()))
			|| (mGroup1->getAllEnclosedNeurons().contains(synapse->getSource()) 
				&& mGroup2->getAllEnclosedNeurons().contains(targetNeuron)))
		{
			//do not change anything, mutual symmetry is switched off.
			return 0;
		}
	}

	if(refSource == 0 && (connectionMode & CONNECTION_MODE_INPUT)) {
		refSource = synapse->getSource();
	}

	if(targetNeuron != 0) {

		Neuron *refTarget = getMatchingNeuron(targetNeuron);
		if(refTarget == 0 && (connectionMode & CONNECTION_MODE_OUTPUT)) {
			refTarget = targetNeuron;
		}

		if(refTarget == 0 || refSource == 0) {
			return 0;
		}

		for(QListIterator<Synapse*> i(refTarget->getSynapses()); i.hasNext();) {
			Synapse *synapse = i.next();
			if(synapse->getTarget() == refTarget && synapse->getSource() == refSource) {
				return synapse;
			}
		}
		//could not find synapse => create a new one
		Synapse *s = synapse->createCopy();
		s->setSource(refSource);
		refTarget->addSynapse(s);
		//mark the new neuron so that it can be handled differently.
		s->setProperty("__new__");
		return s;
	}
	return 0;
}




NeuronGroup* SymmetryConstraint::getNeuronOwnerGroup(Neuron *neuron, NeuronGroup *baseGroup) {
	if(neuron == 0 || baseGroup == 0) {
		return 0;
	}
	QList<NeuroModule*> submodules = baseGroup->getSubModules();
	for(QListIterator<NeuroModule*> i(submodules); i.hasNext();) {
		NeuronGroup *ownerModule = getNeuronOwnerGroup(neuron, i.next());
		if(ownerModule != 0) {
			return ownerModule;
		}
	}
	if(baseGroup->getNeurons().contains(neuron)) {
		return baseGroup;
	}
	return 0;
}


QList<Synapse*> SymmetryConstraint::getSynapsesOfSynapseTarget(SynapseTarget *target) {
	if(target == 0) {
		return QList<Synapse*>();
	}
	QList<Synapse*> synapses;

	QList<Synapse*> incommingSynapses = target->getSynapses();
	for(QListIterator<Synapse*> i(incommingSynapses); i.hasNext();) {
		synapses << getSynapsesOfSynapseTarget(i.next());
	}
	synapses << incommingSynapses;
	return synapses;
}


void SymmetryConstraint::setSlaveMark(bool set) {
	if(mGroup2 == 0 || mGroup1 == 0) {
		return;
	}
	if(set) {
		mGroup2->setProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE);
	}
	else {
		mGroup2->removeProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE);
	}
	QList<Neuron*> allGroup1Neurons = mGroup1->getAllEnclosedNeurons();

	QList<Neuron*> allNeurons = mGroup2->getAllEnclosedNeurons();
	for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
		if(set) {
			i.next()->setProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE);
		}
		else {
			i.next()->removeProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE);
		}
	}

	QList<Synapse*> allSynapses;
	{
		for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
			QList<Synapse*> synapses = getSynapsesOfSynapseTarget(neuron);
			for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
				Synapse *synapse = j.next();
				if(allGroup1Neurons.contains(synapse->getSource())) {
					continue;
				}
				if(!allNeurons.contains(synapse->getSource())
					|| !allNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget())))
				{
					continue;
				}
				if(set) {
					synapse->setProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE);
				}
				else {
					synapse->removeProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE);
				}
			}
		}
	}
	

// 	NeuroModule *module = dynamic_cast<NeuroModule*>(mGroup2);
// 	if(module != 0) {
// 		QList<NeuroModule*> allSubmodules = module->getAllEnclosedModules();
		QList<NeuroModule*> allSubmodules = mGroup2->getAllEnclosedModules();
		for(QListIterator<NeuroModule*> i(allSubmodules); i.hasNext();) {
			if(set) {
				i.next()->setProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE);
			}
			else {
				i.next()->removeProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE);
			}
		}
// 	}
}


void SymmetryConstraint::updateFlipMode() {
	mFlipMode = FLIP_MODE_NORMAL;
	if(mLayoutValue->get().toLower().contains("h")) {
		mFlipMode = mFlipMode | FLIP_MODE_HORIZONTAL;
	}
	if(mLayoutValue->get().toLower().contains("v")) {
		mFlipMode = mFlipMode | FLIP_MODE_VERTICAL;
	}
}


void SymmetryConstraint::updateNetworkElementPairValue() {

	QString pairList;
	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
		NetworkElementPair pair = i.next();
		pairList.append(QString::number(pair.mReferenceId) + "," + QString::number(pair.mOwnerId));
		if(i.hasNext()) {
			pairList.append("|");
		}
	}

	mNetworkElementPairValue->set(pairList);
}

bool SymmetryConstraint::updateNetworkElementPairs(NeuronGroup *owner, NeuronGroup *target) {

	mNetworkElementPairs.clear();

	if(owner == 0 || target == 0) {
		mNetworkElementPairValue->set("");
		mErrorMessage = "Owner or reference group was NULL.";
		return false;
	}

	ModularNeuralNetwork *net = owner->getOwnerNetwork();
	
	if(net == 0) {
		mNetworkElementPairValue->set("");
		mErrorMessage = "Could not find a suitable network.";
		return false;
	}

	QList<Neuron*> neurons = net->getNeurons();
	QList<Neuron*> ownerNeurons = owner->getAllEnclosedNeurons();
	QList<Neuron*> refNeurons = target->getAllEnclosedNeurons();

// 	cerr << "got: owner " << ownerNeurons.size() << " and ref " << refNeurons.size() << endl;
// 	cerr << "Owner is: " << owner << " " << owner->getId() << " and ref is " << target << " " << target->getId() << endl;

	QList<NeuroModule*> ownerModules = owner->getAllEnclosedModules();
	QList<NeuroModule*> refModules = target->getAllEnclosedModules();

// 	cerr << "mods: owner: " << ownerModules.size() << " and ref " << refModules.size() << endl;
	
	//process pair list.
// 	NeuroModule *ownerModule = dynamic_cast<NeuroModule*>(owner);
// 	NeuroModule *refModule = dynamic_cast<NeuroModule*>(target);

	QList<qulonglong> consideredObjectOwnerIds;
	QList<qulonglong> consideredObjectRefIds;

	QStringList pairStrings; // = mNetworkElementPairValue->get().split("|");

	//collect pair strings of all modules and groups that contain this group (module).
	QList<NeuronGroup*> allGroups = net->getNeuronGroups();
	for(QListIterator<NeuronGroup*> i(allGroups); i.hasNext();) {
		NeuronGroup *group = i.next();
		QList<GroupConstraint*> constraints = group->getConstraints();
		for(QListIterator<GroupConstraint*> j(constraints); j.hasNext();) {
			GroupConstraint *constraint = j.next();
			SymmetryConstraint *sc = dynamic_cast<SymmetryConstraint*>(constraint);
			if(sc != 0) {
				pairStrings << sc->getNetworkElementPairValue()->get().split("|");

				//make sure that the group pairs are also in the list.
				QString groupRef =  QString::number(group->getId()) + ","
									+ sc->getTargetGroupIdValue()->getValueAsString();
				pairStrings.append(groupRef);
			}
		}
	}

	for(QListIterator<QString> i(pairStrings); i.hasNext();) {
		QString pairString = i.next();
		QStringList pair = pairString.split(",");

// 		Core::log("Checking " + pairString, true);

		if(pair.size() != 2) {
			continue;
		}
		bool ok = true;
		qulonglong ownerId = pair.at(1).toULongLong(&ok);
		if(!ok) {
			continue;
		}
		qulonglong referenceId = pair.at(0).toULongLong(&ok);
		if(!ok) {
			continue;
		}

		if(consideredObjectOwnerIds.contains(ownerId) || consideredObjectRefIds.contains(referenceId)) {
			continue;
		}

		NetworkElementPair p;

		NeuralNetworkElement *source = 0;
		NeuralNetworkElement *target = 0;
		for(int r = 0; r < 2; ++r) {

			source = ModularNeuralNetwork::selectNeuroModuleById(ownerId, ownerModules);
			target = ModularNeuralNetwork::selectNeuroModuleById(referenceId, refModules);

			if(source == 0 || target == 0) {
				source = ModularNeuralNetwork::selectNeuronById(ownerId, ownerNeurons);
				target = ModularNeuralNetwork::selectNeuronById(referenceId, refNeurons);
			}
			if(source != 0 && target != 0) {
				p.mOwnerId = ownerId;
				p.mReferenceId = referenceId;
			}
			else {
				source = ModularNeuralNetwork::selectNeuroModuleById(referenceId, refModules);
				target = ModularNeuralNetwork::selectNeuroModuleById(ownerId, ownerModules);
				if(source == 0 || target == 0) {
					source = ModularNeuralNetwork::selectNeuronById(referenceId, refNeurons);
					target = ModularNeuralNetwork::selectNeuronById(ownerId, ownerNeurons);
				}
				if(source != 0 && target != 0) {
					p.mOwnerId = referenceId;
					p.mReferenceId = ownerId;
				}
			}

// 			cerr << "Result: " << source << " and " << target << endl;

			if(source != 0 && target != 0) {
				break;
			}

			//exchange target and source and go again
			qulonglong oldRef = referenceId;
			referenceId = ownerId;
			ownerId = oldRef;
		}
		if(source == 0 || target == 0) {
// 			cerr << "no match found" << endl;
			continue;
		}
		consideredObjectOwnerIds.append(ownerId);
		consideredObjectRefIds.append(referenceId);

		//make sure the pairs are always from owner to target.
// 		if(ModularNeuralNetwork::selectNeuroModuleById(ownerId, ownerModules) == 0
// 			&& ModularNeuralNetwork::selectNeuronById(ownerId, ownerNeurons) == 0)
// 		{
// 			p.mOwnerId = referenceId;
// 			p.mReferenceId = ownerId;
// 		}
// 		else {
// 			p.mOwnerId = ownerId;
// 			p.mReferenceId = referenceId;
// 		}
		mNetworkElementPairs.append(p);
	}
	
	QList<NetworkElementPair> validPairs;

	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
		NetworkElementPair pair = i.next();
		Neuron *ownerNeuron = ModularNeuralNetwork::selectNeuronById(pair.mOwnerId, ownerNeurons);
		Neuron *refNeuron = ModularNeuralNetwork::selectNeuronById(pair.mReferenceId, refNeurons);

		if(!(ownerNeuron == 0 && refNeuron == 0)) {
			pair.mOwnerElement = ownerNeuron;
			pair.mReferenceElement = refNeuron;
			validPairs.append(pair);
			ownerNeurons.removeAll(ownerNeuron);
			refNeurons.removeAll(refNeuron);
		}
	}

	for(QListIterator<Neuron*> i(ownerNeurons); i.hasNext();) {
		Neuron *neuron = i.next();

		if(neuron == 0) {
			continue;
		}

		NetworkElementPair pair;
		pair.mOwnerId = neuron->getId();
		pair.mOwnerElement = neuron;

		validPairs.append(pair);
	}

	for(QListIterator<Neuron*> i(refNeurons); i.hasNext();) {
		Neuron *neuron = i.next();

		if(neuron == 0) {
			continue;
		}

		NetworkElementPair pair;
		pair.mReferenceId = neuron->getId();
		pair.mReferenceElement = neuron;

		validPairs.append(pair);
	}

	if(owner != 0 && target != 0) {
		for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
			NetworkElementPair pair = i.next();
			NeuroModule *oModule = ModularNeuralNetwork::selectNeuroModuleById(
						pair.mOwnerId, ownerModules);
			NeuroModule *rModule = ModularNeuralNetwork::selectNeuroModuleById(
						pair.mReferenceId, refModules);

			if(!(oModule == 0 && rModule == 0)) {
				pair.mOwnerElement = oModule;
				pair.mReferenceElement = rModule;
				validPairs.append(pair);
				ownerModules.removeAll(oModule);
				refModules.removeAll(rModule);
			}
		}


		for(QListIterator<NeuroModule*> i(ownerModules); i.hasNext();) {
			NeuroModule *module = i.next();
			if(module == 0) {
				continue;
			}
			NetworkElementPair pair;
			pair.mOwnerId = module->getId();
			pair.mOwnerElement = module;

			validPairs.append(pair);
		}

		for(QListIterator<NeuroModule*> i(refModules); i.hasNext();) {
			NeuroModule *module = i.next();
			if(module == 0) {
				continue;
			}
			NetworkElementPair pair;
			pair.mReferenceId = module->getId();
			pair.mReferenceElement = module;

			validPairs.append(pair);
		}
	}

	mNetworkElementPairs = validPairs;

// 	for(QListIterator<NetworkElementPair> i(validPairs); i.hasNext();) {
// 		NetworkElementPair pair = i.next();
// 		cerr << "Pair: " << pair.mOwnerId << " , " << pair.mReferenceId << endl;
// 	}

	return true;
}


QList<NetworkElementPair> SymmetryConstraint::getNetworkElementPairs() const {
	return mNetworkElementPairs;
}

StringValue* SymmetryConstraint::getNetworkElementPairValue() const {
	return mNetworkElementPairValue;
}

ULongLongValue* SymmetryConstraint::getTargetGroupIdValue() const {
	return mTargetGroupId;
}


}


