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

#include "SphereBody.h"
#include "SphereGeom.h"
#include "Collision/CollisionObject.h"

namespace nerd {

/**
 * Constructor. The radius is initialized as 0.0.
 * @param name 
 */
// SphereBody::SphereBody(const QString &name):SimBody(name) {
// 	mRadius = new DoubleValue(0);
// 	addParameter("Radius", mRadius);
// 
// 	mBodyCollisionObject = new CollisionObject(SphereGeom(this, 0.0));
// 	addGeometry(mBodyCollisionObject->getGeometry());
// 	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
// 	mBodyCollisionObject->setTextureType("Default");
// 	addCollisionObject(mBodyCollisionObject);
// }

SphereBody::SphereBody(const QString &name, double radius) : SimBody(name) {
	mRadius = new DoubleValue(radius);
	addParameter("Radius", mRadius);

	mBodyCollisionObject = new CollisionObject(SphereGeom(this, mRadius->get()));
	addGeometry(mBodyCollisionObject->getGeometry());
	addCollisionObject(mBodyCollisionObject);
}

SphereBody::SphereBody(const SphereBody &body) 
		: Object(), ValueChangedListener(), SimBody(body) 
{
	mRadius = dynamic_cast<DoubleValue*>(getParameter("Radius"));

	mBodyCollisionObject = new CollisionObject(SphereGeom(this, mRadius->get()));
	addGeometry(mBodyCollisionObject->getGeometry());
	addCollisionObject(mBodyCollisionObject);
}

SphereBody::~SphereBody() {
	//todo destroy in body.
	if(mBodyCollisionObject != 0 ) {
		delete mBodyCollisionObject;
	}
}

SimBody* SphereBody::createCopy() const {
	return new SphereBody(*this);
}

void SphereBody::valueChanged(Value *value) {
	SimBody::valueChanged(value);
}

void SphereBody::clear() {
	SimBody::clear();
}

void SphereBody::setup() {
	SimBody::setup();
	if(mBodyCollisionObject != 0) {
		mBodyCollisionObject->setOwner(this);
		mBodyCollisionObject->setHostBody(this);
		SphereGeom *geom = dynamic_cast<SphereGeom*>(mBodyCollisionObject->getGeometry());
		if(geom != 0) {
			geom->setRadius(mRadius->get());
		}
	}
}

}
