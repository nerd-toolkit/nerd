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

#include "Neuron.h"
#include "Network/NeuralNetwork.h"
#include "Core/Core.h"
#include <QListIterator>
#include <iostream>
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


const QString Neuron::NEURON_TYPE_INPUT = "TYPE_INPUT";
const QString Neuron::NEURON_TYPE_OUTPUT = "TYPE_OUTPUT";



Neuron::Neuron(const QString &name, const TransferFunction &tf, const ActivationFunction &af, qulonglong id)
	: mId(id), mNameValue(name), mTransferFunction(0), mActivationFunction(0),
	  mLastActivation(0.0), mLastOutputActivation(0.0), mActivationCalculated(false),
	  mFlipActivation(false), mOwnerNetwork(0)
{
	if(id == 0) {
		mId = NeuralNetwork::generateNextId();
	}
	mTransferFunction = tf.createCopy();
	mActivationFunction = af.createCopy();
}


Neuron::Neuron(const Neuron &other) 
	: SynapseTarget(other),
	  mNameValue(other.mNameValue.get()), 
	  mTransferFunction(0), mActivationFunction(0),
	  mLastActivation(other.mLastActivation), mLastOutputActivation(0.0), 
	  mActivationCalculated(false), mFlipActivation(other.mFlipActivation),
	  mOwnerNetwork(0)
{
	mId = other.mId;
	if(other.mTransferFunction != 0) {
		mTransferFunction = other.mTransferFunction->createCopy();
	}
	if(other.mActivationFunction != 0) {
		mActivationFunction = other.mActivationFunction->createCopy();
	}
	mBiasValue.set(other.mBiasValue.get());
	mActivationValue.set(other.mActivationValue.get());
	mOutputActivationValue.set(other.mOutputActivationValue.get());
}

Neuron::~Neuron() {
	delete mTransferFunction;
	delete mActivationFunction;
	while(!mIncommingSynapses.empty()) {
		Synapse *s = mIncommingSynapses.at(0);
		mIncommingSynapses.removeAll(s);
		delete s;
	}
}


Neuron* Neuron::createCopy() const {
	return new Neuron(*this);
}

void Neuron::prepare() {
	mLastActivation = mActivationValue.get();
	mLastOutputActivation = mOutputActivationValue.get();
	mActivationCalculated = false;
}

double Neuron::getLastActivation() const {
	return mLastActivation;
}


double Neuron::getLastOutputActivation() const {
	return mLastOutputActivation;
}


void Neuron::reset() {
	//TODO reset activation

	if(hasProperty(NeuralNetworkConstants::TAG_FLIP_NEURON_ACTIVITY)) {
		mFlipActivation = true;
	}
	else {
		mFlipActivation = false;
	}

	mActivationValue.set(0.0);
	mOutputActivationValue.set(0.0);
	mLastActivation = 0.0;
	mLastOutputActivation = 0.0;
	if(mTransferFunction != 0) {
		mTransferFunction->reset(this);
	}
	if(mActivationFunction != 0) {
		mActivationFunction->reset(this);
	}
	for(int i = 0; i < mIncommingSynapses.size(); ++i) {
		mIncommingSynapses.at(i)->reset();
	}	
}


void Neuron::updateActivation() {
	if(mTransferFunction == 0 || mActivationFunction == 0) {
		Core::log(QString("Neuron::updateActivation [").append(mNameValue.get())
			.append("] : TransferFunction or ActivationFunction missing [SKIPPING]."));
		return;
	}
	if(!mActivationCalculated) {
		mActivationValue.set(mActivationFunction->calculateActivation(this));
		mOutputActivationValue.set(mTransferFunction->transferActivation(
									mActivationValue.get(), this));
		mActivationCalculated = true;
	}
}


qulonglong Neuron::getId() const {
	return mId;
}


void Neuron::setId(qulonglong id) {
	mId = id;
}


StringValue& Neuron::getNameValue() {
	return mNameValue;
}


DoubleValue& Neuron::getActivationValue() {
	return mActivationValue;
}

DoubleValue& Neuron::getOutputActivationValue() {
	return mOutputActivationValue;
}


DoubleValue& Neuron::getBiasValue() {
	return mBiasValue;
}

bool Neuron::addInputValue(DoubleValue *input) {
	if(input == 0 || mInputValues.contains(input)) {
		return false;
	}
	mInputValues.append(input);
	return true;
}


bool Neuron::removeInputValue(DoubleValue *input) {
	if(input == 0 || !mInputValues.contains(input)) {
		return false;
	}
	mInputValues.removeAll(input);
	return true;
}


QList<DoubleValue*> Neuron::getInputValues() const {
	return mInputValues;
}



void Neuron::setTransferFunction(const TransferFunction &tf) {
	if(mTransferFunction != 0) {
		delete mTransferFunction;
	}
	mTransferFunction = tf.createCopy();
}


/**
 * Test!
 * Was added to support the undo of transfer function changes!
 * 
 * If tf is 0, then nothing is changed.
 */
void Neuron::setTransferFunction(TransferFunction *tf) {
	if(tf == 0) {
		return;
	}
	mTransferFunction = tf;
}


TransferFunction* Neuron::getTransferFunction() const {
	return mTransferFunction;
}


void Neuron::setActivationFunction(const ActivationFunction &af) {
	if(mActivationFunction != 0) {
		delete mActivationFunction;
	}
	mActivationFunction = af.createCopy();
}

/**
 * Test!
 * Same as above
 */
void Neuron::setActivationFunction(ActivationFunction *af) {
	if(af == 0) {
		return;
	}
	mActivationFunction = af;
}


ActivationFunction* Neuron::getActivationFunction() const {
	return mActivationFunction;
}


bool Neuron::addSynapse(Synapse *synapse) {
	if(synapse == 0 || mIncommingSynapses.contains(synapse) || synapse->getSource() == 0) {
		return false;
	}
	for(QListIterator<Synapse*> i(mIncommingSynapses); i.hasNext();) {
		if(i.next()->getSource() == synapse->getSource()) {
			return false;
		}
	}
	synapse->setTarget(this);
	mIncommingSynapses.append(synapse);
	return true;
}


bool Neuron::removeSynapse(Synapse *synapse) {
	if(synapse == 0 || !mIncommingSynapses.contains(synapse)) {
		return false;
	}
	mIncommingSynapses.removeAll(synapse);
	if(synapse->getTarget() == this) {
		synapse->setTarget(0);
	}
	return true;
}


QList<Synapse*> Neuron::getSynapses() const {
	return mIncommingSynapses;
}


bool Neuron::registerOutgoingSynapse(Synapse *synapse) {
	if(synapse == 0 || mOutgoingSynapses.contains(synapse)) {
		return false;
	}
	mOutgoingSynapses.append(synapse);
	return true;
}


bool Neuron::deregisterOutgoingSynapse(Synapse *synapse) {
	if(synapse == 0 || !mOutgoingSynapses.contains(synapse)) {
		return false;
	}
	mOutgoingSynapses.removeAll(synapse);
	return true;
}


QList<Synapse*> Neuron::getOutgoingSynapses() const {
	return mOutgoingSynapses;
}

void Neuron::setOwnerNetwork(NeuralNetwork *network) {
	mOwnerNetwork = network;
}


NeuralNetwork* Neuron::getOwnerNetwork() const {
	return mOwnerNetwork;
}


bool Neuron::equals(Neuron *neuron) {
	if(neuron == 0) {
		return false;
	}
	if(mId != neuron->mId) {
		return false;
	}
	if(mNameValue.equals(&(neuron->mNameValue)) == false) {
		return false;
	}
	if(mBiasValue.equals(&(neuron->mBiasValue)) == false) {
		return false;
	}
	if(mTransferFunction->equals(neuron->mTransferFunction) == false) {
		return false;
	}
	if(mActivationFunction->equals(neuron->mActivationFunction) == false) {
		return false;
	}
	if(mIncommingSynapses.size() != neuron->mIncommingSynapses.size()) {
		return false;
	} 
	if(mFlipActivation != neuron->mFlipActivation) {
		return false;
	}

	//check propertis
	if(Properties::equals(neuron) == false) {
		return false;
	}

	//check synapses
	for(QListIterator<Synapse*> i(mIncommingSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		Synapse *partnerSynapse = NeuralNetwork::selectSynapseById(
					synapse->getId(), neuron->mIncommingSynapses);

		if(partnerSynapse == 0) {
			return false;
		}
		if(synapse->equals(partnerSynapse) == false) {
			return false;
		}
	}
	return true;
}

void Neuron::flipActivation(bool flip) {
	mFlipActivation = flip;
}


bool Neuron::isActivationFlipped() const {
	return mFlipActivation;
}

bool Neuron::requiresUpdate(int iteration) {
	if(getStartIteration() <= iteration && iteration < (getStartIteration() + getRequiredIterations())) {
		return true;
	}
	for(QListIterator<Synapse*> i(mIncommingSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		//TODO extend for higher order synapses
		if(synapse->getStartIteration() == 0 && synapse->getRequiredIterations() == 1) {
			//Ignore default synapses.
			continue;
		}
		if(synapse->getStartIteration() <= iteration && iteration < (synapse->getStartIteration() + synapse->getRequiredIterations())) {
			return true;
		}
	}
	return false;
}

void Neuron::propertyChanged(Properties *owner, const QString &property) {
	NeuralNetworkElement::propertyChanged(owner, property);
	
	if(owner != this) {
		return;
	}

	if(hasProperty(NeuralNetworkConstants::TAG_FLIP_NEURON_ACTIVITY)) {
		mFlipActivation = true;
	}
	else {
		mFlipActivation = false;
	}
}

}


