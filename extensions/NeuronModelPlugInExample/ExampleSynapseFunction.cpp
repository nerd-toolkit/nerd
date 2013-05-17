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

#include "ExampleSynapseFunction.h"
#include <iostream>
#include <Network/NeuralNetwork.h>
#include "Value/RangeValue.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"

using namespace std;

namespace nerd {
	
	
/**
 * Constructor.
 */
ExampleSynapseFunction::ExampleSynapseFunction()
	: SynapseFunction("ExampleSynapseFunction")
{
	mHistorySize = new IntValue(500);
	mDesiredActivationRange = new RangeValue(0.3, 0.8);
	mChangeRate = new DoubleValue(0.0001);
	mCurrentAverage = new DoubleValue(0.0);
	
	mHistorySize->setDescription("The size of the sliding window over which the activation is averaged.");
	mDesiredActivationRange->setDescription("The desired activation range.");
	mChangeRate->setDescription("The rate of change when learning takes lace.");
	
	addParameter("HistorySize", mHistorySize);
	addParameter("DesiredRange", mDesiredActivationRange);
	addParameter("ChangeRate", mChangeRate);
	
	addObservableOutput("CurrentAverage", mCurrentAverage);
}


/**
 * Copy constructor.
 */
ExampleSynapseFunction::ExampleSynapseFunction(const ExampleSynapseFunction &other)
	: Object(), ValueChangedListener(), SynapseFunction(other)
{
	mHistorySize = dynamic_cast<IntValue*>(getParameter("HistorySize"));
	mDesiredActivationRange = dynamic_cast<RangeValue*>(getParameter("DesiredRange"));
	mChangeRate = dynamic_cast<DoubleValue*>(getParameter("ChangeRate"));
	
	mCurrentAverage = dynamic_cast<DoubleValue*>(other.mCurrentAverage->createCopy());
	
	addObservableOutput("CurrentAverage", mCurrentAverage);
}


/**
 * Destructor.
 */
ExampleSynapseFunction::~ExampleSynapseFunction() {
}

SynapseFunction* ExampleSynapseFunction::createCopy() const {
	return new ExampleSynapseFunction(*this);
}


/**
 * Does nothing in this implementation.
 */
void ExampleSynapseFunction::reset(Synapse*) {
	
	mHistory.clear();
	//mHistory.reserve(mHistorySize->get() + 1);
	
	mCurrentAverage->set(0.0);
}


/**
 * Returns the strength of the owner synapse.
 * 
 * @param owner the owner of this SynapseFunction.
 * @return the strength of the owner.
 */
double ExampleSynapseFunction::calculate(Synapse *owner) {
	if(owner == 0) {
		return 0.0;
	}
	
	//Search for target neuron (if existing)
	Neuron *target = dynamic_cast<Neuron*>(owner->getTarget());
	if(target == 0) {
		return owner->getStrengthValue().get();
	}
	
	//Update history
	mHistory.enqueue(target->getLastActivation());
	while(!mHistory.empty() && mHistory.size() > mHistorySize->get()) {
		mHistory.dequeue();
	}
	
	
	//Calcualte average
	double activationSum = 0.0;
	double average = 0.0;
	for(QListIterator<double> i(mHistory); i.hasNext();) {
		activationSum += i.next();
	}
	if(mHistory.size() > 0) {
		average = (activationSum / ((double) mHistory.size()));
	}
	else {
		average = 0.0;
	}
	mCurrentAverage->set(average);
	
	//Adapt synapse if required.
	Neuron *source = owner->getSource();
	if(source != 0) {
		double output = source->getLastOutputActivation();
		if(output != 0) {
			double change = 0.0;
			if(average > mDesiredActivationRange->getMax()) {
				change = (average - mDesiredActivationRange->getMax()) * output * -1.0;
			}
			else if(average < mDesiredActivationRange->getMin()) {
				change = (mDesiredActivationRange->getMin() - average) * output;
			}
			owner->getStrengthValue().set(owner->getStrengthValue().get() + (change * mChangeRate->get()));
		}
	}
	
	return owner->getStrengthValue().get();
}
	

	
}


