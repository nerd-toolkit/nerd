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


#include "TorqueDynamixelMotorAdapter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "MotorModels/MSeriesTorqueSpringMotorModel.h"
#include "Physics/Physics.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new TorqueDynamixelMotorAdapter.
 */
TorqueDynamixelMotorAdapter::TorqueDynamixelMotorAdapter(
									const QString &name, int numberOfTorqueInputValues, bool useJointAngleSensor,
									bool useMotorAngleSensors, bool useFreerunInputValues,
									bool useCurrentConsumptionSensors, bool useIndividualMotorTorqueValues)
	: HingeJointMotorAdapter(name), 
		mNumberOfTorqueInputValues(numberOfTorqueInputValues), mUseJointAngleSensor(useJointAngleSensor), mJointAngleSensorValue(0),
		mUseMotorAngleSensors(useMotorAngleSensors), mUseFreerunInputValues(useFreerunInputValues), 
		mUseCurrentConsumptionSensors(useCurrentConsumptionSensors),
		mUseIndiviualMotorTorqueValues(useIndividualMotorTorqueValues)
{
	// Create Joint Angle Sensor and add it to the OutputValues if used
	if(mUseJointAngleSensor) {
		mJointAngleSensorValue = new InterfaceValue(getName(), "JointAngle", 0.0, -170.0, 170.0);
		addParameter("JointAngle", mJointAngleSensorValue);
		mOutputValues.append(mJointAngleSensorValue);

		mJointAngleSensorOffset = new DoubleValue(0.0);
		addParameter("JointAngleOffset", mJointAngleSensorOffset);
	}

	// Create additional input and output values and parameters that are used for each participating motor
	for(int i = 0; i < mNumberOfTorqueInputValues; ++i) {
		// Create Torque Values
		InterfaceValue *torqueValue = new InterfaceValue(getName(), "DesiredTorque" + QString::number(i),
			0.0, -1.0, 1.0);
		mInputValues.append(torqueValue);
		mTorqueInputValues.append(torqueValue);
		addParameter("DesiredTorque" + QString::number(i), torqueValue);

		// Create Motor Angle Sensor Values
		if(mUseMotorAngleSensors) {
			InterfaceValue *motorAngleSensorValue = new InterfaceValue(getName(), "MotorAngle" + QString::number(i),
				0.0, -150.0, +150.0);
			mOutputValues.append(motorAngleSensorValue);
			mMotorAngleSensorValues.append(motorAngleSensorValue);
			addParameter("MotorAngle" + QString::number(i), motorAngleSensorValue);
		}

		// Create Freerun Input Values
		if(mUseFreerunInputValues) {
			InterfaceValue *freerunValue = new InterfaceValue(getName(), "ReleaseMode" + QString::number(i),
				0.0, -1.0, 1.0);
			mInputValues.append(freerunValue);
			mFreerunInputValues.append(freerunValue);
			addParameter("ReleaseMode" + QString::number(i), freerunValue);
		}

		if(mUseIndiviualMotorTorqueValues) {
			DoubleValue *indTorqueValue = new DoubleValue();
			mIndividualMotorTorqueValues.append(indTorqueValue);
			addParameter("IndividualMotorTorque" + QString::number(i), indTorqueValue);
		}

		// Create Current Consumption Values
		if(mUseCurrentConsumptionSensors) {
			DoubleValue *currentConsumptionValue = new DoubleValue(0.0);
			mCurrentConsumptionValues.append(currentConsumptionValue);
			addParameter("CurrentConsumption" + QString::number(i), currentConsumptionValue);
		}

		//Add flip state value (to indicate whether a motor is flipped.
		BoolValue *flipStateValue = new BoolValue(false);
		mMotorFlipStateValues.append(flipStateValue);
		addParameter("MotorFlipped" + QString::number(i), flipStateValue);
	}
}


/**
 * Copy constructor. 
 * 
 * @param other the TorqueDynamixelMotorAdapter object to copy.
 */
TorqueDynamixelMotorAdapter::TorqueDynamixelMotorAdapter(const TorqueDynamixelMotorAdapter &other)
	: SimSensor(other), SimActuator(other), Object(other), ValueChangedListener(other), HingeJointMotorAdapter(other),
		mNumberOfTorqueInputValues(other.mNumberOfTorqueInputValues), mUseJointAngleSensor(other.mUseJointAngleSensor), mJointAngleSensorValue(0),
		mUseMotorAngleSensors(other.mUseMotorAngleSensors), mUseFreerunInputValues(other.mUseFreerunInputValues),
		mUseCurrentConsumptionSensors(other.mUseCurrentConsumptionSensors),
		mUseIndiviualMotorTorqueValues(other.mUseIndiviualMotorTorqueValues)
{
	// Get Joint Angle Sensor and add it to the OutputValues if used
	if(mUseJointAngleSensor) {
		mJointAngleSensorValue = dynamic_cast<InterfaceValue*>(getParameter("JointAngle"));
		mOutputValues.append(mJointAngleSensorValue);

		mJointAngleSensorOffset = dynamic_cast<DoubleValue*>(getParameter("JointAngleOffset"));
	}

	// Get additional input and output values and parameters that are used for each participating motor
	for(int i = 0; i < mNumberOfTorqueInputValues; ++i) {
		// Get Torque Values
		InterfaceValue *torqueValue = dynamic_cast<InterfaceValue*>(getParameter("DesiredTorque" + QString::number(i)));
		mTorqueInputValues.append(torqueValue);
		mInputValues.append(torqueValue);

		// Get Motor Angle Sensor Values
		if(mUseMotorAngleSensors) {
			InterfaceValue *motorAngleSensorValue = dynamic_cast<InterfaceValue*>(getParameter("MotorAngle" + QString::number(i)));
			mMotorAngleSensorValues.append(motorAngleSensorValue);
			mOutputValues.append(motorAngleSensorValue);
		}

		// Get Freerun Input Values
		if(mUseFreerunInputValues) {
			InterfaceValue *freerunValue = dynamic_cast<InterfaceValue*>(getParameter("ReleaseMode" + QString::number(i)));
			mFreerunInputValues.append(freerunValue);
			mInputValues.append(freerunValue);
		}

		if(mUseIndiviualMotorTorqueValues) {
			DoubleValue *indTorqueValue = dynamic_cast<DoubleValue*>(getParameter("IndividualMotorTorque" + QString::number(i)));
			mIndividualMotorTorqueValues.append(indTorqueValue);
		}

		// Get Current Consumption Parameters
		if(mUseCurrentConsumptionSensors) {
			DoubleValue *currentConsumptionValue = dynamic_cast<DoubleValue*>(getParameter("CurrentConsumption" + QString::number(i)));
			mCurrentConsumptionValues.append(currentConsumptionValue);
		}

		// Get flipped state value
		BoolValue *flippedStateValue = dynamic_cast<BoolValue*>(getParameter("MotorFlipped" + QString::number(i)));
		mMotorFlipStateValues.append(flippedStateValue);
	}
}

/**
 * Destructor.
 */
TorqueDynamixelMotorAdapter::~TorqueDynamixelMotorAdapter() {
}

SimObject* TorqueDynamixelMotorAdapter::createCopy() const {
	TorqueDynamixelMotorAdapter* ret = new TorqueDynamixelMotorAdapter(*this);
	ret->collectCompatibleMotorModels();
	return ret;
}

int TorqueDynamixelMotorAdapter::getNumberOfTorqueInputValues() const {
	return mNumberOfTorqueInputValues;
}

bool TorqueDynamixelMotorAdapter::isUsingJointAngleSensor() const {
	return mUseJointAngleSensor;
}

bool TorqueDynamixelMotorAdapter::isUsingMotorAngleSensor() const {
	return mUseMotorAngleSensors;
}

bool TorqueDynamixelMotorAdapter::isUsingFreerunInputValues() const {
	return mUseFreerunInputValues;
}

bool TorqueDynamixelMotorAdapter::isUsingIndividualMotorTorqueValues() const {
	return mUseIndiviualMotorTorqueValues;
}

bool TorqueDynamixelMotorAdapter::isUsingCurrentConsumptionSensors() const {
	return mUseCurrentConsumptionSensors;
}

void TorqueDynamixelMotorAdapter::valueChanged(Value *value) {
	HingeJointMotorAdapter::valueChanged(value);
	if(value == 0) {
		return;
	}
}

QList<InterfaceValue*> TorqueDynamixelMotorAdapter::getTorqueInputValues() const {
	return mTorqueInputValues;
}

InterfaceValue* TorqueDynamixelMotorAdapter::getJointAngleSensorValue() const {
	return mJointAngleSensorValue;
}

QList<InterfaceValue*> TorqueDynamixelMotorAdapter::getMotorAngleSensorValues() const {
	return mMotorAngleSensorValues;
}

QList<InterfaceValue*> TorqueDynamixelMotorAdapter::getFreerunInputValues() const {
	return mFreerunInputValues;
}

QList<DoubleValue*> TorqueDynamixelMotorAdapter::getIndividualMotorTorqueValues() const {
	return mIndividualMotorTorqueValues;
}

QList<DoubleValue*> TorqueDynamixelMotorAdapter::getCurrentConsumptionValues() const {
	return mCurrentConsumptionValues;
}

QList<BoolValue*> TorqueDynamixelMotorAdapter::getMotorFlipStateValues() const {
	return mMotorFlipStateValues;
}

DoubleValue* TorqueDynamixelMotorAdapter::getJointAngleOffsetValue() const {
	return mJointAngleSensorOffset;
}

bool TorqueDynamixelMotorAdapter::isValidMotorModel(MotorModel *model) const {
	if((dynamic_cast<MSeriesTorqueSpringMotorModel*>(model)) != 0) {
		return true;
	}
	return false;
}

}



