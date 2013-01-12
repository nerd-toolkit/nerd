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



#include "ActivationFrequencyCalculator.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "NeuralNetworkConstants.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include "Control/ControlInterface.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ActivationFrequencyCalculator.
 */
ActivationFrequencyCalculator::ActivationFrequencyCalculator(const QString &name)
	: ParameterizedObject(name, "/Neuro/ActivationFrequencyCalculators/A/"), 
		mObservedNeuron(0), mLastActivation(0), mStepCounter(0), mFoundFirstReference(false)
{
	mAgentName = new StringValue("");
	mNeuronId = new ULongLongValue();
	mReferenceActivation = new DoubleValue();
	mFrequency = new IntValue();

	addParameter("AgentName", mAgentName);
	addParameter("NeuronId", mNeuronId);
	addParameter("ReferenceActivation", mReferenceActivation);
	addParameter("Frequency", mFrequency);

	Core::getInstance()->addSystemObject(this);
	publishAllParameters();
}


/**
 * Destructor.
 */
ActivationFrequencyCalculator::~ActivationFrequencyCalculator() {
}


bool ActivationFrequencyCalculator::init() {
	return true;
}


bool ActivationFrequencyCalculator::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mNetworksModifiedEvent = em->registerForEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED, this);
	mNetworksUpdatedEvent = em->registerForEvent(
				NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_COMPLETED, this);
	mNetworksReplacedEvent = em->registerForEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED, this);
	
	if(mNetworksModifiedEvent == 0 || mNetworksUpdatedEvent == 0 || mNetworksReplacedEvent == 0) {
		ok = false;
		Core::log("ActivationFrequencyCalculator: Could not find all required events.");
	}

	return ok;
}


bool ActivationFrequencyCalculator::cleanUp() {
	return true;
}


QString ActivationFrequencyCalculator::getName() const {
	return ParameterizedObject::getName();
}


void ActivationFrequencyCalculator::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}	
	ParameterizedObject::valueChanged(value);

	if(value == mAgentName) {
		updateNetworkConnection();
	}
	else if(value == mNeuronId) {
		updateNetworkConnection();
	}
	else if(value == mReferenceActivation) {
	}
}


void ActivationFrequencyCalculator::eventOccured(Event *event) {

	if(event == 0) {
		return;
	}
	else if(event == mNetworksUpdatedEvent) {
		calculateFrequency();
	}
	else if(event == mNetworksModifiedEvent || event == mNetworksReplacedEvent) {
		updateNetworkConnection();
	}
}


void ActivationFrequencyCalculator::updateNetworkConnection() {
	
	QList<NeuralNetwork*> networks = Neuro::getNeuralNetworkManager()->getNeuralNetworks();

	for(QListIterator<NeuralNetwork*> i(networks); i.hasNext();) {
		NeuralNetwork *net = i.next();

		if(net->getControlInterface() != 0 
			&& net->getControlInterface()->getName() == mAgentName->get())
		{
			QList<Neuron*> neurons = net->getNeurons();
			for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
				Neuron *neuron = j.next();
				if(neuron->getId() == mNeuronId->get()) {
					if(mObservedNeuron == neuron) {
						return;
					}
					mObservedNeuron = neuron;
					mFrequency->set(0);
					mLastActivation = neuron->getOutputActivationValue().get();
					mStepCounter = 0;
					mFoundFirstReference = false;

					return;
				}
			}
		}
	}
	mObservedNeuron = 0;
	mFrequency->set(0);
	mLastActivation = 0.0;
	mStepCounter = 0;
	mFoundFirstReference = false;

}


void ActivationFrequencyCalculator::calculateFrequency() {

	if(mObservedNeuron == 0) {
		return;
	}
	mStepCounter++;

	double currentActivation = mObservedNeuron->getOutputActivationValue().get();

	if(currentActivation > mLastActivation) {
		if(currentActivation >= mReferenceActivation->get() 
			&& mLastActivation <= mReferenceActivation->get())
		{
			if(mFoundFirstReference) {
				mFrequency->set(mStepCounter);
				mStepCounter = 0;
				mFoundFirstReference = false;
			}
			else {
				mFoundFirstReference = true;
			}
		}
	}
	else {
		if(currentActivation <= mReferenceActivation->get() 
			&& mLastActivation >= mReferenceActivation->get())
		{
			
			if(mFoundFirstReference) {
				mFrequency->set(mStepCounter);
				mStepCounter = 0;
				mFoundFirstReference = false;
			}
			else {
				mFoundFirstReference = true;
			}
		}
	}

	mLastActivation = currentActivation;
}

}



