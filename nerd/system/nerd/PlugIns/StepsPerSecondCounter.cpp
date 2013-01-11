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

#include "StepsPerSecondCounter.h"
#include <QCoreApplication>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "NerdConstants.h"
#include "Event/EventManager.h"
#include <iostream>

using namespace std;


namespace nerd {

StepsPerSecondCounter::StepsPerSecondCounter()
	: mStepsPerSecond(0), mPrintToConsole(0), mTimer(0), mStepCounter(0), mStepCompletedEvent(0),
	  mIndividualStartedEvent(0), mIndividualCompletedEvent(0)
{
	mStepsPerSecond = new IntValue(0);
	mPrintToConsole = new BoolValue(false);

	ValueManager *vm = Core::getInstance()->getValueManager();

	vm->addValue(NerdConstants::VALUE_NERD_STEPS_PER_SECOND, mStepsPerSecond);
	vm->addValue("/StepsPerSecondCounter/StepsPerSecond", mStepsPerSecond);
	vm->addValue("/StepsPerSecondCounter/PrintToConsole", mPrintToConsole);

	mTimer = new QTimer();
	mTimer->moveToThread(QCoreApplication::instance()->thread());
	mTimer->setInterval(1000);

	connect(this, SIGNAL(startTimer()),
			mTimer, SLOT(start()));
	connect(this, SIGNAL(stopTimer()),
			mTimer, SLOT(stop()));
	connect(mTimer, SIGNAL(timeout()),
			this, SLOT(timerExpired()));

	Core::getInstance()->addSystemObject(this);
}

StepsPerSecondCounter::~StepsPerSecondCounter() {
	delete mTimer;
}

QString StepsPerSecondCounter::getName() const {
	return "StepsPerSecondCounter";
}

bool StepsPerSecondCounter::init() {
	bool ok = true;

	return ok;
}

bool StepsPerSecondCounter::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mStepCompletedEvent = em->registerForEvent(
			NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, this);
	mIndividualCompletedEvent = em->registerForEvent(
			NerdConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL, this);
	mIndividualStartedEvent = em->registerForEvent(
			NerdConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED, this);

	if(mStepCompletedEvent == 0
		|| mIndividualCompletedEvent == 0 
		|| mIndividualStartedEvent == 0)
	{
		Core::log("StepsPerSecondCounter: Could not find required Events. "
				  "[FUNCTION DISABLED]");
	}

	emit startTimer();

	return ok;
}

bool StepsPerSecondCounter::cleanUp() {
	emit stopTimer();
	return true;
}


void StepsPerSecondCounter::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mStepCompletedEvent) {
		mStepCounter++;
	}
}

void StepsPerSecondCounter::timerExpired() {
	mStepsPerSecond->set(mStepCounter);
	if(mPrintToConsole->get()) {
		cout << "SPS: " << mStepCounter << endl;
	}
	mStepCounter = 0;
}

}



