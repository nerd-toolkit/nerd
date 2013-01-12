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



#include "Simple2D_DifferentialDrive.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new Simple2D_DifferentialDrive.
 */
Simple2D_DifferentialDrive::Simple2D_DifferentialDrive(const QString &name)
	: SimBody(name), Simple2D_Body(), mLeftVelocity(0), mRightVelocity(0), 
		mWidth(0), mMinVelocity(0), mMaxVelocity(0)
{
	mMinVelocity = new DoubleValue(-0.01);
	mMaxVelocity = new DoubleValue(0.01);
	mWidth = new DoubleValue(0.2);

	mLeftVelocity = new InterfaceValue(getName(), "LeftVelocity", 0.0, mMinVelocity->get(), 
									   mMaxVelocity->get());
	mRightVelocity = new InterfaceValue(getName(), "RightVelocity", 0.0, mMinVelocity->get(), 
									   mMaxVelocity->get());

	addParameter("MinVelocity", mMinVelocity);
	addParameter("MaxVelocity", mMaxVelocity);
	addParameter("Width", mWidth);
	addParameter("LeftVelocity", mLeftVelocity);
	addParameter("RightVelocity", mRightVelocity);

	mInputValues.append(mLeftVelocity);
	mInputValues.append(mRightVelocity);

}


/**
 * Copy constructor. 
 * 
 * @param other the Simple2D_DifferentialDrive object to copy.
 */
Simple2D_DifferentialDrive::Simple2D_DifferentialDrive(const Simple2D_DifferentialDrive &other) 
	: Object(), ValueChangedListener(), SimBody(other), SimActuator(), Simple2D_Body(other)
{
	mMinVelocity = dynamic_cast<DoubleValue*>(getParameter("MinVelocity"));
	mMaxVelocity = dynamic_cast<DoubleValue*>(getParameter("MaxVelocity"));
	mWidth = dynamic_cast<DoubleValue*>(getParameter("Width"));
	mLeftVelocity = dynamic_cast<InterfaceValue*>(getParameter("LeftVelocity"));
	mRightVelocity = dynamic_cast<InterfaceValue*>(getParameter("RightVelocity"));

	mInputValues.clear();
	mInputValues.append(mLeftVelocity);
	mInputValues.append(mRightVelocity);
}

/**
 * Destructor.
 */
Simple2D_DifferentialDrive::~Simple2D_DifferentialDrive() {
}


SimObject* Simple2D_DifferentialDrive::createCopy() const {
	return new Simple2D_DifferentialDrive(*this);
}

void Simple2D_DifferentialDrive::updateActuators() {

	double velRight = mRightVelocity->get();
	double velLeft = mLeftVelocity->get();
	
	double x = 0.0;
	double y = 0.0;
	double angle = mOrientationValue->getY();
	//double angleDiff = 0.0;

	if(mWidth->get() == 0) {
		//prevent division by zero.
		return;
	}

	double globalAngleRad = ((angle) / 180.0 * Math::PI);
	double th = ((velRight - velLeft) / mWidth->get());
		
	x = (velRight + velLeft) / 2.0 * Math::cos(th + globalAngleRad);
	y = (velRight + velLeft) / 2.0 * Math::sin(th + globalAngleRad);

	if(x != 0.0 || y != 0.0) {
		addTranslation(x, -y);
	}
	if(th != 0.0) {
		addRotation(th, mPositionValue->getX(), mPositionValue->getZ());
	}
}


bool Simple2D_DifferentialDrive::addParameter(const QString &name, Value *value) {
	return SimBody::addParameter(name, value);
}


Value* Simple2D_DifferentialDrive::getParameter(const QString &name) const {
	return SimBody::getParameter(name);
}

Vector3DValue* Simple2D_DifferentialDrive::getPositionValue() const {
	return mPositionValue;
}


Vector3DValue* Simple2D_DifferentialDrive::getOrientationValue() const {
	return mOrientationValue;
}

void Simple2D_DifferentialDrive::valueChanged(Value *value) {
	SimBody::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mMinVelocity || value == mMaxVelocity) {
		mLeftVelocity->setMin(mMinVelocity->get());
		mLeftVelocity->setMax(mMaxVelocity->get());
		mRightVelocity->setMin(mMinVelocity->get());
		mRightVelocity->setMax(mMaxVelocity->get());
	}
}


}



