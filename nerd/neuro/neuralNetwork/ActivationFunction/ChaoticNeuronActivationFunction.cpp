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



#include "ChaoticNeuronActivationFunction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuron.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new ChaoticNeuronActivationFunction.
 */
ChaoticNeuronActivationFunction::ChaoticNeuronActivationFunction()
	: ActivationFunction("Chaotic")
{
	mDecay = new DoubleValue(0.99);
	
	addParameter("Decay", mDecay);
}


/**
 * Copy constructor. 
 * 
 * @param other the ChaoticNeuronActivationFunction object to copy.
 */
ChaoticNeuronActivationFunction::ChaoticNeuronActivationFunction(
						const ChaoticNeuronActivationFunction &other) 
	: ActivationFunction(other)
{
	mDecay = dynamic_cast<DoubleValue*>(getParameter("Decay"));
}

/**
 * Destructor.
 */
ChaoticNeuronActivationFunction::~ChaoticNeuronActivationFunction() {
}


ActivationFunction* ChaoticNeuronActivationFunction::createCopy() const {
	return new ChaoticNeuronActivationFunction(*this);
}

void ChaoticNeuronActivationFunction::reset(Neuron *owner) {
}

double ChaoticNeuronActivationFunction::calculateActivation(Neuron *owner) {
	if(owner == 0) {
		return 0.0;
	}

	double activation = owner->getActivationValue().get() * mDecay->get();

	activation += owner->getBiasValue().get();

	QList<Synapse*> synapses = owner->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		activation += i.next()->calculateActivation();
	}
	return activation;
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
	if(af->mDecay->get() != mDecay->get()) {
		return false;
	}
	return true;
}

}


