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


#include "FileNameValue.h"

namespace nerd {

FileNameValue::FileNameValue() 
	: StringValue ()
{
	setTypeName("FileName");
}

FileNameValue::FileNameValue(const QString &value) 
	: StringValue(value)
{
	setTypeName("FileName");
}

FileNameValue::FileNameValue(const FileNameValue& rhs) 
	: Object(), StringValue(rhs)
{
	setTypeName("FileName");
}


FileNameValue::~FileNameValue() {

}

void FileNameValue::set(const QString &value) {
	StringValue::set(value);
	//TODO do some checks for validity?
}

QString FileNameValue::get() const {
	return StringValue::get();
}

QString FileNameValue::getValueAsString() const {
	return StringValue::getValueAsString();
}

bool FileNameValue::setValueFromString(const QString &value) {
	//TODO do some checks for validity?
	set(value);
	return true;
}

Value* FileNameValue::createCopy() {
	return new FileNameValue(*this);
}

bool FileNameValue::equals(const Value *value) const {
	if(!Value::equals(value)) {
		return false;
	}
	const FileNameValue *other = dynamic_cast<const FileNameValue*>(value);
	if(other == 0) {
		return false;
	}
	if(mValue != other->mValue) {
		return false;
	}
	return true;
}

}

