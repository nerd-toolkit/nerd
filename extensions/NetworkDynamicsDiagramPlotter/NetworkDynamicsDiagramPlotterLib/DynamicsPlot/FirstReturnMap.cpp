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


#include "FirstReturnMap.h"
#include <Util/DynamicsPlotterUtil.h>
#include "Core/Core.h"
#include "math.h"
#include <Math/Math.h>
#include <iostream>

using namespace std;

namespace nerd {

FirstReturnMap::FirstReturnMap() : DynamicsPlotter("FirstReturnMap") {
	// initialising

	mObservedElems = new StringValue("0");
	mObservedElems->setDescription("Network elements to observe\n"
					"Format is ID:PARAM or ID:FUNC:PARAM\n"
					"separated by ',' for the mean or\n"
					"          by '|' for multiple plots");
	
	mObservedRanges = new StringValue("-1,1");
	mObservedRanges->setDescription("Limits of the plot values\n"
					"give two comma-separated values for each separate plot\n"
					"e.g. -1,1|1,2");

	mObservedResolution = new IntValue(200);
	mObservedResolution->setDescription("How many pixels on both axes?");
	
	mAccuracy = new DoubleValue(0.001);
	mAccuracy->setDescription("Accuracy for network state comparison");

	mStepsToRun = new IntValue(300);
	mStepsToRun->setDescription("Number of simulation steps that are computed");
	mStepsToPlot = new IntValue(100);
	mStepsToPlot->setDescription("How many steps to plot after running for StepsToRun steps");
	
	mResetNetworkActivation = new BoolValue(true);
	mResetNetworkActivation->setDescription("Whether or not to reset the "
						"network's activation after each parameter change");
	
	mRestoreNetworkConfiguration = new BoolValue(true);
	mRestoreNetworkConfiguration->setDescription("If true, then all network "
					"parameters like bias, weight, observable parameters are "
					"reset to the initial state before each run");

	
	addParameter("Config/ObservedElems", mObservedElems, true);
	addParameter("Config/ObservedRanges", mObservedRanges, true);
	addParameter("Config/ObservedResolution", mObservedResolution, true);
	
	addParameter("Config/Accuracy", mAccuracy, true);
	
	addParameter("Config/StepsToRun", mStepsToRun, true);
	addParameter("Config/StepsToPlot", mStepsToPlot, true);
	
	addParameter("Config/ResetNetworkActivation", mResetNetworkActivation,
true);
	addParameter("Config/RestoreNetworkConfiguration",
mRestoreNetworkConfiguration, true);
}

FirstReturnMap::~FirstReturnMap() {}

void FirstReturnMap::calculateData() {
	
	// get program core
	Core *core = Core::getInstance();
	
	// get network
	ModularNeuralNetwork *network = getCurrentNetwork();
	QList<NeuralNetworkElement*> networkElements;
	network->getNetworkElements(networkElements);
	QList<DoubleValue*> networkValues =
						DynamicsPlotterUtil::getNetworkValues(networkElements);

	// Get parameters for observed elements
	QList<QStringList> observedElems =
		DynamicsPlotterUtil::parseElementString(mObservedElems->get());

	if(observedElems.isEmpty()) {
		reportProblem("FirstReturnMap: No elements to observe.");
		return;
	}
	
	QList< QList <DoubleValue*> > observedValues =
		DynamicsPlotterUtil::getElementValues(observedElems, networkElements);
	
	if(observedValues.isEmpty()) {
		reportProblem("FirstReturnMap: Observed element returned NULL pointer. "
					"Aborting.");
		return;
	}
	
	QList<double> observedRanges = 
				DynamicsPlotterUtil::getDoublesFromString(mObservedRanges->get());
				
	if(observedRanges.isEmpty() || observedRanges.size() != 2*observedValues.size()) {
		reportProblem("FirstReturnMap: Given value ranges don't match the elements."
					" Aborting.");
		return;
	}
		
	int resolution = mObservedResolution->get();
	
	//avoid division by zero!
	if(resolution < 2) {
		reportProblem("FirstReturnMap: Invalid resolution given.");
		return;
	}
	
	storeCurrentNetworkActivities();
	storeNetworkConfiguration();
	
	// PREPARE data matrix
	mData->clear();
	mData->resize(resolution + 1, resolution + 1, observedValues.size());
	mData->fill(0);
	
	QList<double> rangeStart, rangeEnd, rangeStep;
	// iterate through plots and write axes first
	for(int i = 0; i < observedRanges.size(); ++i) {
		rangeStart.append(observedRanges.at(2*i+1));
		rangeEnd.append(observedRanges.at(2*i+2));
		rangeStep.append((rangeEnd.at(i) - rangeStart.at(i)) / (double) (resolution - 1));
		
		for(int x = 1; x <= resolution; ++x) {
			double val = rangeStart.at(i)+(x-1)*rangeStep.at(i);
			mData->set(Math::round(val,5), x, 0, i); // x-axis
			mData->set(Math::round(val,5), 0, x, i); // y-axis
		}
	}

	int stepsRun = mStepsToRun->get();
	int stepsPlot = mStepsToPlot->get();
	
	// run network	
	for(int j=1; j < stepsRun-stepsPlot && mActiveValue->get(); ++j) {
		triggerNetworkStep();
	}
	
	QList<double> oldActs;
	for(int k = 0; k <= stepsPlot && mActiveValue->get(); ++k) {
		triggerNetworkStep();
		
		for(int i = 0; i < observedValues.size(); ++i) {
			QList<DoubleValue*> currentValues = observedValues.at(i);

			if(currentValues.isEmpty()) {
				reportProblem("FirstReturnMap: No Values found to observe. Skipping.");
				continue;
			}

			double act = 0.0;
			for(int n = 0; n < currentValues.size(); ++n) {
				act += currentValues.at(n)->get();
			}
			act = act / currentValues.size();

			if(k > 0) {
				int x = (act - rangeStart.at(i)) / rangeStep.at(i) + 1;
				int y = (oldActs.at(i) - rangeStart.at(i)) / rangeStep.at(i) + 1;
				mData->set(1, x, y, i);
				oldActs.replace(i, act);
			} else {
				oldActs.append(act);
			}
		}	
	
		if(core->isShuttingDown()) {
			return;
		}
		core->executePendingTasks();
	
	}

	// CLEAN UP	
	triggerReset();
	restoreNetworkConfiguration();
	restoreCurrentNetworkActivites();

}

}
