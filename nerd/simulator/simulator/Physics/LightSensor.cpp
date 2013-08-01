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



#include "LightSensor.h"
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Physics/SimSensor.h"
#include <iostream>
#include "Math/Math.h"
#include <math.h>
#include <QStringList>
#include "Physics/BoxGeom.h"

using namespace std;

namespace nerd {

/**
 * Constructs a new LightSensor.
 */
LightSensor::LightSensor(const QString &name)
	: SimSensor(), SimBody(name), mHostBody(0),
	  mNoise(0), mBrightness(0), mLocalPosition(0),
	  mAmbientSensor(0), mLocalOrientation(0),
	  mDetectableTypes(0), mSensorObject(0)
{
	double minDetectBrightness = 0.0;
	double maxDetectBrightness = 1.0;
	
	mDetectableTypesList.append(0);

	mHostBodyName = new StringValue("");
	mNoise = new DoubleValue(0.0);
	mBrightness = new InterfaceValue("", "Brightness",
			minDetectBrightness, minDetectBrightness, maxDetectBrightness);
	mLocalPosition = new Vector3DValue();
	mAmbientSensor = new BoolValue(false);
	mLocalOrientation = new Vector3DValue(0,0,0);
	mDetectableTypes = new StringValue("0");
	mRestrictToPlane = new BoolValue(true);
	mDetectionAngle = new DoubleValue(180.0);
	mDetectableRange = new RangeValue(minDetectBrightness, maxDetectBrightness);
	
	mHostBodyName->setDescription(
			"The full name of the body this light sensor is attached to.");
	mBrightness->setDescription("The measured brightness");
	mLocalPosition->setDescription(
			"The position offset between the sensor and the host body");
	mLocalOrientation->setDescription(
			"The orientation offset between the sensor and the host body");
	mAmbientSensor->setDescription(
			"If true, then the measured brightness is orientation independent. "
			"Otherwise, the brightness is dependent on how the sensor is directed "
			"relative to the light sources. ");
	mDetectableTypes->setDescription(
			"The ids of the light types that can be detected by this sensor.");
	mRestrictToPlane->setDescription(
			"If true, then only the horizontal orientation is used for "
			"the brightness calculation in directed light sensor mode");
	mDetectionAngle->setDescription(
			"If true, then a directed light sensor does not sense any light "
			"if the given angular offset is exceeded.");
	mDetectableRange->setDescription(
			"The minimal and maximal brightness detectable by this sensor");

	addParameter("HostBodyName", mHostBodyName);
	addParameter("Noise", mNoise);
	addParameter("Brightness", mBrightness);
	addParameter("LocalPosition", mLocalPosition);
	addParameter("LocalOrientation", mLocalOrientation);
	addParameter("AmbientSensor", mAmbientSensor);
	addParameter("DetectableTypes", mDetectableTypes);
	addParameter("RestrictToPlane", mRestrictToPlane);
	addParameter("DetectionAngle", mDetectionAngle);

	addParameter("DetectableRange", mDetectableRange);

	mOutputValues.append(mBrightness);

	mSensorObject = new CollisionObject(
			BoxGeom(this, 0.1, 0.1, 0.1), this, false);
	addCollisionObject(mSensorObject);
	
	Physics::getPhysicsManager();
	mSwitchYZAxes = Core::getInstance()->getValueManager()->
		getBoolValue(SimulationConstants::VALUE_SWITCH_YZ_AXES);
}


/**
 * Copy constructor. 
 * 
 * @param other the LightSensor object to copy.
 */
LightSensor::LightSensor(const LightSensor &other) 
	: Object(), ValueChangedListener(), SimSensor(), SimBody(other),
	  mHostBody(0), mHostBodyName(0),
	  mDetectableTypesList(other.mDetectableTypesList), mNoise(0),
	  mBrightness(0), mLocalPosition(0), mAmbientSensor(0),
	  mLocalOrientation(0), mDetectableTypes(0), mSensorObject(0)
{
	mHostBodyName = dynamic_cast<StringValue*>(getParameter("HostBodyName"));
	mNoise = dynamic_cast<DoubleValue*>(getParameter("Noise"));
	mBrightness = dynamic_cast<InterfaceValue*>(getParameter("Brightness"));
	mLocalPosition =
		dynamic_cast<Vector3DValue*>(getParameter("LocalPosition"));
	mAmbientSensor = 
		dynamic_cast<BoolValue*>(getParameter("AmbientSensor"));
	mLocalOrientation =
		dynamic_cast<Vector3DValue*>(getParameter("LocalOrientation"));
	mDetectableTypes =
		dynamic_cast<StringValue*>(getParameter("DetectableTypes"));
	mRestrictToPlane =
		dynamic_cast<BoolValue*>(getParameter("RestrictToPlane"));
	mDetectionAngle =
		dynamic_cast<DoubleValue*>(getParameter("DetectionAngle"));
	mDetectableRange =
		dynamic_cast<RangeValue*>(getParameter("DetectableRange"));

	mLightSources.clear();
	mOutputValues.append(mBrightness);
	
	mSwitchYZAxes = other.mSwitchYZAxes;

	mSensorObject = new CollisionObject(
			BoxGeom(this, 0.1, 0.1, 0.1), this, false);
	addCollisionObject(mSensorObject);
}

/**
 * Destructor.
 */
LightSensor::~LightSensor() {
	removeCollisionObject(mSensorObject);
	delete mSensorObject;
}


SimObject* LightSensor::createCopy() const {
	return new LightSensor(*this);
}


void LightSensor::setup() {
	SimBody::setup();
	
	// get host body object and its position
	//
	if(mHostBody != 0) {
		mHostBody->getPositionValue()->
			removeValueChangedListener(this);
		mHostBody->getQuaternionOrientationValue()->
			removeValueChangedListener(this);
		mHostBody = 0;
	}

	if(!mHostBodyName->get().isEmpty()) {
		mHostBody = Physics::getPhysicsManager()->
			getSimBody(mHostBodyName->get());

		if(mHostBody == 0) {
			Core::log("LightSensor [" + getName() + 
					"]::setup: Could not find host body [" +
					mHostBodyName->get() + "]!", true);
			return;
		} else {
			mHostBody->getPositionValue()->
				addValueChangedListener(this);
			mHostBody->getQuaternionOrientationValue()->
				addValueChangedListener(this);
		}
	}

	updatePositionAndOrientation();
	collectLightSources();
}


// update list of detectable types
// and collect corresponding light sources
void LightSensor::collectLightSources() {
	mLightSources.clear();
	QList<SimObject*> simObjects =
		Physics::getPhysicsManager()->getSimObjects();

	for(QListIterator<SimObject*> i(simObjects); i.hasNext();) {
		LightSource *lightSource = dynamic_cast<LightSource*>(i.next());

		if(lightSource != 0 && !mLightSources.contains(lightSource)) {
			if(mDetectableTypesList.contains(lightSource->getType())) {
				mLightSources.append(lightSource);
			}
		}
	}
}


void LightSensor::clear() {
	SimBody::clear();

	if(mHostBody != 0) {
		mHostBody->getPositionValue()->
			removeValueChangedListener(this);
		mHostBody->getQuaternionOrientationValue()->
			removeValueChangedListener(this);
	}
	mHostBody = 0;

	mLightSources.clear();
	mDetectableTypesList.clear();
}


// inherited method from SimSensor, called in every simulation step
//
void LightSensor::updateSensorValues() {
	double brightness = 0.0;
	for(QListIterator<LightSource*> i(mLightSources); i.hasNext();) {
		brightness += calculateBrightness(i.next());
	}
	brightness = Math::calculateGaussian(brightness, mNoise->get());

	mBrightness->set(brightness);
}


void LightSensor::valueChanged(Value *value) {
	SimBody::valueChanged(value);

	if(value == 0) {
		return;
	}

	// string of detectable light types has changed, update list
	else if(value == mDetectableTypes) {
		mDetectableTypesList.clear();
		QStringList entries = mDetectableTypes->get().split(",");
		for(int i = 0; i < entries.size(); ++i) {
			bool ok = true;
			int type = entries.at(i).toInt(&ok);
			if(ok) { // only if it is an integer!
				mDetectableTypesList.append(type);
			}
		}
		// and collect the corresponding light sources
		collectLightSources();
	}

	// position/orientation of host changed
	else if(mHostBody != 0 && 
			(value == mHostBody->getPositionValue() ||
			 value == mHostBody->getQuaternionOrientationValue()))
	{
		updatePositionAndOrientation();
	}

	// local orientation or position changed
	else if(value == mLocalOrientation ||
			value == mLocalPosition)
	{
		updatePositionAndOrientation();
	}

	// Sensor angle has changed
	else if(value == mDetectionAngle) {
		// only positive values make sense
		if(mDetectionAngle->get() > 0) {
			// shift angle into range 1-360 (instead of 0-359)
			mDetectionAngle->set(
					Math::forceToDegreeRange(mDetectionAngle->get(), 1));
		}
	}

	else if(value == mDetectableRange) {
		mBrightness->setMin(mDetectableRange->getMin());
		mBrightness->setMax(mDetectableRange->getMax());
	}
}


// update the combined position/orientation values
// that are used in the calculations below
void LightSensor::updatePositionAndOrientation() {

	if(mHostBody == 0) {

		mPositionValue->set(mLocalPosition->get());
		mOrientationValue->set(mLocalOrientation->get());

	} else {

		if(mLocalPosition->get() != Vector3D(0,0,0)) {
			Quaternion localPos(0.0, 
							mLocalPosition->getX(), 
							mLocalPosition->getY(), 
							mLocalPosition->getZ());

			Quaternion bodyOrientationInverse = 
				mHostBody->getQuaternionOrientationValue()->get().getInverse();

			Quaternion rotatedLocalPosQuat =
				mHostBody->getQuaternionOrientationValue()->get() *
				localPos * bodyOrientationInverse;

			Vector3D rotatedLocalPos(rotatedLocalPosQuat.getX(),
									 rotatedLocalPosQuat.getY(),
									 rotatedLocalPosQuat.getZ());

			mPositionValue->set(
					mHostBody->getPositionValue()->get() +
					rotatedLocalPos);
		} else {
			mPositionValue->set(mHostBody->getPositionValue()->get());
		}

		Vector3D angle =
			mHostBody->getOrientationValue()->get() +
			mLocalOrientation->get();
		
		mOrientationValue->set(Math::forceToDegreeRange(angle));
	}
}		


SimBody* LightSensor::getHostBody() const {
	return mHostBody;
}


double LightSensor::calculateBrightness(LightSource *lightSource) {

	// get position values
	Vector3D sensorPosition = mPositionValue->get();
	Vector3D lightPosition = lightSource->getPositionValue()->get();

	// get brightness at current position from light source
	double sourceBrightness =
		lightSource->getBrightness(sensorPosition, mRestrictToPlane->get());

	// if ambient sensor, ignore directionality and opening angle
	if(mAmbientSensor->get()) {
		return sourceBrightness;
	}

	// Otherwise, take the directionality into account
	double angleDiff = 0.0;

	// 2-D case
	if(mRestrictToPlane->get()) {

		Vector3D sensorOrientation = mOrientationValue->get();

		if(mSwitchYZAxes != 0 && mSwitchYZAxes->get()) {
			angleDiff = atan2(
						lightPosition.getX() - sensorPosition.getX(),
						lightPosition.getZ() - sensorPosition.getZ()) /
						Math::PI * 180.0 - sensorOrientation.getY();
		}
		else {
			angleDiff = atan2(
						lightPosition.getX() - sensorPosition.getX(),
						lightPosition.getY() - sensorPosition.getY()) /
						Math::PI * 180.0 - sensorOrientation.getZ();
		}
	}
	// 3-D case
	else {
		// solution:
		// * rotate (0,0,1) according to sensor orientation
		// 		(0,0,1) is the default "looking direction"
		// * get vector from sensor to source
		// * calculate angle between those two
		// using: cos(angle) = dot(a,b)/(length(a)*length(b))
		//

		// create quaternion of default direction vector
		Quaternion defDir(0,0,0,1);
		if(mSwitchYZAxes == 0 || !mSwitchYZAxes->get()) {
			defDir.set(0,0,1,0);
		}

		// get orientation quaternion of sensor and its inverse
		Quaternion orientQuat = mQuaternionOrientationValue->get();
		Quaternion orientQuatInv = orientQuat.getInverse();

		// calculate vector for sensor's "looking direction"
		Quaternion rotDir = orientQuat * defDir * orientQuatInv;
		Vector3D rotVec(rotDir.getX(), rotDir.getY(), rotDir.getZ());
		
		// get vector from sensor to light source
		Vector3D sensorToSource = lightPosition - sensorPosition;

		double dotP = sensorToSource * rotVec;
		double vecL = sensorToSource.length() * rotVec.length();

		angleDiff = acos( dotP / vecL ) / Math::PI * 180.0;
	}

	angleDiff = Math::abs(Math::forceToDegreeRange(angleDiff, -180));

	// linear decay of measured light intensity
	// from center of sensor axis to the edges
	// TODO use quadratic decay instead?
	//
	// get whole detection angle, divide by two for each side
	double detectionAngle = mDetectionAngle->get() / 2;

	double factor = detectionAngle - Math::min(detectionAngle, angleDiff);
	double brightness = factor * sourceBrightness / detectionAngle;

	return brightness;
}

}

