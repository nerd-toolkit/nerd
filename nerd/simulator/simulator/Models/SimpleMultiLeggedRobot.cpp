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



#include "SimpleMultiLeggedRobot.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include "Physics/SimBody.h"
#include "Physics/SimObject.h"
#include "Value/ColorValue.h"
#include "Value/Vector3DValue.h"
#include "Physics/SphereGeom.h"
#include "Collision/CollisionObject.h"
#include "Math/Math.h"


#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))

#define INIT_PARAMS SimObject *__paraObj = 0;

#define MAKE_CURRENT(paraObj) __paraObj = paraObj;

#define SETP(paraName, value) \
		{ if(__paraObj == 0) { Core::log("No Current Param found", true); } \
		else { Value *__v = __paraObj->getParameter(paraName); \
			if(__v == 0) { Core::log(QString("Param Problem") + paraName + " = " + value, true); } \
			else { __v->setValueFromString(value); }}}

using namespace std;

namespace nerd {


/**
 * Constructs a new SimpleMultiLeggedRobot.
 */
SimpleMultiLeggedRobot::SimpleMultiLeggedRobot(const QString &groupName)
	: ModelInterface(groupName), mGroupName(groupName)
{
	mNumberOfSegmentsValue = new IntValue(5);
	mUseSingleBody = new BoolValue(false);

	mSegmentLengthValue = new DoubleValue(0.3);
	mUpperLegLengthValue = new DoubleValue(0.25);
	mLowerLegLengthValue = new DoubleValue(0.2);

	addParameter("NumberOfSegments", mNumberOfSegmentsValue);
	addParameter("UseSingleBody", mUseSingleBody);
	addParameter("SegmentLength", mSegmentLengthValue);
	addParameter("UpperLegLength", mUpperLegLengthValue);
	addParameter("LowerLegLength", mLowerLegLengthValue);
}


/**
 * Copy constructor. 
 * 
 * @param other the SimpleMultiLeggedRobot object to copy.
 */
SimpleMultiLeggedRobot::SimpleMultiLeggedRobot(const SimpleMultiLeggedRobot &other) 
	: Object(), ValueChangedListener(), EventListener(), ModelInterface(other), mGroupName(other.mGroupName)
{
	mNumberOfSegmentsValue = dynamic_cast<IntValue*>(getParameter("NumberOfSegments"));
	mUseSingleBody = dynamic_cast<BoolValue*>(getParameter("UseSingleBody"));
	mSegmentLengthValue = dynamic_cast<DoubleValue*>(getParameter("SegmentLength"));
	mUpperLegLengthValue = dynamic_cast<DoubleValue*>(getParameter("UpperLegLength"));
	mLowerLegLengthValue = dynamic_cast<DoubleValue*>(getParameter("LowerLegLength"));
}

/**
 * Destructor.
 */
SimpleMultiLeggedRobot::~SimpleMultiLeggedRobot() {
}


SimObject* SimpleMultiLeggedRobot::createCopy() const {
	return new SimpleMultiLeggedRobot(*this);
}

void SimpleMultiLeggedRobot::createModel() {

	PhysicsManager *pm = Physics::getPhysicsManager();

	mBoxBodyPrototype = pm->getPrototype(SimulationConstants::PROTOTYPE_BOX_BODY); 
	mMotor1DPrototype = pm->getPrototype(SimulationConstants::PROTOTYPE_DYNAMIXEL);
	mFixedJointPrototype =  pm->getPrototype(SimulationConstants::PROTOTYPE_FIXED_JOINT);
	mForceSensorPrototype = pm->getPrototype(SimulationConstants::PROTOTYPE_FORCE_SENSOR);
	mCapsulePrototype = pm->getPrototype(SimulationConstants::PROTOTYPE_CAPSULE_BODY);

	mNamePrefix = "/";
	mGroupPrefix = QString("/").append(mGroupName);
	
	mLegThickness = 0.02;
	mHalfBodyWidth = 0.02;
	//mBodyLength = 0.04;
	mBodyLength = mSegmentLengthValue->get();
	mConnectorLength = 0.02;
	mUpperLegLength = mUpperLegLengthValue->get();
	mLowerLegLength = mLowerLegLengthValue->get();
	mLegStubLength = 0.01;

	if(mUseSingleBody->get()) {
		createSingleBody();
	}
	else {
		createSegmentBody();
	}

	mSimObjects = mAgent->getObjects();
	pm->addSimObjectGroup(mAgent);

}

void SimpleMultiLeggedRobot::layoutObjects() {

}

void SimpleMultiLeggedRobot::createSegmentBody() {
	int numberOfSegments = mNumberOfSegmentsValue->get();

	for(int i = 0; i < numberOfSegments; ++i) {
		createSegment(i);
	}
}


void SimpleMultiLeggedRobot::createSingleBody() {
	int numberOfSegments = mNumberOfSegmentsValue->get();

	INIT_PARAMS;

	double bodyLength = numberOfSegments * mBodyLength;

	SimBody *body = dynamic_cast<SimBody*>(mCapsulePrototype->createCopy());
	mAgent->addObject(body);
	MAKE_CURRENT(body);
	SETP("Name", mNamePrefix + "Body");
	SETP("Mass", QString::number(0.01 * numberOfSegments));
	SETP("Position", Vector3DValue(bodyLength / 2.0, 0.0, 0.0).getValueAsString());
	SETP("CenterOfMass", "(0,0,0)");
	SETP("Orientation", "(0,90,0)");
	SETP("Dynamic", "true");
	//SETP("Width", DoubleValue(bodyLength).getValueAsString());
	//SETP("Height", DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
	//SETP("Depth",  DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
	SETP("Length", DoubleValue(Math::max(0.0, bodyLength - (2 * mHalfBodyWidth))).getValueAsString());
	SETP("Radius", DoubleValue(mHalfBodyWidth).getValueAsString());
	SETP("Color", "red");
	SETP("Material", "ABS");

	mSegmentMainBodies.append(body);

	for(int i = 0; i < numberOfSegments; ++i) {
		createLegs(body, (i * mBodyLength) + (mBodyLength / 2.0), true, i);
		createLegs(body, (i * mBodyLength) + (mBodyLength / 2.0), false, i);
	}
}

void SimpleMultiLeggedRobot::createSegment(int segmentNumber) {

	INIT_PARAMS;

	double offset = segmentNumber * mBodyLength;
	if(!mSegmentMainBodies.empty()) {
		offset += mConnectorLength;
	}

	SimBody *currentBody = dynamic_cast<SimBody*>(mCapsulePrototype->createCopy());
	mAgent->addObject(currentBody);
	MAKE_CURRENT(currentBody);
	SETP("Name", mNamePrefix + QString::number(segmentNumber) + "/Body");
	SETP("Mass", "0.01");
	SETP("Position", Vector3DValue(offset, 0.0, 0.0).getValueAsString());
	SETP("CenterOfMass", "(0,0,0)");
	SETP("Orientation", "(0,90,0)");
	SETP("Dynamic", "true");
	//SETP("Width", DoubleValue(mBodyLength).getValueAsString());
	//SETP("Height", DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
	//SETP("Depth",  DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
	SETP("Length", DoubleValue(Math::max(0.0, mBodyLength - (2.0 * mHalfBodyWidth))).getValueAsString());
	SETP("Radius", DoubleValue(mHalfBodyWidth).getValueAsString());
	SETP("Color", "red");
	SETP("Material", "ABS");

	if(!mSegmentMainBodies.empty()) {
		//add a joint between the current and the previous segment
		SimBody *previousBody = mSegmentMainBodies.last();

		SimBody *middlePart = dynamic_cast<SimBody*>(mCapsulePrototype->createCopy());
		mAgent->addObject(middlePart);
		MAKE_CURRENT(middlePart);
		SETP("Name", mNamePrefix + QString::number(segmentNumber) + "/Connector");
		SETP("Mass", "0.001");
		SETP("Position", Vector3DValue(offset - ((mBodyLength / 2.0) + (mConnectorLength / 2.0)), 
							0.0, 0.0).getValueAsString());
		SETP("CenterOfMass", "(0,0,0)");
		SETP("Orientation", "(0,90,0)");
		SETP("Dynamic", "true");
		//SETP("Width", DoubleValue(mConnectorLength).getValueAsString());
		//SETP("Height", DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
		//SETP("Depth",  DoubleValue(mHalfBodyWidth * 2.0).getValueAsString());
		SETP("Length", DoubleValue(Math::max(0.0, mConnectorLength - (2.0 * mHalfBodyWidth))).getValueAsString());
		SETP("Radius", DoubleValue(mHalfBodyWidth).getValueAsString());
		SETP("Color", "blue");
		SETP("Material", "ABS");
		
		SimObject *tiltMotor = mMotor1DPrototype->createCopy();
		mAgent->addObject(tiltMotor);
		MAKE_CURRENT(tiltMotor);
		SETP("Name", mNamePrefix + QString::number(segmentNumber) + "/SegmentTiltMotor");
		SETP("FirstBody", mGroupPrefix + "/" + QString::number(segmentNumber - 1) + "/Body");
		SETP("SecondBody", mGroupPrefix + "/" + QString::number(segmentNumber) + "/Connector");
		SETP("AxisPoint1", Vector3DValue(offset - ((mBodyLength / 2.0) + mConnectorLength), 0.0, 0.1)
							.getValueAsString());
		SETP("AxisPoint2", Vector3DValue(offset - ((mBodyLength / 2.0) + mConnectorLength), 0.0, -0.1)
							.getValueAsString());
		SETP("MinAngle", "-45");
		SETP("MaxAngle", "90");

		SimObject *panMotor = mMotor1DPrototype->createCopy();
		mAgent->addObject(panMotor);
		MAKE_CURRENT(panMotor);
		SETP("Name", mNamePrefix + QString::number(segmentNumber) + "/SegmentPanMotor");
		SETP("FirstBody", mGroupPrefix + "/" + QString::number(segmentNumber) + "/Connector");
		SETP("SecondBody", mGroupPrefix + "/" + QString::number(segmentNumber) + "/Body");
		SETP("AxisPoint1", Vector3DValue(offset - (mBodyLength / 2.0), 0.1, 0.0)
							.getValueAsString());
		SETP("AxisPoint2", Vector3DValue(offset - ((mBodyLength / 2.0) + mConnectorLength), -0.1, 0.0)
							.getValueAsString());
		SETP("MinAngle", "-50");
		SETP("MaxAngle", "50");

		CollisionManager *cm = Physics::getCollisionManager();

		cm->disableCollisions(previousBody->getCollisionObjects(), middlePart->getCollisionObjects(), true);
		cm->disableCollisions(middlePart->getCollisionObjects(), currentBody->getCollisionObjects(), true);
	}
	mSegmentMainBodies.append(currentBody);

	createLegs(currentBody, offset, true, segmentNumber);
	createLegs(currentBody, offset, false, segmentNumber);
}


void SimpleMultiLeggedRobot::createLegs(SimBody *body, double offset, bool left, int segmentNumber) {
	
	double dir = 1.0;
	QString prefix = "Left/";
	if(!left) {
		dir = -1.0;
		prefix = "Right/";
	}

	INIT_PARAMS;

	//***********
	//Create Bodies
	//***********

	SimBody *legStub = dynamic_cast<SimBody*>(mCapsulePrototype->createCopy());
	mAgent->addObject(legStub);
	MAKE_CURRENT(legStub);
	SETP("Name", mNamePrefix + prefix + QString::number(segmentNumber) + "/LegStub");
	SETP("Mass", "0.0001");
	SETP("Position", Vector3DValue(offset, 0, 
						dir * ((mLegStubLength / 2.0) + mHalfBodyWidth)).getValueAsString());
	SETP("CenterOfMass", "(0,0,0)");
	SETP("Orientation", "(0,0,0)");
	SETP("Dynamic", "true");
	//SETP("Width", DoubleValue(mLegThickness).getValueAsString());
	//SETP("Height", DoubleValue(mLegThickness).getValueAsString());
	//SETP("Depth", DoubleValue(mLegStubLength).getValueAsString());
	SETP("Length", DoubleValue(Math::max(0.0, mLegStubLength - mLegThickness)).getValueAsString());
	SETP("Radius", DoubleValue(mLegThickness / 2.0).getValueAsString());
	SETP("Color", "blue");
	SETP("Material", "ABS");

	SimBody *upperLeg = dynamic_cast<SimBody*>(mCapsulePrototype->createCopy());
	mAgent->addObject(upperLeg);
	MAKE_CURRENT(upperLeg);
	SETP("Name", mNamePrefix + prefix + QString::number(segmentNumber) + "/UpperLeg");
	SETP("Mass", "0.0005");
	SETP("Position", Vector3DValue(offset, 0, 
						dir * ((mUpperLegLength / 2.0) + mLegStubLength + mHalfBodyWidth))
						.getValueAsString());
	SETP("CenterOfMass", "(0,0,0)");
	SETP("Orientation", "(0,0,0)");
	SETP("Dynamic", "true");
	//SETP("Width", DoubleValue(mLegThickness).getValueAsString());
	//SETP("Height", DoubleValue(mLegThickness).getValueAsString());
	//SETP("Depth", DoubleValue(mUpperLegLength).getValueAsString());
	SETP("Length", DoubleValue(mUpperLegLength - mLegThickness).getValueAsString());
	SETP("Radius", DoubleValue(mLegThickness / 2.0).getValueAsString());
	SETP("Color", "red");
	SETP("Material", "ABS");

	SimBody *lowerLeg = dynamic_cast<SimBody*>(mCapsulePrototype->createCopy());
	mAgent->addObject(lowerLeg);
	MAKE_CURRENT(lowerLeg);
	SETP("Name", mNamePrefix + prefix + QString::number(segmentNumber) + "/LowerLeg");
	SETP("Mass", "0.0005");
	SETP("Position", Vector3DValue(offset, mLowerLegLength / -2.0, 
						dir * (mUpperLegLength + mLegStubLength + mHalfBodyWidth))
						.getValueAsString());
	SETP("CenterOfMass", "(0,0,0)");
	SETP("Orientation", "(90,0,0)");
	SETP("Dynamic", "true");
	//SETP("Width", DoubleValue(mLegThickness).getValueAsString());
	//SETP("Height", DoubleValue(mLowerLegLength).getValueAsString());
	//SETP("Depth", DoubleValue(mLegThickness).getValueAsString());
	SETP("Length", DoubleValue(mLowerLegLength - mLegThickness).getValueAsString());
	SETP("Radius", DoubleValue(mLegThickness / 2.0).getValueAsString());
	SETP("Color", "blue");
	SETP("Material", "ABS");

// 	CollisionObject *foot= new CollisionObject(
// 				SphereGeom(lowerLeg, mLegThickness, Vector3D(0.0, -mLowerLegLength / 2.0, 0.0)),
// 											lowerLeg, true);
// 	SphereGeom *footSphere = dynamic_cast<SphereGeom*>(foot->getGeometry());
// 	footSphere->setAutomaticColor(false);
// 	foot->setMass(0.0001);
// 	foot->setMaterialType("PlasticsWhite");
// // 	foot->setTextureType("Dynamixel");
// 	footSphere->setColor(150, 150, 150, 255);
// 	lowerLeg->addCollisionObject(foot);

	//***********
	//Foot Force Sensor
	//***********

	SimObject *footForceSensor = mForceSensorPrototype->createCopy();
	mAgent->addObject(footForceSensor);
	MAKE_CURRENT(footForceSensor);
	SETP("Name", mNamePrefix + prefix + QString::number(segmentNumber) + "/ForceSensor");
	SETP("HostBodyName", mGroupPrefix + "/" + prefix  + QString::number(segmentNumber) + "/LowerLeg");
	SETP("LocalSensorAxis", "(0.0,0.0,1.0)");
	SETP("CollisionObjectIndices", "0");
	SETP("MinForce", "0");
	SETP("MaxForce", "1.07");

	//***********
	//Motors
	//***********

	SimObject *lowerMotor = mMotor1DPrototype->createCopy();
	mAgent->addObject(lowerMotor);
	MAKE_CURRENT(lowerMotor);
	SETP("Name", mNamePrefix + prefix + QString::number(segmentNumber) + "/LowerMotor");
	SETP("FirstBody", mGroupPrefix + "/" + prefix + QString::number(segmentNumber) + "/UpperLeg");
	SETP("SecondBody", mGroupPrefix + "/" + prefix + QString::number(segmentNumber) + "/LowerLeg");
	SETP("AxisPoint1", Vector3DValue(-0.1 + offset, 0.0, 
						dir * (mUpperLegLength + mLegStubLength + mHalfBodyWidth))
						.getValueAsString());
	SETP("AxisPoint2", Vector3DValue(0.1 + offset, 0.0, 
						dir * (mUpperLegLength + mLegStubLength + mHalfBodyWidth))
						.getValueAsString());
	SETP("MinAngle", "-45");
	SETP("MaxAngle", "90");

	SimObject *upperMotorLift = mMotor1DPrototype->createCopy();
	mAgent->addObject(upperMotorLift);
	MAKE_CURRENT(upperMotorLift);
	SETP("Name", mNamePrefix + prefix + QString::number(segmentNumber) + "/UpperMotorLift");
	SETP("FirstBody", mGroupPrefix + body->getName());
	SETP("SecondBody", mGroupPrefix + "/" + prefix + QString::number(segmentNumber) + "/LegStub");
	SETP("AxisPoint1", Vector3DValue(0.1 + offset, 0.0, dir * mHalfBodyWidth).getValueAsString());
	SETP("AxisPoint2", Vector3DValue(-0.1 + offset, 0.0, dir * mHalfBodyWidth).getValueAsString());
	SETP("MinAngle", "-50");
	SETP("MaxAngle", "30");

	SimObject *upperMotorPush = mMotor1DPrototype->createCopy();
	mAgent->addObject(upperMotorPush);
	MAKE_CURRENT(upperMotorPush);
	SETP("Name", mNamePrefix + prefix + QString::number(segmentNumber) + "/UpperMotorPush");
	SETP("FirstBody", mGroupPrefix + "/" + prefix + QString::number(segmentNumber) + "/LegStub");
	SETP("SecondBody", mGroupPrefix + "/" + prefix + QString::number(segmentNumber) + "/UpperLeg");
	SETP("AxisPoint1", Vector3DValue(offset, 0.1, dir * (mLegStubLength + mHalfBodyWidth)).getValueAsString());
	SETP("AxisPoint2", Vector3DValue(offset, -0.1, dir * (mLegStubLength + mHalfBodyWidth)).getValueAsString());
	SETP("MinAngle", "-80");
	SETP("MaxAngle", "80");


	//***********
	//Disable collisions that are ok.
	//***********
	
	CollisionManager *cm = Physics::getCollisionManager();

	cm->disableCollisions(body->getCollisionObjects(), legStub->getCollisionObjects(), true);
	cm->disableCollisions(body->getCollisionObjects(), upperLeg->getCollisionObjects(), true);
	cm->disableCollisions(legStub->getCollisionObjects(), upperLeg->getCollisionObjects(), true);
	cm->disableCollisions(upperLeg->getCollisionObjects(), lowerLeg->getCollisionObjects(), true);
}


}



