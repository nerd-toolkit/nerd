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



#include "MinimizeErrorFitness.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QStringList>
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new MinimizeErrorFitness.
 */
MinimizeErrorFitness::MinimizeErrorFitness(const QString &name)
	: ControllerFitnessFunction(name), mActualValueNames(0), mTargetValues(0)
{
	StringValue *mActualValueNames = new StringValue();
	StringValue *mTargetValues = new StringValue();
	
	addParameter("ActualValueNames", mActualValueNames);
	addParameter("TargetValues", mTargetValues);

	mHelpMessage->set("ActualValueNames Syntax: [Name1,Name2,Name3,...] "
					  "TargetValues Syntax: [value1,value2,value3,...] (no brackets) "
					  "Number of entries has to be the same in both parameters.");
}


/**
 * Copy constructor. 
 * 
 * @param other the MinimizeErrorFitness object to copy.
 */
MinimizeErrorFitness::MinimizeErrorFitness(const MinimizeErrorFitness &other) 
	: Object(), ValueChangedListener(), ControllerFitnessFunction(other),
	  mActualValueNames(0), mTargetValues(0)
{
	mActualValueNames = dynamic_cast<StringValue*>(getParameter("ActualValueNames"));
	mTargetValues = dynamic_cast<StringValue*>(getParameter("TargetValues"));
}

/**
 * Destructor.
 */
MinimizeErrorFitness::~MinimizeErrorFitness() {
}


FitnessFunction* MinimizeErrorFitness::createCopy() const {
	return new MinimizeErrorFitness(*this);
}


double MinimizeErrorFitness::calculateCurrentFitness() {
	if(mActualValueList.size() != mTargetValueList.size()) {
		Core::log("MinimizeErrorFitness: The size of the actualValues and the targetValues does not match!");
		return 0.0;
	}
	if(mActualValueList.empty()) {
		return 0.0;
	}

	double errorSum = 0.0;
	for(int i = 0; i < mActualValueList.size(); ++i) {
		double actualValue = 0;
		DoubleValue *doubleValue = dynamic_cast<DoubleValue*>(mActualValueList.at(i));
		if(doubleValue != 0) {
			actualValue = doubleValue->get();
		}
		else {
			IntValue *intValue = dynamic_cast<IntValue*>(mActualValueList.at(i));
			if(intValue != 0) {
				actualValue = intValue->get();
			}
			else {
				mErrorState->set("Current value is not a DoubleValue of an IntValue!");
				Core::log("MinimizeErrorFitness: Current value is not a DoubleValue of an IntValue!");
				return 0.0;
			}
		}
		errorSum  += Math::abs(actualValue - mTargetValueList.at(i));
	}
	if(errorSum == 0.0) {
		return getCurrentFitness() + 1.0;
	}
	return getCurrentFitness() + ((1.0 / errorSum) / (double) getNumberOfSteps());
}


void MinimizeErrorFitness::prepareNextTry() {

}


void MinimizeErrorFitness::reset() {
	ControllerFitnessFunction::reset();

	mActualValueList.clear();
	mTargetValueList.clear();
	
	//try to find the values to compare in the fitness calculation
	QStringList actualValueNames = mActualValueNames->get().split(",");
	QStringList targetValueStrings = mTargetValues->get().split(",");

	if(actualValueNames.size() != targetValueStrings.size()) {
		mErrorState->set("ValueNames and Targets have different number of entries");
		return;
	}

	ValueManager *vm = Core::getInstance()->getValueManager();

	for(int i = 0; i < actualValueNames.size(); ++i) {
		QString name = actualValueNames.at(i);
		Value *actualValue = vm->getMultiPartValue(name);
		if(actualValue == 0) {
			mErrorState->set("Could not find value [" + name + "]");
			mActualValueList.clear();
			mTargetValueList.clear();
			return;
		}
		if(dynamic_cast<DoubleValue*>(actualValue) == 0 && dynamic_cast<IntValue*>(actualValue) == 0) {
			mErrorState->set("The value [" + name + "] was not an IntValue of a DoubleValue!");
			mActualValueList.clear();
			mTargetValueList.clear();
			return;
		}
		bool ok = false;
		double targetValue = targetValueStrings.at(i).toDouble(&ok);
		if(!ok) {
			mErrorState->set("Target value [" + targetValueStrings.at(i) + "] could not be parsed to double.");
			mActualValueList.clear();
			mTargetValueList.clear();
			return;
		}
		mActualValueList.append(actualValue);
		mTargetValueList.append(targetValue);
	}

	mErrorState->set("");
}



}



