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

#include "PlaneBody.h"
#include "BoxGeom.h"
#include "Physics/Physics.h"
#include <iostream>

using namespace std;

namespace nerd {

/**
 * Constructs a new PlaneBody object.
 * @param name the name of this PlaneBody.
 */
PlaneBody::PlaneBody(const QString &name) : SimBody(name)
{
	mAxisValue = new Vector3DValue(0,1,0);
	mDistanceValue = new DoubleValue(0.0);

	addParameter("Axis", mAxisValue);
	addParameter("Distance", mDistanceValue);

	mBodyCollisionObject = new CollisionObject(SimGeom(this), this, true); 
	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
	mBodyCollisionObject->setTextureType("Default");

	addCollisionObject(mBodyCollisionObject);
	
	mDynamicValue->set(false);

}


/**
 * Creates a copy of object body.
 * The default CollisionObject and Geometry of a PlaneBody are copied as well.
 * @param body the PlaneBody to copy.
 */
PlaneBody::PlaneBody(const PlaneBody &body) 
		: Object(), ValueChangedListener(), SimBody(body)
{
	mAxisValue = dynamic_cast<Vector3DValue*>(getParameter("Axis"));
	mDistanceValue = dynamic_cast<DoubleValue*>(getParameter("Distance"));

	mBodyCollisionObject = new CollisionObject(0, this, true);
	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
	mBodyCollisionObject->setTextureType(mTexturesValue->get());

	addCollisionObject(mBodyCollisionObject);
	mDynamicValue->set(false);

}


/**
 * Destructor.
 */
PlaneBody::~PlaneBody() {
	if(mBodyCollisionObject != 0) {
		delete mBodyCollisionObject;
	}
}

/**
 * Creates a copy of this object, using the copy constructor.
 *
 * @return a copy of this object.
 */
SimBody* PlaneBody::createCopy() const {
	return new PlaneBody(*this);
}


void PlaneBody::setup() {
	SimBody::setup();
	mDynamicValue->set(false);
	Vector3D normalized;
	normalized = mAxisValue->get();
	normalized.normalize();
	mAxisValue->set(normalized);
}

void PlaneBody::clear() {
	SimBody::clear();
	
}

void PlaneBody::valueChanged(Value *value) {
	SimBody::valueChanged(value);

}


}


