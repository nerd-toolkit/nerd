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



#include "SelfRegulatingNeuronV2ActivationFunction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuron.h"
#include "Math/Math.h"
#include <math.h>
#include "SynapseFunction/SimpleLinkSynapseFunction.h"
#include <QStringList>

using namespace std;

namespace nerd {


/**
 * Constructs a new SelfRegulatingNeuronV2ActivationFunction.
 */
SelfRegulatingNeuronV2ActivationFunction::SelfRegulatingNeuronV2ActivationFunction(const QString &name, 
																			   bool showModes)
	: ActivationFunction(name), mOwner(0), mTransmitterResult(0), mReceptorResult(0),
		mActivationT2(0), mAdjustWeights(true), mRestrictToLinks(false), 
		mUseCurrentActivations(false), mEpsilon(0.0), mUpdatingTargetString(false)
{
	mXi = new DoubleValue(1);
	mEta = new DoubleValue(1);
	
	mAlpha = new DoubleValue(0.5);
	mBeta = new DoubleValue(0.1);
	mGamma = new DoubleValue(0.01);
	mDelta = new DoubleValue(0.02);
	mTargetValues = new StringValue("-0.658479, 0.658479");
	mTargets.append(-0.658479);
	mTargets.append(0.658479);
	
	mOptions = new StringValue("w");
	mOptions->setDescription("Selection of SRN options Syntax: [o1,o2,o3].\n"
							 "w : adapt weights\n"
							 "rl : restrict weight adaptions to synapses with SimpleLink synapse function\n");
	
	mReceptorMode = new IntValue(1);
	mReceptorMode->setDescription("Switches g():\n"
						"0: g = |tf(a*)| - |tf(a)|\n"
						"1: g = tf(a*)^2-tf(a)^2\n"
						"2: g = tf(theta) - tf(a(t))\n"
						"3: g = theta - tf(a(t))^2");
	
	mTransmitterMode = new IntValue(0);
	mTransmitterMode->setDescription("Switches h():\n"
						"0: h = 1 + tf(a)\n"
						"1: h = 1 + 0.5 * (tf(a(t)) - tf(a(t-1)))\n"
						"2: h = 1 + tf(a(t) - theta)\n"
						"3: h = |tf(a(t))|");
	
	mBiasMode = new IntValue(0);
	mBiasMode->setDescription("Switches the bias update function:\n"
						"0: none\n"
						"1: theta(t+1) = theta(t) + alpha * (tf(a(t)) - tf(a(t-1)))\n"
						"2: theta(t+1) = theta(t) + alpha * (tf(a(t) - theta(t))^2)");
	
	addParameter("Xi (Rec)", mXi);
	addParameter("Eta (Tra)", mEta);
	addParameter("Alpha", mAlpha);
	addParameter("Beta", mBeta);
	addParameter("Gamma", mGamma);
	addParameter("Delta", mDelta);
	addParameter("Targets", mTargetValues);	
	
	addParameter("Options", mOptions);

	addObserableOutput("Xi", mXi);
	addObserableOutput("Eta", mEta);
	
	if(showModes) {
		addParameter("ReceptorMode", mReceptorMode);
		addParameter("TransmitterMode", mTransmitterMode);
		addParameter("ThetaMode", mBiasMode);
	}
}


/**
 * Copy constructor. 
 * 
 * @param other the SelfRegulatingNeuronV2ActivationFunction object to copy.
 */
SelfRegulatingNeuronV2ActivationFunction::SelfRegulatingNeuronV2ActivationFunction(
						const SelfRegulatingNeuronV2ActivationFunction &other) 
	: ActivationFunction(other),  mOwner(0), mTransmitterResult(0), mReceptorResult(0),
		mAdjustWeights(other.mAdjustWeights), mRestrictToLinks(other.mRestrictToLinks), 
		mUseCurrentActivations(other.mUseCurrentActivations),
		mEpsilon(other.mEpsilon), mUpdatingTargetString(false)
{
	mXi = dynamic_cast<DoubleValue*>(getParameter("Xi (Rec)"));
	mEta = dynamic_cast<DoubleValue*>(getParameter("Eta (Tra)"));
	mAlpha = dynamic_cast<DoubleValue*>(getParameter("Alpha"));
	mBeta = dynamic_cast<DoubleValue*>(getParameter("Beta"));
	mGamma = dynamic_cast<DoubleValue*>(getParameter("Gamma"));
	mDelta = dynamic_cast<DoubleValue*>(getParameter("Delta"));
	mTargetValues = dynamic_cast<StringValue*>(getParameter("Targets"));
// 	mAdjustWeights = dynamic_cast<BoolValue*>(getParameter("AdjustWeights"));
// 	mRestrictToLinkSynapses = dynamic_cast<BoolValue*>(getParameter("RestrictToLinks"));
	mOptions = dynamic_cast<StringValue*>(getParameter("Options"));
	mReceptorMode = dynamic_cast<IntValue*>(getParameter("ReceptorMode"));
	mTransmitterMode = dynamic_cast<IntValue*>(getParameter("TransmitterMode"));
	mBiasMode = dynamic_cast<IntValue*>(getParameter("ThetaMode"));
	
	addObserableOutput("Xi", mXi);
	addObserableOutput("Eta", mEta);
}

/**
 * Destructor.
 */
SelfRegulatingNeuronV2ActivationFunction::~SelfRegulatingNeuronV2ActivationFunction() {
}


ActivationFunction* SelfRegulatingNeuronV2ActivationFunction::createCopy() const {
	return new SelfRegulatingNeuronV2ActivationFunction(*this);
}


void SelfRegulatingNeuronV2ActivationFunction::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mOptions) {
		QStringList rawOptions = mOptions->get().split(",");
		QStringList options;
		
		//allow also white spaces and tabs in option list.
		for(QListIterator<QString> i(rawOptions); i.hasNext();) {
			options.append(i.next().trimmed());
		}
		
		mAdjustWeights = options.contains("w");
		mRestrictToLinks = options.contains("rl");
		mUseCurrentActivations = options.contains("t+1");
		mEpsilon = options.contains("e") ? 0.0001 : 0.0;
	}
	else if(value == mTargetValues && !mUpdatingTargetString) {
		
		//parse targets and update target string.
		QStringList entries = mTargetValues->get().split(",");
		mTargets.clear();
		
		bool ok = true;
		for(int i = 0; i < entries.size(); i++) {
			double value = entries.at(i).toDouble(&ok);
			if(ok) {
				
				//sort target into target list
				bool added = false;
				for(int j = 0; j < mTargets.size(); ++j) {
					if(mTargets.at(j) > value) {
						mTargets.insert(j, value);
						added = true;
					}
				}
				if(!added) {
					mTargets.append(value);
				}
			}
		}
		//recreate string
		QString targetString;
		for(QListIterator<double> i(mTargets); i.hasNext();) {
			targetString = targetString.append(QString::number(i.next()));
			if(i.hasNext()) {
				targetString.append(",");
			}
		}
		mUpdatingTargetString = true;
		mTargetValues->set(targetString);
		mUpdatingTargetString = false;
	}
	ActivationFunction::valueChanged(value);
}


void SelfRegulatingNeuronV2ActivationFunction::reset(Neuron *owner) {
}

double SelfRegulatingNeuronV2ActivationFunction::calculateActivation(Neuron *owner) {
	
	mOwner = owner;
	if(owner == 0) {
		return 0.0;
	}
	
	double activation = updateActivity();
	
	double usedActivation = activation;
	
	//switch wether the current of the previous activation is used for the update.
	if(!mUseCurrentActivations) {
		usedActivation = mOwner->getLastActivation();
	}
	
	mReceptorResult = getReceptorStrengthUpdate(usedActivation);
	mTransmitterResult = getTransmitterStrengthUpdate(usedActivation);
	updateXi(usedActivation);
	updateEta(usedActivation);
	updateTheta(usedActivation);
	
	mActivationT2 = owner->getLastActivation();

	return activation;
}




bool SelfRegulatingNeuronV2ActivationFunction::equals(ActivationFunction *activationFunction) const {
	if(ActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	SelfRegulatingNeuronV2ActivationFunction *af =
 			dynamic_cast<SelfRegulatingNeuronV2ActivationFunction*>(activationFunction);

	if(af == 0) {
		return false;
	}
	if(af->mAlpha->get() != mAlpha->get()
		|| af->mBeta->get() != mBeta->get()
		|| af->mGamma->get() != mGamma->get()
		|| af->mDelta->get() != mDelta->get()
		|| af->mTargetValues->get() != mTargetValues->get()) 
	{
		return false;
	}
	return true;
}

double SelfRegulatingNeuronV2ActivationFunction::getReceptorStrengthUpdate(double activation) {
	
	if(mOwner == 0 || mOwner->getTransferFunction() == 0) {
		return 0.0;
	}
	
// 	if(mReceptorMode == 0 || mReceptorMode->get() == 0) {
// 		//g(a) = |tau(a*)| - |tau(a)|
// 		TransferFunction *tf = mOwner->getTransferFunction();
// 		return Math::abs(tf->transferActivation(mAStar->get(), mOwner)) 
// 					- Math::abs(tf->transferActivation(activation, mOwner)); 
// 	}
// 	else if(mReceptorMode->get() == 1) {
// 		//g(a) = tau²(a*) - tau²(a)
// 		TransferFunction *tf = mOwner->getTransferFunction();
// 		return pow(tf->transferActivation(mAStar->get(), mOwner), 2) 
// 					- pow(tf->transferActivation(activation, mOwner), 2); 
// 	}
// 	else if(mReceptorMode->get() == 2) {
// 		//g(a) = tf(theta) - tf(a(t))
// 		TransferFunction *tf = mOwner->getTransferFunction();
// 		return tf->transferActivation(mOwner->getBiasValue().get(), mOwner) 
// 					- tf->transferActivation(activation, mOwner); 
// 	}
// 	else if(mReceptorMode->get() == 3) {
// 		//g(a) = theta - tf(a(t))^2
// 		TransferFunction *tf = mOwner->getTransferFunction();
// 		return mOwner->getBiasValue().get() -  pow(tf->transferActivation(activation, mOwner), 2);
// 	}
	
	return 0.0;
}


double SelfRegulatingNeuronV2ActivationFunction::getTransmitterStrengthUpdate(double activation) {
	
	if(mOwner == 0 || mOwner->getTransferFunction() == 0) {
		return 0.0;
	}
	
// 	if(mTransmitterMode == 0 || mTransmitterMode->get() == 0) {
// 		//h(a) = 1 + tau(a)
// 		TransferFunction *tf = mOwner->getTransferFunction();
// 		return 1 + tf->transferActivation(activation, mOwner); 
// 	}
// 	else if(mTransmitterMode->get() == 1) {
// 		//h(a) = tau(a(t)) - (tau(a(t-1)))   //CHECK if brackets are correct!
// 		TransferFunction *tf = mOwner->getTransferFunction();
// 		if(mUseCurrentActivations) {
// 			return 1 + 0.5 * (tf->transferActivation(activation, mOwner)
// 					- tf->transferActivation(mOwner->getLastActivation(), mOwner)); 
// 		}
// 		else {
// 			return 1 + 0.5 * (tf->transferActivation(activation, mOwner)
// 					- tf->transferActivation(mActivationT2, mOwner)); 
// 		}
// 	}
// 	else if(mTransmitterMode->get() == 2) {
// 		//h(a) = 1 + tf(a(t) - theta)
// 		TransferFunction *tf = mOwner->getTransferFunction();
// 		return 1 + (tf->transferActivation(activation, mOwner)
// 					- mOwner->getBiasValue().get()); 
// 	}
// 	else if(mTransmitterMode->get() == 3) {
// 		//h(a) = |tf(a(t))|
// 		TransferFunction *tf = mOwner->getTransferFunction();
// 		//cerr << "Got: " << (Math::abs(tf->transferActivation(activation, mOwner)))  << " with " << activation << endl;
// 		return Math::abs(tf->transferActivation(activation, mOwner)); 
// 	}
	
	return 0.0;
}



void SelfRegulatingNeuronV2ActivationFunction::updateXi(double activation) {
	//update learning parameter Xi
	//xi_i(t+1) = xi_i(t) * (1 + (beta * g(a(t)))
// 	mXi->set(Math::min(100.0, Math::max(-100.0,
// 				mEpsilon + mXi->get() * (1.0 + (mBeta->get() * mReceptorResult)))));
}


void SelfRegulatingNeuronV2ActivationFunction::updateEta(double activation) {
	//update learning parameter Eta
	//eta_i(t+1) = ((1 - gamma) * eta_i(t)) + (delta * h(a(t)))
// 	mEta->set(Math::min(100.0, Math::max(-100.0,
// 				mEpsilon + ((1.0 - mGamma->get()) * mEta->get()) 
// 					+ (mDelta->get() * mTransmitterResult))));
}

void SelfRegulatingNeuronV2ActivationFunction::updateTheta(double activation) {
	if(mBiasMode->get() == 0) {
		//do nothing
		return;
	}
// 	else if(mBiasMode->get() == 1) {
// 		//theta(t+1) = theta(t) + alpha * (tf(a(t)) - tf(a(t-1)))
// 		if(mOwner != 0) {
// 			TransferFunction *tf = mOwner->getTransferFunction();
// 			double newBias = 0;
// 			if(mUseCurrentActivations) {
// 				newBias = mOwner->getBiasValue().get() 
// 						+ mAlpha->get() 
// 							* (tf->transferActivation(activation, mOwner)
// 								- tf->transferActivation(mOwner->getLastActivation(), mOwner));
// 			}
// 			else {
// 				newBias = mOwner->getBiasValue().get() 
// 							+ mAlpha->get() 
// 							* (tf->transferActivation(mOwner->getLastActivation(), mOwner)
// 									- tf->transferActivation(mActivationT2, mOwner));
// 			}
// 			mOwner->getBiasValue().set(newBias);
// 		}
// 	}
// 	else if(mBiasMode->get() == 2) {
// 		//theta(t+1) = theta(t) + alpha * (tf(a(t) - theta(t))^2)
// 		if(mOwner != 0) {
// 			TransferFunction *tf = mOwner->getTransferFunction();
// 			double newBias = 0;
// 			if(mUseCurrentActivations) {
// 				newBias = mOwner->getBiasValue().get() 
// 				+ mAlpha->get() 
// 				* pow(tf->transferActivation(activation 
// 				* 			- mOwner->getBiasValue().get(), mOwner), 2);
// 			}
// 			else {
// 				newBias = mOwner->getBiasValue().get() 
// 				+ mAlpha->get() 
// 				* pow(tf->transferActivation(mOwner->getLastActivation() 
// 				* 			- mOwner->getBiasValue().get(), mOwner), 2);
// 			}
// 			mOwner->getBiasValue().set(newBias);
// 		}
// 	}
}


double SelfRegulatingNeuronV2ActivationFunction::updateActivity() {

	if(mOwner == 0) {
		return 0.0;
	}
	
	//Calculate new activation of the neuron.
	
	double inputSum = 0.0;

	QList<Synapse*> synapses = mOwner->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();
		if(synapse == 0 || synapse->getSource() == 0 || !synapse->getEnabledValue().get()) {
			continue;
		}
		
		//weight only used to get the sign of the synapse (-1, 0, 1)
		double weight = synapse->getStrengthValue().get();
		
		Neuron *sourceNeuron = synapse->getSource();
		SelfRegulatingNeuronV2ActivationFunction *af = 
				dynamic_cast<SelfRegulatingNeuronV2ActivationFunction*>(
						sourceNeuron->getActivationFunction());
		
		//if source neuron is NOT a self-regulating neuron with a valid eta,
		//then treat it like a neuron with eta=1
		double eta = 1;		
		if(af != 0) {
			eta = af->mEta->get();
		}
		
		bool isLink = true;
		
		if(mRestrictToLinks) {
			//consider only weights that 
			if(dynamic_cast<SimpleLinkSynapseFunction*>(synapse->getSynapseFunction()) == 0) {
				isLink = false;
			}
		}

		double act = 0.0;
		
		if(isLink) {
			double sign = Math::sign(weight); //zero is treated as positive value.
			
			if(mAdjustWeights) {
				double newWeight = sign * eta * mXi->get();
				
				
				//make sure that numerical inaccuracies near zero do not change the sign.
				if(Math::sign(newWeight) != sign) {
					newWeight *= -1.0;
				}
				
				synapse->getStrengthValue().set(newWeight);
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

	//a_i(t+1) = theta_i + xi_i(t) * I_i(t)
	double activation = mOwner->getBiasValue().get() + mXi->get() * inputSum;
	return activation;
}


}



