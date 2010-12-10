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


#include "Vector3D.h"
#include <math.h>
#include "Math/Math.h"

namespace nerd{


Vector3D::Vector3D() {
	mVector[0] = 0.0;
	mVector[1] = 0.0;
	mVector[2] = 0.0;
}


Vector3D::Vector3D(double x, double y, double z) {
	mVector[0] = x;
	mVector[1] = y;
	mVector[2] = z;
}

Vector3D::Vector3D(const Vector3D &vector) {
	mVector[0] = vector.mVector[0];
	mVector[1] = vector.mVector[1];
	mVector[2] = vector.mVector[2];
}


void Vector3D::setX(double x) {
	mVector[0] = x;
}

void Vector3D::setY(double y) {
	mVector[1] = y;
}

void Vector3D::setZ(double z) {
	mVector[2] = z;
}

void Vector3D::set(double x, double y, double z) {
	mVector[0] = x;
	mVector[1] = y;
	mVector[2] = z;
}

//untested.
void Vector3D::set(const Vector3D &other) {
	mVector[0] = other.mVector[0];
	mVector[1] = other.mVector[1];
	mVector[2] = other.mVector[2];
}

double Vector3D::getX() const {
	return mVector[0];
}


double Vector3D::getY() const {
	return mVector[1];
}


double Vector3D::getZ() const {
	return mVector[2];
}


const double* Vector3D::get() const {
	return mVector;
}

bool Vector3D::equals(const Vector3D &vector, int precision) const {
	if(precision < 0) {
		if(mVector[0] == vector.getX()
			&& mVector[1] == vector.getY()
			&& mVector[2] == vector.getZ())
		{
			return true;
		}
		return false;
	}
	else {
		long x1 = (long) ((mVector[0] * pow(10.0, precision)) + 0.5);
		long x2 = (long) ((vector.getX() * pow(10.0, precision)) + 0.5);
		long y1 = (long) ((mVector[1] * pow(10.0, precision)) + 0.5);
		long y2 =  (long) ((vector.getY() * pow(10.0, precision)) + 0.5);
		long z1 = (long) ((mVector[2] * pow(10.0, precision)) + 0.5);
		long z2 = (long) ((vector.getZ() * pow(10.0, precision)) + 0.5);

		if(x1 == x2 && y1 == y2 && z1 == z2) {
			return true;
		}
		return false;
	}
}

/**
 * Normalizes the given vector.
 */
void Vector3D::normalize() {
	double l = length();
	if(l != 0.0) {
		*this = *this * (1.0/l);
	}
	else{
		mVector[0] = 0.0;
		mVector[1] = 0.0;
		mVector[2] = 0.0;
	}
}

double Vector3D::length() {
 return sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());
}



Vector3D Vector3D::crossProduct(const Vector3D &vector1, const Vector3D &vector2) {

	return Vector3D(vector1.getY() * vector2.getZ() - vector1.getZ() * vector2.getY(),
					vector1.getZ() * vector2.getX() - vector1.getX() * vector2.getZ(),
					vector1.getX() * vector2.getY() - vector1.getY() * vector2.getX());
}

/**
 * Returns true if the two Vector3D are equal. The precision of this comparison is 
 * only 10 decimal places.
 */
bool operator==(const Vector3D &lhs, const Vector3D &rhs) {
	if(Math::compareDoubles(lhs.getX(), rhs.getX(), 10) 
		&& Math::compareDoubles(lhs.getY(), rhs.getY(), 10)
		&& Math::compareDoubles(lhs.getZ(), rhs.getZ(), 10)) 
	{
		return true;
	}
	return false;
}

/**
 * Returns true if the two Vector3D are equal. The precision of this comparison is 
 * only 10 decimal places.
 */
bool operator!=(const Vector3D &lhs, const Vector3D &rhs) {
	if(lhs == rhs) {
		return false;
	}
	return true;
}


double operator*(const Vector3D& lhs, const Vector3D& rhs) {
	return (lhs.getX()*rhs.getX() + lhs.getY()*rhs.getY() + lhs.getZ()*rhs.getZ());
}


Vector3D operator*(const double lhs, const Vector3D& rhs) {
	return Vector3D(rhs.getX()*lhs, rhs.getY()*lhs, rhs.getZ()*lhs);
}


Vector3D operator*(const Vector3D& lhs, const double rhs) {
	return Vector3D(lhs.getX()*rhs, lhs.getY()*rhs, lhs.getZ()*rhs);
}

Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs) {
	return Vector3D(lhs.getX()-rhs.getX(), lhs.getY()-rhs.getY(), lhs.getZ()-rhs.getZ());
}

Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs) {
	return Vector3D(lhs.getX()+rhs.getX(), lhs.getY()+rhs.getY(), lhs.getZ()+rhs.getZ());
}

Vector3D operator-(const Vector3D& rhs) {
	return Vector3D(-rhs.getX(), -rhs.getY(), -rhs.getZ());
}
}
