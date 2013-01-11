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




#include "TestParameterizedObject.h"
#include "Core/ParameterizedObject.h"
#include "Core/ParameterizedObjectAdapter.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"
#include "Value/ValueManager.h"
#include "Core/Core.h"

using namespace nerd;


//Chris
void TestParameterizedObject::testConstructor(){
	Core::resetCore();

	ParameterizedObject po("PO2", "Prefix1");
	
	QVERIFY(po.getParameters().empty());
	QVERIFY(po.getParameterNames().empty());
	QVERIFY(po.getName().compare("PO2") == 0);
	QVERIFY(po.getPrefix().compare("Prefix1") == 0);
	po.setName("ParamObj1");
	QVERIFY(po.getName().compare("ParamObj1") == 0);

	//test copy constructor
	IntValue *v1 = new IntValue(10);
	DoubleValue *v2 = new DoubleValue(22.3);
	QVERIFY(po.addParameter("IntParam1", v1) == true);	
	QVERIFY(po.addParameter("DoubleParam1", v2) == true);
	QVERIFY(po.getParameters().size() == 2);
	QVERIFY(po.getParameters().contains(v1) == true);
	QVERIFY(po.getParameters().contains(v2) == true);

	ParameterizedObject po2(po);
	QVERIFY(po2.getName().compare("ParamObj1") == 0);
	QVERIFY(po2.getPrefix().compare("Prefix1") == 0);
	QVERIFY(po2.getParameters().size() == 2);
	//does not contain the same value objects as po.
	QVERIFY(po2.getParameters().contains(v1) == false);
	QVERIFY(po2.getParameters().contains(v2) == false);	

	IntValue *vc1 = dynamic_cast<IntValue*>(po2.getParameter("IntParam1"));
	QVERIFY(vc1 != 0);
	QVERIFY(vc1->get() == 10);
	DoubleValue *vc2 = dynamic_cast<DoubleValue*>(po2.getParameter("DoubleParam1"));
	QVERIFY(vc2 != 0);
	QVERIFY(vc2->get() == 22.3);

}


//Chris
void TestParameterizedObject::testAddAndRemoveParameters() {

	Core::resetCore();
	

	ParameterizedObjectAdapter po("PO1", "/Prefix/");
	IntValue *v1 = new IntValue();
	IntValue *v2 = new IntValue();
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	QVERIFY(vm->getIntValue("/Prefix/Value1") == 0);
	QVERIFY(vm->getIntValue("/Prefix/Value1b") == 0);

	QVERIFY(po.getParameter("Value1") == 0);
	QVERIFY(po.getParameters().size() == 0);
	QVERIFY(po.getParameterNames().size() == 0);

	//add a NULL value
	QVERIFY(po.addParameter("Value", 0) == false);
	
	//add a real value
	QVERIFY(po.addParameter("Value1", v1) == true); //add without publishing
	QVERIFY(po.getParameters().size() == 1);
	QVERIFY(po.getParameterNames().size() == 1);
	QVERIFY(po.getParameters().first() == v1);
	QVERIFY(po.getParameter("Value1") == v1);
	//was NOT published in ValueManager
	QVERIFY(vm->getIntValue("/Prefix/Value1") == 0);
	
	//the ParameterizedObject was automatically added as ValueChangedListener for the parameter.
	QVERIFY(po.mCountValueChanged == 0);
	QVERIFY(po.mLastChangedValue == 0);
	v1->set(1234);
	QVERIFY(po.mCountValueChanged == 1);
	QVERIFY(po.mLastChangedValue == v1);

	//add another value with the same value (fails)
	QVERIFY(po.addParameter("Value1", v2) == false);
	QVERIFY(po.getParameters().size() == 1);
	QVERIFY(po.getParameterNames().size() == 1);

	//add the same value with another name (works)
	QVERIFY(po.addParameter("Value1b", v1, true) == true); //add and publish
	QVERIFY(po.getParameters().size() == 2); //parameter v1 is contained 2 times.
	QVERIFY(po.getParameterNames().size() == 2);
	QVERIFY(po.getParameter("Value1") == v1);
	QVERIFY(po.getParameter("Value1b") == v1);
	//was published in ValueManager
	QVERIFY(vm->getIntValue("/Prefix/Value1b") == v1);

	//add the second value
	QVERIFY(po.addParameter("Value2", v2) == true);
	QVERIFY(po.getParameters().size() == 3);
	QVERIFY(po.getParameterNames().size() == 3);
	QVERIFY(po.getParameter("Value2") == v2);

	//publish all parameters
	po.publishAllParameters();
	QVERIFY(vm->getIntValue("/Prefix/Value1") == v1);
	QVERIFY(vm->getIntValue("/Prefix/Value1b") == v1);
	QVERIFY(vm->getIntValue("/Prefix/Value2") == v2);

	//unpublish all parameters
	po.unpublishAllParameters();
	QVERIFY(po.getParameter("Value1") == v1);
	QVERIFY(po.getParameter("Value1b") == v1);
	QVERIFY(po.getParameter("Value2") == v2);
	QVERIFY(vm->getValue("/Prefix/Value1") == 0);
	QVERIFY(vm->getValue("/Prefix/Value1b") == 0);
	QVERIFY(vm->getValue("/Prefix/Value2") == 0);

	//TODO check if repository changed event was triggered (and bind() called)

	//publish again
	po.publishAllParameters();
	QVERIFY(vm->getIntValue("/Prefix/Value1") == v1);
	QVERIFY(vm->getIntValue("/Prefix/Value1b") == v1);
	QVERIFY(vm->getIntValue("/Prefix/Value2") == v2);

	//remove all params
	po.removeParameters();

	QVERIFY(po.getParameters().size() == 0);
	QVERIFY(po.getParameterNames().size() == 0);
	QVERIFY(po.getParameter("Value1") == 0);
	QVERIFY(po.getParameter("Value1b") == 0);
	QVERIFY(po.getParameter("Value2") == 0);
	
	//published variables are also removed.
	QVERIFY(vm->getValue("/Prefix/Value1") == 0);
	QVERIFY(vm->getValue("/Prefix/Value1b") == 0);
	QVERIFY(vm->getValue("/Prefix/Value2") == 0);

}


//Chris
void TestParameterizedObject::testEquals() {
	Core::resetCore();
	

	ParameterizedObjectAdapter p1("PO1", "/Prefix/");
	ParameterizedObjectAdapter p2("PO1", "/Prefix/");

	IntValue v1(556);
	DoubleValue v2(557.0);
	
	QVERIFY(p1.equals(0) == false);
	QVERIFY(p1.equals(&p1) == true);
	QVERIFY(p1.equals(&p2) == true);
	QVERIFY(p2.equals(&p1) == true);
	
	p1.setName("PO1_1");
	QVERIFY(p1.equals(&p2) == false);
	QVERIFY(p2.equals(&p1) == false);
	p2.setName("PO1_1");
	QVERIFY(p1.equals(&p2) == true);

	p1.setPrefix("AnotherPrefix");
	QVERIFY(p1.equals(&p2) == false);
	p2.setPrefix("AnotherPrefix");
	QVERIFY(p1.equals(&p2) == true);
	

	p1.addParameter("Param1", v1.createCopy());
	QVERIFY(p1.equals(&p2) == false);
	p2.addParameter("Parameter1", v1.createCopy());
	QVERIFY(p1.equals(&p2) == false);
	p2.removeParameters();
	p2.addParameter("Param1", v1.createCopy());
	QVERIFY(p1.equals(&p2) == true);

	IntValue *iv1 = dynamic_cast<IntValue*>(p1.getParameter("Param1"));
	IntValue *iv2 = dynamic_cast<IntValue*>(p2.getParameter("Param1"));
	QVERIFY(iv1 != 0);
	iv1->set(557);
	QVERIFY(p1.equals(&p2) == false);
	iv2->set(557);
	QVERIFY(p1.equals(&p2) == true);

	p2.removeParameters();
	p2.addParameter("Param1", v2.createCopy()); //wrong type
	QVERIFY(p1.equals(&p2) == false);

	p1.removeParameters();
	p1.addParameter("Param1", v2.createCopy());
	QVERIFY(p1.equals(&p2) == true);
}

