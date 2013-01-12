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

#include "QuaternionValue.h"
#include <QStringList>
#include <math.h>
#include "Math/Math.h"

namespace nerd{

QuaternionValue::QuaternionValue() : Value("Quaternion", false), mSettingValue(false){
	mQuaternion.append(new DoubleValue(1.0));
	mQuaternion.append(new DoubleValue(0.0));
	mQuaternion.append(new DoubleValue(0.0));
	mQuaternion.append(new DoubleValue(0.0));

	for(int i = 0; i < mQuaternion.size(); ++i) {
		mQuaternion.at(i)->addValueChangedListener(this);
	}
}

QuaternionValue::QuaternionValue(double w, double x, double y, double z) 
		: Value("Quaternion", false), mSettingValue(false)
{
	mQuaternion.append(new DoubleValue(w));
	mQuaternion.append(new DoubleValue(x));
	mQuaternion.append(new DoubleValue(y));
	mQuaternion.append(new DoubleValue(z));

	for(int i = 0; i < mQuaternion.size(); ++i) {
		mQuaternion.at(i)->addValueChangedListener(this);
	}
}

QuaternionValue::QuaternionValue(const QuaternionValue& value)
		: Object(), MultiPartValue(), Value(value), mSettingValue(false)
{
	mQuaternion.append(new DoubleValue(value.getW()));
	mQuaternion.append(new DoubleValue(value.getX()));
	mQuaternion.append(new DoubleValue(value.getY()));
	mQuaternion.append(new DoubleValue(value.getZ()));

	for(int i = 0; i < mQuaternion.size(); ++i) {
		mQuaternion.at(i)->addValueChangedListener(this);
	}
}

QuaternionValue::~QuaternionValue() {
	for(int i = 0; i < mQuaternion.size(); ++i) {
		mQuaternion[i]->removeValueChangedListener(this);
		delete mQuaternion[i];
	}
	mQuaternion.clear();
}

Value* QuaternionValue::createCopy() {
	return new QuaternionValue(*this);
}

void QuaternionValue::valueChanged(Value *value) {
	if(dynamic_cast<DoubleValue*>(value) == 0) {
		return;
	}
	if(!mSettingValue && mQuaternion.contains(dynamic_cast<DoubleValue*>(value))) {
		notifyValueChanged();
	}
}


QString QuaternionValue::getName() const {
	return Value::getName();
}


void QuaternionValue::set(double w, double x, double y, double z) {
	if(mNotifyAllSetAttempts
		|| w != mQuaternion[0]->get()
		|| x != mQuaternion[1]->get()
		|| y != mQuaternion[2]->get()
		||  z != mQuaternion[3]->get())
	{
		mSettingValue = true;
		mQuaternion[0]->set(w);
		mQuaternion[1]->set(x);
		mQuaternion[2]->set(y);
		mQuaternion[3]->set(z);
		mSettingValue = false;
		notifyValueChanged();
	}
}

void QuaternionValue::set(const Quaternion &quat) {
	set(quat.getW(), quat.getX(), quat.getY(), quat.getZ());
}


Quaternion QuaternionValue::get() const {
	return Quaternion(mQuaternion[0]->get(),
					mQuaternion[1]->get(),
					mQuaternion[2]->get(),
					mQuaternion[3]->get());
}

double QuaternionValue::getW() const {
	return mQuaternion[0]->get();
}

double QuaternionValue::getX() const {
	return mQuaternion[1]->get();
}

double QuaternionValue::getY() const {
	return mQuaternion[2]->get();
}

double QuaternionValue::getZ() const {
	return mQuaternion[3]->get();
}

QString QuaternionValue::getValueAsString() const {
	QString value = "(";
	value.append(QString::number(mQuaternion[0]->get(), 'g', 8)).append(",")
		.append(QString::number(mQuaternion[1]->get(), 'g', 8)).append(",")
		.append(QString::number(mQuaternion[2]->get(), 'g', 8)).append(",")
		.append(QString::number(mQuaternion[3]->get(), 'g', 8)).append(")");
	return value;
}

bool QuaternionValue::setValueFromString(const QString &value) {
	if(value.size() < 9 || value.at(0) != '(' || value.at(value.size()-1) != ')') {
		return false;
	}

	QStringList values = value.mid(1, value.size() - 2).split(",");

	if(values.size() != 4) {
		return false;
	}

	bool ok = true;
	double w = values.at(0).toDouble(&ok);
	if(!ok) {
		return false;
	}
	double x = values.at(1).toDouble(&ok);
	if(!ok) {
		return false;
	}
	double y = values.at(2).toDouble(&ok);
	if(!ok) {
		return false;
	}
	double z = values.at(3).toDouble(&ok);
	if(!ok) {
		return false;
	}

	set(w,x,y,z);
	return true;
}

/**
 * Converts the given angles into a quaternion.
 * @param alpha: rotation angle around the x-axis in degree
 * @param beta: rotation angle around the y-axis in degree
 * @param gamma: rotation angle around the z-axis in degree
 */
void QuaternionValue::setFromAngles(double alpha, double beta, double gamma) {

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
Vector3D QuaternionValue::toAngles() {

	double w = mQuaternion[0]->get();
	double x = mQuaternion[1]->get();
	double y = mQuaternion[2]->get();
	double z = mQuaternion[3]->get();

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


int QuaternionValue::getNumberOfValueParts() const {
	return 4;
}


Value* QuaternionValue::getValuePart(int index) const {
	if(index < 0 || index >= 4) {
		return 0;
	}
	return mQuaternion[index];
}


QString QuaternionValue::getValuePartName(int index) const {
	switch(index) {
		case 0:
			return "w";
			break;
		case 1:
			return "x";
			break;
		case 2:
			return "y";
			break;
		case 3:
			return "z";
			break;
		default:
			return "";
	};
}

bool QuaternionValue::equals(const Value *value) const {
	if(!Value::equals(value)) {
		return false;
	}
	const QuaternionValue *other = dynamic_cast<const QuaternionValue*>(value);
	if(other == 0) {
		return false;
	}
	if(mQuaternion.size() != other->mQuaternion.size()) {
		return false;
	}
	for(int i = 0; i < mQuaternion.size(); ++i) {
		if(mQuaternion.at(i)->equals(other->mQuaternion.at(i)) == false) {
			return false;
		}
	}
	return true;
}

}
