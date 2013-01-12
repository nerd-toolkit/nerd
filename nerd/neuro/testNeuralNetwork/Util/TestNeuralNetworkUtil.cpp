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




#include "TestNeuralNetworkUtil.h"
#include <iostream>
#include "Core/Core.h"
#include "Network/Neuron.h"
#include "Network/Synapse.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "Util/NeuralNetworkUtil.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "NeuralNetworkConstants.h"

using namespace std;
using namespace nerd;

void TestNeuralNetworkUtil::initTestCase() {
}

void TestNeuralNetworkUtil::cleanUpTestCase() {
}


//chris
void TestNeuralNetworkUtil::testLocationProperties() {
	Neuron *n1 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.0, 1.0), ActivationFunctionAdapter("AF1"));
	Synapse *s1 = new Synapse(n1, n1, 1.0, SynapseFunctionAdapter("SF1"));
	NeuroModule *m1 = new NeuroModule("Module1");
	NeuronGroup *g1 = new NeuronGroup("Group1");

	QPointF pos = NeuralNetworkUtil::getPosition(n1);
	QVERIFY(pos.x() == 0 && pos.y() == 0);
	
	NeuralNetworkUtil::setNetworkElementLocationProperty(n1, 0.1, 2.5, 3.9);
	QVERIFY(n1->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
	QVERIFY(n1->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION) == "0.1,2.5,3.9");
	pos = NeuralNetworkUtil::getPosition(n1);
	QVERIFY(pos.x() == 0.1 && pos.y() == 2.5);

	NeuralNetworkUtil::setNetworkElementLocationProperty(n1, 9999.99, 0.0001, 5000.9);
	QVERIFY(n1->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION) == "9999.99,0.0001,5000.9");
	pos = NeuralNetworkUtil::getPosition(n1);
	QVERIFY(pos.x() == 9999.99 && pos.y() == 0.0001);


	NeuralNetworkUtil::setNetworkElementLocationProperty(s1, -9999.99, -0.0001, -5000.9);
	QVERIFY(s1->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION) == "-9999.99,-0.0001,-5000.9");
	pos = NeuralNetworkUtil::getPosition(s1);
	QVERIFY(pos.x() == -9999.99 && pos.y() == -0.0001);


	NeuralNetworkUtil::setNetworkElementLocationProperty(g1, 0.99, 0.0001, 5000.9);
	QVERIFY(g1->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION) == "0.99,0.0001,5000.9");
	pos = NeuralNetworkUtil::getPosition(g1);
	QVERIFY(pos.x() == 0.99 && pos.y() == 0.0001);


	NeuralNetworkUtil::setNetworkElementLocationProperty(m1, 123.99, 0.0001, 5000.9);
	QVERIFY(m1->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION) == "123.99,0.0001,5000.9");
	pos = NeuralNetworkUtil::getPosition(m1);
	QVERIFY(pos.x() == 123.99 && pos.y() == 0.0001);
	NeuralNetworkUtil::setNeuroModuleLocationSizeProperty(m1, 444.5, -50.777);
	QVERIFY(m1->getProperty(NeuralNetworkConstants::TAG_MODULE_SIZE) == "444.5,-50.777");
	QSizeF size = NeuralNetworkUtil::getModuleSize(m1);
	QVERIFY(size.width() == 444.5 && size.height() == -50.777);


	//check wrong arguments.
	m1->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, "HalloWelt");
	pos = NeuralNetworkUtil::getPosition(m1);
	QVERIFY(pos.x() == 0.0 && pos.y() == 0.0);
	
	m1->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, "123");
	pos = NeuralNetworkUtil::getPosition(m1);
	QVERIFY(pos.x() == 0.0 && pos.y() == 0.0);

	m1->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, "123,456");
	pos = NeuralNetworkUtil::getPosition(m1);
	QVERIFY(pos.x() == 0.0 && pos.y() == 0.0);

	m1->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, "123,456,456,789");
	pos = NeuralNetworkUtil::getPosition(m1);
	QVERIFY(pos.x() == 0.0 && pos.y() == 0.0);

	m1->setProperty(NeuralNetworkConstants::TAG_MODULE_SIZE, "Hallo Welt");
	size = NeuralNetworkUtil::getModuleSize(m1);
	QVERIFY(size.width() == 0.0 && size.height() == 0.0);

	m1->setProperty(NeuralNetworkConstants::TAG_MODULE_SIZE, "123");
	size = NeuralNetworkUtil::getModuleSize(m1);
	QVERIFY(size.width() == 0.0 && size.height() == 0.0);
	
	m1->setProperty(NeuralNetworkConstants::TAG_MODULE_SIZE, "123,456,789");
	size = NeuralNetworkUtil::getModuleSize(m1);
	QVERIFY(size.width() == 0.0 && size.height() == 0.0);


	delete n1;
	delete s1;
	delete g1;
	delete m1;
}

//chris
void TestNeuralNetworkUtil::testGetConnectedNeuronsAndDependentSynapses() {

	Neuron *n1 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.0, 1.0), ActivationFunctionAdapter("AF1"));
	Neuron *n2 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.0, 1.0), ActivationFunctionAdapter("AF1"));
	Neuron *n3 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.0, 1.0), ActivationFunctionAdapter("AF1"));
	Neuron *n4 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.0, 1.0), ActivationFunctionAdapter("AF1"));
	Neuron *n5 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.0, 1.0), ActivationFunctionAdapter("AF1"));

	Synapse *s1 = Synapse::createSynapse(n1, n4, 1.0, SynapseFunctionAdapter("SF1"));

	{
		QList<Neuron*> neurons = NeuralNetworkUtil::getConnectedNeurons(s1);
		QCOMPARE(neurons.size(), 2);
		QVERIFY(neurons.contains(n1));
		QVERIFY(neurons.contains(n4));
	}

	Synapse *s2 = Synapse::createSynapse(n1, n5, 1.0, SynapseFunctionAdapter("SF1"));

	{
		QList<Neuron*> neurons = NeuralNetworkUtil::getConnectedNeurons(s2);
		QCOMPARE(neurons.size(), 2);
		QVERIFY(neurons.contains(n1));
		QVERIFY(neurons.contains(n5));
	}

	Synapse *s3 = Synapse::createSynapse(n2, s1, 1.0, SynapseFunctionAdapter("SF1"));

	{
		QList<Neuron*> neurons = NeuralNetworkUtil::getConnectedNeurons(s1);
		QCOMPARE(neurons.size(), 2);
		QVERIFY(neurons.contains(n1));
		QVERIFY(neurons.contains(n4));
	}
	{
		QList<Neuron*> neurons = NeuralNetworkUtil::getConnectedNeurons(s3);
		QCOMPARE(neurons.size(), 3);
		QVERIFY(neurons.contains(n1));
		QVERIFY(neurons.contains(n4));
		QVERIFY(neurons.contains(n2));
	}
	
	Synapse *s4 = Synapse::createSynapse(n1, s3, 1.0, SynapseFunctionAdapter("SF1"));

	{
		QList<Neuron*> neurons = NeuralNetworkUtil::getConnectedNeurons(s4);
		QCOMPARE(neurons.size(), 3);
		QVERIFY(neurons.contains(n1));
		QVERIFY(neurons.contains(n4));
		QVERIFY(neurons.contains(n2));
	}

	Synapse *s5 = Synapse::createSynapse(n5, s4, 1.0, SynapseFunctionAdapter("SF1"));

	{
		QList<Neuron*> neurons = NeuralNetworkUtil::getConnectedNeurons(s5);
		QCOMPARE(neurons.size(), 4);
		QVERIFY(neurons.contains(n1));
		QVERIFY(neurons.contains(n4));
		QVERIFY(neurons.contains(n2));
		QVERIFY(neurons.contains(n5));
	}
	{
		QList<Neuron*> neurons = NeuralNetworkUtil::getConnectedNeurons(s3);
		QCOMPARE(neurons.size(), 3);
		QVERIFY(neurons.contains(n1));
		QVERIFY(neurons.contains(n4));
		QVERIFY(neurons.contains(n2));
	}

	Synapse *s6 = Synapse::createSynapse(n3, s3, 1.0, SynapseFunctionAdapter("SF1"));

	{
		QList<Neuron*> neurons = NeuralNetworkUtil::getConnectedNeurons(s6);
		QCOMPARE(neurons.size(), 4);
		QVERIFY(neurons.contains(n1));
		QVERIFY(neurons.contains(n4));
		QVERIFY(neurons.contains(n2));
		QVERIFY(neurons.contains(n3));
	}

	//test dependent synapses
	{
		QList<Synapse*> synapses = NeuralNetworkUtil::getDependentSynapses(s5);
		QCOMPARE(synapses.size(), 0);
	}
	{
		QList<Synapse*> synapses = NeuralNetworkUtil::getDependentSynapses(s4);
		QCOMPARE(synapses.size(), 1);
		QVERIFY(synapses.contains(s5));
	}
	{
		QList<Synapse*> synapses = NeuralNetworkUtil::getDependentSynapses(s3);
		QCOMPARE(synapses.size(), 3);
		QVERIFY(synapses.contains(s5));
		QVERIFY(synapses.contains(s4));
		QVERIFY(synapses.contains(s6));
	}
	{
		QList<Synapse*> synapses = NeuralNetworkUtil::getDependentSynapses(s2);
		QCOMPARE(synapses.size(), 0);
	}
	{
		QList<Synapse*> synapses = NeuralNetworkUtil::getDependentSynapses(s1);
		QCOMPARE(synapses.size(), 4);
		QVERIFY(synapses.contains(s5));
		QVERIFY(synapses.contains(s4));
		QVERIFY(synapses.contains(s6));
		QVERIFY(synapses.contains(s3));
	}


	delete n1;
	delete n2;
	delete n3;
	delete n4;
	delete n5;

}



