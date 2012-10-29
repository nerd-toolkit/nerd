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


#include "ODE_ServoMotor.h"
#include "ODE_Body.h"
#include "Physics/SimBody.h"
#include "Math/Math.h"
#include "Core/Core.h"

#include "Util/Tracer.h"

#include <QDebug>
#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

#include <iostream>

using namespace std;

namespace nerd {

/**
 * Constructs a new ODE_ServoMotor.
 */
ODE_ServoMotor::ODE_ServoMotor(const QString &name, bool enableTemperature) 
	: SimSensor(), SimActuator(), ServoMotor(name, enableTemperature), ODE_Joint()
{
}

/**
 * Creates a copy of the given ODE_ServoMotor.
 *
 * Note: The internal ODE_ServoMotor object is NOT copied. Instead all parameters
 * of the ODE_ServoMotor are copied, so that a call to createJoint() will create 
 * an equivalent hinge joint.
 */
ODE_ServoMotor::ODE_ServoMotor(const ODE_ServoMotor &joint) 
	: Object(), ValueChangedListener(), EventListener(), SimSensor(), 
	  SimActuator(), ServoMotor(joint), ODE_Joint()
{
}

/** 
 * Destructor.
 */
ODE_ServoMotor::~ODE_ServoMotor() {
}


/**
 * Creates a copy of thie ODE_ServoMotor by usin its copy constructor.
 *
 * @return a copy of the joint.
 */
SimJoint* ODE_ServoMotor::createCopy() const {
	return new ODE_ServoMotor(*this);
}

void ODE_ServoMotor::valueChanged(Value *value) {
	ServoMotor::valueChanged(value);

	if(value == 0) {
		return;
	}
	if((value == mFMaxValue) && (mJoint != 0)) {
		dJointSetHingeParam(mJoint, dParamFMax, mFMaxValue->get());
	}
}


/**
 * Creates an ODE_ServoMotor with the rotation axis specified by the two points 
 * given by parameters AxisPoint1 and AxisPoint2. The HingeJoint connects the two bodies.
 *
 * @param body1 the first body to connect to.
 * @param body2 the second body to connect o.
 * @return a new ODE_ServoMotor. 
 */
void ODE_ServoMotor::setup() {
	TRACE("ODE_ServoMotor::setup");

	ODE_Joint::setup();
	ServoMotor::setup();
	mVelocity = 0.0;
	if(mJoint == 0) {
		mJoint = (dJointID)ODE_Joint::createJoint();
	}
	if(mJoint == 0) {
		Core::log("ODE_ServoMotor: dJoint could not be created.");
		return;
	}
	mCurrentMotorAngle = dJointGetHingeAngle(mJoint);		
	mLastMotorAngle = mCurrentMotorAngle;
	mLastSensorMotorAngle = mCurrentMotorAngle;
}


/**
 * Clears the ServoMotor. 
 * This implementation sets the internal HingeJoint pointer to NULL.
 */
void ODE_ServoMotor::clear() {
	ODE_Joint::clearJoint();
	mJoint = 0;
}

void ODE_ServoMotor::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm*) {
}


/**
 * Creates a new ODE_ServoMotor.
 *
 * @param body1 the first body to connect the joint to.
 * @param body2 the second body to connect the joint to.
 * @return a new ODE_ServoMotor.
 */
dJointID ODE_ServoMotor::createJoint(dBodyID body1, dBodyID body2) {
	TRACE("ODE_ServoMotor::createJoint");

	if(mJointAxisPoint1->get().equals(mJointAxisPoint2->get())) {
		Core::log("Invalid axes for ODE_ServoMotor.");
		return 0;
	}
	dJointID newJoint = dJointCreateHinge(mWorldID, mGeneralJointGroup);
	dJointAttach(newJoint, body1, body2);

	Vector3D anchor = mJointAxisPoint1->get() ;
	Vector3D axis = mJointAxisPoint2->get() ;

	axis.set(mJointAxisPoint2->getX() - mJointAxisPoint1->getX(), mJointAxisPoint2->getY() - mJointAxisPoint1->getY(), mJointAxisPoint2->getZ() - mJointAxisPoint1->getZ());
	
	
	dJointSetHingeAnchor(newJoint, anchor.getX(), anchor.getY(), anchor.getZ());
	dJointSetHingeAxis(newJoint, axis.getX(), axis.getY(), axis.getZ());

	mMinAngle = (mMinAngleValue->get() * Math::PI) / 180.0;
	mMaxAngle = (mMaxAngleValue->get() * Math::PI) / 180.0;	
	// If the first body is static, min and max need to be switched.
	if(body1 == 0) {
		double tmp = mMinAngle;
		mMinAngle = -1.0 * mMaxAngle;
		mMaxAngle = -1.0 * tmp;
	}
	dJointSetHingeParam(newJoint, dParamLoStop, mMinAngle);
	dJointSetHingeParam(newJoint, dParamHiStop, mMaxAngle);
	dJointSetHingeParam(newJoint, dParamFMax, mFMaxValue->get());

	return newJoint; 
}

void ODE_ServoMotor::updateActuators() {
	TRACE("ODE_ServoMotor::updateActuators");
		ServoMotor::updateActuators();
		if(mJoint != 0) {
			if(mControlMotorAngle->get() && mTimeStepSize != 0) {
				mCurrentMotorAngle = dJointGetHingeAngle(mJoint);
				mVelocity = (mCurrentMotorAngle - mLastMotorAngle)/ mTimeStepSize->get();
				dJointAddHingeTorque(mJoint, mMaxTorque->get() * calculatedTorque(mVelocity, mCurrentMotorAngle));
				mLastMotorAngle = mCurrentMotorAngle;
			} 
			else {	
				// by modifying the parameter "MaxTorque" the speed of the motor is changed.
				//dJointSetHingeParam(mJoint, dParamVel,  mMaxTorque->get() * mDesiredMotorSetting->get());	
				dJointSetHingeParam(mJoint, dParamVel,  mMaxTorque->get() * calculatedTorque(mVelocity, mCurrentMotorAngle));	
			}
	}
}

void ODE_ServoMotor::updateSensorValues() {
	TRACE("ODE_ServoMotor::updateSensorValues");
	if(mJoint != 0) {
		mCurrentMotorAngle = dJointGetHingeAngle(mJoint);
		double angle = mCurrentMotorAngle * 180.0 / Math::PI;
		
		if(mControlMotorAngle->get()) {
			angle = Math::calculateGaussian(angle, mSensorNoise);
			mMotorAngleSensor->set(angle);
		}
		else {
			double velocity = angle - mLastSensorMotorAngle;
// 			if((angle < 0.0 && mLastSensorMotorAngle > 0.0) || (angle > 0.0 && mLastSensorMotorAngle < 0.0)) {
// 				velocity = mLastSensorMotorAngle - angle;
// 			}
			while(velocity >= 180.0) {
				velocity -= 360.0;
			}
			while(velocity <= -180.0) {
				velocity += 360.0;
			}
			mMotorAngleSensor->set(velocity); //-180.0 to 180.0 as change between two time steps (cannot reliably detect more than 180 degrees!) //TODO
		}
		mLastSensorMotorAngle = angle;
	}
}

}




