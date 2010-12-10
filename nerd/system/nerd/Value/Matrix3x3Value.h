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



#ifndef NERDMatrix3x3Value_H
#define NERDMatrix3x3Value_H

#include "Value.h"
#include "Value/MultiPartValue.h"
#include "Value/DoubleValue.h"
#include "Math/Matrix3x3.h"
#include <QVector>

namespace nerd {

/**
 * Matrix3x3Value.
 */
class Matrix3x3Value : public Value, public virtual MultiPartValue {
	public:
		Matrix3x3Value();
		Matrix3x3Value(double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33);
		Matrix3x3Value(const Matrix3x3 &matrix);
		Matrix3x3Value(const Matrix3x3Value &value);
		virtual ~Matrix3x3Value();

		virtual Value* createCopy();

		virtual void valueChanged(Value *value);
		virtual QString getName() const;

		virtual void set(double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33);
		virtual void set(const Matrix3x3 &matrix);

		virtual Matrix3x3 get() const;
		virtual double get(int row, int col) const;

		virtual bool setValueFromString(const QString &value) ;
		virtual QString getValueAsString() const;

		virtual int getNumberOfValueParts() const;
		virtual Value* getValuePart(int index) const;
		virtual QString getValuePartName(int index) const;

		virtual bool equals(const Value *value) const;

	private:
		QVector<DoubleValue*> mMatrixValues;
		bool mSettingValue;

};

}

#endif

