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

#include "ODE_CylinderBody.h"
#include "Collision/CollisionObject.h"
#include "ODE_SimulationAlgorithm.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"

namespace nerd {

ODE_CylinderBody::ODE_CylinderBody(const QString &name) : CylinderBody(name), 
		mRigidBodyID(0), mRigidBodyInitialized(false), mCollisionGeometryInitialized(false)
{
	dMassSetZero(&mBodyMass);
}

ODE_CylinderBody::ODE_CylinderBody(const QString &name, double radius, double length) 
		: CylinderBody(name, radius, length), mRigidBodyID(0), mRigidBodyInitialized(false), 
		mCollisionGeometryInitialized(false)
{
	dMassSetZero(&mBodyMass);
}

ODE_CylinderBody::ODE_CylinderBody(const ODE_CylinderBody &body) : Object(), 
		ValueChangedListener(), ODE_Body(), CylinderBody(body), mRigidBodyID(0), 
		mRigidBodyInitialized(false), mCollisionGeometryInitialized(false)
{
	dMassSetZero(&mBodyMass);
}

ODE_CylinderBody::~ODE_CylinderBody() {
}


SimBody* ODE_CylinderBody::createCopy() const {
	return new ODE_CylinderBody(*this);
}

void ODE_CylinderBody::setup() {
	CylinderBody::setup();
	ODE_Body::setup();

	double radius = mRadius->get();
	if(radius <= 0.0) {
		radius = 0.00001;
		mRadius->set(radius);
		Core::log("ODE_CylinderBody: The radius of the cylinder \"" + getName() + "\" was"
			" negative. The radius is set to the default value: 0.00001");
	}
	
	double length = mLength->get();
	if(length <= 0.0) {
		length = 0.00001;
		mLength->set(length);
	}
	
	if(mDynamicValue->get() == false) {
		return;
	}
	ODE_SimulationAlgorithm *algorithm = dynamic_cast<ODE_SimulationAlgorithm*>(
			Physics::getPhysicsManager()->getPhysicalSimulationAlgorithm());
	if(algorithm == 0) {
		Core::log("ODE_CylinderBody: Simulation algorithm could not be found "
				"or is not an ODE_SimulationAlgorithm.");
		return;
	}
	dWorldID worldID = algorithm->getODEWorldID();
	if(worldID == 0) {
		Core::log("ODE_CylinderBody: Could not setup ODE body. ODE physics is not initialized!");
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

	dQuaternion quat = {mQuaternionOrientationValue->getW(),
						mQuaternionOrientationValue->getX(),
						mQuaternionOrientationValue->getY(),
						mQuaternionOrientationValue->getZ()};

	dBodySetQuaternion(mRigidBodyID, quat);

	// Mass and radius are checked for validity and are altered to valid default values in case of errors.
	double mass = mMassValue->get();
	if(mass <= 0.0) {
		mass = 0.1;
		mMassValue->set(mass);
		Core::log("ODE_CylinderBody: The mass of the cylinder \"" + getName() + "\" was "
			"negative. The mass is set to the default value: 0.1");
	}

	dMassSetCylinderTotal(&mBodyMass, mass, 3, radius, length);
	dBodySetMass(mRigidBodyID, &mBodyMass);
	mBodyCollisionObject->setMass(mass);
	mRigidBodyInitialized = true;
}

void ODE_CylinderBody::clear() {
	CylinderBody::clear();
	ODE_Body::clear();

	mRigidBodyID = 0;
	mRigidBodyInitialized = false;
	dMassSetZero(&mBodyMass);
	for(int i = 0; i < mCollisionObjects.size(); i++) {
		CollisionObject *c = mCollisionObjects.at(i);
		c->setNativeCollisionObject(0);
	}
}

void ODE_CylinderBody::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa) {
	if(psa != 0 && dynamic_cast<ODE_SimulationAlgorithm*>(psa) == 0) {
		return;
	}
	mSynchronizingWithPhysicalModel = true;
	dGeomID cylinderGeom = (dGeomID) getCollisionObjects().at(0)->getNativeCollisionObject();
	synchronizePositionAndOrientation(cylinderGeom, this);
	//TODO export forces and collisions.
	mSynchronizingWithPhysicalModel = false;

	ODE_Body::clearFeedbackList();
}


dBodyID ODE_CylinderBody::getRigidBodyID() {
	if(mRigidBodyID == 0 && mRigidBodyInitialized == false) {
		setup();
	}
	return mRigidBodyID;
}

void ODE_CylinderBody::valueChanged(Value *value) {
	CylinderBody::valueChanged(value);
	if(value == 0 || !mRigidBodyInitialized) {
		return;
	}
}

void ODE_CylinderBody::updateOrientation() {
	dQuaternion quaternion =
				{mQuaternionOrientationValue->get().getW(),
				mQuaternionOrientationValue->get().getX(), mQuaternionOrientationValue->get().getY(), mQuaternionOrientationValue->get().getZ()};
	dBodySetQuaternion(mRigidBodyID, quaternion);
}

}
