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




#include "TestDynamicsPlotterUtil.h"
#include <iostream>
#include "Core/Core.h"
#include "Network/NeuralNetwork.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "TransferFunction/TransferFunctionRamp.h"
#include "Adapters/TransferFunctionAdapter.h"
#include <Util/NeuralNetworkUtil.h>
#include <Util/DynamicsPlotterUtil.h>

using namespace std;
using namespace nerd;

void TestDynamicsPlotterUtil::initTestCase() {
}

void TestDynamicsPlotterUtil::cleanUpTestCase() {
}


//chris
void TestDynamicsPlotterUtil::testTransferActivationToOutput() {
	
	AdditiveTimeDiscreteActivationFunction af;
	TransferFunctionRamp ramp("ramp", -1, 1, false);
	SimpleSynapseFunction sf;

	NeuralNetwork *network = new NeuralNetwork(af, ramp, sf);
	Neuron *n1 = new Neuron("Neuron1", ramp, af);
	network->addNeuron(n1);
	Neuron *n2 = new Neuron("Neuron2", ramp, af);
	network->addNeuron(n2);
	Neuron *n3 = new Neuron("Neuron3", ramp, af);
	network->addNeuron(n3);
	
	Synapse *s1 = Synapse::createSynapse(n1, n2, 0.5, sf);
	Synapse *s2 = Synapse::createSynapse(n3, n3, -1.5, sf);
	
	
	QCOMPARE(n1->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n1->getActivationValue().get(), 0.0);
	QCOMPARE(n2->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n2->getActivationValue().get(), 0.0);
	QCOMPARE(n3->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n3->getActivationValue().get(), 0.0);
	
	n1->getActivationValue().set(0.75);
	n2->getActivationValue().set(-1.2);
	n3->getActivationValue().set(2.8);
	
	network->executeStep();
	
	//did not have any effect (activation is overwritten with current output values)
	QCOMPARE(n1->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n1->getActivationValue().get(), 0.0);
	QCOMPARE(n2->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n2->getActivationValue().get(), 0.0);
	QCOMPARE(n3->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n3->getActivationValue().get(), 0.0);
	
	n1->getActivationValue().set(0.75);
	n2->getActivationValue().set(-1.2);
	n3->getActivationValue().set(2.8);
	
	DynamicsPlotterUtil::transferNeuronActivationToOutput(network);
	
	QCOMPARE(n1->getOutputActivationValue().get(), 0.75);
	QCOMPARE(n1->getActivationValue().get(), 0.75);
	QCOMPARE(n2->getOutputActivationValue().get(), -1.0);
	QCOMPARE(n2->getActivationValue().get(), -1.2);
	QCOMPARE(n3->getOutputActivationValue().get(), 1.0);
	QCOMPARE(n3->getActivationValue().get(), 2.8);
	
	network->executeStep();
	
	QCOMPARE(n1->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n1->getActivationValue().get(), 0.0);
	QCOMPARE(n2->getOutputActivationValue().get(), 0.375);
	QCOMPARE(n2->getActivationValue().get(), 0.375);
	QCOMPARE(n3->getOutputActivationValue().get(), -1.0);
	QCOMPARE(n3->getActivationValue().get(), -1.5);
	
	
	//do only partial transfer
	network->reset();
	
	n1->getActivationValue().set(0.75);
	n2->getActivationValue().set(-1.2);
	n3->getActivationValue().set(2.8);
	
	QList<Neuron*> neuronsToTransfer;
	neuronsToTransfer.append(n1);
	neuronsToTransfer.append(n2);
	
	DynamicsPlotterUtil::transferNeuronActivationToOutput(neuronsToTransfer);
	
	QCOMPARE(n1->getOutputActivationValue().get(), 0.75);
	QCOMPARE(n1->getActivationValue().get(), 0.75);
	QCOMPARE(n2->getOutputActivationValue().get(), -1.0);
	QCOMPARE(n2->getActivationValue().get(), -1.2);
	QCOMPARE(n3->getOutputActivationValue().get(), 0.0); //is not transferred
	QCOMPARE(n3->getActivationValue().get(), 2.8);
	
	network->executeStep();
	
	QCOMPARE(n1->getOutputActivationValue().get(), 0.0);
	QCOMPARE(n1->getActivationValue().get(), 0.0);
	QCOMPARE(n2->getOutputActivationValue().get(), 0.375);
	QCOMPARE(n2->getActivationValue().get(), 0.375);
	QCOMPARE(n3->getOutputActivationValue().get(), 0.0); 
	QCOMPARE(n3->getActivationValue().get(), 0.0); //is overwritten by current activation value
	
}




