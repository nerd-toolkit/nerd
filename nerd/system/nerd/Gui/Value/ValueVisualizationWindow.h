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


/***************************************************************************
 * Deprecated!                                                             *
 *                                                                         *
 * This class will be removed in a later release!                          *
 * *************************************************************************/



#ifndef ValueVisualizationWindow_H_
#define ValueVisualizationWindow_H_

#include <QMainWindow>
#include <QMap>
#include "ValueVisualization.h"
#include "Value/ValueManager.h"
#include "Value/ValueChangedListener.h"
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QString>


namespace nerd{

/**
 * ValueVisualizationWindow.
 */
class ValueVisualizationWindow : public QWidget
{
	Q_OBJECT

	public:
		ValueVisualizationWindow();
		void resetWindow();

	protected:
		virtual void showEvent(QShowEvent *event);

	private:
		void createValueList();
		void createEditButtons();

	private slots:
		void fillComboList(const QString &text);
		void showSelectedValues();
		void showSelectedValue(const QString &text);
		void deleteValuesFromList();
		void applyAllChanges();
		void modifySelection();
		void saveCurrentSettings();
		void loadSettings();
		void closeEvent(QCloseEvent *e);
		void showWindow();
		void showWarningMessage(QString message);

	public slots:
		void deleteValueFromList (QString valueName);

	signals:
		void showWarning(QString message);

	public:
		bool mClosingValueVisualization;

	private:
		bool mAllSelected;
		QPushButton *mSaveSettingsButton;
		QPushButton *mLoadSettingsButton;
		QPushButton *mSelectAllButton;
		QPushButton *mApplyAllChanges;
		QLabel *mNumberOfSelectedValuesLabel;
		QPushButton *mAddSelectedValues;
		QScrollArea *mValueListArea;
		QWidget *mValueList;
		QWidget *mValueSearch;
		QGridLayout *mLayout ;
		QGridLayout *mSearchLayout ;
		QBoxLayout *mListLayout;
		QGridLayout *mEditLayout;
		QWidget *mContainer;
		QLineEdit *mValueSearchField;
		QComboBox *mValueComboBox;
		QWidget *mEditButtons;
		QPushButton *mRemoveAll;
		QMap<QString, ValueVisualization*> mValues;
		ValueManager *manager;

		int mXPos;
		int mYPos;
};
}

#endif
