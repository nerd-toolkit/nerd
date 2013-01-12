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


#include "TriangleGeom.h"
#include "Core/Core.h"
#include "Value/QuaternionValue.h"

namespace nerd {

TriangleGeom::TriangleGeom(SimObject *simObject) : SimGeom(simObject)
{
}

TriangleGeom::TriangleGeom(SimObject *simObject, QVector<Vector3D> points, 
		QVector<Triangle> triangles) : SimGeom(simObject), mTriangles(triangles), 
		mPoints(points)
{
}

TriangleGeom::TriangleGeom(const TriangleGeom &geom) 
		: SimGeom(geom), mTriangles(geom.mTriangles), mPoints(geom.mPoints)
{
}

TriangleGeom::~TriangleGeom() {
}

SimGeom* TriangleGeom::createCopy() const {
	return new TriangleGeom(*this);
}


QVector<Triangle> TriangleGeom::getTriangles() const {
	return mTriangles;
}


void TriangleGeom::setTriangles(QVector<Triangle> triangles) {
	mTriangles.clear();
	mTriangles << triangles;
}

QVector<Vector3D> TriangleGeom::getPoints() const{
	return mPoints;
}


/**
 * Sets the points of this TriangleGeom.
 * Note that these points have to be coordinates NOT considering the 
 * local position. The local position of the TriangleGeom is 
 * automatically considered while setting the points!
 *
 * @param points the point vector in local coordinates.
 */
void TriangleGeom::setPoints(QVector<Vector3D> points) {
	mPoints.clear();
	
	mPoints << points;

	for(int i = 0; i < mPoints.size(); i++) {
		mPoints.replace(i, mPoints.at(i) + mLocalPosition);
	}
}

/**
 * Sets a point of this TriangleGeom.
 * Note that the point has to describe a coordinates NOT considering the 
 * local position. The local position of the TriangleGeom is 
 * automatically considered while setting the point!
 *
 * @param points the point vector in local coordinates.
 */
void TriangleGeom::setPoint(int index, const Vector3D &position) {
	if(index < 0 || index >= mPoints.size()) {
		Core::log("TriangleGeom: Could not set Point, index is not valid.");
		return;
	}
	mPoints.replace(index, position + mLocalPosition);
}

/**
 * Returns the point with the given index. Hereby the local position of
 * the TriangleGeom is already considered.
 *
 * @param index the index of the desired point.
 * @return the point
 */
Vector3D TriangleGeom::getPoint(int index) const{
	if(index < 0 || index >= mPoints.size()) {
		Core::log(QString("TriangleGeom: Point ").append(QString::number(index))
				.append(" does not exist."));
		return Vector3D(0.0, 0.0, 0.0);
	}
	return mPoints.at(index);
}



Triangle TriangleGeom::getTriangle(int index) const{
	if(index < 0 || index >= mTriangles.size()) {
		Core::log(QString("TriangleGeom: Triangle ").append(QString::number(index))
			.append(" does not exist."));
		return Triangle();
	}
	return mTriangles.at(index);
}


/**
* Moves all existing points of the triangle geometry about localTranslation.
*/
// void TriangleGeom::translateGeomPoints() {
// 	for(int i = 0; i < mPoints.size(); i++) {
// 		mPoints.replace(i, mPoints.at(i) + mLocalPosition);
// 	}
// }
		
/**
 * This is a temporary solution to treating TriangleGeom objects correctly. After the local position was changed, all points are moved back about the old local position value and afterwards moved about the local position value.
 */
// void TriangleGeom::updateGeometry() {
// 	for(int i = 0; i < mPoints.size(); i++) {
// 		mPoints.replace(i, mPoints.at(i) - mOldLocalPosition);
// 	}
// 	translateGeomPoints();
// 	mOldLocalPosition = mLocalPosition;
// }

/**
 * 
 * @param localPosition 
 */
void TriangleGeom::setLocalPosition(const Vector3D &localPosition) {

	for(int i = 0; i < mPoints.size(); i++) {
		mPoints.replace(i, (mPoints.at(i) - mLocalPosition) + localPosition);
	}

	SimGeom::setLocalPosition(localPosition);
}


/**
 * 
 * @param localPosition 
 */
void TriangleGeom::setLocalOrientation(const Quaternion &localOrientation) {

	for(int i = 0; i < mPoints.size(); i++) {
		mPoints.replace(i, (mPoints.at(i) - mLocalPosition));
	}

	Vector3D newPoint;
	Quaternion back(mLocalOrientation);
	back.setW(-back.getW());
	Quaternion inv = back.getInverse();
	Quaternion old(0, 0, 0, 0);
	for(int i = 0; i < mPoints.size(); i++) {
		old.set(0, mPoints.at(i).getX(), mPoints.at(i).getY(), mPoints.at(i).getZ());
		Quaternion newPointQ = back * old *inv;
		newPoint.set(newPointQ.getX(), newPointQ.getY(), newPointQ.getZ());
		mPoints.replace(i, newPoint);
	}	

	Quaternion inverse = localOrientation.getInverse();
	Quaternion oldPoint(0, 0, 0, 0);
	for(int i = 0; i < mPoints.size(); i++) {
		oldPoint.set(0,mPoints.at(i).getX(), mPoints.at(i).getY(), mPoints.at(i).getZ());
		Quaternion newPointQ = localOrientation * oldPoint *inverse;
		newPoint.set(newPointQ.getX(), newPointQ.getY(), newPointQ.getZ());
		mPoints.replace(i, newPoint);
	}

	for(int i = 0; i < mPoints.size(); i++) {
		mPoints.replace(i, (mPoints.at(i) + mLocalPosition));
	}

	SimGeom::setLocalOrientation(localOrientation);
}

}
