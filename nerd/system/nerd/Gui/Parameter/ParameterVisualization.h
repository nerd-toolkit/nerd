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


#ifndef NERDParameterVisualization_H_
#define NERDParameterVisualization_H_

#include <QObject>
#include "Value/Value.h"
#include "Value/ValueChangedListener.h"
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include "Core/Task.h"
#include "Gui/Parameter/SetInitValueTaskFactory.h"
#include "Gui/ScriptEditor/ScriptEditor.h"


namespace nerd{

class ParameterVisualizationWindow;

/**
 * ParameterVisualization.
 */
class ParameterVisualization : public QFrame, public virtual ValueChangedListener{

	Q_OBJECT

	public:
		ParameterVisualization(ParameterVisualizationWindow *list, Value *value, QString name);
		virtual ~ParameterVisualization();

		virtual void valueChanged(Value *value);
		QString getName() const;

		QString getValueName() const;
		void reset();
		Value* getValue();
		bool isValueUpdateActive() const;

		void setValueObjectByName(const QString &name);

		void setCurrentValue(const QString &currentValue);
		QString getCurrentValue() const;

        void addOption(const QString &optionText);
        QList<QString> getOptions() const;


	public slots:
        void itemSelected(const QString &item);
		void changeValue();
		void setDoUpdateValue(int doUpdate);
		void updateValueInEnvironmentManager();
		void moveWidgetUp();
		void moveWidgetDown();
		void editButtonPressed();

	signals:
		void lineEditTextChanged(QString newText);
		void destroyThis(QString valueName);
		void move(ParameterVisualization *visu, bool up);

	private slots:
		void destroy();
		void markAsValueEdited();
		void markAsValueUpdated();


	private:
		QLineEdit *mValueField;
		QComboBox *mValueBox;
		Value *mValue;
		QString mValueName;
		ParameterVisualizationWindow *mValueList;
		QLabel *mNameLabel;
		QPushButton *mCloseButton;
		QPushButton *mUpdateSnapshotButton;
		QPushButton *mMoveUpButton;
		QPushButton *mMoveDownButton;
		QPushButton *mEditCodeButton;
		QCheckBox *mUpdateValue;
		bool mDoUpdateValue;
		bool mValueModified;
		SetInitValueTaskFactory *mSetInitValueTaskFactory;
		ScriptEditor *mScriptEditor;
};

}

#endif
