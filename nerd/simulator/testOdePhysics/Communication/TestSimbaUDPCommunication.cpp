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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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


#include "TestSimbaUDPCommunication.h"
#include "Communication/SimbaUDPCommunication.h"
#include <QCoreApplication>
#include <QApplication>
#include <iostream>
#include "Core/Core.h"
#include "Communication/TestActuator.h"
#include "Physics/PhysicsManager.h"
#include "Value/InterfaceValue.h"
#include "Collision/CollisionManager.h"
#include "Value/BoolValue.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"
#include "Physics/AngleSensor.h"
#include "Physics/SimulationEnvironmentManager.h"
#include "Core/ShutDownGuard.h"
#include "Collision/ODE_CollisionHandler.h"
#include "Physics/ODE_SimulationAlgorithm.h"
#include "Physics/ODE_HingeJoint.h"
#include "Physics/ODE_BoxBody.h"
#include "Physics/ODE_TorqueDrivenDynamixel.h"
#include "Physics/ODE_FixedJoint.h"
#include "Physics/AccelSensor.h"
#include "Math/Math.h"
#include <math.h>
#include "Physics/ODE_ServoMotor.h"
#include "Physics/ODE_SphereBody.h"
#include "Physics/Physics.h"
#include "Models/HumanoidModel.h"
#include "Gui/GuiMainWindow.h"

#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))
		

using namespace std;

using namespace nerd;


#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))


void TestSimbaUDPCommunication::testReadWriteInt() {
	Core::resetCore();

	SimbaUDPCommunication *com = new SimbaUDPCommunication();
	
	com->writeInt(12345);
	
	QVERIFY(com->maxAvailableIntegers() == 1);
	QVERIFY(com->readNextInt() == 12345);
	
	QVERIFY(com->maxAvailableIntegers() == 0);
	QVERIFY(com->readNextInt() == -1);
	
	com->writeInt(6565656);
	com->writeInt(16777216);
	
	QVERIFY(com->maxAvailableIntegers() == 2);
	QVERIFY(com->readNextInt() == 6565656);
	QVERIFY(com->maxAvailableIntegers() == 1);
	QVERIFY(com->readNextInt() == 16777216);
	QVERIFY(com->maxAvailableIntegers() == 0);

	QVERIFY(com->readNextInt() == -1);
	QVERIFY(com->maxAvailableIntegers() == 0);
	
	com->writeInt(132412);
	com->writeInt(74567);
	
	QVERIFY(com->maxAvailableIntegers() == 2);
	com->resetDatagram();
	QVERIFY(com->maxAvailableIntegers() == 0);
	
	
}

void TestSimbaUDPCommunication::testReadWriteFloat() {
	Core::resetCore();
	
	SimbaUDPCommunication *com = new SimbaUDPCommunication();
	
	com->writeFloat(123.9876);
	com->writeInt(1113568935);
	
	int i = com->readNextInt();
	float f = com->readNextFloat();

	QVERIFY(i == 1123547559);
	QCOMPARE(((int) (f * 10000)), 559283); //compare up to 4 digits after comma.
	

}

void TestUPDRunner::run() {
	std::cerr << "TestUPDRunner Thread started." << std::endl;

	mSimCom->startUDPServer();
	
	std::cerr << "TestUPDRunner Thread done." << std::endl;
}

void TestSimbaUDPCommunication::run() {

	mSimCom = new SimbaUDPCommunication();
//   mSimCom->getParameter("Port")->setValueFromString("45455");
	
	//Core *core = Core::getInstance();
	
	//Set CollisionHandler
	Physics::getCollisionManager()->setCollisionHandler(
							new ODE_CollisionHandler());
	
	//Set SimulationAlgorithm
	Physics::getPhysicsManager()->setPhysicalSimulationAlgorithm(
							new ODE_SimulationAlgorithm());
	
	HumanoidModel m("Humanoid");
	m.getParameter("Position")->setValueFromString("(0,0.1,0)");
	m.setup();
	
	Core::getInstance()->init();
	
	Physics::getSimulationEnvironmentManager()->createSnapshot();
	//core->getPhysicsManager()->resetSimulation();
	
	mSimCom->startListening(0);
	 
	QThread::exec();
	
}

void TestSimbaUDPCommunication::testConnection(int args, char** argv) { 
	Core::resetCore();
	
	QApplication app(args, argv); 
	
	GuiMainWindow *gui = new GuiMainWindow(true);
	
	mSimCom = 0;
	start();

	cerr << "Running App." << endl;
	
	while(mSimCom == 0) {
		;
	}
	//mSimCom->startUDPServer();
	
	cerr << "Showing Frame." << endl;
	
	gui->show();
	TestUPDRunner udpRunner;
	udpRunner.mSimCom = mSimCom;
	udpRunner.start();
	
	ShutDownGuard shutDownGuard(app);
	

	app.exec();
	
	cout << "Ende" << endl;
	
}


