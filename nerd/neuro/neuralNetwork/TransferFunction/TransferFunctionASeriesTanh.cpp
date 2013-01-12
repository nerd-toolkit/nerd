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

#include "TransferFunctionASeriesTanh.h"
#include "Math/ASeriesFunctions.h"
#include <math.h>

namespace nerd {

TransferFunctionASeriesTanh::TransferFunctionASeriesTanh()
	: TransferFunction("ASeriesTanh", -1.0, 1.0)
{
}

TransferFunctionASeriesTanh::TransferFunctionASeriesTanh(const TransferFunctionASeriesTanh &other) 
	: Object(), ValueChangedListener(), ObservableNetworkElement(other), TransferFunction(other)
{
}

TransferFunctionASeriesTanh::~TransferFunctionASeriesTanh() {
}


TransferFunction* TransferFunctionASeriesTanh::createCopy() const {
	return new TransferFunctionASeriesTanh(*this);
}


void TransferFunctionASeriesTanh::reset(Neuron*) {
}

/* LUT for the bases of the tanh: distance between the bases is 0.001b == 0.125 */
const int16_t TransferFunctionASeriesTanh::mBases[] = {
	    0,  4075,  8025, 11743, 15143, 18173, 20813, 23066,
	24956, 26519, 27797, 28830, 29660, 30322, 30847, 31262,
	31589, 31846, 32048, 32206, 32329, 32426, 32501, 32560,
	32606, 32642, 32670, 32691, 32708, 32721, 32732, 32740,
	32746, 32751, 32755, 32758, 32760, 32762, 32763, 32764, 32765 };

const int16_t TransferFunctionASeriesTanh::mDeltas[] = {
	4075,   3950,  3718,  3400,  3030,  2640,  2253,  1890,
	1563,   1278,  1033,   830,   662,   525,   415,   327,
	 257,    202,   158,   123,    97,    75,    59,    46,
	  36,     28,    21,    17,    13,    11,     8,     6,
	   5,      4,     3,     2,     2,     1,     1,     1,    2 };

double TransferFunctionASeriesTanh::transferActivation(double activation, Neuron*) {
	// get the 8.24 fixed point value of the activation
	int32_t fpActivation = ASeriesFunctions::doubleToFixedPoint_8_24(activation);
	int16_t index, fpResult;
	int32_t fpResult32;

	// if fpActivation is negative, make positive
	if(fpActivation < 0) {
		fpActivation = -fpActivation;
	}

	// extract the LUT index from the activation
	// see tanh doc from the A-Series-Firmware for details
	fpActivation >>= 5;
	index = (int16_t)(fpActivation >> 16);

	// test, if already saturated
	// 40 is the last index of the LUT
	if(index > 40) {
		// if saturated set to approx. 1
		fpResult32 = 0x7FFFFFFF;
	} else {
		// otherwise compute the value from the LUT
		// set base
		fpResult32 = mBases[index] << 16;
		// extract delta from the activation
		// see tanh doc from the A-Series-Firmware for details
		uint32_t delta = fpActivation & 0x0000FFFF;
		delta *= mDeltas[index];
		fpResult32 += delta;
	}

	// shift the result to 1.15 fixed point
	fpResult = (int16_t)(fpResult32 >> 16);

	// restore sign	
	if(activation < 0) {
		fpResult = -fpResult;
	}

	// convert 8.24 fixed point value back to double
	return ASeriesFunctions::fixedPoint_1_15_ToDouble(fpResult);
}


bool TransferFunctionASeriesTanh::equals(TransferFunction *transferFunction) const {
	if(TransferFunction::equals(transferFunction) == false) {
		return false;
	}

	TransferFunctionASeriesTanh *tf = 
			dynamic_cast<TransferFunctionASeriesTanh*>(transferFunction);

	if(tf == 0) {
		return false;
	}
	return true;
}

}


