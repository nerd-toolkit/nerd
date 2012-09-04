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

#include "Collision/CollisionObject.h"
#include "Core/Core.h"
#include "Math/Math.h"
#include "Physics/ActiveDistanceSensor.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Physics/SimSensor.h"
#include <iostream>

using namespace std;

namespace nerd {
	
	ActiveDistanceSensor::ActiveDistanceSensor(const QString &name)
	: DistanceSensor(name), mExternalEnergyValue(0), mExternalTemperatureValue(0), 
	mPreviousActuatorActivation(0), mNumberOfStepsWithSimilarActivation(0)
	{
		mEnergyCostPerScan = new DoubleValue(0.01);
		mEnergyCostPerScan->setDescription("The energy that is drawn from the external power source "
										   "for a single scan.");
		
		mTemperatureIncreasePerScan = new DoubleValue(0.01);
		mTemperatureIncreasePerScan->setDescription("The increase in temperature of the external termometer "
													"for a single scan.");
		
		mNameOfExternalEnergyValue = new StringValue("");
		mNameOfExternalEnergyValue->setDescription("The name of the DoubleValue that is treated as external power source.");
		
		mNameOfExternalTemperatureValue = new StringValue("");
		mNameOfExternalTemperatureValue->setDescription("The name of the DoubleValue that is treated as the "
														"external thermometer.");
		
		mRequiredNumberOfStepsWithSimilarActivation = new IntValue(5);
		mRequiredNumberOfStepsWithSimilarActivation->setDescription("The number of steps the actuator has to remain above"
																	" or below its middle activation to trigger a scan in "
																	"actuator mode 3.");
		
		mActuatorMode = new IntValue(1);
		mActuatorMode->setDescription("Sets the actuator behavior of the distance scanner:\n"
									  "0: Scanner is always on. The actuator neuron is ignored.\n"
									  "1: A single scan is performed, when the actuator neuron exceeds half of its "
									  "maximal activation following an activation below this threshold.\n"
									  "2: A single scan is performed whenever the activation of the actuator neuron "
									  "crosses the half of the maximal activation.\n"
									  "3: A single scan is performed like in 2, but the activation at each activation "
									  "region must last at least the number of time steps given with parameter "
									  "[RequriedPersistentStepsForScan].");
		
		mScanTrigger = new InterfaceValue(getName(), "TriggerScan", 0.0, 0.0, 1.0);
		mScanTrigger->setDescription("Actuator to trigger scans of the distance sensor.");
		mInputValues.append(mScanTrigger);
		
		addParameter("EnergyCostPerScan", mEnergyCostPerScan);
		addParameter("TemperatureIncreasePerScan", mTemperatureIncreasePerScan);
		addParameter("NameOfExternalEnergyValue", mNameOfExternalEnergyValue);
		addParameter("NameOfExternalTemperatureValue", mNameOfExternalTemperatureValue);
		addParameter("ActuatorMode", mActuatorMode);
		addParameter("RequiredPersistentStepsForScan", mRequiredNumberOfStepsWithSimilarActivation);
		addParameter("ScanTriggerActuator", mScanTrigger);
	}
	
	ActiveDistanceSensor::ActiveDistanceSensor(const ActiveDistanceSensor &other)
	: Object(), ValueChangedListener(), DistanceSensor(other), mExternalEnergyValue(0), 
		mExternalTemperatureValue(0), mPreviousActuatorActivation(0), mNumberOfStepsWithSimilarActivation(0)
	{
		mEnergyCostPerScan = dynamic_cast<DoubleValue*>(getParameter("EnergyCostPerScan"));
		mTemperatureIncreasePerScan = dynamic_cast<DoubleValue*>(getParameter("TemperatureIncreasePerScan"));
		mNameOfExternalEnergyValue = dynamic_cast<StringValue*>(getParameter("NameOfExternalEnergyValue"));
		mNameOfExternalTemperatureValue = dynamic_cast<StringValue*>(getParameter("NameOfExternalTemperatureValue"));
		mActuatorMode = dynamic_cast<IntValue*>(getParameter("ActuatorMode"));
		mRequiredNumberOfStepsWithSimilarActivation = dynamic_cast<IntValue*>(getParameter("RequiredPersistentStepsForScan"));
		
		mScanTrigger = dynamic_cast<InterfaceValue*>(getParameter("ScanTriggerActuator"));
		mInputValues.append(mScanTrigger);
	}
	
	
	ActiveDistanceSensor::~ActiveDistanceSensor() {
	}
	
	SimObject* ActiveDistanceSensor::createCopy() const {
		return new ActiveDistanceSensor(*this);
	}
	
	void ActiveDistanceSensor::setup() {
		DistanceSensor::setup();
		
		ValueManager *vm = Core::getInstance()->getValueManager();
		
		mExternalEnergyValue = vm->getDoubleValue(mNameOfExternalEnergyValue->get());
		mExternalTemperatureValue = vm->getDoubleValue(mNameOfExternalTemperatureValue->get());
		
		if(mExternalEnergyValue == 0 && mNameOfExternalEnergyValue->get() != "") {
			Core::log("ActiveDistanceSensor: Warning! Could not find the external energy value with name ["
					  + mNameOfExternalEnergyValue->get() + "]. Ignoring energy handling!", true);
		}
		
		if(mExternalTemperatureValue == 0 && mNameOfExternalTemperatureValue->get() != "") {
			Core::log("ActiveDistanceSensor: Warning! Could not find the external temperature value with name ["
					  + mNameOfExternalTemperatureValue->get() + "]. Ignoring temperature handling!", true);
		}
		mPreviousActuatorActivation = 0.0;
		mNumberOfStepsWithSimilarActivation = 0;
	}
	
	
	void ActiveDistanceSensor::clear() {
		DistanceSensor::clear();
	}
	
	void ActiveDistanceSensor::updateSensorValues() {
		
		if(mExternalEnergyValue != 0) {
			if(mExternalEnergyValue->get() < mEnergyCostPerScan->get()) {
				//there is not enough energy to do the scan.
				mDistance->set(mDistance->getMax());
				return;
			}
		}
		
		switch(mActuatorMode->get()) {
			case 0:
				
				DistanceSensor::updateSensorValues();
				updateExternalValues();
				
				break;
			case 1:
				
				if(mScanTrigger->get() > 0.5 && mPreviousActuatorActivation < 0.5)
				{
					DistanceSensor::updateSensorValues();
					updateExternalValues();
				}
				else {
					mDistance->set(mDistance->getMax());
				}
				mPreviousActuatorActivation = mScanTrigger->get();
				
				break;
			case 2:
				
				if((mScanTrigger->get() > 0.5 && mPreviousActuatorActivation < 0.5)
					|| (mScanTrigger->get() < 0.5 && mPreviousActuatorActivation > 0.5))
				{
					mPreviousActuatorActivation = mScanTrigger->get();
					DistanceSensor::updateSensorValues();
					updateExternalValues();
				}
				else {
					mDistance->set(mDistance->getMax());
				}
				
				break;
			case 3:
				
				if((mPreviousActuatorActivation < 0.5 && mScanTrigger->get() < 0.5)
					|| (mPreviousActuatorActivation > 0.5 && mScanTrigger->get() > 0.5))
				{
					++mNumberOfStepsWithSimilarActivation;
				}
				else {
					mNumberOfStepsWithSimilarActivation = 0;
				}
				mPreviousActuatorActivation = mScanTrigger->get();
				
				if(mNumberOfStepsWithSimilarActivation >= mRequiredNumberOfStepsWithSimilarActivation->get())
				{
					mNumberOfStepsWithSimilarActivation = 0;
					DistanceSensor::updateSensorValues();
					updateExternalValues();
				}
				else {
					mDistance->set(mDistance->getMax());
				}
				
				break;
			default:
				//no distance measurement
				mDistance->set(mDistance->getMax());
		}
		
		DistanceSensor::updateSensorValues();
	}
	
	void ActiveDistanceSensor::valueChanged(Value *value) {
		DistanceSensor::valueChanged(value);
		if(value == 0) {
			return;
		}
	}
	

	void ActiveDistanceSensor::updateExternalValues() {
		if(mExternalEnergyValue != 0) {
			mExternalEnergyValue->set(mExternalEnergyValue->get() - mEnergyCostPerScan->get());
		}
		if(mExternalTemperatureValue != 0) {
			mExternalTemperatureValue->set(mExternalTemperatureValue->get() + mTemperatureIncreasePerScan->get());
		}
	}
	
	
}

