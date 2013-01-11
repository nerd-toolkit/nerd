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
 *   The ORCS library is part of the EU project ALEAR                      *
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



#include "SimpleTestOrcsBaseApplication.h"
#include "Core/Core.h"
#include "Value/InterfaceValue.h"
#include "Physics/SimObjectGroup.h"
#include "Physics/PhysicsManager.h"
#include "Collections/ODE_Physics.h"
#include <iostream>
#include "Physics/SimulationEnvironmentManager.h"
#include "Physics/TorqueDrivenDynamixel.h"
#include "PlugIns/SimpleObjectFileParser.h"
#include "Physics/Physics.h"
#include "Collections/TerminateTryCollisionRule.h"

using namespace std;

namespace nerd {

SimpleTestOrcsBaseApplication::SimpleTestOrcsBaseApplication()
			: OSP_StandardGuiApplication(false, false), mGuiMainWindow(0)
{
}

SimpleTestOrcsBaseApplication::~SimpleTestOrcsBaseApplication() {
}

bool SimpleTestOrcsBaseApplication::setupApplication() {

	OSP_StandardGuiApplication::setupApplication();
	//install ODE PhysicsLayer
	new ODE_Physics();
	new SimpleObjectFileParser();

	return true;
}

bool SimpleTestOrcsBaseApplication::buildSimulationModel() {


// 	Value *orientation = Core::getInstance()->getValueManager()
// 				->getValue("/OpenGLVisualization/ORCS/StartOrientation");
// 	if(orientation != 0) {
// 		orientation->setValueFromString("(-11.3,0.3,0)");
// 	}
// 	Value *position = Core::getInstance()->getValueManager()
// 				->getValue("/OpenGLVisualization/ORCS/StartPosition");
// 	if(position != 0) {
// 		position->setValueFromString("(-0.0088012985,0.4385862,1.1327441)");
// 	}

	Physics::getSimulationEnvironmentManager()->createSnapshot();
	

	return true;
}


bool SimpleTestOrcsBaseApplication::runApplication() {

	bool ok = OSP_StandardGuiApplication::runApplication();

	if(ok) {
		Physics::getPhysicsManager()->resetSimulation();
	}
	return ok;
}



}





