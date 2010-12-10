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




#include "EventDetailPanel.h"
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "EventDetailVisualization.h"
#include <QBoxLayout>
#include <QLabel>
#include <QVector>
#include <QScrollArea>
#include <QDebug>
#include <QEvent>

namespace nerd {

/**
 * Constructor.
 */
EventDetailPanel::EventDetailPanel() : QWidget(0)
{
	setWindowTitle("Event Detail Panel");
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	mEventManager = Core::getInstance()->getEventManager();
	mLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	setLayout(mLayout);

	createSearchView();
	createListView();
	createControlButtonView();

	resize(400, 400);
}

/**
 * Creates the Gui-elements for searching for events and adding events.
 */
void EventDetailPanel::createSearchView() {
	mSearchLayout = new QGridLayout;
	mEventSearch = new QWidget;
	mEventSearch->setLayout(mSearchLayout);
	mLayout->addWidget(mEventSearch);

	mEventLineEdit = new QLineEdit;
	mEventLineEdit->setMinimumSize(350, 25);
	mEventLineEdit->setMaximumSize(350, 25);
	mSearchLayout->addWidget(mEventLineEdit, 0, 0);
	mSearchLayout->setAlignment(mEventLineEdit, Qt::AlignLeft);

	mAvailableEventCount = new QLabel;
	mAvailableEventCount->setMinimumSize(50, 25);
	mAvailableEventCount->setMaximumSize(50, 25);
	mSearchLayout->addWidget(mAvailableEventCount, 0, 1);
	mSearchLayout->setAlignment(mAvailableEventCount, Qt::AlignLeft);

	mEventComboBox = new QComboBox;
	mEventComboBox->setMinimumSize(350, 25);
	mEventComboBox->setMaximumSize(350, 25);
	mEventComboBox->setEditable(false);
	mSearchLayout->addWidget(mEventComboBox, 1, 0);
	mSearchLayout->setAlignment(mEventComboBox, Qt::AlignLeft);

	mAddSelectedEvents = new QPushButton("Add");
	mSearchLayout->addWidget(mAddSelectedEvents, 1, 1);
	mSearchLayout->setAlignment(mAddSelectedEvents, Qt::AlignRight);

	connect(mEventLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(fillEventList(const QString&)));
	connect(mEventLineEdit, SIGNAL(returnPressed()), this, SLOT(showMatchingEvents()));
	connect(mEventComboBox, SIGNAL(activated(const QString&)), mEventLineEdit, SLOT(setText(const QString&)));
	connect(mAddSelectedEvents, SIGNAL(clicked()), this, SLOT(showMatchingEvents()));
}

/**
 * Creates the Gui-elements for displaying the event visualizations.
 */
void EventDetailPanel::createListView() {
	mListArea = new QScrollArea;
	mListArea->setWidgetResizable(true);
	mListWidget = new QWidget(mListArea);
	mListLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	mListLayout->setAlignment(Qt::AlignTop);
	mListLayout->setSpacing(1);
	mListWidget->setLayout(mListLayout);
	mListArea->setWidget(mListWidget);
	mLayout->addWidget(mListArea, 1, 0);
}

/**
 * Creates the Gui-elements for removing event visualizations and update data.
 */
void EventDetailPanel::createControlButtonView() {
	mControlButtons = new QWidget;
	mControlButtonLayout = new QHBoxLayout();
	mControlButtons->setLayout(mControlButtonLayout);
	mLayout->addWidget(mControlButtons);
	mRemoveAllEvents = new QPushButton("Remove All");
	mUpdateData = new QPushButton("Update Data");
	mControlButtonLayout->addWidget(mUpdateData);
	mControlButtonLayout->addWidget(mRemoveAllEvents);

	connect(mRemoveAllEvents, SIGNAL(clicked()), this, SLOT(removeAllEventsFromList()));
	connect(mUpdateData, SIGNAL(clicked()), this, SLOT(updateData()));
}

/**
 * This slot creates the event visualizations of the selected events.
 */
void EventDetailPanel::showMatchingEvents() {
	QVector<QString> selectedEvents;
	QString temp = mEventLineEdit->text();
	temp.replace("**", ".*");
	temp.append(".*").prepend(".*");
	QVector<Event*> events = mEventManager->getEventsMatchingPattern(temp, false);
	for(int i = 0; i < events.size(); i++) {
		Event *event = events.at(i);
		QMap<Event*, EventDetailVisualization*>::iterator index =
		mEventDetailVisualizations.find(event);
		if(index == mEventDetailVisualizations.end()) {
			EventDetailVisualization *newVis = new EventDetailVisualization(event);
			mEventDetailVisualizations[event] = newVis;
			newVis->installEventFilter(this);
			mListLayout->addWidget(newVis, 0, Qt::AlignTop);
		}
	}
}

/**
 * This slot removes all event visualizations.
 */
void EventDetailPanel::removeAllEventsFromList() {
	QMap<Event*, EventDetailVisualization*>::iterator it = mEventDetailVisualizations.begin();
	while(it != mEventDetailVisualizations.end()) {
		EventDetailVisualization *eventVisu = it.value();
		// erase the Visualization from the QMap before sending close event
		// so the event filter of this class doesn't erase the Visualization (SegFault!)
		it = mEventDetailVisualizations.erase(it);
		eventVisu->close();
	}
}

/**
 * This slot reloads all event visualizations.
 */
void EventDetailPanel::updateData() {
	QMap<Event*, EventDetailVisualization*>::iterator it;
	for(it = mEventDetailVisualizations.begin(); it != mEventDetailVisualizations.end(); it++) {
		it.value()->fillContents();
	}
}

/**
 * This slot fills the ComboBox with all events matching the search pattern and updates the number of events found matching the pattern.
 *
 * @param text the search pattern from the TextEdit
 */
void EventDetailPanel::fillEventList(const QString &text) {
	QString temp = text;
	temp = temp.replace("**", ".*");
	temp.append(".*").prepend(".*");
	QVector<Event*> events = mEventManager->getEventsMatchingPattern(temp, false);
	mEventComboBox->clear();
	for(int i = 0; i < events.size(); i++) {
		Event *event = events.at(i);
		QList<QString> names = event->getNames();
		QListIterator<QString> it(names);
		for(; it.hasNext(); ) {
			QString name = it.next();
			mEventComboBox->addItem(name);
		}
		
	}
	QString number;
	number.setNum(events.size());
	mAvailableEventCount->setText(number);
}

/**
 * Catches the close event of the EventDetailVisualizations to remove them from mEventDetailVisualizations.
 *
 * @param object the sender object
 * @param event the event to filter
 */
bool EventDetailPanel::eventFilter(QObject *object, QEvent *event) {
	if(event->type() == QEvent::Close) {
		EventDetailVisualization *visu = dynamic_cast<EventDetailVisualization*>(object);
		if(visu != NULL) {
			QMap<Event*, EventDetailVisualization*>::iterator index = mEventDetailVisualizations.find(visu->getEvent());
			if(index != mEventDetailVisualizations.end()) {
				mEventDetailVisualizations.erase(index);
			}
		}
		else {
			qDebug("EventDetailPanel: Close Event received from EventDetailVisualization, but could not cast sender object to EventDetailVisualization! Event ignored.");
			return true;
		}
	}
	return false;	// send event to the target object itself
}

/**
 * Destruktor.
 */
EventDetailPanel::~EventDetailPanel() {
}

/**
 * Catches the close event to save the last Window position
 */
void EventDetailPanel::closeEvent(QCloseEvent*) {
	mXPos = x();
	mYPos = y();
}

/**
 * This slot restores the last Window position before showing the window
 */
void EventDetailPanel::showWindow() {
	move(mXPos, mYPos);
	show();
}

/**
 * Catches the show event to ensure that the comboBox is initially filled with the current parameter names.
 */
void EventDetailPanel::showEvent(QShowEvent*) {
	if(mEventLineEdit != 0) {
		fillEventList(mEventLineEdit->text());
	}
}

}


