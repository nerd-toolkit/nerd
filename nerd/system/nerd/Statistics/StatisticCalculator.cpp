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

#include "StatisticCalculator.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"

namespace nerd {

StatisticCalculator::StatisticCalculator(const QString &name, const QString &valuePath)
	: mStartIndex(0), mName(name), mObservedValue(0)
{
	mLastSetValue = new DoubleValue(0.0);

	Core::getInstance()->getValueManager()
			->addValue(QString(valuePath).append(name), mLastSetValue);
}

StatisticCalculator::~StatisticCalculator() {
	ValueManager *vm = Core::getInstance()->getValueManager();

	vm->removeValue(mLastSetValue);
	delete mLastSetValue;
}

QString StatisticCalculator::getName() const {
	return mName;
}


/**
 * If getObservedValue() is NULL, then this method does nothing.
 * If there is an observedValue set AND the value is a subtype of
 * DoubleValue or IntValue, then the content of this value is set.
 * 
 * Users may override this default implementation in subclasses to obtain
 * a different calculation behavior. The default implementation is only
 * there for convendience, as this way of usage is very common.
 * 
 * @param index the index to write the calculated values to.
 * @return true if successful, false if there was an observed value with incompatible type.
 */
bool StatisticCalculator::calculateNextValue(int index) {
	if(mObservedValue == 0) {
		return true;
	}
	DoubleValue *doubleValue = dynamic_cast<DoubleValue*>(mObservedValue);
	if(doubleValue != 0) {
		setValue(index, doubleValue->get());
		return true;
	}
	
	IntValue *intValue = dynamic_cast<IntValue*>(mObservedValue);
	if(intValue != 0) {
		setValue(index, intValue->get());
		return true;
	}
	return false;
}

void StatisticCalculator::reset() {
	mStatistics.clear();
}


QList<double> StatisticCalculator::getStatistics(int startIndex, int endIndex) {
	//TODO consider startindex.

	if(startIndex > mStatistics.size() || startIndex < 0) {
		return QList<double>();
	}
	return mStatistics.mid(startIndex, (endIndex - startIndex));
}


QList<double> StatisticCalculator::getStatistics() const {
	return mStatistics;
}


int StatisticCalculator::getStartIndex() const {
	return mStartIndex;
}


void StatisticCalculator::setValue(int index, double value) {
	if(index < 0) {
		//TODO warning?
		return;
	}
	//ensure that the list is large enough.
	while(index >= mStatistics.size()) {
		mStatistics.append(0.0);
	}
	mStatistics[index] = value;

	mLastSetValue->set(value);

	//TODO consider maximal list size to avoid memory problems.
}

void StatisticCalculator::setAllStatisticValues(int index, double value) {
	setValue(index, value);
	for(QListIterator<StatisticCalculator*> i(mChildStatistics); i.hasNext();) {
		i.next()->setValue(index, value);
	}
}

QList<StatisticCalculator*> StatisticCalculator::getChildStatistics() const 
{
	return mChildStatistics;
}

bool StatisticCalculator::setObservedValue(Value *value) {
	if(  value != 0 
		&& dynamic_cast<DoubleValue*>(value) == 0 
    && dynamic_cast<IntValue*>(value) == 0)
	{
		return false;
	}
	mObservedValue = value;
	return true;
}


Value* StatisticCalculator::getObservedValue() const {
	return mObservedValue;
}


}


