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



#include "TestInterfaceValue.h"
#include "Value/InterfaceValue.h"
#include "Core/Core.h"
#include <iostream>

using namespace std;
using namespace nerd;


//chris
void TestInterfaceValue::testConstructor() {
	Core::resetCore();
	
	InterfaceValue iv1("", "FirstInterfaceValue");
	
	QVERIFY(iv1.getName().compare("FirstInterfaceValue") == 0);
	QVERIFY(iv1.getDescription() == "Interface");
	QVERIFY(iv1.getMin() == 0.0);
	QVERIFY(iv1.getMax() == 1.0);
	QVERIFY(iv1.get() == 0.0);

	InterfaceValue iv2("Test", "Second", 100.5, -15.5, 53.8);
	
	QVERIFY(iv2.getName().compare("Test/Second") == 0);
	QVERIFY(iv2.getMin() == -15.5);
	QVERIFY(iv2.getMax() == 53.8);
	QVERIFY(iv2.get() == 53.8); //was restricted by max.
	
	//modify min / max
	iv2.setMax(13.8);
	QVERIFY(iv2.getMax() == 13.8);
	QVERIFY(iv2.get() == 13.8);
	
	iv2.setMax(100.0);
	QVERIFY(iv2.get() == 13.8);
	
	iv2.setMin(44.4);
	QVERIFY(iv2.getMin() == 44.4);
	QVERIFY(iv2.get() == 44.4);
	
	iv2.set(55.8);
	QVERIFY(iv2.get() == 55.8);
	
	//copy constructor.
	InterfaceValue *iv3 = dynamic_cast<InterfaceValue*>(iv2.createCopy());
	QVERIFY(iv3 != 0);
	QVERIFY(iv3 != &iv2);
	QVERIFY(iv3->getName().compare("Test/Second") == 0);
	QCOMPARE(iv3->getMin(), 44.4);
	QCOMPARE(iv3->getMax(), 100.0);
	QCOMPARE(iv3->get(), 55.8);
	
	iv3->setInterfaceName("Third");
	QVERIFY(iv3->getName().compare("Test/Third") == 0);
	QVERIFY(iv2.getName().compare("Test/Second") == 0);

	delete iv3;
	

	
}


//Chris
void TestInterfaceValue::testPropertySetting() {
	InterfaceValue iv1("/Prefix", "AnInterfaceValue", 0.7, -0.9, 0.9, -0.8, 0.8);

	//test read property string
	QString propertyString = iv1.getProperties();
	QVERIFY(propertyString == "/Prefix,AnInterfaceValue,-0.9,0.9,-0.8,0.8");

	iv1.setMin(-1.1);
	iv1.setNormalizedMax(2.11);
	iv1.setPrefix("DifferentPrefix");
	iv1.setInterfaceName("AnotherName");

	propertyString = iv1.getProperties();
	QVERIFY(propertyString == "DifferentPrefix,AnotherName,-1.1,0.9,-0.8,2.11");

	//test set property string
	propertyString = "NewPrefix,NewName,-0.1,0.5,1.01,5.5";
	QVERIFY(iv1.setProperties(propertyString) == true);

	QVERIFY(iv1.getInterfaceName() == "NewName");
	QVERIFY(iv1.getPrefix() == "NewPrefix");
	QVERIFY(iv1.getMin() == -0.1);
	QVERIFY(iv1.getMax() == 0.5);
	QVERIFY(iv1.getNormalizedMin() == 1.01);
	QVERIFY(iv1.getNormalizedMax() == 5.5);

	//test failures with invalid property strings
	//too many params
	QVERIFY(iv1.setProperties("NewPrefix,NewName,-0.1,0.5,1.01,5.5,1.0") == false); 
	//too less params
	QVERIFY(iv1.setProperties("NewName,-0.1,0.5,1.01,5.5") == false); 
	//number format problems 
	QVERIFY(iv1.setProperties("NewPrefix,NewName,a,0.5,1.01,5.5") == false); 
	QVERIFY(iv1.setProperties("NewPrefix,NewName,-0.1,a,1.01,5.5") == false);
	QVERIFY(iv1.setProperties("NewPrefix,NewName,-0.1,0.5,a,5.5") == false);
	QVERIFY(iv1.setProperties("NewPrefix,NewName,-0.1,0.5,1.01,a") == false);
}

