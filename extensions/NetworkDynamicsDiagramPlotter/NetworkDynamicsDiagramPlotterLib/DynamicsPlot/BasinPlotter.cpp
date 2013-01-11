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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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
	
	mResolutionX = new IntValue(200);
	mResolutionX->setDescription("How many data points to generate for "
								 "X axis parameter(s)");
	mResolutionY = new IntValue(200);
	mResolutionY->setDescription("How many data points to generate for "
								 "Y axis parameter(s)");

	mProjectionsX = new StringValue("0");
	mProjectionsX->setDescription("Additional parameters onto which to project on the X axis");
	mProjectionsY = new StringValue("0");
	mProjectionsY->setDescription("Additional parameters onto which to project on the Y axis");

	mProjectionRangesX = new StringValue("0");
	mProjectionRangesX->setDescription("Ranges for additional X axis projections");
	mProjectionRangesY = new StringValue("0");
	mProjectionRangesY->setDescription("Ranges for additional Y axis projections");
	
	mAccuracy = new DoubleValue(0.0001);
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

	mResetSimulator = new BoolValue(false);
	mResetSimulator->setDescription("If a simulation is used, then this property "
									"decides whether the simulation is reset before each run.");
	
	
	addParameter("Config/VariedElementX", mVariedX, true);
	addParameter("Config/VariedElementY", mVariedY, true);
	addParameter("Config/VariedRangeX", mVariedRangeX, true);
	addParameter("Config/VariedRangeY", mVariedRangeY, true);
	addParameter("Config/ResolutionX", mResolutionX, true);
	addParameter("Config/ResolutionY", mResolutionY, true);

	addParameter("Config/ProjectionsX", mProjectionsX, true);
	addParameter("Config/ProjectionsY", mProjectionsY, true);
	addParameter("Config/ProjectionRangesX", mProjectionRangesX, true);
	addParameter("Config/ProjectionRangesY", mProjectionRangesY, true);
	
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

	// Get parameters for varied elements
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
		

	int resolutionX = mResolutionX->get();
	int resolutionY = mResolutionY->get();
	
	//avoid division by zero!
	if(resolutionX < 2 || resolutionY < 2) {
		reportProblem("BasinPlotter: Invalid resolution given.");
		return;
	}


	// projected elements
	int nrProjections = 0;
	QString projectionsX = mProjectionsX->get();
	QString projectionsY = mProjectionsY->get();
	QList< QList<DoubleValue*> > projectionValuesX;
	QList< QList<DoubleValue*> > projectionValuesY;
	QList<double> projectionRangesX;
	QList<double> projectionRangesY;

	if(projectionsX != "0" && projectionsY != "0") {

		QList<QStringList> projectionListX = 
				DynamicsPlotterUtil::parseElementString(projectionsX);
		QList<QStringList> projectionListY =
				DynamicsPlotterUtil::parseElementString(projectionsY);

		projectionValuesX =
				DynamicsPlotterUtil::getElementValues(projectionListX, networkElements);
		projectionValuesY =
				DynamicsPlotterUtil::getElementValues(projectionListY, networkElements);

		if(projectionValuesX.isEmpty() || projectionValuesY.isEmpty()) {
			reportProblem("BasinPlotter: Could not find specified elements to project onto.");
			return;
		}

		if(projectionValuesX.size() != projectionValuesY.size()) {
			reportProblem("BasinPlotter: Mismatching number of projected elements for the two axes.");
			return;
		}

		projectionRangesX =
				DynamicsPlotterUtil::getDoublesFromString(mProjectionRangesX->get());
		projectionRangesY =
				DynamicsPlotterUtil::getDoublesFromString(mProjectionRangesY->get());

		if(projectionRangesX.size() != 2*projectionValuesX.size() ||
		   projectionRangesY.size() != 2*projectionValuesY.size()) {
			reportProblem("BasinPlotter: Given ranges for projection don't match number of elements.");
			return;
		}

		nrProjections = projectionValuesX.size();

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
	mData->resize(resolutionX + 1, resolutionY + 1, 3 + nrProjections);
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

	// same for additional projections
	for(int currProj = 0; currProj < nrProjections; ++currProj) {
		double pStartX = projectionRangesX.at(currProj*2);
		double pEndX = projectionRangesX.at(currProj*2 + 1);
		double pStepX = (pEndX - pStartX) / (double) (resolutionX - 1);
		for(int x = 1; x <= resolutionX; ++x) {
			mData->set(Math::round((pStartX+(x-1)*pStepX),5), x, 0, 3+currProj);
		}
		double pStartY = projectionRangesY.at(currProj*2);
		double pEndY = projectionRangesY.at(currProj*2 + 1);
		double pStepY = (pEndY - pStartY) / (double) (resolutionY - 1);
		for(int y = 1; y <= resolutionY; ++y) {
			mData->set(Math::round((pStartY+(y-1)*pStepY),5), 0, y, 3+currProj);
		}
	}

	// MAIN LOOP over x parameter points
		
	int stepsRun = mStepsToRun->get();
	int stepsCheck = mStepsToCheck->get();
	double accuracy = mAccuracy->get();
	
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
			variedValX->set(xValues.at(x-1));
			// set y parameter
			variedValY->set(yValues.at(y-1));
			
			if(!notifyNetworkParametersChanged(network)) {
				return;
			}

			for(int runStep = 0; runStep < stepsRun && mActiveValue->get(); ++runStep) {
				// let the network run for 1 timestep
				triggerNetworkStep();
			}
			
			QList< QList<double> > networkStates;
			QList<double> networkState;
			QList< QPair<double,double> > variedPositions;

			QList< QPair< QList<double>, QList<double> > > projectionPositions;

			bool foundMatch = false;
			int attrPeriod = 0;

			for(int checkStep = 0; checkStep <= stepsCheck && !foundMatch && mActiveValue->get(); ++checkStep) {
				triggerNetworkStep();
				
				// get current network state
				networkState = DynamicsPlotterUtil::getNetworkState(networkValues);
				
				// abort on empty state
				if(networkState.isEmpty()) {
					reportProblem("BasinPlotter: Encountered empty network state.");
					return;
				}
				
				// compare states to find attractors
				for(int period = 1; period <= checkStep && !foundMatch; ++period) {
					foundMatch = DynamicsPlotterUtil::compareNetworkStates(
							networkStates.at(checkStep-period),
							networkState,
							accuracy);
					attrPeriod = period;
				}
				
				// save current state as last one
				networkStates.append(networkState);

				variedPositions.append(QPair<double,double>(variedValX->get(), variedValY->get()));

				if(nrProjections > 0) {
					QPair< QList<double>, QList<double> > currentPositions;
					currentPositions.first = DynamicsPlotterUtil::getMeanValues(projectionValuesX);
					currentPositions.second = DynamicsPlotterUtil::getMeanValues(projectionValuesY);
					projectionPositions.append(currentPositions);
				}

			}
			
			// at this point, either an attractor has been found
			if(foundMatch && mActiveValue->get()) {
				
				// check for past attractors
				bool attrMatch = false;
				int attrNo = 0;
				while(attrNo < attractors.size() && !attrMatch) {
					for(int state = 1; state <= attrPeriod && !attrMatch; ++state) {
						attrMatch = DynamicsPlotterUtil::compareNetworkStates(
								attractors.at(attrNo),
								networkStates.at(networkStates.size()-1-state),
								accuracy);
					}
					attrNo++;
				}
				
				// write matrix
				mData->set(attrNo, x, y, 0);
				mData->set(attrPeriod, x, y, 1);

				// calculate and plot attractor position(s)
				int nrPositions = variedPositions.size();
				for(int periodPos = 1; periodPos <= attrPeriod; ++periodPos) {
					int currPosition = nrPositions - periodPos;

					double currValX = variedPositions.at(currPosition).first;
					double currValY = variedPositions.at(currPosition).second;
				
					int attrPosX = ceil((currValX - xStart) / xStepSize + 1);
					int attrPosY = ceil((currValY - yStart) / yStepSize + 1);
				
					mData->set(attrNo, attrPosX, attrPosY, 2);

					for(int currProj = 0; currProj < nrProjections; ++currProj) {
						double xVal = projectionPositions.at(currPosition).first.at(currProj);
						double yVal = projectionPositions.at(currPosition).second.at(currProj);

						double pStartX = projectionRangesX.at(currProj*2);
						double pEndX = projectionRangesX.at(currProj*2 + 1);
						double pStepX = (pEndX - pStartX) / (double) (resolutionX - 1);
						double pStartY = projectionRangesY.at(currProj*2);
						double pEndY = projectionRangesY.at(currProj*2 + 1);
						double pStepY = (pEndY - pStartY) / (double) (resolutionY - 1);
						
						int xPos = ceil((xVal - pStartX) / pStepX + 1);
						int yPos = ceil((yVal - pStartY) / pStepY + 1);

						mData->set(attrNo, xPos, yPos, 3+currProj);
					}
				}
				
				if(!attrMatch) {
					attractors.append(networkStates.last());
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

