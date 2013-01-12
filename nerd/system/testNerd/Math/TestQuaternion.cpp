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



#include "TestQuaternion.h"
#include "Core/Core.h"
#include "Math/Quaternion.h"
#include "Math/Math.h"
#include <math.h>
#include <iostream>

using namespace std;


namespace nerd {

// Verena
void TestQuaternion::testQuaternionManipulation(){
	Core::resetCore();
	
	Quaternion first(1,2,3,4);
	QCOMPARE(first.getW(),1.0);
	QCOMPARE(first.getX(),2.0);
	QCOMPARE(first.getY(),3.0);
	QCOMPARE(first.getZ(),4.0);

	first.set(4,3,2,1);
	QCOMPARE(first.getW(),4.0);
	QCOMPARE(first.getX(),3.0);
	QCOMPARE(first.getY(),2.0);
	QCOMPARE(first.getZ(),1.0);
	

	first.setW(2.1);
	QCOMPARE(first.getW(), 2.1);
	first.setX(2.2);
	QCOMPARE(first.getX(), 2.2);
	first.setY(-1.2);
	QCOMPARE(first.getY(), -1.2);
	first.setZ(11.2);
	QCOMPARE(first.getZ(), 11.2);

	first.set(4,3,2,1);
	
	double array[4] = {4.0, 3.0, 2.0, 1.0};
	const double* quatArray = first.get();
	
	QCOMPARE(array[0], quatArray[0]);
	QCOMPARE(array[1], quatArray[1]);
	QCOMPARE(array[2], quatArray[2]);
	QCOMPARE(array[3], quatArray[3]);
	
	Quaternion q2(2, 4, 6, 8);
	QCOMPARE(q2.getW(), 2.0);
	QCOMPARE(q2.getX(), 4.0);
	QCOMPARE(q2.getY(), 6.0);
	QCOMPARE(q2.getZ(), 8.0);
	
	q2.set(first);
	QCOMPARE(q2.getW(), 4.0);
	QCOMPARE(q2.getX(), 3.0);
	QCOMPARE(q2.getY(), 2.0);
	QCOMPARE(q2.getZ(), 1.0);

	first.setY(4.4);
	QCOMPARE(first.getY(), 4.4);
	

}

// Verena
void TestQuaternion::testQuaternionMath() {
	Core::resetCore();

	Quaternion original(1, 1, 1, 1);
	QCOMPARE(original.length(), 2.0);
	Quaternion conjugate = original.getConjugate();
	QCOMPARE(conjugate.getW(), 1.0);	
	QCOMPARE(conjugate.getX(), -1.0);
	QCOMPARE(conjugate.getY(), -1.0);
	QCOMPARE(conjugate.getZ(), -1.0);

	Quaternion inverse = original.getInverse();
	Quaternion expectedInverse(0.25, -0.25, -0.25, -0.25);
	QVERIFY(inverse.equals(expectedInverse) == true); 

	Quaternion zero(0, 0, 0, 0);
	QVERIFY(zero.equals(zero.getInverse()));

	Quaternion first(1, 2, 3, 4);
	Quaternion second(1, 2, 3, 4);
	QVERIFY(first.equals(second) == true);
	QVERIFY(first.equals(second, -1) == true);
	
	first.set(1, 2, 3, 4);
	QCOMPARE(first.length(), sqrt(30.0));
	first.normalize();
	QCOMPARE(first.length(), 1.0);
	Quaternion normalized(1.0 / sqrt(30.0), 2.0 / sqrt(30.0), 3.0 / sqrt(30.0), 4.0 / sqrt(30.0));
	QVERIFY(normalized.equals(first));

	first.setFromAngles(0,0,0);
	QVERIFY(Math::compareDoubles(first.getW(), 1.0,3));
	QVERIFY(Math::compareDoubles(first.getX(), 0.0, 3));
	QVERIFY(Math::compareDoubles(first.getY(), 0.0, 3));
	QVERIFY(Math::compareDoubles(first.getZ(), 0.0, 3));

	first.setFromAngles(90, 0, 0);
	QVERIFY(Math::compareDoubles(first.getW(), 0.707, 3));
	QVERIFY(Math::compareDoubles(first.getX(), 0.707, 3));
	QVERIFY(Math::compareDoubles(first.getY(), 0.0, 3));
	QVERIFY(Math::compareDoubles(first.getZ(), 0.0, 3));

	first.setFromAngles(45, 45, 0);
	QVERIFY(Math::compareDoubles(first.getW(), 0.853, 2));
	QVERIFY(Math::compareDoubles(first.getX(), 0.353, 2));
	QVERIFY(Math::compareDoubles(first.getY(), 0.353, 2));
	QVERIFY(Math::compareDoubles(first.getZ(), -0.146, 2));

	//TODO: online calculators calculate a quaternion: 0.925, -0.163, -0.059, 0.336 for these angles. Are quaterions w,x,y,z and -w,-x-y-z equal??
	first.setFromAngles(-20, 0, 400);
	QVERIFY(Math::compareDoubles(first.getW(), -0.925, 2));
	QVERIFY(Math::compareDoubles(first.getX(), 0.163, 2));
	QVERIFY(Math::compareDoubles(first.getY(), 0.059, 2));
	QVERIFY(Math::compareDoubles(first.getZ(), -0.336, 2));

}

// Verena
void TestQuaternion::testOperators() {
	Core::resetCore();

	Quaternion first(1,2,3,4);
	Quaternion second(5,6,7,8);
	
	Quaternion diff = second - first;
	QCOMPARE(diff.getW(), 4.0);
	QCOMPARE(diff.getX(), 4.0);
	QCOMPARE(diff.getY(), 4.0);
	QCOMPARE(diff.getZ(), 4.0);

	Quaternion sum = diff + second;
	QCOMPARE(sum.getW(), 9.0);
	QCOMPARE(sum.getX(), 10.0);
	QCOMPARE(sum.getY(), 11.0);
	QCOMPARE(sum.getZ(), 12.0);
	
	Quaternion sum2 = second + diff;
	QCOMPARE(sum2.getW(), 9.0);
	QCOMPARE(sum2.getX(), 10.0);
	QCOMPARE(sum2.getY(), 11.0);
	QCOMPARE(sum2.getZ(), 12.0);
	QVERIFY( sum.equals(sum2) == true);

	Quaternion product = first * second;
	QCOMPARE(product.getW(), -60.0);
	QCOMPARE(product.getX(), 12.0);
	QCOMPARE(product.getY(), 30.0);
	QCOMPARE(product.getZ(), 24.0);
	QVERIFY(product.equals(second * first) == false);

	double factor = 1.5;
	Quaternion third(-10.0, 2.5, 4.0, -1.0);
	Quaternion scalarProduct = factor * third;
	Quaternion result(-15.0, 3.75, 6.0, -1.5);
	QVERIFY(result.equals(scalarProduct) == true);
	QVERIFY(scalarProduct.equals( third * factor) == true);

}

}
