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



#include "ODE_BallAndSocketJoint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ODE_BallAndSocketJoint.
 */
ODE_BallAndSocketJoint::ODE_BallAndSocketJoint(const QString &name)
	: BallAndSocketJoint(name), ODE_Joint()
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ODE_BallAndSocketJoint object to copy.
 */
ODE_BallAndSocketJoint::ODE_BallAndSocketJoint(const ODE_BallAndSocketJoint &other)
	: Object(), ValueChangedListener(), BallAndSocketJoint(other), ODE_Joint()
{
}

/**
 * Destructor.
 */
ODE_BallAndSocketJoint::~ODE_BallAndSocketJoint() {
}



SimJoint* ODE_BallAndSocketJoint::createCopy() const {
	return new ODE_BallAndSocketJoint(*this);
}


/**
 * Creates an ODE_BallAndSocketJoint with the rotation axis specified by the four points 
 * given by parameters Axis1Point1, Axis1Point2, Axis2Point1 and Axis2Point2. 
 * The UniversalJoint connects the two bodies.
 *
 * @param body1 the first body to connect to.
 * @param body2 the second body to connect o.
 * @return a new ODE::UniversalJoint. 
 */
void ODE_BallAndSocketJoint::setup() {
	ODE_Joint::setup();	
	BallAndSocketJoint::setup();
	mJoint = (dJointID) ODE_Joint::createJoint();

	if(mJoint == 0) {
		Core::log("ODE_BallAndSocketJoint: dJoint could not be created.");
	}
}


/**
 * Clears the UniversalJoint. 
 * This implementation sets the internal UniversalJoint pointer to NULL.
 */
void ODE_BallAndSocketJoint::clear() {
	ODE_Joint::clearJoint();
	BallAndSocketJoint::clear();
	mJoint = 0;
}
		


void ODE_BallAndSocketJoint::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa) {

}


/**
 * Creates a new ODE_BallAndSocketJoint.
 *
 * @param body1 the first body to connect the joint to.
 * @param body2 the second body to connect the joint to.
 * @return the new ODE_BallAndSocketJoint.
 */
dJointID ODE_BallAndSocketJoint::createJoint(dBodyID body1, dBodyID body2) {


	//if one of the bodyIDs is null, the joint is connected to a static object.
	dJointID newJoint = dJointCreateBall(mWorldID, mGeneralJointGroup);
	dJointAttach(newJoint, body1, body2);
	
	Vector3D anchor = mAnchorPoint->get() ;
	
	dJointSetBallAnchor(newJoint, anchor.getX(), anchor.getY(), anchor.getZ());

	return newJoint; 
}


}



