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

#include "InternalStateValue.h"

namespace nerd {
	
	InternalStateValue::InternalStateValue(const QString &name)
	: SimObject(name), mInternalStateValue(0)
	{
		mInternalStateValue = new NormalizedDoubleValue(0.0, 0.0, 1.0, 0.0, 1.0);
		//mInitialValue = new DoubleValue(0.0);
		mValueRange = new RangeValue(0.0, 1.0);
		
		addParameter("State", mInternalStateValue);
		//addParameter("InitialValue", mInitialValue);
		addParameter("Range", mValueRange);
	}
	
	InternalStateValue::InternalStateValue(const InternalStateValue &other)
		: Object(), ValueChangedListener(), SimObject(other)
	{
		mInternalStateValue = dynamic_cast<InterfaceValue*>(getParameter("InternalStateValue"));
		//mInitialValue = dynamic_cast<DoubleValue*>(getParameter("InitialValue"));
		mValueRange = dynamic_cast<RangeValue*>(getParameter("Range"));
	}
	
	InternalStateValue::~InternalStateValue() {
	}
	
	
	SimObject* InternalStateValue::createCopy() const {
		return new InternalStateValue(*this);
	}

	
	void InternalStateValue::setup() {
		SimObject::setup();
		//mInternalStateValue->set(mInitialValue->get());
	}
	
	
	void InternalStateValue::clear() {
		SimObject::clear();
	}
	
	void InternalStateValue::valueChanged(Value *value) {
		SimObject::valueChanged(value);
		if(value == 0) {
			return;
		}
		else if(value == mValueRange) {
			mInternalStateValue->setMin(mValueRange->getMin());
			mInternalStateValue->setMax(mValueRange->getMax());
		}
	}
	
	NormalizedDoubleValue* InternalStateValue::getInternalStateValue() const {
		return mInternalStateValue;
	}
	
	
	
}


