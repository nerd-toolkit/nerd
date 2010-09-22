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

#include "ODE_FixedJoint.h"
#include "Core/Core.h"

namespace nerd {

/**
 * Constructor
 * @param name Name of the Joint Objects 
 */
ODE_FixedJoint::ODE_FixedJoint(const QString &name) : FixedJoint(name) {
}

/**
 * Copy constructor
 * @param joint 
 */
ODE_FixedJoint::ODE_FixedJoint(const ODE_FixedJoint &joint) : Object(), ValueChangedListener(), 
		FixedJoint(joint), ODE_Joint() 
{
}

ODE_FixedJoint::~ODE_FixedJoint() {
}

SimObject* ODE_FixedJoint::createCopy() const {
	return new ODE_FixedJoint(*this);
}

void ODE_FixedJoint::setup() {
	ODE_Joint::setup();
	FixedJoint::setup();
	
	mJoint = (dJointID) ODE_Joint::createJoint();
	if(mJoint == 0) {
		Core::log("ODE_FixedJoint: dJoint could  not be created.");
		return;
	}
}

void ODE_FixedJoint::clear() {
	ODE_Joint::clearJoint();
	FixedJoint::clear();
	mJoint = 0;
}

/**
 * Creates the ode joint objects and includes it into the ode-engine.
 * @param body1 
 * @param body2 
 * @return 
 */
dJointID ODE_FixedJoint::createJoint(dBodyID body1, dBodyID body2) {
	dJointID newJoint = dJointCreateFixed(mWorldID, mGeneralJointGroup);
	dJointAttach(newJoint, body1, body2);
	dJointSetFixed(newJoint);
	return newJoint; 
}

}
