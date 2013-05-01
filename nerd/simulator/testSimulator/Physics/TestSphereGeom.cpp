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


#include "TestSphereGeom.h"
#include "Physics/SphereGeom.h"
#include "Physics/SimBody.h" 
#include "Math/Vector3D.h"
#include "Core/Core.h"

using namespace std;

namespace nerd{

// Josef
void TestSphereGeom::testConstruction() {
	Core::resetCore();
	// create SimBody object as base for SphereGeom
	SimBody *simBody = new SimBody("simBody");

	// construct a SphereGeometry attached to the SimBody
	SphereGeom *sphereGeom_1 = new SphereGeom(simBody, 1.234);
	QVERIFY(sphereGeom_1 != 0);

	double radius_1 = sphereGeom_1->getRadius();
	QCOMPARE(radius_1, 1.234);

	Vector3D *position = new Vector3D(1.1, 2.2, 3.3);
	SphereGeom *sphereGeom_2 = new SphereGeom(simBody, 4.321, *position);
	QVERIFY(sphereGeom_2 != 0);

	double radius_2 = sphereGeom_2->getRadius();
	QCOMPARE(radius_2, 4.321);
	QVERIFY(sphereGeom_2->getLocalPosition() == *position);

	SphereGeom *sphereGeom_3 = dynamic_cast<SphereGeom*>(sphereGeom_2->createCopy());
	QVERIFY(sphereGeom_3 != 0);
	QVERIFY(sphereGeom_3 != sphereGeom_2);
	QVERIFY(sphereGeom_3->getRadius() == radius_2);

	delete simBody;
	delete sphereGeom_1;
	delete sphereGeom_2;
	delete sphereGeom_3;
}

void TestSphereGeom::testMethods() {
	// create SimBody object as base for SphereGeom
	SimBody *simBody = new SimBody("simBody");

	// construct a SphereGeometry attached to the SimBody
	SphereGeom *sphereGeom_1 = new SphereGeom(simBody, 1.234);

	// test radius setter method
	sphereGeom_1->setRadius(4.321);

	double radius_1 = sphereGeom_1->getRadius();
	QCOMPARE(radius_1, 4.321);

	delete simBody;
	delete sphereGeom_1;
}

};
