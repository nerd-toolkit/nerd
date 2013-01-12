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

#include "TestPhysics.h"
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "Physics/SimBody.h"
#include "Physics/SimObject.h"
#include "Physics/HingeJoint.h"
#include "Math/Math.h"

namespace nerd {

void TestPhysics::testPhysics() {
	Core::resetCore();
	QVERIFY(Physics::getSimulationEnvironmentManager() != 0);
	QVERIFY(Physics::getPhysicsManager() != 0);
	QVERIFY(Physics::getCollisionManager() != 0);

	//Test translate methods.
	QList<SimObject*> objects;
	SimBody body1("Body1");
	SimBody body2("Body2");
	body1.getPositionValue()->set(1.0, 1.0, 1.0);
	body2.getPositionValue()->set(-2.0, 3.0, 4.5);

	HingeJoint joint1("Joint1");
	joint1.getParameter("AxisPoint1")->setValueFromString("(0.1,0.0,2.0)");
	joint1.getParameter("AxisPoint2")->setValueFromString("(1.1,0.0,2.0)");
	objects.push_back(&body1);
	objects.push_back(&body2);
	objects.push_back(&joint1);
	QCOMPARE(body1.getPositionValue()->getX(), 1.0);
	QCOMPARE(body1.getPositionValue()->getY(), 1.0);
	QCOMPARE(body1.getPositionValue()->getZ(), 1.0);
	Vector3DValue *axis1 = dynamic_cast<Vector3DValue*>(joint1.getParameter("AxisPoint1"));
	Vector3DValue *axis2 = dynamic_cast<Vector3DValue*>(joint1.getParameter("AxisPoint2"));
	QCOMPARE(axis1->getX(), 0.1);
	QCOMPARE(axis1->getY(), 0.0);
	QCOMPARE(axis1->getZ(), 2.0);

	Physics::translateSimObjects(objects, Vector3D(1.0, 2.0, -1.0));
	
	QCOMPARE(body1.getPositionValue()->getX(), 2.0);
	QCOMPARE(body1.getPositionValue()->getY(), 3.0);
	QCOMPARE(body1.getPositionValue()->getZ(), 0.0);
	QCOMPARE(axis1->getX(), 1.1);
	QCOMPARE(axis1->getY(), 2.0);
	QCOMPARE(axis1->getZ(), 1.0);
	
	body1.getPositionValue()->set(0.0, 0.0, 0.0);
	axis1->set(0.0, 0.0, 0.0);
	axis2->set(1.0, 0.0, 0.0);

	Physics::rotateSimObjects(objects, Vector3D(0.0, 90.0, 0.0));
	
	QCOMPARE(body1.getOrientationValue()->getX(), 0.0);
	QCOMPARE(body1.getOrientationValue()->getY(), 90.0);
	QCOMPARE(body1.getOrientationValue()->getZ(), 0.0);

	QCOMPARE(axis1->getX(), 0.0);
	QCOMPARE(axis1->getY(), 0.0);
	QCOMPARE(axis1->getZ(), 0.0);
	QVERIFY(Math::compareDoubles(axis2->getX(), 0.0, 4));
	QVERIFY(Math::compareDoubles(axis2->getY(), 0.0, 4));
	QVERIFY(Math::compareDoubles(axis2->getZ(), -1.0, 4));
}	
}
