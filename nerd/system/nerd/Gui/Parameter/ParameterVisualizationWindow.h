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


#ifndef ParameterVisualizationWindow_H_
#define ParameterVisualizationWindow_H_

#include <QMainWindow>
#include <QMap>
#include "Value/ValueManager.h"
#include "Value/ValueChangedListener.h"
#include "Gui/Logger/ParameterLoggerWidget.h"
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QString>
#include <QScrollArea>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QList>
#include <QHBoxLayout>
#include <QKeyEvent>
#include "Gui/ValuePlotter/ValuePlotter.h"
#include "Gui/ValuePlotter/ValuePlotterWidget.h"
#include <QVBoxLayout>
#include "Gui/Logger/ValueProvider.h"
#include <iostream>
#include "Event/EventManager.h"
#include "Event/Event.h"
#include "Gui/Parameter/SetInitValueTaskFactory.h"

namespace nerd{

class ParameterVisualization;
class ParameterComboBox;


/**
 * ParameterVisualizationWindow.
 */
class ParameterVisualizationWindow : public QWidget, public virtual ValueProvider,
									 public virtual EventListener
{
	Q_OBJECT

	public:
		ParameterVisualizationWindow(const QString &name);
		virtual ~ParameterVisualizationWindow();

		QList<ParameterVisualization*> getParameterVisualizations() const;
		QString getName() const;
		virtual void eventOccured(Event *event);

		virtual QHash<QString, Value*> getValueCollection() const;


		void modifySelection(bool select);

	private:
		void createValueList();
		void createEditButtons();
		void resetWindow();

	signals:
		void executeFillComboList(const QString &text);

	public slots:
		void deleteValueFromList (QString valueName);
		void saveCurrentParameters(const QString &fileName, bool saveValueContent);
		QString loadParametersFromFile(const QString &fileName, bool autoApply);

		void selectionChanged(const QString &text);
		void fillComboList(const QString &text);
		void showSelectedValues();
		ParameterVisualization* showSelectedValue(const QString &text, bool startDeselected);
		void deleteValuesFromList();
		void applyAllChanges();
		void modifySelection();
		void closeEvent(QCloseEvent *e);
		void showComboBoxList();
		void showAndUpdateHistoryPlotter(bool show = true);
		void showAndUpdateXYPlotter(bool show = true);
		void updateLoggerButton(bool loggerActive);
		void saveSettings();
		void loadSettings();
		void moveParameterVisualization(ParameterVisualization *visu, bool up);

	protected:
		virtual void showEvent(QShowEvent *event);

	public:
		bool mCloseDownRunning;

	private:
		QString mName;
		bool mAllSelected;
		QPushButton *mSelectAllButton;
		QPushButton *mApplyAllChanges;
		QLabel *mNumberOfSelectedParametersLabel;
		QPushButton *mAddSelectedParameters;
		QScrollArea *mValueListArea;
		QWidget *mValueList;
		QWidget *mValueSearch;
		QVBoxLayout *mLayout ;
		QGridLayout *mSearchLayout ;
		QBoxLayout *mListLayout;
		QGridLayout *mEditLayout;
		QWidget *mContainer;
		ParameterComboBox *mValueComboBox;
		QPushButton *mRemoveAll;
		QMap<QString, ParameterVisualization*> mValues;
		QAction *mAction;
		QPushButton *mLoggerButton;
		ParameterLoggerWidget *mParameterLoggerWidget;
		int mXPos;
		int mYPos;
		bool mFillingComboBox;
		QString mSelectedParameterName;
		QString mLastEnteredParameterName;
		QString mRestorationBuffer;
		ValuePlotterWidget *mValuePlotter;
		ValuePlotterWidget *mXYPlotter;
		QPushButton *mPlotterButton;
		Event *mPhysicsEnvironmentChangedEvent;
		Event *mValueRepositoryChangedEvent;
};


class ParameterComboBox : public QComboBox
{
	Q_OBJECT
	public:
		ParameterComboBox(QWidget *parent = 0)
			: QComboBox(parent), mShiftKeyPressed(false) {}


		bool isShiftKeyPressed() const {
			return mShiftKeyPressed;
		}

		bool isCtrlKeyPressed() const {
			return mCtrlKeyPressed;
		}

	signals:
		void downArrowPressed();

	protected:
		virtual void keyPressEvent(QKeyEvent *event) {
			if(event->key() == Qt::Key_Down) {
				emit downArrowPressed();
				event->accept();
			}
			else {
				//TODO find a way to recognize shift events while popup is shown.
				if(event->key() == Qt::Key_Shift) {
					mShiftKeyPressed = true;
				}
				else if(event->key() == Qt::Key_Control) {
					mCtrlKeyPressed = true;
				}
				QComboBox::keyPressEvent(event);
			}
		}

		virtual void keyReleaseEvent(QKeyEvent *event) {
			if(event->key() == Qt::Key_Shift) {
				mShiftKeyPressed = false;
			}
			else if(event->key() == Qt::Key_Control) {
				mCtrlKeyPressed = false;
			}
			QComboBox::keyReleaseEvent(event);
		}



	private:
		bool mShiftKeyPressed;
		bool mCtrlKeyPressed;

};


}

#endif
