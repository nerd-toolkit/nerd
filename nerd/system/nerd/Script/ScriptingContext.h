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



#ifndef NERDScriptingContext_H
#define NERDScriptingContext_H

#include <QString>
#include <QHash>
#include <QScriptEngine>
#include <QObject>
#include <QVariantList>
#include "Value/ValueChangedListener.h"
#include "Event/EventListener.h"

namespace nerd {

	/**
	 * ScriptingContext.
	 *
	 */
	class ScriptingContext : public QObject, public virtual ValueChangedListener,
							 public virtual EventListener {
	Q_OBJECT

	Q_PROPERTY(QString stringBuffer WRITE setStringBuffer READ getStringBuffer);

	public:
		ScriptingContext(const QString &name);
		ScriptingContext(const ScriptingContext &other);
		virtual ~ScriptingContext();

		virtual QString getName() const;

		virtual void valueChanged(Value *value);
		virtual void eventOccured(Event *event);

		virtual void resetScriptContext();
		virtual void executeScriptFunction(const QString &functionName);

		virtual bool setScriptCode(const QString &code);
		virtual QString getScriptCode() const;
		virtual bool loadScriptCode(bool replaceExistingCode);

	public slots:
		void setStringBuffer(const QString &string);
		QString getStringBuffer() const;

		void defineVariable(const QString &name, const QString &valueName, bool reportMissingValues = true);
		void defineReadOnlyVariable(const QString &name, const QString &valueName, bool reportMissingValues = true);
		void defineEvent(const QString &name, const QString &eventName, bool reportMissingEvents = true);
		void definePersistentParameter(const QString &name, const QString &initialValue);

		void setValueFromString(const QString &valueName, const QString &content, bool warnIfValueMissing = true);
		void triggerEvent(const QString &eventName, bool createIfNotAvailable = false);

		QString loadFileToString(const QString &fileName);

	protected:
		virtual void reportError(const QString &message);
		virtual void importVariables();
		virtual void exportVariables();
		virtual void addCustomScriptContextStructures();

	protected:
		QString mName;
		QScriptEngine *mScript;
		QString mVariableBuffer;
		QHash<QString, Value*> mWrittenVariables;
		QHash<QString, Value*> mReadVariables;
		QHash<Event*, QString> mEventNames;
		QHash<QString, bool> mEventOccurences;
		QHash<QString, QString> mPersistentParameters;
		StringValue *mScriptCode;
		StringValue *mScriptFileName;
	};

}

#endif



