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


#include "TestSphereBody.h"
#include "Core/Core.h"
#include "Physics/SphereBody.h"
#include "Physics/SphereGeom.h"
#include "Collision/CollisionObject.h"

namespace nerd{

void TestSphereBody::testSphereBody() {
	Core::resetCore();
	SphereBody *sphere = new SphereBody("Sphere");
	QCOMPARE(sphere->getParameters().size(), 14);
	QVERIFY(sphere->getName().compare("Sphere") == 0);
	QVERIFY(sphere->getInputValues().size() == 0);
	QVERIFY(sphere->getOutputValues().size() == 0);
	QCOMPARE(sphere->getParameters().size(), 14);
	QVERIFY(sphere->getParameter("CenterOfMass") != 0);
	QVERIFY(sphere->getParameter("Position") != 0);
	QVERIFY(sphere->getParameter("Orientation") != 0);
	QVERIFY(sphere->getParameter("DynamicFriction") != 0);
	QVERIFY(sphere->getParameter("StaticFriction") != 0);
	QVERIFY(sphere->getParameter("Elasticity") != 0);
	QVERIFY(sphere->getParameter("Dynamic") != 0);
	QVERIFY(sphere->getParameter("Name") != 0);
	QVERIFY(sphere->getParameter("Mass") != 0);
	QVERIFY(sphere->getParameter("Color") != 0);
	DoubleValue *radius = dynamic_cast<DoubleValue*>(sphere->getParameter("Radius"));
	QVERIFY(radius != 0);

	QCOMPARE(radius->get(), 0.0);
	QCOMPARE(sphere->getCollisionObjects().size(), 1);
	QCOMPARE(sphere->getGeometries().size(), 1);

	SphereGeom *sphereGeom = dynamic_cast<SphereGeom*>(sphere->getGeometries().at(0));	
	QVERIFY(sphereGeom != 0);
	QCOMPARE(sphereGeom->getRadius(), 0.0);
	
	CollisionObject *collisionObject = sphere->getCollisionObjects().at(0);
	QVERIFY(collisionObject->getHostBody() == sphere);
	QVERIFY(collisionObject->getOwner() == 0);
	SphereGeom *collisionObjectGeom = dynamic_cast<SphereGeom*>(collisionObject->getGeometry());
	QVERIFY(collisionObjectGeom != 0);
	QCOMPARE(collisionObjectGeom->getRadius(), 0.0);

	radius->set(1.0);
	QCOMPARE(collisionObjectGeom->getRadius(), 0.0);
	QCOMPARE(sphereGeom->getRadius(), 0.0);

	sphere->setup();
	QVERIFY(collisionObject->getHostBody() == sphere);
	QVERIFY(collisionObject->getOwner() == sphere);
	QCOMPARE(collisionObjectGeom->getRadius(), 1.0);
	QCOMPARE(sphereGeom->getRadius(), 1.0);
	
	SphereBody *sphere2 = new SphereBody("Sphere2", 5.14);
	DoubleValue *radius2 = dynamic_cast<DoubleValue*>(sphere2->getParameter("Radius"));
	QVERIFY(radius2 != 0);
	QCOMPARE(radius2->get(), 5.14);
	SphereGeom *sphereGeom2 = dynamic_cast<SphereGeom*>(sphere2->getCollisionObjects().at(0)->getGeometry());

	QCOMPARE(sphereGeom2->getRadius(), 5.14);
	QCOMPARE(sphere2->getParameters().size(), 14);
	sphere2->getParameter("Position")->setValueFromString("(1.11, 2.2, 3.9)");
	Vector3DValue *position = dynamic_cast<Vector3DValue*>(sphere2->getParameter("Position"));
	QVERIFY(position != 0);
	QCOMPARE(position->getX(), 1.11);
	QCOMPARE(position->getY(), 2.2);
	QCOMPARE(position->getZ(), 3.9);	

	SphereBody *copy = dynamic_cast<SphereBody*>(sphere2->createCopy());	
	QVERIFY(copy != 0);
	QCOMPARE(copy->getParameters().size(), 14);
	QVERIFY(copy->getCollisionObjects().size() == 1);
	QVERIFY(copy->getGeometries().size() == 1);

	Vector3DValue *copyPosition = dynamic_cast<Vector3DValue*>(copy->getParameter("Position"));
	QVERIFY(copyPosition != 0);
	QCOMPARE(copyPosition->getX(), 1.11);
	QCOMPARE(copyPosition->getY(), 2.2);
	QCOMPARE(copyPosition->getZ(), 3.9);
	
	SphereGeom *copyGeometry = dynamic_cast<SphereGeom*>(copy->getCollisionObjects().at(0)->getGeometry());
	QVERIFY(copyGeometry != 0);
	QCOMPARE(copyGeometry->getRadius(), 5.14);

	CollisionObject *copyCollisionObject = copy->getCollisionObjects().at(0);
	QVERIFY(copyCollisionObject->getHostBody() == copy);
	QVERIFY(copyCollisionObject->getOwner() == 0);	

	copy->setup();
	QVERIFY(copyCollisionObject->getHostBody() == copy);
	QVERIFY(copyCollisionObject->getOwner() == copy);

	radius2->set(-10.0);
	sphere2->setup();
	QVERIFY(sphereGeom2->getRadius() > 0.0);

	delete copy;
	delete sphere2;
	delete sphere;
}

}
