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

#include "Signal.h"
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "NerdConstants.h"
#include "SimulationConstants.h"

namespace nerd {

Signal::Signal(QString name, QString prefix) : ParameterizedObject(name, prefix), 
	mNextStepEvent(0), mResetSimulationEvent(0), mTimeStepSizeValue(0), 
	mCurrentStepValue(0), mName(name) 
{
	mEnabledValue = new BoolValue(true);

	addParameter("Enabled", mEnabledValue, true);

	Core::getInstance()->addSystemObject(this);
}

Signal::~Signal() {
	if(mResetSimulationEvent != 0) {
		mResetSimulationEvent->removeEventListener(this);
	}
	if(mNextStepEvent != 0) {
		mNextStepEvent->removeEventListener(this);
	}
}

bool Signal::init() {
	return true;
}

bool Signal::bind() {
	bool bindOK = true;

	Core* core = Core::getInstance();
	mNextStepEvent = core->getEventManager()->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	if(mNextStepEvent == 0) {
		Core::log("Signal: Could not find required Event.");
		bindOK = false;
	} else {
		mNextStepEvent->addEventListener(this);
	}

	mResetSimulationEvent = core->getEventManager()->getEvent(NerdConstants::EVENT_EXECUTION_RESET);
	if(mResetSimulationEvent == 0) {
		Core::log("Signal: Could not find required Event.");
		bindOK = false;
	} else {
		mResetSimulationEvent->addEventListener(this);
	}

	mTimeStepSizeValue = core->getValueManager()->getDoubleValue(SimulationConstants::VALUE_TIME_STEP_SIZE);
	mCurrentStepValue = core->getValueManager()->getIntValue(SimulationConstants::VALUE_EXECUTION_CURRENT_STEP);

	if((mTimeStepSizeValue == 0) || (mCurrentStepValue == 0)) {
		Core::log("Signal: Could not find required Value.");
		bindOK = false;
	}
	return bindOK;
}

bool Signal::cleanUp() {
	if(mNextStepEvent != 0) {
		mNextStepEvent->removeEventListener(this);
	}
	if(mResetSimulationEvent != 0) {
		mResetSimulationEvent->removeEventListener(this);
	}
	return true;
}

QString Signal::getName() const {
	return mName;
}

void Signal::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if((event == mNextStepEvent) && mEnabledValue->get()) {
		updateSignal();
	}
	if((event == mResetSimulationEvent) && mEnabledValue->get()) {
		resetSignal();
	}
}

}
