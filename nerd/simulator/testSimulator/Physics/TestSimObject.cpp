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



#include "TestSimObject.h"
#include "Physics/SimObject.h"
#include "Value/StringValue.h"
#include "Value/ValueManager.h"
#include "Physics/SimObjectAdapter.h"
#include "Physics/SimGeomAdapter.h"
#include "Core/Core.h"
#include "Value/StringValue.h"
#include "Physics/SimObjectGroupAdapter.h"
#include <iostream>

using namespace std;


using namespace nerd;

//chris
void TestSimObject::testConstructor() {
	
	Core::resetCore();
	
	SimObject so0("SimObject0");
	QVERIFY(so0.getPrefix().compare("") == 0);
	
	//extended constructor
	SimObject *so1 = new SimObject("SimObject1", "PrefixX");
	
	QVERIFY(so1->getName() == "SimObject1");
	QVERIFY(so1->getAbsoluteName() == "/SimObject1");
	QVERIFY(so1->getGeometries().size() == 0);
	QVERIFY(so1->getPrefix() == "PrefixX");
	QVERIFY(so1->getObjectGroup() == 0);

	SimObjectGroupAdapter *sga = new SimObjectGroupAdapter("Group1", "Agent");
	so1->setObjectGroup(sga);
	QVERIFY(so1->getObjectGroup() == sga);
	QVERIFY(so1->getAbsoluteName() == "/Group1/SimObject1");
	
	QVERIFY(so1->getInputValues().size() == 0);
	QVERIFY(so1->getOutputValues().size() == 0);
	QVERIFY(so1->getInfoValues().size() == 0);
	
	bool destroyFlag = false;
	SimGeomAdapter *sga1 = new SimGeomAdapter(so1, &destroyFlag);
	
	//add geometry
	QVERIFY(so1->addGeometry(sga1) == true);
	QVERIFY(so1->getGeometries().size() == 1);
	QVERIFY(so1->getGeometries().at(0) == sga1);
	
	
	//copy constructor
	SimObject so2(*so1);
	QVERIFY(so2.getName() == "SimObject1");
	QVERIFY(so2.getPrefix() == "PrefixX");
	QVERIFY(so2.getGeometries().size() == 0);

	QVERIFY(so2.getObjectGroup() == 0); //owner is not copied (because it is a back reference)
	QVERIFY(so2.getAbsoluteName() == "/SimObject1");

	//the copied gemoetry is a newly created geometry.
// 	QVERIFY(so2.getGeometries().at(0) != sga1);
	
	
	
	delete so1;
	//SimGeoms are destroyed.
	
	//change name by parameter
	StringValue *name = dynamic_cast<StringValue*>(so0.getParameter("Name"));
	QVERIFY(name != 0);
	name->set("NewName");
	QVERIFY(so0.getName() == "NewName");
	
	so0.setName("AnotherName");
	QVERIFY(name->get() == "AnotherName");
	
	
	QVERIFY(destroyFlag == false); //has to be destroyed by its o(subclass-)owner manually
	delete sga1;
}


//Chris
void TestSimObject::testInputOutputInfoValues() {
	
	Core::resetCore();
	
	SimObjectAdapter so("Object1", "/SimObject/One/");
	
	QCOMPARE(so.getName(), QString("Object1"));
	InterfaceValue v1("", "V1");
	InterfaceValue v2("", "V2");
	InterfaceValue v3("", "V3");
	
	QCOMPARE(so.getInputValues().size(), 0);
	QCOMPARE(so.getOutputValues().size() , 0);
	QCOMPARE(so.getInfoValues().size(), 0);
	
	QList<InterfaceValue*> values;
	values.append(&v1);
	
	so.setInputValues(values);
	QCOMPARE(so.getInputValues().size(), 1);
	QCOMPARE(so.getOutputValues().size(), 0);
	QCOMPARE(so.getInfoValues().size(), 0);
	QCOMPARE(so.getInputValues().at(0)->getPrefix(), QString("Object1"));
	QCOMPARE(so.getInputValues().at(0)->getInterfaceName(), QString("V1"));
	
	QCOMPARE(so.getInputValues().at(0)->getName(), QString("Object1/V1"));
	
	so.setName("Test");
	QCOMPARE(so.getInputValues().at(0)->getName(), QString("Test/V1"));
	so.setName("Object1");
	
	values.clear();
	values.append(&v2);
	values.append(&v3);
	
	so.setOutputValues(values);
	QCOMPARE(so.getInputValues().size(), 1);
	QCOMPARE(so.getOutputValues().size() , 2);
	QCOMPARE(so.getInfoValues().size(), 0);
	
	so.useOutputAsInfoValue(&v1, true); //does not do anything, because it's an input value 
	QCOMPARE(so.getInputValues().size(), 1);
	QCOMPARE(so.getOutputValues().size() , 2);
	QCOMPARE(so.getInfoValues().size(), 0);
	
	so.useOutputAsInfoValue(&v2, false); //does nothing, as v2 is already an output value 
	QCOMPARE(so.getInputValues().size(), 1);
	QCOMPARE(so.getOutputValues().size() , 2);
	QCOMPARE(so.getInfoValues().size(), 0);
	
	so.useOutputAsInfoValue(&v2, true); //moves v2 from output values to info values 
	QCOMPARE(so.getInputValues().size(), 1);
	QCOMPARE(so.getOutputValues().size() , 1);
	QCOMPARE(so.getInfoValues().size(), 1);
	QVERIFY(so.getInfoValues().contains(&v2));
	QVERIFY(!so.getOutputValues().contains(&v2));
	
	so.useOutputAsInfoValue(&v3, true);
	QCOMPARE(so.getInputValues().size(), 1);
	QCOMPARE(so.getOutputValues().size() , 0);
	QCOMPARE(so.getInfoValues().size(), 2);
	
	so.useOutputAsInfoValue(&v2, false); //moves v2 back to output values.
	QCOMPARE(so.getInputValues().size(), 1);
	QCOMPARE(so.getOutputValues().size() , 1);
	QCOMPARE(so.getInfoValues().size(), 1);
	QVERIFY(so.getInfoValues().contains(&v2) == false);
	QVERIFY(so.getOutputValues().contains(&v2) == true);
	
	//Copy constructor does NOT copy the input / output / info values!
	SimObjectAdapter soa(so);
	
	QCOMPARE(soa.getInputValues().size(), 0);
	QCOMPARE(soa.getOutputValues().size() , 0);
	QCOMPARE(soa.getInfoValues().size(), 0);
	
	
}


//chris
void TestSimObject::testGeometries() {
	
	Core::resetCore();
	
	SimObjectAdapter so1("Object1", "/SimObject/One/");
	
	//Adding geometries.
	QVERIFY(so1.getGeometries().size() == 0);
	
	bool destroyFlag1 = false;
	bool destroyFlag2 = false;
	
	//adding the 0 pointer
	QVERIFY(so1.addGeometry(0) == false);
	QVERIFY(so1.getGeometries().size() == 0);
	
	SimGeomAdapter *sga1 = new SimGeomAdapter(&so1, &destroyFlag1);
	QVERIFY(so1.addGeometry(sga1) == true);
	QVERIFY(so1.getGeometries().size() == 1);
	QVERIFY(so1.getGeometries().contains(sga1) == true);
	
	//can not add a second time.
	QVERIFY(so1.addGeometry(sga1) == false);
	QVERIFY(so1.getGeometries().size() == 1);
	
	//but can add a different SimGeom.
	SimGeomAdapter *sga2 = new SimGeomAdapter(&so1, &destroyFlag2);
	QVERIFY(so1.addGeometry(sga2) == true);
	QVERIFY(so1.getGeometries().size() == 2);
	QVERIFY(so1.getGeometries().contains(sga1) == true);
	QVERIFY(so1.getGeometries().contains(sga2) == true);
	
	//remove a 0 geom  (false)
	QVERIFY(so1.removeGeometry(0) == false);
	QVERIFY(so1.getGeometries().size() == 2);
	
	//remove an existing geom 
	QVERIFY(so1.removeGeometry(sga1) == true);
	QVERIFY(so1.getGeometries().size() == 1);
	
	//add again for destruction test.
	QVERIFY(so1.addGeometry(sga1) == true);
	QVERIFY(so1.getGeometries().size() == 2);
	
	//replacing current simGeoms
	SimGeomAdapter *sga3 = new SimGeomAdapter(&so1);
	SimGeomAdapter *sga4 = new SimGeomAdapter(&so1);
	QVector<SimGeom*> geoms;
	geoms.append(sga3);
	geoms.append(sga3); //sga3 was added two times! 
	geoms.append(sga4);
	
	QVERIFY(destroyFlag1 == false);
	QVERIFY(destroyFlag2 == false);
	
	so1.setGeometries(geoms);
	QVERIFY(so1.getGeometries().size() == 2); //the duplication of sga3 was detected and removed.
	QVERIFY(so1.getGeometries().contains(sga3) == true);
	QVERIFY(so1.getGeometries().contains(sga4) == true);
	QVERIFY(destroyFlag1 == true); // the old SimGeoms have been destroyed.
	QVERIFY(destroyFlag2 == true);
	
}


//Chris
void TestSimObject::testChildObject() {
	Core::resetCore();

	bool destroyedParent = false;
	bool destroyedChild = false;

	SimObjectAdapter *so1 = new SimObjectAdapter("Object1", "/SimObject/One/", &destroyedParent);
	SimObjectAdapter *child1 = new SimObjectAdapter("Child1", "/Child", &destroyedChild);
	SimObjectAdapter *child2 = new SimObjectAdapter("Child2", "/Child2");

	QCOMPARE(so1->getChildObjects().size(), 0);
	QVERIFY(so1->addChildObject(child1) == true);
	QCOMPARE(so1->getChildObjects().size(), 1);
	QVERIFY(so1->addChildObject(child1) == false); //object can not be added twice
	QCOMPARE(so1->getChildObjects().size(), 1);
	QVERIFY(so1->addChildObject(0) == false); //can not add 0 pointer
	QCOMPARE(so1->getChildObjects().size(), 1);

	QVERIFY(so1->addChildObject(child2) == true);
	QCOMPARE(so1->getChildObjects().size(), 2);

	SimObjectAdapter *copy = dynamic_cast<SimObjectAdapter*>(so1->createCopy());

	QVERIFY(copy != 0);
	QCOMPARE(copy->getChildObjects().size(), 2);
	QVERIFY(dynamic_cast<SimObjectAdapter*>(copy->getChildObjects().at(0)) != 0);
	QVERIFY(dynamic_cast<SimObjectAdapter*>(copy->getChildObjects().at(1)) != 0);
	QVERIFY(copy->getChildObjects().at(0)->getName() == "Child1");

	QVERIFY(copy->getChildObjects().at(0) != so1->getChildObjects().at(0));
	
	QVERIFY(so1->removeChildObject(0) == false); //remove 0 pointer not possible
	QCOMPARE(so1->getChildObjects().size(), 2);
	QVERIFY(so1->removeChildObject(child1) == true);
	QCOMPARE(so1->getChildObjects().size(), 1);
	QVERIFY(so1->removeChildObject(child1) == false);
	QCOMPARE(so1->getChildObjects().size(), 1);

	delete so1;
	QVERIFY(destroyedParent == true);
	QVERIFY(destroyedChild == false);

	delete child1;
	delete child2;


}



