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

#include "AngleSensor.h"
#include "Value/InterfaceValue.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Core/Core.h"
#include "SimBody.h"
#include "Math/Math.h"
#include "Math/Quaternion.h"
#include <math.h>
#include "Math/Math.h"
#include "Value/Vector3DValue.h"
#include "NerdConstants.h"

namespace nerd {


AngleSensor::AngleSensor(const QString &name) 
	: SimObject(name), mSource(0), mTarget(0) 
{
	mLowerBorder = new DoubleValue();
	mUpperBorder = new DoubleValue();
	mAxisOneValue = new Vector3DValue();
	mAxisTwoValue = new Vector3DValue();
	addParameter("Min", mLowerBorder);
	addParameter("Max", mUpperBorder);
	addParameter("RotationsAxisOne", mAxisOneValue);
	addParameter("RotationsAxisTwo", mAxisTwoValue);

	mSensorValue = new InterfaceValue(getName(), "Angle");
	mSensorValue->setMin(mLowerBorder->get());
	mSensorValue->setMax(mUpperBorder->get());
	
	mOutputValues.append(mSensorValue);
	
}




AngleSensor::AngleSensor(const AngleSensor &sensor) : Object(), ValueChangedListener(), 
		SimObject(sensor)
{
	mLowerBorder = dynamic_cast<DoubleValue*>(getParameter("Min"));
	mUpperBorder = dynamic_cast<DoubleValue*>(getParameter("Max"));
	mAxisOneValue = dynamic_cast<Vector3DValue*>(getParameter("RotationAxisOne"));
	mAxisTwoValue = dynamic_cast<Vector3DValue*>(getParameter("RotationsAxisTwo"));

	mSensorValue = new InterfaceValue(getName(), "Angle");
	mSensorValue->setMin(mLowerBorder->get());
	mSensorValue->setMax(mUpperBorder->get());
	mOutputValues.push_back(mSensorValue);
}

AngleSensor::~AngleSensor() {
}

SimObject* AngleSensor::createCopy() const {
	return new AngleSensor(*this);
}


QString AngleSensor::getName() const {
	return SimObject::getName();
}


bool AngleSensor::createSensor(SimBody *sourceBody, SimBody *targetBody, 
		const Vector3D &axis, const Vector3D &axis2) 
{
	if(sourceBody == 0 || targetBody == 0 || sourceBody == targetBody) {
		return false;
	}
	mSource = sourceBody;
	mTarget = targetBody;
	mAxisOneValue->set(axis);
	mAxisTwoValue->set(axis2);
	if(fabs(mAxisOne * mAxisTwo)  > 0.999) {
		Core::log("AngleSensor: Axes are identical! Sensor could not be created!");
		return false;
	}
	return true;
}

void AngleSensor::updateSensorValues() {
	Vector3D axis1;
	Vector3D axis2;

	Quaternion sourceRotation = mSource->getQuaternionOrientationValue()->get();
	Quaternion targetRotation = mTarget->getQuaternionOrientationValue()->get();

	Quaternion inverse1 = sourceRotation.getInverse();
	Quaternion inverse2 = targetRotation.getInverse();

	Quaternion quadAxis1(0, 
						mLocalRotationAxisSource.getX(), 
						mLocalRotationAxisSource.getY(), 
						mLocalRotationAxisSource.getZ());
	Quaternion quadAxis2(0, 
						mLocalRotationAxisSource.getX(), 	
						mLocalRotationAxisSource.getY(), 
						mLocalRotationAxisSource.getZ());
	
	Quaternion localAxis1 = sourceRotation * quadAxis1 * inverse1;
	Quaternion localAxis2 = targetRotation * quadAxis2 * inverse2;

	axis1.set(localAxis1.getX(), localAxis1.getY(), localAxis1.getZ());
	axis2.set(localAxis2.getX(), localAxis2.getY(), localAxis2.getZ());

	double al = axis1.length ();
	double bl = axis2.length ();
	double div = al*bl;
	double angle = 0.0;

	if (div != 0) {
		double c = axis1 * axis2 / div;
		if (c > 1.0) {
			c = 1.0;
			}	else if (c < -1.0) {
			c = -1.0;
			}
		if ((Vector3D::crossProduct(axis1, axis2)) * mAxisOne > 0) {
			angle = acos (c);
		} 
		else {
			angle = -acos (c);
		}
	}
	else {
		angle = 0.0;
	}
	angle = (angle*180.0)/ Math::PI;
	mSensorValue->set((angle));
}


void AngleSensor::valueChanged(Value *value) {
	SimObject::valueChanged(value);
	if(value == mLowerBorder) {
		mSensorValue->setMin(mLowerBorder->get());
	}else if (value == mUpperBorder) {
		mSensorValue->setMax(mUpperBorder->get());
	} 
}

void AngleSensor::setup() {

	mAxisOne = mAxisOneValue->get();
	mAxisTwo = mAxisTwoValue->get();
// 	calculate the sensor axis, rotated into local coordinates of the two objects.
	mAxisOne.normalize();
	mAxisTwo.normalize();
	if(fabs(mAxisOne * mAxisTwo)  > 0.999) {
		Core::log("AngleSensor: Axes are identical! Sensor could not be created!");
		return;
	}
	Vector3D normal = Vector3D::crossProduct(mAxisOne, mAxisTwo);
	normal.normalize();
	
	Quaternion rotation1 = 	mSource->getQuaternionOrientationValue()->get();
	Quaternion rotation2 = 	mTarget->getQuaternionOrientationValue()->get();
	Quaternion inverse1 = rotation1.getInverse();
	Quaternion inverse2 = rotation2.getInverse();

	Quaternion globalAxisQuaternion(0, normal.getX(), normal.getY(), normal.getZ());
	Quaternion localAxis1 = rotation1 * globalAxisQuaternion * inverse1;
	Quaternion localAxis2 = rotation2 * globalAxisQuaternion * inverse2;
	
	mLocalRotationAxisSource.set(localAxis1.getX(), localAxis1.getY(), localAxis1.getZ());
	mLocalRotationAxisTarget.set(localAxis2.getX(), localAxis2.getY(), localAxis2.getZ());
}

void AngleSensor::clear() {
	
}


}
