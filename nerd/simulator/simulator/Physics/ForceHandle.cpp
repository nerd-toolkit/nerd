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


#include "ForceHandle.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ForceHandle.
 */
ForceHandle::ForceHandle(const QString &name)
	: SimObject(name)
{
	mActivateForces = new BoolValue(true);
	mAppliedForce = new Vector3DValue();
	mAppliedTorque = new Vector3DValue();
	mApplyRelativeForces = new BoolValue(true);
	mHostObjectName = new StringValue();
	mFoundHostObject = new BoolValue(false);

	addParameter("ActivateForces", mActivateForces);
	addParameter("AppliedForce", mAppliedForce);
	addParameter("AppliedTorque", mAppliedTorque);
	addParameter("ApplyRelativeForces", mApplyRelativeForces);
	addParameter("HostBodyName", mHostObjectName);
	addParameter("FoundHostBody", mFoundHostObject);
}


/**
 * Copy constructor. 
 * 
 * @param other the ForceHandle object to copy.
 */
ForceHandle::ForceHandle(const ForceHandle &other) 
	: Object(), ValueChangedListener(), SimObject(other)
{
	mActivateForces = dynamic_cast<BoolValue*>(getParameter("ActivateForces"));
	mAppliedForce = dynamic_cast<Vector3DValue*>(getParameter("AppliedForce"));
	mAppliedTorque = dynamic_cast<Vector3DValue*>(getParameter("AppliedTorque"));
	mApplyRelativeForces = dynamic_cast<BoolValue*>(getParameter("ApplyRelativeForces"));
	mHostObjectName = dynamic_cast<StringValue*>(getParameter("HostBodyName"));
	mFoundHostObject = dynamic_cast<BoolValue*>(getParameter("FoundHostBody"));
}

/**
 * Destructor.
 */
ForceHandle::~ForceHandle() {
}

SimObject* ForceHandle::createCopy() const {
	return new ForceHandle(*this);
}

		
void ForceHandle::setup() {
	mFoundHostObject->set(false);
	return SimObject::setup();
}


void ForceHandle::clear() {
	mFoundHostObject->set(false);
	return SimObject::clear();
}

void  ForceHandle::updateActuators() {
}


}



