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



#include "SelfRegulatingNeuronActivationFunction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuron.h"
#include "Math/Math.h"
#include <math.h>


using namespace std;

namespace nerd {


/**
 * Constructs a new SelfRegulatingNeuronActivationFunction.
 */
SelfRegulatingNeuronActivationFunction::SelfRegulatingNeuronActivationFunction(const QString &name)
	: ActivationFunction(name), mOwner(0)
{
	mXi = new DoubleValue(1);
	mEta = new DoubleValue(1);
	
	mAlpha = new DoubleValue(0.5);
	mBeta = new DoubleValue(0.1);
	mGamma = new DoubleValue(0.1);
	mDelta = new DoubleValue(0.1);
	mAStar = new DoubleValue(0.658479);
	
	mAdjustWeights = new BoolValue(true);
	
	addParameter("Xi (Rec)", mXi);
	addParameter("Eta (Tra)", mEta);
	addParameter("Alpha", mAlpha);
	addParameter("Beta", mBeta);
	addParameter("Gamma", mGamma);
	addParameter("Delta", mDelta);
	addParameter("A*", mAStar);	
	
	addParameter("AdjustWeights", mAdjustWeights);
	
	addObserableOutput("Xi", mXi);
	addObserableOutput("Eta", mEta);
}


/**
 * Copy constructor. 
 * 
 * @param other the SelfRegulatingNeuronActivationFunction object to copy.
 */
SelfRegulatingNeuronActivationFunction::SelfRegulatingNeuronActivationFunction(
						const SelfRegulatingNeuronActivationFunction &other) 
	: ActivationFunction(other),  mOwner(0)
{
	mXi = dynamic_cast<DoubleValue*>(getParameter("Xi (Rec)"));
	mEta = dynamic_cast<DoubleValue*>(getParameter("Eta (Tra)"));
	mAlpha = dynamic_cast<DoubleValue*>(getParameter("Alpha"));
	mBeta = dynamic_cast<DoubleValue*>(getParameter("Beta"));
	mGamma = dynamic_cast<DoubleValue*>(getParameter("Gamma"));
	mDelta = dynamic_cast<DoubleValue*>(getParameter("Delta"));
	mAStar = dynamic_cast<DoubleValue*>(getParameter("A*"));
	mAdjustWeights = dynamic_cast<BoolValue*>(getParameter("AdjustWeights"));
	
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
	
// 	if(mBeta->get() <= 0.0) {
// 		mBeta->set(0.0000001);
// 	}
// 	if(mGamma->get() <= 0.0 || mGamma->get() >=1.0) {
// 		mGamma->set(Math::max(0.0000001, Math::min(0.9999999, mGamma->get())));
// 	}
// 	if(mDelta->get() <= 0.0 || mDelta->get() >=1.0) {
// 		mDelta->set(Math::max(0.0000001, Math::min(0.9999999, mDelta->get())));
// 	}
	
	
	//Calculate new activation of the neuron.
	
	double inputSum = 0.0;
	bool adjustWeights = mAdjustWeights->get();

	QList<Synapse*> synapses = owner->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();
		if(synapse == 0 || synapse->getSource() == 0) {
			continue;
		}
		
		//weight only used to get the sign of the synapse (-1, 0, 1)
		double weight = synapse->getStrengthValue().get();
		
		Neuron *sourceNeuron = synapse->getSource();
		SelfRegulatingNeuronActivationFunction *af = 
				dynamic_cast<SelfRegulatingNeuronActivationFunction*>(
						sourceNeuron->getActivationFunction());
		
		//source neuron is NOT a self-regulating neuron with a valid eta.
		//Treat like a neuron with eta=1
		double eta = 1;		
		if(af != 0) {
			eta = af->mEta->get();
		}
		
		double sign = Math::sign(weight, true);
		
		if(adjustWeights) {
			synapse->getStrengthValue().set(sign * eta * mXi->get());
		}
		
		//I_i(t) = c_ij * eta_j(t) * o_j(t)
		inputSum += sign * eta * sourceNeuron->getOutputActivationValue().get();
	}

	//a_i(t+1) = theta + xi_i(t) + I_i(t)
	double activation = owner->getBiasValue().get() + mXi->get() * inputSum;
	
	
	//update learning parameter Xi
	//xi_i(t+1) = xi_i(t) * (1 + (beta * g(a(t)))
	mXi->set(Math::min(100.0, Math::max(-100.0,
				mXi->get() * (1.0 + (mBeta->get() * getReceptorStrengthUpdate(activation))))));
	
	//update learning parameter Eta
	//eta_i(t+1) = ((1 - gamma) * eta_i(t)) + (delta * h(a(t)))
	mEta->set(Math::min(100.0, Math::max(-100.0,
				((1.0 - mGamma->get()) * mEta->get()) 
					+ (mDelta->get() * getTransmitterStrengthUpdate(activation)))));
	
	
	return activation;
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
	
	if(true) {
		//g(a) = |tau(a*)| - |tau(a)|
		TransferFunction *tf = mOwner->getTransferFunction();
		return Math::abs(tf->transferActivation(mAStar->get(), mOwner)) 
					- Math::abs(tf->transferActivation(activation, mOwner)); 
	}
	else {
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
	
	if(true) {
		//h(a) = 1 + tau(a)
		TransferFunction *tf = mOwner->getTransferFunction();
		return 1 + tf->transferActivation(mAStar->get(), mOwner); 
	}
	else {
		//h(a) = tau(a(t) - (tau(a(t-1)))   //CHECK if brackets are correct!
		TransferFunction *tf = mOwner->getTransferFunction();
		return tf->transferActivation(mAStar->get() 
					- tf->transferActivation(mOwner->getLastActivation(), mOwner), mOwner); 
	}
	
	return 0.0;
}


}


