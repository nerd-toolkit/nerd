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


#include "ASeriesDirectPositionControlApplication.h"
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
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Control/NetworkAgentControlParser.h"
#include "Collections/StandardNeuralNetworkFunctions.h"
#include "SimulationConstants.h"
#include "PlugIns/FramesPerSecondCounter.h"
#include "Collections/TerminateTryCollisionRule.h"


using namespace std;

using namespace nerd;


ASeriesDirectPositionControlApplication::ASeriesDirectPositionControlApplication ()
		: ODCP_StandardGuiApplication("Humanoid", true, true)
{
}

ASeriesDirectPositionControlApplication::~ASeriesDirectPositionControlApplication()
{
}

QString ASeriesDirectPositionControlApplication::getName() const
{
	return "ASeriesDirectPositionControlApplication";
}

bool ASeriesDirectPositionControlApplication::setupApplication()
{
	ODCP_StandardGuiApplication::setupApplication();
	//install ODE PhysicsLayer
	ODE_Physics();
	//install file parser
	new SimpleObjectFileParser();
	//install neural network control parser.
	new NetworkAgentControlParser();
	StandardNeuralNetworkFunctions();

	new FramesPerSecondCounter();

	return true;
}

bool ASeriesDirectPositionControlApplication::buildSimulationModel()
{
	SimObject* ground = Physics::getPhysicsManager()->getPrototype("Prototypes/Ground")
						->createCopy();
	ground->getParameter("Name")->setValueFromString("Ground");
	if(dynamic_cast<ModelInterface*>(ground) != 0) {
		dynamic_cast<ModelInterface*>(ground)->setup();
	}
	Physics::getPhysicsManager()->addSimObject(ground);

	SimObject *humanoidModel = Physics::getPhysicsManager()->getPrototype(
							SimulationConstants::PROTOTYPE_A_SERIES)->createCopy();

	humanoidModel->setName("Humanoid");
	if(dynamic_cast<ModelInterface*>(humanoidModel) != 0) {
		dynamic_cast<ModelInterface*>(humanoidModel)->setup();
	}
	Physics::getPhysicsManager()->addSimObject(humanoidModel);


	Physics::getSimulationEnvironmentManager()->createSnapshot();

	return true;
}




