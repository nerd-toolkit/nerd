/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   till.faber@uni-osnabrueck.de
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



#include "SimpleBifurcationPlotter.h"
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
	 * Constructs a new SimpleBifurcationPlotter.
 */
	SimpleBifurcationPlotter::SimpleBifurcationPlotter()
	: DynamicsPlotter("SimpleBifurcationPlotter")
	{
		mData = new MatrixValue();
		mMaxNumberOfStepsPerEvaluation = new IntValue(100000);
		mNumberOfEvaluations = new IntValue(100);
		mIdOfVariedNeuronBias = new ULongLongValue(0);
		mIdOfObservedNeuron = new ULongLongValue(0);
		mMinVariation = new DoubleValue(-1.0);
		mMaxVariation = new DoubleValue(1.0);

		mMinOutputRange = new DoubleValue(-1.0); //in case other transfer function than tanh - now set for tanh
		mMaxOutputRange = new DoubleValue(1.0); // -- " --
		mNumberOfOutputBuckets = new IntValue(1000); // number of values the output is discretized/rounded to
		mNumberOfPreRuns = new IntValue(400); //Number of steps of network to avoid transient



		addParameter("Data", mData, true);
		addParameter("MaxNumberOfStepsPerEvaluation", mMaxNumberOfStepsPerEvaluation, true);
		addParameter("NumberOfEvaluations", mNumberOfEvaluations, true);
		addParameter("IdOfVariedNeuronBias", mIdOfVariedNeuronBias, true);
		addParameter("IdOfObservedNeuron", mIdOfObservedNeuron, true);	
		addParameter("MinVariation", mMinVariation, true);
		addParameter("MaxVariation", mMaxVariation, true);

		addParameter("NumberOfPreRuns", mNumberOfPreRuns, true);
		addParameter("MinOutputRange", mMinOutputRange, true);
		addParameter("MaxOutputRange", mMaxOutputRange, true);
		addParameter("NumberOfOutputBuckets", mNumberOfOutputBuckets, true);
	}




/**
	 * Destructor.
 */
	SimpleBifurcationPlotter::~SimpleBifurcationPlotter() {
	}


	void SimpleBifurcationPlotter::calculateData() {

		if(mNextStepEvent == 0 || mResetEvent == 0 || mEvaluateNetworkEvent == 0) {
			return;
		}

		NeuralNetwork *network = getCurrentNetwork();

		if(network == 0) {
			Core::log("SimpleBifurcationPlotter: Could not find a modular neural network!", true);
			return;
		}
	

	//Get pointers to the relevant input / output neurons:
		Neuron *variedNeuron = NeuralNetwork::selectNeuronById(mIdOfVariedNeuronBias->get(), 
				network->getNeurons());
		Neuron *observedNeuron = NeuralNetwork::selectNeuronById(mIdOfObservedNeuron->get(),
				network->getNeurons());

		if(variedNeuron == 0 || observedNeuron == 0) {
			Core::log("SimpleBifurcationPlotter: Could not find required neurons (varied / observed)!", true);
			return;
		}

	//prepare variables for evaluations
		int maxNumberOfStepsPerEvaluation = mMaxNumberOfStepsPerEvaluation->get();

	//bias paramater variables
		int numberOfEvaluations = mNumberOfEvaluations->get();	//TODO must be > 1
		double minBias = mMinVariation->get();
		double maxBias = mMaxVariation->get();
		if(minBias >= maxBias){
			Core::log("SimpleBifurcationPlotter: Minimal value for parameter must be smaller than maximal value", true);
			return;
		}
		double variationIncrement = (maxBias - minBias) / ((double) numberOfEvaluations -1); //'-1' to include min and max values in steps
		double r = minBias; //running parameter
		int numberOfPreRuns = mNumberOfPreRuns->get();

	//output range variables
		int numberOfOutputBuckets = mNumberOfOutputBuckets->get(); //TODO check > 1, da später -1 und muss != 0
		double minOutputRange = mMinOutputRange->get();
		double maxOutputRange = mMaxOutputRange->get();
		if(minOutputRange > maxOutputRange){ //$$$
			Core::log("SimpleBifurcationPlotter: Minimal value for output range must be smaller than maximal value", true);
			return;
		}
		double bucketIncrement = (maxOutputRange - minOutputRange) / ((double) numberOfOutputBuckets - 1); //'-1' to include min and max 


	//set up matrix:
	//set contains size
		mData->resize(numberOfEvaluations +1, numberOfOutputBuckets +1, 1); //first dimension = no. of parameter changes; second dimension = number of output buckets
		mData->set(-1.0 / 0.0, 0, 0, 0); //set first entry to minus inf to tag where matrix indices are
		for(int j = 1; j < numberOfOutputBuckets+1; ++j){
			mData->set(minOutputRange + (j-1)*bucketIncrement, 0, j, 0); //set first matrix column: indices
		
		}


		Core *core = Core::getInstance();

		double neuronOutput = 0.0;

		storeCurrentNetworkActivities();

	

	//evaluate
		for(int i = 1;  i < numberOfEvaluations+1; ++i) {
			variedNeuron->getBiasValue().set(r);
			mData->set(r, i, 0, 0); //set first matrix row: indices 
			for(int j = 0; j < numberOfPreRuns; ++j){

			//let network run to get rid of the transient part of the trajectory

			//this executes the neural network once.
				mEvaluateNetworkEvent->trigger();


			//This is important: allows to quit the application while the plotter is running.
				if(core->isShuttingDown()) {
					return;
				}			
			//This is important: it keeps the system alive!
				core->executePendingTasks();	
			}
	

			for(int j = 0; j < maxNumberOfStepsPerEvaluation; ++j){
			// add to matrix
				neuronOutput = observedNeuron->getOutputActivationValue().get();
			// find bucket for neuronOutput
			// calculate double modulus: thx to http://bytes.com/topic/c/answers/495889-modulus-double-variables ??@chris -> so was in Refs?
				int temp = static_cast<int>( (neuronOutput - minOutputRange) / bucketIncrement);
				double doubleMod = neuronOutput - minOutputRange - static_cast<double>(temp) * bucketIncrement;

			// round:
				if (doubleMod < 0.5 * bucketIncrement){
					neuronOutput = neuronOutput  - doubleMod;
				}else{ 
					neuronOutput = neuronOutput + bucketIncrement- doubleMod;
				}
				mData->set(1, i, int((neuronOutput + bucketIncrement - minOutputRange)/bucketIncrement), 0); 
// //  			cout<<"\n huh?/* \n";
// 			cerr<<i;
// 			cerr<<"\n"; 
// 			cerr<<int((neuronOutput + bucketIncrement - minOutputRange)/bucketIncrement);
// 			cerr<<"\n";
				//   			cerr<<neuronOutput;//
// 			cerr<<"\n";
// //  			cout<<" <- neuronOutput\n bucketno.:\n ";


			//TODO Abbruchbedingung hinzufügen1


			//this executes the neural network once.
				mEvaluateNetworkEvent->trigger();
			
			//This is important: allows to quit the application while the plotter is running.
				if(core->isShuttingDown()) {
					return;
				}			
			//This is important: it keeps the system alive!
				core->executePendingTasks();
			}
			r = r + variationIncrement; 
	
		//This is important: allows to quit the application while the plotter is running.
			if(Core::getInstance()->isShuttingDown()) {
				return;
			}

		//This is important: it keeps the system alive!
			core->executePendingTasks();








		//store the current output activation distribution.
// 		storeCurrentNetworkActivities();
/*
			double previousBias = variedNeuron->getBiasValue().get(); 
// 		variedNeuron->getBiasValue().set(previousBias + minBias + (i * variationIncrement));
		
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

// 		mData->set(i * variationIncrement, i, 0, 0); 
// 		mData->set(result, i, 1, 0);

			variedNeuron->getBiasValue().set(previousBias);
*/

		//reset the network to the initial output activation distribution.
// 		restoreCurrentNetworkActivites();	


		} //for loop
		restoreCurrentNetworkActivites();


	}


}


