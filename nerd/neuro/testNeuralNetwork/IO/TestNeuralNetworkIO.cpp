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

#include "TestNeuralNetworkIO.h"
#include "IO/NeuralNetworkIO.h"
#include "IO/NeuralNetworkIOBytecode.h"
#include "Network/Neuro.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "IO/NeuralNetworkIOSimbaV3Xml.h"
#include "Collections/StandardNeuralNetworkFunctions.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include "Core/Core.h"
#include "Value/Value.h"
#include <QDebug>
#include <QList>
#include <iostream>
#include "Collections/StandardNeuralNetworkFunctions.h"

using namespace std;

namespace nerd{


//Ferry
void TestNeuralNetworkIO::testCreateSimbaV3XmlNetworkFromFile() {
	Core::resetCore();
	Neuro::install();

	Core::getInstance()->init();
	NeuralNetworkManager* neuralNetworkManager = Neuro::getNeuralNetworkManager();

	NeuralNetwork *net;
	QString errorMsg;
	QList<QString> warnings;

	// No <net>-element!
	net = NeuralNetworkIO::createNetworkFromFile("IO/simbaV3test1.xml", 
		  NeuralNetworkIO::SimbaV3Xml, &errorMsg, &warnings);
	QVERIFY(net == NULL);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	errorMsg = "";

	// Version Mismatch!
	net = NeuralNetworkIO::createNetworkFromFile("IO/simbaV3test2.xml", 
		  NeuralNetworkIO::SimbaV3Xml, &errorMsg, &warnings);
	QVERIFY(net == NULL);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	errorMsg = "";

	// No defaultFunctions element!
	net = NeuralNetworkIO::createNetworkFromFile("IO/simbaV3test3.xml", 
		  NeuralNetworkIO::SimbaV3Xml, &errorMsg, &warnings);
	QVERIFY(net != NULL);
	QVERIFY(errorMsg.isEmpty());
	QCOMPARE(warnings.size(), 3);
	while(!warnings.isEmpty()) {
		warnings.removeFirst();
	}
	delete net;
	net = NULL;

	// No registered defaultFunctions!
	net = NeuralNetworkIO::createNetworkFromFile("IO/simbaV3test4.xml", 
		  NeuralNetworkIO::SimbaV3Xml, &errorMsg, &warnings);
	QVERIFY(net != NULL);
	QVERIFY(errorMsg.isEmpty());
	QCOMPARE(warnings.size(), 3);
	while(!warnings.isEmpty()) {
		warnings.removeFirst();
	}
	delete net;
	net = NULL;

	// With registered defaultFunctions!
	TransferFunctionAdapter *tf = new TransferFunctionAdapter("tanh", 0, 1);
	ActivationFunctionAdapter *af = new ActivationFunctionAdapter("AdditiveTD");
	SynapseFunctionAdapter *sf = new SynapseFunctionAdapter("SimpleUpdateFunction");
	Value *value = new Value();
	sf->addParameter("TestPara", value);
	neuralNetworkManager->addTransferFunctionPrototype(*tf);
	neuralNetworkManager->addActivationFunctionPrototype(*af);
	neuralNetworkManager->addSynapseFunctionPrototype(*sf);
	delete tf;
	delete af;
	delete sf;

	net = NeuralNetworkIO::createNetworkFromFile("IO/simbaV3test4.xml", 
		  NeuralNetworkIO::SimbaV3Xml, &errorMsg, &warnings);
	QVERIFY(net != NULL);
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	delete net;
	net = NULL;

	// With 3 Neurons and 3 Synapses
	net = NeuralNetworkIO::createNetworkFromFile("IO/simbaV3test5.xml", 
		  NeuralNetworkIO::SimbaV3Xml, &errorMsg, &warnings);
	QVERIFY(net != NULL);
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	QList<Neuron*> neurons = net->getNeurons();
	QCOMPARE(neurons.size(), 3);



	//begin chris
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		if(neuron->getNameValue().get() == "/SimObjects/Robot/VelocityX") {
			QCOMPARE(neuron->getSynapses().size(), 2);
		}	
		else if(neuron->getNameValue().get() == "DistanceScannerRight") {
			QCOMPARE(neuron->getSynapses().size(), 0);
		}
		else if(neuron->getNameValue().get() == "DistanceScannerLeft") {
			QCOMPARE(neuron->getSynapses().size(), 0);
		}
		else {
			QVERIFY(false);
		}
	}
	//end chris

	QList<Synapse*> synapses = net->getSynapses();
	QCOMPARE(synapses.size(), 3);


	//begin chris
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();
		if(synapse->getSource()->getNameValue().get() == "DistanceScannerLeft") {
			QCOMPARE(synapse->getSynapses().size(), 0);
		}	
		else if(synapse->getSource()->getNameValue().get() == "DistanceScannerRight") {
			QCOMPARE(synapse->getSynapses().size(), 1);
		}
		else {
			QVERIFY(false);
		}
	}
	//end chris


	delete net;
	net = NULL;

	Core::resetCore();

}


//Ferry
void TestNeuralNetworkIO::testCreateSimbaV3XmlFileFromNetwork() {
	Core::resetCore();
	Neuro::install();

// 	Core::getInstance()->init();
	NeuralNetworkManager* neuralNetworkManager = Neuro::getNeuralNetworkManager();

	// Save a little Network
	TransferFunctionAdapter *tf = new TransferFunctionAdapter("tanh", 0, 1);
	ActivationFunctionAdapter *af = new ActivationFunctionAdapter("AdditiveTD");
	SynapseFunctionAdapter *sf = new SynapseFunctionAdapter("SimpleUpdateFunction");
	DoubleValue *value = new DoubleValue();
	sf->addParameter("TestPara", value);
	neuralNetworkManager->addTransferFunctionPrototype(*tf);
	neuralNetworkManager->addActivationFunctionPrototype(*af);
	neuralNetworkManager->addSynapseFunctionPrototype(*sf);


	NeuralNetwork *net = new NeuralNetwork(*af, *tf, *sf);
	delete tf;
	delete af;
	delete sf;

	QString errorMsg;
	QList<QString> warnings;
	bool ret;

	QFile file("IO/simbaV3write_test.smb");
	if(file.exists()) {
		QVERIFY(file.remove());
	}

	// read in the same Network and compare to the one stored
	ret = NeuralNetworkIO::createFileFromNetwork("IO/simbaV3write_test.smb", net,
		  NeuralNetworkIO::SimbaV3Xml, &errorMsg);
	QVERIFY(errorMsg == "");
	QVERIFY(ret);
	QVERIFY(file.exists());

	NeuralNetwork *net_in = NeuralNetworkIO::createNetworkFromFile("IO/simbaV3write_test.smb",
							NeuralNetworkIO::SimbaV3Xml, &errorMsg, &warnings);
	QVERIFY(net_in != 0);
	QVERIFY(errorMsg.isEmpty());

if(!warnings.isEmpty()) {
	for(QListIterator<QString> i(warnings); i.hasNext();) {
		qDebug() << i.next();
	}
}

	QVERIFY(warnings.isEmpty());
	QCOMPARE(net_in->getDefaultTransferFunction()->getName(), QString("tanh"));
	QCOMPARE(net_in->getDefaultActivationFunction()->getName(), QString("AdditiveTD"));
	QCOMPARE(net_in->getDefaultSynapseFunction()->getName(), QString("SimpleUpdateFunction"));

	delete net_in;
	delete net;
	net_in = 0;
	net = 0;

	QVERIFY(file.remove());	

	// read in complex network, save it, load it again and compare with the one saved
	net_in = NeuralNetworkIO::createNetworkFromFile("IO/simbaV3test5.xml",
			 NeuralNetworkIO::SimbaV3Xml, &errorMsg, &warnings);
	QVERIFY(net_in != 0);
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());

	ret = NeuralNetworkIO::createFileFromNetwork("IO/simbaV3write_test.smb", net_in,
		  NeuralNetworkIO::SimbaV3Xml, &errorMsg);
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(ret);
	QVERIFY(file.exists());

	net = NeuralNetworkIO::createNetworkFromFile("IO/simbaV3write_test.smb",
		  NeuralNetworkIO::SimbaV3Xml, &errorMsg, &warnings);
	QVERIFY(net != 0);
	QVERIFY(errorMsg.isEmpty());

//if(!warnings.isEmpty()) {
//	for(QListIterator<QString> i(warnings); i.hasNext();) {
//		qDebug() << i.next();
//	}
//}

	QVERIFY(warnings.isEmpty());

	// compare the two nets
	QCOMPARE(net->getDefaultTransferFunction()->getName(),
			 net_in->getDefaultTransferFunction()->getName());
	QCOMPARE(net->getDefaultActivationFunction()->getName(),
			 net_in->getDefaultActivationFunction()->getName());
	QCOMPARE(net->getDefaultSynapseFunction()->getName(),
			 net_in->getDefaultSynapseFunction()->getName());
	QCOMPARE(net->getNeurons().size(), net_in->getNeurons().size());
	QCOMPARE(net->getSynapses().size(), net_in->getSynapses().size());
	QVERIFY(net->getDefaultSynapseFunction()->getParameter("TestPara") != NULL);
	QVERIFY(dynamic_cast<DoubleValue*>(net->getDefaultSynapseFunction()
				->getParameter("TestPara")));
	QCOMPARE((dynamic_cast<DoubleValue*>(net->getDefaultSynapseFunction()
				->getParameter("TestPara")))->get(), 123.4);
	QCOMPARE(net->getSynapses().size(), 3);
	QCOMPARE(NeuralNetworkIO::getNeuronNumber(net, net->getSynapses().at(1)->getSource()), 1);
	QCOMPARE(NeuralNetworkIO::getNeuronNumber(net_in, net_in->getSynapses().at(1)->getSource()), 1);

	delete net_in;
	delete net;

	Core::resetCore();

}


//Ferry
void TestNeuralNetworkIO::testCreateEvosunXmlNetworkFromFile() {
	Core::resetCore();
	Neuro::install();

	StandardNeuralNetworkFunctions();

	Core::getInstance()->init();
	Neuro::getNeuralNetworkManager();

	NeuralNetwork *net;
	QString errorMsg;
	QList<QString> warnings;

	// Invalid file!
	net = NeuralNetworkIO::createNetworkFromFile("IO/abrakadabra.xml", 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(net == NULL);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	errorMsg = "";

	// No <Net>-element!
	net = NeuralNetworkIO::createNetworkFromFile("IO/evosunTest1.xml", 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(net == NULL);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	errorMsg = "";

	// XML-error
	net = NeuralNetworkIO::createNetworkFromFile("IO/evosunTest2.xml", 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(net == NULL);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	errorMsg = "";

	// No Transferfunction-Attribute
	net = NeuralNetworkIO::createNetworkFromFile("IO/evosunTest3.xml", 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(net != NULL);
	QVERIFY(errorMsg.isEmpty());
	QCOMPARE(warnings.size(), 1);
	warnings.removeFirst();
	delete net;
	net = NULL;

	// No neurons, Transferfunction tanh
	net = NeuralNetworkIO::createNetworkFromFile("IO/evosunTest4.xml", 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(net != NULL);
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	QVERIFY(net->getDefaultTransferFunction()->getName() == "tanh");
	QCOMPARE(net->getNeurons().size(), 0);
	delete net;
	net = NULL;

	// One neuron, but no layer attribute, Transferfunction tanh
	net = NeuralNetworkIO::createNetworkFromFile("IO/evosunTest5.xml", 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(net != NULL);
	QVERIFY(errorMsg.isEmpty());
	QCOMPARE(warnings.size(), 1);
	warnings.removeFirst();
	QVERIFY(net->getDefaultTransferFunction()->getName() == "tanh");
	QCOMPARE(net->getNeurons().size(), 0);
	delete net;
	net = NULL;

	// Two neurons, Transferfunction tanh
	// Three synapses, one without strength, the other with wrong sourceID, third one ok
	net = NeuralNetworkIO::createNetworkFromFile("IO/evosunTest6.xml", 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(net != NULL);
	QVERIFY(errorMsg.isEmpty());
	QCOMPARE(warnings.size(), 2);
	while(!warnings.isEmpty()) {
		warnings.removeFirst();
	}
	QVERIFY(net->getDefaultTransferFunction()->getName() == "tanh");
	QCOMPARE(net->getNeurons().size(), 2);
	QVERIFY(net->getNeurons().at(0)->getNameValue().get() == "InputNeuron1");
	QCOMPARE(net->getNeurons().at(0)->getBiasValue().get(), 2.0);
	QVERIFY(net->getNeurons().at(0)->getTransferFunction()->getName() == "tanh");
	QCOMPARE(net->getSynapses().size(), 1);
	QVERIFY(net->getSynapses().at(0)->getSource()->getNameValue().get() == "InputNeuron1");
	QCOMPARE(net->getSynapses().at(0)->getStrengthValue().get(), 20.0);
	QVERIFY(net->getSynapses().at(0)->getSynapseFunction()->getName() 
				== net->getDefaultSynapseFunction()->getName());
	delete net;
	net = NULL;

	// Same with Wrapper
	net = NeuralNetworkIO::createNetworkFromFile("IO/evosunTest7.xml", 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(net != NULL);
	QVERIFY(errorMsg.isEmpty());
	QCOMPARE(warnings.size(), 2);
	while(!warnings.isEmpty()) {
		warnings.removeFirst();
	}
	QVERIFY(net->getDefaultTransferFunction()->getName() == "tanh");
	QCOMPARE(net->getNeurons().size(), 2);
	QVERIFY(net->getNeurons().at(0)->getNameValue().get() == "InputNeuron1");
	QCOMPARE(net->getNeurons().at(0)->getBiasValue().get(), 2.0);
	QVERIFY(net->getNeurons().at(0)->getTransferFunction()->getName() == "tanh");
	QCOMPARE(net->getSynapses().size(), 1);
	QVERIFY(net->getSynapses().at(0)->getSource()->getNameValue().get() == "InputNeuron1");
	QCOMPARE(net->getSynapses().at(0)->getStrengthValue().get(), 20.0);
	QVERIFY(net->getSynapses().at(0)->getSynapseFunction()->getName() 
				== net->getDefaultSynapseFunction()->getName());
	delete net;
	net = NULL;

	Core::resetCore();
}


//Ferry
void TestNeuralNetworkIO::testCreateEvosunXmlFileFromNetwork() {
	Core::resetCore();
	Neuro::install();

	Core::getInstance()->init();

	QString errorMsg;
	QList<QString> warnings;
	bool ret;

	QFile file("IO/evosunWrite_test.smb");
	if(file.exists()) {
		QVERIFY(file.remove());
	}

	// save network with unknown transferfunction
	TransferFunctionAdapter *tf = new TransferFunctionAdapter("tansanban", 0, 1);
	NeuralNetwork *net = new NeuralNetwork();
	net->setDefaultTransferFunction(*tf);
	delete tf;

	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(), net, 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(ret);
	QVERIFY(errorMsg.isEmpty());
	QCOMPARE(warnings.size(), 1);
	warnings.removeFirst();
	QVERIFY(file.exists());
	QVERIFY(file.remove());

	// same with known transferfunction
	tf = new TransferFunctionAdapter("tanh", 0, 1);
	net->setDefaultTransferFunction(*tf);
	delete tf;

	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(), net, 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(ret);
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	QVERIFY(file.exists());
	QVERIFY(file.remove());

	// with 2 neurons
	Neuron *n1 = new Neuron("Neuron1", *(net->getDefaultTransferFunction()),
							*(net->getDefaultActivationFunction()));
	Neuron *n2 = new Neuron("Neuron2", *(net->getDefaultTransferFunction()),
							*(net->getDefaultActivationFunction()));
	net->addNeuron(n1);
	net->addNeuron(n2);
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(), net, 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(ret);
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	QVERIFY(file.exists());
	QVERIFY(file.remove());

	// one neuron with unkown Transferfunction
	TransferFunctionAdapter *tf2 = new TransferFunctionAdapter("abrakadabra", 0.0, 1.0);
	n2->setTransferFunction(*tf2);
	delete tf2;
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(), net, 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(ret);
	QVERIFY(errorMsg.isEmpty());
	QCOMPARE(warnings.size(), 1);
	warnings.removeFirst();
	QVERIFY(file.exists());
	QVERIFY(file.remove());

	// one synapse with child synapse
	Synapse *s1 = new Synapse(n1, NULL, 2.0, *(net->getDefaultSynapseFunction()));
	n2->addSynapse(s1);
	Synapse *s2 = new Synapse(n1, NULL, 1.0, *(net->getDefaultSynapseFunction()));
	s1->addSynapse(s2);
	n2->setTransferFunction(*(net->getDefaultTransferFunction()));
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(), net, 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(ret);
	QVERIFY(errorMsg.isEmpty());
	QCOMPARE(warnings.size(), 1);
	warnings.removeFirst();
	QVERIFY(file.exists());
	QVERIFY(file.remove());

	// load complex network, save it, load it again and compare with saved one
	NeuralNetwork *net2 = NeuralNetworkIO::createNetworkFromFile("IO/evosunTest7.xml",
 								NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(net2 != 0);
	QVERIFY(errorMsg.isEmpty());
	QCOMPARE(warnings.size(), 2);
	while(!warnings.isEmpty()) {
		warnings.removeFirst();
	}

	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(), net2, 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	delete net2;
	net2 = 0;
	QVERIFY(ret);
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	QVERIFY(file.exists());

	net2 = NeuralNetworkIO::createNetworkFromFile(file.fileName(), 
				NeuralNetworkIO::EvosunXml, &errorMsg, &warnings);
	QVERIFY(net2 != NULL);
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(warnings.isEmpty());
	QVERIFY(net2->getDefaultTransferFunction()->getName() == "tanh");
	QCOMPARE(net2->getNeurons().size(), 2);
	QVERIFY(net2->getNeurons().at(0)->getNameValue().get() == "InputNeuron1");
	QCOMPARE(net2->getNeurons().at(0)->getBiasValue().get(), 2.0);
	QVERIFY(net2->getNeurons().at(0)->getTransferFunction()->getName() == "tanh");
	QCOMPARE(net2->getSynapses().size(), 1);
	QVERIFY(net2->getSynapses().at(0)->getSource()->getNameValue().get() == "InputNeuron1");
	QCOMPARE(net2->getSynapses().at(0)->getStrengthValue().get(), 20.0);
	QVERIFY(net2->getSynapses().at(0)->getSynapseFunction()->getName() 
				== net2->getDefaultSynapseFunction()->getName());
	

	delete net2;
	delete net;
	QVERIFY(file.remove());	

	Core::resetCore();
}

void TestNeuralNetworkIO::testCreateBytecodeFileFromNetwork() {
	Core::resetCore();
	Neuro::install();

// 	Core::getInstance()->init();
// 
	QString errorMsg;
	QList<QString> warnings;
	bool ret;

	QFile file("IO/bytecodeWrite_test.hyb");
	if(file.exists()) {
		QVERIFY(file.remove());
	}

	// Try to save non modular neural network. Should return false, and an error message.
	NeuralNetwork *net = new NeuralNetwork;

	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			net, NeuralNetworkIO::Bytecode, &errorMsg);

	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == "Network has to be modular!");
	errorMsg.clear();

	delete net;

	// Try to safe a modular network with two modules with the same Accelboard name.
	ModularNeuralNetwork *modNet = new ModularNeuralNetwork;
	NeuroModule *module = new NeuroModule("ABSR");
	module->setProperty("Accelboard", "ABSR");
	modNet->addNeuronGroup(module);
	module = new NeuroModule("ABSR2");
	module->setProperty("Accelboard", "ABSR");
	modNet->addNeuronGroup(module);

	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == "There are at least two modules with property Accelboard = ABSR!");
	errorMsg.clear();

	delete modNet;

	// Try to safe a modular network with unvalid Accelboard name
	modNet = new ModularNeuralNetwork;
	module = new NeuroModule("AAAA");
	module->setProperty("Accelboard", "AAAA");
	modNet->addNeuronGroup(module);
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == "There is a module with property Accelboard = AAAA. This is not a valid accelboard name!");
	errorMsg.clear();

	delete modNet;

	// Try to safe a modular network with no accelboard modules.
	modNet = new ModularNeuralNetwork;
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == "Module with property Accelboard = ABML not found!");
	errorMsg.clear();

	delete modNet;

	// Try to safe a network with a neuron which doesn't belong to any accelboard module.
	modNet = new ModularNeuralNetwork;
	module = new NeuroModule("ABML");
	module->setProperty("Accelboard", "ABML");
	modNet->addNeuronGroup(module);
	module = new NeuroModule("ABSR");
	module->setProperty("Accelboard", "ABSR");
	modNet->addNeuronGroup(module);
	module = new NeuroModule("ABAL");
	module->setProperty("Accelboard", "ABAL");
	modNet->addNeuronGroup(module);
	module = new NeuroModule("ABAR");
	module->setProperty("Accelboard", "ABAR");
	modNet->addNeuronGroup(module);
	module = new NeuroModule("ABHL");
	module->setProperty("Accelboard", "ABHL");
	modNet->addNeuronGroup(module);
	module = new NeuroModule("ABHR");
	module->setProperty("Accelboard", "ABHR");
	modNet->addNeuronGroup(module);
	module = new NeuroModule("ABFL");
	module->setProperty("Accelboard", "ABFL");
	modNet->addNeuronGroup(module);
	module = new NeuroModule("ABFR");
	module->setProperty("Accelboard", "ABFR");
	modNet->addNeuronGroup(module);

	Neuron *neuron = new Neuron("Neuron", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	modNet->addNeuron(neuron);

	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == "There is at least one neuron which doesn't belong to an accelboard module!");
	errorMsg.clear();
	modNet->removeNeuron(neuron);
	delete neuron;

	// Try to safe a network with an input neuron in a accelboard module without the SpinalChordAddressProperty
	NeuronGroup *group = modNet->getNeuronGroup("ABAL");
	QVERIFY(group != 0);
	Neuron *inputNeuron = new Neuron("ABAL_IN_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	group->addNeuron(inputNeuron);
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("There is an input neuron in accelboard module ABAL which has no property %1!").arg(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY));
	errorMsg.clear();
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "42");
	
	// Try to safe a network with an output neuron in a accelboard module without the SpinalChordAddressProperty
	Neuron *outputNeuron = new Neuron("ABAL_OUT_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	Neuron *abalOut1 = outputNeuron;
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	group->addNeuron(outputNeuron);
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("There is an output neuron in accelboard module ABAL which has no property %1!").arg(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY));
	errorMsg.clear();
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "32");

	// Try to safe network with invalid SpinalChordAddress
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "34");
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("Input neuron in accelboard module ABAL has invalid SpinalCordAddress or SpinalCordAddress is assigned twice: %1 = 34!").arg(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY));
	errorMsg.clear();
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "35");

	// Try to safe network with SpinalChordAddress assigned twice
	inputNeuron = new Neuron("ABAL_IN_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	group->addNeuron(inputNeuron);
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "35");
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("Input neuron in accelboard module ABAL has invalid SpinalCordAddress or SpinalCordAddress is assigned twice: %1 = 35!").arg(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY));
	errorMsg.clear();
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "36");

	// Try to safe network with input neuron missing
	inputNeuron = new Neuron("ABAL_IN_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	group->addNeuron(inputNeuron);
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "42");
	inputNeuron = new Neuron("ABAL_IN_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	group->addNeuron(inputNeuron);
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "43");

	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("At least one input neuron in accelboard module ABAL missing. SpinalChordAddress 44 not assigned!"));
	errorMsg.clear();
	
	// Try to safe network with invalid SpinalChordAddress in Output neuron
	inputNeuron = new Neuron("ABAL_IN_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	group->addNeuron(inputNeuron);
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "44");

	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "31");
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("Output neuron in accelboard module ABAL has invalid SpinalCordAddress or SpinalCordAddress is assigned twice: %1 = 31!").arg(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY));
	errorMsg.clear();
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "32");

	// Try to safe network with SpinalChordAddress assigned twice in output neuron
	outputNeuron = new Neuron("ABAL_OUT_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "32");
	group->addNeuron(outputNeuron);
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("Output neuron in accelboard module ABAL has invalid SpinalCordAddress or SpinalCordAddress is assigned twice: %1 = 32!").arg(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY));
	errorMsg.clear();
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "33");

	// Try to safe network with output neuron missing
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("At least one output neuron in accelboard module ABAL missing. SpinalChordAddress 34 not assigned!"));
	errorMsg.clear();

	// Add all missing modules and corresponding input and output neurons
	outputNeuron = new Neuron("ABAL_OUT_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "34");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABAL_OUT_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "45");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABAL_OUT_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "46");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABAL_OUT_6", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "47");
	group->addNeuron(outputNeuron);

	group = modNet->getNeuronGroup("ABML");
	inputNeuron = new Neuron("ABML_IN_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "3");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABML_IN_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "4");
	group->addNeuron(inputNeuron);

	group = modNet->getNeuronGroup("ABSR");
	inputNeuron = new Neuron("ABSR_IN_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "19");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABSR_IN_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "20");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABSR_IN_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "26");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABSR_IN_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "27");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABSR_IN_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "28");
	group->addNeuron(inputNeuron);
	Neuron *absrOut1 = new Neuron("ABSR_OUT_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	absrOut1->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	absrOut1->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "16");
	group->addNeuron(absrOut1);
	outputNeuron = new Neuron("ABSR_OUT_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "17");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABSR_OUT_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "18");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABSR_OUT_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "29");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABSR_OUT_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "30");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABSR_OUT_6", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "31");
	group->addNeuron(outputNeuron);

	group = modNet->getNeuronGroup("ABAR");
	inputNeuron = new Neuron("ABAR_IN_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "51");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABAR_IN_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "52");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABAR_IN_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "58");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABAR_IN_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "59");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABAR_IN_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "60");
	group->addNeuron(inputNeuron);
	outputNeuron = new Neuron("ABAR_OUT_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "48");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABAR_OUT_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "49");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABAR_OUT_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "50");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABAR_OUT_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "61");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABAR_OUT_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "62");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABAR_OUT_6", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "63");
	group->addNeuron(outputNeuron);

	group = modNet->getNeuronGroup("ABHL");
	inputNeuron = new Neuron("ABHL_IN_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "67");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABHL_IN_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "68");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABHL_IN_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "74");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABHL_IN_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "75");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABHL_IN_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "76");
	group->addNeuron(inputNeuron);
	outputNeuron = new Neuron("ABHL_OUT_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "64");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABHL_OUT_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "65");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABHL_OUT_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "66");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABHL_OUT_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "77");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABHL_OUT_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "78");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABHL_OUT_6", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "79");
	group->addNeuron(outputNeuron);

	group = modNet->getNeuronGroup("ABHR");
	inputNeuron = new Neuron("ABHR_IN_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "83");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABHR_IN_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "84");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABHR_IN_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "90");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABHR_IN_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "91");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABHR_IN_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "92");
	group->addNeuron(inputNeuron);
	outputNeuron = new Neuron("ABHR_OUT_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "80");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABHR_OUT_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "81");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABHR_OUT_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "82");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABHR_OUT_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "93");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABHR_OUT_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "94");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABHR_OUT_6", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "95");
	group->addNeuron(outputNeuron);

	group = modNet->getNeuronGroup("ABFL");
	inputNeuron = new Neuron("ABFL_IN_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "99");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABFL_IN_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "100");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABFL_IN_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "106");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABFL_IN_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "107");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABFL_IN_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "108");
	group->addNeuron(inputNeuron);
	outputNeuron = new Neuron("ABFL_OUT_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "96");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABFL_OUT_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "97");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABFL_OUT_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "98");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABFL_OUT_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "109");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABFL_OUT_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "110");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABFL_OUT_6", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "111");
	group->addNeuron(outputNeuron);

	group = modNet->getNeuronGroup("ABFR");
	Neuron *abfrIn1 = new Neuron("ABFR_IN_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	abfrIn1->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	abfrIn1->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "115");
	group->addNeuron(abfrIn1);
	inputNeuron = new Neuron("ABFR_IN_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "116");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABFR_IN_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "122");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABFR_IN_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "123");
	group->addNeuron(inputNeuron);
	inputNeuron = new Neuron("ABFR_IN_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	inputNeuron->setProperty(Neuron::NEURON_TYPE_INPUT, "");
	inputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "124");
	group->addNeuron(inputNeuron);
	outputNeuron = new Neuron("ABFR_OUT_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "112");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABFR_OUT_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "113");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABFR_OUT_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "114");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABFR_OUT_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "125");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABFR_OUT_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "126");
	group->addNeuron(outputNeuron);
	outputNeuron = new Neuron("ABFR_OUT_6", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	outputNeuron->setProperty(Neuron::NEURON_TYPE_OUTPUT, "");
	outputNeuron->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "127");
	group->addNeuron(outputNeuron);

	// Try to safe a net with a synapse with a synapse as target
	Synapse *synapse1 = Synapse::createSynapse(inputNeuron, outputNeuron, 0.1, *(modNet->getDefaultSynapseFunction()));
	Synapse *synapse2 = Synapse::createSynapse(inputNeuron, synapse1, 0.1, *(modNet->getDefaultSynapseFunction()));
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("There is an enabled synapse with a synapse as target in the net!"));
	errorMsg.clear();
	synapse1->removeSynapse(synapse2);
	outputNeuron->removeSynapse(synapse1);
	QList<Synapse*> synapses = modNet->getSynapses();
	QVERIFY(synapses.isEmpty());
	delete synapse1;
	delete synapse2;
	
	// Try to safe a net with hidden neuron with invalid SpinalChordAddress
	group = modNet->getNeuronGroup("ABAL");
	Neuron *abalHidden1 = new Neuron("ABAL_HIDDEN_1", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	abalHidden1->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "37");
	group->addNeuron(abalHidden1);
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("Hidden neuron in accelboard module ABAL has invalid SpinalCordAddress or SpinalCordAddress is assigned twice: %1 = 37!").arg(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY));
	errorMsg.clear();
	abalHidden1->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "38");

	// Try to safe a net with two hidden neurons with same SpinalChordAddress
	Neuron *abalHidden2 = new Neuron("ABAL_HIDDEN_2", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	abalHidden2->setProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY, "38");
	group->addNeuron(abalHidden2);
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("Hidden neuron in accelboard module ABAL has invalid SpinalCordAddress or SpinalCordAddress is assigned twice: %1 = 38!").arg(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY));
	errorMsg.clear();
	abalHidden2->removeProperty(NeuralNetworkIOBytecode::SPINAL_CHORD_ADDRESS_PROPERTY);

	// Try to safe a net with two mich hidden neurons with outgoing connections
	Neuron *abalHidden3 = new Neuron("ABAL_HIDDEN_3", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	group->addNeuron(abalHidden3);
	Neuron *abalHidden4 = new Neuron("ABAL_HIDDEN_4", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	group->addNeuron(abalHidden4);
	Neuron *abalHidden5 = new Neuron("ABAL_HIDDEN_5", *(modNet->getDefaultTransferFunction()),
								*(modNet->getDefaultActivationFunction()));
	group->addNeuron(abalHidden5);

	Synapse::createSynapse(abalHidden1, absrOut1, 2.11, *(modNet->getDefaultSynapseFunction()));
	Synapse::createSynapse(abalHidden2, absrOut1, 2.12, *(modNet->getDefaultSynapseFunction()));
	Synapse::createSynapse(abalHidden3, absrOut1, 2.13, *(modNet->getDefaultSynapseFunction()));
	Synapse::createSynapse(abalHidden4, absrOut1, 2.14, *(modNet->getDefaultSynapseFunction()));
	Synapse *synapse = Synapse::createSynapse(abalHidden5, absrOut1, 2.15, *(modNet->getDefaultSynapseFunction()));

	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("No more free SpinalChordAddresses left for hidden neuron with outgoing connections in accelboard module ABAL!"));
	errorMsg.clear();
	absrOut1->removeSynapse(synapse);
	delete synapse;

	// Try to safe network with two much hidden neurons.
	group = modNet->getNeuronGroup("ABAR");
	Neuron *abarHiddenNeurons[37];
	for(int i = 0; i < 37; i++) {
		abarHiddenNeurons[i] = new Neuron(QString("ABAR_HIDDEN_%1").arg(i+1), *(modNet->getDefaultTransferFunction()), *(modNet->getDefaultActivationFunction()));
		group->addNeuron(abarHiddenNeurons[i]);
	}
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(!ret);
	QVERIFY(!errorMsg.isEmpty());
	QVERIFY(errorMsg == QString("No more free Internal Memory and no more free SpinalChordAddresses left for hidden neuron without outgoing connections in accelboard module ABAR!"));
	errorMsg.clear();
	modNet->removeNeuron(abarHiddenNeurons[36]);
	delete abarHiddenNeurons[36];

	// Safe complete network and compare to reference file
	Synapse::createSynapse(abfrIn1, abalHidden2, 3.32, *(modNet->getDefaultSynapseFunction()));
	Synapse::createSynapse(abfrIn1, abalHidden3, 3.33, *(modNet->getDefaultSynapseFunction()));
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(ret);
	QVERIFY(errorMsg.isEmpty());

	// Compare file to reference file.
	QFile referenceFile("IO/bytecodeWrite_reference.hyb");
	QVERIFY(referenceFile.exists());
	QVERIFY(referenceFile.open(QIODevice::ReadOnly));
	QVERIFY(file.open(QIODevice::ReadOnly));
	QTextStream refIn(&referenceFile);
	QTextStream fileIn(&file);
	while(!refIn.atEnd()) {
		QVERIFY(!fileIn.atEnd());
		QString refLine = refIn.readLine();
		QString fileLine = fileIn.readLine();
		QVERIFY(refLine == fileLine);
	}

	file.close();
	referenceFile.close();
	QVERIFY(file.remove());

	// Safe network with FLIP_ACTIVITY neurons and compare to reference file
	Synapse::createSynapse(abalHidden2, abalOut1, 3.24, *(modNet->getDefaultSynapseFunction()));
	abalOut1->flipActivation(true);
	abalHidden2->flipActivation(true);
	Synapse::createSynapse(abalHidden2, abalHidden2, 3.30, *(modNet->getDefaultSynapseFunction()));
	ret = NeuralNetworkIO::createFileFromNetwork(file.fileName(),
			modNet, NeuralNetworkIO::Bytecode, &errorMsg);
	QVERIFY(ret);
	QVERIFY(errorMsg.isEmpty());

	// Compare file to reference file.
	referenceFile.setFileName("IO/bytecodeWrite_reference2.hyb");
	QVERIFY(referenceFile.exists());
	QVERIFY(referenceFile.open(QIODevice::ReadOnly));
	QVERIFY(file.open(QIODevice::ReadOnly));
	QTextStream refIn2(&referenceFile);
	QTextStream fileIn2(&file);
	while(!refIn2.atEnd()) {
		QVERIFY(!fileIn2.atEnd());
		QString refLine = refIn2.readLine();
		QString fileLine = fileIn2.readLine();
		QVERIFY(refLine == fileLine);
	}

	file.close();
	referenceFile.close();
	QVERIFY(file.remove());

	delete modNet;

	Core::resetCore();
}

void TestNeuralNetworkIO::testCreateASeriesNetFromSimbaV3File() {
	Core::resetCore();
	Neuro::install();

//	Core::getInstance()->init();
	Neuro::getNeuralNetworkManager();

	StandardNeuralNetworkFunctions();

	QString errorMsg;
	QList<QString> warnings;
	ModularNeuralNetwork *modNet;

	// Try to load a network with a synapse with a synapse as target
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries1.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "There is a synapse with a synapse as target in the net!");
	errorMsg.clear();

	// Try to save a network with a neuron which doesn't start with an accelboard name
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries2.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "Every neuron name in the net has to start with an accelboard name (e.g. ABAL/Hidden1). There is a neuron in the net with the name Simsalabimba!");
	errorMsg.clear();

	// Try to save a net with missing input neurons
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries3.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "At least one input neuron missing. Input neuron with name ABAL/Left/Arm/AccelX not found!");
	errorMsg.clear();

	// Try to save a net with all input neurons and one additional unvalid input neuron
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries4.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "Input neuron has an unknown name or name is assigned twice: ABAL/Simsalabimba!");
	errorMsg.clear();

	// Try to save a net with all input neurons and one input neuron twice
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries5.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "Input neuron has an unknown name or name is assigned twice: ABSR/Waist/Angle!");
	errorMsg.clear();

	// Try to save a net with all input neurons and missing output neurons
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries6.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "At least one output neuron missing. Output neuron with name ABAL/Left/Elbow/DesiredAngle not found!");
	errorMsg.clear();

	// Try to save a net with all input and output neurons and one additional unvalid output neuron
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries7.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "Output neuron has an unknown name or name is assigned twice: ABFR/Simsalabimba!");
	errorMsg.clear();

	// Try to save a net with all input and output neurons and one output neuron twice
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries8.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "Output neuron has an unknown name or name is assigned twice: ABSR/Waist/DesiredAngle!");
	errorMsg.clear();

	// Try to load a complete and correct network without synapses
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries9.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(modNet != NULL);
	delete modNet;
	modNet = NULL;

	// Try to load a complete network with unexportable synapses (5 outgoing connections from ABAL)
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries10.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "Imported network is not exportable to A-Series bytecode, error:\nNo more free SpinalChordAddresses left for hidden neuron with outgoing connections in accelboard module ABAL!");
	errorMsg.clear();

	// Try to load a complete network with exportable synapses (4 outgoing connections from ABAL)
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries11.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(modNet != NULL);
	delete modNet;
	modNet = NULL;

	// Try to load a complete network with too much hidden neurons in ABAL (33 neurons without synapses and 4 outgoing synapses
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries12.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "Imported network is not exportable to A-Series bytecode, error:\nNo more free Internal Memory and no more free SpinalChordAddresses left for hidden neuron without outgoing connections in accelboard module ABAL!");
	errorMsg.clear();

	// Try to load a complete network with 32 hidden neurons in ABAL without synapses and 4 outgoing synapses
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries13.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(modNet != NULL);
	delete modNet;
	modNet = NULL;

	// Try to load a complete network with unexportable synapses (5 outgoing connections from ABAR)
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries14.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(modNet == NULL);
	QVERIFY(errorMsg == "Imported network is not exportable to A-Series bytecode, error:\nNo more free SpinalChordAddresses left for hidden neuron with outgoing connections in accelboard module ABAR!");
	errorMsg.clear();

	Core::resetCore();
}

void TestNeuralNetworkIO::testCreateASeriesFileFromNetwork() {
	Core::resetCore();
	Neuro::install();

//	Core::getInstance()->init();
	Neuro::getNeuralNetworkManager();

	StandardNeuralNetworkFunctions();

	QString errorMsg;
	QList<QString> warnings;
	ModularNeuralNetwork *modNet;
	bool ret;

	// Load an A-Series net and try to save it and reload it
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile("IO/simbaV3testASeries13.xml", &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(modNet != NULL);
	
	QString testFileName = "simbaASeriesOutputTest.xml";
	ret = NeuralNetworkIOSimbaV3Xml::createASeriesFileFromNetwork(testFileName, modNet, &errorMsg);
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(ret);
	delete modNet;
	modNet = NULL;

	// Try to load it as an ASeries Net
	modNet = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile(testFileName, &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(modNet != NULL);
	delete modNet;
	modNet = NULL;

	// Try to load it as a normal NERD net
	NeuralNetwork *net = NeuralNetworkIOSimbaV3Xml::createNetFromFile(testFileName, &errorMsg, &warnings);
	QVERIFY(warnings.isEmpty());
	QVERIFY(errorMsg.isEmpty());
	QVERIFY(net != NULL);
	delete net;
	net = NULL;

	QFile testFile(testFileName);
	QVERIFY(testFile.remove());

/*
QListIterator<QString> warningsIt(warnings);
while(warningsIt.hasNext()) {
qDebug() << warningsIt.next();
}
*/

	Core::resetCore();
}

}
