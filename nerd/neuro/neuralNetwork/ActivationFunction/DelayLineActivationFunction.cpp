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



#include "DelayLineActivationFunction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new DelayLineActivationFunction.
 */
DelayLineActivationFunction::DelayLineActivationFunction()
	: ActivationFunction("DelayLine")
{
	mDelay = new IntValue(2);

	addParameter("Delay", mDelay);
}


/**
 * Copy constructor. 
 * 
 * @param other the DelayLineActivationFunction object to copy.
 */
DelayLineActivationFunction::DelayLineActivationFunction(const DelayLineActivationFunction &other)
	: Object(), ValueChangedListener(), ActivationFunction(other)
{
	mDelay = dynamic_cast<IntValue*>(getParameter("Delay"));
}

/**
 * Destructor.
 */
DelayLineActivationFunction::~DelayLineActivationFunction() {
}

ActivationFunction* DelayLineActivationFunction::createCopy() const {
	return new DelayLineActivationFunction(*this);
}

void DelayLineActivationFunction::reset(Neuron*) {
	mDelayedActivations.clear();
}


double DelayLineActivationFunction::calculateActivation(Neuron *owner) {
	if(owner == 0) {
		return 0.0;
	}
	double activation = owner->getBiasValue().get();

	QList<Synapse*> synapses = owner->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		activation += i.next()->calculateActivation();
	}

	int delay = mDelay->get();

	mDelayedActivations.append(activation);
	while(mDelayedActivations.size() > delay + 1) {
		mDelayedActivations.takeFirst();
	}
	if(mDelayedActivations.size() == delay + 1) {
		return mDelayedActivations.takeFirst();
	}
	return 0.0;
}

bool DelayLineActivationFunction::equals(ActivationFunction *activationFunction) const {
	if(ActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	DelayLineActivationFunction *af =
 			dynamic_cast<DelayLineActivationFunction*>(activationFunction);

	if(af == 0) {
		return false;
	}
	if(af->mDelay->get() != mDelay->get()) {
		return false;
	}
	return true;
}



}



