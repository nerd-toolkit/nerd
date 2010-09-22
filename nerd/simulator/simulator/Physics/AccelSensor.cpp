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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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


#include "AccelSensor.h"
#include "Event/Event.h"
#include "Math/Vector3D.h"
#include "Math/Math.h"
#include "Value/Vector3DValue.h"
#include "Value/InterfaceValue.h"
#include "SimBody.h"
#include "BoxGeom.h"
#include "Core/Core.h"
#include "Collision/CollisionObject.h"
#include "Physics/Physics.h"
#include <math.h>

namespace nerd {

/**
 * Create a new acceleration sensor with default behavior of ADXL213. The parameters:
 * "LocalPosition", "LocalOrientation", "Measurements", "Axis1", "Axis2" and 
 * "Noise/Deviation" can be used to modify the sensor.
 * @param name Name of the specific acceleration sensor.
 */
AccelSensor::AccelSensor(const QString &name, int numberOfAxes) 
	: SimSensor(), SimObject(name), mHostBody(0), 
		mSensorGeometry(0), mSensorBody(0), mTimeStepSize(0), mGravitationValue(0),
		mNumberOfAxes(numberOfAxes)
{
	mColor.set(0, 50, 0, 244);
	mLastPosition.set(0.0, 0.0, 0.0);
	mPosition.set(0.0, 0.0, 0.0);
	mLastVelocity.set(0.0, 0.0, 0.0);
	mVelocity.set(0.0, 0.0, 0.0);

	mMaxSensorValue = 11.772;
	double maxSensorMappingValue = 16.072704;

	mLocalOrientation = new QuaternionValue();
	mLocalPosition = new Vector3DValue();

	if(numberOfAxes == 3) {
		mSensorSizeValue = new Vector3DValue(0.045, 0.008, 0.051);
	} else {
		mSensorSizeValue = new Vector3DValue(0.026, 0.0125, 0.0383);
	}

	mSensorAxisOneValue = new Vector3DValue(0.0, 0.0, 1.0);
	mSensorAxisTwoValue = new Vector3DValue(1.0, 0.0, 0.0);
	mSensorAxisThreeValue = new Vector3DValue(0.0, -1.0, 0.0);

	mReferenceBodyName = new StringValue();

	mFirstSensorValue = new InterfaceValue(getName(), "AccelAxis1");
	mSecondSensorValue = new InterfaceValue(getName(), "AccelAxis2");
	mThirdSensorValue = new InterfaceValue(getName(), "AccelAxis3");

	mFirstSensorValue->setMin(-maxSensorMappingValue);
	mFirstSensorValue->setMax(maxSensorMappingValue);
	mSecondSensorValue->setMin(-maxSensorMappingValue);
	mSecondSensorValue->setMax(maxSensorMappingValue);
	mThirdSensorValue->setMin(-maxSensorMappingValue);
	mThirdSensorValue->setMax(maxSensorMappingValue);

	mFirstSensorValue->setNormalizedMin(-1);
	mFirstSensorValue->setNormalizedMax(1);
	mSecondSensorValue->setNormalizedMin(-1);
	mSecondSensorValue->setNormalizedMax(1);
	mThirdSensorValue->setNormalizedMin(-1);
	mThirdSensorValue->setNormalizedMax(1);

	addParameter("LocalPosition", mLocalPosition);
	addParameter("LocalOrientation", mLocalOrientation);
	addParameter("Axis1", mSensorAxisOneValue);
	addParameter("Axis2", mSensorAxisTwoValue);
	addParameter("Axis3", mSensorAxisThreeValue);
	addParameter("Measurements", mSensorSizeValue);
	addParameter("AccelAxis1", mFirstSensorValue);
	addParameter("AccelAxis2", mSecondSensorValue);
	addParameter("AccelAxis3", mThirdSensorValue);
	addParameter("ReferenceBody", mReferenceBodyName);

	//add correct number of axis to the interface.
	if(mNumberOfAxes > 0) {
		mOutputValues.append(mFirstSensorValue);
	}
	if(mNumberOfAxes > 1) {
		mOutputValues.append(mSecondSensorValue);
	}
	if(mNumberOfAxes > 2) {
		mOutputValues.append(mThirdSensorValue);
	}


	mGravitation = 0.0;
	mLastAxisOneMeasurement = 0.0;
	mLastAxisTwoMeasurement = 0.0;
	mLastAxisThreeMeasurement = 0.0;

	if(Core::getInstance()->getValueManager()->getValue(
		"/AccelSensor/LowPassFilterDelay") == 0) 
	{
		mLowPassFilterDelayValue = new IntValue(1);
		Core::getInstance()->getValueManager()->addValue(
			"/AccelSensor/LowPassFilterDelay", mLowPassFilterDelayValue);
	}
	else {
		mLowPassFilterDelayValue = dynamic_cast<IntValue*>(Core::getInstance()->
			getValueManager()->getValue("/AccelSensor/LowPassFilterDelay"));
	}
	if(Core::getInstance()->getValueManager()->getValue("/AccelSensor/Deviation") == 0) {
		mGlobalNoiseDeviationValue = new DoubleValue(0.0001);
		Core::getInstance()->getValueManager()->addValue(
			"/AccelSensor/Deviation", mGlobalNoiseDeviationValue);
	}
	else {
		mGlobalNoiseDeviationValue = dynamic_cast<DoubleValue*>(Core::getInstance()->
			getValueManager()->getValue("/AccelSensor/Deviation"));
	}
	if(mLowPassFilterDelayValue == 0 || mGlobalNoiseDeviationValue == 0) {
		Core::log("AccelSensor: Could not find required Value.");
	}
	mGlobalNoiseDeviation = mGlobalNoiseDeviationValue->get();
	mInitialized = false;
}

/**
 * Copy constructor. Creates a deep copy of the AccelSensor object. All parameters and the
 * settings of the InterfaceValues are adpoted by the copy.
 * @param sensor 
 */
AccelSensor::AccelSensor(const AccelSensor &sensor) : SimSensor(sensor), Object(),
		ValueChangedListener(), SimObject(sensor), mHostBody(0), mSensorBody(0), 
		mGravitationValue(0), mNumberOfAxes(sensor.mNumberOfAxes)
{
	mMaxSensorValue = 11.772;
	mLocalPosition = dynamic_cast<Vector3DValue*>(getParameter("LocalPosition"));
	mLocalOrientation = dynamic_cast<QuaternionValue*>(getParameter("LocalOrientation"));
	mSensorAxisOneValue = dynamic_cast<Vector3DValue*>(getParameter("Axis1"));
	mSensorAxisTwoValue = dynamic_cast<Vector3DValue*>(getParameter("Axis2"));
	mSensorAxisThreeValue = dynamic_cast<Vector3DValue*>(getParameter("Axis3"));
	mSensorSizeValue = dynamic_cast<Vector3DValue*>(getParameter("Measurements"));
	mFirstSensorValue = dynamic_cast<InterfaceValue*>(getParameter("AccelAxis1"));
	mSecondSensorValue = dynamic_cast<InterfaceValue*>(getParameter("AccelAxis2"));
	mThirdSensorValue = dynamic_cast<InterfaceValue*>(getParameter("AccelAxis3"));
	
	mReferenceBodyName = dynamic_cast<StringValue*>(getParameter("ReferenceBody"));
	mReferenceBodyName->set("");

	mLowPassFilterDelayValue = dynamic_cast<IntValue*>(Core::getInstance()->
		getValueManager()->getValue("/AccelSensor/LowPassFilterDelay"));

	mGlobalNoiseDeviationValue = dynamic_cast<DoubleValue*>(Core::getInstance()->
		getValueManager()->getValue("/AccelSensor/Deviation"));

	if(mLowPassFilterDelayValue == 0 || mGlobalNoiseDeviationValue == 0) {
		Core::log("AccelSensor: Could not find required Value.");
	}
	mColor = sensor.mColor;
	if(mNumberOfAxes > 0) {
		mOutputValues.append(mFirstSensorValue);
	}
	if(mNumberOfAxes > 1) {
		mOutputValues.append(mSecondSensorValue);
	}
	if(mNumberOfAxes > 2) {
		mOutputValues.append(mThirdSensorValue);
	}
	mSensorBody = 0;
	mSensorGeometry = 0;
	mGravitation = 0.0;
	mGlobalNoiseDeviation = mGlobalNoiseDeviationValue->get();
	mInitialized = false;
}

AccelSensor::~AccelSensor() {
	if(mSensorBody != 0) {
		delete mSensorBody;
	}
}

SimObject* AccelSensor::createCopy() const {
	return new AccelSensor(*this);
}

void AccelSensor::setup() {
	SimObject::setup();

	if(mInitialized == false) {
		mHostBody = Physics::getPhysicsManager()->getSimBody(mReferenceBodyName->get());
		if(mHostBody == 0) {
			Core::log("AccelSensor: Sensor has no valid reference body.");
			return;
		}
		createSensor();	
		mFirstSensorValue->set(0.0);
		mSecondSensorValue->set(0.0);
		mThirdSensorValue->set(0.0);

		mTimeStepSize = dynamic_cast<DoubleValue*>(Core::getInstance()->
			getValueManager()->getValue(SimulationConstants::VALUE_TIME_STEP_SIZE));
		DoubleValue *mGravitationValue = dynamic_cast<DoubleValue*>(Core::getInstance()->
			getValueManager()->getValue("/Simulation/Gravitation"));
		if(mTimeStepSize == 0 || mGravitationValue == 0) {
			Core::log("AccelSensor: Required Event or Value could not be found.");
			return;
		}
		mGravitation = mGravitationValue->get();
		mLastAxisOneMeasurement = 0.0;
		mLastAxisTwoMeasurement = 0.0;
		mLastAxisThreeMeasurement = 0.0;
		mInitialized = true;
	}
// 	rotate chosen axis about the localRotation of this sensor
	Quaternion localOrientation = mLocalOrientation->get();
	localOrientation.normalize();
	mLocalOrientation->set(localOrientation);
	Quaternion localOrientationInverse = localOrientation.getInverse();
	localOrientationInverse.normalize();

	Quaternion xAxis(0.0, 
					mSensorAxisOneValue->getX(), 
					mSensorAxisOneValue->getY(), 
					mSensorAxisOneValue->getZ());
	Quaternion xAxisRotated = localOrientation * xAxis * localOrientationInverse;
	mSensorAxisOne.set(xAxisRotated.getX(), xAxisRotated.getY(), xAxisRotated.getZ());
	mRotatedSensorAxisOne.set(xAxisRotated.getX(), 
							  xAxisRotated.getY(), 
							  xAxisRotated.getZ());

	Quaternion yAxis(0.0, 
					mSensorAxisTwoValue->getX(), 
					mSensorAxisTwoValue->getY(), 
					mSensorAxisTwoValue->getZ());
	Quaternion yAxisRotated = localOrientation * yAxis * localOrientationInverse;
	mSensorAxisTwo.set(yAxisRotated.getX(), yAxisRotated.getY(), yAxisRotated.getZ());
	mRotatedSensorAxisTwo.set(yAxisRotated.getX(), 
							  yAxisRotated.getY(), 
							  yAxisRotated.getZ());

	Quaternion zAxis(0.0, 
					mSensorAxisThreeValue->getX(), 
					mSensorAxisThreeValue->getY(), 
					mSensorAxisThreeValue->getZ());
	Quaternion zAxisRotated = localOrientation * zAxis * localOrientationInverse;
	mSensorAxisThree.set(zAxisRotated.getX(), zAxisRotated.getY(), zAxisRotated.getZ());
	mRotatedSensorAxisThree.set(zAxisRotated.getX(), 
							  zAxisRotated.getY(), 
							  zAxisRotated.getZ());

	if(mSensorBody == 0 || mHostBody == 0) {
		Core::log("AccelSensor: The sensor has no valid host or sensor-body.");
		return;
	}	

	mSensorBody->setTextureType("None");
	mSensorBody->setFaceTexture(5, "AccelBoard");
	mSensorGeometry->setLocalOrientation(mLocalOrientation->get());
	mSensorBody->getGeometry()->setLocalPosition(mLocalPosition->get());
	Quaternion localPos(0, 
						mLocalPosition->getX(), 
						mLocalPosition->getY(), 
						mLocalPosition->getZ());
	Quaternion bodyOrientationInverse = mHostBody->getQuaternionOrientationValue()->get().getInverse();
	Quaternion rotatedLocalPosQuat = mHostBody->getQuaternionOrientationValue()->get() 
		* localPos * bodyOrientationInverse;
	Vector3D rotatedLocalPos(rotatedLocalPosQuat.getX(), 
							 rotatedLocalPosQuat.getY(), 
							 rotatedLocalPosQuat.getZ());
	mLastPosition = mHostBody->getPositionValue()->get() + rotatedLocalPos;

	if(mLowPassFilterDelayValue != 0) {
		mLowPassFilterDelay = mLowPassFilterDelayValue->get();
	}
	mValueOneHistory.clear();
	mValueTwoHistory.clear();
	mValueThreeHistory.clear();

	mGlobalNoiseDeviation = mGlobalNoiseDeviationValue->get();

}

void AccelSensor::clear() {
	SimObject::clear();
	mLastPosition.set(0.0, 0.0, 0.0);
	mPosition.set(0.0, 0.0, 0.0);
	mLastVelocity.set(0.0, 0.0, 0.0);
	mVelocity.set(0.0, 0.0, 0.0);
	mFirstSensorValue->set(0.0);
	mSecondSensorValue->set(0.0);
	mThirdSensorValue->set(0.0);
	mLastAxisOneMeasurement = 0.0;
	mLastAxisTwoMeasurement = 0.0;
	mLastAxisThreeMeasurement = 0.0;
}


/**
 *
 * @param host SimBody to which this sensor is attached. If not defined differently the 
 * sensor will be located at the position of the body. Use the parameters "LocalPosition" 
 * and LocalOrientation" to change position and orientation of the sensor.
 * @param axis1 First Axis along which the acceleration will be reported. The axis is 
 * normalized to length 1 initially.
 * @param axis2 Second Axis along which the acceleration will be reported. The axis is 
 * normalized to length 1 initially.
 * @return
 */
bool AccelSensor::createSensor() {
	Vector3D normalized1 = mSensorAxisOneValue->get();
	normalized1.normalize();
	Vector3D normalized2 = mSensorAxisTwoValue->get();
	normalized2.normalize();
	Vector3D normalized3 = mSensorAxisThreeValue->get();
	normalized3.normalize();
	mSensorAxisOneValue->set(normalized1);
	mSensorAxisTwoValue->set(normalized2);
	mSensorAxisThreeValue->set(normalized3);

	mSensorBody = new CollisionObject(BoxGeom(this, mSensorSizeValue->getX(),
		mSensorSizeValue->getY(), mSensorSizeValue->getZ()), this);
	mSensorGeometry = dynamic_cast<BoxGeom*>(mSensorBody->getGeometry());
	mSensorGeometry->setAutomaticColor(false);
	if(mSensorGeometry == 0) {
		Core::log("AccelSensor: Error while creating AccelSensor.");
		return false;
	}
	//TODO: Material of AccelSensors might be modified.
	mSensorBody->setMaterialType("ABS");
	mSensorBody->setMass(0.011);
	mSensorGeometry->setColor(mColor);
	mSensorGeometry->enableDrawing(true);
	mSensorBody->setOwner(this);
	mSensorBody->setHostBody(mHostBody);
	mHostBody->addCollisionObject(mSensorBody);
	mSensorGeometry->setLocalOrientation(mLocalOrientation->get());
	mSensorGeometry->setLocalPosition((mLocalPosition->get()));

	return true;
}


void AccelSensor::updateSensorValues() {

	if(mHostBody == 0) {
		return;
	}
	if(mTimeStepSize->get() == 0.0) {
		Core::log("AccelSensor warning: Time step size was 0.0, prevented division by zero.");
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
	mPosition = mHostBody->getPositionValue()->get() + rotatedLocalPos;

	mVelocity = (mPosition - mLastPosition) * (1.0 / mTimeStepSize->get());
	mAcceleration = (mVelocity - mLastVelocity) * (1.0 / mTimeStepSize->get());

	mAcceleration.setY(mAcceleration.getY() + mGravitation);

	Quaternion bodyRotation = mHostBody->getQuaternionOrientationValue()->get();
	bodyRotation.normalize();
	Quaternion bodyRotationInverse = bodyRotation.getInverse();
	bodyRotationInverse.normalize();

	//Axis 1
	Quaternion xAxisQuat(0.0, 
						mSensorAxisOne.getX(), 
						mSensorAxisOne.getY(), 
						mSensorAxisOne.getZ());
	Quaternion xAxisRotatedQuat = bodyRotation * xAxisQuat * bodyRotationInverse;

	mRotatedSensorAxisOne.set(xAxisRotatedQuat.getX(), 
							  xAxisRotatedQuat.getY(),
							  xAxisRotatedQuat.getZ());

	//Axis 2
	Quaternion yAxisQuat(0.0, 
						mSensorAxisTwo.getX(), 
						mSensorAxisTwo.getY(),
					 	mSensorAxisTwo.getZ());
	Quaternion yAxisRotatedQuat = bodyRotation * yAxisQuat * bodyRotationInverse;

	mRotatedSensorAxisTwo.set(yAxisRotatedQuat.getX(), 
							  yAxisRotatedQuat.getY(),
							  yAxisRotatedQuat.getZ());

	//Axis 3
	Quaternion zAxisQuat(0.0, 
						mSensorAxisThree.getX(), 
						mSensorAxisThree.getY(),
					 	mSensorAxisThree.getZ());
	Quaternion zAxisRotatedQuat = bodyRotation * zAxisQuat * bodyRotationInverse;

	mRotatedSensorAxisThree.set(zAxisRotatedQuat.getX(), 
							  zAxisRotatedQuat.getY(),
							  zAxisRotatedQuat.getZ());



	double sensorValue1 = mAcceleration.getX() * xAxisRotatedQuat.getX() 
		+ mAcceleration.getY() * xAxisRotatedQuat.getY() 
		+ mAcceleration.getZ() * xAxisRotatedQuat.getZ();
	sensorValue1 = -1 * sensorValue1;

	double sensorValue2 = mAcceleration.getX() * yAxisRotatedQuat.getX() 
		+ mAcceleration.getY() * yAxisRotatedQuat.getY() 
		+ mAcceleration.getZ() * yAxisRotatedQuat.getZ();
	sensorValue2 = -1 * sensorValue2;

	double sensorValue3 = mAcceleration.getX() * zAxisRotatedQuat.getX() 
		+ mAcceleration.getY() * zAxisRotatedQuat.getY() 
		+ mAcceleration.getZ() * zAxisRotatedQuat.getZ();
	sensorValue3 = -1 * sensorValue3;



	// freeze all sensorValues if one of them exceeds the maximum value.
	if(fabs(sensorValue1) > mMaxSensorValue 
		|| (fabs(sensorValue2) > mMaxSensorValue && mNumberOfAxes > 1)
		|| (fabs(sensorValue3) > mMaxSensorValue && mNumberOfAxes > 2)) 
	{
		sensorValue1 = mLastAxisOneMeasurement;
		sensorValue2 = mLastAxisTwoMeasurement;
		sensorValue3 = mLastAxisThreeMeasurement;
	} 
	else {
		mLastAxisOneMeasurement = sensorValue1;
		mLastAxisTwoMeasurement = sensorValue2;
		mLastAxisThreeMeasurement = sensorValue3;
	}

	if(mValueOneHistory.size() >= mLowPassFilterDelay) {
		mValueOneHistory.pop_front();
		mValueTwoHistory.pop_front();
		mValueThreeHistory.pop_front();
	}
	mValueOneHistory.push_back(sensorValue1);
	mValueTwoHistory.push_back(sensorValue2);
	mValueThreeHistory.push_back(sensorValue3);

	// perform low pass filtering
	QList<double> result = calculateFilteredSensorValues(sensorValue1, sensorValue2, sensorValue3);

	sensorValue1 = result.at(0);
	sensorValue2 = result.at(1);
	sensorValue3 = result.at(2);

	// add noise
	sensorValue1 = Math::calculateGaussian(sensorValue1, mGlobalNoiseDeviation);
	sensorValue2 = Math::calculateGaussian(sensorValue2, mGlobalNoiseDeviation);
	sensorValue3 = Math::calculateGaussian(sensorValue3, mGlobalNoiseDeviation);

	mFirstSensorValue->set(sensorValue1);
	mSecondSensorValue->set(sensorValue2);
	mThirdSensorValue->set(sensorValue3);

	mLastVelocity = mVelocity;
	mLastPosition = mPosition;

}


/**
 * Returns the first sensor axis. The axis can be set during creating the sensor (createSensor()). Default setting is: (1,0,0)
 * @return The first axis along which the acceleration is measured.
 */
Vector3D AccelSensor::getAxisOne() const {
	return mRotatedSensorAxisOne;
}

/**
 * Returns the second sensor axis. The axis can be set during creating the sensor. Default setting is: (0,0,1)
 * @return  The second axis along which the acceleration is measured.
 */
Vector3D AccelSensor::getAxisTwo() const {
	return mRotatedSensorAxisTwo;
}


/**
 * Returns the third sensor axis. The axis can be set during creating the sensor. Default setting is: (0,0,1)
 * @return  The third axis along which the acceleration is measured.
 */
Vector3D AccelSensor::getAxisThree() const { 
	return mRotatedSensorAxisThree;
}


/**
 * 
 * @return The current position of the AccelSensor-object with resepect to the simulation origin.
 */
Vector3D AccelSensor::getGlobalPosition() const {
	return mPosition;
}

QList<double> AccelSensor::calculateFilteredSensorValues(double sensorValue1, 
		double sensorValue2, double sensorValue3) 
{
	QList<double> result;
	int windowSize = mValueOneHistory.size();
	if(windowSize > 0) {
		double sumOne = 0.0;
		double sumTwo = 0.0;
		double sumThree = 0.0;
		for(int i = 0; i < windowSize; i++) {
			sumOne += mValueOneHistory.at(i);
			sumTwo += mValueTwoHistory.at(i);
			sumThree += mValueThreeHistory.at(i);
		}
		result.push_back(sumOne / mLowPassFilterDelay);
		result.push_back(sumTwo / mLowPassFilterDelay);
		result.push_back(sumThree / mLowPassFilterDelay);
	} 
	else {
		result.push_back(sensorValue1);
		result.push_back(sensorValue2);
		result.push_back(sensorValue3);
	}
	return result;
}

}
