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



#include "TestValueManager.h"
#include "Core/Core.h"
#include "Value/Value.h"
#include "Value/IntValue.h"
#include "Value/StringValue.h"
#include "Value/BoolValue.h"
#include "Value/DoubleValue.h"
#include "Value/ValueManager.h"
#include "Event/EventManager.h"
#include <QString>
#include "MyRepositoryChangedListener.h"
#include <iostream>
#include "Value/StringValue.h"
#include "Value/DoubleValue.h"
#include "NerdConstants.h"
#include "Core/Core.h"
#include "Value/ColorValue.h"
#include "Value/InterfaceValue.h"
#include "Value/NormalizedDoubleValue.h"
#include "Value/QuaternionValue.h"
#include "Value/Vector3DValue.h"
#include "Value/MyChangedListener.h"
#include "Value/MyRepositoryChangedListener.h"

using namespace std;
using namespace nerd;


//chris
void TestValueManager::testConstructor() {
	Core::resetCore();
	
	EventManager *em = new EventManager();
	ValueManager *vm = new ValueManager();

	//test available prototypes
	QList<Value*> prototypes = vm->getPrototypes();

	QCOMPARE(prototypes.size(), 11);
	QList<QString> valueNames;
	for(int i = 0; i < prototypes.size(); ++i) {
		valueNames.append(prototypes.at(i)->getTypeName());
	}

	QVERIFY(valueNames.contains("Double"));
	QVERIFY(valueNames.contains("Int"));
	QVERIFY(valueNames.contains("Bool"));
	QVERIFY(valueNames.contains("String"));
	QVERIFY(valueNames.contains("NormalizedDouble"));
	QVERIFY(valueNames.contains("Interface"));
	QVERIFY(valueNames.contains("Vector3D"));
	QVERIFY(valueNames.contains("Color"));
	QVERIFY(valueNames.contains("Quaternion"));
	QVERIFY(valueNames.contains("Vector3D"));
	QVERIFY(valueNames.contains("ULongLong"));

	//test types of copied prototypes
	QVERIFY(vm->createCopyOfPrototype("NotAvailable") == 0);

	DoubleValue *doubleCopy = dynamic_cast<DoubleValue*>(vm->createCopyOfPrototype("Double"));
	QVERIFY(doubleCopy != 0);
	QVERIFY(prototypes.contains(doubleCopy) == false);
	delete doubleCopy;

	IntValue *intCopy = dynamic_cast<IntValue*>(vm->createCopyOfPrototype("Int"));
	QVERIFY(intCopy != 0);
	QVERIFY(prototypes.contains(intCopy) == false);
	delete intCopy;

	BoolValue *boolCopy = dynamic_cast<BoolValue*>(vm->createCopyOfPrototype("Bool"));
	QVERIFY(boolCopy != 0);
	QVERIFY(prototypes.contains(boolCopy) == false);
	delete boolCopy;

	StringValue *stringCopy = dynamic_cast<StringValue*>(vm->createCopyOfPrototype("String"));
	QVERIFY(stringCopy != 0);
	QVERIFY(prototypes.contains(stringCopy) == false);
	delete stringCopy;

	NormalizedDoubleValue *normCopy =
			dynamic_cast<NormalizedDoubleValue*>(vm->createCopyOfPrototype("NormalizedDouble"));
	QVERIFY(normCopy != 0);
	QVERIFY(prototypes.contains(normCopy) == false);
	delete normCopy;

	InterfaceValue *interfaceCopy =
 			dynamic_cast<InterfaceValue*>(vm->createCopyOfPrototype("Interface"));
	QVERIFY(interfaceCopy != 0);
	QVERIFY(prototypes.contains(interfaceCopy) == false);
	delete interfaceCopy;

	Vector3DValue *v3dCopy = dynamic_cast<Vector3DValue*>(vm->createCopyOfPrototype("Vector3D"));
	QVERIFY(v3dCopy != 0);
	QVERIFY(prototypes.contains(v3dCopy) == false);
	delete v3dCopy;

	ColorValue *colorCopy = dynamic_cast<ColorValue*>(vm->createCopyOfPrototype("Color"));
	QVERIFY(colorCopy != 0);
	QVERIFY(prototypes.contains(colorCopy) == false);
	delete colorCopy;

	QuaternionValue *quaternionCopy =
			dynamic_cast<QuaternionValue*>(vm->createCopyOfPrototype("Quaternion"));
	QVERIFY(quaternionCopy != 0);
	QVERIFY(prototypes.contains(quaternionCopy) == false);
	delete quaternionCopy;

	delete em;
	delete vm;
}

//verena
void TestValueManager::testAddValue(){
	Core::resetCore();
	ValueManager *manager = Core::getInstance()->getValueManager();
	

	IntValue *iVal = new IntValue(100);
	IntValue *iVal2 = new IntValue(199);
	StringValue *sVal = new StringValue("Test");
	DoubleValue *dVal = new DoubleValue(11.11);
	BoolValue *bVal = new BoolValue(false);

	QVERIFY(manager->addValue("Int/TestValue", iVal));
	QVERIFY(!manager->addValue("Int/TestValue", iVal));
	QVERIFY(!manager->addValue("Int/TestValue", iVal2));
	QVERIFY(manager->addValue("Int/HighTestValue", iVal));
	QVERIFY(manager->addValue("String/TestValue", sVal));
	QVERIFY(manager->addValue("Bool/TestValue", bVal));
	QVERIFY(!manager->addValue("String/TestValue", dVal));
	QVERIFY(manager->addValue("Double/TestValue", dVal));
	
	QVERIFY(manager->nameExists(QString("Int/TestValue")));
	QVERIFY(!manager->nameExists(QString("Int/Testvalue")));
	
	//TODO test save and load values!
	manager->saveValues("testSaveValues.val", manager->getValueNamesMatchingPattern(".*"), 
				"This is my comment.\n Keep in Mind this is a\r\n Multiline comment\n");
	manager->loadValues("testSaveValues.val");

	delete iVal2;


}


//verena
void TestValueManager::testFindValue(){
	Core::resetCore();
	EventManager *em = new EventManager();
	ValueManager *manager = new ValueManager();
	
	IntValue *iVal = new IntValue(100);
	Value *testVal = new Value();
	IntValue *iVal2 = new IntValue(199);
	StringValue *sVal = new StringValue("Test");
	DoubleValue *dVal = new DoubleValue(11.11);
	BoolValue *bVal = new BoolValue(false);

	QVERIFY(manager->addValue("Int/Val", iVal));
	QVERIFY(manager->addValue("Bool/Val", bVal));
	QVERIFY(manager->addValue("Double/Val", dVal));
	QVERIFY(manager->addValue("String/Val", sVal));
	QVERIFY(manager->addValue("Int/Val2", iVal2));
	QVERIFY(manager->addValue("Test/Val", testVal));

	QCOMPARE(manager->getIntValue("Int/Val"), iVal);
	QCOMPARE(manager->getDoubleValue("Double/Val"), dVal);
	QCOMPARE(manager->getStringValue("String/Val"), sVal);
	QVERIFY(manager->getBoolValue("Int/Val2") == 0);
	QVERIFY(manager->getStringValue("Double/Val") == 0);
	
	IntValue *grep = manager->getIntValue("Test/Val");
	QVERIFY(grep == 0);

	QList<Value*> values = manager->getValuesMatchingPattern(".*/Val");
	QVERIFY(values.size() == 5);
	values.clear();

	manager->addValue("Test2/Val", testVal);
	manager->addValue("Test3/Val", testVal);
	values = manager->getValuesMatchingPattern(".*/Val");
	QVERIFY(values.size() == 5);
	values.clear();

	QList<QString> valueNames = manager->getValueNamesMatchingPattern("Tes.*");
	QVERIFY(valueNames.contains("Test/Val"));
	QVERIFY(valueNames.contains("Test2/Val"));
	QCOMPARE(valueNames.size(), 3);

	values = manager->getValuesMatchingPattern(".*t/Val");
	QVERIFY(values.size() == 2);

	manager->addValue("Int/Val_1", iVal);
	manager->addValue("Int/Val_2", iVal);
	manager->addValue("Int/Val_3", iVal);
	QList<QString> names = manager->getNamesOfValue(iVal);
	QCOMPARE(names.at(0), QString("Int/Val"));
	QCOMPARE(names.at(2), QString("Int/Val_2"));
	QVERIFY(manager->removeValue("Bool/Val"));
	QVERIFY(!manager->removeValue("Bool/Val"));

	QVERIFY(manager->getBoolValue("Bool/Val") == 0);
	
	manager->removeAllValues();
	QVERIFY(manager->getValues().size() == 0);

	manager->addValue("Int/Val_1", iVal);
	manager->addValue("Int/Val_2", dVal);
	manager->addValue("Int/Val_3", bVal);
	manager->addValue("Int/Val_4", bVal);
	QVERIFY(manager->getValues().size() == 4);
	
	QList<Value*> valuesToRemove;
	valuesToRemove.push_back(bVal);
	valuesToRemove.push_back(iVal);
	
	manager->removeValues(valuesToRemove);
	QVERIFY(manager->getValues().size() == 1);
	
	QCOMPARE(manager->getValues().at(0), dVal);

	
	delete bVal;
	delete iVal;
	delete iVal2;
	delete sVal;
	delete testVal;
	delete manager;
	delete em;
}


//verena
void TestValueManager::testNotification(){
	Core::resetCore();
	
	ValueManager *manager = Core::getInstance()->getValueManager();
	MyRepositoryChangedListener listener;
	manager->notifyRepositoryChangedListeners();
	QCOMPARE(listener.getCount(), 1);
	listener.deregister();
	manager->notifyRepositoryChangedListeners();
	QCOMPARE(listener.getCount(), 1);
	

}


//verena
void TestValueManager::testNotificationStack() {
	Core::resetCore();
	
	EventManager eManager;
	ValueManager manager;
	IntValue iVal1(10);
	IntValue iVal2(20);
	manager.pushToNotificationStack(&iVal1);

	QVERIFY(manager.getNotificationStack().size() == 1);
	IntValue *stackObject = dynamic_cast<IntValue*>(manager.getNotificationStack().at(0));
	QVERIFY(stackObject == &iVal1);

	manager.pushToNotificationStack(&iVal2);
	manager.pushToNotificationStack(&iVal2);
	QCOMPARE(manager.getNotificationStack().size(), 3);
	manager.popFromNotificationStack();
	IntValue *firstStackObject =
 				dynamic_cast<IntValue*>(manager.getNotificationStack()
								.at(manager.getNotificationStack().size()-1));
	QVERIFY(firstStackObject == &iVal2);
	QVERIFY(firstStackObject != &iVal1);

}


//Verena
void TestValueManager::testPrototyping() {
	Core::resetCore();
	
	ValueManager *vManager = Core::getInstance()->getValueManager();
	QCOMPARE(vManager->getPrototypes().size(), 11);
	Value *value = vManager->createCopyOfPrototype("Bool");
	QVERIFY(value != 0);
	QVERIFY(dynamic_cast<BoolValue*>(value) != 0);
	delete value;
	value = 0;
	
	value = vManager->createCopyOfPrototype("Int");
	QVERIFY(value != 0);
	QVERIFY(dynamic_cast<IntValue*>(value) != 0);
	delete value;
	value = 0;

	value = vManager->createCopyOfPrototype("Double");
	QVERIFY(value != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(value) != 0);
	delete value;
	value = 0;

	value = vManager->createCopyOfPrototype("String");
	QVERIFY(value != 0);
	QVERIFY(dynamic_cast<StringValue*>(value) != 0);
	delete value;
	value = 0;

	value = vManager->createCopyOfPrototype("Quaternion");
	QVERIFY(value != 0);
	QVERIFY(dynamic_cast<QuaternionValue*>(value) != 0);
	delete value;
	value = 0;

	value = vManager->createCopyOfPrototype("Vector3D");
	QVERIFY(value != 0);
	QVERIFY(dynamic_cast<Vector3DValue*>(value) != 0);
	delete value;
	value = 0;

	value = vManager->createCopyOfPrototype("Color");
	QVERIFY(value != 0);
	QVERIFY(dynamic_cast<ColorValue*>(value) != 0);
	delete value;
	value = 0;

	value = vManager->createCopyOfPrototype("Interface");
	QVERIFY(value != 0);
	QVERIFY(dynamic_cast<InterfaceValue*>(value) != 0);
	delete value;
	value = 0;

	value = vManager->createCopyOfPrototype("NormalizedDouble");
	QVERIFY(value != 0);
	QVERIFY(dynamic_cast<NormalizedDoubleValue*>(value) != 0);	
	delete value;
	value = 0;

	DoubleValue *testPrototype = new DoubleValue(2.3);
	
	testPrototype->setTypeName("Double");
	QVERIFY(!vManager->addPrototype(testPrototype));
	testPrototype->setTypeName("TestDouble");
	QVERIFY(vManager->addPrototype(testPrototype));
	QVERIFY(!vManager->addPrototype(testPrototype));

	value = vManager->createCopyOfPrototype("TestDouble");
	QVERIFY(value != 0);
	QVERIFY(dynamic_cast<DoubleValue*>(value) != 0);	
	QCOMPARE(dynamic_cast<DoubleValue*>(value)->get(), 2.3);
	delete value;
	value = 0;
}


//Chris
void TestValueManager::testRemoveValuesByList() {
	Core::resetCore();

	ValueManager *vm = Core::getInstance()->getValueManager();

	int defaultNumberOfValues = 7;
	QCOMPARE(vm->getValues().size(), defaultNumberOfValues);

	Value *v1 = new Value();
	Value *v2 = new Value();
	Value *v3 = new Value();

	MyChangedListener *l1 = new MyChangedListener();
	MyChangedListener *l2 = new MyChangedListener();
	MyChangedListener *l3 = new MyChangedListener();

	QVERIFY(v1->addValueChangedListener(l1));
	QVERIFY(v1->addValueChangedListener(l2));
	QVERIFY(v2->addValueChangedListener(l3));

	QVERIFY(vm->addValue("Value1", v1) == true);
	QVERIFY(vm->addValue("Value1Again", v1) == true);
	QVERIFY(vm->addValue("Value2", v2) == true);
	QVERIFY(vm->addValue("Value3", v3) == true);

	QCOMPARE(v1->getValueChangedListeners().size(), 2);
	QCOMPARE(v2->getValueChangedListeners().size(), 1);
	QCOMPARE(vm->getValues().size(), defaultNumberOfValues + 4);
	QVERIFY(vm->getValue("Value1") == v1);
	QVERIFY(vm->getValue("Value2") == v2);
	QVERIFY(vm->getValue("Value1Again") == v1);

	QList<Value*> valuesToRemove;
	valuesToRemove.append(v1);
	valuesToRemove.append(v2);
	valuesToRemove.append(v3);

	QVERIFY(vm->removeValues(valuesToRemove));
	
	//values are removed and all listeners are also removed.
	QCOMPARE(v1->getValueChangedListeners().size(), 0);
	QCOMPARE(v2->getValueChangedListeners().size(), 0);
	QCOMPARE(vm->getValues().size(), defaultNumberOfValues);
	QVERIFY(vm->getValue("Value1") == 0);
	QVERIFY(vm->getValue("Value2") == 0);
	QVERIFY(vm->getValue("Value1Again") == 0);

	delete v1;
	delete v2;
	delete v3;
	delete l1;
	delete l2;
	delete l3;
}

//Chris
void TestValueManager::testGetMultiPartValue() {
	Core::resetCore();

	ValueManager *vm = Core::getInstance()->getValueManager();

	Vector3DValue *v3d = new Vector3DValue(1.0, 2.0, 3.0);
	DoubleValue *d = new DoubleValue(123.4);

	QVERIFY(vm->addValue("/TestValues/Vector3DValue/Value", v3d));
	QVERIFY(vm->addValue("/TestValues/DoubleValue/Value", d));

	QVERIFY(vm->getMultiPartValue("/TestValue/WrongName") == 0);
	QVERIFY(vm->getMultiPartValue("/TestValue/WrongName:y") == 0);
	QVERIFY(vm->getMultiPartValue("/TestValue/WrongName", "y") == 0);

	QVERIFY(vm->getMultiPartValue("/TestValues/Vector3DValue/Value") == v3d);
	QVERIFY(vm->getMultiPartValue("/TestValues/Vector3DValue/Value:y") != 0);
	QVERIFY(vm->getMultiPartValue("/TestValues/Vector3DValue/Value:y") == v3d->getValuePart(1));
	QVERIFY(vm->getMultiPartValue("/TestValues/Vector3DValue/Value:z") == v3d->getValuePart(2));
	QVERIFY(vm->getMultiPartValue("/TestValues/Vector3DValue/Value", "y") == v3d->getValuePart(1));
	QVERIFY(vm->getMultiPartValue("/TestValues/Vector3DValue/Value", "x") == v3d->getValuePart(0));
	QVERIFY(vm->getMultiPartValue("/TestValues/Vector3DValue/Value", "a") == 0); //nonexistent part
	QVERIFY(vm->getMultiPartValue("/TestValues/Vector3DValue/Value:max") == 0); //nonexistent part

	QVERIFY(vm->getMultiPartValue("/TestValues/DoubleValue/Value") == d);
	QVERIFY(vm->getMultiPartValue("/TestValues/DoubleValue/Value:y") == 0);
	QVERIFY(vm->getMultiPartValue("/TestValues/DoubleValue/Value", "x") == 0);
}
