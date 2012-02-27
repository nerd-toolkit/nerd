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




#include "TestSynapse.h"
#include "Network/Synapse.h"
#include "Network/SynapseTarget.h"
#include "Network/Neuron.h"
#include <QString>
#include <QList>
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "Math/Math.h"
#include "Network/NeuronAdapter.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "Network/SynapseAdapter.h"
#include <iostream>
#include "TransferFunction/TransferFunctionAdapter.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "NeuralNetworkConstants.h"

using namespace std;

using namespace nerd;

void TestSynapse::initTestCase() {
}

void TestSynapse::cleanUpTestCase() {
}


//Chris
void TestSynapse::testConstruction() {
	SynapseFunctionAdapter sfa1("SynapseFunctionAdapter1");

	//check normal construction.
	Synapse s(0, 0, 0.75145, sfa1);
	QVERIFY(s.getId() != 0);
	QVERIFY(Math::compareDoubles(s.getStrengthValue().get(), 0.75145, 5));
	QVERIFY(s.getEnabledValue().get() == true);
	QVERIFY(s.getSource() == 0);
	QVERIFY(s.getTarget() == 0);
	QVERIFY(s.getSynapseFunction() != 0);
	QVERIFY(s.getSynapseFunction() != &sfa1);
	QVERIFY(s.getSynapseFunction()->getName() == "SynapseFunctionAdapter1");
	QVERIFY(dynamic_cast<SynapseFunctionAdapter*>(s.getSynapseFunction()) != 0);
	QVERIFY(s.getSynapses().size() == 0);
	QVERIFY(s.getPropertyNames().size() == 0);


	NeuronAdapter n1("Neuron1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
					ActivationFunctionAdapter("AF1"));
	NeuronAdapter n2("Neuron2", TransferFunctionAdapter("TF1", 0.0, 1.0), 
					ActivationFunctionAdapter("AF1"));

	
	//destruction behavior (target and source are not deleted, added synapses are.	
	bool destroyedNeuron1 = false;
	bool destroyedNeuron2 = false;
	n1.mDeletedFlag = &destroyedNeuron1;
	n2.mDeletedFlag = &destroyedNeuron2;

	SynapseAdapter *sa = new SynapseAdapter(&n1, &n2, 0.01, sfa1);
	bool destroyedSynapseAdapter1 = false;
	sa->mDestroyedFlag = &destroyedSynapseAdapter1;

	{
		//check normal construction (with target and source)
		Synapse s2(&n1, &n2, 12345.6789, sfa1);
		QVERIFY(s2.addSynapse(sa));
		s2.getEnabledValue().set(false);
		s2.setProperty("FirstProperty", "Value");

		QVERIFY(s2.getSynapses().size() == 1);
		QVERIFY(s2.getId() != 0);
		QVERIFY(s2.getId() != s.getId());
		QVERIFY(s2.getSource() == &n1);
		QVERIFY(s2.getTarget() == &n2);
		QVERIFY(s2.getEnabledValue().get() == false);
		QVERIFY(s2.getPropertyNames().size() == 2);

		//copy constructor
		
		
		Synapse *s3 = s2.createCopy();
		QVERIFY(s3->getId() != 0);
		QVERIFY(s3->getId() == s2.getId());
		QVERIFY(Math::compareDoubles(s3->getStrengthValue().get(),12345.6789, 7));
		QVERIFY(s3->getEnabledValue().get() == false);
		QVERIFY(s3->getSource() == &n1);
		QVERIFY(s3->getTarget() == &n2);
		QVERIFY(s3->getSynapseFunction() != 0);
		QVERIFY(s3->getSynapseFunction() != &sfa1);
		QVERIFY(s3->getSynapseFunction() != s2.getSynapseFunction());
		QVERIFY(s3->getSynapseFunction()->getName() == "SynapseFunctionAdapter1");
		QVERIFY(dynamic_cast<SynapseFunctionAdapter*>(s3->getSynapseFunction()) != 0);
		QVERIFY(s3->getSynapses().size() == 0);
		QVERIFY(s3->getPropertyNames().size() == 2);
		QVERIFY(s3->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
		QVERIFY(s3->hasProperty("FirstProperty"));
		QVERIFY(s3->getProperty("FirstProperty") == "Value");

		delete s3;
	}
	//check destruction behavior.
	QVERIFY(destroyedNeuron1 == false);
	QVERIFY(destroyedNeuron2 == false);
	QVERIFY(destroyedSynapseAdapter1 == true); //destroed by s2;

}



//Chris
void TestSynapse::testSetAndGet() {
	SynapseFunctionAdapter sfa1("SynapseFunctionAdapter1");
	SimpleSynapseFunction ssf1;

	NeuronAdapter n1("Neuron1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
					ActivationFunctionAdapter("AF1"));
	NeuronAdapter n2("Neuron2", TransferFunctionAdapter("TF2", 0.0, 1.0), 
					ActivationFunctionAdapter("AF2"));
	NeuronAdapter n3("Neuron3", TransferFunctionAdapter("TF3", 0.0, 1.0), 
					ActivationFunctionAdapter("AF3"));


	Synapse s(&n1, &n2, 0.75145, sfa1);
	QVERIFY(s.getId() != 0);
	QVERIFY(Math::compareDoubles(s.getStrengthValue().get(), 0.75145, 5));
	QVERIFY(s.getEnabledValue().get() == true);
	QVERIFY(s.getSource() == &n1);
	QVERIFY(s.getTarget() == &n2);
	QVERIFY(s.getSynapseFunction() != 0);
	QVERIFY(s.getSynapseFunction() != &sfa1);
	QVERIFY(s.getSynapseFunction()->getName() == "SynapseFunctionAdapter1");
	QVERIFY(dynamic_cast<SynapseFunctionAdapter*>(s.getSynapseFunction()) != 0);
	QVERIFY(s.getSynapses().size() == 0);

	s.setId(100);
	QVERIFY(s.getId() == 100);
	s.setId(54321);
	QVERIFY(s.getId() == 54321);

	s.getStrengthValue().set(123.987);
	QVERIFY(Math::compareDoubles(s.getStrengthValue().get(), 123.987, 5));
	
	s.getEnabledValue().set(false);
	QVERIFY(s.getEnabledValue().get() == false);
	s.getEnabledValue().set(true);
	QVERIFY(s.getEnabledValue().get() == true);

	s.setSource(&n2);
	QVERIFY(s.getSource() == &n2);
	s.setTarget(&n3);
	QVERIFY(s.getTarget() == &n3);
	//source and target can be 0
	s.setSource(0);	
	QVERIFY(s.getSource() == 0);
	s.setTarget(0);
	QVERIFY(s.getTarget() == 0);
	//source and target the same (ok)
	s.setSource(&n1); 
	QVERIFY(s.getSource() == &n1);
	s.setTarget(&n1);
	QVERIFY(s.getTarget() == &n1);

	s.setSynapseFunction(ssf1);
	QVERIFY(s.getSynapseFunction() != 0);
	QVERIFY(s.getSynapseFunction() != &ssf1);
	QVERIFY(s.getSynapseFunction()->getName() == "SimpleUpdateFunction");
	QVERIFY(dynamic_cast<SimpleSynapseFunction*>(s.getSynapseFunction()) != 0);

	QVERIFY(s.getPropertyNames().size() == 1);
	QVERIFY(s.hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
	s.setProperty("FirstProperty", "Value");
	QVERIFY(s.getPropertyNames().size() == 2);
	QVERIFY(s.hasProperty("FirstProperty"));
	QVERIFY(s.getProperty("FirstProperty") == "Value");
}



//Chris
void TestSynapse::testAddAndRemoveSynapses() {
	SynapseFunctionAdapter sfa1("SynapseFunctionAdapter1");
	NeuronAdapter n1("Neuron1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
					ActivationFunctionAdapter("AF1"));
	NeuronAdapter n2("Neuron2", TransferFunctionAdapter("TF2", 0.0, 1.0), 
					ActivationFunctionAdapter("AF2"));
	NeuronAdapter n3("Neuron3", TransferFunctionAdapter("TF3", 0.0, 1.0), 
					ActivationFunctionAdapter("AF3"));

	Synapse s(&n1, 0, 1.0, sfa1);
	SynapseAdapter sa1(&n2, &n2, 0.5, sfa1);
	SynapseAdapter sa2(&n2, &n2, 1.9, sfa1);
	SynapseAdapter sa3(&n1, &n1, 0.1, sfa1);
	
	QVERIFY(s.getSynapses().empty());
	QVERIFY(sa1.getTarget() == &n2);
	
	//add a first synapse (from neuron n2)
	QVERIFY(s.addSynapse(&sa1) == true);
	QVERIFY(s.getSynapses().size() == 1);
	QVERIFY(s.getSynapses().contains(&sa1)); //not a copy.
	QVERIFY(sa1.getTarget() == &s);

	//add a second synapse (also from neuron n2) [will fail, because also from n2]
	QVERIFY(s.addSynapse(&sa2) == false); 
	QVERIFY(s.getSynapses().size() == 1);
	QVERIFY(sa2.getTarget() == &n2);

	//add the same synapse again, but change the source neuron to n3 (works)
	sa2.setSource(&n3);
	QVERIFY(s.addSynapse(&sa2) == true); 
	QVERIFY(s.getSynapses().size() == 2);
	QVERIFY(s.getSynapses().contains(&sa1));
	QVERIFY(s.getSynapses().contains(&sa2));
	QVERIFY(sa2.getTarget() == &s);
	
	//add a synapse comming from the same neuron s is comming from (works)
	QVERIFY(s.addSynapse(&sa3) == true);
	QVERIFY(s.getSynapses().size() == 3);
	QVERIFY(s.getSynapses().contains(&sa1));
	QVERIFY(s.getSynapses().contains(&sa2));
	QVERIFY(s.getSynapses().contains(&sa3));
	QVERIFY(sa3.getTarget() == &s);

	//remove a synapse
	QVERIFY(s.removeSynapse(&sa2) == true);
	QVERIFY(sa2.getTarget() == 0); //does not point to s any more.
	QVERIFY(sa2.getSource() == &n3); //not modified.
	QVERIFY(s.getSynapses().size() == 2);
	QVERIFY(s.getSynapses().contains(&sa1));
	QVERIFY(s.getSynapses().contains(&sa2) == false);
	QVERIFY(s.getSynapses().contains(&sa3));

	//remove the same synapse again
	QVERIFY(s.removeSynapse(&sa2) == false);
	QVERIFY(s.getSynapses().size() == 2);

	//remove the other synapses
	QVERIFY(s.removeSynapse(&sa1));
	QVERIFY(s.removeSynapse(&sa3));

	QVERIFY(s.getSynapses().size() == 0);
	QVERIFY(sa1.getTarget() == 0);
	QVERIFY(sa3.getTarget() == 0);
	
}


//Chris
void TestSynapse::testActivationCalculation() {
	SimpleSynapseFunction sfa1;
	NeuronAdapter n1("Neuron1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
					ActivationFunctionAdapter("AF1"));
	NeuronAdapter n2("Neuron2", TransferFunctionAdapter("TF2", 0.0, 1.0), 
					ActivationFunctionAdapter("AF2"));

	Synapse s(&n1, 0, 1.5, sfa1);
	n1.mLastOutputActivationReturnValue = 2.0;

	QVERIFY(Math::compareDoubles(s.getActivation(), 0.0, 5));
	QVERIFY(Math::compareDoubles(s.calculateActivation(), 1.5 * 2.0, 5));
	
	n1.mLastOutputActivationReturnValue = 5.66642;
	s.getStrengthValue().set(-0.2);
	QVERIFY(Math::compareDoubles(s.calculateActivation(), 5.66642 * -0.2, 5));

	n2.mLastOutputActivationReturnValue = -10.0;
	QVERIFY(Math::compareDoubles(s.calculateActivation(), 5.66642 * -0.2, 5));
	//change source neuron (activation comes from the new neuron now)
	s.setSource(&n2);
	QVERIFY(Math::compareDoubles(s.calculateActivation(), -10.0 * -0.2, 5));
	

	QVERIFY(Math::compareDoubles(s.getActivation(), -10.0 * -0.2, 5));
	s.getStrengthValue().set(-0.4);
	QVERIFY(Math::compareDoubles(s.calculateActivation(), -10.0 * -0.4, 5));
	QVERIFY(Math::compareDoubles(s.getActivation(), -10.0 * -0.4, 5));

}

//Chris
void TestSynapse::testDuplication() {
	
	Neuron sourceNeuron("Source", TransferFunctionTanh(), AdditiveTimeDiscreteActivationFunction());
	sourceNeuron.setId(15);

	Neuron targetNeuron("Target", TransferFunctionTanh(), AdditiveTimeDiscreteActivationFunction());
	targetNeuron.setId(45);

	Synapse synapse(&sourceNeuron, &targetNeuron, 0.99, SimpleSynapseFunction());
	synapse.setId(22);

	Synapse *synapse2 = synapse.createCopy();

	QVERIFY(synapse.equals(0) == false);
	QVERIFY(synapse.equals(synapse2) == true);
	QVERIFY(synapse2->equals(&synapse) == true);

	//change strength
	synapse.getStrengthValue().set(0.2);
	QVERIFY(synapse.equals(synapse2) == false);
	synapse2->getStrengthValue().set(0.2);
	QVERIFY(synapse.equals(synapse2) == true);

	//change enable
	synapse.getEnabledValue().set(false);
	QVERIFY(synapse.equals(synapse2) == false);
	synapse2->getEnabledValue().set(false);
	QVERIFY(synapse.equals(synapse2) == true);

	//change synapseFunction
	synapse.getSynapseFunction()->setName("AnotherName");
	QVERIFY(synapse.equals(synapse2) == false);
	synapse2->getSynapseFunction()->setName("AnotherName");
	QVERIFY(synapse.equals(synapse2) == true);

	//change target and source
	synapse.setSource(&targetNeuron);
	QVERIFY(synapse.equals(synapse2) == false);
	synapse2->setSource(&targetNeuron);
	QVERIFY(synapse.equals(synapse2) == true);
	synapse.setTarget(&sourceNeuron);
	QVERIFY(synapse.equals(synapse2) == false);
	synapse2->setTarget(&sourceNeuron);
	QVERIFY(synapse.equals(synapse2) == true);

	//add properties
	synapse.setProperty("TestProp");
	QVERIFY(synapse.equals(synapse2) == false);
	synapse2->setProperty("TestProp");
	QVERIFY(synapse.equals(synapse2) == true);
	synapse.setProperty("Prop", "Value");
	QVERIFY(synapse.equals(synapse2) == false);
	synapse2->setProperty("Prop", "WrongValue"); //property value does not match
	QVERIFY(synapse.equals(synapse2) == false);
	synapse2->setProperty("Prop", "Value");
	QVERIFY(synapse.equals(synapse2) == true);

	//add other synapse
	Synapse synapse3(&sourceNeuron, &targetNeuron, 0.13, SimpleSynapseFunction());
	synapse3.setId(44);

	QVERIFY(synapse.addSynapse(synapse3.createCopy()));
	QVERIFY(synapse.equals(synapse2) == false);
	QVERIFY(synapse2->addSynapse(synapse3.createCopy()));
	QVERIFY(synapse.equals(synapse2) == true);

	//change child synapse
	synapse.getSynapses().at(0)->getStrengthValue().set(5.0);
	QVERIFY(synapse.equals(synapse2) == false);
	synapse2->getSynapses().at(0)->getStrengthValue().set(5.0);
	QVERIFY(synapse.equals(synapse2) == true);
	synapse.getSynapses().at(0)->setId(99);
	QVERIFY(synapse.equals(synapse2) == false);
	synapse2->getSynapses().at(0)->setId(99);
	QVERIFY(synapse.equals(synapse2) == true);

	delete synapse2;
}











