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



#include "TestCollisionObject.h"
#include "Core/Core.h"
#include "Collision/CollisionObject.h"
#include "Physics/TriangleGeom.h"
#include "Physics/SphereGeom.h"
#include "Physics/BoxGeom.h"
#include "Physics/SimBody.h"
#include "Physics/SimBodyAdapter.h"
#include "Physics/Physics.h"
#include "Collision/MaterialProperties.h"

using namespace nerd;


void TestCollisionObject::testConstructor() {

	Core::resetCore();
	
	TriangleGeom geom(0);
	
	CollisionObject obj(geom);
	
	QVERIFY(obj.getOwner() == 0);
	QVERIFY(obj.getHostBody() == 0);
	QVERIFY(obj.getGeometry() != 0);
	QVERIFY(obj.getGeometry() != &geom);
	QVERIFY(dynamic_cast<TriangleGeom*>(obj.getGeometry()) != 0);
	QVERIFY(obj.areCollisionsReported() == true);
	QVERIFY(obj.getNativeCollisionObject() == 0);
	
	SimObject simObject("Sim1");
	SimBody simBody("Body1");
	
	SphereGeom sphere(0);
	
	CollisionObject obj2(sphere, &simObject, false);
	
	QVERIFY(obj2.getOwner() == &simObject);
	QVERIFY(obj2.getHostBody() == 0);
	QVERIFY(obj2.getGeometry() != 0);
	QVERIFY(obj2.getGeometry() != &sphere);
	QVERIFY(dynamic_cast<SphereGeom*>(obj2.getGeometry()) != 0);
	QVERIFY(obj2.areCollisionsReported() == false);
	QVERIFY(obj2.getNativeCollisionObject() == 0);
	
	obj2.setNativeCollisionObject(&simObject);
	QVERIFY(obj2.getNativeCollisionObject() == &simObject);
	
	obj2.setCollisionReport(true);
	QVERIFY(obj2.areCollisionsReported() == true);
	
	obj2.setHostBody(&simBody);
	QVERIFY(obj2.getHostBody() == &simBody);
	
	obj2.setGeometry(geom);
	QVERIFY(obj2.getGeometry() != 0);
	QVERIFY(obj2.getGeometry() != &geom);
	QVERIFY(dynamic_cast<TriangleGeom*>(obj2.getGeometry()) != 0);
}

void TestCollisionObject::testSetterAndGetter() {
	Core::resetCore();
	
	CollisionObject *co = new CollisionObject(SimGeom(0), 0);
	QVERIFY(co->areCollisionsReported());
	
	QVERIFY(co->getGeometry() != 0);
	QVERIFY(co->getHostBody() == 0);
	QVERIFY(co->getOwner() == 0);
	
	BoxGeom *box = new BoxGeom(0, 0.1, 0.22, 0.14);
	
	co->setGeometry(*box);
	QVERIFY(co->getGeometry() != box);
	QVERIFY(co->getGeometry() != 0);
	BoxGeom *boxCopy = dynamic_cast<BoxGeom*>(co->getGeometry());
	QVERIFY(boxCopy != 0);
	QCOMPARE(boxCopy->getWidth(), 0.1);
	QCOMPARE(boxCopy->getHeight(), 0.22);
	QCOMPARE(boxCopy->getDepth(), 0.14);

	SimBodyAdapter *body1 = new SimBodyAdapter();
	SimBodyAdapter *body2 = new SimBodyAdapter();
	QCOMPARE(body1->getCollisionObjects().size(), 0);
	QCOMPARE(body2->getCollisionObjects().size(), 0);
	
	co->setHostBody(body1);
	QCOMPARE(body1->getCollisionObjects().size(), 0);
	co->setOwner(body2);
	QCOMPARE(body2->getCollisionObjects().size(), 0);
	
	body2->addCollisionObject(co);
	QCOMPARE(body2->getCollisionObjects().size(), 1);
	QCOMPARE(co->getHostBody(), body2);
	
	co->setCollisionReport(true);
	QVERIFY(co->areCollisionsReported());
	
	co->setMaterialType("ABS");
	QCOMPARE(co->getMaterialType(), Physics::getCollisionManager()->getMaterialProperties()
		->getMaterialType("ABS"));
	
	co->setTextureType("AccelBoard");
	QCOMPARE(co->getTextureTypeInformation().at(0), Physics::getCollisionManager()->getMaterialProperties()->getTextureIndex("AccelBoard"));
	QCOMPARE(co->getTextureTypeInformation().size(), 6);
	
}

