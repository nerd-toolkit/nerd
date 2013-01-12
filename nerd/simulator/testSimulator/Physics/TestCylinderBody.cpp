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

#include "TestCylinderBody.h"
#include "Core/Core.h"
#include "Physics/CylinderBody.h"
#include "Physics/CylinderGeom.h"
#include "Collision/CollisionObject.h"

namespace nerd {

void TestCylinderBody::testCylinderBody() {

	Core::resetCore();
	
	CylinderBody cylinder1("Cylinder1");
	QVERIFY(cylinder1.getName().compare("Cylinder1") == 0);
	QCOMPARE(dynamic_cast<DoubleValue*>(cylinder1.getParameter("Length"))->get(), 0.0);
	QCOMPARE(dynamic_cast<DoubleValue*>(cylinder1.getParameter("Radius"))->get(), 0.0);

	cylinder1.getParameter("Radius")->setValueFromString("0.11");
	cylinder1.getParameter("Length")->setValueFromString("0.5");
	QCOMPARE(dynamic_cast<DoubleValue*>(cylinder1.getParameter("Length"))->get(), 0.5);
	QCOMPARE(dynamic_cast<DoubleValue*>(cylinder1.getParameter("Radius"))->get(), 0.11);

	CylinderBody cylinder2("Cylinder2", 1.0, 10.0);
	QCOMPARE(dynamic_cast<DoubleValue*>(cylinder2.getParameter("Length"))->get(), 10.0);
	QCOMPARE(dynamic_cast<DoubleValue*>(cylinder2.getParameter("Radius"))->get(), 1.0);

	cylinder2.getPositionValue()->set(1.0, 2.0, 3.0);
	QCOMPARE(cylinder2.getPositionValue()->getX(), 1.0);
	QCOMPARE(cylinder2.getPositionValue()->getY(), 2.0);
	QCOMPARE(cylinder2.getPositionValue()->getZ(), 3.0);

	cylinder2.getParameter("Mass")->setValueFromString("1.0");
	
	CylinderBody *copy = dynamic_cast<CylinderBody*>(cylinder2.createCopy());
	QVERIFY(copy != 0);
	QVERIFY(copy->getName().compare("Cylinder2") == 0);
	QCOMPARE(dynamic_cast<DoubleValue*>(copy->getParameter("Length"))->get(), 10.0);
	QCOMPARE(dynamic_cast<DoubleValue*>(copy->getParameter("Radius"))->get(), 1.0);
	QCOMPARE(copy->getPositionValue()->getX(), 1.0);
	QCOMPARE(copy->getPositionValue()->getY(), 2.0);
	QCOMPARE(copy->getPositionValue()->getZ(), 3.0);

	CollisionObject *co = cylinder2.getCollisionObjects().at(0);
	QCOMPARE(co->getMass(), 0.0);
	QCOMPARE(co->getGeometry(), cylinder2.getGeometries().at(0));
	CylinderGeom *geom = dynamic_cast<CylinderGeom*>(cylinder2.getGeometries().at(0));
	QCOMPARE(geom->getRadius(), 1.0);
	QCOMPARE(geom->getLength(), 10.0);
	

	cylinder2.getParameter("Radius")->setValueFromString("1.2");
	cylinder2.getParameter("Length")->setValueFromString("5.1");
	cylinder2.setup();
	QCOMPARE(geom->getRadius(), 1.2);
	QCOMPARE(geom->getLength(), 5.1);

	cylinder2.getParameter("Radius")->setValueFromString("-1.2");
	cylinder2.getParameter("Length")->setValueFromString("-5.1");
	cylinder2.setup();
	QVERIFY(geom->getRadius() > 0.0);
	QVERIFY(geom->getLength() > 0.0);
	
}

}
