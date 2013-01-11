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


#include "LyapunovExponent.h"
#include <Util/DynamicsPlotterUtil.h>
#include "Core/Core.h"
#include "math.h"
#include <Math/Math.h>
#include <iostream>

using namespace std;

namespace nerd {

	LyapunovExponent::LyapunovExponent() : DynamicsPlotter("Lyapunov") {
	// initialising

	mVariedElement = new StringValue("0");
	mVariedElement->setDescription("Specifies the element and parameter to vary in order "
								   "to simulate different initial conditions of the system");
	
	mVariedRange = new StringValue("-1,1");
	mVariedRange->setDescription("Parameter range to vary initial conditions in");
	
	mResolutionX = new IntValue(400);
	mResolutionX->setDescription("How many data points to generate for X axis");
	mResolutionY = new IntValue(400);
	mResolutionY->setDescription("How many data points to generate for Y axis");

	mStepsPrePlot = new IntValue(0);
	mStepsPrePlot->setDescription("Number of simulation steps that are computed "
								"_before_ plotting!");
	mStepsToPlot = new IntValue(100);
	mStepsToPlot->setDescription("Number of simulation steps that are calculated and plotted");

	mDrawNL = new BoolValue(true);
	mDrawNL->setDescription("Draw a red line at y=0 for orientation");
	
	addParameter("Config/VariedElement", mVariedElement, true);
	addParameter("Config/VariedRange", mVariedRange, true);
	addParameter("Config/ResolutionX", mResolutionX, true);
	addParameter("Config/ResolutionY", mResolutionY, true);
	
	addParameter("Config/StepsPrePlot", mStepsPrePlot, true);
	addParameter("Config/StepsToPlot", mStepsToPlot, true);

	addParameter("Config/DrawNL", mDrawNL, true);
	
	mTitleNames->set("Lyapunov Exponent");
}

LyapunovExponent::~LyapunovExponent() {}

void LyapunovExponent::calculateData() {
	
	// get program core
	Core *core = Core::getInstance();
	
	// get network
	ModularNeuralNetwork *network = getCurrentNetwork();
	QList<NeuralNetworkElement*> networkElements;
	network->getNetworkElements(networkElements);
	QList<DoubleValue*> networkValues =
		DynamicsPlotterUtil::getNetworkValues(networkElements);

	// Get parameters for varied element
	QString variedElement = mVariedElement->get();
	if(variedElement.isEmpty()) {
		reportProblem("LyapunovExponent: No element to vary.");
		return;
	}

	DoubleValue *variedValue = 
			DynamicsPlotterUtil::getElementValue(variedElement, networkElements);
	
	if(variedValue == 0) {
		reportProblem("LyapunovExponent: Invalid value or specifier.");
		return;
	}
	
	QList<double> variedRange = 
				DynamicsPlotterUtil::getDoublesFromString(mVariedRange->get());
				
	if(variedRange.size() != 2) {
		reportProblem("LyapunovExponent: Invalid parameter range.");
		return;
	}

		
	int resolutionX = mResolutionX->get();
	int resolutionY = mResolutionY->get();
	
	//avoid division by zero!
	if(resolutionX < 2 || resolutionY < 2) {
		reportProblem("LyapunovExponent: Invalid resolution given.");
		return;
	}

	
	// Let costraint resolver run properly (order matters!)
	storeNetworkConfiguration();
	storeCurrentNetworkActivities();
	triggerReset();
	restoreCurrentNetworkActivites();
	restoreNetworkConfiguration();
	notifyNetworkParametersChanged(network);

	// save original value
	double originalValue = variedValue->get();

	// prepare data matrix
	mData->clear();
	mData->resize(resolutionX + 1, resolutionY + 1, 1);
	mData->fill(0);

	double valStep = (variedRange.at(1) - variedRange.at(0)) / (double) (resolutionX - 1);
	QList<double> variedValues;

	for(int x = 1; x <= resolutionX; ++x) {
		double val = variedRange.at(0) + (x-1) * valStep;
		variedValues.append(val);
		
		mData->set(val, x, 0, 0);
	}
	
	int stepsPrePlot = mStepsPrePlot->get();
	int stepsToPlot = mStepsToPlot->get();

	bool drawNL = mDrawNL->get();

	QList<double> ynum;
	double eps = pow(10,-9);
	for(int x = 0; x < variedValues.size(); ++x) {

		// set initial conditions of this run/trajectory
		variedValue->set(variedValues.at(x));
		notifyNetworkParametersChanged(network);
		
		// calculate activation after X PrePlot-Steps
		for(int s = 0; s < stepsPrePlot && mActiveValue->get(); ++s) {
			triggerNetworkStep();
		}
		
		// list for states			
		QList< QList<double> > networkStates;

		for(int s = 0; s < stepsToPlot && mActiveValue->get(); ++s) {

			triggerNetworkStep();
			
			// get current state of the network
			QList<double> networkState = 
					DynamicsPlotterUtil::getNetworkState(networkValues);
			
			// save to list
			networkStates.append(networkState);
		}

		double ljanum = 0;
		int c = 0;
		for(int i = 0; i < networkStates.size()-1; ++i) {
			double dy = 10000000, df = 100000000;
			bool found = false;

			for(int j = 0; j < networkStates.size()-1; ++j) {

				double d = DynamicsPlotterUtil::getDistance(
								networkStates.at(i), networkStates.at(j));

				if(d < dy && d > eps) {
					dy = d;
					df = DynamicsPlotterUtil::getDistance(
								networkStates.at(i+1), networkStates.at(j+1));
					found = true;
				}
				
			}
			
			if(found && dy != 0 && df != 0) {
				ljanum += log(df/dy);
				c++;
			}

		}

		// save current hightest exponent
		ynum.append(ljanum/c);

		// find smallest and biggest exponent
		double ymin = ynum.first(); double ymax = ynum.first();
		for(int i = 1; i < ynum.size(); ++i) {
			double y = ynum.at(i);
			if(y < ymin) {
				ymin = y;
			}
			if(y > ymax) {
				ymax = y;
			}
		}
		double ystep = (ymax - ymin) / (double)(resolutionY - 1);
		if(ystep == 0) {
			reportProblem("LyapunovExponent: No suitable data found.");
			ymin = 1;
			ymax = 1;
		}

		// clear data matrix
		mData->fill(0);

		// rescale
		for(int y = 1; y <= resolutionY; ++y) {
			double v = ymin + (y-1) * ystep;
			mData->set(Math::round(v,5), 0, y, 0);
		}
		
		// fill rescaled matrix again
		for(int x = 1; x <= ynum.size(); ++x) {
			double v = min(max(ymin,ynum.at(x-1)),ymax);
			int y = ceil(((v-ymin)/ystep)+1);
			mData->set(1, x, y, 0);
		}

		// find null position (if any)
		int ny = ceil(((-ymin)/ystep)+1);
		// and draw red line indicating y=0
		if(drawNL && ny < resolutionY && ny > 0) {
			for(int x = 0; x < resolutionX; ++x) {
				if(mData->get(x, ny, 0) == 0) {
					mData->set(2, x, ny, 0);
				}
			}
		}


		// runtime maintencance
		if(core->isShuttingDown()) {
			return;
		}
		core->executePendingTasks();
	
	}
	
	// re-set original parameter value
	variedValue->set(originalValue);
	// CLEAN UP
	notifyNetworkParametersChanged(network);
	triggerReset();
	restoreNetworkConfiguration();
	restoreCurrentNetworkActivites();

}

}

