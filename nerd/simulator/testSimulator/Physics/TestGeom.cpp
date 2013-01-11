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


#include "TestGeom.h"
#include "Physics/TriangleGeom.h"
#include "Physics/SimGeom.h"
#include "Core/Core.h"
#include "Physics/SimObject.h"
#include "Physics/BoxGeom.h"
#include "Physics/SphereGeom.h"
#include "Physics/SimGeom.h"
#include "Physics/RayGeom.h"
#include "Physics/CylinderGeom.h"
#include "Physics/SimObjectAdapter.h"


namespace nerd{

// TODO: add test for: localTranslation and localOrientation!!!


void TestGeom::testSimGeom() {
	Core::resetCore();

	SimGeom geom(0);
	QVERIFY(geom.getSimObject() == 0);
	QVERIFY(geom.hasAutomaticColor());
	QVERIFY(geom.isDrawingEnabled());
	QCOMPARE(geom.getLocalPosition().getX(), 0.0);
	QCOMPARE(geom.getLocalPosition().getY(), 0.0);
	QCOMPARE(geom.getLocalPosition().getZ(), 0.0);
	
	QCOMPARE(geom.getLocalOrientation().getW(), 1.0);
	QCOMPARE(geom.getLocalOrientation().getX(), 0.0);
	QCOMPARE(geom.getLocalOrientation().getY(), 0.0);
	QCOMPARE(geom.getLocalOrientation().getZ(), 0.0);

	QCOMPARE(geom.getColor().red(), 255);
	QCOMPARE(geom.getColor().green(), 255);
	QCOMPARE(geom.getColor().blue(), 255);
	QCOMPARE(geom.getColor().alpha(), 255);

	SimObjectAdapter *body = new SimObjectAdapter("Object", "");
	
	geom.setSimObject(body);
	QCOMPARE(geom.getSimObject(), body);
	geom.setColor(100,0,100,100);
	QCOMPARE(geom.getColor().red(), 100);
	geom.enableDrawing(false);

	geom.setLocalPosition(Vector3D(0.1, 0.0, 0.5));

	SimGeom *copy = geom.createCopy();
	QVERIFY(copy != 0);
	QCOMPARE(copy->getSimObject(), body);
	QVERIFY(!copy->isDrawingEnabled());
	QCOMPARE(copy->getColor().red(), 100);
	QCOMPARE(copy->getColor().green(), 0);
	QCOMPARE(copy->getColor().blue(), 100);
	QCOMPARE(copy->getColor().alpha(), 100);
	QCOMPARE(copy->getLocalPosition().getX(), 0.1);
	QCOMPARE(copy->getLocalPosition().getY(), 0.0);
	QCOMPARE(copy->getLocalPosition().getZ(), 0.5);
}

//chris
void TestGeom::testConstructor() {
	Core::resetCore();
	
	SimObject obj("Sim1");
	
	TriangleGeom geom(&obj);
	
	QVERIFY(geom.getSimObject() == &obj);
	QVERIFY(geom.getTriangles().isEmpty());
	QVERIFY(geom.getPoints().isEmpty());
	QVERIFY(geom.getColor().red() == 255);
	QVERIFY(geom.getColor().green() == 255);
	QVERIFY(geom.getColor().blue() == 255);
	QVERIFY(geom.getColor().alpha() == 255);
	QVERIFY(geom.getLocalPosition().equals(Vector3D()));
	QVERIFY(geom.isDrawingEnabled() == true);
	
	SimObject obj2("Sim2");
	geom.setSimObject(&obj2);
	QVERIFY(geom.getSimObject() == &obj2);
	
	geom.setLocalPosition(Vector3D(0.1, 0.2, 0.3));
	QVERIFY(geom.getLocalPosition().equals(Vector3D(0.1, 0.2, 0.3)));
	
	geom.setColor(0, 0, 255, 255);
	QVERIFY(geom.getColor().red() == 0);
	QVERIFY(geom.getColor().green() == 0);
	QVERIFY(geom.getColor().blue() == 255);
	QVERIFY(geom.getColor().alpha() == 255);
	
	geom.enableDrawing(false);
	QVERIFY(geom.isDrawingEnabled() == false);
	
	QVector<Triangle> triangles;
	triangles.push_back(Triangle(4, 1, 2));
	triangles.push_back(Triangle(3, 8, 1));
	geom.setTriangles(triangles);
	QVERIFY(geom.getTriangles().size() == 2);
	
	QVector<Vector3D> points;
	points.push_back(Vector3D(4.1, 2.1, 6.1));
	geom.setPoints(points);
	QVERIFY(geom.getPoints().size() == 1);
	QVERIFY(geom.getPoint(0).equals(Vector3D(4.2, 2.3, 6.4))); //local position added.
	
	
	
	//createCopy
	TriangleGeom *geom2 = dynamic_cast<TriangleGeom*>(geom.createCopy());
	
	QVERIFY(geom2 != 0);
	QVERIFY(geom2 != &geom);
	QVERIFY(geom2->getSimObject() == &obj2);
	QVERIFY(geom2->getLocalPosition().equals(Vector3D(0.1, 0.2, 0.3)));
	QVERIFY(geom2->getColor().red() == 0);
	QVERIFY(geom2->getColor().green() == 0);
	QVERIFY(geom2->getColor().blue() == 255);
	QVERIFY(geom2->getColor().alpha() == 255);
	QVERIFY(geom2->isDrawingEnabled() == false);
	QVERIFY(geom2->getTriangles().size() == 2);
	QVERIFY(geom2->getTriangle(0).mEdge[2] == 2);
	QVERIFY(geom2->getPoints().size() == 1);
	QVERIFY(geom2->getPoint(0).equals(Vector3D(4.2, 2.3, 6.4)));
	
	delete geom2;
	
	
	
	//specialized constructors.
	TriangleGeom geom3(&obj, points, triangles);
	QVERIFY(geom3.getSimObject() == &obj);
	QVERIFY(geom3.getTriangles().size() == 2);
	QVERIFY(geom3.getTriangle(0).mEdge[2] == 2);
	QVERIFY(geom3.getPoints().size() == 1);
	QVERIFY(geom3.getPoint(0).equals(Vector3D(4.1, 2.1, 6.1))); //this time local position is 0,0,0
// 	QVERIFY(geom3.getColor() == Qt::white);
	QVERIFY(geom3.getColor().red() == 255);
	QVERIFY(geom3.getColor().green() == 255);
	QVERIFY(geom3.getColor().blue() == 255);
	QVERIFY(geom3.getColor().alpha() == 255);
	QVERIFY(geom3.getLocalPosition().equals(Vector3D()));
	QVERIFY(geom3.isDrawingEnabled() == true);
	
}

//chris
void TestGeom::testPoints() {	
	Core::resetCore();
	
	QVector<Triangle> triangles;
	triangles.push_back(Triangle(2, 5, 9));
	triangles.push_back(Triangle(3, 6, 10));
	
	QVector<Vector3D> points;
	points.push_back(Vector3D(2.1, 3.1, 4.1));
	points.push_back(Vector3D(9.5, 8.5, 7.5));
	

	TriangleGeom geom(0, points, triangles);
	QVERIFY(geom.getTriangles().size() == 2);
	QVERIFY(geom.getPoints().size() == 2);
	
	//check out points
	QVERIFY(geom.getPoint(0).equals(Vector3D(2.1, 3.1, 4.1))); 
	QVERIFY(geom.getPoint(1).equals(Vector3D(9.5, 8.5, 7.5)));
	
	//get Point with non-existing index.
	QVERIFY(geom.getPoint(-1).equals(Vector3D(0.0, 0.0, 0.0)));
	QVERIFY(geom.getPoint(2).equals(Vector3D(0.0, 0.0, 0.0)));
	
	//get points with modified location
	geom.setLocalPosition(Vector3D(1.0, 2.0, 3.0));
	QVERIFY(geom.getLocalPosition().equals(Vector3D(1.0, 2.0, 3.0)));
	QVERIFY(geom.getPoint(0).equals(Vector3D(2.1 + 1.0, 3.1 + 2.0, 4.1 + 3.0))); 
	QVERIFY(geom.getPoint(1).equals(Vector3D(10.5, 10.5, 10.5)));
	
	//replace single point
	geom.setPoint(1, Vector3D(44.2, 44.3, 44.4));
	QVERIFY(geom.getPoint(0).equals(Vector3D(2.1 + 1.0, 3.1 + 2.0, 4.1 + 3.0))); 
	QVERIFY(geom.getPoint(1).equals(Vector3D(44.2 + 1.0, 44.3 + 2.0, 44.4 + 3.0))); 
	
	//replace non-existing index
	geom.setPoint(-1, Vector3D(1.0, 2.0, 3.0));
	geom.setPoint(3, Vector3D(1.0, 2.0, 3.0));
	QVERIFY(geom.getPoints().size() == 2);
	QVERIFY(geom.getPoint(1).equals(Vector3D(44.2 + 1.0, 44.3 + 2.0, 44.4 + 3.0))); 
	
	
	//set all points
	QVector<Vector3D> points2;
	points2.push_back(Vector3D(5.0, 6.0, 7.0));
	points2.push_back(Vector3D(0.1, 0.2, 0.3));
	points2.push_back(Vector3D(0.01, 0.02, 0.03));
	geom.setPoints(points2);
	
	QVERIFY(geom.getPoints().size() == 3);
	QVERIFY(geom.getPoint(0).equals(Vector3D(5.0 + 1.0, 6.0 + 2.0, 7.0 + 3.0))); 
	QVERIFY(geom.getPoint(1).equals(Vector3D(0.1 + 1.0, 0.2 + 2.0, 0.3 + 3.0))); 
	QVERIFY(geom.getPoint(2).equals(Vector3D(0.01 + 1.0, 0.02 + 2.0, 0.03 + 3.0))); 
	
	
	//check out triangles
	QVERIFY(geom.getTriangle(0).mEdge[0] == 2);
	QVERIFY(geom.getTriangle(0).mEdge[1] == 5);
	QVERIFY(geom.getTriangle(0).mEdge[2] == 9);
	
	QVERIFY(geom.getTriangle(1).mEdge[0] == 3);
	QVERIFY(geom.getTriangle(1).mEdge[1] == 6);
	QVERIFY(geom.getTriangle(1).mEdge[2] == 10);
	
	//get Triangle with non-existing index.
	QVERIFY(geom.getTriangle(-1).mEdge[0] == 0);
	QVERIFY(geom.getTriangle(3).mEdge[1] == 0);
	
	//set all triangles
	QVector<Triangle> triangles2;
	triangles2.push_back(Triangle(99, 88, 77));
	geom.setTriangles(triangles2);
	
	QVERIFY(geom.getTriangles().size() == 1);
	QVERIFY(geom.getTriangle(0).mEdge[0] == 99);
	QVERIFY(geom.getTriangle(0).mEdge[1] == 88);
	QVERIFY(geom.getTriangle(0).mEdge[2] == 77);
	
	
}


//chris
void TestGeom::testBoxGeom(){
	Core::resetCore();
	SimObject obj("Sim1");

	BoxGeom *box = new BoxGeom(&obj, 1.0, 10.0, 100.0);
	
	QVERIFY(box->getSimObject() == &obj);
// 	QVERIFY(box->getColor() == Qt::white);	
	QVERIFY(box->getColor().red() == 255);
	QVERIFY(box->getColor().green() == 255);
	QVERIFY(box->getColor().blue() == 255);
	QVERIFY(box->getColor().alpha() == 255);
	QVERIFY(box->getLocalPosition().equals(Vector3D()));
	QVERIFY(box->isDrawingEnabled() == true);
	
	
	QVERIFY(box->getPoints().size() ==  8);
	QVERIFY(box->getTriangles().size() == 12);

	QVERIFY(box->getPoint(0).equals(Vector3D(-0.5, -5.0, 50.0)));
	QVERIFY(box->getPoint(1).equals(Vector3D(-0.5, 5.0, 50.0)));
	QVERIFY(box->getPoint(2).equals(Vector3D(0.5, 5.0, 50.0)));
	QVERIFY(box->getPoint(3).equals(Vector3D(0.5, -5.0, 50.0)));
	QVERIFY(box->getPoint(4).equals(Vector3D(-0.5, 5.0, -50.0)));
	QVERIFY(box->getPoint(5).equals(Vector3D(-0.5, -5.0, -50.0)));
	QVERIFY(box->getPoint(6).equals(Vector3D(0.5, -5.0, -50.0)));
	QVERIFY(box->getPoint(7).equals(Vector3D(0.5, 5.0, -50.0)));
	
	//TODO test triangle indices.
	
	
	//createCopy
	
	BoxGeom *box2 = dynamic_cast<BoxGeom*>(box->createCopy());
	
	QVERIFY(box2 != 0);
	QVERIFY(box2 != box);
	
	QVERIFY(box2->getSimObject() == &obj);
// 	QVERIFY(box2->getColor() == Qt::white);
	QVERIFY(box2->getColor().red() == 255);
	QVERIFY(box2->getColor().green() == 255);
	QVERIFY(box2->getColor().blue() == 255);
	QVERIFY(box2->getColor().alpha() == 255);
	QVERIFY(box2->getLocalPosition().equals(Vector3D()));
	QVERIFY(box2->isDrawingEnabled() == true);
	
	QVERIFY(box2->getPoints().size() ==  8);
	QVERIFY(box2->getTriangles().size() == 12);

	QVERIFY(box2->getPoint(0).equals(Vector3D(-0.5, -5.0, 50.0)));
	QVERIFY(box2->getPoint(1).equals(Vector3D(-0.5, 5.0, 50.0)));
	QVERIFY(box2->getPoint(2).equals(Vector3D(0.5, 5.0, 50.0)));
	QVERIFY(box2->getPoint(3).equals(Vector3D(0.5, -5.0, 50.0)));
	QVERIFY(box2->getPoint(4).equals(Vector3D(-0.5, 5.0, -50.0)));
	QVERIFY(box2->getPoint(5).equals(Vector3D(-0.5, -5.0, -50.0)));
	QVERIFY(box2->getPoint(6).equals(Vector3D(0.5, -5.0, -50.0)));
	QVERIFY(box2->getPoint(7).equals(Vector3D(0.5, 5.0, -50.0)));

	box->setSize(0.2, -0.1, 0.4);
	QCOMPARE(box->getWidth(), 0.2);	
	QCOMPARE(box->getHeight(), 10.0);
	QCOMPARE(box->getDepth(), 0.4);

	delete box;
	delete box2;
	
}


//chris
void TestGeom::testSphereGeom() {
	
	Core::resetCore();
	
	SimObject obj("Sim1");
	
	SphereGeom geom(&obj);
	
	QVERIFY(geom.getSimObject() == &obj);
// 	QVERIFY(geom.getColor() == Qt::white);
	QVERIFY(geom.getColor().red() == 255);
	QVERIFY(geom.getColor().green() == 255);
	QVERIFY(geom.getColor().blue() == 255);
	QVERIFY(geom.getColor().alpha() == 255);
	QVERIFY(geom.getLocalPosition().equals(Vector3D()));
	QVERIFY(geom.isDrawingEnabled() == true);
	QVERIFY(geom.getRadius() == 0.0);
	
	//set radius
	geom.setRadius(0.001);
	QVERIFY(geom.getRadius() == 0.001);
	
	geom.setRadius(-0.2);
	QCOMPARE(geom.getRadius(), 0.001);
	
	//alternative constructor 
	SphereGeom geom2(&obj, 99.876, Vector3D(10.0, 200.0, 3000.0));
	
	QVERIFY(geom2.getSimObject() == &obj);
// 	QVERIFY(geom2.getColor() == Qt::white);
	QVERIFY(geom2.getColor().red() == 255);
	QVERIFY(geom2.getColor().green() == 255);
	QVERIFY(geom2.getColor().blue() == 255);
	QVERIFY(geom2.getColor().alpha() == 255);
	QVERIFY(geom2.getLocalPosition().equals(Vector3D(10.0, 200.0, 3000.0)));
	QVERIFY(geom2.isDrawingEnabled() == true);
	QVERIFY(geom2.getRadius() == 99.876);
	
	//createCopy
	SphereGeom *geom3 = dynamic_cast<SphereGeom*>(geom2.createCopy());
	
	QVERIFY(geom3 != 0);
	QVERIFY(geom3 != &geom2);
	
	QVERIFY(geom3->getSimObject() == &obj);
// 	QVERIFY(geom3->getColor() == Qt::white);	
	QVERIFY(geom3->getColor().red() == 255);
	QVERIFY(geom3->getColor().green() == 255);
	QVERIFY(geom3->getColor().blue() == 255);
	QVERIFY(geom3->getColor().alpha() == 255);
	QVERIFY(geom3->getLocalPosition().equals(Vector3D(10.0, 200.0, 3000.0)));
	QVERIFY(geom3->isDrawingEnabled() == true);
	QVERIFY(geom3->getRadius() == 99.876);
	
	delete geom3;
	
}


void TestGeom::testRayGeom() {

	Core::resetCore();
	SimObject obj("Object");
	RayGeom geom1;
		
	QVERIFY(geom1.getSimObject() == 0);
	QCOMPARE(geom1.getLength(), 0.0);
	QCOMPARE(geom1.getVisibleLength(), 0.0);

	geom1.setSimObject(&obj);
	QVERIFY(geom1.getSimObject() == &obj);

	RayGeom geom2(1.0);
	QCOMPARE(geom2.getLength(), 1.0);
	QCOMPARE(geom2.getVisibleLength(), 1.0);

	geom1.setVisibleLength(0.5);
	geom1.setLength(1.0);
	QCOMPARE(geom1.getLength(), 1.0);
	QCOMPARE(geom1.getVisibleLength(), 0.5);
	geom1.setLength(-0.11);
	QCOMPARE(geom1.getLength(), 1.0);
	geom1.setVisibleLength(-0.22);	
	QCOMPARE(geom1.getVisibleLength(), 0.5);

	QCOMPARE(geom1.getLocalPosition().getX(), 0.0);
	QCOMPARE(geom1.getLocalPosition().getY(), 0.0);
	QCOMPARE(geom1.getLocalPosition().getZ(), 0.0);

	geom1.setLocalPosition(Vector3D(1.0, 20.0, -0.1));
	QCOMPARE(geom1.getLocalPosition().getX(), 1.0);
	QCOMPARE(geom1.getLocalPosition().getY(), 20.0);
	QCOMPARE(geom1.getLocalPosition().getZ(), -0.1);

	RayGeom *copy = dynamic_cast<RayGeom*>(geom1.createCopy());
	QVERIFY(copy != 0);
	
	QCOMPARE(copy->getSimObject(), &obj);
	QCOMPARE(copy->getLength(), 1.0);
	QCOMPARE(copy->getVisibleLength(), 0.5);
	QCOMPARE(copy->getLocalPosition().getX(), 1.0);
	QCOMPARE(copy->getLocalPosition().getY(), 20.0);
	QCOMPARE(copy->getLocalPosition().getZ(), -0.1);
}


void TestGeom::testCylinderGeom() {
	Core::resetCore();

	SimObject obj("Object");
	
	CylinderGeom geom1(&obj);
	QCOMPARE(geom1.getSimObject(), &obj);
	QCOMPARE(geom1.getLength(), 0.0);
	QCOMPARE(geom1.getRadius(), 0.0);
	QCOMPARE(geom1.getLocalPosition().getX(), 0.0);
	QCOMPARE(geom1.getLocalPosition().getY(), 0.0);
	QCOMPARE(geom1.getLocalPosition().getZ(), 0.0);
	geom1.setLength(0.22);
	geom1.setRadius(0.1);
	QCOMPARE(geom1.getLength(), 0.22);	
	QCOMPARE(geom1.getRadius(), 0.1);
	geom1.setLength(-0.5);
	QCOMPARE(geom1.getLength(), 0.22);
	
	CylinderGeom geom2(&obj, 0.1, 0.5, Vector3D(1.0,0.5,0.5));
	QCOMPARE(geom2.getLength(), 0.5);
	QCOMPARE(geom2.getRadius(), 0.1);
	QCOMPARE(geom2.getLocalPosition().getX(), 1.0);
	QCOMPARE(geom2.getLocalPosition().getY(), 0.5);
	QCOMPARE(geom2.getLocalPosition().getZ(), 0.5);
	QCOMPARE(geom2.getLocalOrientation().getW(), 1.0);
	QCOMPARE(geom2.getLocalOrientation().getX(), 0.0);
	QCOMPARE(geom2.getLocalOrientation().getY(), 0.0);
	QCOMPARE(geom2.getLocalOrientation().getZ(), 0.0);

	geom2.setLocalPosition(Vector3D(-1.0, 10.0, 1.0));
	geom2.setLocalOrientation(Quaternion(0.707, 0.707, 0.0, 0.0));
	QCOMPARE(geom2.getLocalPosition().getX(), -1.0);
	QCOMPARE(geom2.getLocalPosition().getY(), 10.0);
	QCOMPARE(geom2.getLocalPosition().getZ(), 1.0);
	QCOMPARE(geom2.getLocalOrientation().getW(), 0.707);
	QCOMPARE(geom2.getLocalOrientation().getX(), 0.707);
	QCOMPARE(geom2.getLocalOrientation().getY(), 0.0);
	QCOMPARE(geom2.getLocalOrientation().getZ(), 0.0);

	CylinderGeom *copy = dynamic_cast<CylinderGeom*>(geom2.createCopy());
	QVERIFY(copy != 0);
	QCOMPARE(copy->getSimObject(), &obj);
	QCOMPARE(copy->getLength(), 0.5);
	QCOMPARE(copy->getRadius(), 0.1);
	QCOMPARE(copy->getLocalPosition().getX(), -1.0);
	QCOMPARE(copy->getLocalPosition().getY(), 10.0);
	QCOMPARE(copy->getLocalPosition().getZ(), 1.0);
	QCOMPARE(copy->getLocalOrientation().getW(), 0.707);
	QCOMPARE(copy->getLocalOrientation().getX(), 0.707);
	QCOMPARE(copy->getLocalOrientation().getY(), 0.0);
	QCOMPARE(copy->getLocalOrientation().getZ(), 0.0);
	
}
}

