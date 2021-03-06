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


#include "BifurcationPlotter.h"
#include <Util/DynamicsPlotterUtil.h>
#include "Core/Core.h"
#include "math.h"
#include <Math/Math.h>
#include <iostream>
#include "DynamicsPlotManager.h"

using namespace std;

namespace nerd {

BifurcationPlotter::BifurcationPlotter() : DynamicsPlotter("Bifurcation") {
	// initialising
	mObservedElements = new StringValue("0");
	mObservedElements->setDescription("Elements to observe, syntax "
									"ID[[:FUNC]:PARAM] \n"
									 "with FUNC = {tf, af, sf) \n"
									 "and PARAM = {a, b, o, w}");

	mObservedRanges = new StringValue("-1,1");
	mObservedRanges->setDescription("Minimum and maximum values for netwerk elements that "
									"are observed (eg 0,1)");

	mObservedResolution = new IntValue(600);
	mObservedResolution->setDescription("Defines how many points the ranges span");

	mVariedElement = new StringValue("0");
	mVariedElement->setDescription("Network element to vary over time. Syntax "
								"is ID[[:FUNC:]PARAM]  (default is output "
								"value of neuron or synapse strength, so "
								"for the bias value use ID:b)");

	mVariedRange = new StringValue("0,1");
	mVariedRange->setDescription("Comma-separated start and end values for parameter variation "
								 "(eg 0,1)");

	mVariedResolution = new IntValue(600);
	mVariedResolution->setDescription("Defines how many points inside the given range for an "
									  "element parameter are being generated");

	mStepsToRun = new IntValue(1000);
	mStepsToRun->setDescription("Number of simulation steps that are computed");
	mStepsToPlot = new IntValue(10);
	mStepsToPlot->setDescription("Number of simulation steps to plot after StepsToRun");

	mResetNetworkActivation = new BoolValue(true);
	mResetNetworkActivation->setDescription("Whether or not to reset the network's activation "
											"after each parameter change");

	mRunBackwards = new BoolValue(true);
	mRunBackwards->setDescription("If true, the plot is additionally run backwards.");

	mRestoreNetworkConfiguration = new BoolValue(true);
	mRestoreNetworkConfiguration->setDescription("If true, then all network parameters like bias, "
					"weight, observable parameters are reset to the initial state before each run");

	mResetSimulator = new BoolValue(true);
	mResetSimulator->setDescription("If a simulation is used, then this property "
									"decides whether the simulation is reset before each run.");

	addParameter("Config/ObservedElements", mObservedElements, true);
	addParameter("Config/ObservedRanges", mObservedRanges, true);
	addParameter("Config/ObservedResolution", mObservedResolution, true);

	addParameter("Config/VariedElement", mVariedElement, true);
	addParameter("Config/VariedRange", mVariedRange, true);
	addParameter("Config/VariedResolution", mVariedResolution, true);

	addParameter("Config/StepsToRun", mStepsToRun, true);
	addParameter("Config/StepsToPlot", mStepsToPlot, true);

	addParameter("Config/ResetNetworkActivation", mResetNetworkActivation, true);
	addParameter("Config/RunBackwards", mRunBackwards, true);
	addParameter("Config/RestoreNetworkConfiguration", mRestoreNetworkConfiguration, true);
	addParameter("Config/ResetSimulation", mResetSimulator, true);

	mTitleNames->set("Bifurcation Plot|Bifurcation Plot|Bifurcation Plot|Bifurcation Plot|Bifurcation Plot|Bifurcation Plot");
}

BifurcationPlotter::~BifurcationPlotter() {}

void BifurcationPlotter::calculateData() {

	// get program core
	Core *core = Core::getInstance();

	// get network
	ModularNeuralNetwork *network = getCurrentNetwork();
	QList<NeuralNetworkElement*> networkElements;
	network->getNetworkElements(networkElements);

	// Get parameters for observed elements
	QString observedElements = mObservedElements->get();
	if(observedElements.isEmpty()) {
		reportProblem("BifurcationPlotter: No elements to observe. Aborting.");
		return;
	}

	QList<QStringList> observedElementsList =
				DynamicsPlotterUtil::parseElementString(observedElements);

	QList< QList< DoubleValue *> > observedValuesList =
				DynamicsPlotterUtil::getElementValues(observedElementsList, networkElements);

	QList<double> observedRanges =
				DynamicsPlotterUtil::getDoublesFromString(mObservedRanges->get());

	if(observedRanges.isEmpty() || observedRanges.size() != 2*observedValuesList.size()) {
		reportProblem("BifurcationPlotter: Invalid number of ranges given, maybe "
					"mismatch to number of elements. Observed Ranges ["
					+ QString::number(observedRanges.size()) + "] Observed Values: ["
					+ QString::number(observedValuesList.size()) + "]");
		return;
	}

	// Get parameters for varied network element
	QString variedElement = mVariedElement->get();
	if(variedElement.isEmpty()) {
		reportProblem("BifurcationPlotter: No elements to vary. Aborting.");
		return;
	}

	DoubleValue *variedValue = DynamicsPlotterUtil::getElementValue(
					variedElement, networkElements, &mNeuronsWithActivationsToTransfer);

	if(variedValue == 0) {
		reportProblem("BifurcationPlotter: Element to vary does not exist.");
		return;
	}

	QList<double> variedRange =
		DynamicsPlotterUtil::getDoublesFromString(mVariedRange->get());

	if(variedRange.size() != 2) {
		reportProblem("BifurcationPlotter: Invalid number of range parameters given. Aborting.");
		return;
	}

	// save original value for clean-up
	double variedValueOrig = variedValue->get();
	bool resetNetworkActivation = mResetNetworkActivation->get();
	storeCurrentNetworkActivities();

	//store network configuration (bias terms, synapse weights, observable parameters of TFs, AFs, SFs.
	bool restoreNetConfiguration = mRestoreNetworkConfiguration->get();
	storeNetworkConfiguration();

	bool resetSimulation = mResetSimulator->get();
	triggerReset();

	int resolutionX = mVariedResolution->get();
	int resolutionY = mObservedResolution->get();

	//avoid division by zero!
	if(resolutionX < 2 || resolutionY < 2) {
		return;
	}


	bool runBackwards = mRunBackwards->get();
	int runSecondIteration = runBackwards ? 1 : 0;

	int numberSteps = mStepsToRun->get();
	int plottedSteps = mStepsToPlot->get();

	// fill with observer range for each plot
	QList< QList <double> > oParams;
	// varied element values
	QList<double> vVals;

	// PREPARE data matrix
	{
		//Thread safety of matrix.
		QMutexLocker guard(mDynamicsPlotManager->getMatrixLocker());

		mData->clear();
		mData->resize(resolutionX + 1, resolutionY + 1, observedValuesList.size());
		mData->fill(0);

        // draw y axis values
		for(int i = 0; i < observedValuesList.size(); ++i) {
			double oStart = observedRanges.at(i*2);
			double oEnd = observedRanges.at(i*2+1);
			double oStepSize = (oEnd - oStart) / (double) (resolutionY - 1);

			for(int y = 1; y <= resolutionY; ++y) {
				mData->set(Math::round(oStart+(y-1)*oStepSize,5), 0, y, i);
			}

			// store parameters for later
			oParams.append(QList<double>() << oStart << oEnd << oStepSize);
		}

		// draw x axis values
        double vStart = variedRange.first();
        double vEnd = variedRange.last();
        double vStepSize = (vEnd - vStart) / (double) (resolutionX - 1);
        for(int x = 1; x <= resolutionX; ++x) {
            // change values of varied element
			double vVal = vStart + (x-1) * vStepSize;
            for(int i = 0; i < observedValuesList.size(); ++i) {
                mData->set(vVal, x, 0, i);
            }
            vVals.append(vVal);
        }
	}

	// MAIN LOOP over parameter points

	//check if the diagram also has to be drawn in backwards mode.

	// two runs if backward calculation is on
	for(int phase = 0; phase <= runSecondIteration; ++phase) {

		restoreCurrentNetworkActivites();
		restoreNetworkConfiguration();

		for(int x = 1; x <= vVals.size() && mActiveValue->get(); ++x) {

			mProgressPercentage->set((double)(100*x/resolutionX));;

			if(resetSimulation) {
				triggerReset();
			}

			if(restoreNetConfiguration) {
				restoreNetworkConfiguration();
			}

			if(resetNetworkActivation) {
				restoreCurrentNetworkActivites();
			}

			//switch between forwards and backwards movement
			if(phase == 0) {
                variedValue->set(vVals.at(x-1)); // set actual value
			} else {
                variedValue->set(vVals.at(resolutionX-x));
			}


			// INNER LOOP over steps
			for(int j = 0; j < numberSteps && mActiveValue->get(); ++j) {
				// let the network run for 1 timestep
				triggerNetworkStep();
			}

			// plotting steps
			for(int j = 0; j < plottedSteps; ++j) {
                triggerNetworkStep();

                // Calculate average neuron activation
                for(int i = 0; i < observedValuesList.size(); ++i) {

                    QList<DoubleValue*> observedValues =
                                                observedValuesList.at(i);
                    int oSize = observedValues.size();
                    if(oSize == 0) {
                        reportProblem("BifurcationPlotter: Observed Values Size was 0!");
                        continue;
                    }

                    double oStart = oParams.at(i).at(0);
                    double oEnd = oParams.at(i).at(1);
                    double act = DynamicsPlotterUtil::getMeanValue(observedValues);

                    //Thread safety of matrix.
                    {
                        QMutexLocker guard(mDynamicsPlotManager->getMatrixLocker());

                        int posX = (phase == 0) ? x :
                                    mData->getMatrixWidth() - x;

                        if(act < oStart) {
                            //Indivate that a value is out of bound (mark with 2)
                            mData->set(2, posX, 1, i);
                        }
                        else if(act > oEnd) {
                            //Indicate that a value is out of bound (mark with 2)
                            mData->set(2, posX, resolutionY, i);
                        }
                        else {
                            double oStepSize = oParams.at(i).at(2);
                            int y = ceil((act - oStart)/oStepSize);

                            mData->set(1, posX, y, i);
                        }
                    }
                }
            }

			// runtime maintencance
			if(core->isShuttingDown()) {
				return;
			}
			core->executePendingTasks();
		}
	}

	// CLEAN UP
	variedValue->set(variedValueOrig);
	notifyNetworkParametersChanged(network);

	triggerReset();
	restoreNetworkConfiguration();
	restoreCurrentNetworkActivites();

}

}

