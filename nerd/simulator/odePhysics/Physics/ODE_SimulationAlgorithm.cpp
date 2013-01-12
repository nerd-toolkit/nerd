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

#include "ODE_SimulationAlgorithm.h"
#include "Physics/SimBody.h"
#include "Physics/SimJoint.h"
#include "Physics/SimObject.h"
#include "Physics/PhysicsManager.h"
#include <QList>
#include "Core/Core.h"
#include "Collision/ODE_CollisionHandler.h"
#include "Collision/CollisionManager.h"
#include "Physics/ODE_Body.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "Value/BoolValue.h"
#include <QTime>
#include "Value/ValueManager.h"
#include "Physics/Physics.h"
#include <iostream>

using namespace std;


namespace nerd {

/**
 * Constructs a new ODE_SimulationAlgorithm. The created object is 
 * NOT automatically registered at the PhysicsManager and therefore
 * has to be registered manually.
 */
ODE_SimulationAlgorithm::ODE_SimulationAlgorithm() 
		: PhysicalSimulationAlgorithm("ODE"), mInitialized(false), mODEWorld(0), 
		mODEMainSpace(0), mContactJointGroup(0), mGeneralJointGroup(0), 
		mODECollisionHandler(0)
{
	mMaxContactPoints = new IntValue(100);
	addParameter("ODE/MaxContactPoints", mMaxContactPoints, true);
		
	mConstraintForceMixing = new DoubleValue(0.00001);
	addParameter("ODE/ConstraintForceMixing", mConstraintForceMixing, true);
	
	mERP = new DoubleValue(0.2);
	addParameter("ODE/ERP", mERP, true);

	mContactSurfaceLayerValue = new DoubleValue(0.001);
	addParameter("ODE/ContactSurfaceLayer",mContactSurfaceLayerValue, true);

	mContactMaxCorrectingVel = new DoubleValue(dInfinity);
	addParameter("ODE/ContactMaxCorrectingVelocity", mContactMaxCorrectingVel, true);

	mUpdateMotorsDurationValue = new IntValue();
	mUpdateSensorsDurationValue = new IntValue();
	mStepDurationValue = new IntValue();
	mStepCollisionDurationValue = new IntValue();

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/ExecutionTime/ODE_SimAlgorithm/MotorUpdate", mUpdateMotorsDurationValue);
	vm->addValue("/ExecutionTime/ODE_SimAlgorithm/SensorUpdate", mUpdateSensorsDurationValue);
	vm->addValue("/ExecutionTime/ODE_SimAlgorithm/StepExecution", mStepDurationValue);
	vm->addValue("/ExecutionTime/ODE_SimAlgorithm/CollisionExecution", mStepCollisionDurationValue);
	
	mSwitchYZAxes = vm->getBoolValue(SimulationConstants::VALUE_SWITCH_YZ_AXES);
	
}

/**
 * Destructor.
 */
ODE_SimulationAlgorithm::~ODE_SimulationAlgorithm() {
	if(mInitialized) {
		dJointGroupEmpty(mContactJointGroup);
		dJointGroupEmpty(mGeneralJointGroup);
		dJointGroupDestroy(mContactJointGroup);
		dJointGroupDestroy(mGeneralJointGroup);
		dSpaceDestroy(mODEMainSpace);
		dWorldDestroy(mODEWorld);
		dCloseODE();
	}
}

bool ODE_SimulationAlgorithm::resetPhysics() {
	if(mInitialized) {
		dJointGroupEmpty(mContactJointGroup);
		dJointGroupEmpty(mGeneralJointGroup);
		dJointGroupDestroy(mContactJointGroup);
		dJointGroupDestroy(mGeneralJointGroup);
		dSpaceDestroy(mODEMainSpace);
		dWorldDestroy(mODEWorld);
		dCloseODE();
	} 
	else {
		mInitialized = true;
	} 
	dInitODE();
	mODEWorld = dWorldCreate();
	mODEMainSpace = dSimpleSpaceCreate(0);
	mContactJointGroup = dJointGroupCreate(0);
	mGeneralJointGroup = dJointGroupCreate(0);

	dWorldSetERP(mODEWorld, mERP->get());
	dWorldSetCFM(mODEWorld, mConstraintForceMixing->get());

	dWorldSetContactSurfaceLayer(mODEWorld, mContactSurfaceLayerValue->get());

	dWorldSetContactMaxCorrectingVel(mODEWorld,mContactMaxCorrectingVel->get());
	
	if(mSwitchYZAxes != 0 && mSwitchYZAxes->get() == false) {
		dWorldSetGravity(mODEWorld, 0.0, 0.0, -1 * mGravitationValue->get());
	}
	else {
		dWorldSetGravity(mODEWorld, 0.0, -1 * mGravitationValue->get(), 0.0);
	}
		
	mODECollisionHandler = dynamic_cast<ODE_CollisionHandler*>(
		Physics::getCollisionManager()->getCollisionHandler());	
	if(mODECollisionHandler == 0) {
		Core::log("ODE_SimulationAlgorithm: CollisionHandler could not be found or is "
			"not an instance of ODE_CollisionHandler.");
		return false;
	}
	return true;
}

/**
 * Called if one of the algorithms parameters have changed.
 * The changes of the values are directly applied to the IBDS 
 * Simulation object.
 *
 * @param value the value that changed.
 */
void ODE_SimulationAlgorithm::valueChanged(Value *value) {
	ParameterizedObject::valueChanged(value);
}



bool ODE_SimulationAlgorithm::finalizeSetup() {
	QList<SimBody*> bodies = Physics::getPhysicsManager()->getSimBodies();

	for(QListIterator<SimBody*> i(bodies); i.hasNext();) {
		ODE_Body *tmp = dynamic_cast<ODE_Body*>(i.next());
		if(tmp == 0) {
			//do not create collision objects for non-ode bodies.
			continue; 
		}
		tmp->createODECollisionObjects();
	}
	return true;
}

dWorldID ODE_SimulationAlgorithm::getODEWorldID() const {
	return mODEWorld;
}

dSpaceID ODE_SimulationAlgorithm::getODEWorldSpaceID() const {
	return mODEMainSpace;
}

dJointGroupID ODE_SimulationAlgorithm::getContactJointGroupID() const {
	return mContactJointGroup;
}

dJointGroupID ODE_SimulationAlgorithm::getJointGroupID() const {
	return mGeneralJointGroup;
}

/**
 * Executes an ode-simulation step.
 * @param p Pointer to the valid PhysicsManager.
 * @return Returns whether the step-execution was successful.
 */
bool ODE_SimulationAlgorithm::executeSimulationStep(PhysicsManager *pManager) {
	QTime time;
	bool measurePerformance = Core::getInstance()->isPerformanceMeasuringEnabled();

	int iterationsPerStep = mIterationsPerStepValue->get();	

	if(measurePerformance) {
		//Run physics with performance measuring.

		int durationMotorUpdate = 0;
		int durationSensorUpdate = 0;
		int durationStep = 0;
		int durationCollision = 0;

		time.start();

		for(int i = 0; i < iterationsPerStep; ++i) {
			pManager->updateActuators();
			durationMotorUpdate += time.restart();
			mODECollisionHandler->clearContactList();
			dSpaceCollide(mODEMainSpace, 0, &nearCallback);
			durationCollision += time.restart();
			dWorldStep(mODEWorld, mTimeStepSizeValue->get());
			dJointGroupEmpty(mContactJointGroup);
			durationStep += time.restart();
			pManager->updateSensors();
			durationSensorUpdate += time.restart();
		}
		mUpdateMotorsDurationValue->set(durationMotorUpdate);
		mUpdateSensorsDurationValue->set(durationSensorUpdate);
		mStepDurationValue->set(durationStep);
		mStepCollisionDurationValue->set(durationCollision);
	}
	else {
		//Run physics without performance measuring.

		for(int i = 0; i < iterationsPerStep; ++i) {
			pManager->updateActuators();
			mODECollisionHandler->clearContactList();
			dSpaceCollide(mODEMainSpace, 0, &nearCallback);
			dWorldStep(mODEWorld, mTimeStepSizeValue->get());
			dJointGroupEmpty(mContactJointGroup);
			pManager->updateSensors();
		}
	}

	return true;
}

/**
 * Callback method called for every collision, that was reported by the ode-engine.
 * @param data 
 * @param o1 
 * @param o2 
 */
void ODE_SimulationAlgorithm::nearCallback (void *data, dGeomID o1, dGeomID o2) {

	//CollisionHandler has to be referenced via global instance objects, since this
	//is a static and not a member function.
	ODE_CollisionHandler *handler = dynamic_cast<ODE_CollisionHandler*>(
			Physics::getCollisionManager()->getCollisionHandler());
	if(handler == 0) {
		Core::log("ODE_SimulationAlgorithm: CollisionHandler could not be found or is not"
			" an ODE_CollisionHandler (Skipping collision-callback).");
		return;
	}
	handler->collisionCallback(data, o1, o2);
}

}
