/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   tfaber@uni-osnabrueck.de
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



#include "DynamicsPlotManager.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "DynamicsPlotConstants.h"

#include "Value/ValueManager.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new DynamicsPlotManager.
 */
DynamicsPlotManager::DynamicsPlotManager()
{
	//**************Till**************c//
	mValueManager = Core::getInstance()->getValueManager();
	mPlotterProgram = new StringValue();
	mActivePlotter = new StringValue();
	mInbuiltPlotterOnline = new BoolValue(true);
	
	mInbuiltPlotterOnline->setDescription("If TRUE, the inbuilt diagram is updated during calculation.");
	mPlotterProgram->setDescription("Specifies the output program, for example the internal plotter or external ones like Matlab");
	mPlotterProgram->getOptionList().append("Internal,matlab");
	mPlotterProgram->getOptionList().append("Matlab");
	mPlotterProgram->getOptionList().append("Matlab,Internal");
	
	mValueManager->addValue(DynamicsPlotConstants::VALUE_PLOTTER_OUTPUT_FORMAT, mPlotterProgram);
	mValueManager->addValue(DynamicsPlotConstants::VALUE_PLOTTER_ACTIVE_PLOTTER, mActivePlotter);
	mValueManager->addValue("/DynamicsPlotters/InbuiltPlotterOnline", mInbuiltPlotterOnline);
	
	//**************/Till**************c//
	
	
}



/**
 * Destructor.
 */
DynamicsPlotManager::~DynamicsPlotManager() {
}



/**
 * This method returns the singleton instance of the DynamicsPlotManager.
 * Note: calling this method in volves string comparisons and is computationally expensive.
 * Try to minimize the use of this method.
 *
 * @return the global DynamicsPlotManager.
 */
DynamicsPlotManager* DynamicsPlotManager::getInstance() {
	DynamicsPlotManager *dm = dynamic_cast<DynamicsPlotManager*>(Core::getInstance()
			->getGlobalObject(DynamicsPlotConstants::OBJECT_DYNAMICS_PLOT_MANAGER));
	if(dm == 0) {
		dm = new DynamicsPlotManager();
		if(!dm->registerAsGlobalObject()) {
			delete dm;
		}
		dm = dynamic_cast<DynamicsPlotManager*>(Core::getInstance()
			->getGlobalObject(DynamicsPlotConstants::OBJECT_DYNAMICS_PLOT_MANAGER));
	}
	return dm;
}


QString DynamicsPlotManager::getName() const {
	return "DynamicsPlotManager";
}


/**
 * Registeres this objets as global object. 
 * Note: this can be done only by one DynamicsPlotManager. For all managers after the first one
 * this method returns false. 
 * Note: this method is automatically called by getInstance() and has not to be done manualle for 
 * the singleton object of the DynamicsPlotManager.
 * 
 * @return true if successful, otherwise false.
 */
bool DynamicsPlotManager::registerAsGlobalObject() {
	Core::getInstance()->addGlobalObject(
		DynamicsPlotConstants::OBJECT_DYNAMICS_PLOT_MANAGER, this);

	return Core::getInstance()->getGlobalObject(
		DynamicsPlotConstants::OBJECT_DYNAMICS_PLOT_MANAGER) == this;
}

bool DynamicsPlotManager::init() {
	bool ok = true;

	return ok;
}

bool DynamicsPlotManager::bind() {
	bool ok = true;

	return ok;
}

bool DynamicsPlotManager::cleanUp() {
	bool ok = true;

	return ok;
}


/**
 * Adds a new DynamicsPlotter to the manager.
 * If plotter is NULL or the plotter is already registered at the manager, then false is returned.
 *
 * @return true if successful, otherwise false.
 */
bool DynamicsPlotManager::addDynamicsPlotter(DynamicsPlotter *plotter) {
	if(plotter == 0 || mDynamicsPlotters.contains(plotter)) {
		return false;
	}
	mDynamicsPlotters.append(plotter);
	return true;
}

/**
 * Removes an existing DynamicsPlotter from the manager.
 * If plotter is NULL or the plotter is not registered at the manager, then false is returned.
 *
 * @return true if successful, otherwise false.
 */
bool DynamicsPlotManager::removeDynamicsPlotter(DynamicsPlotter *plotter) {
	if(plotter == 0 || !mDynamicsPlotters.contains(plotter)) {
		return false;
	}
	mDynamicsPlotters.removeAll(plotter);
	return true;
}

const QList<DynamicsPlotter*>& DynamicsPlotManager::getDynamicsPlotters() const {
	return mDynamicsPlotters;
}




}




