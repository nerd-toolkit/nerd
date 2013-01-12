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


#include "TransientPlotter.h"
#include <Util/DynamicsPlotterUtil.h>
#include "Core/Core.h"
#include "math.h"
#include <Math/Math.h>
#include <iostream>

using namespace std;

namespace nerd {

	TransientPlotter::TransientPlotter() : DynamicsPlotter("Transient"), mCurrentMarker(1) {
	// initialising

	mObservedElementsX = new StringValue("0");
	mObservedElementsX->setDescription("Network elements to observe and plot (x-axis)\n"
									"Syntax is ID:[FUNC:]PARAM, eg 15:a or 16:sf:Xi\n"
									"Use comma-separation for mean and '|' for "
									"multiple plots");
	mObservedElementsY = new StringValue("0");
	mObservedElementsY->setDescription("Network elements to observe and plot (y-axis)\n"
									"Syntax is as above, be consistent!");
	
	mObservedRangesX = new StringValue("-1,1");
	mObservedRangesX->setDescription("");
	mObservedRangesY = new StringValue("-1,1");
	mObservedRangesY->setDescription("");
	
	mResolutionX = new IntValue(400);
	mResolutionX->setDescription("How many data points to generate for "
										"X axis");
	mResolutionY = new IntValue(400);
	mResolutionY->setDescription("How many data points to generate for "
										"Y axis");

	mStepsToRun = new IntValue(0);
	mStepsToRun->setDescription("Number of simulation steps that are computed "
								"_before_ plotting!");
	mStepsToPlot = new IntValue(100);
	mStepsToPlot->setDescription("Number of simulation steps that are plotted");
	
	mKeepPreviousData = new BoolValue(false);
	mKeepPreviousData->setDescription("If true, then the plot will be drawn on top of the previous data, i.e. "
									  "the data matrix will not be deleted before use. However, if the size of "
									  "the matrix changes, then its content will still be deleted. \n"
									  "To make it easier to identify different analyzer runs in the plot "
									  "each run uses a different color.");
	

	addParameter("Config/ObservedElementsX", mObservedElementsX, true);
	addParameter("Config/ObservedElementsY", mObservedElementsY, true);
	addParameter("Config/ObservedRangesX", mObservedRangesX, true);
	addParameter("Config/ObservedRangesY", mObservedRangesY, true);
	addParameter("Config/ResolutionX", mResolutionX, true);
	addParameter("Config/ResolutionY", mResolutionY, true);
	
	addParameter("Config/StepsToRun", mStepsToRun, true);
	addParameter("Config/StepsToPlot", mStepsToPlot, true);
	
	addParameter("Config/KeepPreviousData", mKeepPreviousData, true);
	
	mTitleNames->set("Transient");
}

TransientPlotter::~TransientPlotter() {}

void TransientPlotter::calculateData() {
	
	// get program core
	Core *core = Core::getInstance();
	
	// get network
	ModularNeuralNetwork *network = getCurrentNetwork();
	QList<NeuralNetworkElement*> networkElements;
	network->getNetworkElements(networkElements);


	// Get parameters for observed elements
	QString observedElementSX = mObservedElementsX->get();
	QString observedElementSY = mObservedElementsY->get();
	if(observedElementSX.isEmpty() || observedElementSY.isEmpty()) {
		reportProblem("TransientPlotter: No elements to observe.");
		return;
	}

	QList<QStringList> observedElementsX, observedElementsY;
	observedElementsX = DynamicsPlotterUtil::parseElementString(observedElementSX);	
	observedElementsY = DynamicsPlotterUtil::parseElementString(observedElementSY);	

	QList< QList<DoubleValue*> > observedValuesX, observedValuesY;
	observedValuesX = DynamicsPlotterUtil::getElementValues(
		observedElementsX, networkElements);
	observedValuesY = DynamicsPlotterUtil::getElementValues(
		observedElementsY, networkElements);
	
	if(observedValuesX.isEmpty() ||
		observedValuesX.size() != observedValuesY.size()) {
		reportProblem("TransientPlotter: Number of plots does not match "
						"through the dimensions.");
		return;
	}
	

	QList<double> observedRangesX = 
				DynamicsPlotterUtil::getDoublesFromString(mObservedRangesX->get());
	QList<double> observedRangesY = 
				DynamicsPlotterUtil::getDoublesFromString(mObservedRangesY->get());
				
	if(observedRangesX.size() != observedRangesY.size() ||
		observedRangesX.size() != 2*observedValuesX.size()) {
		reportProblem("TransientPlotter: Invalid ranges given.");
		return;
	}

		
	int resolutionX = mResolutionX->get();
	int resolutionY = mResolutionY->get();
	
	//avoid division by zero!
	if(resolutionX < 2 || resolutionY < 2) {
		reportProblem("TransientPlotter: Invalid resolution given.");
		return;
	}

	
	// Let costraint resolver run properly (order matters!)
	storeNetworkConfiguration();
	storeCurrentNetworkActivities();
	triggerReset();
	restoreCurrentNetworkActivites();
	restoreNetworkConfiguration();
	notifyNetworkParametersChanged(network);

	int nrPlots = observedValuesX.size();
	
	
	bool keepPreviousData = mKeepPreviousData->get();
	if(keepPreviousData
		&& (mData->getMatrixWidth() == (resolutionX + 1))
		&& (mData->getMatrixHeight() == (resolutionY + 1))
		&& (mData->getMatrixDepth() == nrPlots))
	{
		mCurrentMarker++;
		if(mCurrentMarker > 10) {
			mCurrentMarker = 1;
		}
	}
	else {
		//reset the marker color to 0
		keepPreviousData = false;
		mCurrentMarker = 1;
	}
	
	if(!keepPreviousData) {
		// PREPARE data matrix
		mData->clear();
		mData->resize(resolutionX + 1, resolutionY + 1, nrPlots);
		mData->fill(0);
	}

	
	// calculate values and draw axes
	QList< QHash<QString, double> > memX;
	QList< QHash<QString, double> > memY;

	for(int i = 0; i < nrPlots; ++i) {
		QHash<QString, double> mX;
		QHash<QString, double> mY;

		mX["start"] = observedRangesX.at(i*2);
		mX["end"] = observedRangesX.at(i*2+1);
		mX["step"] = (mX["end"] - mX["start"]) / (double)(resolutionX - 1);
		for(int x = 1; x <= resolutionX; ++x) {
			double valX = mX["start"] + (x-1) * mX["step"];
			mData->set(valX, x, 0, i);
		}
		memX.append(mX);

		mY["start"] = observedRangesY.at(i*2);
		mY["end"] = observedRangesY.at(i*2+1);
		mY["step"] = (mY["end"] - mY["start"]) / (double)(resolutionY - 1);
		for(int y = 1; y <= resolutionY; ++y) {
			double valY = mY["start"] + (y-1) * mY["step"];
			mData->set(valY, 0, y, i);
		}
		memY.append(mY);

	}


	int stepsRun = mStepsToRun->get();
	int stepsPlot = mStepsToPlot->get();
	
	for(int s = 0; s < stepsRun && mActiveValue->get(); ++s) {
		triggerNetworkStep();
	}
		
			
	for(int p = 0; p < stepsPlot && mActiveValue->get(); ++p) {

		triggerNetworkStep();

		for(int i = 0; i < nrPlots && mActiveValue->get(); ++i) {

			QList<DoubleValue*> elemsX = observedValuesX.at(i);
			QList<DoubleValue*> elemsY = observedValuesY.at(i);

			QHash<QString, double> mX = memX.at(i);
			QHash<QString, double> mY = memY.at(i);

			double vX = DynamicsPlotterUtil::getMeanValue(elemsX);
			double vY = DynamicsPlotterUtil::getMeanValue(elemsY);

			int color = mCurrentMarker;
			if(vX > mX["end"] || vX < mX["start"] ||
				vY > mY["end"] || vY < mY["start"]) 
			{
				if(!keepPreviousData) {
					color = 2;
				}
				vX = max(mX["start"], min(mX["end"], vX));
				vY = max(mY["start"], min(mX["end"], vY));
			}
				
			double pX = ceil((vX - mX["start"]) / mX["step"] + 1);
			double pY = ceil((vY - mY["start"]) / mY["step"] + 1);
			mData->set(color, pX, pY, i);
			
		}
			
		// runtime maintencance
		if(core->isShuttingDown()) {
			return;
		}
		core->executePendingTasks();
	}
	
	// CLEAN UP
	notifyNetworkParametersChanged(network);
	triggerReset();
	restoreNetworkConfiguration();
	restoreCurrentNetworkActivites();

}

}

