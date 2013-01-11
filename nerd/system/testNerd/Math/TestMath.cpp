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


#include "TestMath.h"
#include "Math/Math.h"
#include <Core/Core.h>
#include <iostream>

using namespace std;

namespace nerd{

//chris
void TestMath::testRound() {
	double number1 = 0.34545;
	double rounded1 = Math::round(number1, 2);
	QCOMPARE(rounded1, 0.35);
	double negativeRounded1 = Math::round(-1.0 * number1, 2);
	QCOMPARE(negativeRounded1, -0.35);

	double number2 = 33.9754;
	double rounded2 = Math::round(number2, 0);
	QCOMPARE(rounded2, 34.0);

	double rounded2a = Math::round(number2, 1);
	QCOMPARE(rounded2a, 34.0);
	double negativeRounded2a = Math::round(-1.0 * number2, 1);
	QCOMPARE(negativeRounded2a, -34.0);
	
	

}


//ferry
void TestMath::testForceToRange() {

	int f = Math::forceToRange(13000, 0, 1);
	QCOMPARE(f, 1);
	// if min > max forceToRange should return min
	f = Math::forceToRange(13000, 200, 100);
	QCOMPARE(f, 200);
	
	f = Math::forceToRange(13000, 12999, 13000);
	QCOMPARE(f, 13000);

	f = Math::forceToRange(13000, 13000, 13000);
	QCOMPARE(f, 13000);

	f = Math::forceToRange(0, 0, 0);
	QCOMPARE(f, 0);
}


//chris
void TestMath::testMinMax() {
	QCOMPARE(Math::min(-10, -100), -100);
	QCOMPARE(Math::min(10, 100), 10);
	QCOMPARE(Math::min(10, -100), -100);
	QCOMPARE(Math::min(50, 50), 50);

	QCOMPARE(Math::max(-10, -100), -10);
	QCOMPARE(Math::max(10, 100), 100);
	QCOMPARE(Math::max(10, -100), 10);
	QCOMPARE(Math::max(50, 50), 50);

	QCOMPARE(Math::min(-1.062, -1.696), -1.696);
	QCOMPARE(Math::min(55.555, 12.6), 12.6);
	QCOMPARE(Math::min(-1.696, -1.696), -1.696);
	QCOMPARE(Math::min(-1.062, 55.12), -1.062);
	
	QCOMPARE(Math::max(-1.062, -1.696), -1.062);
	QCOMPARE(Math::max(55.555, 12.6), 55.555);
	QCOMPARE(Math::max(-1.696, -1.696), -1.696);
	QCOMPARE(Math::max(-1.062, 55.12), 55.12);
}


//Chris
void TestMath::testCompare() {
	QVERIFY(Math::compareDoubles(123.456789, 123.4567, 5) == false);
	QVERIFY(Math::compareDoubles(123.456789, 123.4567, 4) == true);
	QVERIFY(Math::compareDoubles(123.456789, 123.456699, 4) == true);
	QVERIFY(Math::compareDoubles(23.456789, 123.4567, 5) == false);
	QVERIFY(Math::compareDoubles(123.4567891234567, 123.456789123456, 13) == false);
	QVERIFY(Math::compareDoubles(123.4567891234567, 123.456789123456, 12) == true);	
	QVERIFY(Math::compareDoubles(123.4567891234561, 123.4567891234556, 12) == true);

	QVERIFY(Math::compareDoubles(123.456789, 123.4567, 0.00001) == false);
	QVERIFY(Math::compareDoubles(123.456789, 123.4567, 0.0001) == true);
	QVERIFY(Math::compareDoubles(123.456789, 123.45669, 0.0001) == true);
	QVERIFY(Math::compareDoubles(123.0001, 123.0, 0.000100000001) == true); //for numerical errors
	QVERIFY(Math::compareDoubles(123.4567891234567, 123.456789123456, 0.0000000000001001) == false);
	QVERIFY(Math::compareDoubles(123.4567891234567, 123.456789123456, 0.0000000000010001) == true);	
}


//Chris
void TestMath::testDistance() {
	QVERIFY(Math::compareDoubles((double) Math::distance(QPoint(-2, 1), QPoint(1, 5)), 5.0, 0.0001));
	QVERIFY(Math::compareDoubles((double) Math::distance(QPoint(1, 5), QPoint(-2, 1)), 5.0, 0.0001));

	QVERIFY(Math::compareDoubles((double) Math::distance(QPoint(1, 5), QPoint(-2, 2)), 5.0, 0.0001) == false);

	QVERIFY(Math::compareDoubles((double) Math::distance(QPoint(-2, -3), QPoint(-4, 4)), 7.28, 0.001));
	QVERIFY(Math::compareDoubles((double) Math::distance(QPoint(-2, -3), QPoint(-2, -3)), 0.0, 0.001));
	QVERIFY(Math::compareDoubles((double) Math::distance(QPoint(0, 2), QPoint(0, 5)), 3.0, 0.001));
	QVERIFY(Math::compareDoubles((double) Math::distance(QPoint(0, 2), QPoint(1001, 2)), 1001.0, 0.001));
}


//Chris
void TestMath::testFactorial() {
	Core::resetCore();
	
	Math::mFactorials.clear();
	
	QVERIFY(Math::mFactorials.size() == 0);
	QCOMPARE(Math::factorial(0),  (long long) 0);
	QVERIFY(Math::mFactorials.size() == 2);
	QCOMPARE(Math::factorial(1),  (long long) 1);
	QVERIFY(Math::mFactorials.size() == 2);
	QCOMPARE(Math::factorial(3),  (long long) 6);
	QVERIFY(Math::mFactorials.size() == 4);
	QCOMPARE(Math::factorial(1), (long long) 1);
	QVERIFY(Math::mFactorials.size() == 4);
	QCOMPARE(Math::factorial(8), (long long) 40320);
	QVERIFY(Math::mFactorials.size() == 9);
	QCOMPARE(Math::factorial(8), (long long) 40320);
	QVERIFY(Math::mFactorials.size() == 9);
	QCOMPARE(Math::factorial(0), (long long) 0);
	QVERIFY(Math::mFactorials.size() == 9);
	QCOMPARE(Math::factorial(-1), (long long) 0); //only positive ints defined...
	QVERIFY(Math::mFactorials.size() == 9);
	QCOMPARE(Math::factorial(-100), (long long) 0); //only positive
	QVERIFY(Math::mFactorials.size() == 9);
	QCOMPARE(Math::factorial(1),  (long long) 1);
	QVERIFY(Math::mFactorials.size() == 9);
	QCOMPARE(Math::factorial(10),  3628800LL);
	QVERIFY(Math::mFactorials.size() == 11);
	QCOMPARE(Math::factorial(12),  479001600LL);
	QVERIFY(Math::mFactorials.size() == 13);
	QCOMPARE(Math::factorial(50),  479001600LL); //is truncated to 12!
	QVERIFY(Math::mFactorials.size() == 13);

	Core::resetCore();
}

}
