/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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
 *   The NERD Toolkit is part of the EU project ALEAR                      *
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
 *   clearly by citing the NERD homepage and the NERD overview paper.      *  
 ***************************************************************************/




#include "TestLyapunovExponent.h"
#include <iostream>
#include "Core/Core.h"
#include "Network/NeuralNetwork.h"
#include "ActivationFunction/ChaoticNeuronActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "Adapters/TransferFunctionAdapter.h"
#include <Util/NeuralNetworkUtil.h>
#include <Util/DynamicsPlotterUtil.h>
#include <DynamicsPlot/LyapunovExponent.h>
#include "DynamicsPlotConstants.h"
#include "NetworkEditorConstants.h"
#include "NerdConstants.h"
#include "NeuralNetworkConstants.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"

using namespace std;
using namespace nerd;

void TestLyapunovExponent::initTestCase() {
}

void TestLyapunovExponent::cleanUpTestCase() {
}


//Author
void TestLyapunovExponent::testConstructors() {
	
}


//josef
void TestLyapunovExponent::testParameterSettings() {

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
	
	if(e_ClearAllEditorSelections == 0
		|| e_NextStepEvent == 0
		|| e_StepCompletedEvent == 0
		|| e_ResetEvent == 0
		|| e_ResetFinalizedEvent == 0
		|| e_EvaluateNetworkEvent == 0)
	{
		cerr << "Warning, not all required events could be created properly!" << endl;
	}
	
	BoolValue *v_StasisValue = new BoolValue(false);
	vm->addValue(NeuralNetworkConstants::VALUE_EVO_STASIS_MODE, v_StasisValue);


	// create plotter object
	LyapunovExponent *plotter = new LyapunovExponent();


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
	StringValue *v_VariedElement = dynamic_cast<StringValue*>(plotter->getParameter("Config/VariedElement"));
	StringValue *v_VariedRange = dynamic_cast<StringValue*>(plotter->getParameter("Config/VariedRange"));
	IntValue *v_ResolutionX = dynamic_cast<IntValue*>(plotter->getParameter("Config/ResolutionX"));
	IntValue *v_ResolutionY = dynamic_cast<IntValue*>(plotter->getParameter("Config/ResolutionY"));

	IntValue *v_StepsPrePlot = dynamic_cast<IntValue*>(plotter->getParameter("Config/StepsPrePlot"));
	IntValue *v_StepsToPlot = dynamic_cast<IntValue*>(plotter->getParameter("Config/StepsToPlot"));
	
	BoolValue *v_DrawNL = dynamic_cast<BoolValue*>(plotter->getParameter("Config/DrawNL"));
	
	QVERIFY(v_VariedElement != 0);
	QVERIFY(v_VariedRange != 0);
	QVERIFY(v_ResolutionX != 0);
	QVERIFY(v_ResolutionY != 0);
	QVERIFY(v_StepsPrePlot != 0);
	QVERIFY(v_StepsToPlot != 0);
	QVERIFY(v_DrawNL != 0);


	// create network to run
	ChaoticNeuronActivationFunction *af = new ChaoticNeuronActivationFunction();
	TransferFunctionTanh *tf = new TransferFunctionTanh();
	SimpleSynapseFunction *sf = new SimpleSynapseFunction();
	
	ModularNeuralNetwork *network = new ModularNeuralNetwork(*af, *tf, *sf);

	Neuron *n1 = new Neuron("ChaosNeuron", *tf, *af);
	network->addNeuron(n1);
	//Synapse *s1 = Synapse::createSynapse(n1, n1, -18.5, *sf);

	QVERIFY(Neuro::getNeuralNetworkManager()->addNeuralNetwork(network));
	QVERIFY(Core::getInstance()->init());
	
	// check default parameter settings
	QVERIFY(v_VariedRange->get() == "-1,1");
	QVERIFY(v_ResolutionX->get() == 400);
	QVERIFY(v_ResolutionY->get() == 400);
	QVERIFY(v_StepsPrePlot->get() == 0);
	QVERIFY(v_StepsToPlot->get() == 100);
	QVERIFY(v_DrawNL->get() == true);

	DoubleValue *v_Gamma = dynamic_cast<DoubleValue*>(af->getParameter("Gamma"));
	QVERIFY(v_Gamma->get() == 0.9); // default per ChaoticNeuronActivationFunction.cpp

	// and matrix size
	QVERIFY(v_Data->getMatrixWidth() == 2);
	QVERIFY(v_Data->getMatrixHeight() == 2);
	QVERIFY(v_Data->getMatrixDepth() == 1);


	// configure them for the test case
	v_VariedElement->set(QString::number(n1->getId()) + ":o");
	v_VariedRange->set("-1,1");
	v_ResolutionX->set(600);
	v_ResolutionY->set(500);
	v_StepsPrePlot->set(1);
	v_StepsToPlot->set(2000);
	v_DrawNL->set(false);
	v_Gamma->set(0.8);


	// run plotter
	plotter->execute();


	// check matrix dimensions
	QVERIFY(v_Data->getMatrixWidth() == 601);
	QVERIFY(v_Data->getMatrixHeight() == 501);
	QVERIFY(v_Data->getMatrixDepth() == 1);


	// compare saved to newly generated test data
	QString testDataNew = v_Data->getValueAsString();
	QString fileName = "testData/LyapunovExponentTest.txt";

	/**
	QFile out_file(fileName);
	if(!out_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		Core::log("TestLyapunovExponent: Could not open file to save matrix to.", true);
		return;
	}
	QTextStream out_stream(&out_file);
    out_stream << testDataNew;
	out_file.close();
	Core::log("TestLyapunovExponent: Created file containing test data for comparison.", true);
	**/

	QFile in_file(fileName);
	if(!in_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		Core::log("TestLyapunovExponent: Could not open file to read matrix from.", true);
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
