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




#include "TestNeuron.h"
#include <iostream>
#include "Network/Neuron.h"
#include "Network/NeuronAdapter.h"
#include "Network/Synapse.h"
#include "Network/SynapseAdapter.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include <QList>
#include <QListIterator>
#include "TransferFunction/TransferFunctionAdapter.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "Value/DoubleValue.h"
#include "Math/Math.h"


using namespace std;
using namespace nerd;

void TestNeuron::initTestCase() {
}

void TestNeuron::cleanUpTestCase() {
}


//Chris
void TestNeuron::testConstruction() {
	TransferFunctionAdapter tfa("TFA1", 0.0, 1.0);
	ActivationFunctionAdapter afa("AFA1");

	bool destroyedSynapse = false;
	{
		//construction
		Neuron n1("Neuron1", tfa, afa);
	
		QVERIFY(n1.getLastActivation() == 0.0);
		QVERIFY(n1.getId() != 0);
		QVERIFY(n1.getNameValue().get() == "Neuron1");
		QVERIFY(n1.getBiasValue().get() == 0.0);
		QVERIFY(n1.getActivationValue().get() == 0.0);
		QVERIFY(n1.getOutputActivationValue().get() == 0.0);
		QVERIFY(n1.getInputValues().size() == 0);
		QVERIFY(dynamic_cast<TransferFunctionAdapter*>(n1.getTransferFunction()) != 0);
		QVERIFY(n1.getTransferFunction() != &tfa);
		QVERIFY(dynamic_cast<TransferFunctionAdapter*>(n1.getTransferFunction())
					->getName() == "TFA1");
		QVERIFY(dynamic_cast<ActivationFunctionAdapter*>(n1.getActivationFunction()) != 0);
		QVERIFY(n1.getActivationFunction() != &afa);
		QVERIFY(dynamic_cast<ActivationFunctionAdapter*>(n1.getActivationFunction())
					->getName() == "AFA1");
		QVERIFY(n1.getSynapses().size() == 0);
		QVERIFY(n1.getPropertyNames().size() == 0);
		QVERIFY(n1.isActivationFlipped() == false);
	
		//prepare for copy
		SynapseAdapter *sa = new SynapseAdapter(&n1, &n1, 0.05, SynapseFunctionAdapter("SFA"));
		sa->mDestroyedFlag = &destroyedSynapse;
		QVERIFY(n1.addSynapse(sa) == true);
		DoubleValue inputValue(0.0101); //inputValues are not destroyed by neurons (so stack is ok)
		n1.addInputValue(&inputValue);
		n1.getActivationValue().set(0.011);
		n1.getOutputActivationValue().set(8.88);
		n1.getBiasValue().set(1.54321);
		n1.setProperty("Property1", "Value1");
		n1.flipActivation(true);
		QVERIFY(n1.isActivationFlipped() == true);

		//copy neuron
		Neuron *n2 = n1.createCopy();
		
		QVERIFY(n2->getLastActivation() == 0.0);
		QVERIFY(n2->getId() != 0);
		QVERIFY(n2->getId() == n1.getId()); //has the same id!
		QVERIFY(n2->getNameValue().get() == "Neuron1");
		QVERIFY(Math::compareDoubles(n2->getBiasValue().get(), 1.54321, 7));
		QVERIFY(n2->getActivationValue().get() == 0.011);
		QVERIFY(n2->getOutputActivationValue().get() == 8.88);
		QVERIFY(n2->getInputValues().size() == 0); //InputValues are NOT copied!
		QVERIFY(dynamic_cast<TransferFunctionAdapter*>(n2->getTransferFunction()) != 0);
		QVERIFY(n2->getTransferFunction() != &tfa);
		QVERIFY(dynamic_cast<TransferFunctionAdapter*>(n2->getTransferFunction())
					->getName() == "TFA1");
		QVERIFY(dynamic_cast<ActivationFunctionAdapter*>(n2->getActivationFunction()) != 0);
		QVERIFY(n2->getActivationFunction() != &afa);
		QVERIFY(dynamic_cast<ActivationFunctionAdapter*>(n2->getActivationFunction())
					->getName() == "AFA1");
		QVERIFY(n2->getSynapses().size() == 0); //synapses are NOT copied!
		QVERIFY(n2->getPropertyNames().size() == 1);
		QVERIFY(n2->hasProperty("Property1"));
		QVERIFY(n2->getProperty("Property1") == "Value1");
		QVERIFY(n2->isActivationFlipped() == true);

		delete n2;
	}

	QVERIFY(destroyedSynapse == true); //synapse was destroyed by n1.
	
}


//Chris
void TestNeuron::testGetAndSet() {
	TransferFunctionAdapter tfa("TFA1", 0.0, 1.0);
	ActivationFunctionAdapter afa("AFA1");

	Neuron n1("Neuron1", tfa, afa);

	n1.setId(12345);
	QVERIFY(n1.getId() == 12345);

	//get and set of name, bias and activation value are already tested 
	//by the Value test classes of the Core.
	
	DoubleValue v1(0.1);
	DoubleValue v2(0.2);

	QVERIFY(n1.getInputValues().size() == 0);
	
	//add NULL input value (fails)
	QVERIFY(n1.addInputValue(0) == false);
	QVERIFY(n1.getInputValues().size() == 0);

	//add a valid input value
	QVERIFY(n1.addInputValue(&v1) == true);
	QVERIFY(n1.getInputValues().size() == 1);
	QVERIFY(n1.getInputValues().at(0) == &v1);

	//add the same input value again (fails)
	QVERIFY(n1.addInputValue(&v1) == false);
	QVERIFY(n1.getInputValues().size() == 1);

	//add a second input value
	QVERIFY(n1.addInputValue(&v2) == true);
	QVERIFY(n1.getInputValues().size() == 2);
	QVERIFY(n1.getInputValues().contains(&v1));
	QVERIFY(n1.getInputValues().contains(&v2));

	//remove NULL from the input values (fails)
	QVERIFY(n1.removeInputValue(0) == false);
	QVERIFY(n1.getInputValues().size() == 2);

	//remove one of the input values
	QVERIFY(n1.removeInputValue(&v1) == true);
	QVERIFY(n1.getInputValues().size() == 1);
	QVERIFY(n1.getInputValues().at(0) == &v2);
	
	//remove the same input value again (fails)
	QVERIFY(n1.removeInputValue(&v1) == false);
	QVERIFY(n1.getInputValues().size() == 1);
	QVERIFY(n1.getInputValues().at(0) == &v2);
}


//Chris
void TestNeuron::addAndRemoveSynapses() {
	TransferFunctionAdapter tfa("TFA1", 0.0, 1.0);
	ActivationFunctionAdapter afa("AFA1");

	Neuron n1("Neuron1", tfa, afa);
	Neuron n2("Neuron2", tfa, afa);

	SynapseAdapter sa1(0, 0, 0.5, SynapseFunctionAdapter("SFA1"));
	SynapseAdapter sa2(&n1, &n2, 0.5, SynapseFunctionAdapter("SFA1"));

	QVERIFY(n1.getSynapses().size() == 0);
	
	//add synapse with NULL source
	QVERIFY(n1.addSynapse(&sa1) == false);
	QVERIFY(n1.getSynapses().size() == 0);
	sa1.setSource(&n1);

	//add a valid synapse
	QVERIFY(n1.addSynapse(&sa1) == true);
	QVERIFY(n1.getSynapses().size() == 1);
	QVERIFY(n1.getSynapses().at(0) == &sa1);
	QVERIFY(sa1.getTarget() == &n1);

	//add the same synapse again (fails)
	QVERIFY(n1.addSynapse(&sa1) == false);
	QVERIFY(n1.getSynapses().size() == 1);

	//add sa2 (also comming form n1) (fails)
	QVERIFY(n1.addSynapse(&sa2) == false);
	QVERIFY(n1.getSynapses().size() == 1);

	//add sa2 again, this time with a different source
	sa2.setSource(&n2);
	QVERIFY(n1.addSynapse(&sa2) == true);
	QVERIFY(n1.getSynapses().size() == 2);
	QVERIFY(n1.getSynapses().contains(&sa1));
	QVERIFY(n1.getSynapses().contains(&sa2));
	QVERIFY(sa2.getTarget() == &n1);

	//remove one of the synapses
	QVERIFY(n1.removeSynapse(&sa1) == true);
	QVERIFY(n1.getSynapses().size() == 1);
	QVERIFY(n1.getSynapses().contains(&sa2));
	QVERIFY(sa1.getTarget() == 0); //target reset to 0

	//remove the same synapse again
	QVERIFY(n1.removeSynapse(&sa1) == false);
	QVERIFY(n1.getSynapses().size() == 1);
	QVERIFY(n1.getSynapses().contains(&sa2));

	//has to be removed as synapsesa re deleted by the neuron destructors.
	n1.removeSynapse(&sa2);
}

//Chris
void TestNeuron::testNeuronCalculation() {
	TransferFunctionAdapter tfa("TFA1", 0.0, 1.0);
	ActivationFunctionAdapter afa("AFA1");

	bool destroyedTransferFunction = false;
	bool destroyedActivationFunction = false;
	bool destroyedSynapse = false;

	{
		Neuron n1("Neuron1", tfa, afa);
		NeuronAdapter n2("Neuron2", tfa, afa);
	
		SynapseAdapter *sa1 = new SynapseAdapter(&n2, &n2, 0.5, SynapseFunctionAdapter("SFA1"));
		SynapseAdapter *sa2 = new SynapseAdapter(&n1, &n2, 0.5, SynapseFunctionAdapter("SFA1"));

		QVERIFY(n1.addSynapse(sa1));
		sa1->mDestroyedFlag = &destroyedSynapse;
	
		TransferFunctionAdapter *tfan1 = dynamic_cast<TransferFunctionAdapter*>(
				n1.getTransferFunction());
		ActivationFunctionAdapter *afan1 = dynamic_cast<ActivationFunctionAdapter*>(
				n1.getActivationFunction());
		
		QVERIFY(tfan1 != 0);
		QVERIFY(afan1 != 0);
		QVERIFY(tfan1->mResetCounter == 0);
		QVERIFY(afan1->mResetCounter == 0);
		QVERIFY(tfan1->mLastOwner == 0);
		QVERIFY(afan1->mLastOwner == 0);

		QVERIFY(sa1->mResetCounter == 0);
		QVERIFY(sa2->mResetCounter == 0);

		tfan1->mDeletedFlag = &destroyedTransferFunction;
		afan1->mDeletedFlag = &destroyedActivationFunction;
	
		//last activation udpate
		QVERIFY(n1.getLastActivation() == 0.0);
		QVERIFY(n1.getLastOutputActivation() == 0.0);
		n1.getActivationValue().set(0.1112);
		n1.getOutputActivationValue().set(99.9);
		QVERIFY(n1.getLastActivation() == 0.0);
		QVERIFY(n1.getLastOutputActivation() == 0.0);
		n1.prepare();
		QVERIFY(n1.getLastActivation() == 0.1112);
		QVERIFY(n1.getLastOutputActivation() == 99.9);

		//reset
		n1.reset(); //called for all synapses, transferfunction and activationfunction.
		QVERIFY(n1.getLastActivation() == 0.0);
		QVERIFY(n1.getLastOutputActivation() == 0.0);
		QVERIFY(n1.getActivationValue().get() ==  0.0);
		QVERIFY(n1.getOutputActivationValue().get() ==  0.0);
		QVERIFY(tfan1->mResetCounter == 1);
		QVERIFY(afan1->mResetCounter == 1);
		QVERIFY(tfan1->mLastOwner == &n1);
		QVERIFY(afan1->mLastOwner == &n1);
		QVERIFY(sa1->mResetCounter == 1);
		QVERIFY(sa2->mResetCounter == 0);

		//activation update
		QVERIFY(tfan1->mTransferCounter == 0);
		QVERIFY(Math::compareDoubles(tfan1->mLastTransferFunctionInputActivation, 0.0, 5));
		QVERIFY(afan1->mCalculationCounter == 0);
		QVERIFY(sa1->mCalculationCounter == 0);
		afan1->mCalculationReturnValue = 24.6;
		tfan1->mTransferFunctionReturnValue = 77.58;
		tfan1->mUseOwnTransferFunctionReturnValue = true;
		
		n1.updateActivation();
		QVERIFY(tfan1->mTransferCounter == 1);
		QVERIFY(afan1->mCalculationCounter == 1);
		//not directly executed by neuron (=> activationfunction).
		QVERIFY(sa1->mCalculationCounter == 0); 
		//takes return value of activation function
		QVERIFY(Math::compareDoubles(n1.getActivationValue().get(), 24.6, 5)); 
		//takes transfer function return value
		QVERIFY(Math::compareDoubles(n1.getOutputActivationValue().get(), 77.58, 5));
		QVERIFY(Math::compareDoubles(tfan1->mLastTransferFunctionInputActivation, 24.6, 5));

		//a second activation update works only if the neuron was prepared() before.
		afan1->mCalculationReturnValue = 33.3;
		tfan1->mTransferFunctionReturnValue = 44.4;

		n1.updateActivation();
		//tranferfunction and activation function have NOT been called.
		QVERIFY(tfan1->mTransferCounter == 1);
		QVERIFY(afan1->mCalculationCounter == 1);
		//both activation values are NOT updated!
		QVERIFY(Math::compareDoubles(n1.getActivationValue().get(), 24.6, 5)); 
		QVERIFY(Math::compareDoubles(n1.getOutputActivationValue().get(), 77.58, 5));

		QVERIFY(n1.getLastActivation() == 0.0);
		QVERIFY(n1.getLastOutputActivation() == 0.0);
		//stores the last activations and allows the next updateActivation.
		n1.prepare();
		QVERIFY(n1.getLastActivation() == 24.6);
		QVERIFY(n1.getLastOutputActivation() == 77.58);
		//this time updateActivation works.
		n1.updateActivation();
		QVERIFY(tfan1->mTransferCounter == 2);
		QVERIFY(afan1->mCalculationCounter == 2);
		QVERIFY(Math::compareDoubles(n1.getActivationValue().get(), 33.3, 5)); 
		QVERIFY(Math::compareDoubles(n1.getOutputActivationValue().get(), 44.4, 5));

		delete sa2;
	}

	//both transferfuntion and activationfunction are deleted with the neuron n1.
	QVERIFY(destroyedTransferFunction == true);
	QVERIFY(destroyedActivationFunction == true);
	QVERIFY(destroyedSynapse == true);
}


//Chris
void TestNeuron::testDuplication() {

	Neuron sourceNeuron("Source", TransferFunctionTanh(), AdditiveTimeDiscreteActivationFunction());
	sourceNeuron.setId(15);

	Neuron neuron("Neuron1", TransferFunctionTanh(), AdditiveTimeDiscreteActivationFunction());
	neuron.getBiasValue().set(23.0);
	neuron.setId(50);

	Neuron *neuron2 = neuron.createCopy();

	QVERIFY(neuron.equals(0) == false);
	QVERIFY(neuron.equals(&neuron) == true);
	QVERIFY(neuron.equals(neuron2) == true);
	QVERIFY(neuron2->equals(&neuron) == true);

	//change id
	neuron.setId(1001);
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->setId(1001);
	QVERIFY(neuron.equals(neuron2) == true);

	//change name
	neuron.getNameValue().set("NewName");
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->getNameValue().set("NewName");
	QVERIFY(neuron.equals(neuron2) == true);

	//Change Bias
	neuron.getBiasValue().set(6.32);
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->getBiasValue().set(6.32);
	QVERIFY(neuron.equals(neuron2) == true);

	//Change TransferFunction
	neuron.getTransferFunction()->setName("SomethingElse");
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->getTransferFunction()->setName("SomethingElse");
	QVERIFY(neuron.equals(neuron2) == true);

	//Change ActivationFunction
	neuron.getActivationFunction()->setName("SomethingElse");
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->getActivationFunction()->setName("SomethingElse");
	QVERIFY(neuron.equals(neuron2) == true);

	//Add properties
	neuron.setProperty("Test");
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->setProperty("Test");
	QVERIFY(neuron.equals(neuron2) == true);
	neuron.setProperty("Name", "Value");
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->setProperty("Name", "WrongValue");
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->setProperty("Name", "Value");
	QVERIFY(neuron.equals(neuron2) == true);

	//Add synapses
	Synapse synapse1(&sourceNeuron, &neuron, 0.77, SimpleSynapseFunction());
	QVERIFY(neuron.addSynapse(synapse1.createCopy()));
	QVERIFY(neuron.equals(neuron2) == false);
	QVERIFY(neuron2->addSynapse(synapse1.createCopy()));
	QVERIFY(neuron.equals(neuron2) == true);
	
	//Change synapses
	neuron.getSynapses().at(0)->setId(55501);
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->getSynapses().at(0)->setId(55501);
	QVERIFY(neuron.equals(neuron2) == true);
	neuron.getSynapses().at(0)->getStrengthValue().set(432);
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->getSynapses().at(0)->getStrengthValue().set(432);
	QVERIFY(neuron.equals(neuron2) == true);

	//flip activation
	neuron.flipActivation(true);
	QVERIFY(neuron.equals(neuron2) == false);
	neuron2->flipActivation(true);
	QVERIFY(neuron.equals(neuron2) == true);


	delete neuron2;
}











