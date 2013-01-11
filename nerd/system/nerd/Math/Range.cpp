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


#include "Range.h"
#include <math.h>
#include "Math/Math.h"

namespace nerd {
	
	
	Range::Range() {
		mMin = 0.0;
		mMax = 1.0;
	}
	
	
	Range::Range(double min, double max) {
		mMin = min;
		mMax = max;
	}
	
	
	Range::Range(const Range &range) {
		mMin = range.mMin;
		mMax = range.mMax;
	}
	
	Range::~Range() {
	}
	
	
	void Range::setMin(double min) {
		mMin = min;
	}
	
	
	void Range::setMax(double max) {
		mMax = max;
	}
	
	
	void Range::set(double min, double max) {
		mMin = min;
		mMax = max;
	}
	
	
	void Range::set(const Range &other) {
		mMin = other.mMin;
		mMax = other.mMax;
	}
	
	
	
	double Range::getMin() const {
		return mMin;
	}
	
	
	double Range::getMax() const {
		return mMax;
	}
	
	
	bool Range::isValid() const {
		if(mMin <= mMax) {
			return true;
		}
		return false;
	}
	
	
	bool Range::equals(const Range &range, int precision) const {
		if(precision < 0) {
			if(mMin == range.mMin && mMax == range.mMax)
			{
				return true;
			}
			return false;
		}
		else {
			long min = (long) ((mMin * pow(10.0, precision)) + 0.5);
			long min2 = (long) ((range.mMin * pow(10.0, precision)) + 0.5);
			long max = (long) ((mMax * pow(10.0, precision)) + 0.5);
			long max2 =  (long) ((range.mMax * pow(10.0, precision)) + 0.5);
			
			if(min == min2 && max == max2) {
				return true;
			}
			return false;
		}
	}

}
