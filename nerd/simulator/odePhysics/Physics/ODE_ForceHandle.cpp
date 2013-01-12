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



#include "ODE_ForceHandle.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "ode/ode.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ODE_ForceHandle.
 */
ODE_ForceHandle::ODE_ForceHandle(const QString &name)
	: ForceHandle(name), mHostBody(0)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ODE_ForceHandle object to copy.
 */
ODE_ForceHandle::ODE_ForceHandle(const ODE_ForceHandle &other) 
	: Object(), ValueChangedListener(), ForceHandle(other), mHostBody(0)
{
}

/**
 * Destructor.
 */
ODE_ForceHandle::~ODE_ForceHandle() {
}


SimObject* ODE_ForceHandle::createCopy() const {
	return new ODE_ForceHandle(*this);
}

		
void ODE_ForceHandle::valueChanged(Value *value) {
	ForceHandle::valueChanged(value);
	if(value == 0) {
		return;
	}
}

void ODE_ForceHandle::setup() {
	ForceHandle::setup();
	PhysicsManager *pm = Physics::getPhysicsManager();
	mHostBody = dynamic_cast<ODE_Body*>(pm->getSimBody(mHostObjectName->get()));

	if(mHostBody != 0) {
		mFoundHostObject->set(true);
	}
}

void ODE_ForceHandle::clear() {
	ForceHandle::clear();
	mHostBody = 0;
}


void ODE_ForceHandle::updateActuators() {
	if(mHostBody != 0 && mActivateForces->get()) {
		dBodyID hostBodyId = mHostBody->getRigidBodyID();
		if(hostBodyId != 0) {
			if(mApplyRelativeForces->get()) {
				dBodyAddForce(hostBodyId, mAppliedForce->getX(), mAppliedForce->getY(), mAppliedForce->getZ());
				dBodyAddTorque(hostBodyId, mAppliedTorque->getX(), mAppliedTorque->getY(),mAppliedTorque->getZ());
			}
			else {
				dBodyAddRelForce(hostBodyId, mAppliedForce->getX(), mAppliedForce->getY(), mAppliedForce->getZ());
				dBodyAddRelTorque(hostBodyId, mAppliedTorque->getX(), mAppliedTorque->getY(),mAppliedTorque->getZ());
			}
		}
	}
}


}




