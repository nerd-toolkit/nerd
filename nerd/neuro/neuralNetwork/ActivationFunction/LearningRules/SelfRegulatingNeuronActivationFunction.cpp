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



#include "SelfRegulatingNeuronActivationFunction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuron.h"
#include "Math/Math.h"
#include <math.h>
#include "SynapseFunction/SimpleLinkSynapseFunction.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SelfRegulatingNeuronActivationFunction.
 */
SelfRegulatingNeuronActivationFunction::SelfRegulatingNeuronActivationFunction(const QString &name, 
																			   bool showModes)
	: ActivationFunction(name), mOwner(0), mTransmitterResult(0), mReceptorResult(0)
{
	mXi = new DoubleValue(1);
	mEta = new DoubleValue(1);
	
	mAlpha = new DoubleValue(0.5);
	mBeta = new DoubleValue(0.1);
	mGamma = new DoubleValue(0.01);
	mDelta = new DoubleValue(0.02);
	mAStar = new DoubleValue(0.658479);
	
	mAdjustWeights = new BoolValue(true);
	mAdjustWeights->setDescription("Enables an update of the synapse weights in the network.");
	
	mRestrictToLinkSynapses = new BoolValue(false);
	mRestrictToLinkSynapses->setDescription("Restricts weight changes to synapses with the SimpleLinkSynapseFunction.");
	
	mReceptorMode = new IntValue(0);
	mReceptorMode->setDescription("Switches g():\n"
						"0: g = |tf(a*)| - |tf(a)|\n"
						"1: g = tf(a*)^2-tf(a)^2");
	mTransmitterMode = new IntValue(0);
	mTransmitterMode->setDescription("Switches h():\n"
						"0: h = 1 + tf(a)\n"
						"1: h = 1 + 0.5 * (tf(a(t)) - tf(a(t-1)))");
	
	addParameter("Xi (Rec)", mXi);
	addParameter("Eta (Tra)", mEta);
	addParameter("Alpha", mAlpha);
	addParameter("Beta", mBeta);
	addParameter("Gamma", mGamma);
	addParameter("Delta", mDelta);
	addParameter("A*", mAStar);	
	
	addParameter("AdjustWeights", mAdjustWeights);
	addParameter("RestrictToLinks", mRestrictToLinkSynapses);
	
	addObserableOutput("Xi", mXi);
	addObserableOutput("Eta", mEta);
	
	if(showModes) {
		addParameter("ReceptorMode", mReceptorMode);
		addParameter("TransmitterMode", mTransmitterMode);
	}
}


/**
 * Copy constructor. 
 * 
 * @param other the SelfRegulatingNeuronActivationFunction object to copy.
 */
SelfRegulatingNeuronActivationFunction::SelfRegulatingNeuronActivationFunction(
						const SelfRegulatingNeuronActivationFunction &other) 
	: ActivationFunction(other),  mOwner(0), mTransmitterResult(0), mReceptorResult(0)
{
	mXi = dynamic_cast<DoubleValue*>(getParameter("Xi (Rec)"));
	mEta = dynamic_cast<DoubleValue*>(getParameter("Eta (Tra)"));
	mAlpha = dynamic_cast<DoubleValue*>(getParameter("Alpha"));
	mBeta = dynamic_cast<DoubleValue*>(getParameter("Beta"));
	mGamma = dynamic_cast<DoubleValue*>(getParameter("Gamma"));
	mDelta = dynamic_cast<DoubleValue*>(getParameter("Delta"));
	mAStar = dynamic_cast<DoubleValue*>(getParameter("A*"));
	mAdjustWeights = dynamic_cast<BoolValue*>(getParameter("AdjustWeights"));
	mRestrictToLinkSynapses = dynamic_cast<BoolValue*>(getParameter("RestrictToLinks"));
	mReceptorMode = dynamic_cast<IntValue*>(getParameter("ReceptorMode"));
	mTransmitterMode = dynamic_cast<IntValue*>(getParameter("TransmitterMode"));
	
	addObserableOutput("Xi", mXi);
	addObserableOutput("Eta", mEta);
}

/**
 * Destructor.
 */
SelfRegulatingNeuronActivationFunction::~SelfRegulatingNeuronActivationFunction() {
}


ActivationFunction* SelfRegulatingNeuronActivationFunction::createCopy() const {
	return new SelfRegulatingNeuronActivationFunction(*this);
}

void SelfRegulatingNeuronActivationFunction::reset(Neuron *owner) {
}

double SelfRegulatingNeuronActivationFunction::calculateActivation(Neuron *owner) {
	
	mOwner = owner;
	if(owner == 0) {
		return 0.0;
	}
	
	double activation = updateActivity();
	
	mReceptorResult = getReceptorStrengthUpdate(activation);
	mTransmitterResult = getTransmitterStrengthUpdate(activation);
	updateXi(activation);
	updateEta(activation);

	return activation;
}

DoubleValue* SelfRegulatingNeuronActivationFunction::getAlpha() const {
	return mAlpha;
}


DoubleValue* SelfRegulatingNeuronActivationFunction::getBeta() const {
	return mBeta;
}


DoubleValue* SelfRegulatingNeuronActivationFunction::getGamma() const {
	return mGamma;
}


DoubleValue* SelfRegulatingNeuronActivationFunction::getDelta() const {
	return mDelta;
}


DoubleValue* SelfRegulatingNeuronActivationFunction::getAStar() const {
	return mAStar;
}



bool SelfRegulatingNeuronActivationFunction::equals(ActivationFunction *activationFunction) const {
	if(ActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	SelfRegulatingNeuronActivationFunction *af =
 			dynamic_cast<SelfRegulatingNeuronActivationFunction*>(activationFunction);

	if(af == 0) {
		return false;
	}
	if(af->mAlpha->get() != mAlpha->get()
		|| af->mBeta->get() != mBeta->get()
		|| af->mGamma->get() != mGamma->get()
		|| af->mDelta->get() != mDelta->get()
		|| af->mAStar->get() != mAStar->get()) 
	{
		return false;
	}
	return true;
}

double SelfRegulatingNeuronActivationFunction::getReceptorStrengthUpdate(double activation) {
	
	if(mOwner == 0 || mOwner->getTransferFunction() == 0) {
		return 0.0;
	}
	
	if(mReceptorMode == 0 || mReceptorMode->get() == 0) {
		//g(a) = |tau(a*)| - |tau(a)|
		TransferFunction *tf = mOwner->getTransferFunction();
		return Math::abs(tf->transferActivation(mAStar->get(), mOwner)) 
					- Math::abs(tf->transferActivation(activation, mOwner)); 
	}
	else if(mReceptorMode->get() == 1) {
		//g(a) = tau²(a*) - tau²(a)
		TransferFunction *tf = mOwner->getTransferFunction();
		return pow(tf->transferActivation(mAStar->get(), mOwner), 2) 
					- pow(tf->transferActivation(activation, mOwner), 2); 
	}
	
	return 0.0;
}


double SelfRegulatingNeuronActivationFunction::getTransmitterStrengthUpdate(double activation) {
	
	if(mOwner == 0 || mOwner->getTransferFunction() == 0) {
		return 0.0;
	}
	
	if(mTransmitterMode == 0 || mTransmitterMode->get() == 0) {
		//h(a) = 1 + tau(a)
		TransferFunction *tf = mOwner->getTransferFunction();
		return 1 + tf->transferActivation(activation, mOwner); 
	}
	else if(mTransmitterMode->get() == 1) {
		//h(a) = tau(a(t) - (tau(a(t-1)))   //CHECK if brackets are correct!
		TransferFunction *tf = mOwner->getTransferFunction();
		return 1 + 0.5 * (tf->transferActivation(activation, mOwner)
					- tf->transferActivation(mOwner->getLastActivation(), mOwner)); 
	} 
	
	return 0.0;
}

void SelfRegulatingNeuronActivationFunction::updateXi(double activation) {
	//update learning parameter Xi
	//xi_i(t+1) = xi_i(t) * (1 + (beta * g(a(t)))
	mXi->set(Math::min(100.0, Math::max(-100.0,
				0.0001 + mXi->get() * (1.0 + (mBeta->get() * mReceptorResult)))));
}


void SelfRegulatingNeuronActivationFunction::updateEta(double activation) {
	//update learning parameter Eta
	//eta_i(t+1) = ((1 - gamma) * eta_i(t)) + (delta * h(a(t)))
	mEta->set(Math::min(100.0, Math::max(-100.0,
				0.0001 + ((1.0 - mGamma->get()) * mEta->get()) 
					+ (mDelta->get() * mTransmitterResult))));
}


double SelfRegulatingNeuronActivationFunction::updateActivity() {

	if(mOwner == 0) {
		return 0.0;
	}
	
	//Calculate new activation of the neuron.
	
	double inputSum = 0.0;
	bool adjustWeights = mAdjustWeights->get();

	QList<Synapse*> synapses = mOwner->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();
		if(synapse == 0 || synapse->getSource() == 0 || !synapse->getEnabledValue().get()) {
			continue;
		}
		
		//weight only used to get the sign of the synapse (-1, 0, 1)
		double weight = synapse->getStrengthValue().get();
		
		Neuron *sourceNeuron = synapse->getSource();
		SelfRegulatingNeuronActivationFunction *af = 
				dynamic_cast<SelfRegulatingNeuronActivationFunction*>(
						sourceNeuron->getActivationFunction());
		
		//if source neuron is NOT a self-regulating neuron with a valid eta,
		//then treat it like a neuron with eta=1
		double eta = 1;		
		if(af != 0) {
			eta = af->mEta->get();
		}
		
		bool isLink = true;
		
		if(mRestrictToLinkSynapses->get()) {
			//consider only weights that 
			if(dynamic_cast<SimpleLinkSynapseFunction*>(synapse->getSynapseFunction()) == 0) {
				isLink = false;
			}
		}

		double act = 0.0;
		
		if(isLink) {
			double sign = Math::sign(weight); //zero is treated as positive value.
			
			if(adjustWeights) {
				synapse->getStrengthValue().set(sign * eta * mXi->get());
			}
			act = sign * eta * sourceNeuron->getOutputActivationValue().get();
		}
		else {
			act = synapse->calculateActivation();
		}
		
		//I_i(t) = c_ij * eta_j(t) * o_j(t)
		//inputSum += sign * eta * sourceNeuron->getOutputActivationValue().get();
		inputSum += act;
	}

	//a_i(t+1) = theta + xi_i(t) + I_i(t)
	double activation = mOwner->getBiasValue().get() + mXi->get() * inputSum;
	return activation;
}


}



