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


#include "BasinPlotter.h"
#include <Util/DynamicsPlotterUtil.h>
#include "Core/Core.h"
#include "math.h"
#include <Math/Math.h>
#include <iostream>

using namespace std;

namespace nerd {

BasinPlotter::BasinPlotter() : DynamicsPlotter("BasinOfAttraction") {
	// initialising

	mVariedX = new StringValue("0");
	mVariedX->setDescription("Network element to vary on X axis of plot");
	mVariedY = new StringValue("0");
	mVariedY->setDescription("Network element to vary on Y axis of plot");
	
	mVariedRangeX = new StringValue("-1,1");
	mVariedRangeX->setDescription("Min and Max for X axis parameter change");
	mVariedRangeY = new StringValue("-1,1");
	mVariedRangeY->setDescription("Min and Max for Y axis parameter change");
	
	mVariedResolutionX = new IntValue(200);
	mVariedResolutionX->setDescription("How many data points to generate for "
										"X axis parameter");
	mVariedResolutionY = new IntValue(200);
	mVariedResolutionY->setDescription("How many data points to generate for "
										"Y axis parameter");
	
	mAccuracy = new DoubleValue(0.001);
	mAccuracy->setDescription("Accuracy for network state comparison");
	mRoundDigits = new IntValue(-1);
	mRoundDigits->setDescription("Defines how many digits are preserved, "
								"when rounding values, -1 preserves all");

	mStepsToRun = new IntValue(2000);
	mStepsToRun->setDescription("Number of simulation steps that are computed "
								"before checking for attractors");
	mStepsToCheck = new IntValue(100);
	mStepsToCheck->setDescription("Maximum period to check for attractors");
	
	mResetNetworkActivation = new BoolValue(true);
	mResetNetworkActivation->setDescription("Whether or not to reset the "
						"network's activation after each parameter change");
	
	mRestoreNetworkConfiguration = new BoolValue(true);
	mRestoreNetworkConfiguration->setDescription("If true, then all network "
					"parameters like bias, weight, observable parameters are "
					"reset to the initial state before each run");

	mResetSimulator = new BoolValue(true);
	mResetSimulator->setDescription("If a simulation is used, then this property "
									"decides whether the simulation is reset before each run.");
	
	
	addParameter("Config/VariedElementX", mVariedX, true);
	addParameter("Config/VariedElementY", mVariedY, true);
	addParameter("Config/VariedRangeX", mVariedRangeX, true);
	addParameter("Config/VariedRangeY", mVariedRangeY, true);
	addParameter("Config/VariedResolutionX", mVariedResolutionX, true);
	addParameter("Config/VariedResolutionY", mVariedResolutionY, true);
	
	addParameter("Config/Accuracy", mAccuracy, true);
	addParameter("Config/RoundDigits", mRoundDigits, true);
	
	addParameter("Config/StepsToRunPreCheck", mStepsToRun, true);
	addParameter("Config/StepsToCheck", mStepsToCheck, true);
	
	addParameter("Config/ResetNetworkActivation", mResetNetworkActivation, true);
	addParameter("Config/RestoreNetworkConfiguration", mRestoreNetworkConfiguration, true);
	addParameter("Config/ResetSimulation", mResetSimulator, true);
	
	mTitleNames->set("Basins of Attraction|Periods|Attractor");
}

BasinPlotter::~BasinPlotter() {}

void BasinPlotter::calculateData() {
	
	// get program core
	Core *core = Core::getInstance();
	
	// get network
	ModularNeuralNetwork *network = getCurrentNetwork();
	if(network == 0) {
		Core::log("BasinPlotter: Could not find a neural network to work with! Aborting.", true);
		return;
	}
	
	QList<NeuralNetworkElement*> networkElements;
	network->getNetworkElements(networkElements);
	QList<DoubleValue*> networkValues =
						DynamicsPlotterUtil::getNetworkValues(networkElements);

	// Get parameters for observed elements
	QString variedX = mVariedX->get();
	QString variedY = mVariedY->get();
	if(variedX.isEmpty() || variedY.isEmpty()) {
		reportProblem("BasinPlotter: No elements to vary.");
		return;
	}
	

	DoubleValue *variedValX = DynamicsPlotterUtil::getElementValue(
		variedX, networkElements, &mNeuronsWithActivationsToTransfer);
	DoubleValue *variedValY = DynamicsPlotterUtil::getElementValue(
		variedY, networkElements, &mNeuronsWithActivationsToTransfer);
	
	if(variedValX == 0 || variedValY == 0) {
		reportProblem("BasinPlotter: NULL pointer for varied element. Aborting.");
		return;
	}
	
	QList<double> variedRangeX = 
				DynamicsPlotterUtil::getDoublesFromString(mVariedRangeX->get());
	QList<double> variedRangeY = 
				DynamicsPlotterUtil::getDoublesFromString(mVariedRangeY->get());
				
	if(variedRangeX.size() != 2 || variedRangeY.size() != 2) {
		reportProblem("BasinPlotter: Not a valid range given.");
		return;
	}
		
	int resolutionX = mVariedResolutionX->get();
	int resolutionY = mVariedResolutionY->get();
	
	//avoid division by zero!
	if(resolutionX < 2 || resolutionY < 2) {
		reportProblem("BasinPlotter: Invalid resolution given.");
		return;
	}
	
	// save original values for clean-up
	QList<double> variedValuesOrig;
	variedValuesOrig.append(QList<double>() << variedValX->get()
											<< variedValY->get());

	bool resetNetworkActivation = mResetNetworkActivation->get();
	storeCurrentNetworkActivities();
	
	/* store network configuration (bias terms, synapse weights,
			observable parameters of TFs, AFs, SFs. */
	bool restoreNetConfiguration = mRestoreNetworkConfiguration->get();
	storeNetworkConfiguration();
	
	//This is important when the physical simulator is activated!
	bool resetSimulation = mResetSimulator->get();
	triggerReset();
	
	// PREPARE data matrix
	mData->clear();
	mData->resize(resolutionX + 1, resolutionY + 1, 3);
	mData->fill(0);
	
	// calculate values and draw axes
	double xStart = variedRangeX.first();
	double xEnd = variedRangeX.last();
	double xStepSize = (xEnd - xStart) / (double) (resolutionX - 1);
	int roundDigits = mRoundDigits->get();
	
	double xVal;
	QList<double> xValues;
	for(int x = 1; x <= resolutionX; ++x) {
		xVal = xStart+(x-1)*xStepSize;
		mData->set(Math::round(xVal,5), x, 0, 0);
		mData->set(Math::round(xVal,5), x, 0, 1);
		mData->set(Math::round(xVal,5), x, 0, 2);
		
		if(roundDigits >= 0) {
			xVal = Math::round(xVal, roundDigits);
		}
		xValues.append(xVal);
	}
	
	double yStart = variedRangeY.first();
	double yEnd = variedRangeY.last();
	double yStepSize = (yEnd - yStart) / (double) (resolutionY - 1);
	
	double yVal;
	QList<double> yValues;
	for(int y = 1; y <= resolutionY; ++y) {
		yVal = yStart+(y-1)*yStepSize;
		mData->set(Math::round(yVal,5), 0, y, 0);
		mData->set(Math::round(yVal,5), 0, y, 1);
		mData->set(Math::round(yVal,5), 0, y, 2);
		
		if(roundDigits >= 0) {
			yVal = Math::round(yVal, roundDigits);
		}
		yValues.append(yVal);
	}

	// MAIN LOOP over x parameter points
		
	int stepsRun = mStepsToRun->get();
	int stepsCheck = mStepsToCheck->get();
	
	QList< QList<double> > attractors;
		
	for(int x = 1; x <= resolutionX && mActiveValue->get(); ++x) {
			
		mProgressPercentage->set((double)(100 * x / resolutionX));

		// INNER LOOP over y parameter points
		for(int y = 1; y <= resolutionY && mActiveValue->get(); ++y) {
			
			if(resetSimulation) {
				triggerReset();
			}
			
			if(restoreNetConfiguration) {
				restoreNetworkConfiguration();
			}
			
			if(resetNetworkActivation) {
				restoreCurrentNetworkActivites();
			}
			
			// set x parameter
			variedValX->set(xValues.at(x - 1));
			// set y parameter
			variedValY->set(yValues.at(y - 1));
			
			if(!notifyNetworkParametersChanged(network)) {
				return;
			}

			for(int runStep = 0; runStep < stepsRun && mActiveValue->get(); ++runStep) {
				// let the network run for 1 timestep
				triggerNetworkStep();
			}
			
			QList< QList<double> > states;
			QList< QPair<double,double> > positions;
			bool foundMatch = false;
			int currPeriod = 0;

			for(int checkStep = 0; checkStep <= stepsCheck && !foundMatch && mActiveValue->get(); ++checkStep) {
				triggerNetworkStep();
				
				// get current network state
				QList<double> networkState = 
						DynamicsPlotterUtil::getNetworkState(networkValues);
				
				// abort on empty state
				if(networkState.isEmpty()) {
					reportProblem("BasinPlotter: Encountered empty "
								"network state. Something went wrong.");
					return;
				}
				
				// compare states to find attractors
				for(int period = 1; period <= checkStep && !foundMatch; ++period) {
					foundMatch = DynamicsPlotterUtil::compareNetworkStates(states.at(checkStep-period), networkState);
					currPeriod = period;
				}
				
				// save current state as last one
				states.append(networkState);
				positions.append(QPair<double,double>(variedValX->get(), variedValY->get()));
					
			}
			
			// at this point, either an attractor has been found
			if(foundMatch && mActiveValue->get()) {
				
				// check for past attractors
				bool attrMatch = false;
				int attrNo = 0;
				while(attrNo < attractors.size() && !attrMatch) {
					for(int j = 1; j <= currPeriod && !attrMatch; ++j) {
						attrMatch = DynamicsPlotterUtil::compareNetworkStates(
											attractors.at(attrNo),
											states.at(states.size()-1-j));
					}
					attrNo++;
				}
				
				// write matrix
				mData->set(attrNo, x, y, 0);
				mData->set(currPeriod, x, y, 1);

				// calculate and plot attractor position(s)
				int posC = positions.size();
				for(int i = 1; i <= currPeriod; ++i) {
					double currValX = positions.at(posC - i).first;
					double currValY = positions.at(posC - i).second;
				
					int attrPosX = ceil((currValX - xStart) / xStepSize + 1);
					int attrPosY = ceil((currValY - yStart) / yStepSize + 1);
				
					mData->set(attrNo, attrPosX, attrPosY, 2);
				}
				
				if(!attrMatch) {
					attractors.append(states.last());
				}
			}
			
			// or not, but then there's nothing to do :D
			
			// runtime maintencance
			if(core->isShuttingDown()) {
				return;
			}
			core->executePendingTasks();
		}
	}
	
	// CLEAN UP
	variedValX->set(variedValuesOrig.at(0));
	variedValY->set(variedValuesOrig.at(1));
	notifyNetworkParametersChanged(network);

	triggerReset();
	restoreNetworkConfiguration();
	restoreCurrentNetworkActivites();

}

}

