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


#include "BifurcationPlotter.h"
#include <Util/DynamicsPlotterUtil.h>
#include "Core/Core.h"
#include "math.h"
#include <iostream>

using namespace std;

namespace nerd {

BifurcationPlotter::BifurcationPlotter() : DynamicsPlotter("Bifurcation") {
	// initialising
	mObservedElements = new StringValue("0");
	mObservedElements->setDescription("IDs of network elements whose activations should be observed");
	mObservedRange = new StringValue("-1,1");
	mObservedRange->setDescription("Minimum and maximum values for netwerk elements that are observed (eg 0,1)");
	mObservedPoints = new IntValue(600);
	mObservedPoints->setDescription("Defines how many points the ranges span");

	mVariedElements = new StringValue("0");
	mVariedElements->setDescription("IDs of network elements whose values should be changed");
	mVariedRanges = new StringValue("0,1");
	mVariedRanges->setDescription("List of minimum and maximum values for netwerk elements that are changed, given as line-separated pairs (eg 0,1|1,2)");
	mVariedPoints = new IntValue(600);
	mVariedPoints->setDescription("Defines how many points inside the given range for an element parameter are being generated");

	mNumberPreSteps = new IntValue(50);
	mNumberPreSteps->setDescription("Number of simulation steps computed beforehand");
	mNumberSteps = new IntValue(1000);
	mNumberSteps->setDescription("Number of simulation steps that are computed");
	mPlottedSteps = new IntValue(10);
	mPlottedSteps->setDescription("Number of simulation steps to actually plot in the end");
	
	mResetNetworkActivation = new BoolValue(true);
	mResetNetworkActivation->setDescription("Whether or not to reset the network's activation after each parameter change");

	addParameter("Config/ObservedElements", mObservedElements, true);
	addParameter("Config/ObservedRange", mObservedRange, true);
	addParameter("Config/ObservedPoints", mObservedPoints, true);
	addParameter("Config/VariedElements", mVariedElements, true);
	addParameter("Config/VariedRanges", mVariedRanges, true);
	addParameter("Config/VariedPoints", mVariedPoints, true);
	addParameter("Config/NumberPreSteps", mNumberPreSteps, true);
	addParameter("Config/NumberSteps", mNumberSteps, true);
	addParameter("Config/PlottedSteps", mPlottedSteps, true);
	addParameter("Config/ResetNetworkActivation", mResetNetworkActivation, true);
}

BifurcationPlotter::~BifurcationPlotter() {}

void BifurcationPlotter::calculateData() {
	
	// get program core
	Core *core = Core::getInstance();
	// get network
	NeuralNetwork *network = getCurrentNetwork();

	// get list of DoubleValue references for observed elements
	QList<qulonglong> observedElementsList = DynamicsPlotterUtil::getIDsFromString(mObservedElements->get());
	QList<DoubleValue*> observedValuesList = DynamicsPlotterUtil::getElementValuesFromIDs(observedElementsList, network, 2);
	QList<double> observedRangeList = DynamicsPlotterUtil::getDoublesFromString(mObservedRange->get());
	
	// get list of DoubleValue references for varied elements (copypasta ...)
	QList<qulonglong>variedElementsList = DynamicsPlotterUtil::getIDsFromString(mVariedElements->get());
	QList<DoubleValue*> variedValuesList = DynamicsPlotterUtil::getElementValuesFromIDs(variedElementsList, network);
	QList< QPair<double, double> > variedRangesList = DynamicsPlotterUtil::getPairsFromString(mVariedRanges->get());
	
	// some checks for syntactic validity of parameters
	if(variedValuesList.size() != variedRangesList.size()) {
		Core::log("Elements to vary and their ranges don't match up. Aborting.", true);
		return;
	}
	
	// save original values for clean-up
	QList<double> variedValuesOrig;
	for(int i = 0; i < variedValuesList.size(); ++i) {
		variedValuesOrig.append(variedValuesList.at(i)->get());
	}
	storeCurrentNetworkActivities();

	// PREPARE data matrix
	mData->clear();
	mData->resize(mVariedPoints->get(), mObservedPoints->get(), 1);
	mData->fill(0);

	// MAIN LOOP over parameter points
	for(int x = 1; x <= mVariedPoints->get() && mActiveValue; ++x) {
		
		if(mResetNetworkActivation->get()) {
			restoreCurrentNetworkActivites();
		}
		
		// change values of varied elements
		for(int j = 0; j < variedValuesList.size(); ++j) {
			double start = variedRangesList.at(j).first;
			double end = variedRangesList.at(j).second;
			double step = (end - start) / (double) mVariedPoints->get();
			variedValuesList.at(j)->set(start + (x-1) * step);
		}
		// TODO call DynamicsPlotter:networkChanged (or similar), if available

		// PRE-activation runs
		for(int j = 1; j <= mNumberPreSteps->get() && mActiveValue; ++j) {
			triggerNetworkStep();
		}

		// INNER LOOP over steps
		for(int j = 1; j <= mNumberSteps->get() && mActiveValue; ++j) {
			// let the network run for 1 timestep
			triggerNetworkStep();
			
			// plot values
			if(j > mNumberSteps->get() - mPlottedSteps->get()) {
				// Calculate average neuron activation
				double act = 0;
				for(int k = 0; k < observedValuesList.size(); ++k) {
					act += observedValuesList.at(k)->get();
				}
				act = act / observedValuesList.size();
				
				double obsEnd = observedRangeList.at(1);
				double obsStart = observedRangeList.at(0);
				
				if(obsStart < act && act < obsEnd) {
					// calculate step size / increment
					double step = (obsEnd - obsStart) / (double)mObservedPoints->get();
					
					// calculate position in data matrix
					double y = floor(act / step - obsStart / step);
				
					// write to matrix
					mData->set(1, x, y, 0);
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
	for(int i = 0; i < variedValuesOrig.size(); ++i) {
		variedValuesList.at(i)->set(variedValuesOrig.at(i));
	}
	restoreCurrentNetworkActivites();
}

}

