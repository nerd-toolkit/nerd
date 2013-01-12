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



#include "CurrentConsumptionSensor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QStringList>
#include "Value/ValueManager.h"
#include "Core/Core.h"
#include "Math/Math.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new CurrentConsumptionSensor.
 */
CurrentConsumptionSensor::CurrentConsumptionSensor(const QString &name)
	: SimObject(name)
{
	mMinCurrentValue = new DoubleValue(0.0);
	mMaxCurrentValue = new DoubleValue(100.0);
	mMotorTorqueValueNameList = new StringValue("");
	mNoiseValue = new DoubleValue(0.0);
	addParameter("Min", mMinCurrentValue);
	addParameter("Max", mMaxCurrentValue);
	addParameter("TorqueValueList", mMotorTorqueValueNameList);
	addParameter("Noise", mNoiseValue);

	mSensorValue = new InterfaceValue(getName(), "CurrentConsumption");
	mSensorValue->setMin(mMinCurrentValue->get());
	mSensorValue->setMax(mMaxCurrentValue->get());
	addParameter("Current", mSensorValue);
	
	mOutputValues.append(mSensorValue);
}


/**
 * Copy constructor. 
 * 
 * @param other the CurrentConsumptionSensor object to copy.
 */
CurrentConsumptionSensor::CurrentConsumptionSensor(const CurrentConsumptionSensor &other) 
	: Object(), ValueChangedListener(), SimObject(other)
{
	mMinCurrentValue = dynamic_cast<DoubleValue*>(getParameter("Min"));
	mMaxCurrentValue = dynamic_cast<DoubleValue*>(getParameter("Max"));
	mMotorTorqueValueNameList = dynamic_cast<StringValue*>(getParameter("TorqueValueList"));
	mNoiseValue = dynamic_cast<DoubleValue*>(getParameter("Noise"));
	mSensorValue = dynamic_cast<InterfaceValue*>(getParameter("Current"));

	mOutputValues.push_back(mSensorValue);
}

/**
 * Destructor.
 */
CurrentConsumptionSensor::~CurrentConsumptionSensor() {
}

SimObject* CurrentConsumptionSensor::createCopy() const {
	return new CurrentConsumptionSensor(*this);
}



QString CurrentConsumptionSensor::getName() const {
	return SimObject::getName();
}


void CurrentConsumptionSensor::valueChanged(Value *value) {
	SimObject::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mMinCurrentValue || value == mMaxCurrentValue) {
		mSensorValue->setMin(mMinCurrentValue->get());
		mSensorValue->setMax(mMaxCurrentValue->get());
	}
	else if(value == mMotorTorqueValueNameList) {
		getMotorTorqueValues();
	}
}

void CurrentConsumptionSensor::updateSensorValues() {
	//TODO add a realistic current consuption model based on the motor torques.

	double sum = 0.0;
	
	for(QListIterator<DoubleValue*> i(mMotorTorqueValues); i.hasNext();) {
		sum += i.next()->get();
	}

	//add noise
	sum = Math::calculateGaussian(sum, mNoiseValue->get());

	mSensorValue->set(sum);
}



void CurrentConsumptionSensor::setup() {
	getMotorTorqueValues();
}


void CurrentConsumptionSensor::clear() {


}

void CurrentConsumptionSensor::getMotorTorqueValues() {
	mMotorTorqueValues.clear();
	QStringList valueNames = mMotorTorqueValueNameList->get().split(",");
	ValueManager *vm = Core::getInstance()->getValueManager();
	for(QListIterator<QString> i(valueNames); i.hasNext();) {

		DoubleValue *v = vm->getDoubleValue(i.next().trimmed());
		if(v != 0) {
			mMotorTorqueValues.append(v);
		}
	}
}




}



