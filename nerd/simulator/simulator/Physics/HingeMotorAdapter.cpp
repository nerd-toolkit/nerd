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


#include "Physics/HingeMotorAdapter.h"

#include <QString>
#include "Core/Core.h"
#include "Physics/HingeMotor.h"
#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include "Value/BoolValue.h"

namespace nerd { 

  HingeMotorAdapter::HingeMotorAdapter(const QString &name, 
                                       const QString &globalActiveMotorValue)
  : HingeJoint(name), SimSensor(), SimActuator(), 
	mMinAngleValue(0), 
	mMaxAngleValue(0),
	mActiveMotorName(0),
	mUseGlobalActiveMotorChanges(0),
	mGlobalActiveMotorName(0),
	mGlobalActiveMotorNameInterace(0),
	mActiveMotor(0)
{ 
	// add local parameters
	mMinAngleValue = new DoubleValue(-150.0);
	mMaxAngleValue = new DoubleValue(150.0);
	mActiveMotorName = new StringValue("");
	mUseGlobalActiveMotorChanges = new BoolValue(true);
	mGlobalActiveMotorNameInterace = new StringValue("");
	
	// add global parameter 
	ValueManager * vm = Core::getInstance()->getValueManager();
	
	if(vm->nameExists(globalActiveMotorValue)) {
		mGlobalActiveMotorName = vm->getStringValue(globalActiveMotorValue);
	}
	else {
		mGlobalActiveMotorName = new StringValue(mActiveMotorName->get());
		vm->addValue(globalActiveMotorValue, mGlobalActiveMotorName);
	}
	mGlobalActiveMotorName->addValueChangedListener(this);
	
	addParameter("MinAngle", mMinAngleValue);
	addParameter("MaxAngle", mMaxAngleValue);
	addParameter("ActiveMotor", mActiveMotorName);
	addParameter("UseGlobalActiveMotorChanges", mUseGlobalActiveMotorChanges);
	addParameter("GlobalActiveMotor", mGlobalActiveMotorNameInterace);
	
	
	if(mUseGlobalActiveMotorChanges->get() == true)
	{
	mGlobalActiveMotorNameInterace->set(mGlobalActiveMotorName->get());
	}
}


HingeMotorAdapter::HingeMotorAdapter(const HingeMotorAdapter &hingeMotorAdapter)
  : HingeJoint(hingeMotorAdapter), SimSensor(), SimActuator(),
	mMinAngleValue(0), 
	mMaxAngleValue(0),
	mActiveMotorName(0),
	mUseGlobalActiveMotorChanges(0),
	mGlobalActiveMotorName(0),
	mGlobalActiveMotorNameInterace(0),
	mActiveMotor(0)
{ 
	// local values
	mMinAngleValue = dynamic_cast<DoubleValue*>(getParameter("MinAngle"));
	mMaxAngleValue = dynamic_cast<DoubleValue*>(getParameter("MaxAngle"));
	mActiveMotorName = dynamic_cast<StringValue*>(getParameter("ActiveMotor"));
	mUseGlobalActiveMotorChanges =
		dynamic_cast<BoolValue*>(getParameter("UseGlobalActiveMotorChanges"));
	mGlobalActiveMotorNameInterace = dynamic_cast<StringValue*>(getParameter("GlobalActiveMotor"));
	
	// global values 
	mGlobalActiveMotorName = hingeMotorAdapter.mGlobalActiveMotorName;
	mGlobalActiveMotorName->addValueChangedListener(this);
	
	if(mUseGlobalActiveMotorChanges->get() == true)
	{
	mGlobalActiveMotorNameInterace->set(mGlobalActiveMotorName->get());
	}
}



void HingeMotorAdapter::valueChanged(Value *value) 
{
	SimJoint::valueChanged(value);
	
	if(value == 0){
		return;
	}
	
	// avoid that the prototyp which has no motors
	// influences the configuration of the global
	// active motor
	if(mHingeMotors.size() <= 0) {
		return;
	}
  
  //////////////////////////////////////////////////////
  // values which must be provided to all hingemotors
  //////////////////////////////////////////////////////
  
  if(value == mFirstBodyName){
    this->updateMotorParameter("FirstBody", mFirstBodyName);
    return;
  }
  if(value == mSecondBodyName){
    this->updateMotorParameter("SecondBody", mSecondBodyName);
    return;
  }
  if(value == mJointAxisPoint1){
    this->updateMotorParameter("AxisPoint1", mJointAxisPoint1);
    return;
  }
  if(value == mJointAxisPoint2){
    this->updateMotorParameter("AxisPoint2", mJointAxisPoint2);
    return;
  }
  if(value == mMinAngleValue){
    this->updateMotorParameter("MinAngle", mMinAngleValue);
    return;
  }
  if(value == mMaxAngleValue){
    this->updateMotorParameter("MaxAngle", mMaxAngleValue);
    return;
  }
  
  if(value == mNameValue)
  {
    for(int i = 0; i < mHingeMotors.values().size(); i++)
    {
      mHingeMotors.values().at(i)->getParameter("Name")->setValueFromString(
                          mNameValue->get() + "/" + mHingeMotors.values().at(i)->getMotorName());
    }
    
    return;
  }
  
  ////////////////////////////////////////////////////////////////
  // values which are responsibel to set the current active motor
  ////////////////////////////////////////////////////////////////
  
  if(value == mActiveMotorName)
  {
    // check if the user input is the error message 
    // to avoid endless loops
    if(!mActiveMotorName->get().contains("ERROR"))
    {
      // check if HingeMotor with this name exists
      // if not --> error message
      if(mHingeMotors.contains(mActiveMotorName->get()))
      { 
        mActiveMotor = mHingeMotors.value(mActiveMotorName->get());
      }
      else
      {
        mActiveMotor = 0;
                      
        mActiveMotorName->set(createMissingMotorErrorMsg());        
      }
    }
    
    // set initial value of global motor variable
    if(mGlobalActiveMotorName->get() == "")
    {
      mGlobalActiveMotorName->set(mActiveMotorName->get());
    }
    
    return;
  }
  
  // if global motor changes should be used 
  // --> set active motor to the globally used motor
  if(value == mUseGlobalActiveMotorChanges)
  {
    if(mUseGlobalActiveMotorChanges->get() == true) 
    {
      mActiveMotorName->set(mGlobalActiveMotorName->get());
      mGlobalActiveMotorNameInterace->set(mGlobalActiveMotorName->get());
    }
    else
    {
      mGlobalActiveMotorNameInterace->set("");  
    }
    
    return;
  }
  
  // propagate global active motor change via the interface to the real value
  if(value == mGlobalActiveMotorNameInterace)
  {
    if(mUseGlobalActiveMotorChanges->get() == true)
    {
      if(mGlobalActiveMotorNameInterace->get() != mGlobalActiveMotorName->get())
      {
        mGlobalActiveMotorName->set(mGlobalActiveMotorNameInterace->get());
      }
    }
    else
    {
      if(mGlobalActiveMotorNameInterace->get() != ""
         && mGlobalActiveMotorNameInterace->get().contains("Warning") == false)
      {
        mGlobalActiveMotorNameInterace->set("Warning - The value [UseGlobalActiveMotorChanges] is set to false. Thus, the input has no effect!");
      }
    }
    
    return;
  }
  
  
  if(value == mGlobalActiveMotorName)
  {
    if(mUseGlobalActiveMotorChanges->get() == true)
    {
      // check if the user input is the error message 
      // to avoid endless loops
      if(!mGlobalActiveMotorName->get().contains("ERROR"))
      {
        // check if HingeMotor with this name exists
        // if not --> error message
        if(mHingeMotors.contains(mGlobalActiveMotorName->get()))
        { 
          mActiveMotorName->set(mGlobalActiveMotorName->get());
        }
        else
        {
          mGlobalActiveMotorName->set(createMissingMotorErrorMsg());          
          
          mActiveMotor = 0;
          
          mGlobalActiveMotorNameInterace->set(mGlobalActiveMotorName->get());
          mActiveMotorName->set(mGlobalActiveMotorName->get());
        }
      }
      else
      {
        mActiveMotor = 0;
        
        mGlobalActiveMotorNameInterace->set(mGlobalActiveMotorName->get());
        mActiveMotorName->set(mGlobalActiveMotorName->get());
      }
      
      if(mGlobalActiveMotorName->get() != mGlobalActiveMotorNameInterace->get())
      {
        mGlobalActiveMotorNameInterace->set(mGlobalActiveMotorName->get());
      }
    }
    
    return;
  }
}

void HingeMotorAdapter::updateMotorParameter(const QString &parameter, Value *value)
{
  for(int i = 0; i < mHingeMotors.values().size(); i++)
  {
    mHingeMotors.values().at(i)->getParameter(parameter)->setValueFromString(
                    value->getValueAsString());
  }
}


QString HingeMotorAdapter::createMissingMotorErrorMsg()
{
  QString errorMsg = "ERROR - The specified motor type does not exist! Possible types are: ";

  for(int i = 0; i < mHingeMotors.keys().size(); i++)
  {
    if(i > 0) {
      errorMsg += ", ";
    }
    errorMsg += "\"" + mHingeMotors.keys().at(i) + "\"";
  }

  return errorMsg;
}

  
void HingeMotorAdapter::setup() 
{
  HingeJoint::setup();
  
  if(mActiveMotor != 0)
  {
    mActiveMotor->setupComponents();
  }
}

void HingeMotorAdapter::clear() 
{
  HingeJoint::clear();
  
  if(mActiveMotor != 0)
  {
    mActiveMotor->clearComponents();
  }
}

void HingeMotorAdapter::updateActuators() 
{
  if(mActiveMotor != 0)
  {
    mActiveMotor->updateComponentInput();
  }
}

void HingeMotorAdapter::updateSensorValues() 
{
  if(mActiveMotor != 0)
  {
    mActiveMotor->updateComponentOutput();
  }
}



} // namespace nerd
