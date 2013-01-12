/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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
 *   The NERD Toolkit is part of the EU project ALEAR                      *
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
 *   clearly by citing the NERD homepage and the NERD overview paper.      *  
 ***************************************************************************/

#include "ModelPrototypes.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Models/PlaneGround.h"
#include "Models/HumanoidModel.h"
#include "Models/Volksbot.h"
#include "SimulationConstants.h"
#include "Models/ControlParameterModel.h"
#include "Physics/ListSwitchControlParameter.h"
#include "Models/Khepera.h"
#include "Models/ShakerBoard.h"
#include "Models/ASeriesUpperBodyPart.h"
#include "Models/Alice.h"
#include "Models/ASeriesModel.h"
#include "Models/MSeriesV1Model.h"
#include "Models/MSeriesV2Model.h"
#include "Models/MSeriesMotorTestModel.h"
#include "Models/MSeriesMotorChainTest.h"
#include "Models/MSeriesMotorMultipleTest.h"
#include "Models/ASeriesMorph.h"
#include "Models/SimpleMultiLeggedRobot.h"
#include "Models/DummyModel.h"
#include "Models/InvertedPendulum.h"
#include "Models/CantileverArm.h"

#include "Models/MSeriesV2Model_Obsolete.h"
#include "Models/ConfigurablePendulum.h"

namespace nerd{

ModelPrototypes::ModelPrototypes() {
	PhysicsManager *pm = Physics::getPhysicsManager();
	
	pm->addPrototype("Prototypes/Ground", 
				new PlaneGround("Prototypes/Ground"));
	
	pm->addPrototype(SimulationConstants::PROTOTYPE_A_SERIES, 
				new HumanoidModel(SimulationConstants::PROTOTYPE_A_SERIES));
	pm->addPrototype("ASeriesM", 
				new ASeriesMorph("ASeriesMorph"));

  // new prototype from Aseries with new motor model
	pm->addPrototype(SimulationConstants::PROTOTYPE_A_SERIES + "WithMotorModel", 
				new ASeriesModel(SimulationConstants::PROTOTYPE_A_SERIES + "WithMotorModel"));
  
	pm->addPrototype("Prototypes/Volksbot", 
				new Volksbot("Prototypes/Volksbot"));

	pm->addPrototype("Prototypes/Khepera", 
				new Khepera(SimulationConstants::PROTOTYPE_KHEPERA, true, true));
// 	pm->addPrototype("Prototypes/KheperaDist", 
// 				new Khepera(SimulationConstants::PROTOTYPE_KHEPERA, true, false));
// 	pm->addPrototype("Prototypes/KheperaLight", 
// 				new Khepera(SimulationConstants::PROTOTYPE_KHEPERA, false, true));
// 	pm->addPrototype("Prototypes/KheperaRestrictedSensors", 
// 				new Khepera(SimulationConstants::PROTOTYPE_KHEPERA, false, false));

	ListSwitchControlParameter *lscp = new ListSwitchControlParameter("ControlParameter");
	pm->addPrototype("Prototypes/ListSwitchControlParameter", 
				new ControlParameterModel("ListSwitchControlParameter", lscp));

	ControlParameter *scp = new ControlParameter("ControlParameter");
	pm->addPrototype("Prototypes/ControlParameter", 
				new ControlParameterModel("ControlParameter", scp));

	pm->addPrototype(SimulationConstants::PROTOTYPE_SHAKER_BOARD,
				new ShakerBoard(SimulationConstants::PROTOTYPE_SHAKER_BOARD));

	pm->addPrototype("Prototypes/ASeriesUpperBody", 
				new ASeriesUpperBodyPart("Prototypes/ASeriesUpperBody"));

	pm->addPrototype("Prototypes/Alice", 
				new Alice("Alice"));

	pm->addPrototype(SimulationConstants::PROTOTYPE_M_SERIES_V1, 
				new MSeriesV1Model("MSeriesV1"));

	pm->addPrototype("Prototypes/MSeriesV2", 
				new MSeriesV2Model("MSeriesV2"));

	//TODO this prototype will be the default setting in the future (will be renamed to MSeriesV2 ?)
	pm->addPrototype("Prototypes/MSeriesV2c", 
				new MSeriesV2Model("MSeriesV2c", "", true));
				
	pm->addPrototype("Prototypes/MSeriesV2d", 
				new MSeriesV2Model("MSeriesV2d", "", true, true));

	//TODO remove later
	pm->addPrototype("Prototypes/MSeriesV2Obsolete",
				new MSeriesV2Model_Obsolete("MSeriesV2Obsolete"));

// 	pm->addPrototype("Prototypes/MSeriesV2RightLeg", 
// 				new MSeriesV2Model("MSeriesV2RightLeg", false, false, false, false, false, false, true));
// 
// 	pm->addPrototype("Prototypes/MSeriesV2BodyAndRightArm", 
// 				new MSeriesV2Model("MSeriesV2BodyAndRightArm", false, true, false, true, false, false, false));
// 
// 	pm->addPrototype("Prototypes/MSeriesV2Legs", 
// 				new MSeriesV2Model("MSeriesV2Legs", false, false, false, false, true, true, true));

	pm->addPrototype("Prototypes/MSeriesMotorTestModel", 
				new MSeriesMotorTestModel("MSeriesMotorTestModel"));

	pm->addPrototype("Prototypes/MSeriesMotorChainTest", 
				new MSeriesMotorChainTest("MSeriesMotorChainTest"));

	pm->addPrototype("Prototypes/MSeriesMotorMultipleTestModel", 
				new MSeriesMotorMultipleTest("MSeriesMotorMultipleTestModel"));

	pm->addPrototype("Prototypes/SimpleWalker",
				new SimpleMultiLeggedRobot("SimpleWalker"));

	pm->addPrototype("Prototypes/Dummy",
				new DummyModel("Dummy"));

	pm->addPrototype("Prototypes/InvertedPendulum", 
				new InvertedPendulum("InvertedPendulum"));

	pm->addPrototype("Prototypes/CantileverArm",
				new CantileverArm("CantileverArm"));

	pm->addPrototype("Prototypes/ConfigurablePendulum",
				new ConfigurablePendulum("ConfigurablePendulum"));

}

}
