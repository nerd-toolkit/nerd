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




#include "TestSimObjectGroup.h"
#include <iostream>
#include "Core/Core.h"
#include "Physics/SimObjectGroup.h"
#include "Physics/SimObjectAdapter.h"

using namespace std;
using namespace nerd;


//Chris
void TestSimObjectGroup::testConstruction() {

	Core::resetCore();

	SimObjectGroup *g1 = new SimObjectGroup("Group1", "Type1");

	QVERIFY(g1->getName() == "Group1");
	QVERIFY(g1->getType() == "Type1");
	QVERIFY(g1->getObjects().empty());
	QVERIFY(g1->getInputValues().empty());
	QVERIFY(g1->getInfoValues().empty());
	QVERIFY(g1->getOutputValues().empty());

	bool destroySimObject1;
	SimObjectAdapter *obj1 = new SimObjectAdapter("Obj1", "", &destroySimObject1);
	QVERIFY(obj1->getObjectGroup() == 0);
	
	QVERIFY(g1->addObject(obj1));
	QVERIFY(g1->getObjects().size() == 1);
	QVERIFY(g1->getObjects().at(0) == obj1);
	QVERIFY(obj1->getObjectGroup() == g1);

	
	SimObjectGroup *g2 = new SimObjectGroup(*g1);
	QVERIFY(g2->getName() == "Group1");
	
	QVERIFY(g2->getType() == "Type1");
	QVERIFY(g2->getObjects().size() == 1);
	QVERIFY(g2->getObjects().at(0) != obj1);
	SimObjectAdapter *obj2 = dynamic_cast<SimObjectAdapter*>(g2->getObjects().at(0));
	QVERIFY(obj2 != 0);
	QVERIFY(obj2 != obj1);
	QVERIFY(obj2->getName() == "Obj1");
	QVERIFY(obj2->getObjectGroup() == g2);

	delete g1;
	QVERIFY(destroySimObject1 == false); //objects are not destroyed by their groups.
	
	delete g2;
	delete obj1;
	delete obj2;

	QVERIFY(destroySimObject1);
	
}



//Chris
void TestSimObjectGroup::testObjectHandling() {

	Core::resetCore();
	SimObjectGroup *g1 = new SimObjectGroup("Group1", "Type1");

	bool destroySimObject1;
	SimObjectAdapter *obj1 = new SimObjectAdapter("Obj1", "", &destroySimObject1);
	SimObjectAdapter *obj2 = new SimObjectAdapter("Obj2", "");

	InterfaceValue *iv1 = new InterfaceValue("/Test", "Input");
	InterfaceValue *iv2 = new InterfaceValue("/Test", "Output");
	InterfaceValue *iv3 = new InterfaceValue("/Test", "OutputInfo");

	QList<InterfaceValue*> inputs;
	inputs.append(iv1);
	obj1->setInputValues(inputs);

	QList<InterfaceValue*> outputs;
	outputs.append(iv2);
	obj2->setOutputValues(outputs);

	QList<InterfaceValue*> infos;
	infos.append(iv3);
	obj2->setInfoValues(infos);

	QCOMPARE(g1->getObjects().size(), 0);
	QCOMPARE(g1->getInputValues().size(), 0);
	QCOMPARE(g1->getInfoValues().size(), 0);
	QCOMPARE(g1->getOutputValues().size(), 0);


	QVERIFY(g1->addObject(0) == false); //fails
	QCOMPARE(g1->getObjects().size(), 0);

	//add the first object
	QVERIFY(g1->addObject(obj1) == true);
	QCOMPARE(g1->getObjects().size(), 1);
	QVERIFY(g1->getObjects().at(0) == obj1);

	QCOMPARE(g1->getInputValues().size(), 1);
	QCOMPARE(g1->getInfoValues().size(), 0);
	QCOMPARE(g1->getOutputValues().size(), 0);

	QVERIFY(g1->getInputValues().contains(iv1));

	//try to add again
	QVERIFY(g1->addObject(obj1) == false);
	QCOMPARE(g1->getObjects().size(), 1);
	QCOMPARE(g1->getInputValues().size(), 1);

	//add the second obj
	QVERIFY(g1->addObject(obj2) == true);
		QCOMPARE(g1->getObjects().size(), 2);
	QVERIFY(g1->getObjects().contains(obj1));
	QVERIFY(g1->getObjects().contains(obj2));

	QCOMPARE(g1->getInputValues().size(), 1);
	QCOMPARE(g1->getInfoValues().size(), 1);
	QCOMPARE(g1->getOutputValues().size(), 1);
	QVERIFY(g1->getInputValues().contains(iv1));
	QVERIFY(g1->getOutputValues().contains(iv2));
	QVERIFY(g1->getInfoValues().contains(iv3));

	//switch an info to an output and back
	obj2->useOutputAsInfoValue(iv3, false);
	QCOMPARE(g1->getInputValues().size(), 1);
	QCOMPARE(g1->getInfoValues().size(), 0);
	QCOMPARE(g1->getOutputValues().size(), 2);
	QVERIFY(g1->getInputValues().contains(iv1));
	QVERIFY(g1->getOutputValues().contains(iv2));
	QVERIFY(g1->getOutputValues().contains(iv3));
	
	obj2->useOutputAsInfoValue(iv2, true);
	QCOMPARE(g1->getInputValues().size(), 1);
	QCOMPARE(g1->getInfoValues().size(), 1);
	QCOMPARE(g1->getOutputValues().size(), 1);
	QVERIFY(g1->getInputValues().contains(iv1));
	QVERIFY(g1->getOutputValues().contains(iv3));
	QVERIFY(g1->getInfoValues().contains(iv2)); //iv3 and iv2 switched places

	//change interfaces manually
	outputs.append(iv1);
	inputs.append(iv2);
	obj1->setInputValues(inputs);
	obj2->setOutputValues(outputs);

	QCOMPARE(g1->getInputValues().size(), 2);
	QCOMPARE(g1->getInfoValues().size(), 1);
	QCOMPARE(g1->getOutputValues().size(), 2);
	QVERIFY(g1->getInputValues().contains(iv1));
	QVERIFY(g1->getInputValues().contains(iv2));
	QVERIFY(g1->getOutputValues().contains(iv1));
	QVERIFY(g1->getOutputValues().contains(iv2));
	QVERIFY(g1->getInfoValues().contains(iv2));
	
	delete g1;
	delete obj1;
	delete obj2;
	delete iv1;
	delete iv2;
	delete iv3;
}





