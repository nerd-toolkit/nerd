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


#include "CodeValue.h"

namespace nerd {

CodeValue::CodeValue() 
	: StringValue(), mErrorValue(0)
{
	setTypeName("SourceCode");
}

CodeValue::CodeValue(const QString &value) 
	: StringValue(value), mErrorValue(0)
{
	setTypeName("SourceCode");
}

CodeValue::CodeValue(const CodeValue& rhs) 
	: Object(), StringValue(rhs), mErrorValue(0)
{
	//Do NOT copy mErrorValue object
	setTypeName("SourceCode");
}


CodeValue::~CodeValue() {
	//ignore mErrorValue because that value is usually managed elsewhere...
}

void CodeValue::set(const QString &value) {
	StringValue::set(value);
}

QString CodeValue::get() const {
	return StringValue::get();
}

QString CodeValue::getValueAsString() const {
	QString singleLine = mValue;
	singleLine = singleLine.replace("/**/", "/***/");
	singleLine = singleLine.replace("\n", "/**/");
	return singleLine;
}

bool CodeValue::setValueFromString(const QString &value) {
	QString multiLine = value;
	multiLine = multiLine.replace("/**/", "\n");
	multiLine = multiLine.replace("/***/", "/**/");
	set(multiLine);
	return true;
}

Value* CodeValue::createCopy() {
	return new CodeValue(*this);
}

bool CodeValue::equals(const Value *value) const {
	return StringValue::equals(value);
}

/**
 * Allows to add an optional StringValue that contains the error stream of an 
 * executed code (e.g. a script in the QScriptEngine)
 */
void CodeValue::setErrorValue(StringValue *errorValue) {
	mErrorValue = errorValue;
}

StringValue* CodeValue::getErrorValue() const {
	return mErrorValue;
}

}

