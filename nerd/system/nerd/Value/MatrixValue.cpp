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



#include "MatrixValue.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QString>
#include <QTextStream>

using namespace std;

namespace nerd {


/**
 * Constructs a new MatrixValue.
 */
MatrixValue::MatrixValue()
	: Value("Matrix", false)
{
}

MatrixValue::MatrixValue(int width, int height, int depth)
	: Value("Matrix", false), mValue(width, height, depth)
{

}


/**
 * Copy constructor. 
 * 
 * @param other the MatrixValue object to copy.
 */
MatrixValue::MatrixValue(const MatrixValue &other) 
	: Value(other), mValue(other.mValue)
{
}

/**
 * Destructor.
 */
MatrixValue::~MatrixValue() {
}


Value* MatrixValue::createCopy() {
	return new MatrixValue(*this);
}

void MatrixValue::resize(int w, int h, int d) {
	mValue.resize(w, h, d);
	fill(0);
}

int MatrixValue::getMatrixWidth() const {
	return mValue.getWidth();
}

int MatrixValue::getMatrixHeight() const {
	return mValue.getHeight();
}

int MatrixValue::getMatrixDepth() const {
	return mValue.getDepth();
}


void MatrixValue::set(const Matrix &matrix) {
	mValue = matrix;
	notifyValueChanged();
}

void MatrixValue::set(double value, int w, int h, int d) {
	mValue.set(value, w, h, d);
	notifyValueChanged();
}

Matrix MatrixValue::get() const {
	return mValue;
}


double MatrixValue::get(int w, int h, int d) {
	return mValue.get(w, h, d);
}


bool MatrixValue::setValueFromString(const QString&) {
	//TODO
	Core::log("MatrixValue: This value can NOT be set from a string yet!", true);
	return false;
}

QString MatrixValue::getValueAsString() const {
	QString valueString;
	QTextStream out(&valueString);

	out << "{";
	QVector<QVector<QVector<double> > > data = mValue.getData();
	for(int i = 0; i < data.size(); ++i) {
		out << "{";
		QVector<QVector<double> > v1 = data.at(i);
		for(int j = 0; j < v1.size(); ++j) {
			out << "{";
			QVector<double> v2 = v1.at(j);
			for(int k = 0; k < v2.size(); ++k) {
				out << QString::number(v2.at(k));
				if(k < (v2.size() - 1)) {
					out << ",";
				}
			}
			out << "}";
		}
		out << "}";
	}
	out << "}";
	return valueString;
}

void MatrixValue::fill(double value) {
	mValue.fill(value);
}


void MatrixValue::clear() {
	mValue.clear();
}

QVector<QVector<QVector<double> > > MatrixValue::getData() const {
	return mValue.getData();
}


bool MatrixValue::equals(const Value *value) const {
	if(!Value::equals(value)) {
		return false;
	}
	const MatrixValue *other = dynamic_cast<const MatrixValue*>(value);
	if(other == 0) {
		return false;
	}
	//TODO check equality
	return true;
}



}



