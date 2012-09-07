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



#include "LightSensor.h"
#include "Core/Core.h"
#include "Physics/DistanceSensor.h"
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
	: SimSensor(), SimBody(name), mHostBody(0), mSensorNoise(0), mBrightness(0), mLocalPosition(0),
	  mUseAsAmbientLightSensor(0), mLocalOrientation(0), mDetectableLightSourceTypes(0), mSensorObject(0)
{
	mDetectableLightTypes.append(0);

	mHostBodyName = new StringValue("");
	mSensorNoise = new DoubleValue(0.0);
	mBrightness = new InterfaceValue("", "Brightness", 0.0, 0.0, 1.0);
	mLocalPosition = new Vector3DValue();
	mUseAsAmbientLightSensor = new BoolValue(false);
	mAngleDifferences = new Vector3DValue();
	mLocalOrientation = new Vector3DValue(0,0,0);
	mTmpPosition = new Vector3DValue();
	mTmpOrienations = new Vector3DValue();
	mDetectableLightSourceTypes = new StringValue("0");

	addParameter("HostBody", mHostBodyName);
	addParameter("Noise", mSensorNoise);
	addParameter("Brightness", mBrightness);
	addParameter("LocalPosition", mLocalPosition);
	addParameter("UseAsAmbientLightSensor", mUseAsAmbientLightSensor);
	addParameter("AngleDifferences", mAngleDifferences);
	addParameter("LocalOrientation", mLocalOrientation);
	addParameter("DetectableLightTypes", mDetectableLightSourceTypes);

	addParameter("Tmp_pos", mTmpPosition);
	addParameter("Tmp_ori", mTmpOrienations);

	mOutputValues.append(mBrightness);

	mSensorObject = new CollisionObject(BoxGeom(this, 0.1, 0.1, 0.1), this, false);
	addCollisionObject(mSensorObject);
}


/**
 * Copy constructor. 
 * 
 * @param other the LightSensor object to copy.
 */
LightSensor::LightSensor(const LightSensor &other) 
	: Object(), ValueChangedListener(), SimSensor(), SimBody(other), mHostBody(0), 
		mHostBodyName(0), mDetectableLightTypes(other.mDetectableLightTypes), mSensorNoise(0), 
		mBrightness(0), mLocalPosition(0), mUseAsAmbientLightSensor(0), 
		mLocalOrientation(0), mAngleDifferences(0), mDetectableLightSourceTypes(0),
		 mSensorObject(0)
{
	mHostBodyName = dynamic_cast<StringValue*>(getParameter("HostBody"));
	mSensorNoise = dynamic_cast<DoubleValue*>(getParameter("Noise"));
	mBrightness = dynamic_cast<InterfaceValue*>(getParameter("Brightness"));
	mLocalPosition = dynamic_cast<Vector3DValue*>(getParameter("LocalPosition"));
	mUseAsAmbientLightSensor = dynamic_cast<BoolValue*>(getParameter("UseAsAmbientLightSensor"));
	mAngleDifferences = dynamic_cast<Vector3DValue*>(getParameter("AngleDifferences"));
	mLocalOrientation = dynamic_cast<Vector3DValue*>(getParameter("LocalOrientation"));
	mDetectableLightSourceTypes = dynamic_cast<StringValue*>(getParameter("DetectableLightTypes"));

	mTmpPosition = dynamic_cast<Vector3DValue*>(getParameter("Tmp_pos"));
	mTmpOrienations = dynamic_cast<Vector3DValue*>(getParameter("Tmp_ori"));

	mLightSources.clear();
	mOutputValues.append(mBrightness);

	valueChanged(mDetectableLightSourceTypes);

	mSensorObject = new CollisionObject(BoxGeom(this, 0.01, 0.01, 0.01), this, false);
	addCollisionObject(mSensorObject);
}

/**
 * Destructor.
 */
LightSensor::~LightSensor() {
	//TODO destroy mSensorObject?
	delete mSensorObject;
}


SimObject* LightSensor::createCopy() const {
	return new LightSensor(*this);
}


void LightSensor::setup() {
	SimBody::setup();

	mHostBody = Physics::getPhysicsManager()->getSimBody(mHostBodyName->get());

	if(mHostBody != 0) {
		mHostBody->getPositionValue()->addValueChangedListener(this);
		mHostBody->getQuaternionOrientationValue()->addValueChangedListener(this);
		valueChanged(mHostBody->getPositionValue());
	}

	if(mHostBody == 0) {
		Core::log(QString("LightSensor [").append(getName())
				.append("]::setup: Could not find host body [")
				.append(mHostBodyName->get()).append("]!"));
		return;
	}

	//update list of lightsources
	mLightSources.clear();
	QList<SimObject*> simObjects = Physics::getPhysicsManager()->getSimObjects();
	for(QListIterator<SimObject*> i(simObjects); i.hasNext();) {
		LightSource *lightSource = dynamic_cast<LightSource*>(i.next());

		if(lightSource != 0 && !mLightSources.contains(lightSource)) {
			if(mDetectableLightTypes.contains(lightSource->getType())) {
				mLightSources.append(lightSource);
			}
		}
	}
}


void LightSensor::clear() {
	SimBody::clear();

	if(mHostBody != 0) {
		mHostBody->getPositionValue()->removeValueChangedListener(this);
		mHostBody->getQuaternionOrientationValue()->removeValueChangedListener(this);
	}
	mHostBody = 0;

	mLightSources.clear();
}


void LightSensor::updateSensorValues() {

	if(mHostBody == 0) {
		return;
	}

	Quaternion localPos(0.0, 
						mLocalPosition->getX(), 
						mLocalPosition->getY(), 
						mLocalPosition->getZ());
	Quaternion bodyOrientationInverse = 
		mHostBody->getQuaternionOrientationValue()->get().getInverse();
	Quaternion rotatedLocalPosQuat = mHostBody->getQuaternionOrientationValue()->get() 
		* localPos * bodyOrientationInverse;
	Vector3D rotatedLocalPos(rotatedLocalPosQuat.getX(), 
							 rotatedLocalPosQuat.getY(), 
							 rotatedLocalPosQuat.getZ());
	Vector3D globalPos = mHostBody->getPositionValue()->get() + rotatedLocalPos;

	double brightness = 0.0;
	for(QListIterator<LightSource*> i(mLightSources); i.hasNext();) {
		brightness += calculateBrightness(i.next(), globalPos);
	}
	brightness = Math::calculateGaussian(brightness, mSensorNoise->get());

	mBrightness->set(brightness);
}


void LightSensor::valueChanged(Value *value) {
	SimBody::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mDetectableLightSourceTypes) {
		mDetectableLightTypes.clear();
		QStringList entries = mDetectableLightSourceTypes->get().split(",");
		for(int i = 0; i < entries.size(); ++i) {
			bool ok = true;
			int type = entries.at(i).toInt(&ok);
			if(ok) {
				mDetectableLightTypes.append(type);
			}
		}
	}
	else if(mHostBody != 0 
			&& (value == mHostBody->getPositionValue() || value == mHostBody->getQuaternionOrientationValue()))
	{
		Quaternion localPos(0.0, 
						mLocalPosition->getX(), 
						mLocalPosition->getY(), 
						mLocalPosition->getZ());
		Quaternion bodyOrientationInverse = 
			mHostBody->getQuaternionOrientationValue()->get().getInverse();
		Quaternion rotatedLocalPosQuat = mHostBody->getQuaternionOrientationValue()->get() 
			* localPos * bodyOrientationInverse;
		Vector3D rotatedLocalPos(rotatedLocalPosQuat.getX(), 
								rotatedLocalPosQuat.getY(), 
								rotatedLocalPosQuat.getZ());
		mPositionValue->set(mHostBody->getPositionValue()->get() + rotatedLocalPos);
		Vector3D angle = mHostBody->getOrientationValue()->get() + mLocalOrientation->get();
		while(angle.getX() > 180.0) {
			angle.setX(angle.getX() - 360.0);
		}
		while(angle.getX() < -180.0) {
			angle.setX(angle.getX() + 360.0);
		}
		while(angle.getY() > 180.0) {
			angle.setY(angle.getY() - 360.0);
		}
		while(angle.getY() < -180.0) {
			angle.setY(angle.getY() + 360.0);
		}
		while(angle.getZ() > 180.0) {
			angle.setZ(angle.getZ() - 360.0);
		}
		while(angle.getZ() < -180.0) {
			angle.setZ(angle.getZ() + 360.0);
		}
		mOrientationValue->set(angle);
	}
}



SimBody* LightSensor::getHostBody() const {
	return mHostBody;
}


double LightSensor::calculateBrightness(LightSource *lightSource, const Vector3D &globalPosition) {
	double brightness = 0.0;

	if(mHostBody == 0) {
		return brightness;
	}

// 	double distance = Math::distance(globalPosition, lightSource->getPositionValue()->get());
// 
// 	double range = lightSource->getRange();
// 
// 	if(range == 0.0 || distance > range) {
// 		return brightness;
// 	}
// 
// 	//currently the light is assumed to decay linearly 
// 	//TODO should be changed to 1/(r*r).
// 	double ambientLight = (range - distance) / range * lightSource->getCurrentBrightness();

	//let the light source calculate its current brightness at the desired global position.
	double ambientLight = lightSource->getBrightness(globalPosition);

	if(mUseAsAmbientLightSensor->get()) {
		brightness = ambientLight;
	}
	else {
		//take orientation into account.

		Vector3D sensorPosition = globalPosition;
		Vector3D sensorOrientation = mHostBody->getOrientationValue()->get() + mLocalOrientation->get();

		mTmpPosition->set(sensorPosition);
		mTmpOrienations->set(sensorOrientation);

		Vector3D anglesToLightSource;

		anglesToLightSource.setX((atan2(
				(lightSource->getPositionValue()->getZ() - sensorPosition.getZ()),
 				(lightSource->getPositionValue()->getY() - sensorPosition.getY())) 
				/ Math::PI * 180.0) - sensorOrientation.getX());


		anglesToLightSource.setY((atan2(
				(lightSource->getPositionValue()->getX() - sensorPosition.getX()),
 				(lightSource->getPositionValue()->getZ() - sensorPosition.getZ())) 
				/ Math::PI * 180.0) - sensorOrientation.getY());

		
		anglesToLightSource.setZ((atan2(
				(lightSource->getPositionValue()->getY() - sensorPosition.getY()),
 				(lightSource->getPositionValue()->getX() - sensorPosition.getX())) 
				/ Math::PI * 180.0) - sensorOrientation.getZ());



		while(anglesToLightSource.getX() > 180.0) {
			anglesToLightSource.setX(anglesToLightSource.getX() - 360.0);
		}
		while(anglesToLightSource.getX() < -180.0) {
			anglesToLightSource.setX(anglesToLightSource.getX() + 360.0);
		}
		while(anglesToLightSource.getY() > 180.0) {
			anglesToLightSource.setY(anglesToLightSource.getY() - 360.0);
		}
		while(anglesToLightSource.getY() < -180.0) {
			anglesToLightSource.setY(anglesToLightSource.getY() + 360.0);
		}
		while(anglesToLightSource.getZ() > 180.0) {
			anglesToLightSource.setZ(anglesToLightSource.getZ() - 360.0);
		}
		while(anglesToLightSource.getZ() < -180.0) {
			anglesToLightSource.setZ(anglesToLightSource.getZ() + 360.0);
		}

		mAngleDifferences->set(anglesToLightSource);

		ambientLight = ambientLight / 180.0;

		double maxDifference = Math::abs(anglesToLightSource.getY());

		brightness = (180.0 - maxDifference) * ambientLight;

// 		brightness = (((180.0 - Math::abs(anglesToLightSource.getX())) / 3.0)
// 					+ ((180.0 - Math::abs(anglesToLightSource.getY())) / 3.0)
// 					+ ((180.0 - Math::abs(anglesToLightSource.getZ())) / 3.0)) * ambientLight;
	}

	return brightness;
}

}



