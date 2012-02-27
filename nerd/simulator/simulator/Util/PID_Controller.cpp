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



#include "PID_Controller.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"


using namespace std;

namespace nerd {


PID_Controller::PID_Controller()
	: mProportional(0.0), mIntegral(0.0), 
		mDifferential(0.0), mStepSize(0.0), mLastError(0.0)
{

}

/**
 * Constructs a new PID_Controller.
 */
PID_Controller::PID_Controller(double p, double i, double d, double stepSize)
{
	mProportional = p;
	mIntegral = i;
	mDifferential = d;
	mStepSize = stepSize;
	mLastError = 0.0;
}


/**
 * Copy constructor. 
 * 
 * @param other the PID_Controller object to copy.
 */
PID_Controller::PID_Controller(const PID_Controller &other) 
	: mProportional(other.mProportional), mIntegral(other.mIntegral), 
		mDifferential(other.mDifferential), mStepSize(other.mStepSize),
		mLastError(0.0)
{
}

/**
 * Destructor.
 */
PID_Controller::~PID_Controller() {
}


void PID_Controller::setPIDParameters(double p, double i, double d) {
	mProportional = p;
	mIntegral = i;
	mDifferential = d;
}

void PID_Controller::setStepSize(double stepSize) {
	mStepSize = stepSize;
}

void PID_Controller::setHistorySize(int size) {
	mHistorySize = size;
}


bool PID_Controller::update(double currentAngle, double desiredAngle) {

	double velocity = 0.0;
	double currentError = desiredAngle - currentAngle;
	
	mErrorHistory.push_back(currentError);
	
	if(mErrorHistory.size() > mHistorySize) {
		mErrorHistory.pop_front();
	}
	
	double currentI = 0.0;
	
	for(int i = 0; i < mErrorHistory.size(); i++) {
		currentI += mErrorHistory.at(i);
	}
	double errorDistance = currentError - mLastError;
	double iPart;
	if(Math::compareDoubles(mIntegral, 0.0, 10)) {
		iPart= 0.0;
	}
	else {
		iPart= mStepSize / mIntegral * currentI;
	}
	
	if(mStepSize == 0.0) {
		Core::log("DynamixelFrictionMotor Warning: TimeStepSize was 0.0, prevented division by zero.");
		mVelocity = 0.0;
		return false;
	}
	
	velocity = mProportional
		* (currentError + iPart + mDifferential / mStepSize * errorDistance);
	
	mLastError = currentError;
	
	mVelocity = velocity;
	return true;
}


double PID_Controller::getVelocity() const {
	return mVelocity;
}


void PID_Controller::clear() {
	mErrorHistory.clear();
}


}



