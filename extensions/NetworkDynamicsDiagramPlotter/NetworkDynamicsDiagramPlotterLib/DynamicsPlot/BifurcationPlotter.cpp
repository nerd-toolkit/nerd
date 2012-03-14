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
	mObservedElements->setDescription("IDs of network elements whose activations should "
									  "be observed"); ///NOTE////
	mObservedRanges = new StringValue("-1,1");
	mObservedRanges->setDescription("Minimum and maximum values for netwerk elements that "
									"are observed (eg 0,1)");
	mObservedResolution = new IntValue(600);
	mObservedResolution->setDescription("Defines how many points the ranges span");

	mVariedElement = new StringValue("0");
	mVariedElement->setDescription("");
	mVariedRange = new StringValue("0,1");
	mVariedRange->setDescription("Comma-separated start and end values for parameter variation "
								 "(eg 0,1)");
	mVariedResolution = new IntValue(600);
	mVariedResolution->setDescription("Defines how many points inside the given range for an "
									  "element parameter are being generated");

	mNumberSteps = new IntValue(1000);
	mNumberSteps->setDescription("Number of simulation steps that are computed");
	mPlottedSteps = new IntValue(10);
	mPlottedSteps->setDescription("Number of simulation steps to actually plot in the end");
	
	mResetNetworkActivation = new BoolValue(true);
	mResetNetworkActivation->setDescription("Whether or not to reset the network's activation "
											"after each parameter change");

	
	addParameter("Config/ObservedElements", mObservedElements, true);
	addParameter("Config/ObservedRanges", mObservedRanges, true);
	addParameter("Config/ObservedResolution", mObservedResolution, true);
	
	addParameter("Config/VariedElement", mVariedElement, true);
	addParameter("Config/VariedRange", mVariedRange, true);
	addParameter("Config/VariedResolution", mVariedResolution, true);
	
	addParameter("Config/NumberSteps", mNumberSteps, true);
	addParameter("Config/PlottedSteps", mPlottedSteps, true);
	
	addParameter("Config/ResetNetworkActivation", mResetNetworkActivation, true);
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
		Core::log("BifurcationPlotter: No elements to observe. Aborting.", true);
		return;
	}
	///NOTE////
	QList<QStringList> observedElementsList = 
				DynamicsPlotterUtil::parseElementString(observedElements); ///NOTE////
	
	QList< QList< DoubleValue *> > observedValuesList = 
				DynamicsPlotterUtil::getElementValues(observedElementsList, networkElements);
	
	QList<double> observedRanges = 
				DynamicsPlotterUtil::getDoublesFromString(mObservedRanges->get());
	
	if(observedRanges.isEmpty() || observedRanges.size() != 2*observedValuesList.size()) {
		Core::log("BifurcationPlotter: Invalid number of ranges given, maybe mismatch to "
				  "number of elements.", true); ///NOTE////
		return;
	}

	// Get parameters for varied network element
	QString variedElement = mVariedElement->get();
	if(variedElement.isEmpty()) {
		Core::log("BifurcationPlotter: No elements to vary. Aborting.", true); ///NOTE////
		return;
	}
	
	DoubleValue *variedValue = DynamicsPlotterUtil::getElementValue(variedElement, networkElements);
	QList<double> variedRange = DynamicsPlotterUtil::getDoublesFromString(mVariedRange->get());
	if(variedRange.size() != 2) {
		Core::log("BifurcationPlotter: Invalid number of range parameters given. Aborting.", true);
		return;
	}
	
	// save original value for clean-up
	double variedValueOrig = variedValue->get();
	storeCurrentNetworkActivities();

	// PREPARE data matrix
	mData->clear();
	mData->resize(mVariedResolution->get(), mObservedResolution->get(), observedValuesList.size());
	mData->fill(0);

	// MAIN LOOP over parameter points
	for(int x = 1; x <= mVariedResolution->get() && mActiveValue->get(); ++x) { ///NOTE////
		
		if(mResetNetworkActivation->get()) {
			restoreCurrentNetworkActivites();
		}
		
		// change values of varied element
		double vStart = variedRange.first();
		double vEnd = variedRange.last();
		double vStepSize = (vEnd - vStart) / (double) mVariedResolution->get();
		variedValue->set(vStart + (x-1) * vStepSize);
		
		// TODO call DynamicsPlotter:networkChanged (or similar), if available
		notifyNetworkParametersChanged(network); ///NOTE////

		// INNER LOOP over steps
		for(int j = 1; j <= mNumberSteps->get() && mActiveValue->get(); ++j) { ///NOTE////
			// let the network run for 1 timestep
			triggerNetworkStep();
			
			// plot values
			if(j > mNumberSteps->get() - mPlottedSteps->get()) {
				// Calculate average neuron activation
				for(int i = 0; i < observedValuesList.size(); ++i) {
					QList<DoubleValue*> observedValues = observedValuesList.at(i);
					double act = 0;
					
					for(int k = 0; k < observedValues.size(); ++k) {
						act += observedValues.at(k)->get();
					}
					act = act / observedValues.size();

					double oStart = observedRanges.at(i*2);
					double oEnd = observedRanges.at(i*2+1);

					if(oStart <= act && act <= oEnd) {
						double oStepSize = (oEnd - oStart) / (double) mObservedResolution->get();
						double y = floor(act/oStepSize - oStart/oStepSize);
						mData->set(1, x, y, i);
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
	
	// CLEAN UP
	variedValue->set(variedValueOrig);
	notifyNetworkParametersChanged(network); ///NOTE////
	
	restoreCurrentNetworkActivites();
}

}

