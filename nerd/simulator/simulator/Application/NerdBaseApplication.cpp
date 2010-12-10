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



#include "NerdBaseApplication.h"
#include "Core/Core.h"
#include "PlugIns/CommandLineParser.h"
#include <QString>
#include <iostream>
#include "Value/ValueManager.h"
#include "Physics/Physics.h"
#include "PlugIns/CommandLineArgument.h"

using namespace std;

namespace nerd {

NerdBaseApplication::NerdBaseApplication(int argc, char *argv[], bool enableGui)
		: BaseApplication(), mSeedCommunication(0), 
		  mEnableGui(enableGui), mServerPort(45454)
{
	for(int i = 0; i < argc; ++i) {
		QString flag(argv[i]);
		flag = flag.trimmed();

		if((flag.compare("-port") == 0
				|| flag.compare("-p") == 0)
			&& (i < (argc - 1)))
		{
			QString portValue(argv[i + 1]);
			++i;
			bool ok = false;
			int port = portValue.toInt(&ok);

			if(ok) {
				mServerPort = port;
			}
			else {
				qDebug("Could not parse the port!");
			}
		}
	}

	CommandLineArgument *guiArgument = 
			new CommandLineArgument(
				"enableGui", "gui", "",
				"Starts the application with graphical user interface.",
				0, 0,
				true);
	CommandLineArgument *noGuiArgument = 
			new CommandLineArgument(
				"disableGui", "nogui", "",
				"Starts the application without graphical user interface.",
				0, 0,
				true);

	if(noGuiArgument->getParameterValue()->get() != "") {
		mEnableGui = false;
	}
	if(guiArgument->getParameterValue()->get() != "") {
		mEnableGui = true;
	}
}

NerdBaseApplication::~NerdBaseApplication() {
}

bool NerdBaseApplication::init() {
	return true;
}


bool NerdBaseApplication::bind() {
	return BaseApplication::bind();
}


bool NerdBaseApplication::cleanUp() {
	return BaseApplication::cleanUp();
}


QString NerdBaseApplication::getName() const {
	return "NerdBaseApplication";
}


bool NerdBaseApplication::setUp() {
	mSeedCommunication = new SeedUDPCommunication();
	setupApplication();
	buildSimulationModel();
	return true;
}


bool NerdBaseApplication::setupApplication() {
	return true;
}

bool NerdBaseApplication::runApplication() {
	Physics::getSimulationEnvironmentManager()->createSnapshot();

	mSeedCommunication->setPort(mServerPort);
	mSeedCommunication->startListening(this);
	mSeedCommunication->start();

	return true;
}


bool NerdBaseApplication::buildSimulationModel() {
	return true;
}

}




