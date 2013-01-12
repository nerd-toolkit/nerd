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

#include "ASeriesFunctions.h"
#include <math.h>
#include <stdint.h>

namespace nerd {


double ASeriesFunctions::sMinDoubleFixedPoint_4_9 = -8.0;
double ASeriesFunctions::sMaxDoubleFixedPoint_4_9 = 7.998046875;

/**
 * Converts a signed fixed point number with 1 bit integer part and 15 bits fractional
 * part into a double.
 *
 * This function is used to rebuild the A-Series precision of a neuron activation value
 * which is represented in 16 bit with 1 bit integer part and 15 bits fractional part.
 * This 16 bit value is stored in the 16 bit V-register of the A-Series.
 *
 * @param i the 16 bit signed fixed point number to convert.
 * @return the converted double.
 */
double ASeriesFunctions::fixedPoint_1_15_ToDouble(int16_t i) {
	// cast to double and shift 15 bits to the right
	return (double)i/(double)0x8000;
}

/**
 * Converts a double into a 16 bit signed fixed point number with 1 bit integer part and 
 * 15 bits fractional part.
 *
 * This function is used to rebuild the A-Series precision of a neuron activation value
 * which is represented in 16 bit with 1 bit integer part and 15 bits fractional part.
 * This 16 bit value is stored in the 16 bit V-register of the A-Series.
 *
 * @param d the double to convert.
 * @return the 16 bit signed fixed point number.
 */
int16_t ASeriesFunctions::doubleToFixedPoint_1_15(double d) {
	// if double is smaller than the smallest representable fixed_point_1_15 number,
	// return the smallest number
	if(d < -1.0) {
		return 0x8000;
	}
	// if double is bigger than the biggest representable fixed_point_1_15 number,
	// return the biggest number
	if(d > 0.999969482421875) {
		return 0x7FFF;
	}
	// shift the double 15 bits to the left
	d *= (double)0x8000;
	// extract the integer part of the double
	double intpart;
	modf(d, &intpart);
	// cast the integer part to int16_t
	int16_t ret = (int16_t)intpart;
	// that's it
	return ret;
}

/**
 * Converts a signed fixed point number with 4 bits integer part and 9 bits fractional
 * part into a double. The fixed point number is stored in 16 bit, but the maximum
 * number is 0000 111.1 1111 1111 (7.998046875) and the minimum number is
 * -0001 000.0 0000 0000 (-8.0). If the fixed point number exceeds the 13 bits the result
 * is set to this maximum/minimum number.
 *
 * This function is used to rebuild the A-Series precision of a weight value
 * which is represented in 13 bit with 4 bits integer part and 9 bits fractional part.
 * This 13 bit value is stored in the 16 bit W-register of the A-Series.
 *
 * @param i the 16 bit signed fixed point number to convert.
 * @return the converted double.
 */
double ASeriesFunctions::fixedPoint_4_9_ToDouble(int16_t i) {
	// if i is smaller than the smalles representable fixed_point_4_9 number,
	// return the smalles number
	if(i < -0x1000) {
		return sMaxDoubleFixedPoint_4_9;
	}
	// if i is bigger than the biggest representable fixed_point_4_9 number,
	// return the biggest number
	if(i > 0x0FFF) {
		return sMaxDoubleFixedPoint_4_9;
	}
	// otherwise cast to double and shift 9 bits to the right
	return (double)i/(double)0x200;
}

/**
 * Converts a double into a 16 bit signed fixed point number with 4 bits integer part and 
 * 9 bits fractional part. This means that the result is stored in 16 bit, but the maximum
 * number is 0000 111.1 1111 1111 (7.998046875) and the minimum number is
 * -0001 000.0 0000 0000 (-8.0). If the double doesn't fit into 13 bit the result is
 * set to this maximum/minimum number.
 *
 * This function is used to rebuild the A-Series precision of a weight value
 * which is represented in 13 bit with 4 bits integer part and 9 bits fractional part.
 * This 13 bit value is stored in the 16 bit W-register of the A-Series.
 *
 * @param d the double to convert.
 * @return the 16 bit signed fixed point number.
 */
int16_t ASeriesFunctions::doubleToFixedPoint_4_9(double d) {
	// if double is smaller than the smallest representable fixed_point_4_9 number,
	// return the smallest number
	if(d < -8.0) {
		return -0x1000;
	}
	// if double is bigger than the biggest representable fixed_point_4_9 number,
	// return the biggest number
	if(d > sMaxDoubleFixedPoint_4_9) {
		return 0x0FFF;
	}
	// shift the double 9 bits to the left
	d *= (double)0x200;
	// extract the integer part of the double
	double intpart;
	modf(d, &intpart);
	// cast the integer part to int16_t
	int16_t ret = (int16_t)intpart;
	// that's it
	return ret;
}

/**
 * Truncates a double to 13 bit signed fixed point number precision with
 * 4 bits integer and 9 bits fractional part.
 *
 * This function is used to rebuild the A-Series precision of a synapse strength
 * which is represented in 13 bit in the A-Series.
 *
 * @param d the double to map.
 * @return the truncated double.
 */
double ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(double d) {
	// if double is smaller than the smallest representable
    // fixed_point_4_9 number, return the smallest number
	if(d < sMinDoubleFixedPoint_4_9) {
		return sMinDoubleFixedPoint_4_9;
	}
	// if double is bigger than the biggest representable
    // fixed_point_4_9 number, return the biggest number
	if(d > sMaxDoubleFixedPoint_4_9) {
		return sMaxDoubleFixedPoint_4_9;
	}
	// otherwise shift double 9 bits to the left
	d *= (double)0x200;
	// extract the integer part of the double
	double intpart;
	modf(d, &intpart);
	// shift integer part back 9 bits to the right
	d = intpart / (double) 0x200;
	// that's it
	return d;
}

double ASeriesFunctions::mapDoubleToFixedPoint_4_9_MultipartPrecision(double d) {
	double rest = 0.0;
	// if double is smaller than the smallest representable
    // fixed_point_4_9 number, return the smallest number
	while(d < sMinDoubleFixedPoint_4_9) {
		rest += sMinDoubleFixedPoint_4_9;
		d -= sMinDoubleFixedPoint_4_9;
	}
	// if double is bigger than the biggest representable
    // fixed_point_4_9 number, return the biggest number
	while(d > sMaxDoubleFixedPoint_4_9) {
		rest += sMaxDoubleFixedPoint_4_9;
		d -= sMaxDoubleFixedPoint_4_9;
	}
	// otherwise shift double 9 bits to the left
	d *= (double)0x200;
	// extract the integer part of the double
	double intpart;
	modf(d, &intpart);
	// shift integer part back 9 bits to the left
	d = intpart / (double) 0x200;
	// that's it
	return d + rest;
}

/**
 * Converts a double into a 32 bit signed fixed point number with 8 bits integer part and 
 * 24 bits fractional part.
 * If the double doesn't fit into 32 bit the fixed point is set to INT32_MAX or INT32_MIN.
 *
 * This function is used to rebuild the A-Series precision of a neuron activation value
 * which is represented in 32 bit in the ACCU-register of the A-Series.
 *
 * @param d the double to convert.
 * @return the 32 bit signed fixed point number.
 */
int32_t ASeriesFunctions::doubleToFixedPoint_8_24(double d) {
	// shift double 24 bits to the left
	d *= (double)0x1000000;
	// extract the integer part of the double
	double intpart;
	modf(d, &intpart);
	// if the integer part doesn't fit into int32_t, fit it
	int32_t ret;
	if(intpart < (double)INT32_MIN) {
		ret = INT32_MIN;
	} else if(intpart > (double)INT32_MAX) {
		ret = INT32_MAX;
	} else {
		ret = (int32_t)intpart;
	}
	return ret;
}

/**
 * Converts a 32 bit signed fixed point number with 8 bits integer part and 24 bits
 * fractional part into a double. 
 *
 * This function is used to rebuild the A-Series precision of a neuron activation value
 * which is represented in 32 bit in the ACCU-register of the A-Series.
 *
 * @param i the fixed point number to convert.
 * @return the double.
 */
double ASeriesFunctions::fixedPoint_8_24_ToDouble(int32_t i) {
	// cast to double and shift 24 bits to the right
	return (double)i/(double)0x1000000;
}

}
