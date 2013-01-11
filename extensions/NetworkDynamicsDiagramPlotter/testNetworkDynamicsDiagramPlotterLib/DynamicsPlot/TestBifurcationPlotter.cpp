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




#include "TestBifurcationPlotter.h"
#include <iostream>
#include "Core/Core.h"
#include "Network/NeuralNetwork.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "TransferFunction/TransferFunctionRamp.h"
#include "Adapters/TransferFunctionAdapter.h"
#include <Util/NeuralNetworkUtil.h>
#include <Util/DynamicsPlotterUtil.h>
#include <DynamicsPlot/BifurcationPlotter.h>
#include "DynamicsPlotConstants.h"
#include "NetworkEditorConstants.h"
#include "NerdConstants.h"
#include "NeuralNetworkConstants.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"

using namespace std;
using namespace nerd;

void TestBifurcationPlotter::initTestCase() {
}

void TestBifurcationPlotter::cleanUpTestCase() {
}


//Author
void TestBifurcationPlotter::testConstructors() {
	
}


//josef
void TestBifurcationPlotter::testParameterSettings() {

	// initialize NERD
	Core::resetCore();
	ValueManager *vm = Core::getInstance()->getValueManager();
	EventManager *em = Core::getInstance()->getEventManager();

	Neuro::getNeuralNetworkManager();

	Event *e_ClearAllEditorSelections = em->getEvent(NetworkEditorConstants::VALUE_EDITOR_CLEAR_ALL_SELECTIONS, true);
	Event *e_NextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP, true);
	Event *e_StepCompletedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, true);
	Event *e_ResetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET, true);
	Event *e_ResetFinalizedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED, true);
	Event *e_EvaluateNetworkEvent = em->getEvent(NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_STARTED, true);
	
	BoolValue *v_StasisValue = new BoolValue(false);
	vm->addValue(NeuralNetworkConstants::VALUE_EVO_STASIS_MODE, v_StasisValue);


	// create plotter object
	BifurcationPlotter *plotter = new BifurcationPlotter();


	// get general parameters and check them
	BoolValue *v_ActiveValue = dynamic_cast<BoolValue*>(plotter->getParameter("Config/Activate"));
	IntValue *v_ExecutionTime = dynamic_cast<IntValue*>(plotter->getParameter("Performance/ExecutionTime"));
	DoubleValue *v_ProgressPercentage = dynamic_cast<DoubleValue*>(plotter->getParameter("Performance/ProgressPercentage"));
	BoolValue *v_EnableConstraints = dynamic_cast<BoolValue*>(plotter->getParameter("Config/EnableConstraints"));
	
	MatrixValue *v_Data = dynamic_cast<MatrixValue*>(plotter->getParameter("Internal/Data"));
	StringValue *v_FilePrefix = dynamic_cast<StringValue*>(plotter->getParameter("Config/Diagram/FilePrefix"));
	StringValue *v_AxisNames = dynamic_cast<StringValue*>(plotter->getParameter("Config/Diagram/AxisNames"));
	StringValue *v_TitleNames = dynamic_cast<StringValue*>(plotter->getParameter("Config/Diagram/TitleNames"));
	
	QVERIFY(v_ActiveValue != 0);
	QVERIFY(v_ExecutionTime != 0);
	QVERIFY(v_ProgressPercentage != 0);
	QVERIFY(v_EnableConstraints != 0);
	
	QVERIFY(v_Data != 0);
	QVERIFY(v_FilePrefix != 0);
	QVERIFY(v_AxisNames != 0);
	QVERIFY(v_TitleNames != 0);

	
	// get specific parameters and do simple non-null checks
	StringValue *v_ObservedElements = dynamic_cast<StringValue*>(plotter->getParameter("Config/ObservedElements"));
	StringValue *v_ObservedRanges = dynamic_cast<StringValue*>(plotter->getParameter("Config/ObservedRanges"));
	StringValue *v_VariedElement = dynamic_cast<StringValue*>(plotter->getParameter("Config/VariedElement"));
	StringValue *v_VariedRange = dynamic_cast<StringValue*>(plotter->getParameter("Config/VariedRange"));
	IntValue *v_ObservedResolution = dynamic_cast<IntValue*>(plotter->getParameter("Config/ObservedResolution"));
	IntValue *v_VariedResolution = dynamic_cast<IntValue*>(plotter->getParameter("Config/VariedResolution"));

	IntValue *v_StepsToRun = dynamic_cast<IntValue*>(plotter->getParameter("Config/StepsToRun"));
	IntValue *v_StepsToPlot = dynamic_cast<IntValue*>(plotter->getParameter("Config/StepsToPlot"));
	
	BoolValue *v_ResetNetworkActivation = dynamic_cast<BoolValue*>(plotter->getParameter("Config/ResetNetworkActivation"));
	BoolValue *v_RunBackwards = dynamic_cast<BoolValue*>(plotter->getParameter("Config/RunBackwards"));
	BoolValue *v_RestoreNetworkConfiguration = dynamic_cast<BoolValue*>(plotter->getParameter("Config/RestoreNetworkConfiguration"));
	BoolValue *v_ResetSimulator = dynamic_cast<BoolValue*>(plotter->getParameter("Config/ResetSimulation"));
	
	QVERIFY(v_ObservedElements != 0);
	QVERIFY(v_ObservedRanges != 0);
	QVERIFY(v_VariedElement != 0);
	QVERIFY(v_VariedRange != 0);
	QVERIFY(v_ObservedResolution != 0);
	QVERIFY(v_VariedResolution != 0);
	QVERIFY(v_StepsToRun != 0);
	QVERIFY(v_StepsToPlot != 0);
	QVERIFY(v_ResetNetworkActivation != 0);
	QVERIFY(v_RunBackwards != 0);
	QVERIFY(v_RestoreNetworkConfiguration != 0);
	QVERIFY(v_ResetSimulator != 0);


	// create network to run
	AdditiveTimeDiscreteActivationFunction *af = new AdditiveTimeDiscreteActivationFunction();
	TransferFunctionRamp *ramp = new TransferFunctionRamp("ramp", -1, 1, false);
	SimpleSynapseFunction *sf = new SimpleSynapseFunction();
	
	ModularNeuralNetwork *network = new ModularNeuralNetwork(*af, *ramp, *sf);

	Neuron *n1 = new Neuron("Neuron1", *ramp, *af);
	network->addNeuron(n1);
	Neuron *n2 = new Neuron("Neuron2", *ramp, *af);
	network->addNeuron(n2);
	Synapse *s1 = Synapse::createSynapse(n1, n1, 1.02, *sf);
	Synapse *s2 = Synapse::createSynapse(n1, n2, -1.1, *sf);
	Synapse *s3 = Synapse::createSynapse(n2, n1, 1.1, *sf);

	QVERIFY(Neuro::getNeuralNetworkManager()->addNeuralNetwork(network));
	QVERIFY(Core::getInstance()->init());
	

	// check default parameter settings
	//QVERIFY(v_ObservedElements->get() == false);
	QVERIFY(v_ObservedRanges->get() == "-1,1");
	//QVERIFY(v_VariedElement->get() == false);
	QVERIFY(v_VariedRange->get() == "0,1");
	QVERIFY(v_ObservedResolution->get() == 600);
	QVERIFY(v_VariedResolution->get() == 600);
	QVERIFY(v_StepsToRun->get() == 1000);
	QVERIFY(v_StepsToPlot->get() == 10);
	QVERIFY(v_ResetNetworkActivation->get() == true);
	QVERIFY(v_RestoreNetworkConfiguration->get() == true);
	QVERIFY(v_RunBackwards->get() == true);
	QVERIFY(v_ResetSimulator->get() == true);

	// and matrix size
	QVERIFY(v_Data->getMatrixWidth() == 2);
	QVERIFY(v_Data->getMatrixHeight() == 2);
	QVERIFY(v_Data->getMatrixDepth() == 1);


	// configure them for the test case
	v_ObservedElements->set(QString::number(n1->getId()) + ":o");
	v_ObservedRanges->set("-1,1");
	v_VariedElement->set(QString::number(n2->getId()) + ":o");
	v_VariedRange->set("-1,1");
	v_ObservedResolution->set(1000);
	v_VariedResolution->set(1000);


	// run plotter
	plotter->execute();


	// check matrix dimensions
	QVERIFY(v_Data->getMatrixWidth() == 1001);
	QVERIFY(v_Data->getMatrixHeight() == 1001);
	QVERIFY(v_Data->getMatrixDepth() == 1);


	// compare saved to newly generated test data
	QString testDataNew = v_Data->getValueAsString();
	QString fileName = "testData/BifurcationPlotterTest.txt";

	/**
	QFile out_file(fileName);
	if(!out_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		Core::log("TestBifurcationPlotter: Could not open file to save matrix to.", true);
		return;
	}
	QTextStream out_stream(&out_file);
    out_stream << testDataNew;
	out_file.close();
	Core::log("TestBifurcationPlotter: Created file containing test data for comparison.", true);
	**/

	QFile in_file(fileName);
	if(!in_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		Core::log("TestBifurcationPlotter: Could not open file to read matrix from.", true);
		return;
	}
	QTextStream in_stream(&in_file);
	QString testDataOld;
    while(!in_stream.atEnd()) {
    	testDataOld += in_stream.readLine();
    }
	in_file.close();

	//compare matrices
	QVERIFY(testDataNew == testDataOld);
	
	
	// clean-up
	Neuro::reset();
	Core::resetCore();

}
