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



#include "PID_PassiveActuatorModel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "SimulationConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new PID_PassiveActuatorModel.
 */
PID_PassiveActuatorModel::PID_PassiveActuatorModel(const QString &name)
	: PassiveActuatorModel(name), mTimeStepSize(0)
{
	mFriction = new DoubleValue(0.1);
	mMaxForce = new DoubleValue(0.5);
	mProportionalFactor = new DoubleValue(15.0);

	addParameter("Friction", mFriction);
	addParameter("MaxForce", mMaxForce);
	addParameter("Proportional", mProportionalFactor);
}


/**
 * Copy constructor. 
 * 
 * @param other the PID_PassiveActuatorModel object to copy.
 */
PID_PassiveActuatorModel::PID_PassiveActuatorModel(const PID_PassiveActuatorModel &other) 
	: PassiveActuatorModel(other), mTimeStepSize(0)
{
	mFriction = dynamic_cast<DoubleValue*>(getParameter("Friction"));
	mMaxForce = dynamic_cast<DoubleValue*>(getParameter("MaxForce"));
	mProportionalFactor = dynamic_cast<DoubleValue*>(getParameter("Proportional"));
}

/**
 * Destructor.
 */
PID_PassiveActuatorModel::~PID_PassiveActuatorModel() {
}


void PID_PassiveActuatorModel::setup() {
	PassiveActuatorModel::setup();

	if(mTimeStepSize == 0) {
		mTimeStepSize = Core::getInstance()->getValueManager()
					->getDoubleValue(SimulationConstants::VALUE_TIME_STEP_SIZE);
	}

	mPID_Controller.clear();
	mPID_Controller.setHistorySize(10);
	mPID_Controller.setPIDParameters(mProportionalFactor->get(), 5.5, 0.0);
	if(mTimeStepSize == 0) {
		Core::log("PID_PassiveActuatorModel: Could not find time step size value!");
		return;
	}
	mPID_Controller.setStepSize(mTimeStepSize->get());


}


void PID_PassiveActuatorModel::clear() {
	PassiveActuatorModel::clear();
}


void PID_PassiveActuatorModel::updateInputValues() {
}


void PID_PassiveActuatorModel::updateOutputValues() {

}




}



