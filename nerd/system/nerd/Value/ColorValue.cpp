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


#include "ColorValue.h"
#include <QStringList>
#include "Value/IntValue.h"

namespace nerd {


ColorValue::ColorValue()
	: Value("Color", false), mColor(Color(0,0,0,255)), mSettingValue(false)
{
	init();
}


ColorValue::ColorValue(const QString &color)
	: Value("Color", false), mColor(Color(0,0,0,255)), mSettingValue(false)
{
	init();
	setValueFromString(color);
}

ColorValue::ColorValue(const Color &color)
	: Value("Color", false), mColor(color), mSettingValue(false)
{
	init();
}


ColorValue::ColorValue(int red, int green, int blue, int alpha)
	: Value("Color", false), mSettingValue(false)
{
	mColor.set(red, green, blue, alpha);
	init();
}

ColorValue::ColorValue(const ColorValue &value)
	: Object(), Value(value), mColor(value.mColor), mSettingValue(false)
{
	mColorValues.append(new IntValue(mColor.red()));
	mColorValues.append(new IntValue(mColor.green()));
	mColorValues.append(new IntValue(mColor.blue()));
	mColorValues.append(new IntValue(mColor.alpha()));

	for(int i = 0; i < mColorValues.size(); ++i) {
		mColorValues.at(i)->addValueChangedListener(this);
	}
}


ColorValue::~ColorValue() {
	for(int i = 0; i < mColorValues.size(); ++i) {
		mColorValues.at(i)->removeValueChangedListener(this);
		delete mColorValues.at(i);
	}
	mColorValues.clear();
}

void ColorValue::init() {
    mColorValues.append(new IntValue(mColor.red()));
	mColorValues.append(new IntValue(mColor.green()));
	mColorValues.append(new IntValue(mColor.blue()));
	mColorValues.append(new IntValue(mColor.alpha()));

	for(int i = 0; i < mColorValues.size(); ++i) {
		mColorValues.at(i)->addValueChangedListener(this);
	}

	mOptionList.append("black");
	mOptionList.append("white");
	mOptionList.append("red");
	mOptionList.append("darkRed");
	mOptionList.append("green");
	mOptionList.append("darkGreen");
	mOptionList.append("blue");
	mOptionList.append("darkBlue");
	mOptionList.append("cyan");
	mOptionList.append("darkCyan");
	mOptionList.append("magenta");
	mOptionList.append("darkMagenta");
	mOptionList.append("yellow");
	mOptionList.append("darkYellow");
	mOptionList.append("gray");
	mOptionList.append("darkGray");
	mOptionList.append("lightGray");
	mOptionList.append("transparent");
}

QString ColorValue::getName() const {
	return Value::getName();
}


Value* ColorValue::createCopy() {
	return new ColorValue(*this);
}

void ColorValue::set(const Color &color) {
	if(!color.equals(mColor) || mNotifyAllSetAttempts) {
		mSettingValue = true;
		mColor = color;
		mColorValues[0]->set(mColor.red());
		mColorValues[1]->set(mColor.green());
		mColorValues[2]->set(mColor.blue());
		mColorValues[3]->set(mColor.alpha());
		mSettingValue = false;
		notifyValueChanged();
	}
}

const Color& ColorValue::get() const {
	return mColor;
}


QString ColorValue::getValueAsString() const {

	if(mColor.equals(Color(0,0,0,255))) {
		return "black";
	}
	else if(mColor.equals(Color(255,255,255,255))) {
		return "white";
	}
	else if(mColor.equals(Color(255,0,0,255))) {
		return "red";
	}
	else if(mColor.equals(Color(128,0,0,255))) {
		return "darkRed";
	}
	else if(mColor.equals(Color(0,255,0,255))) {
		return "green";
	}
	else if(mColor.equals(Color(0,128,0,255))) {
		return "darkGreen";
	}
	else if(mColor.equals(Color(0,0,255,255))) {
		return "blue";
	}
	else if(mColor.equals(Color(0,0,128,255))) {
		return "darkBlue";
	}
	else if(mColor.equals(Color(0,255,255,255))) {
		return "cyan";
	}
	else if(mColor.equals(Color(0,128,128,255))) {
		return "darkCyan";
	}
	else if(mColor.equals(Color(255,0,255,255))) {
		return "magenta";
	}
	else if(mColor.equals(Color(128,0,128,255))) {
		return "darkMagenta";
	}
	else if(mColor.equals(Color(255,255,0,255))) {
		return "yellow";
	}
	else if(mColor.equals(Color(128,128,0,255))) {
		return "darkYellow";
	}
	else if(mColor.equals(Color(160,160,160,255))) {
		return "gray";
	}
	else if(mColor.equals(Color(128,128,128,255))) {
		return "darkGray";
	}
	else if(mColor.equals(Color(192,192,192,255))) {
		return "lightGray";
	}
	else if(mColor.equals(Color(0,0,0,0))) {
		return "transparent";
	}
	else {
		QString value = "(";
		value.append(QString::number(mColor.red(), 'g', 8)).append(",")
			.append(QString::number(mColor.green(), 'g', 8)).append(",")
			.append(QString::number(mColor.blue(), 'g', 8)).append(",")
			.append(QString::number(mColor.alpha(), 'g', 8)).append(")");
		return value;
	}
}


bool ColorValue::setValueFromString(const QString &value) {
	if(value.toLower().compare("black") == 0) {
		set(0,0,0);
	}
	else if(value.toLower().compare("white") == 0) {
		set(255,255,255);
	}
	else if(value.toLower().compare("red") == 0) {
		set(255,0,0);
	}
	else if(value.toLower().compare("darkred") == 0) {
		set(128,0,0);
	}
	else if(value.toLower().compare("green") == 0) {
		set(0,255,0);
	}
	else if(value.toLower().compare("darkgreen") == 0) {
		set(0,128,0);
	}
	else if(value.toLower().compare("blue") == 0) {
		set(0,0,255);
	}
	else if(value.toLower().compare("darkblue") == 0) {
		set(0,0,128);
	}
	else if(value.toLower().compare("cyan") == 0) {
		set(0,255,255);
	}
	else if(value.toLower().compare("darkcyan") == 0) {
		set(0,128,128);
	}
	else if(value.toLower().compare("magenta") == 0) {
		set(255,0,255);
	}
	else if(value.toLower().compare("darkmagenta") == 0) {
		set(128,0,128);
	}
	else if(value.toLower().compare("yellow") == 0) {
		set(255,255,0);
	}
	else if(value.toLower().compare("darkyellow") == 0) {
		set(128,128,0);
	}
	else if(value.toLower().compare("gray") == 0) {
		set(160,160,160);
	}
	else if(value.toLower().compare("darkgray") == 0) {
		set(128,128,128);
	}
	else if(value.toLower().compare("lightgray") == 0) {
		set(192,192,192);
	}
	else if(value.toLower().compare("transparent") == 0) {
		set(0,0,0,0);
	}
	else {
		if(value.size() < 7 || value.at(0) != '(' || value.at(value.size()-1) != ')') {
// 			set(0,0,0,255);
			return false;
		}

		//Obtain the strings representing the 3 doubles of the vector.
		QStringList values = value.mid(1, value.size() - 2).split(",");
		bool ok = true;

		if(values.size() < 3 || values.size() > 4) {
			ok = false;
		}

		//Convert strings to ints.
		if(ok) {
			int w = values.at(0).toInt(&ok);
			if(!ok) {
				return ok;
			}
			int x = values.at(1).toInt(&ok);
			if(!ok) {
				return ok;
			}
			int y = values.at(2).toInt(&ok);
			if(!ok) {
				return ok;
			}
			int z = 255;
			if(values.size() == 4) {
				z = values.at(3).toInt(&ok);
			}
			if(ok) {
			//set the new vector.
			set(w, x, y, z);
			}
			return ok;
		}
		return ok;
	}
	return true;
}

void ColorValue::set(int red, int green, int blue, int alpha) {
	if(mNotifyAllSetAttempts || !mColor.equals(Color(red, green, blue, alpha))) {
		mSettingValue = true;
		mColor.set(red, green, blue, alpha);
		mColorValues[0]->set(mColor.red());
		mColorValues[1]->set(mColor.green());
		mColorValues[2]->set(mColor.blue());
		mColorValues[3]->set(mColor.alpha());
		mSettingValue = false;
		notifyValueChanged();
	}

}

void ColorValue::set(int red, int green, int blue) {
	if(mNotifyAllSetAttempts || !mColor.equals(Color(red, green, blue))) {
		mSettingValue = true;
		mColor.set(red, green, blue);
		mColorValues[0]->set(mColor.red());
		mColorValues[1]->set(mColor.green());
		mColorValues[2]->set(mColor.blue());
		mColorValues[3]->set(mColor.alpha());
		mSettingValue = false;
		notifyValueChanged();
	}
}

bool ColorValue::equals(const Value *value) const {
	if(!Value::equals(value)) {
		return false;
	}
	const ColorValue *cv = dynamic_cast<const ColorValue*>(value);
	if(cv == 0) {
		return false;
	}
	if(!mColor.equals(cv->mColor)) {
		return false;
	}
	return true;
}


void ColorValue::valueChanged(Value *value) {
	if(dynamic_cast<IntValue*>(value) == 0) {
		return;
	}
	if(mColorValues.contains(dynamic_cast<IntValue*>(value)) && !mSettingValue) {
		set(mColorValues[0]->get(), mColorValues[1]->get(), mColorValues[2]->get(), mColorValues[3]->get());
	}
}


int ColorValue::getNumberOfValueParts() const {
	return 4;
}


Value* ColorValue::getValuePart(int index) const {
	if(index < 0 || index >= mColorValues.size()) {
		return 0;
	}
	return mColorValues.at(index);
}


QString ColorValue::getValuePartName(int index) const {
	switch(index) {
		case 0:
			return "red";
			break;
		case 1:
			return "green";
			break;
		case 2:
			return "blue";
			break;
		case 3:
			return "alpha";
			break;
		default:
			return "";
	};
	return "";
}


}




