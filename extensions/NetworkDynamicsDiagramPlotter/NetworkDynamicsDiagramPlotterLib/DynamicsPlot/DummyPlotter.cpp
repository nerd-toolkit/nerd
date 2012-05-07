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


#include "DummyPlotter.h"
#include "Core/Core.h"
#include <iostream>

using namespace std;

namespace nerd {

DummyPlotter::DummyPlotter() : DynamicsPlotter("Dummy") {
	// initialising

	mNumberSteps = new IntValue(0);
	mNumberSteps->setDescription("Number of simulation steps that are computed,"
									" for unlimited runs use zero");
	
	mCurrentStep = new IntValue(0);
	
	addParameter("Config/NumberSteps", mNumberSteps, true);
	addParameter("Data/CurrentStep", mCurrentStep, true);
}

DummyPlotter::~DummyPlotter() {}

void DummyPlotter::calculateData() {
	
	// get program core
	Core *core = Core::getInstance();
	
	// get network
	ModularNeuralNetwork *network = getCurrentNetwork();
	storeNetworkConfiguration();
	storeCurrentNetworkActivities();
	
	// PREPARE data matrix
	mData->clear();
	mData->resize(0, 0, 0);
	
	int numberSteps = mNumberSteps->get();
	int step = 0;

	while(mActiveValue->get()) {
		// let the network run for 1 timestep
		triggerNetworkStep();
		mCurrentStep->set(++step);
		
		if(numberSteps > 0 && step >= numberSteps) {
			break;
		}
		
		// runtime maintencance
		if(core->isShuttingDown()) {
			return;
		}
		core->executePendingTasks();
	}
	
	restoreNetworkConfiguration();
	restoreCurrentNetworkActivites();

}

}

