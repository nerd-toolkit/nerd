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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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



#include "SinkBox.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Collision/CollisionManager.h"
#include <QStringList>

using namespace std;

namespace nerd {


/**
 * Constructs a new SinkBox.
 */
SinkBox::SinkBox(const QString &name)
	: SimObject(name), mBody(0), mSlider(0), mPosition(0), mOrientation(0), mDimensions(0),
		mColor(0), mMaximalForce(0), mCollisionFreeObjectNames(0), 
		mGlobalSizeFactor(0), mBoxWidth(0), mBoxHeight(0), 
		mBoxDepth(0), mBoxPosition(0), mBoxOrientation(0), mBoxColor(0), mSliderDesiredSetting(0),
		mSliderAxisPoint1(0), mSliderAxisPoint2(0), mSliderMinPosition(0),
		mSliderMaxPosition(0), mSliderMaxForce(0)
{
	mTolerance = 0.01;

	PhysicsManager *pm = Physics::getPhysicsManager();

	BoxBody *boxPrototype = dynamic_cast<BoxBody*>(
					pm->getPrototype(SimulationConstants::PROTOTYPE_BOX_BODY));
	if(boxPrototype != 0) {
		mBody = dynamic_cast<BoxBody*>(boxPrototype->createCopy());
	}
	SliderMotor *sliderProtoype = dynamic_cast<SliderMotor*>(
					pm->getPrototype(SimulationConstants::PROTOTYPE_SLIDER_MOTOR));
	if(sliderProtoype != 0) {
		mSlider = dynamic_cast<SliderMotor*>(sliderProtoype->createCopy());
	}

	if(mBody != 0) {
		addChildObject(mBody);
	}
	if(mSlider != 0) {
		addChildObject(mSlider);
	}

	mGlobalSizeFactor = Core::getInstance()->getValueManager()
				->getDoubleValue("/Sim/SinkBox/GlobalSizeFactor");
	if(mGlobalSizeFactor == 0) {
		mGlobalSizeFactor = new DoubleValue(1.0);
		Core::getInstance()->getValueManager()
						->addValue("/Sim/SinkBox/GlobalSizeFactor", mGlobalSizeFactor);
	}

	mPosition = new Vector3DValue();
	mOrientation = new Vector3DValue();
	mDimensions = new Vector3DValue(0.1, 0.1, 0.1);
	mColor = new ColorValue("black");
	mMaximalForce = new DoubleValue(0.8);
	mCollisionFreeObjectNames = new StringValue("/Ground");

	addParameter("Position", mPosition);
	addParameter("Orientation", mOrientation);
	addParameter("Dimensions", mDimensions);
	addParameter("Color", mColor);
	addParameter("MaxForce", mMaximalForce);	
	addParameter("CollisionFreeObjects", mCollisionFreeObjectNames);
	
	mSwitchYZAxes = Core::getInstance()->getValueManager()->getBoolValue(SimulationConstants::VALUE_SWITCH_YZ_AXES);
	

	updateChildObjectNames();
	collectParameters();
}


/**
 * Copy constructor. 
 * 
 * @param other the SinkBox object to copy.
 */
SinkBox::SinkBox(const SinkBox &other) 
	: Object(), ValueChangedListener(), SimObject(other), mBody(0), mSlider(0),
		mPosition(0), mOrientation(0), mDimensions(0), mColor(0), mMaximalForce(0),
		mCollisionFreeObjectNames(0), mBoxWidth(0), mBoxHeight(0), mBoxDepth(0),
		mBoxPosition(0), mBoxOrientation(0), mBoxColor(0), mSliderDesiredSetting(0),
		mSliderAxisPoint1(0), mSliderAxisPoint2(0), mSliderMinPosition(0),
		mSliderMaxPosition(0), mSliderMaxForce(0), mTolerance(other.mTolerance)
{

	if(mChildObjects.size() == 0) {
		PhysicsManager *pm = Physics::getPhysicsManager();

		BoxBody *boxPrototype = dynamic_cast<BoxBody*>(
					pm->getPrototype(SimulationConstants::PROTOTYPE_BOX_BODY));
		if(boxPrototype != 0) {
			mBody = dynamic_cast<BoxBody*>(boxPrototype->createCopy());
		}
		SliderMotor *sliderProtoype = dynamic_cast<SliderMotor*>(
						pm->getPrototype(SimulationConstants::PROTOTYPE_SLIDER_MOTOR));
		if(sliderProtoype != 0) {
			mSlider = dynamic_cast<SliderMotor*>(sliderProtoype->createCopy());
		}
	
		if(mBody != 0) {
			addChildObject(mBody);
		}
		if(mSlider != 0) {
			addChildObject(mSlider);
		}
	}
	if(mChildObjects.size() > 0) {
		mBody = dynamic_cast<BoxBody*>(mChildObjects.at(0));
	}
	if(mChildObjects.size() > 1) {
		mSlider = dynamic_cast<SliderMotor*>(mChildObjects.at(1));
	}

	mGlobalSizeFactor = Core::getInstance()->getValueManager()
				->getDoubleValue("/Sim/SinkBox/GlobalSizeFactor");

	mPosition = dynamic_cast<Vector3DValue*>(getParameter("Position"));
	mOrientation = dynamic_cast<Vector3DValue*>(getParameter("Orientation"));
	mDimensions = dynamic_cast<Vector3DValue*>(getParameter("Dimensions"));
	mColor = dynamic_cast<ColorValue*>(getParameter("Color"));
	mMaximalForce = dynamic_cast<DoubleValue*>(getParameter("MaxForce"));
	mCollisionFreeObjectNames = dynamic_cast<StringValue*>(getParameter("CollisionFreeObjects"));
	
	mSwitchYZAxes = other.mSwitchYZAxes;

	updateChildObjectNames();
	collectParameters();
}

/**
 * Destructor.
 */
SinkBox::~SinkBox() {
}

SimObject* SinkBox::createCopy() const {
	return new SinkBox(*this);
}

void SinkBox::setup() {
	updateChildObjects();
	updateCollisionFreeObjects();
}


void SinkBox::clear() {

}


		
void SinkBox::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	if(value == mPosition || value == mDimensions || value == mOrientation 
		|| value == mColor || value == mMaximalForce) 
	{
		updateChildObjects();
	}
	else if(value == mNameValue) {
		updateChildObjectNames();
	}
	else if(value == mCollisionFreeObjectNames) {

	}
}


void SinkBox::updateChildObjectNames() {
	if(mBody != 0) {
		mBody->setName(getName() + "/Box");
	}
	if(mSlider != 0) {
		mSlider->setName(getName() + "/SliderMotor");

		StringValue *firstBodyName = dynamic_cast<StringValue*>(mSlider->getParameter("FirstBody"));
		if(firstBodyName != 0) {
			QString name = QString("/") + getName() + "/Box";
			firstBodyName->set(name);
		}
	}
}


void SinkBox::collectParameters() {

	if(mBody != 0) {

		mBoxWidth = dynamic_cast<DoubleValue*>(mBody->getParameter("Width"));
		mBoxHeight = dynamic_cast<DoubleValue*>(mBody->getParameter("Height"));
		mBoxDepth = dynamic_cast<DoubleValue*>(mBody->getParameter("Depth"));
		mBoxPosition = dynamic_cast<Vector3DValue*>(mBody->getParameter("Position"));
		mBoxOrientation = dynamic_cast<Vector3DValue*>(mBody->getParameter("Orientation"));
		mBoxColor = dynamic_cast<ColorValue*>(mBody->getParameter("Color"));

		DoubleValue *mass = dynamic_cast<DoubleValue*>(mBody->getParameter("Mass"));
		if(mass != 0) {
			mass->set(0.05);
		}
	}
	else {
		mBoxWidth = 0;
		mBoxHeight = 0;
		mBoxDepth = 0;
		mBoxPosition = 0;
		mBoxOrientation = 0;
		mBoxColor = 0;
	}
	if(mSlider != 0) {
		mSliderDesiredSetting = dynamic_cast<DoubleValue*>(mSlider->getParameter("DesiredSetting"));
		mSliderAxisPoint1 = dynamic_cast<Vector3DValue*>(mSlider->getParameter("AxisPoint1"));
		mSliderAxisPoint2 = dynamic_cast<Vector3DValue*>(mSlider->getParameter("AxisPoint2"));
		mSliderMinPosition = dynamic_cast<DoubleValue*>(mSlider->getParameter("MinPosition"));
		mSliderMaxPosition = dynamic_cast<DoubleValue*>(mSlider->getParameter("MaxPosition"));
		mSliderMaxForce = dynamic_cast<DoubleValue*>(mSlider->getParameter("MaxForce"));
	
		DoubleValue *param = dynamic_cast<DoubleValue*>(mSlider->getParameter("Friction/StaticFriction"));
		if(param != 0) {
			param->set(0.0);
		}
		param = dynamic_cast<DoubleValue*>(mSlider->getParameter("Friction/DynamicFriction"));
		if(param != 0) {
			param->set(0.0);
		}
		param = dynamic_cast<DoubleValue*>(mSlider->getParameter("SensorNoise"));
		if(param != 0) {
			param->set(0.0);
		}
		param = dynamic_cast<DoubleValue*>(mSlider->getParameter("PID_P"));
		if(param != 0) {
			param->set(10.0);
		}
		
		
	}
	else {
		mSliderDesiredSetting = 0;
		mSliderAxisPoint1 = 0;
		mSliderAxisPoint2 = 0;
		mSliderMinPosition = 0;
		mSliderMaxPosition = 0;
	}
}

void SinkBox::updateChildObjects() {

	if(mBody == 0 || mSlider == 0) {
		return;
	}

	double dimY = 0.0;
	if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
		dimY = mDimensions->getY();
	}
	else {
		dimY = mDimensions->getZ();
	}
	
	if(mGlobalSizeFactor != 0) {
		dimY = dimY * mGlobalSizeFactor->get();
	}

	mBoxWidth->set(mDimensions->getX());
	
	if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
		mBoxHeight->set(dimY);
		mBoxDepth->set(mDimensions->getZ());
	}
	else {
		mBoxHeight->set(mDimensions->getY());
		mBoxDepth->set(dimY);
	}
	
	mBoxColor->set(mColor->get());

	mBoxOrientation->set(mOrientation->get());
	if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
		mBoxPosition->set(mPosition->get() - Vector3D(0.0, mTolerance, 0.0));
	}
	else {
		mBoxPosition->set(mPosition->get() - Vector3D(0.0, 0.0, mTolerance));
	}

	if(mSwitchYZAxes == 0 || mSwitchYZAxes->get()) {
		mSliderAxisPoint1->set(Vector3D(mPosition->getX(), 
					mPosition->getY() - dimY - mTolerance, mPosition->getZ()));
		mSliderAxisPoint2->set(Vector3D(
					mPosition->getX(), mPosition->getY() + mTolerance, mPosition->getZ()));
	}
	else {
		mSliderAxisPoint1->set(Vector3D(mPosition->getX(), 
										mPosition->getY(), mPosition->getZ() - dimY - mTolerance));
		mSliderAxisPoint2->set(Vector3D(
			mPosition->getX(), mPosition->getY(), mPosition->getZ() + mTolerance));
	}
	mSliderDesiredSetting->set(dimY + mTolerance);
	mSliderMinPosition->set(0);
	mSliderMaxPosition->set(dimY + (2 * mTolerance));
	mSliderMaxForce->set(mMaximalForce->get());
	
	
}


void SinkBox::updateCollisionFreeObjects() {
	CollisionManager *cm = Physics::getCollisionManager();
	PhysicsManager *pm = Physics::getPhysicsManager();

	CollisionObject *bodyCollisionObject = mBody->getCollisionObjects().at(0);

	//enable collisions with previous collision free objects
	while(!mCollisionFreeObjects.empty()) {
		SimBody *obj = mCollisionFreeObjects.takeFirst();
		cm->disableCollisions(bodyCollisionObject, obj->getCollisionObjects(), false);
	}

	//disable collisions with current collision free objects
	QStringList collisionFreeObjectNames = mCollisionFreeObjectNames->get().split(",");
	for(QListIterator<QString> i(collisionFreeObjectNames); i.hasNext();) {
		QString name = i.next();
		SimBody *body = pm->getSimBody(name);
		if(body != 0) {
			mCollisionFreeObjects.append(body);
			cm->disableCollisions(bodyCollisionObject, body->getCollisionObjects(), true);
		}
		else {
			Core::log("SinkBox: Could not find collision free object [" + name + "]");
			cerr << "Could not find collisionObject" << endl;
		}
	}
}	

}



