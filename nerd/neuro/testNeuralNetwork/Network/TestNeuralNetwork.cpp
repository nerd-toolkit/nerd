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




#include "TestNeuralNetwork.h"
#include <iostream>
#include "Network/NeuralNetwork.h"
#include "Network/SynapseAdapter.h"
#include "Network/NeuronAdapter.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "Control/ControlInterfaceAdapter.h"
#include "Network/NeuralNetworkAdapter.h"

using namespace std;
using namespace nerd;

void TestNeuralNetwork::initTestCase() {
}

void TestNeuralNetwork::cleanUpTestCase() {
}

//Chris
void TestNeuralNetwork::testConstructionAndSetters() {
	
	//create default network
	NeuralNetwork n1;

	n1.setProperty("ATestProperty", "Content");
	QVERIFY(n1.getProperty("ATestProperty") == "Content");
	
	QVERIFY(n1.getDefaultTransferFunction() != 0);
	QVERIFY(dynamic_cast<TransferFunctionTanh*>(n1.getDefaultTransferFunction()) != 0);
	QVERIFY(n1.getDefaultActivationFunction() != 0);
	QVERIFY(dynamic_cast<AdditiveTimeDiscreteActivationFunction*>(
			n1.getDefaultActivationFunction()) != 0);
	QVERIFY(n1.getDefaultSynapseFunction() != 0);
	QVERIFY(dynamic_cast<SimpleSynapseFunction*>(n1.getDefaultSynapseFunction()) != 0);
	QVERIFY(n1.getControlInterface() == 0);

	QVERIFY(n1.getNeurons().size() == 0);
	QVERIFY(n1.getInputNeurons().size() == 0);
	QVERIFY(n1.getOutputNeurons().size() == 0);
	QVERIFY(n1.getSynapses().size() == 0);

	qulonglong id = NeuralNetwork::generateNextId();
	QVERIFY(id != 0);
	QVERIFY(id != NeuralNetwork::generateNextId());


	//create network with specific default functions.
	TransferFunctionAdapter tfa("TFA", -0.5, 0.5);
	ActivationFunctionAdapter afa("AFA");
	SynapseFunctionAdapter sfa("SFA");

	bool delTfa = false;
	bool delAfa = false;
	bool delSfa = false;

	{
		NeuralNetwork n2(afa, tfa, sfa);

		TransferFunctionAdapter *tfa2 =
 			dynamic_cast<TransferFunctionAdapter*>(n2.getDefaultTransferFunction());
		ActivationFunctionAdapter *afa2 =
			dynamic_cast<ActivationFunctionAdapter*>(n2.getDefaultActivationFunction());
		SynapseFunctionAdapter *sfa2 =
			dynamic_cast<SynapseFunctionAdapter*>(n2.getDefaultSynapseFunction());

		QVERIFY(tfa2 != 0);
		QVERIFY(afa2 != 0);
		QVERIFY(sfa2 != 0);
		tfa2->mDeletedFlag = &delTfa;
		afa2->mDeletedFlag = &delAfa;
		sfa2->mDeletedFlag = &delSfa;

		n2.setDefaultTransferFunction(tfa);
		QVERIFY(delTfa == true);
		QVERIFY(delAfa == false);
		QVERIFY(delSfa == false);

		n2.setDefaultActivationFunction(afa);
		QVERIFY(delTfa == true);
		QVERIFY(delAfa == true);
		QVERIFY(delSfa == false);

		n2.setDefaultSynapseFunction(sfa);
		QVERIFY(delTfa == true);
		QVERIFY(delAfa == true);
		QVERIFY(delSfa == true);


		delTfa = false;
		delAfa = false;
		delSfa = false;

		tfa2 = dynamic_cast<TransferFunctionAdapter*>(n2.getDefaultTransferFunction());
		afa2 = dynamic_cast<ActivationFunctionAdapter*>(n2.getDefaultActivationFunction());
		sfa2 = dynamic_cast<SynapseFunctionAdapter*>(n2.getDefaultSynapseFunction());

		QVERIFY(tfa2 != 0);
		QVERIFY(afa2 != 0);
		QVERIFY(sfa2 != 0);
		tfa2->mDeletedFlag = &delTfa;
		afa2->mDeletedFlag = &delAfa;
		sfa2->mDeletedFlag = &delSfa;
	}

	//Default functions are deleted by neuralNetwork.
	QVERIFY(delTfa == true);
	QVERIFY(delAfa == true);
	QVERIFY(delSfa == true);
}



//Chris
void TestNeuralNetwork::testNeuronAndSynapseManagement() {
	TransferFunctionAdapter tfa("TFA", -0.5, 0.5);
	ActivationFunctionAdapter afa("AFA");
	SynapseFunctionAdapter sfa("SFA");

	NeuronAdapter *n1 = new NeuronAdapter("Neuron1", TransferFunctionAdapter("", 0.0, 1.0), 
			ActivationFunctionAdapter(""));
	NeuronAdapter *n2 = new NeuronAdapter("Neuron2", TransferFunctionAdapter("", 0.0, 1.0), 
			ActivationFunctionAdapter(""));
	NeuronAdapter *n3 = new NeuronAdapter("Neuron3", TransferFunctionAdapter("", 0.0, 1.0), 
			ActivationFunctionAdapter(""));

	SynapseAdapter *s1 = new SynapseAdapter(n1, 0, 0.5, SynapseFunctionAdapter(""));
	SynapseAdapter *s2 = new SynapseAdapter(n2, 0, 0.5, SynapseFunctionAdapter(""));
	SynapseAdapter *s3 = new SynapseAdapter(n3, 0, 0.5, SynapseFunctionAdapter(""));
	SynapseAdapter *s4 = new SynapseAdapter(n3, 0, 0.5, SynapseFunctionAdapter(""));
	SynapseAdapter *s5 = new SynapseAdapter(n1, 0, 0.5, SynapseFunctionAdapter(""));

	bool delS1 = false;
	bool delS2 = false;
	bool delS3 = false;
	bool delS4 = false;
	bool delS5 = false;

	s1->mDestroyedFlag = &delS1;
	s2->mDestroyedFlag = &delS2;
	s3->mDestroyedFlag = &delS3;
	s4->mDestroyedFlag = &delS4;
	s5->mDestroyedFlag = &delS5;


	//s4 is a synapse onto synapse s3.
	QVERIFY(s3->addSynapse(s4));

	//s5 is a synapse onto synspe s4
	QVERIFY(s4->addSynapse(s5));

	NeuralNetwork nn1(afa, tfa, sfa);

	QVERIFY(nn1.getNeurons().size() == 0);
	QVERIFY(nn1.getSynapses().size() == 0);

	//add NULL
	QVERIFY(nn1.addNeuron(0) == false);
	QVERIFY(nn1.getNeurons().size() == 0);

	//add first neuron
	QVERIFY(nn1.addNeuron(n1) == true);
	QVERIFY(nn1.getNeurons().size() == 1);
	QVERIFY(nn1.getNeurons().contains(n1));
	
	//add the same neuron again (fails)
	QVERIFY(nn1.addNeuron(n1) == false);
	QVERIFY(nn1.getNeurons().size() == 1);

	//add a neuron with synapses.
	QVERIFY(nn1.getSynapses().size() == 0);
	QVERIFY(n2->addSynapse(s1) == true);
	QVERIFY(n2->addSynapse(s2) == true);

	QVERIFY(nn1.addNeuron(n2) == true);
	QVERIFY(nn1.getNeurons().size() == 2);
	QVERIFY(nn1.getNeurons().contains(n1));
	QVERIFY(nn1.getNeurons().contains(n2));
	QVERIFY(nn1.getSynapses().size() == 2);	
	QVERIFY(nn1.getSynapses().contains(s1));
	QVERIFY(nn1.getSynapses().contains(s2));

	//add a synapse to an existing neuron
	n1->addSynapse(s3);	//works although synapse source is not part of the network.
	QVERIFY(nn1.getSynapses().size() == 5);
	QVERIFY(nn1.getSynapses().contains(s1));
	QVERIFY(nn1.getSynapses().contains(s2));
	QVERIFY(nn1.getSynapses().contains(s3));
	QVERIFY(nn1.getSynapses().contains(s4));
	QVERIFY(nn1.getSynapses().contains(s5));

	//add third neuron
	QVERIFY(nn1.addNeuron(n3) == true);
	QVERIFY(nn1.getNeurons().size() == 3);

	//check input / output neuron behavior
	QVERIFY(nn1.getInputNeurons().size() == 0);
	QVERIFY(nn1.getOutputNeurons().size() == 0);
	
	n1->setProperty(Neuron::NEURON_TYPE_INPUT);
	n2->setProperty(Neuron::NEURON_TYPE_INPUT);
	n2->setProperty(Neuron::NEURON_TYPE_OUTPUT);
	n3->setProperty(Neuron::NEURON_TYPE_OUTPUT);

	QVERIFY(nn1.getInputNeurons().size() == 2);
	QVERIFY(nn1.getInputNeurons().contains(n1));
	QVERIFY(nn1.getInputNeurons().contains(n2));
	QVERIFY(nn1.getOutputNeurons().size() == 2);
	QVERIFY(nn1.getOutputNeurons().contains(n2));
	QVERIFY(nn1.getOutputNeurons().contains(n3));

	//validate synapses (everything is ok)
	nn1.validateSynapseConnections();
	QVERIFY(nn1.getSynapses().size() == 5);
	QVERIFY(nn1.getSynapses().contains(s1));
	QVERIFY(nn1.getSynapses().contains(s2));
	QVERIFY(nn1.getSynapses().contains(s3));
	QVERIFY(nn1.getSynapses().contains(s4));
	QVERIFY(nn1.getSynapses().contains(s5));

	//remove NULL
	QVERIFY(nn1.removeNeuron(0) == false);
	QVERIFY(nn1.getSynapses().size() == 5);

	//remove one of the neurons
	QVERIFY(nn1.removeNeuron(n3) == true);
	QVERIFY(nn1.getNeurons().size() == 2);
	QVERIFY(nn1.getOutputNeurons().size() == 1);
	QVERIFY(nn1.getSynapses().size() == 5);
	QVERIFY(nn1.getNeurons().contains(n1));
	QVERIFY(nn1.getNeurons().contains(n2));

	//validate synapses (n3 is missing, so s3 is invalid and is removed)
	nn1.validateSynapseConnections();
	QVERIFY(nn1.getSynapses().size() == 2);
	QVERIFY(nn1.getSynapses().contains(s1));
	QVERIFY(nn1.getSynapses().contains(s2));

	//invalid synapses are destroyed (also indirect ones)
	QVERIFY(delS3 == true);
	QVERIFY(delS4 == true);
	QVERIFY(delS5 == true);

	QVERIFY(nn1.removeNeuron(n1));
	QVERIFY(nn1.getNeurons().size() == 1);
	QVERIFY(nn1.getSynapses().size() == 2);
	QVERIFY(delS1 == false);

	//validate
	nn1.validateSynapseConnections();
	QVERIFY(nn1.getSynapses().size() == 1);
	QVERIFY(nn1.getSynapses().contains(s2));
	QVERIFY(delS1 == true);

	QVERIFY(nn1.removeNeuron(n2));
	QVERIFY(nn1.getNeurons().size() == 0);
	QVERIFY(nn1.getSynapses().size() == 0);
	
	QVERIFY(delS2 == false);
	QVERIFY(n1->getSynapses().size() == 0);
	QVERIFY(n2->getSynapses().size() == 1);
	QVERIFY(n2->getSynapses().contains(s2));
	QVERIFY(n3->getSynapses().size() == 0);

	delete n1;
	delete n2;
	delete n3;

	QVERIFY(delS2 == true);
}


//Chris
void TestNeuralNetwork::testExecution() {
	TransferFunctionAdapter tfa("TFA", -0.5, 0.5);
	ActivationFunctionAdapter afa("AFA");
	SynapseFunctionAdapter sfa("SFA");

	NeuronAdapter *n1 = new NeuronAdapter("Neuron1", TransferFunctionAdapter("", 0.0, 1.0), 
			ActivationFunctionAdapter(""));
	NeuronAdapter *n2 = new NeuronAdapter("Neuron2", TransferFunctionAdapter("", 0.0, 1.0), 
			ActivationFunctionAdapter(""));

	NeuralNetwork nn1(afa, tfa, sfa);

	QVERIFY(nn1.addNeuron(n1));
	QVERIFY(nn1.addNeuron(n2));

	QCOMPARE(n1->getRequiredIterations(), 1);
	QCOMPARE(n1->getStartIteration(), 0);

	QVERIFY(n1->mResetCounter == 0);
	QVERIFY(n2->mResetCounter == 0);
	QVERIFY(n1->mUpdateActivationCounter == 0);
	QVERIFY(n2->mUpdateActivationCounter == 0);
	QVERIFY(n1->mPrepareCounter == 0);
	QVERIFY(n2->mPrepareCounter == 0);

	nn1.reset();

	QVERIFY(n1->mResetCounter == 1);
	QVERIFY(n2->mResetCounter == 1);
	QVERIFY(n1->mUpdateActivationCounter == 0);
	QVERIFY(n2->mUpdateActivationCounter == 0);
	QVERIFY(n1->mPrepareCounter == 0);
	QVERIFY(n2->mPrepareCounter == 0);

	nn1.executeStep();

	QCOMPARE(n1->mResetCounter, 1);
	QCOMPARE(n2->mResetCounter, 1);
	QCOMPARE(n1->mUpdateActivationCounter, 1);
	QCOMPARE(n2->mUpdateActivationCounter, 1);
	QCOMPARE(n1->mPrepareCounter, 2);
	QCOMPARE(n2->mPrepareCounter, 2);

}


//Chris
void TestNeuralNetwork::testNetworkDuplication() {
	TransferFunctionAdapter tfa("DefTFA", -0.5, 0.5);
	ActivationFunctionAdapter afa("DefAFA");
	SynapseFunctionAdapter sfa("DefSFA");

	NeuronAdapter *n1 = new NeuronAdapter("Neuron1", TransferFunctionAdapter("TF1", -1.0, 1.0), 
			ActivationFunctionAdapter("AF1"));
	NeuronAdapter *n2 = new NeuronAdapter("Neuron2", TransferFunctionAdapter("TF2", 0.0, 5.0), 
			ActivationFunctionAdapter("AF2"));
	NeuronAdapter *n3 = new NeuronAdapter("Neuron3", TransferFunctionAdapter("TF3", 0.0, 1.0), 
			ActivationFunctionAdapter("AF3"));

	SynapseAdapter *s1 = new SynapseAdapter(n1, 0, 1.5, SynapseFunctionAdapter("SF1"));
	SynapseAdapter *s2 = new SynapseAdapter(n3, 0, 2.5, SynapseFunctionAdapter("SF2"));
	SynapseAdapter *s3 = new SynapseAdapter(n1, 0, 3.5, SynapseFunctionAdapter("SF3"));
	SynapseAdapter *s4 = new SynapseAdapter(n2, 0, 4.5, SynapseFunctionAdapter("SF4"));
	SynapseAdapter *s5 = new SynapseAdapter(n2, 0, 5.5, SynapseFunctionAdapter("SF5"));
	
	//s5 is a synapse onto synapse s4.
	QVERIFY(s4->addSynapse(s5));

	bool delN1 = false;
	bool delN2 = false;
	bool delN3 = false;
	
	n1->mDeletedFlag = &delN1;
	n2->mDeletedFlag = &delN2;
	n3->mDeletedFlag = &delN3;

	NeuralNetwork *nn1 = new NeuralNetwork(afa, tfa, sfa);

	QVERIFY(nn1->addNeuron(n1));
	QVERIFY(nn1->addNeuron(n2));
	QVERIFY(nn1->addNeuron(n3));

	n2->setProperty(Neuron::NEURON_TYPE_INPUT);
	n3->setProperty(Neuron::NEURON_TYPE_OUTPUT);

	QVERIFY(n1->addSynapse(s1));
	QVERIFY(n1->addSynapse(s2));
	QVERIFY(n2->addSynapse(s3));
	QVERIFY(n3->addSynapse(s4));
	
	nn1->validateSynapseConnections();

	QVERIFY(nn1->getNeurons().size() == 3);
	QVERIFY(nn1->getInputNeurons().size() == 1);
	QVERIFY(nn1->getInputNeurons().contains(n2));
	QVERIFY(nn1->getOutputNeurons().size() == 1);
	QVERIFY(nn1->getOutputNeurons().contains(n3));
	QVERIFY(nn1->getSynapses().size() == 5);

	//add a control interface (to show that it is NOT copied).
	ControlInterfaceAdapter cia;
	nn1->setControlInterface(&cia);
	QVERIFY(nn1->getControlInterface() == &cia);


	nn1->setProperty("TestProperty", "Content");
	QVERIFY(nn1->getProperty("TestProperty") == "Content");

	//COPY the network

	NeuralNetwork *nn2 = nn1->createCopy();

	QVERIFY(nn2->getProperty("TestProperty") == "Content");

	QVERIFY(nn1->equals(nn2)); //see separate test below 

	QVERIFY(nn2->getControlInterface() == 0); //interface is NOT copied.
	QVERIFY(nn2->getNeurons().size() == 3);
	QVERIFY(nn2->getInputNeurons().size() == 1);
	QVERIFY(nn2->getOutputNeurons().size() == 1);
	QCOMPARE(nn2->getSynapses().size(), 5);

	//network already is valid, so changes to expect.
	nn2->validateSynapseConnections();

	QVERIFY(nn2->getNeurons().size() == 3);
	QVERIFY(nn2->getInputNeurons().size() == 1);
	QVERIFY(nn2->getOutputNeurons().size() == 1);
	QVERIFY(nn2->getSynapses().size() == 5);

	QVERIFY(nn2->getNeurons().at(0)->getNameValue().get() == "Neuron1");
	QVERIFY(nn2->getNeurons().at(1)->getNameValue().get() == "Neuron2");
	QVERIFY(nn2->getNeurons().at(2)->getNameValue().get() == "Neuron3");

	//neurons are deep copies, not just pointer copies.
	Neuron *cn1 = nn2->getNeurons().at(0);
	Neuron *cn2 = nn2->getNeurons().at(1);
	Neuron *cn3 = nn2->getNeurons().at(2);

	QVERIFY(cn1 != 0);
	QVERIFY(cn2 != 0);
	QVERIFY(cn3 != 0);


	QVERIFY(cn1 != n1);	
	QVERIFY(cn2 != n2);
	QVERIFY(cn3 != n3);
	
	QVERIFY(cn1->getTransferFunction()->getName() == "TF1");
	QVERIFY(cn1->getActivationFunction()->getName() == "AF1");
	QVERIFY(cn2->getTransferFunction()->getName() == "TF2");
	QVERIFY(cn2->getActivationFunction()->getName() == "AF2");
	QVERIFY(cn3->getTransferFunction()->getName() == "TF3");
	QVERIFY(cn3->getActivationFunction()->getName() == "AF3");

	//Synapses are copied corretly
	QVERIFY(cn1->getSynapses().size() == 2);
	QVERIFY(cn2->getSynapses().size() == 1);
	QVERIFY(cn3->getSynapses().size() == 1);

	Synapse *cs3 = cn2->getSynapses().at(0);
	
	QVERIFY(cs3->getStrengthValue().get() == 3.5);
	QVERIFY(cs3->getSynapseFunction() != 0);
	QVERIFY(cs3->getSynapseFunction()->getName() == "SF3");
	QVERIFY(cs3->getSynapses().size() == 0);
	QVERIFY(cs3->getSource() == cn1);
	QVERIFY(cs3->getTarget() == cn2);

	Synapse *cs4 = cn3->getSynapses().at(0);

	QVERIFY(cs4->getStrengthValue().get() == 4.5);
	QVERIFY(cs4->getSynapseFunction() != 0);
	QVERIFY(cs4->getSynapseFunction()->getName() == "SF4");
	QVERIFY(cs4->getSynapses().size() == 1);
	QVERIFY(cs4->getSource() == cn2);
	QVERIFY(cs4->getTarget() == cn3);

	Synapse *cs5 = cs4->getSynapses().at(0);
	
	QVERIFY(cs5->getStrengthValue().get() == 5.5);
	QVERIFY(cs5->getSynapseFunction() != 0);
	QVERIFY(cs5->getSynapseFunction()->getName() == "SF5");
	QVERIFY(cs5->getSynapses().size() == 0);
	QVERIFY(cs5->getSource() == cn2);
	QVERIFY(cs5->getTarget() == cs4);


	delete nn1;
	delete nn2;

	//neurons are deleted with the neural network
	QVERIFY(delN1 == true);
	QVERIFY(delN2 == true);
	QVERIFY(delN3 == true);

}


//Chris
void TestNeuralNetwork::testInterfaceHandling() {
	TransferFunctionAdapter tfa("DefTFA", -0.5, 0.5);
	ActivationFunctionAdapter afa("DefAFA");
	SynapseFunctionAdapter sfa("DefSFA");

	NeuronAdapter *n1 = new NeuronAdapter("INeuron1", TransferFunctionAdapter("TF1", -1.0, 1.0), 
			ActivationFunctionAdapter("AF1"));
	NeuronAdapter *n2 = new NeuronAdapter("IONeuron2", TransferFunctionAdapter("TF2", 0.0, 5.0), 
			ActivationFunctionAdapter("AF2"));
	NeuronAdapter *n3 = new NeuronAdapter("ONeuron3", TransferFunctionAdapter("TF3", 0.0, 1.0), 
			ActivationFunctionAdapter("AF3"));

	InterfaceValue iv1("", "Input1", 0.0, -0.5, 0.5);
	InterfaceValue iv2("", "Input2", -1.5, -1.5, 1.5);
	InterfaceValue iv3("", "Output1", -1.5, -1.5, 1.5);
	InterfaceValue iv4("", "Output2", 0.0, -2.0, 1.0);
	
	ControlInterfaceAdapter cia;
	cia.mInputValues.append(&iv1);
	cia.mInputValues.append(&iv2);
	cia.mOutputValues.append(&iv3);
	cia.mOutputValues.append(&iv4);

	NeuralNetworkAdapter *nn1 = new NeuralNetworkAdapter(afa, tfa, sfa);

	QVERIFY(nn1->getControlInterface() == 0);
	QVERIFY(nn1->getRawInputPairs().size() == 0);
	QVERIFY(nn1->getRawOutputPairs().size() == 0);

	nn1->setControlInterface(&cia);
	QVERIFY(nn1->getRawInputPairs().size() == 0); //no neurons available
	QVERIFY(nn1->getRawOutputPairs().size() == 0); //no neurons available
	
	QVERIFY(nn1->addNeuron(n1) == true);
	n1->setProperty(Neuron::NEURON_TYPE_INPUT);
	QVERIFY(nn1->addNeuron(n2) == true);
	n2->setProperty(Neuron::NEURON_TYPE_INPUT);
	n2->setProperty(Neuron::NEURON_TYPE_OUTPUT);
	QVERIFY(nn1->addNeuron(n3) == true);
	n3->setProperty(Neuron::NEURON_TYPE_OUTPUT);

	QVERIFY(nn1->getInputNeurons().size() == 2);
	QVERIFY(nn1->getOutputNeurons().size() == 2);

	QVERIFY(nn1->getRawInputPairs().size() == 0); //new neurons are not considere until next set.
	QVERIFY(nn1->getRawOutputPairs().size() == 0);
	
	nn1->setControlInterface(&cia);
	QVERIFY(nn1->getControlInterface() == &cia);
	QVERIFY(nn1->getRawInputPairs().size() == 2); //now neurons are considered.
	QVERIFY(nn1->getRawOutputPairs().size() == 2);

	//the order is the order in which the neurons and interface values have been found...
	QVERIFY(nn1->getRawInputPairs().at(0).mNeuron == n1);
	QVERIFY(nn1->getRawInputPairs().at(0).mInterfaceValue == &iv3); //output vals with input neur.
	QVERIFY(nn1->getRawInputPairs().at(1).mNeuron == n2);
	QVERIFY(nn1->getRawInputPairs().at(1).mInterfaceValue == &iv4);
	QVERIFY(nn1->getRawOutputPairs().at(0).mNeuron == n2);
	QVERIFY(nn1->getRawOutputPairs().at(0).mInterfaceValue == &iv1);//input neur. with ouput vals
	QVERIFY(nn1->getRawOutputPairs().at(1).mNeuron == n3);
	QVERIFY(nn1->getRawOutputPairs().at(1).mInterfaceValue == &iv2);

	//min max of mapping changes only before each step execution.
	QCOMPARE(iv1.getNormalizedMin(), -0.5);
	QCOMPARE(iv1.getNormalizedMax(), 0.5);
	QCOMPARE(iv2.getNormalizedMin(), -1.5);
	QCOMPARE(iv2.getNormalizedMax(), 1.5);
	QCOMPARE(iv3.getNormalizedMin(), -1.5);
	QCOMPARE(iv3.getNormalizedMax(), 1.5);
	QCOMPARE(iv4.getNormalizedMin(), -2.0);
	QCOMPARE(iv4.getNormalizedMax(), 1.0);

	nn1->executeStep();

	QCOMPARE(iv1.getNormalizedMin(), 0.0);
	QCOMPARE(iv1.getNormalizedMax(), 5.0);
	QCOMPARE(iv2.getNormalizedMin(), 0.0);
	QCOMPARE(iv2.getNormalizedMax(), 1.0);
	QCOMPARE(iv3.getNormalizedMin(), -1.0);
	QCOMPARE(iv3.getNormalizedMax(), 1.0);
	QCOMPARE(iv4.getNormalizedMin(), 0.0);
	QCOMPARE(iv4.getNormalizedMax(), 5.0);
	

	//setting NULL as ControlInterface removes all pairs.
	nn1->setControlInterface(0);
	QVERIFY(nn1->getControlInterface() == 0);
	QVERIFY(nn1->getRawInputPairs().size() == 0); 
	QVERIFY(nn1->getRawOutputPairs().size() == 0);

	//order changes when name matches are found
	iv1.setInterfaceName("IONeuron2");
	iv2.setInterfaceName("ONeuron3");
	iv3.setInterfaceName("INeuron1");
	iv4.setInterfaceName("IONeuron2");

	nn1->setControlInterface(&cia);
	QVERIFY(nn1->getControlInterface() == &cia);
	QVERIFY(nn1->getRawInputPairs().size() == 2); //now neurons are considered.
	QVERIFY(nn1->getRawOutputPairs().size() == 2);

	//now the order is based on the matchingnames of neurons and interface values...
	QVERIFY(nn1->getRawInputPairs().at(0).mNeuron == n1);
	QVERIFY(nn1->getRawInputPairs().at(0).mInterfaceValue == &iv3); 
	QVERIFY(nn1->getRawInputPairs().at(1).mNeuron == n2);
	QVERIFY(nn1->getRawInputPairs().at(1).mInterfaceValue == &iv4);
	QVERIFY(nn1->getRawOutputPairs().at(0).mNeuron == n2);
	QVERIFY(nn1->getRawOutputPairs().at(0).mInterfaceValue == &iv1);
	QVERIFY(nn1->getRawOutputPairs().at(1).mNeuron == n3);
	QVERIFY(nn1->getRawOutputPairs().at(1).mInterfaceValue == &iv2);

	iv3.setInterfaceName("INeuron1_new");
	iv1.setInterfaceName("IONeuron2_new");

	nn1->setControlInterface(&cia);
	QVERIFY(nn1->getControlInterface() == &cia);
	QVERIFY(nn1->getRawInputPairs().size() == 2); //now neurons are considered.
	QVERIFY(nn1->getRawOutputPairs().size() == 2);

	//now the order is based on the matchingnames of neurons and interface values
	//but iv3/n1 and iv1/n2 are now added by order, not by name matching...
	QVERIFY(nn1->getRawInputPairs().at(0).mNeuron == n2);
	QVERIFY(nn1->getRawInputPairs().at(0).mInterfaceValue == &iv4);
	QVERIFY(nn1->getRawInputPairs().at(1).mNeuron == n1);
	QVERIFY(nn1->getRawInputPairs().at(1).mInterfaceValue == &iv3); 
	QVERIFY(nn1->getRawOutputPairs().at(0).mNeuron == n3);
	QVERIFY(nn1->getRawOutputPairs().at(0).mInterfaceValue == &iv2);
	QVERIFY(nn1->getRawOutputPairs().at(1).mNeuron == n2);
	QVERIFY(nn1->getRawOutputPairs().at(1).mInterfaceValue == &iv1);
	
	delete nn1;
}


// Chris
void TestNeuralNetwork::testDuplicationAndEquals() {
	TransferFunctionAdapter tfa("TFA", -0.5, 0.5);
	ActivationFunctionAdapter afa("AFA");
	SynapseFunctionAdapter sfa("SFA");

	NeuralNetwork *net = new NeuralNetwork(afa, tfa, sfa);

	ControlInterfaceAdapter controlInterface;
	net->setControlInterface(&controlInterface);

	QVERIFY(net->getControlInterface() == &controlInterface);

	Neuron *neuron1 = new Neuron("Neuron1", tfa, afa, 2001);
	Neuron *neuron2 = new Neuron("Neuron1", tfa, afa, 2002);
	Neuron *neuron3 = new Neuron("Neuron1", tfa, afa, 2003);

	neuron1->setProperty(Neuron::NEURON_TYPE_INPUT);
	neuron3->setProperty(Neuron::NEURON_TYPE_OUTPUT);

	Synapse *synapse1 = Synapse::createSynapse(neuron1, neuron2, 0.5, sfa, 3001);
	Synapse *synapse2 = Synapse::createSynapse(neuron2, neuron3, 1.5, sfa, 3002);
	Synapse *synapse3 = Synapse::createSynapse(neuron3, synapse1, 0.1, sfa, 3003);

	net->addNeuron(neuron1);
	net->addNeuron(neuron2);
	net->addNeuron(neuron3);

	QCOMPARE(net->getNeurons().size(), 3);
	QVERIFY(net->getNeurons().contains(neuron1));
	QVERIFY(net->getNeurons().contains(neuron2));
	QVERIFY(net->getNeurons().contains(neuron3));

	QCOMPARE(net->getSynapses().size(), 3);
	QVERIFY(net->getSynapses().contains(synapse1));
	QVERIFY(net->getSynapses().contains(synapse2));
	QVERIFY(net->getSynapses().contains(synapse3));
	
	
	NeuralNetwork *copy = net->createCopy();

	//control interface is NOT copied.
	QVERIFY(copy->getControlInterface() == 0);

	QCOMPARE(copy->getNeurons().size(), 3);
	QVERIFY(!copy->getNeurons().contains(neuron1));
	QVERIFY(!copy->getNeurons().contains(neuron2));
	QVERIFY(!copy->getNeurons().contains(neuron3));

	QCOMPARE(copy->getSynapses().size(), 3);
	QVERIFY(!copy->getSynapses().contains(synapse1));
	QVERIFY(!copy->getSynapses().contains(synapse2));
	QVERIFY(!copy->getSynapses().contains(synapse3));

	Neuron *cNeuron1 = NeuralNetwork::selectNeuronById(neuron1->getId(), copy->getNeurons());
	Neuron *cNeuron2 = NeuralNetwork::selectNeuronById(neuron2->getId(), copy->getNeurons());
	Neuron *cNeuron3 = NeuralNetwork::selectNeuronById(neuron3->getId(), copy->getNeurons());

	Synapse *cSynapse1 = NeuralNetwork::selectSynapseById(synapse1->getId(), copy->getSynapses());
	Synapse *cSynapse2 = NeuralNetwork::selectSynapseById(synapse2->getId(), copy->getSynapses());
	Synapse *cSynapse3 = NeuralNetwork::selectSynapseById(synapse3->getId(), copy->getSynapses());
	
	QVERIFY(cNeuron1 != 0);
	QVERIFY(cNeuron2 != 0);
	QVERIFY(cNeuron3 != 0);

	QVERIFY(cNeuron1->equals(neuron1));
	QVERIFY(cNeuron2->equals(neuron2));
	QVERIFY(cNeuron3->equals(neuron3));

	QCOMPARE(cNeuron1->getId(), (qulonglong) 2001);
	QCOMPARE(cNeuron2->getId(), (qulonglong) 2002);
	QCOMPARE(cNeuron3->getId(), (qulonglong) 2003);

	QVERIFY(net->getInputNeurons().size() == 1);
	QVERIFY(copy->getInputNeurons().size() == 1);
	QVERIFY(net->getInputNeurons().at(0) == neuron1);
	QVERIFY(copy->getInputNeurons().at(0) == cNeuron1);
	QVERIFY(net->getOutputNeurons().size() == 1);
	QVERIFY(copy->getOutputNeurons().size() == 1);
	QVERIFY(net->getOutputNeurons().at(0) == neuron3);
	QVERIFY(copy->getOutputNeurons().at(0) == cNeuron3);

	QVERIFY(cSynapse1 != 0);
	QVERIFY(cSynapse2 != 0);
	QVERIFY(cSynapse3 != 0);

	QVERIFY(cSynapse1->equals(synapse1));
	QVERIFY(cSynapse2->equals(synapse2));
	QVERIFY(cSynapse3->equals(synapse3));

	QCOMPARE(cSynapse1->getId(), (qulonglong) 3001);
	QCOMPARE(cSynapse2->getId(), (qulonglong) 3002);
	QCOMPARE(cSynapse3->getId(), (qulonglong) 3003);

	QVERIFY(cSynapse1->getSource() == cNeuron1);
	QVERIFY(cSynapse2->getSource() == cNeuron2);
	QVERIFY(cSynapse3->getSource() == cNeuron3);
	QVERIFY(cSynapse1->getTarget() == cNeuron2);
	QVERIFY(cSynapse2->getTarget() == cNeuron3);
	QVERIFY(cSynapse3->getTarget() == cSynapse1);

	QVERIFY(net->equals(copy));

	//apply changes

	//change properties
	net->setProperty("TestP", "Test");
	QVERIFY(net->equals(copy) == false);
	copy->setProperty("TestP", "Test");
	QVERIFY(net->equals(copy) == true);

	//neuron bias
	neuron1->getBiasValue().set(0.001);
	QVERIFY(net->equals(copy) == false);
	cNeuron1->getBiasValue().set(0.001);
	QVERIFY(net->equals(copy) == true);

	//synapse target
	synapse3->setTarget(synapse2);
	QVERIFY(net->equals(copy) == false);
	cSynapse3->setTarget(cSynapse2);
	QVERIFY(net->equals(copy) == true);

	//default functions
	net->setDefaultActivationFunction(ActivationFunctionAdapter("Afa1"));
	QVERIFY(net->equals(copy) == false);
	copy->setDefaultActivationFunction(ActivationFunctionAdapter("WrongName"));
	QVERIFY(net->equals(copy) == false);
	copy->setDefaultActivationFunction(ActivationFunctionAdapter("Afa1"));
	QVERIFY(net->equals(copy) == true);

	net->setDefaultSynapseFunction(SynapseFunctionAdapter("Afa1"));
	QVERIFY(net->equals(copy) == false);
	copy->setDefaultSynapseFunction(SynapseFunctionAdapter("WrongName"));
	QVERIFY(net->equals(copy) == false);
	copy->setDefaultSynapseFunction(SynapseFunctionAdapter("Afa1"));
	QVERIFY(net->equals(copy) == true);

	net->setDefaultTransferFunction(TransferFunctionAdapter("Afa1", 0.1, 0.5));
	QVERIFY(net->equals(copy) == false);
	copy->setDefaultTransferFunction(TransferFunctionAdapter("WrongName", 0.1, 0.5));
	QVERIFY(net->equals(copy) == false);
	copy->setDefaultTransferFunction(TransferFunctionAdapter("Afa1", 0.3, 0.5));
	QVERIFY(net->equals(copy) == false);
	copy->setDefaultTransferFunction(TransferFunctionAdapter("Afa1", 0.1, 0.5));
	QVERIFY(net->equals(copy) == true);

	//input / output neurons
	neuron1->removeProperty(Neuron::NEURON_TYPE_INPUT);
	QVERIFY(net->equals(copy) == false);
	cNeuron1->removeProperty(Neuron::NEURON_TYPE_INPUT);
	QVERIFY(net->equals(copy) == true);	
	
	delete net;
	delete copy;
}


// chris
void TestNeuralNetwork::testSelectObjectsById() {
	
	Neuron *neuron1 = new Neuron("Neuron1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 100);
	Neuron *neuron2 = new Neuron("Neuron2", TransferFunctionAdapter("TF2", 0.0, 1.0), 
							ActivationFunctionAdapter("AF2"), 115);

	Synapse *synapse1 = new Synapse(0, 0, 0.5, SynapseFunctionAdapter("SF1"), 200);
	Synapse *synapse2 = new Synapse(0, 0, 0.5, SynapseFunctionAdapter("SF2"), 225);

	QList<Neuron*> neurons;
	neurons.append(neuron1);
	neurons.append(neuron2);
	
	QList<Synapse*> synapses;
	synapses.append(synapse1);
	synapses.append(synapse2);

	QList<SynapseTarget*> targets;
	targets.append(neuron2);
	targets.append(synapse2);

	//Neurons
	QVERIFY(NeuralNetwork::selectNeuronById(10101, neurons) == 0);
	QVERIFY(NeuralNetwork::selectNeuronById(100, QList<Neuron*>()) == 0);
	QVERIFY(NeuralNetwork::selectNeuronById(100, neurons) == neuron1);
	neuron1->setId(155);
	QVERIFY(NeuralNetwork::selectNeuronById(100, neurons) == 0);
	QVERIFY(NeuralNetwork::selectNeuronById(155, neurons) == neuron1);

	QVERIFY(NeuralNetwork::selectNeuronById(115, neurons) == neuron2);
	neurons.removeAll(neuron2);
	QVERIFY(NeuralNetwork::selectNeuronById(115, neurons) == 0);

	
	//Synapses
	QVERIFY(NeuralNetwork::selectSynapseById(20101, synapses) == 0);
	QVERIFY(NeuralNetwork::selectSynapseById(200, QList<Synapse*>()) == 0);
	QVERIFY(NeuralNetwork::selectSynapseById(200, synapses) == synapse1);
	synapse1->setId(255);
	QVERIFY(NeuralNetwork::selectSynapseById(200, synapses) == 0);
	QVERIFY(NeuralNetwork::selectSynapseById(255, synapses) == synapse1);

	QVERIFY(NeuralNetwork::selectSynapseById(225, synapses) == synapse2);
	synapses.removeAll(synapse2);
	QVERIFY(NeuralNetwork::selectSynapseById(225, synapses) == 0);


	//SynapseTargets
	QVERIFY(NeuralNetwork::selectSynapseTargetById(10101, targets) == 0);
	QVERIFY(NeuralNetwork::selectSynapseTargetById(115, QList<SynapseTarget*>()) == 0);
	QVERIFY(NeuralNetwork::selectSynapseTargetById(115, targets) == neuron2);
	neuron2->setId(355);
	QVERIFY(NeuralNetwork::selectSynapseTargetById(115, targets) == 0);
	QVERIFY(NeuralNetwork::selectSynapseTargetById(355, targets) == neuron2);

	QVERIFY(NeuralNetwork::selectSynapseTargetById(225, targets) == synapse2);
	targets.removeAll(synapse2);
	QVERIFY(NeuralNetwork::selectSynapseTargetById(225, targets) == 0);


	delete neuron1;
	delete neuron2;
	delete synapse1;
	delete synapse2;
	
}


void TestNeuralNetwork::testFreeElements() {
	bool destroyedNeuron1 = false;
	bool destroyedNeuron2 = false;

	NeuronAdapter *neuron1 = new NeuronAdapter("Neuron1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), &destroyedNeuron1);

	NeuronAdapter *neuron2 = new NeuronAdapter("Neuron2", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), &destroyedNeuron2);

	NeuralNetwork *net = new NeuralNetwork();

	InterfaceValue *inputValue = new InterfaceValue("/", "Test1");
	InterfaceValue *outputValue = new InterfaceValue("/", "Test1");
	ControlInterfaceAdapter *controller = new ControlInterfaceAdapter();
	controller->mInputValues.append(inputValue);
	controller->mOutputValues.append(outputValue);



	//free elements(false)

	QVERIFY(net->addNeuron(neuron1) == true);
	QVERIFY(net->addNeuron(neuron2) == true);

	net->setControlInterface(controller);

	QCOMPARE(net->getNeurons().size(), 2);
	QVERIFY(net->getNeurons().contains(neuron1));
	QVERIFY(net->getNeurons().contains(neuron2));

	net->freeElements(false);

	QCOMPARE(net->getNeurons().size(), 0);
	QVERIFY(destroyedNeuron1 == false);
	QVERIFY(destroyedNeuron2 == false);

	//free elements(true)

	QVERIFY(net->addNeuron(neuron1) == true);
	QVERIFY(net->addNeuron(neuron2) == true);

	net->setControlInterface(controller);

	QCOMPARE(net->getNeurons().size(), 2);
	QVERIFY(net->getNeurons().contains(neuron1));
	QVERIFY(net->getNeurons().contains(neuron2));

	net->freeElements(true);

	QCOMPARE(net->getNeurons().size(), 0);
	QVERIFY(destroyedNeuron1 == true);
	QVERIFY(destroyedNeuron2 == true);
}
