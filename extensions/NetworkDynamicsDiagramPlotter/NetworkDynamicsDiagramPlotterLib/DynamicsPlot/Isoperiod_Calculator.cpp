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



#include "Isoperiod_Calculator.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "math.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new Isoperiod_Calculator.
 */
	Isoperiod_Calculator::Isoperiod_Calculator(): DynamicsPlotter("Isoperiod_Calculator")
	{
		// IDs, minima, maxima for network elements, that will be plotted on x-axis:
		mIdsOfVariedNetworkElementsX = new StringValue("0, 0 | 0");
		mMinimaOfVariedNetworkElementsX = new StringValue("0 , 0, 0");
		mMaximaOfVariedNetworkElementsX = new StringValue("1, 1, 1");
		// same for y-axis:
		mIdsOfVariedNetworkElementsY = new StringValue("0");
		mMinimaOfVariedNetworkElementsY = new StringValue("0");
		mMaximaOfVariedNetworkElementsY = new StringValue("1");
		
		mPlotPixelsX = new IntValue(600); //accuracy of x-parameter variation, no. of pixels in x-dimension of diagram
		mPlotPixelsY = new IntValue(500); //same for y
		mResetToInitState = new BoolValue(false); // reset to initial activity state after every parameter change; if false: uses last state ("follows attractor")
		mMaxSteps = new IntValue(250); // defines maximal number of steps that are executed
		mTolerance = new DoubleValue(0.000001);//Specifies how large the margin is, such that the two outputs count as the same one 
		mPrerunSteps = new DoubleValue(100);//Number of steps the network takes before started searching for attractors
		
		mXAxisDescription->set("Param. 1");//setting initial axes descriptions
		mYAxisDescription->set("Param. 2");
		
		mIdsOfVariedNetworkElementsX->setDescription("Comma-separated list of IDs of the neurons that are varied (x-axis).");
		mMinimaOfVariedNetworkElementsX->setDescription("Comma-separated list of the minimal values of the varied network elements (x-axis)");
		mMaximaOfVariedNetworkElementsX->setDescription("Comma-separated list of the maximal values of the varied network elements (x-axis)");
		mIdsOfVariedNetworkElementsY->setDescription("Comma-separated list of IDs of the neurons that are varied (y-axis).");
		mMinimaOfVariedNetworkElementsY->setDescription("Comma-separated list of the minimal values of the varied network elements (y-axis)");
		mMaximaOfVariedNetworkElementsY->setDescription("Comma-separated list of the maximal values of the varied network elements (y-axis)");
		mPlotPixelsX->setDescription("Horizontal size of plot, also determines the step size of the parameter change (max - min)/plotPixels");
		mPlotPixelsY->setDescription("Vertical size of plot, also determines the step size of the parameter change (max - min)/plotPixels");
		mResetToInitState->setDescription("If TRUE then the network activity is reset after every network step.");
		mMaxSteps->setDescription("Maximal number of steps taken, if no attractor is found");
		mTolerance->setDescription("Tolerance for that two values are taken as the same; x = x +/- tol");
		mPrerunSteps->setDescription("Number of steps before search for attractors is started.");
		
		addParameter("XIdsOfVariedNetworkElements", mIdsOfVariedNetworkElementsX, true);
		addParameter("XMinimaOfVariedNetworkElements", mMinimaOfVariedNetworkElementsX, true);
		addParameter("XMaximaOfVariedNetworkElements", mMaximaOfVariedNetworkElementsX, true);
		addParameter("YIdsOfVariedNetworkElements", mIdsOfVariedNetworkElementsY, true);
		addParameter("YMinimaOfVariedNetworkElements", mMinimaOfVariedNetworkElementsY, true);
		addParameter("YMaximaOfVariedNetworkElements", mMaximaOfVariedNetworkElementsY, true);
		addParameter("PlotPixelsX", mPlotPixelsX, true); 
		addParameter("PlotPixelsY", mPlotPixelsY, true); 
		addParameter("Tolerance", mTolerance, true); 
		addParameter("ResetToInitState", mResetToInitState, true);
		addParameter("MaxSteps", mMaxSteps, true);
		addParameter("PrerunSteps", mPrerunSteps, true);
	}



/**
 * Destructor.
 */
	Isoperiod_Calculator::~Isoperiod_Calculator() {
	}

	/**
	 * Calculates the output data
	 */
	void Isoperiod_Calculator::calculateData() {
		if(mNextStepEvent == 0 || mResetEvent == 0 || mEvaluateNetworkEvent == 0) {
			return;
		}

		NeuralNetwork *network = getCurrentNetwork();

		if(network == 0) {
			Core::log("Isoperiod_Calculator: Could not find a modular neural network!", true);
			return;
		}
		
		QList<ULongLongValue*>vIdsListX = createListOfIds(mIdsOfVariedNetworkElementsX);//list of IDs of varied elements
		QList<double> vMinsListX = createListOfDoubles(mMinimaOfVariedNetworkElementsX);
		QList<double> vMaxsListX = createListOfDoubles(mMaximaOfVariedNetworkElementsX);
		
		QList<ULongLongValue*>vIdsListY = createListOfIds(mIdsOfVariedNetworkElementsY);//list of IDs of varied elements
		QList<double> vMinsListY = createListOfDoubles(mMinimaOfVariedNetworkElementsY);
		QList<double> vMaxsListY = createListOfDoubles(mMaximaOfVariedNetworkElementsY);
		
		//check IDs, minima, and maxima
		if(!checkStringlistsItemCount(mIdsOfVariedNetworkElementsX, mMinimaOfVariedNetworkElementsX, mMaximaOfVariedNetworkElementsX)){
			Core::log("Isoperiod_Calculator: The number of IDs, minima and maxima (x-axis parameters) must be the same!", true);		
			return;
		}
		if(!checkStringlistsItemCount(mIdsOfVariedNetworkElementsY, mMinimaOfVariedNetworkElementsY, mMaximaOfVariedNetworkElementsY)){
			Core::log("Isoperiod_Calculator: The number of IDs, minima and maxima (y-axis parameters) must be the same!", true);		
			return;
		}
		
		int noOfvElemsX = vIdsListX.size();//No. of varied elements
		int noOfvElemsY = vIdsListY.size();
		
		QList<NeuralNetworkElement*> vElemsListX; //list of varied elements
		QList<NeuralNetworkElement*> vElemsListY;
		
		//get pointer to the varied neurons and synapses on the x-axis
		for(int j = 0; j < noOfvElemsX; j++){
			vElemsListX.append(getVariedNetworkElement(vIdsListX[j]));	
			if(getVariedNetworkElement(vIdsListX[j]) == 0){
				Core::log("Isoperiod_Calculator: Could not find required (varied) neurons or synapses (x-axis)!", true);		
				return;	
			}
		}
		//y-axis
		for(int j = 0; j < noOfvElemsY; j++){
			vElemsListY.append(getVariedNetworkElement(vIdsListY[j]));	
			if(getVariedNetworkElement(vIdsListY[j]) == 0){
				Core::log("Isoperiod_Calculator: Could not find required (varied) neurons or synapses (y-axis)!", true);		
				return;	
			}
		}	
		
		//prepare variables for evaluations
		int numberNeurons = network->getNeurons().count();
		int maxSteps = mMaxSteps->get();
		bool resetToInitState = mResetToInitState->get();
		double prerunSteps = mPrerunSteps->get();
		int plotPixelsX = mPlotPixelsX->get();
		int plotPixelsY = mPlotPixelsY->get();		
		
		if(plotPixelsX < 2){
			Core::log("Isoperiod_Calculator: Size of diagram must be over 1 pixel (x-axis).", true);
			return;
		}
		if(plotPixelsY < 2){
			Core::log("Isoperiod_Calculator: Size of diagram must be over 1 pixel (y-axis).", true);
			return;
		}	

		QList<double> xIncrements; //range of pixels
		QList<double> yIncrements;
		for(int j = 0; j < noOfvElemsX; j++){
			xIncrements.append((vMaxsListX[j] - vMinsListX[j]) / ((double) plotPixelsX - 1)); //'-1' to include min and max values in steps
		}
		for(int j = 0; j < noOfvElemsY; j++){
			yIncrements.append((vMaxsListY[j] - vMinsListY[j]) / ((double) plotPixelsY - 1)); //'-1' to include min and max values in steps
		}	

		QList<double> rListX; // running parameters for varied elements
		QList<double> rListY;
		for(int j = 0; j < noOfvElemsX; j++){
			if(vMinsListX[j] != vMaxsListX[j]){
				rListX.append(vMinsListX[j]);
			}else{
				Core::log("Isoperiod_Calculator: Minimum of variation must not be equal to maximum.", true);
				return;
			}
		}
		for(int j = 0; j < noOfvElemsY; j++){
			if(vMinsListY[j] != vMaxsListY[j]){
				rListY.append(vMinsListY[j]);
			}else{
				Core::log("Isoperiod_Calculator: Minimum of variation must not be equal to maximum.", true);
				return;
			}
		}
		QList<double> rListYInit = rListY; //Save initial parameter list, parameter of y-elements are reset after every step on x-axis
		
		Core *core = Core::getInstance();

		QList<double> oldValuesX;
		QList<double> oldValuesY;
		for(int j = 0; j < noOfvElemsX; j++){
			oldValuesX.append(getVariedNetworkElementValue(vElemsListX[j]));
		}
		for(int j = 0; j < noOfvElemsY; j++){
			oldValuesY.append(getVariedNetworkElementValue(vElemsListY[j]));
		}
		
		double tempMatrix[maxSteps][numberNeurons];
		bool attractorFound = false;
		int periodLength = 0;

		QList<Neuron*> neuronsList = network->getNeurons();
		
		//set up matrix:
		mData->clear();
		//matrix is as large as diagram (in pixels) + 1
		mData->resize(plotPixelsX + 1, plotPixelsY + 1, 1); //first dimension = no. of parameter changes; second dimension = number of output buckets
		mData->fill(0);
		
		storeCurrentNetworkActivities();
		//evaluate
		for(int i = 0;  i < plotPixelsX; i++) { //runs through x-parameter changes
			for(int j = 0; j < noOfvElemsX; j++){ //set varied values to network elements
				setVariedNetworkElementValue(vElemsListX[j], rListX[j]); 
			}
			//set first matrix row: indices of x-axis 
			if(noOfvElemsX == 1){
				mData->set(rListX[0], i + 1, 0, 0);//if only one parameter is changed, take its values
			}else{
				mData->set(i + 1, i + 1, 0, 0); //if more, take pixelcount
			}
			rListY = rListYInit; //reset parameters
			for(int l = 0; l < plotPixelsY; l++){//runs through y-parameter changes	
				
				for(int j = 0; j < noOfvElemsY; j++){//set varied values
					setVariedNetworkElementValue(vElemsListY[j], rListY[j]); //inherited by parent class DynamicsPlotter
				}
				if(i == 0 && noOfvElemsY == 1){ //set first matrix column
					mData->set(rListY[0], 0, l + 1, 0);//if only one parameter is changed, take its values
				}else if(i == 0){
					mData->set(l + 1, 0, l + 1, 0); //if more, take pixelcount
				}
				if(resetToInitState) restoreCurrentNetworkActivites(); //if false, then the last activity state is used 
				attractorFound = false;
				for(int j = 0; j < prerunSteps; j++){//pre run
					mEvaluateNetworkEvent->trigger();
				}
				for(int j = 0; j < maxSteps && attractorFound == false; j++){//look for attractor	
					//this executes the neural network once.
					mEvaluateNetworkEvent->trigger();
					
					for(int k = 0; k < numberNeurons; k++){ //add all neuron activations to temporary matrix
						tempMatrix[j][k] = neuronsList.at(k)->getOutputActivationValue().get(); 
					}
	
					for(int m = j - 1; m >= 0 && attractorFound == false;--m){ //check if network state appeared before
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
				}//attractor loop
				
				
				for(int j = 0; j < noOfvElemsY; j++){
					rListY[j] = rListY[j] + yIncrements[j]; //inherited by parent class DynamicsPlotter
				}
				
				if(attractorFound){//update data matrix
					mData->set(periodLength, i + 1, l + 1, 0); 
				}else{
					mData->set(0, i + 1, l + 1, 0); 	
				}
				
				
			}//for-loop: y-params
		
				
			for(int j = 0; j < noOfvElemsX; j++){
				rListX[j] = rListX[j] + xIncrements[j]; //inherited by parent class DynamicsPlotter
			}

			
			//This is important: allows to quit the application while the plotter is running.
			if(Core::getInstance()->isShuttingDown()) {
				return;
			}

			//This is important: it keeps the system alive!
			core->executePendingTasks();

			
		} //for-loop: x-params
		
		restoreCurrentNetworkActivites();
		
		//restore old bias/synapse strength
		for(int j = 0; j < noOfvElemsX; j++){
			setVariedNetworkElementValue(vElemsListX[j], oldValuesX[j]);
		}
		for(int j = 0; j < noOfvElemsY; j++){
			setVariedNetworkElementValue(vElemsListY[j], oldValuesY[j]);
		}
		cerr<<"Finished isoperiod calculation."<<endl;
	}
}



