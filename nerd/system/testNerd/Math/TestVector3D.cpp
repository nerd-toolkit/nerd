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



#include "TestVector3D.h"
#include "Core/Core.h"
#include "Math/Vector3D.h"
#include <math.h>

namespace nerd{

//verena
void TestVector3D::testVector() {
	Core::resetCore();
	
	Vector3D vector1(1,1,1);
	Vector3D vector2(0.001, 0.002, 3);
	QCOMPARE(vector1.getX(), 1.0);
	QCOMPARE(vector2.getX(), 0.001);
	QCOMPARE(vector2.getY(), 0.002);
	QCOMPARE(vector2.getZ(), 3.0);
	vector2.set(3.0, 0.122221, 0.3030003);
	QCOMPARE(vector2.getX(), 3.0);
	QCOMPARE(vector2.getY(), 0.122221);
	QCOMPARE(vector2.getZ(), 0.3030003);

	vector1 = vector2;
	QCOMPARE(vector1.getX(), 3.0);
	QCOMPARE(vector1.getY(), 0.122221);
	QCOMPARE(vector1.getZ(), 0.3030003);

	vector1.setX(1.123456789);
	vector1.setY(9.87654321);
	vector1.setZ(100000);
	QCOMPARE(vector1.getX(), 1.123456789);
	QCOMPARE(vector1.getY(), 9.87654321);
	QCOMPARE(vector1.getZ(), 100000.0);
	
	Vector3D toBeNormalized(2.0, 0.0, 0.0);
	QCOMPARE(toBeNormalized.getX(), 2.0);
	QCOMPARE(toBeNormalized.getY(), 0.0);
	QCOMPARE(toBeNormalized.getZ(), 0.0);
	toBeNormalized.normalize();
	QCOMPARE(toBeNormalized.getX(), 1.0);
	QCOMPARE(toBeNormalized.getY(), 0.0);
	QCOMPARE(toBeNormalized.getZ(), 0.0);
	
	double n = sqrt(14.0);
	double first = 1.0/n;
	double second = 2.0/n;
	double third = 3.0/n;
	toBeNormalized.setX(1.0);
	toBeNormalized.setY(2.0);
	toBeNormalized.setZ(3.0);
	QCOMPARE(toBeNormalized.length(), n);
	toBeNormalized.normalize();
	QCOMPARE(toBeNormalized.getX(), first);
	QCOMPARE(toBeNormalized.getY(), second);
	QCOMPARE(toBeNormalized.getZ(), third);


}


//chris
void TestVector3D::testEquals() {
	Core::resetCore();
	
	Vector3D vector1(0, 1, 2);
	Vector3D vector2(0, 1, 2);
	Vector3D vector3(0.1, 1, 2);
	Vector3D vector4(0, 1.001, 2);
	Vector3D vector5(0, 1, 1.99992);

	QVERIFY(vector1.equals(vector2) == true);
	QVERIFY(vector1.equals(vector3) == false);
	
	QVERIFY(vector1.equals(vector4, 2) == true);
	QVERIFY(vector1.equals(vector4, 3) == false);
	QVERIFY(vector4.equals(vector1, 2) == true);
	QVERIFY(vector4.equals(vector1, 3) == false);
	
	QVERIFY(vector2.equals(vector5, 3) == true);
	QVERIFY(vector2.equals(vector5, 4) == false);
	QVERIFY(vector5.equals(vector2, 3) == true);
	QVERIFY(vector5.equals(vector2, 4) == false);
	

}

//Verena
void TestVector3D::testOperatorMultiplication() {
	Core::resetCore();
	
	Vector3D vector1(1,2,3);
	Vector3D vector2(0.5,0.25,1);
	
	double product = 4;
	QVERIFY(product == vector1*vector2);
	
	product = 0.25 + 0.0625 + 1;
	QVERIFY(product == vector2*vector2);

	vector2.set(-2.0, 0.5, -1.0);
	QCOMPARE(vector1*vector2, -4.0);	

	QCOMPARE(vector1*vector2, vector2*vector1);

// scalar multiplication
	double a = -1.5;
	Vector3D v(2.0, 4.5, -22.0);

	Vector3D vProduct1 = a*v;
	
	Vector3D vProduct2 = v*a;

	QVERIFY(vProduct1.equals(vProduct2));
	
	QCOMPARE(vProduct1.getX(), -3.0);
	QCOMPARE(vProduct1.getY(), -6.75);
	QCOMPARE(vProduct1.getZ(), 33.0);


}


//Verena
void TestVector3D::testOperatorAddition() {
	Core::resetCore();
	
	Vector3D vector1(1,2,3);
	Vector3D vector2(0.5,0.25,1);
	
	Vector3D result(1.5,2.25, 4);	
	Vector3D add = (vector1+vector2);
	QVERIFY(result.equals(add));
	QVERIFY(vector1.equals(vector1+Vector3D(0,0,0)));

	Vector3D sum1 = (vector1 + vector2);
	Vector3D sum2 = (vector2 + vector1);
	QVERIFY(sum1.equals(sum2));
	

}

//Verena
void TestVector3D::testOperatorSubtraction() {
	Core::resetCore();
	
	Vector3D vector1(1,2,3);
	Vector3D vector2(0.5,0.25,1);
	Vector3D diff =  vector1 - vector2;
	QVERIFY(diff.equals(vector2 - vector1) == false);
	Vector3D sub = ((vector1+vector2) - vector2);
	QVERIFY(sub.equals(vector1) == true);
	sub = sub - sub;
	QVERIFY(sub.equals(Vector3D(0,0,0)) == true);
	QVERIFY(vector2.equals(vector2-Vector3D(0,0,0)) == true);


}

// Verena
void TestVector3D::testOperatorNegative() {
	Core::resetCore();
	
	Vector3D original(0.5, 1, 9);
	Vector3D negative(-0.5,-1, -9);
	QVERIFY(negative.equals(-original) == true);
	

}

// Verena
void TestVector3D::testMethods() {
	Core::resetCore();
	Vector3D one(1.0, 2.0, 3.0);
	Vector3D two(2.0, 3.0, 4.0);

	Vector3D cross = Vector3D::crossProduct(one, two);
	QCOMPARE(cross.getX(), -1.0);
	QCOMPARE(cross.getY(), 2.0);
	QCOMPARE(cross.getZ(), -1.0);


	one.set(1.5, 2.11, 10.02);
	two.set(-2.0, 0.005, 400.0);

	cross = Vector3D::crossProduct(one, two);
	QCOMPARE(cross.getX(), 843.9499);
	QCOMPARE(cross.getY(), -620.04);
	QCOMPARE(cross.getZ(), 4.2275);


	double length = two.length();

	double expected = sqrt(160004.000025);
	QCOMPARE(length, expected);

	Vector3D vector(2.0, 5.0, 6.5);
	double lengthVector = vector.length();
	QCOMPARE(lengthVector, sqrt(71.25));

	Vector3D normalized = vector;
	normalized.normalize();
	QCOMPARE(normalized.length(), 1.0);
	QCOMPARE(normalized.getX(), vector.getX()/lengthVector);
	QCOMPARE(normalized.getY(), vector.getY()/lengthVector);
	QCOMPARE(normalized.getZ(), vector.getZ()/lengthVector);


}

}
