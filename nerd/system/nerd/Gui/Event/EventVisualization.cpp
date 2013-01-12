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

#include "EventVisualization.h"
#include "EventVisualizationWindow.h"
#include "Core/Core.h"
#include "Core/Task.h"
#include "Event/Event.h"
#include <QApplication>
#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>

namespace nerd{

class CloseTask : public Task {

	public:
	CloseTask(Event *event, EventVisualization *visu) {
		mEvent = event;
		mVisu = visu;
	}

	virtual ~CloseTask() {}

	virtual bool runTask() {
		if(mEvent != 0) {
			mEvent->removeEventListener(mVisu);
		}
		else {
			Core::log("EventVisualization Error: mEvent == 0 in CloseTask::runTask()");
		}
		mVisu->mCloseDownMutex.lock();
		mVisu->mCloseDownWaitCondition.wakeAll();
		mVisu->mCloseDownMutex.unlock();
		return true;
	}

	private:
		Event *mEvent;
		EventVisualization *mVisu;
};

EventVisualization::EventVisualization(Event *event, EventVisualizationWindow *mainWindow) {
	mEvent = event;
	mMainWindow = mainWindow;

	setAttribute(Qt::WA_DeleteOnClose, true);

	mNumberOfTriggering = 0;
	mDoUpdate = true;

	QLabel *nameLabel = new QLabel(mEvent->getName());

	mUpdateTriggerCount = new QCheckBox();
	mUpdateTriggerCount->setCheckState(Qt::Checked);

	mEventNumberOfTriggers = new QLabel(QString::number(mNumberOfTriggering));

	QPushButton *resetButton = new QPushButton("reset");
	mCloseButton = new QPushButton("x");

	QVBoxLayout *layout = new QVBoxLayout;
	QHBoxLayout *subLayout = new QHBoxLayout;

	setLayout(layout);
	layout->setMargin(1);
	layout->addWidget(nameLabel);
	layout->addLayout(subLayout);
	subLayout->addWidget(mUpdateTriggerCount);
	subLayout->addWidget(mEventNumberOfTriggers);
	subLayout->addStretch(100);
	subLayout->addWidget(resetButton);
	subLayout->addWidget(mCloseButton);

	mCloseButton->setFixedWidth(20);

	connect(mUpdateTriggerCount, SIGNAL(stateChanged(int)), this, SLOT(setDoUpdateSlot(int)));
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetTriggerCount()));
	connect(mCloseButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(this, SIGNAL(numberChanged(QString)), mEventNumberOfTriggers, SLOT(setText(QString)));
	connect(this, SIGNAL(changeFrameColorSignal(int)), this, SLOT(changeFrameColorSlot(int)));
	connect(this, SIGNAL(changeTriggerStepButton(bool)), mMainWindow->mTriggerStep, SLOT(setEnabled(bool)));

	QPalette framePalette = palette();
 	framePalette.setColor(QPalette::Normal, QPalette::Foreground, Qt::black);
 	setPalette(framePalette);
	setFrameStyle(QFrame::Panel | QFrame::Plain);
    setLineWidth(2);

	mEvent->addEventListener(this);
}

EventVisualization::~EventVisualization() {

}

void EventVisualization::setDoUpdate(int doUpdate) {
	if(doUpdate == 0) {
		mUpdateTriggerCount->setCheckState(Qt::Unchecked);
	} 
	else {
		mUpdateTriggerCount->setCheckState(Qt::Checked);
	}
}

void EventVisualization::setDoUpdateSlot(int doUpdate) {
	mMutex.lock();
	if(doUpdate == 0) {
		mDoUpdate = false;
	} 
	else {
		mDoUpdate = true;
	}
	mMutex.unlock();
}

void EventVisualization::eventOccured(Event *event) {
	if(mEvent != 0) {
		if(mEvent == event) {
			mMutex.lock();
			if(mDoUpdate) {
				mNumberOfTriggering++;
				emit numberChanged(QString::number(mNumberOfTriggering));	
			}
			mMutex.unlock();
			mMainWindow->mBlockingMutex.lock();
			if(mMainWindow->mBlockAfterEvent) {
				emit changeTriggerStepButton(true);
				emit changeFrameColorSignal(1);
				mMainWindow->mBlockingCondition.wait(&mMainWindow->mBlockingMutex);
				emit changeTriggerStepButton(false);
				mMainWindow->mBlockingMutex.unlock();
				emit changeFrameColorSignal(0);
			} 
			else {
				mMainWindow->mBlockingMutex.unlock();
			}
		}
	}
}

void EventVisualization::closeEvent(QCloseEvent *e) {
	removeEvent();		// be sure that EventListener is deregistered
	e->accept();		// accept the Event to delete the EventVisualization
}

void EventVisualization::removeEvent() {
	setDoUpdate(false);
	mCloseButton->setEnabled(false);

	mMainWindow->mBlockingMutex.lock();
	if(mMainWindow->mBlockAfterEvent) {
		mMainWindow->mBlockAfterEvent = false;	// don't allow to block while deregistering EventListener
		mMainWindow->mBlockingCondition.wakeAll();	// if someone is blocking, wake him up
		mMainWindow->mBlockingMutex.unlock();
		CloseTask *task = new CloseTask(mEvent, this);
		mCloseDownMutex.lock();
		Core::getInstance()->scheduleTask(task);
		mCloseDownWaitCondition.wait(&mCloseDownMutex);
		mCloseDownMutex.unlock();
		mMainWindow->mBlockingMutex.lock();
		mMainWindow->mBlockAfterEvent = true;
		mMainWindow->mBlockingMutex.unlock();
	} 
	else {
		mMainWindow->mBlockingMutex.unlock();
		CloseTask *task = new CloseTask(mEvent, this);
		mCloseDownMutex.lock();
		Core::getInstance()->scheduleTask(task);
		mCloseDownWaitCondition.wait(&mCloseDownMutex);
		mCloseDownMutex.unlock();
	}
}

void EventVisualization::resetTriggerCount() {
	mMutex.lock();
	mNumberOfTriggering = 0;
	emit numberChanged(QString::number(mNumberOfTriggering));
	mMutex.unlock();
}

Event* EventVisualization::getEvent() {
	return mEvent;
}

QString EventVisualization::getName() const {
	return "EventVisualization";
}

void EventVisualization::changeFrameColorSlot(int color) {
	if(color == 1) {
		setStyleSheet(
        "QFrame         { background-color: rgb(0,250,0); }\n"
        "QLabel         { color: black; }\n"
		);
	} 
	else{
		setStyleSheet(
        "QFrame         { background-color: none; }\n"
		);
	}
}

}
