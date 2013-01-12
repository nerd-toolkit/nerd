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




#include "TestIndividual.h"
#include <iostream>
#include "Evolution/Individual.h"
#include "Core/Core.h"
#include "Core/ObjectAdapter.h"

using namespace std;
using namespace nerd;

void TestIndividual::initTestCase() {
}

void TestIndividual::cleanUpTestCase() {
}

//chris
void TestIndividual::testConstruction() {
	Core::resetCore();

	Individual ind1;
	Individual ind2;
	Individual ind3(111);

	//ids.
	QVERIFY(ind1.getId() != ind2.getId());
	QCOMPARE(ind3.getId(), 111);

	QVERIFY(ind1.getGenome() == 0);
	QVERIFY(ind1.getPhenotype() == 0);

	//set genome and phenotype.	
	ObjectAdapter obj1("Obj1");
	ObjectAdapter obj2("Obj2");

	ind1.setGenome(&obj1);
	QVERIFY(ind1.getGenome() == &obj1);
	QVERIFY(ind1.getPhenotype() == 0);

	ind1.setPhenotype(&obj2);
	QVERIFY(ind1.getGenome() == &obj1);
	QVERIFY(ind1.getPhenotype() == &obj2);

	ind1.setGenome(0);
	QVERIFY(ind1.getGenome() == 0);
	QVERIFY(ind1.getPhenotype() == &obj2);

	ind1.setPhenotype(0);
	QVERIFY(ind1.getGenome() == 0);
	QVERIFY(ind1.getPhenotype() == 0);

	//parents
	QVERIFY(ind1.getParents().empty());
	ind1.getParents().append(&ind2);
	QVERIFY(ind1.getParents().size() == 1);
	QVERIFY(ind1.getParents().at(0) == &ind2);

	//properties
	QVERIFY(ind1.hasProperty("TestProp") == false);
	ind1.setProperty("TestProp", "Content");
	QVERIFY(ind1.hasProperty("TestProp") == true);
	QVERIFY(ind1.getProperty("TestProp") == "Content");

	//protection
	QVERIFY(ind1.isGenomeProtected() == false);
	ind1.protectGenome(true);
	QVERIFY(ind1.isGenomeProtected() == true);
	ind1.protectGenome(false);
	QVERIFY(ind1.isGenomeProtected() == false);


}


