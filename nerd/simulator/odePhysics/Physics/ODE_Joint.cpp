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


#include "ODE_Joint.h"
#include "Physics/SimJoint.h"
#include "Physics/SimBody.h"
#include "Physics/PhysicsManager.h"
#include "Physics/ODE_Body.h"
#include "Physics/ODE_SimulationAlgorithm.h"
#include "Core/Core.h"
#include "Physics/Physics.h"


namespace nerd {

ODE_Joint::ODE_Joint() : mJoint(0), mGeneralJointGroup(0), mWorldID(0) {

	ValueManager *vm = Core::getInstance()->getValueManager();
	if(vm->getValue("/ODEJoint/MotorBounce") == 0) {
		mBounceValue = new DoubleValue(0.0);
		vm->addValue("/ODEJoint/MotorBounce", mBounceValue);
	}
	else {
		mBounceValue = vm->getDoubleValue("/ODEJoint/MotorBounce");
	}

	if(vm->getValue("/ODEJoint/MotorStopCFM") == 0) {
		mStopCFMValue = new DoubleValue(0.00001);
		vm->addValue("/ODEJoint/MotorStopCFM", mStopCFMValue);
	}
	else {
		mStopCFMValue = vm->getDoubleValue("/ODEJoint/MotorStopCFM");
	}

	if(vm->getValue("/ODEJoint/MotorCFM") == 0) {
		mCFMValue = new DoubleValue(0.00001);
		vm->addValue("/ODEJoint/MotorCFM", mCFMValue);
	}
	else {
		mCFMValue = vm->getDoubleValue("/ODEJoint/MotorCFM");
	}

	if(vm->getValue("/ODEJoint/MotorStopERP") == 0) {
		mStopERPValue = new DoubleValue(0.2);
		vm->addValue("/ODEJoint/MotorStopERP", mStopERPValue);
	}
	else {
		mStopERPValue = vm->getDoubleValue("/ODEJoint/MotorStopERP");
	}

	if(vm->getValue("/ODEJoint/FudgeFactor") == 0) {
		mFudgeFactorValue = new DoubleValue(1.0);
		vm->addValue("/ODEJoint/FudgeFactor", mFudgeFactorValue);
	}
	else {
		mFudgeFactorValue = vm->getDoubleValue("/ODEJoint/FudgeFactor");
	}

}

ODE_Joint::~ODE_Joint() {
}


/**
 * Returns the current IBDS::Joint. May be NULL setup was not called with
 * a valid parameterization.
 *
 * @return the current IBDS::Joint.
 */
dJointID ODE_Joint::getJoint() {
	return mJoint;
}

void ODE_Joint::setup() {
    ODE_SimulationAlgorithm *odeAlg = dynamic_cast<ODE_SimulationAlgorithm*>(
				Physics::getPhysicsManager()->getPhysicalSimulationAlgorithm());
	if(odeAlg == 0) {
        Core::log("ODE_Joint: Simulation algorithm could not be found or is not "
			"an instance of ODE_SimulationAlgorithm.", true);
		return;
	}
	mGeneralJointGroup = odeAlg->getJointGroupID();
	mWorldID = odeAlg->getODEWorldID();

}

void ODE_Joint::clearJoint() {
	if(mJoint != 0) {
		dJointDestroy(mJoint);
        mJoint = 0;
	}
}

dJointID ODE_Joint::createJoint() {
	SimJoint *joint = dynamic_cast<SimJoint*>(this);

	if(mWorldID == 0) {
		Core::log("ODE_Joint: Could not create ODE joint. ODE physics is not initialized!", true);
		return 0;
	}
	if(joint == 0) {
		Core::log("ODE_Joint: Could not create Joint.", true);
		return 0;
	}

	ODE_Body *body1 = dynamic_cast<ODE_Body*>(joint->getFirstBody());
	ODE_Body *body2 = dynamic_cast<ODE_Body*>(joint->getSecondBody());

	if(body1 == 0 && body2 == 0) {
		Core::log("ODE_Joint: Associated SimBody objects are not valid. (Joint [" + joint->getName() 
			+ "])", true);
		return 0;
	}

	if(body1 == body2) {
		Core::log("ODE_Joint: Both SimBodies of the joint are the same! (Skipping)", true);
		return 0;
	}

	dBodyID odeBody1 = 0;
	dBodyID odeBody2 = 0;
	if(body1 != 0) {
		odeBody1 = body1->getRigidBodyID();
	}
	if(body2 != 0) {
		odeBody2 = body2->getRigidBodyID();	
	}

	if(odeBody1 == odeBody2) {
		Core::log("ODE_Joint: Both dBodyID's of the joint are the same! (Skipping)", true);
		return 0;
	}

	//calls the joint specific method of the subclass to create the actual ODE joint.
	mJoint = createJoint(odeBody1, odeBody2);

	return mJoint;
}

}





