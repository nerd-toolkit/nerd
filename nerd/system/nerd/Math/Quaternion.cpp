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

#include "Quaternion.h"
#include <math.h>
#include "Math/Math.h"

namespace nerd{

Quaternion::Quaternion(double w, double x, double y, double z) {
	mQuaternion[0] = w;
	mQuaternion[1] = x;
	mQuaternion[2] = y;
	mQuaternion[3] = z;
}

double Quaternion::getW() const {
	return mQuaternion[0];
}

double Quaternion::getX() const {
	return mQuaternion[1];
}

double Quaternion::getY() const {
	return mQuaternion[2];
}

double Quaternion::getZ() const {
	return mQuaternion[3];
}

const double* Quaternion::get() const {
	return mQuaternion;
}

void Quaternion::setW(double w) {
	mQuaternion[0] = w;
}

void Quaternion::setX(double x) {
	mQuaternion[1] = x;
}

void Quaternion::setY(double y) {
	mQuaternion[2] = y;
}

void Quaternion::setZ(double z) {
	mQuaternion[3] = z;
}

void Quaternion::set(const Quaternion &quaternion) {
	mQuaternion[0] = quaternion.mQuaternion[0];
	mQuaternion[1] = quaternion.mQuaternion[1];
	mQuaternion[2] = quaternion.mQuaternion[2];
	mQuaternion[3] = quaternion.mQuaternion[3];
}

void Quaternion::set(double w, double x, double y, double z) {
	mQuaternion[0] = w;
	mQuaternion[1] = x;
	mQuaternion[2] = y;
	mQuaternion[3] = z;
}

/**
 * Calculates the length of the quaternion: sqrt(sum(q_i)²).
 * @return 
 */
double Quaternion::length() const {
	double product = mQuaternion[0] * mQuaternion[0] 
					+ mQuaternion[1] * mQuaternion[1] 
					+ mQuaternion[2] * mQuaternion[2] 
					+ mQuaternion[3] * mQuaternion[3];
	return sqrt(product);
}

/**
 * Calculates the inverse of the given quaternion.
 * @return The inverse quaternion.
 */
Quaternion Quaternion::getInverse() const {
	Quaternion conjugate = getConjugate();
		double absoluteValue = 0.0;
	if(length() != 0) {
		absoluteValue = 1.0/ (length() * length());
	}
	return conjugate * absoluteValue;
}

/**
 * Calculates the conjugate of the given quaternion.
 * @return The conjugate quaternion (w, -x, -y -z). 
 */
Quaternion Quaternion::getConjugate() const {
	return Quaternion(getW(), -getX(), -getY(), -getZ());
}

/**
 * Normalizes the given quaternion.
 */
void Quaternion::normalize() {
	if(length() != 0) {
		double quaternionLength = length();
		mQuaternion[0] = mQuaternion[0] / quaternionLength;
		mQuaternion[1] = mQuaternion[1] / quaternionLength;
		mQuaternion[2] = mQuaternion[2] / quaternionLength;
		mQuaternion[3] = mQuaternion[3] / quaternionLength;
	}
	else{
		mQuaternion[0] = 0.0;
		mQuaternion[1] = 0.0;
		mQuaternion[2] = 0.0;
		mQuaternion[3] = 0.0;

	}
}

Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) {
	return Quaternion(lhs.getW() + rhs.getW(), 
					  lhs.getX() + rhs.getX(), 
					  lhs.getY() + rhs.getY(), 
					  lhs.getZ() + rhs.getZ());
}

Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs) {
	return Quaternion(lhs.getW() - rhs.getW(), 
					  lhs.getX() - rhs.getX(), 
					  lhs.getY() - rhs.getY(), 
					  lhs.getZ() - rhs.getZ());
}


Quaternion operator*(const Quaternion& lhs, double rhs) {
	return Quaternion(lhs.getW() * rhs, 
					  lhs.getX() * rhs, 
					  lhs.getY() * rhs, 
					  lhs.getZ() * rhs);
}


Quaternion operator*(double lhs, const Quaternion& rhs) {
	return Quaternion(lhs * rhs.getW(), 
					  lhs * rhs.getX(), 
					  lhs * rhs.getY(), 
					  lhs * rhs.getZ());
}

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) {
	return Quaternion(
		lhs.getW()*rhs.getW()-lhs.getX()*rhs.getX()-lhs.getY()*rhs.getY()-lhs.getZ()*rhs.getZ(),
		lhs.getW()*rhs.getX()+lhs.getX()*rhs.getW()+lhs.getY()*rhs.getZ()-lhs.getZ()*rhs.getY(),
		lhs.getW()*rhs.getY()-lhs.getX()*rhs.getZ()+lhs.getY()*rhs.getW()+lhs.getZ()*rhs.getX(),
		lhs.getW()*rhs.getZ()+lhs.getX()*rhs.getY()-lhs.getY()*rhs.getX()+lhs.getZ()*rhs.getW());
}


//TODO: Are quaterions w,x,y,z and -w,-x-y-z equal??
bool Quaternion::equals(const Quaternion &quaternion, int precision) {
	if(!Math::compareDoubles(mQuaternion[0], quaternion.mQuaternion[0], precision)) {
		return false;
	}
	if(!Math::compareDoubles(mQuaternion[1], quaternion.mQuaternion[1], precision)) {
		return false;
	}
	if(!Math::compareDoubles(mQuaternion[2], quaternion.mQuaternion[2], precision)) {
		return false;
	}
	if(!Math::compareDoubles(mQuaternion[3], quaternion.mQuaternion[3], precision)) {
		return false;
	}
	return true;
}

/**
 * Converts euklidian angle information (in degree) into a quaternion.
 * @param alpha 
 * @param beta 
 * @param gamma 
 */
void Quaternion::setFromAngles(double alpha, double beta, double gamma) {

	alpha = alpha * Math::PI / 180.0;
	beta = beta * Math::PI / 180.0;
	gamma = gamma * Math::PI / 180.0;

	double w = cos(alpha / 2.0) * cos(beta / 2.0) * cos(gamma / 2.0)
				+ sin(alpha / 2.0) * sin(beta / 2.0) * sin(gamma / 2.0);

	double x = sin(alpha / 2.0) * cos(beta / 2.0) * cos(gamma / 2.0)
				- cos(alpha / 2.0) * sin(beta / 2.0) * sin(gamma / 2.0);

	double y = cos(alpha / 2.0) * sin(beta / 2.0) * cos(gamma / 2.0)
				+ sin(alpha / 2.0) * cos(beta / 2.0) * sin(gamma / 2.0);

	double z = cos(alpha / 2.0) * cos(beta / 2.0) * sin(gamma / 2.0)
				- sin(alpha / 2.0) * sin(beta / 2.0) * cos(gamma / 2.0);

	set(w,x,y,z);
}


/**
 * converts the quaternion in one valid angle-representation. There are ambiguous conversion results possible.
 * @return
 */
Vector3D Quaternion::toAngles() {

	double w = mQuaternion[0];
	double x = mQuaternion[1];
	double y = mQuaternion[2];
	double z = mQuaternion[3];

	double alpha = atan2(2 * x * w 	- 2 * y * z,
						1 - 2 * (x * x) - 2 * (z * z));

	double beta = atan2(2 * y * w - 2 * x * z,
						1 - 2 * (y * y) - 2 * (z * z));

	double gamma = asin(2 * x * y + 2 * z * w);

	alpha = (alpha * 180.0) / Math::PI;
	beta = (beta * 180.0) / Math::PI;
	gamma = (gamma * 180.0) / Math::PI;

	return Vector3D(alpha, beta, gamma);
}

}
