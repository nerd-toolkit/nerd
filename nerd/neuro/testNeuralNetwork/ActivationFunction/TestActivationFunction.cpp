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




#include "TestActivationFunction.h"
#include "Value/DoubleValue.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "ActivationFunction/ASeriesActivationFunction.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "Network/Neuron.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "Network/Synapse.h"
#include "Network/SynapseAdapter.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "SynapseFunction/ASeriesSynapseFunction.h"
#include "Math/ASeriesFunctions.h"
#include <iostream>
#include "Math/Math.h"
#include "ActivationFunctionAdapter.h"

using namespace std;

using namespace nerd;

void TestActivationFunction::initTestCase() {
}

void TestActivationFunction::cleanUpTestCase() {
}

//Chris
void TestActivationFunction::testAdditiveTimeDiscreteActivationFunction() {
	AdditiveTimeDiscreteActivationFunction af;
	DoubleValue *param = new DoubleValue(62718.81239);
	af.addParameter("Param", param);
	
	//construction
	QVERIFY(af.getName() == "AdditiveTD");
	QVERIFY(af.getParameter("Param") == param);
	QVERIFY(dynamic_cast<DoubleValue*>(af.getParameter("Param")) != 0);
	
	//copy
	ActivationFunction *af2 = af.createCopy();
	QVERIFY(af2->getName() == "AdditiveTD");
	QVERIFY(af2->getParameter("Param") != param);
	QVERIFY(dynamic_cast<DoubleValue*>(af2->getParameter("Param")) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(af2->getParameter("Param"))->get() == 62718.81239);

	//equals
	QVERIFY(af.equals(af2) == true);
	QVERIFY(af2->equals(&af) == true);

	ActivationFunctionAdapter afa("AdditiveTD");
	QVERIFY(af.equals(&afa) == false); //wrong type
	//the rest of equals is tested in ParameterizedObject.

	//calculate
	QVERIFY(af.calculateActivation(0) == 0.0); //this is the default behavior in absense of a Neuron.

	Neuron n("Neuron1", TransferFunctionAdapter("", 0.0, 1.0), af);
	Neuron n2("Neuron2", TransferFunctionAdapter("", 0.0, 1.0), af);
	Neuron n3("Neuron3", TransferFunctionAdapter("", 0.0, 1.0), af);
	SynapseAdapter *s1 = new SynapseAdapter(&n, &n, 1.0, SimpleSynapseFunction());
	SynapseAdapter *s2 = new SynapseAdapter(&n2, &n2, 159.0001, SimpleSynapseFunction());
	SynapseAdapter *s3 = new SynapseAdapter(&n3, &n3, 0.12121, SimpleSynapseFunction());

	QVERIFY(af.calculateActivation(&n) == 0.0);
	
	n.getBiasValue().set(0.355);
	QVERIFY(af.calculateActivation(&n) == 0.355);

	QVERIFY(n.addSynapse(s1));
	QVERIFY(af.calculateActivation(&n) == 1.355);

	QVERIFY(n.addSynapse(s2));
	QVERIFY(n.addSynapse(s3));
	QVERIFY(Math::compareDoubles(af.calculateActivation(&n), 160.47631, 5));
	
	for(int i = 0; i < 10; ++i) {
		//multiple calles poduce the same output (history independent)
		QVERIFY(Math::compareDoubles(af.calculateActivation(&n), 160.47631, 5));
	}

	//use same activation function with another neuron.
	n2.getBiasValue().set(99.991);
	QVERIFY(Math::compareDoubles(af.calculateActivation(&n2), 99.991, 3));

	delete af2;
}


//Ferry
void TestActivationFunction::testASeriesActivationFunction() {
	ASeriesActivationFunction af;
	DoubleValue *param = new DoubleValue(62718.81239);
	af.addParameter("Param", param);
	
	//construction
	QVERIFY(af.getName() == "ASeries");
	QVERIFY(af.getParameter("Param") == param);
	QVERIFY(dynamic_cast<DoubleValue*>(af.getParameter("Param")) != 0);
	
	//copy
	ActivationFunction *af2 = af.createCopy();
	QVERIFY(af2->getName() == "ASeries");
	QVERIFY(af2->getParameter("Param") != param);
	QVERIFY(dynamic_cast<DoubleValue*>(af2->getParameter("Param")) != 0);
	QCOMPARE(dynamic_cast<DoubleValue*>(af2->getParameter("Param"))->get(), 62718.81239);

	//equals
	QVERIFY(af.equals(af2) == true);
	QVERIFY(af2->equals(&af) == true);

	ActivationFunctionAdapter afa("AdditiveTD");
	QVERIFY(af.equals(&afa) == false); //wrong type
	//the rest of equals is tested in ParameterizedObject.

	//calculate
	QVERIFY(af.calculateActivation(0) == 0.0); //this is the default behavior in absense of a Neuron.

	Neuron n("Neuron1", TransferFunctionAdapter("", 0.0, 1.0), af);
	Neuron n2("Neuron2", TransferFunctionAdapter("", 0.0, 1.0), af);

	// no bias, no synapses
	QVERIFY(af.calculateActivation(&n) == 0.0);
	
	// only a bias value, no synapses
	n.getBiasValue().set(7.3);
	int32_t fpActivation = ASeriesFunctions::doubleToFixedPoint_8_24(7.3);
	QCOMPARE(af.calculateActivation(&n),
			 ASeriesFunctions::fixedPoint_8_24_ToDouble(fpActivation));

	// one synapse
	SynapseAdapter *s1 = new SynapseAdapter(&n, &n, -0.3, ASeriesSynapseFunction());
	QVERIFY(n.addSynapse(s1));
	
	fpActivation += ASeriesFunctions::doubleToFixedPoint_8_24(-0.3);
	QCOMPARE(af.calculateActivation(&n), ASeriesFunctions::fixedPoint_8_24_ToDouble(fpActivation));

	// another synapse
	SynapseAdapter *s2 = new SynapseAdapter(&n2, &n2, 0.7, ASeriesSynapseFunction());
	QVERIFY(n.addSynapse(s2));

	fpActivation += ASeriesFunctions::doubleToFixedPoint_8_24(0.7);
	QCOMPARE(af.calculateActivation(&n), ASeriesFunctions::fixedPoint_8_24_ToDouble(fpActivation));

	for(int i = 0; i < 10; ++i) {
		//multiple calles poduce the same output (history independent)
		QCOMPARE(af.calculateActivation(&n), ASeriesFunctions::fixedPoint_8_24_ToDouble(fpActivation));
	}

	//use same activation function with another neuron.
	n2.getBiasValue().set(99.991);
	QCOMPARE(af.calculateActivation(&n2),
			 ASeriesFunctions::fixedPoint_8_24_ToDouble(ASeriesFunctions::doubleToFixedPoint_8_24(99.991)));


	delete af2;
}

void TestActivationFunction::testObservableParameters() {
	ActivationFunctionAdapter afa("ActivationFunction1");
	
	QVERIFY(afa.getObservableOutputNames().empty());
	Value v1, v2, v3;
	QVERIFY(afa.addObserableOutput("Value1", &v1));
	QVERIFY(afa.addObserableOutput("Value2", &v2));
	QVERIFY(afa.addObserableOutput("Value3", &v3));
	
	QVERIFY(afa.getObservableOutputNames().size() == 3);
	QVERIFY(afa.getObservableOutputNames().contains("Value1"));
	QVERIFY(afa.getObservableOutputNames().contains("Value2"));
	QVERIFY(afa.getObservableOutputNames().contains("Value3"));
	
	QVERIFY(afa.getObservableOutput("Value1") == &v1);
	QVERIFY(afa.getObservableOutput("Value2") == &v2);
	QVERIFY(afa.getObservableOutput("Value3") == &v3);

	//same value with different name ok.
	QVERIFY(afa.addObserableOutput("Value4", &v1));
	QVERIFY(afa.getObservableOutputNames().size() == 4);
	QVERIFY(afa.getObservableOutput("Value4") == &v1);
	
	QVERIFY(afa.getObservableOutput("Dummy") == 0);
	
	//a value with existing name not ok.
	Value v4;
	QVERIFY(afa.addObserableOutput("Value1", &v4) == false);
	QVERIFY(afa.getObservableOutputNames().size() == 4);
	QVERIFY(afa.getObservableOutput("Value1") == &v1);
	
	QVERIFY(afa.addObserableOutput("Value5", 0) == 0);
	QVERIFY(afa.getObservableOutputNames().size() == 4);
	QVERIFY(afa.getObservableOutputNames().contains("Value5") == false);
}
