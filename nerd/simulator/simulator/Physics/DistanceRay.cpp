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

#include "DistanceRay.h"
#include "Physics/DistanceSensor.h"
#include <iostream>

using namespace std;

namespace nerd {

DistanceRay::DistanceRay(const QString &name, const Vector3D &position,
		const Quaternion &orientation, double length, DistanceSensorRule *rule,
		const Color &active, const Color &inactive, const Color &disabledColor)
		: mName(name), mRule(rule), mCollisionObject(0), mGeometry(0), mOwner(0),
		mActiveColor(active), mInactiveColor(inactive), mDisabledColor(disabledColor)
{
// 	mClosestKnownCollisionPoint.set(0.0, 0.0, 0.0);
	
	RayGeom geom(length);
	geom.setLocalPosition(position);
	geom.setLocalOrientation(orientation);

	mCollisionObject = new CollisionObject(geom, 0, true);
	mGeometry = dynamic_cast<RayGeom*>(mCollisionObject->getGeometry());

	// This ensures, that after doing a reset, the color of the
	// ray will still be white.
	mGeometry->setAutomaticColor(false);
	mGeometry->setLength(length);
}

DistanceRay::~DistanceRay() {
	delete mCollisionObject;
}

void DistanceRay::setOwner(DistanceSensor *sensor) {
	if(mOwner != 0) {
		mOwner->getHostBody()->removeCollisionObject(mCollisionObject);
	}
	mOwner = sensor;
	if(mOwner != 0) {
		mOwner->getHostBody()->addCollisionObject(mCollisionObject);
	}
	// TODO other case
}


DistanceSensor* DistanceRay::getOwner() const {
	return mOwner;
}

void DistanceRay::setName(const QString &name) {
	mName = name;
}

QString DistanceRay::getName() const {
	return mName;
}

CollisionObject* DistanceRay::getCollisionObject() const {
	return mCollisionObject;
}

RayGeom* DistanceRay::getGeometry() const {
	return mGeometry;
}

double DistanceRay::getDistance(double minRange) {
	Vector3D rayOffset = mGeometry->getLocalPosition();
	Vector3D hostPos =
		mCollisionObject->getHostBody()->getPositionValue()->get();
	Vector3D rayPos = hostPos + rayOffset;
	double colDist = mGeometry->getLength(); // start at greatest distance

// 	mClosestKnownCollisionPoint.set(0.0, 0.0, 0.0);
	
	if(mRule == 0) {
		return colDist;
	}

	QList<Vector3D> *colPoints = mRule->getCollisionPoints(mCollisionObject);
	
	for(QListIterator<Vector3D> i(*colPoints); i.hasNext();) {
		Vector3D colPoint = i.next();
		Vector3D colVec = colPoint - rayPos;
		double colVecLen = colVec.length();

		if(colVecLen < colDist && colVecLen >= minRange) {
			colDist = colVecLen;
// 			mClosestKnownCollisionPoint = colPoint;
		}
		
// 		if(collisionVectorLength < minRange) {
// 			collisionDistance = mRay->getLength();
// 			mClosestKnownCollisionPoint.set(0.0, 0.0, 0.0);
// 			break;
// 		}
	}
	colPoints->clear();
	
	return colDist;
}

// Vector3D DistanceRay::getClosestKnownCollisionPoint() const {
// 	return mClosestKnownCollisionPoint;
// }

void DistanceRay::updateRay(double rayLength, bool disableRay) {
	if(disableRay) {
		mGeometry->setColor(mDisabledColor);
		mGeometry->setVisibleLength(mGeometry->getLength());
		
		//erase the stored collision data.
// 		if(mRule != 0) {
// 			QList<Vector3D> *collisionPoints = mRule->getCollisionPoints(mCollisionObject);
// 			collisionPoints->clear();
// 		}
	}
	else {
		if(rayLength < mGeometry->getLength()) {
			mGeometry->setColor(mActiveColor);
			mGeometry->setVisibleLength(rayLength);
		}
		else {
			mGeometry->setColor(mInactiveColor);
			mGeometry->setVisibleLength(mGeometry->getLength());
		}
	}
}

}

