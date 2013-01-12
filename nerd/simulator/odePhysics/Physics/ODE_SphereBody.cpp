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


#include "ODE_SphereBody.h"
#include "Collision/CollisionObject.h"
#include "ODE_SimulationAlgorithm.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"

namespace nerd {

ODE_SphereBody::ODE_SphereBody(const QString &name)
	: SphereBody(name), mRigidBodyID(0), mRigidBodyInitialized(false),
	  mCollisionGeometryInitialized(false)
{
	dMassSetZero(&mBodyMass);
}

ODE_SphereBody::ODE_SphereBody(const QString &name, double radius) : SphereBody(name, radius), 
		mRigidBodyID(0), mRigidBodyInitialized(false), mCollisionGeometryInitialized(false)
{
	dMassSetZero(&mBodyMass);
}

ODE_SphereBody::ODE_SphereBody(const ODE_SphereBody &body) : Object(), 
		ValueChangedListener(), ODE_Body(), SphereBody(body), mRigidBodyID(0),
		mRigidBodyInitialized(false), mCollisionGeometryInitialized(false)
{
	dMassSetZero(&mBodyMass);
}

ODE_SphereBody::~ODE_SphereBody(){
}


SimBody* ODE_SphereBody::createCopy() const {
	return new ODE_SphereBody(*this);
}

void ODE_SphereBody::setup() {
	SphereBody::setup();
	ODE_Body::setup();

	double radius = mRadius->get();
	if(radius <= 0.0) {
		radius = 0.00001;
		mRadius->set(radius);
	}
	
	if(mDynamicValue->get() == false) {
		return;
	}
	ODE_SimulationAlgorithm *algorithm = dynamic_cast<ODE_SimulationAlgorithm*>(
			Physics::getPhysicsManager()->getPhysicalSimulationAlgorithm());
	if(algorithm == 0) {
		Core::log("ODE_SphereBody: Simulation algorithm could not be found "
			"or is not an ODE_SimulationAlgorithm.");
		return;
	}
	dWorldID worldID = algorithm->getODEWorldID();
	if(worldID == 0) {
		Core::log("ODE_SphereBody: Could not setup ODE body. ODE physics is not initialized!");
		return;
	}
	mRigidBodyID = dBodyCreate(worldID);

	// The rotation of the body has to be considered when the position of the ode-body (which coincides with the COM) is calculated. The center of mass value is understood as "local" displacement from the body position.
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
// 	dBodySetPosition(mRigidBodyID, 
// 		mPositionValue->getX() + mCenterOfMassValue->getX(),
// 		mPositionValue->getY() + mCenterOfMassValue->getY(), 
// 		mPositionValue->getZ() + mCenterOfMassValue->getZ());

	dQuaternion quat = {mQuaternionOrientationValue->getW(),
						mQuaternionOrientationValue->getX(),
						mQuaternionOrientationValue->getY(),
						mQuaternionOrientationValue->getZ()};
	dBodySetQuaternion(mRigidBodyID, quat);

	//Change mass and radius to valid values, if they are 0.0
	double mass = mMassValue->get();
	if(mass <= 0.0) {
		mass = 0.1;
		mMassValue->set(mass);
	}
	
	dMassSetSphereTotal(&mBodyMass, mass, radius);
	dBodySetMass(mRigidBodyID, &mBodyMass);
	mBodyCollisionObject->setMass(mass);
	mRigidBodyInitialized = true;
}

void ODE_SphereBody::clear() {
	SphereBody::clear();
	ODE_Body::clear();
	mRigidBodyID = 0;
	mRigidBodyInitialized = false;
	dMassSetZero(&mBodyMass);
	for(int i = 0; i < mCollisionObjects.size(); i++) {
		CollisionObject *c = mCollisionObjects.at(i);
		c->setNativeCollisionObject(0);
	}
}

void ODE_SphereBody::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa) {
	if(psa != 0 && dynamic_cast<ODE_SimulationAlgorithm*>(psa) == 0) {
		return;
	}
	mSynchronizingWithPhysicalModel = true;
	dGeomID sphereGeom = (dGeomID) getCollisionObjects().at(0)->getNativeCollisionObject();
	synchronizePositionAndOrientation(sphereGeom, this);
// 	//TODO export forces and collisions.
	mSynchronizingWithPhysicalModel = false;

	ODE_Body::clearFeedbackList();
}


dBodyID ODE_SphereBody::getRigidBodyID() {
	if(mRigidBodyID == 0 && mRigidBodyInitialized == false) {
		setup();
	}
	return mRigidBodyID;
}

void ODE_SphereBody::valueChanged(Value *value) {
	SphereBody::valueChanged(value);
	if(value == 0 || !mRigidBodyInitialized) {
		return;
	}
}

void ODE_SphereBody::updateOrientation() {
	dQuaternion quaternion =
				{mQuaternionOrientationValue->get().getW(),
				mQuaternionOrientationValue->get().getX(), mQuaternionOrientationValue->get().getY(), mQuaternionOrientationValue->get().getZ()};
	dBodySetQuaternion(mRigidBodyID, quaternion);
}

}
