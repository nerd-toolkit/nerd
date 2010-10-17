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



#include "Simple2D_FixedJoint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/SimBody.h"
#include "Physics/Simple2D_Body.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new Simple2D_FixedJoint.
 */
Simple2D_FixedJoint::Simple2D_FixedJoint(const QString &name) 
		: FixedJoint(name) 
{
}


/**
 * Copy constructor. 
 * 
 * @param other the Simple2D_FixedJoint object to copy.
 */
Simple2D_FixedJoint::Simple2D_FixedJoint(const Simple2D_FixedJoint &other)
	: Object(), ValueChangedListener(), FixedJoint(other) 
{
}

/**
 * Destructor.
 */
Simple2D_FixedJoint::~Simple2D_FixedJoint() {
}

SimObject* Simple2D_FixedJoint::createCopy() const {
	return new Simple2D_FixedJoint(*this);
}

		
void Simple2D_FixedJoint::setup() {
	FixedJoint::setup();

	Simple2D_Body *parentBody = dynamic_cast<Simple2D_Body*>(getFirstBody());
	Simple2D_Body *child = dynamic_cast<Simple2D_Body*>(getSecondBody());

	if(child != 0 && parentBody != 0) {
		if(child->getParent() != 0 && child->getParent() != parentBody) {
			Core::log("Simple2D_FixedJoint::setup(): Warning, child ["
				+ getSecondBody()->getName() + "] has multiple parents!");
			return;
		}
		parentBody->addChildBody(child);
		child->setParent(parentBody);

		//calculate local orientation and position
		Vector3DValue *globalChildPosition = child->getPositionValue();
		Vector3DValue *globalChildOrientation = child->getOrientationValue();
		Vector3DValue *localChildPosition = child->getLocalPosition();
		Vector3DValue *localChildOrientation = child->getLocalOrientation();
		Vector3DValue *globalParentPosition = parentBody->getPositionValue();
		Vector3DValue *globalParentOrientation = parentBody->getOrientationValue();

		localChildPosition->set(globalChildPosition->getX() - globalParentPosition->getX(),
								globalChildPosition->getY() - globalParentPosition->getY(),
								globalChildPosition->getZ() - globalParentPosition->getZ());
		localChildOrientation->set(globalChildOrientation->getX() - globalParentOrientation->getX(),
								globalChildOrientation->getY() - globalParentOrientation->getY(),
								globalChildOrientation->getZ() - globalParentOrientation->getZ());
	}
	
}


void Simple2D_FixedJoint::clear() {
	FixedJoint::clear();
}



}



