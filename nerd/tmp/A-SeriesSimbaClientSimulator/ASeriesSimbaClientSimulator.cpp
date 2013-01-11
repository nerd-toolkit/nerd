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



#include "ASeriesSimbaClientSimulator.h"
#include "Core/Core.h"
#include "Value/InterfaceValue.h"
#include "Physics/SimObjectGroup.h"
#include "Physics/PhysicsManager.h"
#include "Collections/ODE_Physics.h"
#include <iostream>
#include "Physics/SimulationEnvironmentManager.h"
#include "PlugIns/SimpleObjectFileParser.h"
#include "PlugIns/KeyFramePlayer.h"
#include "Physics/Physics.h"
#include "PlugIns/PlugInManager.h"
#include "PlugIns/StepsPerSecondCounter.h"
#include <iostream>
#include "Collections/ModelPrototypes.h"
#include "PlugIns/SimObjectGroupPrinter.h"
#include "Collections/TerminateTryCollisionRule.h"

using namespace std;

using namespace nerd;

ASeriesSimbaClientSimulator::ASeriesSimbaClientSimulator(int argc, char *argv[])
	: NerdBaseApplication(argc, argv, true), mGuiMainWindow(0), mSimbaCom(0)
{
}

ASeriesSimbaClientSimulator::~ASeriesSimbaClientSimulator() {
}


QString ASeriesSimbaClientSimulator::getName() const {
	return "ASeriesDirectPositionControlApplication";
}

bool ASeriesSimbaClientSimulator::cleanUp() {
	QThread::quit();
	return true;
}

bool ASeriesSimbaClientSimulator::setupGui() {

	if(mEnableGui) {
		mGuiMainWindow = new GuiMainWindow(false, false);
		connect(this, SIGNAL(showGui()), mGuiMainWindow, SLOT(showWindow()));
	}
	return true;
}
	
bool ASeriesSimbaClientSimulator::setupSimulator() {
	Physics::install();
	//install ODE PhysicsLayer
	new ODE_Physics();
	mSimbaCom = new SimbaUDPCommunication();

	//install file parser
	new SimpleObjectFileParser();
	new StepsPerSecondCounter();
	new SimObjectGroupPrinter();

	return true;
}

void ASeriesSimbaClientSimulator::run() {

	setUp();
	startSystemLoop();
	QThread::exec();
}

bool ASeriesSimbaClientSimulator::setUp() {
	setupSimulator();
	buildSimulationModel();
	return true;
}


bool ASeriesSimbaClientSimulator::startSystemLoop() {
	Core *core = Core::getInstance();

	loadValues(mArgument_ival->getEntries());

	if(!core->isInitialized()) {
		if(!core->init()) {
			qDebug("Warning: Could not initialize Core. Terminating Application.");
			return false;
		}
	}

	Physics::getPhysicsManager()->resetSimulation();
	
	loadValues(mArgument_val->getEntries());

	emit showGui();

	mSimbaCom->setPort(mServerPort);
	mSimbaCom->startListening(this);
	mSimbaCom->start();

	return true;
}

bool ASeriesSimbaClientSimulator::buildSimulationModel() {

	Physics::getSimulationEnvironmentManager()->createSnapshot();

	return true;
} 






