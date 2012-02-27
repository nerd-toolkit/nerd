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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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


#ifndef TriangleGeom_H_
#define TriangleGeom_H_

#include "SimGeom.h"
#include "Math/Vector3D.h"
#include "Math/Quaternion.h"

namespace nerd {


/**
 * Class TriangleGeom serves as a universal geometry container for SimObject's. A 
 * TriangleGeom describes the geometry by a set of points, that form triangular faces. 
*/
struct Triangle {
	Triangle() {
		mEdge[0] = 0;
		mEdge[1] = 0;
		mEdge[2] = 0;
	}

	Triangle(int first, int second, int third) {
		mEdge[0] = first;
		mEdge[1] = second;
		mEdge[2] = third;
	}
	
	Triangle(const Triangle &other) {
		mEdge[0] = other.mEdge[0];
		mEdge[1] = other.mEdge[1];
		mEdge[2] = other.mEdge[2];
	}
	
	int mEdge[3];
};


/**
 * TriangleGeom.
 */
class TriangleGeom : public SimGeom {

	public:
	
		TriangleGeom(SimObject *simGeom);
		TriangleGeom(SimObject *simGeom, QVector<Vector3D> points, 
					 QVector<Triangle> triangles);
		TriangleGeom(const TriangleGeom &geom);
		virtual ~TriangleGeom();
		
		virtual SimGeom* createCopy() const;
		
		QVector<Triangle> getTriangles() const;
		void setTriangles(QVector<Triangle> triangles);
		Triangle getTriangle(int index) const;
		
		QVector<Vector3D> getPoints() const;
		Vector3D getPoint(int index) const;
		void setPoints(QVector<Vector3D> points);
		void setPoint(int index, const Vector3D &position);
		
		virtual void setLocalPosition(const Vector3D &localPosition);
		virtual void setLocalOrientation(const Quaternion &localOrientation);
	
	protected:
		QVector<Triangle> mTriangles;
		QVector<Vector3D> mPoints;
};
}
#endif
