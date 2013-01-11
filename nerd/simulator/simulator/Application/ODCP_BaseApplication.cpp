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


#include "ODCP_BaseApplication.h"
#include "Core/Core.h"
#include "PlugIns/CommandLineParser.h"
#include <QString>
#include <iostream>
#include "Value/ValueManager.h"
#include "Physics/Physics.h"
#include "PlugIns/KeyFramePlayer.h"
#include "PlugIns/PlugInManager.h"
#include "PlugIns/SimObjectGroupPrinter.h"

using namespace std;

namespace nerd {

ODCP_BaseApplication::ODCP_BaseApplication(const QString &controllableAgentName)
		: BaseApplication(), mUdpMotorInterface(0), mSimulationLoop(0), 
		  mRunner(0), mControllableAgentName(controllableAgentName)
{
// 	CommandLineArgument *mPlayArgument = 
// 		new CommandLineArgument("playKeyFrames", "play", "<agent> <file>",
// 			"Loads keyframe file <file> and starts to playback the keyframes on agent <agent>",
// 			2, 0, true);
// 
// 	QStringList files = mPlayArgument->getParameters();
// 
// 	for(int i = 0; i < files.size(); ++i) {
// 		QStringList args = files.at(i).split(" ");
// 		if(args.size() == 2) {
// 			QString keyFrameTarget(args.at(0));
// 			QString keyFrameFileName(args.at(1));
// 
// 			KeyFramePlayer *player = new KeyFramePlayer(keyFrameTarget);
// 			player->setKeyFrameFile(keyFrameFileName);
// 		}
// 		else {
// 			cerr << "Warning: Size was " << args.size() << endl;
// 		}
// 	}
}

ODCP_BaseApplication::~ODCP_BaseApplication() {
}

bool ODCP_BaseApplication::init() {
	return BaseApplication::init();
}


bool ODCP_BaseApplication::bind() {
	return BaseApplication::bind();
}


bool ODCP_BaseApplication::cleanUp() {
	return BaseApplication::cleanUp();
}


QString ODCP_BaseApplication::getName() const {
	return "ODCP_BaseApplication";
}


bool ODCP_BaseApplication::setUp() {
	bool ok = BaseApplication::setUp();

	if(!buildSimulationModel()) {
		Core::log("ODCP_BaseApplication: Problem while building the simulation model.");
		ok = false;
	}
	return ok;
}

bool ODCP_BaseApplication::setupApplication() {

	mSimulationLoop = new SimulationLoop();
	mSimulationLoop->setSimulationDelay(0);


	mUdpMotorInterface = new SimpleUdpMotorInterface(mControllableAgentName);

	CommandLineArgument *mPortArgument = 
		new CommandLineArgument("odcp_port", "port", "<port>",
			"Sets the port of the ODCP communication interface to <port>",
			1, 0, true);

	QStringList ports = mPortArgument->getParameters();
	if(ports.size() > 0) {
		int port = ports.at(0).toInt();
		mUdpMotorInterface->getPortValue()->set(port);
	}

	mRunner = new SimpleUdpMotorInterfaceRunner(mUdpMotorInterface);

	new SimObjectGroupPrinter();

	return true;
}	

bool ODCP_BaseApplication::runApplication() {

	Physics::getSimulationEnvironmentManager()->createSnapshot();

	if(mRunner != 0) {
		mRunner->start();
	}
	else {
		Core::log("ODCP_BaseApplication: DCP Runner was not created. "
			"Missing call to ODCP_BaseApplication::setupApplication()?");
	}

	mSimulationLoop->start();

	return true;
}


bool ODCP_BaseApplication::buildSimulationModel() {
	return true;
}

}


