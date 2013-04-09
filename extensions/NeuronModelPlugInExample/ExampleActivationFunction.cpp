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

#include "ExampleActivationFunction.h"
#include <QListIterator>
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <iostream>

using namespace std;

namespace nerd {

/**
 * Standard Constructor.
 * Give your ActivationFunction a propert type name in the constructor call of the superclass.
 * Here, also create all parameters and add them properly with a clear name.
 * Dont forget a documentiation string for each parameter.
 * If you want to have internal parameters to be observable with timeseries plots, then add
 * them (maybe additional) as Observable (addObservable).
 */
ExampleActivationFunction::ExampleActivationFunction()
	: ActivationFunction("ExampleActivationFunction")
{
	mFactor = new DoubleValue(1.0);
	mInnerState = new DoubleValue(0.0);
	
	mFactor->setDescription("A factor to multiply the activation with.");
	mInnerState->setDescription("Just an inner state to observe.");
	
	//add a parameter that can be changed by the user in the network editor.
	addParameter("Factor", mFactor);
	
	
	//add an observable that can be plotted with a ParameterPlotter in the editor.
	addObserableOutput("InnerState", mInnerState);
	
}


/**
 * Copy Constructor.
 * Always provide this in the given format.
 * Make sure that you reassin all parameters added with addParameters() in the standard constructor
 * here again with exactly the same name (getParameter()) and that you cast it to exactly the correct
 * Value type! Otherwise the function will not work properly and you may even get segmentation faults.
 * 
 * Also, make sure that you register observable outputs manually again!
 */
ExampleActivationFunction::ExampleActivationFunction(
			const ExampleActivationFunction &other)
	: Object(), ValueChangedListener(), ObservableNetworkElement(other), ActivationFunction(other)
{
	//make sure to get the locally copied parameter and assin it to the local varibable for later use.
	mFactor = dynamic_cast<DoubleValue*>(getParameter("Factor"));
	
	//here, we have to copy the value manually, because mInnerState is not an automatically copied parameter.
	mInnerState = dynamic_cast<DoubleValue*>(other.mInnerState->createCopy());
	
	//explicitly add the observable again.
	addObserableOutput("InnerState", mInnerState);
}


/**
 * Destructor.
 */
ExampleActivationFunction::~ExampleActivationFunction() {
}


/**
 * Creates a new copy of an existing object of this type.
 * Just make sure that this method calls the copy constructore with *this as parameter.
 */
ActivationFunction* ExampleActivationFunction::createCopy() const {
	return new ExampleActivationFunction(*this);
}


/**
 * Called when the network is reset.
 * This method can be used to reset variables, such as internal states.
 */
void ExampleActivationFunction::reset(Neuron*) {
	
	//Let's reset the inner state.
	mInnerState->set(0.0);
}


/**
 * Is called during each network update step to calculate the current activation of the 
 * neuron. The return value is interpreted as the current activation.
 */
double ExampleActivationFunction::calculateActivation(Neuron *owner) {
	ActivationFunction::calculateActivation(owner);
	
	if(owner == 0) {
		return 0.0;
	}
	double activation = owner->getBiasValue().get();

	QList<Synapse*> synapses = owner->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		activation += i.next()->calculateActivation();
	}
	
	//here, we increment the inner state.
	mInnerState->set(mInnerState->get() + 0.001);

	
	return (activation * mFactor->get());
}


}


