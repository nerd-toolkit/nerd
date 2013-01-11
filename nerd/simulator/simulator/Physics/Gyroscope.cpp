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



#include "Gyroscope.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "Math/Random.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new Gyroscope.
 */
Gyroscope::Gyroscope(const QString &name)
	: SimSensor(), SimObject(name), mHostBody(0), mHostBodyOrientation(0)
{
	mRotation1stAxis = new InterfaceValue(getName(), "Axis1", 0.0, -180.0, 180.0);
	mRotation2ndAxis = new InterfaceValue(getName(), "Axis2", 0.0, -180.0, 180.0);
	mRotation3rdAxis = new InterfaceValue(getName(), "Axis3", 0.0, -180.0, 180.0);
	mDrift = new DoubleValue(0.00001);
	mOutputRateOfChange = new BoolValue(false);
	mHostBodyName = new StringValue();
	mMaxSensorOutput = new DoubleValue(30.0);
	mRandomizeInitialOffsets = new BoolValue(false);
	mEliminateSignChange = new BoolValue(false);

	addParameter("Axis1", mRotation1stAxis);
	addParameter("Axis2", mRotation2ndAxis);
	addParameter("Axis3", mRotation3rdAxis);
	addParameter("Drift", mDrift);
	addParameter("OutputIsRateOfChange", mOutputRateOfChange);
	addParameter("HostBody", mHostBodyName);
	addParameter("MaxSensorOutput", mMaxSensorOutput);
	addParameter("RandomizeInitOffsets", mRandomizeInitialOffsets);
	addParameter("EliminateSignChanges", mEliminateSignChange);

	mOutputValues.append(mRotation1stAxis);
	mOutputValues.append(mRotation2ndAxis);
	mOutputValues.append(mRotation3rdAxis);
}


/**
 * Copy constructor. 
 * 
 * @param other the Gyroscope object to copy.
 */
Gyroscope::Gyroscope(const Gyroscope &other) 
	: SimSensor(other), Object(), ValueChangedListener(), SimObject(other), mHostBody(0),
	  mHostBodyOrientation(0)
{
	mRotation1stAxis = dynamic_cast<InterfaceValue*>(getParameter("Axis1"));
	mRotation2ndAxis = dynamic_cast<InterfaceValue*>(getParameter("Axis2"));
	mRotation3rdAxis = dynamic_cast<InterfaceValue*>(getParameter("Axis3"));
	mDrift = dynamic_cast<DoubleValue*>(getParameter("Drift"));
	mOutputRateOfChange = dynamic_cast<BoolValue*>(getParameter("OutputIsRateOfChange"));
	mHostBodyName = dynamic_cast<StringValue*>(getParameter("HostBody"));
	mMaxSensorOutput = dynamic_cast<DoubleValue*>(getParameter("MaxSensorOutput"));
	mRandomizeInitialOffsets = dynamic_cast<BoolValue*>(getParameter("RandomizeInitOffsets"));
	mEliminateSignChange = dynamic_cast<BoolValue*>(getParameter("EliminateSignChanges"));

	mOutputValues.append(mRotation1stAxis);
	mOutputValues.append(mRotation2ndAxis);
	mOutputValues.append(mRotation3rdAxis);
}


/**
 * Destructor.
 */
Gyroscope::~Gyroscope() {
}


SimObject* Gyroscope::createCopy() const {
	return new Gyroscope(*this);
}


void Gyroscope::setup() {

	mHostBodyOrientation = 0;
	mHostBody = Physics::getPhysicsManager()->getSimBody(mHostBodyName->get());
	if(mHostBody == 0) {
		Core::log("Gyroscope: Sensor has no valid reference body. Name was [" 
					+ mHostBodyName->get() + "].Ignoring.", true);
		return;
	}
	else {
		mHostBodyOrientation = dynamic_cast<Vector3DValue*>(mHostBody->getParameter("Orientation"));
	}
	mPreviousBodyAngles = Vector3D(0.0, 0.0, 0.0);
	mPreviousSensorAngles = Vector3D(0.0, 0.0, 0.0);
	if(mRandomizeInitialOffsets->get()) {
		mCurrentOffsets = Vector3D(Random::nextDouble() * 360.0, 
									Random::nextDouble() * 360.0,
									Random::nextDouble() * 360.0);
	}
	else {
		mCurrentOffsets = Vector3D(0.0, 0.0, 0.0); //TODO
	}
}


void Gyroscope::clear() {
	mHostBodyOrientation = 0;
	mHostBody = 0;
}

void Gyroscope::valueChanged(Value *value) {
	SimObject::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mMaxSensorOutput) {
		mRotation1stAxis->setMax(mMaxSensorOutput->get());
		mRotation2ndAxis->setMax(mMaxSensorOutput->get());
		mRotation3rdAxis->setMax(mMaxSensorOutput->get());
		mRotation1stAxis->setMin(-mMaxSensorOutput->get());
		mRotation2ndAxis->setMin(-mMaxSensorOutput->get());
		mRotation3rdAxis->setMin(-mMaxSensorOutput->get());
	}
}

void Gyroscope::updateSensorValues() {
	
	if(mHostBody == 0 || mHostBodyOrientation == 0) {
		return;
	}
	Vector3D currentHostBodyOrientation = mHostBodyOrientation->get();
	//Core::log("Ori: " + QString::number(currentHostBodyOrientation.getX()), true);
	
	if(!mOutputRateOfChange->get()) {
		
		while(currentHostBodyOrientation.getX() <= -180.0) {
			currentHostBodyOrientation.setX(currentHostBodyOrientation.getX() + 360.0);
		}
		while(currentHostBodyOrientation.getY() <= -180.0) {
			currentHostBodyOrientation.setY(currentHostBodyOrientation.getY() + 360.0);
		}
		while(currentHostBodyOrientation.getZ() <= -180.0) {
			currentHostBodyOrientation.setZ(currentHostBodyOrientation.getZ() + 360.0);
		}
		while(currentHostBodyOrientation.getX() > 180.0) {
			currentHostBodyOrientation.setX(currentHostBodyOrientation.getX() - 360.0);
		}
		while(currentHostBodyOrientation.getY() > 180.0) {
			currentHostBodyOrientation.setY(currentHostBodyOrientation.getY() - 360.0);
		}
		while(currentHostBodyOrientation.getZ() > 180.0) {
			currentHostBodyOrientation.setZ(currentHostBodyOrientation.getZ() - 360.0);
		}
		
		if(mEliminateSignChange->get()) {
			if(currentHostBodyOrientation.getX() > 90.0) {
				currentHostBodyOrientation.setX(180.0 - currentHostBodyOrientation.getX());
			}
			if(currentHostBodyOrientation.getX() < -90.0) {
				currentHostBodyOrientation.setX(-180.0 - currentHostBodyOrientation.getX());
			}
			if(currentHostBodyOrientation.getY() > 90.0) {
				currentHostBodyOrientation.setY(180.0 - currentHostBodyOrientation.getY());
			}
			if(currentHostBodyOrientation.getY() < -90.0) {
				currentHostBodyOrientation.setY(-180.0 - currentHostBodyOrientation.getY());
			}
			if(currentHostBodyOrientation.getZ() > 90.0) {
				currentHostBodyOrientation.setZ(180.0 - currentHostBodyOrientation.getZ());
			}
			if(currentHostBodyOrientation.getZ() < -90.0) {
				currentHostBodyOrientation.setZ(-180.0 - currentHostBodyOrientation.getZ());
			}
		}
		
		mRotation1stAxis->set(currentHostBodyOrientation.getX());
		mRotation2ndAxis->set(currentHostBodyOrientation.getY());
		mRotation3rdAxis->set(currentHostBodyOrientation.getZ());
	}
	else {
		
		Vector3D difference = currentHostBodyOrientation - mPreviousBodyAngles;
		while(difference.getX() <= -180.0) {
			difference.setX(difference.getX() + 360.0);
		}
		while(difference.getY() <= -180.0) {
			difference.setY(difference.getY() + 360.0);
		}
		while(difference.getZ() <= -180.0) {
			difference.setZ(difference.getZ() + 360.0);
		}
		while(difference.getX() > 180.0) {
			difference.setX(difference.getX() - 360.0);
		}
		while(difference.getY() > 180.0) {
			difference.setY(difference.getY() - 360.0);
		}
		while(difference.getZ() > 180.0) {
			difference.setZ(difference.getZ() - 360.0);
		}

		mRotation1stAxis->set(difference.getX());
		mRotation2ndAxis->set(difference.getY());
		mRotation3rdAxis->set(difference.getZ());
		
	}

	mPreviousBodyAngles = currentHostBodyOrientation;
	
}


}



