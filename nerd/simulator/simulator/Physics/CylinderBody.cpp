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

#include "CylinderBody.h"
#include "CylinderGeom.h"
#include "WavefrontGeom.h"
#include "Collision/CollisionObject.h"

namespace nerd{

/**
 * Constructor of a CylinderBody.
 * @param name The name of the cylinder body.
 */
// CylinderBody::CylinderBody(const QString &name) : SimBody(name) {
// 	mRadius = new DoubleValue(0.0);
// 	mLength = new DoubleValue(0.0);
// 	addParameter("Radius", mRadius);
// 	addParameter("Length", mLength);
// 
// 	mBodyCollisionObject = new CollisionObject(CylinderGeom(this, 0.0, 0.0));
// 	addGeometry(mBodyCollisionObject->getGeometry());
// 	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
// 	mBodyCollisionObject->setTextureType("Default");
// 	addCollisionObject(mBodyCollisionObject);
// }

/**
 * Constructor of a CylinderBody.
 * @param name Name of the cylinder body.
 * @param radius The radius of the cylinder geometry.
 * @param length The length of the cylinder geometry.
 */
CylinderBody::CylinderBody(const QString &name, double radius, double length) 
		: SimBody(name) 
{
	mRadius = new DoubleValue(radius);
	mLength = new DoubleValue(length);
	addParameter("Length", mLength);
	addParameter("Radius", mRadius);

	mBodyCollisionObject = new CollisionObject(
			CylinderGeom(this, mRadius->get(), mLength->get()));
	//addGeometry(mBodyCollisionObject->getGeometry());
	addCollisionObject(mBodyCollisionObject);
	//addGeometry(new WavefrontGeom(this, "alice.obj"));
}

CylinderBody::CylinderBody(const CylinderBody &body) 
		: Object(), ValueChangedListener(), SimBody(body) 
{
	mRadius = dynamic_cast<DoubleValue*>(getParameter("Radius"));
	mLength = dynamic_cast<DoubleValue*>(getParameter("Length"));

	mBodyCollisionObject = new CollisionObject(CylinderGeom(
		this, mRadius->get(), mLength->get()));
//	addGeometry(mBodyCollisionObject->getGeometry());
	addCollisionObject(mBodyCollisionObject);
	//addGeometry(new WavefrontGeom(this, "alice.obj"));
}

CylinderBody::~CylinderBody() {
	if(mBodyCollisionObject != 0 ) {
		delete mBodyCollisionObject;
	}
}

SimBody* CylinderBody::createCopy() const {
	return new CylinderBody(*this);
}

void CylinderBody::valueChanged(Value *value) {
	SimBody::valueChanged(value);
}

void CylinderBody::clear() {
	SimBody::clear();
}

void CylinderBody::setup() {
	SimBody::setup();
	if(mBodyCollisionObject != 0) {
		mBodyCollisionObject->setOwner(this);
		mBodyCollisionObject->setHostBody(this);
		CylinderGeom *geom = dynamic_cast<CylinderGeom*>(
			mBodyCollisionObject->getGeometry());
		if(geom != 0) {
			geom->setRadius(mRadius->get());
			geom->setLength(mLength->get());
		}
	}
}

}
