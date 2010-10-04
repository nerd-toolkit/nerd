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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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

#include "PhysicalSimulationAlgorithm.h"
#include "SimulationConstants.h"
#include "Core/Core.h"

namespace nerd {

/**
 * Constructor.
 * @param name 
 */
PhysicalSimulationAlgorithm::PhysicalSimulationAlgorithm(const QString &name) 
		: ParameterizedObject(name) 
{
	mValuePrefix = QString("/Simulation/");
	mGravitationValue = new DoubleValue(9.81);
	addParameter("Gravitation", mGravitationValue, true);
	
	mIterationsPerStepValue = new IntValue(1);
	addParameter("IterationsPerStep", mIterationsPerStepValue, true);
	
	mTimeStepSizeValue = new DoubleValue(0.01);
	addParameter("TimeStepSize", mTimeStepSizeValue, true);

	mUpdateSensorsEvent = Core::getInstance()->getEventManager()->createEvent(
		SimulationConstants::EVENT_SIMULATION_UPDATE_SENSORS);
	if(mUpdateSensorsEvent == 0) {
		Core::log("PhysicalSimulationAlgorithm: Could not create event: "
			"[SimulationConstants::EVENT_SIMULATION_UPDATE_SENSORS]");
	}	

	mUpdateMotorsEvent = Core::getInstance()->getEventManager()
		->createEvent(SimulationConstants::EVENT_SIMULATION_UPDATE_MOTORS);
	if(mUpdateMotorsEvent == 0) {
		Core::log("PhysicalSimulationAlgorithm: Could not create event: "
			"[SimulationConstants::EVENT_SIMULATION_UPDATE_MOTORS]");
	}	
}

PhysicalSimulationAlgorithm::~PhysicalSimulationAlgorithm() {
}

int PhysicalSimulationAlgorithm::getIterationsPerStep() const {
	return mIterationsPerStepValue->get();
}

double PhysicalSimulationAlgorithm::getTimeStepSize() const {
	return mTimeStepSizeValue->get();
}

}



