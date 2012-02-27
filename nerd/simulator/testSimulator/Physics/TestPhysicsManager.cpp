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



#include "TestPhysicsManager.h"
#include "Physics/PhysicsManager.h"
#include "Core/Core.h"
#include "Physics/SimBodyAdapter.h"
#include "Physics/SimObjectGroupAdapter.h"
#include "Physics/SimObjectAdapter.h"

using namespace nerd;

void TestPhysicsManager::initTestCase(){
}

void TestPhysicsManager::cleanUpTestCase(){
}


void TestPhysicsManager::testConstructor() {
	Core::resetCore();
	
	PhysicsManager pm;
	QVERIFY(pm.getPhysicalSimulationAlgorithm() == 0);
	QVERIFY(pm.getName().compare("PhysicsManager") == 0);
	
}


//Chris
void TestPhysicsManager::testAddAndRemoveBodyAndJointPrototypes() {
	Core::resetCore();
	
	//Standard constructor.
	PhysicsManager pm;
	
	SimBody *body1 = new SimBody("Body1");
	SimBody *body2 = new SimBody("Body2");
	SimJoint *joint1 = new SimJoint("Joint1");

	QVERIFY(pm.getPrototypeNames().empty());
	QVERIFY(pm.getPrototypes().empty());

	QVERIFY(pm.getPrototype("Type1") == 0);
	QVERIFY(pm.getBodyPrototype("Type1") == 0);

	//add a null pointer body
	QVERIFY(pm.addPrototype("Type1", 0) == false);

	//****** Body Prototype ****

	//add Body1 
	QVERIFY(pm.addPrototype("Type1", body1));
	QVERIFY(pm.getPrototypeNames().size() == 1);
	QVERIFY(pm.getPrototypeNames().contains("Type1") == true);
	QVERIFY(pm.getBodyPrototype("Type1") == body1);
	QVERIFY(pm.getPrototype("Type1") == body1);
	QVERIFY(pm.getJointPrototype("Type1") == 0); //prototype has a different type.

	//add Body1 with different name
	QVERIFY(pm.addPrototype("Type2", body1));
	QVERIFY(pm.getPrototypeNames().size() == 2);
	QVERIFY(pm.getPrototypeNames().contains("Type2") == true);
	QVERIFY(pm.getPrototype("Type2") == body1);
	QVERIFY(pm.getBodyPrototype("Type2") == body1);

	//add Body2 with an existing name (fails)
	QVERIFY(pm.addPrototype("Type1", body2) == false);
	QVERIFY(pm.getPrototypeNames().size() == 2);
	QVERIFY(pm.getPrototypeNames().contains("Type1") == true);
	QVERIFY(pm.getPrototype("Type1") == body1);

	//add Body2 with a new name
	QVERIFY(pm.addPrototype("Type3", body2));
	QVERIFY(pm.getPrototypeNames().size() == 3);
	QVERIFY(pm.getPrototypeNames().contains("Type3") == true);
	QVERIFY(pm.getPrototype("Type3") == body2);

	
	//****** Joint Prototype ****

	//add Joint1
	QVERIFY(pm.addPrototype("JType1", joint1));
	QVERIFY(pm.getPrototypeNames().size() == 4);
	QVERIFY(pm.getPrototypeNames().contains("JType1") == true);
	QVERIFY(pm.getPrototype("JType1") == joint1);
	QVERIFY(pm.getJointPrototype("JType1") == joint1);
	QVERIFY(pm.getBodyPrototype("JType1") == 0); //wrong type.

	
	//Add simbodyAdapter for destruction test.
	bool destroyFlag = false;
	SimBodyAdapter *bodyAdapter = new SimBodyAdapter("Body1", &destroyFlag);
	QVERIFY(pm.addPrototype("Adapter", bodyAdapter));
	QVERIFY(pm.getPrototypeNames().size() == 5);
	
	//Remove prototypes (prototypes are destroyed by PhysicsManager.
	pm.destroyPrototypes();
	QVERIFY(pm.getPrototypeNames().size() == 0);
	QVERIFY(destroyFlag == true); //Adapter was destroyed.
	
}

//Chris
void TestPhysicsManager::testAddAndRemoveObjects() {
	Core::resetCore();
	
	PhysicsManager pm;

	ValueManager *vm = Core::getInstance()->getValueManager();
	
	SimObject *obj1 = new SimObject("Object1");
	SimBody *body1 = new SimBody("Body1");
	SimJoint *joint1 = new SimJoint("/Joint1"); //also works with a starting slash (naming)
	bool destroyFlag = false;
	SimBodyAdapter *bodyAdapter = new SimBodyAdapter("Body2", &destroyFlag);

	IntValue *param1 = new IntValue(123);
	obj1->addParameter("Param1", param1);
	IntValue *param2 = new IntValue(234);
	body1->addParameter("Param2", param2);
	IntValue *param3 = new IntValue(345);
	joint1->addParameter("Param3", param3);

	QVERIFY(vm->getIntValue("/Sim/Object1/Param1") == 0);
	QVERIFY(vm->getIntValue("/Sim/Body1/Param2") == 0);
	QVERIFY(vm->getIntValue("/Sim/Joint1/Param3") == 0);
	
	QVERIFY(pm.getSimObjects().size() == 0);
	QVERIFY(pm.getSimBodies().size() == 0);
	QVERIFY(pm.getSimJoints().size() == 0);
	
	//add NULL as SimObject (fails)
	QVERIFY(pm.addSimObject(0) == false);
	QVERIFY(pm.getSimObjects().size() == 0);
	
	QVERIFY(pm.addSimObject(obj1) == true);
	QVERIFY(pm.getSimObjects().size() == 1);
	QVERIFY(pm.getSimBodies().size() == 0);
	QVERIFY(pm.getSimJoints().size() == 0);
	QVERIFY(pm.getSimObjects().contains(obj1));

	//was automatically registered ad ValueManager
	QVERIFY(vm->getIntValue("/Sim/Object1/Param1") == param1);
	QVERIFY(vm->getIntValue("/Sim/Body1/Param2") == 0);
	QVERIFY(vm->getIntValue("/Sim/Joint1/Param3") == 0);
	
	//add bodyAdapter
	QVERIFY(pm.addSimObject(bodyAdapter) == true);
	QVERIFY(pm.getSimObjects().size() == 2);
	QVERIFY(pm.getSimBodies().size() == 1); //automatically added as SimBody.
	QVERIFY(pm.getSimJoints().size() == 0);
	QVERIFY(pm.getSimObjects().contains(bodyAdapter));

	QVERIFY(vm->getIntValue("/Sim/Object1/Param1") == param1);
	QVERIFY(vm->getIntValue("/Sim/Body1/Param2") == 0);
	QVERIFY(vm->getIntValue("/Sim/Joint1/Param3") == 0);
	
	//add bodyAdapter a second time (fails)
	QVERIFY(pm.addSimObject(bodyAdapter) == false);
	QVERIFY(pm.getSimObjects().size() == 2);
	
	//add body1 as SimObject
	QVERIFY(pm.addSimObject(body1) == true);
	QVERIFY(pm.getSimObjects().size() == 3);
	QVERIFY(pm.getSimBodies().size() == 2); 
	QVERIFY(pm.getSimJoints().size() == 0);
	QVERIFY(pm.getSimBody("Body1") == body1);
	QVERIFY(pm.getSimObjects().contains(body1));

	QVERIFY(vm->getIntValue("/Sim/Object1/Param1") == param1);
	QVERIFY(vm->getIntValue("/Sim/Body1/Param2") == param2); //body1 parameters are added
	QVERIFY(vm->getIntValue("/Sim/Joint1/Param3") == 0);
	
	//add joint 
	QVERIFY(pm.addSimObject(joint1) == true);
	QVERIFY(pm.getSimObjects().size() == 4);
	QVERIFY(pm.getSimBodies().size() == 2); 
	QVERIFY(pm.getSimJoints().size() == 1);
	QVERIFY(pm.getSimJoint("Joint1") == joint1);
	QVERIFY(pm.getSimObjects().contains(joint1));

	QVERIFY(vm->getIntValue("/Sim/Object1/Param1") == param1);
	QVERIFY(vm->getIntValue("/Sim/Body1/Param2") == param2);
	QVERIFY(vm->getIntValue("/Sim/Joint1/Param3") == param3);
	
	pm.destroySimObjects();
	QVERIFY(pm.getSimObjects().size() == 0);
	QVERIFY(pm.getSimBodies().size() == 0); 
	QVERIFY(pm.getSimJoints().size() == 0);
	
	QVERIFY(vm->getIntValue("/Sim/Object1/Param1") == 0);
	QVERIFY(vm->getIntValue("/Sim/Body1/Param2") == 0);
	QVERIFY(vm->getIntValue("/Sim/Joint1/Param3") == 0);

	QVERIFY(destroyFlag == true); //bodyAdapter was destroyed.


	//add SimObject with children
	SimObjectAdapter *so1 = new SimObjectAdapter("Parent1", "Test");
	SimObjectAdapter *child1 = new SimObjectAdapter("Child1", "Test2");
	SimObjectAdapter *child2 = new SimObjectAdapter("Child2", "Test3");
	so1->addChildObject(child1);
	so1->addChildObject(child2);
	QCOMPARE(so1->getChildObjects().size(), 2);
	QCOMPARE(pm.getSimObjects().size(), 0);

	QVERIFY(vm->getStringValue("/Sim/Parent1/Name") == 0);
	QVERIFY(vm->getStringValue("/Sim/Child1/Name") == 0);
	QVERIFY(vm->getStringValue("/Sim/Child2/Name") == 0);
	
	pm.addSimObject(so1);
	QVERIFY(vm->getStringValue("/Sim/Parent1/Name") != 0);
	QVERIFY(vm->getStringValue("/Sim/Child1/Name") != 0);
	QVERIFY(vm->getStringValue("/Sim/Child2/Name") != 0);
	QCOMPARE(pm.getSimObjects().size(), 3);


}



//Chris
void TestPhysicsManager::testAddAndRemoveSimObjectGroups() {
	Core::resetCore();

	ValueManager *vm = Core::getInstance()->getValueManager();
	
	PhysicsManager pm;
	bool destroy1 = false;
	bool destroy2 = false;
	bool destroyBody1 = false;
	
	SimObjectGroupAdapter *g1 = new SimObjectGroupAdapter("Group1", "Type1", &destroy1);
	SimObjectGroupAdapter *g2 = new SimObjectGroupAdapter("Group2", "Type2", &destroy2);
	
	SimBodyAdapter *body1 = new SimBodyAdapter("Body1", &destroyBody1);
	SimBodyAdapter *body2 = new SimBodyAdapter("/Body2"); //the / does not make a differnce in ValueManager.
	g1->addObject(body1);
	g2->addObject(body2);

	IntValue *param1 = new IntValue(456);
	body1->addParameter("TestParam1", param1);
	IntValue *param2 = new IntValue(156);
	body2->addParameter("TestParam2", param2);

	QVERIFY(vm->getIntValue("/Sim/Group1/Body1/TestParam1") == 0);
	QVERIFY(vm->getIntValue("/Sim/Group2/Body2/TestParam2") == 0);
	
	//add NULL as object group
	QVERIFY(pm.addSimObjectGroup(0) == false);
	QCOMPARE(pm.getSimObjectGroups().size(), 0);
	
	//add group g1
	QVERIFY(pm.addSimObjectGroup(g1) == true);
	QCOMPARE(pm.getSimObjectGroups().size(), 1);
	QVERIFY(pm.getSimObjectGroups().contains(g1));
	
	//SimObject in g1 was added to the SimObject pool.
	QCOMPARE(pm.getSimObjects().size(), 1);
	QVERIFY(pm.getSimObjects().at(0) == body1);

	//SimObject parameters were automatically added to ValueManager.
	QVERIFY(vm->getIntValue("/Sim/Group1/Body1/TestParam1") == param1);
	
	//add group g1 again (fails)
	QVERIFY(pm.addSimObjectGroup(g1) == false);
	QCOMPARE(pm.getSimObjectGroups().size(), 1);
	
	//add group g2
	QVERIFY(pm.addSimObjectGroup(g2) == true);
	QCOMPARE(pm.getSimObjectGroups().size(), 2);

	//SimObject parameters were automatically added to ValueManager.
	//note that the path does not contain duplicated slashes.
	QVERIFY(vm->getIntValue("/Sim/Group2/Body2/TestParam2") == param2);

	//SimObject in group is not removed.
	QCOMPARE(pm.getSimObjects().size(), 2);
	
	//remove NULL (fails)
	QVERIFY(pm.removeSimObjectGroup(0) == false);
	QCOMPARE(pm.getSimObjectGroups().size(), 2);
	
	//remove g2
	QVERIFY(pm.removeSimObjectGroup(g2) == true);
	QCOMPARE(pm.getSimObjectGroups().size(), 1);
	QVERIFY(pm.getSimObjectGroups().contains(g1));
	
	//remove g2 again (fails)
	QVERIFY(pm.removeSimObjectGroup(g2) == false);
	QCOMPARE(pm.getSimObjectGroups().size(), 1);
	
	//destroy all object groups
	pm.destroySimObjectGroups();
	
	QVERIFY(pm.getSimObjectGroups().size() == 0);
	QVERIFY(destroy1 == true);
	QVERIFY(destroy2 == false); // g2 was not in PhysicsManager.
	QVERIFY(destroyBody1 == false); //SimObject was not automatically destroyed with the group!
	
	delete g2;
	
}


void TestPhysicsManager::testParameterLookup() {
	//TODO
}






