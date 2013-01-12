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

#include "CloneSimpleSynapseFunction.h"
#include <iostream>
#include "Value/ULongLongValue.h"
#include <Network/NeuralNetwork.h>

using namespace std;

namespace nerd {
	
	
	/**
	 * Constructor.
	 */
	CloneSimpleSynapseFunction::CloneSimpleSynapseFunction()
	: SimpleSynapseFunction(), mTargetSynapse(0)
	{
		setName("SimpleClone");
		mTargetId = new ULongLongValue();
		
		addParameter("TargetId", mTargetId);
	}
	
	
	/**
	 * Copy constructor.
	 */
	CloneSimpleSynapseFunction::CloneSimpleSynapseFunction(const CloneSimpleSynapseFunction &other)
	: Object(), ValueChangedListener(), SimpleSynapseFunction(other), mTargetSynapse(0)
	{
		mTargetId = dynamic_cast<ULongLongValue*>(getParameter("TargetId"));
	}
	
	
	/**
	 * Destructor.
	 */
	CloneSimpleSynapseFunction::~CloneSimpleSynapseFunction() {
	}
	
	SynapseFunction* CloneSimpleSynapseFunction::createCopy() const {
		return new CloneSimpleSynapseFunction(*this);
	}
	
	
	/**
	 * Does nothing in this implementation.
	 */
	void CloneSimpleSynapseFunction::reset(Synapse*) {
		mTargetSynapse = 0;
	}
	
	
	/**
	 * Returns the strength of the owner synapse.
	 * 
	 * @param owner the owner of this SynapseFunction.
	 * @return the strength of the owner.
	 */
	double CloneSimpleSynapseFunction::calculate(Synapse *owner) {
		if(owner == 0) {
			return 0.0;
		}
		if(mTargetId->get() == 0) {
			//per default set the id to the own synapse.
			mTargetId->set(owner->getId());
		}
		if(mLastKnownTargetId != mTargetId->get()) {
			mTargetSynapse = 0;
		}
		mLastKnownTargetId = mTargetId->get();
		if(mTargetId->get() == owner->getId()) {
			mTargetSynapse = owner;
		}
		else {
			Neuron *neuron = owner->getSource();
			QList<Synapse*> synapses;
			if(neuron != 0) {
				NeuralNetwork *network = neuron->getOwnerNetwork();
				if(network != 0) {
					synapses = network->getSynapses();
				}
			}
			if(mTargetSynapse == 0) {
				mTargetSynapse = NeuralNetwork::selectSynapseById(mTargetId->get(), synapses);
			}
			if(mTargetSynapse != 0) {
				if(!synapses.contains(mTargetSynapse)) {
					mTargetSynapse = 0;
				}
			}
			if(mTargetSynapse != 0) {
				owner->getStrengthValue().set(mTargetSynapse->getStrengthValue().get());
			}
		}
		
		return SimpleSynapseFunction::calculate(owner);
	}
	
	bool CloneSimpleSynapseFunction::equals(SynapseFunction *synapseFunction) const {
		if(SimpleSynapseFunction::equals(synapseFunction) == false) {
			return false;
		}
		
		CloneSimpleSynapseFunction *sf = dynamic_cast<CloneSimpleSynapseFunction*>(synapseFunction);
		
		if(sf == 0) {
			return false;
		}
		return true;
	}
	
	
}


