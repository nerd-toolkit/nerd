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


#ifndef NERDSelfRegulatingNeuronV2ActivationFunction_H
#define NERDSelfRegulatingNeuronV2ActivationFunction_H

#include <QString>
#include <QHash>
#include "ActivationFunction/ActivationFunction.h"
#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include "Value/BoolValue.h"
#include "Value/IntValue.h"

namespace nerd {

	/**
	 * SelfRegulatingNeuronV2ActivationFunction.
	 *
	 */
	class SelfRegulatingNeuronV2ActivationFunction : public ActivationFunction {
	public:
		SelfRegulatingNeuronV2ActivationFunction(const QString &name = "SRN_V1", bool showModes = true);
		SelfRegulatingNeuronV2ActivationFunction(const SelfRegulatingNeuronV2ActivationFunction &other);
		virtual ~SelfRegulatingNeuronV2ActivationFunction();

		virtual ActivationFunction* createCopy() const;
		
		virtual void valueChanged(Value *value);

		virtual void reset(Neuron *owner);
		virtual double calculateActivation(Neuron *owner);

		bool equals(ActivationFunction *activationFunction) const;
		
	protected:
		virtual double getReceptorStrengthUpdate(double activation);
		virtual double getTransmitterStrengthUpdate(double activation);
		virtual void updateXi(double activation);
		virtual void updateEta(double activation);
		virtual void updateTheta(double activation);
		
		virtual double updateActivity();

	protected:
		DoubleValue *mXi;
		DoubleValue *mEta;
		DoubleValue *mAlpha;
		DoubleValue *mBeta;
		DoubleValue *mGamma;
		DoubleValue *mDelta;
		StringValue *mTargetValues;
		StringValue *mOptions;
		
		IntValue *mReceptorMode;
		IntValue *mTransmitterMode;
		IntValue *mBiasMode;
		
		Neuron *mOwner;
		double mTransmitterResult;
		double mReceptorResult;
		
		double mActivationT2;
		bool mAdjustWeights;
		bool mRestrictToLinks;
		bool mUseCurrentActivations;
		double mEpsilon;
		
		QList<double> mTargets;
		bool mUpdatingTargetString;
		
	};

}

#endif



