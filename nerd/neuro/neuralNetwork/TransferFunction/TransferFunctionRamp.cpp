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
#include "TransferFunctionRamp.h"
#include "Value/DoubleValue.h"

namespace nerd {

TransferFunctionRamp::TransferFunctionRamp(const QString &name, double min, double max, bool adjustable)
	: TransferFunction(name, min, max), mMinValue(0), mMaxValue(0)
{
	if(adjustable) {
		mMinValue = new DoubleValue(-1.0);
		mMaxValue = new DoubleValue(1.0);

		addParameter("Min", mMinValue);
		addParameter("Max", mMaxValue);
	}
}

TransferFunctionRamp::TransferFunctionRamp(const TransferFunctionRamp &other)
	: Object(), ValueChangedListener(), ObservableNetworkElement(other), TransferFunction(other)
{
	mMinValue = dynamic_cast<DoubleValue*>(getParameter("Min"));
	mMaxValue = dynamic_cast<DoubleValue*>(getParameter("Max"));
}



TransferFunctionRamp::~TransferFunctionRamp() {
}

TransferFunction* TransferFunctionRamp::createCopy() const {
	return new TransferFunctionRamp(*this);
}

void TransferFunctionRamp::valueChanged(Value *value) {
	TransferFunction::valueChanged(value);	
	if(value == 0) {
		return;
	}
	else if(value == mMinValue) {
		mLowerBound = mMinValue->get();
	}
	else if(value == mMaxValue) {
		mUpperBound = mMaxValue->get();
	}
}

void TransferFunctionRamp::reset(Neuron*) {
}


double TransferFunctionRamp::transferActivation(double activation, Neuron*) {
	if(activation < mLowerBound) {
		return mLowerBound;
	}
	else if(activation > mUpperBound) {
		return mUpperBound;
	}
	else {
		return activation;
	}
}

bool TransferFunctionRamp::equals(TransferFunction *transferFunction) const {
	if(TransferFunction::equals(transferFunction) == false) {
		return false;
	}

	TransferFunctionRamp *tf = 
			dynamic_cast<TransferFunctionRamp*>(transferFunction);

	if(tf == 0) {
		return false;
	}
	return true;
}

}


