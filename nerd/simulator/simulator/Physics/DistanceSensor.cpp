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
	: SimObject(name), SimSensor(), mRule(0), mHostBodyObj(0)
{
	mActiveColor = new ColorValue(Color(255, 0, 0));
	mAngleOfAperture = new DoubleValue(0.0);
	mCalculateMinimum = new BoolValue(true);
	mHostBody = new StringValue("");
	mInactiveColor = new ColorValue(Color(255, 255, 255));
	mDisabledColor = new ColorValue(Color(255, 255, 255, 50));
	mLocalOrientation = new QuaternionValue(0.0, 0.0, 0.0, 0.0);
	mLocalPosition = new Vector3DValue(0.0, 0.0, 0.0);
	mNumberOfRays = new IntValue(1);
	mRayLength = new DoubleValue(1.0);
	mRayOffset = new DoubleValue(0.0);
	mNoise = new DoubleValue(0.0);
	mIgnoreList = new StringValue("");
	mIgnoreList->setDescription("Comma-separated list of regular expressions "
								"matching sim bodies to ignore");
	

	mDistance = new InterfaceValue("", "Distance",
			mRayLength->get() - mRayOffset->get(), 0.0,
			mRayLength->get() - mRayOffset->get());
	mOutputValues.append(mDistance);

	addParameter("ActiveColor", mActiveColor);
	addParameter("AngleOfAperture", mAngleOfAperture);
	addParameter("CalculateMinimum", mCalculateMinimum);
	addParameter("Distance", mDistance);
	addParameter("HostBody", mHostBody);
	addParameter("InactiveColor", mInactiveColor);
	addParameter("DisabledColor", mDisabledColor);
	addParameter("RayLength", mRayLength);
	addParameter("RayOffset", mRayOffset);
	addParameter("Noise", mNoise);
	addParameter("NumberOfRays", mNumberOfRays);
	addParameter("IgnoreList", mIgnoreList);
}

DistanceSensor::DistanceSensor(const DistanceSensor &other)
	: Object(), ValueChangedListener(), SimObject(other), SimSensor(),
	mRule(0), mHostBodyObj(0)
{
	mActiveColor = dynamic_cast<ColorValue*>(getParameter("ActiveColor"));
	mAngleOfAperture =
		dynamic_cast<DoubleValue*>(getParameter("AngleOfAperture"));
	mCalculateMinimum = dynamic_cast<BoolValue*>(
			getParameter("CalculateMinimum"));
	mDistance = dynamic_cast<InterfaceValue*>(getParameter("Distance"));
	mHostBody = dynamic_cast<StringValue*>(getParameter("HostBody"));
	mInactiveColor = dynamic_cast<ColorValue*>(getParameter("InactiveColor"));
	mDisabledColor = dynamic_cast<ColorValue*>(getParameter("DisabledColor"));
	mRayLength = dynamic_cast<DoubleValue*>(getParameter("RayLength"));
	mRayOffset = dynamic_cast<DoubleValue*>(getParameter("RayOffset"));
	mNoise = dynamic_cast<DoubleValue*>(getParameter("Noise"));
	mNumberOfRays = dynamic_cast<IntValue*>(getParameter("NumberOfRays"));
	mIgnoreList = dynamic_cast<StringValue*>(getParameter("IgnoreList"));
	
	mLocalOrientation = new QuaternionValue(*other.mLocalOrientation);
	mLocalPosition = new Vector3DValue(*other.mLocalPosition);

	if(mDistance != 0) {
		mOutputValues.append(mDistance);
	}
}


DistanceSensor::~DistanceSensor() {
	//TODO destroy rule?
}

SimObject* DistanceSensor::createCopy() const {
	return new DistanceSensor(*this);
}

void DistanceSensor::updateSensorValues() {
	double distance;
	
	if(mCalculateMinimum->get()) {
		distance = getMinSensorValue();
	}
	else {
		distance = getAvgSensorValue();
	}
	mDistance->set(Math::calculateGaussian(distance, mNoise->get()));
}

double DistanceSensor::getMinSensorValue() {
	DistanceRay *ray;
	double offset = mRayOffset->get();
	double min = mRayLength->get();

	for(QListIterator<DistanceRay*> i(mRays); i.hasNext();) {
		ray = i.next();
		double dist = ray->getDistance(offset);
		ray->updateRay(dist);
		if(offset <= dist && dist < min) {
			min = dist;
		}
	}
	
	return min - offset;
}

double DistanceSensor::getAvgSensorValue() {
	DistanceRay *ray;
	double d = mRayOffset->get();
	double distance = 0.0;
	int n = mNumberOfRays->get();

	for (QListIterator<DistanceRay*> i(mRays); i.hasNext();) {
		ray = i.next();
		d = ray->getDistance(mRayOffset->get());
		ray->updateRay(d);
		if(mRayOffset->get() <= d) {
			distance += d;
		}
		else {
			n--;
		}
	}
	return (0 < n ? distance / n : mRayLength->get()) - mRayOffset->get();
}

void DistanceSensor::resetSensor() {
	mDistance->set(mDistance->getMax());
	for(int i = 0; i < mRays.size(); ++i) {
		DistanceRay *ray = mRays.at(i);
		RayGeom *rayGeom = ray->getGeometry();
		ray->updateRay(rayGeom->getLength(), true);
	}
}

void DistanceSensor::valueChanged(Value *value) {
	SimObject::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mRayLength || value == mRayOffset) {
		//adapt normalization of InterfaceValue to the current max range.
		mDistance->setMax(mRayLength->get() - mRayOffset->get());
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
	else if(value == mIgnoreList) {
		updateCollisionRule();
	}
}

void DistanceSensor::setup() {
	SimObject::setup();

	mHostBodyObj = Physics::getPhysicsManager()->getSimBody(mHostBody->get());

	if(mHostBodyObj == 0) {
		Core::log(QString("DistanceSensor [").append(getName())
				.append("]::setup: Could not find host body [")
				.append(mHostBody->get()).append("]!"), true);
		return;
	}
	
	// create and setup the initial collision rule
	updateCollisionRule();

	QList<Quaternion> orientations = getRayOrientations();
	QList<CollisionObject*> cRays;

	for(int i = 0; i < orientations.size(); ++i) {
		DistanceRay *ray
				= new DistanceRay(getName() + "/Ray" + QString::number(i),
				mLocalPosition->get(), orientations.at(i), mRayLength->get(),
				mRule, mActiveColor->get(), mInactiveColor->get(), mDisabledColor->get());
		ray->setOwner(this);
		mRays.append(ray);
		CollisionObject *colObj = ray->getCollisionObject();
		colObj->disableCollisions(true);
		mRule->addToSourceGroup(colObj);
		mHostBodyObj->addCollisionObject(colObj);
// 		cRays += ray->getCollisionObject();
	}
/*
	SimBody *hostBody = getHostBody();
	
	for(QListIterator<CollisionObject*> i(cRays); i.hasNext();) {
		CollisionObject *co = i.next();
		mRule->addToSourceGroup(co);
		if(hostBody != 0) {
			hostBody->addCollisionObject(co);
		}
	}
*/
}


void DistanceSensor::clear() {
	QList<CollisionObject*> cRays;
	
	for(QListIterator<DistanceRay*> i(mRays); i.hasNext();) {
		CollisionObject *colObj = i.next()->getCollisionObject();
		mRule->removeFromSourceGroup(colObj);
// 		mRule->removeFromTargetGroup(co);
// 		if(hostBody != 0) {
		mHostBodyObj->removeCollisionObject(colObj);
// 		}
	}
// 	while(!mRays.empty()) {
// 		DistanceRay *dRay = mRays.front();
// 		mRays.removeAll(dRay);
// 		dRay->setOwner(0);
// 		delete dRay;
// 	}
	mRays.clear();
	
	SimObject::clear();
}

void DistanceSensor::updateCollisionRule() {
	if(mRule == 0) {
		mRule = new DistanceSensorRule(getName() + "/CollisionRule");
	}
	
	// at first, add all sim bodies (except rays)
	QList<CollisionObject*> clist;
	QList<SimBody*> bodies = Physics::getPhysicsManager()->getSimBodies();
	
	for(QListIterator<SimBody*> i(bodies); i.hasNext();) {
		QList<CollisionObject*> collisionObjects =
									i.next()->getCollisionObjects();
									
		for(QListIterator<CollisionObject*> j(collisionObjects); j.hasNext();) {
			CollisionObject *obj = j.next();
			if(dynamic_cast<RayGeom*>(obj->getGeometry()) == 0) {
				clist.append(obj);
			}
		}
	}
	mRule->setTargetGroup(clist);
	
	// remove host object from collision targets
	for(QListIterator<CollisionObject*> j(mHostBodyObj->getCollisionObjects());
		j.hasNext();) {
		mRule->removeFromTargetGroup(j.next());
	}
	
	// Ignore additional objects given as a list of regular expressions
	QStringList ignoreNames =
		mIgnoreList->get().split(",", QString::SkipEmptyParts);
	PhysicsManager *pm = Physics::getPhysicsManager();
		
	for(int i = 0; i < ignoreNames.size(); ++i) {		
		QString regExp = ignoreNames.at(i);
		QList<SimObject*> ignoreObjects = pm->getSimObjects(regExp);
		
		if(ignoreObjects.empty()) {
			Core::log(QString("DistanceSensor [").append(getName())
			.append("]::updateCollisionRule: Could not find object(s) [")
			.append(regExp).append("]!"), true);
		} else {
			for(int j = 0; j < ignoreObjects.size(); ++j) {
				SimBody *body = dynamic_cast<SimBody*>(ignoreObjects.at(j));
				if(body != 0) {
// 					Core::log(QString("DistanceSensor [").append(getName())
// 					.append("]::updateCollisionRule: Ignoring sim object [")
// 					.append(body->getAbsoluteName()).append("]!"), true);
					QList<CollisionObject*> colObjs = body->getCollisionObjects();
					for(int k = 0; k < colObjs.size(); ++k) {
						mRule->removeFromTargetGroup(colObjs.at(k));
					}
				}
			}
		}
	}
}

QList<Quaternion> DistanceSensor::getRayOrientations() const {
	
// 	QList<Quaternion> l;
// 	double angle = mAngleOfAperture->get();
// 	int n = mNumberOfRays->get() - 1;
// 	
// 	double startAngle = angle / -2.0;
// 	double angleIncrement = 0.0;
// 	
// 	if (n > 0) {
// 		angleIncrement = angle / ((double) n);
// 	}
// 	
// 	Quaternion angleQuat(1.0, 0.0, 0.0, 0.0);
// 	
// 	if(n <= 0) {
// 		l.append(angleQuat);
// 	}
// 	else {
// 		for(int i = 0; i < n + 1; ++i) {
// 			angleQuat.setFromAngles(0.0, startAngle + (i * angleIncrement), 0.0);
// 			l.append(angleQuat);
// 		}
// 	}
// 	
// 	return l;
	
	QList<Quaternion> orientations;
	double angle = mAngleOfAperture->get();
	int n = mNumberOfRays->get() - 1;
	
	Quaternion angleQuat(1.0, 0.0, 0.0, 0.0);
	
	// only one ray, same orientation as sensor
	if(n <= 0) {
		orientations.append(angleQuat);
	}
	
	// multiple rays, calculcate individual orientations throughout angle range
	else {
		double startAngle = angle / -2.0;
		double angleInc = angle / ((double) n);
		
		for(int i = 0; i < n+1; ++i) {
			angleQuat.setFromAngles(0.0, startAngle + (i * angleInc), 0.0);
			orientations.append(angleQuat);
		}
	}
	
	return orientations;
}

SimBody* DistanceSensor::getHostBody() const {
	return mHostBodyObj;
}

}

