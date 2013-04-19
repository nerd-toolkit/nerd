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



#include "Simple2D_Body.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/SimBody.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new Simple2D_Body.
 */
Simple2D_Body::Simple2D_Body()
	: mParent(0), mLocalPosition(0), mLocalOrientation(0)
{
	mLocalPosition = new Vector3DValue();
	mLocalOrientation = new Vector3DValue();
}


/**
 * Copy constructor. 
 * 
 * @param other the Simple2D_Body object to copy.
 */
Simple2D_Body::Simple2D_Body(const Simple2D_Body&) 
	: mParent(0), mLocalPosition(0), mLocalOrientation(0)
{
	mLocalPosition = new Vector3DValue();
	mLocalOrientation = new Vector3DValue();
}

/**
 * Destructor.
 */
Simple2D_Body::~Simple2D_Body() {
}

bool Simple2D_Body::addChildBody(Simple2D_Body *body) {
	if(body == 0 || mChildBodies.contains(body)) {
		return false;
	}
	mChildBodies.append(body);
	return true;
}

bool Simple2D_Body::removeChildBody(Simple2D_Body *body) {
	if(body == 0 || !mChildBodies.contains(body)) {
		return false;
	}
	mChildBodies.removeAll(body);
	return true;
}	

void Simple2D_Body::clearChildBodies() {
	mChildBodies.clear();
}

QList<Simple2D_Body*> Simple2D_Body::getChildBodies() const {
	return mChildBodies;
}


void Simple2D_Body::setParent(Simple2D_Body *parent) {
	mParent = parent;
}

Simple2D_Body* Simple2D_Body::getParent() const {
	return mParent;
}

Vector3DValue* Simple2D_Body::getLocalPositionValue() const {
	return mLocalPosition;
}


Vector3DValue* Simple2D_Body::getLocalOrientationValue() const {
	return mLocalOrientation;
}

void Simple2D_Body::addRotation(double rad, double x, double y) {

	if(rad == 0.0) {
		return;
	}

	Vector3DValue *position = getPositionValue();
	Vector3DValue *orientation = getOrientationValue();

	if(position == 0 || orientation == 0) {
		Core::log("Simple2D_Body: Could not find required position or orientation parameter.");
		return;
	}

	double posX = ((position->getX() - x) * Math::cos(-rad)) 
				 - ((position->getZ() - y) * Math::sin(-rad))
				 + x;
	double posY = ((position->getX() - x) * Math::sin(-rad)) 
				 + ((position->getZ() - y) * Math::cos(-rad))
				 + y;

	double newAngle = Math::forceToRadRange((orientation->getY() / 180.0 * Math::PI) + rad);

	orientation->set(orientation->getX(), 
						newAngle * 180.0 / Math::PI, 
						orientation->getZ());
	position->set(posX, position->getY(), posY);

	for(QListIterator<Simple2D_Body*> i(mChildBodies); i.hasNext();) {
		Simple2D_Body *body = i.next();
		body->addRotation(rad, x, y);
	}

	//x' = (x-u) cos(beta) - (y-v) sin(beta) + u
	//y' = (x-u) sin(beta) + (y-v) cos(beta) + v 
}

void Simple2D_Body::addTranslation(double x, double y) {

	Vector3DValue *position = getPositionValue();
	if(position != 0) {
		position->set(position->getX() + x, position->getY(), position->getZ() + y);
	}
	else {
		Core::log("Simple2D_Body: This object is not a SimBody!", true);
	}
	for(QListIterator<Simple2D_Body*> i(mChildBodies); i.hasNext();) {
		Simple2D_Body *body = i.next();
		body->addTranslation(x, y);
	}
}


Vector3DValue* Simple2D_Body::getLocalPosition() {
	return mLocalPosition;
}


Vector3DValue* Simple2D_Body::getLocalOrientation() {
	return mLocalOrientation;
}
}
