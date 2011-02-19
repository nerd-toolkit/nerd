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

#include "TransferFunctionMSeriesTanh.h"
#include <math.h>
#include <iostream>
#include "Math/Math.h"

using namespace std;

namespace nerd {

TransferFunctionMSeriesTanh::TransferFunctionMSeriesTanh()
	: TransferFunction("MSeriesTanh", -1.0, 1.0)
{
}

TransferFunctionMSeriesTanh::TransferFunctionMSeriesTanh(const TransferFunctionMSeriesTanh &other) 
	: Object(), ValueChangedListener(), TransferFunction(other)
{
}

TransferFunctionMSeriesTanh::~TransferFunctionMSeriesTanh() {
}


TransferFunction* TransferFunctionMSeriesTanh::createCopy() const {
	return new TransferFunctionMSeriesTanh(*this);
}


void TransferFunctionMSeriesTanh::reset(Neuron*) {
}


double TransferFunctionMSeriesTanh::transferActivation(double activation, Neuron*) {

	if(activation >= 5.0) {
		return 1.0;
	}
	else if(activation <= -5.0) {
		return -1.0;
	}

	bool negative = activation < 0.0 ? true : false;
	double actShifted = Math::abs(activation) * 8.0;
	int actBase = (int) actShifted;
	
	double transferedBase = (2.0 / (pow(M_E, -2 * ((double) actBase)) + 1)) - 1.0;
	
	//add linear interpolated part
	transferedBase *= (negative ? -1.0 : 1.0);

	return transferedBase;
}

bool TransferFunctionMSeriesTanh::equals(TransferFunction *transferFunction) const {
	if(TransferFunction::equals(transferFunction) == false) {
		return false;
	}

	TransferFunctionMSeriesTanh *tf = 
			dynamic_cast<TransferFunctionMSeriesTanh*>(transferFunction);

	if(tf == 0) {
		return false;
	}
	return true;
}


}


