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




#include "TestDynamicsPlotterUtil.h"
#include <iostream>
#include <sstream>
#include "Core/Core.h"
#include "Network/NeuralNetwork.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "ActivationFunction/SignalGeneratorActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "TransferFunction/TransferFunctionRamp.h"
#include "TransferFunction/TransferFunctionParameterizedSigmoid.h"
#include "Adapters/TransferFunctionAdapter.h"
#include <Util/NeuralNetworkUtil.h>
#include <Util/DynamicsPlotterUtil.h>

using namespace std;
using namespace nerd;

void TestDynamicsPlotterUtil::initTestCase() {
}

void TestDynamicsPlotterUtil::cleanUpTestCase() {
}

//josef
void TestDynamicsPlotterUtil::testGetElementValue() {
	AdditiveTimeDiscreteActivationFunction af;
	SignalGeneratorActivationFunction saf;
	TransferFunctionRamp ramp("ramp", -1, 1, false);
	TransferFunctionParameterizedSigmoid pasig(0.221, 0.987);
	SimpleSynapseFunction sf;
	NeuralNetwork *network = new NeuralNetwork(af, ramp, sf);

	Neuron *n1 = new Neuron("Neuron1", ramp, saf);
	network->addNeuron(n1);
	Neuron *n2 = new Neuron("Neuron2", ramp, af);
	network->addNeuron(n2);
	Neuron *n3 = new Neuron("Neuron3", pasig, af);
	network->addNeuron(n3);

	Synapse *s1 = Synapse::createSynapse(n1, n2, -1, sf);
	Synapse *s2 = Synapse::createSynapse(n2, n2, 0.6, sf);
	Synapse *s3 = Synapse::createSynapse(n2, n3, 0.2, sf);

	QList<NeuralNetworkElement*> networkElements;
	network->getNetworkElements(networkElements);
	QVERIFY(!networkElements.isEmpty());
	
	qulonglong idn1 = n1->getId();
	qulonglong idn2 = n2->getId();
	qulonglong idn3 = n3->getId();
	qulonglong ids1 = s1->getId();
	qulonglong ids2 = s2->getId();
	qulonglong ids3 = s3->getId();

	// Bullshit should always return 0
	QString elem0Str = "lsfnvaleunvnu";
	DoubleValue *v0 = DynamicsPlotterUtil::getElementValue(elem0Str, networkElements);
	QVERIFY(v0 == 0);

	elem0Str = QString::number(idn1).append(":ucamli");
	v0 = DynamicsPlotterUtil::getElementValue(elem0Str, networkElements);
	QVERIFY(v0 == 0);

	elem0Str = QString::number(idn1).append(":tf:");
	v0 = DynamicsPlotterUtil::getElementValue(elem0Str, networkElements);
	QVERIFY(v0 == 0);

	elem0Str = QString::number(idn2).append(":a:");
	v0 = DynamicsPlotterUtil::getElementValue(elem0Str, networkElements);
	//QVERIFY(v0 == 0);

	elem0Str = QString::number(idn2).append("::");
	v0 = DynamicsPlotterUtil::getElementValue(elem0Str, networkElements);
	QVERIFY(v0 == 0);

	elem0Str = QString::number(idn2).append(":af:Wurst:Salat");
	v0 = DynamicsPlotterUtil::getElementValue(elem0Str, networkElements);
	QVERIFY(v0 == 0);


	// OutputActivationValue
	QString elem1Str = QString::number(idn1).append(":o");
	DoubleValue *v1 = DynamicsPlotterUtil::getElementValue(elem1Str, networkElements);
	QVERIFY(v1 != 0);
	QVERIFY(&(n1->getOutputActivationValue()) == v1);

	QString nelem1Str = elem1Str.append(":Lalala");
	DoubleValue *nv1 = DynamicsPlotterUtil::getElementValue(nelem1Str, networkElements);
	QVERIFY(nv1 == 0);

	// ActivationValue
	QList<Neuron*> activationNeuronList;
	QString elem2Str = QString::number(idn2).append(":a");
	DoubleValue *v2 = DynamicsPlotterUtil::getElementValue(elem2Str, networkElements, &activationNeuronList);
	QVERIFY(v2 != 0);
	QVERIFY(&(n2->getActivationValue()) == v2);
	
	// BiasValue
	QString elem3Str = QString::number(idn2).append(":b");
	DoubleValue *v3 = DynamicsPlotterUtil::getElementValue(elem3Str, networkElements);
	QVERIFY(v3 != 0);
	QVERIFY(&(n2->getBiasValue()) == v3);

	// TransferFunction: Parameter
	QString elem4Str = QString::number(idn3).append(":tf:Steepness");
	DoubleValue *v4 = DynamicsPlotterUtil::getElementValue(elem4Str, networkElements);
	QVERIFY(v4 != 0);
	QVERIFY(n3->getTransferFunction()->getParameter("Steepness") == v4);
	QVERIFY(v4->get() == 0.987);

	QString nelem4Str = QString::number(idn3).append(":tf:Deepness");
	DoubleValue *nv4 = DynamicsPlotterUtil::getElementValue(nelem4Str, networkElements);
	QVERIFY(nv4 == 0);

	// TransferFunction: Observable
	DoubleValue *testObservable = new DoubleValue(1.2345);
	n2->getTransferFunction()->addObserableOutput("TestObservable", testObservable);

	QString elem5Str = QString::number(idn2).append(":tf:TestObservable");
	DoubleValue *v5 = DynamicsPlotterUtil::getElementValue(elem5Str, networkElements);
	QVERIFY(v5 != 0);
	QVERIFY(testObservable == v5);
	QVERIFY(v5->get() == 1.2345);

	// ActivationFunction: Parameter
	QString elem6Str = QString::number(idn1).append(":af:MinActivation");
	DoubleValue *v6 = DynamicsPlotterUtil::getElementValue(elem6Str, networkElements);
	QVERIFY(v6 != 0);
	QVERIFY(n1->getActivationFunction()->getParameter("MinActivation") == v6);

	// ActivationFunction: Observable
	DoubleValue *testObservable2 = new DoubleValue(5.4321);
	n1->getActivationFunction()->addObserableOutput("TestObservable2", testObservable2);
	
	QString elem7Str = QString::number(idn1).append(":af:TestObservable2");
	DoubleValue *v7 = DynamicsPlotterUtil::getElementValue(elem7Str, networkElements);
	QVERIFY(v7 != 0);
	QVERIFY(testObservable2 == v7);
	QVERIFY(v7->get() == 5.4321);

	// SynapseStrength
	QString elem8Str = QString::number(ids2).append(":w");
	DoubleValue *v8 = DynamicsPlotterUtil::getElementValue(elem8Str, networkElements);
	QVERIFY(v8 != 0);
	QVERIFY(s2->getStrengthValue().get() == 0.6);

	// SynapseFunction: Parameter
	DoubleValue *testParameter = new DoubleValue(8.88);
	s1->getSynapseFunction()->addParameter("TestParameter", testParameter);

	QString elem9Str = QString::number(ids1).append(":sf:TestParameter");
	DoubleValue *v9 = DynamicsPlotterUtil::getElementValue(elem9Str, networkElements);
	QVERIFY(v9 != 0);
	QVERIFY(testParameter == v9);
	
	// SynapseFunction: Observable
	DoubleValue *testObservable3 = new DoubleValue(3.42);
	s3->getSynapseFunction()->addObserableOutput("TestObservable3", testObservable3);

	QString elem10Str = QString::number(ids3).append(":sf:TestObservable3");
	DoubleValue *v10 = DynamicsPlotterUtil::getElementValue(elem10Str, networkElements);
	QVERIFY(v10 != 0);
	QVERIFY(testObservable3 == v10);

}

//chris
void TestDynamicsPlotterUtil::testTransferActivationToOutput() {
	
	AdditiveTimeDiscreteActivationFunction af;
	TransferFunctionRamp ramp("ramp", -1, 1, false);
	SimpleSynapseFunction sf;

	NeuralNetwork *network = new NeuralNetwork(af, ramp, sf);
	Neuron *n1 = new Neuron("Neuron1", ramp, af);
	network->addNeuron(n1);
	Neuron *n2 = new Neuron("Neuron2", ramp, af);
	network->addNeuron(n2);
	Neuron *n3 = new Neuron("Neuron3", ramp, af);
	network->addNeuron(n3);
	
	Synapse *s1 = Synapse::createSynapse(n1, n2, 0.5, sf);
	Synapse *s2 = Synapse::createSynapse(n3, n3, -1.5, sf);
	
	QVERIFY(network->getSynapses().contains(s1));
	QVERIFY(network->getSynapses().contains(s2));
	QVERIFY(network->getSynapses().size() == 2);
	
	
	QCOMPARE(n1->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n1->getActivationValue().get(), 0.0);
	QCOMPARE(n2->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n2->getActivationValue().get(), 0.0);
	QCOMPARE(n3->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n3->getActivationValue().get(), 0.0);
	
	n1->getActivationValue().set(0.75);
	n2->getActivationValue().set(-1.2);
	n3->getActivationValue().set(2.8);
	
	network->executeStep();
	
	//did not have any effect (activation is overwritten with current output values)
	QCOMPARE(n1->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n1->getActivationValue().get(), 0.0);
	QCOMPARE(n2->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n2->getActivationValue().get(), 0.0);
	QCOMPARE(n3->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n3->getActivationValue().get(), 0.0);
	
	n1->getActivationValue().set(0.75);
	n2->getActivationValue().set(-1.2);
	n3->getActivationValue().set(2.8);
	
	DynamicsPlotterUtil::transferNeuronActivationToOutput(network);
	
	QCOMPARE(n1->getOutputActivationValue().get(), 0.75);
	QCOMPARE(n1->getActivationValue().get(), 0.75);
	QCOMPARE(n2->getOutputActivationValue().get(), -1.0);
	QCOMPARE(n2->getActivationValue().get(), -1.2);
	QCOMPARE(n3->getOutputActivationValue().get(), 1.0);
	QCOMPARE(n3->getActivationValue().get(), 2.8);
	
	network->executeStep();
	
	QCOMPARE(n1->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n1->getActivationValue().get(), 0.0);
	QCOMPARE(n2->getOutputActivationValue().get(), 0.375);
	QCOMPARE(n2->getActivationValue().get(), 0.375);
	QCOMPARE(n3->getOutputActivationValue().get(), -1.0);
	QCOMPARE(n3->getActivationValue().get(), -1.5);
	
	
	//do only partial transfer
	network->reset();
	
	n1->getActivationValue().set(0.75);
	n2->getActivationValue().set(-1.2);
	n3->getActivationValue().set(2.8);
	
	QList<Neuron*> neuronsToTransfer;
	neuronsToTransfer.append(n1);
	neuronsToTransfer.append(n2);
	
	DynamicsPlotterUtil::transferNeuronActivationToOutput(neuronsToTransfer);
	
	QCOMPARE(n1->getOutputActivationValue().get(), 0.75);
	QCOMPARE(n1->getActivationValue().get(), 0.75);
	QCOMPARE(n2->getOutputActivationValue().get(), -1.0);
	QCOMPARE(n2->getActivationValue().get(), -1.2);
	QCOMPARE(n3->getOutputActivationValue().get(), 0.0); //is not transferred
	QCOMPARE(n3->getActivationValue().get(), 2.8);
	
	network->executeStep();
	
	QCOMPARE(n1->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n1->getActivationValue().get(), 0.0);
	QCOMPARE(n2->getOutputActivationValue().get(), 0.375);
	QCOMPARE(n2->getActivationValue().get(), 0.375);
	QCOMPARE(n3->getOutputActivationValue().get(), 0.0); 
	QCOMPARE(n3->getActivationValue().get(), 0.0); //is overwritten by current activation value
	
}




