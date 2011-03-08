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

#include "NeuralNetwork.h"
#include <QListIterator>
#include "TransferFunction/TransferFunctionTanh.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "Core/Core.h"
#include <iostream>
#include "Control/ControlInterface.h"
#include "Math/Math.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Util/Util.h"
#include "NeuralNetworkConstants.h"
#include <limits>

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {


NeuralNetwork::NeuralNetwork(const ActivationFunction &defaultActivationFunction,
					  const TransferFunction &defaultTransferFunction,
					  const SynapseFunction &defaultSynapseFunction)
	: mDefaultActivationFunction(defaultActivationFunction.createCopy()),
	  mDefaultTransferFunction(defaultTransferFunction.createCopy()),
	  mDefaultSynapseFunction(defaultSynapseFunction.createCopy()),
	  mControlInterface(0), mBypassNetwork(false)
{
	TRACE("NeuralNetwork::NeuralNetwork");
}

NeuralNetwork::NeuralNetwork(const NeuralNetwork &other) 
		: Controller(), Object(), Properties(other), mControlInterface(0), mBypassNetwork(false)
{
	TRACE("NeuralNetwork::NeuralNetworkCopy");

	mDefaultActivationFunction = other.mDefaultActivationFunction->createCopy();
	mDefaultTransferFunction = other.mDefaultTransferFunction->createCopy();
	mDefaultSynapseFunction = other.mDefaultSynapseFunction->createCopy();

	QList<SynapseTarget*> synapseTargets;
	QList<Synapse*> newSynapses;

	QList<Neuron*> neuronPrototypes = other.mNeurons;
	QList<Synapse*> synapsePrototypes = other.getSynapses();

// 	{
// 		QList<SynapseTarget*> synapseTargetPrototypes;
// 		for(QListIterator<Neuron*> i(neuronPrototypes); i.hasNext();) {
// 			synapseTargetPrototypes.append(i.next());
// 		}
// 		for(QListIterator<Synapse*> i(synapsePrototypes); i.hasNext();) {
// 			synapseTargetPrototypes.append(i.next());
// 		}
// 	}

	//copy all neurons.
	{
		for(QListIterator<Neuron*> i(neuronPrototypes); i.hasNext();) {
			Neuron *orig = i.next();
			Neuron *n = orig->createCopy();
			//memorize copy in orig. Never use this pointer for anything else than 
			//during construction! This is for performance optimization only.
			orig->mCopyPtr = n;
			addNeuron(n);
// 			synapseTargets.append(n);
		}
	}
	//copy all synapses.
	{
		for(QListIterator<Synapse*> i(synapsePrototypes); i.hasNext();) {
			Synapse *orig = i.next();
			Synapse *s = orig->createCopy();
			orig->mCopyPtr = s;
// 			synapseTargets.append(s);
			newSynapses.append(s);
		}
	}
	//assign synapses
	{
		for(QListIterator<Synapse*> i(newSynapses); i.hasNext();) {
			Synapse *copy = i.next();
			Synapse *prototype = selectSynapseById(copy->getId(), synapsePrototypes);
			if(prototype == 0 || prototype->getSource() == 0 || prototype->getTarget() == 0) {
				//TODO this has to be removed, because when creating a module this is valid.
				Core::log("NeuralNetwork::copyConstructor: Detected synapse without "
						  "target or source in synapse prototypes or a NULL synapse! [Skipping]");
				continue;
			}
			//Neuron *source = selectNeuronById(prototype->getSource()->getId(), mNeurons);
			Neuron *source = dynamic_cast<Neuron*>(prototype->getSource()->mCopyPtr);
// 			SynapseTarget *target = selectSynapseTargetById(prototype->getTarget()->getId(),
//  										synapseTargets);
			SynapseTarget *target = dynamic_cast<SynapseTarget*>(prototype->getTarget()->mCopyPtr);

			if(source == 0 || target == 0) {
				Core::log("NeuralNetwork::copyConstructor: Could not find appropriate "
						  "target or source for synapse copy! [Skipping]");
				continue;
			}
			copy->setSource(source);
			copy->setTarget(target);
			target->addSynapse(copy);
		}
	}
	
	validateSynapseConnections();
}

NeuralNetwork::~NeuralNetwork() {
	TRACE("NeuralNetwork::~NeuralNetwork");

	delete mDefaultTransferFunction;
	delete mDefaultActivationFunction;
	delete mDefaultSynapseFunction;

	while(!mNeurons.empty()) {
		Neuron *n = mNeurons.at(0);
		mNeurons.removeAll(n);
		delete n;
	}
}


NeuralNetwork* NeuralNetwork::createCopy() {
	TRACE("NeuralNetwork::NeuralNetwork");

	return new NeuralNetwork(*this);
}

QString NeuralNetwork::getName() const {
	TRACE("NeuralNetwork::getName");

	return "NeuralNetwork";
}

void NeuralNetwork::setControlInterface(ControlInterface *controlInterface) {
	TRACE("NeuralNetwork::setControlInterface");

	mInputPairs.clear();
	mOutputPairs.clear();
	mControlInterface = controlInterface;

	mBypassNetwork = Neuro::getNeuralNetworkManager()->getBypassNetworksValue()->get();

	if(mControlInterface != 0) {
		//add all input and output values sorted by names or order.

		QList<InterfaceValue*> mInputValues = mControlInterface->getInputValues();
		QList<InterfaceValue*> mOutputValues = mControlInterface->getOutputValues();
		QList<Neuron*> mInputNeurons = getInputNeurons();
		QList<Neuron*> mOutputNeurons = getOutputNeurons();

		QList<Neuron*> mInputNeuronBuffer(mInputNeurons);
		QList<Neuron*> mOutputNeuronBuffer(mOutputNeurons);
		QList<InterfaceValue*> mInputValueBuffer(mInputValues);
		QList<InterfaceValue*> mOutputValueBuffer(mOutputValues);

		bool foundNameMatches = false;
		//check for matching names
		for(QListIterator<Neuron*> i(mInputNeurons); i.hasNext();) {
			Neuron *n = i.next();
			for(QListIterator<InterfaceValue*> j(mOutputValues); j.hasNext();) {
				InterfaceValue *v = j.next();
				if(n->getNameValue().get() == v->getName()) {
					mInputPairs.append(NeuronInterfaceValuePair(n, v));
					//mOutputValues.removeAll(v);
					mOutputValueBuffer.removeAll(v);
					mInputNeuronBuffer.removeAll(n);
					foundNameMatches = true;
					break;
				}
			}
		}

		//connect output neurons only if the network is not bypassed.
		for(QListIterator<Neuron*> i(mOutputNeurons); i.hasNext();) {
			Neuron *n = i.next();
			for(QListIterator<InterfaceValue*> j(mInputValues); j.hasNext();) {
				InterfaceValue *v = j.next();
				if(n->getNameValue().get() == v->getName()) {
					mOutputPairs.append(NeuronInterfaceValuePair(n, v));
					//mInputValues.removeAll(v);
					mInputValueBuffer.removeAll(v);
					mOutputNeuronBuffer.removeAll(n);
					foundNameMatches = true;
					break;
				}
			}
		}
		
		//assign all pairs by order that could not be matched by name
		{
			QListIterator<Neuron*> in(mInputNeuronBuffer);
			QListIterator<InterfaceValue*> iv(mOutputValueBuffer);
			for(; in.hasNext() && iv.hasNext();) 
			{
				Neuron *n = in.next();
				InterfaceValue *ifv = iv.next();
				if(foundNameMatches) {
					QString message = QString("NeuralNetowrk: Could not find a name match for input [")
									 + ifv->getName() + "] and neuron [" + n->getNameValue().get() + "]";
					Core::log(message);
					//cerr << message.toStdString().c_str() << endl;
				}
				mInputPairs.append(NeuronInterfaceValuePair(n, ifv));
				n->getNameValue().set(ifv->getName());
			}
		}
		//connect output neurons only if the network is not bypassed.
		{
			QListIterator<Neuron*> on(mOutputNeuronBuffer);
			QListIterator<InterfaceValue*> ov(mInputValueBuffer);
			for(; on.hasNext() && ov.hasNext();) 
			{
				Neuron *n = on.next();
				InterfaceValue *ifv = ov.next();
				if(foundNameMatches) {
					QString message = QString("NeuralNetowrk: Could not find a name match for output [")
									 + ifv->getName() + "] and neuron [" + n->getNameValue().get() + "]";
					Core::log(message);
					//cerr << message.toStdString().c_str() << endl;
				}
				mOutputPairs.append(NeuronInterfaceValuePair(n, ifv));
				n->getNameValue().set(ifv->getName());
			}	
		}
	}
}


ControlInterface* NeuralNetwork::getControlInterface() const {
	TRACE("NeuralNetwork::getControlInterface");

	return mControlInterface;
}



void NeuralNetwork::executeStep() {
	TRACE("NeuralNetwork::executeStep");

	//update input neurons
	for(QListIterator<NeuronInterfaceValuePair> i(mInputPairs); i.hasNext();) {
		NeuronInterfaceValuePair pair = i.next();
		if(pair.mNeuron != 0 && pair.mInterfaceValue != 0) {
			TransferFunction *tf = pair.mNeuron->getTransferFunction();
			pair.mInterfaceValue->setNormalizedMin(tf->getLowerBound());
			pair.mInterfaceValue->setNormalizedMax(tf->getUpperBound());
			double activation = pair.mInterfaceValue->getNormalized() + pair.mNeuron->getBiasValue().get();
			if(pair.mNeuron->isActivationFlipped()) {
				activation = tf->getUpperBound() + (tf->getLowerBound() - activation);
			}
			pair.mNeuron->getActivationValue().set(activation);
			//TODO check what to do with input behavior (consider af, tf?)
			pair.mNeuron->getOutputActivationValue().set(activation);
		}
	}

	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		//prepare ALL neurons
		i.next()->prepare();
	}

	int currentIteration = mMinimalIterationNumber;

	QList<Neuron*> remainingNeurons = mProcessibleNeurons;
	QList<Neuron*> lastExecutedNeurons;

	do {
		
		QList<Neuron*> remainingNeuronsBuffer = remainingNeurons;
		for(QListIterator<Neuron*> j(remainingNeuronsBuffer); j.hasNext();) {
			Neuron *neuron = j.next();
			//if(neuron->getStartIteration() <= currentIteration)
			if(neuron->requiresUpdate(currentIteration)) {
				lastExecutedNeurons.append(neuron);
				remainingNeurons.removeAll(neuron);
			}
			else if(neuron->getStartIteration() < currentIteration) {
				remainingNeurons.removeAll(neuron);
			}
		}

		//update all affected neurons
		for(QListIterator<Neuron*> i(lastExecutedNeurons); i.hasNext();) {
			i.next()->updateActivation();
		}

		//prepare for next iteration
		for(QListIterator<Neuron*> i(lastExecutedNeurons); i.hasNext();) {
			i.next()->prepare();
		}

		++currentIteration;

		//update list or affected neurons
		QList<Neuron*> lastExecutedNeuronsBuffer = lastExecutedNeurons;
		for(QListIterator<Neuron*> j(lastExecutedNeuronsBuffer); j.hasNext();) {
			Neuron *neuron = j.next();
			//if(neuron->getStartIteration() + (neuron->getRequiredIterations() - 1) < currentIteration) {
			if(!neuron->requiresUpdate(currentIteration)) {
				lastExecutedNeurons.removeAll(neuron);
			}
		}

		Neuro::getNeuralNetworkManager()->triggerNetworkIterationCompleted();

	} while(!lastExecutedNeurons.empty() || !remainingNeurons.empty());
	
	//transfer output neuron activations to actuators
	if(!mBypassNetwork) {
		for(QListIterator<NeuronInterfaceValuePair> i(mOutputPairs); i.hasNext();) {
			NeuronInterfaceValuePair pair = i.next();
			if(pair.mNeuron != 0 && pair.mInterfaceValue != 0) {
				TransferFunction *tf = pair.mNeuron->getTransferFunction();
				pair.mInterfaceValue->setNormalizedMin(tf->getLowerBound());
				pair.mInterfaceValue->setNormalizedMax(tf->getUpperBound());
				double activation = pair.mNeuron->getOutputActivationValue().get();
				if(pair.mNeuron->isActivationFlipped()) {
					activation = tf->getUpperBound() + (tf->getLowerBound() - activation);
				}
				pair.mInterfaceValue->setNormalized(activation);
				//cerr << "Set interface to " << pair.mNeuron->getOutputActivationValue().get() << endl;
			}
		}
	}
}

void NeuralNetwork::reset() { 
	TRACE("NeuralNetwork::reset");

	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		i.next()->reset();
	}

	mMinimalIterationNumber = getMinimalStartIteration();
}

void NeuralNetwork::propertyChanged(Properties *owner, const QString &property) {
		
	Neuron *neuron = dynamic_cast<Neuron*>(owner);
	if(neuron != 0 && mNeurons.contains(neuron)) {
		if(property == NeuralNetworkConstants::TAG_INPUT_NEURON) {
			if(owner->hasProperty(NeuralNetworkConstants::TAG_INPUT_NEURON)) {
				if(!mInputNeurons.contains(neuron)) {
					mInputNeurons.append(neuron);
					mProcessibleNeurons.removeAll(neuron);
				}
			}
			else {
				mInputNeurons.removeAll(neuron);
				if(!mProcessibleNeurons.contains(neuron)) {
					mProcessibleNeurons.append(neuron);
				}
			}
		}
		else if(property == NeuralNetworkConstants::TAG_OUTPUT_NEURON) {
			if(owner->hasProperty(NeuralNetworkConstants::TAG_OUTPUT_NEURON)) {
				if(!mOutputNeurons.contains(neuron)) {
					mOutputNeurons.append(neuron);
				}
			}
			else {
				mOutputNeurons.removeAll(neuron);
			}
		}
	}
	if(property == NeuralNetworkConstants::TAG_FAST_ITERATIONS) {
		mMinimalIterationNumber = getMinimalStartIteration();
	}
}

bool NeuralNetwork::addNeuron(Neuron *neuron) {
	TRACE("NeuralNetwork::addNeuron");

	if(neuron == 0 || mNeurons.contains(neuron)) {
		return false;
	}
	if(neuron->getTransferFunction() == 0) {
		neuron->setTransferFunction(*mDefaultTransferFunction);
	}
	if(neuron->getActivationFunction() == 0) {
		neuron->setActivationFunction(*mDefaultActivationFunction);
	}
	if(neuron->hasProperty(NeuralNetworkConstants::TAG_INPUT_NEURON) && !mInputNeurons.contains(neuron)) {
		mInputNeurons.append(neuron);
		mProcessibleNeurons.removeAll(neuron);
	}
	if(neuron->hasProperty(NeuralNetworkConstants::TAG_OUTPUT_NEURON) && !mOutputNeurons.contains(neuron)) {
		mOutputNeurons.append(neuron);
	}
	if(!mInputNeurons.contains(neuron) && !mProcessibleNeurons.contains(neuron)) {
		mProcessibleNeurons.append(neuron);
	}
	mNeurons.append(neuron);
	mNeuronsById.insert(neuron->getId(), neuron);
	
	neuron->addPropertyChangedListener(this);

	mMinimalIterationNumber = getMinimalStartIteration();

	return true;
}


bool NeuralNetwork::removeNeuron(Neuron *neuron) {
	TRACE("NeuralNetwork::removeNeuron");

	if(neuron == 0 || !mNeurons.contains(neuron)) {
		return false;
	}
	mNeurons.removeAll(neuron);
	mNeuronsById.remove(neuron->getId());

	mInputNeurons.removeAll(neuron);
	mOutputNeurons.removeAll(neuron);
	mProcessibleNeurons.removeAll(neuron);

	neuron->removePropertyChangedListener(this);
	return true;
}


QList<Neuron*> NeuralNetwork::getNeurons() const {
	return mNeurons;
}


QList<Neuron*> NeuralNetwork::getInputNeurons() const {
	TRACE("NeuralNetwork::getInputNeurons");

// 	QList<Neuron*> inputNeurons;
// 	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
// 		Neuron *n = i.next();
// 		if(n->hasProperty(Neuron::NEURON_TYPE_INPUT)) {
// 			inputNeurons.append(n);
// 		}
// 	}
// 	return inputNeurons;
	return mInputNeurons;
}


QList<Neuron*> NeuralNetwork::getOutputNeurons() const {
	TRACE("NeuralNetwork::getOutputNeurons");

// 	QList<Neuron*> outputNeurons;
// 	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
// 		Neuron *n = i.next();
// 		if(n->hasProperty(Neuron::NEURON_TYPE_OUTPUT)) {
// 			outputNeurons.append(n);
// 		}
// 	}
// 	return outputNeurons;
	return mOutputNeurons;
}

QList<NeuronInterfaceValuePair> NeuralNetwork::getInputPairs() const {
	TRACE("NeuralNetwork::getInputPairs");

	return mInputPairs;
}


QList<NeuronInterfaceValuePair> NeuralNetwork::getOutputPairs() const {
	TRACE("NeuralNetwork::getOutputPairs");

	return mOutputPairs;
}


QList<Synapse*> NeuralNetwork::getSynapses() const {
	//TRACE("NeuralNetwork::getSynapses");

	QList<Synapse*> synapses;
	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		QList<Synapse*> newSynapses = i.next()->getSynapses();
		synapses += newSynapses;
		for(QListIterator<Synapse*> j(newSynapses); j.hasNext();) {
			getSynapses(j.next(), synapses);
		}
	}
	return synapses;
}


void NeuralNetwork::getSynapses(const Synapse *synapse, QList<Synapse*> &synapses) const {
	//TRACE("NeuralNetwork::getSynapses");

	if(synapse == 0) {
		return;
	}
	QList<Synapse*> newSynapses = synapse->getSynapses();
	synapses += newSynapses;
	for(QListIterator<Synapse*> i(newSynapses); i.hasNext();) {
		getSynapses(i.next(), synapses);
	}
}


void NeuralNetwork::getNetworkElements(QList<NeuralNetworkElement*> &elementList) const {
	TRACE("NeuralNetwork::getNetworkElements");

	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		elementList.append(i.next());
	}
	QList<Synapse*> synapses = getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		elementList.append(i.next());
	}
}

/**
 * Removes the element and all other NeuralNetwork elements that are not longer valid 
 * through the removal of element.
 * A list with all removed elements, including the given element, is returned to allow their
 * deletion.
 * If the element could not be removed (e.g. because it was not part of the network), then 
 * the returned list is empty.
 */
QList<NeuralNetworkElement*> NeuralNetwork::savelyRemoveNetworkElement(NeuralNetworkElement *element) {
	QList<NeuralNetworkElement*> removedElements;

	Neuron *neuron = dynamic_cast<Neuron*>(element);
	if(neuron != 0) {
		if(!mNeurons.contains(neuron)) {
			return removedElements;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_INPUT_NEURON)
			|| neuron->hasProperty(NeuralNetworkConstants::TAG_OUTPUT_NEURON))
		{
			return removedElements;
		}

		removedElements.append(neuron);
		
		QList<Synapse*> inSynapses = neuron->getSynapses();
		for(QListIterator<Synapse*> i(inSynapses); i.hasNext();) {
			Synapse *synapse = i.next();
			Util::addWithoutDuplicates(removedElements, savelyRemoveNetworkElement(synapse));
		}
		QList<Synapse*> outSynapses = neuron->getOutgoingSynapses();
		for(QListIterator<Synapse*> i(outSynapses); i.hasNext();) {
			Synapse *synapse = i.next();	
			Util::addWithoutDuplicates(removedElements, savelyRemoveNetworkElement(synapse));
		}

		removeNeuron(neuron);
	}
	Synapse *synapse = dynamic_cast<Synapse*>(element);
	if(synapse != 0) {
		removedElements.append(synapse);

		QList<Synapse*> inSynapses = synapse->getSynapses();
		for(QListIterator<Synapse*> i(inSynapses); i.hasNext();) {
			Synapse *s = i.next();
			Util::addWithoutDuplicates(removedElements, savelyRemoveNetworkElement(s));
		}

		if(synapse->getTarget() != 0) {
			synapse->getTarget()->removeSynapse(synapse);
		}
		synapse->setSource(0);
	}

	return removedElements;
}

void NeuralNetwork::setDefaultTransferFunction(const TransferFunction &tf) {
	TRACE("NeuralNetwork::setDefaultTransferFunction");

	if(mDefaultTransferFunction != 0) {
		delete mDefaultTransferFunction;
	}
	mDefaultTransferFunction = tf.createCopy();
}


TransferFunction* NeuralNetwork::getDefaultTransferFunction() const {
	TRACE("NeuralNetwork::getDefaultTransferFunction");

	return mDefaultTransferFunction;
}


void NeuralNetwork::setDefaultActivationFunction(const ActivationFunction &af) {
	TRACE("NeuralNetwork::setDefaultActivationFunction");

	if(mDefaultActivationFunction != 0) {
		delete mDefaultActivationFunction;
	}
	mDefaultActivationFunction = af.createCopy();
}


ActivationFunction* NeuralNetwork::getDefaultActivationFunction() const {
	TRACE("NeuralNetwork::getDefaultActivationFunction");

	return mDefaultActivationFunction;
}


void NeuralNetwork::setDefaultSynapseFunction(const SynapseFunction &sf) {
	TRACE("NeuralNetwork::setDefaultSynapseFunction");

	if(mDefaultSynapseFunction != 0) {
		delete mDefaultSynapseFunction;
	}
	mDefaultSynapseFunction = sf.createCopy();
}


SynapseFunction* NeuralNetwork::getDefaultSynapseFunction() const {
	TRACE("NeuralNetwork::getDefaultSynapseFunction");

	return mDefaultSynapseFunction;
}


void NeuralNetwork::validateSynapseConnections() {
	TRACE("NeuralNetwork::validateSynapseConnections");

	bool ok = false;
	QList<Synapse*> removedSynapses;

	while(!ok) {
		QList<Synapse*> synapses = getSynapses();
		ok = true;
		for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
			Synapse *s = i.next();
			if(s == 0 || s->getTarget() == 0) {
				Core::log("NeuralNetwork::validateSynapseConnections: Invalid state detected! "
						  "Synapse is NULL or does not have a specified target neuron! ");
				return;
			}
			else {
				if(s->getSource() == 0 || !mNeurons.contains(s->getSource())) {
					ok = false;
					removedSynapses.append(s);
					s->getTarget()->removeSynapse(s);
					break;
				}
				else if(s->getTarget() != 0) {
					SynapseTarget *target = s->getTarget();
					if(!getSynapses().contains(dynamic_cast<Synapse*>(target)) 
							&& !mNeurons.contains(dynamic_cast<Neuron*>(target))) {
						removedSynapses.append(s);
						target->removeSynapse(s);
					}
				}
			}
		}
	}
	
	for(QListIterator<Synapse*> i(removedSynapses); i.hasNext();) {
		QList<NeuralNetworkElement*> elements = savelyRemoveNetworkElement(i.next());
		for(QListIterator<NeuralNetworkElement*> j(elements); j.hasNext();) {
			Synapse *s = dynamic_cast<Synapse*>(j.next());
			if(s != 0 && !removedSynapses.contains(s)) {
				removedSynapses.append(s);
			}
		}
	}
	while(!removedSynapses.empty()) {
		Synapse *s = removedSynapses.at(0);
		removedSynapses.removeAll(s);
		delete s;
	}
}


bool NeuralNetwork::equals(NeuralNetwork *network) {
	TRACE("NeuralNetwork::equals");

	if(network == 0) {
		return false;
	}
	if(Properties::equals(network) == false) {
		return false;
	}
	if(mDefaultSynapseFunction->equals(network->mDefaultSynapseFunction) == false) {
		return false;
	}
	if(mDefaultActivationFunction->equals(network->mDefaultActivationFunction) == false) {
		return false;
	}
	if(mDefaultTransferFunction->equals(network->mDefaultTransferFunction) == false) {
		return false;
	}

	QList<Neuron*> neurons = network->getNeurons();
	if(neurons.size() != mNeurons.size()) {
		return false;
	}


	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		Neuron *partnerNeuron = NeuralNetwork::selectNeuronById(neuron->getId(), neurons);

		if(partnerNeuron == 0) {
			return false;
		}

		if(neuron->equals(partnerNeuron) == false) {
			return false;
		}
	}

	
	
	return true;
}


void NeuralNetwork::adjustIdCounter() {
	TRACE("NeuralNetwork::adjustIdCounter");

	qulonglong maxId = 1;
	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		maxId = Math::max(i.next()->getId(), maxId);
	}
	QList<Synapse*> synapses = getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		maxId = Math::max(i.next()->getId(), maxId);
	}
	NeuralNetwork::resetIdCounter(Math::max(NeuralNetwork::generateNextId(), maxId));
}


/**
 * Releases all network elements. Thus these elements will not be destroyed 
 * by the network. After calling this method the NeuralNetork is cleared().
 */
void NeuralNetwork::freeElements(bool destroyElements) {
	TRACE("NeuralNetwork::freeElements");

	ControlInterface *controller = mControlInterface;
	if(controller != 0) {
		setControlInterface(0);
	}
	
	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		i.next()->removePropertyChangedListener(this);
	}

	if(destroyElements) {
		while(!mNeurons.empty()) {
			Neuron *n = mNeurons.at(0);
			mNeurons.removeAll(n);
			delete n;
		}
	}
	mNeurons.clear();
	mInputNeurons.clear();
	mOutputNeurons.clear();
	mProcessibleNeurons.clear();
	mNeuronsById.clear();
	if(controller != 0) {
		setControlInterface(controller);
	}
}


/**
 * If the bypass mode is enabled, then the output neurons are not written to the corresponding
 * controller inputs. This allows for a manual control of the agent while it is possible
 * to observe the behavior of the network.
 */
void NeuralNetwork::bypassNetwork(bool bypass) {
	mBypassNetwork = bypass;
}

bool NeuralNetwork::isBypassingNetwork() const {
	return mBypassNetwork;
}


int NeuralNetwork::getHighestRequiredIterationNumber() const {
	int highest = 0;
	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		int iterations = i.next()->getRequiredIterations();
		if(highest < iterations) {
			highest = iterations;
		}
	}
	QList<Synapse*> synapses = getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		int iterations = i.next()->getRequiredIterations();
		if(highest < iterations) {
			highest = iterations;
		}
	}
	return highest;
}


QList<Neuron*> NeuralNetwork::getNeuronsWithIterationRequirement(int requirement) {
	QList<Neuron*> neurons;
	
	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		Neuron *neuron = i.next();

		if(neuron->requiresUpdate(requirement)) {
			neurons.append(neuron);
		}
	}

	return neurons;
}

int NeuralNetwork::getMinimalStartIteration() const {
	int minimal = numeric_limits<int>::max();
	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		int iterations = i.next()->getStartIteration();
		if(minimal > iterations) {
			minimal = iterations;
		}
	}
	QList<Synapse*> synapses = getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		int iterations = i.next()->getStartIteration();
		if(minimal > iterations) {
			minimal = iterations;
		}
	}
	return minimal;
}


Neuron* NeuralNetwork::getNeuronById(qulonglong id) const {
	return mNeuronsById.value(id);
}


void NeuralNetwork::synchronizeNeuronIds() {
	mNeuronsById.clear();
	for(QListIterator<Neuron*> i(mNeurons); i.hasNext();) {
		Neuron *n = i.next();
		mNeuronsById.insert(n->getId(), n);
	}
}


qulonglong NeuralNetwork::mIdPool = 1;

qulonglong NeuralNetwork::generateNextId() {
	TRACE("NeuralNetwork::generateNextId");

	++mIdPool;
	return mIdPool;
}

void NeuralNetwork::resetIdCounter(qulonglong currentId) {
	TRACE("NeuralNetwork::resetIdCounter");

	mIdPool = currentId;
}


Neuron* NeuralNetwork::selectNeuronById(qulonglong id, QList<Neuron*> neurons) {
	TRACE("NeuralNetwork::selectNeuronById");

	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *n = i.next();
		if(n->getId() == id) {
			return n;
		}
	}
	return 0;
}


SynapseTarget* NeuralNetwork::selectSynapseTargetById(qulonglong id, QList<SynapseTarget*> targets) {
	TRACE("NeuralNetwork::selectSynapseTargetById");

	for(QListIterator<SynapseTarget*> i(targets); i.hasNext();) {
		SynapseTarget *s = i.next();
		if(s->getId() == id) {
			return s;
		}
	}
	return 0;
}

Synapse* NeuralNetwork::selectSynapseById(qulonglong id, QList<Synapse*> synapses) {
	TRACE("NeuralNetwork::selectSynapseById");

	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *s = i.next();
		if(s->getId() == id) {
			return s;
		}
	}
	return 0;
}


NeuralNetworkElement* NeuralNetwork::selectNetworkElementById(qulonglong id, 
							QList<NeuralNetworkElement*> elements)
{
	TRACE("NeuralNetwork::selectNetworkElementById");

	for(QListIterator<NeuralNetworkElement*> i(elements); i.hasNext();) {
		NeuralNetworkElement *e = i.next();
		if(e->getId() == id) {
			return e;
		}
	}
	return 0;
}

// void NeuralNetwork::renewIds(QList<SynapseTarget*> objects) {
// 	TRACE("NeuralNetwork::renewIds");
// 
// 	for(QListIterator<SynapseTarget*> i(objects); i.hasNext();) {
// 		i.next()->setId(generateNextId());
// 	}
// }



}


