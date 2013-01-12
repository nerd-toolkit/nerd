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

#include "NextStepTriggerAction.h"
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "Event/TriggerEventTask.h"
#include "NerdConstants.h"


namespace nerd {

NextStepTriggerAction::NextStepTriggerAction(const QString &name)
	: QAction(name, 0), mPauseValue(0), mWasPaused(false), mStepCompletedEvent(0)
{
	mName = QString("NextStepTriggerAction (").append(name).append(")");
	connect(this, SIGNAL(triggered()), this, SLOT(triggerActivated()));
	connect(this, SIGNAL(enableAction(bool)), this, SLOT(setEnabled(bool)));

	Core::getInstance()->addSystemObject(this);
}

NextStepTriggerAction::~NextStepTriggerAction() {
}

bool NextStepTriggerAction::init() {
	return true;
}


bool NextStepTriggerAction::bind() {
	bool ok = true;
	
	ValueManager *vm = Core::getInstance()->getValueManager();

	mPauseValue = vm->getBoolValue(NerdConstants::VALUE_EXECUTION_PAUSE);

	if(mPauseValue == 0) {
		Core::log("NextStepTriggerAction: Did not find pause value [DISABLE FUNCTION]");
		emit enableAction(false);
	}
	else {
		mPauseValue->addValueChangedListener(this);
		emit enableAction(mPauseValue->get() == true);
	}

	EventManager *em = Core::getInstance()->getEventManager();
	mStepCompletedEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED,
							this);

	if(mStepCompletedEvent == 0) {
		Core::log("NextStepTriggerAction: Did not find step completed event [DISABLE FUNCTION]");
		emit enableAction(false);
	}

	return ok;
}


bool NextStepTriggerAction::cleanUp() {
	if(mPauseValue != 0) {
		mPauseValue->removeValueChangedListener(this);
	}
	if(mStepCompletedEvent != 0) {
		mStepCompletedEvent->removeEventListener(this);
	}
	Core::getInstance()->removeSystemObject(this);
	return true;
}

void NextStepTriggerAction::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mPauseValue) {
		emit enableAction(mPauseValue->get() == true);
	}
}

void NextStepTriggerAction::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(event == mStepCompletedEvent && mWasPaused) {
		mPauseValue->set(true);
	}
	mWasPaused = false;
}


QString NextStepTriggerAction::getName() const {
	return mName;
}


void NextStepTriggerAction::triggerActivated() {
	if(mPauseValue->get()) {
		mWasPaused = true;
		mPauseValue->set(false);
	}
}

}


