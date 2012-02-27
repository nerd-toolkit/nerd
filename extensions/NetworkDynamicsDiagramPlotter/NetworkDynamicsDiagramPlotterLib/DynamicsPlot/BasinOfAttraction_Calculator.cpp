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


#include "BasinOfAttraction_Calculator.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new BasinOfAttraction_Calculator.
 */
	BasinOfAttraction_Calculator::BasinOfAttraction_Calculator() : DynamicsPlotter("BasinOfAttraction_Calculator")
	{
		// IDs, minima, maxima for network elements, that will be plotted on x-axis:
		mIdsOfVariedNeuronsX = new StringValue("0, 0 | 0");
		mMinimaOfVariedNeuronsX = new StringValue("0 , 0, 0");
		mMaximaOfVariedNeuronsX = new StringValue("1, 1, 1");
		// same for y-axis:
		mIdsOfVariedNeuronsY = new StringValue("0");
		mMinimaOfVariedNeuronsY = new StringValue("0");
		mMaximaOfVariedNeuronsY = new StringValue("1");
		
		mPrerunSteps = new DoubleValue(100);//Number of steps the network takes before started searching for attractors
		mPlotPixelsX = new IntValue(600); //accuracy of x-parameter variation, no. of pixels in x-dimension of diagram
		mPlotPixelsY = new IntValue(500); //same for y
		mResetToInitState = new BoolValue(false); // reset to initial activity state after every parameter change; if false: uses last state ("follows attractor")
		mMaxSteps = new IntValue(250); // defines size of maximal steps that are executed
		mTolerance = new DoubleValue(0.000001);//Specifies how large the margin is, such that the two outputs count as the same one 
		mMaxPeriod = new IntValue(16); //max. size of period that can be found
		mXAxisDescription->set("Param. 1");//setting initial axes descriptions
		mYAxisDescription->set("Param. 2");
		
		mIdsOfVariedNeuronsX->setDescription("Comma-separated list of IDs of the neurons that are varied (x-axis).");
		mMinimaOfVariedNeuronsX->setDescription("Comma-separated list of the minimal values of the varied network elements (x-axis)");
		mMaximaOfVariedNeuronsX->setDescription("Comma-separated list of the maximal values of the varied network elements (x-axis)");
		mIdsOfVariedNeuronsY->setDescription("Comma-separated list of IDs of the neurons that are varied (y-axis).");
		mMinimaOfVariedNeuronsY->setDescription("Comma-separated list of the minimal values of the varied network elements (y-axis)");
		mMaximaOfVariedNeuronsY->setDescription("Comma-separated list of the maximal values of the varied network elements (y-axis)");
		mPlotPixelsX->setDescription("Horizontal size of plot, also determines the step size of the parameter change (max - min)/plotPixels");
		mPlotPixelsY->setDescription("Vertical size of plot, also determines the step size of the parameter change (max - min)/plotPixels");
		mResetToInitState->setDescription("If TRUE then the network activity is reset after every network step.");
		mMaxSteps->setDescription("Maximal number of steps taken, if no attractor is found");
		mTolerance->setDescription("Tolerance for that two values are taken as the same; x = x +/- tol");
		mMaxPeriod->setDescription("Maximal size of period that will be found.");
		mPrerunSteps->setDescription("Number of steps before search for attractors is started.");

		addParameter("XIdsOfVariedNeurons", mIdsOfVariedNeuronsX, true);
		addParameter("XMinimaOfVariedNeurons", mMinimaOfVariedNeuronsX, true);
		addParameter("XMaximaOfVariedNeurons", mMaximaOfVariedNeuronsX, true);
		addParameter("YIdsOfVariedNeurons", mIdsOfVariedNeuronsY, true);
		addParameter("YMinimaOfVariedNeurons", mMinimaOfVariedNeuronsY, true);
		addParameter("YMaximaOfVariedNeurons", mMaximaOfVariedNeuronsY, true);
		addParameter("PlotPixelsX", mPlotPixelsX, true); 
		addParameter("PlotPixelsY", mPlotPixelsY, true); 
		addParameter("Tolerance", mTolerance, true); 
		addParameter("ResetToInitState", mResetToInitState, true);
		addParameter("MaxSteps", mMaxSteps, true);
		addParameter("MaxPeriod", mMaxPeriod, true);
		addParameter("PrerunSteps", mPrerunSteps, true);
	}



/**
 * Destructor.
 */
	BasinOfAttraction_Calculator::~BasinOfAttraction_Calculator() {
	}

	/**
	 * Calculates the output data
	 */
	void BasinOfAttraction_Calculator::calculateData() {
		if(mNextStepEvent == 0 || mResetEvent == 0 || mEvaluateNetworkEvent == 0) {
			return;
		}

		NeuralNetwork *network = getCurrentNetwork();

		if(network == 0) {
			Core::log("BasinOfAttractionCalculator: Could not find a modular neural network!", true);
			return;
		}
		
		QList<ULongLongValue*>vIdsListX = createListOfIds(mIdsOfVariedNeuronsX);//list of IDs of varied elements
		QList<double> vMinsListX = createListOfDoubles(mMinimaOfVariedNeuronsX);
		QList<double> vMaxsListX = createListOfDoubles(mMaximaOfVariedNeuronsX);
		
		QList<ULongLongValue*>vIdsListY = createListOfIds(mIdsOfVariedNeuronsY);//list of IDs of varied elements
		QList<double> vMinsListY = createListOfDoubles(mMinimaOfVariedNeuronsY);
		QList<double> vMaxsListY = createListOfDoubles(mMaximaOfVariedNeuronsY);
		
		//check IDs, minima, and maxima
		if(!checkStringlistsItemCount(mIdsOfVariedNeuronsX, mMinimaOfVariedNeuronsX, mMaximaOfVariedNeuronsX)){
			Core::log("BasinOfAttractionCalculator: The number of IDs, minima and maxima (x-axis parameters) must be the same!", true);		
			return;
		}
		if(!checkStringlistsItemCount(mIdsOfVariedNeuronsY, mMinimaOfVariedNeuronsY, mMaximaOfVariedNeuronsY)){
			Core::log("BasinOfAttractionCalculator: The number of IDs, minima and maxima (y-axis parameters) must be the same!", true);		
			return;
		}
		
		int noOfvNeuronsX = vIdsListX.size();//No. of neurons
		int noOfvNeuronsY = vIdsListY.size();
		
		for(int j = 0; j < noOfvNeuronsX; j++){
			for(int k = 0; k < noOfvNeuronsY; k++){
				if(vIdsListX[j]->get() == vIdsListY[k]->get()){
					Core::log("BasinOfAttraction_Calculator: Warning! IDs should not be used in both parameters!", true);
				}
			}
		}
		
		QList<Neuron*> vNeuronsListX; //list of neurons
		QList<Neuron*> vNeuronsListY;
	
		//get pointer to the varied neurons on the x-axis
		for(int j = 0; j < noOfvNeuronsX; j++){
			vNeuronsListX.append(NeuralNetwork::selectNeuronById(vIdsListX[j]->get(), network->getNeurons()));	
			if(vNeuronsListX[j] == 0){
				Core::log("BasinOfAttractionCalculator: Could not find required (varied) neurons (x-axis)!", true);		
				return;	
			}
		}
		//y-axis
		for(int j = 0; j < noOfvNeuronsY; j++){
			vNeuronsListY.append(NeuralNetwork::selectNeuronById(vIdsListY[j]->get(), network->getNeurons()));	
			if(vNeuronsListY[j] == 0){
				Core::log("BasinOfAttractionCalculator: Could not find required (varied) neurons(y-axis)!", true);		
				return;	
			}
		}	
		
		//prepare variables for evaluations
		int numberNeurons = network->getNeurons().count();
		int maxSteps = mMaxSteps->get();
		int maxPeriod = mMaxPeriod->get();
		double prerunSteps = mPrerunSteps->get();

		if(maxPeriod > maxSteps){
			Core::log("BasinOfAttractionCalculator: MaxPeriod must not be larger that max steps.", true);
			return;	
		}
		bool resetToInitState = mResetToInitState->get();

		int plotPixelsX = mPlotPixelsX->get();
		int plotPixelsY = mPlotPixelsY->get();		
		
		if(plotPixelsX < 2){
			Core::log("BasinOfAttractionCalculator: Size of diagram must be over 1 pixel (x-axis).", true);
			return;
		}
		if(plotPixelsY < 2){
			Core::log("BasinOfAttractionCalculator: Size of diagram must be over 1 pixel (y-axis).", true);
			return;
		}	

		QList<double> xIncrements; //range of pixels
		QList<double> yIncrements;
		for(int j = 0; j < noOfvNeuronsX; j++){
			xIncrements.append((vMaxsListX[j] - vMinsListX[j]) / ((double) plotPixelsX - 1)); //'-1' to include min and max values in steps
		}
		for(int j = 0; j < noOfvNeuronsY; j++){
			yIncrements.append((vMaxsListY[j] - vMinsListY[j]) / ((double) plotPixelsY - 1)); //'-1' to include min and max values in steps
		}	

		QList<double> rListX; // running parameters for varied elements
		QList<double> rListY;
		for(int j = 0; j < noOfvNeuronsX; j++){
			if(vMinsListX[j] != vMaxsListX[j]){
				rListX.append(vMinsListX[j]);
			}else{
				Core::log("BasinOfAttractionCalculator: Minimum of variation must not be equal to maximum.", true);
				return;
			}
		}
		for(int j = 0; j < noOfvNeuronsY; j++){
			if(vMinsListY[j] != vMaxsListY[j]){
				rListY.append(vMinsListY[j]);
				
			}else{
				Core::log("BasinOfAttractionCalculator: Minimum of variation must not be equal to maximum.", true);
				return;
			}
		}
		QList<double> rListYInit = rListY; //Save initial parameter list, parameter of y-elements are reset after every step on x-axis
		Core *core = Core::getInstance();

		
		double tempMatrix[maxSteps][numberNeurons];
		bool attractorFound = false;
		int periodLength = 0;

		QList<Neuron*> neuronsList = network->getNeurons();
		//matrix that stores one state of all found attractors, in the worst case plotPixelsX*plotPixelsY 
		double attrMatrix[plotPixelsX * plotPixelsY][numberNeurons]; //only one state is saved, since if one state is the same, all others must be, too
		int attractorCount = 0; //counts the attractors
		//set up matrix:
		mData->clear();
		//matrix is as large as diagram (in pixels) + 1
		mData->resize(plotPixelsX + 1, plotPixelsY + 1, 1); //first dimension = no. of parameter changes; second dimension = number of output buckets
		mData->fill(0);
		
		storeCurrentNetworkActivities();
		//evaluate
		for(int i = 0;  i < plotPixelsX; i++) { //runs through x-parameter changes
			if(!mActiveValue->get()) {
				//stop evaluation when the user wants to cancel it.
				restoreCurrentNetworkActivites();
				return;
			}

			//set first matrix row: indices of x-axis 
			if(noOfvNeuronsX == 1){
				mData->set(rListX[0], i + 1, 0, 0);//if only one parameter is changed, take its values
			}else{
				mData->set(i + 1, i + 1, 0, 0); //if more, take pixelcount
			}
			rListY = rListYInit; //reset parameters
			for(int l = 0; l < plotPixelsY; l++){//runs through y-parameter changes	
				if(resetToInitState) restoreCurrentNetworkActivites(); //if false, then the last activity state is used 
				for(int j = 0; j < noOfvNeuronsY; j++){//set outputs of varied neurons
					vNeuronsListY[j]->getOutputActivationValue().set(rListY[j]); 
				}
				for(int j = 0; j < noOfvNeuronsX; j++){
					vNeuronsListX[j]->getOutputActivationValue().set(rListX[j]); 
				}
				
				if(i == 0 && noOfvNeuronsY == 1){ //set first matrix column
					mData->set(rListY[0], 0, l + 1, 0);//if only one parameter is changed, take its values
				}else if(i == 0){
					mData->set(l + 1, 0, l + 1, 0); //if more, take pixelcount
				}

				for(int j = 0; j < prerunSteps; j++){//pre run prerunSteps
					mEvaluateNetworkEvent->trigger();
				}
				
				int steps;
				attractorFound = false;
				for(int j = 0; j < maxSteps && attractorFound == false; j++){//look for attractor	
					//this executes the neural network once.
					mEvaluateNetworkEvent->trigger();
					
					for(int k = 0; k < numberNeurons; k++){ //add all neuron activations to temporary matrix
						tempMatrix[j][k] = neuronsList.at(k)->getOutputActivationValue().get(); 
					}
	
					for(int m = j - 1; m > j - maxPeriod && m >= 0 && attractorFound == false;--m){ //check if network state appeared before
						attractorFound = true;
						for(int k = 0; k < numberNeurons && attractorFound == true; k++){
							if(fabs(tempMatrix[m][k] - tempMatrix[j][k]) > fabs(mTolerance->get())){ // if one neuron's output is unequal to its output at the respective former time step
								attractorFound = false;	// break inner for-loop
							}
						}
						periodLength = j - m;
					}
					//This is important: allows to quit the application while the plotter is running.
					if(core->isShuttingDown()) {
						return;
					}			
					//This is important: it keeps the system alive!
					core->executePendingTasks();
					steps = j;
				}//attractor loop
				
				
				for(int j = 0; j < noOfvNeuronsY; j++){
					rListY[j] = rListY[j] + yIncrements[j]; 
				}

				
				if(attractorFound){//update data matrix
					bool oldAttractor = false;
					int attrNo = 0; 
					if(attractorCount != 0){//check if the current attractor is already known:
						for(int k = 0; k < attractorCount && oldAttractor == false; k++){//for all attractors
							
							for(int m = steps; m >= steps - periodLength + 1 && oldAttractor == false; m--){//for every state that is part of the attractor
								oldAttractor = true;	
								for(int j = 0; j < numberNeurons && oldAttractor == true; j++){//for all neurons
									if(fabs(attrMatrix[k][j] - tempMatrix[m][j]) > fabs(mTolerance->get())){
										oldAttractor = false;
									}
								}
							}
							
							attrNo = k + 1;//hier?
						}
						if(oldAttractor == false){
							for(int j = 0; j < numberNeurons; j++){
								attrMatrix[attractorCount + 1][j] = tempMatrix[steps][j];
							}
							attractorCount = attractorCount + 1;
							mData->set(attractorCount, i + 1, l + 1, 0); 
						}else{ //is known attractor
							mData->set(attrNo, i + 1, l + 1, 0); 
						}
					}else{
						
						for(int j = 0; j < numberNeurons; j++){
							attrMatrix[0][j] = tempMatrix[steps][j];
						}
						attractorCount = 1;
						mData->set(1, i + 1, l + 1, 0); 
					}					
				}else{
					mData->set(0, i + 1, l + 1, 0); 	
				}
			}//for-loop: y-params
			for(int j = 0; j < noOfvNeuronsX; j++){
				rListX[j] = rListX[j] + xIncrements[j]; 
			}
							
			//This is important: allows to quit the application while the plotter is running.
			if(Core::getInstance()->isShuttingDown()) {
				return;
			}

			//This is important: it keeps the system alive!
			core->executePendingTasks();

			
		} //for-loop: x-params
		
		restoreCurrentNetworkActivites();
	}

}



