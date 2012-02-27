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

#include "Test_ODEBodies.h"
#include "Core/Core.h"
#include "Physics/ODE_BodyAdapter.h"
#include "Physics/ODE_SimulationAlgorithm.h"
#include "Physics/Physics.h"
#include "Physics/BoxGeom.h"
#include "Physics/TriangleGeom.h"
#include "Physics/CylinderGeom.h"
#include "Physics/SphereGeom.h"
#include "Collision/CollisionObject.h"
#include "Physics/ODE_BoxBody.h"
#include "Physics/ODE_SphereBody.h"
#include "Physics/ODE_CylinderBody.h"
#include "Physics/ODE_Plane.h"
#include "Math/Math.h"

namespace nerd {

//Verena ?
void Test_ODEBodies::testODEBody() {
	Core::resetCore();
	ODE_SimulationAlgorithm *algorithm = new ODE_SimulationAlgorithm();
	Physics::getPhysicsManager()->setPhysicalSimulationAlgorithm(algorithm);
	ODE_BodyAdapter *body = new ODE_BodyAdapter("Body1");
	algorithm->resetPhysics();
	
	QVERIFY(body->getRigidBodyID() == 0);
	body->setup();
	QVERIFY(body->getRigidBodyID() != 0);
	QCOMPARE(body->getCollisionObjects().size(), 0);
	body->addCollisionObject(new CollisionObject(BoxGeom(body, 1, 1, 1), body));
	QCOMPARE(body->getCollisionObjects().size(), 1);
	// Test create Collision Objects: Creates a dGeomID if a valid Geom exists
	QVERIFY(body->createODECollisionObjects());
	CollisionObject *boxCollisionObject = body->getCollisionObjects().at(0);
	QVERIFY(boxCollisionObject->getNativeCollisionObject() != 0);
	// A valid dGeomID was created and added to the collision object as native collision object.
	QVERIFY(dGeomGetBody(static_cast<dGeomID>(boxCollisionObject->getNativeCollisionObject())) == body->getRigidBodyID());
	// create fails, if an invalid SimulationAlgorithm is used
	Physics::getPhysicsManager()->setPhysicalSimulationAlgorithm(0);
	QVERIFY(!body->createODECollisionObjects());
	Physics::getPhysicsManager()->setPhysicalSimulationAlgorithm(algorithm);
	QVERIFY(body->createODECollisionObjects());
	body->clear();
	// create fails, if no dRigidBody exists and the object is dynamic.
	QVERIFY(body->getRigidBodyID() == 0);
	QVERIFY(!body->createODECollisionObjects());
	// create succeeds, if the object is static.
	body->getParameter("Dynamic")->setValueFromString("False");
	QVERIFY(body->createODECollisionObjects());
	QVERIFY(body->getCollisionObjects().at(0)->getNativeCollisionObject() != 0);
	QCOMPARE(dGeomGetClass(static_cast<dGeomID>(body->getCollisionObjects().at(0)->getNativeCollisionObject())), 1);
	body->removeCollisionObject(boxCollisionObject);
	// Check whether the ode-type of the created objects is correct.
	body->addCollisionObject(new CollisionObject(SphereGeom(body, 1), body));
	QVERIFY(body->createODECollisionObjects());
	CollisionObject *sphereCollisionObject = body->getCollisionObjects().at(0);
	QVERIFY(sphereCollisionObject->getNativeCollisionObject() != 0);
	QCOMPARE(dGeomGetClass(static_cast<dGeomID>(body->getCollisionObjects().at(0)->getNativeCollisionObject())), 0);
	body->removeCollisionObject(sphereCollisionObject);
	body->addCollisionObject(new CollisionObject(CylinderGeom(body, 1, 2), body));
	QVERIFY(body->createODECollisionObjects());
	CollisionObject *cylinderCollisionObject = body->getCollisionObjects().at(0);
	QVERIFY(cylinderCollisionObject ->getNativeCollisionObject() != 0);
	QCOMPARE(dGeomGetClass(static_cast<dGeomID>(body->getCollisionObjects().at(0)->getNativeCollisionObject())), 3);
	body->removeCollisionObject(cylinderCollisionObject);
	body->addCollisionObject(new CollisionObject(TriangleGeom(body), body));
	QVERIFY(body->createODECollisionObjects());
	CollisionObject *trimeshCollisionObject = body->getCollisionObjects().at(0);
	QVERIFY(trimeshCollisionObject->getNativeCollisionObject() != 0);
	QCOMPARE(dGeomGetClass(static_cast<dGeomID>(body->getCollisionObjects().at(0)->getNativeCollisionObject())), 8);

}


//Verena ?
void Test_ODEBodies::testBoxBody() {
	Core::resetCore();
	ODE_SimulationAlgorithm *algorithm = new ODE_SimulationAlgorithm();
	Physics::getPhysicsManager()->setPhysicalSimulationAlgorithm(algorithm);
	ODE_BoxBody *box = new ODE_BoxBody("Box", 1, 1, 1);
	algorithm->resetPhysics();	
	QVERIFY(box->getParameter("Dynamic")->getValueAsString().compare("T") == 0);
	box->getPositionValue()->set(1, 0, 11);
	QVERIFY(box->getRigidBodyID() != 0);
	QVERIFY(box->createODECollisionObjects());

	dBodySetPosition(box->getRigidBodyID(), 2, 4, 0);
	QCOMPARE(box->getPositionValue()->getX(), 1.0);
	box->synchronizeWithPhysicalModel(algorithm);
	QCOMPARE(box->getPositionValue()->getX(), 2.0);
	box->getParameter("Mass")->setValueFromString("1.0");
	
	ODE_BoxBody *copy = dynamic_cast<ODE_BoxBody*>(box->createCopy());
	// Position is copied.
	QCOMPARE(copy->getPositionValue()->getX(), 2.0);
	QCOMPARE(copy->getPositionValue()->getY(), 4.0);
	QCOMPARE(copy->getPositionValue()->getZ(), 0.0);
	// Weight is copied.
	QVERIFY(copy->getParameter("Mass")->getValueAsString().compare("1") == 0);
	// RigidBody is not copied.
	QVERIFY(copy->getRigidBodyID() != box->getRigidBodyID());
	QVERIFY(copy->createODECollisionObjects());

	dBodySetPosition(copy->getRigidBodyID(), 0, 0, -2);
	copy->synchronizeWithPhysicalModel(algorithm);
	box->synchronizeWithPhysicalModel(algorithm);
	QCOMPARE(copy->getPositionValue()->getZ(), -2.0);
	QCOMPARE(box->getPositionValue()->getZ(), 0.0);
	
	// The ODE-Geom of a collision object is assoziated with the ODE-body of the host.
	CollisionObject *addOn = new CollisionObject(BoxGeom(box, 0.1, 0.1, 0.1), box);
	QCOMPARE(addOn->getOwner(), box);
	QVERIFY(addOn->getHostBody() == 0);
	copy->addCollisionObject(addOn);
	QCOMPARE(addOn->getHostBody(), copy);
	addOn->getGeometry()->setLocalPosition(Vector3D(2,0,0));
	QVERIFY(box->createODECollisionObjects());
	QVERIFY(addOn->getNativeCollisionObject() == 0);
	QVERIFY(copy->createODECollisionObjects());
	QVERIFY(dGeomGetBody(static_cast<dGeomID>(addOn->getNativeCollisionObject())) == copy->getRigidBodyID());

	delete box;
	delete copy;
}

void Test_ODEBodies::testSphereBody() {
	Core::resetCore();
	ODE_SimulationAlgorithm *algorithm = new ODE_SimulationAlgorithm();
	Physics::getPhysicsManager()->setPhysicalSimulationAlgorithm(algorithm);
	ODE_SphereBody *sphere = new ODE_SphereBody("Sphere", 0.01);

	algorithm->resetPhysics();
	QVERIFY(sphere->getRigidBodyID() != 0);
	QCOMPARE((double) dBodyGetPosition(sphere->getRigidBodyID())[0], 0.0);
	QCOMPARE((double) dBodyGetPosition(sphere->getRigidBodyID())[1], 0.0);
	QCOMPARE((double) dBodyGetPosition(sphere->getRigidBodyID())[2], 0.0);

	sphere->createODECollisionObjects();
		
	dBodySetPosition(sphere->getRigidBodyID(), 2.0, -1.0, 3.3);
	QCOMPARE(sphere->getPositionValue()->getX(), 0.0);
	QCOMPARE(sphere->getPositionValue()->getY(), 0.0);
	QCOMPARE(sphere->getPositionValue()->getZ(), 0.0);
	
	sphere->synchronizeWithPhysicalModel(algorithm);
	QCOMPARE(sphere->getPositionValue()->getX(), 2.0);
	QCOMPARE(sphere->getPositionValue()->getY(), -1.0);
	QCOMPARE(sphere->getPositionValue()->getZ(), 3.3);

	dMass mass;
	dBodyGetMass(sphere->getRigidBodyID(), &mass);
	QCOMPARE((double) mass.mass, 0.1);

	ODE_SphereBody *sphere2 = new ODE_SphereBody("Sphere2");	
	sphere2->getParameter("Dynamic")->setValueFromString("F");
	algorithm->resetPhysics();
	QVERIFY(sphere2->getRigidBodyID() == 0);
	
	QCOMPARE(dynamic_cast<DoubleValue*>(sphere2->getParameter("Radius"))->get(), 0.00001);

	ODE_SphereBody *copy = dynamic_cast<ODE_SphereBody*>(sphere->createCopy());
	QVERIFY(copy != 0);
	
	QCOMPARE(copy->getPositionValue()->getX(), 2.0);
	QCOMPARE((double) dBodyGetPosition(copy->getRigidBodyID())[0], 2.0);
	QCOMPARE((double) dBodyGetPosition(copy->getRigidBodyID())[1], -1.0);
	QCOMPARE((double) dBodyGetPosition(copy->getRigidBodyID())[2], 3.3);
	
	delete sphere;
	delete sphere2;
	delete copy;
}

void Test_ODEBodies::testCylinderBody() {
	Core::resetCore();
	ODE_SimulationAlgorithm *algorithm = new ODE_SimulationAlgorithm();
	Physics::getPhysicsManager()->setPhysicalSimulationAlgorithm(algorithm);
	ODE_CylinderBody *cylinder1 = new ODE_CylinderBody("Cylinder1", 0.01, 0.1);
	cylinder1->getPositionValue()->set(1.0, -1.0, 0.05);
	cylinder1->getCenterOfMassValue()->set(1.0, 0.0, 0.0);	

	QVERIFY(cylinder1->getRigidBodyID() == 0);
	
	algorithm->resetPhysics();
	QVERIFY(cylinder1->getRigidBodyID() != 0);
	
	QCOMPARE((double) dBodyGetPosition(cylinder1->getRigidBodyID())[0], 2.0);
	QCOMPARE((double) dBodyGetPosition(cylinder1->getRigidBodyID())[1], -1.0);
	QCOMPARE((double) dBodyGetPosition(cylinder1->getRigidBodyID())[2], 0.05);

	dBodySetPosition(cylinder1->getRigidBodyID(), 1.0, -2.0, 5.0);
	QCOMPARE((double) cylinder1->getPositionValue()->getX(), 1.0);
	QCOMPARE((double) cylinder1->getPositionValue()->getY(), -1.0);
	QCOMPARE((double) cylinder1->getPositionValue()->getZ(), 0.05);

	QCOMPARE(cylinder1->getCenterOfMassValue()->getX(), 1.0);	
	cylinder1->createODECollisionObjects();
	
	cylinder1->synchronizeWithPhysicalModel(algorithm);
	QCOMPARE((double) cylinder1->getCenterOfMassValue()->getX(), 1.0);
	QCOMPARE((double) cylinder1->getPositionValue()->getX(), 0.0);
	QCOMPARE((double) cylinder1->getPositionValue()->getY(), -2.0);
	QCOMPARE((double) cylinder1->getPositionValue()->getZ(), 5.0);

	cylinder1->getPositionValue()->set(1.0, 0.0, 1.0);
	cylinder1->getOrientationValue()->set(0.0, 90.0, 0.0);
	cylinder1->getCenterOfMassValue()->set(1.0, 0.0, 0.0);
	
	cylinder1->clear();
	cylinder1->setup();
	cylinder1->createODECollisionObjects();

	QVERIFY(Math::compareDoubles(dBodyGetPosition(cylinder1->getRigidBodyID())[0], 1.0, 3));
	QVERIFY(Math::compareDoubles(dBodyGetPosition(cylinder1->getRigidBodyID())[1], 0.0, 3));
	QVERIFY(Math::compareDoubles(dBodyGetPosition(cylinder1->getRigidBodyID())[2], 0.0, 3));

	cylinder1->synchronizeWithPhysicalModel(algorithm);
	
	QCOMPARE((double) cylinder1->getPositionValue()->getX(), 1.0);
	QCOMPARE((double) cylinder1->getPositionValue()->getY(), 0.0);
	QCOMPARE((double) cylinder1->getPositionValue()->getZ(), 1.0);

	dBodySetPosition(cylinder1->getRigidBodyID(), 2.0, 1.0, 2.0);
	cylinder1->synchronizeWithPhysicalModel(algorithm);
	
	QVERIFY(Math::compareDoubles(cylinder1->getPositionValue()->getX(), 2.0, 3));
	QVERIFY(Math::compareDoubles(cylinder1->getPositionValue()->getY(), 1.0, 3));
	QVERIFY(Math::compareDoubles(cylinder1->getPositionValue()->getZ(), 3.0, 3));

	delete cylinder1;
}

void Test_ODEBodies::testPlane() {
	Core::resetCore();
	ODE_SimulationAlgorithm *algorithm = new ODE_SimulationAlgorithm();
	Physics::getPhysicsManager()->setPhysicalSimulationAlgorithm(algorithm);
	ODE_Plane *plane1 = new ODE_Plane("Plane1");
	plane1->getParameter("Axis")->setValueFromString("(0.1, 0.0, 0.0)");	
	dynamic_cast<DoubleValue*>(plane1->getParameter("Distance"))->set(1.5);

	CollisionObject *co = plane1->getCollisionObjects().at(0);
	QVERIFY(plane1->getRigidBodyID() == 0);

	algorithm->resetPhysics();
	
	QVERIFY(plane1->getRigidBodyID() == 0);
	QVERIFY(co->getNativeCollisionObject() == 0);
	QVERIFY(plane1->createODECollisionObjects());
	QVERIFY(co->getNativeCollisionObject() != 0);
	
	dGeomID planeGeom = (dGeomID) co->getNativeCollisionObject(); //TODO check cast

	dVector4 params;
	dGeomPlaneGetParams(planeGeom, params);
	QCOMPARE((double) params[0], 1.0);
	QCOMPARE((double) params[1], 0.0);
	QCOMPARE((double) params[2], 0.0);
	QCOMPARE((double) params[3], 1.5);
	delete plane1;
}
}
