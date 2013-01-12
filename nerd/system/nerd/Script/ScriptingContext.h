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



#ifndef NERDScriptingContext_H
#define NERDScriptingContext_H

#include <QString>
#include <QHash>
#include <QScriptEngine>
#include <QObject>
#include <QVariantList>
#include "Value/ValueChangedListener.h"
#include "Event/EventListener.h"
#include <QFile>
#include "Value/FileNameValue.h"
#include "Value/CodeValue.h"



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
		ScriptingContext(const QString &name, const QString &mainContextName = "nerd");
		ScriptingContext(const ScriptingContext &other);
		virtual ~ScriptingContext();

		virtual QString getName() const;

		virtual void valueChanged(Value *value);
		virtual void eventOccured(Event *event);

		virtual void resetScriptContext();
		virtual void executeScriptFunction(const QString &functionName);

		virtual void reloadEventAndValueDefinitions();

		virtual bool setScriptCode(const QString &code);
		virtual QString getScriptCode() const;
		virtual bool loadScriptCode(bool replaceExistingCode);

		void setMainContextName(const QString &contextName);
		QString getMainContextName() const;

	public slots:
		void setStringBuffer(const QString &string);
		QString getStringBuffer() const;

		void defineVariable(const QString &name, const QString &valueName, bool reportMissingValues = true);
		void defineVariable(const QString &name, Value *value);
		void defineReadOnlyVariable(const QString &name, const QString &valueName, bool reportMissingValues = true);
		void defineReadOnlyVariable(const QString &name, Value *value);
		void defineEvent(const QString &name, const QString &eventName, bool reportMissingEvents = true);
		void definePersistentParameter(const QString &name, const QString &initialValue);

		bool setProperty(const QString &fullPropertyName, const QString &value);
		QString getProperty(const QString &fullPropertyName);
		QScriptValue getPropertyAsArray(const QString fullPropertyName);

		bool createGlobalStringProperty(const QString &propertyName, const QString &content = "");
		bool createGlobalDoubleProperty(const QString &propertyName, double content = 0.0);

		void setValueFromString(const QString &valueName, const QString &content, bool warnIfValueMissing = true);
		void triggerEvent(const QString &eventName, bool createIfNotAvailable = false);

		QString loadFileToString(const QString &fileName);
		int openFile(const QString &fileName);
		bool closeFile(int fileId);
		bool writeToFile(int fileId, const QString &content);
		QString getTimeStamp();
		
		double random();
		int randomInt(int max);
		double sign(double value);

		QString toVector3DString(double x, double y, double z);
		QString toColorString(double r, double g, double b, double t);
		double getVectorElement(const QString &vector3DString, int index);

		bool loadValues(const QString &fileName);
		
		void error(const QString &message);

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
		CodeValue *mScriptCode;
		FileNameValue *mScriptFileName;
		QString mMainContextName;
		bool mHasUnresolvedValueDefinitions;
		bool mHasUnresolvedEventDefinitions;
		int mMaxNumberOfTriesToResolveDefinitions;
		int mFileIdCounter;
		QHash<int, QFile*> mOpenFiles;
		bool mRestrictToMainExecutionThread;
	};

}

#endif



