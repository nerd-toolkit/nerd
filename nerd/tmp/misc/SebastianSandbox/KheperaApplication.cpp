/***************************************************************************
 *   ORCS - Open Robot Control and Simulation Library                      *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Copyright (C) 2008 by Christian Rempis, Verena Thomas                 *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   verena.thomas@uni-osnabrueck.de                                       *
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
 ***************************************************************************/

#include "Collections/ODE_Physics.h"
#include "Core/Core.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "KheperaApplication.h"
#include "Models/ModelInterface.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Physics/SimObjectGroup.h"
#include "Physics/SimulationEnvironmentManager.h"
#include "PlugIns/KeyFramePlayer.h"
#include "PlugIns/SimpleObjectFileParser.h"
#include "Value/InterfaceValue.h"
#include <QString>
#include <iostream>

using namespace std;

namespace nerd {

KheperaApplication::KheperaApplication()
		: ODCP_StandardGuiApplication("Khepera", true, true) {
}

KheperaApplication::~KheperaApplication() {
}

QString KheperaApplication::getName() const {
	return "KheperaApplication";
}

bool KheperaApplication::setupApplication() {
	ODCP_StandardGuiApplication::setupApplication();
	ODE_Physics();
	new SimpleObjectFileParser();
	return true;
}

bool KheperaApplication::buildSimulationModel() {
	Value *orientation = Core::getInstance()->getValueManager()
				->getValue("/OpenGLVisualization/Main/StartOrientation");
	if(orientation != 0) {
		orientation->setValueFromString("(-45, -270, 0)");
	}
	Value *position = Core::getInstance()->getValueManager()
				->getValue("/OpenGLVisualization/Main/StartPosition");
	if(position != 0) {
		position->setValueFromString("(1.6, 1.6, 0.5)");
	}
	Physics::getSimulationEnvironmentManager()->createSnapshot();
	return true;
}

}

