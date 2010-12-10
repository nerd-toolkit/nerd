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


#include "SimpleTestNerdBaseApplication.h"
#include "Core/Core.h"
#include "Physics/SimObjectAdapter.h"
#include "Value/InterfaceValue.h"
#include "Physics/SimObjectGroup.h"
#include "Physics/PhysicsManager.h"
#include "Collections/ODE_Physics.h"
#include <iostream>
#include "Physics/SimulationEnvironmentManager.h"
#include "Physics/Physics.h"


using namespace std;

namespace nerd {

SimpleTestNerdBaseApplication::SimpleTestNerdBaseApplication(int argc, char *argv[])
			: NerdBaseApplication(argc, argv, true), mHumanoidModel(0)
{
	Physics::install();
	if(mEnableGui) {
		mGuiMainWindow = new GuiMainWindow(true);
		mGuiMainWindow->show();
	}
}

SimpleTestNerdBaseApplication::~SimpleTestNerdBaseApplication() {
	delete mGuiMainWindow;
	delete mHumanoidModel;
}

bool SimpleTestNerdBaseApplication::setupSimulator() {

	//install ODE PhysicsLayer
	new ODE_Physics();

	return true;
}

bool SimpleTestNerdBaseApplication::buildSimulationModel() {
	//return buildSimpleModel();
	return buildHumanoidModel();
}

bool SimpleTestNerdBaseApplication::buildSimpleModel() {

	SimObjectAdapter *obj1 = new SimObjectAdapter("SimObject1", "/Sims/");

	InterfaceValue *v1 = new InterfaceValue("", "InValue1", 0.0, -1.0, 1.0);
	InterfaceValue *v2 = new InterfaceValue("", "OutValue1", 0.5, 0.0, 1.0);

	QList<InterfaceValue*> agentInterface;

	agentInterface.append(v1);
	obj1->setInputValues(agentInterface);

	agentInterface.clear();
	agentInterface.append(v2);
	obj1->setOutputValues(agentInterface);

	SimObjectGroup *group1 = new SimObjectGroup("Group1", "Agent");
	group1->addObject(obj1);

	Physics::getPhysicsManager()->addSimObject(obj1);
	Physics::getPhysicsManager()->addSimObjectGroup(group1);

	SimObjectAdapter *obj2 = new SimObjectAdapter("/SimObjects/MoreSimObjects/FurthermoreSimObjects/LongString/SimObject2",
								"/Sims/");

	InterfaceValue *v3 = new InterfaceValue("", "InOutValue3", 0.1, -1.0, 1.0);

	agentInterface.clear();
	agentInterface.append(v3);
	obj2->setInputValues(agentInterface);
	obj2->setOutputValues(agentInterface);

	InterfaceValue *v4 = new InterfaceValue("", "Info1", 0.987654321, 0.0, 1.0);
	agentInterface.clear();
	agentInterface.append(v4);
	obj2->setInfoValues(agentInterface);

	SimObjectGroup *group2 = new SimObjectGroup("/Groups/AllTheGroups/SpecificGroups/Group2", "Agent");
	group2->addObject(obj2);

	Physics::getPhysicsManager()->addSimObject(obj2);
	Physics::getPhysicsManager()->addSimObjectGroup(group2);

	return true;
}


bool SimpleTestNerdBaseApplication::buildHumanoidModel() {

	cerr << "Humanoid build model" << endl;
	mHumanoidModel = new HumanoidModel("Humanoid");
	mHumanoidModel->setup();

	Value *orientation = Core::getInstance()->getValueManager()
				->getValue("/Visualization/Orientation");
	if(orientation != 0) {
		orientation->setValueFromString("(9.4,-0.9,0)");
	}
	Value *position = Core::getInstance()->getValueManager()
				->getValue("/Visualization/Position");
	if(position != 0) {
		position->setValueFromString("(0.0056590958,0.055699985,1.3552705)");
	}

	cerr << "Humanoid build model done" << endl;

	Physics::getSimulationEnvironmentManager()->createSnapshot();

	cerr << "SnapShot" << endl;

	return true;
}




}





