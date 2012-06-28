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

#include "BoolValue.h"

namespace nerd {

BoolValue::BoolValue() : Value("Bool", false) {
	mValue = true;
	mOptionList.append("true");
    mOptionList.append("false");
}

BoolValue::BoolValue(bool value) : Value("Bool", false) {
	mValue = value;
	mOptionList.append("true");
    mOptionList.append("false");
}


BoolValue::BoolValue(const BoolValue& rhs) : Object(), Value(rhs) {
	mValue = rhs.mValue;
}

BoolValue::~BoolValue() {
}

Value* BoolValue::createCopy() {
	return new BoolValue(*this);
}

void BoolValue::set(bool value) {
	if(mNotifyAllSetAttempts || value != mValue) {
		mValue = value;
		notifyValueChanged();
	}
}

bool BoolValue::get() const {
	return mValue;
}

QString BoolValue::getValueAsString() const {
	if(mValue) {
		return "T";
	}
	else {
		return "F";
	}
}

bool BoolValue::setValueFromString(const QString &value) {
	if(value == "1" || value.toLower() == "t" || value.toLower() == "true") {
		set(true);
		return true;
	}
	if(value == "0" || value.toLower() == "f" || value.toLower() == "false") {
		set(false);
		return true;
	}
	return false;
}

bool BoolValue::equals(const Value *value) const {
	if(!Value::equals(value)) {
		return false;
	}
	const BoolValue *bv = dynamic_cast<const BoolValue*>(value);
	if(bv == 0) {
		return false;
	}
	if(mValue != bv->mValue) {
		return false;
	}
	return true;
}

}


