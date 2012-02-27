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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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




#include "TestTransferFunction.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "Value/DoubleValue.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "TransferFunction/TransferFunctionASeriesTanh.h"
#include "Math/ASeriesFunctions.h"
#include <iostream>
#include "Math/Math.h"

using namespace std;

using namespace nerd;

void TestTransferFunction::initTestCase() {
}

void TestTransferFunction::cleanUpTestCase() {
}

//Chris
void TestTransferFunction::testTransferFunction() {
	TransferFunctionAdapter tfa("TransferFunction1", -55.1, 100.2);
 
	DoubleValue *testParameter = new DoubleValue(123.4); //has to be on the heap!
	tfa.addParameter("TestParam", testParameter);

	QVERIFY(tfa.getName() == "TransferFunction1");
	QCOMPARE(tfa.getUpperBound(), 100.2);
	QCOMPARE(tfa.getLowerBound(), -55.1);
	QVERIFY(tfa.getParameter("TestParam") != 0);
	QVERIFY(tfa.getParameter("TestParam") == testParameter);
	

	//Copy Transferfunction.
	//The copy has the same type, name, bounds and parameters, the parameters are separate 
	//value objects.
	TransferFunction *tfa2 = tfa.createCopy();
	QVERIFY(tfa2 != 0);
	QVERIFY(dynamic_cast<TransferFunctionAdapter*>(tfa2) != 0);
	QVERIFY(tfa2->getName() == tfa.getName());
	QCOMPARE(tfa2->getUpperBound(), 100.2);
	QCOMPARE(tfa2->getLowerBound(), -55.1);
	QVERIFY(tfa2->getParameter("TestParam") != 0);
	QVERIFY(tfa2->getParameter("TestParam") != testParameter);
	QVERIFY(dynamic_cast<DoubleValue*>(tfa2->getParameter("TestParam")) != 0);
	QCOMPARE(dynamic_cast<DoubleValue*>(tfa2->getParameter("TestParam"))->get(), 123.4);
	
	delete tfa2;
	//don't delete the parameters as they are automatically destroyed by the TransferFunction.
}


//Chris
void TestTransferFunction::testTransferFunctionTanh() {
	TransferFunctionTanh tanh;

	DoubleValue *testParameter = new DoubleValue(123.4);
	tanh.addParameter("TestParam", testParameter);
	
	QVERIFY(tanh.getName() == "tanh");
	QCOMPARE(tanh.getUpperBound(), 1.0);
	QCOMPARE(tanh.getLowerBound(), -1.0);
	QVERIFY(tanh.getParameter("TestParam") != 0);
	QVERIFY(tanh.getParameter("TestParam") == testParameter);

	//Copy
	TransferFunction *tanh2 = tanh.createCopy();
	QVERIFY(tanh2 != 0);
	QVERIFY(dynamic_cast<TransferFunctionTanh*>(tanh2) != 0);
	QVERIFY(tanh2->getName() == tanh.getName());
	QCOMPARE(tanh2->getUpperBound(), 1.0);
	QCOMPARE(tanh2->getLowerBound(), -1.0);
	QVERIFY(tanh2->getParameter("TestParam") != 0);
	QVERIFY(tanh2->getParameter("TestParam") != testParameter);
	QVERIFY(dynamic_cast<DoubleValue*>(tanh2->getParameter("TestParam")) != 0);
	QCOMPARE(dynamic_cast<DoubleValue*>(tanh2->getParameter("TestParam"))->get(), 123.4);

	//Equals
	QVERIFY(tanh.equals(tanh2) == true);
	QVERIFY(tanh2->equals(&tanh) == true);

	TransferFunctionAdapter tfa("tanh", -1.0, 1.0);
	QVERIFY(tanh.equals(&tfa) == false); //wrong type
	//rest of equals is tested by ParameterizedObject.
	
	//calculation (sample based test)
	QVERIFY(Math::compareDoubles(tanh.transferActivation(10.0, 0), 1.0, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(5.0, 0), 0.9999, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(2.0, 0), 0.9640, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(1.0, 0), 0.7616, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(0.9, 0), 0.7163, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(0.5, 0), 0.4621, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(0.3, 0), 0.2913, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(0.1, 0), 0.0997, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(0.0, 0), 0.0, 4));

	QVERIFY(Math::compareDoubles(tanh.transferActivation(-10.0, 0), -1.0, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(-5.0, 0), -0.9999, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(-2.0, 0), -0.9640, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(-1.0, 0), -0.7616, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(-0.9, 0), -0.7163, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(-0.5, 0), -0.4621, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(-0.3, 0), -0.2913, 4));
	QVERIFY(Math::compareDoubles(tanh.transferActivation(-0.1, 0), -0.0997, 4));

	delete tanh2;
}

//Ferry
void TestTransferFunction::testTransferFunctionASeriesTanh() {
	TransferFunctionASeriesTanh aSeriesTanh;

	DoubleValue *testParameter = new DoubleValue(123.4);
	aSeriesTanh.addParameter("TestParam", testParameter);
	
	QVERIFY(aSeriesTanh.getName() == "ASeriesTanh");
	QCOMPARE(aSeriesTanh.getUpperBound(), 1.0);
	QCOMPARE(aSeriesTanh.getLowerBound(), -1.0);
	QVERIFY(aSeriesTanh.getParameter("TestParam") != 0);
	QVERIFY(aSeriesTanh.getParameter("TestParam") == testParameter);

	//Copy
	TransferFunction *aSeriesTanh2 = aSeriesTanh.createCopy();
	QVERIFY(aSeriesTanh2 != 0);
	QVERIFY(dynamic_cast<TransferFunctionASeriesTanh*>(aSeriesTanh2) != 0);
	QVERIFY(aSeriesTanh2->getName() == aSeriesTanh.getName());
	QCOMPARE(aSeriesTanh2->getUpperBound(), 1.0);
	QCOMPARE(aSeriesTanh2->getLowerBound(), -1.0);
	QVERIFY(aSeriesTanh2->getParameter("TestParam") != 0);
	QVERIFY(aSeriesTanh2->getParameter("TestParam") != testParameter);
	QVERIFY(dynamic_cast<DoubleValue*>(aSeriesTanh2->getParameter("TestParam")) != 0);
	QCOMPARE(dynamic_cast<DoubleValue*>(aSeriesTanh2->getParameter("TestParam"))->get(), 123.4);

	//Equals
	QVERIFY(aSeriesTanh.equals(aSeriesTanh2) == true);
	QVERIFY(aSeriesTanh2->equals(&aSeriesTanh) == true);

	TransferFunctionAdapter tfa("tanh", -1.0, 1.0);
	QVERIFY(aSeriesTanh.equals(&tfa) == false); //wrong type
	//rest of equals is tested by ParameterizedObject.

	delete aSeriesTanh2;

	//calculation
	QCOMPARE(aSeriesTanh.transferActivation(-10.0, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(-0x7FFF));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(-10.0, 0), -0.99997, 5));

	QCOMPARE(aSeriesTanh.transferActivation(10.0, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(0x7FFF));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(10.0, 0), 0.99997, 5));

	// last base
	QCOMPARE(aSeriesTanh.transferActivation(-5.0, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(-0x7FFD));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(-5.0, 0), -0.9999, 4));

	QCOMPARE(aSeriesTanh.transferActivation(5.0, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(0x7FFD));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(5.0, 0), 0.9999, 4));

	// last base + 1/2 delta
	QCOMPARE(aSeriesTanh.transferActivation(-5.0625, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(-0x7FFE));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(-5.0625, 0), -0.9999, 4));

	QCOMPARE(aSeriesTanh.transferActivation(5.0625, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(0x7FFE));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(5.0625, 0), 0.9999, 4));

	// 18th base == 17 * 0.125
	QCOMPARE(aSeriesTanh.transferActivation(-2.125, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(-0x7C66));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(-2.125, 0), -0.9719, 4));

	QCOMPARE(aSeriesTanh.transferActivation(2.125, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(0x7C66));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(2.125, 0), 0.9719, 4));

	// 18th base + 1/2 delta
	QCOMPARE(aSeriesTanh.transferActivation(-2.1875, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(-0x7CCB));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(-2.1875, 0), -0.9749, 4));

	QCOMPARE(aSeriesTanh.transferActivation(2.1875, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(0x7CCB));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(2.1875, 0), 0.9749, 4));

	// 6th base + 1/4 delta
	QCOMPARE(aSeriesTanh.transferActivation(-0.65625, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(-0x4991));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(-0.65625, 0), -0.5747, 4));

	QCOMPARE(aSeriesTanh.transferActivation(0.65625, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(0x4991));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(0.65625, 0), 0.5747, 4));

	// 0
	QCOMPARE(aSeriesTanh.transferActivation(0.0, 0), ASeriesFunctions::fixedPoint_1_15_ToDouble(0x0000));

	QVERIFY(Math::compareDoubles(aSeriesTanh.transferActivation(0.0, 0), 0.00000000, 8));

}
