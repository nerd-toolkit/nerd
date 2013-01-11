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


#include "TestValue.h"
#include "MyChangedListener.h"
#include "Value/Value.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"
#include "Value/BoolValue.h"
#include "Value/StringValue.h"
#include "Value/Vector3DValue.h"
#include "Value/Matrix3x3Value.h"
#include "Value/QuaternionValue.h"
#include "Value/NormalizedDoubleValue.h"
#include "Value/InterfaceValue.h"
#include "Math/Quaternion.h"
#include <string>
#include <iostream>
#include <QString>
#include "Core/Core.h"
#include "Value/ColorValue.h"
#include <iostream>
#include "Value/MatrixValue.h"
#include "Value/RangeValue.h"
#include <Value/CodeValue.h>
#include <Value/FileNameValue.h>


using namespace std;


//TODO InterfaceValue is untested!

using namespace nerd;

void TestValue::initTestCase(){
}

//verena
void TestValue::testValue(){
	Core::resetCore();
	Value *value = new Value();

	QCOMPARE ( QString("") , value->getTypeName()); 

	value->setTypeName ( "newValue" );

	QCOMPARE ( QString("newValue") ,value->getTypeName()); 

	QVERIFY ( "newValue" == value->getTypeName() );

	QVERIFY ( value->getValueChangedListeners().size() == 0 );
	
	delete value;
}


//verena
void TestValue::testValueNotify(){
	Core::resetCore();
	Value *value = new Value();
	MyChangedListener listener1;

	QVERIFY ( value->addValueChangedListener ( &listener1 ) );
	value->notifyValueChanged();
	QCOMPARE ( listener1.getCount() , 1 );

	QCOMPARE ( value->getValueChangedListeners().at ( 0 ),&listener1 );

	MyChangedListener listener2;
	MyChangedListener listener3;
	MyChangedListener listener4;

	QVERIFY ( value->addValueChangedListener ( &listener2 ) );
	value->notifyValueChanged();

	QCOMPARE ( listener1.getCount() , 2 );
	QCOMPARE ( listener2.getCount() , 1 );
	QCOMPARE ( listener3.getCount() , 0 );
	QCOMPARE ( listener4.getCount() , 0 );

	QVERIFY ( value->addValueChangedListener ( &listener3 ) );
	value->notifyValueChanged();
	QCOMPARE ( listener1.getCount() , 3 );
	QCOMPARE ( listener2.getCount() , 2 );
	QCOMPARE ( listener3.getCount() , 1 );
	QCOMPARE ( listener4.getCount() , 0 );


	QVERIFY ( value->removeValueChangedListener ( &listener1 ) );
	value->notifyValueChanged();
	QCOMPARE ( listener1.getCount() , 3 );
	QCOMPARE ( listener2.getCount() , 3 );
	QCOMPARE ( listener3.getCount() , 2 );
	QCOMPARE ( listener4.getCount() , 0 );

	QVERIFY ( value->removeValueChangedListener ( &listener2 ) );
	QVERIFY ( value->removeValueChangedListener ( &listener3 ) );
	QVERIFY ( !value->removeValueChangedListener ( &listener4 ) );
	delete value;
}

//chris
void TestValue::testRemoveAllListeners() {
	Core::resetCore();

	Value value;
	MyChangedListener l1;
	MyChangedListener l2;

	QVERIFY(value.addValueChangedListener(&l1));
	QVERIFY(value.addValueChangedListener(&l2));
	QCOMPARE(value.getValueChangedListeners().size(), 2);

	value.removeValueChangedListeners();
	QCOMPARE(value.getValueChangedListeners().size(), 0);
}


//verena
void TestValue::testIntValue(){
	Core::resetCore();
	IntValue v1;
	QVERIFY(v1.get() == 0);

	IntValue *value = new IntValue ( 10 );
	MyChangedListener listener;
	MyChangedListener listener2;

	value->addValueChangedListener ( &listener );
	value->set ( 100 );
	
	QCOMPARE ( listener.getCount() , 1 );

	value->set ( 100 );
	QCOMPARE ( listener.getCount() , 1 );


	QString test = "10000";
	QVERIFY ( value->setValueFromString ( test ) );
	QCOMPARE ( 10000 ,  value->get());
	QCOMPARE ( listener.getCount() , 2 );


	QVERIFY ( !value->setValueFromString ("1100a") );
	QCOMPARE ( 10000 ,  value->get());
	QCOMPARE ( listener.getCount() , 2 );

	QVERIFY ( value->setValueFromString ( test ) );
	QCOMPARE ( listener.getCount() , 2 );
	value->addValueChangedListener ( &listener2 );

	for ( int i = 0; i < 1000; i++ )
	{
		value->set ( i+19 );
	}

	QCOMPARE ( listener.getCount() , 1002 );

	
	QCOMPARE ( listener2.getCount() , 1000 );

	QString number = "1018";
	QCOMPARE (value->getValueAsString() , number );

	IntValue *copyValue = dynamic_cast<IntValue*>(value->createCopy());
	
	
	QCOMPARE(copyValue->getTypeName() , QString("Int") );
	QCOMPARE(copyValue->get() , 1018 );
	
	delete value;
	delete copyValue;
}


//verena
void TestValue::testBoolValue(){
	Core::resetCore();
	BoolValue v1;
	QVERIFY(v1.get() == true);

	BoolValue value(true);
	BoolValue *copy = dynamic_cast<BoolValue*>(value.createCopy());

	MyChangedListener l1;
	QVERIFY(copy->addValueChangedListener(&l1));
	copy->setNotifyAllSetAttempts(true);
	
	QCOMPARE(copy->getTypeName() , QString("Bool"));
	QCOMPARE (copy->getValueAsString() , QString("T"));
	QVERIFY(copy->setValueFromString("false"));
	QVERIFY(!copy->get());
	QCOMPARE(l1.getCount(), 1);	

	QVERIFY(copy->setValueFromString("FALsE"));
	QVERIFY(!copy->get());
	QCOMPARE(l1.getCount(), 2);	

	QVERIFY(copy->setValueFromString("F"));
	QVERIFY(!copy->get());

	QVERIFY(copy->setValueFromString("f"));
	QVERIFY(!copy->get());

	QVERIFY(copy->setValueFromString("0"));
	QVERIFY(!copy->get());
	QCOMPARE(l1.getCount(), 5);	
	
	QVERIFY(!copy->setValueFromString("notFalse"));
	QCOMPARE(l1.getCount(), 5);	//no notification

	copy->setValueFromString("t");
	QVERIFY(copy->get());
	QCOMPARE(l1.getCount(), 6);	

	copy->setValueFromString("T");
	QVERIFY(copy->get());

	copy->setValueFromString("TRUE");
	QVERIFY(copy->get());

	copy->setValueFromString("trUe");
	QVERIFY(copy->get());

	copy->setValueFromString("1");
	QVERIFY(copy->get());
	QCOMPARE(l1.getCount(), 10);	

	copy->setNotifyAllSetAttempts(false);
	copy->setValueFromString("true");
	QCOMPARE(l1.getCount(), 10); //no notification.

	QVERIFY(!copy->setValueFromString("Hello"));
	
	delete copy;
	
}

//verena
void TestValue::testDoubleValue(){
	Core::resetCore();
	DoubleValue v1;
	QCOMPARE(v1.get(), 0.0);

	DoubleValue *value1 = new DoubleValue (0.1);
	QCOMPARE(value1->get(), 0.1);
	MyChangedListener listener;
	MyChangedListener listener2;

	value1->addValueChangedListener (&listener);
	value1->set (0.2);
	QCOMPARE(listener.getCount() , 1);
	QString test = "0.0001";
	QVERIFY(value1->setValueFromString(test));
	QCOMPARE(0.0001 ,  value1->get());
	QCOMPARE(listener.getCount() , 2);

	value1->setNotifyAllSetAttempts(true);
	QVERIFY(value1->setValueFromString(test)); //should still trigger a notify.
	QCOMPARE(0.0001, value1->get());
	QCOMPARE(listener.getCount(), 3);
	

	value1->addValueChangedListener (&listener2);

	DoubleValue *copy = dynamic_cast<DoubleValue*>(value1->createCopy());
	QCOMPARE(copy->get() , 0.0001);
	QVERIFY(!copy->setValueFromString("0!98"));
	QCOMPARE(copy->get() , 0.0001);
	
	delete value1;
	delete copy;
}

//Chris
void TestValue::testVector3DValue() {
	Core::resetCore();

	MyChangedListener listener;
	Vector3DValue v1;

	//test constructos
	QVERIFY(v1.getName().compare("Vector3D") == 0);
	QVERIFY(v1.getTypeName().compare("Vector3D") == 0);
	QVERIFY(v1.getX() == 0.0);	
	QVERIFY(v1.getY() == 0.0);
	QVERIFY(v1.getZ() == 0.0);
	QVERIFY(v1.isNotifyingAllSetAttempts() == false);
	QVERIFY(v1.getValueAsString().compare("(0,0,0)") == 0);
	QVERIFY(v1.getValueChangedListeners().empty() == true);
	
	Vector3DValue v2(99.9, 0.0008, 7777777.7);
	QVERIFY(v2.getX() == 99.9);	
	QVERIFY(v2.getY() == 0.0008);
	QVERIFY(v2.getZ() == 7777777.7);
	
	Vector3D v3d = v2.get();
	QVERIFY(v3d.getX() == 99.9);
	QVERIFY(v3d.getY() == 0.0008);
	QVERIFY(v3d.getZ() == 7777777.7);

	v2.set(Vector3D(111.1, 2.22, 0.33));
	QVERIFY(v2.getX() == 111.1);	
	QVERIFY(v2.getY() == 2.22);
	QVERIFY(v2.getZ() == 0.33);
	QVERIFY(v2.getValueAsString().compare("(111.1,2.22,0.33)") == 0);

	v2.setNotifyAllSetAttempts(true);
	v2.setTypeName("MyDesc");

	QVERIFY(v2.isNotifyingAllSetAttempts() == true);
	QVERIFY(v2.getName().compare("MyDesc") == 0);
	QVERIFY(v2.getTypeName().compare("MyDesc") == 0);

	//Copy constructor
	Vector3DValue v3(v2);
	QVERIFY(v3.getX() == 111.1);	
	QVERIFY(v3.getY() == 2.22);
	QVERIFY(v3.getZ() == 0.33);
	QVERIFY(v3.getValueAsString().compare("(111.1,2.22,0.33)") == 0);
	QVERIFY(v3.isNotifyingAllSetAttempts() == true);
	QVERIFY(v3.getName().compare("MyDesc") == 0);
	QVERIFY(v3.getTypeName().compare("MyDesc") == 0);

	//createCopy()
	Vector3DValue *v4 = dynamic_cast<Vector3DValue*>(v3.createCopy());
	QVERIFY(v4 != 0);
	QVERIFY(v4 != &v3);
	QVERIFY(v4->getX() == 111.1);	
	QVERIFY(v4->getY() == 2.22);
	QVERIFY(v4->getZ() == 0.33);
	QVERIFY(v4->getValueAsString().compare("(111.1,2.22,0.33)") == 0);
	QVERIFY(v4->isNotifyingAllSetAttempts() == true);
	QVERIFY(v4->getName().compare("MyDesc") == 0);
	QVERIFY(v4->getTypeName().compare("MyDesc") == 0);
	delete v4;

	//test set and get and listeners
	//Listeners

	v1.addValueChangedListener(&listener);
	QVERIFY(listener.mCount == 0);
	QVERIFY(listener.mLastChangedValue == 0);

	v1.set(50, 40, 30);
	QVERIFY(v1.getX() == 50);	
	QVERIFY(v1.getY() == 40);
	QVERIFY(v1.getZ() == 30);

	QCOMPARE(listener.mCount, 1);
	QVERIFY(listener.mLastChangedValue == &v1);

	listener.reset();
	
	QVERIFY(v1.setValueFromString("DoesNotWork") == false);
	QVERIFY(v1.getX() == 50);	
	QVERIFY(v1.getY() == 40);
	QVERIFY(v1.getZ() == 30);

	QVERIFY(listener.mCount == 0);
	QVERIFY(listener.mLastChangedValue == 0);

	listener.reset();
	
	QVERIFY(v1.setValueFromString("(Does,not,work,either)") == false);
	QVERIFY(v1.getX() == 50);	
	QVERIFY(v1.getY() == 40);
	QVERIFY(v1.getZ() == 30);

	QVERIFY(listener.mCount == 0);
	QVERIFY(listener.mLastChangedValue == 0);

	listener.reset();
	
	QVERIFY(v1.setValueFromString("(10.0,,5.5,1)") == false); //does not work (missing number)
	QVERIFY(v1.getX() == 50);	
	QVERIFY(v1.getY() == 40);
	QVERIFY(v1.getZ() == 30);

	QVERIFY(listener.mCount == 0);
	QVERIFY(listener.mLastChangedValue == 0);

	listener.reset();
	
	QVERIFY(v1.setValueFromString("(1,2,3,4)") == false); //does not work (too many numbers)
	QVERIFY(v1.getX() == 50);	
	QVERIFY(v1.getY() == 40);
	QVERIFY(v1.getZ() == 30);

	QVERIFY(listener.mCount == 0);
	QVERIFY(listener.mLastChangedValue == 0);

	listener.reset();

	QVERIFY(v1.setValueFromString("(101.123456789,55,0.0012345678912345)") == true); //works
	QVERIFY(v1.getX() == 101.123456789);	
	QVERIFY(v1.getY() == 55);
	QVERIFY(v1.getZ() == 0.0012345678912345);
	std::cout << "IS: " << v1.getValueAsString().toStdString() << std::endl;
	QVERIFY(v1.getValueAsString()
				.compare("(101.12346,55,0.0012345679)") == 0);  
		//rounds (!), up to 8 significant digits.

	QVERIFY(listener.mCount == 1);
	QVERIFY(listener.mLastChangedValue == &v1);

	QVERIFY(v1.setValueFromString("3, 2, 1") == false); //missing braces
	QVERIFY(v1.getX() == 101.123456789);	
	QVERIFY(v1.getY() == 55);
	QVERIFY(v1.getZ() == 0.0012345678912345);
	
	QVERIFY(v1.setValueFromString("0.255,1.22,-5.0") == false); //missing braces
	QVERIFY(v1.getX() == 101.123456789);	
	QVERIFY(v1.getY() == 55);
	QVERIFY(v1.getZ() == 0.0012345678912345);

	QVERIFY(v1.setValueFromString("(3.0, 2.0, 1.11") == false); //missing brace
	QVERIFY(v1.setValueFromString("3.5, 2.0, -1.0") == false); //missing braces


	QVERIFY(v1.setValueFromString("(3.0, 2.0, 1.0)") == true); //ok

	listener.reset();

	QVERIFY(v1.setValueFromString(v2.getValueAsString()) == true); //works
	QVERIFY(v1.getX() == 111.1);	
	QVERIFY(v1.getY() == 2.22);
	QVERIFY(v1.getZ() == 0.33);

	QVERIFY(listener.mCount == 1);
	QVERIFY(listener.mLastChangedValue == &v1);


	//test multi part value
	QCOMPARE(v1.getNumberOfValueParts(), 3);
	QVERIFY(dynamic_cast<DoubleValue*>(v1.getValuePart(0)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(v1.getValuePart(1)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(v1.getValuePart(2)) != 0);

	QCOMPARE(dynamic_cast<DoubleValue*>(v1.getValuePart(0))->get(), 111.1);
	QCOMPARE(dynamic_cast<DoubleValue*>(v1.getValuePart(1))->get(), 2.22);
	QCOMPARE(dynamic_cast<DoubleValue*>(v1.getValuePart(2))->get(), 0.33);

	QVERIFY(v1.getValuePartName(0) == "x");
	QVERIFY(v1.getValuePartName(1) == "y");
	QVERIFY(v1.getValuePartName(2) == "z");

	listener.reset();

	dynamic_cast<DoubleValue*>(v1.getValuePart(0))->set(123.4);
	QCOMPARE(listener.mCount, 1);
	QVERIFY(listener.mLastChangedValue = &v1); //not v1.getvaluePart(0)
	dynamic_cast<DoubleValue*>(v1.getValuePart(1))->set(234.4);
	QCOMPARE(listener.mCount, 2);
	QVERIFY(listener.mLastChangedValue = &v1);
	dynamic_cast<DoubleValue*>(v1.getValuePart(2))->set(345.4);
	QCOMPARE(listener.mCount, 3);
	QVERIFY(listener.mLastChangedValue = &v1);

	v1.removeValueChangedListener(&listener);
	listener.reset();

	dynamic_cast<DoubleValue*>(v1.getValuePart(0))->set(123.4);
	QCOMPARE(listener.mCount, 0);
	QVERIFY(listener.mLastChangedValue == 0);
}

void TestValue::testMatrix3x3Value() {
	Core::resetCore();

	MyChangedListener listener;
	Matrix3x3Value m1;

	//test constructos
	QVERIFY(m1.getName().compare("Matrix3x3") == 0);
	QVERIFY(m1.getTypeName().compare("Matrix3x3") == 0);
	QVERIFY(m1.get(1, 1) == 0.0);	
	QVERIFY(m1.get(1, 2) == 0.0);
	QVERIFY(m1.get(1, 3) == 0.0);
	QVERIFY(m1.get(2, 1) == 0.0);
	QVERIFY(m1.get(2, 2) == 0.0);
	QVERIFY(m1.get(2, 3) == 0.0);
	QVERIFY(m1.get(3, 1) == 0.0);
	QVERIFY(m1.get(3, 2) == 0.0);
	QVERIFY(m1.get(3, 3) == 0.0);
	QVERIFY(m1.isNotifyingAllSetAttempts() == false);
	QVERIFY(m1.getValueAsString().compare("(0,0,0;0,0,0;0,0,0)") == 0);

	QVERIFY(m1.getValueChangedListeners().empty() == true);
	
	Matrix3x3Value m2(1.1, 1.2, 1.3, 2.1, 2.2, 2.3, 3.1, 3.2, 3.3);
	QVERIFY(m2.get(1, 1) == 1.1);
	QVERIFY(m2.get(1, 2) == 1.2);
	QVERIFY(m2.get(1, 3) == 1.3);
	QVERIFY(m2.get(2, 1) == 2.1);
	QVERIFY(m2.get(2, 2) == 2.2);
	QVERIFY(m2.get(2, 3) == 2.3);
	QVERIFY(m2.get(3, 1) == 3.1);
	QVERIFY(m2.get(3, 2) == 3.2);
	QVERIFY(m2.get(3, 3) == 3.3);

	Matrix3x3 m = m2.get();
	QVERIFY(m.get(1, 1) == 1.1);
	QVERIFY(m.get(1, 2) == 1.2);
	QVERIFY(m.get(1, 3) == 1.3);
	QVERIFY(m.get(2, 1) == 2.1);
	QVERIFY(m.get(2, 2) == 2.2);
	QVERIFY(m.get(2, 3) == 2.3);
	QVERIFY(m.get(3, 1) == 3.1);
	QVERIFY(m.get(3, 2) == 3.2);
	QVERIFY(m.get(3, 3) == 3.3);

	Matrix3x3Value m3(m2);
	QVERIFY(m3.get(1, 1) == 1.1);
	QVERIFY(m3.get(1, 2) == 1.2);
	QVERIFY(m3.get(1, 3) == 1.3);
	QVERIFY(m3.get(2, 1) == 2.1);
	QVERIFY(m3.get(2, 2) == 2.2);
	QVERIFY(m3.get(2, 3) == 2.3);
	QVERIFY(m3.get(3, 1) == 3.1);
	QVERIFY(m3.get(3, 2) == 3.2);
	QVERIFY(m3.get(3, 3) == 3.3);

	m.set(1, 1, 11);
	m.set(1, 2, 12);
	m.set(1, 3, 13);
	m.set(2, 1, 21);
	m.set(2, 2, 22);
	m.set(2, 3, 23);
	m.set(3, 1, 31);
	m.set(3, 2, 32);
	m.set(3, 3, 33);
	m3.set(m);
	QVERIFY(m3.get(1, 1) == 11);
	QVERIFY(m3.get(1, 2) == 12);
	QVERIFY(m3.get(1, 3) == 13);
	QVERIFY(m3.get(2, 1) == 21);
	QVERIFY(m3.get(2, 2) == 22);
	QVERIFY(m3.get(2, 3) == 23);
	QVERIFY(m3.get(3, 1) == 31);
	QVERIFY(m3.get(3, 2) == 32);
	QVERIFY(m3.get(3, 3) == 33);

	QVERIFY(!m3.setValueFromString("(0.11, 0.12, 0.13; 0.21, 0.22, 0.23; 0.31, 0.32, 0.33]"));
	QVERIFY(m3.get(1, 1) == 11);
	QVERIFY(m3.get(1, 2) == 12);
	QVERIFY(m3.get(1, 3) == 13);
	QVERIFY(m3.get(2, 1) == 21);
	QVERIFY(m3.get(2, 2) == 22);
	QVERIFY(m3.get(2, 3) == 23);
	QVERIFY(m3.get(3, 1) == 31);
	QVERIFY(m3.get(3, 2) == 32);
	QVERIFY(m3.get(3, 3) == 33);
	QVERIFY(!m3.setValueFromString("(0.11, 0.12, 0.13; 0.21, 0.22, 0.23; 0.31, 0.32, 0.33"));
	QVERIFY(m3.get(1, 1) == 11);
	QVERIFY(m3.get(1, 2) == 12);
	QVERIFY(m3.get(1, 3) == 13);
	QVERIFY(m3.get(2, 1) == 21);
	QVERIFY(m3.get(2, 2) == 22);
	QVERIFY(m3.get(2, 3) == 23);
	QVERIFY(m3.get(3, 1) == 31);
	QVERIFY(m3.get(3, 2) == 32);
	QVERIFY(m3.get(3, 3) == 33);
	QVERIFY(!m3.setValueFromString("(0.11, 0.12, 0.13; 0.21, 0.22, 0.23, 0.31, 0.32, 0.33)"));
	QVERIFY(m3.get(1, 1) == 11);
	QVERIFY(m3.get(1, 2) == 12);
	QVERIFY(m3.get(1, 3) == 13);
	QVERIFY(m3.get(2, 1) == 21);
	QVERIFY(m3.get(2, 2) == 22);
	QVERIFY(m3.get(2, 3) == 23);
	QVERIFY(m3.get(3, 1) == 31);
	QVERIFY(m3.get(3, 2) == 32);
	QVERIFY(m3.get(3, 3) == 33);
	QVERIFY(m3.setValueFromString("(0.11, 0.12, 0.13; 0.21, 0.22, 0.23; 0.31, 0.32, 0.33)"));
	QVERIFY(m3.get(1, 1) == 0.11);
	QVERIFY(m3.get(1, 2) == 0.12);
	QVERIFY(m3.get(1, 3) == 0.13);
	QVERIFY(m3.get(2, 1) == 0.21);
	QVERIFY(m3.get(2, 2) == 0.22);
	QVERIFY(m3.get(2, 3) == 0.23);
	QVERIFY(m3.get(3, 1) == 0.31);
	QVERIFY(m3.get(3, 2) == 0.32);
	QVERIFY(m3.get(3, 3) == 0.33);
	QVERIFY(!m3.setValueFromString("(0.011, 0.012, 0.013; 0.021, 0.022, 0.023; 0.031, 0.032, )"));
	QVERIFY(m3.get(1, 1) == 0.11);
	QVERIFY(m3.get(1, 2) == 0.12);
	QVERIFY(m3.get(1, 3) == 0.13);
	QVERIFY(m3.get(2, 1) == 0.21);
	QVERIFY(m3.get(2, 2) == 0.22);
	QVERIFY(m3.get(2, 3) == 0.23);
	QVERIFY(m3.get(3, 1) == 0.31);
	QVERIFY(m3.get(3, 2) == 0.32);
	QVERIFY(m3.get(3, 3) == 0.33);
	QVERIFY(!m3.setValueFromString("(0.011, 0.012, 0.013; 0.021, 0.022, 0.023; 0.031, 0.032, 0.033, 0.034)"));
	QVERIFY(m3.get(1, 1) == 0.11);
	QVERIFY(m3.get(1, 2) == 0.12);
	QVERIFY(m3.get(1, 3) == 0.13);
	QVERIFY(m3.get(2, 1) == 0.21);
	QVERIFY(m3.get(2, 2) == 0.22);
	QVERIFY(m3.get(2, 3) == 0.23);
	QVERIFY(m3.get(3, 1) == 0.31);
	QVERIFY(m3.get(3, 2) == 0.32);
	QVERIFY(m3.get(3, 3) == 0.33);
	QVERIFY(m3.setValueFromString("(0.011, 0.012, 0.013; 0.021, 0.022, 0.023; 0.031, 0.032, 0.033)"));
	QVERIFY(m3.get(1, 1) == 0.011);
	QVERIFY(m3.get(1, 2) == 0.012);
	QVERIFY(m3.get(1, 3) == 0.013);
	QVERIFY(m3.get(2, 1) == 0.021);
	QVERIFY(m3.get(2, 2) == 0.022);
	QVERIFY(m3.get(2, 3) == 0.023);
	QVERIFY(m3.get(3, 1) == 0.031);
	QVERIFY(m3.get(3, 2) == 0.032);
	QVERIFY(m3.get(3, 3) == 0.033);
	QVERIFY(m3.getValueAsString().compare("(0.011,0.012,0.013;0.021,0.022,0.023;0.031,0.032,0.033)") == 0);

	Matrix3x3Value m4(m3);
	QVERIFY(m4.equals(&m3));

	// test multi part value
	QCOMPARE(m4.getNumberOfValueParts(), 9);
	QVERIFY(dynamic_cast<DoubleValue*>(m4.getValuePart(0)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(m4.getValuePart(1)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(m4.getValuePart(2)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(m4.getValuePart(3)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(m4.getValuePart(4)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(m4.getValuePart(5)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(m4.getValuePart(6)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(m4.getValuePart(7)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(m4.getValuePart(8)) != 0);

	QCOMPARE(dynamic_cast<DoubleValue*>(m4.getValuePart(0))->get(), 0.011);
	QCOMPARE(dynamic_cast<DoubleValue*>(m4.getValuePart(1))->get(), 0.012);
	QCOMPARE(dynamic_cast<DoubleValue*>(m4.getValuePart(2))->get(), 0.013);
	QCOMPARE(dynamic_cast<DoubleValue*>(m4.getValuePart(3))->get(), 0.021);
	QCOMPARE(dynamic_cast<DoubleValue*>(m4.getValuePart(4))->get(), 0.022);
	QCOMPARE(dynamic_cast<DoubleValue*>(m4.getValuePart(5))->get(), 0.023);
	QCOMPARE(dynamic_cast<DoubleValue*>(m4.getValuePart(6))->get(), 0.031);
	QCOMPARE(dynamic_cast<DoubleValue*>(m4.getValuePart(7))->get(), 0.032);
	QCOMPARE(dynamic_cast<DoubleValue*>(m4.getValuePart(8))->get(), 0.033);

	QVERIFY(m4.getValuePartName(0) == "m11");
	QVERIFY(m4.getValuePartName(1) == "m12");
	QVERIFY(m4.getValuePartName(2) == "m13");
	QVERIFY(m4.getValuePartName(3) == "m21");
	QVERIFY(m4.getValuePartName(4) == "m22");
	QVERIFY(m4.getValuePartName(5) == "m23");
	QVERIFY(m4.getValuePartName(6) == "m31");
	QVERIFY(m4.getValuePartName(7) == "m32");
	QVERIFY(m4.getValuePartName(8) == "m33");

	//test set and get and listeners
	//Listeners
	m1.addValueChangedListener(&listener);
	QVERIFY(listener.mCount == 0);
	QVERIFY(listener.mLastChangedValue == 0);

	m1.set(1, 2, 3, 4, 5, 6, 7, 8, 9);
	QVERIFY(m1.get(1, 1) == 1);
	QVERIFY(m1.get(1, 2) == 2);
	QVERIFY(m1.get(1, 3) == 3);
	QVERIFY(m1.get(2, 1) == 4);
	QVERIFY(m1.get(2, 2) == 5);
	QVERIFY(m1.get(2, 3) == 6);
	QVERIFY(m1.get(3, 1) == 7);
	QVERIFY(m1.get(3, 2) == 8);
	QVERIFY(m1.get(3, 3) == 9);

	QCOMPARE(listener.mCount, 1);
	QVERIFY(listener.mLastChangedValue == &m1);

	listener.reset();
}


//verena & chris
void TestValue::testStringValue(){
	Core::resetCore();
	StringValue *value = new StringValue();
	
	QVERIFY(value->get().compare("") == 0);
	value->set("Stuff");
	QVERIFY(value->get().compare("Stuff") == 0);
	
	QVERIFY(value->setValueFromString("TestValue"));
	StringValue *copy = dynamic_cast<StringValue*>(value->createCopy());
	QCOMPARE(copy->get(), QString("TestValue"));
	
	StringValue v2("Another Test");
	QVERIFY(v2.get() == "Another Test");
	QVERIFY(v2.getValueAsString() == "Another Test");
	
	StringValue v3(v2);
	QVERIFY(v2.get() == "Another Test");
	QVERIFY(v2.getValueAsString() == "Another Test");
	
	delete value;
	delete copy;
}

// Verena
void TestValue::testQuaternionValue() {
	Core::resetCore();

	MyChangedListener listener;
	
	QuaternionValue *value = new QuaternionValue();
	QCOMPARE(value->getW(), 1.0);
	QCOMPARE(value->getX(), 0.0);
	QCOMPARE(value->getY(), 0.0);
	QCOMPARE(value->getZ(), 0.0);

	QuaternionValue *value2 = new QuaternionValue(1,2,3,4);
	QCOMPARE(value2->getW(), 1.0);
	QCOMPARE(value2->getX(), 2.0);
	QCOMPARE(value2->getY(), 3.0);
	QCOMPARE(value2->getZ(), 4.0);

	QuaternionValue *copy = value2;
	QCOMPARE(copy->getW(), 1.0);
	QCOMPARE(copy->getX(), 2.0);
	QCOMPARE(copy->getY(), 3.0);
	QCOMPARE(copy->getZ(), 4.0);

	copy->set(4,3,2,1);
	QCOMPARE(copy->getW(), 4.0);
	QCOMPARE(copy->getX(), 3.0);
	QCOMPARE(copy->getY(), 2.0);
	QCOMPARE(copy->getZ(), 1.0);
	
	QuaternionValue *copyOfCopy = dynamic_cast<QuaternionValue*>(copy->createCopy());

	QCOMPARE(copyOfCopy->getW(), 4.0);
	QCOMPARE(copyOfCopy->getX(), 3.0);
	QCOMPARE(copyOfCopy->getY(), 2.0);
	QCOMPARE(copyOfCopy->getZ(), 1.0);
	
	Quaternion *quat = new Quaternion(2.2, 1.1, 3.3, 4.4);
	value->set(*quat);
	QCOMPARE(value->getW(), 2.2);
	QCOMPARE(value->getX(), 1.1);
	QCOMPARE(value->getY(), 3.3);
	QCOMPARE(value->getZ(), 4.4);

	*quat = copy->get();
	QCOMPARE(quat->getW(), 4.0);
	QCOMPARE(quat->getX(), 3.0);
	QCOMPARE(quat->getY(), 2.0);
	QCOMPARE(quat->getZ(), 1.0);

	value->addValueChangedListener(&listener);

	QVERIFY(value->getValueAsString().compare("(2.2,1.1,3.3,4.4)") == 0);

	value->setValueFromString("(0.1, 0.5, 3.1, 5.2)");
	QCOMPARE(value->getW(), 0.1);
	QCOMPARE(value->getX(), 0.5);
	QCOMPARE(value->getY(), 3.1);
	QCOMPARE(value->getZ(), 5.2);

	value->setValueFromString("(4,4,4,4,4)");
	QCOMPARE(value->getW(), 0.1);
	QCOMPARE(value->getX(), 0.5);
	QCOMPARE(value->getY(), 3.1);
	QCOMPARE(value->getZ(), 5.2);

	value->setValueFromString("(4,4,4,4");
	QCOMPARE(value->getW(), 0.1);
	QCOMPARE(value->getX(), 0.5);
	QCOMPARE(value->getY(), 3.1);
	QCOMPARE(value->getZ(), 5.2);

	value->setValueFromString("(4,4,4,)");
	QCOMPARE(value->getW(), 0.1);
	QCOMPARE(value->getX(), 0.5);
	QCOMPARE(value->getY(), 3.1);
	QCOMPARE(value->getZ(), 5.2);

	value->setValueFromString("(4.1,4.2,4.3,4.4)");
	QCOMPARE(value->getW(), 4.1);
	QCOMPARE(value->getX(), 4.2);
	QCOMPARE(value->getY(), 4.3);
	QCOMPARE(value->getZ(), 4.4);
	
	//test multi part value
	QCOMPARE(value->getNumberOfValueParts(), 4);
	QVERIFY(dynamic_cast<DoubleValue*>(value->getValuePart(0)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(value->getValuePart(1)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(value->getValuePart(2)) != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(value->getValuePart(3)) != 0);

	QCOMPARE(dynamic_cast<DoubleValue*>(value->getValuePart(0))->get(), 4.1);
	QCOMPARE(dynamic_cast<DoubleValue*>(value->getValuePart(1))->get(), 4.2);
	QCOMPARE(dynamic_cast<DoubleValue*>(value->getValuePart(2))->get(), 4.3);
	QCOMPARE(dynamic_cast<DoubleValue*>(value->getValuePart(3))->get(), 4.4);

	QVERIFY(value->getValuePartName(0) == "w");
	QVERIFY(value->getValuePartName(1) == "x");
	QVERIFY(value->getValuePartName(2) == "y");
	QVERIFY(value->getValuePartName(3) == "z");

	listener.reset();

	dynamic_cast<DoubleValue*>(value->getValuePart(0))->set(123.4);
	QCOMPARE(listener.mCount, 1);
	QVERIFY(listener.mLastChangedValue = value); //not value->getvaluePart(0)
	dynamic_cast<DoubleValue*>(value->getValuePart(1))->set(234.4);
	QCOMPARE(listener.mCount, 2);
	QVERIFY(listener.mLastChangedValue = value);
	dynamic_cast<DoubleValue*>(value->getValuePart(2))->set(345.4);
	QCOMPARE(listener.mCount, 3);
	QVERIFY(listener.mLastChangedValue = value);

	value->removeValueChangedListener(&listener);
	listener.reset();

	dynamic_cast<DoubleValue*>(value->getValuePart(0))->set(123.4);
	QCOMPARE(listener.mCount, 0);
	QVERIFY(listener.mLastChangedValue == 0);


	delete quat;
	delete value;
	delete value2;
	delete copyOfCopy;
}


//chris
void TestValue::testColorValue() {
	Core::resetCore();
	
	ColorValue c1;
	
	QVERIFY(c1.get().equals(Color(0,0,0,255)));
	QVERIFY(c1.getValueAsString().compare("black") == 0);
	
	ColorValue c2("green");
	QVERIFY(c2.get().equals(Color(0,255,0,255)));
	QVERIFY(c2.getValueAsString().compare("green") == 0);
	
	ColorValue c3(QString("blue"));
	QVERIFY(c3.get().equals(Color(0,0,255,255)));
	QVERIFY(c3.getValueAsString().compare("blue") == 0);
	
	//wrong name (use default black)
	ColorValue c4(QString("ultravioletcyan"));
	QVERIFY(c4.get().equals(Color(0,0,0,255)));
	QVERIFY(c4.getValueAsString().compare("black") == 0);

	//non-standard value  //TODO maybe arbitrary values should be supported.
	ColorValue c5(Color(1, 2, 3, 4));
	QVERIFY(c5.get().equals(Color(1, 2, 3, 4)));
	QVERIFY(c5.getValueAsString().compare("(1,2,3,4)") == 0);
	
	//copy construtor
	ColorValue *copy3 = dynamic_cast<ColorValue*>(c3.createCopy());
	QVERIFY(copy3 != 0);
	QVERIFY(copy3->get().equals(Color(0,0,255,255)) == true);
	QVERIFY(copy3->getValueAsString().compare("blue") == 0);
	delete copy3;
	
	ColorValue *copy5 =  dynamic_cast<ColorValue*>(c5.createCopy());
	QVERIFY(copy5 != 0);
	QVERIFY(copy5->get().equals(Color(1, 2, 3, 4)));
	delete copy5;
	
	//test value changes (get and set)
	MyChangedListener listener;
	c1.addValueChangedListener(&listener);
	
	QVERIFY(listener.mCount == 0);
	
	//value changed, listener notified
	c1.set(0,255,255);
	QVERIFY(listener.mCount == 1);
	QVERIFY(listener.mLastChangedValue == &c1);
	
	c1.set(0,0,255);
	QVERIFY(listener.mCount == 2);

	//value not altered, listener not notified
	c1.set(0,0,255);
	QVERIFY(listener.mCount == 2);

	//notified, because alwaysNotify is on.
	c1.setNotifyAllSetAttempts(true);
	c1.set(0,0,255);
	QVERIFY(listener.mCount == 3);
	
	c1.set(-100, 20, 400);
	QCOMPARE(c1.get().red(), 0);
	QCOMPARE(c1.get().green(), 20);
	QCOMPARE(c1.get().blue(), 255);

	//Test all string-color pairs.
	c1.setValueFromString("red");
	QCOMPARE(c1.get().red(), 255);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 0);
	QCOMPARE(c1.get().alpha(), 255);

	c1.setValueFromString("blue");
	QCOMPARE(c1.get().red(), 0);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 255);
	QCOMPARE(c1.get().alpha(), 255);

	c1.setValueFromString("rEd");
	QCOMPARE(c1.get().red(), 255);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 0);
	QCOMPARE(c1.get().alpha(), 255);

	c1.setValueFromString("GREEN");
	QCOMPARE(c1.get().red(), 0);
	QCOMPARE(c1.get().green(), 255);
	QCOMPARE(c1.get().blue(), 0);
	QCOMPARE(c1.get().alpha(), 255);

	c1.setValueFromString("darkred");
	QCOMPARE(c1.get().red(), 128);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 0);
	QCOMPARE(c1.get().alpha(), 255);

	c1.setValueFromString("darkGreen");
	QCOMPARE(c1.get().red(), 0);
	QCOMPARE(c1.get().green(), 128);
	QCOMPARE(c1.get().blue(), 0);
	QCOMPARE(c1.get().alpha(), 255);

	c1.setValueFromString("darkblue");
	QCOMPARE(c1.get().red(), 0);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 128);
	QCOMPARE(c1.get().alpha(), 255);	

	QVERIFY(c1.getValueAsString().compare("darkBlue") == 0);

	c1.setValueFromString("cyan");
	QCOMPARE(c1.get().red(), 0);
	QCOMPARE(c1.get().green(), 255);
	QCOMPARE(c1.get().blue(), 255);
	QCOMPARE(c1.get().alpha(), 255);	

	c1.setValueFromString("darkcyan");
	QCOMPARE(c1.get().red(), 0);
	QCOMPARE(c1.get().green(), 128);
	QCOMPARE(c1.get().blue(), 128);
	QCOMPARE(c1.get().alpha(), 255);	

	c1.setValueFromString("magenta");
	QCOMPARE(c1.get().red(), 255);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 255);
	QCOMPARE(c1.get().alpha(), 255);	

	c1.setValueFromString("darkmagenta");
	QCOMPARE(c1.get().red(), 128);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 128);
	QCOMPARE(c1.get().alpha(), 255);	

	c1.setValueFromString("white");
	QCOMPARE(c1.get().red(), 255);
	QCOMPARE(c1.get().green(), 255);
	QCOMPARE(c1.get().blue(), 255);
	QCOMPARE(c1.get().alpha(), 255);	
	c1.set(0, 0, 0);
	QVERIFY(c1.get().equals(Color(0, 0, 0)));
	QVERIFY(c1.getValueAsString() == "black");


	c1.setValueFromString("yelloW");
	QCOMPARE(c1.get().red(), 255);
	QCOMPARE(c1.get().green(), 255);
	QCOMPARE(c1.get().blue(), 0);
	QCOMPARE(c1.get().alpha(), 255);	
	
	c1.setValueFromString("darkyelloW");
	QCOMPARE(c1.get().red(), 128);
	QCOMPARE(c1.get().green(), 128);
	QCOMPARE(c1.get().blue(), 0);
	QCOMPARE(c1.get().alpha(), 255);	

	c1.setValueFromString("gray");
	QCOMPARE(c1.get().red(), 160);
	QCOMPARE(c1.get().green(), 160);
	QCOMPARE(c1.get().blue(), 160);
	QCOMPARE(c1.get().alpha(), 255);	

	c1.setValueFromString("darkgray");
	QCOMPARE(c1.get().red(), 128);
	QCOMPARE(c1.get().green(), 128);
	QCOMPARE(c1.get().blue(), 128);
	QCOMPARE(c1.get().alpha(), 255);	

	c1.setValueFromString("lightgray");
	QCOMPARE(c1.get().red(), 192);
	QCOMPARE(c1.get().green(), 192);
	QCOMPARE(c1.get().blue(), 192);
	QCOMPARE(c1.get().alpha(), 255);	

	c1.setValueFromString("(100,20,44)");
	QCOMPARE(c1.get().red(), 100);
	QCOMPARE(c1.get().green(), 20);
	QCOMPARE(c1.get().blue(), 44);
	QCOMPARE(c1.get().alpha(), 255);	

	c1.setValueFromString("(11,0,42, 100)");
	QCOMPARE(c1.get().red(), 11);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 42);
	QCOMPARE(c1.get().alpha(), 100);	

	QVERIFY(!c1.setValueFromString("(0,200,200,200"));
	QCOMPARE(c1.get().red(), 11);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 42);
	QCOMPARE(c1.get().alpha(), 100);	

	QVERIFY(!c1.setValueFromString("0,200,200,200)"));
	QCOMPARE(c1.get().red(), 11);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 42);
	QCOMPARE(c1.get().alpha(), 100);	
	
	QVERIFY(!c1.setValueFromString("(0,11a,200,200)"));
	QCOMPARE(c1.get().red(), 11);
	QCOMPARE(c1.get().green(), 0);
	QCOMPARE(c1.get().blue(), 42);
	QCOMPARE(c1.get().alpha(), 100);	


	c1.removeValueChangedListener(&listener);

}

//chris
void TestValue::testMatrixValue() {
	Core::resetCore();
	
	MatrixValue m1;

	QCOMPARE(m1.getMatrixWidth(), 1);
	QCOMPARE(m1.getMatrixHeight(), 1);
	QCOMPARE(m1.getMatrixDepth(), 1);

	QCOMPARE(m1.get(0, 0, 0), 0.0);
	m1.set(2.34, 0, 0, 0);
	QCOMPARE(m1.get(0, 0, 0), 2.34);

	m1.resize(2, 4, 3);
	QCOMPARE(m1.getMatrixWidth(), 2);
	QCOMPARE(m1.getMatrixHeight(), 4);
	QCOMPARE(m1.getMatrixDepth(), 3);

	//previous values are reset to zero
	QCOMPARE(m1.get(0, 0, 0), 0.0);

	//other values are set to zero, too
	{
		Matrix mdata1 = m1.get();
		for(int i = 0; i < 2; ++i) {
			for(int j = 0; j < 4; ++j) {
				for(int k = 0; k < 3; ++k) {
					if(i == 0 && j == 0 && k == 0) {
						continue;
					}
					QCOMPARE(mdata1.get(i, j, k), 0.0);
					m1.set((double) (i + j + k), i, j, k);
				}
			}
		}
	}
	{
		Matrix mdata1 = m1.get();
		for(int i = 0; i < 2; ++i) {
			for(int j = 0; j < 4; ++j) {
				for(int k = 0; k < 3; ++k) {
					if(i == 0 && j == 0 && k == 0) {
						continue;
					}
					QCOMPARE(mdata1.get(i, j, k), (double) (i + j + k));
				}
			}
		}
	}

	QString compare = "{{{0,1}{1,2}{2,3}{3,4}}{{1,2}{2,3}{3,4}{4,5}}{{2,3}{3,4}{4,5}{5,6}}}";
	QVERIFY(m1.getValueAsString() == compare);
	
}

//chris
void TestValue::testValueEquality() {

	//Value
	Value v1("MyDescription", true);
	Value v2("MyDescription", true);

	QVERIFY(v1.equals(0) == false);

	QVERIFY(v1.equals(&v1) == true);
	QVERIFY(v1.equals(&v2) == true);
	QVERIFY(v2.equals(&v1) == true);

	v2.setTypeName("MyDescription2");
	QVERIFY(v1.equals(&v2) == false);
	QVERIFY(v2.equals(&v1) == false);

	v1.setTypeName("MyDescription2");
	QVERIFY(v1.equals(&v2) == true);

	v2.setNotifyAllSetAttempts(false);
	QVERIFY(v1.equals(&v2) == false);
	QVERIFY(v2.equals(&v1) == false);

	v1.setNotifyAllSetAttempts(false);
	QVERIFY(v1.equals(&v2) == true);

	{
		//BoolValue
		BoolValue bv1(true);
		BoolValue bv2(true);
	
		QVERIFY(bv1.equals(0) == false);
	
		QVERIFY(bv1.equals(&bv1) == true);
		QVERIFY(bv1.equals(&bv2) == true);
		QVERIFY(bv2.equals(&bv1) == true);
	
		bv1.set(false);
		QVERIFY(bv1.equals(&bv2) == false);
	
		bv2.set(false);
		QVERIFY(bv1.equals(&bv2) == true);
	
		bv2.setTypeName("SomeStuff");
		QVERIFY(bv1.equals(&bv2) == false);
	}

	{
		//ColorValue
		ColorValue cv1("red");
		ColorValue cv2("red");
	
		QVERIFY(cv1.equals(0) == false);
		
		QVERIFY(cv1.equals(&cv1) == true);
		QVERIFY(cv1.equals(&cv2) == true);
		QVERIFY(cv2.equals(&cv1) == true);
	
		cv1.set(10, 20, 30, 40);
		QVERIFY(cv1.equals(&cv2) == false);
	
		cv2.set(10, 20, 30, 40);
		QVERIFY(cv1.equals(&cv2) == true);
	
		cv2.setTypeName("SomeStuff");
		QVERIFY(cv1.equals(&cv2) == false);
	}

	{
		//DoubleValue
		DoubleValue v1(123.456);
		DoubleValue v2(123.456);
		
		QVERIFY(v1.equals(0) == false);
		
		QVERIFY(v1.equals(&v1) == true);
		QVERIFY(v1.equals(&v2) == true);
		QVERIFY(v2.equals(&v1) == true);
	
		cout << "Set to 123.4567" << endl;
		v1.set(123.4567);
		QVERIFY(v1.equals(&v2) == false);
	
		v2.set(123.4567);
		QVERIFY(v1.equals(&v2) == true);
	
		v2.setTypeName("SomeStuff");
		QVERIFY(v1.equals(&v2) == false);
	}

	{
		//NormalizedDoubleValue
		NormalizedDoubleValue v1(123.456, 0.0, 200.0);
		NormalizedDoubleValue v2(123.456, 0.0, 200.0);
		
		QVERIFY(v1.equals(0) == false);
		
		QVERIFY(v1.equals(&v1) == true);
		QVERIFY(v1.equals(&v2) == true);
		QVERIFY(v2.equals(&v1) == true);
	
		v1.set(123.4567);
		QVERIFY(v1.equals(&v2) == false);
	
		v2.set(123.4567);
		QVERIFY(v1.equals(&v2) == true);

		v1.setMin(-0.5);
		QVERIFY(v1.equals(&v2) == false);

		v2.setMin(-0.5);
		QVERIFY(v1.equals(&v2) == true);

		v1.setMax(500.5);
		QVERIFY(v1.equals(&v2) == false);

		v2.setMax(500.5);
		QVERIFY(v1.equals(&v2) == true);

		v1.setNormalizedMax(-0.001);
		QVERIFY(v1.equals(&v2) == false);

		v2.setNormalizedMax(-0.001);
		QVERIFY(v1.equals(&v2) == true);

		v1.setNormalizedMin(1.005);
		QVERIFY(v1.equals(&v2) == false);

		v2.setNormalizedMin(1.005);
		QVERIFY(v1.equals(&v2) == true);
	
		v2.setTypeName("SomeStuff");
		QVERIFY(v1.equals(&v2) == false);
	}

	{
		//InterfaceValue
		InterfaceValue v1("pref/", "InterfaceName", 123.456, 0.0, 200.0, -0.1, 500.0);
		InterfaceValue v2("pref/", "InterfaceName", 123.456, 0.0, 200.0, -0.1, 500.0);
		
		QVERIFY(v1.equals(0) == false);
		
		QVERIFY(v1.equals(&v1) == true);
		QVERIFY(v1.equals(&v2) == true);
		QVERIFY(v2.equals(&v1) == true);
	
		v1.setInterfaceName("AnotherInterface");
		QVERIFY(v1.equals(&v2) == false);
	
		v2.setInterfaceName("AnotherInterface");
		QVERIFY(v1.equals(&v2) == true);

		v1.setPrefix("/Prefix2/");
		QVERIFY(v1.equals(&v2) == false);
	
		v2.setPrefix("/Prefix2/");
		QVERIFY(v1.equals(&v2) == true);

		v1.setNormalizedMax(400.1);
		QVERIFY(v1.equals(&v2) == false);
	
		v2.setNormalizedMax(400.1);
		QVERIFY(v1.equals(&v2) == true);
	
		v2.setTypeName("SomeStuff");
		QVERIFY(v1.equals(&v2) == false);
	}

	{
		//IntValue
		IntValue v1(123);
		IntValue v2(123);
		
		QVERIFY(v1.equals(0) == false);
		
		QVERIFY(v1.equals(&v1) == true);
		QVERIFY(v1.equals(&v2) == true);
		QVERIFY(v2.equals(&v1) == true);
	
		v1.set(124);
		QVERIFY(v1.equals(&v2) == false);
	
		v2.set(124);
		QVERIFY(v1.equals(&v2) == true);
	
		v2.setTypeName("SomeStuff");
		QVERIFY(v1.equals(&v2) == false);
	}

	{
		//QuaternionValue
		QuaternionValue v1(0.1, 0.2, 0.3, 0.4);
		QuaternionValue v2(0.1, 0.2, 0.3, 0.4);
		
		QVERIFY(v1.equals(0) == false);
		
		QVERIFY(v1.equals(&v1) == true);
		QVERIFY(v1.equals(&v2) == true);
		QVERIFY(v2.equals(&v1) == true);
	
		v1.set(0.11, 0.2, 0.3, 0.4);
		QVERIFY(v1.equals(&v2) == false);
		v2.set(0.11, 0.2, 0.3, 0.4);
		QVERIFY(v1.equals(&v2) == true);

		v1.set(0.11, 0.21, 0.3, 0.4);
		QVERIFY(v1.equals(&v2) == false);
		v2.set(0.11, 0.21, 0.3, 0.4);
		QVERIFY(v1.equals(&v2) == true);

		v1.set(0.11, 0.21, 0.31, 0.4);
		QVERIFY(v1.equals(&v2) == false);
		v2.set(0.11, 0.21, 0.31, 0.4);
		QVERIFY(v1.equals(&v2) == true);

		v1.set(0.11, 0.21, 0.31, 0.41);
		QVERIFY(v1.equals(&v2) == false);
		v2.set(0.11, 0.21, 0.31, 0.41);
		QVERIFY(v1.equals(&v2) == true);
	
		v2.setTypeName("SomeStuff");
		QVERIFY(v1.equals(&v2) == false);
	}

	{
		//StringValue
		StringValue v1("Hallo Welt");
		StringValue v2("Hallo Welt");
		
		QVERIFY(v1.equals(0) == false);
		
		QVERIFY(v1.equals(&v1) == true);
		QVERIFY(v1.equals(&v2) == true);
		QVERIFY(v2.equals(&v1) == true);
	
		v1.set("Ciao Welt");
		QVERIFY(v1.equals(&v2) == false);
	
		v2.set("Ciao Welt");
		QVERIFY(v1.equals(&v2) == true);
	
		v2.setTypeName("SomeStuff");
		QVERIFY(v1.equals(&v2) == false);
	}

	{
		//Vector3DValue
		Vector3DValue v1(0.1, 0.2, 0.3);
		Vector3DValue v2(0.1, 0.2, 0.3);
		
		QVERIFY(v1.equals(0) == false);
		
		QVERIFY(v1.equals(&v1) == true);
		QVERIFY(v1.equals(&v2) == true);
		QVERIFY(v2.equals(&v1) == true);
	
		v1.set(0.11, 0.2, 0.3);
		QVERIFY(v1.equals(&v2) == false);
		v2.set(0.11, 0.2, 0.3);
		QVERIFY(v1.equals(&v2) == true);

		v1.set(0.11, 0.21, 0.3);
		QVERIFY(v1.equals(&v2) == false);
		v2.set(0.11, 0.21, 0.3);
		QVERIFY(v1.equals(&v2) == true);

		v1.set(0.11, 0.21, 0.31);
		QVERIFY(v1.equals(&v2) == false);
		v2.set(0.11, 0.21, 0.31);
		QVERIFY(v1.equals(&v2) == true);
	
		v2.setTypeName("SomeStuff");
		QVERIFY(v1.equals(&v2) == false);
	}
	
}


void TestValue::cleanUpTestCase(){
}


//Chris
void TestValue::testRangeValue() {
	
	//constructors
	{
		RangeValue r1;
		QCOMPARE(r1.getMin(), 0.0);
		QCOMPARE(r1.getMax(), 1.0);
		QVERIFY(r1.get().isValid());
		
		RangeValue r2(-100.0, 42.5);
		QCOMPARE(r2.getMin(), -100.0);
		QCOMPARE(r2.getMax(), 42.5);
		QVERIFY(r2.get().isValid());
		
		//if min and max are exchanged, then this is corrected in constructor...
		RangeValue r3(100.2, -42.3);
		QCOMPARE(r3.getMin(), -42.3);
		QCOMPARE(r3.getMax(), 100.2);
		QVERIFY(r3.get().isValid());
		
		//Copy constructor
		RangeValue r4(r2);
		QCOMPARE(r4.getMin(), -100.0);
		QCOMPARE(r4.getMax(), 42.5);
		QVERIFY(r4.get().isValid());
		
		//equal min and max
		RangeValue r5(55.5, 55.5);
		QCOMPARE(r5.getMin(), 55.5);
		QCOMPARE(r5.getMax(), 55.5);
		QVERIFY(r5.get().isValid());
	}
	
	//set and get 
	{
		RangeValue r1;
		QCOMPARE(r1.getMin(), 0.0);
		QCOMPARE(r1.getMax(), 1.0);
		QVERIFY(r1.get().isValid());
		
		r1.set(100.5, 200.1);
		QCOMPARE(r1.getMin(), 100.5);
		QCOMPARE(r1.getMax(), 200.1);
		QVERIFY(r1.get().isValid());
		
		//fails if min > max
		r1.set(4.0, 2.1);
		QCOMPARE(r1.getMin(), 100.5);
		QCOMPARE(r1.getMax(), 200.1);
		QVERIFY(r1.get().isValid());
		
		//get as string
		QVERIFY(r1.getValueAsString() == "100.5,200.1");
	}
	
	//set from string
	{
		RangeValue r1(-55.55, 11.111);
		QCOMPARE(r1.getMin(), -55.55);
		QCOMPARE(r1.getMax(), 11.111);
		QVERIFY(r1.getValueAsString() == "-55.55,11.111");
		QVERIFY(r1.get().isValid());
		
		//set valid string with min and max
		QVERIFY(r1.setValueFromString("-1.5,60.1") == true);
		QCOMPARE(r1.getMin(), -1.5);
		QCOMPARE(r1.getMax(), 60.1);
		QVERIFY(r1.getValueAsString() == "-1.5,60.1");
		QVERIFY(r1.get().isValid());
		
		//set valid string with min and max with additional blanks
		QVERIFY(r1.setValueFromString("  -1.2   , 60.2   ") == true);
		QCOMPARE(r1.getMin(), -1.2);
		QCOMPARE(r1.getMax(), 60.2);
		QVERIFY(r1.getValueAsString() == "-1.2,60.2");
		QVERIFY(r1.get().isValid());
		
		//set valid string with min and max, min == max
		QVERIFY(r1.setValueFromString("66.6,66.6") == true);
		QCOMPARE(r1.getMin(), 66.6);
		QCOMPARE(r1.getMax(), 66.6);
		QVERIFY(r1.getValueAsString() == "66.6");
		QVERIFY(r1.get().isValid());
		
		//set valid string with a single value (min == max)
		QVERIFY(r1.setValueFromString("123.456") == true);
		QCOMPARE(r1.getMin(), 123.456);
		QCOMPARE(r1.getMax(), 123.456);
		QVERIFY(r1.getValueAsString() == "123.456");
		QVERIFY(r1.get().isValid());
		
		//set to valid value
		QVERIFY(r1.setValueFromString("-1.23,4.56") == true);
		QCOMPARE(r1.getMin(), -1.23);
		QCOMPARE(r1.getMax(), 4.56);
		
		//fails: min > max
		QVERIFY(r1.setValueFromString("1000,-1000") == false);
		QCOMPARE(r1.getMin(), -1.23);
		QCOMPARE(r1.getMax(), 4.56);
		QVERIFY(r1.get().isValid());
		
		//fails: non-numerical characters in string.
		QVERIFY(r1.setValueFromString("[50.0,200.0]") == false);
		QVERIFY(r1.setValueFromString("eins") == false);
		QVERIFY(r1.setValueFromString("6.0-7.0") == false);
		QVERIFY(r1.setValueFromString("!5.0,$1") == false);
		QCOMPARE(r1.getMin(), -1.23);
		QCOMPARE(r1.getMax(), 4.56);
		QVERIFY(r1.get().isValid());
		
		//fails: too many items
		QVERIFY(r1.setValueFromString("[123.456,200.0,300.0]") == false);
		QCOMPARE(r1.getMin(), -1.23);
		QCOMPARE(r1.getMax(), 4.56);
		QVERIFY(r1.get().isValid());
		
		//fails: too few items
		QVERIFY(r1.setValueFromString("") == false);
		QCOMPARE(r1.getMin(), -1.23);
		QCOMPARE(r1.getMax(), 4.56);
		QVERIFY(r1.get().isValid());
		
	}
}
	
	
void TestValue::testCodeValue() {
	CodeValue v1;
	
	//set and get (and handling of newline characters)
	QVERIFY(v1.get() == "");
	v1.set("Hallo Welt\nThis is a\nmulti-line string");
	QVERIFY(v1.get() == "Hallo Welt\nThis is a\nmulti-line string");
	QVERIFY(v1.getValueAsString() == "Hallo Welt/**/This is a/**/multi-line string");
	
	QVERIFY(v1.setValueFromString("Here/**/Another Twoliner") == true);
	QVERIFY(v1.getValueAsString() == "Here/**/Another Twoliner");
	QVERIFY(v1.get() == "Here\nAnother Twoliner");
	
	//constructor with string initializer
	CodeValue v2("This is a code fragment\nWith newline characters!");
	QVERIFY(v2.get() == "This is a code fragment\nWith newline characters!");
	QVERIFY(v2.getValueAsString() == "This is a code fragment/**/With newline characters!");
	
	
	//copy constructor
	CodeValue v3(v1);
	QVERIFY(v3.getValueAsString() == "Here/**/Another Twoliner");
	QVERIFY(v3.get() == "Here\nAnother Twoliner");
}


void TestValue::testFileNameValue() {
	
	FileNameValue v1;
	
	QVERIFY(v1.get() == "");
	v1.set("/home/test/hallo.txt");
	QVERIFY(v1.get() == "/home/test/hallo.txt");
	QVERIFY(v1.getValueAsString() == "/home/test/hallo.txt");
	
	QVERIFY(v1.setValueFromString("/file.net") == true);
	QVERIFY(v1.getValueAsString() == "/file.net");
	QVERIFY(v1.get() == "/file.net");
	
	//constructor with string initializer
	FileNameValue v2("C:\\data.txt");
	Core::log("Got: " + v2.getValueAsString(), true);
	Core::log("Got: " + v2.get(), true);
	QVERIFY(v2.get() == "C:\\data.txt");
	QVERIFY(v2.getValueAsString() == "C:\\data.txt");
	
	
	//copy constructor
	FileNameValue v3(v1);
	QVERIFY(v3.getValueAsString() == "/file.net");
	QVERIFY(v3.get() == "/file.net");
}
	
	
	
