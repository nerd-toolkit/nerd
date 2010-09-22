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



#include "TestNeuroModule.h"
#include <iostream>
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "Constraints/GroupConstraint.h"
#include "Network/Neuron.h"
#include "Network/NeuralNetwork.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <QList>
#include <QListIterator>
#include "Constraints/GroupConstraintAdapter.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "Value/IntValue.h"
#include "Core/Core.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "NeuralNetworkConstants.h"

using namespace std;
using namespace nerd;


//Chris
void TestNeuroModule::testConstruction() {
	
	NeuroModule module("Module1", 654);

	QVERIFY(module.getId() == 654);
	QVERIFY(module.getName() == "Module1");
	//QVERIFY(module.hasProperty("ModuleName"));
	//QVERIFY(module.getProperty("ModuleName") == "Module1_654");
	QVERIFY(module.getInputNeurons().empty());
	QVERIFY(module.getOutputNeurons().empty());
	QVERIFY(module.getNeurons().empty());
	QVERIFY(module.getConstraints().empty());

	NeuroModule module2("Module2");
		
	QVERIFY(module.getId() > 0);
}


//Chris
void TestNeuroModule::testDuplicationAndEquals() {
	Core::resetCore();

	Neuron *neuron1 = new Neuron("Neuron1", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *neuron2 = new Neuron("Neuron2", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 102);
	Neuron *neuron3 = new Neuron("Neuron3", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 103);

	GroupConstraintAdapter 	*constraint1 = new GroupConstraintAdapter("Constraint1", 0, 201);
	GroupConstraintAdapter 	*constraint2 = new GroupConstraintAdapter("Constraint1", 0, 202);

	ModularNeuralNetwork *netObject = new ModularNeuralNetwork(
				ActivationFunctionAdapter("AF1"), TransferFunctionAdapter("TF1", -0.5, 0.5),
				SynapseFunctionAdapter("SF1"));
	ModularNeuralNetwork &net = *netObject;

	IntValue *testIntValue = new IntValue(101);
	
	NeuroModule *subModule = new NeuroModule("Submodule", 777);
	subModule->addConstraint(constraint2);
	subModule->addInputNeuron(neuron3);


	//create test module
	NeuroModule *module = new NeuroModule("Module", 393);
	QVERIFY(module->setOwnerNetwork(&net));
	QVERIFY(module->addInputNeuron(neuron1));
	QVERIFY(module->addOutputNeuron(neuron2));
	QVERIFY(module->addConstraint(constraint1));
	module->setProperty("TestProp", "Content");
	QVERIFY(module->addParameter("TestParam", testIntValue));
	QVERIFY(module->addSubModule(subModule));

	//create copy
	NeuroModule *copy = dynamic_cast<NeuroModule*>(module->createCopy());

	QVERIFY(copy->getId() == 393);
	QVERIFY(copy->getOwnerNetwork() == 0);
	QCOMPARE(copy->getNeurons().size(), 2);
	QVERIFY(copy->getName() == "Module");
	QVERIFY(copy->getConstraints().size() == 1);
	QVERIFY(copy->getProperty("TestProp") == "Content");
	IntValue *valueCopy = dynamic_cast<IntValue*>(copy->getParameter("TestParam"));
	QVERIFY(valueCopy != 0);
	QVERIFY(valueCopy->equals(testIntValue));

	QVERIFY(copy->getNeurons().contains(neuron1) == true);
	QVERIFY(copy->getNeurons().contains(neuron2) == true);
	QVERIFY(copy->getConstraints().contains(constraint1) == false);

	QCOMPARE(copy->getInputNeurons().size(), 1);
	QVERIFY(copy->getInputNeurons().contains(neuron1));
	QCOMPARE(copy->getOutputNeurons().size(), 1);
	QVERIFY(copy->getOutputNeurons().contains(neuron2));

	GroupConstraint *constraintCopy = ModularNeuralNetwork::selectConstraintById(
						constraint1->getId(), copy->getConstraints());
	QVERIFY(constraintCopy != 0) ;
	QVERIFY(constraintCopy != constraint1);
	QVERIFY(constraintCopy->equals(constraint1));

	QCOMPARE(copy->getSubModules().size(), 1);
	QVERIFY(copy->getSubModules().contains(subModule) == false);
	NeuroModule *subModuleCopy = copy->getSubModules().at(0);
	QVERIFY(subModuleCopy->equals(subModule) == true);


	//---------
	//Equals
	QVERIFY(module->equals(0) == false);
	QVERIFY(module->equals(copy) == true);
	QVERIFY(copy->equals(module) == true);

	//change id
	module->setId(999);
	QVERIFY(module->equals(copy) == false);
	copy->setId(999);
	QVERIFY(module->equals(copy) == true);

	//change owner network (a difference here is NOT a problem)
	module->setOwnerNetwork(0);
	QVERIFY(module->equals(copy) == true);

	//change a neuron (is NOT a problem)
	neuron1->getBiasValue().set(0.01010);
	QVERIFY(module->equals(copy) == true);

	//remove a neuron
	QVERIFY(module->removeNeuron(neuron1));
	QVERIFY(module->equals(copy) == false);
	QVERIFY(copy->removeNeuron(neuron1));
	QVERIFY(module->equals(copy) == true);

	//add a neuron
	neuron1->removeProperty(NeuralNetworkConstants::TAG_MODULE_INPUT);
	Neuron *neuron1Copy = neuron1->createCopy();
	QVERIFY(module->addNeuron(neuron1));
	QVERIFY(module->equals(copy) == false);
	QVERIFY(copy->addNeuron(neuron1Copy));
	QVERIFY(module->equals(copy) == true);

	Neuron *neuron2Copy = neuron2->createCopy();
	copy->removeNeuron(neuron2);
	copy->addNeuron(neuron2Copy);
	QVERIFY(copy->equals(module) == true);

	//change a constraint (is a problem)
	constraint1->setId(9877);
	QVERIFY(module->equals(copy) == false);
	constraintCopy->setId(9877);
	QVERIFY(module->equals(copy) == true);

	constraint1->addParameter("Test", new IntValue(123));
	QVERIFY(module->equals(copy) == false);
	constraintCopy->addParameter("Test", new IntValue(55555));
	QVERIFY(module->equals(copy) == false);
	constraintCopy->getParameter("Test")->setValueFromString("123");
	QVERIFY(module->equals(copy) == true);

	//remove a constraint
	QVERIFY(module->removeConstraint(constraint1));
	QVERIFY(module->equals(copy) == false);
	QVERIFY(copy->removeConstraint(constraintCopy));
	QVERIFY(module->equals(copy) == true);
	
	//add constraint
	QVERIFY(module->addConstraint(constraint1));
	QVERIFY(module->equals(copy) == false);
	QVERIFY(copy->addConstraint(constraintCopy));
	QVERIFY(module->equals(copy) == true);

	//remove / or add a submodule
	QVERIFY(module->removeSubModule(subModule) == true);
	QVERIFY(module->equals(copy) == false);
	QVERIFY(copy->removeSubModule(subModuleCopy) == true);
	QVERIFY(module->equals(copy) == true);
	
	QVERIFY(module->addSubModule(subModule) == true);
	QVERIFY(module->equals(copy) == false);
	QVERIFY(copy->addSubModule(subModuleCopy) == true);
	QVERIFY(module->equals(copy) == true);

	QVERIFY(module->getSubModules().contains(subModule) == true);
	QVERIFY(copy->getSubModules().contains(subModule) == false);

	//change a submodule
	subModule->setProperty("NewProp");
	QVERIFY(subModule->equals(subModuleCopy) == false);
	QVERIFY(module->equals(copy) == false);
	subModuleCopy->setProperty("NewProp");
	QVERIFY(module->equals(copy) == true);

	//change input / output neurons
	QVERIFY(neuron1->hasProperty(NeuralNetworkConstants::TAG_MODULE_INPUT) == false);
	QVERIFY(neuron1Copy->hasProperty(NeuralNetworkConstants::TAG_MODULE_INPUT) == false);
	QVERIFY(module->addInputNeuron(neuron1) == true);
	QVERIFY(neuron1->hasProperty(NeuralNetworkConstants::TAG_MODULE_INPUT) == true);
	QVERIFY(neuron1Copy->hasProperty(NeuralNetworkConstants::TAG_MODULE_INPUT) == false);
	QVERIFY(module->equals(copy) == false);
	QVERIFY(copy->addInputNeuron(neuron1Copy) == true);
	QVERIFY(module->equals(copy) == true);

	QVERIFY(module->removeOutputNeuron(neuron2) == true);
	QVERIFY(module->equals(copy) == false);
	QVERIFY(copy->removeOutputNeuron(neuron2Copy) == true);
	QVERIFY(module->equals(copy) == true);


	delete netObject;
	delete module;
	delete copy;

	delete subModuleCopy;
}


//Chris
void TestNeuroModule::testAddAndRemoveNeuron() {
	Core::resetCore();

	Neuron *neuron1p = new Neuron("Neuron1", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *neuron2p = new Neuron("Neuron2", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 102);

	//to adapt to changes
	Neuron &neuron1 = *neuron1p;
	Neuron &neuron2 = *neuron2p;

	ModularNeuralNetwork *netObj = new ModularNeuralNetwork(ActivationFunctionAdapter("AF1"), 
								TransferFunctionAdapter("TF1", -0.5, 0.5),
								SynapseFunctionAdapter("SF1"));
	ModularNeuralNetwork &net = *netObj;

	NeuroModule *otherModule = new NeuroModule("OtherModule", 8118);
	QVERIFY(otherModule->addNeuron(&neuron1));
	QVERIFY(net.addNeuronGroup(otherModule));

	NeuroModule *moduleObj = new NeuroModule("Module1", 4242);
	NeuroModule &module = *moduleObj;

	QCOMPARE(module.getNeurons().size(), 0);
	QCOMPARE(net.getNeurons().size(), 1);

	//add neurons
	QVERIFY(module.addNeuron(0) == 0);
	QVERIFY(module.addNeuron(&neuron1) == true);
	QVERIFY(module.addNeuron(&neuron1) == false); //already contained.
	QCOMPARE(module.getNeurons().size(), 1);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QCOMPARE(net.getNeurons().size(), 1); //still only the neuron form the otherModule.

	QVERIFY(module.addNeuron(&neuron2) == true);
	QCOMPARE(module.getNeurons().size(), 2);
	QVERIFY(module.getNeurons().contains(&neuron2));
	QCOMPARE(net.getNeurons().size(), 1);
	
	//remove neurons
	QVERIFY(module.removeNeuron(0) == false);
	QVERIFY(module.removeNeuron(&neuron1) == true);
	QVERIFY(module.removeNeuron(&neuron1) == false); //not contained any more
	QCOMPARE(net.getNeurons().size(), 1);

	

	//---------------------
	//try to add neuron1, while the owner network is set and neuron1 is 
	//already used in another module (this IS a problem for a Neuronmodule).
	QCOMPARE(net.getNeurons().size(), 1);
	QVERIFY(net.addNeuronGroup(&module));
	QCOMPARE(net.getNeurons().size(), 2);
	QVERIFY(net.getNeurons().contains(&neuron2));
	QVERIFY(net.getNeurons().contains(&neuron1));	

	QVERIFY(module.addNeuron(&neuron1) == false);

	QVERIFY(otherModule->removeNeuron(&neuron1) == true);
	QCOMPARE(otherModule->getNeurons().size(), 0);
	QCOMPARE(net.getNeurons().size(), 2); //neuron is not removed from network, only from module
	QVERIFY(net.getNeurons().contains(&neuron2));
	QVERIFY(module.addNeuron(&neuron1) == true); //now it works.
	QCOMPARE(net.getNeurons().size(), 2);
	QVERIFY(net.getNeurons().contains(&neuron2));
	QVERIFY(net.getNeurons().contains(&neuron1));

	
	//Check input and output neurons
	QVERIFY(module.getInputNeurons().empty());
	QVERIFY(module.getOutputNeurons().empty());

	QVERIFY(module.removeInputNeuron(&neuron1) == true);
	QVERIFY(module.getNeurons().size() == 2);
	QVERIFY(neuron1.hasProperty(NeuralNetworkConstants::TAG_MODULE_INPUT) == false);

	net.removeNeuron(&neuron1);
	net.removeNeuron(&neuron2);

	QVERIFY(module.getNeurons().size() == 0); //neurons are deleted from modules when deleted from net.
	QCOMPARE(net.getNeurons().size(), 0);

	//-------------------------
	//add input neurons
	QVERIFY(module.addInputNeuron(0) == false);

	QVERIFY(module.addInputNeuron(&neuron1) == true);
	QVERIFY(module.getNeurons().size() == 1);
	QVERIFY(module.getInputNeurons().size() == 1);
	QVERIFY(module.getOutputNeurons().size() == 0);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QVERIFY(module.getInputNeurons().contains(&neuron1));
	QCOMPARE(net.getNeurons().size(), 1);

	//add a member neuron to the input neurons
	QVERIFY(module.addNeuron(&neuron2) == true);
	QVERIFY(module.addNeuron(&neuron2) == false); //was already part of the module
	QVERIFY(module.getNeurons().size() == 2);
	QVERIFY(module.getInputNeurons().size() == 1);
	QVERIFY(module.getOutputNeurons().size() == 0);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QVERIFY(module.getNeurons().contains(&neuron2));
	QCOMPARE(net.getNeurons().size(), 2);

	QVERIFY(module.addInputNeuron(&neuron2) == true);
	QVERIFY(module.getNeurons().size() == 2);
	QVERIFY(module.getInputNeurons().size() == 2);
	QVERIFY(module.getOutputNeurons().size() == 0);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QVERIFY(module.getNeurons().contains(&neuron2));
	QVERIFY(module.getInputNeurons().contains(&neuron1));
	QVERIFY(module.getInputNeurons().contains(&neuron2));

	//remove input neurons
	QVERIFY(module.removeInputNeuron(0) == false);
	QVERIFY(module.getNeurons().size() == 2);
	QVERIFY(module.getInputNeurons().size() == 2);

	QVERIFY(module.removeInputNeuron(&neuron1) == true);
	QVERIFY(module.removeInputNeuron(&neuron1) == true); //also returns true, although neuron1 was no input.
	QVERIFY(module.getNeurons().size() == 2);	//only removed as input neuron, but still in the module.
	QVERIFY(module.getInputNeurons().size() == 1);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QVERIFY(module.getNeurons().contains(&neuron2));
	QVERIFY(module.getInputNeurons().contains(&neuron2));
	QCOMPARE(net.getNeurons().size(), 2);
	
	QVERIFY(module.removeNeuron(&neuron2) == true); //delete a neuron from the module
	QVERIFY(module.getNeurons().size() == 1);	//only removed as input neuron, but still in the module.
	QVERIFY(module.getInputNeurons().size() == 0);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QCOMPARE(net.getNeurons().size(), 2);
	
	QVERIFY(module.removeNeuron(&neuron1) == true); 
	QVERIFY(module.getNeurons().size() == 0);
	QVERIFY(module.getInputNeurons().size() == 0);
	QCOMPARE(net.getNeurons().size(), 2);

	net.removeNeuron(&neuron1);	
	net.removeNeuron(&neuron2);
	QCOMPARE(net.getNeurons().size(), 0);

	//-------------------------
	//add output neurons
	QVERIFY(module.addOutputNeuron(0) == false);

	QVERIFY(module.addOutputNeuron(&neuron1) == true);
	QVERIFY(module.getNeurons().size() == 1);
	QVERIFY(module.getOutputNeurons().size() == 1);
	QVERIFY(module.getInputNeurons().size() == 0);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QVERIFY(module.getOutputNeurons().contains(&neuron1));
	QCOMPARE(net.getNeurons().size(), 1);

	//add a member neuron to the Output neurons
	neuron2.removeProperty(NeuralNetworkConstants::TAG_MODULE_INPUT);
	QVERIFY(module.addNeuron(&neuron2) == true);
	QVERIFY(module.addNeuron(&neuron2) == false); //was already part of the module
	QVERIFY(module.getNeurons().size() == 2);
	QVERIFY(module.getOutputNeurons().size() == 1);
	QVERIFY(module.getInputNeurons().size() == 0);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QVERIFY(module.getNeurons().contains(&neuron2));
	QCOMPARE(net.getNeurons().size(), 2);

	QVERIFY(module.addOutputNeuron(&neuron2) == true);
	QVERIFY(module.getNeurons().size() == 2);
	QVERIFY(module.getOutputNeurons().size() == 2);
	QVERIFY(module.getInputNeurons().size() == 0);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QVERIFY(module.getNeurons().contains(&neuron2));
	QVERIFY(module.getOutputNeurons().contains(&neuron1));
	QVERIFY(module.getOutputNeurons().contains(&neuron2));

	//remove Output neurons
	QVERIFY(module.removeOutputNeuron(0) == false);
	QVERIFY(module.getNeurons().size() == 2);
	QVERIFY(module.getOutputNeurons().size() == 2);

	QVERIFY(module.removeOutputNeuron(&neuron1) == true);
	QVERIFY(module.removeOutputNeuron(&neuron1) == true); //still ok, although neuron1 is not an output.
	QVERIFY(module.getNeurons().size() == 2);	//only removed as Output neuron, but still in the module.
	QVERIFY(module.getOutputNeurons().size() == 1);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QVERIFY(module.getNeurons().contains(&neuron2));
	QVERIFY(module.getOutputNeurons().contains(&neuron2));
	QCOMPARE(net.getNeurons().size(), 2);
	
	QVERIFY(module.removeNeuron(&neuron2) == true); //delete a neuron from the module
	QVERIFY(module.getNeurons().size() == 1);	//only removed as Output neuron, but still in the module.
	QVERIFY(module.getOutputNeurons().size() == 0);
	QVERIFY(module.getNeurons().contains(&neuron1));
	QCOMPARE(net.getNeurons().size(), 2);
	
	QVERIFY(module.removeNeuron(&neuron1) == true); 
	QVERIFY(module.getNeurons().size() == 0);
	QVERIFY(module.getOutputNeurons().size() == 0);
	QCOMPARE(net.getNeurons().size(), 2);

	net.removeNeuron(&neuron1);
	net.removeNeuron(&neuron2);
	QCOMPARE(net.getNeurons().size(), 0);

	//---------------------------
	//Add input and output

	QVERIFY(module.addInputNeuron(&neuron1) == true);
	QVERIFY(module.addOutputNeuron(&neuron1) == true);
	QVERIFY(module.getNeurons().size() == 1);
	QVERIFY(module.getOutputNeurons().size() == 1);
	QVERIFY(module.getInputNeurons().size() == 1);
	QCOMPARE(net.getNeurons().size(), 1);


	delete netObj;
	delete neuron2p;
}


//Chris
void TestNeuroModule::testAddAndRemoveSubModules() {
	Core::resetCore();

	Neuron *neuron1p = new Neuron("Neuron1", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *neuron2p = new Neuron("Neuron2", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 102);
	Neuron *neuron3p = new Neuron("Neuron3", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 103);
	Neuron *neuron4p = new Neuron("Neuron4", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 104);

	Neuron &neuron1 = *neuron1p;
	Neuron &neuron2 = *neuron2p;
	Neuron &neuron3 = *neuron3p;
	Neuron &neuron4 = *neuron4p;

// 	GroupConstraintAdapter 	*constraint1 = new GroupConstraintAdapter("Constraint1", 0, 201);
	GroupConstraintAdapter 	*constraint2 = new GroupConstraintAdapter("Constraint1", 0, 202);

	ModularNeuralNetwork net(ActivationFunctionAdapter("AF1"), TransferFunctionAdapter("TF1", -0.5, 0.5),
								SynapseFunctionAdapter("SF1"));

	QCOMPARE(net.getNeurons().size(), 0);

// 	IntValue *testIntValue = new IntValue(101);

	NeuroModule *module1 = new NeuroModule("Module1", 776);	
	module1->addInputNeuron(&neuron3);

	NeuroModule *subModule1 = new NeuroModule("Submodule1", 777);
	subModule1->addConstraint(constraint2);
	subModule1->addInputNeuron(&neuron3);

	NeuroModule *subModule2 = new NeuroModule("Submodule2", 778);
	subModule2->addInputNeuron(&neuron4);


	//----------------------
	//create test module
	NeuroModule *module = new NeuroModule("Module", 393);
	QVERIFY(net.addNeuronGroup(module));
	
	QCOMPARE(module->getNeurons().size(), 0);
	QCOMPARE(module->getAllEnclosedNeurons().size(), 0);

	//add a submodule (NULL)
	QVERIFY(module->addSubModule(0) == false);

	//add a submodule that uses a neuron already in the module.
	QVERIFY(module->addNeuron(&neuron3));
	QCOMPARE(net.getNeurons().size(), 1);
	QVERIFY(module->addSubModule(subModule1) == false); //neuron3 is already in module.

	QVERIFY(module->removeNeuron(&neuron3));
	QCOMPARE(net.getNeurons().size(), 1);

	//add a submodule that uses a neuron already in another module
	QVERIFY(net.addNeuronGroup(module1));
	QVERIFY(module->addSubModule(subModule1) == false);

	//add a submodule with no neuron conflict
	QVERIFY(net.removeNeuronGroup(module1));
	QVERIFY(module->addSubModule(subModule1) == true);

	QCOMPARE(module->getNeurons().size(), 0);
	QCOMPARE(module->getAllEnclosedNeurons().size(), 1);
	QVERIFY(subModule1->getOwnerNetwork() == &net); //is now part of the network
	QCOMPARE(net.getNeurons().size(), 1);
	

	//add the same module again
	QVERIFY(module->addSubModule(subModule1) == false); //is alreday contained.
	
	//-----------
	//add a subsubmodule
	QVERIFY(module->addSubModule(subModule2) == true);
	QCOMPARE(module->getNeurons().size(), 0);
	QCOMPARE(module->getAllEnclosedNeurons().size(), 2);
	QVERIFY(subModule1->addSubModule(subModule1) == false); //can not add itself
	QVERIFY(subModule1->addSubModule(subModule2) == false); //subModule2 is already contained.
	QCOMPARE(net.getNeurons().size(), 2);

	QVERIFY(module->removeSubModule(subModule2) == true);
	QVERIFY(subModule1->addSubModule(subModule2) == true);
	QCOMPARE(module->getAllEnclosedNeurons().size(), 2);
	QCOMPARE(net.getNeurons().size(), 2);
	
	QCOMPARE(module->getNeurons().size(), 0);
	QCOMPARE(subModule1->getNeurons().size(), 1);
	QCOMPARE(subModule1->getAllEnclosedNeurons().size(), 2);
	QCOMPARE(module->getAllEnclosedNeurons().size(), 2);
	QCOMPARE(net.getNeurons().size(), 2);

	//remove and add a module with submodules (neurons af all (sub)modules are present
	QVERIFY(net.removeNeuronGroup(module) == true);
	QCOMPARE(net.getNeurons().size(), 0);
	QVERIFY(net.addNeuronGroup(module) == true);
	QCOMPARE(net.getNeurons().size(), 2);

	//remove networks from neural net to prevent double deletion
	net.removeNeuron(&neuron1);
	net.removeNeuron(&neuron2);
	net.removeNeuron(&neuron3);
	net.removeNeuron(&neuron4);

	QCOMPARE(module->getNeurons().size(), 0);	
	QCOMPARE(subModule1->getNeurons().size(), 0);
	QCOMPARE(subModule2->getNeurons().size(), 0);

	delete neuron1p;
	delete neuron2p;
	delete neuron3p;
	delete neuron4p;
	delete module1;
}

//Chris
void TestNeuroModule::testGetInputOutputNeurons() {
	Neuron *n1 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *n2 = new Neuron("N2", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *n3 = new Neuron("N3", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *n4 = new Neuron("N4", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);

	NeuroModule *module1 = new NeuroModule("Module1");
	NeuroModule *module2 = new NeuroModule("Module2");
	NeuroModule *module3 = new NeuroModule("Module3");
	NeuroModule *module4 = new NeuroModule("Module4");

	module3->addInputNeuron(n1);
	module2->addInputNeuron(n2);
	module1->addInputNeuron(n3);
	module4->addInputNeuron(n4);

	module3->addOutputNeuron(n1);
	module2->addOutputNeuron(n2);
	module1->addOutputNeuron(n3);
	module4->addOutputNeuron(n4);
	
	module1->addSubModule(module2);
	module2->addSubModule(module3);
	module2->addSubModule(module4);


	//Test Input Neurons

	QList<Neuron*> inputNeurons = module3->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 1);
	QVERIFY(inputNeurons.contains(n1));

	inputNeurons = module2->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 1);
	QVERIFY(inputNeurons.contains(n2));

	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 1);
	QVERIFY(inputNeurons.contains(n3));  

	n2->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE, "1");
	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 2);
	QVERIFY(inputNeurons.contains(n3));
	QVERIFY(inputNeurons.contains(n2));

	n1->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 3);
	QVERIFY(inputNeurons.contains(n3));
	QVERIFY(inputNeurons.contains(n2));
	QVERIFY(inputNeurons.contains(n1));

	n1->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE, "2");
	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 3);
	QVERIFY(inputNeurons.contains(n3));
	QVERIFY(inputNeurons.contains(n2));
	QVERIFY(inputNeurons.contains(n1));

	n1->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE, "1");
	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 2);
	QVERIFY(inputNeurons.contains(n3));
	QVERIFY(inputNeurons.contains(n2)); 

	inputNeurons = module2->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 2);
	QVERIFY(inputNeurons.contains(n2));
	QVERIFY(inputNeurons.contains(n1)); 

	n2->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE, "0");
	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 1);
	QVERIFY(inputNeurons.contains(n3)); 

	n4->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 2);
	QVERIFY(inputNeurons.contains(n3)); 
	QVERIFY(inputNeurons.contains(n4)); 

	n1->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	n2->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 4);
	QVERIFY(inputNeurons.contains(n3)); 
	QVERIFY(inputNeurons.contains(n4)); 
	QVERIFY(inputNeurons.contains(n1)); 
	QVERIFY(inputNeurons.contains(n2)); 

	n4->removeProperty(NeuralNetworkConstants::TAG_MODULE_INPUT);
	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 3);
	QVERIFY(inputNeurons.contains(n3)); 
	QVERIFY(inputNeurons.contains(n1)); 
	QVERIFY(inputNeurons.contains(n2)); 

	n2->removeProperty(NeuralNetworkConstants::TAG_MODULE_INPUT);
	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 2);
	QVERIFY(inputNeurons.contains(n3)); 
	QVERIFY(inputNeurons.contains(n1)); 

	n1->removeProperty(NeuralNetworkConstants::TAG_MODULE_INPUT);
	inputNeurons = module1->getInputNeurons();
	QCOMPARE(inputNeurons.size(), 1);
	QVERIFY(inputNeurons.contains(n3)); 


	//Test Output Neurons
	n1->removeProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	n2->removeProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	n3->removeProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	n4->removeProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	
	QList<Neuron*> outputNeurons = module3->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 1);
	QVERIFY(outputNeurons.contains(n1));

	outputNeurons = module2->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 1);
	QVERIFY(outputNeurons.contains(n2));

	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 1);
	QVERIFY(outputNeurons.contains(n3));  

	n2->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE, "1");
	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 2);
	QVERIFY(outputNeurons.contains(n3));
	QVERIFY(outputNeurons.contains(n2));

	n1->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 3);
	QVERIFY(outputNeurons.contains(n3));
	QVERIFY(outputNeurons.contains(n2));
	QVERIFY(outputNeurons.contains(n1));

	n1->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE, "2");
	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 3);
	QVERIFY(outputNeurons.contains(n3));
	QVERIFY(outputNeurons.contains(n2));
	QVERIFY(outputNeurons.contains(n1));

	n1->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE, "1");
	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 2);
	QVERIFY(outputNeurons.contains(n3)); 
	QVERIFY(outputNeurons.contains(n2)); 

	outputNeurons = module2->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 2);
	QVERIFY(outputNeurons.contains(n2));
	QVERIFY(outputNeurons.contains(n1)); 

	n2->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE, "0");
	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 1);
	QVERIFY(outputNeurons.contains(n3)); 

	n4->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 2);
	QVERIFY(outputNeurons.contains(n3)); 
	QVERIFY(outputNeurons.contains(n4)); 

	n1->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	n2->setProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 4);
	QVERIFY(outputNeurons.contains(n3)); 
	QVERIFY(outputNeurons.contains(n4)); 
	QVERIFY(outputNeurons.contains(n1)); 
	QVERIFY(outputNeurons.contains(n2)); 

	n4->removeProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT);
	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 3);
	QVERIFY(outputNeurons.contains(n3)); 
	QVERIFY(outputNeurons.contains(n1)); 
	QVERIFY(outputNeurons.contains(n2)); 

	n2->removeProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT);
	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 2);
	QVERIFY(outputNeurons.contains(n3)); 
	QVERIFY(outputNeurons.contains(n1)); 

	n1->removeProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT);
	outputNeurons = module1->getOutputNeurons();
	QCOMPARE(outputNeurons.size(), 1);
	QVERIFY(outputNeurons.contains(n3)); 

}


//Chris
void TestNeuroModule::testReplaceNeuron() {
	Neuron *n1 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *n2 = new Neuron("N2", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *n3 = new Neuron("N3", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *n4 = new Neuron("N4", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *n5 = new Neuron("N5", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);
	Neuron *replic = new Neuron("N5", TransferFunctionAdapter("TF1", 0.1, 1.2), 
						ActivationFunctionAdapter("AF1"), 101);

	NeuroModule *module1 = new NeuroModule("Module1");
	NeuroModule *module2 = new NeuroModule("Module2");
	NeuroModule *module3 = new NeuroModule("Module3");

	module1->addSubModule(module2);
	module2->addSubModule(module3);

	module1->addNeuron(n1);
	module2->addNeuron(n4);
	module3->addNeuron(n3);
	module2->addNeuron(n2);
	//n5 is not in one of the modules here.

	Synapse *s1 = Synapse::createSynapse(n1, n4, 0.5, SynapseFunctionAdapter("SF1"));
	Synapse *s2 = Synapse::createSynapse(n4, n1, 0.5, SynapseFunctionAdapter("SF1"));
	Synapse *s3 = Synapse::createSynapse(n4, n4, 0.5, SynapseFunctionAdapter("SF1"));
	Synapse *s4 = Synapse::createSynapse(n2, n4, 0.5, SynapseFunctionAdapter("SF1"));
	Synapse *s5 = Synapse::createSynapse(n4, n2, 0.5, SynapseFunctionAdapter("SF1"));
	Synapse *s6 = Synapse::createSynapse(n3, n4, 0.5, SynapseFunctionAdapter("SF1"));
	Synapse *s7 = Synapse::createSynapse(n4, n3, 0.5, SynapseFunctionAdapter("SF1"));
	Synapse *s8 = Synapse::createSynapse(n5, n4, 0.5, SynapseFunctionAdapter("SF1"));
	Synapse *s9 = Synapse::createSynapse(n4, n5, 0.5, SynapseFunctionAdapter("SF1"));

	QCOMPARE(module1->getAllEnclosedNeurons().at(1), n4);

	QVERIFY(s1->getSource() == n1);
	QVERIFY(s1->getTarget() == n4);
	QVERIFY(s2->getSource() == n4);
	QVERIFY(s2->getTarget() == n1);
	QVERIFY(s3->getSource() == n4);
	QVERIFY(s3->getTarget() == n4);
	QVERIFY(s4->getSource() == n2);
	QVERIFY(s4->getTarget() == n4);
	QVERIFY(s5->getSource() == n4);
	QVERIFY(s5->getTarget() == n2);
	QVERIFY(s6->getSource() == n3);
	QVERIFY(s6->getTarget() == n4);
	QVERIFY(s7->getSource() == n4);
	QVERIFY(s7->getTarget() == n3);
	QVERIFY(s8->getSource() == n5);
	QVERIFY(s8->getTarget() == n4);
	QVERIFY(s9->getSource() == n4);
	QVERIFY(s9->getTarget() == n5);

	QVERIFY(module1->replaceNeuron(0, 0) == false);
	QVERIFY(module1->replaceNeuron(n4, 0) == false);
	QVERIFY(module1->replaceNeuron(0, replic) == false);
	QVERIFY(module3->replaceNeuron(n1, replic) == false); //n1 is not in module3 or submodules

	QVERIFY(module1->replaceNeuron(n4, n4) == true); //no chnages, but valid.
	
	QVERIFY(s1->getSource() == n1);
	QVERIFY(s1->getTarget() == n4);
	QVERIFY(s2->getSource() == n4);
	QVERIFY(s2->getTarget() == n1);
	QVERIFY(s3->getSource() == n4);
	QVERIFY(s3->getTarget() == n4);
	QVERIFY(s4->getSource() == n2);
	QVERIFY(s4->getTarget() == n4);
	QVERIFY(s5->getSource() == n4);
	QVERIFY(s5->getTarget() == n2);
	QVERIFY(s6->getSource() == n3);
	QVERIFY(s6->getTarget() == n4);
	QVERIFY(s7->getSource() == n4);
	QVERIFY(s7->getTarget() == n3);
	QVERIFY(s8->getSource() == n5);
	QVERIFY(s8->getTarget() == n4);
	QVERIFY(s9->getSource() == n4);
	QVERIFY(s9->getTarget() == n5);

	QVERIFY(module1->replaceNeuron(n4, n3) == false); //n3 is already in the module.

	QVERIFY(module1->replaceNeuron(n4, replic) == true); //no chages, but valid.

	QVERIFY(s1->getSource() == n1);
	QVERIFY(s1->getTarget() == replic);
	QVERIFY(s2->getSource() == replic);
	QVERIFY(s2->getTarget() == n1);
	QVERIFY(s3->getSource() == replic);
	QVERIFY(s3->getTarget() == replic);
	QVERIFY(s4->getSource() == n2);
	QVERIFY(s4->getTarget() == replic);
	QVERIFY(s5->getSource() == replic);
	QVERIFY(s5->getTarget() == n2);
	QVERIFY(s6->getSource() == n3);
	QVERIFY(s6->getTarget() == replic);
	QVERIFY(s7->getSource() == replic);
	QVERIFY(s7->getTarget() == n3);
	QVERIFY(s8->getSource() == n5);
	QVERIFY(s8->getTarget() == replic);
	QVERIFY(s9->getSource() == replic);
	QVERIFY(s9->getTarget() == n5);

	//order was preserved
	QCOMPARE(module1->getAllEnclosedNeurons().at(1), replic);

	//still in the same module
	QVERIFY(module2->getNeurons().contains(replic));
}




