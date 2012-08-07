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


#include "CodeValue.h"

namespace nerd {

CodeValue::CodeValue() 
	: StringValue ()
{
	setTypeName("Code");
	mValue = "";
}

CodeValue::CodeValue(const QString &value) 
	: StringValue(value)
{
	setTypeName("Code");
}

CodeValue::CodeValue(const CodeValue& rhs) 
	: Object(), StringValue(rhs)
{
	setTypeName("Code");
}


CodeValue::~CodeValue() {

}

void CodeValue::set(const QString &value) {
	if(mNotifyAllSetAttempts || value != mValue) {
		mValue = value;
		notifyValueChanged();
	}
}

QString CodeValue::get() const {
	return mValue;
}

QString CodeValue::getValueAsString() const {
	QString singleLine = mValue;
	singleLine = singleLine.replace("\n", "/**/");
	return singleLine;
}

bool CodeValue::setValueFromString(const QString &value) {
	QString multiLine = value;
	multiLine = multiLine.replace("/**/", "\n");
	set(multiLine);
	return true;
}

Value* CodeValue::createCopy() {
	return new CodeValue(*this);
}

bool CodeValue::equals(const Value *value) const {
	if(!Value::equals(value)) {
		return false;
	}
	const CodeValue *other = dynamic_cast<const CodeValue*>(value);
	if(other == 0) {
		return false;
	}
	if(mValue != other->mValue) {
		return false;
	}
	return true;
}

}

