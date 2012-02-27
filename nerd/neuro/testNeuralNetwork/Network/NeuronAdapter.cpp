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

#include "NeuronAdapter.h"

namespace nerd {

NeuronAdapter::NeuronAdapter(const QString &name, const TransferFunction &tf, 
							 const ActivationFunction &af, bool *destroyFlag)
	: Neuron(name, tf, af), mPrepareCounter(0), mResetCounter(0),
	  mUpdateActivationCounter(0), mGetActivationValueCounter(0),
	  mLastActivationReturnValue(0), mLastOutputActivationReturnValue(0.0),
	  mDeletedFlag(destroyFlag)
{
}

NeuronAdapter::NeuronAdapter(const NeuronAdapter &other)
	: Neuron(other), mPrepareCounter(0), mResetCounter(0),
	  mUpdateActivationCounter(0), mGetActivationValueCounter(0),
	  mLastActivationReturnValue(0), mLastOutputActivationReturnValue(0.0), 
	  mDeletedFlag(0)
{
}

NeuronAdapter::~NeuronAdapter() {
	if(mDeletedFlag != 0) {
		(*mDeletedFlag) = true;
	}
}


Neuron* NeuronAdapter::createCopy() const {
	return new NeuronAdapter(*this);
}


void NeuronAdapter::prepare() {
	mPrepareCounter++;
	Neuron::prepare();
}


double NeuronAdapter::getLastActivation() const {
	return mLastActivationReturnValue;
}

double NeuronAdapter::getLastOutputActivation() const {
	return mLastOutputActivationReturnValue;
}


void NeuronAdapter::reset() {
	mResetCounter++;
	Neuron::reset();
}


void NeuronAdapter::updateActivation() {
	mUpdateActivationCounter++;
	Neuron::updateActivation();
}



DoubleValue& NeuronAdapter::getActivationValue() {
	mGetActivationValueCounter++;
	return Neuron::getActivationValue();
}

}



