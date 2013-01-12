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




#include "TestBasinPlotter.h"
#include <iostream>
#include "Core/Core.h"
#include "Network/NeuralNetwork.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "TransferFunction/TransferFunctionRamp.h"
#include "Adapters/TransferFunctionAdapter.h"
#include <Util/NeuralNetworkUtil.h>
#include <Util/DynamicsPlotterUtil.h>
#include <DynamicsPlot/BasinPlotter.h>
#include "DynamicsPlotConstants.h"
#include "NetworkEditorConstants.h"
#include "NerdConstants.h"
#include "NeuralNetworkConstants.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"

using namespace std;
using namespace nerd;

void TestBasinPlotter::initTestCase() {
}

void TestBasinPlotter::cleanUpTestCase() {
}


//Author
void TestBasinPlotter::testConstructors() {
	
}


void TestBasinPlotter::testParameterSettings() {
	
	Core::resetCore();
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	EventManager *em = Core::getInstance()->getEventManager();
	
	Neuro::getNeuralNetworkManager();
	
	//create required values and events
	//***************************************************************************************
	
	Event *e_ClearAllEditorSelections = em->getEvent(NetworkEditorConstants::VALUE_EDITOR_CLEAR_ALL_SELECTIONS, true);
	Event *e_NextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP, true);
	Event *e_StepCompletedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, true);
	Event *e_ResetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET, true);
	Event *e_ResetFinalizedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED, true);
	Event *e_EvaluateNetworkEvent = em->getEvent(NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_STARTED, true);
	
	BoolValue *v_StasisValue = new BoolValue(false);
	vm->addValue(NeuralNetworkConstants::VALUE_EVO_STASIS_MODE, v_StasisValue);
	
	

	//create basin plotter
	//***************************************************************************************
	BasinPlotter *basinPlotter = new BasinPlotter();
	

	//collect parameters and config of DynamicsPlotter
	//***************************************************************************************
	BoolValue *v_ActiveValue = dynamic_cast<BoolValue*>(basinPlotter->getParameter("Config/Activate"));
	IntValue *v_ExecutionTime = dynamic_cast<IntValue*>(basinPlotter->getParameter("Performance/ExecutionTime"));
	DoubleValue *v_ProgressPercentage = dynamic_cast<DoubleValue*>(basinPlotter->getParameter("Performance/ProgressPercentage"));
	BoolValue *v_EnableConstraints = dynamic_cast<BoolValue*>(basinPlotter->getParameter("Config/EnableConstraints"));
	
	MatrixValue *v_Data = dynamic_cast<MatrixValue*>(basinPlotter->getParameter("Internal/Data"));
	StringValue *v_FilePrefix = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/Diagram/FilePrefix"));
	StringValue *v_AxisNames = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/Diagram/AxisNames"));
	StringValue *v_TitleNames = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/Diagram/TitleNames"));
	
	QVERIFY(v_ActiveValue != 0);
	QVERIFY(v_ExecutionTime != 0);
	QVERIFY(v_ProgressPercentage != 0);
	QVERIFY(v_EnableConstraints != 0);
	
	QVERIFY(v_Data != 0);
	QVERIFY(v_FilePrefix != 0);
	QVERIFY(v_AxisNames != 0);
	QVERIFY(v_TitleNames != 0);
	
	
	//collect parameter and config values of BasinPlotter
	//***************************************************************************************
	StringValue *v_VariedX = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/VariedElementX"));
	StringValue *v_VariedY = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/VariedElementY"));
	StringValue *v_VariedRangeX = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/VariedRangeX"));
	StringValue *v_VariedRangeY = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/VariedRangeY"));
	IntValue *v_ResolutionX = dynamic_cast<IntValue*>(basinPlotter->getParameter("Config/ResolutionX"));
	IntValue *v_ResolutionY = dynamic_cast<IntValue*>(basinPlotter->getParameter("Config/ResolutionY"));
	
	StringValue *v_ProjectionsX = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/ProjectionsX"));
	StringValue *v_ProjectionsY = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/ProjectionsY"));
	StringValue *v_ProjectionRangesX = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/ProjectionRangesX"));
	StringValue *v_ProjectionRangesY = dynamic_cast<StringValue*>(basinPlotter->getParameter("Config/ProjectionRangesY"));
	
	DoubleValue *v_Accuracy = dynamic_cast<DoubleValue*>(basinPlotter->getParameter("Config/Accuracy"));
	IntValue *v_RoundDigits = dynamic_cast<IntValue*>(basinPlotter->getParameter("Config/RoundDigits"));
	
	IntValue *v_StepsToRunPreCheck = dynamic_cast<IntValue*>(basinPlotter->getParameter("Config/StepsToRunPreCheck"));
	IntValue *v_StepsToCheck = dynamic_cast<IntValue*>(basinPlotter->getParameter("Config/StepsToCheck"));
	
	BoolValue *v_ResetNetworkActivation = dynamic_cast<BoolValue*>(basinPlotter->getParameter("Config/ResetNetworkActivation"));
	BoolValue *v_RestoreNetworkConfiguration = dynamic_cast<BoolValue*>(basinPlotter->getParameter("Config/RestoreNetworkConfiguration"));
	BoolValue *v_ResetSimulator = dynamic_cast<BoolValue*>(basinPlotter->getParameter("Config/ResetSimulation"));
	
	QVERIFY(v_VariedX != 0);
	QVERIFY(v_VariedY != 0);
	QVERIFY(v_VariedRangeX != 0);
	QVERIFY(v_VariedRangeY != 0);
	QVERIFY(v_ResolutionX != 0);
	QVERIFY(v_ResolutionY != 0);
	QVERIFY(v_ProjectionsX != 0);
	QVERIFY(v_ProjectionsY != 0);
	QVERIFY(v_ProjectionRangesX != 0);
	QVERIFY(v_ProjectionRangesY != 0);
	QVERIFY(v_Accuracy != 0);
	QVERIFY(v_RoundDigits != 0);
	QVERIFY(v_StepsToRunPreCheck != 0);
	QVERIFY(v_StepsToCheck != 0);
	QVERIFY(v_ResetNetworkActivation != 0);
	QVERIFY(v_RestoreNetworkConfiguration != 0);
	QVERIFY(v_ResetSimulator != 0);
	
	
	//create network
	//***************************************************************************************
	
	AdditiveTimeDiscreteActivationFunction *af = new AdditiveTimeDiscreteActivationFunction();
	TransferFunctionRamp *ramp = new TransferFunctionRamp("ramp", -1, 1, false);
	SimpleSynapseFunction *sf = new SimpleSynapseFunction();
	
	ModularNeuralNetwork *network = new ModularNeuralNetwork(*af, *ramp, *sf);
	
	Neuron *n1 = new Neuron("Neuron1", *ramp, *af);
	network->addNeuron(n1);
	//Neuron *n2 = new Neuron("Neuron2", *ramp, *af);
	//network->addNeuron(n2);
	//Neuron *n3 = new Neuron("Neuron3", *ramp, *af);
	//network->addNeuron(n3);
	
	Synapse *s1 = Synapse::createSynapse(n1, n1, 1.02, *sf);
	//Synapse *s2 = Synapse::createSynapse(n3, n3, -1.5, *sf);
	
	

	//add to neural network manager
	QVERIFY(Neuro::getNeuralNetworkManager()->addNeuralNetwork(network));
	
	//Initialize NERD
	//***************************************************************************************
	QVERIFY(Core::getInstance()->init());
	
	
	
	
	
	//***************************************************************************************
	//***************************************************************************************
	//***************************************************************************************
	// from here on do tests!!
	//***************************************************************************************
	//***************************************************************************************
	//***************************************************************************************
	
	//check default values
	QVERIFY(v_EnableConstraints->get() == false);
	QVERIFY(v_FilePrefix->get() == "BasinOfAttraction");
	QVERIFY(v_AxisNames->get() == "x, y");
	QVERIFY(v_TitleNames->get() == "Basins of Attraction|Periods|Attractor");
	QCOMPARE(v_Accuracy->get(), 0.0001);
	QCOMPARE(v_RoundDigits->get(), -1);
	QCOMPARE(v_StepsToRunPreCheck->get(), 2000);
	QCOMPARE(v_StepsToCheck->get(), 100);
	QVERIFY(v_ResetNetworkActivation->get() == true);
	QVERIFY(v_RestoreNetworkConfiguration->get() == true);
	QVERIFY(v_ResetSimulator->get() == false);
	
	//... (seems to be all that are necessary, the others are unimportant.
	
	
	
	
	
	
	

	//Configure the main parameters
	//***************************************************************************************
	v_VariedX->set(QString::number(n1->getId()) + ":o");
	v_VariedY->set(QString::number(n1->getId()) + ":o");
	v_VariedRangeX->set("-1,1");
	v_VariedRangeY->set("-1,1");
	v_ResolutionX->set(200);
	v_ResolutionY->set(300);
	
	//... continue testing
	
	//default matrix size (after initialization)
	QCOMPARE(v_Data->getMatrixWidth(), 2);
	QCOMPARE(v_Data->getMatrixHeight(), 2);
	QCOMPARE(v_Data->getMatrixDepth(), 1);
	
	basinPlotter->execute();

	//matrix size after analyzer run.
	QCOMPARE(v_Data->getMatrixWidth(), 201);  //resolution 200 + 1 for coordinate system
	QCOMPARE(v_Data->getMatrixHeight(), 301); //resolution 200 + 1 for coordinate system
	QCOMPARE(v_Data->getMatrixDepth(), 3);
	
	QString matrixString = v_Data->getValueAsString();
	QString fileName = "testData/BasinPlotterTest.txt";
	//save matrix

	/**
	QFile out_file(fileName);
    if(!out_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		Core::log("TestBasinPlotter: Could not open file to save matrix to.", true);
		return;
	}
    QTextStream out_stream(&out_file);
    out_stream << matrixString;
	out_file.close();
	Core::log("TestBasinPlotter: Created file containing test data for comparison.", true);
	**/

	//load saved matrix
	QFile in_file(fileName);
	if(!in_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		Core::log("TestBasinPlotter: Could not open file to read matrix from.", true);
		return;
	}
	QTextStream in_stream(&in_file);
	QString oldMatrixString;
    while(!in_stream.atEnd()) {
    	oldMatrixString += in_stream.readLine();
    }
	in_file.close();

	//compare matrices
	QVERIFY(matrixString == oldMatrixString);
	
	
	//***************************************************************************************
	//***************************************************************************************
	//***************************************************************************************
	//cleanup
	//***************************************************************************************
	//***************************************************************************************
	//***************************************************************************************
	Neuro::reset();
	Core::resetCore();
}
