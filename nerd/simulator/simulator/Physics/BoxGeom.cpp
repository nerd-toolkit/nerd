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

#include "BoxGeom.h"
#include "BoxBody.h"

namespace nerd {

/**
 * Constructs a new BoxGeom and hands the DoubleValues for updating the 
 * geometry to the new object. 
 */
BoxGeom::BoxGeom(SimObject *sim, double width, double height, double depth) 
		: TriangleGeom(sim) 
{
	mSimObject = sim;

	QVector<Triangle> triangles;

	triangles.push_back(Triangle(0, 2, 1));
	triangles.push_back(Triangle(0, 3, 2));
	triangles.push_back(Triangle(0, 1, 4));
	triangles.push_back(Triangle(0, 4, 5));
	triangles.push_back(Triangle(0, 5, 6));
	triangles.push_back(Triangle(0, 6, 3));
	triangles.push_back(Triangle(1, 7, 4));
	triangles.push_back(Triangle(1, 2, 7));
	triangles.push_back(Triangle(2, 3, 6));
	triangles.push_back(Triangle(2, 6, 7));
	triangles.push_back(Triangle(4, 6, 5));
	triangles.push_back(Triangle(4, 7, 6));

	setTriangles(triangles);
	
	setSize(width, height, depth);
}

BoxGeom::BoxGeom(const BoxGeom &geom) : TriangleGeom(geom) 
{
	mWidth = geom.getWidth();
	mDepth = geom.getDepth();
	mHeight = geom.getHeight();
	
}

BoxGeom::~BoxGeom() {
}


SimGeom* BoxGeom::createCopy() const {
	return new BoxGeom(*this);
}


void BoxGeom::setSize(double width, double height, double depth) {

	if(width >= 0.0) {
		mWidth = width;
	}
	if(height >= 0.0) {
		mHeight = height;
	}
	if(depth >= 0.0) {
		mDepth = depth;
	}
		
	width = width / 2.0;
	height = height / 2.0;
	depth = depth / 2.0;

	QVector<Vector3D> points;

	points.append(Vector3D(-width, -height,  depth));
	points.append(Vector3D(-width,  height,  depth));
	points.append(Vector3D( width,  height,  depth));
	points.append(Vector3D( width, -height,  depth));
	points.append(Vector3D(-width,  height, -depth));
	points.append(Vector3D(-width, -height, -depth));
	points.append(Vector3D( width, -height, -depth));
	points.append(Vector3D( width,  height, -depth));
	
	setPoints(points);
}

double BoxGeom::getDepth() const {
	return mDepth;
}
	
double BoxGeom::getHeight() const {
	return mHeight;
}

double BoxGeom::getWidth() const{
	return mWidth;
}

}
