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



#ifndef NERDVector3DValue_H
#define NERDVector3DValue_H

#include "Value.h"
#include "Math/Vector3D.h"
#include "Value/MultiPartValue.h"
#include "Value/DoubleValue.h"
#include <QVector>

namespace nerd {

/**
 * Vector3DValue.
 */
class Vector3DValue : public Value, public virtual MultiPartValue {
	public:
		Vector3DValue();
		Vector3DValue(double x, double y, double z);
		Vector3DValue(const Vector3DValue &value);
		virtual ~Vector3DValue();

		virtual Value* createCopy();

		virtual void valueChanged(Value *value);
		virtual QString getName() const;

		virtual void set(const Vector3D &vector);
		virtual void set(double x, double y, double z);

		virtual Vector3D get() const;
		virtual double getX() const;
		virtual double getY() const;
		virtual double getZ() const;

		virtual bool setValueFromString(const QString &value) ;
		virtual QString getValueAsString() const;

		virtual int getNumberOfValueParts() const;
		virtual Value* getValuePart(int index) const;
		virtual QString getValuePartName(int index) const;

		virtual bool equals(const Value *value) const;

	private:
		QVector<DoubleValue*> mVectorValues;
		bool mSettingValue;

};

}

#endif

