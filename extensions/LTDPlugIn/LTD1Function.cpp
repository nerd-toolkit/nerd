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

#include "LTD1Function.h"
#include <QStringListIterator>
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <Network/NeuralNetwork.h>
#include <NeuroModulation/NeuroModulator.h>
#include <iostream>
#include <math.h>
#include "Math/Random.h"

using namespace std;

namespace nerd {

/**
 * Standard Constructor.
 * Give your SynapseFunction a propert type name in the constructor call of the superclass.
 * Here, also create all parameters and add them properly with a clear name.
 * Dont forget a documentiation string for each parameter.
 * If you want to have internal parameters to be observable with timeseries plots, then add
 * them (maybe additional) as Observable (addObservable).
 */
LTD1Function::LTD1Function()
	: SynapseFunction("LTD1")
{
  mModTypes = new StringValue("1");
	mDelta = new DoubleValue(0.01);
	mRecoveryRate = new DoubleValue(0.001);
  mResetWeights = new BoolValue(false);

	mModTypes->setDescription("Please enter a space seperated list of Integers");
	mDelta->setDescription("");	
	mRecoveryRate->setDescription("Recovery rate");
  mResetWeights->setDescription("If true the weights are reinitialized at each reset.");

	//add a parameter that can be changed by the user in the network editor.
  addParameter("modTypes", mModTypes);
	addParameter("delta",mDelta);
	addParameter("recovery rate",mRecoveryRate);
  addParameter("resetWeights",mResetWeights);
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
LTD1Function::LTD1Function(
			const LTD1Function &other)
	: Object(), ValueChangedListener(), SynapseFunction(other)
{
	//make sure to get the locally copied parameter and assign it to the local varibable for later use.
	mModTypes = dynamic_cast<StringValue*>(getParameter("modTypes"));
	mDelta = dynamic_cast<DoubleValue*>(getParameter("delta"));
	mRecoveryRate = dynamic_cast<DoubleValue*>(getParameter("recovery rate"));
  mResetWeights = dynamic_cast<BoolValue*>(getParameter("resetWeights"));
}


/**
 * Destructor.
 */
LTD1Function::~LTD1Function() {
}


/**
 * Creates a new copy of an existing object of this type.
 * Just make sure that this method calls the copy constructore with *this as parameter.
 */
SynapseFunction* LTD1Function::createCopy() const {
	return new LTD1Function(*this);
}


/**
 * Called when the network is reset.
 * This method can be used to reset variables, such as internal states.
 */
void LTD1Function::reset(Synapse *owner) {
  if(mResetWeights->get()){
    owner->getStrengthValue().set(0.5+Random::nextDouble()*0.5); 
  }
}


/**
 * Returns the strength of the owner synapse.
 * 
 * @param owner the owner of this SynapseFunction.
 * @return the strength of the owner.
 */

double LTD1Function::calculate(Synapse *owner) {
  DoubleValue *weight = &owner->getStrengthValue();
  double w = weight->get(); 

  double c = 0;
  QStringListIterator modTypesIterator(mModTypes->get().split(","));
  while(modTypesIterator.hasNext()){
    c += NeuroModulator::getConcentrationInNetworkAt(
            modTypesIterator.next().toInt(), owner->getPosition(),
            owner->getSource()->getOwnerNetwork()
          );  
  }
  
  double as = ceil(owner->getSource()->getLastOutputActivation());
  Neuron* target = dynamic_cast<Neuron*>(owner->getTarget());
  
  double at = 1;
  if(target != 0){
    at = ceil(target->getLastOutputActivation());
  }
  
  w = w * (1- c*as*at*mDelta->get()*Random::nextDouble());
  w = w + (1-w)*mRecoveryRate->get();
  
  weight->set(w);
  
  return w;
}


}


