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



#ifndef NERDScriptedFitnessEditor_H
#define NERDScriptedFitnessEditor_H

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

namespace nerd {

	/**
	 * ScriptedFitnessEditor.
	 *
	 */
	class ScriptedFitnessEditor : public QWidget, public virtual ValueChangedListener {
	Q_OBJECT
	public:
		ScriptedFitnessEditor(const QString &fitnessFunctionName);
		virtual ~ScriptedFitnessEditor();

		virtual QString getName() const;
		virtual void valueChanged(Value *value);

	signals:
		void changeFitnessCodeArea();
		void changeErrorMessageArea();
		void markAsUnmodified();

	public slots:
		void applyButtonPressed();
		void reloadButtonPressed();
		void loadButtonPressed();
		void saveButtonPressed();
		void saveCurrentScript(const QString &fileName = "");
		void calculationModeSelectorChanged(int);
		void setFitnessCodeArea();
		void setErrorMessageArea();
		void markAsModified();
		void markTitleAsUnmodified();

	protected:
		virtual void keyPressEvent(QKeyEvent *event);

	private:
		void updateFitnessCalculationMode();
		void setCodeFromValue();

	private:
		StringValue *mFitnessCode;
		StringValue *mErrorValue;
		StringValue *mCurrentFileName;
		IntValue *mCalculationModeValue;

		QPushButton *mReloadFitnessCodeButton;
		QPushButton *mLoadFitnessCodeButton;
		QPushButton *mStoreFitnessCodeButton;
		QPushButton *mApplyButton;
		QTextEdit *mCodeArea;
		QTextEdit *mErrorMessageField;
		QComboBox *mCalculationModeSelector;
		QLabel *mTitleLabel;
		bool mScriptModified;
	};

}

#endif



