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



#include "OSP_BaseApplication.h"
#include "Core/Core.h"
#include "PlugIns/CommandLineParser.h"
#include <QString>
#include <iostream>
#include "Value/ValueManager.h"
#include "Physics/Physics.h"
#include "PlugIns/CommandLineArgument.h"
#include <QStringList>
#include "PlugIns/SimObjectGroupPrinter.h"

using namespace std;

namespace nerd {

OSP_BaseApplication::OSP_BaseApplication()
		: BaseApplication(), mSeedCommunication(0), 
		  mServerPort(45454)
{
	CommandLineArgument *portArgument =
			new CommandLineArgument("port", "p", "<port>", 
				"Sets the port of the OSP server. ", 1, 0, true);

	QStringList params = portArgument->getParameters();
	if(params.size() > 0) {
		bool ok = false;
		int port = params.at(0).toInt(&ok);
		if(ok) {
				mServerPort = port;
			}
			else {
				Core::log("OSP_BaseApplication: Could not parse the port!");
			}
	}
}

OSP_BaseApplication::~OSP_BaseApplication() {
}

bool OSP_BaseApplication::init() {
	return true;
}


bool OSP_BaseApplication::bind() {
	return BaseApplication::bind();
}


bool OSP_BaseApplication::cleanUp() {
	return BaseApplication::cleanUp();
}


QString OSP_BaseApplication::getName() const {
	return "OSP_BaseApplication";
}


bool OSP_BaseApplication::setUp() {
// 	mSeedCommunication = new SeedUDPCommunication();
	BaseApplication::setUp();
	buildSimulationModel();
	return true;
}

bool OSP_BaseApplication::runApplication() {
	Physics::getSimulationEnvironmentManager()->createSnapshot();

// 	mSeedCommunication->setPort(mServerPort);
// 	mSeedCommunication->startListening(this);
// 	mSeedCommunication->start();

	return true;
}


bool OSP_BaseApplication::setupApplication() {
	new SimObjectGroupPrinter();
	return true;
}


bool OSP_BaseApplication::buildSimulationModel() {
	return true;
}

}


