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

#include "Matrix3x3Value.h"
#include <QStringList>

namespace nerd {

Matrix3x3Value::Matrix3x3Value() : Value("Matrix3x3", false), mSettingValue(false) {
	mMatrixValues.append(new DoubleValue(0.0));
	mMatrixValues.append(new DoubleValue(0.0));
	mMatrixValues.append(new DoubleValue(0.0));
	mMatrixValues.append(new DoubleValue(0.0));
	mMatrixValues.append(new DoubleValue(0.0));
	mMatrixValues.append(new DoubleValue(0.0));
	mMatrixValues.append(new DoubleValue(0.0));
	mMatrixValues.append(new DoubleValue(0.0));
	mMatrixValues.append(new DoubleValue(0.0));

	for(int i = 0; i < mMatrixValues.size(); ++i) {
		mMatrixValues.at(i)->addValueChangedListener(this);
	}
}

Matrix3x3Value::Matrix3x3Value(double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33) : Value("Matrix3x3", false), mSettingValue(false) {

	mMatrixValues.append(new DoubleValue(m11));
	mMatrixValues.append(new DoubleValue(m12));
	mMatrixValues.append(new DoubleValue(m13));
	mMatrixValues.append(new DoubleValue(m21));
	mMatrixValues.append(new DoubleValue(m22));
	mMatrixValues.append(new DoubleValue(m23));
	mMatrixValues.append(new DoubleValue(m31));
	mMatrixValues.append(new DoubleValue(m32));
	mMatrixValues.append(new DoubleValue(m33));

	for(int i = 0; i < mMatrixValues.size(); ++i) {
		mMatrixValues.at(i)->addValueChangedListener(this);
	}
}


Matrix3x3Value::Matrix3x3Value(const Matrix3x3 &matrix) {

	mMatrixValues[0]->set(matrix.get(1, 1));
	mMatrixValues[1]->set(matrix.get(1, 2));
	mMatrixValues[2]->set(matrix.get(1, 3));
	mMatrixValues[3]->set(matrix.get(2, 1));
	mMatrixValues[4]->set(matrix.get(2, 2));
	mMatrixValues[5]->set(matrix.get(2, 3));
	mMatrixValues[6]->set(matrix.get(3, 1));
	mMatrixValues[7]->set(matrix.get(3, 2));
	mMatrixValues[8]->set(matrix.get(3, 3));

	for(int i = 0; i < mMatrixValues.size(); ++i) {
		mMatrixValues.at(i)->addValueChangedListener(this);
	}
}

Matrix3x3Value::Matrix3x3Value(const Matrix3x3Value &value)
		: Object(), ValueChangedListener(), MultiPartValue(), Value(value), mSettingValue(false)
{
	mMatrixValues.append(new DoubleValue(value.get(1, 1)));
	mMatrixValues.append(new DoubleValue(value.get(1, 2)));
	mMatrixValues.append(new DoubleValue(value.get(1, 3)));
	mMatrixValues.append(new DoubleValue(value.get(2, 1)));
	mMatrixValues.append(new DoubleValue(value.get(2, 2)));
	mMatrixValues.append(new DoubleValue(value.get(2, 3)));
	mMatrixValues.append(new DoubleValue(value.get(3, 1)));
	mMatrixValues.append(new DoubleValue(value.get(3, 2)));
	mMatrixValues.append(new DoubleValue(value.get(3, 3)));

	for(int i = 0; i < mMatrixValues.size(); ++i) {
		mMatrixValues.at(i)->addValueChangedListener(this);
	}
}


Matrix3x3Value::~Matrix3x3Value() {
	for(int i = 0; i < mMatrixValues.size(); ++i) {
		mMatrixValues[i]->removeValueChangedListener(this);
		delete mMatrixValues[i];
	}
	mMatrixValues.clear();
}

Value* Matrix3x3Value::createCopy() {
	return new Matrix3x3Value(*this);
}


void Matrix3x3Value::valueChanged(Value *value) {
	if(dynamic_cast<DoubleValue*>(value) == 0) {
		return;
	}
	if(mMatrixValues.contains(dynamic_cast<DoubleValue*>(value)) && !mSettingValue) {
		notifyValueChanged();
	}
}

QString Matrix3x3Value::getName() const {
	return Value::getName();
}


void Matrix3x3Value::set(const Matrix3x3 &matrix) {
	if(mNotifyAllSetAttempts
		|| mMatrixValues[0]->get() != matrix.get(1, 1)
		|| mMatrixValues[1]->get() != matrix.get(1, 2)
		|| mMatrixValues[2]->get() != matrix.get(1, 3)
		|| mMatrixValues[3]->get() != matrix.get(2, 1)
		|| mMatrixValues[4]->get() != matrix.get(2, 2)
		|| mMatrixValues[5]->get() != matrix.get(2, 3)
		|| mMatrixValues[6]->get() != matrix.get(3, 1)
		|| mMatrixValues[7]->get() != matrix.get(3, 2)
		|| mMatrixValues[8]->get() != matrix.get(3, 3))
	{
		mSettingValue = true;
		mMatrixValues[0]->set(matrix.get(1, 1));
		mMatrixValues[1]->set(matrix.get(1, 2));
		mMatrixValues[2]->set(matrix.get(1, 3));
		mMatrixValues[3]->set(matrix.get(2, 1));
		mMatrixValues[4]->set(matrix.get(2, 2));
		mMatrixValues[5]->set(matrix.get(2, 3));
		mMatrixValues[6]->set(matrix.get(3, 1));
		mMatrixValues[7]->set(matrix.get(3, 2));
		mMatrixValues[8]->set(matrix.get(3, 3));
		mSettingValue = false;
		notifyValueChanged();
	}
}

void Matrix3x3Value::set(double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33) {
	if(mNotifyAllSetAttempts
		|| mMatrixValues[0]->get() != m11
		|| mMatrixValues[1]->get() != m12
		|| mMatrixValues[2]->get() != m13
		|| mMatrixValues[3]->get() != m21
		|| mMatrixValues[4]->get() != m22
		|| mMatrixValues[5]->get() != m23
		|| mMatrixValues[6]->get() != m31
		|| mMatrixValues[7]->get() != m32
		|| mMatrixValues[8]->get() != m33)
	{
		mSettingValue = true;
		mMatrixValues[0]->set(m11);
		mMatrixValues[1]->set(m12);
		mMatrixValues[2]->set(m13);
		mMatrixValues[3]->set(m21);
		mMatrixValues[4]->set(m22);
		mMatrixValues[5]->set(m23);
		mMatrixValues[6]->set(m31);
		mMatrixValues[7]->set(m32);
		mMatrixValues[8]->set(m33);
		mSettingValue = false;
		notifyValueChanged();
	}
}

Matrix3x3 Matrix3x3Value::get() const {
	return Matrix3x3(mMatrixValues[0]->get(), mMatrixValues[1]->get(), mMatrixValues[2]->get(),
					mMatrixValues[3]->get(), mMatrixValues[4]->get(), mMatrixValues[5]->get(),
					mMatrixValues[6]->get(), mMatrixValues[7]->get(), mMatrixValues[8]->get());
}

double Matrix3x3Value::get(int row, int col) const {
	if((row <= 3) && (row >= 1) && (col <= 3) && (col >= 1)) {
		return mMatrixValues[(row-1)*3+col-1]->get();
	}
	return 0.0;
}

bool Matrix3x3Value::setValueFromString(const QString &value) {
	if(value.size() < 19 || value.at(0) != '(' || value.at(value.size()-1) != ')'){
		return false;
	}

	//Obtain the strings representing the rows.
	QStringList rows = value.mid(1, value.size() - 2).split(";");
	if(rows.size() != 3) {
		return false;
	}

	double values[9];
	int i = 0;

	QStringListIterator rowIterator(rows);
	while(rowIterator.hasNext()) {
		QString row = rowIterator.next();
		QStringList rowValues = row.split(",");
		if(rowValues.size() != 3) {
			return false;
		}
		QStringListIterator colIterator(rowValues);
		while(colIterator.hasNext()) {
			bool ok = true;
			values[i] = colIterator.next().toDouble(&ok);
			if(!ok) {
				return false;
			}
			i++;
		}
	}

	set(values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8]);

	return true;
}

QString Matrix3x3Value::getValueAsString() const {
	QString value = "(";
	value.append(QString::number(mMatrixValues[0]->get(), 'g', 8)).append(",")
		.append(QString::number(mMatrixValues[1]->get(), 'g', 8)).append(",")
		.append(QString::number(mMatrixValues[2]->get(), 'g', 8)).append(";")
		.append(QString::number(mMatrixValues[3]->get(), 'g', 8)).append(",")
		.append(QString::number(mMatrixValues[4]->get(), 'g', 8)).append(",")
		.append(QString::number(mMatrixValues[5]->get(), 'g', 8)).append(";")
		.append(QString::number(mMatrixValues[6]->get(), 'g', 8)).append(",")
		.append(QString::number(mMatrixValues[7]->get(), 'g', 8)).append(",")
		.append(QString::number(mMatrixValues[8]->get(), 'g', 8)).append(")");
	return value;
}

int Matrix3x3Value::getNumberOfValueParts() const {
	return 9;
}

Value* Matrix3x3Value::getValuePart(int index) const {
	if(index < 0 || index >= mMatrixValues.size()) {
		return 0;
	}
	return mMatrixValues[index];
}

QString Matrix3x3Value::getValuePartName(int index) const {
	switch(index) {
		case 0:
			return "m11";
			break;
		case 1:
			return "m12";
			break;
		case 2:
			return "m13";
			break;
		case 3:
			return "m21";
			break;
		case 4:
			return "m22";
			break;
		case 5:
			return "m23";
			break;
		case 6:
			return "m31";
			break;
		case 7:
			return "m32";
			break;
		case 8:
			return "m33";
			break;
		default:
			return "";
	};
}

bool Matrix3x3Value::equals(const Value *value) const {
	if(!Value::equals(value)) {
		return false;
	}
	const Matrix3x3Value *other = dynamic_cast<const Matrix3x3Value*>(value);
	if(other == 0) {
		return false;
	}
	if(mMatrixValues.size() != other->mMatrixValues.size()) {
		return false;
	}
	for(int i = 0; i < mMatrixValues.size(); ++i) {
		if(mMatrixValues.at(i)->equals(other->mMatrixValues.at(i)) == false) {
			return false;
		}
	}
	return true;
}

}



