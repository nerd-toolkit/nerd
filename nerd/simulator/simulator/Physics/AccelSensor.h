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

#ifndef AccelSensor_H_
#define AccelSensor_H_

#include "SimObject.h"
#include "SimSensor.h"
#include "Util/Color.h"

namespace nerd{

class BoxGeom;
class InterfaceValue;
class SimBody;
class Vector3D;
class Vector3DValue;
class CollisionObject;
class QuaternionValue; 

/**
 * AccelSensor. Implementation of an acceleration sensor with linear characteristics. 
 * There are two sensor values which represent the acceleration in direction of two sensor 
 * axis. The default behavior of this sensor corresponds to the Acceleration Sensor: ADXL213
 * by "Analog Devices". The sensor output is currently a value within the intervall:
 * [-16.072704, 16.072704] mapped to [-1,1]. The global value: "/AccelSensor/Deviation" 
 * can be used to add gaussian noise to all AccelSensor-object and 
 * "/AccelSensor/LowPassFilterDelay" can be used to add a simple lowpass filter to all 
 * AccelSensor-objects. AccelSensor object provide a parameter: ReferenceBody, which holds
 * the name of the body to which the sensor is attached. This parameter can not be modified
 * during runtime!
 */
class AccelSensor : public virtual SimSensor, public SimObject {

	public:
		AccelSensor(const QString &name, int numberOfAxes, double scalingFactor = 1.0); //scaling factor is temporarily!
		AccelSensor(const AccelSensor &sensor);

		virtual ~AccelSensor();
		virtual SimObject* createCopy() const;
		virtual void setup();
		virtual void clear();

		Vector3D getAxisOne() const;		
		Vector3D getAxisTwo() const;
		Vector3D getAxisThree() const;
		Vector3D getGlobalPosition() const;
		virtual void updateSensorValues();	
		
		virtual void valueChanged(Value *value);

	private:
		QList<double> calculateFilteredSensorValues(double sensorValue1, 
													double sensorValue2,
													double sensorValue3);
		bool createSensor();	

	private:
		double mMaxSensorValue;
		
		InterfaceValue *mFirstSensorValue;
		InterfaceValue *mSecondSensorValue;
		InterfaceValue *mThirdSensorValue;
		SimBody *mHostBody;
		BoxGeom *mSensorGeometry;
		CollisionObject *mSensorBody;
		Vector3DValue *mLocalPosition;
		QuaternionValue *mLocalOrientation;
		BoolValue *mDisableBodyCollisions;
		Vector3DValue *mSensorAxisOneValue;
		Vector3DValue *mSensorAxisTwoValue;
		Vector3DValue *mSensorAxisThreeValue;
		Vector3DValue *mSensorSizeValue;
		Vector3D mSensorAxisOne;
		Vector3D mSensorAxisTwo;
		Vector3D mSensorAxisThree;
		Vector3D mRotatedSensorAxisOne;
		Vector3D mRotatedSensorAxisTwo;
		Vector3D mRotatedSensorAxisThree;
		Vector3D mAcceleration;
		Vector3D mVelocity;
		Vector3D mLastVelocity;
		Vector3D mPosition;
		Vector3D mLastPosition;
		double mLastAxisOneMeasurement;
		double mLastAxisTwoMeasurement;
		double mLastAxisThreeMeasurement;

		DoubleValue *mTimeStepSize;
		DoubleValue* mGravitationValue;
		DoubleValue *mGlobalNoiseDeviationValue;
		double mGlobalNoiseDeviation;

		StringValue *mReferenceBodyName;

		bool mInitialized;
		double mGravitation;

		IntValue *mLowPassFilterDelayValue;
		int mLowPassFilterDelay;

		Color mColor;

		QVector<double> mValueOneHistory;
		QVector<double> mValueTwoHistory;
		QVector<double> mValueThreeHistory;

		int mNumberOfAxes;
		
		DoubleValue *mScalingFactor;
};
}
#endif

