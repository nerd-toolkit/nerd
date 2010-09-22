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


#ifndef NERDValueDetailPanel_H
#define NERDValueDetailPanel_H

#include <QObject>
#include <QWidget>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include "Value/Value.h"
#include "ValueDetailVisualization.h"

namespace nerd {

	class ValueDetailVisualization;

	/**
	 * ValueDetailPanel.
	 * This Widget allows the visualization of value properties of one or more values.
	 * One or more values can be selected by a search pattern and/or a ComboBox and
	 * added to a scrollable subwidget.
	 * The subwidget shows the value with its properties "names", "description",
	 * "currentValue", "valueChangedListeners" and "isNotifyingAllSetAttempts".
	 * The values can be removed from the subwidget seperately or all at once by
	 * PushButtons. Another PushButton allows the update of the displayed data.
	 */
	class ValueDetailPanel : public QWidget {

	Q_OBJECT

	public:
		ValueDetailPanel();
		virtual ~ValueDetailPanel();

	protected:
		virtual void showEvent(QShowEvent *event);

	private:
		void createSearchView();
		void createListView();
		void createControlButtonView();
		bool eventFilter(QObject *object, QEvent *event);

	private:
		int mXPos;
		int mYPos;

		ValueManager *mValueManager;

		QBoxLayout *mLayout;
		QWidget *mValueSearch;
		QGridLayout *mSearchLayout;
		QLineEdit *mValueLineEdit;
		QLabel *mAvailableValueCount;
		QComboBox *mValueComboBox;
		QPushButton *mAddSelectedValues;
		QPushButton *mRemoveAllValues;
		QPushButton *mUpdateData;
		QScrollArea *mListArea;
		QWidget *mListWidget;
		QBoxLayout *mListLayout;
		QWidget *mControlButtons;
		QBoxLayout *mControlButtonLayout;
		QMap<Value*, ValueDetailVisualization*> mValueDetailVisualizations;

	private slots:
		void closeEvent(QCloseEvent *e);
		void showWindow();
		void fillValueList(const QString  &text);
		void showMatchingValues();
		void removeAllValuesFromList();
		void updateData();
	};

}

#endif


