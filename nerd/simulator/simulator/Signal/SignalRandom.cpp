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

#include "SignalRandom.h"
#include "Math/Math.h"
#include "Math/Random.h"
#include <QDebug>

namespace nerd {

SignalRandom::SignalRandom(QString name, QString prefix) 
	: ParameterizedObject(name, prefix), Signal(name, prefix), SignalDouble(name, prefix), 
		mOldRandomValue(0.0), mCurrentRandomValue(0.0), mCurrentRandomValueIndex(0) 
{
	mPeriodValue = new DoubleValue(1.0);
	mAmplitudeValue = new DoubleValue(1.0);
	mOffsetValue = new DoubleValue(0.0);
	mSeedValue = new IntValue(10);
	mNumberOfRandomValuesValue = new IntValue(100);

	addParameter("Period", mPeriodValue, true);
	addParameter("Amplitude", mAmplitudeValue, true);
	addParameter("Offset", mOffsetValue, true);
	addParameter("Seed", mSeedValue, true);
	addParameter("NumberOfRandomValues", mNumberOfRandomValuesValue, true);

	fillRandomValues();
}

SignalRandom::~SignalRandom() {
}

/**
 * Fills the mRandomValues list with new random numbers.
 */
void SignalRandom::fillRandomValues() {
	Random::setSeed(mSeedValue->get());
	mRandomValues.clear();
	for(int i = 0; i < mNumberOfRandomValuesValue->get(); i++) {
		mRandomValues.append(Math::getNextUniformlyDistributedValue(1.0));
	}
	mCurrentRandomValue = mOffsetValue->get();
	mCurrentRandomValueIndex = 0;
}

void SignalRandom::valueChanged(Value *value) {
	// Catch values relevant for random numbers and update the random numbers
	if((value == mNumberOfRandomValuesValue) || (value == mSeedValue)) {
		fillRandomValues();
	}
	SignalDouble::valueChanged(value);
}

void SignalRandom::resetSignal() {
	mOldRandomValue = 0.0;
	mCurrentRandomValue = 0.0;
	mCurrentRandomValueIndex = 0;
	SignalDouble::resetSignal();
}

/**
 * Computes the current value of the random signal.
 */
void SignalRandom::updateSignal() {
	if(mTimeStepSizeValue != 0) {
		// Get the parameters
		int computationStep = mComputationStepValue->get();
		double timeStepSize = mTimeStepSizeValue->get();
		double periodInSeconds = mPeriodValue->get();
		double amplitude = mAmplitudeValue->get();
		double offset = mOffsetValue->get();

		if(periodInSeconds != 0.0) {
			// Compute the period in steps from the period in seconds
			int periodInSteps = (int)(periodInSeconds / timeStepSize);
			// Compute the number of steps in the period
			int stepsInPeriod = computationStep % periodInSteps;
			if(stepsInPeriod == 0) {
				stepsInPeriod = periodInSteps;
			}
			// If new period, get new random number
			if(stepsInPeriod == 1) {
				mOldRandomValue = mCurrentRandomValue;
				mCurrentRandomValue = mRandomValues.at(mCurrentRandomValueIndex);
				mCurrentRandomValueIndex++;
				if(mCurrentRandomValueIndex == mRandomValues.size()) {
					mCurrentRandomValueIndex = 0;
				}
			}
			// Linear Interpolation between mOldRandomValue and mCurrentRandomValue
			double newCurrentValue = offset + amplitude * (mCurrentRandomValue * (stepsInPeriod / (double)periodInSteps) + mOldRandomValue * (1.0 - (stepsInPeriod / (double)periodInSteps)));
			// Set new Current Value
			mCurrentValue->set(newCurrentValue);
		}
	}
}

}
