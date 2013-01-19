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

#include "RangeValue.h"

#include <QStringList>

namespace nerd {
	
	RangeValue::RangeValue(bool validateRange) : Value("Range", false), mValidateRange(validateRange) {
	}
	
	RangeValue::RangeValue(double min, double max, bool validateRange) : Value("Range", false), mValidateRange(validateRange) {
		if(validateRange && (min > max)) {
			//make sure the range is valid.
			mValue.set(max, min);
		}
		else {
			mValue.set(min, max); 
		}
	}
	
	
	RangeValue::RangeValue(const RangeValue& other) : Object(), Value(other), mValidateRange(other.mValidateRange) {
		mValue.set(other.getMin(), other.getMax());
	}
	
	RangeValue::~RangeValue() {
	}
	
	Value* RangeValue::createCopy() {
		return new RangeValue(*this);
	}
	
	void RangeValue::set(double min, double max) {
		//do not change if min > max.
		if(mValidateRange && (min > max)) {
			return;
		}
		if(mNotifyAllSetAttempts || mValue.getMin() != min || mValue.getMax() != max) {
			mValue.set(min, max);
			notifyValueChanged();
		}
	}
	
	
	Range RangeValue::get() const {
		return mValue;
	}
	
	
	double RangeValue::getMin() const {
		return mValue.getMin();
	}
	
	
	double RangeValue::getMax() const {
		return mValue.getMax();
	}
	
	
	QString RangeValue::getValueAsString() const {
		if(mValue.getMin() == mValue.getMax()) {
			return QString::number(mValue.getMin());
		}
		else {
			return QString::number(mValue.getMin()) + "," + QString::number(mValue.getMax());
		}
	}
	
	bool RangeValue::setValueFromString(const QString &value) {
		QStringList components = value.split(",");
		
		if(components.size() < 1 || components.size() > 2) {
			return false;
		}
		bool ok = true;
		double min = components[0].toDouble(&ok);
		if(!ok) {
			return false;
		}
		if(components.size() == 1) {
			set(min, min);
			return true;
		}
		
		double max = components[1].toDouble(&ok);
		if(!ok) {
			return false;
		}
		if(mValidateRange && (min > max)) {
			return false;
		}
		set(min, max);
		return true;
	}
	
	bool RangeValue::equals(const Value *value) const {
		if(!Value::equals(value)) {
			return false;
		}
		const RangeValue *rv = dynamic_cast<const RangeValue*>(value);
		if(rv == 0) {
			return false;
		}
		if(!mValue.equals(rv->mValue)) {
			return false;
		}
		return true;
	}
	
}


