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



#include "ODE_LinearSpringModel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "MotorModels/SpringAdapter.h"
#include "Physics/HingeJoint.h"
#include "Physics/SliderJoint.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ODE_LinearSpringModel.
 */
ODE_LinearSpringModel::ODE_LinearSpringModel(MotorModel::JointType type, const QString &name)
	: LinearSpringModel(name, type)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ODE_LinearSpringModel object to copy.
 */
ODE_LinearSpringModel::ODE_LinearSpringModel(const ODE_LinearSpringModel &other)
	: Object(other), ValueChangedListener(other), LinearSpringModel(other)
{
}

/**
 * Destructor.
 */
ODE_LinearSpringModel::~ODE_LinearSpringModel() {
}


SimObject* ODE_LinearSpringModel::createCopy() const {
	return new ODE_LinearSpringModel(*this);
}


void ODE_LinearSpringModel::setup() {
	LinearSpringModel::setup();

	mJoint = 0;
}


void ODE_LinearSpringModel::clear() {
	LinearSpringModel::clear();

	mJoint = 0;
}


	
void ODE_LinearSpringModel::updateInputValues() {

	if(mOwnerSpringAdapter == 0 || mOwnerSpringAdapter->getCurrentTargetJoint() == 0) {
		return;
	}

	//The first time an update is run the mJoint is located. 
	//This has to be done here instead of setup() to ensure that the joint is
 	//really available, which can not be guaranteed in setup().
	if(mJoint == 0) {
		SimJoint *simJoint = mOwnerSpringAdapter->getCurrentTargetJoint();
		
		ODE_Joint *odeJoint = dynamic_cast<ODE_Joint*>(simJoint);

		if(odeJoint == 0) {
			MotorAdapter *adapter = dynamic_cast<MotorAdapter*>(simJoint);
			if(adapter != 0) {
				odeJoint = dynamic_cast<ODE_Joint*>(adapter->getActiveMotorModel());
			}
		}

		if(simJoint != 0 && odeJoint != 0) {
			if(getType() == MotorModel::HINGE_JOINT) {
				if(dynamic_cast<HingeJoint*>(simJoint) != 0) {
					mJoint = odeJoint->getJoint();
				}
			}
			else if(getType() == MotorModel::SLIDER_JOINT) {
				if(dynamic_cast<SliderJoint*>(simJoint) != 0) {
					mJoint = odeJoint->getJoint();
				}
			}
		}
	}

	if(mJoint != 0) {

		if(getType() == MotorModel::HINGE_JOINT) {
			double torque = calculateTorque(dJointGetHingeAngle(mJoint));
			dJointAddHingeTorque(mJoint, torque);
			mOwnerSpringAdapter->getCurrentTorqueValue()->set(torque);
		}
		else if(getType() == MotorModel::SLIDER_JOINT) {
			double force = calculateForce(dJointGetSliderPosition(mJoint));
			dJointAddSliderForce(mJoint, force);
			mOwnerSpringAdapter->getCurrentTorqueValue()->set(force);
		}
	}
}

void ODE_LinearSpringModel::updateOutputValues() {

}



}



