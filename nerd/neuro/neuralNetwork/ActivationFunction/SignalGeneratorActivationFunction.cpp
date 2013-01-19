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



#include "SignalGeneratorActivationFunction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "Value/BoolValue.h"
#include "Math/Math.h"
#include "Math/Random.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SignalGeneratorActivationFunction.
 */
SignalGeneratorActivationFunction::SignalGeneratorActivationFunction()
	: ActivationFunction("SignalGenerator"), mCurrentActivation(0.0), 
	  mCurrentTargetActivation(0), mCurrentDuration(0), mCurrentStep(0),
	  mTypeId(TYPE_RAMP)
{
		mMinActivation = new DoubleValue(-1.0);
		mMaxActivation = new DoubleValue(1.0);
		mMinDuration = new IntValue(100);
		mMaxDuration = new IntValue(1000);
		mChangeDuration = new IntValue(100);
		mRandomizeTargetActivations = new BoolValue(false);
		mRandomizeDuration = new BoolValue(false);
		mStartActivation = new StringValue("0.0");
		mSignalType = new StringValue("Ramp");
		mSignalType->getOptionList().append("Ramp");
		mSignalType->getOptionList().append("Noise");
		mSignalType->getOptionList().append("Sine");
		mSignalType->getOptionList().append("Spikes");

		addParameter("MinActivation", mMinActivation);
		addParameter("MaxActivation", mMaxActivation);
		addParameter("MinDuration", mMinDuration);
		addParameter("MaxDuration", mMaxDuration);
		addParameter("ChangeDuration", mChangeDuration);
		addParameter("RandActivation", mRandomizeTargetActivations);
		addParameter("RandDuration", mRandomizeDuration);
		addParameter("InitActivation", mStartActivation);
		addParameter("Type", mSignalType);

}


/**
 * Copy constructor. 
 * 
 * @param other the SignalGeneratorActivationFunction object to copy.
 */
SignalGeneratorActivationFunction::SignalGeneratorActivationFunction(
			const SignalGeneratorActivationFunction &other) 
	: ObservableNetworkElement(other), ActivationFunction(other), mCurrentActivation(0.0), 
	  mCurrentTargetActivation(0), mCurrentDuration(0), mCurrentStep(0),
	  mTypeId(other.mTypeId)
{
	mMinActivation = dynamic_cast<DoubleValue*>(getParameter("MinActivation"));
	mMaxActivation = dynamic_cast<DoubleValue*>(getParameter("MaxActivation"));
	mMinDuration = dynamic_cast<IntValue*>(getParameter("MinDuration"));
	mMaxDuration = dynamic_cast<IntValue*>(getParameter("MaxDuration"));
	mChangeDuration = dynamic_cast<IntValue*>(getParameter("ChangeDuration"));
	mRandomizeTargetActivations = dynamic_cast<BoolValue*>(getParameter("RandActivation"));
	mRandomizeDuration = dynamic_cast<BoolValue*>(getParameter("RandDuration"));
	mStartActivation = dynamic_cast<StringValue*>(getParameter("InitActivation"));
	mSignalType = dynamic_cast<StringValue*>(getParameter("Type"));
}

/**
 * Destructor.
 */
SignalGeneratorActivationFunction::~SignalGeneratorActivationFunction() {
	
}


ActivationFunction* SignalGeneratorActivationFunction::createCopy() const {
	return new SignalGeneratorActivationFunction(*this);
}

void SignalGeneratorActivationFunction::valueChanged(Value *value) {
	ActivationFunction::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mSignalType) {
		QString type = mSignalType->get().trimmed().toLower();
		if(type == "ramp") {
			mTypeId = TYPE_RAMP;
		}
		else if(type == "sine") {
			mTypeId = TYPE_SINE;
		}
		else if(type == "noise") {
			mTypeId = TYPE_NOISE;
		}
		else if(type == "spikes") {
			mTypeId = TYPE_SPIKES;
		}
	}
}

void SignalGeneratorActivationFunction::reset(Neuron*) {
	QString startActivation = mStartActivation->get();
	mCurrentActivation = mMinActivation->get();

	mCurrentTargetActivation = 0;
	mCurrentStep = 0;
	mCurrentDuration = 0;

	if(startActivation.contains("*")) {
		int iterations = Random::nextInt(mMinDuration->get());
		for(int i = 0; i < iterations; ++i) {
			//calculate n steps so that the starting activation is varied.
			calculateActivation(0);
		}
		mStartActivation->set("*");
	}
	else {
		mCurrentActivation = startActivation.toDouble();
		mStartActivation->set(QString::number(mCurrentActivation));
	}
}


double SignalGeneratorActivationFunction::calculateActivation(Neuron *owner) {
	ActivationFunction::calculateActivation(owner);
	if(mCurrentStep >= mCurrentDuration) {
		//determine new target values.
		if(mRandomizeDuration->get()) {
			mCurrentDuration = Random::nextInt(mMaxDuration->get() - mMinDuration->get()) + mMinDuration->get();
		}
		else {
			mCurrentDuration = mMinDuration->get();
		}
		if(mRandomizeTargetActivations->get()) {
			mCurrentTargetActivation = mMinActivation->get() + 
					(Random::nextDouble() * (mMaxActivation->get() - mMinActivation->get()));
		}
		else {
			if(mCurrentActivation >= mMaxActivation->get()) {
				mCurrentTargetActivation = mMinActivation->get();
			}
			else {
				mCurrentTargetActivation = mMaxActivation->get();
			}
		}
		mCurrentStep = 0;
		mCurrentDuration = mCurrentDuration;
		mCurrentActivation = Math::min(mMaxActivation->get(), 
										Math::max(mMinActivation->get(), mCurrentActivation));

	}
	double changeDuration = mChangeDuration->get();	
	if(changeDuration <= mCurrentStep) {
		mCurrentActivation = mCurrentTargetActivation;
	}
	else {
		mCurrentActivation += (mCurrentTargetActivation - mCurrentActivation)
							 / (double) (mChangeDuration->get() - mCurrentStep);
	}
	++mCurrentStep;

	if(mTypeId == TYPE_RAMP) {
		return mCurrentActivation;
	}
	else if(mTypeId == TYPE_SINE) {
		
	}

	return mCurrentActivation;
}



bool SignalGeneratorActivationFunction::equals(ActivationFunction *activationFunction) const  {
	if(ActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	SignalGeneratorActivationFunction *sf = 
			dynamic_cast<SignalGeneratorActivationFunction*>(activationFunction);

	if(sf == 0) {
		return false;
	}

	if(sf->mMinActivation == 0 || mMinActivation == 0 
		|| sf->mMinActivation->get() != mMinActivation->get()) 
	{
		return false;
	}
	if(sf->mMaxActivation == 0 || mMaxActivation == 0 
		|| sf->mMaxActivation->get() != mMaxActivation->get()) 
	{
		return false;
	}
	if(sf->mMinDuration == 0 || mMinDuration == 0 
		|| sf->mMinDuration->get() != mMinDuration->get()) 
	{
		return false;
	}
	if(sf->mMaxDuration == 0 || mMaxDuration == 0 
		|| sf->mMaxDuration->get() != mMaxDuration->get()) 
	{
		return false;
	}
	if(sf->mStartActivation == 0 || mStartActivation == 0
		|| sf->mStartActivation->get() != mStartActivation->get())
	{
		return false;
	}
	if(sf->mRandomizeDuration == 0 || mRandomizeDuration == 0 
		|| sf->mRandomizeDuration->get() != mRandomizeDuration->get()) 
	{
		return false;
	}
	if(sf->mRandomizeTargetActivations == 0 || mRandomizeTargetActivations == 0 
		|| sf->mRandomizeTargetActivations->get() != mRandomizeTargetActivations->get()) 
	{
		return false;
	}
	if(sf->mSignalType == 0 || mSignalType == 0 
		|| sf->mSignalType->get() != mSignalType->get())
	{
		return false;
	}

	return true;
}


}



