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

#include "DynamicsPlotterAdapter.h"

namespace nerd {
	
	DynamicsPlotterAdapter::DynamicsPlotterAdapter(const QString &name)
	: DynamicsPlotter(name), mCalculateDataCounter(0), mCountStoreNetworkActivities(0),
		mCountRestoreNetworkActivities(0), mCountStoreNetworkConfigurations(0),
		mCountRestoreNetworkConfigurations(0), mCountTriggerNetworkSteps(0),
		mCountNotifyParameterChanged(0)
	{
		
	}
	
	
	DynamicsPlotterAdapter::~DynamicsPlotterAdapter() {
	}
	
	bool DynamicsPlotterAdapter::init() {
		return DynamicsPlotter::init();
	}
	
	
	bool DynamicsPlotterAdapter::bind() {
		return DynamicsPlotter::bind();
	}
	
	
	bool DynamicsPlotterAdapter::cleanUp() {
		return DynamicsPlotter::cleanUp();
	}
	
	
	void DynamicsPlotterAdapter::eventOccured(Event *event) {
		DynamicsPlotter::eventOccured(event);
	}
	
	
	
	void DynamicsPlotterAdapter::calculateData() {
		mCalculateDataCounter++;
	}
	
	void DynamicsPlotterAdapter::storeCurrentNetworkActivities() {
		mCountStoreNetworkActivities++;
		DynamicsPlotter::storeCurrentNetworkActivities();
	}
	
	
	void DynamicsPlotterAdapter::restoreCurrentNetworkActivites() {
		mCountRestoreNetworkActivities++;
		DynamicsPlotter::restoreCurrentNetworkActivites();
	}
	
	
	void DynamicsPlotterAdapter::storeNetworkConfiguration() {
		mCountStoreNetworkConfigurations++;
		DynamicsPlotter::storeNetworkConfiguration();
	}
	
	
	void DynamicsPlotterAdapter::restoreNetworkConfiguration() {
		mCountRestoreNetworkConfigurations++;
		DynamicsPlotter::restoreNetworkConfiguration();
	}
	
	
	void DynamicsPlotterAdapter::triggerNetworkStep() {
		mCountTriggerNetworkSteps++;
		DynamicsPlotter::triggerNetworkStep();
	}
	
	
	bool DynamicsPlotterAdapter::notifyNetworkParametersChanged(ModularNeuralNetwork *network) {
		mCountNotifyParameterChanged++;
		return DynamicsPlotter::notifyNetworkParametersChanged(network);
	}
	
	
	
	
}


