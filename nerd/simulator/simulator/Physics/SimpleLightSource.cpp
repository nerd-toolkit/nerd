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

#include "SimpleLightSource.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/CylinderGeom.h"
#include "Physics/Physics.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {
	
	
	/**
	 * Constructs a new SimpleLightSource.
	 */
	SimpleLightSource::SimpleLightSource(
			const QString &name, double brightness, double radius, int type)
	: LightSource(name, type), mRadius(0), mLightColor(0),
		mHideLightCone(0), mReferenceObjectName(0),
		mLocalPosition(0), mReferenceObject(0),
		mDistributionType(0), mSwitchYZAxes(0)
	{
		mRadius = new DoubleValue(Math::max(0.0,radius));
		mCenterBrightness = new DoubleValue(brightness);
		mLightColor = new ColorValue("yellow");
		mHideLightCone = new BoolValue(false);
		mReferenceObjectName = new StringValue("");
		mLocalPosition = new Vector3DValue(0.0, 0.0, 0.0);
		mDistributionType = new IntValue(0);
		
		mRadius->setDescription(
				"Radius of light around the light source's center");
		mCenterBrightness->setDescription(
				"The light source's brightness at center");
		mLightColor->setDescription(
				"The color of the light for visualization/simulation");
		mHideLightCone->setDescription(
				"If set to True, the light cone is hidden in the simulation");
		mReferenceObjectName->setDescription(
				"The name on another simulation object "
				"to attach the light source to");
		mLocalPosition->setDescription(
				"Position of the light source in the simulation environment");
		mDistributionType->setDescription("The type of light distribution:\n"
										   "0: homogeneous distribution\n"
										   "1: linear decay from center\n"
										   "2: Quadratic decay from center\n"
										   "3: Cubic decay from the center");
		
		addParameter("Radius", mRadius);
		addParameter("CenterBrightness", mCenterBrightness);
		addParameter("LightColor", mLightColor);
		addParameter("HideLightCone", mHideLightCone);
		addParameter("ReferenceObject", mReferenceObjectName);
		addParameter("LocalPosition", mLocalPosition);
		addParameter("DistributionType", mDistributionType);
		
		Physics::getPhysicsManager();
		
		if(mSwitchYZAxes == 0) {
			mSwitchYZAxes = Core::getInstance()->getValueManager()->
				getBoolValue(SimulationConstants::VALUE_SWITCH_YZ_AXES);
		}
		
		createCollisionObject();
		updateLightCone();
	}
	
	
	/**
	 * Copy constructor. 
	 * 
	 * @param other the SimpleLightSource object to copy.
	 */
	SimpleLightSource::SimpleLightSource(const SimpleLightSource &other) 
	: Object(), ValueChangedListener(), LightSource(other), 
		mReferenceObjectName(0), mLocalPosition(0), mReferenceObject(0),
		mDistributionType(other.mDistributionType),
		mSwitchYZAxes(other.mSwitchYZAxes)
	{
		mRadius = dynamic_cast<DoubleValue*>
			(getParameter("Radius"));
		mCenterBrightness = dynamic_cast<DoubleValue*>
			(getParameter("CenterBrightness"));
		mLightColor = dynamic_cast<ColorValue*>
			(getParameter("LightColor"));
		mHideLightCone = dynamic_cast<BoolValue*>
			(getParameter("HideLightCone"));
		mReferenceObjectName = dynamic_cast<StringValue*>
			(getParameter("ReferenceObject"));
		mLocalPosition = dynamic_cast<Vector3DValue*>
			(getParameter("LocalPosition"));
		mDistributionType = dynamic_cast<IntValue*>
			(getParameter("DistributionType"));

		mOutputValues.clear();
		mInputValues.clear();
		
		createCollisionObject();
		updateLightCone();
	}
	
	/**
	 * Destructor.
	 */
	SimpleLightSource::~SimpleLightSource() {
		removeCollisionObject(mBodyCollisionObject);
		delete mBodyCollisionObject;
	}
	
	
	SimBody* SimpleLightSource::createCopy() const {
		return new SimpleLightSource(*this);
	}
	
	void SimpleLightSource::setup()  {
		//Core::log("SimpleLightSource: setup() called!", true);
		LightSource::setup();

		if(mReferenceObject != 0) {
			mReferenceObject->getPositionValue()->
				removeValueChangedListener(this);
			mReferenceObject->getQuaternionOrientationValue()->
				removeValueChangedListener(this);
			mReferenceObject = 0;
		}

		if(!mReferenceObjectName->get().isEmpty()) {
		
			mReferenceObject = Physics::getPhysicsManager()->
				getSimBody(mReferenceObjectName->get());

			if(mReferenceObject == 0) {
				Core::log("SimpleLightSource: Could not find "
						"reference object [" + 
						mReferenceObjectName->get() +
						"]! Ignoring!", true);
				// TODO return here?
			} else {
				mReferenceObject->getPositionValue()->
					addValueChangedListener(this);
				mReferenceObject->getQuaternionOrientationValue()->
					addValueChangedListener(this);
				valueChanged(mReferenceObject->getPositionValue());
			}
		}
		
		if(mBodyCollisionObject != 0) {
			mBodyCollisionObject->setOwner(this);
		}
	}
	
	
	void SimpleLightSource::clear() {
		if(mReferenceObject != 0) {
			mReferenceObject->getPositionValue()->
				removeValueChangedListener(this);
			mReferenceObject->getQuaternionOrientationValue()->
				removeValueChangedListener(this);
		}
		mReferenceObject = 0;
		LightSource::clear();
	}
	
	void SimpleLightSource::valueChanged(Value *value) {
		LightSource::valueChanged(value);
		if(value == 0) {
			return;
		}
		if(value == mCenterBrightness) {
			updateLightCone();
		}
		else if(value == mRadius) {
			CylinderGeom *geom = dynamic_cast<CylinderGeom*>
				(mBodyCollisionObject->getGeometry());
			if(geom != 0) {
				geom->setRadius(mRadius->get());
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
			// TODO change this? see Test line 247
			//
			Quaternion localPos(
					0.0,
					mLocalPosition->getX(),
					mLocalPosition->getY(),
					mLocalPosition->getZ()
				);
			Quaternion bodyOrientationInverse =	mReferenceObject->
				getQuaternionOrientationValue()->get().getInverse();

			Quaternion rotatedLocalPosQuat = mReferenceObject->
				getQuaternionOrientationValue()->get() * localPos *
				bodyOrientationInverse;

			Vector3D rotatedLocalPos(
					rotatedLocalPosQuat.getX(),
					rotatedLocalPosQuat.getY(),
					rotatedLocalPosQuat.getZ()
				);
			mPositionValue->set(mReferenceObject->
					getPositionValue()->get() + rotatedLocalPos);
		}
	}
	
	void SimpleLightSource::updateLightCone() {
		// Set transparency to a value between 0 and 80,
		// depending on the current brightness.
		// 80 is about 30 percent of full opacity (255) at max.
		Color color = mLightColor->get();
		if(mHideLightCone->get()) {
			color.setAlpha(0); 
		}
		else {
			color.setAlpha((int)
					(Math::abs(mCenterBrightness->get()) * 80.0));
		}
		mGeometryColorValue->set(color);
		//mBodyCollisionObject->getGeometry()->setColor(color);
	}
	
	double SimpleLightSource::getRadius() const {
		return mRadius->get();
	}
	
	void SimpleLightSource::setRadius(double radius) {
		mRadius->set(Math::max(0.0, radius));
	}

	double SimpleLightSource::getCenterBrightness() const {
		return mCenterBrightness->get();
	}
	
	void SimpleLightSource::setCenterBrightness(double brightness) {
		mCenterBrightness->set(brightness);
	}
	
	double SimpleLightSource::getBrightness(
			const Vector3D &globalPosition, const bool &restrictToHorizontal)
	{
		double brightness = 0.0;
		double distance = 0.0;

		if(restrictToHorizontal) {
			if(mSwitchYZAxes != 0 && mSwitchYZAxes->get()) {
				distance = Math::abs(Math::distance(
							QPointF(globalPosition.getX(),
								globalPosition.getZ()),
							QPointF(getPositionValue()->getX(),
								getPositionValue()->getZ()
				)));
			}
			else {
				distance = Math::abs(Math::distance(
							QPointF(globalPosition.getX(),
								globalPosition.getY()),
							QPointF(getPositionValue()->getX(),
								getPositionValue()->getY()
				)));
			}
		}
		else {
			distance = Math::distance(globalPosition,
					getPositionValue()->get());
		}
		
		double radius = getRadius();
		
		if(radius == 0.0 || distance > radius) {
			return brightness;
		}
		
		double relDis = radius - distance;

		switch(mDistributionType->get()) {
			case 0: // uniform distribution
				brightness = getCenterBrightness();
				break;
			case 1: // linear decay
				brightness = relDis / radius * getCenterBrightness();
				break;
			case 2: // quadratic decay
				brightness = (relDis * relDis) / (radius * radius)
					* getCenterBrightness();
				break;
			case 3: // cubic decay
				brightness = (relDis * relDis * relDis)
					/ (radius * radius * radius)
					* getCenterBrightness();
				break;				
		}
		return brightness;
	}
	
	
	void SimpleLightSource::createCollisionObject() {
		
		if(mBodyCollisionObject != 0) {
			delete mBodyCollisionObject;
		}
		mBodyCollisionObject = 0;

		CylinderGeom geom(this, mRadius->get(), 0.05);
		Quaternion orientation;
		if(mSwitchYZAxes != 0 && mSwitchYZAxes->get()) {
			orientation.setFromAngles(90.0, 0.0, 0.0);
		}
		else {
			orientation.setFromAngles(0.0, 0.0, 90.0);
		}
		geom.setLocalOrientation(orientation);
		mBodyCollisionObject = new CollisionObject(geom);

		addGeometry(mBodyCollisionObject->getGeometry());
		mBodyCollisionObject->setMaterialType("Light");
		mBodyCollisionObject->setTextureType("None");
		mBodyCollisionObject->getGeometry()->setColor(mLightColor->get());
		addCollisionObject(mBodyCollisionObject);
	}
	
	
	
	
}
