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


#ifndef VALUEMANAGER_H_
#define VALUEMANAGER_H_

#include <QMap>
#include "Core/Object.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include <QMutex>
#include <QList>
#include <QVector>
#include <QString>

namespace nerd {
class Value;
class ValueChangedListener;
class IntValue;
class BoolValue;
class StringValue;
class DoubleValue;

/**
 * ValueManager.
 */
class ValueManager {

	public:
		ValueManager(EventManager *eventManager);
		~ValueManager();

		bool nameExists(const QString &name);
		bool addValue(const QString &name, Value *value);
		bool removeValue(const QString &name);
		bool removeValue(Value *value);
		bool removeValues(const QList<Value*> &values);

		Value* getValue(const QString &name);
		Value* getMultiPartValue(const QString &name, const QString &valuePart = "");

		IntValue* getIntValue(const QString &name);
		BoolValue* getBoolValue(const QString &name);
		DoubleValue* getDoubleValue(const QString &name);
		StringValue* getStringValue(const QString &name);


		QList<Value*> getValues();
		QList<Value*> getValuesMatchingPattern(const QString &pattern, bool caseSensitive = true);
		QList<QString> getNamesOfValue(const Value *value);
		QList<QString> getValueNames() ;
		QList<QString> getValueNamesMatchingPattern(const QString &name, 
													  bool caseSensitive = true);
		void removeAllValues();

		void notifyRepositoryChangedListeners();
		void removeAllListeners();

		void pushToNotificationStack(Object *stackedObject);
		void popFromNotificationStack();
		QVector<Object*> getNotificationStack() const;

		bool loadValues(const QString &fileName, bool useFileLocking = false);
		bool saveValues(const QString &fileName, 
						const QList<QString> valuesToSave,
						const QString &comment, 
						bool useFileLocking = false);

		bool addPrototype(Value *prototype);
		const QList<Value*>& getPrototypes() const;
		Value* createCopyOfPrototype(const QString &typeName) const;

		void printValues(const QString &valueNamePattern);

	private:
		QMap<QString, Value*> mValues;
		QList<Value*> mPrototypes;
		QVector<Object*> mNotificationStack;
		Event *mRepositoryChangedEvent;
		QMutex mMutex;
		IntValue *mRepositoryChangedCounter;
		QList<QString> mFileNameBuffer;
};
}
#endif /*VALUEMANAGER_H_*/
