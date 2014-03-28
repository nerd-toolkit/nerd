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

#include "EnergyNeuronActivationFunction.h"
#include <QListIterator>
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <Math/Math.h>
#include <iostream>
#include <math.h>

using namespace std;

namespace nerd {

EnergyNeuronActivationFunction::EnergyNeuronActivationFunction()
	: ActivationFunction("EnergyNeuron")
{
    mRechargeRate = new DoubleValue(0.001);
    mAutoDischargeRate = new DoubleValue(0.0001);
    mMaxCharge = new DoubleValue(1.0);
    mInitCharge = new DoubleValue(1.0);
    mCurrentCharge = new DoubleValue(1.0);
    mThreshold = new DoubleValue(0.0);

	addParameter("RechargeRate", mRechargeRate);
	mRechargeRate->setDescription("Factor for incoming charge from synapses");
	addParameter("AutoDischargeRate", mAutoDischargeRate);
	mAutoDischargeRate->setDescription("Rate for autmomatic decay of energy");
	addParameter("MaxCharge", mMaxCharge);
	mMaxCharge->setDescription("Maximum value of energy the neuron can 'hold'");
	addParameter("InitCharge", mInitCharge);
	mInitCharge->setDescription("The Initial charge of the neuron at reset");
	addParameter("Threshold", mThreshold);
	mThreshold->setDescription("Minimum required input to start charging");

	addObservableOutput("Charge", mCurrentCharge);
}

EnergyNeuronActivationFunction::EnergyNeuronActivationFunction(
			const EnergyNeuronActivationFunction &other)
	: Object(), ValueChangedListener(),
	  ObservableNetworkElement(other), ActivationFunction(other)
{
	mRechargeRate = dynamic_cast<DoubleValue*>(getParameter("RechargeRate"));
	mAutoDischargeRate =
        dynamic_cast<DoubleValue*>(getParameter("AutoDischargeRate"));
	mMaxCharge = dynamic_cast<DoubleValue*>(getParameter("MaxCharge"));
	mInitCharge = dynamic_cast<DoubleValue*>(getParameter("InitCharge"));
	mThreshold = dynamic_cast<DoubleValue*>(getParameter("Threshold"));
	mCurrentCharge = new DoubleValue(mInitCharge->get());

	addObservableOutput("Charge", mCurrentCharge);
}

EnergyNeuronActivationFunction::~EnergyNeuronActivationFunction() {
}

ActivationFunction* EnergyNeuronActivationFunction::createCopy() const {
	return new EnergyNeuronActivationFunction(*this);
}

void EnergyNeuronActivationFunction::reset(Neuron*) {
	mCurrentCharge->set(mInitCharge->get());
}


double EnergyNeuronActivationFunction::calculateActivation(Neuron *owner) {
	ActivationFunction::calculateActivation(owner);
	if(owner == 0) {
		return 0.0;
	}
	//double activation = owner->getBiasValue().get(); // ignore bias?
	double activation = mCurrentCharge->get() - mAutoDischargeRate->get();
	if(activation < 0) {
        activation = 0.0;
	}

	QList<Synapse*> synapses = owner->getSynapses();
	double sIn;
	double inCharge = 0.0;
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
         sIn = i.next()->calculateActivation();
         if(sIn > 0) {
            inCharge += sIn;
         }
	}
	if(inCharge >= mThreshold->get()) {
        activation += mRechargeRate->get() * inCharge;
	}

	if(activation > mMaxCharge->get()) {
        activation = mMaxCharge->get();
	}

	mCurrentCharge->set(activation);
	return activation;
}

bool EnergyNeuronActivationFunction::equals(ActivationFunction *activationFunction) const {
	if(ActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	EnergyNeuronActivationFunction *af =
 			dynamic_cast<EnergyNeuronActivationFunction*>(activationFunction);

	if(af == 0) {
		return false;
	}
	return true;
}


}


