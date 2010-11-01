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



#include "EventVisualizationWindow.h"
#include "../../Event/Event.h"
#include "../../Core/Core.h"
#include <QPalette>
#include <QColorGroup>
#include "Core/Task.h"
#include <QMessageBox>
#include <QEvent>


namespace nerd{

EventVisualizationWindow::EventVisualizationWindow() : QWidget(0) {

	setWindowTitle("Event Observer");
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);
	mEventManager = Core::getInstance()->getEventManager();
	mLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	setLayout(mLayout);
	createSearchView();
	createListView();
	createControlButtonView();
	resize(400, 400);
	mAllSelected = false;
	
	mBlockAfterEvent = false;

	mXPos = 0;
	mYPos = 0;

	Core::getInstance()->addSystemObject(this);
}

EventVisualizationWindow::~EventVisualizationWindow() {
}

bool EventVisualizationWindow::init() {
	return true;
}

bool EventVisualizationWindow::bind() {
	return true;
}

bool EventVisualizationWindow::cleanUp() {
	return true;
}

QString EventVisualizationWindow::getName() const {
	return "EventVisualizationWindow";
}

void EventVisualizationWindow::createListView() {
	QScrollArea *listArea = new QScrollArea(this);
	listArea->setWidgetResizable(true);
	QWidget *listWidget = new QWidget(listArea);
	mListLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	mListLayout->setAlignment(Qt::AlignTop);
	mListLayout->setSpacing(1);
	listWidget->setLayout(mListLayout);
	listArea->setWidget(listWidget);
	mLayout->addWidget(listArea, 1,0);
}

void EventVisualizationWindow::createSearchView() {
	QGridLayout *searchLayout = new QGridLayout;
	QWidget *eventSearch = new QWidget;
	eventSearch->setLayout(searchLayout);
	mLayout->addWidget(eventSearch);

	mEventLineEdit = new QLineEdit;
	mEventLineEdit->setMinimumSize(350, 25);
	mEventLineEdit->setMaximumSize(350, 25);
	searchLayout->addWidget(mEventLineEdit, 0, 0);
	searchLayout->setAlignment(mEventLineEdit, Qt::AlignLeft);

	mAvailableValueCount = new QLabel;
	mAvailableValueCount->setMinimumSize(50, 25);
	mAvailableValueCount->setMaximumSize(50, 25);
	searchLayout->addWidget(mAvailableValueCount, 0, 1);
	searchLayout->setAlignment(mAvailableValueCount, Qt::AlignLeft);

	mEventComboBox = new QComboBox;
	mEventComboBox->setMinimumSize(350, 25);
	mEventComboBox->setMaximumSize(350, 25);
	mEventComboBox->setEditable(false);
	searchLayout->addWidget(mEventComboBox, 1, 0);
	searchLayout->setAlignment(mEventComboBox, Qt::AlignLeft);

	mAddSelectedEvents = new QPushButton("Add");
	searchLayout->addWidget(mAddSelectedEvents, 1, 1);
	searchLayout->setAlignment(mAddSelectedEvents, Qt::AlignRight);

	connect(mEventLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(fillEventList(const QString&)));
	connect(mEventLineEdit, SIGNAL(returnPressed()), this, SLOT(showMatchingEvents()));
	connect(mEventComboBox, SIGNAL(activated(const QString&)), mEventLineEdit, SLOT(setText(const QString&)));
	connect(mAddSelectedEvents, SIGNAL(clicked()), this, SLOT(showMatchingEvents()));
}

void EventVisualizationWindow::createControlButtonView() {
	QWidget *controlButtons = new QWidget;
	QHBoxLayout *controlButtonLayout = new QHBoxLayout;
	controlButtons->setLayout(controlButtonLayout);
	mLayout->addWidget(controlButtons);
	mSelectAllButton = new QPushButton("Select All");

	mRemoveAllEvents = new QPushButton("Remove All");

	mStepModeLabel = new QFrame(controlButtons);
	mSwitchToStepMode = new QCheckBox(mStepModeLabel);
	mTriggerStep = new QPushButton("Next Event", mStepModeLabel);
	mTriggerStep->setEnabled(false);

	QBoxLayout *mEventControlLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	mStepModeLabel->setLayout(mEventControlLayout);
	mEventControlLayout->addWidget(mSwitchToStepMode);
	mEventControlLayout->addWidget(mTriggerStep);
	mEventControlLayout->setAlignment(Qt::AlignLeft);

	mStepModeLabel->setFrameStyle(QFrame::Panel | QFrame::Plain);
	mStepModeLabel->setLineWidth(2);

	controlButtonLayout->addWidget(mSelectAllButton);
	controlButtonLayout->addWidget(mRemoveAllEvents);
	controlButtonLayout->addStretch(100);
	controlButtonLayout->addWidget(mStepModeLabel);

	connect(mRemoveAllEvents, SIGNAL(clicked()), this, SLOT(removeAllEventsFromList()));
	connect(mSwitchToStepMode, SIGNAL(stateChanged(int)), this, SLOT(changeMode(int)));
	connect(mTriggerStep, SIGNAL(clicked()), this, SLOT(moveToNextEvent()));
	connect(mSelectAllButton, SIGNAL(clicked()), this, SLOT(modifySelection()));
}

void EventVisualizationWindow::showMatchingEvents() {
	QVector<QString> selectedEvents;
	QString temp = mEventLineEdit->text();
	temp = temp.replace("**", ".*");
	temp.append(".*").prepend(".*");
	QVector<Event*> events = mEventManager->getEventsMatchingPattern(temp, false);

	for(int i = 0; i < events.size(); i++) {
		Event *event = events.at(i);
		QMap<Event*, EventVisualization*>::iterator index =
		mEventVisualizations.find(event);
		if(index == mEventVisualizations.end()) {
			EventVisualization *newVis = new EventVisualization(event, this);
			mEventVisualizations[event] = newVis;
			newVis->installEventFilter(this);
			mListLayout->addWidget(newVis, 0, Qt::AlignTop);
		}
	}
}

bool EventVisualizationWindow::eventFilter(QObject *object, QEvent *event) {
	if(event->type() == QEvent::Close) {
		EventVisualization *visu = dynamic_cast<EventVisualization*>(object);
		if(visu != NULL) {
			QMap<Event*, EventVisualization*>::iterator index = mEventVisualizations.find(visu->getEvent());
			if(index != mEventVisualizations.end()) {
				mEventVisualizations.erase(index);
			}
		}
		else {
			qDebug("EventVisualizationWindow: Close Event received from EventVisualization, but could not cast sender object to EventVisualization! Event ignored.");
			return true;
		}
	}
	return false;	// send event to the target object itself
}

void EventVisualizationWindow::fillEventList(const QString  &text) {
	QString temp = text;
	temp = temp.replace("**", ".*");
	temp.append(".*").prepend(".*");
	QVector<Event*> events = mEventManager->getEventsMatchingPattern(temp, false);
	mEventComboBox->clear();
	for(int i = 0; i < events.size(); i++) {
		Event *event = events.at(i);
		const QList<QString> &names = event->getNames();
		QListIterator<QString> it(names);
		for(; it.hasNext(); ) {
			QString name = it.next();
			mEventComboBox->addItem(name);
		}
		
	}
	QString number;
	number.setNum(events.size());
	mAvailableValueCount->setText(number);
}

void EventVisualizationWindow::removeAllEventsFromList() {
	mRemoveAllEvents->setEnabled(false);
	QMap<Event*, EventVisualization*>::iterator it = mEventVisualizations.begin();
	while(it != mEventVisualizations.end()) {
		EventVisualization *eventVisu = it.value();
		// erase the Visualization from the QMap before sending close event
		// so the event filter of this class doesn't erase the Visualization (SegFault!)
		it = mEventVisualizations.erase(it);
		eventVisu->close();
	}
	mRemoveAllEvents->setEnabled(true);
}

void EventVisualizationWindow::changeMode(int mode) {
	if(mode) {
		mStepModeLabel->setFrameStyle(QFrame::Panel | QFrame::Plain);
	    mStepModeLabel->setLineWidth(2);
		mStepModeLabel->setStyleSheet(
       "QFrame         { background-color: rgb(250,0,0); }\n"
       "QLabel         { color: black; }\n"
		);
		mBlockingMutex.lock();
		mBlockAfterEvent = true;
		mBlockingMutex.unlock();
	} 
	else {
		mStepModeLabel->setStyleSheet(
       "QFrame         { background-color: none; }\n"
       "QLabel         { color: black; }\n"
		);
		mStepModeLabel->setFrameStyle(QFrame::Panel | QFrame::Plain);
	    mStepModeLabel->setLineWidth(2);
		mBlockingMutex.lock();
		mBlockAfterEvent = false;
		mBlockingCondition.wakeAll();	// if someone is blocking, wake him up
		mBlockingMutex.unlock();
	}

	if(!mEventVisualizations.isEmpty()) {
		mTriggerStep->setEnabled(mode);
	}
}

void EventVisualizationWindow::moveToNextEvent() {
	mBlockingMutex.lock();
	mBlockingCondition.wakeAll();
	mBlockingMutex.unlock();
}

void EventVisualizationWindow::modifySelection() {
	mAllSelected = !mAllSelected;
	if(mAllSelected) {
		mSelectAllButton->setText("Deselect All");
	} 
	else {
		mSelectAllButton->setText("Select All");
	}
	QMap<Event*, EventVisualization*>::iterator index;
	for(index = mEventVisualizations.begin(); index != mEventVisualizations.end(); index++) {
		index.value()->setDoUpdate(mAllSelected);
	}
}

void EventVisualizationWindow::closeEvent(QCloseEvent*) {
	mXPos = x();
	mYPos = y();
}

void EventVisualizationWindow::showWindow() {
	move(mXPos, mYPos);
	show();
}

void EventVisualizationWindow::showEvent(QShowEvent*) {
	//Ensure that the comboBox is initially filled with the current parameter names.
	if(mEventLineEdit != 0) {
		fillEventList(mEventLineEdit->text());
	}
}

}
