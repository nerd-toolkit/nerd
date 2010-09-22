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


#include "HingeJoint.h"

namespace nerd {


/**
 * Constructor.
 * Creates a new HingeJoint object with the given name. 
 * 
 * @param the name of this HingeJoint.
 */
HingeJoint::HingeJoint(const QString &name) : SimJoint(name) {
	mJointAxisPoint1 = new Vector3DValue();
	mJointAxisPoint2 = new Vector3DValue();
	
	addParameter("AxisPoint1", mJointAxisPoint1);
	addParameter("AxisPoint2", mJointAxisPoint2);

	mAxisPoints.push_back(mJointAxisPoint1);
	mAxisPoints.push_back(mJointAxisPoint2);
	
}


/**
 * Copy constructor. Creates a new HingeJoint based on the given Hingejoint.
 *
 * @param joint the HingeJoint to copy.
 */
HingeJoint::HingeJoint(const HingeJoint &joint) : Object(), 
		ValueChangedListener(), SimJoint(joint)
{
	mJointAxisPoint1 = dynamic_cast<Vector3DValue*>(getParameter("AxisPoint1"));
	mJointAxisPoint2 = dynamic_cast<Vector3DValue*>(getParameter("AxisPoint2"));

	mAxisPoints.push_back(mJointAxisPoint1);
	mAxisPoints.push_back(mJointAxisPoint2); 
}


/**
 * Destructor.
 */
HingeJoint::~HingeJoint() {
}


/**
 * Creates a copy of this HingeJoint using the copy constructor.
 *
 * @return a new created HingeJoint.
 */
SimObject* HingeJoint::createCopy() const {
	return new HingeJoint(*this);
}

/**
 * Called to setup the HingeJoint for the next simulation.
 * This implementation does nothing.
 */
void HingeJoint::setup() {
	SimJoint::setup();
}

/**
 * Called during reset to clear the HingeJoint.
 * This implementation does nothing.
 */
void HingeJoint::clear() {
	SimJoint::clear();
}

}

