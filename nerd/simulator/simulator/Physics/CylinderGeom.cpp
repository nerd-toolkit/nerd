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

#include "CylinderGeom.h"

namespace nerd {

/**
 * Constructor
 * @param simObject The owner object, with which this geometry will be associated.
 * @param radius Radius of the cylinder (default 0.0).
 * @param length Length of the cylinder (default 0.0).
 */
CylinderGeom::CylinderGeom(SimObject *simObject, double radius, double length) 
		: SimGeom(simObject), mRadius(radius), mLength(length)
{
}

CylinderGeom::CylinderGeom(SimObject *simObject, double radius, double length, 
		const Vector3D &localPosition) : SimGeom(simObject) 
{
	mRadius = radius;
	mLength = length;
	mLocalPosition = localPosition;
}

CylinderGeom::~CylinderGeom() {
}

SimGeom* CylinderGeom::createCopy() const {
	return new CylinderGeom(*this);
}

double CylinderGeom::getRadius() const{
	return mRadius;
}

void CylinderGeom::setRadius(double radius) {
	if(radius < 0.0) {
		return;
	}
	mRadius = radius;
}

double CylinderGeom::getLength() const {
	return mLength;
}

void CylinderGeom::setLength(double length) {
	if(length < 0.0) {
		return;
	}
	mLength = length;
}

}
