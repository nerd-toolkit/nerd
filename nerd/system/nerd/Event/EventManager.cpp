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



#include "./EventManager.h"
#include "./Event.h"
#include <QRegExp>
#include <iostream>
#include "Core/Core.h"
#include <QListIterator>
using namespace std;

namespace nerd {


/**
 * Constructs a new EventManager.
 */
EventManager::EventManager() {
}


/**
 * Destroys the EventManager. All available Events are hereby destroyed as well.
 * Since EventListeners and other objects might use an Event this is highly 
 * dangerous and should be performed only after the shutdown sequence of the Core
 * has been completed.
 */
EventManager::~EventManager() {
	mNotificationStack.clear();
	for(int i = 0; i < mEvents.size(); i++){
		delete mEvents.at(i);
	}
	mEvents.clear();
}



/**
 * Creates a new Event with the given name. If the name is already used by another 
 * Event, NULL is returned. Otherwise the newly created Event is returned.
 *
 * @param name the name of the Event to create.
 * @return the created Event, or NULL is the Event could not be created.
 */
Event* EventManager::createEvent(const QString &name) {
	return createEvent(name, "");
}


//synchronized
/**
 * Creates a new Event with the given name and description. 
 * If the name is already used by another 
 * Event, NULL is returned. Otherwise the newly created Event is returned.
 *
 * @param name the name of the Event to create.
 * @return the created Event, or NULL is the Event could not be created.
 */
Event* EventManager::createEvent(const QString &name, const QString &description) {
	for(int i = 0; i < mEvents.size(); i++) {
		Event *event = mEvents.at(i);

		QList<QString> names = event->getNames();
		QListIterator<QString> it(names);
		for(; it.hasNext();) {
			if(it.next().compare(name) == 0) {
				Core::log(QString("EventManager: Could not create event [")
						.append(name).append("]. Event already exists!"));
				return 0;
			}
		}
	}

	Event *event = new Event(name, description);
	mEvents.push_back(event);
	return event;
}


//synchronized
/**
 * Returns the Event with the given name. If createIfNotAvailable is true, then 
 * a new Event with the given name is created before returning it, if there is 
 * no such event with that name. 
 * 
 * @param name the name of the desired Event.
 * @param createIfNotAvailable if true an Event is created if no matching Event was found.
 * @return the Event with the given name, or NULL if no such Event could be found.
 */
Event* EventManager::getEvent(const QString &name, bool createIfNotAvailable) {
	for(int i = 0; i < mEvents.size(); i++) {
		Event *event = mEvents.at(i);
		QList<QString> names = event->getNames();
		QListIterator<QString> it(names);
		for(; it.hasNext();) {
			if(it.next().compare(name) == 0) {
				return event;
			}
		}
	}
	if(createIfNotAvailable) {
		Event *event = new Event(name);
		mEvents.push_back(event);
		return event;
	}
	return 0;
}



/**
 * Tries to register EventListener listener to the Event with the given name. 
 * If no such Event could be found NULL is returned.
 *
 * @param name the name of the desired Event to register for.
 * @param listener the listener to register.
 * @param createIfNotAvailable if this flag is true, then a warning is logged in case the event is missing.
 * @return the Event the listener was registered for, or NULL if no such Event could be found.
 */
Event* EventManager::registerForEvent(const QString &name, EventListener *listener, bool warnIfNotAvailable) {
	if(listener == 0) {
		return 0;
	}
	Event *event = getEvent(name, false);
	if(event != 0) {
		event->addEventListener(listener);
	}
	else {
		if(warnIfNotAvailable) {
			Core::log(QString("EventManager: Could not register EventListener [")
					.append(listener->getName()).append("] to event [")
					.append(name).append("]: Event not found."));
		}
	}
	return event;
}


/**
 * Tries to deregister an EventListener listener from the Event with the given name.
 * If no such Event could be found false is returned. Otherwise the EventListener
 * is deregistered and true is returned.
 *
 * @param name the name of the Event to deregister the listener from.
 * @param listener the listener to deregister.
 * @return true if successful, false if no matching Event could be found.
 */
bool EventManager::deregisterFromEvent(const QString &name, EventListener *listener) {
	if(listener == 0) {
		return false;
	}
	Event *event = getEvent(name, false);
	if(event != 0) {
		event->removeEventListener(listener);
		return true;
	}
	else {
		Core::log(QString("EventManager: Could not deregister listener [")
				.append(listener->getName()).append("] from event [")
				.append(name).append("]. Event not found."));
		return false;
	}
}



/**
 * Returns a vector with all Events whose names match the given regular expression.
 *
 * @param regularExpression the regularExpression to describe the desired names.
 * @return a vector with all matching Events.
 */
QVector<Event*> EventManager::getEventsMatchingPattern(const QString &regularExpression, 
													   bool caseSensitive) const 
{
	QVector<Event*> objects;
	QRegExp expr(regularExpression);
	if(caseSensitive) {
		expr.setCaseSensitivity(Qt::CaseSensitive);
	}
	else {
		expr.setCaseSensitivity(Qt::CaseInsensitive);
	}

	for(int i = 0; i < mEvents.size(); i++) {
		Event *event = mEvents.at(i);
		if(event == 0) {
			continue;
		}
		QList<QString> names = event->getNames();
		QListIterator<QString> it(names);
		for(; it.hasNext();) {
			QString name = it.next();
			if(expr.exactMatch(name)) {
				if(!objects.contains(event)) {
					objects.push_back(event);
				}
			}
		}
	}
	return objects;
}


/**
 * Returns a vector with all Event names that match the given regular expression.
 * 
 * @param regularExpression the regularExpression to describe the matching names.
 * @return a vector with all matching names of Events.
 */
QVector<QString> EventManager::getNamesMatchingPattern(const QString &regularExpression, 
													   bool caseSensitive) const 
{
	QVector<QString> objects;
	QRegExp expr(regularExpression);

	if(caseSensitive) {
		expr.setCaseSensitivity(Qt::CaseSensitive);
	}
	else {
		expr.setCaseSensitivity(Qt::CaseInsensitive);
	}

	for(int i = 0; i < mEvents.size(); i++) {
		Event *event = mEvents.at(i);
		if(event == 0) {
			continue;
		}
		QList<QString> names = event->getNames();
		QListIterator<QString> it(names);
		for(; it.hasNext();) {
			QString name = it.next();
			if(expr.exactMatch(name)) {
				objects.push_back(name);
			}
		}
	}
	return objects;
}


/**
 * Returns a vector containing all available Events.
 * The returned vector is a reference to the internally used vector
 * holding all Events, so its content can change when Events are created.
 *
 * @return a vector with all available Events.
 */
QVector<Event*> EventManager::getEvents() const {
	return mEvents;
}


//deprecated
void EventManager::pushToNotificationStack(Object *object) {
	mNotificationStack.push_back(object);
}

//deprecated
void EventManager::popFromNotificationStack() {
	mNotificationStack.pop_back();
}

//deprecated
QVector<Object*> EventManager::getNotificationStack() const {
	return mNotificationStack;
}


}

