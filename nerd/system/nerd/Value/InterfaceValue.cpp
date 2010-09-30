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



#include "InterfaceValue.h"
#include <QStringList>
#include <iostream>

using namespace std;

namespace nerd {

InterfaceValue::InterfaceValue(const QString &prefix, const QString &name) 
	: NormalizedDoubleValue(), mInterfaceName(name), mPrefix(prefix)
{
	setTypeName("Interface");
}

InterfaceValue::InterfaceValue(const QString &prefix, const QString &name, 
							   double value, double min, double max)
	: NormalizedDoubleValue(value, min, max), mInterfaceName(name), mPrefix(prefix)
{
	setTypeName("Interface");
}

InterfaceValue::InterfaceValue(const QString &prefix, const QString &name, 
							   double value, double min, double max, 
							   double normalizedMin, double normalizedMax)
	: NormalizedDoubleValue(value, min, max, normalizedMin, normalizedMax), 
	  mInterfaceName(name), mPrefix(prefix)
{
	setTypeName("Interface");
}

InterfaceValue::InterfaceValue(const InterfaceValue &value)
	: Object(), NormalizedDoubleValue(value), 
	  mInterfaceName(value.mInterfaceName), mPrefix(value.mPrefix)
{
	set(value.get());
}


InterfaceValue::~InterfaceValue() {
}

Value* InterfaceValue::createCopy() {
	return new InterfaceValue(*this);
}

QString InterfaceValue::getName() const {
	if(mPrefix == "") {
		return mInterfaceName;
	}else {
		return mPrefix + "/" + mInterfaceName;
	}
}

QString InterfaceValue::getInterfaceName() const {
	return mInterfaceName;
}

void InterfaceValue::setInterfaceName(const QString &name) {
	if(mInterfaceName.compare(name) != 0) {
		mInterfaceName = name;
	}
}

QString InterfaceValue::getPrefix() const {
	return mPrefix;
}

void InterfaceValue::setPrefix(const QString &prefix) {
	if(mPrefix.compare(prefix) != 0) {
		mPrefix = prefix;
	}
}

bool InterfaceValue::setProperties(const QString &properties) {

	QStringList parameters = properties.split(",");

	if(parameters.size() != 6) {
		return false;
	}

	bool ok = true;
	double min = parameters.at(2).toDouble(&ok);
	if(!ok) {
		return false;
	}

	double max = parameters.at(3).toDouble(&ok);
	if(!ok) {
		return false;
	}

	double normMin = parameters.at(4).toDouble(&ok);
	if(!ok) {
		return false;
	}

	double normMax = parameters.at(5).toDouble(&ok);
	if(!ok) {
		return false;
	}

	mPrefix = parameters.at(0);
	mInterfaceName = parameters.at(1);
	setMin(min);
	setMax(max);
	setNormalizedMin(normMin);
	setNormalizedMax(normMax);

	return true;
}


QString InterfaceValue::getProperties() const {
	return QString(mPrefix).append(",").append(mInterfaceName).append(",")
		.append(QString::number(getMin())).append(",").append(QString::number(getMax()))
		.append(",").append(QString::number(getNormalizedMin())).append(",")
		.append(QString::number(getNormalizedMax()));
}

bool InterfaceValue::equals(const Value *value) const {
	if(!NormalizedDoubleValue::equals(value)) {
		return false;
	}
	const InterfaceValue *other = dynamic_cast<const InterfaceValue*>(value);
	if(other == 0) {
		return false;
	}
	if(mInterfaceName != other->mInterfaceName) {
		return false;
	}
	if(mPrefix != other->mPrefix) {
		return false;
	}
	return true;
}

}





