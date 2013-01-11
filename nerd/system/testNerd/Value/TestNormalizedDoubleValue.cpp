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




#include "TestNormalizedDoubleValue.h"
#include "Value/NormalizedDoubleValue.h"
#include "Core/Core.h"

using namespace nerd;

//chris
void TestNormalizedDoubleValue::testConstructor(){

	Core::resetCore();
	
	
	NormalizedDoubleValue v1;
	
	QCOMPARE(v1.getMin(), 0.0);
	QCOMPARE(v1.getMax(), 1.0);
	QCOMPARE(v1.getNormalizedMin(), 0.0);
	QCOMPARE(v1.getNormalizedMax(), 1.0);
	QCOMPARE(v1.get(), 0.0);
	
	QVERIFY(v1.getTypeName().compare("NormalizedDouble") == 0);
	QVERIFY(v1.getName().compare("NormalizedDouble") == 0);

	NormalizedDoubleValue v2(0.7, -0.5, 0.8);
	QCOMPARE(v2.getMin(), -0.5);
	QCOMPARE(v2.getMax(), 0.8);
	QCOMPARE(v2.getNormalizedMin(), -0.5);
	QCOMPARE(v2.getNormalizedMax(), 0.8);
	QCOMPARE(v2.get(), 0.7);
	
	NormalizedDoubleValue v3(0.9, -0.4, 0.5, -1.1, 2.2);
	QCOMPARE(v3.getMin(), -0.4);
	QCOMPARE(v3.getMax(), 0.5);
	QCOMPARE(v3.getNormalizedMin(), -1.1);
	QCOMPARE(v3.getNormalizedMax(), 2.2);
	QCOMPARE(v3.get(), 0.5); //was truncated, because max is 0.5
	
	v3.setTypeName("Desc1");
	
	NormalizedDoubleValue *v4 = dynamic_cast<NormalizedDoubleValue*>(v3.createCopy());
	
	QVERIFY(v4 != 0);
	QVERIFY(v4 != &v3);
	
	QCOMPARE(v4->getMin(), -0.4);
	QCOMPARE(v4->getMax(), 0.5);
	QCOMPARE(v4->getNormalizedMin(), -1.1);
	QCOMPARE(v4->getNormalizedMax(), 2.2);
	QCOMPARE(v4->get(), 0.5);
	QVERIFY(v4->getTypeName().compare("Desc1") == 0);
	
	delete v4;


}


//chris
void TestNormalizedDoubleValue::testSettingValues() {
	Core::resetCore();
	
	NormalizedDoubleValue v1(-0.9, -0.1, 0.1, -1.0, 1.0);
	
	QCOMPARE(v1.get(), -0.1); //was truncated because of min == -0.1
	QCOMPARE(v1.getNormalized(), -1.0);
	
	//set real value, read normalized value.
	
	v1.set(0.0);
	QCOMPARE(v1.get(), 0.0);
	QCOMPARE(v1.getNormalized(), 0.0);
	
	v1.set(10.0);
	QCOMPARE(v1.get(), 0.1); //was truncated because of max == 0.1
	QCOMPARE(v1.getNormalized(), 1.0);
	
	v1.set(0.05);
	QCOMPARE(v1.get(), 0.05); 
	QCOMPARE(v1.getNormalized(), 0.5);
	
	v1.set(-0.025);
	QCOMPARE(v1.get(), -0.025); 
	QCOMPARE(v1.getNormalized(), -0.25);
	
	v1.setNormalizedMin(0.0);
	v1.setNormalizedMax(10.0);
	QCOMPARE(v1.get(), -0.025); 
	QCOMPARE(v1.getNormalized(), 3.75);
	
	v1.setMin(0.0);
	v1.setMax(0.5);
	QCOMPARE(v1.get(), 0.0);  //raised by changed min
	QCOMPARE(v1.getNormalized(), 0.0);
	
	v1.set(0.4);
	QCOMPARE(v1.get(), 0.4);  
	QCOMPARE(v1.getNormalized(), 8.0);
	
	//set normalized value, read real value.
	
	v1.setMin(-1.0);
	v1.setMax(1.0);
	v1.setNormalizedMin(-0.1);
	v1.setNormalizedMax(0.1);
	
	v1.setNormalized(0.0);
	QCOMPARE(v1.getNormalized(), 0.0);
	QCOMPARE(v1.get(), 0.0);  
	
	v1.setNormalized(0.1);
	QCOMPARE(v1.getNormalized(), 0.1);
	QCOMPARE(v1.get(), 1.0);
	
	v1.setNormalized(-10.0);
	QCOMPARE(v1.getNormalized(), -0.1);
	QCOMPARE(v1.get(), -1.0);
	
	v1.setMin(-2.0);
	v1.setMax(-1.0);
	v1.setNormalizedMin(1.0);
	v1.setNormalizedMax(5.0);
	
	QCOMPARE(v1.getNormalized(), 5.0); //was changed by setMin
	QCOMPARE(v1.get(), -1.0);
	
	v1.setNormalized(4.0);
	QCOMPARE(v1.getNormalized(), 4.0); 
	QCOMPARE(v1.get(), -1.25);
	
	v1.setNormalized(-5.0);
	QCOMPARE(v1.getNormalized(), 1.0);  //bounded by normalizedMin
	QCOMPARE(v1.get(), -2.0);
	
	v1.setNormalized(20.0);
	QCOMPARE(v1.getNormalized(), 5.0);  //bounded by normalizedMin
	QCOMPARE(v1.get(), -1.0);
	

}

//Chris
void TestNormalizedDoubleValue::testPropertySetting() {
	NormalizedDoubleValue n1(0.7, -0.9, 0.9, -0.8, 0.8);

	//test read property string
	QString propertyString = n1.getProperties();
	QVERIFY(propertyString == "-0.9,0.9,-0.8,0.8");

	n1.setMin(-1.1);
	n1.setNormalizedMax(2.11);

	propertyString = n1.getProperties();
	QVERIFY(propertyString == "-1.1,0.9,-0.8,2.11");

	//test set property string
	propertyString = "-0.1,0.5,1.01,5.5";
	QVERIFY(n1.setProperties(propertyString) == true);

	QVERIFY(n1.getMin() == -0.1);
	QVERIFY(n1.getMax() == 0.5);
	QVERIFY(n1.getNormalizedMin() == 1.01);
	QVERIFY(n1.getNormalizedMax() == 5.5);

	//test failures with invalid property strings
	//too many params
	QVERIFY(n1.setProperties("-0.1,0.5,1.01,5.5,1.0") == false); 
	//too less params
	QVERIFY(n1.setProperties("0.5,1.01,5.5") == false); 
	//number format problems 
	QVERIFY(n1.setProperties("a,0.5,1.01,5.5") == false); 
	QVERIFY(n1.setProperties("-0.1,a,1.01,5.5") == false);
	QVERIFY(n1.setProperties("-0.1,0.5,a,5.5") == false);
	QVERIFY(n1.setProperties("-0.1,0.5,1.01,a") == false);
}

