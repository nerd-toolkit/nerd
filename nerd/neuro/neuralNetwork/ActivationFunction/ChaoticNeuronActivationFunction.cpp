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



#include "ChaoticNeuronActivationFunction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <Value/IntValue.h>
#include "Network/Neuron.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new ChaoticNeuronActivationFunction.
 */
ChaoticNeuronActivationFunction::ChaoticNeuronActivationFunction()
	: ActivationFunction("Chaotic")
{
	mGamma = new DoubleValue(0.9);
	mMode = new IntValue(0);
	
	addParameter("Gamma", mGamma);
	addParameter("Mode", mMode);
	
	mMode->setDescription("Update Mode:\n"
							"(0) a(t+1) = (gamma * a(t)) + ((1 - gamma) * input)\n"
							"(1) a(t+1) = (gamma * a(t) + input");
}


/**
 * Copy constructor. 
 * 
 * @param other the ChaoticNeuronActivationFunction object to copy.
 */
ChaoticNeuronActivationFunction::ChaoticNeuronActivationFunction(
						const ChaoticNeuronActivationFunction &other) 
	: ObservableNetworkElement(other), ActivationFunction(other)
{
	mGamma = dynamic_cast<DoubleValue*>(getParameter("Gamma"));
	mMode = dynamic_cast<IntValue*>(getParameter("Mode"));
}

/**
 * Destructor.
 */
ChaoticNeuronActivationFunction::~ChaoticNeuronActivationFunction() {
}


ActivationFunction* ChaoticNeuronActivationFunction::createCopy() const {
	return new ChaoticNeuronActivationFunction(*this);
}

void ChaoticNeuronActivationFunction::reset(Neuron*) {
}

double ChaoticNeuronActivationFunction::calculateActivation(Neuron *owner) {
	if(owner == 0) {
		return 0.0;
	}

	double activation = owner->getActivationValue().get();

	double externalInput = owner->getBiasValue().get();

	QList<Synapse*> synapses = owner->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		externalInput += i.next()->calculateActivation();
	}
	if(mMode->get() == 0) {
		return (mGamma->get() * activation) + ((1.0 - mGamma->get()) * externalInput);
	}
	else {
		return (mGamma->get() * activation) + externalInput;
	}
}


bool ChaoticNeuronActivationFunction::equals(ActivationFunction *activationFunction) const {
	if(ActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	ChaoticNeuronActivationFunction *af =
 			dynamic_cast<ChaoticNeuronActivationFunction*>(activationFunction);

	if(af == 0) {
		return false;
	}
	if(af->mGamma->get() != mGamma->get()) {
		return false;
	}
	if(af->mMode->get() != mMode->get()) {
		return false;
	}
	return true;
}

}



