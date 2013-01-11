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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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




#include "TestNeuralNetworkIONerdV1Xml.h"
#include <iostream>
#include "Network/Neuron.h"
#include "Network/Neuro.h"
#include "Network/Synapse.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Constraints/GroupConstraint.h"
#include "Constraints/NumberOfNeuronsConstraint.h"
#include "TransferFunction/TransferFunctionRamp.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "Core/Core.h"
#include "IO/NeuralNetworkIONerdV1Xml.h"
#include <QFile>
#include <QIODevice>
#include "Collections/StandardNeuralNetworkFunctions.h"
#include "Collections/StandardConstraintCollection.h"
#include "Network/NeuralNetworkManager.h"

using namespace std;
using namespace nerd;


//Chris
void TestNeuralNetworkIONerdV1Xml::testCreateXmlFromNetwork() {
	
	Core::resetCore();
	NeuralNetwork::resetIdCounter();

	//construct network
	AdditiveTimeDiscreteActivationFunction defaultAf;
	TransferFunctionRamp defaultTf("ramp[0,1]", 0, 1);
	SimpleSynapseFunction defaultSf;

	defaultAf.addParameter("TestParam1", new IntValue(111));
	defaultTf.addParameter("TestParam2", new DoubleValue(432.1));
	defaultTf.addParameter("TestParam4", new IntValue(1711));
	defaultSf.addParameter("TestParam3", new BoolValue(true));

	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();
	nnm->addActivationFunctionPrototype(defaultAf);
	nnm->addTransferFunctionPrototype(defaultTf);
	nnm->addSynapseFunctionPrototype(defaultSf);

	ModularNeuralNetwork *net = new ModularNeuralNetwork(defaultAf, defaultTf, defaultSf);
	
	Neuron *neuron1 = new Neuron("Neuron1", defaultTf, defaultAf);
	neuron1->setProperty(Neuron::NEURON_TYPE_INPUT);
	neuron1->setProperty("TestProperty1", "SomeContent");
	neuron1->getBiasValue().set(0.98765);

	Neuron *neuron2 = new Neuron("Neuron2", defaultTf, defaultAf);
	neuron2->setProperty(Neuron::NEURON_TYPE_OUTPUT);
	neuron2->setProperty(Neuron::NEURON_TYPE_INPUT);

	Neuron *neuron3 = new Neuron("Neuron3", defaultTf, defaultAf);
	Neuron *neuron4 = new Neuron("Neuron4", defaultTf, defaultAf);
	Neuron *neuron5 = new Neuron("Neuron5", defaultTf, defaultAf);

	Synapse *synapse1 = Synapse::createSynapse(neuron1, neuron2, 0.5, defaultSf);
	synapse1->setProperty("SynapseProperty1", "Property");
	synapse1->setProperty("ProtectStrength");

	
	Synapse *synapse2 = Synapse::createSynapse(neuron1, neuron1, 11.53456, defaultSf);
	Synapse *synapse3 = Synapse::createSynapse(neuron1, synapse2, 50.5, defaultSf);
	synapse3->setPosition(Vector3D(0.0, -35.0, 0));
	Synapse *synapse4 = Synapse::createSynapse(neuron2, synapse3, 10.0, defaultSf);
	synapse4->setPosition(Vector3D(0, -17.5, 0));

	NeuroModule *groupSubModule = new NeuroModule("GroupSubModule");

	net->addNeuron(neuron1);
	net->addNeuron(neuron2);
	net->addNeuron(neuron3);
	net->addNeuron(neuron4);
	net->addNeuron(neuron5);

	NeuronGroup *group1 = new NeuronGroup("Group1");
	group1->setProperty("Type", "{Type1, Type2}");
	group1->setProperty("ProtectRemoval");
	group1->setProperty("ProtectNeurons");
	group1->addNeuron(neuron1);
	group1->addNeuron(neuron2);
	group1->addNeuron(neuron3);
	group1->addSubModule(groupSubModule);

	NumberOfNeuronsConstraint *constraint1 = new NumberOfNeuronsConstraint(1, 10);
	NumberOfNeuronsConstraint *constraint2 = new NumberOfNeuronsConstraint(-5, 5);
	group1->addConstraint(constraint1);
	group1->addConstraint(constraint2);
	
	NeuroModule *module1 = new NeuroModule("Module1");
	module1->setProperty("SomeProp", "Prop");
	module1->addNeuron(neuron5); 
	module1->addInputNeuron(neuron3);
	module1->addOutputNeuron(neuron4);
	module1->addInputNeuron(neuron1);
	module1->addOutputNeuron(neuron1);

	net->addNeuronGroup(group1);
	net->addNeuronGroup(module1);

	QString xml = NeuralNetworkIONerdV1Xml::createXmlFromNetwork(net);

	//cout << xml.toStdString().c_str() << endl;

	//Test output against a file containing the desired xml output.
	QStringList xmlLines = xml.split("\n");

	QFile file("TestOrcsV1XmlDesired.txt");

	QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

	{
		int i = 0; 
		while(!file.atEnd() && i < xmlLines.size()) {
			QString line = file.readLine();
			while(line.trimmed() == "" && !file.atEnd()) {
				line = file.readLine();
			}
			QString xmlLine = xmlLines.at(i);
			while(line.trimmed() == "" && i < (xmlLines.size() + 1)) {
				++i;
				xmlLine = xmlLines.at(i);
			}

			//cout << line.toStdString().c_str();
			//cout << xmlLine.toStdString().c_str() << endl;;

			QVERIFY(line.trimmed() == xmlLine.trimmed());

			++i;
		}
	}
	
	QString errorMessage;
	QList<QString> warnings;

	//StandardNeuralNetworkFunctions();
	StandardConstraintCollection();

	ModularNeuralNetwork *net2 = dynamic_cast<ModularNeuralNetwork*>(
					NeuralNetworkIONerdV1Xml::createNetFromXml(xml, &errorMessage, &warnings));

	for(int i = 0; i < warnings.size(); ++i) {
		cerr << warnings.at(i).toStdString().c_str() << endl;
	}

	Neuron *neuron1c = NeuralNetwork::selectNeuronById(neuron1->getId(), net2->getNeurons());
	QCOMPARE(neuron1c->getBiasValue().get(), 0.98765);

	NeuronGroup *groupCopy = ModularNeuralNetwork::selectNeuronGroupById(group1->getId(),
								net2->getNeuronGroups());
	QVERIFY(groupCopy != 0);
	QCOMPARE(groupCopy->getSubModules().size(), 1);

	QVERIFY(net->equals(net2));

	delete net;
	delete net2;
	

}


void TestNeuralNetworkIONerdV1Xml::testCreateNetworkFromXML() {

}


