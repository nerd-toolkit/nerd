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

#include "Test_ODESimulationAlgorithm.h"
#include "Core/Core.h"
#include "Physics/ODE_SimulationAlgorithm.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include "Collision/ODE_CollisionHandler.h"
#include "Physics/ODE_BodyAdapter.h"
#include <ode/ode.h>

namespace nerd {

void Test_ODESimulationAlgorithm::testResetPhysics() {
	Core::resetCore();
	PhysicsManager *pManager = Physics::getPhysicsManager();
	Physics::getCollisionManager()->setCollisionHandler(
	new ODE_CollisionHandler());
	ODE_SimulationAlgorithm *sAlgo = new ODE_SimulationAlgorithm();
	pManager->setPhysicalSimulationAlgorithm(sAlgo);
	
	QVERIFY(sAlgo->getODEWorldID() == 0);
	QVERIFY(sAlgo->getContactJointGroupID() == 0);
	QVERIFY(sAlgo->getJointGroupID() == 0);
	QVERIFY(sAlgo->getODEWorldSpaceID() == 0);

	QVERIFY(sAlgo->resetPhysics());

	QVERIFY(sAlgo->getODEWorldID() != 0);
	QVERIFY(sAlgo->getContactJointGroupID() != 0);
	QVERIFY(sAlgo->getJointGroupID() != 0);
	QVERIFY(sAlgo->getODEWorldSpaceID() != 0);
	
	QCOMPARE(dynamic_cast<DoubleValue*>(sAlgo->getParameter("ODE/ERP"))->get(), (double) dWorldGetERP(sAlgo->getODEWorldID()));
	QCOMPARE(dynamic_cast<DoubleValue*>(sAlgo->getParameter("ODE/ContactSurfaceLayer"))->get(), (double) dWorldGetContactSurfaceLayer(sAlgo->getODEWorldID()));
	QVERIFY(dynamic_cast<DoubleValue*>(sAlgo->getParameter("ODE/ContactMaxCorrectingVelocity"))->get() == (double) dWorldGetContactMaxCorrectingVel(sAlgo->getODEWorldID()));
	QCOMPARE(dynamic_cast<DoubleValue*>(sAlgo->getParameter("ODE/ConstraintForceMixing"))->get(), (double) dWorldGetCFM(sAlgo->getODEWorldID()));	

	sAlgo->getParameter("ODE/ERP")->setValueFromString("0.123");
	sAlgo->getParameter("ODE/ContactSurfaceLayer")->setValueFromString("0.022");
	sAlgo->getParameter("ODE/ContactMaxCorrectingVelocity")->setValueFromString("11111");
	sAlgo->getParameter("ODE/ConstraintForceMixing")->setValueFromString("0.00022");
	
	QCOMPARE(0.2, (double) dWorldGetERP(sAlgo->getODEWorldID()));
	QCOMPARE(0.001, (double) dWorldGetContactSurfaceLayer(sAlgo->getODEWorldID()));
	//QCOMPARE((double) dInfinity, (double) dWorldGetContactMaxCorrectingVel(sAlgo->getODEWorldID()));
	QCOMPARE(0.00001, (double) dWorldGetCFM(sAlgo->getODEWorldID()));	

	sAlgo->resetPhysics();

	QCOMPARE(0.123, (double) dWorldGetERP(sAlgo->getODEWorldID()));
	QCOMPARE(0.022, (double) dWorldGetContactSurfaceLayer(sAlgo->getODEWorldID()));
	QCOMPARE(11111.0, (double) dWorldGetContactMaxCorrectingVel(sAlgo->getODEWorldID()));
	QCOMPARE(0.00022, (double) dWorldGetCFM(sAlgo->getODEWorldID()));	

	//test finalize
	
	ODE_BodyAdapter *body1 = new ODE_BodyAdapter("Body1");
	pManager->addSimObject(body1);
	QCOMPARE(body1->mCreateCounter, 0);
	
	QVERIFY(sAlgo->resetPhysics());
	QVERIFY(sAlgo->finalizeSetup());
	QCOMPARE(body1->mCreateCounter, 1);
}


}
