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

#include "EvolutionRunner.h"
#include <QCoreApplication>
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "NerdConstants.h"
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include <iostream>
#include "Value/ValueManager.h"
#include "EvolutionConstants.h"
#include <QCoreApplication>


using namespace std;

namespace nerd {

EvolutionRunner::EvolutionRunner()
	: mDoShutDown(false), mShutDownEvent(0), mRunEvolutionValue(0)
{
	connect(this, SIGNAL(quitMainApplication()),
			QCoreApplication::instance(), SLOT(quit()));

	mRunEvolutionValue = new BoolValue(false);
	Core::getInstance()->getValueManager()->addValue(
			EvolutionConstants::VALUE_EVO_RUN_EVOLUTION, mRunEvolutionValue);

	Core::getInstance()->addSystemObject(this);
}

EvolutionRunner::~EvolutionRunner() {	
	Core::getInstance()->deregisterThread(this);
}


bool EvolutionRunner::init() {
	bool ok = true;

	Core::getInstance()->registerThread(this);

	return ok;
}


bool EvolutionRunner::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mShutDownEvent = em->registerForEvent(NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this);

	if(mShutDownEvent == 0) 
	{
		Core::log("SimulationLoop: Required Events could not be found.");
		ok = false;
	}

	return ok;
}


bool EvolutionRunner::cleanUp() {
	bool ok = true;

	return ok;
}

		
void EvolutionRunner::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mShutDownEvent) {
		mDoShutDown = true;
	}
}




QString EvolutionRunner::getName() const {
	return "EvolutionRunner";
}

void EvolutionRunner::run() {
	//set the running thread as main execution thread.
	Core::getInstance()->setMainExecutionThread();
	
	while(!mDoShutDown) {
		while(!mRunEvolutionValue->get() && !mDoShutDown) {
			QCoreApplication::instance()->thread()->wait(150);
			Core::getInstance()->executePendingTasks();
		}
		if(mDoShutDown) {
			break;
		}
		Evolution::getEvolutionManager()->processNextGeneration();
		Core::getInstance()->executePendingTasks();
	}

	if(!mDoShutDown) {
		if(Core::getInstance()->scheduleTask(new ShutDownTask())) {
		}
		Core::getInstance()->executePendingTasks();
	}
	while(!mDoShutDown) {
		Core::getInstance()->executePendingTasks();
	}
	
	Core::getInstance()->clearMainExecutionThread();

	//emit quitMainApplication();
}


BoolValue* EvolutionRunner::getRunEvolutionValue() const {
	return mRunEvolutionValue;
}

}


