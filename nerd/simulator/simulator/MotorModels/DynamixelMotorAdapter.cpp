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


#include "MotorModels/DynamixelMotorAdapter.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "MotorModels/DynamixelMotor.h"
#include "Value/InterfaceValue.h"
#include "Value/StringValue.h"

#include <QList>


namespace nerd {

DynamixelMotorAdapter::DynamixelMotorAdapter(const QString &name, 
										const QString &globalActiveMotorValue)
	: HingeMotorAdapter(name, globalActiveMotorValue), mDesiredMotorTorqueValue(0), 
		mDesiredMotorAngleValue(0), mAngleValue(0)
{
	mDesiredMotorTorqueValue = new InterfaceValue(getName(), "DesiredTorque", 
		0.586810005, 0.0, 1.0);
	mDesiredMotorTorqueValue->setNormalizedMin(0.0);
	mDesiredMotorTorqueValue->setNormalizedMax(1.0);
	
	mDesiredMotorAngleValue = new InterfaceValue(getName(), "DesiredAngle", 0, -150.0, 150.0);
	mDesiredMotorAngleValue->setNormalizedMin(-1.0);
	mDesiredMotorAngleValue->setNormalizedMax(1.0);
	
	mAngleValue = new InterfaceValue(getName(), "JointAngle", 0.0, -150.0, 150.0);
	mAngleValue->setNormalizedMin(-1.0);
	mAngleValue->setNormalizedMax(1.0);
	
	// add values to input and output values
	// the order is important to make it possible that the
	// keyframeplayer can control the motors
	mInputValues.append(mDesiredMotorAngleValue);
	mInputValues.append(mDesiredMotorTorqueValue);
	mOutputValues.append(mAngleValue);
	
	// add values as parameters
	addParameter("DesiredAngle", mDesiredMotorAngleValue);
	addParameter("DesiredTorque", mDesiredMotorTorqueValue);
	addParameter("JointAngle", mAngleValue);
}

DynamixelMotorAdapter::DynamixelMotorAdapter(const DynamixelMotorAdapter &dynamixelMotorAdapter)
  : HingeMotorAdapter(dynamixelMotorAdapter), 
    mDesiredMotorTorqueValue(0), 
    mDesiredMotorAngleValue(0), 
    mAngleValue(0)
{
	mDesiredMotorTorqueValue = dynamic_cast<InterfaceValue*>(getParameter("DesiredTorque"));
	mDesiredMotorAngleValue = dynamic_cast<InterfaceValue*>(getParameter("DesiredAngle")); 
	mAngleValue = dynamic_cast<InterfaceValue*>(getParameter("JointAngle"));
	
		// add values to input and output values
	// the order is important to make it possible that the
	// keyframeplayer can control the motors
	mInputValues.append(mDesiredMotorAngleValue);
	mInputValues.append(mDesiredMotorTorqueValue);
	mOutputValues.append(mAngleValue);
	
	// get list of motors if they exist already
	for(int i = 0; i < mChildObjects.size(); i++)
	{
		DynamixelMotor *curDynamixelPrototype = 
			dynamic_cast<DynamixelMotor*>(mChildObjects.at(i));
		
		if(curDynamixelPrototype != 0) {
			mHingeMotors.insert(curDynamixelPrototype->getMotorName(), curDynamixelPrototype); 
		}
	}
	
	if(mHingeMotors.size() <= 0) {
		addDynamixelModels();
	}
	
	if(mActiveMotorName->get() == "")
	{
		// set standard hinge motor
		if(mHingeMotors.size() > 0){
			mActiveMotorName->set(mHingeMotors.keys().at(0));
		} 
	}
	else
	{
		// reset mActiveMotorName to set intern the active motor
		mActiveMotorName->set(mActiveMotorName->get());
	}
}


SimObject* DynamixelMotorAdapter::createCopy() const 
{
	return new DynamixelMotorAdapter(*this);
}
  
void DynamixelMotorAdapter::addDynamixelModels() 
{
	// prepare list with input and output values
	QList<InterfaceValue*> inputValues;
	inputValues.append(mDesiredMotorTorqueValue);
	inputValues.append(mDesiredMotorAngleValue);
	
	QList<InterfaceValue*> outputValues;
	outputValues.append(mAngleValue);
	
	
	// search all dynamixel motor prototypes
	// and create a copy of them
	PhysicsManager *pm = Physics::getPhysicsManager();
	
	QList<SimObject*> prototypes = pm->getPrototypes();
	
	for(int i = 0; i < prototypes.size(); i++)
	{
		DynamixelMotor *curDynamixelPrototype = 
			dynamic_cast<DynamixelMotor*>(prototypes.at(i));
		
		if(curDynamixelPrototype != 0)
		{
		DynamixelMotor *curDynamixel = (DynamixelMotor*)curDynamixelPrototype->createCopy();
		
		curDynamixel->setInputValues(inputValues);
		curDynamixel->setOutputValues(outputValues);      
		
		mHingeMotors.insert(curDynamixel->getMotorName(), curDynamixel);
		
		// add motor to list of child objects 
		addChildObject(curDynamixel);
		}
	}
}


} // namespace nerd
