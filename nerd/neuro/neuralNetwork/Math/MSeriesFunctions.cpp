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

#include "MSeriesFunctions.h"
#include <math.h>
#include <stdint.h>
#include "ASeriesFunctions.h"

namespace nerd {


/**
 * Converts a double into a 32 bit signed fixed point number with 17 bits integer part and 
 * 15 bits fractional part.
 * If the double doesn't fit into 32 bit the fixed point is set to INT32_MAX or INT32_MIN.
 *
 * This function is used to rebuild the M-Series precision of a neuron activation value
 * which is represented in 32 bit in the ACCU-register of the M-Series.
 *
 * @param d the double to convert.
 * @return the 32 bit signed fixed point number.
 */
int32_t MSeriesFunctions::doubleToFixedPoint_17_15(double d) {
	// shift double 15 bits to the left
	d *= (double)0x8000;
	// extract the integer part of the double
	double intpart = 0;
	modf(d, &intpart);
	// if the integer part doesn't fit into int32_t, fit it
	int32_t ret = 0;
	if(intpart < (double)INT32_MIN) {
		ret = INT32_MIN;
	} 
	else if(intpart > (double)INT32_MAX) {
		ret = INT32_MAX;
	}
	else {
		ret = (int32_t) intpart;
	}
	return ret;
}

/**
 * Converts a 32 bit signed fixed point number with 17 bits integer part and 15 bits
 * fractional part into a double. 
 *
 * This function is used to rebuild the M-Series precision of a neuron activation value
 * which is represented in 32 bit in the ACCU-register of the M-Series.
 *
 * @param i the fixed point number to convert.
 * @return the double.
 */
double MSeriesFunctions::fixedPoint_17_15_ToDouble(int32_t i) {
	// cast to double and shift 15 bits to the right
	return ((double) i) / ((double) 0x8000);
}

}
