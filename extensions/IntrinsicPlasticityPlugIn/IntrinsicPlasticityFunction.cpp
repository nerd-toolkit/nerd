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

#include "IntrinsicPlasticityFunction.h"
#include <QListIterator>
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <iostream>
#include <math.h>

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
IntrinsicPlasticityFunction::IntrinsicPlasticityFunction()
	: ActivationFunction("IntrinsicPlasticityFunction")
{
	mA = new DoubleValue(1.0);
        mB = new DoubleValue(0.0);
	mEta = new DoubleValue(0.001);
	mMu = new DoubleValue(0.1);
	mMode = new IntValue(0);
	
	mA->setDescription("Slope of the transfer function");
	mB->setDescription("Shift of the transfer function");
	mEta->setDescription("Intrinsic plasticity learning rate");	
	mMu->setDescription("Target mean activity");
	mMode->setDescription("Switches target output distribution.\n (0) for exponential");

	//add a parameter that can be changed by the user in the network editor.
	addParameter("a", mA);
        addParameter("b", mB);
	addParameter("eta",mEta);
	addParameter("mu",mMu);
	addParameter("Mode",mMode);
	
	//add an observable that can be plotted with a ParameterPlotter in the editor.
	addObservableOutput("a", mA);
	addObservableOutput("b", mB);
	
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
IntrinsicPlasticityFunction::IntrinsicPlasticityFunction(
			const IntrinsicPlasticityFunction &other)
	: Object(), ValueChangedListener(), ObservableNetworkElement(other), ActivationFunction(other)
{
	//make sure to get the locally copied parameter and assin it to the local varibable for later use.
	mA = dynamic_cast<DoubleValue*>(getParameter("a"));
	mB = dynamic_cast<DoubleValue*>(getParameter("b"));
	mEta = dynamic_cast<DoubleValue*>(getParameter("eta"));
	mMu = dynamic_cast<DoubleValue*>(getParameter("mu"));
	mMode = dynamic_cast<IntValue*>(getParameter("Mode"));
	
	//explicitly add the observable again.
	addObservableOutput("a", mA);
	addObservableOutput("b", mB);
}


/**
 * Destructor.
 */
IntrinsicPlasticityFunction::~IntrinsicPlasticityFunction() {
}


/**
 * Creates a new copy of an existing object of this type.
 * Just make sure that this method calls the copy constructore with *this as parameter.
 */
ActivationFunction* IntrinsicPlasticityFunction::createCopy() const {
	return new IntrinsicPlasticityFunction(*this);
}


/**
 * Called when the network is reset.
 * This method can be used to reset variables, such as internal states.
 */
void IntrinsicPlasticityFunction::reset(Neuron*) {
	
  //We have no inner State to take care of	
}


/**
 * Is called during each network update step to calculate the current activation of the 
 * neuron. The return value is interpreted as the current activation.
 */
double IntrinsicPlasticityFunction::calculateActivation(Neuron *owner) {
	ActivationFunction::calculateActivation(owner);
  	
	double aValue = mA->get();
	double bValue = mB->get();
	double muValue = mMu ->get();
	double etaValue = mEta ->get();
		
	if(owner == 0) {
		return 0.0;
	}
	
	double activation = owner->getBiasValue().get();

	QList<Synapse*> synapses = owner->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		activation += i.next()->calculateActivation();
	}
	
	double y = 1.0/(1.0+exp(-(aValue*activation+bValue)));
	double term =-(2+1/muValue)*y+(1/muValue)*y*y;
	aValue += etaValue*(1.0/aValue+activation+activation*term);
	bValue += etaValue*(1.0+term);
	
	mA->set(aValue);
	mB->set(bValue);
	
	return y;
}


}


