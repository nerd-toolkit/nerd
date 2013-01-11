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

#include "ODE_HingeJoint.h"
#include "ODE_Body.h"
#include "Physics/SimBody.h"
#include "Core/Core.h"

namespace nerd {

/**
 * Constructs a new ODE_HingeJoint.
 */
ODE_HingeJoint::ODE_HingeJoint(const QString &name) : HingeJoint(name), 
		ODE_Joint()
{
}

/**
 * Creates a copy of the given ODE_HingeJoint.
 *
 * Note: The internal ODE_HingeJoint object is NOT copied. Instead all parameters
 * of the ODE_HingeJoint are copied, so that a call to createJoint() will create 
 * an equivalent hinge joint.
 */
ODE_HingeJoint::ODE_HingeJoint(const ODE_HingeJoint &joint) : Object(), 
		ValueChangedListener(), HingeJoint(joint), ODE_Joint()
{
}

/** 
 * Destructor.
 */
ODE_HingeJoint::~ODE_HingeJoint() {
}


/**
 * Creates a copy of the ODE_HingeJoint by using its copy constructor.
 *
 * @return a copy of the joint.
 */
SimJoint* ODE_HingeJoint::createCopy() const {
	return new ODE_HingeJoint(*this);
}


/**
 * Creates an ODE_HingeJoint with the rotation axis specified by the two points 
 * given by parameters AxisPoint1 and AxisPoint2. The HingeJoint connects the two bodies.
 *
 * @param body1 the first body to connect to.
 * @param body2 the second body to connect o.
 * @return a new IBDS::HingeJoint. 
 */
void ODE_HingeJoint::setup() {
	ODE_Joint::setup();	
	HingeJoint::setup();
	mJoint = (dJointID)ODE_Joint::createJoint();

	if(mJoint == 0) {
		Core::log("ODE_HingeJoint: dJoint could not be created.");
	}
}


/**
 * Clears the HingeJoint. 
 * This implementation sets the internal HingeJoint pointer to NULL.
 */
void ODE_HingeJoint::clear() {
	ODE_Joint::clearJoint();
	HingeJoint::clear();
	mJoint = 0;
}

void ODE_HingeJoint::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm*) {
}


/**
 * Creates a new ODE_HingeJoint.
 *
 * @param body1 the first body to connect the joint to.
 * @param body2 the second body to connect the joint to.
 * @return the new ODE_HingeJoint.
 */
dJointID ODE_HingeJoint::createJoint(dBodyID body1, dBodyID body2) {
	if(mJointAxisPoint1->get().equals(mJointAxisPoint2->get())) {
		Core::log("Invalid axes for ODE_HingeJoint.");
		return 0;
	}
	//if one of the bodyIDs is null, the joint is connected to a static object.
	dJointID newJoint = dJointCreateHinge(mWorldID, mGeneralJointGroup);
	dJointAttach(newJoint, body1, body2);
	
	Vector3D anchor = mJointAxisPoint1->get() ;
	Vector3D axis = mJointAxisPoint2->get() ;
	
	axis.set(mJointAxisPoint2->getX() - mJointAxisPoint1->getX(), mJointAxisPoint2->getY() - mJointAxisPoint1->getY(), mJointAxisPoint2->getZ() - mJointAxisPoint1->getZ());
	
	dJointSetHingeAnchor(newJoint, anchor.getX(), anchor.getY(), anchor.getZ());
	dJointSetHingeAxis(newJoint, axis.getX(), axis.getY(), axis.getZ());
	return newJoint; 
}

}




