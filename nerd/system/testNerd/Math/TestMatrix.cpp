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




#include "TestMatrix.h"
#include <iostream>
#include "Core/Core.h"
#include "Math/Matrix.h"

using namespace std;
using namespace nerd;

void TestMatrix::initTestCase() {
}

void TestMatrix::cleanUpTestCase() {
}

//chris
void TestMatrix::testConstruction() {
	Matrix m1;
	QCOMPARE(m1.getWidth(), 1);
	QCOMPARE(m1.getHeight(), 1);
	QCOMPARE(m1.getDepth(), 1);

	Matrix m2(2, 3, 4);
	QCOMPARE(m2.getWidth(), 2);
	QCOMPARE(m2.getHeight(), 3);
	QCOMPARE(m2.getDepth(), 4);
}

//chris
void TestMatrix::testSetAndGet() {
	Matrix m1(2, 3, 4);

	QCOMPARE(m1.get(0, 0, 0), 0.0);
	m1.set(11.22, 0, 0, 0);
	QCOMPARE(m1.get(0, 0, 0), 11.22);
	m1.set(11.33, 1, 1, 1);
	QCOMPARE(m1.get(1, 1, 1), 11.33);
	m1.set(15.55, 1, 2, 3);
	QCOMPARE(m1.get(1, 2, 3), 15.55);
	
	//access non-existing index
	QCOMPARE(m1.get(5, 5, 5), 0.0);

}

//chris
void TestMatrix::testResize() {

	Matrix m1(3, 2, 5);

	QCOMPARE(m1.getWidth(), 3);
	QCOMPARE(m1.getHeight(), 2);
	QCOMPARE(m1.getDepth(), 5);

	m1.resize(100, 10, 5);
	
	QCOMPARE(m1.getWidth(), 100);
	QCOMPARE(m1.getHeight(), 10);
	QCOMPARE(m1.getDepth(), 5);

	m1.set(9.999111, 99, 9, 4);

	QCOMPARE(m1.get(99, 9, 4), 9.999111);

	m1.resize(-10, -5, -9);
	
	QCOMPARE(m1.getWidth(), 0);
	QCOMPARE(m1.getHeight(), 0);
	QCOMPARE(m1.getDepth(), 0);
}

//chris
void TestMatrix::testFillAndClear() {
	Matrix m1(3, 2, 5);

	QCOMPARE(m1.getWidth(), 3);
	QCOMPARE(m1.getHeight(), 2);
	QCOMPARE(m1.getDepth(), 5);

	QVERIFY(allValuesEqual(m1, 0));

	m1.fill(9765);
	QVERIFY(allValuesEqual(m1, 9765));

	m1.fill(-9862.07);
	QVERIFY(allValuesEqual(m1, -9862.07));

	m1.fill(-0.0000001);
	QVERIFY(allValuesEqual(m1, -0.0000001));

	//clear
	QCOMPARE(m1.getWidth(), 3);
	QCOMPARE(m1.getHeight(), 2);
	QCOMPARE(m1.getDepth(), 5);
	m1.clear();
	QCOMPARE(m1.getWidth(), 0);
	QCOMPARE(m1.getHeight(), 0);
	QCOMPARE(m1.getDepth(), 0);

	
}

bool TestMatrix::allValuesEqual(Matrix &matrix, double value) {
	for(int i = 0; i < matrix.getWidth(); ++i) {
		for(int j = 0; j < matrix.getHeight(); ++j) {
			for(int k = 0; k < matrix.getDepth(); ++k) {
				if(matrix.get(i, j, k) != value) {
					cerr << "TestMatrix: Matrix entry [" << i << "," << j << "," << k << "] was not "
						 << value << " (instead: " << matrix.get(i, j, k) << ")" << endl;
					return false;
				}
			}
		}
	}
	return true;
}



