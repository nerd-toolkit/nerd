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


#include "BoxBody.h"
#include "WavefrontGeom.h"
#include <iostream>

using namespace std;

namespace nerd {

/**
 * Constructs a new BoxBody object.
 *
 * @param name the name of this BoxBody.
 */
// BoxBody::BoxBody(const QString &name) : SimBody(name) {
// 	mWidth = new DoubleValue();
// 	mHeight = new DoubleValue();
// 	mDepth = new DoubleValue();
//
// 	addParameter("Width", mWidth);
// 	addParameter("Height", mHeight);
// 	addParameter("Depth", mDepth);
//
// 	//Collision and visualization geometries (will be initialized in update().
// 	mBodyCollisionObject = new CollisionObject(BoxGeom(this, 0.0, 0.0, 0.0));
// 	addGeometry(mBodyCollisionObject->getGeometry());
// 	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
// 	mBodyCollisionObject->setTextureType("Default");
// 	addCollisionObject(mBodyCollisionObject);
// }

BoxBody::BoxBody(const QString &name, double width, double height, double depth)
		: SimBody(name)
{
	mWidth = new DoubleValue(width);
	mHeight = new DoubleValue(height);
	mDepth = new DoubleValue(depth);

	addParameter("Width", mWidth);
	addParameter("Height", mHeight);
	addParameter("Depth", mDepth);

	//Collision and visualization geometries (will be initialized in update().
	mBodyCollisionObject = new CollisionObject(
		BoxGeom(this, mWidth->get(), mHeight->get(), mDepth->get()));
	//mBodyCollisionObject = new CollisionObject(WavefrontGeom(this, "alice.obj", 0.1));

	addGeometry(mBodyCollisionObject->getGeometry());
	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
	mBodyCollisionObject->setTextureType("Default");
	mBodyCollisionObject->getGeometry()->setColor(mGeometryColorValue->get());
	addCollisionObject(mBodyCollisionObject);
}


/**
 * Creates a copy of object body.
 * The default CollisionObject and Geometry of a BoxBody are copied as well.
 * @param body the BoxBody to copy.
 */
BoxBody::BoxBody(const BoxBody &body)
	: Object(), ValueChangedListener(), SimBody(body)
{
	mWidth = dynamic_cast<DoubleValue*>(getParameter("Width"));
	mHeight = dynamic_cast<DoubleValue*>(getParameter("Height"));
	mDepth = dynamic_cast<DoubleValue*>(getParameter("Depth"));

	mBodyCollisionObject = new CollisionObject(
		BoxGeom(this, mWidth->get(), mHeight->get(), mDepth->get()));
	//mBodyCollisionObject = new CollisionObject(WavefrontGeom(this, "alice.obj", 0.1));

	mBodyCollisionObject->getGeometry()->setColor(mGeometryColorValue->get());
	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
	mBodyCollisionObject->setTextureType(mTexturesValue->get());

	addGeometry(mBodyCollisionObject->getGeometry());
	addCollisionObject(mBodyCollisionObject);
}


/**
 * Destructor.
 */
BoxBody::~BoxBody() {
	if(mBodyCollisionObject != 0) {
// 		removeGeometry(mBodyCollisionObject->getGeometry());
		delete mBodyCollisionObject;
	}
}


/**
 * Creates a copy of this object, using the copy constructor.
 *
 * @return a copy of this object.
 */
SimBody* BoxBody::createCopy() const {
	return new BoxBody(*this);
}


/**
 * Updates the box geometry and the CollisionObject with the current
 * parameter values for width, height, depth.
 *
 * If there was no CollisionObject set so far, then a new CollisionObject
 * is created to represent the Box collision and visualization.
 */

void BoxBody::setup() {
	SimBody::setup();
    if(mBodyCollisionObject != 0) {
        mBodyCollisionObject->setOwner(this);
        mBodyCollisionObject->setHostBody(this);

        BoxGeom *geom = dynamic_cast<BoxGeom*>(mBodyCollisionObject->getGeometry());
        if(geom != 0) {
            geom->setSize(mWidth->get(), mHeight->get(), mDepth->get());
        }
    }
}


void BoxBody::clear() {
	SimBody::clear();

}

/**
 * Called when a parameter Value changed.
 * Changing the dimensions of the box (width, depth, height) will be applied to the
 * physics and the visualization only after calling setup() on the object.
 * @param value the value that changed.
 */
void BoxBody::valueChanged(Value *value) {
	SimBody::valueChanged(value);
}




}


