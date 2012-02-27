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

#include "Vector3DValue.h"
#include <QStringList>

namespace nerd {

Vector3DValue::Vector3DValue() : Value("Vector3D", false), mSettingValue(false) {
	mVectorValues.append(new DoubleValue(0.0));
	mVectorValues.append(new DoubleValue(0.0));
	mVectorValues.append(new DoubleValue(0.0));

	for(int i = 0; i < mVectorValues.size(); ++i) {
		mVectorValues.at(i)->addValueChangedListener(this);
	}
}

Vector3DValue::Vector3DValue(double x, double y, double z) : Value("Vector3D", false), mSettingValue(false) {
	mVectorValues.append(new DoubleValue(x));
	mVectorValues.append(new DoubleValue(y));
	mVectorValues.append(new DoubleValue(z));

	for(int i = 0; i < mVectorValues.size(); ++i) {
		mVectorValues.at(i)->addValueChangedListener(this);
	}
}


Vector3DValue::Vector3DValue(const Vector3DValue &value)
		: Object(), MultiPartValue(), Value(value), mSettingValue(false)
{
	mVectorValues.append(new DoubleValue(value.getX()));
	mVectorValues.append(new DoubleValue(value.getY()));
	mVectorValues.append(new DoubleValue(value.getZ()));

	for(int i = 0; i < mVectorValues.size(); ++i) {
		mVectorValues.at(i)->addValueChangedListener(this);
	}
}


Vector3DValue::~Vector3DValue() {
	for(int i = 0; i < mVectorValues.size(); ++i) {
		mVectorValues[i]->removeValueChangedListener(this);
		delete mVectorValues[i];
	}
	mVectorValues.clear();
}

Value* Vector3DValue::createCopy() {
	return new Vector3DValue(*this);
}


void Vector3DValue::valueChanged(Value *value) {
	if(dynamic_cast<DoubleValue*>(value) == 0) {
		return;
	}
	if(mVectorValues.contains(dynamic_cast<DoubleValue*>(value)) && !mSettingValue) {
		notifyValueChanged();
	}
}

QString Vector3DValue::getName() const {
	return Value::getName();
}


void Vector3DValue::set(const Vector3D &vector) {
	if(mNotifyAllSetAttempts
		|| mVectorValues[0]->get() != vector.getX()
		|| mVectorValues[1]->get() != vector.getY()
		|| mVectorValues[2]->get() != vector.getZ())
	{
		mSettingValue = true;
		mVectorValues[0]->set(vector.getX());
		mVectorValues[1]->set(vector.getY());
		mVectorValues[2]->set(vector.getZ());
		mSettingValue = false;
		notifyValueChanged();
	}
}

void Vector3DValue::set(double x, double y, double z) {
	if(mNotifyAllSetAttempts
		|| x != mVectorValues[0]->get()
		|| y != mVectorValues[1]->get()
		|| z != mVectorValues[2]->get())
	{
		mSettingValue = true;
		mVectorValues[0]->set(x);
		mVectorValues[1]->set(y);
		mVectorValues[2]->set(z);
		mSettingValue = false;
		notifyValueChanged();
	}
}

Vector3D Vector3DValue::get() const {
	return Vector3D(mVectorValues[0]->get(),
					mVectorValues[1]->get(),
					mVectorValues[2]->get());
}

double Vector3DValue::getX() const {
	return mVectorValues[0]->get();
}

double Vector3DValue::getY() const {
	return mVectorValues[1]->get();
}

double Vector3DValue::getZ() const {
	return mVectorValues[2]->get();
}

bool Vector3DValue::setValueFromString(const QString &value) {
	if(value.size() < 7 || value.at(0) != '(' || value.at(value.size()-1) != ')'){
		return false;
	}

	//Obtain the strings representing the 3 doubles of the vector.
	QStringList values = value.mid(1, value.size() - 2).split(",");
	if(values.size() != 3) {
		return false;
	}

	//Convert strings to doubles.
	bool ok = true;
	double x = values.at(0).toDouble(&ok);
	if(!ok) {
		return false;
	}
	double y = values.at(1).toDouble(&ok);
	if(!ok) {
		return false;
	}
	double z = values.at(2).toDouble(&ok);
	if(!ok) {
		return false;
	}

	//set the new vector.
	set(x, y, z);
	return true;

}

QString Vector3DValue::getValueAsString() const {
	QString value = "(";
	value.append(QString::number(mVectorValues[0]->get(), 'g', 8)).append(",")
		.append(QString::number(mVectorValues[1]->get(), 'g', 8)).append(",")
		.append(QString::number(mVectorValues[2]->get(), 'g', 8)).append(")");
	return value;
}

int Vector3DValue::getNumberOfValueParts() const {
	return 3;
}

Value* Vector3DValue::getValuePart(int index) const {
	if(index < 0 || index >= mVectorValues.size()) {
		return 0;
	}
	return mVectorValues[index];
}

QString Vector3DValue::getValuePartName(int index) const {
	switch(index) {
		case 0:
			return "x";
			break;
		case 1:
			return "y";
			break;
		case 2:
			return "z";
			break;
		default:
			return "";
	};
}

bool Vector3DValue::equals(const Value *value) const {
	if(!Value::equals(value)) {
		return false;
	}
	const Vector3DValue *other = dynamic_cast<const Vector3DValue*>(value);
	if(other == 0) {
		return false;
	}
	if(mVectorValues.size() != other->mVectorValues.size()) {
		return false;
	}
	for(int i = 0; i < mVectorValues.size(); ++i) {
		if(mVectorValues.at(i)->equals(other->mVectorValues.at(i)) == false) {
			return false;
		}
	}
	return true;
}

}



