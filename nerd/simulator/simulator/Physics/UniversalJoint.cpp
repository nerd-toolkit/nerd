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



#include "UniversalJoint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new UniversalJoint.
 */
UniversalJoint::UniversalJoint(const QString &name)
	: SimJoint(name)
{
	mJointAxis1Point1 = new Vector3DValue();
	mJointAxis1Point2 = new Vector3DValue();
	mJointAxis2Point1 = new Vector3DValue();
	mJointAxis2Point2 = new Vector3DValue();
	mAnchorPoint = new Vector3DValue();
	
	addParameter("JointAxis1Point1", mJointAxis1Point1);
	addParameter("JointAxis1Point2", mJointAxis1Point2);
	addParameter("JointAxis2Point1", mJointAxis2Point1);
	addParameter("JointAxis2Point2", mJointAxis2Point2);
	addParameter("JointAnchor", mAnchorPoint);

	mAxisPoints.push_back(mJointAxis1Point1);
	mAxisPoints.push_back(mJointAxis1Point2);
	mAxisPoints.push_back(mJointAxis2Point1);
	mAxisPoints.push_back(mJointAxis2Point2);
	mAxisPoints.push_back(mAnchorPoint);
}


/**
 * Copy constructor. 
 * 
 * @param other the UniversalJoint object to copy.
 */
UniversalJoint::UniversalJoint(const UniversalJoint &other) 
	: SimJoint(other)
{
	mJointAxis1Point1 = dynamic_cast<Vector3DValue*>(getParameter("JointAxis1Point1"));
	mJointAxis1Point2 = dynamic_cast<Vector3DValue*>(getParameter("JointAxis1Point2"));
	mJointAxis2Point1 = dynamic_cast<Vector3DValue*>(getParameter("JointAxis2Point1"));
	mJointAxis2Point2 = dynamic_cast<Vector3DValue*>(getParameter("JointAxis2Point2"));
	mAnchorPoint = dynamic_cast<Vector3DValue*>(getParameter("JointAnchor"));
}

/**
 * Destructor.
 */
UniversalJoint::~UniversalJoint() {
}


SimObject* UniversalJoint::createCopy() const {
	return new UniversalJoint(*this);
}


void UniversalJoint::setup() {
	SimJoint::setup();
}


void UniversalJoint::clear() {
	SimJoint::clear();
}


}



