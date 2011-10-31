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



#include "ConnectionSymmetryConstraint.h"
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
#include "Math/Random.h"
#include "Constraints/ConstraintManager.h"

using namespace std;

namespace nerd {


const int ConnectionSymmetryConstraint::CONNECTION_MODE_NORMAL = 1;
const int ConnectionSymmetryConstraint::CONNECTION_MODE_INPUT = 2;
const int ConnectionSymmetryConstraint::CONNECTION_MODE_OUTPUT = 4;
const int ConnectionSymmetryConstraint::CONNECTION_MODE_MUTUAL = 8;
const int ConnectionSymmetryConstraint::CONNECTION_MODE_ASYNC = 16;
const int ConnectionSymmetryConstraint::CONNECTION_MODE_RANDOM = 32;
const int ConnectionSymmetryConstraint::CONNECTION_MODE_STRUCTURAL = 64;

/**
 * Constructs a new ConnectionSymmetryConstraint.
 */
ConnectionSymmetryConstraint::ConnectionSymmetryConstraint(qulonglong id)
	: GroupConstraint("ConnectionSymmetry", id), mGroup1(0), mGroup2(0)
{
	mTargetGroupId = new ULongLongValue(0);
	addParameter("TargetId", mTargetGroupId);

	mNetworkElementPairValue = new StringValue("");
	addParameter("References", mNetworkElementPairValue);

	mConnectionModeValue = new StringValue("");
	addParameter("Mode", mConnectionModeValue);

	mAutoSelectPairs = new BoolValue(false);
	addParameter("AutoSelectPairs", mAutoSelectPairs);
}


/**
 * Copy constructor. 
 * 
 * @param other the ConnectionSymmetryConstraint object to copy.
 */
ConnectionSymmetryConstraint::ConnectionSymmetryConstraint(const ConnectionSymmetryConstraint &other)
	: Object(), ValueChangedListener(), GroupConstraint(other),
	  mGroup1(0), mGroup2(0)
{
	mTargetGroupId = dynamic_cast<ULongLongValue*>(getParameter("TargetId"));
	mNetworkElementPairValue = dynamic_cast<StringValue*>(getParameter("References"));
	mConnectionModeValue = dynamic_cast<StringValue*>(getParameter("Mode"));
	mAutoSelectPairs = dynamic_cast<BoolValue*>(getParameter("AutoSelectPairs"));
}

/**
 * Destructor.
 */
ConnectionSymmetryConstraint::~ConnectionSymmetryConstraint() {
}


GroupConstraint* ConnectionSymmetryConstraint::createCopy() const {
	return new ConnectionSymmetryConstraint(*this);
}


bool ConnectionSymmetryConstraint::attachToGroup(NeuronGroup *group) {
	mGroup2 = group;
	return GroupConstraint::attachToGroup(group);
}

bool ConnectionSymmetryConstraint::detachFromGroup(NeuronGroup *group) {
	setSlaveMark(false);
	return GroupConstraint::detachFromGroup(group);
}


bool ConnectionSymmetryConstraint::isValid(NeuronGroup *owner) {
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

	if(!updateNetworkElementPairs(mGroup2, mGroup1)) {
		return false;
	}

	return true;
}


bool ConnectionSymmetryConstraint::applyConstraint(NeuronGroup *owner, CommandExecutor*,
									 QList<NeuralNetworkElement*> &trashcan)
{

	if(!isValid(owner)) {
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

	if(mGroup1 == 0) {
		mErrorMessage = "The owner group was NULL.";
		return false;
	}

	
	int inputConnectionMode = 0;
	int outputConnectionMode = 0;
	int mutualConnectionMode = 0;
	if(mConnectionModeValue->get().contains("i", Qt::CaseInsensitive)) {
		inputConnectionMode = inputConnectionMode | CONNECTION_MODE_INPUT;
	}
	if(mConnectionModeValue->get().contains("I")) {
		inputConnectionMode = inputConnectionMode | CONNECTION_MODE_ASYNC;
	}
	if(mConnectionModeValue->get().toLower().contains("is")) {
		inputConnectionMode = inputConnectionMode | CONNECTION_MODE_STRUCTURAL;
	}
	if(mConnectionModeValue->get().toLower().contains("ir")) {
		inputConnectionMode = inputConnectionMode | CONNECTION_MODE_RANDOM;
	}
	if(mConnectionModeValue->get().contains("o", Qt::CaseInsensitive)) {
		outputConnectionMode = outputConnectionMode | CONNECTION_MODE_OUTPUT;
	}
	if(mConnectionModeValue->get().contains("O")) {
		outputConnectionMode = outputConnectionMode | CONNECTION_MODE_ASYNC;
	}
	if(mConnectionModeValue->get().toLower().contains("os")) {
		outputConnectionMode = outputConnectionMode | CONNECTION_MODE_STRUCTURAL;
	}
	if(mConnectionModeValue->get().toLower().contains("or")) {
		outputConnectionMode = outputConnectionMode | CONNECTION_MODE_RANDOM;
	}
	if(mConnectionModeValue->get().contains("m", Qt::CaseInsensitive)) {
		mutualConnectionMode = mutualConnectionMode | CONNECTION_MODE_MUTUAL;
	}
	if(mConnectionModeValue->get().contains("M")) {
		mutualConnectionMode = mutualConnectionMode | CONNECTION_MODE_ASYNC;
	}
	if(mConnectionModeValue->get().toLower().contains("ms")) {
		mutualConnectionMode = mutualConnectionMode | CONNECTION_MODE_STRUCTURAL;
	}
	if(mConnectionModeValue->get().toLower().contains("mr")) {
		mutualConnectionMode = mutualConnectionMode | CONNECTION_MODE_RANDOM;
	}

	bool networkWasModified = false;
	mModulePairs.clear();
	QList<NetworkElementPair> elementPairs;


	QList<Neuron*> ownerNeurons;
	QList<Neuron*> referenceNeurons;

	//Check for validity of element pairs (and autocomplete or remove invalid pairs.
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
				//TODO autocomplete pairs
			}
			else if(rNeuron == 0) { //create a new reference object
				//TODO autocomplete pairs
			}
			else if(oNeuron != 0 && rNeuron != 0) {
				ownerNeurons.append(oNeuron);
				referenceNeurons.append(rNeuron);
// 				cerr << "Added to owner: " << oNeuron->getId() << endl;
// 				cerr << "Added to ref  : " << rNeuron->getId() << endl;
				//nothing to do?
			}

			if(pair.mOwnerElement != 0 && pair.mReferenceElement != 0) {
				elementPairs.append(pair);
			}
		}
	}

	//Check for synapses and add missing synapses / remove superflous ones.
	QList<Synapse*> allSynapses;
	{
		for(QListIterator<Neuron*> i(referenceNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
			Util::addWithoutDuplicates(allSynapses, getSynapsesOfSynapseTarget(neuron));
			Util::addWithoutDuplicates(allSynapses, neuron->getOutgoingSynapses());
		}
	}
	QList<Neuron*> allOwnerNeurons = mGroup2->getAllEnclosedNeurons();
	QList<Neuron*> allReferenceNeurons = mGroup1->getAllEnclosedNeurons();

	QList<Synapse*> referencedSynapses;

	for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
		Synapse *synapse = i.next();

// 		cerr << "Check synapse " << synapse->getId() << endl;

// 		Neuron *sourceN = synapse->getSource();
// 		Neuron *targetN = dynamic_cast<Neuron*>(synapse->getTarget());
// 		cerr << "Connected: " << sourceN->getId() << " to " << targetN->getId() << endl;

// 		if(referenceNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))
// 			&& ownerNeurons.contains(synapse->getSource()))
// 		{
// 			continue;
// 		} 

		
		//determin what kind of connection this synapse represents
		//(and which connectionMode should be used for this connection)
		int synapseMode = 0;
// 		cerr << "*******************MODE*******************" << endl;

		bool mutualMode = false;
		bool outputMode = false;
		bool inputMode = false;
		if((referenceNeurons.contains(synapse->getSource())
				&& referenceNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget())))
			|| (ownerNeurons.contains(synapse->getSource())
				&& ownerNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))))
		{
// 			cerr << "normal mode" << endl;
			synapseMode = CONNECTION_MODE_NORMAL;
		}	
		else if((referenceNeurons.contains(synapse->getSource())
				&& ownerNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget())))
			|| (referenceNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))
				&& ownerNeurons.contains(synapse->getSource())))
		{
// 			cerr << "mutual mode" << endl;
			mutualMode = true;
			synapseMode = mutualConnectionMode;
		}	
		else if(referenceNeurons.contains(synapse->getSource())
			&& !ownerNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget())))
		{
// 			cerr << "output mode" << endl;
			outputMode = true;
			synapseMode = outputConnectionMode;
		}
		else if(!ownerNeurons.contains(synapse->getSource())
			&& referenceNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget())))
		{
// 			cerr << "input mode" << endl;
			inputMode = true;
			synapseMode = inputConnectionMode;
		}


		Synapse *refSynapse = getMatchingSynapse(synapse, synapseMode);

		if(refSynapse == 0) {
// 			cerr << "No synapse found and created." << endl;
			continue;
		}

		referencedSynapses.append(synapse);
		referencedSynapses.append(refSynapse);

		//no changes if the stucture is symmetrized only.
		if((synapseMode & CONNECTION_MODE_STRUCTURAL) == 0) {
			if(refSynapse->getEnabledValue().get() != synapse->getEnabledValue().get()) {
				refSynapse->getEnabledValue().set(synapse->getEnabledValue().get());
				networkWasModified = true;
			}
			if(!refSynapse->getSynapseFunction()->equals(synapse->getSynapseFunction())) {
				refSynapse->setSynapseFunction(*synapse->getSynapseFunction());
				networkWasModified = true;
			}
			double strength = synapse->getStrengthValue().get();
			if((synapseMode & CONNECTION_MODE_ASYNC) != 0) {
				strength = -strength;
			}
			if((synapseMode & CONNECTION_MODE_RANDOM) != 0) {
				if(refSynapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_SOURCE)) {
					//TODO allow changes of flip when sign of synapse was changed.
				}
				else {
					bool flip = Random::nextDouble() < 0.5 ? true : false;
					if(flip) {
						refSynapse->setProperty(
								NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_SOURCE);
					}
				}
				if(refSynapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_SOURCE)) {
					strength = -1 * synapse->getStrengthValue().get();
				}
			}
			if(refSynapse->getStrengthValue().get() != strength) {
				refSynapse->getStrengthValue().set(strength);
				networkWasModified = true;
			}
		}
	
		//center __new__ synapses, give new ids and remove marker property.
		if(refSynapse->hasProperty("__new__")) {
			refSynapse->centerPosition();
		}

		if(refSynapse->hasProperty("__new__")) {
			//TODO make sure that this here happens AFTER higher-order synapses are handled
			//TODO when higher order synapses are supported.

			refSynapse->setId(NeuralNetwork::generateNextId());
			refSynapse->removeProperty("__new__");
			networkWasModified = true;
		}
		
		ConstraintManager::markElementAsConstrained(refSynapse, "E");
		if((synapseMode & CONNECTION_MODE_STRUCTURAL) == 0) {
			ConstraintManager::markElementAsConstrained(refSynapse, "W");
			ConstraintManager::markElementAsConstrained(refSynapse, "S");
		}
	}

	QList<Synapse*> allRefSynapses;
	{
		for(QListIterator<Neuron*> i(ownerNeurons); i.hasNext();) {
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

		bool mutualMode = false;
		bool inputMode = false;
		bool outputMode = false;
		int connectionMode = 0;
		if((referenceNeurons.contains(synapse->getSource())
				&& referenceNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget())))
			|| (ownerNeurons.contains(synapse->getSource())
				&& ownerNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))))
		{
			cerr << "R-normal" << endl;
			connectionMode = CONNECTION_MODE_NORMAL;
		}	
		else if((referenceNeurons.contains(synapse->getSource()) 
				&& ownerNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget())))
			|| (referenceNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))
				&& ownerNeurons.contains(synapse->getSource())))
		{
			cerr << "R-mutual" << endl;
			connectionMode = mutualConnectionMode;
			mutualMode = true;
		}	
		else if((ownerNeurons.contains(synapse->getSource()) 
				|| referenceNeurons.contains(synapse->getSource()))
			&& !referenceNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))
			&& !ownerNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget())))
		{
			cerr << "R-output" << endl;
			connectionMode = outputConnectionMode;
			outputMode = true;
		}
		else if((ownerNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))
				|| referenceNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget())))
			&& !referenceNeurons.contains(synapse->getSource())
			&& !ownerNeurons.contains(synapse->getSource()))
		{
			cerr << "R-input" << endl;
			connectionMode = inputConnectionMode;
			inputMode = true;
		}

		if(mutualMode) {
			if(!(connectionMode & CONNECTION_MODE_MUTUAL)) {
// 				Neuron *targetNeuron = dynamic_cast<Neuron*>(synapse->getTarget());
// 				if((mGroup1->getAllEnclosedNeurons().contains(targetNeuron)
// 						&& mGroup2->getAllEnclosedNeurons().contains(synapse->getSource()))
// 					|| (mGroup1->getAllEnclosedNeurons().contains(synapse->getSource()) 
// 						&& mGroup2->getAllEnclosedNeurons().contains(targetNeuron)))
// 				{
					//do not change anything, mutual symmetry is switched off.
					continue;
// 				}
			}
		}

		if(inputMode) {
			cerr << "input mode " << endl;
			if((connectionMode & CONNECTION_MODE_INPUT) == 0) {
				cerr << "not input mode" << endl;
// 				if(!allReferenceNeurons.contains(synapse->getSource())
// 					&& !allOwnerNeurons.contains(synapse->getSource())) 
// 				{
					continue;
// 				}
			}
		}

		if(outputMode) {
			if((connectionMode & CONNECTION_MODE_OUTPUT) == 0) {
// 				if((!allReferenceNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))
// 					&& !allRefSynapsesCopy.contains(dynamic_cast<Synapse*>(synapse->getTarget())))
// 					|| allOwnerNeurons.contains(dynamic_cast<Neuron*>(synapse->getTarget()))
// 					|| allSynapses.contains(dynamic_cast<Synapse*>(synapse->getTarget())))
// 				{
					continue;
// 				}
			}
		}

		//remove all non-matching synapses.
		if(!referencedSynapses.contains(synapse)) {
			trashcan << net->savelyRemoveNetworkElement(synapse);
			networkWasModified = true;
		}
	}

	mNetworkElementPairs = elementPairs;

	updateNetworkElementPairValue();
	return !networkWasModified;
}

bool ConnectionSymmetryConstraint::groupIdsChanged(QHash<qulonglong, qulonglong> changedIds) {
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

		
bool ConnectionSymmetryConstraint::equals(GroupConstraint *constraint) {
	constraint = constraint; //just to get rid of the compiler warning.
	//TODO
	return false;
}

// Neuron* ConnectionSymmetryConstraint::getMatchingNeuron(Neuron *neuron) {
// 	if(neuron == 0) {
// 		return 0;
// 	}
// 	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
// 		NetworkElementPair pair = i.next();	
// 		if(pair.mOwnerElement == neuron) {
// 			return dynamic_cast<Neuron*>(pair.mReferenceElement);
// 		}
// // 		if(pair.mReferenceElement == neuron) {
// // 			return dynamic_cast<Neuron*>(pair.mOwnerElement);
// // 		}
// 	}
// 	return 0;
// }


Neuron* ConnectionSymmetryConstraint::getMatchingOwnerNeuron(Neuron *neuron) {
	if(neuron == 0) {
		return 0;
	}
	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
		NetworkElementPair pair = i.next();	
// 		if(pair.mOwnerElement == neuron) {
// 			return dynamic_cast<Neuron*>(pair.mReferenceElement);
// 		}
		if(pair.mReferenceElement == neuron) {
			return dynamic_cast<Neuron*>(pair.mOwnerElement);
		}
	}
	return 0;
}


Neuron* ConnectionSymmetryConstraint::getMatchingReferenceNeuron(Neuron *neuron) {
	if(neuron == 0) {
		return 0;
	}
	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
		NetworkElementPair pair = i.next();	
		if(pair.mOwnerElement == neuron) {
			return dynamic_cast<Neuron*>(pair.mReferenceElement);
		}
// 		if(pair.mReferenceElement == neuron) {
// 			return dynamic_cast<Neuron*>(pair.mOwnerElement);
// 		}
	}
	return 0;
}


Synapse* ConnectionSymmetryConstraint::getMatchingSynapse(Synapse *synapse, int connectionMode) 
{
	if(synapse == 0) {
		return 0;
	}

// 	cerr << "Check for Synapse ---------------------------------" << endl;

	QList<Neuron*> allGroup1Neurons = mGroup1->getAllEnclosedNeurons();
	QList<Neuron*> allGroup2Neurons = mGroup2->getAllEnclosedNeurons();

// 	if(synapse->getSource() != 0) {
// 		cerr << "OrigSource: " << synapse->getSource()->getId() << endl;
// 	}

	Neuron *synapseSourceNeuron = synapse->getSource();
	Neuron *synapseTargetNeuron = dynamic_cast<Neuron*>(synapse->getTarget());

	if(allGroup2Neurons.contains(synapseTargetNeuron) 
		&& allGroup2Neurons.contains(synapseSourceNeuron)
		&& allGroup1Neurons.contains(synapseTargetNeuron) 
		&& allGroup1Neurons.contains(synapseSourceNeuron))
	{
		//check special case if this synapse is valid.
		//this is only the case when a synapse present in both groups also has a valid
		//synapse in the reference group.

		bool foundSynapse = false;
		Neuron *sourceRef = 0;
		Neuron *targetRef = 0;

		for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
			NetworkElementPair pair = i.next();

			if(pair.mOwnerElement == synapseSourceNeuron) {
				sourceRef = dynamic_cast<Neuron*>(pair.mReferenceElement);
			}
			if(pair.mOwnerElement == synapseTargetNeuron) {
				targetRef = dynamic_cast<Neuron*>(pair.mReferenceElement);
			}
			if(sourceRef != 0 && targetRef != 0) {

				//check if there is a synapse from sourceRef to targetRef
				ModularNeuralNetwork *net = mGroup2->getOwnerNetwork();
				if(net != 0) {
					QList<Synapse*> synapses = net->getSynapses();
					for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
						Synapse *synapse = j.next();
						if(synapse->getSource() == sourceRef 
							&& synapse->getTarget() == targetRef) 
						{
							foundSynapse = true;
							break;
						}
					}
				}
				break;
			}
		}
		if(!foundSynapse) {
			return 0;
		}
	}


	Neuron *sourceNeuron = getMatchingOwnerNeuron(synapseSourceNeuron);
	Neuron *targetNeuron = synapseTargetNeuron;
	//TODO support higher order synapses
	//Synapse *targetSynapse = dynamic_cast<Synapse*>(synapse->getTarget());

	//if not mutual mode
	if((connectionMode & CONNECTION_MODE_MUTUAL) == 0) {
		if((allGroup1Neurons.contains(targetNeuron) 
				&& !allGroup2Neurons.contains(targetNeuron) //
				&& !allGroup1Neurons.contains(synapse->getSource())
				&& allGroup2Neurons.contains(synapse->getSource()))
			|| (allGroup1Neurons.contains(synapse->getSource())
				&& allGroup2Neurons.contains(synapse->getSource()) //
				&& !allGroup1Neurons.contains(targetNeuron) 
				&& allGroup2Neurons.contains(targetNeuron)))
		{
			//do not change anything, mutual symmetry is switched off.
			return 0;
		}
	}
	else { //if mutual mode is active
		if(sourceNeuron == 0) {
			sourceNeuron = getMatchingReferenceNeuron(synapse->getSource());
		}
	}

	if(sourceNeuron == 0 && (connectionMode & CONNECTION_MODE_INPUT)) {
		//if input mode is active, then complete missing source by the same source as the t
		sourceNeuron = synapse->getSource();
	}

	if(targetNeuron != 0) {

		Neuron *refTarget = getMatchingOwnerNeuron(targetNeuron);
		if(refTarget == 0 && (connectionMode & CONNECTION_MODE_OUTPUT)) {
			refTarget = targetNeuron;
		}

		if(refTarget == 0 || sourceNeuron == 0) {
			return 0;
		}

		for(QListIterator<Synapse*> i(refTarget->getSynapses()); i.hasNext();) {
			Synapse *synapse = i.next();
			if(synapse->getTarget() == refTarget && synapse->getSource() == sourceNeuron) {
				return synapse;
			}
		}

		//could not find synapse => create a new one
		Synapse *s = synapse->createCopy();
		s->setSource(sourceNeuron);
		refTarget->addSynapse(s);
		//mark the new neuron so that it can be handled differently.
		s->setProperty("__new__");
		return s;
	}

	return 0;
}


QList<Synapse*> ConnectionSymmetryConstraint::getSynapsesOfSynapseTarget(SynapseTarget *target) {
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


void ConnectionSymmetryConstraint::setSlaveMark(bool set) {
	//TODO check if neccessary (i.e. does it save computation time?)
	set = set; //just to get rid of the compiler warning.
}


void ConnectionSymmetryConstraint::updateNetworkElementPairValue() {
	QString pairList;
	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
		NetworkElementPair pair = i.next();
		pairList.append(QString::number(pair.mOwnerId) + "," + QString::number(pair.mReferenceId));
		if(i.hasNext()) {
			pairList.append("|");
		}
	}

	mNetworkElementPairValue->set(pairList);
}


bool ConnectionSymmetryConstraint::updateNetworkElementPairs(NeuronGroup *owner, NeuronGroup *target) {
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

	QList<qulonglong> consideredObjectOwnerIds;
	QList<qulonglong> consideredObjectRefIds;

	QStringList pairStrings = mNetworkElementPairValue->get().split("|");

	for(QListIterator<QString> i(pairStrings); i.hasNext();) {
		QString pairString = i.next();
		QStringList pair = pairString.split(",");

// 		cerr << "Check: " << pairString.toStdString().c_str() << endl;

		if(pair.size() != 2) {
			continue;
		}
		bool ok = true;
		qulonglong ownerId = pair.at(0).toULongLong(&ok);
		if(!ok) {
			continue;
		}
		qulonglong referenceId = pair.at(1).toULongLong(&ok);
		if(!ok) {
			continue;
		}

// 		cerr << "Got: " << ownerId << " and " << referenceId << endl;

		if(consideredObjectOwnerIds.contains(ownerId) || consideredObjectRefIds.contains(referenceId)) {
			continue;
		}

// 		cerr << "hier" << endl;

		NetworkElementPair p;

		NeuralNetworkElement *oElem = 0;
		NeuralNetworkElement *rElem = 0;
// 		for(int r = 0; r < 1; ++r) {

			oElem = ModularNeuralNetwork::selectNeuronById(ownerId, ownerNeurons);
			rElem = ModularNeuralNetwork::selectNeuronById(referenceId, refNeurons);

// 			cerr << "Found: " << oElem << " and " << rElem << endl;

			if(oElem != 0 && rElem != 0) {
				p.mOwnerId = ownerId;
				p.mReferenceId = referenceId;
			}
// 			else {
// 				source = ModularNeuralNetwork::selectNeuronById(referenceId, refNeurons);
// 				target = ModularNeuralNetwork::selectNeuronById(ownerId, ownerNeurons);
// 
// 				if(source != 0 && target != 0) {
// 					p.mOwnerId = referenceId;
// 					p.mReferenceId = ownerId;
// 				}
// 			}

// 			if(oElem != 0 && rElem != 0) {
// 				break;
// 			}
/*
			//exchange target and source and go again
			qulonglong oldRef = referenceId;
			referenceId = ownerId;
			ownerId = oldRef;
		}*/
		if(oElem == 0 || rElem == 0) {
			continue;
		}
		consideredObjectOwnerIds.append(ownerId);
		consideredObjectRefIds.append(referenceId);

		mNetworkElementPairs.append(p);
// 		cerr << "Added: " << p.mOwnerId << " , " << p.mReferenceId << endl;
	}
	
	QList<NetworkElementPair> validPairs;

	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
		NetworkElementPair pair = i.next();
		Neuron *ownerNeuron = ModularNeuralNetwork::selectNeuronById(pair.mOwnerId, ownerNeurons);
		Neuron *refNeuron = ModularNeuralNetwork::selectNeuronById(pair.mReferenceId, refNeurons);

		if(ownerNeuron != 0 && refNeuron != 0) {
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

	mNetworkElementPairs = validPairs;

// 	cerr << "Pairs: " << endl;
// 	for(QListIterator<NetworkElementPair> i(mNetworkElementPairs); i.hasNext();) {
// 		NetworkElementPair pair = i.next();
// 		cerr << pair.mOwnerId << ", " << pair.mReferenceId << " obj: " << pair.mOwnerElement << " " << pair.mReferenceElement << endl;
// 	}
// 	cerr << "--" << endl;
	
	return true;
}


QList<NetworkElementPair> ConnectionSymmetryConstraint::getNetworkElementPairs() const {
	return mNetworkElementPairs;
}



StringValue* ConnectionSymmetryConstraint::getNetworkElementPairValue() const {
	return mNetworkElementPairValue;
}


ULongLongValue* ConnectionSymmetryConstraint::getTargetGroupIdValue() const {
	return mTargetGroupId;
}





}



