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



#include "LinearSpringModel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "MotorModels/SpringAdapter.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new LinearSpringModel.
 */
LinearSpringModel::LinearSpringModel(const QString &name, JointType type)
	: SpringModel(name, type)
{
	mSpringConstant = new DoubleValue(14.8396); //0.259 Nm/grad (in Nm pro rad)
	mSpringOffset = new DoubleValue(0.0);

	addParameter("SpringConstant", mSpringConstant);
	addParameter("Offset", mSpringOffset);
}


/**
 * Copy constructor. 
 * 
 * @param other the LinearSpringModel object to copy.
 */
LinearSpringModel::LinearSpringModel(const LinearSpringModel &other) 
	: SpringModel(other)
{
	mSpringConstant = dynamic_cast<DoubleValue*>(getParameter("SpringConstant"));
	mSpringOffset = dynamic_cast<DoubleValue*>(getParameter("Offset"));
}

/**
 * Destructor.
 */
LinearSpringModel::~LinearSpringModel() {
}


void LinearSpringModel::setup() {
	SpringModel::setup();
}


void LinearSpringModel::clear() {
	SpringModel::clear();
}

		
void LinearSpringModel::updateInputValues() {

}


void LinearSpringModel::updateOutputValues() {

}


bool LinearSpringModel::setOwner(MotorAdapter *owner) {
	mOwnerSpringAdapter = dynamic_cast<SpringAdapter*>(owner);

	if(mOwnerSpringAdapter == 0) {
		return false;
	}
	return true;
}


double LinearSpringModel::calculateTorque(double currentPosition) {
	//currentPosition has to be in rad.
	return -mSpringConstant->get() * ((mSpringOffset->get() / 180.0 * Math::PI) + currentPosition);
}

double LinearSpringModel::calculateForce(double currentPosition) {
	return -mSpringConstant->get() * (mSpringOffset->get() + currentPosition);
}

}


