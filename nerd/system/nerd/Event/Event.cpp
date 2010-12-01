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

#include "Event.h"
#include <algorithm>
#include "EventListener.h"
#include "EventManager.h"
#include "Core/Core.h"
#include <iostream>

using namespace std;

namespace nerd {


/**
 * Constructs an event with the given name and the Event description.
 *
 * @param name the name of the Event.
 * @param description a short description of this Event in a human readable format.
 */
Event::Event(const QString &name, const QString &description) : mDescription(description),
		mTriggered(false), mTraceEventNotifications(false),
		mEventManager(0) 
{
	mEventManager = Core::getInstance()->getEventManager();
	mNames.append(name);
}


/**
 * Destroy the Event.
 * Does not destroy the registered EventListeners.
 */
Event::~Event() {
}


QString Event::getName() const {
	QString name("Event [");
	for(int i = 0; i < mNames.size(); ++i) {
		name.append(mNames.at(i));
		if(i < mNames.size() - 1) {
			name.append(", ");
		}
	}
	name.append("]");
	return name;
}


/**
 * Adds an (alias) name to the Event.
 * The Event can be identified by any of these names.
 * 
 * @param name the name to add.
 */
bool  Event::addName(const QString &name) {
	if(mNames.contains(name)) {
		return false;
	}
	mNames.append(name);
	return true;
}

/**
 * Removes a name from the Event. If there is only one name, then this method
 * will fail and return false, as at least one name is mandatory for an Event.
 * 
 * @param name the name to remove.
 */
bool  Event::removeName(const QString &name) {
	if(!mNames.contains(name) || mNames.size() <= 1) {
		return false;
	}
	mNames.removeAll(name);
	return true;
}


/**
 * Returns the name of this Event.
 *
 * @return the name of the Event.
 */
QList<QString> Event::getNames() const {
	return mNames;
}


/**
 * Sets the description of the Event. This description should briefly mention
 * what this Event is for, so that debugging tools can display this information
 * to the users.
 *
 * @param description a brief descritption of the Event's purpose.
 */
void Event::setDescription(const QString &description) {
	mDescription = description;
}


/**
 * Returns the description of the Event.
 * 
 * @return the description of the Event.
 */
QString Event::getDescription() const{
	return mDescription;
}


/**
 * Triggers the Event. 
 * This will call the eventOccured() method for all registered EventListeners, with 
 * this as argument. 
 *
 * The trigger() method will print out a warning if the Event was not executed in the 
 * main execution thread, which can lead to critical problems. However the Event
 * does not prevent the wrong thread to execute the Event to ensure that the application
 * is not blocked or stopped because of an ill treated Event call.
 */
void Event::trigger() {
	if(mTriggered) {
		Core::log(QString("Warning: ").append(getName())
					.append(" was triggered while executing!"));
		return;
	}

	if(!Core::getInstance()->isMainExecutionThread()) {
		Core::log(QString("Warning: ").append(getName())
			.append(" was triggered outside of main execution thread!"));
		Core::log("  Maybe the Event should be triggered by a Task object?");
	}

	mTriggered = true;
	if(mTraceEventNotifications) {
		//TODO deprecated
		mEventManager->pushToNotificationStack(this);
	}

	for(int i = 0; i < mUpstreamEvents.size(); ++i) {
		mUpstreamEvents.at(i)->trigger();
	}

	mEventBuffer.clear();
	mEventBuffer = mEventListeners;
	if(mTraceEventNotifications) {
		for(int index = 0; index < mEventBuffer.size(); index++) {
			EventListener *listener = mEventBuffer.at(index);
			//TODO deprecated
			mEventManager->pushToNotificationStack(listener);
			mEventManager->popFromNotificationStack();

			listener->eventOccured(this);
		}
	}
	else {
		//cerr << "Event: " << getName().toStdString().c_str() << endl;
		for(int index = 0; index < mEventBuffer.size(); index++) {
			EventListener *e = mEventBuffer.at(index);
			if(mEventListeners.contains(e)) {
				e->eventOccured(this);
			}
		}
	}

	if(mTraceEventNotifications) {
		//TODO deprecated
		mEventManager->popFromNotificationStack();
	}

	mTriggered = false;
}

/**
 * Adds a new EventListner to this Event. EventListeners are notified when the 
 * Event is triggered with method trigger().
 * 
 * @param eventListener the listener to add.
 * @return true if successful, false if already contained of adding not possible.
 */
bool Event::addEventListener(EventListener *eventListener) {
	if(eventListener == 0 || mEventListeners.contains(eventListener)) {
		return false;
	}

	mEventListeners.prepend(eventListener);
	return true;
}


/**
+* Removes an EventListener from the Event. The EventListener will not be notified 
 * any more when the Event is triggerd until it is added again. 
 * The EventListener is not destroyed.
 * 
 * @param eventListener the listener to remove.
 * @return true if successful, false if not contained.
 */
bool Event::removeEventListener(EventListener *eventListener) {
	if(eventListener == 0 || !mEventListeners.contains(eventListener)) {
		return false;
	}

	mEventListeners.removeAll(eventListener);
	return true;
}


/**
 * Returns a vector with all currently registered EventListeners.
 * As this is a reference to the internally used vector, it might change
 * when EventListeners are added or removed to the Event.
 * 
 * @return a vector with all registered EventListeners.
 */
QList<EventListener*> Event::getEventListeners() const {
	return mEventListeners;
}


/**
 * Adds another Event to the Event as upstream Event. 
 * All upstream Events will be triggered in any order BEFORE this Event will notify
 * its EventListeners, when this Event is triggered. Therefore upstream Events can 
 * be used to get a notification link that is guaranteed to execute shortly before
 * this Event.
 * 
 * This method is seldom useful, but sometimes it is required to react before the 
 * regular EventListeners are executed. In this case this mechanism can be used.
 * 
 * @param event the Event to add as upstream Event.
 * @return true if successful, false if already contained.
 */
bool Event::addUpstreamEvent(Event *event) {
	if(event == 0 || mUpstreamEvents.contains(event)) {
		return false;
	}

	mUpstreamEvents.append(event);
	return true;
}


/**
 * Removes an upstream Event from this Event. 
 * The removed Event is not destroyed.
 * 
 * @param event the Event to remove from the upsteam event list.
 * @return true if successful, false if not contained.
 */
bool Event::removeUpstreamEvent(Event *event) {
	if(event == 0 || !mUpstreamEvents.contains(event)) {
		return false;
	}

	mUpstreamEvents.removeAll(event);
	return true;
}

/**
 * Returns a vector with all currently registered upstream Events.
 * As this vector is a reference to the internally used vector, it might change
 * due to calls to addUpstreamEvent() and removeUpstreamEvent().
 *
 * @return a vector with all upstream Events of this Event.
 */
QList<Event*> Event::getUpstreamEvents() const{
	return mUpstreamEvents;
}

}
