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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
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
 *   clearly by citing the nerd homepage and the nerd overview paper.      *
 ***************************************************************************/


#ifndef EventVisualizationWindow_H_
#define EventVisualizationWindow_H_

#include <QString>
#include <QWidget>
#include <QBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QGridLayout>
#include <QCheckBox>
#include <QMap>
#include <QFrame>
#include <QMutex>
#include "EventVisualization.h"
#include "../../Event/EventManager.h"
#include "../../Event/Event.h"
#include <QHBoxLayout>
#include "Core/SystemObject.h"

namespace nerd{

class CloseAllTask;

/**
 * EventVisualizationWindow.
 */
class EventVisualizationWindow : public QWidget, public virtual SystemObject {

	Q_OBJECT

	friend class CloseAllTask;
	friend class EventVisualization;

	public:
		EventVisualizationWindow();
		virtual ~EventVisualizationWindow();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		virtual QString getName() const;

	protected:
		virtual void showEvent(QShowEvent *event);
		virtual void closeEvent(QCloseEvent *e);

	private:
		void createListView();
		void createSearchView();
		void createControlButtonView();

		bool eventFilter(QObject *object, QEvent *event);

	private slots:
		void showMatchingEvents();
		void fillEventList(const QString &text);
		void removeAllEventsFromList();
		void moveToNextEvent();
		void modifySelection();
		void showWindow();
		void changeMode(int);

	private:
		bool mAllSelected;
		QPushButton *mSelectAllButton;
		QLabel *mAvailableValueCount;
		QMap<Event*, EventVisualization*> mEventVisualizations;
		QPushButton *mAddSelectedEvents;
		QBoxLayout *mListLayout;
		QBoxLayout *mLayout;
		QLineEdit *mEventLineEdit;
		QComboBox *mEventComboBox;
		QPushButton *mRemoveAllEvents;

		QWaitCondition mBlockingCondition;
		QMutex mBlockingMutex;
		bool mBlockAfterEvent;

		EventManager *mEventManager;
		QFrame *mStepModeLabel;
		QCheckBox *mSwitchToStepMode;
		QPushButton *mTriggerStep;
		int mXPos;
		int mYPos;
	};
}
#endif
