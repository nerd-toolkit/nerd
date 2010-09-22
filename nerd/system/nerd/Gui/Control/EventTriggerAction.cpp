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



#include "EventTriggerAction.h"
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "Event/TriggerEventTask.h"
#include <QListIterator>

namespace nerd {


EventTriggerAction::EventTriggerAction(const QString &actionName, const QList<QString> &eventNames)
	: QAction(actionName, 0), mEventNames(eventNames), mActivationBoolValue(0)
{
	mName = QString("EventTriggerAction (").append(actionName).append(")");
	connect(this, SIGNAL(triggered()), this, SLOT(triggerActivated()));
	connect(this, SIGNAL(enableAction(bool)), this, SLOT(setEnabled(bool)));

	Core::getInstance()->addSystemObject(this);
}

EventTriggerAction::EventTriggerAction(const QString &actionName, const QList<QString> &eventNames,
					const QString &activationBoolValueName, bool requiredBoolState)
	: QAction(actionName, 0), mEventNames(eventNames), 
		mActivationBoolValueName(activationBoolValueName), mActivationBoolValue(0),
		mRequiredBoolState(requiredBoolState)
{
	mName = QString("EventTriggerAction (").append(actionName).append(")");
	connect(this, SIGNAL(triggered()), this, SLOT(triggerActivated()));
	connect(this, SIGNAL(enableAction(bool)), this, SLOT(setEnabled(bool)));

	Core::getInstance()->addSystemObject(this);
}

EventTriggerAction::~EventTriggerAction() {
}

bool EventTriggerAction::init() {
	return true;
}


bool EventTriggerAction::bind() {
	bool ok = true;

	for(QListIterator<QString> i(mEventNames); i.hasNext();) {
		QString name = i.next();
		Event *event = Core::getInstance()->getEventManager()->getEvent(name);
		if(event == 0) {
			Core::log(QString("EventTriggerAction: Could not find event [")
					.append(name).append("] [DISABLING FUNCTION]"));
			ok = false;
			break;
		}
		else {
			mEvents.append(event);
		}
	}

	if(mActivationBoolValueName.compare("") != 0) {
		mActivationBoolValue = Core::getInstance()->getValueManager()
				->getBoolValue(mActivationBoolValueName);
		if(mActivationBoolValue == 0) {
			Core::log(QString("EventTriggerAction: Could not find required value [")
						.append(mActivationBoolValueName).append("]!"));
			return false;
		}
		mActivationBoolValue->addValueChangedListener(this);
	}
	if(!ok) {
		emit enableAction(false);
	}
	else if(mActivationBoolValue != 0) {
		emit enableAction(mActivationBoolValue->get() == mRequiredBoolState);
	}
	else {
		emit enableAction(true);
	}
	return ok;
}


bool EventTriggerAction::cleanUp() {
	if(mActivationBoolValue != 0) {
		mActivationBoolValue->removeValueChangedListener(this);
	}
	Core::getInstance()->removeSystemObject(this);
	return true;
}

void EventTriggerAction::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mActivationBoolValue) {
		emit enableAction(mActivationBoolValue->get() == mRequiredBoolState);
	}
}


QString EventTriggerAction::getName() const {
	return mName;
}


void EventTriggerAction::triggerActivated() {
	for(QListIterator<Event*> i(mEvents); i.hasNext();) {
		TriggerEventTask::scheduleEvent(i.next());
	}
}


}



