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

#include "Izhikevitch2003SpikingActivationFunction.h"
#include <QListIterator>
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <Math/Math.h>
#include <iostream>
#include <math.h>

using namespace std;

namespace nerd {

Izhikevitch2003SpikingActivationFunction::Izhikevitch2003SpikingActivationFunction()
	: ActivationFunction("Izhikevitch2003")
{
	mMembranePotential_v = new DoubleValue(-70.0);
	mMembraneRecovery_u = new DoubleValue(-14.0);
	mTimeScaleRecovery_a = new DoubleValue(0.02);
	mSubthresholdSensitivityRelation_b = new DoubleValue(0.2);
	mRestValueAfterSpike_c = new DoubleValue(-65.0);
	mSpikingEffectOnU_d = new DoubleValue(8.0);
	mStepSize_dt = new DoubleValue(0.1);
	mX = new DoubleValue(5.0);
	mY = new DoubleValue(140.0);
	mRecoveryUpdateMode_du = new IntValue(0);
	mInputCurrent = new DoubleValue(0.0);
	
	addParameter("v", mMembranePotential_v);
	mMembranePotential_v->setDescription("Membrane potential (mV)");
	addParameter("u", mMembraneRecovery_u);
	mMembraneRecovery_u->setDescription("Membrane recovery");
	addParameter("a", mTimeScaleRecovery_a);
	mTimeScaleRecovery_a->setDescription("Time scale of u");
	addParameter("b", mSubthresholdSensitivityRelation_b);
	mSubthresholdSensitivityRelation_b->setDescription("Subthreshold sensity relation between u and v");
	addParameter("c", mRestValueAfterSpike_c);
	mRestValueAfterSpike_c->setDescription("Rest value of v after spike");
	addParameter("d", mSpikingEffectOnU_d);
	mSpikingEffectOnU_d->setDescription("Effect of spike on u at reset");
	addParameter("dt", mStepSize_dt);
	mStepSize_dt->setDescription("Update step size in ms");
	addParameter("x", mX);
	mX->setDescription("0.04 v^2 + x v + y - u + I");
	addParameter("y", mY);
	mX->setDescription("0.04 v^2 + x v + y - u + I");
	addParameter("du Mode", mRecoveryUpdateMode_du);
	mRecoveryUpdateMode_du->setDescription("Update modus for du:\n"
					"0: du = a (b v - u)");
	
	addObservableOutput("v", mMembranePotential_v);
	addObservableOutput("u", mMembraneRecovery_u);
	addObservableOutput("I", mInputCurrent);
	
}

Izhikevitch2003SpikingActivationFunction::Izhikevitch2003SpikingActivationFunction(
			const Izhikevitch2003SpikingActivationFunction &other)
	: Object(), ValueChangedListener(), ObservableNetworkElement(other), ActivationFunction(other)
{
	mMembranePotential_v = dynamic_cast<DoubleValue*>(getParameter("v"));
	mMembraneRecovery_u = dynamic_cast<DoubleValue*>(getParameter("u"));
	mTimeScaleRecovery_a = dynamic_cast<DoubleValue*>(getParameter("a"));
	mSubthresholdSensitivityRelation_b = dynamic_cast<DoubleValue*>(getParameter("b"));
	mRestValueAfterSpike_c = dynamic_cast<DoubleValue*>(getParameter("c"));
	mSpikingEffectOnU_d = dynamic_cast<DoubleValue*>(getParameter("d"));
	mStepSize_dt = dynamic_cast<DoubleValue*>(getParameter("dt"));
	mX = dynamic_cast<DoubleValue*>(getParameter("x"));
	mY = dynamic_cast<DoubleValue*>(getParameter("y"));
	mRecoveryUpdateMode_du = dynamic_cast<IntValue*>(getParameter("du Mode"));
	
	mInputCurrent = new DoubleValue(0.0);
	
	addObservableOutput("v", mMembranePotential_v);
	addObservableOutput("u", mMembraneRecovery_u);
	addObservableOutput("I", mInputCurrent);
}

Izhikevitch2003SpikingActivationFunction::~Izhikevitch2003SpikingActivationFunction() {
}

ActivationFunction* Izhikevitch2003SpikingActivationFunction::createCopy() const {
	return new Izhikevitch2003SpikingActivationFunction(*this);
}

void Izhikevitch2003SpikingActivationFunction::reset(Neuron*) {
	mMembranePotential_v->set(-70.0);
	mMembraneRecovery_u->set(mSubthresholdSensitivityRelation_b->get() 
								* mMembranePotential_v->get());
	mInputCurrent->set(0.0);
}


double Izhikevitch2003SpikingActivationFunction::calculateActivation(Neuron *owner) {
	ActivationFunction::calculateActivation(owner);
	if(owner == 0) {
		return 0.0;
	}
	double activation = owner->getBiasValue().get();

	QList<Synapse*> synapses = owner->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		activation += i.next()->calculateActivation();
	}
	
	mInputCurrent->set(activation);
	
	double v = mMembranePotential_v->get();
	
	if(v >= 30.0) {
		mMembranePotential_v->set(mRestValueAfterSpike_c->get());
		mMembraneRecovery_u->set(mMembraneRecovery_u->get() + mSpikingEffectOnU_d->get());
		v = mMembranePotential_v->get();
	}
	
	double u = mMembraneRecovery_u->get();
	
	v = Math::min(30.0, 
					v + (mStepSize_dt->get() 
							* (0.04 * v * v
								+ mX->get() * v
								+ mY->get() 
								- u
								+ activation)));
	mMembranePotential_v->set(v);

	
	if(mRecoveryUpdateMode_du->get() == 1) {
		
	}
	else {
		u = u + (mStepSize_dt->get() 
					* (mTimeScaleRecovery_a->get() * 
						(mSubthresholdSensitivityRelation_b->get() * v - u)));
		mMembraneRecovery_u->set(u);
	}
	
	
// a*(b*v - u)
// 	n.v += self.dt * (0.04*n.v**2 + self.x*n.v + self.y - n.u + self.stim[i])
//       n.u += self.dt * self.du(n.a, n.b, n.v, n.u)
//       if n.v > 30:
//         trace[0,i] = 30
//         n.v        = n.c
//         n.u       += n.d
//       else:
//         trace[0,i] = n.v
//         trace[1,i] = n.u
//     return trace

	return mMembranePotential_v->get();
}

bool Izhikevitch2003SpikingActivationFunction::equals(ActivationFunction *activationFunction) const {
	if(ActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	Izhikevitch2003SpikingActivationFunction *af =
 			dynamic_cast<Izhikevitch2003SpikingActivationFunction*>(activationFunction);

	if(af == 0) {
		return false;
	}
	return true;
}


}


