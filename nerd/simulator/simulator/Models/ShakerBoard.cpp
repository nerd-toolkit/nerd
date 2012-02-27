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

#include "ShakerBoard.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Collision/CollisionManager.h"
#include "Physics/SimObjectGroup.h"
#include "Util/Color.h"
#include "Physics/Physics.h"
#include "SimulationConstants.h"
#include "Physics/BoxGeom.h"
#include "Collision/CollisionObject.h"
#include "Physics/SliderJoint.h"
#include "Physics/FixedJoint.h"
#include "Physics/Dynamixel.h"
#include <QDebug>
#include "Util/Tracer.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);


#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))

using namespace nerd;


ShakerBoard::ShakerBoard(const QString &groupName) : ModelInterface(groupName), mDesiredMotorSettingX(0), mDesiredMotorSettingY(0), mSliderMotorX(0), mSliderMotorZ(0), mRandomSignalX(0), mRandomSignalZ(0), mSinusSignalX(0), mSinusSignalZ(0)
{
//	mPosition->addValueChangedListener(this);
}

ShakerBoard::ShakerBoard(const ShakerBoard &model) : Object(), ValueChangedListener(), 
		EventListener(), ModelInterface(model), mDesiredMotorSettingX(0), mDesiredMotorSettingY(0), mSliderMotorX(0), mSliderMotorZ(0), mRandomSignalX(0), mRandomSignalZ(0), mSinusSignalX(0), mSinusSignalZ(0)
{
//	mPosition->addValueChangedListener(this);
}

ShakerBoard::~ShakerBoard(){
}

void ShakerBoard::createModel() {
	TRACE("ShakerBoard::createModel");
	// Get Physics Manager
	PhysicsManager *pm = Physics::getPhysicsManager();

	// Create SimObjectGroup for all SimObjects of the ShakerBoard
	SimObjectGroup *shakerBoard = mAgent;

	// Create board without motor and add it to SimObjectGroup and mSimObjects
	SimObject *boxBody = pm->getPrototype(
			SimulationConstants::PROTOTYPE_BOX_BODY); 
	SimObject *board = boxBody->createCopy();
	board->setName("Board");
	PARAM(DoubleValue, board, "Width")->set(1.0);
	PARAM(DoubleValue, board, "Height")->set(0.01);
	PARAM(DoubleValue, board, "Depth")->set(1.0);
	PARAM(Vector3DValue, board, "Position")->set(0.0, 0.0, 0.0);
	PARAM(ColorValue, board, "Color")->setValueFromString("blue");
	shakerBoard->addObject(board);
	mSimObjects.append(board);

	// Create a body between MotorX and MotorZ for connection of the motors
	SimObject *bodyMotorXMotorZ = boxBody->createCopy();
	bodyMotorXMotorZ->setName("BodyBetweenMotorXAndMotorZ");
	PARAM(DoubleValue, bodyMotorXMotorZ, "Width")->set(0.001);
	PARAM(DoubleValue, bodyMotorXMotorZ, "Height")->set(0.001);
	PARAM(DoubleValue, bodyMotorXMotorZ, "Depth")->set(0.001);
	PARAM(Vector3DValue, bodyMotorXMotorZ, "Position")->set(0.0, -0.5, 0.0);
	PARAM(ColorValue, bodyMotorXMotorZ, "Color")->setValueFromString("yellow");
	shakerBoard->addObject(bodyMotorXMotorZ);
	mSimObjects.append(bodyMotorXMotorZ);

	// Create bodies between MotorZ and ServoPitch and between ServoPitch and ServoRoll
	SimObject *bodyMotorZServoPitch = boxBody->createCopy();
	bodyMotorZServoPitch->setName("BodyBetweenMotorZAndServoPitch");
	PARAM(DoubleValue, bodyMotorZServoPitch, "Width")->set(0.001);
	PARAM(DoubleValue, bodyMotorZServoPitch, "Height")->set(0.001);
	PARAM(DoubleValue, bodyMotorZServoPitch, "Depth")->set(0.001);
	PARAM(Vector3DValue, bodyMotorZServoPitch, "Position")->set(0.0, -0.002, 0.0);
	PARAM(ColorValue, bodyMotorZServoPitch, "Color")->setValueFromString("blue");
	shakerBoard->addObject(bodyMotorZServoPitch);
	mSimObjects.append(bodyMotorZServoPitch);

	SimObject *bodyServoPitchServoRoll = boxBody->createCopy();
	bodyServoPitchServoRoll->setName("BodyBetweenServoPitchAndServoRoll");
	PARAM(DoubleValue, bodyServoPitchServoRoll, "Width")->set(0.001);
	PARAM(DoubleValue, bodyServoPitchServoRoll, "Height")->set(0.001);
	PARAM(DoubleValue, bodyServoPitchServoRoll, "Depth")->set(0.001);
	PARAM(Vector3DValue, bodyServoPitchServoRoll, "Position")->set(0.0, -0.001, 0.0);
	PARAM(ColorValue, bodyServoPitchServoRoll, "Color")->setValueFromString("blue");
	shakerBoard->addObject(bodyServoPitchServoRoll);
	mSimObjects.append(bodyServoPitchServoRoll);

	// Create motors and connect them
	SimObject *sliderMotorX = (pm->getPrototype(
			SimulationConstants::PROTOTYPE_SLIDER_MOTOR))->createCopy();
	sliderMotorX->setName("SliderMotorX");
	PARAM(Vector3DValue, sliderMotorX, "AxisPoint1")->set(0.0, 0.0, 0.0);
	PARAM(Vector3DValue, sliderMotorX, "AxisPoint2")->set(1.0, 0.0, 0.0);
	PARAM(DoubleValue, sliderMotorX, "MaxForce")->set(10000000);
	shakerBoard->addObject(sliderMotorX);
	mSimObjects.append(sliderMotorX);

	SimObject *sliderMotorZ = (pm->getPrototype(
			SimulationConstants::PROTOTYPE_SLIDER_MOTOR))->createCopy();
	sliderMotorZ->setName("SliderMotorZ");
	PARAM(Vector3DValue, sliderMotorZ, "AxisPoint1")->set(0.0, 0.0, 0.0);
	PARAM(Vector3DValue, sliderMotorZ, "AxisPoint2")->set(0.0, 0.0, 1.0);
	PARAM(DoubleValue, sliderMotorZ, "MaxForce")->set(10000000);
	shakerBoard->addObject(sliderMotorZ);
	mSimObjects.append(sliderMotorZ);

	SimObject *servoPitch = (pm->getPrototype(
			SimulationConstants::PROTOTYPE_SERVO_MOTOR))->createCopy();
	servoPitch->setName("ServoPitch");
	PARAM(Vector3DValue, servoPitch, "AxisPoint1")->set(0.0, 0.0, 0.0);
	PARAM(Vector3DValue, servoPitch, "AxisPoint2")->set(1.0, 0.0, 0.0);
	PARAM(DoubleValue, servoPitch, "MaxForce")->set(10000000);
	PARAM(DoubleValue, servoPitch, "MaxTorque")->set(1000);
	shakerBoard->addObject(servoPitch);
	mSimObjects.append(servoPitch);

	SimObject *servoRoll = (pm->getPrototype(
			SimulationConstants::PROTOTYPE_SERVO_MOTOR))->createCopy();
	servoRoll->setName("ServoRoll");
	PARAM(Vector3DValue, servoRoll, "AxisPoint1")->set(0.0, 0.0, 0.0);
	PARAM(Vector3DValue, servoRoll, "AxisPoint2")->set(0.0, 0.0, 1.0);
	PARAM(DoubleValue, servoRoll, "MaxForce")->set(10000000);
	PARAM(DoubleValue, servoRoll, "MaxTorque")->set(1000);
	shakerBoard->addObject(servoRoll);
	mSimObjects.append(servoRoll);

	PARAM(StringValue, sliderMotorX, "SecondBody")->set(bodyMotorXMotorZ->getAbsoluteName());
	PARAM(StringValue, sliderMotorZ, "FirstBody")->set(bodyMotorXMotorZ->getAbsoluteName());
	PARAM(StringValue, sliderMotorZ, "SecondBody")->set(bodyMotorZServoPitch->getAbsoluteName());
	PARAM(StringValue, servoPitch, "FirstBody")->set(bodyMotorZServoPitch->getAbsoluteName());
	PARAM(StringValue, servoPitch, "SecondBody")->set(bodyServoPitchServoRoll->getAbsoluteName());
	PARAM(StringValue, servoRoll, "FirstBody")->set(bodyServoPitchServoRoll->getAbsoluteName());
	PARAM(StringValue, servoRoll, "SecondBody")->set(board->getAbsoluteName());

	// Get motor input of Sliders
	mDesiredMotorSettingX = sliderMotorX->getInputValues().first();
	mDesiredMotorSettingY = sliderMotorZ->getInputValues().first();
	mDesiredMotorSettingPitch = servoPitch->getInputValues().first();
	mDesiredMotorSettingRoll = servoRoll->getInputValues().first();

	// Create Signals and set to disabled
	mSinusSignalX = new SignalSinus("SinusSignalX", "/Sim" + getAbsoluteName() + "/SinusSignalX/");
	PARAM(IntValue, mSinusSignalX, "IdleSteps")->set(100);
	PARAM(DoubleValue, mSinusSignalX, "Amplitude")->set(0.3);
	PARAM(BoolValue, mSinusSignalX, "Enabled")->set(false);

	mSinusSignalZ = new SignalSinus("SinusSignalZ", "/Sim" + getAbsoluteName() + "/SinusSignalZ/");
	PARAM(IntValue, mSinusSignalZ, "IdleSteps")->set(100);
	PARAM(DoubleValue, mSinusSignalZ, "Amplitude")->set(0.3);
	PARAM(BoolValue, mSinusSignalZ, "Enabled")->set(false);

	mRandomSignalX = new SignalRandom("RandomSignalX", "/Sim" + getAbsoluteName() + "/RandomSignalX/");
	PARAM(IntValue, mRandomSignalX, "IdleSteps")->set(100);
	PARAM(DoubleValue, mRandomSignalX, "Amplitude")->set(0.3);
	PARAM(IntValue, mRandomSignalX, "Seed")->set(10);
	PARAM(BoolValue, mRandomSignalX, "Enabled")->set(false);

	mRandomSignalZ = new SignalRandom("RandomSignalZ", "/Sim" + getAbsoluteName() + "/RandomSignalZ/");
	PARAM(IntValue, mRandomSignalZ, "IdleSteps")->set(100);
	PARAM(DoubleValue, mRandomSignalZ, "Amplitude")->set(0.3);
	PARAM(IntValue, mRandomSignalZ, "Seed")->set(11);
	PARAM(BoolValue, mRandomSignalZ, "Enabled")->set(false);

	mSinusSignalPitch = new SignalSinus("SinusSignalPitch", "/Sim" + getAbsoluteName() + "/SinusSignalPitch/");
	PARAM(IntValue, mSinusSignalPitch, "IdleSteps")->set(100);
	PARAM(DoubleValue, mSinusSignalPitch, "Amplitude")->set(0.3);
	PARAM(BoolValue, mSinusSignalPitch, "Enabled")->set(false);

	mSinusSignalRoll = new SignalSinus("SinusSignalRoll", "/Sim" + getAbsoluteName() + "/SinusSignalRoll/");
	PARAM(IntValue, mSinusSignalRoll, "IdleSteps")->set(100);
	PARAM(DoubleValue, mSinusSignalRoll, "Amplitude")->set(0.3);
	PARAM(BoolValue, mSinusSignalRoll, "Enabled")->set(false);

	mRandomSignalPitch = new SignalRandom("RandomSignalPitch", "/Sim" + getAbsoluteName() + "/RandomSignalPitch/");
	PARAM(IntValue, mRandomSignalPitch, "IdleSteps")->set(100);
	PARAM(DoubleValue, mRandomSignalPitch, "Amplitude")->set(0.3);
	PARAM(IntValue, mRandomSignalPitch, "Seed")->set(10);
	PARAM(BoolValue, mRandomSignalPitch, "Enabled")->set(false);

	mRandomSignalRoll = new SignalRandom("RandomSignalRoll", "/Sim" + getAbsoluteName() + "/RandomSignalRoll/");
	PARAM(IntValue, mRandomSignalRoll, "IdleSteps")->set(100);
	PARAM(DoubleValue, mRandomSignalRoll, "Amplitude")->set(0.3);
	PARAM(IntValue, mRandomSignalRoll, "Seed")->set(11);
	PARAM(BoolValue, mRandomSignalRoll, "Enabled")->set(false);

	// Connect signals with motor inputs
	mSinusSignalX->addDestinationValue(mDesiredMotorSettingX);
	mSinusSignalZ->addDestinationValue(mDesiredMotorSettingY);
	mRandomSignalX->addDestinationValue(mDesiredMotorSettingX);
	mRandomSignalZ->addDestinationValue(mDesiredMotorSettingY);
	mSinusSignalPitch->addDestinationValue(mDesiredMotorSettingPitch);
	mSinusSignalRoll->addDestinationValue(mDesiredMotorSettingRoll);
	mRandomSignalPitch->addDestinationValue(mDesiredMotorSettingPitch);
	mRandomSignalRoll->addDestinationValue(mDesiredMotorSettingRoll);

	// Add SimObjectGroup to physics
	pm->addSimObjectGroup(shakerBoard);
}

SimObject* ShakerBoard::createCopy() const {
	return new ShakerBoard("Default");
}
