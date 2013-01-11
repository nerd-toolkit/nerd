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

#include "SimGeom.h"
#include "SimObject.h"

namespace nerd {	

/**
 * Constructor
 * @param simObject 
 */
SimGeom::SimGeom(SimObject *simObject) : mSimObject(simObject), 
		mGeomColor(Color(255, 255, 255, 255)), mLocalPosition(Vector3D(0.0,0.0,0.0)), 
		mLocalOrientation(Quaternion(1.0,0.0,0.0,0.0)), mEnableDrawing(true), 
		mAutoColor(true)
{
}

/**
 * Copy-Constructor
 * @param geom 
 */
SimGeom::SimGeom(const SimGeom &geom) : mSimObject(geom.mSimObject), 
		mGeomColor(geom.mGeomColor), mLocalPosition(geom.mLocalPosition), 
		mLocalOrientation(geom.mLocalOrientation), mEnableDrawing(geom.mEnableDrawing), 
		mAutoColor(true)
{
}

SimGeom::~SimGeom() {
}

SimGeom* SimGeom::createCopy() const {
	return new SimGeom(*this);
}

/**
 * Returns the SimObject to which this SimGeom belongs. 
 * @return the SimObject this geometry belongs to.
 */
SimObject* SimGeom::getSimObject() const {
	return mSimObject;
}

void SimGeom::setSimObject(SimObject *simObject) {
	mSimObject = simObject;
}

/**
 * Changes the color of the geometry.
 * @param newColor the new color of this geometry.
 */
void SimGeom::setColor(const Color &newColor) {
	mGeomColor = newColor;
}

void SimGeom::setColor(int red, int green, int blue, int alpha) {
	mGeomColor.set(red, green, blue, alpha);
}

Color SimGeom::getColor() const {
	return mGeomColor;
}


/**
 * Enables or disables automatic color changes.
 * If enabled, then the color of this geom is automatically changed when it's 
 * owners color is changed (and owner is a subclass of SimBody).
 *
 * @enable set to true to enable automatic color adjustment.
 */
void SimGeom::setAutomaticColor(bool enable) {
	mAutoColor = enable;
}


/**
 * Returns true if automatic color adjustment is allowed.
 *
 * @return true if color adjustment is allowed.
 */
bool SimGeom::hasAutomaticColor() const {
	return mAutoColor;
}


/**
 * Returns the local position of the geometry.
 * The local position describes the geometries position relative to 
 * the position of the *related SimObject.
 * @return the local position of the geometry.
 */
Vector3D SimGeom::getLocalPosition() const {
	return mLocalPosition;
}

void SimGeom::setLocalPosition(const Vector3D &localPosition) {
	mLocalPosition = localPosition;
}

void SimGeom::enableDrawing(bool enable) {
	mEnableDrawing = enable;
}


bool SimGeom::isDrawingEnabled() const {
	return mEnableDrawing;
}


void SimGeom::setLocalOrientation(const Quaternion  &localOrientation) {
	mLocalOrientation = localOrientation;
}


Quaternion SimGeom::getLocalOrientation() const  {
	return mLocalOrientation;
}

}


