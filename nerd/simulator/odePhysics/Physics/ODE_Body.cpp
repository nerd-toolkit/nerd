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


#include "ODE_Body.h"
#include "ODE_Plane.h"
#include "ODE_SimulationAlgorithm.h"
#include "Physics/SimBody.h"
#include "Physics/BoxBody.h"
#include "Physics/PhysicsManager.h"
#include "Collision/CollisionObject.h"
#include "Physics/TriangleGeom.h"
#include "Physics/SphereGeom.h"
#include "Physics/CylinderGeom.h"
#include "Physics/RayGeom.h"
#include "Core/Core.h"
#include "Physics/BoxGeom.h"
#include "Physics/CylinderGeom.h"
#include "Physics/Physics.h"
#include "Value/Vector3DValue.h"
#include "Physics/SimGeom.h"
#include "Physics/CapsuleGeom.h"
#include <iostream>

using namespace std;

namespace nerd {


ODE_Body::ODE_Body()
	: mGeomPoints(0), mGeomTriangles(0), mCollectForceFeedback(false) 
{
}

ODE_Body::~ODE_Body() {
	delete[] mGeomTriangles;
	delete[] mGeomPoints;
}
			
/**
 * Creates for every collision objects of the (ODE) SimBody-object an ODE geometry and connects it with the ode body.
 * @return 
 */
bool ODE_Body::createODECollisionObjects() {

	SimBody *body = dynamic_cast<SimBody*>(this);

	Vector3D comOffset = body->getCenterOfMassValue()->get();
	if(body == 0)  {
		Core::log("ODE_Body: Error creating collision objects. No SimBody.");
		return false;
	}
	BoolValue* isDynamic = dynamic_cast<BoolValue*>(body->getParameter("Dynamic"));
	if(isDynamic == 0) {
		Core::log("ODE_Body: Error creating collision objects. Parameter \"Dynamic\" is missing.");
		return false;
	}
	if(getRigidBodyID() == 0 && (isDynamic->get() == true)) {
		Core::log("ODE_Body: Error creating collision objects. No dBodyID for dynamic simulation body.");
		return false;
	}
 	ODE_SimulationAlgorithm *algo = dynamic_cast<ODE_SimulationAlgorithm*>(
			Physics::getPhysicsManager()->getPhysicalSimulationAlgorithm());
	if(algo == 0) {
		Core::log("ODE_Body: Simulation algorithm could not be found or is not an instance"
			" of ODE_SimulationAlgorithm.");
		return false; 
	} 
	
	QList<CollisionObject*> collisionObjects = body->getCollisionObjects();

	double bodyMass = 0.0;

	for(int i = 0; i < collisionObjects.size(); i++) {

		CollisionObject *co = collisionObjects.at(i);
		co->setNativeCollisionObject(0);

		if(isDynamic->get() == true) {
			bodyMass += co->getMass();
		}
		SimGeom *geom = co->getGeometry();

		if(geom == 0) {
			Core::log("ODE_BOdy: Could not find a geom to create the ODE collision objects.");
			continue;
		}

		if(dynamic_cast<ODE_Plane*>(body) != 0) {

			ODE_Plane *planeBody = dynamic_cast<ODE_Plane*>(body);
			Vector3DValue *axis = 
					dynamic_cast<Vector3DValue*>(planeBody->getParameter("Axis"));
			Vector3D axisNormalized = axis->get();
			axisNormalized.normalize();
			axis->set(axisNormalized);
			DoubleValue *distance = 
					dynamic_cast<DoubleValue*>(planeBody->getParameter("Distance"));
			if(axis != 0 && distance != 0) {	
				dGeomID plane = dCreatePlane(algo->getODEWorldSpaceID(), 
								axis->getX(), axis->getY(), axis->getZ(), distance->get());
				co->setNativeCollisionObject(plane);
			} 
			else {
				return false;
			}
		}
		else if(dynamic_cast<TriangleGeom*>(geom) != 0) {

			if(dynamic_cast<BoxGeom*>(geom) != 0) {
				BoxGeom *box = dynamic_cast<BoxGeom*>(geom);
				if(box == 0) {
				Core::log("ODE_Body: Could not create collision objects.");
				return false;
				}
				dGeomID boxGeom = dCreateBox(algo->getODEWorldSpaceID(),
								box->getWidth(), 
								box->getHeight(), box->getDepth());	
				if(isDynamic->get() == true) {
					dGeomSetBody(boxGeom, getRigidBodyID());
					Vector3D positionOffset = box->getLocalPosition();
					dGeomSetOffsetPosition(boxGeom, positionOffset.getX() - comOffset.getX(), 
						positionOffset.getY() - comOffset.getY(), 
						positionOffset.getZ() - comOffset.getZ());
					Quaternion orientationOffset = box->getLocalOrientation();
					dQuaternion quat = {orientationOffset.getW(), 
						orientationOffset.getX(), orientationOffset.getY(), 
						orientationOffset.getZ()};
					dGeomSetOffsetQuaternion(boxGeom, quat);
				} 
				else {		
					Vector3D positionOffset = box->getLocalPosition();
					dGeomSetPosition(boxGeom, body->getPositionValue()->getX() + 
						positionOffset.getX() - comOffset.getX(), 
						body->getPositionValue()->getY() + positionOffset.getY() - 
						comOffset.getY(), body->getPositionValue()->getZ() + 
						positionOffset.getZ() - comOffset.getZ());
					Quaternion quat(body->getQuaternionOrientationValue()->getW(), 
						body->getQuaternionOrientationValue()->getX(), 
						body->getQuaternionOrientationValue()->getY(), 
						body->getQuaternionOrientationValue()->getZ());
					Quaternion orientationOffset = box->getLocalOrientation();
					Quaternion product = orientationOffset * quat;
					dQuaternion quaternion = {product.getW(), product.getX(), 
						product.getY(), product.getZ()};
					dGeomSetQuaternion(boxGeom, quaternion);
				}
				co->setNativeCollisionObject(boxGeom);
				continue;
			}
			TriangleGeom *tgeom = dynamic_cast<TriangleGeom*>(geom);
			if(tgeom == 0) {
				Core::log("ODE_Body: Could not create collision objects.");
				return false;
			}
			QVector<Vector3D> pointVector = tgeom->getPoints();
			mGeomPoints = new float[pointVector.size() * 3];
			{
				for(int j = 0; j < pointVector.size(); ++j) {
					Vector3D point = pointVector.at(j);
					mGeomPoints[3 * j + 0] = point.getX();
					mGeomPoints[3 * j + 1] = point.getY();
					mGeomPoints[3 * j + 2] = point.getZ();
				}
			}
			QVector<Triangle> triangleVector = tgeom->getTriangles();
			
			mGeomTriangles = new int[triangleVector.size() * 3];
			{
				for(int j = 0; j < triangleVector.size(); ++j) {
					Triangle triangle = triangleVector.at(j);
					mGeomTriangles[3 * j + 0] = triangle.mEdge[0];
					mGeomTriangles[3 * j + 1] = triangle.mEdge[1];
					mGeomTriangles[3 * j + 2] = triangle.mEdge[2];
				}
			}
			dTriMeshDataID meshData = dGeomTriMeshDataCreate();
      		dGeomTriMeshDataBuildSingle(meshData, &mGeomPoints[0], 3 * sizeof(float), 
				pointVector.size(), (int*)&mGeomTriangles[0], 
				triangleVector.size() * 3, 3 * sizeof(int));
			dGeomID meshID = dCreateTriMesh(algo->getODEWorldSpaceID(), 
				meshData, 0, 0, 0);
			dGeomSetData(meshID, meshData);
			if(isDynamic->get() == true) {
				dGeomSetBody(meshID, getRigidBodyID());
				dGeomSetOffsetPosition(meshID, 0.0, 0.0, 0.0);
				dQuaternion quat = {0.0, 0.0, 0.0, 0.0};
 				dGeomSetOffsetQuaternion(meshID, quat);
			} 
			else {
				dGeomSetPosition(meshID, 
					body->getPositionValue()->getX(), 
					body->getPositionValue()->getY(), 
					body->getPositionValue()->getZ());
				dQuaternion quat = { body->getQuaternionOrientationValue()->getW(),
					body->getQuaternionOrientationValue()->getX(), 
					body->getQuaternionOrientationValue()->getY(), 
					body->getQuaternionOrientationValue()->getZ() };
				dGeomSetQuaternion(meshID, quat);
			}
			co->setNativeCollisionObject(meshID);
		}
		else if(dynamic_cast<SphereGeom*>(geom) != 0) {

			SphereGeom *sphere = dynamic_cast<SphereGeom*>(geom);
			if(sphere == 0) {
				Core::log("ODE_Body: Could not create collision objects.");
				return false;
			}
			Vector3D translation = sphere->getLocalPosition();
			dGeomID sphereGeomID = dCreateSphere(algo->getODEWorldSpaceID(), 
				sphere->getRadius());
			
			if(isDynamic->get() == true) {
				dGeomSetBody(sphereGeomID, getRigidBodyID());
				Vector3D positionOffset = sphere->getLocalPosition();
				dGeomSetOffsetPosition(sphereGeomID, positionOffset.getX() - 
					comOffset.getX(), positionOffset.getY() - comOffset.getY(), 
					positionOffset.getZ() - comOffset.getZ());
				Quaternion orientationOffset = sphere->getLocalOrientation();
				dQuaternion quat = { orientationOffset.getW(),
					orientationOffset.getX(), orientationOffset.getY(), 
					orientationOffset.getZ() };
				dGeomSetOffsetQuaternion(sphereGeomID, quat);
			} 
			else {
				dGeomSetPosition(sphereGeomID, 
					body->getPositionValue()->getX() + translation.getX(),
					body->getPositionValue()->getY() + translation.getY(), 
					body->getPositionValue()->getZ() + translation.getZ());
				Quaternion quat(body->getQuaternionOrientationValue()->getW(), 			
					body->getQuaternionOrientationValue()->getX(), 			
					body->getQuaternionOrientationValue()->getY(), 			
					body->getQuaternionOrientationValue()->getZ());
				Quaternion orientationOffset = sphere->getLocalOrientation();
				Quaternion product = orientationOffset * quat;
				dQuaternion quaternion = {product.getW(), product.getX(), product.getY(),
					product.getZ()};
				dGeomSetQuaternion(sphereGeomID, quaternion);
			}
			co->setNativeCollisionObject(sphereGeomID);
		} 
		else if(dynamic_cast<CylinderGeom*>(geom) != 0) {

			CylinderGeom *cylinder = dynamic_cast<CylinderGeom*>(geom);
			if(cylinder == 0) {
				Core::log("ODE_Body: Could not create collision objects.");
				return false;
			}
			Vector3D translation = cylinder->getLocalPosition();
			dGeomID cylinderGeomID = dCreateCylinder(algo->getODEWorldSpaceID(),
				cylinder->getRadius(), cylinder->getLength());
			
			if(isDynamic->get() == true) {
				dGeomSetBody(cylinderGeomID, getRigidBodyID());
				Vector3D positionOffset = cylinder->getLocalPosition();
				dGeomSetOffsetPosition(cylinderGeomID, positionOffset.getX() - 
					comOffset.getX(), positionOffset.getY() - comOffset.getY(), 
					positionOffset.getZ() - comOffset.getZ());
				Quaternion orientationOffset = cylinder->getLocalOrientation();
				dQuaternion quat = { orientationOffset.getW(), orientationOffset.getX(), 
					orientationOffset.getY(), orientationOffset.getZ() };
 				dGeomSetOffsetQuaternion(cylinderGeomID, quat);
			} 
			else {
				dGeomSetPosition(cylinderGeomID, 
					body->getPositionValue()->getX() + translation.getX(), 
					body->getPositionValue()->getY() + translation.getY(), 
					body->getPositionValue()->getZ() + translation.getZ());
				Quaternion quat(body->getQuaternionOrientationValue()->getW(), 			
					body->getQuaternionOrientationValue()->getX(), 			
					body->getQuaternionOrientationValue()->getY(), 			
					body->getQuaternionOrientationValue()->getZ());
				Quaternion orientationOffset = cylinder->getLocalOrientation();
				Quaternion product = orientationOffset * quat;
				dQuaternion quaternion = { product.getW(), product.getX(), 
					product.getY(), product.getZ() };
				dGeomSetQuaternion(cylinderGeomID, quaternion);
			}
			co->setNativeCollisionObject(cylinderGeomID);
		}
		else if(dynamic_cast<RayGeom*>(geom) != 0) {

			RayGeom *ray = dynamic_cast<RayGeom*>(geom);
			if(ray == 0) {
				Core::log("ODE_Body: Could not create collision objects.");
				return false;
			}
			Vector3D translation = ray->getLocalPosition();
			dGeomID rayGeomID = dCreateRay(algo->getODEWorldSpaceID(), 
				ray->getLength());
			dGeomRaySetLength(rayGeomID, ray->getLength());
			
			if(isDynamic->get() == true) {
				dGeomSetBody(rayGeomID, getRigidBodyID());
				Quaternion orientationOffset = ray->getLocalOrientation();
				dQuaternion quat = { orientationOffset.getW(),orientationOffset.getX(),
					orientationOffset.getY(), orientationOffset.getZ() };
 				dGeomSetOffsetQuaternion(rayGeomID, quat);
				dGeomSetOffsetPosition(rayGeomID, translation.getX(), 
					translation.getY(), translation.getZ());
			} 
			else {
				Quaternion quat(body->getQuaternionOrientationValue()->getW(), 			
					body->getQuaternionOrientationValue()->getX(), 			
					body->getQuaternionOrientationValue()->getY(), 			
					body->getQuaternionOrientationValue()->getZ());
				Quaternion orientationOffset = ray->getLocalOrientation();
				Quaternion product = orientationOffset * quat;
				dQuaternion quaternion = { product.getW(), product.getX(), 
					product.getY(), product.getZ() };
				dGeomSetQuaternion(rayGeomID, quaternion);
				dGeomSetPosition(rayGeomID, 
					body->getPositionValue()->getX() + translation.getX(), 
					body->getPositionValue()->getY() + translation.getY(), 
					body->getPositionValue()->getZ() + translation.getZ());
			}
			co->setNativeCollisionObject(rayGeomID);
		}
		else if(dynamic_cast<CapsuleGeom*>(geom) != 0) {

			CapsuleGeom *capsule = dynamic_cast<CapsuleGeom*>(geom);
			if(capsule == 0) {
				Core::log("ODE_Body: Could not create collision objects.");
				return false;
			}
			Vector3D translation = capsule->getLocalPosition();
			dGeomID capsuleGeomID = dCreateCCylinder(algo->getODEWorldSpaceID(), 
				capsule->getRadius(), capsule->getLength());
			
			if(isDynamic->get() == true) {
				dGeomSetBody(capsuleGeomID, getRigidBodyID());
				Vector3D positionOffset = capsule->getLocalPosition();
				dGeomSetOffsetPosition(capsuleGeomID, positionOffset.getX() - 
					comOffset.getX(), positionOffset.getY() - comOffset.getY(), 
					positionOffset.getZ() - comOffset.getZ());
				Quaternion orientationOffset = capsule->getLocalOrientation();
				dQuaternion quat = {orientationOffset.getW(),
									orientationOffset.getX(), 
									orientationOffset.getY(), 
									orientationOffset.getZ()};
				dGeomSetOffsetQuaternion(capsuleGeomID, quat);
			} 
			else {
				dGeomSetPosition(capsuleGeomID, 
					body->getPositionValue()->getX() + translation.getX(),
					body->getPositionValue()->getY() + translation.getY(), 
					body->getPositionValue()->getZ() + translation.getZ());
				Quaternion quat(body->getQuaternionOrientationValue()->getW(), 			
					body->getQuaternionOrientationValue()->getX(), 			
					body->getQuaternionOrientationValue()->getY(), 			
					body->getQuaternionOrientationValue()->getZ());
				Quaternion orientationOffset = capsule->getLocalOrientation();
				Quaternion product = orientationOffset * quat;
				dQuaternion quaternion = {product.getW(), product.getX(), product.getY(),
					product.getZ()};
				dGeomSetQuaternion(capsuleGeomID, quaternion);
			}
			co->setNativeCollisionObject(capsuleGeomID);
		} 
		else {
			Core::log("ODE_Body: Found a unknown geometry object during creating the"
				" collision objects of the SimBody \"" + body->getName() + "\"", true);
			continue;
		}
	}	

	if(isDynamic->get() == true) {
		dMass mass;
		dBodyGetMass(getRigidBodyID(), &mass);
		dMassAdjust(&mass, bodyMass);
		dBodySetMass(getRigidBodyID(), &mass);
		dBodySetAngularVel(getRigidBodyID(), 0,0,0);
		if(mass.mass < 0.00001) {
			Core::log("ODE_Body: The mass of \"" + body->getName() + "\" might be too"
				" small.");	
		}
	}

	return true;
}

void ODE_Body::synchronizePositionAndOrientation(dGeomID odeGeom, SimBody *simBody) {
	if(odeGeom == 0) {
		return;
	}

	if(simBody == 0) {
		return;
	}
	//Position
	BoolValue* isDynamic = simBody->getDynamicValue();
	if(isDynamic == 0) {
		Core::log("ODE_Body: Could not find required Value.");
		return;
	}

	if(isDynamic->get() == false) {
		bool notFound = true;
		int counter = 0;
		int objectIndex = 0;

		QList<CollisionObject*> collisionObjects = simBody->getCollisionObjects();

		while(notFound && counter < collisionObjects.size()) {
			if(collisionObjects.at(counter)->getNativeCollisionObject() == odeGeom) 
			{
				notFound = false;
				objectIndex = counter;
				break;
			}
			counter++;
		}
		if(notFound) {
			return;
		}

		CollisionObject *co = simBody->getCollisionObjects().at(objectIndex);
		Vector3D localPos(0,0,0);
		SimGeom *geom = co->getGeometry();
		if(geom != 0) {
			if((dynamic_cast<SphereGeom*>(geom) != 0) 
			|| (dynamic_cast<BoxGeom*>(geom) != 0)
			|| (dynamic_cast<CylinderGeom*>(geom) != 0)
			|| (dynamic_cast<CapsuleGeom*>(geom) != 0)) 
			{
				localPos = Vector3D(geom->getLocalPosition());
			}
// // 			Vector3D pos(dGeomGetPosition(odeGeom)[0], dGeomGetPosition(odeGeom)[1], 
// // 				dGeomGetPosition(odeGeom)[2]);
// // 			simBody->getPositionValue()->set(pos.getX() - localPos.getX(), 
// // 				pos.getY() - localPos.getY(), pos.getZ() - localPos.getZ());
// 			const dReal *geomPos = dGeomGetPosition(odeGeom);
// 			simBody->getPositionValue()->set(geomPos[0] - localPos.getX(), 
// 				geomPos[1] - localPos.getY(), geomPos[2] - localPos.getZ());
// 	
// 			//Orientation
// 			dQuaternion quaternion;
// 			dGeomGetQuaternion(odeGeom, quaternion);
// 			Quaternion quad(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
// 			simBody->getQuaternionOrientationValue()->set(quad);
// 			simBody->getOrientationValue()->set(quad.toAngles());

			//Orientation
			dQuaternion quaternion;
			dGeomGetQuaternion(odeGeom, quaternion);
			Quaternion quad(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
			simBody->getQuaternionOrientationValue()->set(quad);
			simBody->getOrientationValue()->set(quad.toAngles());


			// Position
			const dReal *bodyPos = dGeomGetPosition(odeGeom);
			Vector3D pos(bodyPos[0], bodyPos[1], bodyPos[2]);
			Vector3DValue *com = simBody->getCenterOfMassValue();
			Quaternion comQuat(0.0, 
							com->getX(), 
							com->getY(), 
							com->getZ());
			Quaternion orientationInverse = 
				simBody->getQuaternionOrientationValue()->get().getInverse();
			Quaternion rotatedComQuat = simBody->getQuaternionOrientationValue()->get() 
				* comQuat * orientationInverse;
			Vector3D rotatedCom(rotatedComQuat.getX(), 
									rotatedComQuat.getY(), 
									rotatedComQuat.getZ());
			simBody->getPositionValue()->set(pos.getX() + rotatedCom.getX(), 
				pos.getY() + rotatedCom.getY(), pos.getZ() + rotatedCom.getZ());
		}
	} 
	else {
		
		dBodyID bodyID = dGeomGetBody(odeGeom);
		if(bodyID != 0) {
			bool notFound = true;
			int counter = 0;
			int objectIndex = 0;
			while(notFound && counter < simBody->getCollisionObjects().size()) {
				if(simBody->getCollisionObjects().at(counter)->getNativeCollisionObject() 
					== odeGeom) 
				{
					notFound = false;
					objectIndex = counter;
					break;
				}
				counter++;
			}
			if(notFound) {
				return;
			}

			CollisionObject *co = simBody->getCollisionObjects().at(objectIndex);
			Vector3D localPos(0,0,0);
			SimGeom *geom = co->getGeometry();
			if(geom != 0) {
				if((dynamic_cast<SphereGeom*>(geom) != 0) 
					|| (dynamic_cast<BoxGeom*>(geom) != 0)
					|| (dynamic_cast<CylinderGeom*>(geom) != 0)) 
				{
					localPos = Vector3D(geom->getLocalPosition());
				}
	
				//Orientation
				const dReal *bodyQuat = dBodyGetQuaternion(bodyID);
				Quaternion quad(bodyQuat[0], bodyQuat[1], bodyQuat[2], bodyQuat[3]);
				simBody->getQuaternionOrientationValue()->set(quad);
				simBody->getOrientationValue()->set(quad.toAngles());
	
				// Position
				const dReal *bodyPos = dBodyGetPosition(bodyID);
				Vector3D pos(bodyPos[0], bodyPos[1], bodyPos[2]);
				Vector3DValue *com = simBody->getCenterOfMassValue();
				Quaternion comQuat(0.0, 
								com->getX(), 
								com->getY(), 
								com->getZ());
				Quaternion orientationInverse = 
					simBody->getQuaternionOrientationValue()->get().getInverse();
				Quaternion rotatedComQuat = simBody->getQuaternionOrientationValue()->get() 
					* comQuat * orientationInverse;
				Vector3D rotatedCom(rotatedComQuat.getX(), 
										rotatedComQuat.getY(), 
										rotatedComQuat.getZ());
				simBody->getPositionValue()->set(pos.getX() - rotatedCom.getX(), 
					pos.getY() - rotatedCom.getY(), pos.getZ() - rotatedCom.getZ());
			}
		}
	}
}


void ODE_Body::setup() {
}

void ODE_Body::clear() {
	SimBody *body = dynamic_cast<SimBody*>(this);
	if(body == 0) {
		return;
	}	
	QList<CollisionObject*> collisionObjects = body->getCollisionObjects();
	for(int i = 0; i < collisionObjects.size(); ++i) {
		CollisionObject *co = collisionObjects.at(i);
		co->setNativeCollisionObject(0);
	}
	delete[] mGeomTriangles;
	delete[] mGeomPoints;
}


/**
 * Enables or disables the collection of force feedback information for contact joints.
 * This slows down processing a bit, therefore this has to be enabled explicitely for each
 * body who needs this information. This is useful for instance to create force sensors.
 */
void ODE_Body::enableContactJointFeedback(bool enable) {
	mCollectForceFeedback = enable;
}


bool ODE_Body::isFeedbackEnabled() const {
	return mCollectForceFeedback;
}

/**
 * Destroys all feedback structures owned by this body (but not the structures "borrowed" from 
 * a collision partner.
 */
void ODE_Body::clearFeedbackList() {

	mContactJointFeedback.clear();

	while(!mOwnedContactJointFeedbacks.empty()) {
		dJointFeedback *feedback = mOwnedContactJointFeedbacks.at(0);
		mOwnedContactJointFeedbacks.removeAll(feedback);
		delete feedback;
	}
	
}


/**
 * Contains all dJointFeedback objects that where created for collisions with this object.
 * Includes both types of strucutures, the ones created and owned by this body, and the 
 * structures owned by the body collided with this body.
 */
QHash<dGeomID, dJointFeedback*>& ODE_Body::getJointFeedbackList() {
	return mContactJointFeedback;
}

/**
 * Contains all dJointFeedback structures that have to be destroyed by this body object.
 */
QList<dJointFeedback*>& ODE_Body::getOwnedJointFeedbackList() {
	return mOwnedContactJointFeedbacks;
}

}
