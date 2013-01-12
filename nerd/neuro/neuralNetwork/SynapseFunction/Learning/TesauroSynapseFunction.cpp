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

#include "TesauroSynapseFunction.h"
#include <iostream>
#include <Math/Math.h>
#include <Math/Random.h>
#include "Network/Neuron.h"
#include "Value/IntValue.h"


using namespace std;

namespace nerd {
	
	
	/**
	 * Constructor.
	 */
	TesauroSynapseFunction::TesauroSynapseFunction()
	: SynapseFunction("Hebb-Variants"), mPreviousPresynapticActivity(0), mPreviousPostSynapticActivity(0),
		mPrevPrevPresynapticActivity(0)
	{
		mInitialValue = new RangeValue(0.0, 0.1);
		mLearningRate = new DoubleValue(0.1);
		mMode = new IntValue(0);
		
		mMode->setDescription("Mode:\n"
							  "0: Hebb: epsilon * Vi(t) * Vj(t)   (i pre, j post)\n"
							  "1: Tesauro-c: epsilon * Vi(t-1) * (Vj(t) - f(Vj(t-1))\n"
							  "2: Tesauro-d: epsioon * sigma * (Vi(t-1) - Vi(t-2)) * (Vj(t) - f(Vj(t-1)))\n"
							  "3: Barto & Sutton: epsilon * Vi(t-1) * (Vj(t) - Vj(t-1))\n"
							  "4: GHT: epsilon * Vi(t) * (Vj(t) - Vj(t-1))\n");
		
		addParameter("Init", mInitialValue);
		addParameter("Rate", mLearningRate);
		addParameter("Mode", mMode);
	}
	
	
	/**
	 * Copy constructor.
	 */
	TesauroSynapseFunction::TesauroSynapseFunction(const TesauroSynapseFunction &other)
	: Object(), ValueChangedListener(), SynapseFunction(other), 
		mPreviousPresynapticActivity(other.mPreviousPresynapticActivity),
		mPreviousPostSynapticActivity(other.mPreviousPostSynapticActivity),
		mPrevPrevPresynapticActivity(other.mPrevPrevPresynapticActivity)
	{
		mInitialValue = dynamic_cast<RangeValue*>(getParameter("Init"));
		mLearningRate = dynamic_cast<DoubleValue*>(getParameter("Rate"));
		mMode = dynamic_cast<IntValue*>(getParameter("Mode"));
	}
	
	
	/**
	 * Destructor.
	 */
	TesauroSynapseFunction::~TesauroSynapseFunction() {
	}
	
	SynapseFunction* TesauroSynapseFunction::createCopy() const {
		return new TesauroSynapseFunction(*this);
	}
	
	
	/**
	 * Does nothing in this implementation.
	 */
	void TesauroSynapseFunction::reset(Synapse *synapse) {
		//reset to default value... (randomly in the range given by mInitialValue
		if(synapse != 0) {
			synapse->getStrengthValue().set(
				Random::nextDoubleBetween(mInitialValue->getMin(), 
										  mInitialValue->getMax()));
		}
		mPreviousPresynapticActivity = 0.0;
		mPrevPrevPresynapticActivity = 0.0;
		mPreviousPostSynapticActivity = 0.0;
	}
	
	
	/**
	 * Returns the strength of the owner synapse.
	 * 
	 * @param owner the owner of this SynapseFunction.
	 * @return the strength of the owner.
	 */
	double TesauroSynapseFunction::calculate(Synapse *owner) {
		if(owner == 0) {
			return 0.0;
		}
		Neuron *preSynapticNeuron = owner->getSource();
		Neuron *postSynapticNeuron = dynamic_cast<Neuron*>(owner->getTarget());
		
		if(preSynapticNeuron == 0 || postSynapticNeuron == 0) {
			return 0.0;
		}
		
		double change = 0.0;
		
		if(mMode->get() == 0) {
			//1: hebb: epsilon * Vi(t) * Vj(t)  (i pre, j post)
			change = mLearningRate->get()
							* preSynapticNeuron->getLastOutputActivation()
							* postSynapticNeuron->getLastOutputActivation();
		}
		else if(mMode->get() == 1) {
		
			//1: tesauro-c: epsilon * Vi(t-1) * (Vj(t) - f(Vj(t-1))  (i pre, j post)
			change = mLearningRate->get()
							* mPreviousPresynapticActivity
							* (postSynapticNeuron->getLastOutputActivation() 
							 		- applyExpectationFunction(mPreviousPostSynapticActivity));
		}
		else if(mMode->get() == 2) {
			
			//2: tesauro-d: sigma * (Vi(t-1) - Vi(t-2)) * (Vj(t) - f(Vj(t-1))
			//sigma has to be adjusted as part of the leraning rate 
			change = mLearningRate->get()
					* (mPreviousPresynapticActivity - mPrevPrevPresynapticActivity) 
					* (postSynapticNeuron->getLastOutputActivation() 
							- applyExpectationFunction(mPreviousPostSynapticActivity));
		}
		else if(mMode->get() == 3) {
			//"3: Barto & Sutton: epsilon * Vi(t-1) * (Vj(t) - Vj(t-1))\n"
			change = mLearningRate->get()
					* mPreviousPresynapticActivity
					* (postSynapticNeuron->getLastOutputActivation() 
							- mPreviousPostSynapticActivity);
		}
		else if(mMode->get() == 4) {
			//"4: GHT: epsilon * Vi(t) * (Vj(t) - Vj(t-1))\n");
			change = mLearningRate->get()
					* preSynapticNeuron->getLastOutputActivation()
					* (postSynapticNeuron->getLastOutputActivation() 
							- mPreviousPostSynapticActivity);
		}
		
		owner->getStrengthValue().set(owner->getStrengthValue().get() + change);
		
		mPrevPrevPresynapticActivity = mPreviousPresynapticActivity;
		mPreviousPresynapticActivity = preSynapticNeuron->getLastOutputActivation();
		mPreviousPostSynapticActivity = postSynapticNeuron->getLastOutputActivation();
		
		return owner->getStrengthValue().get();
	}
	
	
	
	bool TesauroSynapseFunction::equals(SynapseFunction *synapseFunction) const {
		if(SynapseFunction::equals(synapseFunction) == false) {
			return false;
		}
		
		TesauroSynapseFunction *sf = dynamic_cast<TesauroSynapseFunction*>(synapseFunction);
		
		if(sf == 0) {
			return false;
		}
		if(!sf->mInitialValue->equals(mInitialValue)) {
			return false;
		}
		if(!sf->mLearningRate->equals(mLearningRate)) {
			return false;
		}
		if(!sf->mMode->equals(mMode)) {
			return false;
		}
		return true;
	}
	
	double TesauroSynapseFunction::applyExpectationFunction(double x) {
		
		return Math::min(1.0, x + 0.1);  //tessauro: f(x) = x + ny (with ny = 0.1)
	}
	
	
}


