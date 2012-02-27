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




#include "TestSymmetryConstraint.h"
#include <iostream>
#include "Core/Core.h"
#include "Constraints/SymmetryConstraint.h"
#include "Network/Neuron.h"
#include "Network/Synapse.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "NeuralNetworkConstants.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"

#define QV QVERIFY
#define QC QCOMPARE

using namespace std;
using namespace nerd;

void TestSymmetryConstraint::initTestCase() {
}

void TestSymmetryConstraint::cleanUpTestCase() {
}

//chris
void TestSymmetryConstraint::testConstruction() {
	SymmetryConstraint *sc = new SymmetryConstraint();

	QVERIFY(sc->getName() == "Symmetry");
	QCOMPARE(sc->getParameters().size(), 4);
	QVERIFY(sc->getId() != 0);
	
	ULongLongValue *idValue = dynamic_cast<ULongLongValue*>(sc->getParameter("TargetId"));
	StringValue *referencesValue = dynamic_cast<StringValue*>(sc->getParameter("References"));
	StringValue *layoutValue = dynamic_cast<StringValue*>(sc->getParameter("Layout"));
	StringValue *modeValue = dynamic_cast<StringValue*>(sc->getParameter("Mode"));

	QVERIFY(idValue != 0);
	QVERIFY(referencesValue != 0);
	QVERIFY(layoutValue != 0);
	QVERIFY(modeValue != 0);

	QCOMPARE(idValue->get(), (qulonglong) 0);
	QVERIFY(referencesValue->get() == "");
	QVERIFY(layoutValue->get() == "");
	QVERIFY(modeValue->get() == "");

	SymmetryConstraint *copy = dynamic_cast<SymmetryConstraint*>(sc->createCopy());
	QVERIFY(copy != 0);
	QVERIFY(sc->getId() == copy->getId());

	ULongLongValue *idValue2 = dynamic_cast<ULongLongValue*>(copy->getParameter("TargetId"));
	StringValue *referencesValue2 = dynamic_cast<StringValue*>(copy->getParameter("References"));
	StringValue *layoutValue2 = dynamic_cast<StringValue*>(copy->getParameter("Layout"));
	StringValue *modeValue2 = dynamic_cast<StringValue*>(copy->getParameter("Mode"));

	QVERIFY(idValue2 != 0);
	QVERIFY(referencesValue2 != 0);
	QVERIFY(layoutValue2 != 0);
	QVERIFY(modeValue2 != 0);

	QVERIFY(idValue2 != idValue);
	QVERIFY(referencesValue2 != referencesValue);
	QVERIFY(layoutValue2 != layoutValue);
	QVERIFY(modeValue2 != modeValue);
	
	delete copy;
	delete sc;
}

//Chris
void TestSymmetryConstraint::testAttachDetach() {
	SymmetryConstraint *sc = new SymmetryConstraint();

	NeuronGroup *ref = new NeuronGroup("Group1", 101);

	NeuronGroup *group = new NeuronGroup("Group1", 101);
	NeuroModule *module = new NeuroModule("Module1", 201);
	NeuroModule *subModule = new NeuroModule("NM1", 202);


	Neuron *n1 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1001);
	Neuron *n2 = new Neuron("N2", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1002);
	Neuron *n3 = new Neuron("External", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1003);
	Synapse *s1 = Synapse::createSynapse(n1, n1, 0.1, SynapseFunctionAdapter("SF1"), 2001);
	Synapse *s2 = Synapse::createSynapse(n1, n2, 0.1, SynapseFunctionAdapter("SF1"), 2002);
	Synapse *s3 = Synapse::createSynapse(n2, n3, 0.1, SynapseFunctionAdapter("SF1"), 2003);	
	Synapse *s4 = Synapse::createSynapse(n3, n2, 0.1, SynapseFunctionAdapter("SF1"), 2004);

	subModule->addNeuron(n1);
	module->addNeuron(n2);
	module->addSubModule(subModule);
	
	group->addNeuron(n1);
	group->addNeuron(n2);

	QVERIFY(n1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(n2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(n3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s4->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(group->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(module->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(subModule->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);

	//attach to group
	QVERIFY(sc->attachToGroup(group) == true);

	QVERIFY(n1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(n2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(n3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s4->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(group->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(module->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(subModule->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);

	QList<NeuralNetworkElement*> elements;
	elements.append(ref);
	elements.append(group);
	sc->setRequiredElements(elements);

	QVERIFY(sc->attachToGroup(group) == true);

	QVERIFY(n1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	QVERIFY(n2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	QVERIFY(n3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	QVERIFY(s2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	QVERIFY(s3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s4->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(group->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	QVERIFY(module->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(subModule->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	
	QVERIFY(sc->detachFromGroup(group) == true);

	QVERIFY(n1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(n2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(n3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s4->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(group->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(module->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(subModule->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);

	QVERIFY(sc->attachToGroup(module) == true);

	QVERIFY(n1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	QVERIFY(n2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	QVERIFY(n3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	QVERIFY(s2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	QVERIFY(s3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s4->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(group->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(module->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	QVERIFY(subModule->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == true);
	
	QVERIFY(sc->detachFromGroup(module) == true);

	QVERIFY(n1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(n2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(n3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s1->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s2->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s3->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(s4->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(group->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(module->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);
	QVERIFY(subModule->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE) == false);

	//TODO delete
}


//chris
void TestSymmetryConstraint::testGetGroupOrigin() {
	SymmetryConstraint *sc = new SymmetryConstraint();

	NeuronGroup *refGroup = new NeuronGroup("GroupR1", 101);
	NeuronGroup *group = new NeuronGroup("Group1", 102);

	NeuroModule *refModule = new NeuroModule("ModuleR1", 200);
	NeuroModule *module = new NeuroModule("Module1", 201);
	NeuroModule *subModule = new NeuroModule("NM1", 202);


	Neuron *n1 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1001);
	Neuron *n2 = new Neuron("N2", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1002);
	Neuron *n3 = new Neuron("N3", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1003);

	
	group->addNeuron(n1);
	group->addNeuron(n2);
	module->addNeuron(n1);
	module->addSubModule(subModule);
	subModule->addNeuron(n2);
	refGroup->addNeuron(n3);

	ModularNeuralNetwork *net = new ModularNeuralNetwork();
	net->addNeuronGroup(group);
	net->addNeuronGroup(refGroup);
	net->addNeuronGroup(module);
	net->addNeuronGroup(refModule);
	net->addNeuronGroup(subModule);
	net->addNeuron(n1);
	net->addNeuron(n2);
	net->addNeuron(n3);

	module->setPosition(Vector3D(123.45, 6.78, 0.5));
	subModule->setPosition(Vector3D(100.0, 0.4, 0.1));
	refModule->setPosition(Vector3D(-45.1, -0.5, -0.2));
	n1->setPosition(Vector3D(50.0, 10.21, 0.1));
	n2->setPosition(Vector3D(-150.0, -15.21, -0.1));
	n3->setPosition(Vector3D(-55.0, 40.21, 0.066));

	Vector3D origin1;
	Vector3D origin2;

	sc->getParameter("TargetId")->setValueFromString("101");
	QList<NeuralNetworkElement*> elements;
	elements.append(refModule);
	elements.append(module);
	sc->setRequiredElements(elements);

	QVERIFY(sc->getGroupOrigins(origin1, origin2) == true);
	QCOMPARE(origin1, Vector3D(-45.1, -0.5, 0.0)); //z-axis is currently missing (only 2D is supoorted)
	QCOMPARE(origin2, Vector3D(123.45, 6.78, 0.0));
	
	elements.clear();
	elements.append(refGroup);
	elements.append(group);
	QVERIFY(sc->setRequiredElements(elements) == true);
	sc->getParameter("TargetId")->setValueFromString("200");
	refGroup->addConstraint(sc);

	sc->getParameter("References")->setValueFromString("1002,1003");

	sc->updateNetworkElementPairs(refGroup, group);
	QVERIFY(sc->getGroupOrigins(origin1, origin2) == true);
	QCOMPARE(origin1, Vector3D(-55.0, 40.21, 0.0)); //z-axis is currently missing (only 2D is supoorted)
	QCOMPARE(origin2, Vector3D(-150.0, -15.21, 0.0));
	
	delete refGroup;
	delete group;
	delete module;
	delete subModule;
	delete n1;
	delete n2;
	delete n3;
}


//Chris
void TestSymmetryConstraint::testSetElementPosition() {
	SymmetryConstraint *sc = new SymmetryConstraint();

	NeuronGroup *refGroup = new NeuronGroup("Group1", 501);
	NeuroModule *refModule = new NeuroModule("Module1", 502);

	NeuronGroup *group = new NeuronGroup("Group1", 102);
	NeuroModule *module = new NeuroModule("Module1", 201);
	NeuroModule *subModule = new NeuroModule("NM1", 202);
	Neuron *n1 = new Neuron("N1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1001);
	Synapse *s1 = Synapse::createSynapse(n1, n1, 0.1, SynapseFunctionAdapter("SF1"), 2001);

	module->addSubModule(subModule);

	double refModuleWidth = 100.5;
	double refModuleHeight = 200.7;
	double moduleWidth = 53.1;
	double moduleHeight = 99.1;
	double subModuleWidth = 40.1;
	double subModuleHeight = 60.5;

	refModule->setSize(QSizeF(refModuleWidth, refModuleHeight));
	module->setSize(QSizeF(moduleWidth, moduleHeight));
	subModule->setSize(QSizeF(subModuleWidth, subModuleHeight));

	QList<NeuralNetworkElement*> elements;
	elements.append(refGroup);
	elements.append(group);
	sc->setRequiredElements(elements);

	sc->updateFlipMode();

	sc->setElementPosition(group, Vector3D(1010.1, 2020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(group->getPosition() == Vector3D(1010.1 + 10.0, 2020.2 - 10.0, 0.0));
	sc->setElementPosition(module, Vector3D(2010.1, 3020.2, 0.0), Vector3D(10.0, 15.0, 0.0));
	QVERIFY(module->getPosition() == Vector3D(2010.1 + 10.0, 3020.2 + 15.0, 0.0));
	sc->setElementPosition(subModule, Vector3D(4010.1, 3020.2, 0.0), Vector3D(55.0, -44.4, 0.0));
	QVERIFY(subModule->getPosition() == Vector3D(4010.1 + 55.0, 3020.2 - 44.4, 0.0));
	sc->setElementPosition(n1, Vector3D(9010.1, 8020.2, 0.0), Vector3D(-10.0, -10.0, 0.0));
	QVERIFY(n1->getPosition() == Vector3D(9010.1 - 10.0, 8020.2 - 10.0, 0.0));
	sc->setElementPosition(s1, Vector3D(7010.1, 3020.2, 0.0), Vector3D(-99.9, -15.55, 0.0));
	QVERIFY(s1->getPosition() == Vector3D(7010.1 - 99.9, 3020.2 - 15.55, 0.0));

	sc->getParameter("Layout")->setValueFromString("h"); //flip horizontally
	sc->updateFlipMode();

	sc->setElementPosition(group, Vector3D(1010.1, 2020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(group->getPosition() == Vector3D(1010.1 - 10.0, 2020.2 - 10.0, 0.0));
	sc->setElementPosition(module, Vector3D(2010.1, 3020.2, 0.0), Vector3D(-10.0, 15.0, 0.0));
	QVERIFY(module->getPosition() == Vector3D(2010.1 + 10.0 - moduleWidth, 3020.2 + 15.0, 0.0));
	sc->setElementPosition(subModule, Vector3D(4010.1, 3020.2, 0.0), Vector3D(-55.0, -44.4, 0.0));
	QVERIFY(subModule->getPosition() == Vector3D(4010.1 + 55.0 - subModuleWidth, 3020.2 - 44.4, 0.0));
	sc->setElementPosition(n1, Vector3D(9010.1, 8020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(n1->getPosition() == Vector3D(9010.1 - 10.0, 8020.2 - 10.0, 0.0));
	sc->setElementPosition(s1, Vector3D(7010.1, 3020.2, 0.0), Vector3D(99.9, -15.55, 0.0));
	QVERIFY(s1->getPosition() == Vector3D(7010.1 - 99.9, 3020.2 - 15.55, 0.0));

	sc->getParameter("Layout")->setValueFromString("hv"); //flip horizontally and vertically
	sc->updateFlipMode();

	sc->setElementPosition(group, Vector3D(1010.1, 2020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(group->getPosition() == Vector3D(1010.1 - 10.0, 2020.2 + 10.0, 0.0));
	sc->setElementPosition(module, Vector3D(2010.1, 3020.2, 0.0), Vector3D(-10.0, 15.0, 0.0));
	QVERIFY(module->getPosition() == Vector3D(2010.1 + 10.0 - moduleWidth, 
							3020.2 - 15.0 - moduleHeight, 0.0));
	sc->setElementPosition(subModule, Vector3D(4010.1, 3020.2, 0.0), Vector3D(-55.0, 44.4, 0.0));
	QVERIFY(subModule->getPosition() == Vector3D(4010.1 + 55.0 - subModuleWidth, 
							3020.2 - 44.4 - subModuleHeight, 0.0));
	sc->setElementPosition(n1, Vector3D(9010.1, 8020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(n1->getPosition() == Vector3D(9010.1 - 10.0, 8020.2 + 10.0, 0.0));
	sc->setElementPosition(s1, Vector3D(7010.1, 3020.2, 0.0), Vector3D(99.9, -15.55, 0.0));
	QVERIFY(s1->getPosition() == Vector3D(7010.1 - 99.9, 3020.2 + 15.55, 0.0));

	sc->getParameter("Layout")->setValueFromString("v"); //flip vertically
	sc->updateFlipMode();

	sc->setElementPosition(group, Vector3D(1010.1, 2020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(group->getPosition() == Vector3D(1010.1 + 10.0, 2020.2 + 10.0, 0.0));
	sc->setElementPosition(module, Vector3D(2010.1, 3020.2, 0.0), Vector3D(-10.0, 15.0, 0.0));
	QVERIFY(module->getPosition() == Vector3D(2010.1 - 10.0, 3020.2 - 15.0 - moduleHeight, 0.0));
	sc->setElementPosition(subModule, Vector3D(4010.1, 3020.2, 0.0), Vector3D(-55.0, 44.4, 0.0));
	QVERIFY(subModule->getPosition() == Vector3D(4010.1 - 55.0, 3020.2 - 44.4 - subModuleHeight, 0.0));
	sc->setElementPosition(n1, Vector3D(9010.1, 8020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(n1->getPosition() == Vector3D(9010.1 + 10.0, 8020.2 + 10.0, 0.0));
	sc->setElementPosition(s1, Vector3D(7010.1, 3020.2, 0.0), Vector3D(99.9, -15.55, 0.0));
	QVERIFY(s1->getPosition() == Vector3D(7010.1 + 99.9, 3020.2 + 15.55, 0.0));



	//check with modules
	elements.clear();
	elements.append(module);
	elements.append(refModule);
	sc->setRequiredElements(elements);

	sc->getParameter("Layout")->setValueFromString(""); //no flip
	sc->updateFlipMode();
	
	sc->setElementPosition(group, Vector3D(1010.1, 2020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(group->getPosition() == Vector3D(1010.1 + 10.0, 2020.2 - 10.0, 0.0));
	sc->setElementPosition(module, Vector3D(2010.1, 3020.2, 0.0), Vector3D(10.0, 15.0, 0.0));
	QVERIFY(module->getPosition() == Vector3D(2010.1 + 10.0, 3020.2 + 15.0, 0.0));
	sc->setElementPosition(subModule, Vector3D(4010.1, 3020.2, 0.0), Vector3D(55.0, -44.4, 0.0));
	QVERIFY(subModule->getPosition() == Vector3D(4010.1 + 55.0, 3020.2 - 44.4, 0.0));
	sc->setElementPosition(n1, Vector3D(9010.1, 8020.2, 0.0), Vector3D(-10.0, -10.0, 0.0));
	QVERIFY(n1->getPosition() == Vector3D(9010.1 - 10.0, 8020.2 - 10.0, 0.0));
	sc->setElementPosition(s1, Vector3D(7010.1, 3020.2, 0.0), Vector3D(-99.9, -15.55, 0.0));
	QVERIFY(s1->getPosition() == Vector3D(7010.1 - 99.9, 3020.2 - 15.55, 0.0));


	sc->getParameter("Layout")->setValueFromString("h"); //flip horizontally
	sc->updateFlipMode();

	sc->setElementPosition(group, Vector3D(1010.1, 2020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(group->getPosition() == Vector3D(1010.1 - 10.0 + refModuleWidth, 2020.2 - 10.0, 0.0));
	sc->setElementPosition(module, Vector3D(2010.1, 3020.2, 0.0), Vector3D(-10.0, 15.0, 0.0));
	QVERIFY(module->getPosition() == Vector3D(2010.1 + 10.0 - moduleWidth + refModuleWidth, 
							3020.2 + 15.0, 0.0));
	sc->setElementPosition(subModule, Vector3D(4010.1, 3020.2, 0.0), Vector3D(-55.0, -44.4, 0.0));
	QVERIFY(subModule->getPosition() == Vector3D(4010.1 + 55.0 - subModuleWidth + refModuleWidth, 
							3020.2 - 44.4, 0.0));
	sc->setElementPosition(n1, Vector3D(9010.1, 8020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(n1->getPosition() == Vector3D(9010.1 - 10.0 + refModuleWidth, 8020.2 - 10.0, 0.0));
	sc->setElementPosition(s1, Vector3D(7010.1, 3020.2, 0.0), Vector3D(99.9, -15.55, 0.0));
	QVERIFY(s1->getPosition() == Vector3D(7010.1 - 99.9 + refModuleWidth, 3020.2 - 15.55, 0.0));

	sc->getParameter("Layout")->setValueFromString("hv"); //flip horizontally and vertically
	sc->updateFlipMode();

	sc->setElementPosition(group, Vector3D(1010.1, 2020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(group->getPosition() == Vector3D(1010.1 - 10.0 + refModuleWidth, 
							2020.2 + 10.0 + refModuleHeight, 0.0));
	sc->setElementPosition(module, Vector3D(2010.1, 3020.2, 0.0), Vector3D(-10.0, 15.0, 0.0));
	QVERIFY(module->getPosition() == Vector3D(2010.1 + 10.0 - moduleWidth + refModuleWidth, 
							3020.2 - 15.0 - moduleHeight + refModuleHeight, 0.0));
	sc->setElementPosition(subModule, Vector3D(4010.1, 3020.2, 0.0), Vector3D(-55.0, 44.4, 0.0));
	QVERIFY(subModule->getPosition() == Vector3D(4010.1 + 55.0 - subModuleWidth + refModuleWidth, 
							3020.2 - 44.4 - subModuleHeight + refModuleHeight, 0.0));
	sc->setElementPosition(n1, Vector3D(9010.1, 8020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(n1->getPosition() == Vector3D(9010.1 - 10.0 + refModuleWidth, 
							8020.2 + 10.0 + refModuleHeight, 0.0));
	sc->setElementPosition(s1, Vector3D(7010.1, 3020.2, 0.0), Vector3D(99.9, -15.55, 0.0));
	QVERIFY(s1->getPosition() == Vector3D(7010.1 - 99.9 + refModuleWidth, 	
							3020.2 + 15.55 + refModuleHeight, 0.0));

	sc->getParameter("Layout")->setValueFromString("v"); //flip vertically
	sc->updateFlipMode();

	sc->setElementPosition(group, Vector3D(1010.1, 2020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(group->getPosition() == Vector3D(1010.1 + 10.0, 
							2020.2 + 10.0 + refModuleHeight, 0.0));
	sc->setElementPosition(module, Vector3D(2010.1, 3020.2, 0.0), Vector3D(-10.0, 15.0, 0.0));
	QVERIFY(module->getPosition() == Vector3D(2010.1 - 10.0, 
							3020.2 - 15.0 - moduleHeight + refModuleHeight, 0.0));
	sc->setElementPosition(subModule, Vector3D(4010.1, 3020.2, 0.0), Vector3D(-55.0, 44.4, 0.0));
	QVERIFY(subModule->getPosition() == Vector3D(4010.1 - 55.0, 
							3020.2 - 44.4 - subModuleHeight + refModuleHeight, 0.0));
	sc->setElementPosition(n1, Vector3D(9010.1, 8020.2, 0.0), Vector3D(10.0, -10.0, 0.0));
	QVERIFY(n1->getPosition() == Vector3D(9010.1 + 10.0, 
							8020.2 + 10.0 + refModuleHeight, 0.0));
	sc->setElementPosition(s1, Vector3D(7010.1, 3020.2, 0.0), Vector3D(99.9, -15.55, 0.0));
	QVERIFY(s1->getPosition() == Vector3D(7010.1 + 99.9, 
							3020.2 + 15.55 + refModuleHeight, 0.0));
}


//Chris
void TestSymmetryConstraint::testModulePositionRecursively() {
	//TODO
// 	SymmetryConstraint *sc = new SymmetryConstraint();
// 
// 	NeuroModule *refModule = new NeuroModule("Module1", 101);
// 	NeuroModule *module = new NeuroModule("Module1", 201);
// 
// 	NeuroModule *sm_o1 = new NeuroModule("NM1", 301);
// 	NeuroModule *sm_r1 = new NeuroModule("NM1", 401);
// 	NeuroModule *sm_o2 = new NeuroModule("NM1", 302);
// 	NeuroModule *sm_r2 = new NeuroModule("NM1", 402);
// 	NeuroModule *sm_o3 = new NeuroModule("NM1", 303);
// 	NeuroModule *sm_r3 = new NeuroModule("NM1", 403);
// 	NeuroModule *sm_o4 = new NeuroModule("NM1", 304);
// 	NeuroModule *sm_r4 = new NeuroModule("NM1", 404);

}


//Chris
void TestSymmetryConstraint::testGetElementMatching() {
	SymmetryConstraint *sc = new SymmetryConstraint();

	ModularNeuralNetwork *net = new ModularNeuralNetwork();

	NeuroModule *refModule = new NeuroModule("Module1", 101);
	NeuroModule *module = new NeuroModule("Module1", 102);

	NeuroModule *sm_o1 = new NeuroModule("NM1", 301);
	NeuroModule *sm_o2 = new NeuroModule("NM1", 302);
	NeuroModule *sm_o3 = new NeuroModule("NM1", 303);
	NeuroModule *sm_r1 = new NeuroModule("NM1", 401);
	NeuroModule *sm_r2 = new NeuroModule("NM1", 402);
	NeuroModule *sm_r4 = new NeuroModule("NM1", 404);

	Neuron *n_o1 = new Neuron("No1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1001);
	Neuron *n_o2 = new Neuron("No2", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1002);
	Neuron *n_o3 = new Neuron("No3", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1003);
	Neuron *n_r1 = new Neuron("Nr1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1101);
	Neuron *n_r2 = new Neuron("Nr2", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1102);
	Neuron *n_r4 = new Neuron("Nr4", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1104);
	Neuron *external = new Neuron("N1", TransferFunctionAdapter("TF1", 0.0, 1.0), 
							ActivationFunctionAdapter("AF1"), 1201);

	Synapse *s_o1 = Synapse::createSynapse(n_o1, n_o1, 0.1, SynapseFunctionAdapter("SF1"), 2001);
	Synapse *s_o2 = Synapse::createSynapse(n_o1, n_o2, 0.1, SynapseFunctionAdapter("SF1"), 2002);
	Synapse *s_o3 = Synapse::createSynapse(external, n_o1, 0.1, SynapseFunctionAdapter("SF1"), 2003);
	Synapse *s_o4 = Synapse::createSynapse(n_o2, external, 0.1, SynapseFunctionAdapter("SF1"), 2004);
	Synapse *s_o5 = Synapse::createSynapse(n_o2, n_o2, 0.1, SynapseFunctionAdapter("SF1"), 2005);

	Synapse *s_r1 = Synapse::createSynapse(n_r1, n_r1, 0.1, SynapseFunctionAdapter("SF1"), 2101);
	Synapse *s_r2 = Synapse::createSynapse(n_r1, n_r2, 0.1, SynapseFunctionAdapter("SF1"), 2102);
	Synapse *s_r3 = Synapse::createSynapse(external, n_r1, 0.1, SynapseFunctionAdapter("SF1"), 2103);
	Synapse *s_r4 = Synapse::createSynapse(n_r2, external, 0.1, SynapseFunctionAdapter("SF1"), 2104);
	Synapse *s_r6 = Synapse::createSynapse(n_r2, n_r1, 0.1, SynapseFunctionAdapter("SF1"), 2106);

	sm_o1->addNeuron(n_o1);
	sm_o2->addNeuron(n_o2);
	module->addNeuron(n_o3);
	sm_o2->addSubModule(sm_o1);
	module->addSubModule(sm_o2);
	module->addSubModule(sm_o3);
	net->addNeuronGroup(module);
	net->addNeuronGroup(sm_o1);
	net->addNeuronGroup(sm_o2);

	sm_r1->addNeuron(n_r1);
	sm_r2->addNeuron(n_r2);
	refModule->addNeuron(n_r4);
	sm_r2->addSubModule(sm_r1);
	refModule->addSubModule(sm_r2);
	refModule->addSubModule(sm_r4);
	net->addNeuronGroup(refModule);
	net->addNeuronGroup(sm_r1);
	net->addNeuronGroup(sm_r2);

	net->addNeuron(n_o1);
	net->addNeuron(n_o2);
	net->addNeuron(n_o3);
	net->addNeuron(n_r1);
	net->addNeuron(n_r2);
	net->addNeuron(n_r4);
	net->addNeuron(external);

	refModule->addConstraint(sc);
	
	QList<NeuralNetworkElement*> elements;
	elements.append(refModule);
	elements.append(module);
	sc->setRequiredElements(elements);

	sc->getParameter("TargetId")->setValueFromString("102");
	sc->getParameter("References")->setValueFromString(
			"301,401|302,402|1001,1101|1002,1102|2001,2101|2002,2102|2003,2103|2004,2104");

	sc->updateNetworkElementPairs(refModule, module);


	//Check network element pairs.

	QList<NetworkElementPair> pairs = sc->getNetworkElementPairs();
	
	QCOMPARE(pairs.size(), 8);
	NetworkElementPair pair = pairs.at(0);
	QVERIFY(pair.mOwnerElement == n_r1);
	QVERIFY(pair.mReferenceElement == n_o1);

	pair = pairs.at(1);
	QVERIFY(pair.mOwnerElement == n_r2);
	QVERIFY(pair.mReferenceElement == n_o2);

	pair = pairs.at(2);
	QVERIFY(pair.mOwnerElement == n_r4);
	QVERIFY(pair.mReferenceElement == 0);

	pair = pairs.at(3);
	QVERIFY(pair.mOwnerElement == 0);
	QVERIFY(pair.mReferenceElement == n_o3);

	pair = pairs.at(4);
	QVERIFY(pair.mOwnerElement == sm_r1);
	QVERIFY(pair.mReferenceElement == sm_o1);

	pair = pairs.at(5);
	QVERIFY(pair.mOwnerElement == sm_r2);
	QVERIFY(pair.mReferenceElement == sm_o2);

	pair = pairs.at(6);
	QVERIFY(pair.mOwnerElement == sm_r4);
	QVERIFY(pair.mReferenceElement == 0);

	pair = pairs.at(7);
	QVERIFY(pair.mOwnerElement == 0);
	QVERIFY(pair.mReferenceElement == sm_o3);

	
	//test getMatching functions
	QList<Synapse*> allModuleSynapses = module->getAllEnclosedSynapses();
	QList<Synapse*> allRefModuleSynapses = refModule->getAllEnclosedSynapses();

	QVERIFY(sc->getMatchingNeuron(n_o1) == n_r1);
	QVERIFY(sc->getMatchingNeuron(n_o2) == n_r2);
	QVERIFY(sc->getMatchingNeuron(n_o3) == 0);
	QVERIFY(sc->getMatchingNeuron(n_r1) == n_o1);
	QVERIFY(sc->getMatchingNeuron(n_r2) == n_o2);
	QVERIFY(sc->getMatchingNeuron(n_r4) == 0);

	QVERIFY(sc->getMatchingModule(module) == refModule); 
	QVERIFY(sc->getMatchingModule(sm_o1) == sm_r1);
	QVERIFY(sc->getMatchingModule(sm_o2) == sm_r2);
	QVERIFY(sc->getMatchingModule(sm_o3) == 0);
	QVERIFY(sc->getMatchingModule(refModule) == module); 
	QVERIFY(sc->getMatchingModule(sm_r1) == sm_o1);
	QVERIFY(sc->getMatchingModule(sm_r2) == sm_o2);
	QVERIFY(sc->getMatchingModule(sm_r4) == 0);

	//check getMatchingSynapses.
	QVERIFY(sc->getMatchingSynapse(s_o1, 0) == s_r1);
	QVERIFY(sc->getMatchingSynapse(s_o2, 0) == s_r2);
	QVERIFY(sc->getMatchingSynapse(s_o3, 0) == 0);
	QVERIFY(sc->getMatchingSynapse(s_o3, SymmetryConstraint::CONNECTION_MODE_INPUT) == s_r3);
	QVERIFY(sc->getMatchingSynapse(s_o3, SymmetryConstraint::CONNECTION_MODE_OUTPUT) == 0);
	QVERIFY(sc->getMatchingSynapse(s_o3, SymmetryConstraint::CONNECTION_MODE_INPUT 
							| SymmetryConstraint::CONNECTION_MODE_OUTPUT) == s_r3);
	QVERIFY(sc->getMatchingSynapse(s_o4, 0) == 0);
	QVERIFY(sc->getMatchingSynapse(s_o4, SymmetryConstraint::CONNECTION_MODE_INPUT) == 0);
	QVERIFY(sc->getMatchingSynapse(s_o4, SymmetryConstraint::CONNECTION_MODE_OUTPUT) == s_r4);
	QVERIFY(sc->getMatchingSynapse(s_o4, SymmetryConstraint::CONNECTION_MODE_INPUT 
							| SymmetryConstraint::CONNECTION_MODE_OUTPUT) == s_r4);
	
	{
		Synapse *newSynapse = sc->getMatchingSynapse(s_o5, 0);
		QVERIFY(newSynapse != 0);
		QVERIFY(!allModuleSynapses.contains(newSynapse));
		QVERIFY(!allRefModuleSynapses.contains(newSynapse));
		QVERIFY(!newSynapse->equals(s_o5));
		QVERIFY(newSynapse->getSource() == n_r2); //n_r2 is partner neuron of n_o2 (which is used by s_o5)
		QVERIFY(newSynapse->getTarget() == n_r2);
	}

	QVERIFY(sc->getMatchingSynapse(s_r1, 0) == s_o1);
	QVERIFY(sc->getMatchingSynapse(s_r2, 0) == s_o2);
	QVERIFY(sc->getMatchingSynapse(s_r3, 0) == 0);
	QVERIFY(sc->getMatchingSynapse(s_r3, SymmetryConstraint::CONNECTION_MODE_INPUT) == s_o3);
	QVERIFY(sc->getMatchingSynapse(s_r3, SymmetryConstraint::CONNECTION_MODE_OUTPUT) == 0);
	QVERIFY(sc->getMatchingSynapse(s_r3, SymmetryConstraint::CONNECTION_MODE_INPUT 
							| SymmetryConstraint::CONNECTION_MODE_OUTPUT) == s_o3);
	QVERIFY(sc->getMatchingSynapse(s_r4, 0) == 0);
	QVERIFY(sc->getMatchingSynapse(s_r4, SymmetryConstraint::CONNECTION_MODE_INPUT) == 0);
	QVERIFY(sc->getMatchingSynapse(s_r4, SymmetryConstraint::CONNECTION_MODE_OUTPUT) == s_o4);
	QVERIFY(sc->getMatchingSynapse(s_r4, SymmetryConstraint::CONNECTION_MODE_INPUT 
							| SymmetryConstraint::CONNECTION_MODE_OUTPUT) == s_o4);
	
	{
		Synapse *newSynapse = sc->getMatchingSynapse(s_r6, 0);
		QVERIFY(newSynapse != 0);
		QVERIFY(!allModuleSynapses.contains(newSynapse));
		QVERIFY(!allRefModuleSynapses.contains(newSynapse));
		QVERIFY(!newSynapse->equals(s_r6));
		QVERIFY(newSynapse->getSource() == n_o2);
		QVERIFY(newSynapse->getTarget() == n_o1);
	}

	delete net;
}
