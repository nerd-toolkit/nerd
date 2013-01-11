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


#include "Math.h"
#include <cmath>
#include <sys/time.h>
#include <QString>
#include "Core/Core.h"
#include <iostream>

using namespace std;

namespace nerd {

const double Math::PI = 3.14159265;
QVector<long long> Math::mFactorials = QVector<long long>();

Math::Math() {
}

Math::~Math() {
}


/**
 * Compares two doubles with a given precision.
 *
 * @param value1 the first double to compare.
 * @param value2 the second double to compare.
 * @param precision the precision (number of digits behind the comma to consider)
 * @param verbose if true a debug message will be printed out.
 * @return true if both doubles are the same up to the given precision.
 */
bool Math::compareDoubles(double value1, double value2, int precision, bool verbose) {
	double maxError = pow(10.0, -precision);
	if(verbose) {
		qDebug("Compared Doubles: %f  %f %f", value1, value2, maxError);
	}
	return ((Math::abs(value1 - value2)) < maxError);
}

/**
 * Compares two doubles with a given maximal error.
 * If the absolute difference between both doubles is less or equal than the maximal error, then 
 * true is returned, otherwise false.
 *
 * @param value1 the first double to compare.
 * @param value2 the second double to compare.
 * @param maxError the maximal error the difference of the two values may be.
 * @return true if the absolute difference between the double is less or equal to maxError, else false.
 */
bool Math::compareDoubles(double value1, double value2, double maxError) {
	return ((Math::abs(value1 - value2)) <= maxError);
}


/**
 * Rounds a number by the given precision (numberOfDigits).
 *
 * @param value the number to round.
 * @param numberOfDigits the relevant number of digits behind the comma.
 * @return the rounded value
 */
double Math::round(double value, int numberOfDigits) {
	if(value > 0) {
		return (double) (((long) ((value * pow(10.0, numberOfDigits)) + 0.5))
											/ pow(10.0, numberOfDigits));
	}
	else {
		return (double) (((long) ((value * pow(10.0, numberOfDigits)) - 0.5))
											/ pow(10.0, numberOfDigits));
	}
}



/**
 * Forces a value into a range. If the value is less than min, then it is
 * set to min. If it is larger than max, then it is set to max. Otherwise
 * it is unaffected.
 *
 * @param value the value to be forced
 * @param min the lower limit of the range
 * @param max the upper limit of the range
 * @return the forced value.
 */
int Math::forceToRange(int value, int min, int max) {
	if(min > max) {
		Core::log("Math::forceToRange Error: Min > Max");
		return min;
	}

	if(value < min) {
		return min;
	}

	if(value > max) {
		return max;
	}

	return value;
}

/**
 * Forces a value to a range
 * @param value the value to be forced
 * @param min the lower limit of the range
 * @param max the upper limit of the range
 * @return the forced value.
 */
double Math::forceToRangeDouble(double value, double min, double max) {
	if(min > max) {
		Core::log("Math::forceToRange Error: Min > Max");
		return min;
	}

	if(value < min) {
		return min;
	}

	if(value > max) {
		return max;
	}

	return value;
}

double Math::forceToDegreeRange(double value) {
	while(value < 0) {
		value += 360.0;
	}
	while(value >= 360.0) {
		value -= 360.0;
	}
	return value;
}


double Math::forceToRadRange(double value) {
	return forceToDegreeRange(value / Math::PI * 180.0) * Math::PI / 180.0;
}

int Math::abs(int value) {
	if(value < 0) {
		return -value;
	}
	return value;
}


double Math::abs(double value) {
	if(value < 0.0) {
		return -value;
	}
	return value;
}


/**
 * Returns -1.0 for negative and 1.0 for positive values.
 * If zeroIsThirdSign is false, then value == 0 is treated as positive value.
 * Otherwise, if value == zero, the third "sign" 0 is returned.
 */
double Math::sign(double value, bool zeroIsThirdSign) {
	if(value == 0.0 && zeroIsThirdSign) {
		return 0.0;
	}
	return value < 0.0 ? -1.0 : 1.0;
}



double Math::min(double value1, double value2) {
	return value1 < value2 ? value1 : value2;
}


int Math::min(int value1, int value2) {
	return value1 < value2 ? value1 : value2;
}

qulonglong Math::min(qulonglong value1, qulonglong value2) { 
	return value1 < value2 ? value1 : value2;
}


double Math::max(double value1, double value2) {
	return value1 > value2 ? value1 : value2;
}


int Math::max(int value1, int value2) {
	return value1 > value2 ? value1 : value2;
}

qulonglong Math::max(qulonglong value1, qulonglong value2) {
	return value1 > value2 ? value1 : value2;
}


/**
 * Calculate gaussian noise.
 *
 * @param mean the mean value for the noise calculation
 * @param deviation the deviation for the noise calculation
 * @return mean + calculated noise.
 */
double Math::calculateGaussian(double mean, double deviation) {
	return mean + getNextGaussianDistributedValue(deviation);
}


/**
 * Calculate noraml noise.
 *
 * @param mean the mean value for the noise calculation
 * @param range the normal noise range.
 * @return mean + calculated noise.
 */
double Math::calculateUniformNoise(double mean, double range) {
	double random = rand();
	random = random / RAND_MAX;
	random = 2 * random - 1;
	return mean + random * range;
}


/**
 * Returns a uniformly distributed value in the interval [-range, range].
 *
 * @param range the range in which values can occur.
 * @return a uniformly distributed value within interval [-range, range].
 */
double Math::getNextUniformlyDistributedValue(double range) {
	double random = rand();
	random = random / RAND_MAX;
	random = 2 * random - 1;
	return random * range;
}



double Math::getNextGaussianDistributedValue(double deviation) {
	double v1, v2, s;
	double rand1, rand2;

	do{
		rand1 = rand();
		rand1 = rand1/RAND_MAX;
		rand2 = rand();
		rand2 = rand2/RAND_MAX;
		v1 = 2 * rand1 - 1;
		v2 = 2 * rand2 - 1;
		s = v1 * v1 + v2 * v2;
	} while(s >= 1 || s == 0);

	double multiplier = sqrt(-2 * log(s) / s);
	double nextGaussian = v1 * multiplier;
	return deviation * nextGaussian;
}


double  Math::distance(const QPoint &p1, const QPoint &p2) {
	return sqrt(((p1.x() - p2.x()) * (p1.x() - p2.x()))
				+ ((p1.y() - p2.y()) * (p1.y() - p2.y())));
}

double Math::sin(double value) {
	return ::sin(value);
}

double Math::cos(double value) {
	return ::cos(value);
}

double Math::distance(const QPointF &p1, const QPointF &p2) {
	return sqrt(((p1.x() - p2.x()) * (p1.x() - p2.x()))
				+ ((p1.y() - p2.y()) * (p1.y() - p2.y())));
}

double  Math::distance(const Vector3D &p1, const Vector3D &p2) {
	return sqrt(((p1.getX() - p2.getX()) * (p1.getX() - p2.getX()))
				+ ((p1.getY() - p2.getY()) * (p1.getY() - p2.getY()))
				+ ((p1.getZ() - p2.getZ()) * (p1.getZ() - p2.getZ())));
}

QPointF Math::centerOfLine(const QPointF &p1, const QPointF &p2, double offset) {
	if(p1 == p2) {
		return p1;
	}
	
	double dist = Math::distance(p1, p2);
	double angle = atan2(p2.y() - p1.y(), p2.x() - p1.x());
	
	
	double x = (cos(angle) * 0.5 * dist) + p1.x();
	double y = (sin(angle) * 0.5 * dist) + p1.y();
	
	if(offset != 0.0) {
		x = x + (sin((Math::PI) - angle) * offset);
		y = y + (cos((Math::PI) - angle) * offset);
	}
	
	return QPointF(x, y);
}


/**
 * TODO Warning, this has to be implemented! Currently, this is only 2D distance.
 */
Vector3D Math::centerOfLine(const Vector3D &p1, const Vector3D &p2, double offset) {
	QPointF pos = Math::centerOfLine(QPointF(p1.getX(), p1.getY()), QPointF(p2.getX(), p2.getY()), offset);
	return Vector3D(pos.x(), pos.y(), 0.0);
}


/**
 * Factorial is only defined for long long and thus up to approx. 12!.
 * Larger x are just treated as 12 and a warning is printed to the console!
 */
long long Math::factorial(int x) {
	if(mFactorials.empty()) {
		mFactorials.append(0);
		mFactorials.append(1);
	}
	if(x <= 0) {
		return 0;
	}
	if(x < mFactorials.size()) {
		return mFactorials.at(x);
	}
	if(x > 12) {
		Core::log(QString("Math::factorial() Warning: A number greater than 12 (") 
				+ QString::number(x) + ")! has been requested. Due to numeric limitations, x cannot be greater than 12! "
				+ "x therefore has been truncated to 12.", true);
		x = 12;
	}
	for(int i = mFactorials.size(); i <= x; ++i) {
		mFactorials.append(mFactorials.at(i - 1) * (long long) i);
	}
	return mFactorials.last();
}

}




