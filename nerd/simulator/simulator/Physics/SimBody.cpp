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


 
#include "SimBody.h"
#include "SimObject.h"
#include "TriangleGeom.h"
#include "Collision/CollisionObject.h"
#include "Physics/Physics.h"
#include "Collision/MaterialProperties.h"
#include "Core/Task.h"
#include "Core/Core.h"
#include <iostream>

using namespace std;


namespace nerd {

class ChangeStringValueTask: public Task {

	public:
		ChangeStringValueTask(StringValue *value, const QString &newValue) {
			mValueToBeChanged = value;
			mNewValue = newValue;
		}

	bool runTask() {
		mValueToBeChanged->set(mNewValue);
		return true;
	}

	private:
		StringValue *mValueToBeChanged;
		QString mNewValue;
};


/**
 * Constructs a new SimBody.
 * All available parameters are set to zero as default. Except for "OrientationQuaterion" (set to (1, 0, 0, 0)) and "Elasticity" (set to 1).
 */
SimBody::SimBody(const QString &name, const QString &prefix) 
		: SimObject(name, prefix), mBodyCollisionObject(0)
{
	CollisionManager *cm = Physics::getCollisionManager();
	
	mGeometryColorValue = new ColorValue();
	mPositionValue = new Vector3DValue(0.0, 0.0, 0.0);
	mOrientationValue = new Vector3DValue(0.0, 0.0, 0.0);
	mOrientationValue->setNotifyAllSetAttempts(true);
	mCenterOfMassValue = new Vector3DValue(0.0, 0.0, 0.0);
	mQuaternionOrientationValue = new QuaternionValue();
	mDynamicValue = new BoolValue(true);
	mMassValue = new 	DoubleValue(0.0);
	mDynamicFrictionValue = new DoubleValue(0.0);
	mStaticFrictionValue = new DoubleValue(0.0);
	mElasticityValue = new DoubleValue(0.05);
	mMaterialValue = new StringValue("Default");
	cm->getMaterialProperties()->addMaterialNamesAsValuePresets(mMaterialValue);
	mTexturesValue = new StringValue("Default");
	cm->getMaterialProperties()->addMaterialNamesAsValuePresets(mTexturesValue);
	

	addParameter("Position", mPositionValue);
	addParameter("Orientation", mOrientationValue);
	addParameter("OrientationQuaternion", mQuaternionOrientationValue);
	addParameter("CenterOfMass", mCenterOfMassValue);
	addParameter("Dynamic", mDynamicValue);
	addParameter("Mass", mMassValue);
	addParameter("DynamicFriction", mDynamicFrictionValue);
	addParameter("StaticFriction", mStaticFrictionValue);
	addParameter("Elasticity", mElasticityValue);
	addParameter("Material", mMaterialValue);
	addParameter("Texture", mTexturesValue);
	addParameter("Color", mGeometryColorValue);

	mGeometryColorValue->setNotifyAllSetAttempts(true);
}


/**
 * Creates a copy of another SimBody object.
 * The parent pointer is not copied and set to NULL.
 * CollisionGeometries and Geometries are not copied.
 */

SimBody::SimBody(const SimBody &body) 
	: Object(), ValueChangedListener(), SimObject(body), mBodyCollisionObject(0)
{
	mPositionValue = dynamic_cast<Vector3DValue*>(getParameter("Position"));
	mOrientationValue = dynamic_cast<Vector3DValue*>(getParameter("Orientation"));
	mCenterOfMassValue = dynamic_cast<Vector3DValue*>(getParameter("CenterOfMass"));
	mDynamicValue = dynamic_cast<BoolValue*>(getParameter("Dynamic"));
	mMassValue = dynamic_cast<DoubleValue*>(getParameter("Mass"));
	mDynamicFrictionValue = dynamic_cast<DoubleValue*>(getParameter("DynamicFriction"));
	mStaticFrictionValue = dynamic_cast<DoubleValue*>(getParameter("StaticFriction"));
	mElasticityValue = dynamic_cast<DoubleValue*>(getParameter("Elasticity"));
	mMaterialValue = dynamic_cast<StringValue*>(getParameter("Material"));
	mTexturesValue = dynamic_cast<StringValue*>(getParameter("Texture"));
	mGeometryColorValue = dynamic_cast<ColorValue*>(getParameter("Color"));

	mQuaternionOrientationValue = dynamic_cast<QuaternionValue*>(getParameter("OrientationQuaternion"));
// 	mQuaternionOrientationValue = new QuaternionValue();
// 	mQuaternionOrientationValue->set(body.getQuaternionOrientationValue()->get());
	mGeometryColorValue->setNotifyAllSetAttempts(true); 
}


/**
 * Destructor. Removes all CollisionObjects and their geometries but does not destroy them. The owner of a CollisionObject is responsible for deleting.
 */
SimBody::~SimBody()
{
// 	if(mBodyCollisionObject != 0) {
// 		delete mBodyCollisionObject;
// 		mBodyCollisionObject = 0;
// 	}
	//remove collision objects to avoid multiple destruction of their geoms.
// 	while(!mCollisionObjects.isEmpty()) {
// 		CollisionObject *obj = mCollisionObjects.at(0);
// 		removeCollisionObject(mCollisionObjects.at(0));
// 	}
}

/**
 * This method is called by setup() and can be used in derived classes to 
 * update the internals of the SimBody during a reset phase.
 *
 * The implementation of this class does nothing.
 */
/**
 * Called during the reset phase to setup the SimBody for the next simulation.
 * This method is called during the reset sequence of the simulator.
 */
void SimBody::setup() {
	SimObject::setup();
}


/**
 * Clears the SimObjects to an initial state.
 */
void SimBody::clear() {
	SimObject::clear();
	
}


/**
 * Creates a copy of the SimBody, WITHOUT considering the parent pointer.
 * The parent is always NULL.
 * 
 * This method should be overwritten in all subclasses of SimBody, so that
 * this call always returns a SimBody*, pointing to an object of the subclass
 * on which this method wasa called.
 *
 * @return a copy of the SimBody.
 */
SimObject* SimBody::createCopy() const {
	return new SimBody(*this);
}



/**
 * Called whenever one of the parameters was changed.
 * @param value the changed value.
 */
void SimBody::valueChanged(Value *value) {
	SimObject::valueChanged(value);
	if(value == mOrientationValue && !mSynchronizingWithPhysicalModel) {	
		mQuaternionOrientationValue->setFromAngles(mOrientationValue->getX(),
		 	mOrientationValue->getY(), mOrientationValue->getZ());
	}
	else if(value == mGeometryColorValue) {
		for(int i = 0; i < mSimGeometries.size(); ++i) {
			SimGeom*geom = mSimGeometries.at(i);
			if(geom->hasAutomaticColor()) {
				geom->setColor(mGeometryColorValue->get());
			}
		}
	}

	if(value == mMaterialValue) {
		if(mBodyCollisionObject != 0) {
			mBodyCollisionObject->setMaterialType(mMaterialValue->get());
			if((mBodyCollisionObject->getMaterialType() == -1) 
				&& (mMaterialValue->get().compare("None") != 0)) 
			{
				Core::getInstance()->scheduleTask(new ChangeStringValueTask(
					mMaterialValue, "None"));
			}
		}
	}
	if(value == mTexturesValue) {
		if(mBodyCollisionObject != 0) {
			mBodyCollisionObject->setTextureType(mTexturesValue->get());
		}
	}
}


/**
 * Returns the Value object representing the current position of the SimBody.
 * @return the position Value.
 */
Vector3DValue* SimBody::getPositionValue() const {
	return mPositionValue;
}


/**
 * Returns the Value object representing the current orientation of the SimBody.
 * @return the orientation Value.
 */
Vector3DValue* SimBody::getOrientationValue() const {
	return mOrientationValue;
}


/**
 * Returns the Value object representing the current orientation of the SimBody.
 * @return the orientation Value.
 */
QuaternionValue* SimBody::getQuaternionOrientationValue() const {
	return mQuaternionOrientationValue;
}

BoolValue* SimBody::getDynamicValue() const {
	return mDynamicValue;
}


/**
 * Returns the Value object representing the current center of mass of the SimBody.
 * @return the center of mass Value.
 */
Vector3DValue* SimBody::getCenterOfMassValue() const {
	return mCenterOfMassValue;
}



/**
 * Adds a CollisionObject to this SimBody.
 * CollisionObjects are NOT destroyed when the SimBody is. This has to be done
 * by the owner of the CollisionObject manually. 
 * The SimGeom embedded to the CollisionObject is NOT added to the list of SimGeoms
 * available with getGeometries(). This i sbecause getGeometries() returns the 
 * geoms used in the visualization, which may differ from the CollisionObjects.
 *
 * @param object the CollisionObject to add.
 * @return true if successful, false if already contained or NULL.
 */
bool SimBody::addCollisionObject(CollisionObject *object) {
	if(object == 0 || mCollisionObjects.contains(object)) {
		return false;
	}
	object->setHostBody(this);
	mCollisionObjects.append(object);
	addGeometry(object->getGeometry());

	return true;
}


/**
 * Removes a CollisionObject from this SimBody.
 *
 * @param object the CollisionObject to remove.
 * @return true if successful, false if not available or NULL.
 */
bool SimBody::removeCollisionObject(CollisionObject *object) {
	if(object == 0 || !mCollisionObjects.contains(object)) {
		return false;
	}
	object->setHostBody(0);
	mCollisionObjects.removeAll(object);
	removeGeometry(object->getGeometry());
	return true;
}


/**
 * Returns a list with all registered CollisionObjects.
 *
 * @return a list with the CollisionObjects.
 */
QList<CollisionObject*> SimBody::getCollisionObjects() const {
	return mCollisionObjects;
}


DoubleValue* SimBody::getElasticityValue() const {
	return mElasticityValue;
}

DoubleValue* SimBody::getDynamicFrictionValue() const {
	return mDynamicFrictionValue;
}

DoubleValue* SimBody::getStaticFrictionValue() const {
	return mStaticFrictionValue;
}






}
