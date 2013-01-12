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


#ifndef NERDEventDetailPanel_H
#define NERDEventDetailPanel_H

#include <QObject>
#include <QWidget>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include "Event/Event.h"
#include "EventDetailVisualization.h"

namespace nerd {

	class EventDetailVisualization;

	/**
	 * EventDetailPanel.
	 * This Widget allows the visualization of event properties of one or more events.
	 * One or more events can be selected by a search pattern and/or a ComboBox and
	 * added to a scrollable subwidget.
	 * The subwidget shows the event with its properties "names", "description",
	 * "eventListeners" and "upstreamEvents".
	 * The events can be removed from the subwidget seperately or all at once by
	 * PushButtons. Another PushButton allows the update of the displayed data.
	 */
	class EventDetailPanel : public QWidget {

	Q_OBJECT

	public:
		EventDetailPanel();
		virtual ~EventDetailPanel();

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

		EventManager *mEventManager;

		QBoxLayout *mLayout;
		QWidget *mEventSearch;
		QGridLayout *mSearchLayout;
		QLineEdit *mEventLineEdit;
		QLabel *mAvailableEventCount;
		QComboBox *mEventComboBox;
		QPushButton *mAddSelectedEvents;
		QPushButton *mRemoveAllEvents;
		QPushButton *mUpdateData;
		QScrollArea *mListArea;
		QWidget *mListWidget;
		QBoxLayout *mListLayout;
		QWidget *mControlButtons;
		QBoxLayout *mControlButtonLayout;
		QMap<Event*, EventDetailVisualization*> mEventDetailVisualizations;

	private slots:
		void closeEvent(QCloseEvent *e);
		void showWindow();
		void fillEventList(const QString  &text);
		void showMatchingEvents();
		void removeAllEventsFromList();
		void updateData();
	};

}

#endif


