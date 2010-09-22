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



#include "CantileverArm.h"
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


#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))

#define INIT_PARAMS SimObject *__paraObj = 0;

#define MAKE_CURRENT(paraObj) __paraObj = paraObj;

#define SETP(paraName, value) \
		{ if(__paraObj == 0) { Core::log("No Current Param found", true); } \
		else { Value *__v = __paraObj->getParameter(paraName); \
			if(__v == 0) { Core::log("Param Problem", true); } \
			else { __v->setValueFromString(value); }}}

using namespace std;

namespace nerd {


/**
 * Constructs a new CantileverArm.
 */
CantileverArm::CantileverArm(const QString &groupName)
	: ModelInterface(groupName), mGroupName(groupName)
{
	mBoomLength = new DoubleValue(3);
	mBoomOffset = new DoubleValue(0.5);
	mCantileverMass = new DoubleValue(0.1);
	mCOG_Offset = new DoubleValue(0.6);
	mTargetObjectName = new StringValue("");

	addParameter("Length", mBoomLength);
	addParameter("BoomOffset", mBoomOffset);
	addParameter("Mass", mCantileverMass);
	addParameter("COG_Offset", mCOG_Offset);
	addParameter("TargetBody", mTargetObjectName);
}


/**
 * Copy constructor. 
 * 
 * @param other the CantileverArm object to copy.
 */
CantileverArm::CantileverArm(const CantileverArm &other) 
	: Object(), ValueChangedListener(), EventListener(), ModelInterface(other), mGroupName(other.mGroupName)
{
	mBoomLength = dynamic_cast<DoubleValue*>(getParameter("Length"));
	mBoomOffset = dynamic_cast<DoubleValue*>(getParameter("BoomOffset"));
	mCantileverMass = dynamic_cast<DoubleValue*>(getParameter("Mass"));
	mCOG_Offset = dynamic_cast<DoubleValue*>(getParameter("COG_Offset"));
	mTargetObjectName = dynamic_cast<StringValue*>(getParameter("TargetBody"));
}

/**
 * Destructor.
 */
CantileverArm::~CantileverArm() {
}

SimObject* CantileverArm::createCopy() const {
	return new CantileverArm(*this);
}


void CantileverArm::createModel() {

	PhysicsManager *pm = Physics::getPhysicsManager();

	SimObject *boxBodyPrototype = pm->getPrototype(SimulationConstants::PROTOTYPE_BOX_BODY); 
	//SimObject *motor1DPrototype = pm->getPrototype(SimulationConstants::PROTOTYPE_DYNAMIXEL);
	SimObject *fixedJointPrototype =  pm->getPrototype(SimulationConstants::PROTOTYPE_FIXED_JOINT);
	SimObject *hingeJointPrototype = pm->getPrototype(SimulationConstants::PROTOTYPE_HINGE_JOINT);

	mNamePrefix = "/";
	mGroupPrefix = QString("/").append(mGroupName).append("/");

	mAgent = new SimObjectGroup(mGroupName, "Agent");


	double length = mBoomLength->get();
	double xArmPos = -1 * mBoomOffset->get();
	double mass = mCantileverMass->get();
	double cog_offset = mCOG_Offset->get();
	double armDiameter = 0.05;

	INIT_PARAMS;
	
	SimBody *base = dynamic_cast<SimBody*>(boxBodyPrototype->createCopy());
	mAgent->addObject(base);
	MAKE_CURRENT(base);
	SETP("Name", mNamePrefix + "Base");
	SETP("Mass", DoubleValue(mass).getValueAsString());
	SETP("Position", "(0,0.1,0.0)");
	SETP("CenterOfMass", "(0,0,0)");
	SETP("Orientation", "(0,0,0)");
	SETP("Dynamic", "true");
	SETP("Width", "0.1");
	SETP("Height", "0.2");
	SETP("Depth",  "0.1");
	SETP("Color", "red");
	SETP("Material", "ABS");

	SimObject *baseHinge = hingeJointPrototype->createCopy();
	mAgent->addObject(baseHinge);
	MAKE_CURRENT(baseHinge);
	SETP("Name", mNamePrefix + "BaseHinge");
	SETP("FirstBody", mGroupPrefix + "Base");
	SETP("SecondBody", "");
	SETP("AxisPoint1", Vector3DValue(0.0, 0.1, 0.0).getValueAsString());
	SETP("AxisPoint2", Vector3DValue(0.0, -0.1, 0.0).getValueAsString());

	SimBody *arm = dynamic_cast<SimBody*>(boxBodyPrototype->createCopy());
	mAgent->addObject(arm);
	MAKE_CURRENT(arm);
	SETP("Name", mNamePrefix + "Boom");
	SETP("Mass", DoubleValue(mass).getValueAsString());
	SETP("Position", Vector3DValue(xArmPos, 0.2 + (armDiameter / 2.0), 0.0).getValueAsString());
	SETP("CenterOfMass", Vector3DValue(cog_offset, 0.0, 0.0).getValueAsString());
	SETP("Orientation", "(0,0,0)");
	SETP("Dynamic", "true");
	SETP("Width", DoubleValue(length).getValueAsString());
	SETP("Height", DoubleValue(armDiameter).getValueAsString());
	SETP("Depth",  DoubleValue(armDiameter).getValueAsString());
	SETP("Color", "red");
	SETP("Material", "ABS");

	SimObject *armHinge = hingeJointPrototype->createCopy();
	mAgent->addObject(armHinge);
	MAKE_CURRENT(armHinge);
	SETP("Name", mNamePrefix + "BoomHinge");
	SETP("FirstBody", mGroupPrefix + "Boom");
	SETP("SecondBody", mGroupPrefix + "Base");
	SETP("AxisPoint1", Vector3DValue(0.0, 0.2, 0.1).getValueAsString());
	SETP("AxisPoint2", Vector3DValue(0.0, 0.2, -0.1).getValueAsString());

	SimObject *targetFixation = fixedJointPrototype->createCopy();
	mAgent->addObject(targetFixation);
	MAKE_CURRENT(targetFixation);
	SETP("Name", mNamePrefix + "TargetFixation");

	QString firstBodyName = "";
	if(mTargetObjectName->get() != "") {
		firstBodyName = mGroupPrefix + "Boom";
	}
	SETP("FirstBody", firstBodyName);
	SETP("SecondBody", mTargetObjectName->get());
	

	CollisionManager *collisionManager = Physics::getCollisionManager();
	collisionManager->disableCollisions(base->getCollisionObjects(), arm->getCollisionObjects(), true);

	mSimObjects = mAgent->getObjects();
	pm->addSimObjectGroup(mAgent);
}


void CantileverArm::layoutObjects() {

}


}



