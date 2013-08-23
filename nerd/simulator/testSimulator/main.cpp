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



#include "Util/UnitTestMacros.h"

#include "Physics/TestPhysicsManager.h"
#include "Physics/TestGeom.h"
#include "Physics/TestSimObject.h"
#include "Physics/TestSimBody.h"
#include "Collision/TestCollisionRule.h"
#include "Collision/TestCollisionManager.h"
#include "Physics/TestSimJoint.h"
#include "Collision/TestCollisionObject.h"
#include "Collision/TestMaterialProperties.h"
#include "Physics/TestBoxBody.h"
#include "Physics/TestSphereBody.h"
#include "Physics/TestAccelSensor.h"
#include "Randomization/TestRandomizer.h"
#include "Gui/GuiMainWindow.h"
#include "Physics/TestSimulationEnvironmentManager.h"
#include "Physics/TestCylinderBody.h"
#include "Physics/TestPhysics.h"
#include "Physics/TestSphereBody.h"
#include "Physics/TestSimObjectGroup.h"
#include "Signal/TestSignals.h"
#include "TestSimulationConstants.h"
#include "Physics/TestValueTransferController.h"
#include "Physics/TestSimpleLightSource.h"
#include "Physics/TestSphericLightSource.h"
#include "Physics/TestLightSensor.h"

TEST_START("TestSimulator", 1, -1, 24);

	TEST(TestGeom); //tests all geoms.
	TEST(TestCollisionObject);
	TEST(TestCollisionRule);
	TEST(TestSimObject);
	TEST(TestSimBody); 
	TEST(TestSimJoint);
	TEST(TestBoxBody);
	TEST(TestPhysicsManager); //still missing many tests. (see header)
	TEST(TestCollisionManager); //in progress. //missing updateCollisionHandler.

	//up to here test cases are checked for memory leaks.

	TEST(TestSphereBody);
// TODO: update test: axes were switched!
	TEST(TestAccelSensor);
	TEST(TestRandomizer);
	TEST(TestSimulationEnvironmentManager);
	TEST(TestCylinderBody);
	TEST(TestPhysics);
	TEST(TestMaterialProperties);
	TEST(TestSimObjectGroup);
	TEST(TestSignals);
	TEST(TestSimulationConstants);
	TEST(TestValueTransferController);
	TEST(TestSimpleLightSource);
	TEST(TestSphericLightSource);
	TEST(TestLightSensor);

TEST_END;


