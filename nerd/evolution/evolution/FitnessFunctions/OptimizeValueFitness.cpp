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

#include "OptimizeValueFitness.h"
#include "Value/DoubleValue.h"
#include "Value/MultiPartValue.h"
#include "Math/Math.h"

namespace nerd {


OptimizeValueFitness::OptimizeValueFitness(const QString &name) 
		: ControllerFitnessFunction(name), mNumberOfInitialSteps(0)
{
	mInitSteps = 0;
	mCore = Core::getInstance();
	mPositionName = new StringValue("");
	addParameter("Position", mPositionName);
	mIndex = new StringValue("");
	addParameter("Index", mIndex);
	defineIntParam("NumberOfInitialSteps", 50);
	defineIntParam("Alpha", 1.0);
	defineDoubleParam("DesiredPosition", 0.31);
	
}

OptimizeValueFitness::OptimizeValueFitness(const OptimizeValueFitness &copy) 
		: Object(), ValueChangedListener(), ControllerFitnessFunction(copy) 
{
	mCore = Core::getInstance();
	defineIntParam("NumberOfInitialSteps", 50);
	defineDoubleParam("DesiredPosition", 0.31);
	defineIntParam("Alpha", 1.0);
	mPositionName = dynamic_cast<StringValue*>(getParameter("Position"));
	mIndex = dynamic_cast<StringValue*>(getParameter("Index"));
	mInitSteps = 0;
}


OptimizeValueFitness::~OptimizeValueFitness() {
}

FitnessFunction* OptimizeValueFitness::createCopy() const {
	return new OptimizeValueFitness(*this);
}


double OptimizeValueFitness::calculateCurrentFitness() {
	double currentValue = 0.0;
	if(mPositionName == 0) {
		return mCurrentFitness->get();
	}
	Value *value= mCore->getValueManager()->getValue(mPositionName->get());
	double position = 0.0;
	if(value == 0) {
		return mCurrentFitness->get();
	}

	if (dynamic_cast<MultiPartValue*>(value) != 0) {
		MultiPartValue *multiVal = dynamic_cast<MultiPartValue*>(value);
		bool ok;
		int index = getParameter("Index")->getValueAsString().toInt(&ok);
		if(ok) {
			Value *multiValPart = multiVal->getValuePart(index);
			if(dynamic_cast<DoubleValue*>(multiValPart) != 0
				|| dynamic_cast<IntValue*>(multiValPart) != 0)
			{
				value = multiValPart;
			}
			else {
				return mCurrentFitness->get();
			 }
		}
	}

	if(dynamic_cast<DoubleValue*>(value) != 0) {
		position = dynamic_cast<DoubleValue*>(value)->get();
	}
	else if (dynamic_cast<IntValue*>(value) != 0) {
		position = dynamic_cast<IntValue*>(value)->get();
	}
	
	if(mInitSteps < getParam("NumberOfInitialSteps")) {
		mInitSteps++;
		return mCurrentFitness->get();
	}
	currentValue = getParam("Alpha") * Math::abs(getParam("DesiredPosition") 	
		- position);
	return mCurrentFitness->get() - Math::abs(currentValue);
}


void OptimizeValueFitness::prepareNextTry() {
	mInitSteps = 0;
}

}
