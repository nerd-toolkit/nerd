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



#include "CapsuleBody.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new CapsuleBody.
 */
CapsuleBody::CapsuleBody(const QString &name, double length, double radius)
	: SimBody(name), mRadius(0), mLength(0)
{
	mRadius = new DoubleValue(radius);
	mLength = new DoubleValue(length);

	addParameter("Radius", mRadius);
	addParameter("Length", mLength);

	//Collision and visualization geometries (will be initialized in update().
	mBodyCollisionObject = new CollisionObject(
		CapsuleGeom(this, mLength->get(), mRadius->get()));
	addGeometry(mBodyCollisionObject->getGeometry());
	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
	mBodyCollisionObject->setTextureType("None");
	mBodyCollisionObject->getGeometry()->setColor(mGeometryColorValue->get());
	addCollisionObject(mBodyCollisionObject);
}


/**
 * Copy constructor. 
 * 
 * @param other the CapsuleBody object to copy.
 */
CapsuleBody::CapsuleBody(const CapsuleBody &other) 
	: Object(), ValueChangedListener(), SimBody(other) 
{
	mRadius = dynamic_cast<DoubleValue*>(getParameter("Radius"));
	mLength = dynamic_cast<DoubleValue*>(getParameter("Length"));

	mBodyCollisionObject = new CollisionObject(
		CapsuleGeom(this, mLength->get(), mRadius->get()));

	mBodyCollisionObject->getGeometry()->setColor(mGeometryColorValue->get());
	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
	mBodyCollisionObject->setTextureType(mTexturesValue->get());

	addGeometry(mBodyCollisionObject->getGeometry());
	addCollisionObject(mBodyCollisionObject);
}

/**
 * Destructor.
 */
CapsuleBody::~CapsuleBody() {
	if(mBodyCollisionObject != 0) {
		delete mBodyCollisionObject;
	}
}


SimBody* CapsuleBody::createCopy() const {
	return new CapsuleBody(*this);
}



/**
 * Update length and radius of geom.
 */
void CapsuleBody::setup() {
	SimBody::setup();
    if(mBodyCollisionObject != 0) {
        mBodyCollisionObject->setOwner(this);
        mBodyCollisionObject->setHostBody(this);
        CapsuleGeom *geom = dynamic_cast<CapsuleGeom*>(mBodyCollisionObject->getGeometry());
        if(geom != 0) {
            geom->setRadius(mRadius->get());
			geom->setLength(mLength->get());
        }
    }
}


void CapsuleBody::clear() {
	SimBody::clear();
}


		
void CapsuleBody::valueChanged(Value *value) {
	SimBody::valueChanged(value);
}



}



