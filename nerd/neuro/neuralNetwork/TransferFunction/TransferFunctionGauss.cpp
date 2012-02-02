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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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



#include "TransferFunctionGauss.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <math.h>
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new TransferFunctionGauss.
 */
TransferFunctionGauss::TransferFunctionGauss(double deviation, double shift)
	: TransferFunction("Gauss", 0.0, 1.0)
{
	mDeviation = new DoubleValue(deviation);
	mShift = new DoubleValue(shift);

	addParameter("Deviation", mDeviation);
	addParameter("Shift", mShift);
}


/**
 * Copy constructor. 
 * 
 * @param other the TransferFunctionGauss object to copy.
 */
TransferFunctionGauss::TransferFunctionGauss(
			const TransferFunctionGauss &other) 
	: Object(), ValueChangedListener(), TransferFunction(other)
{
	mDeviation = dynamic_cast<DoubleValue*>(getParameter("Deviation"));
	mShift = dynamic_cast<DoubleValue*>(getParameter("Shift"));
}

/**
 * Destructor.
 */
TransferFunctionGauss::~TransferFunctionGauss() 
{
}


TransferFunction* TransferFunctionGauss::createCopy() const {
	return new TransferFunctionGauss(*this);
}


void TransferFunctionGauss::reset(Neuron*) {
}


double TransferFunctionGauss::transferActivation(double activation, Neuron*) {
	double sigma = mDeviation->get();
	
	//avoid division by zero.
	if(sigma == 0) {
			return 0.0;
	}
	return ((1.0 / (sqrt(2 * Math::PI) * sigma)) 
			* exp(-1 * ((activation * activation) / (2 * sigma * sigma)))) - mShift->get();
}

bool TransferFunctionGauss::equals(TransferFunction *transferFunction) const {
	if(TransferFunction::equals(transferFunction) == false) {
		return false;
	}

	TransferFunctionGauss *tf = 
			dynamic_cast<TransferFunctionGauss*>(transferFunction);

	if(tf == 0) {
		return false;
	}
	if(tf->mDeviation->get() != mDeviation->get()) {
		return false;
	}
	if(tf->mShift->get() != mShift->get()) {
		return false;
	}
	return true;
}



}



