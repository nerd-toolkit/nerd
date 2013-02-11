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


#ifndef VALUE_H_
#define VALUE_H_

#include <QString>

#include "ValueManager.h"
#include "Core/Object.h"
#include <QMutex>


namespace nerd {

class ValueChangedListener;

/**
 * Value.
 */
class Value : public virtual Object
{

	public:
		Value();
		Value(const QString &typeName, bool notifyAllSetAttempts);
		Value(const Value &value);
		virtual ~Value();

		virtual Value* createCopy();

		virtual QString getName() const;

		virtual void setTypeName(const QString &typeName);
		virtual QString getTypeName() const;
        virtual void setDescription(const QString &description);
        virtual QString getDescription() const;
        virtual QList<QString>& getOptionList();

		virtual bool setValueFromString(const QString &value) ;
		virtual QString getValueAsString() const;

		virtual bool addValueChangedListener(ValueChangedListener *listener);
		virtual bool removeValueChangedListener(ValueChangedListener *listener);
		virtual void removeValueChangedListeners();
		virtual QList<ValueChangedListener*> getValueChangedListeners() const;
		virtual void notifyValueChanged();

		virtual void setNotifyAllSetAttempts(bool notify);
		virtual bool isNotifyingAllSetAttempts() const;

		virtual bool setProperties(const QString &properties);
		virtual QString getProperties() const;

		virtual bool equals(const Value *value) const;

	private:
		void createValue(const QString &typeName, bool notifyAllSetAttempts);

	private:
		int mNotifyCount;
		QMutex mMutex;
		QString mTypeName;
		QList<ValueChangedListener*> mValueChangedListeners;
		QList<ValueChangedListener*> mChangedListenerBufferVector;
        QString mDescription;

	protected:
		bool mNotifyAllSetAttempts;
		QList<QString> mOptionList;

};
}
#endif /*VALUE_H_*/

