/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   tfaber@uni-osnabrueck.de
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



#include "Transients_Calculator.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/DoubleValue.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new Transients_Calculator.
 */
	Transients_Calculator::Transients_Calculator(): DynamicsPlotter("Transients_Calculator")
	{
		mData = new MatrixValue();
		mIdOfObservedNeuron = new ULongLongValue(0);
		mNoOfStepsX = new IntValue(600); // No. of time steps, x-size of plot in pixels
		mMinOutputRange = new DoubleValue(-1.0); //in case other transfer function than tanh - is now set for tanh
		mMaxOutputRange = new DoubleValue(1.0); // -- " --
		mPlotPixelsY = new IntValue(500); // number of values the output is discretized/rounded to (is number of pixels)
		
		addParameter("Data", mData, true);
		addParameter("IdOfObservedNeuron", mIdOfObservedNeuron, true);	
		addParameter("NoOfStepsX", mNoOfStepsX, true);
		addParameter("MinOutputRange", mMinOutputRange, true); 
		addParameter("MaxOutputRange", mMaxOutputRange, true);
		addParameter("PlotPixelsY", mPlotPixelsY, true); 
		
	}

/**
 * Destructor.
 */
	Transients_Calculator::~Transients_Calculator() {
	}

	/**
	 * Calculates the output data
	 */
	void Transients_Calculator::calculateData() {
		

		Core *core = Core::getInstance();
		
		if(mNextStepEvent == 0 || mResetEvent == 0 || mEvaluateNetworkEvent == 0) {
			return;
		}

		NeuralNetwork *network = getCurrentNetwork();

		if(network == 0) {
			Core::log("Transients_Calculator: Could not find a modular neural network!", true);
			return;
		}
	

		//Get pointers to the output neuron:
		Neuron *observedNeuron = NeuralNetwork::selectNeuronById(mIdOfObservedNeuron->get(),
				network->getNeurons());
		if(observedNeuron == 0) {
			Core::log("Transients_Calculator: Could not find required neurons (varied / observed)!", true);		
			return;
		}
		double minOutputRange = mMinOutputRange->get();
		double maxOutputRange = mMaxOutputRange->get();
		int plotPixelsX = mNoOfStepsX->get(); //No. of pixels on x-axis
		int plotPixelsY = mPlotPixelsY->get();
		double yIncrement = (maxOutputRange - minOutputRange) / ((double) plotPixelsY - 1); //'-1' to include min and max; gives the increment of each pixel in y-dimension

		//set up matrix:
		mData->resize(1, 1, 1); //clear matrix //nötig? gibts clear
		//matrix is as large as diagram (in pixels) + 1
		mData->resize(plotPixelsX + 1, plotPixelsY + 1, 1); //first dimension = no. of parameter changes; second dimension = number of output buckets
		for(int j = 1; j < plotPixelsY + 1; ++j){
			mData->set(minOutputRange + (j - 1) * yIncrement, 0, j, 0); //set first matrix column: indices	
		}
				
		double neuronOutput = 0.0;
		double temp = 0.0;
		double doubleMod = 0.0;
		
		storeCurrentNetworkActivities();
		//TODO variedElement-wert zurücksetzen, wenn durchgelaufen
		
		//evaluate
		for(int i = 0;  i < plotPixelsX; i++) { //runs through parameter changes
			mData->set(i+1, i + 1, 0, 0); //set first matrix row: indices 

			//this executes the neural network once.
			mEvaluateNetworkEvent->trigger();

			//This is important: allows to quit the application while the plotter is running.
			if(core->isShuttingDown()) {
				return;
			}			
			//This is important: it keeps the system alive!
			core->executePendingTasks();
			
			//find pixel for neuronOutput:
			//calculate double-modulus: thx to http://bytes.com/topic/c/answers/495889-modulus-double-variables ??@chris -> so was in Refs?
			neuronOutput = observedNeuron->getOutputActivationValue().get();
			temp = static_cast<double>(static_cast<int>( (neuronOutput - minOutputRange) / yIncrement)); //how often fits the bucket in neuronOutput (starting at minOutputRange)
			doubleMod = neuronOutput - minOutputRange - static_cast<double>(temp) * yIncrement; //get modulus neuronOutput%yIncrement
			//round:
			if (doubleMod < 0.5 * yIncrement){
				neuronOutput = neuronOutput  - doubleMod; //round to a multiple of yIncrement (starting at minOutputRange)
			}else{ 
				neuronOutput = neuronOutput + yIncrement- doubleMod;
			}
			mData->set(1, i + 1, int((neuronOutput + yIncrement - minOutputRange)/yIncrement + 0.5), 0); 
				
	
//		This is important: allows to quit the application while the plotter is running.
			if(Core::getInstance()->isShuttingDown()) {
				return;
			}

//		This is important: it keeps the system alive!
			core->executePendingTasks();

		} //for-loop
		restoreCurrentNetworkActivites();

	}
}



