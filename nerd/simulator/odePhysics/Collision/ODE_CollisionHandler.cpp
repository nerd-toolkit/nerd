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


#include "ODE_CollisionHandler.h"
#include "Collision/CollisionManager.h"
#include "Collision/CollisionObject.h"
#include "Collision/MaterialProperties.h"
#include "Physics/ODE_Body.h"
#include "Physics/ODE_SimulationAlgorithm.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include "Core/Core.h"
#include "Math/Math.h"
#include <ode/ode.h>

namespace nerd {

/**
 * Constructor.
 */
ODE_CollisionHandler::ODE_CollisionHandler() 
	: mAlgorithm(0), mMaxContactPointsValue(0) 
{

	ValueManager *vm = Core::getInstance()->getValueManager();

	mMaxContactPoints = 0;
	mSoftERPValue = new DoubleValue(0.2);
	mSoftCFMValue = new DoubleValue(0.00001);
	mBouncingVelocityValue = new DoubleValue(0.0);
	mFrictionScalingValue = new DoubleValue(1.0);
	mContactSlipValue = new DoubleValue(0.001);
	mStaticFrictionAccuracyValue = new IntValue(10);

	vm->addValue("/Simulation/ODE/SoftERP", mSoftERPValue);
	vm->addValue("/Simulation/ODE/SoftCFM", mSoftCFMValue);
	vm->addValue("/Simulation/ODE/MinimumBouncingVelocity", 
		mBouncingVelocityValue);
	vm->addValue("/Simulation/ODE/FrictionScaling", 
		mFrictionScalingValue);
	vm->addValue("/Simulation/ODE/ContactSlip", mContactSlipValue);
	vm->addValue("/Simulation/ODE/StaticFrictionAccuracy", 
		mStaticFrictionAccuracyValue);

	mFrictionScalingFactor = mFrictionScalingValue->get();
	mSoftERP = mSoftERPValue->get();
	mSoftCFM = mSoftCFMValue->get();
	mBouncingVelocity = mBouncingVelocityValue->get();
	mContactSlip = mContactSlipValue->get();
	mStaticFrictionAccuracy = mStaticFrictionAccuracyValue->get();
}	

/**
 * Destructor.
 */
ODE_CollisionHandler::~ODE_CollisionHandler() {
}
	
/**
 * All contacts reported during the last collision detection.
 * @return 
 */
QList<Contact> ODE_CollisionHandler::getContacts() const {
	return mCurrentContacts;
}
	

/**
 * Defines the collision-property between to CollisionObjects. 
 * If set to true the two objects are allowed to penetrate each other.
 *
 * @param firstCollisionPartner 
 * @param secondCollisionPartner 
 * @param disable If true, the two CollisionObjects are allowed to penetrate each other.
 */
void ODE_CollisionHandler::disableCollisions(CollisionObject *firstCollisionPartner, 
		CollisionObject *secondCollisionPartner, bool disable) 
{
	if(firstCollisionPartner == 0 || secondCollisionPartner == 0) {
		Core::log("ODE_CollisionHandler: CollisionObjects are identical.");
		return;
	}
	if(firstCollisionPartner->getHostBody() == 0 
	   || secondCollisionPartner->getHostBody() == 0)
	{
		Core::log("ODE_CollisionHandler: CollisionObject did not provide a host body!");
		return;
	}
		
	if(disable == true) {	
		if(!mAllowedCollisionPairs.contains(firstCollisionPartner) 
		  && !mAllowedCollisionPairs.contains(secondCollisionPartner)) 
		{
			QList<CollisionObject*> newList;
			newList.push_back(secondCollisionPartner);
			mAllowedCollisionPairs[firstCollisionPartner] = newList;
		} 
		else {
			bool firstIsKey = false;
			if(mAllowedCollisionPairs.contains(firstCollisionPartner)) {
				QList<CollisionObject*> partners =
					mAllowedCollisionPairs.value(firstCollisionPartner);
				if(partners.indexOf(secondCollisionPartner) != -1) {
					return;
				}
				firstIsKey = true;
			}
			if (mAllowedCollisionPairs.contains(secondCollisionPartner)) {
				QList<CollisionObject*> partners =
					mAllowedCollisionPairs.value(secondCollisionPartner);
				if(partners.indexOf(firstCollisionPartner) != -1) {
					return;
				} 
			}
			if(firstIsKey) {
				QList<CollisionObject*> partners =
					mAllowedCollisionPairs.value(firstCollisionPartner);
				partners.push_back(secondCollisionPartner);
				mAllowedCollisionPairs[firstCollisionPartner] = partners;
			} 
			else {
				QList<CollisionObject*> partners =
					mAllowedCollisionPairs.value(secondCollisionPartner);
				partners.push_back(firstCollisionPartner);
				mAllowedCollisionPairs[secondCollisionPartner] = partners;
			}
		}
	}
	else {
		if(mAllowedCollisionPairs.contains(firstCollisionPartner)) {
			QList<CollisionObject*> partners =
				mAllowedCollisionPairs.value(firstCollisionPartner);	
			int index = partners.indexOf(secondCollisionPartner);
			if(index != -1) {
				partners.removeAt(index); 
				if(partners.size() == 0) {
					mAllowedCollisionPairs.remove(firstCollisionPartner);
				} 
				else {
					mAllowedCollisionPairs[firstCollisionPartner] = partners;
				}
			}
		} 
		if (mAllowedCollisionPairs.contains(secondCollisionPartner)){
			QList<CollisionObject*> partners = 
				mAllowedCollisionPairs.value(secondCollisionPartner);
			int index = partners.indexOf(firstCollisionPartner);
			if(index != -1) {
				partners.removeAt(index); 
				if(partners.size() == 0) {
					mAllowedCollisionPairs.remove(secondCollisionPartner);
				} 
				else {
					mAllowedCollisionPairs[secondCollisionPartner] = partners;
				}
			}
		}
	}
}

void ODE_CollisionHandler::updateCollisionHandler(CollisionManager *cManager) {

	if(cManager == 0) {
		Core::log("ODE_CollisionHandler: CollisionManager does not exist.");
		return;
	}

	if(Physics::getPhysicsManager()->getPhysicalSimulationAlgorithm() == 0) {
		Core::log("ODE_CollisionHandler: Simulation algorithm could not be found.");
		return;
	}

	mAlgorithm = dynamic_cast<ODE_SimulationAlgorithm*>(
				Physics::getPhysicsManager()->getPhysicalSimulationAlgorithm());

	if(mAlgorithm == 0) {
		Core::log("ODE_CollisionHandler: Simulation algorithm is "
			"not an instance of ODE_SimulationAlgorithm.");
		return;
	}

	if(mMaxContactPointsValue == 0) {
		mMaxContactPointsValue = Core::getInstance()->getValueManager()->
			getIntValue("/Simulation/ODE/MaxContactPoints");
	}

	if(mMaxContactPointsValue == 0) {
		Core::log("ODE_CollisionHandler: Could not find required Value: "
			"\"MaxContactPoints\"");
	} 
	else {
		mMaxContactPoints = mMaxContactPointsValue->get();
	}

	mLookUpTable.clear();
	const QList<SimBody*> &bodies = Physics::getPhysicsManager()->getSimBodies();
	for(int i = 0; i < bodies.size(); i++) {
		SimBody *body = bodies.at(i);

		const QList<CollisionObject*> &collisionObjects = body->getCollisionObjects();
		for(int j = 0; j < collisionObjects.size(); j++) {
			void* nativeCollisionObject = collisionObjects.at(j)->getNativeCollisionObject();
			if(nativeCollisionObject != 0) {
				mLookUpTable.insert((dGeomID) nativeCollisionObject, collisionObjects.at(j));
			}
		}
	}

	mSoftERP = mSoftERPValue->get();
	mSoftCFM = mSoftCFMValue->get();
	mBouncingVelocity = mBouncingVelocityValue->get();
	mFrictionScalingFactor = mFrictionScalingValue->get();
	mContactSlip = mContactSlipValue->get();
	mStaticFrictionAccuracy = mStaticFrictionAccuracyValue->get();
}

/**
 * nearCallback method that is called for every geom-pair that collides during the current simulation step. Here the collisions are resolved by creating contact joints with appropriate properties are added to the simulation and all contacts are stored to be reported to the CollisionManager.
 * @param data 
 * @param o1 first ode geom that is involved in the contact.
 * @param o2 second ode geom that is involved in the contact.
 */
void ODE_CollisionHandler::mCollisionCallback(void *data, dGeomID o1, dGeomID o2) {	

	//TODO verify if this is correct (seems to be copied)

	// checks, whether one of the dGeomID-objects is actually a space instead of a geom.
	if(dGeomIsSpace(o1) || dGeomIsSpace(o2)) { 
	 	// colliding a space with something :
  		dSpaceCollide2(o1, o2, data, &ODE_SimulationAlgorithm::nearCallback); 
		// collide all geoms internal to the space(s)
		if(dGeomIsSpace(o1)) {
			dSpaceCollide((dSpaceID) o1, data, &ODE_SimulationAlgorithm::nearCallback);
		}
		if(dGeomIsSpace(o2)) {
			dSpaceCollide((dSpaceID) o2, data, &ODE_SimulationAlgorithm::nearCallback);
		}
 	} 
	else {
		// find the CollisionObjects which belong to the ode-objects.
		CollisionObject *first = mLookUpTable.value(o1);
		CollisionObject *second = mLookUpTable.value(o2);	

		if(first == 0 || second == 0) {
			Core::log("ODE_CollisionHandler: CollisionObject could not be defined.");
			return;
		}

		//check for disabled Pairs
		bool collisionAllowed = false;
		if(first->areCollisionsDisabled() || second->areCollisionsDisabled()) {
			collisionAllowed = true;
		}
		else if(mAllowedCollisionPairs.contains(first)) {
			QList<CollisionObject*> partners = mAllowedCollisionPairs.value(first);
			if(partners.indexOf(second) != -1) {
				collisionAllowed = true;
			}	
		}
		if(mAllowedCollisionPairs.contains(second)) {
			QList<CollisionObject*> partners = mAllowedCollisionPairs.value(second);
			if(partners.indexOf(first) != -1) {
				collisionAllowed = true;
			}
		}
  		// colliding two non-space geoms, so generate contact
  		// points between o1 and o2
		// there are reported max: mMaxContactPointsValue->get() contact points.
		dContact contact[mMaxContactPoints];
  		int num_contact = dCollide(o1, o2, mMaxContactPointsValue->get(), 
			&contact[0].geom, sizeof(dContact));

		// TODO: maybe introduce getElasticity, getStaticFriction Methods to SimBody
		double restitution = 0.0;
		double staticFriction = 0.0;
		double dynamicFriction = 0.0;
		int material1 = -1;
		int material2 = -1;
		SimBody *host1 = 0;
		SimBody *host2 = 0;
		ODE_Body *odeHost1 = 0;
		ODE_Body *odeHost2 = 0;

		// determine material properties of the two colliding objects.
		if(first != 0) {
			material1 = first->getMaterialType();
			host1 = first->getHostBody();
			odeHost1 = dynamic_cast<ODE_Body*>(host1);
		}	
		if(second != 0) {
			material2 = second->getMaterialType();
			host2 = second->getHostBody();
			odeHost2 = dynamic_cast<ODE_Body*>(host2);
		}

		//TODO check if parameters should be accessed directly instead of via strings (performance)

		if(material1 < 0 || material2 < 0) {
		// Determine the friction properties of the colliding SimBody objects, 
		// in case one of the bodies has a unknown material property value.
			if(first != 0 && second != 0) {
				DoubleValue *restitutionValueOne = 
					dynamic_cast<DoubleValue*>(host1->getParameter("Elasticity"));
				DoubleValue *restitutionValueTwo = 
					dynamic_cast<DoubleValue*>(host2->getParameter("Elasticity"));
				if(restitutionValueOne != 0 && restitutionValueTwo != 0) {
					restitution = restitutionValueOne->get() 
						* restitutionValueTwo->get();
				}
				DoubleValue *staticFValueOne = 
					dynamic_cast<DoubleValue*>(host1->getParameter("StaticFriction"));
				DoubleValue *staticFValueTwo = 			
					dynamic_cast<DoubleValue*>(host2->getParameter("StaticFriction"));
				if(staticFValueOne != 0 && staticFValueTwo != 0) {
					staticFriction = staticFValueOne->get() + staticFValueTwo->get();
				}
				DoubleValue *dynamicFValueOne = 
					dynamic_cast<DoubleValue*>(host1->getParameter("DynamicFriction"));
				DoubleValue *dynamicFValueTwo = 
					dynamic_cast<DoubleValue*>(host2->getParameter("DynamicFriction"));
				if(dynamicFValueOne != 0 && dynamicFValueTwo != 0) {
					dynamicFriction = dynamicFValueOne->get() + dynamicFValueTwo->get();
				}
			}
		} 
		else {
			//Determine the friction properties in case both SimBody objects have valid material types.
			restitution = Physics::getCollisionManager()->getMaterialProperties()->
				getRestitution(material1, material2);
			dynamicFriction = Physics::getCollisionManager()->getMaterialProperties()->
				getDynamicFriction(material1, material2);
			staticFriction = Physics::getCollisionManager()->getMaterialProperties()->
				getStaticFriction(material1, material2);
		}

		dynamicFriction = dynamicFriction * mFrictionScalingFactor;

		//every notified collision is saved as a contact. Enables the notification of collision rules in every case
		Contact newContact(first, second);
		QList<Vector3D> contactPoints;

		//generate contact joints for all collision points of the two objects which collide.
		for(int i = 0; i < num_contact; i++) {
			// enable required ode-specific contact-definition properties.
    		contact[i].surface.mode = dContactBounce 
										| dContactSoftCFM 
										| dContactSoftERP 
										| dContactSlip1 
										| dContactSlip2 
										| dContactApprox1 
										| dContactMu2;
		
			dContactGeom contactGeom = contact[i].geom;
			// add contact point to list of contact points.
			Vector3D contactPoint = Vector3D(contactGeom.pos[0], contactGeom.pos[1], 
				contactGeom.pos[2]);
			contactPoints.push_back(contactPoint);

			// If the two collision objects are allowed to collide, no contact joint is created.
			if(collisionAllowed) {
				break;
			}
			// set the friction coefficient depending on the velocity in the friction directions 
			// (TODO: needs to be confirmed)
			if(Math::compareDoubles(contact[i].surface.motion1, 0.0,  mStaticFrictionAccuracy) == true) 
			{
				contact[i].surface.mu = staticFriction;
			}
			else {
    			contact[i].surface.mu = dynamicFriction;
			}
			if(Math::compareDoubles(contact[i].surface.motion2, 0.0, mStaticFrictionAccuracy) == true) 
			{
				contact[i].surface.mu2 = staticFriction;
			}
			else {
    			contact[i].surface.mu2 = dynamicFriction;
			}
		
			contact[i].surface.bounce = restitution;
			contact[i].surface.slip1 = mContactSlip;
			contact[i].surface.slip2 = mContactSlip;
			contact[i].surface.bounce_vel = mBouncingVelocity;
			contact[i].surface.soft_cfm = mSoftCFM;
			contact[i].surface.soft_erp = mSoftERP;
					
			if(fabs(contact[i].geom.depth) < 0.000001 ) {
				continue; 
			}
			dJointID contactJoint = dJointCreateContact(mAlgorithm->getODEWorldID(), 
				mAlgorithm->getContactJointGroupID(), &contact[i]);
			dJointAttach(contactJoint, dGeomGetBody(contact[i].geom.g1),
				dGeomGetBody(contact[i].geom.g2));

			//add feedback structure if needed.
			dJointFeedback *feedbackStructure = 0;
			if(odeHost1 != 0 && odeHost1->isFeedbackEnabled()) {
				feedbackStructure = new dJointFeedback();
				dJointSetFeedback(contactJoint, feedbackStructure);
				odeHost1->getOwnedJointFeedbackList().append(feedbackStructure);
				odeHost1->getJointFeedbackList().insertMulti(contactGeom.g1, feedbackStructure);
				odeHost1->getJointFeedbackList().insertMulti(contactGeom.g2, feedbackStructure);
			}
			if(odeHost2 != 0 && odeHost2->isFeedbackEnabled()) {
				if(feedbackStructure == 0) {
					feedbackStructure = new dJointFeedback();
					dJointSetFeedback(contactJoint, feedbackStructure);
					odeHost2->getOwnedJointFeedbackList().append(feedbackStructure);
					
				}
				odeHost2->getJointFeedbackList().insertMulti(contactGeom.g1, feedbackStructure);
				odeHost2->getJointFeedbackList().insertMulti(contactGeom.g2, feedbackStructure);
			}
		}
		// add the contact points to the contact and add the contact to the list of occured contacts, 
		// if for both CollisionObjects reporting collisions is enabled (default).
		if(num_contact != 0) {
			if(first->areCollisionsReported() || second->areCollisionsReported()) {
				newContact.setContactPoints(contactPoints);
				mCurrentContacts.push_back(newContact);
			}
		}
 	}
}

void ODE_CollisionHandler::clearContactList() {
	mCurrentContacts.clear();
}

}
