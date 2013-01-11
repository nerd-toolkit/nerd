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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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

#include "HebbSynapseFunction.h"
#include <iostream>
#include <Math/Math.h>
#include <Math/Random.h>
#include "Network/Neuron.h"


using namespace std;

namespace nerd {
	
	
	/**
	 * Constructor.
	 */
	HebbSynapseFunction::HebbSynapseFunction()
	: SynapseFunction("Hebb")
	{
		mInitialValue = new RangeValue(0.0, 0.1);
		mLearningRate = new DoubleValue(0.01);
		
		addParameter("Init", mInitialValue);
		addParameter("Rate", mLearningRate);
	}
	
	
	/**
	 * Copy constructor.
	 */
	HebbSynapseFunction::HebbSynapseFunction(const HebbSynapseFunction &other)
	: Object(), ValueChangedListener(), SynapseFunction(other)
	{
		mInitialValue = dynamic_cast<RangeValue*>(getParameter("Init"));
		mLearningRate = dynamic_cast<DoubleValue*>(getParameter("Rate"));
	}
	
	
	/**
	 * Destructor.
	 */
	HebbSynapseFunction::~HebbSynapseFunction() {
	}
	
	SynapseFunction* HebbSynapseFunction::createCopy() const {
		return new HebbSynapseFunction(*this);
	}
	
	
	/**
	 * Does nothing in this implementation.
	 */
	void HebbSynapseFunction::reset(Synapse *synapse) {
		//reset to default value... (randomly in the range given by mInitialValue
		if(synapse != 0) {
			synapse->getStrengthValue().set(
					Random::nextDoubleBetween(mInitialValue->getMin(), 
											  mInitialValue->getMax()));
		}
	}
	
	
	/**
	 * Returns the strength of the owner synapse.
	 * 
	 * @param owner the owner of this SynapseFunction.
	 * @return the strength of the owner.
	 */
	double HebbSynapseFunction::calculate(Synapse *owner) {
		if(owner == 0) {
			return 0.0;
		}
		Neuron *preSynapticNeuron = owner->getSource();
		Neuron *postSynapticNeuron = dynamic_cast<Neuron*>(owner->getTarget());
		
		if(preSynapticNeuron == 0 || postSynapticNeuron == 0) {
			return 0.0;
		}
		double change = mLearningRate->get()
				* preSynapticNeuron->getLastOutputActivation() 
				* postSynapticNeuron->getLastOutputActivation();

		owner->getStrengthValue().set(owner->getStrengthValue().get() + change);
		
		return owner->getStrengthValue().get();
	}
	
	bool HebbSynapseFunction::equals(SynapseFunction *synapseFunction) const {
		if(SynapseFunction::equals(synapseFunction) == false) {
			return false;
		}
		
		HebbSynapseFunction *sf = dynamic_cast<HebbSynapseFunction*>(synapseFunction);
		
		if(sf == 0) {
			return false;
		}
		if(!sf->mInitialValue->equals(mInitialValue)) {
			return false;
		}
		if(!sf->mLearningRate->equals(mLearningRate)) {
			return false;
		}
		return true;
	}
	
	
}


