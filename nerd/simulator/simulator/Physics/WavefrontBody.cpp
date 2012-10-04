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


#include "WavefrontBody.h"
#include "WavefrontGeom.h"
#include "Physics/Physics.h"
#include "Core/Core.h"


#include <iostream>
using namespace std;

namespace nerd {

/**
 * Constructs a new WavefrontBody object.
 *
 * @param name the name of this WavefrontBody.
 */
WavefrontBody::WavefrontBody(const QString &name, const QString& filename, double scale)
	: SimBody(name), mReferenceObjectName(0), mLocalPosition(0), mReferenceObject(0)
{
	mScale = new DoubleValue(scale);
	mFilename = new FileNameValue(filename);
	//mFilename->useAsFileName(true);
	mReferenceObjectName = new StringValue("");
	mLocalPosition = new Vector3DValue(0.0, 0.0, 0.0);
	mLocalOrientation = new Vector3DValue(0.0, 0.0, 0.0);

	addParameter("Scale", mScale);
	addParameter("FileName", mFilename);
	addParameter("ReferenceObject", mReferenceObjectName);
	addParameter("LocalPosition", mLocalPosition);
	addParameter("LocalOrientation", mLocalOrientation);

	//Collision and visualization geometries (will be initialized in update().
	mBodyCollisionObject = new CollisionObject(WavefrontGeom(this, filename, scale));

	addGeometry(mBodyCollisionObject->getGeometry());
	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
	mBodyCollisionObject->setTextureType("Default");
	mBodyCollisionObject->getGeometry()->setColor(mGeometryColorValue->get());
	addCollisionObject(mBodyCollisionObject);
}


/**
 * Creates a copy of object body.
 * The default CollisionObject and Geometry of a WavefrontBody are copied as well.
 * @param body the WavefrontBody to copy.
 */
WavefrontBody::WavefrontBody(const WavefrontBody &body) 
	: Object(), ValueChangedListener(), SimBody(body), mReferenceObjectName(0), 
	  mLocalPosition(0), mReferenceObject(0)
{
	mScale = dynamic_cast<DoubleValue*>(getParameter("Scale"));
	mFilename = dynamic_cast<FileNameValue*>(getParameter("FileName"));
	mReferenceObjectName = dynamic_cast<StringValue*>(getParameter("ReferenceObject"));
	mLocalPosition = dynamic_cast<Vector3DValue*>(getParameter("LocalPosition"));
	mLocalOrientation = dynamic_cast<Vector3DValue*>(getParameter("LocalOrientation"));

	mBodyCollisionObject = new CollisionObject(WavefrontGeom(this, mFilename->get(), mScale->get()));

	addGeometry(mBodyCollisionObject->getGeometry());
	mBodyCollisionObject->setMaterialType(mMaterialValue->get());
	mBodyCollisionObject->setTextureType("Default");
	mBodyCollisionObject->getGeometry()->setColor(mGeometryColorValue->get());
	
	addCollisionObject(mBodyCollisionObject);
}


/**
 * Destructor.
 */
WavefrontBody::~WavefrontBody() {
	if(mBodyCollisionObject != 0)
		delete mBodyCollisionObject;
}


/**
 * Creates a copy of this object, using the copy constructor.
 *
 * @return a copy of this object.
 */
SimBody* WavefrontBody::createCopy() const {
	return new WavefrontBody(*this);
}


/**
 * Updates the box geometry and the CollisionObject with the current
 * parameter values for width, height, depth.
 *
 * If there was no CollisionObject set so far, then a new CollisionObject
 * is created to represent the Box collision and visualization.
 */

void WavefrontBody::setup() {
	SimBody::setup();

	if(mReferenceObjectName->get() != "") {
		if(mReferenceObject != 0) {
			mReferenceObject->getPositionValue()->removeValueChangedListener(this);
			mReferenceObject->getQuaternionOrientationValue()->removeValueChangedListener(this);
			mReferenceObject->getOrientationValue()->removeValueChangedListener(this);
			mReferenceObject = 0;
		}
		
		mReferenceObject = Physics::getPhysicsManager()->getSimBody(mReferenceObjectName->get());
		if(mReferenceObject != 0) {
			mReferenceObject->getPositionValue()->addValueChangedListener(this);
			mReferenceObject->getQuaternionOrientationValue()->addValueChangedListener(this);
			mReferenceObject->getOrientationValue()->addValueChangedListener(this);
			valueChanged(mReferenceObject->getPositionValue());
		}
		else if(mReferenceObjectName->get().trimmed() != "") {
			//error
			Core::log("WavefrontBody: Could not find reference body with name [" 
					  + mReferenceObjectName->get() + "]! Ignoring.", true);
		}
	}


	if(mBodyCollisionObject != 0) {
		mBodyCollisionObject->setOwner(this);
		mBodyCollisionObject->setHostBody(this);
		WavefrontGeom *geom = dynamic_cast<WavefrontGeom*>(mBodyCollisionObject->getGeometry());
		if(geom != 0) {
			geom->setFilename(mFilename->get());
			geom->setScale(mScale->get());
		}
		mBodyCollisionObject->setMaterialType(mMaterialValue->get());	
		mBodyCollisionObject->setTextureType("Default");
	}
}


void WavefrontBody::clear() {
	if(mReferenceObject != 0) {
		mReferenceObject->getPositionValue()->removeValueChangedListener(this);
		mReferenceObject->getQuaternionOrientationValue()->removeValueChangedListener(this);	
		mReferenceObject->getOrientationValue()->removeValueChangedListener(this);
	}
	mReferenceObject = 0;

	SimBody::clear();
}

/**
 * Called when a parameter Value changed.
 * @param value the value that changed.
 */
void WavefrontBody::valueChanged(Value *value) {
	SimBody::valueChanged(value);

	if(value == 0) {
		return;
	} 
	else if(mReferenceObject != 0 
		&& (value == mReferenceObject->getPositionValue() 
			|| value == mReferenceObject->getQuaternionOrientationValue()))
	{
		Quaternion localPos(0.0, 
						mLocalPosition->getX(), 
						mLocalPosition->getY(), 
						mLocalPosition->getZ());
		Quaternion bodyOrientationInverse = 
			mReferenceObject->getQuaternionOrientationValue()->get().getInverse();
		Quaternion rotatedLocalPosQuat = mReferenceObject->getQuaternionOrientationValue()->get() 
			* localPos * bodyOrientationInverse;
		Vector3D rotatedLocalPos(rotatedLocalPosQuat.getX(), 
								rotatedLocalPosQuat.getY(), 
								rotatedLocalPosQuat.getZ());
		mPositionValue->set(mReferenceObject->getPositionValue()->get() + rotatedLocalPos);
	}
	else if(mReferenceObject != 0 && value == mReferenceObject->getOrientationValue()) {
		Vector3D orientation = mReferenceObject->getOrientationValue()->get() + mLocalOrientation->get();
		while(orientation.getX() > 180.0) {
			orientation.setX(orientation.getX() - 360.0);
		}
		while(orientation.getX() < -180.0) {
			orientation.setX(orientation.getX() + 360.0);
		}
		while(orientation.getY() > 180.0) {
			orientation.setY(orientation.getY() - 360.0);
		}
		while(orientation.getY() < -180.0) {
			orientation.setY(orientation.getY() + 360.0);
		}
		while(orientation.getZ() > 180.0) {
			orientation.setZ(orientation.getZ() - 360.0);
		}
		while(orientation.getZ() < -180.0) {
			orientation.setZ(orientation.getZ() + 360.0);
		}
		getOrientationValue()->set(orientation);
	}
}



}
