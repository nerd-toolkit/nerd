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


#include "ODE_SliderJoint.h"
#include "ODE_Body.h"
#include "Physics/SimBody.h"
#include "Core/Core.h"

namespace nerd {

/**
 * Constructs a new ODE_SliderJoint.
 */
ODE_SliderJoint::ODE_SliderJoint(const QString &name) 
	:  ODE_Joint(), SliderJoint(name)
{

}

/**
 * Creates a copy of the given ODE_SliderJoint.
 *
 * Note: The internal IBDS::SliderJoint object is NOT copied. Instead all parameters
 * of the ODE_SliderJoint are copied, so that a call to createJoint() will create 
 * an equivalent hinge joint.
 */
ODE_SliderJoint::ODE_SliderJoint(const ODE_SliderJoint &joint) 
	: Object(), ValueChangedListener(), ODE_Joint() , SliderJoint(joint)
{

}

/** 
 * Destructor.
 */
ODE_SliderJoint::~ODE_SliderJoint() {
}


/**
 * Creates a copy of thie ODE_SliderJoint by usin its copy constructor.
 *
 * @return a copy of the joint.
 */
SimJoint* ODE_SliderJoint::createCopy() const {
	return new ODE_SliderJoint(*this);
}


/**
 * Creates an IBDS::SliderJoint with the rotation axis specified by the two points 
 * given by parameters AxisPoint1 and AxisPoint2. The SliderJoint connects the two bodies.
 *
 * @param body1 the first body to connect to.
 * @param body2 the second body to connect o.
 * @return a new IBDS::SliderJoint. 
 */
void ODE_SliderJoint::setup() {
	ODE_Joint::setup();	
	SliderJoint::setup();
	mJoint = (dJointID)ODE_Joint::createJoint();

	if(mJoint == 0) {
		Core::log("ODE_SliderJoint: dJoint could not be created.");
	}
}


/**
 * Clears the SliderJoint. 
 * This implementation sets the internal SliderJoint pointer to NULL.
 */
void ODE_SliderJoint::clear() {
	ODE_Joint::clearJoint();
	SliderJoint::clear();
	mJoint = 0;
}

void ODE_SliderJoint::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm*) {
}


/**
 * Creates a new IBDS::SliderJoint.
 *
 * @param body1 the first body to connect the joint to.
 * @param body2 the second body to connect the joint to.
 * @return a new IBDS::SliderJoint.
 */
dJointID ODE_SliderJoint::createJoint(dBodyID body1, dBodyID body2) {
	if(mFirstAxisPoint->get().equals(mSecondAxisPoint->get())) {
		Core::log("Invalid axes for ODE_SliderJoint.");
		return 0;
	}
	//if one of the bodyIDs is null, the joint is connected to a static object.
	dJointID newJoint = dJointCreateSlider(mWorldID, mGeneralJointGroup);
	dJointAttach(newJoint, body1, body2);
	
	Vector3D axis(mSecondAxisPoint->getX() - mFirstAxisPoint->getX(), 
		mSecondAxisPoint->getY() - mFirstAxisPoint->getY(), 
		mSecondAxisPoint->getZ() - mFirstAxisPoint->getZ());
	axis.normalize();
	dJointSetSliderAxis(newJoint, axis.getX(), axis.getY(), axis.getZ());
	dJointSetSliderParam(newJoint, dParamLoStop, mMinPositionValue->get());
	dJointSetSliderParam(newJoint, dParamHiStop, mMaxPositionValue->get());
	dJointSetSliderParam(newJoint, dParamVel, 0.0);
	dJointSetSliderParam(newJoint, dParamFMax, mFrictionValue->get());

	return newJoint; 
}

}




