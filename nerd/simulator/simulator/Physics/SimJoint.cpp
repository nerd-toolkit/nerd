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
#include "SimJoint.h"
#include <QString>
#include "SimBody.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"

namespace nerd {


/**
 * Constructs a new SimJoint.
 * 
 * @param name the name of the SimJoint object.
 */
SimJoint::SimJoint(const QString &name) 
	: SimObject(name), mFirstBody(0), mSecondBody(0), mBodyNamesChanged(false)
{
	mFirstBodyName = new StringValue();
	mSecondBodyName = new StringValue();

	addParameter("FirstBody", mFirstBodyName);
	addParameter("SecondBody", mSecondBodyName);
}


/**
 * Copy constructor.
 * Does NOT copy the first and second SimBody object. Both are set to NULL.
 * 
 * @param joint the joint to copy.
 */
SimJoint::SimJoint(const SimJoint &joint)
	: Object(), ValueChangedListener(), 
		SimObject(joint), mFirstBody(0), mSecondBody(0), mBodyNamesChanged(false)
{
	mFirstBodyName = dynamic_cast<StringValue*>(getParameter("FirstBody"));
	mSecondBodyName = dynamic_cast<StringValue*>(getParameter("SecondBody"));

// 	mFirstBodyName->set("");
// 	mSecondBodyName->set("");
}


/**
 * Destructor.
 */
SimJoint::~SimJoint() {
}


/**
 * Creates a copy of this SimJoint using its copy constructor.
 *
 * @return a newly created copy of this SimJoint.
 */
SimObject* SimJoint::createCopy() const {
	return new SimJoint(*this);
}

/**
 * Returns the SimBody object which is the first of two SimBodies connected 
 * by this joint.
 *
 * @return the first connected SimBody.
 */
SimBody* SimJoint::getFirstBody() const {
	return mFirstBody;
}


/**
 * Returns the SimBody object which is the second of the two SimBodies connected
 * by this joint.
 *
 * @return the second connected SimBody.
 */
SimBody* SimJoint::getSecondBody() const {
	return mSecondBody;
}


StringValue* SimJoint::getFirstBodyName() const {
	return mFirstBodyName;
}


StringValue* SimJoint::getSecondBodyName() const {
	return mSecondBodyName;
}


/**
 * Called when one of the parameter values changed.
 */
void SimJoint::valueChanged(Value *value) {
	SimObject::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mFirstBodyName || value == mSecondBodyName) {
		mBodyNamesChanged = true;
	}
}


/**
 * Called during the reset phase to clear the SimJoint.
 * This implementation does nothing.
 * Subclasses can overwrite this method to clear the physics dependent objects
 * and data structures.
 */
void SimJoint::clear() {
	SimObject::clear();
}


/**
 * Called during the reset phase to setup the SimJoint for the next simulation.
 * This implementation of setup() does nothing. The method can be overwritten
 * in subclasses to setup the object state during reset.
 */
void SimJoint::setup() {
	SimObject::setup();
	determineBodies();
}


void SimJoint::determineBodies() {
	PhysicsManager *pManger = Physics::getPhysicsManager();
	if(mFirstBodyName->get() != "") {
		mFirstBody = pManger->getSimBody(mFirstBodyName->get());
		if(mFirstBody == 0) {
			Core::log(QString("SimJoint [%1]: First reference body has "
				"invalid name : %2.").arg(getName()).arg(mFirstBodyName->get()), true);
			return;
		}
	} else {
		mFirstBody = 0;
	}
	if(mSecondBodyName->get() != "") {
		mSecondBody = pManger->getSimBody(mSecondBodyName->get());
		if(mSecondBody == 0) {
			Core::log(QString("SimJoint [%1]: Second reference body has "
				"invalid name : %2.").arg(getName()).arg(mSecondBodyName->get()), true);
			return;
		}
	} else {
		mSecondBody = 0;
	}
	mBodyNamesChanged = false;
}

/**
 * Updates the StringValue parameters containing the names of the connected SimBodies. 
 * This methods sets both paramters to the names of the corresponding SimBodies.
 */
// void SimJoint::updateBodyNames() {
// 	mUpdatingNames = true;
// 	if(mFirstBody == 0) {
// 		mFirstBodyName->set("");
// 	}
// 	else {
// 		mFirstBodyName->set(mFirstBody->getName());
// 	}
// 	if(mSecondBody == 0) {
// 		mSecondBodyName->set("");
// 	}
// 	else {
// 		mSecondBodyName->set(mSecondBody->getName());
// 	}
// 	mUpdatingNames = false;
// }

QList<Vector3DValue*> SimJoint::getAxisPoints() const {
	return mAxisPoints;
}

}
