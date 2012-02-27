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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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



#include "ODE_UniversalJoint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ODE_UniversalJoint.
 */
ODE_UniversalJoint::ODE_UniversalJoint(const QString &name)
	: UniversalJoint(name), ODE_Joint()
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ODE_UniversalJoint object to copy.
 */
ODE_UniversalJoint::ODE_UniversalJoint(const ODE_UniversalJoint &other)
	: Object(), ValueChangedListener(), UniversalJoint(other), ODE_Joint()
{
}

/**
 * Destructor.
 */
ODE_UniversalJoint::~ODE_UniversalJoint() {
}



SimJoint* ODE_UniversalJoint::createCopy() const {
	return new ODE_UniversalJoint(*this);
}


/**
 * Creates an ODE_UniversalJoint with the rotation axis specified by the four points 
 * given by parameters Axis1Point1, Axis1Point2, Axis2Point1 and Axis2Point2. 
 * The UniversalJoint connects the two bodies.
 *
 * @param body1 the first body to connect to.
 * @param body2 the second body to connect o.
 * @return a new ODE::UniversalJoint. 
 */
void ODE_UniversalJoint::setup() {
	ODE_Joint::setup();	
	UniversalJoint::setup();
	mJoint = (dJointID) ODE_Joint::createJoint();

	if(mJoint == 0) {
		Core::log("ODE_UniversalJoint: dJoint could not be created.");
	}
}


/**
 * Clears the UniversalJoint. 
 * This implementation sets the internal UniversalJoint pointer to NULL.
 */
void ODE_UniversalJoint::clear() {
	ODE_Joint::clearJoint();
	UniversalJoint::clear();
	mJoint = 0;
}
		


void ODE_UniversalJoint::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa) {

}


/**
 * Creates a new ODE_UniversalJoint.
 *
 * @param body1 the first body to connect the joint to.
 * @param body2 the second body to connect the joint to.
 * @return the new ODE_UniversalJoint.
 */
dJointID ODE_UniversalJoint::createJoint(dBodyID body1, dBodyID body2) {

	if(mJointAxis1Point1->get().equals(mJointAxis1Point2->get())
		|| mJointAxis2Point1->get().equals(mJointAxis2Point2->get())) {
		Core::log("Invalid axes for ODE_UniversalJoint.");
		return 0;
	}

	//if one of the bodyIDs is null, the joint is connected to a static object.
	dJointID newJoint = dJointCreateUniversal(mWorldID, mGeneralJointGroup);
	dJointAttach(newJoint, body1, body2);
	
	Vector3D anchor = mAnchorPoint->get() ;
	Vector3D axis1;
	Vector3D axis2;
	
	axis1.set(mJointAxis1Point2->getX() - mJointAxis1Point1->getX(), 
			  mJointAxis1Point2->getY() - mJointAxis1Point1->getY(), 
			  mJointAxis1Point2->getZ() - mJointAxis1Point1->getZ());

	axis2.set(mJointAxis2Point2->getX() - mJointAxis2Point1->getX(), 
			  mJointAxis2Point2->getY() - mJointAxis2Point1->getY(), 
			  mJointAxis2Point2->getZ() - mJointAxis2Point1->getZ());
	
	dJointSetUniversalAnchor(newJoint, anchor.getX(), anchor.getY(), anchor.getZ());
	dJointSetUniversalAxis1(newJoint, axis1.getX(), axis1.getY(), axis1.getZ());
	dJointSetUniversalAxis2(newJoint, axis2.getX(), axis2.getY(), axis2.getZ());

	return newJoint; 
}


}



