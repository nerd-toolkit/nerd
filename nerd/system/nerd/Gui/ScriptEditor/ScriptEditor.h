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



#ifndef NERDScriptEditor_H
#define NERDScriptEditor_H

#include <QString>
#include <QHash>
#include <QWidget>
#include "Value/ValueChangedListener.h"
#include "Value/IntValue.h"
#include "Value/StringValue.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include "Gui/ValuePlotter/ValuePlotter.h"
#include "Gui/ValuePlotter/ValuePlotterWidget.h"
#include "Value/DoubleValue.h"
#include "Value/FileNameValue.h"
#include "Value/CodeValue.h"



namespace nerd {

	/**
	 * ScriptEditor.
	 *
	 */
	class ScriptEditor : public QWidget, public virtual ValueChangedListener {
	Q_OBJECT
	public:
		ScriptEditor(const QString &scriptName, bool autoHandleChanges = true);
		virtual ~ScriptEditor();

		virtual QString getName() const;
		virtual void valueChanged(Value *value);
		
		virtual bool attachToCodeBase(CodeValue *code);

	signals:
		void changeScriptCodeArea();
		void changeErrorMessageArea();
		void markAsUnmodified();
		
		void handleChangedContent(const QString &newContent);

	public slots:
		void applyButtonPressed();
		void reloadButtonPressed();
		void loadButtonPressed();
		bool loadScriptCode(const QString &fileName, bool replaceExistingCode);
		void saveButtonPressed();
		void saveCurrentScript(const QString &fileName = "");
		void setScriptCodeArea();
		void setErrorMessageArea();
		void markAsModified();
		void markTitleAsUnmodified();

	protected:
		virtual void keyPressEvent(QKeyEvent *event);

	private:
		void setCodeFromValue();

	private:
		CodeValue *mScriptCode;
		StringValue *mErrorValue;
		FileNameValue *mCurrentFileName;

		QPushButton *mReloadScriptCodeButton;
		QPushButton *mLoadScriptCodeButton;
		QPushButton *mStoreScriptCodeButton;
		QPushButton *mApplyButton;
		QTextEdit *mCodeArea;
		QTextEdit *mErrorMessageField;
		QLabel *mTitleLabel;
		bool mScriptModified;
		bool mAutoHandleChanges;
	};

}

#endif



