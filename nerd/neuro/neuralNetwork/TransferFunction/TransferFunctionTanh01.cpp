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

#include "TransferFunctionTanh01.h"
#include <math.h>
#include <iostream>
#include "Math/Math.h"

using namespace std;

namespace nerd {

TransferFunctionTanh01::TransferFunctionTanh01()
	: TransferFunction("tanh[0,1]", 0.0, 1.0)
{
}

TransferFunctionTanh01::TransferFunctionTanh01(const TransferFunctionTanh01 &other) 
	: Object(), ValueChangedListener(), TransferFunction(other)
{
}

TransferFunctionTanh01::~TransferFunctionTanh01() {
}


TransferFunction* TransferFunctionTanh01::createCopy() const {
	return new TransferFunctionTanh01(*this);
}


void TransferFunctionTanh01::reset(Neuron*) {
}


double TransferFunctionTanh01::transferActivation(double activation, Neuron*) {
	double raw = (pow(M_E, -2 * activation) + 1);
	//avoid division by zero
	if(raw == 0.0) {
		return -1.0;
	}
	return Math::max((2.0 / raw) - 1, 0.0);
}

bool TransferFunctionTanh01::equals(TransferFunction *transferFunction) const {
	if(TransferFunction::equals(transferFunction) == false) {
		return false;
	}

	TransferFunctionTanh01 *tf = 
			dynamic_cast<TransferFunctionTanh01*>(transferFunction);

	if(tf == 0) {
		return false;
	}
	return true;
}


}


