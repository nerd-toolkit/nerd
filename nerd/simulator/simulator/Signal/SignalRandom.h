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

#ifndef NERDSignalRandom_H
#define NERDSignalRandom_H

#include "Signal/SignalDouble.h"
#include "Value/IntValue.h"
#include "Math/Math.h"

namespace nerd {

	/**
	 * SignalRandom
	 * Random Signal with parameters Period, Amplitude, Offset, Seed and NumberOfRandomValues.
	 * Period is the time in seconds between two random numbers. The random signal is linearly interpolated between this numbers. The random number is multiplied by Amplitude and the Offset is added. Seed is the Seed of the random number generator. A list of random numbers is generated before the run and used during the run and repeated if needed. The number of RandomNumbers in the list is stored in the NumberOfRandomValues parameter.
	 * 
	 */
	class SignalRandom : public virtual SignalDouble {
	public:
		SignalRandom(QString name, QString prefix);
		virtual ~SignalRandom();

		virtual void updateSignal();
		virtual void resetSignal();

		virtual void valueChanged(Value *value);

	private:
		void fillRandomValues();

	protected:
		DoubleValue* mPeriodValue;
		DoubleValue* mAmplitudeValue;
		DoubleValue* mOffsetValue;
		IntValue* mSeedValue;
		IntValue* mNumberOfRandomValuesValue;

	private:
		double mOldRandomValue;
		double mCurrentRandomValue;
		QList<double> mRandomValues;
		int mCurrentRandomValueIndex;
	};

}

#endif
