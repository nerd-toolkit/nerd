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

#include "DoubleRandomizer.h"
#include "Math/Random.h"
#include "Math/Math.h"

namespace nerd {

/**
 * Constructor. 
 * @param value The DoubleValue that should be randomized by this randomizer.
 */
DoubleRandomizer::DoubleRandomizer(DoubleValue *value) {
	mValue = value;
	mCurrentValue = mValue->get();
	mInitialValue = mValue->get();

	mIsAdditve = false;
	mMinValue = 0;
	mMaxValue = 0;
	mProbability = 0;
	mDeviation = 0;
}

DoubleRandomizer::~DoubleRandomizer() {
}

void DoubleRandomizer::reset() {
	mCurrentValue = mInitialValue;
}

void DoubleRandomizer::applyRandomization() {

	if(Random::nextDouble() >= mProbability) {	
		mValue->set(mCurrentValue);
		return;
	}
	
	if(mIsAdditve) {
		double offset = (Random::nextDouble() * (2.0 * mDeviation)) - mDeviation;
		mCurrentValue += offset;
	}
	else {
		double offset = (Random::nextDouble() * (mMaxValue - mMinValue)) + mMinValue;
		mCurrentValue = offset;
	}
	mCurrentValue = Math::min(mCurrentValue, mMaxValue);
	mCurrentValue = Math::max(mCurrentValue, mMinValue);
	
	mValue->set(mCurrentValue);
}


/**
 * This method is used, to change the randomization-modus of the randomizer and to change between additive and nom-additive randomization.
 * @param isAdditve 
 */
void DoubleRandomizer::setAdditive(bool isAdditve) {
	mIsAdditve = isAdditve;
}

/**
 * Set the minimal value that will be allowed for the DoubleValue that is randomized.
 * @param min 
 */
void DoubleRandomizer::setMin(double min) {
	mMinValue = min;
}

/**
 * Set the maximal value that will be allowed for the DoubleValue that is randomized.
 * @param max 
 */
void DoubleRandomizer::setMax(double max) {
	mMaxValue = max;
}

/**
 * Set the probability, that a randomization will take place.
 * @param probability 
 */
void DoubleRandomizer::setProbability(double probability) {
	mProbability = probability;
}

/**
 * In additive mode, the deviation defines the average offset, that will be added to the last value.
 * @param deviation 
 */
void DoubleRandomizer::setDeviation(double deviation) {
	mDeviation = deviation;
}

}
