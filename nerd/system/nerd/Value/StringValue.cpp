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


#include "StringValue.h"

namespace nerd {

StringValue::StringValue() : Value ("String", false) {
	mValue = "";
}

StringValue::StringValue(const QString &value) : Value("String", false) {
	mValue = value;
}

StringValue::StringValue(const StringValue& rhs) : Object(), Value(rhs) {
	mValue = rhs.mValue;
}


StringValue::~StringValue() {

}

void StringValue::set(const QString &value) {
	if(mNotifyAllSetAttempts || value != mValue) {
		mValue = value;
		notifyValueChanged();
	}
}

QString StringValue::get() const {
	return mValue;
}

QString StringValue::getValueAsString() const {
	return mValue;
}

bool StringValue::setValueFromString(const QString &value) {
	set(value);
	return true;
}

Value* StringValue::createCopy() {
	return new StringValue(*this);
}

bool StringValue::equals(const Value *value) const {
	if(!Value::equals(value)) {
		return false;
	}
	const StringValue *other = dynamic_cast<const StringValue*>(value);
	if(other == 0) {
		return false;
	}
	if(mValue != other->mValue) {
		return false;
	}
	return true;
}

}

