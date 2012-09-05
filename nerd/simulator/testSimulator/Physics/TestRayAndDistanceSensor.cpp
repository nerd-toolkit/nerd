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

#include "Core/Core.h"
#include "Physics/DistanceRay.h"
#include "Physics/DistanceSensor.h"
#include "Physics/RayGeom.h"
#include "Physics/SimObjectAdapter.h"
#include "TestRayAndDistanceSensor.h"
#include <iostream>

using namespace std;
using namespace nerd;

#define FOO(q) \
		"(" << q.getW() << ", " << q.getX() << ", " \
		<< q.getY() << ", " << q.getZ() << ")"
#define BAR(v) \
		"(" << ", " << v.getX() << ", " << v.getY() << ", " << v.getZ() << ")"

//chris
void TestRayAndDistanceSensor::testRayGeomConstructor() {
	Core::resetCore();
	SimObjectAdapter *simObject = new SimObjectAdapter("SimObje1", "/prefix/");

	RayGeom *ray1 = new RayGeom(0);

	QCOMPARE(ray1->getLength(), 0.0);
	QCOMPARE(ray1->getVisibleLength(), 0.0);
	QVERIFY(ray1->getSimObject() == 0);
	QVERIFY(ray1->getLocalPosition().equals(Vector3D(0.0, 0.0, 0.0)));
	QVERIFY(ray1->getLocalOrientation().equals(Quaternion(1.0, 0.0, 0.0, 0.0)));

	RayGeom *ray2 = new RayGeom(123.456);

	QCOMPARE(ray2->getLength(), 123.456);
	QCOMPARE(ray2->getVisibleLength(), 123.456);
	QVERIFY(ray2->getSimObject() == 0);
	ray2->setSimObject(simObject);
	QVERIFY(ray2->getSimObject() == simObject);
	QVERIFY(ray2->isDrawingEnabled() == true);
	QVERIFY(ray2->hasAutomaticColor() == true);

	RayGeom *rayCopy = dynamic_cast<RayGeom*>(ray2->createCopy());

	QVERIFY(rayCopy != 0);
	QVERIFY(rayCopy != ray2);
	QCOMPARE(rayCopy->getLength(), 123.456);
	QCOMPARE(rayCopy->getVisibleLength(), 123.456);
	QVERIFY(rayCopy->getSimObject() == simObject);
	QVERIFY(rayCopy->isDrawingEnabled() == true);
	QVERIFY(rayCopy->hasAutomaticColor() == true);

	// Change VisibleLength.
	ray1->setVisibleLength(50.01);
	QCOMPARE(ray1->getVisibleLength(), 50.01);
	QCOMPARE(ray1->getLength(), 0.0);
	ray1->setLength(12.56);
	QCOMPARE(ray1->getVisibleLength(), 50.01);
	QCOMPARE(ray1->getLength(), 12.56);

	// Change Length and VisibleLength to invalid values.
	ray1->setLength(-0.1);
	ray1->setVisibleLength(-10.5);
	QCOMPARE(ray1->getVisibleLength(), 50.01);
	QCOMPARE(ray1->getLength(), 12.56);

	Vector3D v(1.0, 2.0, 3.0);
	Quaternion q(0.9238795325112867, 0.3826834323650898, 0.7653668647301796,
			1.1480502970952693);
	q.normalize();

	// Translation to (1, 2, 3)...
	ray1->setLocalPosition(v);
	QVERIFY(ray1->getLocalPosition().equals(v, -1));
	QVERIFY(ray1->getLocalOrientation().equals(
				Quaternion(1.0, 0.0, 0.0, 0.0), -1));

	// ... and a rotation by 45 degrees along (1, 2, 3).
	ray1->setLocalOrientation(q);
	QVERIFY(ray1->getLocalPosition().equals(v, -1));
	QVERIFY(ray1->getLocalOrientation().equals(q, -1));

	ray1->setLocalPosition(Vector3D(0.0, 0.0, 0.0));
	ray1->setLocalOrientation(Quaternion(0.0, 0.0, 0.0, 0.0));

	// Rotation...
	ray1->setLocalOrientation(q);
	QVERIFY(ray1->getLocalPosition().equals(Vector3D(0.0, 0.0, 0.0), -1));
	QVERIFY(ray1->getLocalOrientation().equals(q, -1));

	// ... then translation.
	ray1->setLocalPosition(v);
	QVERIFY(ray1->getLocalPosition().equals(v, -1));
	QVERIFY(ray1->getLocalOrientation().equals(q, -1));

	delete ray1;
	delete ray2;
	delete rayCopy;
	delete simObject;
}

//Chris TODO!
void TestRayAndDistanceSensor::testDistanceRayConstructor() {
	Core::resetCore();

	DistanceRay *distanceRay = new DistanceRay("DistanceRay",
			Vector3D(1.0, 2.0, 3.0), Quaternion(1.05, 0.5, 0.4, 0.3), 100.0, 0,
			Color(255, 255, 255), Color(0, 0, 0), Color(100, 100, 100));

	QVERIFY(distanceRay->getCollisionObject() != 0);

	Vector3D v(1.0, 2.0, 3.0);
	Quaternion q(0.9238795325112867, 0.3826834323650898, 0.7653668647301796,
			1.1480502970952693);
	q.normalize();

	delete distanceRay;
}

