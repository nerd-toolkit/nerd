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
#include "ListRandomizer.h"
#include "Math/Random.h"
#include <iostream>

using namespace std;

namespace nerd {

/**
 * Constructor.
 * @param value The value that will be randomized by the Randomizer. 
 */
ListRandomizer::ListRandomizer(Value *value) {
	mValue = value;
	mCurrentIndex = -1;
	mIsSequential = false;
	mIsAdditive = false;
	mIncremental = true;
}

ListRandomizer::~ListRandomizer() {
}

void ListRandomizer::applyRandomization() {

	if(mValueList.size() == 0) {
		return;
	}
	if(mIsSequential) {
		mCurrentIndex++;
		mCurrentIndex = mCurrentIndex % mValueList.size();
		mValue->setValueFromString(mValueList.at(mCurrentIndex));
	}
	else {	
		int index = 0;
		index = Random::nextInt(mValueList.size());	
		if(mIsAdditive) {
			if(mIncremental) {
				index = (index + mCurrentIndex) % mValueList.size();
			}
			else {
				index = (mCurrentIndex - index) % mValueList.size();
			}
		}
		mValue->setValueFromString(mValueList.at(index));
	}
}

void ListRandomizer::reset() {
	mCurrentIndex = -1;
}

/**
 * Add a new entry to the list of possible settings of the randomized value.
 * @param valueSetting A string based definition of a possible setting.
 */
void ListRandomizer::addListItem(QString valueSetting) {
	mValueList.push_back(valueSetting);
}

/**
 * Defines, whethter the randomization will be sequential or not. In sequential randomization, the 
 * list entries will be used in order of appearance.
 * @param sequential 
 */
void ListRandomizer::setSequential(bool sequential) {
	mIsSequential = sequential;
}

/**
 * Defines, whether the randomization will be additive or not. In additive randomization, there will be added/substracted a random offset to the current position. Whether it will be added or substracted depends on setIncremental. Default is incrementing (adding) the list position. If the end of the list ist reach, modulo calculation is used to start from the first index again.
 * @param additive 
 */
void ListRandomizer::setAdditive(bool additive) {
	mIsAdditive = additive;
}

/**
 * Defines whether in additive-mode, the random offset is added or substracted from the last used 
 * list-index.
 * @param incremental 
 */
void ListRandomizer::setIncremental(bool incremental) {
	mIncremental = incremental;
}
}
