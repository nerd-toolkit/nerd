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


#include "DoubleValue.h"
#include "Math/Math.h"

namespace nerd {

DoubleValue::DoubleValue() : Value("Double", false) {
	mValue = 0.0;
}

DoubleValue::DoubleValue(double value) : Value("Double", false) {
	mValue = value;
}

DoubleValue::DoubleValue(const DoubleValue &rhs) : Object(), Value(rhs) {
	mValue = rhs.mValue;
}

DoubleValue::~DoubleValue() {
}

void DoubleValue::set(double value) {
	if(mValue != value || mNotifyAllSetAttempts) {
		mValue = value;
		notifyValueChanged();
	}
}

double DoubleValue::get() const {
	return mValue;
}

bool DoubleValue::setValueFromString(const QString &value) {
	bool ok = true;
	double d = value.toDouble(&ok);
	if(!ok) {
		return false;
	}
	else {
		set(d);
	}
	return true;
}

QString DoubleValue::getValueAsString() const {
	return QString::number(mValue, 'g', 15);
}

Value* DoubleValue::createCopy() {
	return new DoubleValue(*this);
}

bool DoubleValue::equals(const Value *value) const {
	if(!Value::equals(value)) {
		return false;
	}
	const DoubleValue *other = dynamic_cast<const DoubleValue*>(value);
	if(other == 0) {
		return false;
	}
	if(Math::compareDoubles(mValue, other->mValue) == false) {
		return false;
	}
	return true;
}


}


