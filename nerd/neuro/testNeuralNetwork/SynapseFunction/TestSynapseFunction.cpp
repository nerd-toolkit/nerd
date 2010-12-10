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




#include "TestSynapseFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "SynapseFunction/ASeriesSynapseFunction.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "Value/DoubleValue.h"
#include "Network/Synapse.h"
#include "Math/ASeriesFunctions.h"

using namespace nerd;

void TestSynapseFunction::initTestCase() {
}

void TestSynapseFunction::cleanUpTestCase() {
}


//Chris
void TestSynapseFunction::testSimpleSynapseFunction() {
	
	SimpleSynapseFunction sf;
	DoubleValue *param1 = new DoubleValue(543.1);
	sf.addParameter("TestParam", param1);

	//Construction
	QVERIFY(sf.getName() == "SimpleUpdateFunction");
	QVERIFY(sf.getParameter("TestParam") == param1);


	//Copy
	SynapseFunction *sf2 = sf.createCopy();
	QVERIFY(sf2 != 0);
	QVERIFY(dynamic_cast<SimpleSynapseFunction*>(sf2) != 0);
	QVERIFY(sf2->getName() == sf.getName());
	QVERIFY(sf2->getParameter("TestParam") != param1);
	QVERIFY(dynamic_cast<DoubleValue*>(sf2->getParameter("TestParam")) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(sf2->getParameter("TestParam"))->get() == 543.1);

	//Equals
	QVERIFY(sf.equals(sf2) == true);
	QVERIFY(sf2->equals(&sf) == true);

	SynapseFunctionAdapter sfa("SimpleUpdateFunction");
	QVERIFY(sf.equals(&sfa) == false); //wrong type
	//Rest of equals is tested by TestParameterizedObject.

	//calculation
	Synapse synapse1(0, 0, 123.4, SimpleSynapseFunction());
	Synapse synapse2(0, 0, 9.651234, SimpleSynapseFunction());

	QCOMPARE(sf.calculate(&synapse1), 123.4);
	QCOMPARE(sf.calculate(&synapse2), 9.651234);

	delete sf2;
}


//Ferry
void TestSynapseFunction::testASeriesSynapseFunction() {
	ASeriesSynapseFunction sf;

	DoubleValue *param1 = new DoubleValue(543.1);
	sf.addParameter("TestParam", param1);

	//Construction
	QVERIFY(sf.getName() == "ASeries");
	QVERIFY(sf.getParameter("TestParam") == param1);

	//Copy
	SynapseFunction *sf2 = sf.createCopy();
	QVERIFY(sf2 != 0);
	QVERIFY(dynamic_cast<ASeriesSynapseFunction*>(sf2) != 0);
	QVERIFY(sf2->getName() == sf.getName());
	QVERIFY(sf2->getParameter("TestParam") != param1);
	QVERIFY(dynamic_cast<DoubleValue*>(sf2->getParameter("TestParam")) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(sf2->getParameter("TestParam"))->get() == 543.1);

	//Equals
	QVERIFY(sf.equals(sf2) == true);
	QVERIFY(sf2->equals(&sf) == true);

	SynapseFunctionAdapter sfa("SimpleUpdateFunction");
	QVERIFY(sf.equals(&sfa) == false); //wrong type
	//Rest of equals is tested by TestParameterizedObject.

	//calculation

	Synapse synapse1(0, 0, 8.0, ASeriesSynapseFunction());
	QCOMPARE(sf.calculate(&synapse1), ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(8.0));

	Synapse synapse2(0, 0, 8000.0, ASeriesSynapseFunction());
	QCOMPARE(sf.calculate(&synapse2), ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(8000.0));

	Synapse synapse3(0, 0, -9.0, ASeriesSynapseFunction());
	QCOMPARE(sf.calculate(&synapse3), ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(-9.0));

	Synapse synapse4(0, 0, 3.3, ASeriesSynapseFunction());
	QCOMPARE(sf.calculate(&synapse4), ASeriesFunctions::mapDoubleToFixedPoint_4_9_Precision(3.3));

	delete sf2;
}
