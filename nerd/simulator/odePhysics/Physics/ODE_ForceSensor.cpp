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



#include "ODE_ForceSensor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "Math/Math.h"
#include <QStringList>

using namespace std;

namespace nerd {


/**
 * Constructs a new ODE_ForceSensor.
 */
ODE_ForceSensor::ODE_ForceSensor(const QString &name, double minForce, double maxForce)
	: SimObject(name), mForceSensorValue(0), mMinForce(0), mMaxForce(0), 
	  mHostBodyName(0), mCollisionObjectIndices(0), mLocalSensorAxis(0),
	  mHostBody(0)
{
	mForceSensorValue = new InterfaceValue(getName(), "Force", 0.0, minForce, maxForce);
	mMinForce = new DoubleValue(minForce);
	mMaxForce = new DoubleValue(maxForce);
	mHostBodyName = new StringValue("");
	mCollisionObjectIndices = new StringValue("0");
	mLocalSensorAxis = new Vector3DValue(1.0, 0.0, 0.0);

	addParameter("Force", mForceSensorValue);
	addParameter("MinForce", mMinForce);
	addParameter("MaxForce", mMaxForce);
	addParameter("HostBodyName", mHostBodyName);
	addParameter("CollisionObjectIndices", mCollisionObjectIndices);
	addParameter("LocalSensorAxis", mLocalSensorAxis);

	mOutputValues.append(mForceSensorValue);
}


/**
 * Copy constructor. 
 * 
 * @param other the ODE_ForceSensor object to copy.
 */
ODE_ForceSensor::ODE_ForceSensor(const ODE_ForceSensor &other) 
	: Object(), ValueChangedListener(), SimSensor(), SimObject(other), 
	  mHostBody(0)
{
	mForceSensorValue = dynamic_cast<InterfaceValue*>(getParameter("Force"));
	mMinForce = dynamic_cast<DoubleValue*>(getParameter("MinForce"));
	mMaxForce = dynamic_cast<DoubleValue*>(getParameter("MaxForce"));
	mHostBodyName = dynamic_cast<StringValue*>(getParameter("HostBodyName"));
	mCollisionObjectIndices = dynamic_cast<StringValue*>(getParameter("CollisionObjectIndices"));
	mLocalSensorAxis = dynamic_cast<Vector3DValue*>(getParameter("LocalSensorAxis"));

	mOutputValues.append(mForceSensorValue);
}

/**
 * Destructor.
 */
ODE_ForceSensor::~ODE_ForceSensor() {
}

SimObject* ODE_ForceSensor::createCopy() const {
	return new ODE_ForceSensor(*this);
}


void ODE_ForceSensor::setup() {
	PhysicsManager *pm = Physics::getPhysicsManager();

	SimBody *simBody = pm->getSimBody(mHostBodyName->get());

	if(simBody != 0) {
		QStringList indices = mCollisionObjectIndices->get().split(",");
		for(QListIterator<QString> i(indices); i.hasNext();) {
			bool ok = false;	
			QString indexString = i.next();
			int index = indexString.toInt(&ok);
			if(ok && index >= 0 && simBody->getCollisionObjects().size() > index) {
				mReferenceCollisionObjects.append(simBody->getCollisionObjects().at(index));
			}
			else {
				Core::log(QString("ODE_ForceSensor: Could not find index [")
							+ indexString + "] of host body [" + mHostBodyName->get() + "]", true);
			}
		}

		//enable force feedback
		mHostBody = dynamic_cast<ODE_Body*>(simBody);
		if(mHostBody != 0 && !mReferenceCollisionObjects.empty()) {
			mHostBody->enableContactJointFeedback(true);
		}
		else {
			Core::log(QString("ODE_ForceSensor: Could not activate force feedback on host body [") 
						+ mHostBodyName->get() + "]", true);
		}
	}
	else {
		Core::log(QString("ODE_ForceSensor: Could not find host body [") + mHostBodyName->get() + "]", true);
	}
}


void ODE_ForceSensor::clear() {
	//disable force feedback.
	if(mHostBody != 0) {
		mHostBody->enableContactJointFeedback(true);
	}

	mHostBody = 0;
	mReferenceCollisionObjects.clear();
}

void ODE_ForceSensor::valueChanged(Value *value) {
	SimObject::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mMinForce || value == mMaxForce) {
		mForceSensorValue->setMin(mMinForce->get());
		mForceSensorValue->setMax(mMaxForce->get());
	}
}
		
void ODE_ForceSensor::updateSensorValues() {

	if(mHostBody == 0 || mReferenceCollisionObjects.empty())
	{
		mForceSensorValue->set(0.0);
		return;
	}

	//calculate sum of all forces from all contact points on all reference collision objects.
	Vector3D forceSum;

	for(QListIterator<CollisionObject*> j(mReferenceCollisionObjects); j.hasNext();) {
		CollisionObject *obj = j.next();
		if(obj == 0 || obj->getNativeCollisionObject() == 0) {
			continue;
		}	

		dGeomID geomId = (dGeomID) obj->getNativeCollisionObject();
	
		QList<dJointFeedback*> feedbackStructures = mHostBody->getJointFeedbackList().values(geomId);
	
		//calculate force sum (involving both geoms of each collision)
		for(QListIterator<dJointFeedback*> i(feedbackStructures); i.hasNext();) {
			dJointFeedback *feedback = i.next();
			forceSum.setX(Math::abs(feedback->f1[0]) + Math::abs(feedback->f2[0]) + forceSum.getX());
			forceSum.setY(Math::abs(feedback->f1[1]) + Math::abs(feedback->f2[1]) + forceSum.getY());
			forceSum.setZ(Math::abs(feedback->f1[2]) + Math::abs(feedback->f2[2]) + forceSum.getZ());
		}
	}


	//rotate force axis from global to local coordinate system.

	SimBody *hostSimBody = dynamic_cast<SimBody*>(mHostBody);
	if(hostSimBody == 0) {
		mForceSensorValue->set(0.0);
		return;
	}

	Quaternion bodyOrientationInverse = 
		hostSimBody->getQuaternionOrientationValue()->get().getInverse();

	Quaternion bodyRotation = hostSimBody->getQuaternionOrientationValue()->get();
	bodyRotation.normalize();
	Quaternion bodyRotationInverse = bodyRotation.getInverse();
	bodyRotationInverse.normalize();

	Quaternion axisQuat(0.0, 
						mLocalSensorAxis->getX(), 
						mLocalSensorAxis->getY(), 
						mLocalSensorAxis->getZ());
	Quaternion axisRotatedQuat = bodyRotation * axisQuat * bodyRotationInverse;

	Vector3D rotatedAxis(axisRotatedQuat.getX(), 
							  axisRotatedQuat.getY(),
							  axisRotatedQuat.getZ());


	double sensorValue = forceSum.getX() * axisRotatedQuat.getX() 
		+ forceSum.getY() * axisRotatedQuat.getY() 
		+ forceSum.getZ() * axisRotatedQuat.getZ();

	mForceSensorValue->set(sensorValue);

}


ODE_Body* ODE_ForceSensor::getHostBody() const {
	return mHostBody;
}


QList<CollisionObject*> ODE_ForceSensor::getReferenceCollisionObject() const {
	return mReferenceCollisionObjects;
}

void ODE_ForceSensor::setLocalSensorAxis(const Vector3D &axis) {
	mLocalSensorAxis->set(axis);
}


Vector3D ODE_ForceSensor::getLocalSensorAxis() const {
	return mLocalSensorAxis->get();
}



}



