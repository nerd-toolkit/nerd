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



#include "TestASeriesFunctions.h"
#include "Math/ASeriesFunctions.h"
#include <math.h>

using namespace nerd;

void TestASeriesFunctions::testDoubleToFixedPoint_8_24() {
	// activation too big
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_8_24(128.0), INT32_MAX);
	// activation not too big
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_8_24(127.0), 2130706432);
	// activation too small
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_8_24(-129.0), INT32_MIN);
	// activation min
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_8_24(-128.0), INT32_MIN);
	// activation not too small
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_8_24(-127.0), -2130706432);
	// activation too precise
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_8_24(3.3), 55364812);
	// same with negative number
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_8_24(-3.3), -55364812);
}

void TestASeriesFunctions::testFixedPoint_8_24_ToDouble() {

	QCOMPARE(ASeriesFunctions::fixedPoint_8_24_ToDouble(INT32_MAX), 127.999999940395355);
	QCOMPARE(ASeriesFunctions::fixedPoint_8_24_ToDouble(INT32_MIN), -128.0);
	QCOMPARE(ASeriesFunctions::fixedPoint_8_24_ToDouble(-0x7F000000), -127.0);
	QCOMPARE(ASeriesFunctions::fixedPoint_8_24_ToDouble(0x12345678), 18.204444408416748);
}

/*
 * This is nearly the original code from the MotionEditor for comparison with the NERD code.
 * It is changed to extend it to a 7.9 representation instead of 4.9 and some
 * C# to C++ changes are made.
 */
int16_t hybridCodeConvertDoubleToWeight(double w) {
	// The part from here ...
	uint16_t interimValue = 0;
	if (w >= 8) w = 7.999999; else if (w < -8) w = -8;

	if (w < 0) { interimValue = 0x1000; w += 8; }
	for (int i = 2; i > -10; i--)
		if (w >= pow(2, i)) { interimValue = (uint16_t)(interimValue | (1 << (i + 9))); w -= pow(2, i); }
	// ... to here equals the C#  UInt16 convertDoubleToWeight(double w)  function from the motion editor
	// interimValue corresponds to the returnValue from that function

	// The part from here ...
	int16_t returnValue = 0;
	returnValue |= interimValue;
	if(returnValue & 0x1000) {
		returnValue |= 0xE000;
	}
	// ... to here equals the 4.9 to 7.9 convertion from the ABXX_Auto-Firmware.
	return returnValue;
}

void TestASeriesFunctions::testDoubleToFixedPoint_4_9() {
	// first of all test the code as it is intended to work
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-9.0), (int16_t)0xF000);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-9.0), (int16_t)-0x1000);	
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-8.0), (int16_t)0xF000);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-7.0), (int16_t)-0x0E00);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-7.3), (int16_t)-0xE99);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(0.0), (int16_t)0);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(7.3), (int16_t)0xE99);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(8.0), (int16_t)0x0FFF);

	// now compare it to the MotionEditor function
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-2000.00), hybridCodeConvertDoubleToWeight(-2000.00));
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-9.0), hybridCodeConvertDoubleToWeight(-9.0));
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-8.0), hybridCodeConvertDoubleToWeight(-8.0));
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-7.0), hybridCodeConvertDoubleToWeight(-7.0));

//	This is commented out, because there is a little difference in the computation between
//	the NERD code and the MotionEditor code. If there is a negative fractional number which uses
//	the complete 9 bit fractional range, there will be a difference in the last bit. The NERD
//	computation outspeeds the MotionEditor computation, so this "error" is ignored.

//	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-7.3), hybridCodeConvertDoubleToWeight(-7.3));

// Explanation:
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(-7.3), (int16_t)-0xE99);
	QCOMPARE(hybridCodeConvertDoubleToWeight(-7.3), (int16_t)-0xE9A);

	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(0.0), hybridCodeConvertDoubleToWeight(0.0));
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(7.3), hybridCodeConvertDoubleToWeight(7.3));
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(8.0), hybridCodeConvertDoubleToWeight(8.0));
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_4_9(2000.123), hybridCodeConvertDoubleToWeight(2000.123));
}

void TestASeriesFunctions::testFixedPoint_4_9_ToDouble() {
	QCOMPARE(ASeriesFunctions::fixedPoint_4_9_ToDouble((int16_t)0xF000), -8.0);
	QCOMPARE(ASeriesFunctions::fixedPoint_4_9_ToDouble((int16_t)-0x1000), -8.0);	
	QCOMPARE(ASeriesFunctions::fixedPoint_4_9_ToDouble((int16_t)-0x0E00), -7.0);
	QCOMPARE(ASeriesFunctions::fixedPoint_4_9_ToDouble((int16_t)-0xE99), -7.298828125);
	QCOMPARE(ASeriesFunctions::fixedPoint_4_9_ToDouble((int16_t)0), 0.0);
	QCOMPARE(ASeriesFunctions::fixedPoint_4_9_ToDouble((int16_t)0xE99), 7.298828125);
	QCOMPARE(ASeriesFunctions::fixedPoint_4_9_ToDouble((int16_t)0x0FFF), 7.998046875);

}

void TestASeriesFunctions::testMapDoubleToFixedPoint_4_9_Precision() {

	// strength too big
	QCOMPARE(ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(8.0), 7.998046875);
	// strength too small
	QCOMPARE(ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(-9.0), -8.0);
	// strength too precise
	QCOMPARE(ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(3.3), 3.298828125);	// == 11,010011001 (binär)
	// strength to precise 3,2998046875 == 11,0100110011 (binär)
	QCOMPARE(ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(3.2998046875), 3.298828125);
	// same with negative number
	QCOMPARE(ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(-3.2998046875), -3.298828125);

	// test with other functions
	QCOMPARE(ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(8.0),
			ASeriesFunctions::fixedPoint_4_9_ToDouble(ASeriesFunctions::doubleToFixedPoint_4_9(8.0)));
	QCOMPARE(ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(-9.0),
			ASeriesFunctions::fixedPoint_4_9_ToDouble(ASeriesFunctions::doubleToFixedPoint_4_9(-9.0)));
	QCOMPARE(ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(3.3),
			ASeriesFunctions::fixedPoint_4_9_ToDouble(ASeriesFunctions::doubleToFixedPoint_4_9(3.3)));
	QCOMPARE(ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(3.2998046875),
			ASeriesFunctions::fixedPoint_4_9_ToDouble(ASeriesFunctions::doubleToFixedPoint_4_9(3.2998046875)));
	QCOMPARE(ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(-3.2998046875),
			ASeriesFunctions::fixedPoint_4_9_ToDouble(ASeriesFunctions::doubleToFixedPoint_4_9(-3.2998046875)));
}

void TestASeriesFunctions::testFixedPoint_1_15_ToDouble() {

	QCOMPARE(ASeriesFunctions::fixedPoint_1_15_ToDouble(0xFFFF), -0.000030517578125);
	QCOMPARE(ASeriesFunctions::fixedPoint_1_15_ToDouble(0xFFFE), -0.00006103515625);
	QCOMPARE(ASeriesFunctions::fixedPoint_1_15_ToDouble(0x8000), -1.0);
	QCOMPARE(ASeriesFunctions::fixedPoint_1_15_ToDouble(0x8001), -0.999969482421875);
	QCOMPARE(ASeriesFunctions::fixedPoint_1_15_ToDouble(0x0000), 0.0);
	QCOMPARE(ASeriesFunctions::fixedPoint_1_15_ToDouble(0x0001), 0.000030517578125);
	QCOMPARE(ASeriesFunctions::fixedPoint_1_15_ToDouble(0x0F00), 0.1171875);
	QCOMPARE(ASeriesFunctions::fixedPoint_1_15_ToDouble(0x7FFF), 0.999969482421875);

}

void TestASeriesFunctions::testDoubleToFixedPoint_1_15() {

	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_1_15(-15.0), (int16_t)0x8000);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_1_15(-1.0), (int16_t)0x8000);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_1_15(-0.9), (int16_t)-0x7333);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_1_15(-0.3), (int16_t)-0x2666);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_1_15(0.0), (int16_t)0x0000);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_1_15(0.3), (int16_t)0x2666);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_1_15(0.9), (int16_t)0x7333);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_1_15(1.0), (int16_t)0x7FFF);
	QCOMPARE(ASeriesFunctions::doubleToFixedPoint_1_15(100.0123), (int16_t)0x7FFF);

}
