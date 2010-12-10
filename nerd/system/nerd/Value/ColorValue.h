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


#ifndef NERDColorValue_H
#define NERDColorValue_H

#include "Value/Value.h"
#include "Util/Color.h"
#include <QString>
#include "Value/MultiPartValue.h"

namespace nerd {

	/**
	 * ColorValue
	 */
	class ColorValue : public Value, public ValueChangedListener /*, public virtual MultiPartValue*/ {
	public:
		ColorValue();
		ColorValue(const QString &color);
		ColorValue(const Color &color);
		ColorValue(int red, int green, int blue, int alpha);
		ColorValue(const ColorValue &value);
		virtual ~ColorValue();

		void init();
		virtual QString getName() const;

		virtual Value* createCopy();
		virtual void set(const Color &color);
		virtual void set(int red, int green, int blue);
		virtual void set(int red, int green, int blue, int alpha);
		virtual const Color& get() const;

		virtual QString getValueAsString() const;
		virtual bool setValueFromString(const QString &value);

		virtual bool equals(const Value *value) const;

		virtual void valueChanged(Value *value);

		virtual int getNumberOfValueParts() const;
		virtual Value* getValuePart(int index) const;
		virtual QString getValuePartName(int index) const;

	private:
		Color mColor;
		QVector<IntValue*> mColorValues;
		bool mSettingValue;
	};

}

#endif

