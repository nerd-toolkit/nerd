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


#include "ODE_BoxBody.h"
#include "Collision/CollisionObject.h"
#include "ODE_SimulationAlgorithm.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"

namespace nerd {

/**
 * Constructor. The parameters for the box geometry and for the physical properties 
 * are initialized with default values (see BoxBody).
 * @param name 
 */
ODE_BoxBody::ODE_BoxBody(const QString &name) : BoxBody(name), mRigidBodyID(0),
		mRigidBodyInitialized(false), mCollisionGeometryInitialized(false)
{
	dMassSetZero(&mBodyMass);
}

/**
 * Constructor where the box geometry parameters are initialized with the given values. 
 * The physical parameters are initialized with default values.
 * @param name 
 * @param width
 * @param height 
 * @param depth 
 */
ODE_BoxBody::ODE_BoxBody(const QString &name, double width, double height, double depth) 
		: BoxBody(name, width, height, depth), mRigidBodyID(0), mRigidBodyInitialized(false),
		mCollisionGeometryInitialized(false)
{
	dMassSetZero(&mBodyMass);
}


ODE_BoxBody::ODE_BoxBody(const ODE_BoxBody &body) : Object(), ValueChangedListener(),
		ODE_Body(), BoxBody(body), mRigidBodyID(0), mRigidBodyInitialized(false), 
		mCollisionGeometryInitialized(false)
{
	dMassSetZero(&mBodyMass);
}

ODE_BoxBody::~ODE_BoxBody(){
}


SimBody* ODE_BoxBody::createCopy() const {
	return new ODE_BoxBody(*this);
}

dBodyID ODE_BoxBody::getRigidBodyID() {
	if(mRigidBodyInitialized == false) {
		setup();
	}
	return mRigidBodyID;
}

/**
 * On simulation start and during a reset, the box is reconstructed. In case of "invalid" parameters (mass=0), these values are set to very small but valid values, to avoid a termination of the simulation due to user-errors.
 */
void ODE_BoxBody::setup() {


	double mass = mMassValue->get();
	if(mass <= 0.0) {
		mass = 0.1;
		mMassValue->set(mass);
	}
	double width = mWidth->get();
	if(width <= 0.0) {
		width = 0.00001;
		mWidth->set(width);
	}
	double height = mHeight->get();
	if(height <= 0.0) {
		height = 0.00001;
		mHeight->set(height);
	}
	double depth = mDepth->get();
	if(depth <= 0.0) {
		depth = 0.00001;
		mDepth->set(depth);
	}

	BoxBody::setup();
	ODE_Body::setup();
	
	// If The object is static, no body has to be created!
	if(mDynamicValue->get() == false) {
		return;
	}
	//create a ode body object with the given properties (mass, geometry, position...)
	ODE_SimulationAlgorithm *algorithm = dynamic_cast<ODE_SimulationAlgorithm*>(
			Physics::getPhysicsManager()->getPhysicalSimulationAlgorithm());
	if(algorithm == 0) {
		Core::log("ODE_BoxBody: Simulation algorithm could not be found "
				  "or is not an instance of ODE_SimulationAlgorithm.");
		return;
	}
	dWorldID worldID = algorithm->getODEWorldID();
	if(worldID == 0) {
		Core::log("ODE_BoxBody: Could not setup ODE body. ODE physics is "
				  "not initialized!");
		return;
	}
	mRigidBodyID = dBodyCreate(worldID);

	// The rotation of the body has to be considered when the position of the ode-body 
	// (which coincides with the COM) is calculated. The center of mass value is understood 
	// as "local" displacement from the body position.
	Quaternion comPosQuat(0.0, 
						mCenterOfMassValue->getX(), 
						mCenterOfMassValue->getY(), 
						mCenterOfMassValue->getZ());
	Quaternion orientationInverse = 
		getQuaternionOrientationValue()->get().getInverse();
	Quaternion rotatedComPosQuat = getQuaternionOrientationValue()->get() 
		* comPosQuat * orientationInverse;
	Vector3D rotatedComPos(rotatedComPosQuat.getX(), 
							 rotatedComPosQuat.getY(), 
							 rotatedComPosQuat.getZ());

	dBodySetPosition(mRigidBodyID, 
		mPositionValue->getX() + rotatedComPosQuat.getX(),
 		mPositionValue->getY() + rotatedComPosQuat.getY(), 
		mPositionValue->getZ() + rotatedComPosQuat.getZ());
// 	dBodySetPosition(mRigidBodyID, mPositionValue->getX() + mCenterOfMassValue->getX(), mPositionValue->getY() + mCenterOfMassValue->getY(), mPositionValue->getZ() + mCenterOfMassValue->getZ());
	
	mBodyCollisionObject->setMass(mass);
	dQuaternion quat;

	quat[0] = mQuaternionOrientationValue->getW();
	quat[1] = mQuaternionOrientationValue->getX();
	quat[2] = mQuaternionOrientationValue->getY();
	quat[3] = mQuaternionOrientationValue->getZ();
	dBodySetQuaternion(mRigidBodyID, quat);

	dMassSetBoxTotal(&mBodyMass, mass, width, height, depth);
	dBodySetMass(mRigidBodyID, &mBodyMass);

	mRigidBodyInitialized = true;

}

void ODE_BoxBody::clear() {
	BoxBody::clear();
	ODE_Body::clear();
	if(mRigidBodyInitialized == true) {
		dBodyDestroy(mRigidBodyID);
	}
	mRigidBodyID = 0;
	mRigidBodyInitialized = false;
	dMassSetZero(&mBodyMass);
	for(int i = 0; i < mCollisionObjects.size(); i++) {
		CollisionObject *c = mCollisionObjects.at(i);
		c->setNativeCollisionObject(0);
	}
}

void ODE_BoxBody::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa) {
	if(psa != 0 && dynamic_cast<ODE_SimulationAlgorithm*>(psa) == 0) {
		return;
	}
	mSynchronizingWithPhysicalModel = true;
	dGeomID boxID = (dGeomID) getCollisionObjects().at(0)->getNativeCollisionObject();
	synchronizePositionAndOrientation(boxID, this);
	//TODO export forces and collisions.
	mSynchronizingWithPhysicalModel = false;

	ODE_Body::clearFeedbackList();
}

void ODE_BoxBody::valueChanged(Value *value) {
	BoxBody::valueChanged(value);

	if(value == 0 || !mRigidBodyInitialized) {
		return;
	}
	else if(value == mQuaternionOrientationValue) {
		if(!mSynchronizingWithPhysicalModel) {
			updateOrientation();
		}
	}
}

void ODE_BoxBody::updateOrientation() {
	dQuaternion quaternion = 
				{mQuaternionOrientationValue->get().getW(),
				mQuaternionOrientationValue->get().getX(), 
				mQuaternionOrientationValue->get().getY(), 
				mQuaternionOrientationValue->get().getZ()};
	dBodySetQuaternion(mRigidBodyID, quaternion);
}
}
