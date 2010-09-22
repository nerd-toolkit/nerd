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



#include "TestNeuronGroup.h"
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
#include "ModularNeuralNetwork/NeuronGroupAdapter.h"


using namespace std;
using namespace nerd;


//Chris
void TestNeuronGroup::testConstruction() {
	Core::resetCore();

	NeuronGroup group("Group1", 1234);

	QCOMPARE(group.getId(), (qulonglong) 1234);
	QVERIFY(group.getOwnerNetwork() == 0);
	QVERIFY(group.getNeurons().size() == 0);
	QVERIFY(group.getSubModules().size() == 0);
	QVERIFY(group.getName() == "Group1");
	QVERIFY(group.getConstraints().size() == 0);

	NeuronGroup group2("Group2");
	QVERIFY(group2.getId() > 0); //auto id
}


//Chris
void TestNeuronGroup::testDuplicationAndEquals() {
	Core::resetCore();

	Neuron neuron1("Neuron1", TransferFunctionAdapter("TF1", 0.1, 1.2), ActivationFunctionAdapter("AF1"), 101);
	Neuron neuron2("Neuron2", TransferFunctionAdapter("TF1", 0.1, 1.2), ActivationFunctionAdapter("AF1"), 102);

	NeuroModule *module1 = new NeuroModule("Module1", 543);
	NeuroModule *module2 = new NeuroModule("Module2", 544);

	bool destroyC1 = false;

	GroupConstraintAdapter 	*constraint1 = new GroupConstraintAdapter("Constraint1", &destroyC1, 201);
	GroupConstraintAdapter 	*constraint2 = new GroupConstraintAdapter("Constraint1", 0, 202);

	ModularNeuralNetwork net(ActivationFunctionAdapter("AF1"), TransferFunctionAdapter("TF1", -0.5, 0.5),
								SynapseFunctionAdapter("SF1"));

	IntValue *testIntValue = new IntValue(101);

	{
		NeuronGroup group("Group1", 5432);
		QVERIFY(group.addSubModule(module1) == true);
		QVERIFY(group.addNeuron(&neuron1));
		QVERIFY(group.addNeuron(&neuron2));
		QVERIFY(group.addConstraint(constraint1));
		QVERIFY(group.addConstraint(constraint2));
		group.setOwnerNetwork(&net);
		group.addParameter("TestParam", testIntValue);
		group.setProperty("TestProperty", "Content");
		QCOMPARE(group.getPropertyNames().size(), 1);
		

		NeuronGroup *copy = group.createCopy();

		QCOMPARE(copy->getId(), (qulonglong) 5432);
		QVERIFY(copy->getOwnerNetwork() == 0);
		QVERIFY(copy->getSubModules().size() == 1);
		QVERIFY(copy->getSubModules().contains(module1) == true); //module is the same as in group.
		QVERIFY(copy->getNeurons().size() == 2);
		QVERIFY(copy->getName() == "Group1");
		QVERIFY(copy->getConstraints().size() == 2);
		QVERIFY(copy->getProperty("TestProperty") == "Content");
		IntValue *valueCopy = dynamic_cast<IntValue*>(copy->getParameter("TestParam"));
		QVERIFY(valueCopy != 0);
		QVERIFY(valueCopy->equals(testIntValue));
		QCOMPARE(copy->getPropertyNames().size(), 1); 

		QVERIFY(copy->getNeurons().contains(&neuron1) == true);
		QVERIFY(copy->getNeurons().contains(&neuron2) == true);
		QVERIFY(copy->getConstraints().contains(constraint1) == false);
		QVERIFY(copy->getConstraints().contains(constraint2) == false);

		GroupConstraint *constraintCopy = ModularNeuralNetwork::selectConstraintById(
							constraint1->getId(), copy->getConstraints());
		QVERIFY(constraintCopy != 0) ;
		QVERIFY(constraintCopy != constraint1);
		QVERIFY(constraintCopy->equals(constraint1));

		//test equals
		QVERIFY(group.equals(0) == false);
		QVERIFY(group.equals(copy) == true);
		QVERIFY(copy->equals(&group) == true);

		//change id
		group.setId(999);
		QVERIFY(group.equals(copy) == false);
		copy->setId(999);
		QVERIFY(group.equals(copy) == true);

		//change owner network (a difference here is NOT a problem)
		group.setOwnerNetwork(0);
		QVERIFY(group.equals(copy) == true);

		//change a neuron (is NOT a problem)
		neuron1.getBiasValue().set(0.01010);
		QVERIFY(group.equals(copy) == true);

		//remove a neuron
		QVERIFY(group.removeNeuron(&neuron1));
		QVERIFY(group.equals(copy) == false);
		QVERIFY(copy->removeNeuron(&neuron1));
		QVERIFY(group.equals(copy) == true);

		//add a neuron
		QVERIFY(group.addNeuron(&neuron1));
		QVERIFY(group.equals(copy) == false);
		QVERIFY(copy->addNeuron(&neuron1));
		QVERIFY(group.equals(copy) == true);

		//change an attribute of the submodule (is not a problem, because both use the same module object
		module1->setId(10101010);
		QVERIFY(group.equals(copy) == true);

		//add a second module (is a problem)
		group.addSubModule(module2);
		QVERIFY(group.equals(copy) == false);
		copy->addSubModule(module2);
		QVERIFY(group.equals(copy) == true);

		//instead of the same module pointers, also an equal module is valid.
		group.removeSubModule(module2);
		QVERIFY(group.equals(copy) == false);
		group.addSubModule(dynamic_cast<NeuroModule*>(module2->createCopy()));
		QVERIFY(group.equals(copy) == true);
		QVERIFY(group.getSubModules().at(1) != copy->getSubModules().at(1));

		//change a constraint (is a problem)
		constraint1->setId(9877);
		QVERIFY(group.equals(copy) == false);
		constraintCopy->setId(9877);
		QVERIFY(group.equals(copy) == true);

		constraint1->addParameter("Test", new IntValue(123));
		QVERIFY(group.equals(copy) == false);
		constraintCopy->addParameter("Test", new IntValue(55555));
		QVERIFY(group.equals(copy) == false);
		constraintCopy->getParameter("Test")->setValueFromString("123");
		QVERIFY(group.equals(copy) == true);

		//remove a constraint
		QVERIFY(group.removeConstraint(constraint1));
		QVERIFY(group.equals(copy) == false);
		QVERIFY(copy->removeConstraint(constraintCopy));
		QVERIFY(group.equals(copy) == true);
		
		//add constraint
		QVERIFY(group.addConstraint(constraint1));
		QVERIFY(group.equals(copy) == false);
		QVERIFY(copy->addConstraint(constraintCopy));
		QVERIFY(group.equals(copy) == true);

		delete copy;

	}

	QVERIFY(destroyC1 == true);
	
	net.removeNeuron(&neuron1);
	net.removeNeuron(&neuron2);
}


//Chris
void TestNeuronGroup::testAddAndRemoveNeurons() {
	Core::resetCore();

	Neuron neuron1("Neuron1", TransferFunctionAdapter("TF1", 0.1, 1.2), ActivationFunctionAdapter("AF1"), 101);
	Neuron neuron2("Neuron2", TransferFunctionAdapter("TF1", 0.1, 1.2), ActivationFunctionAdapter("AF1"), 102);

	ModularNeuralNetwork net(ActivationFunctionAdapter("AF1"), 
								TransferFunctionAdapter("TF1", -0.5, 0.5),
								SynapseFunctionAdapter("SF1"));

	NeuronGroup *otherGroup = new NeuronGroup("OtherGroup", 9119);
	otherGroup->addNeuron(&neuron1);
	net.addNeuronGroup(otherGroup);

	NeuronGroup group("Group1", 4242);
	

	QCOMPARE(group.getNeurons().size(), 0);

	//add neurons
	QVERIFY(group.addNeuron(0) == 0);
	QVERIFY(group.addNeuron(&neuron1) == true);
	QVERIFY(group.addNeuron(&neuron1) == false); //already contained.
	QCOMPARE(group.getNeurons().size(), 1);
	QVERIFY(group.getNeurons().contains(&neuron1));

	QVERIFY(group.addNeuron(&neuron2) == true);
	QCOMPARE(group.getNeurons().size(), 2);
	QVERIFY(group.getNeurons().contains(&neuron2));
	
	//remove neurons
	QVERIFY(group.removeNeuron(0) == false);
	QVERIFY(group.removeNeuron(&neuron1) == true);
	QVERIFY(group.removeNeuron(&neuron1) == false); //not contained any more
	
	//try to add neuron1, while the owner network is set and neuron1 is 
	//already used in another group (is not a problem for a NeuronGroup).
	group.setOwnerNetwork(&net);
	QVERIFY(group.addNeuron(&neuron1) == true);


	net.removeNeuron(&neuron1);
	net.removeNeuron(&neuron2);
}


//Chris
void TestNeuronGroup::testAddRemoveModules() {
	Core::resetCore();

	Neuron *neuron1 = new Neuron("Neuron1", TransferFunctionAdapter("TF1", 0.1, 1.2),
 						ActivationFunctionAdapter("AF1"), 101);
	Neuron *neuron2 = new Neuron("Neuron2", TransferFunctionAdapter("TF1", 0.1, 1.2),
 						ActivationFunctionAdapter("AF1"), 102);
	Neuron *neuron3 = new Neuron("Neuron3", TransferFunctionAdapter("TF1", 0.1, 1.2),
 						ActivationFunctionAdapter("AF1"), 102);
	NeuroModule *module1 = new NeuroModule("Module1", 5001);
	NeuroModule *module2 = new NeuroModule("Module2", 5002);
	NeuroModule *module3 = new NeuroModule("Module3", 5003);

	module1->addNeuron(neuron1);
	module2->addNeuron(neuron2);
	module3->addNeuron(neuron3);
	module2->addSubModule(module3);

	ModularNeuralNetwork *net = new ModularNeuralNetwork(ActivationFunctionAdapter("AF1"), 
								TransferFunctionAdapter("TF1", -0.5, 0.5),
								SynapseFunctionAdapter("SF1"));

	NeuronGroup *group = new NeuronGroup("Group1", 4242);
	
	QCOMPARE(group->getNeurons().size(), 0);
	QCOMPARE(group->getSubModules().size(), 0);

	net->addNeuronGroup(group);
	QCOMPARE(net->getNeuronGroups().size(), 2);
	QVERIFY(net->getNeuronGroups().contains(group));
	QVERIFY(net->getNeuronGroups().contains(net->getDefaultNeuronGroup()));
	
	
	//add submodules
	QVERIFY(group->addSubModule(0) == false);
	

	QVERIFY(group->addSubModule(module1) == true);
	QCOMPARE(group->getSubModules().size(), 1);
	QCOMPARE(group->getAllEnclosedModules().size(), 1);
	QCOMPARE(group->getNeurons().size(), 0);
	QCOMPARE(group->getAllEnclosedNeurons().size(), 1);	
	QVERIFY(group->getAllEnclosedNeurons().contains(neuron1));
	QCOMPARE(net->getNeuronGroups().size(), 3);
	QVERIFY(net->getNeuronGroups().contains(module1));

	QVERIFY(group->addSubModule(module1) == false); //already part of the group
	QCOMPARE(group->getSubModules().size(), 1);

	QVERIFY(group->addSubModule(module2) == true);
	QCOMPARE(group->getSubModules().size(), 2);
	QCOMPARE(group->getAllEnclosedModules().size(), 3);
	QCOMPARE(group->getNeurons().size(), 0);
	QCOMPARE(group->getAllEnclosedNeurons().size(), 3);	
	QVERIFY(group->getAllEnclosedNeurons().contains(neuron1));
	QVERIFY(group->getAllEnclosedNeurons().contains(neuron2));
	QVERIFY(group->getAllEnclosedNeurons().contains(neuron3));
	QCOMPARE(net->getNeuronGroups().size(), 5);
	QVERIFY(net->getNeuronGroups().contains(module2));
	QVERIFY(net->getNeuronGroups().contains(module3));

	QVERIFY(group->addNeuron(neuron3) == true);
	QCOMPARE(group->getNeurons().size(), 1);
	QCOMPARE(group->getAllEnclosedNeurons().size(), 3);

	QVERIFY(group->addSubModule(module3) == true);
	QCOMPARE(group->getSubModules().size(), 3);
	QCOMPARE(group->getAllEnclosedModules().size(), 3);
	QCOMPARE(group->getNeurons().size(), 1);
	QCOMPARE(group->getAllEnclosedNeurons().size(), 3);	
	QCOMPARE(net->getNeuronGroups().size(), 5);

	//remove groups
	QVERIFY(group->removeSubModule(0) == false);
	
	QVERIFY(group->removeSubModule(module2) == true);
	QCOMPARE(group->getSubModules().size(), 2);
	QCOMPARE(group->getAllEnclosedModules().size(), 2);
	QCOMPARE(group->getNeurons().size(), 1);
	QCOMPARE(group->getAllEnclosedNeurons().size(), 2);	
	QCOMPARE(net->getNeuronGroups().size(), 5); //modules are NOT automatically removed.

	QVERIFY(group->removeSubModule(module2) == false); //was not contained any more
	
	QVERIFY(group->removeSubModule(module3) == true);
	QCOMPARE(group->getSubModules().size(), 1);
	QCOMPARE(group->getAllEnclosedModules().size(), 1);
	QCOMPARE(group->getNeurons().size(), 1);
	QCOMPARE(group->getAllEnclosedNeurons().size(), 2);	
	QCOMPARE(net->getNeuronGroups().size(), 5);

	QVERIFY(group->removeSubModule(module1) == true);
	QCOMPARE(group->getSubModules().size(), 0);
	QCOMPARE(group->getAllEnclosedModules().size(), 0);
	QCOMPARE(group->getNeurons().size(), 1);
	QCOMPARE(group->getAllEnclosedNeurons().size(), 1);	
	QCOMPARE(net->getNeuronGroups().size(), 5);

	delete net;	
}

//Chris
void TestNeuronGroup::testAddAndRemoveConstraints() {
	Core::resetCore();

	GroupConstraintAdapter 	*constraint1 = new GroupConstraintAdapter("Constraint1", 0, 201);
	GroupConstraintAdapter 	*constraint2 = new GroupConstraintAdapter("Constraint1", 0, 202);

	ModularNeuralNetwork net(ActivationFunctionAdapter("AF1"), TransferFunctionAdapter("TF1", -0.5, 0.5),
								SynapseFunctionAdapter("SF1"));
	
	NeuronGroup *otherGroup = new NeuronGroup("OtherGroup", 9119);
	QVERIFY(otherGroup->addConstraint(constraint1) == true);
	net.addNeuronGroup(otherGroup);


	NeuronGroup group("Group1", 443);

	//add constraints
	QCOMPARE(group.getConstraints().size(), 0);

	QVERIFY(group.addConstraint(0) == false);
	QVERIFY(group.addConstraint(constraint1) == true);
	QVERIFY(group.addConstraint(constraint1) == false); //is already contained

	QCOMPARE(group.getConstraints().size(), 1);
	QVERIFY(group.getConstraints().contains(constraint1));

	QVERIFY(group.addConstraint(constraint2) == true);
	QCOMPARE(group.getConstraints().size(), 2);
	QVERIFY(group.getConstraints().contains(constraint2));

	//remove constraints
	QVERIFY(group.removeConstraint(0) == false);
	QVERIFY(group.removeConstraint(constraint1) == true);
	QCOMPARE(group.getConstraints().size(), 1);
	QVERIFY(group.getConstraints().contains(constraint2));

	//add constraint while another NeuronGroup of the ownerNetwork uses the same constraint object
	//(this will fail)
	net.addNeuronGroup(&group);
	QVERIFY(group.addConstraint(constraint1) == false);

	otherGroup->removeConstraint(constraint1);
	QVERIFY(group.addConstraint(constraint1) == true); //works because constraint1 is unused.

	group.removeConstraint(constraint1);
	otherGroup->addConstraint(constraint1);
	QVERIFY(group.addConstraint(constraint1) == false);
	net.removeNeuronGroup(otherGroup);
	QVERIFY(group.addConstraint(constraint1) == true); //works because otherGroup is not in the same net.


	group.removeConstraint(constraint1);
	net.addNeuronGroup(otherGroup);
	net.removeNeuronGroup(&group);
	
}


//Chris
void TestNeuronGroup::testMoveToNetwork() {
	Core::resetCore();

	Neuron *neuron1 = new Neuron("Neuron1", TransferFunctionAdapter("TF1", 0.1, 1.2), 
								ActivationFunctionAdapter("AF1"), 101);
	Neuron *neuron1copy = new Neuron("Neuron1Copy", TransferFunctionAdapter("TF2", -0.1, 9.0), 
								ActivationFunctionAdapter("AF2"), 101);
	Neuron *neuron2 = new Neuron("Neuron2", TransferFunctionAdapter("TF1", 0.1, 1.2), 
								ActivationFunctionAdapter("AF1"), 102);
	Neuron *neuron2copy = new Neuron("Neuron2Copy", TransferFunctionAdapter("TF3", 0.0, 2.0), 
								ActivationFunctionAdapter("AF3"), 102);

	GroupConstraintAdapter 	*constraint1 = new GroupConstraintAdapter("Constraint1", 0, 201);
	GroupConstraintAdapter 	*constraint2 = new GroupConstraintAdapter("Constraint1", 0, 202);

	bool destroyGroup = false;
	NeuronGroupAdapter *groupObject = new NeuronGroupAdapter("Group1", 123, &destroyGroup);
	NeuronGroupAdapter &group = *groupObject;
	group.addNeuron(neuron1);
	group.addNeuron(neuron2);
	group.addConstraint(constraint1);
	group.addConstraint(constraint2);

	{
		ModularNeuralNetwork net1(ActivationFunctionAdapter("AF1"), 
									TransferFunctionAdapter("TF1", -0.5, 0.5),
									SynapseFunctionAdapter("SF1"));
	
		ModularNeuralNetwork net2(ActivationFunctionAdapter("AF1"), 
									TransferFunctionAdapter("TF1", -0.5, 0.5),
									SynapseFunctionAdapter("SF1"));
	
		net1.addNeuron(neuron1);
		net1.addNeuron(neuron2);
		net2.addNeuron(neuron1copy);
		net2.addNeuron(neuron2copy);
	
		group.setOwnerNetwork(&net1);
	
		QVERIFY(constraint1->mLastOwner == 0);
		QCOMPARE(constraint1->mElementIdChangedCounter, 0);
		QVERIFY(constraint1->mElementIdChangedNewId == 0);
		QVERIFY(constraint1->mElementIdChangedOldId == 0);
	
		QCOMPARE(group.getNeurons().size(), 2);
		
		//move group
		QVERIFY(group.moveGroupToNetwork(&net1, &net2) == true);
	
		QCOMPARE(group.getNeurons().size(), 2);
		QVERIFY(group.getNeurons().contains(neuron1) == false);
		QVERIFY(group.getNeurons().contains(neuron2) == false);
		QVERIFY(group.getNeurons().contains(neuron1copy) == true); //changed neurons
		QVERIFY(group.getNeurons().contains(neuron2copy) == true);
	
		QVERIFY(group.getOwnerNetwork() == &net2); //changed network
	
		QVERIFY(constraint1->mLastOwner == &group);
		QVERIFY(constraint1->mElementIdChangedCounter == 2);
		QVERIFY(constraint1->mElementIdChangedOldId == 101 || constraint1->mElementIdChangedOldId == 102);
		QVERIFY(constraint1->mElementIdChangedNewId == 101 || constraint1->mElementIdChangedNewId == 102);
	
		
		QVERIFY(group.moveGroupToNetwork(&net1, &net2) == true);
		QVERIFY(constraint1->mElementIdChangedCounter == 2); //was not executed, because group was already moved.
	
		QVERIFY(group.moveGroupToNetwork(0, &net2) == false);
		QVERIFY(group.moveGroupToNetwork(&net1, 0) == false);
	
	}

	QVERIFY(destroyGroup == true);
}






