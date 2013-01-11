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


#include "TestBoxBody.h"
#include "Core/Core.h"
#include "Physics/BoxBody.h"
#include "Collision/CollisionObject.h"
#include "Physics/BoxGeom.h"
#include "Physics/Physics.h"
#include "Collision/MaterialProperties.h"
#include <iostream>

using namespace std;

namespace nerd{

void TestBoxBody::testBoxBody() {
	Core::resetCore();

	BoxBody *box = new BoxBody("Box1",1,1,1);	

	QCOMPARE(box->getParameters().size(), 16);
	QVERIFY(box->getParameter("Width") != 0);
	QVERIFY(box->getParameter("Height") != 0);
	QVERIFY(box->getParameter("Depth") != 0);
	QVERIFY(box->getParameter("Name") != 0);
	QVERIFY(box->getParameter("Position") != 0);
	QVERIFY(box->getParameter("Orientation") != 0);
	QVERIFY(box->getParameter("CenterOfMass") != 0);
	QVERIFY(box->getParameter("Dynamic") != 0);
	QVERIFY(box->getParameter("DynamicFriction") != 0);
	QVERIFY(box->getParameter("StaticFriction") != 0);
	QVERIFY(box->getParameter("Elasticity") != 0);
	QVERIFY(box->getParameter("Mass") != 0);
	QVERIFY(box->getParameter("Color") != 0);
	QVERIFY(box->getParameter("Material") != 0);
	
	box->getParameter("Material")->setValueFromString("Carpet");
	box->getParameter("Texture")->setValueFromString("ABS");
	box->getParameter("Color")->setValueFromString("(100, 77, 35, 100");

	QVERIFY(box->getParameter("Color")->getValueAsString().compare("black") == 0);
	QCOMPARE(box->getCollisionObjects().at(0)->getMaterialType(), 
		Physics::getCollisionManager()->getMaterialProperties()->getMaterialType("Carpet"));

	DoubleValue *width = dynamic_cast<DoubleValue*>(box->getParameter("Width"));
	QVERIFY(width != 0);
	QCOMPARE(width->get(), 1.0);

	QVERIFY(box->getCollisionObjects().size() == 1);
	QVERIFY(box->getGeometries().size() == 1);
	QCOMPARE(box->getCollisionObjects().at(0)->getGeometry(), box->getGeometries().at(0));

	CollisionObject *boxCo = box->getCollisionObjects().at(0);
	BoxGeom *boxGeom = dynamic_cast<BoxGeom*>(boxCo->getGeometry());
	QVERIFY(boxGeom != 0);
	QCOMPARE(boxGeom->getPoint(0).getX(), -0.5);
	QVERIFY(boxCo->getHostBody() == box);
	QVERIFY(boxCo->getOwner() == 0);

	box->setup();
	QVERIFY(boxCo->getOwner() == box);


	box->getParameter("Color")->setValueFromString("(100, 77, 35, 100)");
	ColorValue *color = dynamic_cast<ColorValue*>(box->getParameter("Color"));
	QCOMPARE(color->get().red(), 100);
// Test copy constructor and createCopy
	BoxBody *copy = dynamic_cast<BoxBody*>(box->createCopy());
	QCOMPARE(copy->getCollisionObjects().at(0)->getGeometry(), copy->getGeometries().at(0));

	QCOMPARE(copy->getParameters().size(), 16);
	

	QCOMPARE(copy->getCollisionObjects().at(0)->getMaterialType(),
		Physics::getCollisionManager()->getMaterialProperties()->getMaterialType("Carpet"));

	DoubleValue *width2 = dynamic_cast<DoubleValue*>(copy->getParameter("Width"));
	QVERIFY(width2 != 0);
	QCOMPARE(width2->get(), 1.0);
	QVERIFY(copy->getGeometries().size() == 1);
	QVERIFY(copy->getCollisionObjects().size() == 1);
	
	QVERIFY(copy->getGeometries().at(0) != box->getGeometries().at(0));
	QVERIFY(copy->getCollisionObjects().at(0) != box->getCollisionObjects().at(0));
	
	BoxGeom *collisionBox = dynamic_cast<BoxGeom*>(copy->getCollisionObjects().at(0)->getGeometry());
	copy->setup();
	QCOMPARE(collisionBox->getPoint(0).getX(), -0.5);
	Color copyColor = collisionBox->getColor();
	
	QCOMPARE(copyColor.red(), 100);
	QCOMPARE(copyColor.green(), 77);
	QCOMPARE(copyColor.blue(), 35);
	QCOMPARE(copyColor.alpha(), 100);
	
	QCOMPARE(copy->getCollisionObjects().at(0)->getMaterialType(),  Physics::getCollisionManager()->getMaterialProperties()->getMaterialType("Carpet"));

	QVERIFY(copy->getParameter("Texture")->getValueAsString().compare("ABS") == 0);
	
	width->set(20);

	box->setup();
	copy->setup();
	QVERIFY(box->getGeometries().size() == 1);
	QVERIFY(box->getGeometries().at(0) == boxGeom);
	QVERIFY(box->getCollisionObjects().size() == 1);
	QVERIFY(box->getCollisionObjects().at(0) == boxCo);
	QCOMPARE(boxGeom->getPoint(0).getX(), -10.0);
	QCOMPARE(collisionBox->getPoint(0).getX(), -0.5);

	width->set(-1.1);
	box->setup();
	QVERIFY(boxGeom->getWidth() > 0.0);
	
	delete box;
	delete copy;

	//test default Constructor
	BoxBody *defaultBox = new BoxBody("Default");
	QCOMPARE(defaultBox->getParameters().size(), 16);
	QVERIFY(defaultBox->getParameter("Width") != 0);
	QVERIFY(defaultBox->getParameter("Height") != 0);
	QVERIFY(defaultBox->getParameter("Depth") != 0);
	QVERIFY(defaultBox->getParameter("Name") != 0);
	QVERIFY(defaultBox->getParameter("Position") != 0);
	QVERIFY(defaultBox->getParameter("Orientation") != 0);
	QVERIFY(defaultBox->getParameter("CenterOfMass") != 0);
	QVERIFY(defaultBox->getParameter("Dynamic") != 0);
	QVERIFY(defaultBox->getParameter("DynamicFriction") != 0);
	QVERIFY(defaultBox->getParameter("StaticFriction") != 0);
	QVERIFY(defaultBox->getParameter("Elasticity") != 0);
	QVERIFY(defaultBox->getParameter("Mass") != 0);
	QVERIFY(defaultBox->getParameter("Color") != 0);

	BoxGeom *defaultBoxGeom = dynamic_cast<BoxGeom*>(defaultBox->getCollisionObjects().at(0)->getGeometry());
	
	DoubleValue *defaultwidth = dynamic_cast<DoubleValue*>(defaultBox->getParameter("Width"));
	QVERIFY(defaultwidth != 0);
	QCOMPARE(defaultwidth->get(), 0.01);
	defaultBox->setup();
	QCOMPARE(defaultBoxGeom->getPoint(0).getX(), -0.005);
	defaultwidth->set(5);
	QCOMPARE(defaultBoxGeom->getPoint(0).getX(), -0.005);
	defaultBox->setup();
	QCOMPARE(defaultBoxGeom->getPoint(0).getX(), -2.5);

	Vector3DValue *position = dynamic_cast<Vector3DValue*>(defaultBox->getParameter("Position"));
	QCOMPARE(position->getX(), 0.0);
	QCOMPARE(position->getY(), 0.0);
	QCOMPARE(position->getZ(), 0.0);
	position->set(10,20,30);
	QCOMPARE(position->getX(), 10.0);
	QCOMPARE(position->getY(), 20.0);
	QCOMPARE(position->getZ(), 30.0);
	delete defaultBox;
}

}
