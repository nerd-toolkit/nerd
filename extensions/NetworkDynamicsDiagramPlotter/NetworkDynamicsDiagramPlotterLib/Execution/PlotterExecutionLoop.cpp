/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   till.faber@uni-osnabrueck.de
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


#include "PlotterExecutionLoop.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "NerdConstants.h"
#include "DynamicsPlotConstants.h"
#include "Value/IntValue.h"
#include "DynamicsPlot/DynamicsPlotManager.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new PlotterExecutionLoop.
 */
PlotterExecutionLoop::PlotterExecutionLoop()
	: mDoShutDown(false)
{
	Core::getInstance()->addSystemObject(this);

	mExecutePlotters = new BoolValue(true); 
	Core::getInstance()->getValueManager()->addValue(DynamicsPlotConstants::VALUE_PLOTTER_EXECUTE, mExecutePlotters);

	mDelayValue = new IntValue(100);
	Core::getInstance()->getValueManager()->addValue("/Execution/Delay", mDelayValue);
}



/**
 * Destructor.
 */
PlotterExecutionLoop::~PlotterExecutionLoop() {
}

bool PlotterExecutionLoop::init() {
	bool ok = true;

	return ok;
}

bool PlotterExecutionLoop::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();
	mShutDownEvent = em->registerForEvent(NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this);

	if(mShutDownEvent == 0) {
		ok = false;
	}

	return ok;
}

bool PlotterExecutionLoop::cleanUp() {
	bool ok = true;

	return ok;
}

void PlotterExecutionLoop::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mShutDownEvent) {
		mDoShutDown = true;
	}
}

void PlotterExecutionLoop::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
}

QString PlotterExecutionLoop::getName() const {
	return "PlotterExecutionLoop";
}

void PlotterExecutionLoop::run() {

	Core::getInstance()->setMainExecutionThread();	

	DynamicsPlotManager *dpm = DynamicsPlotManager::getInstance();

	while(!mDoShutDown) {
		Core::getInstance()->executePendingTasks();
		if(mExecutePlotters->get()) {
			//TODO	
			if(dpm->getDynamicsPlotters().empty()) {
				Core::getInstance()->executePendingTasks();
				performWait();
			}
			else {
				bool executedPlotter = false;

				QList<DynamicsPlotter*> plotters = dpm->getDynamicsPlotters();
				for(QListIterator<DynamicsPlotter*> i(plotters); i.hasNext();) {
					DynamicsPlotter *plotter = i.next();
					if(plotter->getActiveValue()->get()) {
						plotter->execute();
					}
				}

				if(!executedPlotter) {
					Core::getInstance()->executePendingTasks();
					performWait();
				}
			}
		}
		else {
			performWait();
		}
	}
	if( !mDoShutDown) {
		if(Core::getInstance()->scheduleTask(new ShutDownTask())) {
		}
		Core::getInstance()->executePendingTasks();
	}
	while(!mDoShutDown) {
		Core::getInstance()->executePendingTasks();
	}

	Core::getInstance()->clearMainExecutionThread();
}


void PlotterExecutionLoop::performWait() {
	//wait delay ms (in small intervals of at max 10000 us to preserve reactivity.
	int waitedDelayTime = 0;
	int pendingDelay = mDelayValue->get();
	if(pendingDelay > 10000) {
		pendingDelay = 10000;
	}
	while(pendingDelay > 0 && !mDoShutDown) {

		usleep(pendingDelay);
		Core::getInstance()->executePendingTasks();

		waitedDelayTime += pendingDelay;
		pendingDelay = mDelayValue->get() - waitedDelayTime;
		if(pendingDelay > 10000) {
			pendingDelay = 10000;
		}
	}
}




}




