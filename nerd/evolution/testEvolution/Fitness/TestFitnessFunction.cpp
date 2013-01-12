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

#include "TestFitnessFunction.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "Value/BoolValue.h"
#include "Value/StringValue.h"
#include "FitnessFunctionAdapter.h"
#include "Network/NeuralNetwork.h"
#include "Network/Neuron.h"
#include "Math/Math.h"
#include "FitnessFunctions/SurvivalTimeFitnessFunction.h"
#include "Control/ControlInterfaceAdapter.h"
#include "Statistics/StatisticsManager.h"
#include "Statistics/Statistics.h"
#include "EvolutionConstants.h"
#include <iostream>

using namespace std;

namespace nerd {

// Verena
void TestFitnessFunction::testFitnessFunction() {
	Core::resetCore();
	ValueManager *vManager = Core::getInstance()->getValueManager();
	FitnessFunctionAdapter *fitness = new FitnessFunctionAdapter("Fitness1");	
	
	// Fitness Values are not registered at the ValueManager
	QVERIFY(fitness->getParameter("Fitness/Fitness") != 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/Fitness") == 0);
	
	QVERIFY(fitness->getParameter("Fitness/CurrentFitness") != 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/CurrentFitness") == 0);
	
	QVERIFY(fitness->getParameter("Config/Help") != 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Config/Help") == 0);

	QVERIFY(fitness->getParameter("Config/ErrorState") != 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Config/ErrorState") == 0);

	QVERIFY(fitness->getParameter("Fitness/MeanFitness") != 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/MeanFitness") == 0);

	QVERIFY(fitness->getParameter("Fitness/MinFitness") != 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/MinFitness") == 0);

	QVERIFY(fitness->getParameter("Fitness/MaxFitness") != 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/MaxFitness") == 0);

	QVERIFY(fitness->getParameter("Fitness/Variance") != 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/Variance") == 0);

	fitness->mCalculateReturnValue = 1122.33;
	
	// test update
	QCOMPARE(fitness->getCurrentFitness(), 0.0);
	fitness->update();
	QCOMPARE(fitness->getCurrentFitness(), 1122.33);
	QCOMPARE(fitness->getFitness(), 0.0);
	
	QCOMPARE(fitness->mUpdateCounter, 1);
	QCOMPARE(fitness->mCalculateCurrentFitnessCounter, 1);
	QCOMPARE(fitness->mGetCurrentFitnessCount, 2);

	// calling calculateFitness uses only the list of saved fitness values for all tries
	fitness->calculateFitness();
	QCOMPARE(fitness->getFitness(), 0.0);

	// to push the current fitness into the list of values, finishTry needs to be called
	fitness->finishTry();
	QCOMPARE(fitness->getCurrentFitness(), 1122.33);
	// Finish also updates the total fitness, based on the values currently stored in in the fitness list.
	QCOMPARE(fitness->getFitness(), 1122.33);
	// 	Now calculateFitness finds an entry in the list of values.
	fitness->calculateFitness();
	QCOMPARE(fitness->getFitness(), 1122.33);

	fitness->resetTry();
	QCOMPARE(fitness->getCurrentFitness(), 0.0);
	QCOMPARE(fitness->mCalculateFitnessCounter, 3);
	QCOMPARE(fitness->mResetTryCounter, 1);
	QCOMPARE(fitness->mPrepareNextTryCounter, 1);
	QCOMPARE(fitness->mGetFitnessCount, 4);
	
// 	fitness->finishTry();
	QCOMPARE(fitness->getFitness(), 1122.33);
	QCOMPARE(fitness->getCurrentFitness(), 0.0);
	QCOMPARE(fitness->mPrepareNextTryCounter, 1);

	fitness->reset();
	QCOMPARE(fitness->getFitness(), 0.0);
	QCOMPARE(fitness->getCurrentFitness(), 0.0);
	QCOMPARE(fitness->mResetCounter, 1);
	QCOMPARE(fitness->mPrepareNextTryCounter, 1);

	fitness->mCalculateReturnValue = 1.0;
	fitness->update();
	fitness->update();
	fitness->update();
	fitness->update();
	QCOMPARE(fitness->mUpdateCounter, 5);
	QCOMPARE(fitness->mCalculateCurrentFitnessCounter, 5);
	QCOMPARE(fitness->getParam("Fitness/CurrentFitness"), 4.0);
	QCOMPARE(fitness->getFitness(), 0.0);
	fitness->finishTry();
	fitness->resetTry();

	fitness->update();
	fitness->update();
	QCOMPARE(fitness->mUpdateCounter, 7);
	QCOMPARE(fitness->mCalculateCurrentFitnessCounter, 7);
	fitness->finishTry();

	fitness->calculateFitness();
	QCOMPARE(fitness->getFitness(), 3.0);

	//check other fitness calcualtion modes
	IntValue *mode = dynamic_cast<IntValue*>(fitness->getParameter("Fitness/CalculationMode"));	
	QVERIFY(mode != 0);
	mode->set(FitnessFunction::MIN_FITNESS);
	fitness->calculateFitness();
	QCOMPARE(fitness->getFitness(), 2.0);

	mode->set(FitnessFunction::MAX_FITNESS);
	fitness->calculateFitness();
	QCOMPARE(fitness->getFitness(), 4.0);

	
	fitness->attachToSystem();
	fitness->detachFromSystem();	
	QCOMPARE(fitness->mAttachToSystemCounter, 1);
	QCOMPARE(fitness->mDetachFromSystemCounter, 1);
	fitness->attachToSystem();
	fitness->detachFromSystem();	
	QCOMPARE(fitness->mAttachToSystemCounter, 2);
	QCOMPARE(fitness->mDetachFromSystemCounter, 2);

	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/Fitness") == 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/CurrentFitness") == 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Config/Help") == 0);;
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Config/ErrorState") == 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/MeanFitness") == 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/MinFitness") == 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/MaxFitness") == 0);
	QVERIFY(vManager->getValue("/FitnessFunctions/Fitness1/Fitness/Variance") == 0);

	delete fitness;
	Core::resetCore();
}

// Verena
void TestFitnessFunction::testGetValue() {
	Core::resetCore();
	ValueManager *vManager = Core::getInstance()->getValueManager();
	FitnessFunctionAdapter *fitness = new FitnessFunctionAdapter("Fitness1");	
	
	DoubleValue *doubleValue1 = new DoubleValue(11.5);
	vManager->addValue("Double1", doubleValue1);
	DoubleValue *doubleValue2 = new DoubleValue(55.1);
	vManager->addValue("Double2", doubleValue2);
	IntValue *intValue1 = new IntValue(22);
	vManager->addValue("Int1", intValue1);
	BoolValue *boolValue = new BoolValue(false);
	vManager->addValue("Bool1", boolValue);
		
	QCOMPARE(fitness->getValue("Double1"), 11.5);
	// Values that are not double or int will result in a return value of 0.0
	QCOMPARE(fitness->getValue("Bool1"), 0.0);
	// The value of an IntValue is return as double
	QCOMPARE(fitness->getValue("Int1"), 22.0);
	// For Names that do not match a value in the system, 0.0 is returned and a debug-logging message is added.
	QCOMPARE(fitness->getValue("Empty"), 0.0);
	QCOMPARE(vManager->getStringValue("/Logger/RecentMessage")->get()
			.compare("Fitness Fitness1: Could not find value Empty!"), 0);


	delete fitness;
	Core::resetCore();
}

// Verena
void TestFitnessFunction::testParam() {
	Core::resetCore();

	FitnessFunctionAdapter *fitness = new FitnessFunctionAdapter("Fitness1");
	
	QCOMPARE(fitness->getParameterNames().size(), 9);
	
	QCOMPARE(fitness->getParam("DoubleParam1"), 0.0);
	fitness->defineDoubleParam("DoubleParam1", 11.22);
	QCOMPARE(fitness->getParameterNames().size(), 10);
	QCOMPARE(fitness->getParam("DoubleParam1"), 11.22);

	fitness->defineIntParam("IntParam1");
	QCOMPARE(fitness->getParameterNames().size(), 11);
	QCOMPARE(fitness->getParam("IntParam1"), 0.0);
	fitness->setParam("IntParam1", 23);
	QCOMPARE(fitness->getParam("IntParam1"), 23.0);

	QVERIFY(Core::getInstance()->getValueManager()->getValuesMatchingPattern(".*DoubleParam1").size() == 0);
	
	fitness->publishAllParameters();
	QVERIFY(Core::getInstance()->getValueManager()->getValuesMatchingPattern(".*DoubleParam1").size() == 1);
		
	delete fitness;
	Core::resetCore();
}


//Chris
void TestFitnessFunction::testFitnessStatistics() {
	Core::getInstance()->resetCore();

	ValueManager *vm = Core::getInstance()->getValueManager();
	StatisticsManager *sm = Statistics::getStatisticsManager();

	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Mean") == 0);
	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Min") == 0);
	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Max") == 0);
	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Variance") == 0);

	QVERIFY(sm->getGenerationStatistics().size() == 0);

	FitnessFunctionAdapter *fitness = new FitnessFunctionAdapter("Fitness1");

	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Mean") == 0);
	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Min") == 0);
	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Max") == 0);
	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Variance") == 0);

	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/Fitness") == 0);
	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/CurrentFitness") == 0);
	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/MeanFitness") == 0);
	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/MinFitness") == 0);
	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/MaxFitness") == 0);
	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/Variance") == 0);

	QVERIFY(sm->getGenerationStatistics().size() == 0);

	
	//Attach the FitnessFunction to the system
	fitness->attachToSystem();
	
	DoubleValue *valFitness = vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/Fitness");
	DoubleValue *valCurrentFitness = vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/CurrentFitness");
	DoubleValue *valMeanFitness = vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/MeanFitness");
	DoubleValue *valMinFitness = vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/MinFitness");
	DoubleValue *valMaxFitness = vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/MaxFitness");
	DoubleValue *valVariance = vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/Variance");

	QVERIFY(valFitness != 0);
	QVERIFY(valCurrentFitness != 0);
	QVERIFY(valMeanFitness != 0);
	QVERIFY(valMinFitness != 0);
	QVERIFY(valMaxFitness != 0);
	QVERIFY(valVariance != 0);

	DoubleValue *statMean = vm->getDoubleValue("/Statistics/Fitness/Fitness1/Mean");
	DoubleValue *statMin = vm->getDoubleValue("/Statistics/Fitness/Fitness1/Min");
	DoubleValue *statMax = vm->getDoubleValue("/Statistics/Fitness/Fitness1/Max");
	DoubleValue *statVariance = vm->getDoubleValue("/Statistics/Fitness/Fitness1/Variance");

	QVERIFY(statMean != 0);
	QVERIFY(statMin != 0);
	QVERIFY(statMax != 0);
	QVERIFY(statVariance != 0);
	
	QVERIFY(sm->getGenerationStatistics().size() == 4);

	//verify that the statisticCalculators have the correct names.
	QList<QString> statisticNames;
	for(QListIterator<StatisticCalculator*> i(sm->getGenerationStatistics()); i.hasNext();) {
		QString name = i.next()->getName();
		QVERIFY(!statisticNames.contains(name));
		statisticNames.append(name);
	}
	
	QVERIFY(statisticNames.contains("Fitness/Fitness1/Mean"));
	QVERIFY(statisticNames.contains("Fitness/Fitness1/Min"));
	QVERIFY(statisticNames.contains("Fitness/Fitness1/Max"));
	QVERIFY(statisticNames.contains("Fitness/Fitness1/Variance"));


	//execute the fitness function
	QCOMPARE(fitness->getCurrentFitness(), 0.0);

	fitness->mCalculateReturnValue = 0.4;
	fitness->update();
	fitness->update();
	fitness->calculateFitness();

	QCOMPARE(fitness->getCurrentFitness(), 0.8);
	QCOMPARE(valFitness->get(), 0.0);
	QCOMPARE(valCurrentFitness->get(), 0.8);

	fitness->finishTry();
	
	QCOMPARE(fitness->getCurrentFitness(), 0.8);
	QCOMPARE(valFitness->get(), 0.8);
	QCOMPARE(valCurrentFitness->get(), 0.8);

	fitness->resetTry();
	
	QCOMPARE(fitness->getCurrentFitness(), 0.0);
	QCOMPARE(valFitness->get(), 0.8);
	QCOMPARE(valCurrentFitness->get(), 0.0);

	fitness->finishTry();
	fitness->resetTry();

	QCOMPARE(fitness->getCurrentFitness(), 0.0);
	QCOMPARE(valFitness->get(), 0.4); //halved because second try was 0.0
	QCOMPARE(valCurrentFitness->get(), 0.0);

	QCOMPARE(valMeanFitness->get(), 0.0);
	QCOMPARE(valMinFitness->get(), 0.0);
	QCOMPARE(valMaxFitness->get(), 0.0);
	QCOMPARE(valVariance->get(), 0.0);

	//Calcualte mean, min, max and variance after each individual.
	fitness->finishIndividual();
	
	QCOMPARE(valMeanFitness->get(), 0.4);
	QCOMPARE(valMinFitness->get(), 0.4);
	QCOMPARE(valMaxFitness->get(), 0.4);
	QCOMPARE(valVariance->get(), 0.0);

	fitness->mCalculateReturnValue = 0.2;
	
	fitness->reset();
	fitness->resetTry();
	fitness->update();
	fitness->finishTry();

	QCOMPARE(valFitness->get(), 0.2);
	
	fitness->finishIndividual();

	QCOMPARE(valMeanFitness->get(), 0.3);
	QCOMPARE(valMinFitness->get(), 0.2);
	QCOMPARE(valMaxFitness->get(), 0.4);
	QCOMPARE(valVariance->get(), 0.01);


	QCOMPARE(statMean->get(), 0.0);
	QCOMPARE(statMin->get(), 0.0);
	QCOMPARE(statMax->get(), 0.0);
	QCOMPARE(statVariance->get(), 0.0);

	//execute statistics
	for(QListIterator<StatisticCalculator*> i(sm->getGenerationStatistics()); i.hasNext();) {
		i.next()->calculateNextValue(0);
	}

	//now the last known statistic values in the ValueManager are updated, too.
	QCOMPARE(statMean->get(), 0.3);
	QCOMPARE(statMin->get(), 0.2);
	QCOMPARE(statMax->get(), 0.4);
	QCOMPARE(statVariance->get(), 0.01);

	
	

	//detach the FitnessFunction from the system.
	fitness->detachFromSystem();

	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Fitness/Mean") == 0);
	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Fitness/Min") == 0);
	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Fitness/Max") == 0);
	QVERIFY(vm->getValue("/Statistics/Fitness/Fitness1/Fitness/Variance") == 0);

	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/Fitness") == 0);
	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/CurrentFitness") == 0);
	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/MeanFitness") == 0);
	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/MinFitness") == 0);
	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/MaxFitness") == 0);
	QVERIFY(vm->getDoubleValue(
			EvolutionConstants::FITNESS_FUNCTION_PATH + "Fitness1/Fitness/Variance") == 0);

	QVERIFY(sm->getGenerationStatistics().size() == 0);	

	delete fitness;
	Core::resetCore();
}

// Verena
// void TestFitnessFunction::testNeuralNetworkFitnessFunction() {
// 	Core::resetCore();
// 
// 	NeuralNetworkFitnessFunctionAdapter *fitness1 = new NeuralNetworkFitnessFunctionAdapter("Fitness1");
// 	NeuralNetwork *net = new NeuralNetwork();
// 	
// 	Neuron *input1 = new Neuron("Input/One", TransferFunctionTanh(), AdditiveTimeDiscreteActivationFunction());
// 	input1->setProperty(Neuron::NEURON_TYPE_INPUT);
// 	Neuron *output1 = new Neuron("Output/One", TransferFunctionTanh(), AdditiveTimeDiscreteActivationFunction());
// 	output1->setProperty(Neuron::NEURON_TYPE_OUTPUT);
// 	Neuron *hidden1 = new Neuron("Hidden/One", TransferFunctionTanh(), AdditiveTimeDiscreteActivationFunction());
// 
// 	input1->getBiasValue().set(1.0);
// 	hidden1->getBiasValue().set(2.0);
// 	output1->getBiasValue().set(33.0);
// 	QCOMPARE(input1->getBiasValue().get(), 1.0);
// 	
// 	net->addNeuron(input1);
// 	net->addNeuron(output1);
// 	net->addNeuron(hidden1);
// 
// 	QVERIFY(net->getInputNeurons().size() > 0);
// 	QCOMPARE(net->getInputNeurons().at(0)->getBiasValue().get(), 1.0);
// 
// 	fitness1->update();
// 	QCOMPARE(fitness1->getCurrentFitness(), 0.0);
// 
// 	
// 	ControlInterfaceAdapter cia;
// 	cia.setController(net);
// 
// 	fitness1->setControlInterface(&cia);
// 	fitness1->update();
// 	QCOMPARE(fitness1->getCurrentFitness(), 33.0);
// 
// 	net->executeStep();
// 	
// 	// test getNeuronActivity.
// 	QVERIFY(Math::compareDoubles(hidden1->getOutputActivationValue().get(), 0.964028, 5));
// 	QCOMPARE(fitness1->getNeuronActivity("Hidden/One"), hidden1->getOutputActivationValue().get());
// 	QCOMPARE(fitness1->getNeuronActivity("Input/One"), input1->getOutputActivationValue().get());
// 	QCOMPARE(fitness1->getNeuronActivity("Output/One"), output1->getOutputActivationValue().get());
// 	//If no neuron of the specified name exists, zero will be returned.
// 	QCOMPARE(fitness1->getNeuronActivity("Test"), 0.0);
// 	
// 	delete fitness1;
// 	delete net;
// 	Core::resetCore();
// 
// }


//Verena
void TestFitnessFunction::testSurvivalTimeFitnessFunction() {
	Core::resetCore();
	SurvivalTimeFitnessFunction *fitness = new SurvivalTimeFitnessFunction("SurvivalTime");

	QCOMPARE(fitness->getCurrentFitness(), 0.0);
	QCOMPARE(fitness->getFitness(), 0.0);
	fitness->update();
	fitness->update();
	QCOMPARE(fitness->getCurrentFitness(), 2.0);
	QCOMPARE(fitness->getFitness(), 0.0);
	fitness->finishTry();
	fitness->resetTry();
	QCOMPARE(fitness->getCurrentFitness(), 0.0);
	QCOMPARE(fitness->getFitness(), 2.0);

	fitness->update();
	fitness->update();
	fitness->update();
	fitness->update();
	QCOMPARE(fitness->getCurrentFitness(), 4.0);
	QCOMPARE(fitness->getFitness(), 2.0);
	fitness->finishTry();
	fitness->resetTry();
	QCOMPARE(fitness->getCurrentFitness(), 0.0);
	QCOMPARE(fitness->getFitness(), 3.0);

	delete fitness;
	Core::resetCore();

}

}
