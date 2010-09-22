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



#include "LightSource.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/SphereGeom.h"
#include "Physics/CylinderGeom.h"
#include "Physics/Physics.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new LightSource.
 */
LightSource::LightSource(const QString &name, double brightness, double range, int type)
	: SimBody(name), mRange(0), mBrightnessSensor(0), mBrightnessControl(0), 
	  mLightColor(0), mType(0), mHideLightCone(0), mUseSphereAsLightCone(0), 
	  mReferenceObjectName(0), mLocalPosition(0), mReferenceObject(0)
{
	mRange = new DoubleValue(range);
	mBrightnessSensor = new InterfaceValue("", "Brightness", brightness, 0.0, 1.0);
	mBrightnessControl = new InterfaceValue("", "DesiredBrightness", brightness, 0.0, 1.0);
	mLightColor = new ColorValue("yellow");
	mHideLightCone = new BoolValue(false);
	mUseSphereAsLightCone = new BoolValue(false);
	mType = new IntValue(type);
	mReferenceObjectName = new StringValue("");
	mLocalPosition = new Vector3DValue(0.0, 0.0, 0.0);

	addParameter("Range", mRange);
	addParameter("Brightness", mBrightnessSensor);
	addParameter("DesiredBrightness", mBrightnessControl);
	addParameter("Color", mLightColor);
	addParameter("HideLightCone", mHideLightCone);
	addParameter("UseSphericLightCone", mUseSphereAsLightCone);
	addParameter("LightType", mType);
	addParameter("ReferenceObject", mReferenceObjectName);
	addParameter("LocalPosition", mLocalPosition);

	mOutputValues.append(mBrightnessSensor);
	mInputValues.append(mBrightnessControl);


	createCollisionObject();

	updateSensorValues();
}


/**
 * Copy constructor. 
 * 
 * @param other the LightSource object to copy.
 */
LightSource::LightSource(const LightSource &other) 
	: Object(), ValueChangedListener(), SimBody(other), mType(other.mType), 
	  mReferenceObjectName(0), mLocalPosition(0), mReferenceObject(0)
{
	mRange = dynamic_cast<DoubleValue*>(getParameter("Range"));
	mBrightnessSensor = dynamic_cast<InterfaceValue*>(getParameter("Brightness"));
	mBrightnessControl = dynamic_cast<InterfaceValue*>(getParameter("DesiredBrightness"));
	mLightColor = dynamic_cast<ColorValue*>(getParameter("Color"));
	mHideLightCone = dynamic_cast<BoolValue*>(getParameter("HideLightCone"));
	mUseSphereAsLightCone = dynamic_cast<BoolValue*>(getParameter("UseSphericLightCone"));
	mType = dynamic_cast<IntValue*>(getParameter("LightType"));
	mReferenceObjectName = dynamic_cast<StringValue*>(getParameter("ReferenceObject"));
	mLocalPosition = dynamic_cast<Vector3DValue*>(getParameter("LocalPosition"));

	mOutputValues.clear();
	mInputValues.clear();

	mOutputValues.append(mBrightnessSensor);
	mInputValues.append(mBrightnessControl);

	createCollisionObject();

	updateSensorValues();
}

/**
 * Destructor.
 */
LightSource::~LightSource() {
}


SimBody* LightSource::createCopy() const {
	return new LightSource(*this);
}


		
void LightSource::setup()  {
	SimBody::setup();

	if(mReferenceObjectName->get() != "") {
		mReferenceObject = Physics::getPhysicsManager()->getSimBody(mReferenceObjectName->get());
		if(mReferenceObject != 0) {
			mReferenceObject->getPositionValue()->addValueChangedListener(this);
			mReferenceObject->getQuaternionOrientationValue()->addValueChangedListener(this);
			valueChanged(mReferenceObject->getPositionValue());
		}
	}

	if(mBodyCollisionObject != 0) {
		mBodyCollisionObject->setOwner(this);
		mBodyCollisionObject->setHostBody(this);

		if(mUseSphereAsLightCone->get()) {
			SphereGeom *geom = dynamic_cast<SphereGeom*>(mBodyCollisionObject->getGeometry());
			if(geom != 0) {
				geom->setRadius(mRange->get());
			}
		}
		else {
			CylinderGeom *geom = dynamic_cast<CylinderGeom*>(mBodyCollisionObject->getGeometry());
			if(geom != 0) {
				geom->setRadius(mRange->get());
			}
		}
		updateSensorValues();
	}
}


void LightSource::clear() {
	if(mReferenceObject != 0) {
		mReferenceObject->getPositionValue()->removeValueChangedListener(this);
		mReferenceObject->getQuaternionOrientationValue()->removeValueChangedListener(this);
	}
	mReferenceObject = 0;
	SimBody::clear();
}


		
void LightSource::valueChanged(Value *value) {

	SimBody::valueChanged(value);
	if(value == 0) {
		return;
	}
	if(value == mRange) {
		if(mUseSphereAsLightCone->get()) {
			SphereGeom *geom = dynamic_cast<SphereGeom*>(mBodyCollisionObject->getGeometry());
			if(geom != 0) {
				geom->setRadius(mRange->get());
			}
		}
		else {
			CylinderGeom *geom = dynamic_cast<CylinderGeom*>(mBodyCollisionObject->getGeometry());
			if(geom != 0) {
				geom->setRadius(mRange->get());
			}
		}
	}
	else if(value == mHideLightCone) {
		updateSensorValues();
	}
	else if(value == mLightColor) {
		updateSensorValues();
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
}


void LightSource::updateSensorValues() {
	mBrightnessSensor->set(mBrightnessControl->get());

	//Set transparency to a value between 0 and 80, depending on the current brightness.
	//80 is about 30 percent of full opacity (255) at max.
	Color color = mBodyCollisionObject->getGeometry()->getColor();
	if(mHideLightCone->get()) {
		color.setAlpha(0); 
	}
	else {
		color.setAlpha((int) (mBrightnessSensor->get() * 80.0)); 
	}
	mBodyCollisionObject->getGeometry()->setColor(color);
}


int LightSource::getType() const {
	return mType->get();
}


void LightSource::setType(int type) {
	mType->set(type);
}

double LightSource::getRange() const {
	return mRange->get();
}


void LightSource::setRange(double range) {
	mRange->set(range);
}


double LightSource::getCurrentBrightness() const {
	return mBrightnessSensor->get();
}


double LightSource::getDesiredBrightness() const {
	return mBrightnessControl->get();
}


void LightSource::setDesiredBrightness(double brightness) {
	mBrightnessControl->set(brightness);
}


void LightSource::createCollisionObject() {

	if(mBodyCollisionObject != 0) {
		delete mBodyCollisionObject;
	}
	mBodyCollisionObject = 0;
	if(mUseSphereAsLightCone->get()) {
		mBodyCollisionObject = new CollisionObject(SphereGeom(this, mRange->get()));
	}
	else {
		mBodyCollisionObject = new CollisionObject(CylinderGeom(this, mRange->get(), 0.05));
	}
	addGeometry(mBodyCollisionObject->getGeometry());
	mBodyCollisionObject->setMaterialType("Light");
	mBodyCollisionObject->setTextureType("None");
	mBodyCollisionObject->getGeometry()->setColor(mLightColor->get());
	addCollisionObject(mBodyCollisionObject);
}




}



