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



#include "Bifurcation_Calculator.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QTime>
#include "Value/DoubleValue.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "math.h"


using namespace std;

namespace nerd {


	/**
	 * Constructs a new Bifurcation_Calculator.
 	 */
	Bifurcation_Calculator::Bifurcation_Calculator()
	: DynamicsPlotter("Bifurcation_Calculator")
	{
// 		mData = new MatrixValue();
		mIdOfObservedNeuron = new ULongLongValue(0);
		
		mIdsOfVariedNetworkElements = new StringValue("0"); //comma or |-separated list of network element (either neuron or synapse) IDs.
		mMinimaOfVariedNetworkElements = new StringValue("0"); //comma-separated list of minimum values for the parameter change. These must be in same sequence as IDs.
		mMaximaOfVariedNetworkElements = new StringValue("2"); //maxima 
		mPrerunSteps = new DoubleValue(100);//Number of steps the network takes before started searching for attractors

		mPlotPixelsX = new IntValue(600); //accuracy of parameter variation, no. of pixels in x-dimension of diagram
		mMinOutputRange = new DoubleValue(-1.0); //Adjust for other transfer function than tanh - is now set for tanh
		mMaxOutputRange = new DoubleValue(1.0); // -- " --
		mPlotPixelsY = new IntValue(500); // number of values the output is discretized/rounded to (is number of pixels on y-axis)
		mResetToInitState = new BoolValue(false); // reset to initial activity state after every parameter change; if false: uses last state ("follows attractor")
		mMaxSteps = new IntValue(500); // defines maximal number of steps that are executed
		mTolerance = new DoubleValue(0.00001);//Specifies how large the margin is, such that the two outputs count as the same one:   x = x +/- mTolerance
		mBidirectional = new BoolValue(false);
		mXAxisDescription->set("Parameter");//setting initial axes descriptions
		mYAxisDescription->set("Output");
		
		mIdOfObservedNeuron->setDescription("ID of the neuron that is observed.");
		mIdsOfVariedNetworkElements->setDescription("List of the IDs of the network elements (neurons or synapse) which are varied separated by commas or '|'.");
		mMinimaOfVariedNetworkElements->setDescription("Comma-separated list of the minimal values of the varied network elements");
		mMaximaOfVariedNetworkElements->setDescription("Comma-separated list of the maximal values of the varied network elements");
		mPlotPixelsX->setDescription("Horizontal size of plot, also determines the step size of the parameter change (max - min)/plotPixels");
		mPlotPixelsY->setDescription("Vertical size of plot, also determines the number of output buckets to which the output is discretesized to.");
		mResetToInitState->setDescription("If TRUE then the network activity is reset after every network step.");
		mMaxSteps->setDescription("Maximal number of steps taken, if no attractor is found");
		mTolerance->setDescription("Tolerance for that two values are taken as the same; x = x +/- tol");
		mMinOutputRange->setDescription("Minimum of output range");
		mMaxOutputRange->setDescription("Maximum of output range");
		mBidirectional->setDescription("If TRUE, the calculator runs twice. After the standard run, parameter min and max are switched, thus the algorithm is backwards.");
		mPrerunSteps->setDescription("Number of steps before search for attractors is started.");
		
// 		addParameter("Data", mData, true);
		addParameter("IdOfObservedNeuron", mIdOfObservedNeuron, true);	
		
		addParameter("IdsOfVariedNetworkElements", mIdsOfVariedNetworkElements, true);
		addParameter("MinimaOfVariedNetworkElements", mMinimaOfVariedNetworkElements, true);
		addParameter("MaximaOfVariedNetworkElements", mMaximaOfVariedNetworkElements, true);
						
		addParameter("MinOutputRange", mMinOutputRange, true); 
		addParameter("MaxOutputRange", mMaxOutputRange, true);
		addParameter("PlotPixelsX", mPlotPixelsX, true); 
		addParameter("PlotPixelsY", mPlotPixelsY, true); 
		addParameter("Tolerance", mTolerance, true); 
		addParameter("ResetToInitState", mResetToInitState, true);
		addParameter("MaxSteps", mMaxSteps, true);
		addParameter("Bidirectional", mBidirectional, true);
		addParameter("PrerunSteps", mPrerunSteps, true);
		
		
	}




/**
	 * Destructor.
 */
	Bifurcation_Calculator::~Bifurcation_Calculator() {
	}


	
/**
	 * Calculates the output data
 */
	void Bifurcation_Calculator::calculateData() {
		if(mNextStepEvent == 0 || mResetEvent == 0 || mEvaluateNetworkEvent == 0) {
			return;
		}

		NeuralNetwork *network = getCurrentNetwork();

		if(network == 0) {
			Core::log("Bifurcation_Calculator: Could not find a modular neural network!", true);
			return;
		}
		
		QList<ULongLongValue*>vIdsList = createListOfIds(mIdsOfVariedNetworkElements);//list of IDs of varied elements
		QList<double> vMinsList = createListOfDoubles(mMinimaOfVariedNetworkElements);
		QList<double> vMaxsList = createListOfDoubles(mMaximaOfVariedNetworkElements);
		//check IDs, minima, and maxima
		if(!checkStringlistsItemCount(mIdsOfVariedNetworkElements, mMinimaOfVariedNetworkElements, mMaximaOfVariedNetworkElements)){
			Core::log("Bifurcation_Calculator: The number of IDs, minima and maxima must be the same!", true);		
			return;
		}
		int noOfvElems = vIdsList.size();//No. of varied elements
		
		QList<NeuralNetworkElement*> vElemsList;
		//get pointer to the varied neuron or synapse
		for(int j = 0; j < noOfvElems; j++){
			vElemsList.append(getVariedNetworkElement(vIdsList[j]));	
			if(getVariedNetworkElement(vIdsList[j]) == 0){
				Core::log("Bifurcation_Calculator: Could not find required (varied) neurons!", true);		
				return;	
			}
		}
// 		NeuralNetworkElement *variedElem = getVariedNetworkElement(mIdOfVariedNetworkElement); //inherited by parent class DynamicsPlotter

		
		//Get pointers to the output neuron:
		Neuron *observedNeuron = NeuralNetwork::selectNeuronById(mIdOfObservedNeuron->get(),
				network->getNeurons());
		
		if(observedNeuron == 0) {
			Core::log("Bifurcation_Calculator: Could not find required neurons (observed)!", true);		
			return;
		}
		
		//prepare variables for evaluations
		int numberNeurons = network->getNeurons().count(); //no. of neurons in network
		int maxSteps = mMaxSteps->get();
		bool resetToInitState = mResetToInitState->get();
		double prerunSteps = mPrerunSteps->get();


		//bias paramater variables
		int plotPixelsX = mPlotPixelsX->get();	
		if(plotPixelsX < 2){
			Core::log("Bifurcation_Calculator: Size of diagram must be over 1 pixel", true);
			return;
		}
		
		QList<double> xIncrements; //List of increments of all varied elements
		for(int j = 0; j < noOfvElems; j++){
			xIncrements.append((vMaxsList[j] - vMinsList[j]) / ((double) plotPixelsX - 1)); //'-1' to include min and max values in steps
		}

		QList<double> rList; // running parameters of all varied elements
		for(int j = 0; j < noOfvElems; j++){
			if(vMinsList[j] != vMaxsList[j]){
				rList.append(vMinsList[j]);
			}else{
				Core::log("Bifurcation_Calculator: Minimum of variation must not be equal to maximum.", true);
				return;
			}
		}

		//output-range variables:
		int plotPixelsY = mPlotPixelsY->get(); 
		if(plotPixelsY < 2){ // must > 1 for division (avoid division by 0)
			Core::log("Bifurcation_Calculator: Size of diagram must be over 1 pixel", true);
			return;
		}
		double minOutputRange = mMinOutputRange->get();
		double maxOutputRange = mMaxOutputRange->get();
		if(minOutputRange > maxOutputRange){ 
			Core::log("Bifurcation_Calculator: Minimal value for output range must be smaller than maximal value", true);
			return;
		}
		double yIncrement = (maxOutputRange - minOutputRange) / ((double) plotPixelsY - 1); //'-1' to include min and max; gives the range of each pixel in y-dimension

		Core *core = Core::getInstance();

		QList<double> oldValues; //for restoration of initial bias or synapse strength value
		for(int j = 0; j < noOfvElems; j++){
			oldValues.append(getVariedNetworkElementValue(vElemsList[j]));
		}
		
		//prepare variables needed in calculations:
		double neuronOutput = 0.0;
		double temp = 0.0;
		double tempMatrix[maxSteps][numberNeurons];
		bool attractorFound = false;
		int periodLength = 0;
		double doubleMod = 0.0;
		int numberOfSteps = 0;

		
		QList<Neuron*> neuronsList = network->getNeurons(); //list of all neurons
		
		//get index of observed neuron in list:
		qulonglong posOutputNeuron = -1;
		for(int k = 0; k < numberNeurons; k++){ 
			if(static_cast<nerd::Neuron*>(neuronsList.at(k))->getId() == mIdOfObservedNeuron->get()){
				posOutputNeuron = k;
				break;
			}
		}
		
		//set up matrix:
		mData->clear();
		//matrix is as large as diagram (in pixels) + 1
		mData->resize(plotPixelsX + 1, plotPixelsY + 1, 1); //first dimension = no. of parameter changes; second dimension = number of output buckets
		mData->fill(0);
		for(int j = 1; j < plotPixelsY + 1; ++j){
			mData->set(minOutputRange + (j - 1) * yIncrement, 0, j, 0); //set first matrix column: indices
		}
		
		storeCurrentNetworkActivities();
		//evaluate:
		for(int i = 0;  i < plotPixelsX; i++) { //runs through parameter changes
			//change parameter:
			for(int j = 0; j < noOfvElems; j++){ 
				setVariedNetworkElementValue(vElemsList[j], rList[j]); //inherited by parent class DynamicsPlotter
			}		
			
			//set first matrix row: indices of x-axis 
			if(noOfvElems == 1){
				mData->set(rList[0], i + 1, 0, 0);//if only one parameter is changed, take its values
			}else{
				mData->set(i + 1, i + 1, 0, 0); //if more, take pixelcount
			}
			
			if(resetToInitState) restoreCurrentNetworkActivites(); //if false, then the last value is used 
			for(int j = 0; j < prerunSteps; j++){//pre run prerunSteps
				mEvaluateNetworkEvent->trigger();
			}
			attractorFound = false;
			for(int j = 0; j < maxSteps && attractorFound == false; j++){ //evaluate network with current parameters
				//this executes the neural network once.
				mEvaluateNetworkEvent->trigger();
				
				for(int k = 0; k < numberNeurons; k++){ //add all neuron activations to temporary matrix
					tempMatrix[j][k] = neuronsList.at(k)->getOutputActivationValue().get(); 
				}
				
				//check if network state appeared before:
				for(int m = j - 1; m >= 0 && attractorFound == false;--m){ 
					attractorFound = true;
					for(int k = 0; k < numberNeurons && attractorFound == true; k++){
						if(fabs(tempMatrix[m][k] - tempMatrix[j][k]) > fabs(mTolerance->get())){ // if one neuron's output is unequal to its output at the respective former time step
							attractorFound = false;	// break inner for-loop
						}
					}
					periodLength = j - m; //period length of found attractor
				}
				numberOfSteps = j; 



				//This is important: allows to quit the application while the plotter is running.
				if(core->isShuttingDown()) {
					return;
				}			
				//This is important: it keeps the system alive!
				core->executePendingTasks();
			
			}//for-loop: evaluation

		
			if(attractorFound){
				//save outputs from the last step to the one where the repetition was found
				for(int j = numberOfSteps - periodLength + 1; j <= numberOfSteps; j++){
					neuronOutput = tempMatrix[j][posOutputNeuron]; //get output activation of output neuron of every time step
					//find pixel for neuronOutput:
					//calculate double modulus: thx to http://bytes.com/topic/c/answers/495889-modulus-double-variables ??@chris -> so was in Refs?
					if(neuronOutput < minOutputRange || neuronOutput > maxOutputRange){
					}else{
						temp = static_cast<int>( (neuronOutput - minOutputRange) / yIncrement); //how often fits the bucket in neuronOutput (starting at minOutputRange)
						doubleMod = neuronOutput - minOutputRange - static_cast<double>(temp) * yIncrement; //get modulus neuronOutput%yIncrement
						//round:
						if (doubleMod < 0.5 * yIncrement){
							neuronOutput = neuronOutput  - doubleMod; //round to a multiple of yIncrement (starting at minOutputRange)
						}else{ 
							neuronOutput = neuronOutput + yIncrement- doubleMod;
						}
						
						
						mData->set(1, i + 1, int((neuronOutput + yIncrement - minOutputRange)/yIncrement + 0.5), 0); 
					}
				}
			}else{
				//no attractor found - plot all activations of output neuron
				for(int j = 0; j < maxSteps; j++){
					neuronOutput = tempMatrix[j][posOutputNeuron]; //get output activation of output neuron of every time step
					if(neuronOutput < minOutputRange || neuronOutput > maxOutputRange){
						//do nothing
					}else{
						//find pixel for neuronOutput:
						//calculate double modulus: thx to http://bytes.com/topic/c/answers/495889-modulus-double-variables ??@chris -> so was in Refs?
						temp = static_cast<int>( (neuronOutput - minOutputRange) / yIncrement);
						doubleMod = neuronOutput - minOutputRange - static_cast<double>(temp) * yIncrement;
						//round:
						if (doubleMod < 0.5 * yIncrement){
							neuronOutput = neuronOutput  - doubleMod;
						}else{ 
							neuronOutput = neuronOutput + yIncrement- doubleMod;
						}
						
							mData->set(1, i + 1, int((neuronOutput + yIncrement - minOutputRange)/yIncrement + 0.5), 0); 
					}
				}
			}//if-statement
	
			for(int j = 0; j < noOfvElems; j++){
				rList[j] = rList[j] + xIncrements[j]; //set new parameter values
			}
			
			//This is important: allows to quit the application while the plotter is running.
			if(Core::getInstance()->isShuttingDown()) {
				return;
			}

			//This is important: it keeps the system alive!
			core->executePendingTasks();

		} //for-loop
		
		if(mBidirectional->get()){
// 			restoreCurrentNetworkActivites(); //??
			//Prepare parameters:
			rList.clear();
			for(int j = 0; j < noOfvElems; j++){
				rList.append(vMaxsList[j]);
				xIncrements[j] = -xIncrements[j];
			}
			
			
			for(int i = plotPixelsX - 1;  i >= 0 ; i--) { //runs through parameter changes
			//change parameter:
				for(int j = 0; j < noOfvElems; j++){ 
					setVariedNetworkElementValue(vElemsList[j], rList[j]); //inherited by parent class DynamicsPlotter
				}		
			
				if(resetToInitState) restoreCurrentNetworkActivites(); //if false, then the last value is used 
			
				attractorFound = false;
				for(int j = 0; j < maxSteps && attractorFound == false; j++){ //evaluate network with current parameters
				//this executes the neural network once.
					mEvaluateNetworkEvent->trigger();
				
					for(int k = 0; k < numberNeurons; k++){ //add all neuron activations to temporary matrix
						tempMatrix[j][k] = neuronsList.at(k)->getOutputActivationValue().get(); 
					}
				
				//check if network state appeared before:
					for(int m = j - 1; m >= 0 && attractorFound == false;--m){ 
						attractorFound = true;
						for(int k = 0; k < numberNeurons && attractorFound == true; k++){
							if(fabs(tempMatrix[m][k] - tempMatrix[j][k]) > fabs(mTolerance->get())){ // if one neuron's output is unequal to its output at the respective former time step
								attractorFound = false;	// break inner for-loop
							}
						}
						periodLength = j - m; //period length of found attractor
					}
					numberOfSteps = j; 



				//This is important: allows to quit the application while the plotter is running.
					if(core->isShuttingDown()) {
						return;
					}			
				//This is important: it keeps the system alive!
					core->executePendingTasks();
			
				}//for-loop: evaluation

		
				if(attractorFound){
				//save outputs from the last step to the one where the repetition was found
					for(int j = numberOfSteps - periodLength + 1; j <= numberOfSteps; j++){
						neuronOutput = tempMatrix[j][posOutputNeuron]; //get output activation of output neuron of every time step
					//find pixel for neuronOutput:
					//calculate double modulus: thx to http://bytes.com/topic/c/answers/495889-modulus-double-variables ??@chris -> so was in Refs?
						if(neuronOutput < minOutputRange || neuronOutput > maxOutputRange){
						}else{
							temp = static_cast<int>( (neuronOutput - minOutputRange) / yIncrement); //how often fits the bucket in neuronOutput (starting at minOutputRange)
							doubleMod = neuronOutput - minOutputRange - static_cast<double>(temp) * yIncrement; //get modulus neuronOutput%yIncrement
						//round:
							if (doubleMod < 0.5 * yIncrement){
								neuronOutput = neuronOutput  - doubleMod; //round to a multiple of yIncrement (starting at minOutputRange)
							}else{ 
								neuronOutput = neuronOutput + yIncrement- doubleMod;
							}
						
						
							mData->set(1, i + 1, int((neuronOutput + yIncrement - minOutputRange)/yIncrement + 0.5), 0); 
						}
					}
				}else{
				//no attractor found - plot all activations of output neuron
					for(int j = 0; j < maxSteps; j++){
						neuronOutput = tempMatrix[j][posOutputNeuron]; //get output activation of output neuron of every time step
						if(neuronOutput < minOutputRange || neuronOutput > maxOutputRange){
						//do nothing
						}else{
						//find pixel for neuronOutput:
						//calculate double modulus: thx to http://bytes.com/topic/c/answers/495889-modulus-double-variables ??@chris -> so was in Refs?
							temp = static_cast<int>( (neuronOutput - minOutputRange) / yIncrement);
							doubleMod = neuronOutput - minOutputRange - static_cast<double>(temp) * yIncrement;
						//round:
							if (doubleMod < 0.5 * yIncrement){
								neuronOutput = neuronOutput  - doubleMod;
							}else{ 
								neuronOutput = neuronOutput + yIncrement- doubleMod;
							}
						
							mData->set(1, i + 1, int((neuronOutput + yIncrement - minOutputRange)/yIncrement + 0.5), 0); 
						}
					}
				}//if-statement
	
				for(int j = 0; j < noOfvElems; j++){
					rList[j] = rList[j] + xIncrements[j]; //set new parameter values
				}
			
			//This is important: allows to quit the application while the plotter is running.
				if(Core::getInstance()->isShuttingDown()) {
					return;
				}

			//This is important: it keeps the system alive!
				core->executePendingTasks();

			} //for-loop
			
		}//if-clause: bidirectional
		
		
		
		//restore former bias values and synapse strengths
		for(int j = 0; j < noOfvElems; j++){
			setVariedNetworkElementValue(vElemsList[j], oldValues[j]);
		}
		restoreCurrentNetworkActivites();
		
	}//calculateData()


}



