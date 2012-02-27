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


#include "NormalizedDoubleValue.h"
#include <QStringList>
#include "Math/Math.h"

namespace nerd {


NormalizedDoubleValue::NormalizedDoubleValue()
					: DoubleValue(), mMin(0.0), mMax(1.0), mNormalizedMin(0.0), mNormalizedMax(1.0)
{
	setTypeName("NormalizedDouble");
}

NormalizedDoubleValue::NormalizedDoubleValue(double value, double min, double max)
					: DoubleValue(), mMin(min), mMax(max),
						mNormalizedMin(min), mNormalizedMax(max)
{
	setTypeName("NormalizedDouble");
	set(value);
}

NormalizedDoubleValue::NormalizedDoubleValue(double value, double min, double max,
													double normalizedMin, double normalizedMax)
					: mMin(min), mMax(max),
						mNormalizedMin(normalizedMin), mNormalizedMax(normalizedMax)
{
	setTypeName("NormalizedDouble");
	set(value);
}


NormalizedDoubleValue::NormalizedDoubleValue(const NormalizedDoubleValue &value)
				: Object(), DoubleValue(value), mMin(value.mMin), mMax(value.mMax),
					mNormalizedMin(value.mNormalizedMin), mNormalizedMax(value.mNormalizedMax)
{

}

NormalizedDoubleValue::~NormalizedDoubleValue() {
}


Value* NormalizedDoubleValue::createCopy() {
	return new NormalizedDoubleValue(*this);
}


void NormalizedDoubleValue::set(double value) {
	if(mMin < mMax) {
		if(value < mMin) {
			value = mMin;
		}
		else if(value > mMax) {
			value = mMax;
		}
	}
	else {
		if(value > mMin) {
			value = mMin;
		}
		else if(value < mMax) {
			value = mMax;
		}
	}
	DoubleValue::set(value);
}


double NormalizedDoubleValue::getNormalized() const {
	if(mMin == mMax || mNormalizedMin == mNormalizedMax) {
		return 0;
	}
	if(mMin < mMax && mNormalizedMin < mNormalizedMax) {
		double ret = (mNormalizedMin + (((mNormalizedMax - mNormalizedMin)
						/ (mMax - mMin)) * (mValue - mMin)));
		ret = (ret > mNormalizedMax) ? mNormalizedMax : ret;
		ret = (ret < mNormalizedMin) ? mNormalizedMin : ret;
		return ret;
	}
	else if(mMax < mMin && mNormalizedMin < mNormalizedMax) {
		double ret = (mNormalizedMin + (((mNormalizedMax - mNormalizedMin)
						/ (mMin - mMax)) * (mValue - mMax)));
		ret = (ret > mNormalizedMax) ? mNormalizedMax : ret;
		ret = (ret < mNormalizedMin) ? mNormalizedMin : ret;
		return ret;
	}
	//TODO
	return 0.0;
}


void NormalizedDoubleValue::setNormalized(double value) {
	if(mMin == mMax || mNormalizedMin == mNormalizedMax) {
		return;
	}
	if(mMin < mMax && mNormalizedMin < mNormalizedMax) {
		value = (value > mNormalizedMax) ? mNormalizedMax : value;
		value = (value < mNormalizedMin) ? mNormalizedMin : value;

		set(mMin + (((mMax - mMin)
					/ (mNormalizedMax - mNormalizedMin)) * (value - mNormalizedMin)));
	}
	else if(mMax < mMin && mNormalizedMin < mNormalizedMax) {
		value = (value > mNormalizedMax) ? mNormalizedMax : value;
		value = (value < mNormalizedMin) ? mNormalizedMin : value;

		set(mMax + (((mMin - mMax)
					/ (mNormalizedMax - mNormalizedMin)) * (value - mNormalizedMin)));
	}
	//TODO
}



void NormalizedDoubleValue::setMin(double min) {
	mMin = min;
	if(mMin <= mMax) {
		if(mValue < mMin) {
			set(mMin);
		}
	}
	else {
		if(mValue > mMin) {
			set(mMin);
		}
	}
}


double NormalizedDoubleValue::getMin() const {
	return mMin;
}


void NormalizedDoubleValue::setMax(double max) {
	mMax = max;
	if(mMin <= mMax) {
		if(mValue > mMax) {
			set(mMax);
		}
	}
	else {
		if(mValue < mMax) {
			set(mMax);
		}
	}
}


double NormalizedDoubleValue::getMax() const {
	return mMax;
}



void NormalizedDoubleValue::setNormalizedMin(double min) {
	mNormalizedMin = min;
}


double NormalizedDoubleValue::getNormalizedMin() const {
	return mNormalizedMin;
}


void NormalizedDoubleValue::setNormalizedMax(double max) {
	mNormalizedMax = max;
}


double NormalizedDoubleValue::getNormalizedMax() const {
	return mNormalizedMax;
}


bool  NormalizedDoubleValue::setProperties(const QString &properties) {
	QStringList parameters = properties.split(",");

	if(parameters.size() != 4) {
		return false;
	}

	bool ok = true;
	double min = parameters.at(0).toDouble(&ok);
	if(!ok) {
		return false;
	}

	double max = parameters.at(1).toDouble(&ok);
	if(!ok) {
		return false;
	}

	double normMin = parameters.at(2).toDouble(&ok);
	if(!ok) {
		return false;
	}

	double normMax = parameters.at(3).toDouble(&ok);
	if(!ok) {
		return false;
	}

	setMin(min);
	setMax(max);
	setNormalizedMin(normMin);
	setNormalizedMax(normMax);

	return true;
}


QString  NormalizedDoubleValue::getProperties() const {
	return QString::number(getMin()).append(",").append(QString::number(getMax()))
		.append(",").append(QString::number(getNormalizedMin())).append(",")
		.append(QString::number(getNormalizedMax()));
}

bool NormalizedDoubleValue::equals(const Value *value) const {
	if(!DoubleValue::equals(value)) {
		return false;
	}
	const NormalizedDoubleValue *other = dynamic_cast<const NormalizedDoubleValue*>(value);
	if(other == 0) {
		return false;
	}
	if(Math::compareDoubles(mMin, other->mMin) == false 
		|| Math::compareDoubles(mMax, other->mMax) == false
		|| Math::compareDoubles(mNormalizedMin, other->mNormalizedMin) == false
		|| Math::compareDoubles(mNormalizedMax, other->mNormalizedMax) == false) 
	{
		return false;
	}
	return true;
}

}





