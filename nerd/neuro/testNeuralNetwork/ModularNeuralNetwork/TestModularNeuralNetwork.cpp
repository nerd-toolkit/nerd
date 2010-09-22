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




#include "TestModularNeuralNetwork.h"
#include <iostream>
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Constraints/GroupConstraintAdapter.h"
#include "ModularNeuralNetwork/NeuronGroupAdapter.h"
#include "Network/NeuronAdapter.h"
#include "Network/SynapseAdapter.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "ModularNeuralNetwork/NeuroModule.h"


using namespace std;
using namespace nerd;


void TestModularNeuralNetwork::testConstruction() {


	ModularNeuralNetwork network(ActivationFunctionAdapter("AF1"),
						TransferFunctionAdapter("AF1", -0.1, 0.1),
						SynapseFunctionAdapter("SF1"));

	QVERIFY(network.getNeurons().empty());
	QVERIFY(network.getNeuronGroups().size() == 1);
	QVERIFY(network.getNeuronGroup("Default") != 0);
	QVERIFY(network.getNeuroModules().empty());
	QList<NeuralNetworkElement*> elements;
	network.getNetworkElements(elements);
	QVERIFY(elements.size() == 1);
	QVERIFY(elements.at(0) == network.getNeuronGroup("Default"));
	QVERIFY(network.getControlInterface() == 0);
	QVERIFY(network.getSynapses().empty());

	QVERIFY(network.getDefaultTransferFunction() != 0);
	QVERIFY(dynamic_cast<TransferFunctionAdapter*>(network.getDefaultTransferFunction()) != 0);
	QVERIFY(network.getDefaultActivationFunction() != 0);
	QVERIFY(dynamic_cast<ActivationFunctionAdapter*>(
			network.getDefaultActivationFunction()) != 0);
	QVERIFY(network.getDefaultSynapseFunction() != 0);
	QVERIFY(dynamic_cast<SynapseFunctionAdapter*>(network.getDefaultSynapseFunction()) != 0);
	
}


void TestModularNeuralNetwork::testDuplicationAndEquals() {
	
	ModularNeuralNetwork net(ActivationFunctionAdapter("AF1"),
						TransferFunctionAdapter("AF1", -0.1, 0.1),
						SynapseFunctionAdapter("SF1"));

	Neuron *n1 = new Neuron("Neuron1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1001);
	Neuron *n2 = new Neuron("Neuron2", TransferFunctionAdapter("TF2", 0.0, 1.0), 
							ActivationFunctionAdapter("AF2"), 1002);
	Neuron *n3 = new Neuron("Neuron3", TransferFunctionAdapter("TF3", 0.0, 1.0), 
							ActivationFunctionAdapter("AF3"), 1003);
	Neuron *n4 = new Neuron("Neuron4", TransferFunctionAdapter("TF4", 0.0, 1.0), 
							ActivationFunctionAdapter("AF4"), 1004);
	Neuron *n5 = n4->createCopy();
	Neuron *n6 = new Neuron("Neuron6", TransferFunctionAdapter("TF4", 0.0, 1.0), 
							ActivationFunctionAdapter("AF4"), 1011);
	
	QVERIFY(net.addNeuron(n1));
	QVERIFY(net.addNeuron(n2));
	QVERIFY(net.addNeuron(n3));

	Synapse *s1 = Synapse::createSynapse(n1, n1, 0.5, SynapseFunctionAdapter("SF1"), 2001);
	Synapse *s2 = Synapse::createSynapse(n1, n2, 1.5, SynapseFunctionAdapter("SF1"), 2002);
	Synapse *s3 = Synapse::createSynapse(n2, n3, 2.5, SynapseFunctionAdapter("SF1"), 2003);
	Synapse *s4 = Synapse::createSynapse(n1, s1, 3.5, SynapseFunctionAdapter("SF1"), 2004);

	NeuronGroup *group1 = new NeuronGroup("Group1", 3001);
	NeuroModule *module1 = new NeuroModule("Module1", 4001);
	NeuroModule *subModule1 = new NeuroModule("SubModule1", 4010);
	module1->addSubModule(subModule1);
	subModule1->addNeuron(n6);

	GroupConstraint *constraint1 = new GroupConstraintAdapter("Constraint1", 0, 5001);
	GroupConstraint *constraint2 = new GroupConstraintAdapter("Constraint2", 0, 5002);

	module1->addNeuron(n1);
	module1->addNeuron(n2);
	module1->addConstraint(constraint1);
	QVERIFY(net.addNeuronGroup(module1));

	group1->addNeuron(n1);
	group1->addNeuron(n3);
	group1->addConstraint(constraint2);
	QVERIFY(net.addNeuronGroup(group1));

	QCOMPARE(net.getNeuronGroups().size(), 4);
	QVERIFY(net.getNeuronGroups().contains(group1));
	QVERIFY(net.getNeuronGroups().contains(module1));
	QVERIFY(net.getNeuronGroups().contains(subModule1));
	NeuronGroup *defaultGroup = net.getNeuronGroup("Default");
	QVERIFY(defaultGroup != 0);
	QVERIFY(net.getNeuronGroups().contains(defaultGroup));

	QCOMPARE(net.getNeuroModules().size(), 2);
	QVERIFY(net.getNeuroModules().contains(module1));
	QVERIFY(net.getNeuroModules().contains(subModule1));
	QVERIFY(net.getSubModuleOwner(subModule1) == module1);
	
	QCOMPARE(net.getNeurons().size(), 4);
	QVERIFY(net.getNeurons().contains(n1));
	QVERIFY(net.getNeurons().contains(n2));
	QVERIFY(net.getNeurons().contains(n3));
	QVERIFY(net.getNeurons().contains(n6));

	QCOMPARE(net.getSynapses().size(), 4);
	QVERIFY(net.getSynapses().contains(s1));
	QVERIFY(net.getSynapses().contains(s2));
	QVERIFY(net.getSynapses().contains(s3));
	QVERIFY(net.getSynapses().contains(s4));

	QCOMPARE(module1->getConstraints().size(), 1);
	QVERIFY(module1->getConstraints().contains(constraint1));
	QCOMPARE(group1->getConstraints().size(), 1);
	QVERIFY(group1->getConstraints().contains(constraint2));
	
	net.setProperty("NewProp", "Value");
	QVERIFY(net.getProperty("NewProp") == "Value");

	//------------------------------
	//create a copy

	ModularNeuralNetwork *copy = dynamic_cast<ModularNeuralNetwork*>(net.createCopy());

	QVERIFY(copy->getProperty("NewProp") == "Value");

	QCOMPARE(net.getNeuronGroups().size(), 4);
	NeuronGroup *cgroup1 = ModularNeuralNetwork::selectNeuronGroupById(group1->getId(), 
								copy->getNeuronGroups());
	NeuronGroup *cgroup2 = ModularNeuralNetwork::selectNeuronGroupById(module1->getId(), 
								copy->getNeuronGroups());
	NeuronGroup *cgroup3 = ModularNeuralNetwork::selectNeuronGroupById(subModule1->getId(), 
								copy->getNeuronGroups());
	NeuronGroup *cdefaultGroup = copy->getNeuronGroup("Default");

	QVERIFY(cgroup1 != 0 && cgroup2 != 0 && cdefaultGroup != 0 && cgroup3 != 0);
	QVERIFY(cgroup1 != group1 && cgroup2 != module1
			&& cdefaultGroup != defaultGroup && cgroup3 != subModule1);
	QVERIFY(copy->getNeuronGroups().contains(cgroup1));
	QVERIFY(copy->getNeuronGroups().contains(cgroup2));
	QVERIFY(copy->getNeuronGroups().contains(cgroup3));
	QVERIFY(copy->getNeuronGroups().contains(cdefaultGroup));

	QCOMPARE(copy->getNeuroModules().size(), 2);
	NeuroModule *cmodule1 = ModularNeuralNetwork::selectNeuroModuleById(module1->getId(), 
								copy->getNeuroModules());
	NeuroModule *csubModule1 = ModularNeuralNetwork::selectNeuroModuleById(subModule1->getId(), 
								copy->getNeuroModules());
	QVERIFY(cmodule1 != 0 && cmodule1 != module1);
	QVERIFY(csubModule1 != 0 && csubModule1 != subModule1);
	QVERIFY(copy->getNeuroModules().contains(cmodule1));
	QVERIFY(copy->getNeuroModules().contains(csubModule1));

	QVERIFY(copy->getSubModuleOwner(csubModule1) == cmodule1);
	
	QCOMPARE(copy->getNeurons().size(), 4);
	Neuron *cn1 = NeuralNetwork::selectNeuronById(n1->getId(), copy->getNeurons());
	Neuron *cn2 = NeuralNetwork::selectNeuronById(n2->getId(), copy->getNeurons());
	Neuron *cn3 = NeuralNetwork::selectNeuronById(n3->getId(), copy->getNeurons());
	Neuron *cn6 = NeuralNetwork::selectNeuronById(n6->getId(), copy->getNeurons());

	QVERIFY(cn1 != 0 && cn2 != 0 && cn3 != 0 && cn6 != 0);
	QVERIFY(cn1 != n1 && cn2 != n2 && cn3 != n3 && cn6 != n6);
	QVERIFY(copy->getNeurons().contains(cn1));
	QVERIFY(copy->getNeurons().contains(cn2));
	QVERIFY(copy->getNeurons().contains(cn3));
	QVERIFY(copy->getNeurons().contains(cn6));

	QCOMPARE(copy->getSynapses().size(), 4);
	Synapse *cs1 = NeuralNetwork::selectSynapseById(s1->getId(), copy->getSynapses());
	Synapse *cs2 = NeuralNetwork::selectSynapseById(s2->getId(), copy->getSynapses());
	Synapse *cs3 = NeuralNetwork::selectSynapseById(s3->getId(), copy->getSynapses());
	Synapse *cs4 = NeuralNetwork::selectSynapseById(s4->getId(), copy->getSynapses());
	QVERIFY(cs1 != 0 && cs2 != 0 && cs3 != 0 && cs4 != 0);
	QVERIFY(cs1 != s1 && cs2 != s2 && cs3 != s3 && cs4 != s4);
	QVERIFY(copy->getSynapses().contains(cs1));
	QVERIFY(copy->getSynapses().contains(cs2));
	QVERIFY(copy->getSynapses().contains(cs3));
	QVERIFY(copy->getSynapses().contains(cs4));

	QCOMPARE(cmodule1->getConstraints().size(), 1);
	GroupConstraint *cconstraint1 = cmodule1->getConstraints().at(0);
	QVERIFY(cconstraint1 != 0);
	QCOMPARE(cgroup1->getConstraints().size(), 1);
	GroupConstraint *cconstraint2 = cgroup1->getConstraints().at(0);
	QVERIFY(cconstraint2 != 0);

	//check component equality
	QVERIFY(cgroup1->equals(group1) == true);
	QVERIFY(cgroup2->equals(module1) == true);
	QVERIFY(defaultGroup->equals(cdefaultGroup) == true);
	QVERIFY(cmodule1->equals(module1) == true);
	QVERIFY(cn1->equals(n1) == true);
	QVERIFY(cn2->equals(n2) == true);
	QVERIFY(cn3->equals(n3) == true);
	QVERIFY(cs1->equals(s1) == true);
	QVERIFY(cs2->equals(s2) == true);
	QVERIFY(cs3->equals(s3) == true);
	QVERIFY(cs4->equals(s4) == true);
	QVERIFY(cconstraint1->equals(constraint1));
	QVERIFY(cconstraint2->equals(constraint2));


	//----------------------------
	//test network equality

	QVERIFY(net.equals(copy) == true);

	//change properties
	net.setProperty("TestProp", "Hallo");
	QVERIFY(net.equals(copy) == false);
	copy->setProperty("TestProp", "Hallo");
	QVERIFY(net.equals(copy) == true);

	//change neurons
	QVERIFY(net.addNeuron(n4));
	QVERIFY(net.equals(copy) == false);
	QVERIFY(copy->addNeuron(n5));
	QVERIFY(net.equals(copy) == true);

	//change synapses
	s1->getStrengthValue().set(11.11);
	QVERIFY(net.equals(copy) == false);
	cs1->getStrengthValue().set(11.11);
	QVERIFY(net.equals(copy) == true);

	s2->setTarget(n4);
	QVERIFY(net.equals(copy) == false);
	cs2->setTarget(n5);
	QVERIFY(net.equals(copy) == true);


	//change neurongroups
	group1->setName("AnotherName");
	QVERIFY(net.equals(copy) == false);
	cgroup1->setName("AnotherName");
	QVERIFY(net.equals(copy) == true);	

	QVERIFY(net.removeNeuronGroup(group1));
	QVERIFY(net.equals(copy) == false);
	QVERIFY(copy->removeNeuronGroup(cgroup1));
	QVERIFY(net.equals(copy) == true);

	QVERIFY(net.addNeuronGroup(group1));
	QVERIFY(net.equals(copy) == false);
	QVERIFY(copy->addNeuronGroup(cgroup1));
	QVERIFY(net.equals(copy) == true);

	//change constraints
	constraint1->setId(9999);
	QVERIFY(net.equals(copy) == false);
	cconstraint1->setId(9999);
	QVERIFY(net.equals(copy) == true);

	//change submodule
	module1->removeSubModule(subModule1);
	QVERIFY(net.equals(copy) == false);
	net.addNeuronGroup(subModule1);
	QVERIFY(net.equals(copy) == false);
	net.removeNeuronGroup(subModule1);
	module1->addSubModule(subModule1);
	QVERIFY(net.equals(copy) == true);

	//change parts of submodule
	subModule1->setName("SUB-Module");
	QVERIFY(net.equals(copy) == false);
	csubModule1->setName("SUB-Module");
	QVERIFY(net.equals(copy) == true);

	Neuron *n7 = new Neuron("Neuron6", TransferFunctionAdapter("TF4", 0.0, 1.0), 
							ActivationFunctionAdapter("AF4"), 1011);

	QVERIFY(subModule1->addNeuron(n7));
	QVERIFY(net.equals(copy) == false);
	QVERIFY(csubModule1->addNeuron(n7->createCopy()));
	QVERIFY(net.equals(copy) == true);

	delete copy;
}


void TestModularNeuralNetwork::testExecutionAndReset() {
	
}


void TestModularNeuralNetwork::testAddAndRemoveNeurons() {
	
}


void TestModularNeuralNetwork::testAddAndRemoveNeuroModulesAndGroups() {

	ModularNeuralNetwork net(ActivationFunctionAdapter("AF1"),
						TransferFunctionAdapter("AF1", -0.1, 0.1),
						SynapseFunctionAdapter("SF1"));

	Neuron *n1 = new Neuron("Neuron1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1001);
	Neuron *n2 = new Neuron("Neuron2", TransferFunctionAdapter("TF2", 0.0, 1.0), 
							ActivationFunctionAdapter("AF2"), 1002);
	Neuron *n3 = new Neuron("Neuron3", TransferFunctionAdapter("TF3", 0.0, 1.0), 
							ActivationFunctionAdapter("AF3"), 1003);
	Neuron *n4 = new Neuron("Neuron4", TransferFunctionAdapter("TF4", 0.0, 1.0), 
							ActivationFunctionAdapter("AF4"), 1004);
	//Neuron *n5 = n4->createCopy();
	
	QVERIFY(net.addNeuron(n1));
	QVERIFY(net.addNeuron(n2));
	QVERIFY(net.addNeuron(n3));

	//Synapse *s1 = Synapse::createSynapse(n1, n1, 0.5, SynapseFunctionAdapter("SF1"), 2001);
	//Synapse *s2 = Synapse::createSynapse(n1, n2, 1.5, SynapseFunctionAdapter("SF1"), 2002);
	//Synapse *s3 = Synapse::createSynapse(n2, n3, 2.5, SynapseFunctionAdapter("SF1"), 2003);
	//Synapse *s4 = Synapse::createSynapse(n1, s1, 3.5, SynapseFunctionAdapter("SF1"), 2004);

	//NeuronGroup *group1 = new NeuronGroup("Group1", 3001);
	NeuroModule *module1 = new NeuroModule("Module1", 4001);
	NeuroModule *module2 = new NeuroModule("Module2", 4001);
	QVERIFY(module1->addSubModule(module2));

	QVERIFY(module1->addNeuron(n1));
	QVERIFY(module1->addNeuron(n2));
	QVERIFY(module2->addNeuron(n3));
	QVERIFY(module2->addNeuron(n4));

	QVERIFY(net.addNeuronGroup(module1));

	QCOMPARE(net.getNeuronGroups().size(), 3);
	QVERIFY(net.getNeuronGroups().contains(module1));
	QVERIFY(net.getNeuronGroups().contains(module2));
	QVERIFY(net.getNeurons().contains(n4));

	//TODO not finished!
}


void TestModularNeuralNetwork::testSelectObjectsById() {

	
	NeuronGroup *group1 = new NeuronGroup("Group1", 100);
	NeuronGroup *group2 = new NeuronGroup("Group2", 110);

	NeuroModule *module1 = new NeuroModule("Module1", 200);
	NeuroModule *module2 = new NeuroModule("Module2", 215);

	GroupConstraint *constraint1 = new GroupConstraintAdapter("Constraint1", 0, 300);
	GroupConstraint *constraint2 = new GroupConstraintAdapter("Constraint2", 0, 399);

	QList<NeuronGroup*> groups;
	groups.append(group1);
	groups.append(group2);

	QList<NeuroModule*> modules;
	modules.append(module1);
	modules.append(module2);

	QList<GroupConstraint*> constraints;
	constraints.append(constraint1);
	constraints.append(constraint2);


	
	//NeuronGroup
	QVERIFY(ModularNeuralNetwork::selectNeuronGroupById(10101, groups) == 0);
	QVERIFY(ModularNeuralNetwork::selectNeuronGroupById(100, QList<NeuronGroup*>()) == 0);
	QVERIFY(ModularNeuralNetwork::selectNeuronGroupById(100, groups) == group1);
	group1->setId(155);
	QVERIFY(ModularNeuralNetwork::selectNeuronGroupById(100, groups) == 0);
	QVERIFY(ModularNeuralNetwork::selectNeuronGroupById(155, groups) == group1);

	QVERIFY(ModularNeuralNetwork::selectNeuronGroupById(110, groups) == group2);
	groups.removeAll(group2);
	QVERIFY(ModularNeuralNetwork::selectNeuronGroupById(110, groups) == 0);


	//NeuroModule
	QVERIFY(ModularNeuralNetwork::selectNeuroModuleById(201, modules) == 0);
	QVERIFY(ModularNeuralNetwork::selectNeuroModuleById(200, QList<NeuroModule*>()) == 0);
	QVERIFY(ModularNeuralNetwork::selectNeuroModuleById(200, modules) == module1);
	module1->setId(255);
	QVERIFY(ModularNeuralNetwork::selectNeuroModuleById(200, modules) == 0);
	QVERIFY(ModularNeuralNetwork::selectNeuroModuleById(255, modules) == module1);

	QVERIFY(ModularNeuralNetwork::selectNeuroModuleById(215, modules) == module2);
	modules.removeAll(module2);
	QVERIFY(ModularNeuralNetwork::selectNeuroModuleById(215, modules) == 0);


	//Constraints
	QVERIFY(ModularNeuralNetwork::selectConstraintById(301, constraints) == 0);
	QVERIFY(ModularNeuralNetwork::selectConstraintById(300, QList<GroupConstraint*>()) == 0);
	QVERIFY(ModularNeuralNetwork::selectConstraintById(300, constraints) == constraint1);
	constraint1->setId(355);
	QVERIFY(ModularNeuralNetwork::selectConstraintById(300, constraints) == 0);
	QVERIFY(ModularNeuralNetwork::selectConstraintById(355, constraints) == constraint1);

	QVERIFY(ModularNeuralNetwork::selectConstraintById(399, constraints) == constraint2);
	constraints.removeAll(constraint2);
	QVERIFY(ModularNeuralNetwork::selectConstraintById(399, constraints) == 0);

	delete group1;
	delete group2;
	delete module1;
	delete module2;
	delete constraint1;
	delete constraint2;
}




