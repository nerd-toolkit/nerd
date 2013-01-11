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


#ifndef NERDNormalizedDoubleValue_H
#define NERDNormalizedDoubleValue_H

#include "Value/DoubleValue.h"

namespace nerd {

	/**
	 * NormalizedDoubleValue
	 */
	class NormalizedDoubleValue : public DoubleValue {

	public:
		NormalizedDoubleValue();
		NormalizedDoubleValue(double value, double min, double max);
		NormalizedDoubleValue(double value, double min, double max,
							double normalizedMin, double normalizedMax);
		NormalizedDoubleValue(const NormalizedDoubleValue &value);
		virtual ~NormalizedDoubleValue();

		virtual Value* createCopy();

		virtual void set(double value);
		virtual double getNormalized() const;
		virtual void setNormalized(double value);

		virtual void setMin(double min);
		virtual double getMin() const;
		virtual void setMax(double max);
		virtual double getMax() const;

		virtual void setNormalizedMin(double min);
		virtual double getNormalizedMin() const;
		virtual void setNormalizedMax(double max);
		virtual double getNormalizedMax() const;

		virtual bool setProperties(const QString &properties);
		virtual QString getProperties() const;

		virtual bool equals(const Value *value) const;

	protected:
		double mMin;
		double mMax;
		double mNormalizedMin;
		double mNormalizedMax;


	};

}

#endif


