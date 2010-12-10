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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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



#include "TestSimBody.h"
#include "Physics/SimBody.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"
#include "Value/Vector3DValue.h"
#include "Core/Core.h"
#include "Core/Core.h"
#include "Physics/SimBodyAdapter.h"
#include "Collision/CollisionObjectAdapter.h"
#include "Physics/BoxGeom.h"
#include "Value/QuaternionValue.h"
#include "Value/IntValue.h"
#include "Value/ColorValue.h"
#include "Physics/BoxGeom.h"

using namespace nerd;

void TestSimBody::initTestCase(){
}



void TestSimBody::cleanUpTestCase(){
}


//chris
void TestSimBody::testConstructor() {
	Core::resetCore();
	
	SimBody sb1("SimBody1", "/Prefix/");
	
	QVERIFY(sb1.getName().compare("SimBody1") == 0);
	QVERIFY(sb1.getPrefix().compare("/Prefix/") == 0);
	
	StringValue *nameValue = dynamic_cast<StringValue*>(sb1.getParameter("Name"));
	Vector3DValue *positionValue = dynamic_cast<Vector3DValue*>(sb1.getParameter("Position"));
	Vector3DValue *orientationValue = 
				dynamic_cast<Vector3DValue*>(sb1.getParameter("Orientation"));
	Vector3DValue *centerOfMassValue =
	 			dynamic_cast<Vector3DValue*>(sb1.getParameter("CenterOfMass"));
	BoolValue *dynamicValue = dynamic_cast<BoolValue*>(sb1.getParameter("Dynamic"));
	DoubleValue *massValue = dynamic_cast<DoubleValue*>(sb1.getParameter("Mass"));
	DoubleValue *dynamicFrictionValue =
	 			dynamic_cast<DoubleValue*>(sb1.getParameter("DynamicFriction"));
	DoubleValue *staticFrictionValue =
	 			dynamic_cast<DoubleValue*>(sb1.getParameter("StaticFriction"));
	DoubleValue *elasticityValue = dynamic_cast<DoubleValue*>(sb1.getParameter("Elasticity"));
	ColorValue *colorValue = dynamic_cast<ColorValue*>(sb1.getParameter("Color"));
	StringValue *materialValue = dynamic_cast<StringValue*>(sb1.getParameter("Material"));

	QCOMPARE(sb1.getParameters().size(), 13);
	
	QVERIFY(nameValue != 0);
	QVERIFY(positionValue != 0);
	QVERIFY(orientationValue != 0);
	QVERIFY(centerOfMassValue != 0);
	QVERIFY(dynamicValue != 0);
	QVERIFY(massValue != 0);
	QVERIFY(dynamicFrictionValue != 0);
	QVERIFY(staticFrictionValue != 0);
	QVERIFY(elasticityValue != 0);
	QVERIFY(colorValue != 0);
	QVERIFY(materialValue != 0);
	
	QVERIFY(sb1.getPositionValue() == positionValue);
	QVERIFY(sb1.getOrientationValue() == orientationValue);
	QVERIFY(sb1.getCenterOfMassValue() == centerOfMassValue);
	QVERIFY(sb1.getQuaternionOrientationValue() != 0);

	materialValue->setValueFromString("Floor");

	
	
	//CreateCopy
	SimBody *sb2 = dynamic_cast<SimBody*>(sb1.createCopy());
	
	QVERIFY(sb2 != 0);
	
	QVERIFY(sb2->getName().compare("SimBody1") == 0);
	QVERIFY(sb2->getPrefix().compare("/Prefix/") == 0);
	
	StringValue *nameValue2 = dynamic_cast<StringValue*>(sb2->getParameter("Name"));
	Vector3DValue *positionValue2 = dynamic_cast<Vector3DValue*>(sb2->getParameter("Position"));
	Vector3DValue *orientationValue2 = 
				dynamic_cast<Vector3DValue*>(sb2->getParameter("Orientation"));
	Vector3DValue *centerOfMassValue2 =
	 			dynamic_cast<Vector3DValue*>(sb2->getParameter("CenterOfMass"));
	BoolValue *dynamicValue2 = dynamic_cast<BoolValue*>(sb2->getParameter("Dynamic"));
	DoubleValue *massValue2 = dynamic_cast<DoubleValue*>(sb2->getParameter("Mass"));
	DoubleValue *dynamicFrictionValue2 =
	 			dynamic_cast<DoubleValue*>(sb2->getParameter("DynamicFriction"));
	DoubleValue *staticFrictionValue2 =
	 			dynamic_cast<DoubleValue*>(sb2->getParameter("StaticFriction"));
	DoubleValue *elasticityValue2 = dynamic_cast<DoubleValue*>(sb2->getParameter("Elasticity"));
	ColorValue *colorValue2 = dynamic_cast<ColorValue*>(sb2->getParameter("Color"));
	StringValue *materialValue2 = dynamic_cast<StringValue*>(sb2->getParameter("Material"));
	
	QVERIFY(sb2->getParameters().size() == 13);
	
	QVERIFY(nameValue2 != 0);
	QVERIFY(positionValue2 != 0);
	QVERIFY(orientationValue2 != 0);
	QVERIFY(centerOfMassValue2 != 0);
	QVERIFY(dynamicValue2 != 0);
	QVERIFY(massValue2 != 0);
	QVERIFY(dynamicFrictionValue2 != 0);
	QVERIFY(staticFrictionValue2 != 0);
	QVERIFY(elasticityValue2 != 0);
	QVERIFY(colorValue2 != 0);
	QVERIFY(materialValue2 != 0);
	
	QVERIFY(sb2->getPositionValue() == positionValue2);
	QVERIFY(sb2->getOrientationValue() == orientationValue2);
	QVERIFY(sb2->getCenterOfMassValue() == centerOfMassValue2);
	QVERIFY(sb2->getQuaternionOrientationValue() != 0);
		

	QVERIFY(nameValue != nameValue2);
	QVERIFY(positionValue != positionValue2);
	QVERIFY(orientationValue != orientationValue2);
	QVERIFY(centerOfMassValue != centerOfMassValue2);
	QVERIFY(sb1.getQuaternionOrientationValue() != sb2->getQuaternionOrientationValue());
	QVERIFY(colorValue != colorValue2);
	QVERIFY(materialValue != materialValue2);
	QVERIFY(materialValue2->getValueAsString().compare("Floor") == 0);

	delete sb2;
	

}

//chris
void TestSimBody::testSetupAndClear() {
	Core::resetCore();

	//setup and clear. 
	SimBodyAdapter sba;
	QVERIFY(sba.mCountUpdate == 0);
	
	sba.setup(); //implicitely calls update().
	QVERIFY(sba.mCountUpdate == 1);
	
	sba.clear();
	QVERIFY(sba.mCountUpdate == 1);
	
	sba.setup();
	QVERIFY(sba.mCountUpdate == 2);
	
}

//chris
void TestSimBody::testAddAndRemoveCollisionObjects() {
	
	Core::resetCore();
	
	bool co1Destroyed = false;
	bool co2Destroyed = false;
	
	SimBody sbTmp("Temp");
	
	CollisionObjectAdapter *co1 = new CollisionObjectAdapter(
								BoxGeom(0, 1.0, 1.0, 1.0), &sbTmp, &co1Destroyed);
	
	CollisionObjectAdapter *co2 = new CollisionObjectAdapter(
								BoxGeom(0, 0.5, 0.5, 0.5), &sbTmp, &co2Destroyed);
	
	QVERIFY(co2->getHostBody() == 0);
	QVERIFY(co2->getOwner() == &sbTmp);
	
	
	
	{
		SimBody sb1("SimBody", "/Prefix/");
		
		QVERIFY(sb1.getCollisionObjects().size() == 0);
		QVERIFY(sb1.getGeometries().size() == 0);
		
		//adding NULL
		QVERIFY(sb1.addCollisionObject(0) == false);
		QVERIFY(sb1.getCollisionObjects().size() == 0);
		QVERIFY(sb1.getGeometries().size() == 0);
		
		//add co1 
		QVERIFY(sb1.addCollisionObject(co1) == true);
		QVERIFY(sb1.getCollisionObjects().size() == 1);
		QVERIFY(sb1.getCollisionObjects().contains(co1) == true);
		QVERIFY(sb1.getGeometries().size() == 1); //geometries are registered automatically.
		
		//add co1 again
		QVERIFY(sb1.addCollisionObject(co1) == false);
		QVERIFY(sb1.getCollisionObjects().size() == 1);
		
		//remove NULL
		QVERIFY(sb1.removeCollisionObject(0) == false);
		QVERIFY(sb1.getCollisionObjects().size() == 1);
		
		//remove a not previously added CollisionObject.
		QVERIFY(sb1.removeCollisionObject(co2) == false);
		QVERIFY(sb1.getCollisionObjects().size() == 1);
		
		//add the second collisionObject
		QVERIFY(sb1.addCollisionObject(co2) == true);
		QVERIFY(sb1.getCollisionObjects().size() == 2);
		QVERIFY(sb1.getGeometries().size() == 2);
		QVERIFY(co2->getHostBody() == &sb1);
		QVERIFY(co2->getOwner() == &sbTmp);
		
		//setup (SimGeoms are NOT AUTOMATICALLY created for visualization for each CollisionObject)
		//Only explicitely created CollisionObjects in subclasses of SimBody are added 
		//manually to the geometries.
		sb1.setup();
		QVERIFY(sb1.getCollisionObjects().size() == 2);
		QVERIFY(sb1.getGeometries().size() == 2);
		QVERIFY(co2->getHostBody() == &sb1);
		QVERIFY(co2->getOwner() == &sbTmp);
	
		// Collision Objects that are added to a SimBody are not copied.	
		SimBody *copy = dynamic_cast<SimBody*>(sb1.createCopy());
		QCOMPARE(copy->getCollisionObjects().size(), 0);
		
		//remove a CollisionObject.
		QVERIFY(co2Destroyed == false);
		QVERIFY(sb1.removeCollisionObject(co2) == true);
		QVERIFY(sb1.getCollisionObjects().size() == 1);
		QVERIFY(co2->getHostBody() == 0);
		QVERIFY(co2->getOwner() == &sbTmp);
		 //corresponding collision objects are removed automatically
		QVERIFY(sb1.getGeometries().size() == 1);
		
		//co2 was NOT destroyed
		QVERIFY(co2Destroyed == false);
		
		//destroy SimBody. (was on stack)
	}
	
	//Registered CollisionObject was also NOT destroyed (owner has to do this manually!)
	QVERIFY(co1Destroyed == false);
	
	delete co1;
	delete co2;

}


//Chris
void TestSimBody::testChangeParameterValues() {
	Core::resetCore();

	SimBody sb1("SimBody1", "P/");
	
	QVERIFY(sb1.getName().compare("SimBody1") == 0);
	QVERIFY(sb1.getPrefix().compare("P/") == 0);
	
	Vector3DValue *orientationValue = 
				dynamic_cast<Vector3DValue*>(sb1.getParameter("Orientation"));
	QuaternionValue *quaternionValue = sb1.getQuaternionOrientationValue();
	
	QCOMPARE(quaternionValue->getW(), 1.0);
	QCOMPARE(quaternionValue->getX(), 0.0);
	QCOMPARE(quaternionValue->getY(), 0.0);
	QCOMPARE(quaternionValue->getZ(), 0.0);
	
	QVERIFY(orientationValue->get().equals(Vector3D(0.0, 0.0, 0.0)));
	
	//setting the quaternionValue does NOT change the orientationValue.
	quaternionValue->set(5.0, 2.0, 4.0, 1.0);
	
	QCOMPARE(quaternionValue->getW(), 5.0);
	QCOMPARE(quaternionValue->getX(), 2.0);
	QCOMPARE(quaternionValue->getY(), 4.0);
	QCOMPARE(quaternionValue->getZ(), 1.0);
	
	QVERIFY(orientationValue->get().equals(Vector3D(0.0, 0.0, 0.0)));
	
	
	//setting the positionValue DOES change the quaternionValue.
	orientationValue->set(0.1, 0.2, 0.3);
	
	QuaternionValue qv;
	qv.setFromAngles(0.1, 0.2, 0.3);
	
	QCOMPARE(quaternionValue->getW(), qv.getW());
	QCOMPARE(quaternionValue->getX(), qv.getX());
	QCOMPARE(quaternionValue->getY(), qv.getY());
	QCOMPARE(quaternionValue->getZ(), qv.getZ());
	
	QVERIFY(orientationValue->get().equals(Vector3D(0.1, 0.2, 0.3)));

	BoxGeom *box1 = new BoxGeom(&sb1, 1, 1, 1);
	box1->setColor(100, 0, 0, 100);
	box1->setAutomaticColor(false);
	sb1.addGeometry(box1);

	BoxGeom *box2 = new BoxGeom(&sb1, 1, 1, 1);
	box2->setColor(0, 100, 0, 100);
	sb1.addGeometry(box2);
	box2->setAutomaticColor(true);
	
	sb1.getParameter("Color")->setValueFromString("blue");
	
	QCOMPARE(box1->getColor().red(), 100);
	QCOMPARE(box1->getColor().green(), 0);
	QCOMPARE(box1->getColor().blue(), 0);

	QCOMPARE(box2->getColor().red(), 0);
	QCOMPARE(box2->getColor().green(), 0);
	QCOMPARE(box2->getColor().blue(), 255);
	
}
