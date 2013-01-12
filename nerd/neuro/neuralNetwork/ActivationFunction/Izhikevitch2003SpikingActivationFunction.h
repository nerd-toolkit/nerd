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


#ifndef NERDIzhikevitch2003SpikingActivationFunction_H
#define NERDIzhikevitch2003SpikingActivationFunction_H

#include "ActivationFunction/ActivationFunction.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"

namespace nerd {

	/**
	 * Izhikevitch2003SpikingActivationFunction.
	 * Implementation of spiking neuron model proposed in Izhikevitch 2003
	 * (IEEE Transactions on Neural Networks, Vol. 14, No. 6, Nov. 2003)
	 * 
	 * Implemenation is a C++ port of the phython code published at
	 * http://www.neurdon.com/2011/02/02/neural-modeling-with-python-part-3/
	 */
	class Izhikevitch2003SpikingActivationFunction : public ActivationFunction {
	public:
		Izhikevitch2003SpikingActivationFunction();
		Izhikevitch2003SpikingActivationFunction(const Izhikevitch2003SpikingActivationFunction &other);
		virtual ~Izhikevitch2003SpikingActivationFunction();

		virtual ActivationFunction* createCopy() const;

		virtual void reset(Neuron *owner);
		virtual double calculateActivation(Neuron *owner);

		bool equals(ActivationFunction *activationFunction) const;
		
	private:
		DoubleValue *mMembranePotential_v;
		DoubleValue *mMembraneRecovery_u;
		DoubleValue *mTimeScaleRecovery_a;
		DoubleValue *mSubthresholdSensitivityRelation_b;
		DoubleValue *mRestValueAfterSpike_c;
		DoubleValue *mSpikingEffectOnU_d;
		DoubleValue *mStepSize_dt;
		DoubleValue *mX;
		DoubleValue *mY;
		IntValue *mRecoveryUpdateMode_du;
		DoubleValue *mInputCurrent;
	};

}

#endif


