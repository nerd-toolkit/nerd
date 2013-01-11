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


#include "NeuroAndSimEvaluationBaseApplication.h"
#include "Core/Core.h"
#include "PlugIns/CommandLineParser.h"
#include <QString>
#include <iostream>
#include "Value/ValueManager.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "PlugIns/KeyFramePlayer.h"
#include "PlugIns/PlugInManager.h"
#include "PlugIns/ControllerFitnessFunctionParser.h"
#include "PlugIns/SimObjectGroupPrinter.h"

using namespace std;

namespace nerd {

NeuroAndSimEvaluationBaseApplication::NeuroAndSimEvaluationBaseApplication() 
	: EvaluationBaseApplication()
{
	Physics::install();
	new ControllerFitnessFunctionParser(dynamic_cast<ControllerProvider*>(Physics::getPhysicsManager()));
	CommandLineArgument *mPlayArgument = 
		new CommandLineArgument("playKeyFrames", "play", "<agent> <file>",
			"Loads keyframe file <file> and starts to playback the keyframes on agent <agent>",
			2, 0, true);

	QStringList files = mPlayArgument->getEntries();

	for(int i = 0; i < files.size(); ++i) {
		QStringList args = files.at(i).split(" ");
		if(args.size() == 2) {
			QString keyFrameTarget(args.at(0));
			QString keyFrameFileName(args.at(1));

			KeyFramePlayer *player = new KeyFramePlayer(keyFrameTarget);
			player->setKeyFrameFile(keyFrameFileName);
		}
		else {
			cerr << "Warning: Size was " << args.size() << endl;
		}
	}
}

NeuroAndSimEvaluationBaseApplication::~NeuroAndSimEvaluationBaseApplication() {
}

bool NeuroAndSimEvaluationBaseApplication::init() {
	return BaseApplication::init();
}


bool NeuroAndSimEvaluationBaseApplication::bind() {
	return BaseApplication::bind();
}


bool NeuroAndSimEvaluationBaseApplication::cleanUp() {
	return BaseApplication::cleanUp();
}


QString NeuroAndSimEvaluationBaseApplication::getName() const {
	return "NeuroAndSimEvaluationBaseApplication";
}


bool NeuroAndSimEvaluationBaseApplication::setUp() {
	bool ok = EvaluationBaseApplication::setUp();

	if(!buildSimulationModel()) {
		Core::log("NeuroAndSimEvaluationBaseApplication: Problem while building the simulation model.");
		ok = false;
	}
	return ok;
}

bool NeuroAndSimEvaluationBaseApplication::setupApplication() {
	new SimObjectGroupPrinter();
	return true;
}	

bool NeuroAndSimEvaluationBaseApplication::runApplication() {

	Physics::getSimulationEnvironmentManager()->createSnapshot();

	EvaluationBaseApplication::runApplication();
	return true;
}


bool NeuroAndSimEvaluationBaseApplication::buildSimulationModel() {
	return true;
}

}


