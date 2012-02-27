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


#include "ODE_TorqueDrivenDynamixel.h"
#include "ODE_Body.h"
#include "Physics/SimBody.h"
#include "Core/Core.h"
#include "Math/Math.h"
#include <iostream>

using namespace std;
namespace nerd {

/**
 * Constructs a new ODE_TorqueDrivenDynamixel.
 */
ODE_TorqueDrivenDynamixel::ODE_TorqueDrivenDynamixel(const QString &name) 
	: TorqueDrivenDynamixel(name), ODE_Joint()
{
	Core *core = Core::getInstance();

	if(core->getValueManager()->getValue("/Dynamixel/ERP") == 0) {
		mDynamixelERP = new DoubleValue(0.2);
		core->getValueManager()->addValue("/Dynamixel/ERP", mDynamixelERP);
	}
	else
	{
		mDynamixelERP = dynamic_cast<DoubleValue*>(core->getValueManager()->getValue("/Dynamixel/ERP"));
	}
	if(core->getValueManager()->getValue("/Dynamixel/CFM") == 0) {
		mDynamixelCFM = new DoubleValue(0.000000001);
		core->getValueManager()->addValue("/Dynamixel/CFM", mDynamixelCFM);
	}
	else {
		mDynamixelCFM = dynamic_cast<DoubleValue*>(core->getValueManager()->getValue("/Dynamixel/CFM"));
	}

	if(core->getValueManager()->getValue("/Dynamixel/StopERP") == 0) {
		mDynamixelStopERP = new DoubleValue(0.2);
		core->getValueManager()->addValue("/Dynamixel/StopERP", mDynamixelStopERP);
	}
	else {
		mDynamixelStopERP = dynamic_cast<DoubleValue*>(core->getValueManager()->getValue("/Dynamixel/StopERP"));
	}
	if(core->getValueManager()->getValue("/Dynamixel/StopCFM") == 0) {
		mDynamixelStopCFM = new DoubleValue(0.000000001);
		core->getValueManager()->addValue("/Dynamixel/StopCFM", mDynamixelStopCFM);
	}
	else {
		mDynamixelStopCFM = dynamic_cast<DoubleValue*>(core->getValueManager()->getValue("/Dynamixel/StopCFM"));
	}
	mIsStart = true;
}

/**
 * Creates a copy of the given ODE_TorqueDrivenDynamixel.
 *
 * Note: The internal IBDS::HingeJoint object is NOT copied. Instead all parameters
 * of the ODE_TorqueDrivenDynamixel are copied, so that a call to createJoint() will create 
 * an equivalent hinge joint.
 */
ODE_TorqueDrivenDynamixel::ODE_TorqueDrivenDynamixel(const ODE_TorqueDrivenDynamixel &joint) : 
		Object(), ValueChangedListener(), EventListener(), TorqueDrivenDynamixel(joint),
		ODE_Joint()
{
}

/** 
 * Destructor.
 */
ODE_TorqueDrivenDynamixel::~ODE_TorqueDrivenDynamixel() {
}


/**
 * Creates a copy of thie ODE_TorqueDrivenDynamixel by usin its copy constructor.
 *
 * @return a copy of the joint.
 */
SimJoint* ODE_TorqueDrivenDynamixel::createCopy() const {
	return new ODE_TorqueDrivenDynamixel(*this);
}


/**
 * Creates an IBDS::HingeJoint with the rotation axis specified by the two points 
 * given by parameters AxisPoint1 and AxisPoint2. The HingeJoint connects the two bodies.
 *
 * @param body1 the first body to connect to.
 * @param body2 the second body to connect o.
 * @return a new IBDS::HingeJoint. 
 */
void ODE_TorqueDrivenDynamixel::setup() {
	ODE_Joint::setup();
	TorqueDrivenDynamixel::setup();
	mJoint = (dJointID)ODE_Joint::createJoint();
	if(mJoint == 0) {
		Core::log("ODE_TorqueDrivenDynamixel: dJoint could not be created.");
	}
}

/**
 * Clears the HingeJoint. 
 * This implementation sets the internal HingeJoint pointer to NULL.
 */
void ODE_TorqueDrivenDynamixel::clear() {
	ODE_Joint::clearJoint();
	HingeJoint::clear();
	mJoint = 0;
	mIsStart = true;
	friction = 0;
	mSimulationTime = 0;	
}

// void ODE_TorqueDrivenDynamixel::eventOccured(Event *event) {
// 	if(event == mUpdateMotors) {
// 			updateMotor();
// 	}
// 	if(event == mNextStepCompletedEvent) {
// // 		cout << "Joint: " << mJoint << endl;
// 		double sensorValue = dJointGetHingeAngle(mJoint);
// 		if(mIsStart) {
// 			startAngle = sensorValue;
// 			angle = sensorValue;
// 		}
// // First add the choosen offset 
// // Add desired Noise to the measured angle value
// // Map the value into the intervall -150 - 150 degree ( = -1 - 1)
// 		double pi56 = 6.0/(5.0* Math::PI);
// 		sensorValue = Math::calculateGaussianNoise(sensorValue + mOffset, mSensorNoiseValue->get()) * pi56;
// 		mJointAngle->set(sensorValue);	
// // 		if(getName().compare("Left/ElbowMotor") == 0) {
// // 			qDebug("%f", sensorValue);
// // 		}
// 		
// 	}
// }


void ODE_TorqueDrivenDynamixel::valueChanged(Value *value){
	TorqueDrivenDynamixel::valueChanged(value);
// 	if(value == mDesiredMotorSetting) {
// 		t_torque = mDesiredMotorSetting->get();
// 	}
}
/**
 * Creates a new IBDS::HingeJoint.
 *
 * @param body1 the first body to connect the joint to.
 * @param body2 the second body to connect the joint to.
 * @return a new IBDS::HingeJoint.
 */
dJointID ODE_TorqueDrivenDynamixel::createJoint(dBodyID body1, dBodyID body2) {

	dJointID newJoint = dJointCreateHinge(mWorldID, mGeneralJointGroup);
	dJointAttach(newJoint, body1, body2);

	Vector3D anchor = mJointAxisPoint1->get() ;
	Vector3D axis = mJointAxisPoint2->get() ;

	axis.set(mJointAxisPoint2->getX() - mJointAxisPoint1->getX(), 
			 mJointAxisPoint2->getY() - mJointAxisPoint1->getY(), 
			 mJointAxisPoint2->getZ() - mJointAxisPoint1->getZ());
	
	
	dJointSetHingeAnchor(newJoint, anchor.getX(), anchor.getY(), anchor.getZ());
	dJointSetHingeAxis(newJoint, axis.getX(), axis.getY(), axis.getZ());

	mMinAngle = (mMinAngleValue->get() * Math::PI) / 180.0;
	mMaxAngle = (mMaxAngleValue->get() * Math::PI) / 180.0;
	dJointSetHingeParam(newJoint, dParamLoStop, mMinAngle);
	dJointSetHingeParam(newJoint, dParamHiStop, mMaxAngle);
	
	return newJoint; 
}

void ODE_TorqueDrivenDynamixel::updateMotors() {

	if(mJoint == 0) {
		return;
	}
	double t_torque = mDesiredMotorSetting->get();

	if(mIsStart == true) {
		startAngle = dJointGetHingeAngle(mJoint);
		mIsStart = false;
	}
		
	angle = dJointGetHingeAngle(mJoint);
	
	if(angle > Math::PI) {
		angle = angle - 2.0*Math::PI;
	}
	if(angle < -1.0 * Math::PI) {
		angle = angle + 2.0 * Math::PI;
	}
	torque = t_torque * mM_max;
	friction = getFrictionMoment(mW);

	mE = mP - (angle - startAngle);
	mE = fmod(mE, (2.0 * 3.1415927));
	mZ1 = (mZ1 + mE * mH) * mExp1;
	mZ2 = (mZ2 + mE * mH) * mExp2;

	//avoid division by zero in seldom cases //TODO check if this is the correct default behavior.
	if(mC1 != 0.0 && mC2 != 0.0 && mJ != 0.0) {
		mO = mE0 * mE + mE1 * (mE - mZ1 * mE1 / mC1) + mE2 * (mE - mZ2 * mE2 / mC2);
		mW = mW + mM * mH / mJ;
	}

	mM = torque - mO - friction;
	mP = mP + mW * mH;

	dJointAddHingeTorque(mJoint, mO);

	oldTorque = mO;
	oldMotorSetting = mDesiredMotorSetting->get();
	mSimulationTime += mH;
}

void ODE_TorqueDrivenDynamixel::updateSensors() {
	if(mJoint == 0) {
		return;
	}
	double sensorValue = dJointGetHingeAngle(mJoint);
	if(mIsStart) {
		startAngle = sensorValue;
		angle = sensorValue;
	}
// First add the choosen offset 
// Add desired Noise to the measured angle value
// Map the value into the intervall -150 - 150 degree ( = -1 - 1)
	sensorValue = sensorValue * 180.0 / Math::PI;
	sensorValue = Math::calculateGaussian(sensorValue + mOffset, mSensorNoiseValue->get());
	mJointAngle->set(sensorValue);	
}

}




