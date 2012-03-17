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


#ifndef NERDSelfRegulatingNeuronActivationFunction_H
#define NERDSelfRegulatingNeuronActivationFunction_H

#include <QString>
#include <QHash>
#include "ActivationFunction/ActivationFunction.h"
#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include "Value/BoolValue.h"
#include "Value/IntValue.h"

namespace nerd {

	/**
	 * SelfRegulatingNeuronActivationFunction.
	 *
	 */
	class SelfRegulatingNeuronActivationFunction : public ActivationFunction {
	public:
		SelfRegulatingNeuronActivationFunction(const QString &name = "SRN_V1", bool showModes = true);
		SelfRegulatingNeuronActivationFunction(const SelfRegulatingNeuronActivationFunction &other);
		virtual ~SelfRegulatingNeuronActivationFunction();

		virtual ActivationFunction* createCopy() const;

		virtual void reset(Neuron *owner);
		virtual double calculateActivation(Neuron *owner);
		
		DoubleValue* getAlpha() const;
		DoubleValue* getBeta() const;
		DoubleValue* getGamma() const;
		DoubleValue* getDelta() const;
		DoubleValue* getAStar() const;

		bool equals(ActivationFunction *activationFunction) const;
		
	protected:
		virtual double getReceptorStrengthUpdate(double activation);
		virtual double getTransmitterStrengthUpdate(double activation);
		virtual void updateXi(double activation);
		virtual void updateEta(double activation);
		
		virtual double updateActivity();

	protected:
		DoubleValue *mXi;
		DoubleValue *mEta;
		DoubleValue *mAlpha;
		DoubleValue *mBeta;
		DoubleValue *mGamma;
		DoubleValue *mDelta;
		DoubleValue *mAStar;
		BoolValue *mAdjustWeights;
		BoolValue *mRestrictToLinkSynapses;
		
		IntValue *mReceptorMode;
		IntValue *mTransmitterMode;
		
		Neuron *mOwner;
		double mTransmitterResult;
		double mReceptorResult;
		
	};

}

#endif




