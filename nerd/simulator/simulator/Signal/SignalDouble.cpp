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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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

#include "SignalDouble.h"
#include "Value/IntValue.h"
#include "Core/Core.h"
#include <QDebug>

namespace nerd {

SignalDouble::SignalDouble(QString name, QString prefix) : ParameterizedObject(name, prefix), Signal(name, prefix) {
	mCurrentValue = new DoubleValue(0.0);
	mIdleStepsValue = new IntValue(0);
	mComputationStepValue = new IntValue(0);

	addParameter("CurrentValue" , mCurrentValue, true);
	addParameter("IdleSteps", mIdleStepsValue, true);
	addParameter("ComputationStep", mComputationStepValue, true);

	mCurrentValue->addValueChangedListener(this);
}

SignalDouble::~SignalDouble() {
}

/**
 * Add a destination value to mDestinationValues. All values from mDestinationValues are set to the currentValue if the currentValue changes.
 * 
 * @param destValue the destination Value to add.
 */
void SignalDouble::addDestinationValue(DoubleValue* destValue) {
	if(!mDestinationValues.contains(destValue) && (destValue != mCurrentValue)) {
		mDestinationValues.append(destValue);
	}
}

/**
 * Remove a destination value from mDestinationValues.
 * 
 * @param destValue the destination Value to temove.
 */
void SignalDouble::removeDestinationValue(DoubleValue* destValue) {
	mDestinationValues.removeAll(destValue);
}

/**
 * Removes all destination values.
 */
void SignalDouble::removeDestinationValues() {
	mDestinationValues.clear();
}

void SignalDouble::valueChanged(Value *value) {
	if(value == mCurrentValue) {
		double currentValue = mCurrentValue->get();
		QListIterator<DoubleValue*> destValuesIt(mDestinationValues);
		while(destValuesIt.hasNext()) {
			destValuesIt.next()->set(currentValue);
		}
	}
}

double SignalDouble::getCurrentValue() {
	return mCurrentValue->get();
}

void SignalDouble::resetSignal() {
	mCurrentValue->set(0.0);
}

void SignalDouble::eventOccured(Event *event) {
	// Signal::eventOccured overwritten to consider the idleSteps and compute the computationStep value
	if(event == 0) {
		return;
	}
	if(event == mNextStepEvent) {
		if(mCurrentStepValue != 0) {
			int currentStep = mCurrentStepValue->get();
			int idleSteps = mIdleStepsValue->get();
			if(currentStep <= idleSteps) {
				mComputationStepValue->set(0);
			} else {
				mComputationStepValue->set(currentStep - idleSteps);
			}
		}
	}
	if((event == mResetSimulationEvent) && mEnabledValue->get()) {
		mCurrentValue->set(0.0);
	}

	Signal::eventOccured(event);
}

}
