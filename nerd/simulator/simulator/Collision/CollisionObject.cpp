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


#include "CollisionObject.h"
#include "CollisionManager.h"
#include "MaterialProperties.h"
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "Physics/SphereGeom.h"
#include "Physics/TriangleGeom.h"
#include "Physics/BoxGeom.h"
#include "Physics/PlaneBody.h"
#include <iostream>

using namespace std;

namespace nerd {

CollisionObject::CollisionObject(const SimGeom &geometry, SimObject *owner, bool reportCollision) 
		: mOwner(owner), mHostBody(0), mGeometry(0), mReportCollisions(reportCollision),
		mIsPenetrable(false), mNativeCollisionObject(0)
{
	mGeometry = geometry.createCopy();
	setMaterialType("Default");
	setTextureType("Default");
	mTextureInformation = "Default";
	mMass = 0.0;
	mMaterialType = 0;
}

CollisionObject::CollisionObject(const CollisionObject &other) 
	: mOwner(0), mHostBody(0), mGeometry(0), mReportCollisions(other.mReportCollisions),
	  mIsPenetrable(other.mIsPenetrable), mNativeCollisionObject(0)
{
	mGeometry = other.mGeometry->createCopy();
	mMaterialType = other.mMaterialType;
	mTextureInformation = other.mTextureInformation;
	mMass = other.mMass;
}


CollisionObject::~CollisionObject() {
	delete mGeometry;
}


/**
 * Sets the owner of this CollisionObject. The owner can be 
 * any SimObject and is not neccessarily a SimBody. 
 *
 * @param owner the owner of this CollisionObject.
 */
void CollisionObject::setOwner(SimObject *owner) {
	mOwner = owner;
}


/**
 * Returns the owner of this CollisionObject.
 *
 * @return the owner of this CollisionObject.
 */
SimObject* CollisionObject::getOwner() const {
	return mOwner;
}


/**
 * Sets the SimBody that hosts this CollisionObject.
 * Note that this is not neccessarily the owner of the CollisionObject.
 *
 * @param body the host body of this CollisionObject.
 */
void CollisionObject::setHostBody(SimBody *body) {
	mHostBody = body;
}

/**
 * Returns the SimBody that holds the CollisionObject. 
 * This is not neccessarily the owner of the CollisionObject.
 *
 * @return the host body of this CollisionObject.
 */
SimBody* CollisionObject::getHostBody() const {
	return mHostBody;
}


		
void CollisionObject::setGeometry(const SimGeom &geometry) {
	delete mGeometry;
	mGeometry = geometry.createCopy();
}


/**
 * Returns a pointer to the internally used SimGeom.
 * This SimGeom is destroyed with this CollisionObject.
 */
SimGeom* CollisionObject::getGeometry() const {
	return mGeometry;
}


		
void CollisionObject::setCollisionReport(bool enableReport) {
	mReportCollisions = enableReport;
}


/**
 * Sets whether collisions, in which this collision object is involved are reported. Only if areCollisionsReported() returns false for both of the colliding CollisionObject's their collision is not reported back. If a contact is not reported, it will not show up in the list of occurred contacts, that is held by the CollisionManager. Thereby CollisionRules won't handle this contact.
 * @return 
 */
bool CollisionObject::areCollisionsReported() const {
	return mReportCollisions;
}


		
void CollisionObject::setNativeCollisionObject(void *object) {
	mNativeCollisionObject = object;
}


void* CollisionObject::getNativeCollisionObject() const {
	return mNativeCollisionObject;
}


void CollisionObject::setMaterialType(const QString &materialName) {
	QString name = materialName;
	mMaterialType = Physics::getCollisionManager()->getMaterialProperties()
		->getMaterialType(materialName);

	QList<int> textureInformation = getTextureTypeInformation();

	if(mTextureInformation.compare("Default") == 0) {
		for(int i = 0; i < textureInformation.size(); i++) {
			setFaceTexture(i, materialName);
		}
	}
}

int CollisionObject::getMaterialType() const {
	return mMaterialType;
}

void CollisionObject::setTextureType(const QString &textureName) {

	mTextureInformation = textureName;
	mFaceTextureTypes.clear();
	int textureType = 0;
	if(textureName.compare("Default") == 0) {
		textureType = mMaterialType;
	}
	else {
		textureType = Physics::getCollisionManager()->getMaterialProperties()
				->getTextureIndex(textureName);
	}
	if(dynamic_cast<SphereGeom*>(mGeometry) != 0 || dynamic_cast<PlaneBody*>(mOwner) != 0) {
		mFaceTextureTypes.push_back(textureType);
	}
	else if(dynamic_cast<BoxGeom*>(mGeometry) != 0) {
		mFaceTextureTypes.push_back(textureType);
		mFaceTextureTypes.push_back(textureType);
		mFaceTextureTypes.push_back(textureType);
		mFaceTextureTypes.push_back(textureType);
		mFaceTextureTypes.push_back(textureType);
		mFaceTextureTypes.push_back(textureType);
	}
	else {
		TriangleGeom *geom = dynamic_cast<TriangleGeom*>(mGeometry);
		if(geom != 0) {	
			for(int i = 0; i < geom->getTriangles().size(); i++) {
				mFaceTextureTypes.push_back(textureType);
			}
		}
	}
}


void CollisionObject::setFaceTexture(int index, const QString &textureName) {
	if(mFaceTextureTypes.size() > index) {
		int textureType =  Physics::getCollisionManager()->getMaterialProperties()
			->getTextureIndex(textureName);
		mFaceTextureTypes.replace(index, textureType);
	}
}

QList<int> CollisionObject::getTextureTypeInformation() const {
	return mFaceTextureTypes;
}


void CollisionObject::setMass(double mass) {
	mMass = mass;
}

double CollisionObject::getMass() const {
	return mMass;
}

void CollisionObject::disableCollisions(bool penetrable) {
	mIsPenetrable = penetrable;
}

bool CollisionObject::areCollisionsDisabled() const {
	return mIsPenetrable;
}

}




