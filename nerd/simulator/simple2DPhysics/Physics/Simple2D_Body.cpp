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



#include "Simple2D_Body.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/SimBody.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new Simple2D_Body.
 */
Simple2D_Body::Simple2D_Body()
	: mLocalPosition(0), mLocalOrientation(0)
{
	mLocalPosition = new Vector3DValue();
	mLocalOrientation = new Vector3DValue();

	SimBody *simBody = dynamic_cast<SimBody*>(this);
	if(simBody != 0) {
		simBody->addParameter("LocalPosition", mLocalPosition);
		simBody->addParameter("LocalOrientation", mLocalOrientation);
	}
}


/**
 * Copy constructor. 
 * 
 * @param other the Simple2D_Body object to copy.
 */
Simple2D_Body::Simple2D_Body(const Simple2D_Body &other) 
	: mLocalPosition(0), mLocalOrientation(0)
{
	SimBody *simBody = dynamic_cast<SimBody*>(this);
	if(simBody != 0) {
		mLocalPosition = dynamic_cast<Vector3DValue*>(simBody->getParameter("LocalPosition"));
		mLocalOrientation = dynamic_cast<Vector3DValue*>(simBody->getParameter("LocalOrientation"));
	}
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

void Simple2D_Body::addRotation(double xRad, double yRad, const QPointF &origin) {

}

void Simple2D_Body::addTranslation(double x, double y) {
	SimBody *body = dynamic_cast<SimBody*>(this);
	if(body != 0) {
		Vector3DValue *pos = body->getPositionValue();
		body->getPositionValue()->set(pos->getX() + x, pos->getY(), pos->getZ() + y);
	}
	else {
		Core::log("Simple2D_Body: This object is not a SimBody!");
	}
}

}
