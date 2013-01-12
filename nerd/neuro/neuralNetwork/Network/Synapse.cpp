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

#include "Synapse.h"
#include <QListIterator>
#include "SynapseFunction/SynapseFunction.h"
#include "Network/NeuralNetwork.h"
#include "Math/Math.h"
#include "Util/NeuralNetworkUtil.h"
#include <iostream>

using namespace std;

namespace nerd {



Synapse::Synapse(Neuron *source, SynapseTarget *target, double strength, 
				const SynapseFunction &synapseFunction, qulonglong id) 
	: mSourceNeuron(source), mTarget(target), mCurrentActivation(0.0),
	  mSynapseFunction(0), mId(id)
{
	if(id == 0) {
		mId = NeuralNetwork::generateNextId();
	}
	mStrengthValue.set(strength);
	mEnabledValue.set(true);
	mSynapseFunction = synapseFunction.createCopy();

	if(mSourceNeuron != 0 && mTarget != 0) {
		Vector3D pos;
		Vector3D sourcePos = mSourceNeuron->getPosition();
		Vector3D targetPos = mTarget->getPosition();
		if(mSourceNeuron == mTarget) {
			pos = sourcePos + Vector3D(0.0, -70.0, 0.0);
		}
		else {
			QPointF center = Math::centerOfLine(QPointF(sourcePos.getX(), sourcePos.getY()), 
									QPointF(targetPos.getX(), targetPos.getY()));
			pos = Vector3D(center.x(), center.y(), 0.0);
		}
		setPosition(pos);
	}
	if(mSourceNeuron != 0) {
		mSourceNeuron->registerOutgoingSynapse(this);
	}
}


Synapse::Synapse(const Synapse &other) 
	: SynapseTarget(other),
	  mSourceNeuron(other.mSourceNeuron), mTarget(other.mTarget),
	  mCurrentActivation(other.mCurrentActivation), mSynapseFunction(0), mId(other.mId)
{
	mStrengthValue.set(other.mStrengthValue.get());
	mEnabledValue.set(other.mEnabledValue.get());
	if(other.mSynapseFunction != 0) {
		mSynapseFunction = other.mSynapseFunction->createCopy();
	}
	if(mSourceNeuron != 0) {
		mSourceNeuron->registerOutgoingSynapse(this);
	}
}

Synapse::~Synapse() {
	//deletes all incomming Synapses.
	while(!mSynapses.empty()) {
		Synapse *s = mSynapses.at(0);
		if(s != 0 && s != this) {
			mSynapses.removeAll(s);
			delete s;
		}
	}
	//delete synapse function
	delete mSynapseFunction;
}


/**
 * Creates a synapse and (in difference to the constructor) automatically adds this synapse
 * to the target neuron if it is not NULL.
 *
 */
Synapse* Synapse::createSynapse(Neuron *source, SynapseTarget *target, double strength,
				const SynapseFunction &synapseFunction, qulonglong id)
{
	Synapse *synapse = new Synapse(source, target, strength, synapseFunction, id);
	if(target != 0) {
		target->addSynapse(synapse);
	}
	return synapse;
}

Synapse* Synapse::createCopy() const {
	return new Synapse(*this);
}

void Synapse::setId(qulonglong id) {
	mId = id;
}

qulonglong Synapse::getId() const {
	return mId;
}

void Synapse::reset() {
	if(mSynapseFunction != 0) {
		mSynapseFunction->reset(this);
	}
	for(QListIterator<Synapse*> i(mSynapses); i.hasNext();) {
		i.next()->reset();
	}
	mCurrentActivation = 0.0;
}


double Synapse::calculateActivation() {

	if(mSourceNeuron != 0 && mSynapseFunction != 0) {

		//always execute synapse calculation, even if a synapse is disabled.
		if(mEnabledValue.get()) {
			mCurrentActivation = mSourceNeuron->getLastOutputActivation() 
								 * (mSynapseFunction->calculate(this));
		}
		else {
			mCurrentActivation = 0.0;
			mSynapseFunction->calculate(this);
		}
	}
	return mCurrentActivation;
}


double Synapse::getActivation() {
	return mCurrentActivation;
}

DoubleValue& Synapse::getStrengthValue() {
	return mStrengthValue;
}


BoolValue& Synapse::getEnabledValue() {
	return mEnabledValue;
}


bool Synapse::addSynapse(Synapse *synapse) {
	if(synapse == 0 
		|| mSynapses.contains(synapse) 
		|| synapse == this 
		|| synapse->getSource() == 0) 
	{
		return false;
	}
	//make sure that there are only synapses coming from different neurons.
	for(QListIterator<Synapse*> i(mSynapses); i.hasNext();) {
		Neuron *neuron = i.next()->getSource();
		if(neuron != 0 && neuron == synapse->getSource()) {
			return false;
		}
	}
	mSynapses.append(synapse);
	synapse->setTarget(this);
	return true;
}


bool Synapse::removeSynapse(Synapse *synapse) {
	if(synapse == 0 || !mSynapses.contains(synapse)) {
		return false;
	}
	synapse->setTarget(0);
	mSynapses.removeAll(synapse);
	return true;
}


QList<Synapse*> Synapse::getSynapses() const {
	return mSynapses;
}


void Synapse::setSource(Neuron *source) {
	if(mSourceNeuron != 0) {
		mSourceNeuron->deregisterOutgoingSynapse(this);
	}
	mSourceNeuron = source;
	if(mSourceNeuron != 0) {
		mSourceNeuron->registerOutgoingSynapse(this);
	}
}

Neuron* Synapse::getSource() {
	return mSourceNeuron;
}


void Synapse::setTarget(SynapseTarget *target) {
	mTarget = target;
}

SynapseTarget* Synapse::getTarget() {
	return mTarget;
}


void Synapse::setSynapseFunction(const SynapseFunction &synapseFunction) {
	if(mSynapseFunction != 0) {
		delete mSynapseFunction;
	}
	mSynapseFunction = synapseFunction.createCopy();
}

/**
 * Test!
 * Was added to allow an undoable synapse function chance!
 * If synapsefunction is 0, then nothing is changed.
 */
void Synapse::setSynapseFunction(SynapseFunction *synapseFunction) {
	if(synapseFunction == 0) {
		return;
	}
	mSynapseFunction = synapseFunction;
}


SynapseFunction* Synapse::getSynapseFunction() {
	return mSynapseFunction;
}


bool Synapse::equals(Synapse *synapse) {
	if(synapse == 0) {
		return false;
	}
	if(mId != synapse->mId) {
		return false;
	}
	if(mStrengthValue.equals(&(synapse->mStrengthValue)) == false) {
		return false;
	}
	if(mEnabledValue.equals(&(synapse->mEnabledValue)) == false) {
		return false;
	}
	if(mSynapseFunction->equals(synapse->mSynapseFunction) == false) {
		return false;
	}
	if(mTarget != 0 && synapse->mTarget != 0) {
		if(mTarget->getId() != synapse->mTarget->getId()) {
			return false;
		}
	}
	else {
		if(!(mTarget == 0 && synapse->mTarget == 0)) {
			return false;
		}
	}
	if(mSourceNeuron != 0 && synapse->mSourceNeuron != 0) {
		if(mSourceNeuron->getId() != synapse->mSourceNeuron->getId()) {
			return false;
		}
	}
	else {
		if(!(mSourceNeuron == 0 && synapse->mSourceNeuron == 0)) {
			return false;
		}
	}

	//check properties
// 	QList<QString> propertyNames = getPropertyNames();
// 	QList<QString> otherPropertyNames = synapse->getPropertyNames();
// 
// 	if(propertyNames.size() != otherPropertyNames.size()) {
// 		return false;
// 	}
// 	for(QListIterator<QString> i(propertyNames); i.hasNext();) {
// 		QString name = i.next();
// 		if(synapse->hasProperty(name) == false) {
// 			return false;
// 		}
// 		if(getProperty(name) != synapse->getProperty(name)) {
// 			return false;
// 		}
// 	}
	if(Properties::equals(synapse) == false) {
		return false;
	}

	//check synapses
	for(QListIterator<Synapse*> i(mSynapses); i.hasNext();) {
		Synapse *s = i.next();
		Synapse *partnerSynapse = NeuralNetwork::selectSynapseById(
					s->getId(), synapse->mSynapses);

		if(partnerSynapse == 0) {
			return false;
		}
		if(s->equals(partnerSynapse) == false) {
			return false;
		}
	}
	return true;
}


void Synapse::centerPosition() {
	if(mSourceNeuron == 0 || mTarget == 0) {
		return;
	}
	Vector3D pos1 = mSourceNeuron->getPosition();
	Vector3D pos2 = mTarget->getPosition();

	double offset = 0.0;
	if(mSourceNeuron == mTarget) {
		offset = 20.0;
	}
	QPointF newPos = Math::centerOfLine(QPointF(pos1.getX(), pos1.getY()), 
					QPointF(pos2.getX(), pos2.getY()), offset);
	if(mSourceNeuron == mTarget) {
		newPos = newPos + QPointF(0.0, -70.0);
	}
	
	setPosition(Vector3D(newPos.x(), newPos.y(), 0.0));

}



}


