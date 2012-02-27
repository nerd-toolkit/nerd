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


#include "Color.h"
#include <math.h>
#include "Math/Math.h"


namespace nerd{


/**
 * Default constructor that creates a redhite color.
 */
Color::Color() {
	mVector[0] = 255;
	mVector[1] = 255;
	mVector[2] = 255;
	mVector[3] = 255;
}


/**
 * Constructor.
 * @param red
 * @param green
 * @param blue
 * @param alpha
 */
Color::Color(int red, int green, int blue, int alpha) {
	mVector[0] = Math::forceToRange(red, 0, 255);
	mVector[1] = Math::forceToRange(green, 0, 255);
	mVector[2] = Math::forceToRange(blue, 0, 255);
	mVector[3] = Math::forceToRange(alpha, 0, 255);
}

/**
 * Constructor to create a RGB-color. The alpha-canal is set to 255.
 * @param red
 * @param green
 * @param blue
 */
Color::Color(int red, int green, int blue) {
	mVector[0] = Math::forceToRange(red, 0, 255);
	mVector[1] = Math::forceToRange(green, 0, 255);
	mVector[2] = Math::forceToRange(blue, 0, 255);
	mVector[3] = 255;
}

/**
 * Copy constructor
 * @param color 
 */
Color::Color(const Color &color) {
	mVector[0] = color.mVector[0];
	mVector[1] = color.mVector[1];
	mVector[2] = color.mVector[2];
	mVector[3] = color.mVector[3];
}

/**
 * Changes the red-canal value of the color.
 * @param red the red-canal of the color.
 */
void Color::setRed(int red) {
	mVector[0] = Math::forceToRange(red, 0, 255);
}

/**
 * The current red-canal value of the color.
 * @return
 */
int Color::red() const {
	return mVector[0];
}

/**
 * Changes the green-canal value of the color.
 * @param green the green-canal of the color.
 */
void Color::setGreen(int green) {
	mVector[1] = Math::forceToRange(green, 0, 255);
}

/**
 * The current green-canal value of the color.
 * @return
 */
int Color::green() const {
	return mVector[1];
}

/**
 * Changes the blue-canal value of the color.
 * @param blue the blue-canal of the color.
 */
void Color::setBlue(int blue) {
	mVector[2] = Math::forceToRange(blue, 0, 255);
}

/**
 * The current blue-canal value of the color.
 * @return
 */
int Color::blue() const {
	return mVector[2];
}

/**
 * Changes the alpha-canal value of the color.
 * @param alpha the alpha-canal of the color.
 */
void Color::setAlpha(int alpha) {
	mVector[3] = Math::forceToRange(alpha, 0, 255);
}

/**
 * The current alpha-canal value of the color.
 * @return
 */
int Color::alpha() const {
	return mVector[3];
}

/**
 * @param red
 * @param green
 * @param blue
 * @param alpha
 */
void Color::set(int red, int green, int blue, int alpha) {
	mVector[0] = Math::forceToRange(red, 0, 255);
	mVector[1] = Math::forceToRange(green, 0, 255);
	mVector[2] = Math::forceToRange(blue, 0, 255);
	mVector[3] = Math::forceToRange(alpha, 0, 255);
}

/**
 * Sets the RGB-canals of the color to the parameter values. The alpha-canal is set to 255.
 * @param red
 * @param green
 * @param blue
 */
void Color::set(int red, int green, int blue) {
	mVector[0] = Math::forceToRange(red, 0, 255);
	mVector[1] = Math::forceToRange(green, 0, 255);
	mVector[2] = Math::forceToRange(blue, 0, 255);
	mVector[3] = 255;
}

/**
 *
 * @return The RGBA-values of the color-object.
 */
const int* Color::get() const {
	return mVector;
}

/**
 * Comparing the color-object with a second color-object. If all canals of the two color
 * objects are equal, true will be returned.
 * @param toCompareWith
 * @return
 */
bool Color::equals(const Color &toCompareWith) const{

	if(this->red() == toCompareWith.red() 
		&& this->green() == toCompareWith.green() 
		&& this->blue() == toCompareWith.blue() 
		&& this->alpha() == toCompareWith.alpha()) 
	{
		return true;
	}
	return false;
}

}
