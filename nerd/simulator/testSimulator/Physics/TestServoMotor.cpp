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


#include "Core/Core.h"
#include "Value/QuaternionValue.h"
#include "Value/Vector3DValue.h"
#include "SimulationConstants.h"	
#include "Physics/Physics.h"
#include "Physics/ServoMotor.h"
#include "Math/Math.h"
#include <cmath>
#include "TestServoMotor.h"

using namespace std;

namespace nerd{

// josef
void TestServoMotor::testConstruction() {
	Core::resetCore();

	// construct default motor, no temperature
	ServoMotor *motor_1 = new ServoMotor("Motor_1", false);
	QVERIFY(motor_1 != 0);
	
	QVERIFY(dynamic_cast<DoubleValue*>(motor_1->
		getParameter("PID_P"))->get() == 1.0);
	
	InterfaceValue *dS = dynamic_cast<InterfaceValue*>(motor_1->
		getParameter("DesiredSetting"));
	
	dS->set(1.0);
	
	Physics::getPhysicsManager()->addSimObject(motor_1);
	motor_1->setup();
	
	motor_1->clear();
	Physics::getPhysicsManager()->removeSimObject(motor_1);

	delete motor_1;
}


// josef
void TestServoMotor::testCopy() {
	Core::resetCore();

	
}


// josef TODO
void TestServoMotor::testMethods() {
	Core::resetCore();
	PhysicsManager *pManager = Physics::getPhysicsManager();

}

}

