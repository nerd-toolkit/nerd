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

#include "ListSwitchControlParameter.h"
#include "Core/Core.h"
#include <QStringList>

namespace nerd {

ListSwitchControlParameter::ListSwitchControlParameter(const QString &name)
	: ControlParameter(name), mSwitchInterval(0), 
	  mValueList(0), mCurrentStep(0)
{
	mSwitchInterval = new IntValue(1000);
	mValueList = new StringValue("{0.0, 1.0}");

	addParameter("SwitchInterval", mSwitchInterval);
	addParameter("ValueList", mValueList);
}


ListSwitchControlParameter::ListSwitchControlParameter(const ListSwitchControlParameter &other) 
	: Object(), ValueChangedListener(), SimSensor(), ControlParameter(other), 
	  mSwitchInterval(0), mValueList(0), mCurrentStep(0)
{
	mSwitchInterval = dynamic_cast<IntValue*>(getParameter("SwitchInterval"));
	mValueList = dynamic_cast<StringValue*>(getParameter("ValueList"));
}

ListSwitchControlParameter::~ListSwitchControlParameter() {
}

SimObject* ListSwitchControlParameter::createCopy() const {
	return new ListSwitchControlParameter(*this);
}

		
void ListSwitchControlParameter::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm*) {

}


void ListSwitchControlParameter::setup() {
	ControlParameter::setup();
	mCurrentStep = 0;

	bool ok = true;
	
	QString values = mValueList->get();
	if(values.startsWith("{") && values.endsWith("}")) {
		QStringList numbers = values.mid(1, values.size() - 2).split(",");
		mValues.clear();
		for(int i = 0; i < numbers.size(); ++i) {
			bool doubleOk = true;
			double val = numbers.at(i).toDouble(&doubleOk);
			if(doubleOk) {
				mValues.append(val);
			}
			else {
				ok = false;
				break;
			}
		}
	}
	else {
		ok = false;
	}
	if(!ok) {
		Core::log(QString("ListSwitchControlParameter: Could not read input list [")
				.append(mValueList->get()).append("]."));
	}
}


void ListSwitchControlParameter::clear() {
	ControlParameter::clear();
	mCurrentStep = 0;
}


void ListSwitchControlParameter::updateSensorValues() {
	mCurrentStep++;
	if(mAutoControl->get()) {
		if(mSwitchInterval->get() == 0) {
			mSwitchInterval->set(1);
		}
		int index = mCurrentStep / mSwitchInterval->get();
		if(mValues.size() > 0) {
			mControlParameter->set(mValues.value(index % mValues.size()));
		}
	}
}


}


