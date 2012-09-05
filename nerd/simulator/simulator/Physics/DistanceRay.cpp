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

#include "DistanceRay.h"
#include "Physics/DistanceSensor.h"
#include <iostream>

using namespace std;

namespace nerd {

DistanceRay::DistanceRay(const QString &name, const Vector3D &position,
		const Quaternion &orientation, double length, DistanceSensorRule *rule,
		const Color &active, const Color &inactive, const Color &disabledColor)
		: mName(name), mRule(rule), mRayCollisionObject(0), mRay(0), mOwner(0),
		mActiveColor(active), mInactiveColor(inactive), mDisabledColor(disabledColor)
{
	mClosestKnownCollisionPoint.set(0.0, 0.0, 0.0);
	
	RayGeom geom(length);
	geom.setLocalPosition(position);
	geom.setLocalOrientation(orientation);

	mRayCollisionObject = new CollisionObject(geom, 0, true);
	mRay = dynamic_cast<RayGeom*>(mRayCollisionObject->getGeometry());

	// This ensures, that after doing a reset, the color of the
	// ray will still be white.
	mRay->setAutomaticColor(false);
	mRay->setLength(length);
}

DistanceRay::~DistanceRay() {
	delete mRayCollisionObject;
}

void DistanceRay::setOwner(DistanceSensor *sensor) {
	if(mOwner != 0) {
		mOwner->getHostBody()->removeCollisionObject(mRayCollisionObject);
	}
	mOwner = sensor;
	if(mOwner != 0) {
		mOwner->getHostBody()->addCollisionObject(mRayCollisionObject);
	}
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
	return mRayCollisionObject;
}

RayGeom* DistanceRay::getRayCollisionObject() const {
	return mRay;
}

double DistanceRay::getDistance(double minRange) {
	Vector3D localpos = mRay->getLocalPosition();
	Vector3D simpos
	= mRayCollisionObject->getHostBody()->getPositionValue()->get();
	Vector3D pos = localpos + simpos;
	Vector3D vto;
	double distance = mRay->getLength();
	
	mClosestKnownCollisionPoint.set(0.0, 0.0, 0.0);
	
	if(mRule == 0) {
		return distance;
	}
	QList<Vector3D> *points = mRule->getCollisionPoints(mRayCollisionObject);
	
	for(QListIterator<Vector3D> i(*points); i.hasNext();) {
		Vector3D point = i.next();
		vto = pos - point;
		double length = vto.length();
		if(length < distance && length >= minRange) {
			distance = vto.length();
			mClosestKnownCollisionPoint = point;
		}
	}
	points->clear();
	return distance;
}

Vector3D DistanceRay::getClosestKnownCollisionPoint() const {
	return mClosestKnownCollisionPoint;
}

void DistanceRay::updateRay(double length, bool disableRay) {
	if(disableRay) {
		mRay->setColor(mDisabledColor);
		mRay->setVisibleLength(mRay->getLength());
	}
	else {
		if(length < mRay->getLength()) {
			mRay->setColor(mActiveColor);
			mRay->setVisibleLength(length);
		}
		else {
			mRay->setColor(mInactiveColor);
			mRay->setVisibleLength(mRay->getLength());
		}
	}
}

}

