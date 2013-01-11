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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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



#include "SimpleToeMotor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"
#include "Math/Vector3D.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SimpleToeMotor.
 */
SimpleToeMotor::SimpleToeMotor(const QString &name)
	: SimSensor(), SimActuator(), Dynamixel(name, "/ToeMotor"), ODE_Joint()
{
	mInputValues.removeAll(mDesiredMotorAngleValue);
	mInputValues.removeAll(mDesiredMotorTorqueValue);

	mDesiredMotorTorqueValue->set(mDesiredMotorTorqueValue->getMin());
	mDesiredMotorAngleValue->set(0.0);
}


/**
 * Copy constructor. 
 * 
 * @param other the SimpleToeMotor object to copy.
 */
SimpleToeMotor::SimpleToeMotor(const SimpleToeMotor &other)
	: Object(),	ValueChangedListener(), SimSensor(), SimActuator(), Dynamixel(other),
	   ODE_Joint()
{
	mInputValues.removeAll(mDesiredMotorAngleValue);
	mInputValues.removeAll(mDesiredMotorTorqueValue);

	mDesiredMotorTorqueValue->set(mDesiredMotorTorqueValue->getMin());
}

/**
 * Destructor.
 */
SimpleToeMotor::~SimpleToeMotor() {
}


SimObject* SimpleToeMotor::createCopy() const {
	return new SimpleToeMotor(*this);
}

void SimpleToeMotor::setup() {
	ODE_Joint::setup();
	Dynamixel::setup();

	if(mJoint == 0) {
		mJoint = (dJointID) ODE_Joint::createJoint();
	}
	if(mJoint == 0) {
		Core::log("ODE_Dynamixel: dJoint could not be created.");
		return;
	}

}

void SimpleToeMotor::clear() {
	ODE_Joint::clearJoint();
	Dynamixel::clear();
	mJoint = 0;
}

void SimpleToeMotor::valueChanged(Value *value) {

	Dynamixel::valueChanged(value);

	if((value == mDesiredMotorTorqueValue || value == mFMaxFactor) && mJoint != 0) {
		dJointSetAMotorParam(mJoint, dParamFMax, mFMaxFactor->get() 
			* Math::max(0.0, mDesiredMotorTorqueValue->get()));
	}
}

void SimpleToeMotor::updateActuators() {

	if(mJoint != 0) {
		double sensorValue = dJointGetAMotorAngle(mJoint, 0);
		sensorValue = (sensorValue * 180.0) / Math::PI;

		//linear relation of torque and angular error
		double torque = ((mDesiredMotorAngleValue->get() - sensorValue)
							/ (mDesiredMotorAngleValue->get() - 180.0))
						 * mDesiredMotorTorqueValue->getMax();

		mDesiredMotorTorqueValue->set(torque);
 
		mCurrentPosition = dJointGetAMotorAngle(mJoint, 0);
		calculateMotorFrictionAndVelocity();
	
		dJointSetHingeParam(mHingeJoint, dParamFMax, mCalculatedFriction);	
		dJointSetAMotorParam(mJoint, dParamVel, mCalculatedVelocity);
		dJointSetAMotorParam(mJoint, dParamFMax, mFMaxFactor->get() 
				* Math::max(0.0, Math::abs(mDesiredMotorTorqueValue->get())));
	}
}

/**
 * Calculates the new motor angle.
 */
void SimpleToeMotor::updateSensorValues() {

	if(mJoint != 0) {
		//read sensor value and add noise.
		double sensorValue = dJointGetAMotorAngle(mJoint, 0);
	
		// First add the choosen offset
		// Add desired noise to the measured angle value
		// Map the value into the interval -150 - 150 degree ( = -1 - 1)
		
		sensorValue = (sensorValue * 180.0)/Math::PI;
		sensorValue = Math::calculateGaussian(sensorValue, mSensorNoiseDeviation);
		mAngleValue->set(sensorValue);
	}

}


dJointID SimpleToeMotor::createJoint(dBodyID body1, dBodyID body2) {

	if(mJointAxisPoint1->get().equals(mJointAxisPoint2->get(), -1)) {
		Core::log("Invalid axes for ODE_Dynamixel.");
		return 0;
	}

	Vector3D anchor = mJointAxisPoint1->get();
	Vector3D axis = mJointAxisPoint2->get() - mJointAxisPoint1->get();

	dJointID joint = dJointCreateAMotor(mWorldID, mGeneralJointGroup);
	dJointAttach(joint, body1, body2);
	dJointSetAMotorMode(joint, dAMotorEuler);
	dJointSetAMotorParam(joint, dParamVel, 0.0);
	dJointSetAMotorParam(joint, dParamFMax, mDesiredMotorTorqueValue->get());
	dJointSetAMotorParam(joint, dParamCFM, mCFMValue->get());
	dJointSetAMotorParam(joint, dParamStopERP, mStopERPValue->get());
	dJointSetAMotorParam(joint, dParamStopCFM, mStopCFMValue->get());
	dJointSetAMotorParam(joint, dParamBounce, mBounceValue->get());
	dJointSetAMotorParam(joint, dParamFudgeFactor, mFudgeFactorValue->get());

	axis.normalize();
	Vector3D perpedicular;
	if(axis.getY() != 0.0 || axis.getX() != 0.0) {
		perpedicular.set(-axis.getY(), axis.getX(), 0);
	}
	else {
		perpedicular.set(0, -axis.getZ(), axis.getY());
	}

	perpedicular.normalize();

	// If one of the bodies is static, the motor axis need to be defined in a different way. For different constellations of static and dynamic bodies, the turn direction of the motor changed, so this had to be added.
	if(body1 == 0) {
		dJointSetAMotorAxis(joint, 0, 0, -axis.getX(), -axis.getY(), -axis.getZ());
	}
	else {
		dJointSetAMotorAxis(joint, 0, 1, axis.getX(), axis.getY(), axis.getZ());
	}
	if(body2 == 0) {
		dJointSetAMotorAxis(joint, 2, 0, -perpedicular.getX(), -perpedicular.getY(), 
			-perpedicular.getZ());
	}
	else {
		dJointSetAMotorAxis(joint, 2, 2, perpedicular.getX(), perpedicular.getY(), 
			perpedicular.getZ());
	}

	mHingeJoint = dJointCreateHinge(mWorldID, mGeneralJointGroup);
	dJointAttach(mHingeJoint, body1, body2);

	dJointSetHingeAnchor(mHingeJoint, anchor.getX(), anchor.getY(), anchor.getZ());
	dJointSetHingeAxis(mHingeJoint, axis.getX(), axis.getY(), axis.getZ());

	if(body1 == 0) {
		double tmp = mMinAngle;
		mMinAngle = -1.0 * mMaxAngle;
		mMaxAngle = -1.0 * tmp;
	}

	dJointSetHingeParam(mHingeJoint, dParamLoStop, mMinAngle);
	dJointSetHingeParam(mHingeJoint, dParamHiStop, mMaxAngle);

	dJointSetHingeParam(mHingeJoint, dParamVel, 0.0);
	return joint;
}



}



