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


#include "Physics.h"
#include "Core/Core.h"
#include "Math/Quaternion.h"

namespace nerd {

bool Physics::install() {
	bool ok = true;
	Core *core = Core::getInstance();

	PhysicsManager *pm = dynamic_cast<PhysicsManager*>(Core::getInstance()
			->getGlobalObject("PhysicsManager"));
	if(pm == 0) {
		pm = new PhysicsManager();
		core->addGlobalObject("PhysicsManager", pm);
	}
	else if(dynamic_cast<PhysicsManager*>(pm) == 0) {
		Core::log("Physics::install() : There was a PhysicsManager which is not a"
				" subclass of PhysicsManager!");
		ok = false;
	}

	CollisionManager *cm = dynamic_cast<CollisionManager*>(Core::getInstance()
			->getGlobalObject("CollisionManager"));
	if(cm == 0) {
		cm = new CollisionManager();
		core->addGlobalObject("CollisionManager", cm);
	}
	else if(dynamic_cast<CollisionManager*>(cm) == 0) {
		Core::log("Physics::install() : There was a CollisionManager which is not a"
				" subclass of CollisionManager!");
		ok = false;
	}

	SimulationEnvironmentManager *sm = dynamic_cast<SimulationEnvironmentManager*>(
		Core::getInstance()->getGlobalObject("SimulationEnvironmentManager"));
	if(sm == 0) {
		sm = new SimulationEnvironmentManager();
		core->addGlobalObject("SimulationEnvironmentManager", sm);
	}
	else if(dynamic_cast<SimulationEnvironmentManager*>(sm) == 0) {
		Core::log("Physics::install() : There was a SimulationEnvironmentManager "
			"which is not a subclass of CollisionManager!");
		ok = false;
	}
	return ok;
}

PhysicsManager* Physics::getPhysicsManager() {
	PhysicsManager *pm = dynamic_cast<PhysicsManager*>(Core::getInstance()
		->getGlobalObject("PhysicsManager"));
	if(pm == 0) {
		Physics::install();
		pm = dynamic_cast<PhysicsManager*>(Core::getInstance()
			->getGlobalObject("PhysicsManager"));
	}
	return pm;
}


CollisionManager* Physics::getCollisionManager() {
	CollisionManager *cm = dynamic_cast<CollisionManager*>(Core::getInstance()
		->getGlobalObject("CollisionManager"));
	if(cm == 0) {
		Physics::install();
		cm = dynamic_cast<CollisionManager*>(Core::getInstance()
		->getGlobalObject("CollisionManager"));
	}
	return cm;
}


SimulationEnvironmentManager* Physics::getSimulationEnvironmentManager() {
	SimulationEnvironmentManager *em = dynamic_cast<SimulationEnvironmentManager*>(
			Core::getInstance()->getGlobalObject("SimulationEnvironmentManager"));
	if(em == 0) {
		Physics::install();
		em = dynamic_cast<SimulationEnvironmentManager*>(
			Core::getInstance()->getGlobalObject("SimulationEnvironmentManager"));
	}
	return em;
}

/**
 * Translate a list of SimObject's relativ to the origin. The translation does not consider the current orientation of the objects towards the origin (To enable this behavior, an information conserning the amount of rotation offset towards the default position would be required).
 * @param simObjects The SimObjects's to be translated.
 * @param offset The offset from the current position.
 */
void Physics::translateSimObjects(QList<SimObject*> simObjects, Vector3D offset) {

	for(int i = 0; i < simObjects.size(); i++) {
		if(dynamic_cast<SimBody*>(simObjects.at(i)) != 0) {	
			SimBody *body = dynamic_cast<SimBody*>(simObjects.at(i));
			body->getPositionValue()->set(body->getPositionValue()->get() + offset);
			continue;
		}
		else if(dynamic_cast<SimJoint*>(simObjects.at(i)) != 0) {
			SimJoint *joint = dynamic_cast<SimJoint*>(simObjects.at(i));
			for(int j = 0; j < joint->getAxisPoints().size(); j++) {
				joint->getAxisPoints().at(j)->set(joint->getAxisPoints().at(j)->get() + offset);
			}
			continue;
		}
		else {
			continue;
		}
	}
}

/**
 * Rotates the a SimObjects in the list around the origin with the given orientation-vector.
 * @param simObjects The list of objects to be rotated.
 * @param orientationOffset The angles (in degree) about which each object is to be rotated.
 */
void Physics::rotateSimObjects(QList<SimObject*> simObjects, Vector3D orientationOffset) {
	Quaternion orientation;
	orientation.setFromAngles(orientationOffset.getX(), orientationOffset.getY(),
			 				  orientationOffset.getZ());
	Quaternion inverse = orientation.getInverse();

	for(int i = 0; i < simObjects.size(); i++) {
		if(dynamic_cast<SimBody*>(simObjects.at(i)) != 0) {	
			Vector3DValue *bodyPosition = dynamic_cast<SimBody*>(simObjects.at(i))->getPositionValue();
			
			Quaternion oldPosition(0, 0, 0, 0);
			oldPosition.set(0, bodyPosition->getX(), bodyPosition->getY(), bodyPosition->getZ());
			Quaternion newPointQ = orientation * oldPosition *inverse;
			bodyPosition->set(newPointQ.getX(), newPointQ.getY(), newPointQ.getZ());
			
			Vector3DValue *bodyOrientation = 
							dynamic_cast<SimBody*>(simObjects.at(i))->getOrientationValue();
			bodyOrientation->set(bodyOrientation->get() + orientationOffset);
			continue;
		}
		else if(dynamic_cast<SimJoint*>(simObjects.at(i)) != 0) {
			SimJoint *joint = dynamic_cast<SimJoint*>(simObjects.at(i));
			for(int j = 0; j < joint->getAxisPoints().size(); j++) {
	
				Vector3DValue *axisPointPosition = joint->getAxisPoints().at(j);
				
				Quaternion oldPosition(0, 0, 0, 0);
				oldPosition.set(0, axisPointPosition->getX(), axisPointPosition->getY(), axisPointPosition->getZ());
				Quaternion newPointQ = orientation * oldPosition *inverse;
				axisPointPosition->set(newPointQ.getX(), newPointQ.getY(), newPointQ.getZ());

			}
			continue;
		}
		else {
			continue;
		}
	}
}


}


