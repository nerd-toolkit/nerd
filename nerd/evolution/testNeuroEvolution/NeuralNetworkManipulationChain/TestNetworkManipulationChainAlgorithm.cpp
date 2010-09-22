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




#include "TestNetworkManipulationChainAlgorithm.h"
#include <iostream>
#include "Core/Core.h"
#include "NeuralNetworkManipulationChain/NeuralNetworkManipulationChainAlgorithm.h"
#include "Value/IntValue.h"
#include "NeuralNetworkManipulationChain/NeuralNetworkManipulationOperatorAdapter.h"
#include "Evolution/WorldAdapter.h"
#include "Evolution/PopulationAdapter.h"
#include "Network/NeuralNetwork.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "Evolution/IndividualAdapter.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "NeuralNetworkConstants.h"

using namespace std;
using namespace nerd;

void TestNetworkManipulationChainAlgorithm::initTestCase() {
}

void TestNetworkManipulationChainAlgorithm::cleanUpTestCase() {
}

//chris
void TestNetworkManipulationChainAlgorithm::testConstructor() {
	Core::resetCore();

	NeuralNetworkManipulationChainAlgorithm *cna = 
			new NeuralNetworkManipulationChainAlgorithm("TestName");

	QVERIFY(cna->getName() == "TestName");	
	QCOMPARE(cna->getOperators().size(), 0);
	QCOMPARE(cna->getRequiredNumberOfParentsPerIndividual(), 1);
	QVERIFY(cna->getOwnerWorld() == 0);
	
	IntValue *testInt = new IntValue(123);
	cna->addParameter("TestParam", testInt);
	QVERIFY(cna->getParameter("TestParam") != 0);

	bool destroyOp1 = false;
	NeuralNetworkManipulationOperatorAdapter *op1 = 	
			new NeuralNetworkManipulationOperatorAdapter("Op1", &destroyOp1);

	QCOMPARE(cna->getOperators().size(), 0);
	QVERIFY(cna->addOperator(op1));
	QCOMPARE(cna->getOperators().size(), 1);
	
	NeuralNetworkManipulationChainAlgorithm *copy =
 				dynamic_cast<NeuralNetworkManipulationChainAlgorithm*>(cna->createCopy());
	
	QVERIFY(copy != cna);
	QVERIFY(copy->getName() == "TestName");
	QCOMPARE(copy->getRequiredNumberOfParentsPerIndividual(), 1);
	QCOMPARE(copy->getOperators().size(), 1);
	
	NeuralNetworkManipulationOperator *op1c =
			dynamic_cast<NeuralNetworkManipulationOperatorAdapter*>(copy->getOperators().at(0));
	QVERIFY(op1c != 0);
	QVERIFY(op1c != op1);
	QVERIFY(op1c->getName() == "Op1");

	IntValue *testIntc = dynamic_cast<IntValue*>(copy->getParameter("TestParam"));	
	QVERIFY(testIntc != 0);	
	QCOMPARE(testIntc->get(), 123);

	delete copy;
	QVERIFY(destroyOp1 == false);
	delete cna;
	QVERIFY(destroyOp1 == true);
}


// Chris
void TestNetworkManipulationChainAlgorithm::testAddAndRemoveOperators() {
	Core::resetCore();

	ValueManager *vm = Core::getInstance()->getValueManager();

	NeuralNetworkManipulationChainAlgorithm *cna = 
			new NeuralNetworkManipulationChainAlgorithm("TestName");
	cna->setPrefix("TestPrefix/");

	NeuralNetworkManipulationOperatorAdapter *op1 = 	
			new NeuralNetworkManipulationOperatorAdapter("Op1", 0);
	NeuralNetworkManipulationOperatorAdapter *op2 = 	
			new NeuralNetworkManipulationOperatorAdapter("Op2", 0);

	QCOMPARE(cna->getOperators().size(), 0);

	IntValue *op1Param = new IntValue(123);
	op1->addParameter("Op1Param", op1Param);
	QVERIFY(op1->getPrefix() == "");
	QVERIFY(vm->getValues().contains(op1Param) == false);
	
	//add
	QVERIFY(cna->addOperator(0) == false); 
	QVERIFY(cna->addOperator(op1) == true);
	QVERIFY(cna->addOperator(op1) == false); //can not add twice

	QVERIFY(op1->getPrefix() == "TestPrefix/Op1/"); //prefix is set according to algorithm prefix.
	QVERIFY(vm->getValues().contains(op1Param) == true); //parameters are added
	QVERIFY(vm->getIntValue("TestPrefix/Op1/Op1Param") == op1Param);

	QCOMPARE(cna->getOperators().size(), 1);
	
	QVERIFY(cna->addOperator(op2) == true);
	QCOMPARE(cna->getOperators().size(), 2);
	QVERIFY(cna->getOperators().contains(op1) == true);
	QVERIFY(cna->getOperators().contains(op2) == true);

	//remove
	QVERIFY(cna->removeOperator(0) == false);
	QCOMPARE(cna->getOperators().size(), 2);

	QVERIFY(cna->removeOperator(op1) == true);
	QVERIFY(cna->removeOperator(op1) == false);
	QCOMPARE(cna->getOperators().size(), 1);
	QVERIFY(cna->getOperators().contains(op2) == true);
	QVERIFY(op1->getPrefix() == "TestPrefix/Op1/"); // is not changed by the algorithm
	QVERIFY(vm->getValues().contains(op1Param) == false); //parameters are removed
	QVERIFY(vm->getIntValue("TestPrefix/Op1/Op1Param") == 0);

	QVERIFY(cna->removeOperator(op2) == true);
	QVERIFY(cna->getOperators().empty());

	delete op1;
	delete op2;
	delete cna;

	
}


// Chris
void TestNetworkManipulationChainAlgorithm::testCreateNextGeneration() {
	Core::resetCore();

	NeuralNetworkManipulationChainAlgorithm *cna = 
			new NeuralNetworkManipulationChainAlgorithm("TestName");
	cna->setPrefix("TestPrefix/");

	NeuralNetworkManipulationOperatorAdapter *op1 = 	
			new NeuralNetworkManipulationOperatorAdapter("Op1", 0);
	NeuralNetworkManipulationOperatorAdapter *op2 = 	
			new NeuralNetworkManipulationOperatorAdapter("Op2", 0);
	cna->addOperator(op1);
	cna->addOperator(op2);
	op1->getMaximalNumberOfApplicationsValue()->set(100);
	op2->getMaximalNumberOfApplicationsValue()->set(100);

	WorldAdapter *world = new WorldAdapter("TestWorld");
	PopulationAdapter *pop1 = new PopulationAdapter("Population1");
	PopulationAdapter *pop2 = new PopulationAdapter("Population2");

	bool destroyInd1_1 = false;
	bool destroyInd1_2 = false;
	bool destroyInd2_1 = false;
	bool destroyInd2_2 = false;

	IndividualAdapter *ind1_1 = new IndividualAdapter(&destroyInd1_1);
	IndividualAdapter *ind1_2 = new IndividualAdapter(&destroyInd1_2);
	IndividualAdapter *ind2_1 = new IndividualAdapter(&destroyInd2_1);
	IndividualAdapter *ind2_2 = new IndividualAdapter(&destroyInd2_2);

	NeuralNetwork *network = new NeuralNetwork(AdditiveTimeDiscreteActivationFunction(), TransferFunctionTanh(),
												SimpleSynapseFunction());

	
	QList<Individual*> trash;
	QVERIFY(cna->createNextGeneration(trash) == false); //no world set
	
	world->setEvolutionAlgorithm(cna);
	QVERIFY(cna->getOwnerWorld() == world);

	QVERIFY(cna->createNextGeneration(trash) == true); //already works without any population
	
	world->addPopulation(pop1);
	QVERIFY(cna->createNextGeneration(trash) == true);

	pop1->getIndividuals().append(ind1_1);

	QCOMPARE(op1->mCountApplyOperator, 0);
	QCOMPARE(op2->mCountApplyOperator, 0);
	QVERIFY(cna->createNextGeneration(trash) == true); //an empty genome is no problem
	QCOMPARE(op1->mCountApplyOperator, 1);
	QCOMPARE(op2->mCountApplyOperator, 1);
	
	WorldAdapter *fakeWorld = new WorldAdapter("Dummy");
	ind1_1->setGenome(fakeWorld); //set a non NeuralNetwork as genome (is a problem)

	QCOMPARE(trash.size(), 0);
	QVERIFY(cna->createNextGeneration(trash) == true); //individual did not have a NeuralNetwork genome, so is removed
	QCOMPARE(trash.size(), 1);
	QVERIFY(trash.contains(ind1_1));
	delete ind1_1;

	QVERIFY(pop1->getIndividuals().empty());
	QCOMPARE(op1->mCountApplyOperator, 1);
	QCOMPARE(op2->mCountApplyOperator, 1);
	

	//add a valid individual
	
	NeuralNetwork *net1 = network->createCopy();

	destroyInd1_1 = false;
	ind1_1 = new IndividualAdapter(&destroyInd1_1);
	ind1_1->setGenome(net1);
	pop1->getIndividuals().append(ind1_1);
	
	op1->mCountApplyOperator = 0;
	op2->mCountApplyOperator = 0;

	QVERIFY(cna->createNextGeneration(trash) == true);
	QCOMPARE(pop1->getIndividuals().size(), 1);
	QCOMPARE(op1->mCountApplyOperator, 1);
	QCOMPARE(op2->mCountApplyOperator, 1);
	QVERIFY(op1->mLastIndividual == ind1_1);

	//if protected, then no operators are applied.
	ind1_1->protectGenome(true);
	QVERIFY(cna->createNextGeneration(trash) == true);
	QVERIFY(pop1->getIndividuals().size() == 1);
	QCOMPARE(op1->mCountApplyOperator, 1);
	QCOMPARE(op2->mCountApplyOperator, 1);


	ind1_2->setGenome(network->createCopy());
	pop1->getIndividuals().append(ind1_2);
	QVERIFY(pop1->getIndividuals().size() == 2);

	//ind1_1 is still protected, so the operators are called only once
	QVERIFY(cna->createNextGeneration(trash) == true);
	QVERIFY(pop1->getIndividuals().size() == 2);
	QCOMPARE(op1->mCountApplyOperator, 2);
	QCOMPARE(op2->mCountApplyOperator, 2);

	ind1_1->protectGenome(false);
	QVERIFY(pop1->getIndividuals().size() == 2);
	QCOMPARE(op1->mCountApplyOperator, 2);
	QCOMPARE(op2->mCountApplyOperator, 2);


	//test number of operator executions	

	op1->mCountApplyOperator = 0;
	op2->mCountApplyOperator = 0;
	op1->mNumberOfApplicationsUntilTrue = 50;
	op2->mNumberOfApplicationsUntilTrue = 25;

	QVERIFY(cna->createNextGeneration(trash) == true);
	QVERIFY(pop1->getIndividuals().size() == 2);
	QCOMPARE(op1->mCountApplyOperator, 52);
	QCOMPARE(op2->mCountApplyOperator, 52);

	
	//test: one operator returns false until its max executions expire.
	pop1->getIndividuals().removeAll(ind1_1);
	QVERIFY(pop1->getIndividuals().size() == 1);
	
	op1->getMaximalNumberOfApplicationsValue()->set(30);
	op1->getOperatorIndexValue()->set(10);
	op2->getMaximalNumberOfApplicationsValue()->set(100);
	op2->getOperatorIndexValue()->set(20);

	op1->mCountApplyOperator = 0;
	op2->mCountApplyOperator = 0;
	op1->mNumberOfApplicationsUntilTrue = 50;
	op2->mNumberOfApplicationsUntilTrue = 25;

	QVERIFY(cna->createNextGeneration(trash) == true);
	QVERIFY(pop1->getIndividuals().size() == 1);
	QCOMPARE(op1->mCountApplyOperator, 30);
	QCOMPARE(op2->mCountApplyOperator, 31); //executed until op1 is at max, then op2 alone returns true.

	//test: one operator is deactivated.
	op1->getMaximalNumberOfApplicationsValue()->set(30);
	op1->getOperatorIndexValue()->set(10);
	op2->getMaximalNumberOfApplicationsValue()->set(100);
	op2->getOperatorIndexValue()->set(20);

	op1->mCountApplyOperator = 0;
	op2->mCountApplyOperator = 0;
	op1->mNumberOfApplicationsUntilTrue = 50;
	op2->mNumberOfApplicationsUntilTrue = 25;
	
	op1->getEnableOperatorValue()->set(false);

	QVERIFY(cna->createNextGeneration(trash) == true);
	QVERIFY(pop1->getIndividuals().size() == 1);
	QCOMPARE(op1->mCountApplyOperator, 0);
	QCOMPARE(op2->mCountApplyOperator, 26);

	op1->getEnableOperatorValue()->set(true);
	
	
	//Test: if all operators return false, then the individual is destroyed.
	pop1->getIndividuals().append(ind1_1);

	op1->getMaximalNumberOfApplicationsValue()->set(20);
	op1->getOperatorIndexValue()->set(10);
	op2->getMaximalNumberOfApplicationsValue()->set(30);
	op2->getOperatorIndexValue()->set(20);

	op1->mCountApplyOperator = 0;
	op2->mCountApplyOperator = 0;
	op1->mNumberOfApplicationsUntilTrue = 50;
	op2->mNumberOfApplicationsUntilTrue = 50;

	trash.clear();
	QVERIFY(cna->createNextGeneration(trash) == true);
	QVERIFY(pop1->getIndividuals().size() == 1); //only the first individual survives
	QCOMPARE(op1->mCountApplyOperator, 40);
	QCOMPARE(op2->mCountApplyOperator, 51);

	QCOMPARE(trash.size(), 1);
	QVERIFY(trash.contains(ind1_2));
	delete ind1_2;

	QCOMPARE(pop1->getIndividuals().size(), 1);
	QVERIFY(pop1->getIndividuals().contains(ind1_1));

	//Test: use two populations
	world->addPopulation(pop2);
	pop2->getIndividuals().append(ind2_1);
	pop2->getIndividuals().append(ind2_2);
	ind2_1->setGenome(network->createCopy());
	ind2_2->setGenome(network->createCopy());
	QCOMPARE(pop2->getIndividuals().size(), 2);


	op1->getMaximalNumberOfApplicationsValue()->set(10);
	op1->getOperatorIndexValue()->set(10);
	op2->getMaximalNumberOfApplicationsValue()->set(10);
	op2->getOperatorIndexValue()->set(20);

	op1->mCountApplyOperator = 0;
	op2->mCountApplyOperator = 0;
	op1->mNumberOfApplicationsUntilTrue = 0;
	op2->mNumberOfApplicationsUntilTrue = 0;

	QVERIFY(cna->createNextGeneration(trash) == true);
	QCOMPARE(pop1->getIndividuals().size(), 1);
	QCOMPARE(pop2->getIndividuals().size(), 2);
	QCOMPARE(op1->mCountApplyOperator, 3);
	QCOMPARE(op2->mCountApplyOperator, 3);
	QVERIFY(op1->mLastIndividual == ind2_2);

	//Test: different order of operators
	op1->getOperatorIndexValue()->set(1);
	op2->getOperatorIndexValue()->set(2);  //change index order

	QVERIFY(cna->createNextGeneration(trash) == true);
	QVERIFY(op1->mApplicationOrderIdentifier < op2->mApplicationOrderIdentifier);

	op1->getOperatorIndexValue()->set(2);
	op2->getOperatorIndexValue()->set(1);  //change index order

	QVERIFY(cna->createNextGeneration(trash) == true);
	QVERIFY(op1->mApplicationOrderIdentifier > op2->mApplicationOrderIdentifier);

	//Test: strap modification marker
	ModularNeuralNetwork *net = new ModularNeuralNetwork(AdditiveTimeDiscreteActivationFunction(),
 									TransferFunctionTanh(),	SimpleSynapseFunction());
	Neuron *n1 = new Neuron("N1", TransferFunctionTanh(), AdditiveTimeDiscreteActivationFunction(), 101);
	Synapse *s1 = Synapse::createSynapse(n1, n1, 0.5, SimpleSynapseFunction(), 102);
	NeuroModule *nm1 = new NeuroModule("Module1", 103);
	
	QVERIFY(net->addNeuron(n1));
	QVERIFY(net->addNeuronGroup(nm1));

	QList<NeuralNetworkElement*> elements;
	net->getNetworkElements(elements);
	QCOMPARE(elements.size(), 4);

	n1->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
	s1->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);	
	nm1->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);

	delete ind1_1->getGenome();
	ind1_1->setGenome(net);

	QVERIFY(cna->createNextGeneration(trash) == true);
	QCOMPARE(pop1->getIndividuals().size(), 1);
	QCOMPARE(pop2->getIndividuals().size(), 2);
	QVERIFY(pop1->getIndividuals().contains(ind1_1));
	QVERIFY(n1->hasProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED) == false);
	QVERIFY(s1->hasProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED) == false);
	QVERIFY(nm1->hasProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED) == false);

	delete world;
	delete network;
}


