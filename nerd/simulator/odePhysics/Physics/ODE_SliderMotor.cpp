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

#include "ODE_SliderMotor.h"
#include "Value/BoolValue.h"
#include "Math/Math.h"
#include "Core/Core.h"
#include <iostream>
#include "Physics/ODE_Body.h"
#include <ode/ode.h>
#include "Physics/SimBody.h"


using namespace std;

namespace nerd {

ODE_SliderMotor::ODE_SliderMotor(const QString &name) 
	: SimSensor(), SimActuator(), SliderMotor(name), ODE_Joint() 
{
}

ODE_SliderMotor::ODE_SliderMotor(const ODE_SliderMotor &joint) 
		: Object(), ValueChangedListener(), 
		SimSensor(), SimActuator(), SliderMotor(joint), ODE_Joint() 
{
}

ODE_SliderMotor::~ODE_SliderMotor() {
}
	
SimJoint* ODE_SliderMotor::createCopy() const {
	return new ODE_SliderMotor(*this);
}
	
void ODE_SliderMotor::setup() {
	ODE_Joint::setup();
	SliderMotor::setup();
	mJoint = (dJointID)ODE_Joint::createJoint();
	if(mJoint == 0) {
		Core::log("ODE_SliderMotor: dJoint could not be created.");
		return;
	}
	mCurrentMotorPosition = dJointGetSliderPosition(mJoint);		
	mLastMotorPosition = mCurrentMotorPosition;
}

void ODE_SliderMotor::clear() {
	ODE_Joint::clearJoint();
	mJoint = 0;
}

void ODE_SliderMotor::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm*) {
}

void ODE_SliderMotor::updateActuators() {
	SliderMotor::updateActuators();

	if(mBodyNamesChanged) {
		determineBodies();
		if(mJoint != 0) {
			ODE_Body *body1 = dynamic_cast<ODE_Body*>(getFirstBody());
			ODE_Body *body2 = dynamic_cast<ODE_Body*>(getSecondBody());

			dBodyID odeBody1 = 0;
			dBodyID odeBody2 = 0;
			if(body1 != 0) {
				odeBody1 = body1->getRigidBodyID();
			}
			if(body2 != 0) {
				odeBody2 = body2->getRigidBodyID();	
			}
			dJointAttach(mJoint, odeBody1, odeBody2);
		}
	}
	
	if(mJoint != 0) {
		if(mControlMotorPosition->get()) {
			mCurrentMotorPosition = dJointGetSliderPosition(mJoint);
			
			double timeStepSize = mTimeStepSize->get();
			if(timeStepSize == 0.0) {
				mVelocity = 0.0;
			}
			else {
				mVelocity = (mCurrentMotorPosition - mLastMotorPosition) / mTimeStepSize->get();
			}

			dJointSetSliderParam(mJoint, dParamVel, calculateVelocity(mCurrentMotorPosition));	
			mLastMotorPosition = mCurrentMotorPosition;
			
			//TODO add slider friction!
		} 
		else {	
			dJointSetSliderParam(mJoint, dParamVel,  mDesiredMotorSetting->get());	
		}
	}
}

void ODE_SliderMotor::updateSensorValues() {
	SliderMotor::updateSensorValues();
	if(mJoint != 0) {
		mCurrentMotorPosition = dJointGetSliderPosition(mJoint);
		mCurrentMotorPosition = Math::calculateGaussian(mCurrentMotorPosition, mSensorNoise);
		mMotorPositionSensor->set(mCurrentMotorPosition);
	}
}


void ODE_SliderMotor::connectBodies() {
	
}

dJointID ODE_SliderMotor::createJoint(dBodyID body1, dBodyID body2) {
	if(mFirstAxisPoint->get().equals(mSecondAxisPoint->get())) {
		Core::log("Invalid axes for ODE_SliderMotor.");
		return 0;
	}
	dJointID newJoint = dJointCreateSlider(mWorldID, mGeneralJointGroup);
	dJointAttach(newJoint, body1, body2);

	Vector3D axis(mSecondAxisPoint->getX() - mFirstAxisPoint->getX(), 
		mSecondAxisPoint->getY() - mFirstAxisPoint->getY(), 
		mSecondAxisPoint->getZ() - mFirstAxisPoint->getZ());
	axis.normalize();
	dJointSetSliderAxis(newJoint, axis.getX(), axis.getY(), axis.getZ());
	dJointSetSliderParam(newJoint, dParamLoStop, mMinPositionValue->get());
	dJointSetSliderParam(newJoint, dParamHiStop, mMaxPositionValue->get());
	dJointSetSliderParam(newJoint, dParamFMax, mFMaxValue->get());

	return newJoint; 
}

}
