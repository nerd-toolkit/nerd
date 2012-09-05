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

#include "Collision/CollisionObject.h"
#include "Core/Core.h"
#include "Math/Math.h"
#include "Physics/DistanceSensor.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Physics/SimSensor.h"
#include <iostream>
#include <QStringList>

using namespace std;

namespace nerd {

DistanceSensor::DistanceSensor(const QString &name)
	: SimObject(name), SimSensor(), mRule(0)
{
	mActiveColor = new ColorValue(Color(255, 0, 0));
	mAngle = new DoubleValue(0.0);
	mCalcMinDistance = new BoolValue(true);
	mHostBodyName = new StringValue("");
	mInactiveColor = new ColorValue(Color(255, 255, 255));
	mDisabledColor = new ColorValue(Color(255, 255, 255, 50));
	mLocalOrientation = new QuaternionValue(0.0, 0.0, 0.0, 0.0);
	mLocalPosition = new Vector3DValue(0.0, 0.0, 0.0);
	mNumberOfRays = new IntValue(1);
	mMaxRange = new DoubleValue(1.0);
	mMinRange = new DoubleValue(0.0);
	mSensorNoise = new DoubleValue(0.0);
	mMinIntersectionPoint = new Vector3DValue();
	

	mDistance = new InterfaceValue("", "Distance",
			mMaxRange->get() - mMinRange->get(), 0.0,
			mMaxRange->get() - mMinRange->get());
	mOutputValues.append(mDistance);

	//TODO this has to be solved differently to allow multiple DistanceSensors.
	//mRule = new DistanceSensorRule(getName() + "/CollisionRule");

	addParameter("ActiveColor", mActiveColor);
	addParameter("AngleOfAperture", mAngle);
	addParameter("CalculateMinimum", mCalcMinDistance);
	addParameter("Distance", mDistance);
	addParameter("HostBody", mHostBodyName);
	addParameter("InactiveColor", mInactiveColor);
	addParameter("DisabledColor", mDisabledColor);
	//addParameter("LocalOrientation", mLocalOrientation);
	//addParameter("LocalPosition", mLocalPosition);
	addParameter("MaxRange", mMaxRange);
	addParameter("MinRange", mMinRange);
	addParameter("Noise", mSensorNoise);
	addParameter("NumberOfRays", mNumberOfRays);
	addParameter("MinIntersectionPoint", mMinIntersectionPoint);
}

DistanceSensor::DistanceSensor(const DistanceSensor &other)
	: Object(), ValueChangedListener(), SimObject(other), SimSensor(), mRule(0)
{
	mActiveColor = dynamic_cast<ColorValue*>(getParameter("ActiveColor"));
	mAngle = dynamic_cast<DoubleValue*>(getParameter("AngleOfAperture"));
	mCalcMinDistance = dynamic_cast<BoolValue*>(
			getParameter("CalculateMinimum"));
	mDistance = dynamic_cast<InterfaceValue*>(getParameter("Distance"));
	mHostBodyName = dynamic_cast<StringValue*>(getParameter("HostBody"));
	mInactiveColor = dynamic_cast<ColorValue*>(getParameter("InactiveColor"));
	mDisabledColor = dynamic_cast<ColorValue*>(getParameter("DisabledColor"));
	//mLocalOrientation = dynamic_cast<QuaternionValue*>(getParameter("LocalOrientation"));
	//mLocalPosition = dynamic_cast<Vector3DValue*>(getParameter("LocalPosition"));
	mMaxRange = dynamic_cast<DoubleValue*>(getParameter("MaxRange"));
	mMinRange = dynamic_cast<DoubleValue*>(getParameter("MinRange"));
	mSensorNoise = dynamic_cast<DoubleValue*>(getParameter("Noise"));
	mNumberOfRays = dynamic_cast<IntValue*>(getParameter("NumberOfRays"));
	mMinIntersectionPoint = dynamic_cast<Vector3DValue*>(getParameter("MinIntersectionPoint"));
	
	mLocalOrientation = new QuaternionValue(*other.mLocalOrientation);
	mLocalPosition = new Vector3DValue(*other.mLocalPosition);

	if(mDistance != 0) {
		mOutputValues.append(mDistance);
	}

	//mRule = other.mRule; //TODO
}


DistanceSensor::~DistanceSensor() {
	//TODO destroy rule?
}

SimObject* DistanceSensor::createCopy() const {
	return new DistanceSensor(*this);
}

void DistanceSensor::updateSensorValues() {
	double distance;
	
	if (mCalcMinDistance->get()) {
		distance = getMinSensorValue();
	}
	else {
		distance = getAvgSensorValue();
	}
	mDistance->set(Math::calculateGaussian(distance, mSensorNoise->get()));
}

double DistanceSensor::getMinSensorValue() {
	DistanceRay *ray;
	double d = mMinRange->get();
	double distance = mMaxRange->get();
	
	Vector3D minIntersectionPoint;

	for(QListIterator<DistanceRay*> i(mRays); i.hasNext();) {
		ray = i.next();
		d = ray->getDistance(mMinRange->get());
		ray->updateRay(d);
		if(mMinRange->get() <= d && d < distance) {
			distance = d;
			minIntersectionPoint = ray->getClosestKnownCollisionPoint();
		}
	}
	mMinIntersectionPoint->set(minIntersectionPoint);
	
	return distance - mMinRange->get();
}

double DistanceSensor::getAvgSensorValue() {
	DistanceRay *ray;
	double d = mMinRange->get();
	double distance = 0.0;
	int n = mNumberOfRays->get();

	for (QListIterator<DistanceRay*> i(mRays); i.hasNext();) {
		ray = i.next();
		d = ray->getDistance(mMinRange->get());
		ray->updateRay(d);
		if(mMinRange->get() <= d) {
			distance += d;
		}
		else {
			n--;
		}
	}
	return (0 < n ? distance / n : mMaxRange->get()) - mMinRange->get();
}

void DistanceSensor::resetSensor() {
	mDistance->set(mDistance->getMax());
	for(int i = 0; i < mRays.size(); ++i) {
		DistanceRay *ray = mRays.at(i);
		RayGeom *rayGeom = ray->getRayCollisionObject();
		ray->updateRay(rayGeom->getLength(), true);
	}
}

void DistanceSensor::valueChanged(Value *value) {
	SimObject::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mMaxRange || value == mMinRange) {
		//adapt normalization of InterfaceValue to the current max range.
		mDistance->setMax(mMaxRange->get() - mMinRange->get());
	}
	else if(value == mNameValue) {
		if(mRule != 0) {
			mRule->setName(getName() + "/CollisionRule");
		}
		else {
			//create the collision rule only when the name of the (prototype) has changed!
			mRule = new DistanceSensorRule(getName() + "/CollisionRule");
		}
	}
}

void DistanceSensor::setup() {
	SimObject::setup();
	
	if(mRule == 0) {
		mRule = new DistanceSensorRule(getName() + "/CollisionRule");
	}

	mHostBody = Physics::getPhysicsManager()->getSimBody(mHostBodyName->get());

	if(mHostBody == 0) {
		Core::log(QString("DistanceSensor [").append(getName())
				.append("]::setup: Could not find host body [")
				.append(mHostBodyName->get()).append("]!"), true);
		return;
	}
	
// 	//collect undetectable objects
// 	QStringList undetectableObjectNames = mUndetecableObjectsRegExps->get().split(",");
// 	PhysicsManager *pm = Physics::getPhysicsManager();
// 	
// 	for(int i = 0; i < undetectableObjectNames.size(); ++i) {
// 		QString regExp = undetectableObjectNames.at(i);
// 		QList<SimObject*> undetectableObjects = pm->getSimObjects(regExp);
// 		for(int j = 0; j < undetectableObjects.size(); ++j) {
// 			SimBody *body = dynamic_cast<SimBody*>(undetectableObjects.at(j));
// 			if(body != 0) {
// 				mUndetectableObjects << body->getCollisionObjects();
// 			}
// 		}
// 	}

	//set target objects
	QList<CollisionObject*> clist;
	QList<SimBody*> bodies = Physics::getPhysicsManager()->getSimBodies();

	for(QListIterator<SimBody*> i(bodies); i.hasNext();) {
		QList<CollisionObject*> collisionObjects = i.next()->getCollisionObjects();
		for(QListIterator<CollisionObject*> j(collisionObjects); j.hasNext();) {
			CollisionObject *obj = j.next();
			if(dynamic_cast<RayGeom*>(obj->getGeometry()) == 0) {
				clist.append(obj);
			}
		}
	}
	mRule->setTargetGroup(clist);

	for(QListIterator<CollisionObject*> j(mHostBody->getCollisionObjects()); j.hasNext();) {
		CollisionObject *obj = j.next();
		mRule->removeFromTargetGroup(obj);
	}

	QList<Quaternion> orientations = calcOrientations();
	QList<CollisionObject*> cRays;

	for(int i = 0; i < orientations.size(); ++i) {
		DistanceRay *ray
				= new DistanceRay(getName() + "/Ray" + QString::number(i),
				mLocalPosition->get(), orientations.at(i), mMaxRange->get(),
				mRule, mActiveColor->get(), mInactiveColor->get(), mDisabledColor->get());
		ray->setOwner(this);
		mRays.append(ray);
		ray->getCollisionObject()->disableCollisions(true);
		cRays += ray->getCollisionObject();
	}

	SimBody *hostBody = getHostBody();
	
	for(QListIterator<CollisionObject*> i(cRays); i.hasNext();) {
		CollisionObject *co = i.next();
		mRule->addToSourceGroup(co);
		if(hostBody != 0) {
			hostBody->addCollisionObject(co);
		}
	}
}


void DistanceSensor::clear() {
	QList<CollisionObject*> cRays;

	SimObject::clear();
	SimBody *hostBody = getHostBody();
	
	for(QListIterator<DistanceRay*> i(mRays); i.hasNext();) {
		CollisionObject *co = i.next()->getCollisionObject();
		mRule->removeFromSourceGroup(co);
		mRule->removeFromTargetGroup(co);
		if(hostBody != 0) {
			hostBody->removeCollisionObject(co);
		}
	}
	while(!mRays.empty()) {
		DistanceRay *dRay = mRays.front();
		mRays.removeAll(dRay);
		dRay->setOwner(0);
		delete dRay;
	}
}

QList<Quaternion> DistanceSensor::calcOrientations() const {
	
	QList<Quaternion> l;
	double angle = mAngle->get();
	int n = mNumberOfRays->get() - 1;
	
	double startAngle = angle / -2.0;
	double angleIncrement = 0.0;
	
	if (n > 0) {
		angleIncrement = angle / ((double) n);
	}
	
	Quaternion angleQuat(1.0, 0.0, 0.0, 0.0);
	
	if(n <= 0) {
		l.append(angleQuat);
	}
	else {
		for(int i = 0; i < n + 1; ++i) {
			angleQuat.setFromAngles(0.0, startAngle + (i * angleIncrement), 0.0);
			l.append(angleQuat);
		}
	}
	
// 	QList<Quaternion> l;
// 	Quaternion peripheral(1.0, 0.0, 0.0, 0.0);
// 	Quaternion rotation(1.0, 0.0, 0.0, 0.0);
// 	Quaternion local = mLocalOrientation->get();
// 	double angle = mAngle->get();
// 	int n = mNumberOfRays->get() - 1;
// 
// 	if (n > 0) {
// 		peripheral.setFromAngles(0.0, 0.0, angle / -2.0);
// 		rotation.setFromAngles(0.0, 0.0, angle / ((double) n));
// 	}
// 	peripheral.normalize();
// 	rotation.normalize();
// 	if (n >= 0) {
// 		//peripheral = peripheral * local * peripheral.getInverse();
// 		l.append(peripheral);
// 	} 
// 	peripheral.normalize();
// 	
// 	cerr << "Peri: " << peripheral.getX() << " " << peripheral.getY() << " " << peripheral.getZ() << " " << peripheral.getW() << endl;
// 	cerr << "Rot : " << rotation.getX() << " " << rotation.getY() << " " << rotation.getZ() << " " << rotation.getW() << endl;
// 	
// 	for (int i = 0; i < n; i++) {
// 		peripheral = rotation * peripheral * rotation.getInverse();
// 		peripheral.normalize();
// 		l.append(peripheral);
// 	}


// 	QList<Quaternion> l;
// 	Quaternion peripheral(1.0, 0.0, 0.0, 0.0);
// 	Quaternion rotation(1.0, 0.0, 0.0, 0.0);
// 	Quaternion local = mLocalOrientation->get();
// 	double angle = mAngle->get();
// 	int n = mNumberOfRays->get() - 1;
// 
// 	if (n > 0) {
// 		peripheral.setFromAngles(0.0, 0.0, angle / -2.0);
// 		rotation.setFromAngles(0.0, 0.0, angle / ((double) n));
// 	}
// 	peripheral.normalize();
// 	rotation.normalize();
// 	if (n >= 0) {
// 		Quaternion newRotation = local * peripheral;
// 		newRotation.normalize();
// 		l.append(newRotation);
// 	}
// 	peripheral.normalize();
// 	for (int i = 0; i < n; i++) {
// 		peripheral = rotation * peripheral;
// 		peripheral.normalize();
// 		Quaternion newRotation = local * peripheral;
// 		newRotation.normalize();
// 		l.append(newRotation);
// 	}

	
	
	/*
	 m SensorGeometry->setLocalOrientatio*n(mLocalOrientation->get());
	 mSensorBody->getGeometry()->setLocalPosition(mLocalPosition->get());
	 Quaternion localPos(0, 
			mLocalPosition->getX(), 
			mLocalPosition->getY(), 
			mLocalPosition->getZ());
	 Quaternion bodyOrientationInverse = mHostBody->getQuaternionOrientationValue()->get().getInverse();
	 Quaternion rotatedLocalPosQuat = mHostBody->getQuaternionOrientationValue()->get() 
	 * localPos * bodyOrientationInverse;
	 V ector3D rotatedLocalPos(rotatedLoc*alPosQuat.getX(), 
	 rotatedLocalPosQuat.getY(), 
	 rotatedLocalPosQuat.getZ());
	*/
	
	return l;
}

SimBody* DistanceSensor::getHostBody() const {
	return mHostBody;
}

}

