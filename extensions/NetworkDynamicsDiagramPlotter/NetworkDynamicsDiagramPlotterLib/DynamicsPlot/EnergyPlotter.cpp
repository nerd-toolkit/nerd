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


#include "EnergyPlotter.h"
#include <Util/DynamicsPlotterUtil.h>
#include "Core/Core.h"
#include "math.h"
#include <Math/Math.h>
#include <iostream>
#include "DynamicsPlotManager.h"

using namespace std;

namespace nerd {

EnergyPlotter::EnergyPlotter() : DynamicsPlotter("Energy") {
	// initialising
	mVariedElement = new StringValue("0");
	mVariedElement->setDescription("Network element to vary over time. Syntax "
								"is ID[[:FUNC:]PARAM]  (default is output "
								"value of neuron or synapse strength, so "
								"for the bias value use ID:b)");

	mVariedRange = new StringValue("0,1");
	mVariedRange->setDescription("Comma-separated start and end values for parameter variation "
								 "(eg 0,1)");

	mResolutionX = new IntValue(600);
	mResolutionX->setDescription("How many data points to generate for X axis");
	mResolutionY = new IntValue(600);
	mResolutionY->setDescription("How many data points to generate for Y axis");

	mPreIterations = new IntValue(1000);
	mPreIterations->setDescription("Number of simulation steps to reach attractor");
	mIterations = new IntValue(10);
	mIterations->setDescription("Number of simulation steps to run on attractor");
	mPostIterations = new IntValue(0);
	mPostIterations->setDescription("Number of steps to run additionally, if no attractor is found initially");

	mResetNetworkActivation = new BoolValue(true);
	mResetNetworkActivation->setDescription("Whether or not to reset the network's activation "
											"after each parameter change");

	mRestoreNetworkConfiguration = new BoolValue(true);
	mRestoreNetworkConfiguration->setDescription("If true, then all network parameters like bias, "
					"weight, observable parameters are reset to the initial state before each run");

	mResetSimulator = new BoolValue(true);
	mResetSimulator->setDescription("If a simulation is used, then this property "
									"decides whether the simulation is reset before each run.");

	addParameter("Config/VariedElement", mVariedElement, true);
	addParameter("Config/VariedRange", mVariedRange, true);

	addParameter("Config/ResolutionX", mResolutionX, true);
	addParameter("Config/ResolutionY", mResolutionY, true);

	addParameter("Config/PreIterations", mPreIterations, true);
	addParameter("Config/Iterations", mIterations, true);
	addParameter("Config/PostIterations", mPostIterations, true);

	addParameter("Config/ResetNetworkActivation", mResetNetworkActivation, true);
	addParameter("Config/RestoreNetworkConfiguration", mRestoreNetworkConfiguration, true);
	addParameter("Config/ResetSimulation", mResetSimulator, true);

	mTitleNames->set("Energy Plot|Energy Plot|Energy Plot|Energy Plot|Energy Plot|Energy Plot");
}

EnergyPlotter::~EnergyPlotter() {}

void EnergyPlotter::calculateData() {

	// get program core
	Core *core = Core::getInstance();

	// get network
	ModularNeuralNetwork *network = getCurrentNetwork();
	QList<NeuralNetworkElement*> networkElements;
	network->getNetworkElements(networkElements);

	// Get parameters for varied network element
	QString variedElement = mVariedElement->get();
	if(variedElement.isEmpty()) {
		reportProblem("EnergyPlotter: No elements to vary. Aborting.");
		return;
	}

	DoubleValue *variedValue = DynamicsPlotterUtil::getElementValue(
					variedElement, networkElements, &mNeuronsWithActivationsToTransfer);

	if(variedValue == 0) {
		reportProblem("EnergyPlotter: Element to vary does not exist.");
		return;
	}

	QList<double> variedRange =
		DynamicsPlotterUtil::getDoublesFromString(mVariedRange->get());

	if(variedRange.size() != 2) {
		reportProblem("EnergyPlotter: Invalid number of range parameters given. Aborting.");
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

	int resolutionX = mResolutionX->get();
	int resolutionY = mResolutionY->get();

	//avoid division by zero!
	if(resolutionX < 2 || resolutionY < 2) {
		return;
	}

	double vStart = variedRange.first();
	double vEnd = variedRange.last();
	double vStepSize = (vEnd - vStart) / (double) (resolutionX - 1);

    // change values of varied element
    QList<double> vValues;
    for(int x = 0; x < resolutionX; ++x) {
        vValues.append(vStart + x * vStepSize);
    }

	int preIterations = mPreIterations->get();
	int iterations = mIterations->get();
	int postIterations = mPostIterations->get();

	// PREPARE data matrix
	{
		//Thread safety of matrix.
		QMutexLocker guard(mDynamicsPlotManager->getMatrixLocker());

		mData->clear();
		mData->resize(resolutionX + 1, resolutionY + 1, 1);
		mData->fill(0);

	}

	QList<Neuron*> neurons = network->getNeurons();
	QList<double> energies;

	// MAIN LOOP over parameter points

    for(int x = 0; x < vValues.size() && mActiveValue->get(); ++x) {

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

        variedValue->set(vValues.at(x)); // set actual value

        // PRE-ITERATIONS - 1
        for(int j = 1; j < preIterations && mActiveValue->get(); ++j) {
            triggerNetworkStep();
        }

        QList< QList<double> > activations;

        // ITERATIONS + 1
        for(int j = 0; j <= iterations && mActiveValue->get(); ++j) {
            triggerNetworkStep();
            activations.append(DynamicsPlotterUtil::getNeuronActivations(network));
        }

        // look for attractor/period
        int period = DynamicsPlotterUtil::findAttractorPeriod(activations);

        // no attractor found
        if(period == 0) {
            // run additional PostIterations, if any
            for(int j = 0; j < postIterations; ++j) {
                activations.clear();
                triggerNetworkStep();
                activations.append(DynamicsPlotterUtil::getNeuronActivations(network));
            }
            // look for an attractor again
            period = DynamicsPlotterUtil::findAttractorPeriod(activations);
        }

        // found attractor
        if(period > 0) {

            // calculate the actual energy of the attractor!
            double sum = 0.0;
            for(int p = 1; p < period; ++p) {
                QList<double> curr = activations.at(p);
                QList<double> last = activations.at(p-1);
                double currSum = 0.0; //sum_i=1^N
                for(int n = 0; n < curr.size(); ++n) {
                    currSum += pow(curr.at(n) - last.at(n), 2); //a(t)-a(t-1))^2
                }
                sum += currSum / curr.size(); // sum_i=1^N / N
            }
            double energy = sum / period; // sum... / p
            energies.append(energy);


            // updating the matrix and axes:
            //
            // find smallest and biggest energy
            double ymin = energies.first(); double ymax = energies.first();
            for(int i = 1; i < energies.size(); ++i) {
                double y = energies.at(i);
                if(y < ymin) {
                    ymin = y;
                }
                if(y > ymax) {
                    ymax = y;
                }
            }
            if(ymax-ymin == 0) {
                ymin = 0;
                ymax = 1;
            }
            double ystep = (ymax - ymin) / (double)(resolutionY - 1);

            {
                //Thread safety of matrix.
                QMutexLocker guard(mDynamicsPlotManager->getMatrixLocker());

                // clear data matrix
                mData->fill(0);

                // rescale
                for(int y = 1; y <= resolutionY; ++y) {
                    double v = ymin + (y-1) * ystep;
                    mData->set(Math::round(v, 5), 0, y, 0);
                }

                // fill rescaled matrix again
                for(int x = 1; x <= energies.size(); ++x) {
                    double v = min(max(ymin, energies.at(x - 1)), ymax);
                    int y = ceil(((v - ymin) / ystep) + 1);
                    mData->set(1, x, y, 0);
                    mData->set(vValues.at(x-1), x, 0, 0);
                }

            }
        }

        // runtime maintencance
        if(core->isShuttingDown()) {
            return;
        }
        core->executePendingTasks();
    }

	// CLEAN UP
	variedValue->set(variedValueOrig);
	notifyNetworkParametersChanged(network);

	triggerReset();
	restoreNetworkConfiguration();
	restoreCurrentNetworkActivites();

}

}

