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

#include "SphericLightSource.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/SphereGeom.h"
#include "Physics/CylinderGeom.h"
#include "Physics/Physics.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {
	
	
	/**
	 * Constructs a new SphericLightSource.
	 */
	SphericLightSource::SphericLightSource(
			const QString &name, double brightness, double radius, int type)
	: LightSource(name, type), mRadius(0), mBrightness(0), mLightColor(0),
		mHideLightCone(0), mSphericLightCone(0), mReferenceObjectName(0),
		mLocalPosition(0), mReferenceObject(0),
		mDistributionType(0), mSwitchYZAxes(0)
	{
		mRadius = new DoubleValue(radius);
		mDesiredBrightness = new DoubleValue(brightness);
		mBrightness = new InterfaceValue("", "Brightness", brightness, 0.0, 1.0);
		mBrightness->setNotifyAllSetAttempts(true);
		mRange = new RangeValue(0,1);
		mLightColor = new ColorValue("yellow");
		mHideLightCone = new BoolValue(false);
		mSphericLightCone = new BoolValue(false);
		mReferenceObjectName = new StringValue("");
		mLocalPosition = new Vector3DValue(0.0, 0.0, 0.0);
		mDistributionType = new IntValue(0);
		//mUniformLight = new BoolValue(false);
		
		mDistributionType->setDescription("The type of light distribution:\n"
										   "0: homogeneous distribution\n"
										   "1: linear decay from center");
		//TODO add other documentation strings
		
		addParameter("Radius", mRadius);
		addParameter("DesiredBrightness", mDesiredBrightness);
		//addParameter("Brightness", mBrightness);
		addParameter("Range", mRange);
		addParameter("LightColor", mLightColor);
		addParameter("HideLightCone", mHideLightCone);
		addParameter("SphericLightCone", mSphericLightCone);
		addParameter("ReferenceObject", mReferenceObjectName);
		addParameter("LocalPosition", mLocalPosition);
		addParameter("DistributionType", mDistributionType);
		//addParameter("UniformLight", mUniformLight);
		
		mOutputValues.append(mBrightness);
		mInputValues.append(mBrightness);
		
		Physics::getPhysicsManager();
		
		if(mSwitchYZAxes == 0) {
			mSwitchYZAxes = Core::getInstance()->getValueManager()->
				getBoolValue(SimulationConstants::VALUE_SWITCH_YZ_AXES);
		}
		
		mGeometryColorValue->set(0, 0, 0, 0);
		
		createCollisionObject();
		
		updateBrightnessValue();
	}
	
	
	/**
	 * Copy constructor. 
	 * 
	 * @param other the SphericLightSource object to copy.
	 */
	SphericLightSource::SphericLightSource(const SphericLightSource &other) 
	: Object(), ValueChangedListener(), LightSource(other), 
		mReferenceObjectName(0), mLocalPosition(0), mReferenceObject(0),
		mDistributionType(other.mDistributionType), mSwitchYZAxes(other.mSwitchYZAxes)
	{
		mRadius = dynamic_cast<DoubleValue*>(getParameter("Radius"));
		mDesiredBrightness = dynamic_cast<DoubleValue*>(getParameter("DesiredBrightness"));
		//mBrightness = dynamic_cast<InterfaceValue*>(getParameter("Brightness"));
		// Reconstruct brightness interface value (!)
		mBrightness = new InterfaceValue("", "Brightness", mDesiredBrightness->get(), 0, 1);
		mLightColor = dynamic_cast<ColorValue*>(getParameter("LightColor"));
		mHideLightCone = dynamic_cast<BoolValue*>(getParameter("HideLightCone"));
		mSphericLightCone = dynamic_cast<BoolValue*>(getParameter("SphericLightCone"));
		mReferenceObjectName = dynamic_cast<StringValue*>(getParameter("ReferenceObject"));
		mLocalPosition = dynamic_cast<Vector3DValue*>(getParameter("LocalPosition"));
		//mUniformLight = dynamic_cast<BoolValue*>(getParameter("UniformLight"));
		mDistributionType = dynamic_cast<IntValue*>(getParameter("DistributionType"));
		mRange = dynamic_cast<RangeValue*>(getParameter("Range"));
		
		mOutputValues.clear();
		mInputValues.clear();
		
		mOutputValues.append(mBrightness);
		mInputValues.append(mBrightness);
		
		createCollisionObject();
		
		updateBrightnessValue();
	}
	
	/**
	 * Destructor.
	 */
	SphericLightSource::~SphericLightSource() {
		removeCollisionObject(mBodyCollisionObject);
		delete mBodyCollisionObject;
	}
	
	
	SimBody* SphericLightSource::createCopy() const {
		return new SphericLightSource(*this);
	}
	
	
	
	void SphericLightSource::setup()  {
		LightSource::setup();

		if(mReferenceObjectName->get() != "") {
			if(mReferenceObject != 0) {
				mReferenceObject->getPositionValue()->removeValueChangedListener(this);
				mReferenceObject->getQuaternionOrientationValue()->removeValueChangedListener(this);
				mReferenceObject = 0;
			}
			
			mReferenceObject = Physics::getPhysicsManager()->getSimBody(mReferenceObjectName->get());
			if(mReferenceObject != 0) {
				mReferenceObject->getPositionValue()->addValueChangedListener(this);
				mReferenceObject->getQuaternionOrientationValue()->addValueChangedListener(this);
				valueChanged(mReferenceObject->getPositionValue());
			}
			else if(mReferenceObjectName->get().trimmed() != "") {
				//error
				Core::log("SphericalLightSource: Could not find reference object ["
						  + mReferenceObjectName->get() + "]! Ignoring!", true);
			}
		}
		
		if(mBodyCollisionObject != 0) {
			mBodyCollisionObject->setOwner(this);

			// is already done by addCollisionObject()
			//mBodyCollisionObject->setHostBody(this);

			/* radius is already handled/set by createCollisionObject, why again?
			if(mSphericLightCone->get()) {
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
			*/
		}
		
		updateBrightnessValue();
	}
	
	
	void SphericLightSource::clear() {
		if(mReferenceObject != 0) {
			mReferenceObject->getPositionValue()->removeValueChangedListener(this);
			mReferenceObject->getQuaternionOrientationValue()->removeValueChangedListener(this);
		}
		mReferenceObject = 0;
		LightSource::clear();
	}
	
	
	
	void SphericLightSource::valueChanged(Value *value) {
		LightSource::valueChanged(value);
		if(value == 0) {
			return;
		}
		else if(value == mDesiredBrightness) {
			//Core::log("SphericLightSource: DesiredBrightness has changed!", true);
			updateBrightnessValue();
		}
		else if(value == mRadius) {
			if(mSphericLightCone->get()) {
				SphereGeom *geom = dynamic_cast<SphereGeom*>(mBodyCollisionObject->getGeometry());
				if(geom != 0) {
					geom->setRadius(mRadius->get());
				}
			}
			else {
				CylinderGeom *geom = dynamic_cast<CylinderGeom*>(mBodyCollisionObject->getGeometry());
				if(geom != 0) {
					geom->setRadius(mRadius->get());
				}
			}
		}
		else if(value == mHideLightCone) {
			updateLightCone();
		}
		else if(value == mLightColor) {
			updateLightCone();
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
			Quaternion rotatedLocalPosQuat = mReferenceObject->getQuaternionOrientationValue()->get() *
												localPos * bodyOrientationInverse;

			Vector3D rotatedLocalPos(rotatedLocalPosQuat.getX(), 
				rotatedLocalPosQuat.getY(), 
				rotatedLocalPosQuat.getZ());
			mPositionValue->set(mReferenceObject->getPositionValue()->get() + rotatedLocalPos);
		}
		else if(value == mRange) {
			// Core::log("SphericLightSource: Range has been changed!", true);
			mBrightness->setMin(mRange->getMin());
			mBrightness->setMax(mRange->getMax());
			updateBrightnessValue();
		}
	}
	
	
	void SphericLightSource::updateBrightnessValue() {
		//Core::log("SphericLightSource: updateBrightnessValue called!", true);
		//cout << "old Desired Brightness: " << mDesiredBrightness->get() << endl;
		//cout << "old Actual Brightness: " << mBrightness->get() << endl;
		mBrightness->set(mDesiredBrightness->get());
		//cout << "new Actual Brightness: " << mBrightness->get() << endl;
		
		updateLightCone();
		
	}
	
	void SphericLightSource::updateLightCone() {
		//Set transparency to a value between 0 and 80, depending on the current brightness.
		//80 is about 30 percent of full opacity (255) at max.
		Color color = mLightColor->get();
		if(mHideLightCone->get()) {
			color.setAlpha(0); 
		}
		else {
			color.setAlpha((int) (Math::abs(mBrightness->get()) * 80.0)); 
		}
		mGeometryColorValue->set(color);
		mBodyCollisionObject->getGeometry()->setColor(color);
	}
	
	double SphericLightSource::getRadius() const {
		return mRadius->get();
	}
	
	void SphericLightSource::setRadius(double radius) {
		if(radius < 0) {
			radius = 0;
		}
		mRadius->set(radius);
	}

	void SphericLightSource::setRange(double min, double max) {
		mRange->set(min, max);
	}
	
	double SphericLightSource::getActualBrightness() const {
		return mBrightness->get();
	}
	
	double SphericLightSource::getDesiredBrightness() const {
		return mDesiredBrightness->get();
	}
	
	void SphericLightSource::setDesiredBrightness(double brightness) {
		mDesiredBrightness->set(brightness);
	}
	
	double SphericLightSource::getBrightness(
			const Vector3D &globalPosition, const bool &restrictToHorizontal)
	{
		double brightness = 0.0;
		double distance = 0.0;
		
		if(restrictToHorizontal) {
			if(mSwitchYZAxes != 0 && mSwitchYZAxes->get()) {
				distance = Math::abs(Math::distance(
							QPointF(globalPosition.getX(), globalPosition.getZ()),
							QPointF(getPositionValue()->getX(), getPositionValue()->getZ()
				)));
			}
			else {
				distance = Math::abs(Math::distance(
							QPointF(globalPosition.getX(), globalPosition.getY()),
							QPointF(getPositionValue()->getX(), getPositionValue()->getY()
				)));
			}
		}
		else {
			distance = Math::distance(globalPosition, getPositionValue()->get());
		}
		
		double radius = getRadius();
		
		if(radius == 0.0 || distance > radius) {
			return brightness;
		}
		
		/**
		if(mUniformLight->get()) {
			brightness = getCurrentBrightness();
		}
		else {
			//currently the light is assumed to decay linearly 
			//TODO should be changed to 1/(r*r).
			brightness = (range - distance) / range * getCurrentBrightness();
		}
		**/

		switch(mDistributionType->get()) {
			case 0:
				brightness = getActualBrightness();
				break;
			case 1:
				brightness = (radius - distance) / radius * getActualBrightness();
				break;
			// TODO add cubic decay
		}
		
		return brightness;
	}
	
	
	void SphericLightSource::createCollisionObject() {
		
		if(mBodyCollisionObject != 0) {
			delete mBodyCollisionObject;
		}
		mBodyCollisionObject = 0;

		// sphere
		if(mSphericLightCone->get()) {
			mBodyCollisionObject = new CollisionObject(SphereGeom(this, mRadius->get()));
		}
		// cylinder
		else {
			CylinderGeom geom(this, mRadius->get(), 0.05); // why a length of 0.05 exactly?
			Quaternion orientation;
			if(mSwitchYZAxes != 0 && mSwitchYZAxes->get()) {
				orientation.setFromAngles(90.0, 0.0, 0.0);
			}
			else {
				orientation.setFromAngles(0.0, 0.0, 90.0);
			}
			geom.setLocalOrientation(orientation);
			mBodyCollisionObject = new CollisionObject(geom);
		}
		addGeometry(mBodyCollisionObject->getGeometry());
		mBodyCollisionObject->setMaterialType("Light");
		mBodyCollisionObject->setTextureType("None");
		mBodyCollisionObject->getGeometry()->setColor(mLightColor->get());
		addCollisionObject(mBodyCollisionObject);
	}
	
	
	
	
}
