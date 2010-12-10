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



#include "PassiveActuatorAdapter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "MotorModels/PassiveActuatorModel.h"
#include "Value/ValueManager.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new PassiveActuatorAdapter.
 */
PassiveActuatorAdapter::PassiveActuatorAdapter(const QString &name)
	: HingeJointMotorAdapter(name)
{
	mReferenceAngleName = new StringValue();
	mGearRatio = new DoubleValue(1.0);
	mReferenceOffsetAngle = new DoubleValue();
	mOffsetAngle = new DoubleValue();

	addParameter("ReferenceAngleName", mReferenceAngleName);
	addParameter("GearRatio", mGearRatio);
	addParameter("ReferenceAngleOffset", mReferenceOffsetAngle);
	addParameter("AngleOffset", mOffsetAngle);
}


/**
 * Copy constructor. 
 * 
 * @param other the PassiveActuatorAdapter object to copy.
 */
PassiveActuatorAdapter::PassiveActuatorAdapter(const PassiveActuatorAdapter &other)
	: SimSensor(other), SimActuator(other), Object(other), ValueChangedListener(other), 
		HingeJointMotorAdapter(other) 
{
	mReferenceAngleName = dynamic_cast<StringValue*>(getParameter("ReferenceAngleName"));
	mGearRatio = dynamic_cast<DoubleValue*>(getParameter("GearRatio"));
	mReferenceOffsetAngle = dynamic_cast<DoubleValue*>(getParameter("ReferenceAngleOffset"));
	mOffsetAngle = dynamic_cast<DoubleValue*>(getParameter("AngleOffset"));
}

/**
 * Destructor.
 */
PassiveActuatorAdapter::~PassiveActuatorAdapter() {
}

SimObject* PassiveActuatorAdapter::createCopy() const {
	PassiveActuatorAdapter *adapter = new PassiveActuatorAdapter(*this);
	adapter->collectCompatibleMotorModels();
	return adapter;
}

void PassiveActuatorAdapter::setup() {
	HingeJointMotorAdapter::setup();

	mReferenceAngle = Core::getInstance()->getValueManager()
						->getDoubleValue(mReferenceAngleName->get());

	if(mReferenceAngle == 0) {
		Core::log("PassiveActuatorAdapter [" + getName() + "]: Could not find required "
				  "reference angle value [" + mReferenceAngleName->get() + "]!", true);
	}
}


void PassiveActuatorAdapter::clear() {
	HingeJointMotorAdapter::clear();
	mReferenceAngle = 0;
}


double PassiveActuatorAdapter::getReferenceAngle() const {
	if(mReferenceAngle == 0) {
		return 0.0;
	}
	return mReferenceAngle->get();
}


double PassiveActuatorAdapter::getGearRatio() const {
	return mGearRatio->get();
}


double PassiveActuatorAdapter::getReferenceOffsetAngle() const {
	return mReferenceOffsetAngle->get();
}


double PassiveActuatorAdapter::getOffsetAngle() const {
	return mOffsetAngle->get();
}


bool PassiveActuatorAdapter::isValidMotorModel(MotorModel *model) const {
	if((dynamic_cast<PassiveActuatorModel*>(model)) != 0) {
		return true;
	}
	return false;
}



}



