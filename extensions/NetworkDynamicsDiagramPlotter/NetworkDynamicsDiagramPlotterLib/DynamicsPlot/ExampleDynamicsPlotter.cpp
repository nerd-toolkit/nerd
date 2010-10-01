/***************************************************************************
 *   NERD - Open Robot Control and Simulation Library                      *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Copyright (C) 2008 by Christian Rempis, Ferry Bachmann                *
 *   christian.rempis@uni-osnabrueck.de +                                  *
 *   ferry.bachmann@uni-osnabrueck.de                                      *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD library is part of the EU project ALEAR                      *
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
 ***************************************************************************/



#include "ExampleDynamicsPlotter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QTime>
#include "Value/DoubleValue.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ExampleDynamicsPlotter.
 */
ExampleDynamicsPlotter::ExampleDynamicsPlotter()
	: DynamicsPlotter("ExampleDynamicsPlotter")
{
	mData = new MatrixValue();
	mMaxNumberOfStepsPerEvaluation = new IntValue(100000);
	mNumberOfEvaluations = new IntValue(100);
	mIdOfVariedNeuronBias = new ULongLongValue(0);
	mIdOfObservedNeuron = new ULongLongValue(0);
	mMinVariation = new DoubleValue(-1.0);
	mMaxVariation = new DoubleValue(1.0);

	addParameter("Data", mData, true);
	addParameter("MaxNumberOfStepsPerEvaluation", mMaxNumberOfStepsPerEvaluation, true);
	addParameter("NumberOfEvaluations", mNumberOfEvaluations, true);
	addParameter("IdOfVariedNeuronBias", mIdOfVariedNeuronBias, true);
	addParameter("IdOfObservedNeuron", mIdOfObservedNeuron, true);	
	addParameter("MinVariation", mMinVariation, true);
	addParameter("MaxVariation", mMaxVariation, true);
}




/**
 * Destructor.
 */
ExampleDynamicsPlotter::~ExampleDynamicsPlotter() {
}


void ExampleDynamicsPlotter::calculateData() {

	if(mNextStepEvent == 0 || mResetEvent == 0 || mEvaluateNetworkEvent == 0) {
		return;
	}

	NeuralNetwork *network = getCurrentNetwork();

	if(network == 0) {
		Core::log("ExampleDynamicsPlotter: Could not find a modular neural network!", true);
		return;
	}
	

	//Get pointers to the relevant input / output neurons:
	Neuron *variedNeuron = NeuralNetwork::selectNeuronById(mIdOfVariedNeuronBias->get(), 
						network->getNeurons());
	Neuron *observedNeuron = NeuralNetwork::selectNeuronById(mIdOfObservedNeuron->get(),
						network->getNeurons());

	if(variedNeuron == 0 || observedNeuron == 0) {
		Core::log("ExampleDynamicsPlotter: Could not find required neurons (varied / observed)!", true);
		return;
	}

	//prepare variables for evaluations
	int maxNumberOfStepsPerEvaluation = mMaxNumberOfStepsPerEvaluation->get();
	int numberOfEvaluations = mNumberOfEvaluations->get();

	  //TODO evt. make sure that min < max
	double minBias = mMinVariation->get();
	double maxBias = mMaxVariation->get();
	double variationIncrement = (maxBias - minBias) / (double) numberOfEvaluations;

	//set contains size
	mData->resize(numberOfEvaluations, 2, 1); 

	Core *core = Core::getInstance();

	double neuronOutput = 0.0;

	//evaluate
	for(int i = 0;  i < numberOfEvaluations; ++i) {

		//store the current output activation distribution.
		storeCurrentNetworkActivities();

		double previousBias = variedNeuron->getBiasValue().get(); 
		variedNeuron->getBiasValue().set(previousBias + minBias + (i * variationIncrement));
		
		for(int j = 0; j < maxNumberOfStepsPerEvaluation; ++j) {

			//This is important: allows to quit the application while the plotter is running.
			if(core->isShuttingDown()) {
				return;
			}

			neuronOutput = observedNeuron->getOutputActivationValue().get();

			//this executes the neural network once.
			mEvaluateNetworkEvent->trigger();

			//This is important: it keeps the system alive!
			core->executePendingTasks();

			//TODO check for evaluation end (e.g.: did the neuronOutput converge?)
		}
		//determin result value
		double result = neuronOutput; //TODO this is just an example :)

		mData->set(i * variationIncrement, i, 0, 0); 
		mData->set(result, i, 1, 0);

		variedNeuron->getBiasValue().set(previousBias);

		//reset the network to the initial output activation distribution.
		restoreCurrentNetworkActivites();	

		//This is important: allows to quit the application while the plotter is running.
		if(Core::getInstance()->isShuttingDown()) {
			return;
		}

		//This is important: it keeps the system alive!
		core->executePendingTasks();
	}
}


}




