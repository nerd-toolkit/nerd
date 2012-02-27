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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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



#include "EventCollisionRule.h"
#include "Core/Core.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Value/StringValue.h"
#include <QStringList>
#include <iostream>

using namespace std;

namespace nerd {

EventCollisionRule::EventCollisionRule(const QString &name) 
	: CollisionRule(name), mEventList(0)
{
	mEventList = new StringValue();
	addParameter("Events", mEventList);
}

EventCollisionRule::EventCollisionRule(const EventCollisionRule &rule) 
	: Object(), ValueChangedListener(), CollisionRule(rule) 
{
	mEventsToAddOnInit.clear();
	mGroupEvents.clear();
	
	mEventList = dynamic_cast<StringValue*>(getParameter("Events"));
}

EventCollisionRule::~EventCollisionRule() {
}

CollisionRule* EventCollisionRule::createCopy() const {
	return new EventCollisionRule(*this);
}

void EventCollisionRule::set(QList<CollisionObject*> sourceGroup, 
						QList<CollisionObject*> targetGroup, 
						const QString &eventName)
{
	CollisionRule::set(sourceGroup, targetGroup);
	
	Event *mGroupEvent = Core::getInstance()->getEventManager()
					->getEvent(eventName, true);
	if(mGroupEvent != 0) {
		mGroupEvents.push_back(mGroupEvent);
	}
}


void EventCollisionRule::set(QList<CollisionObject*> sourceGroup, 
							QList<CollisionObject*> targetGroup, 
							QList<QString> eventNames)
{
	CollisionRule::set(sourceGroup, targetGroup);
	
	for(int i = 0; i < eventNames.size(); i++) {
		Event *groupEvent = Core::getInstance()->getEventManager()
				->getEvent(eventNames.at(i), true);
					
		if(groupEvent != 0) {
			addEvent(groupEvent);
		}
	}
}


void EventCollisionRule::addEvent(const QString &eventName) {
	Event *event = Core::getInstance()->getEventManager()->getEvent(eventName, true);

	if(event == 0 && !Core::getInstance()->isInitialized()) {
		mEventsToAddOnInit.push_back(eventName);
	} 
	else {
		addEvent(event);
	}
}


/**
 * Adds an Event to be triggered when the collision rule holds.
 * 
 * NOTE: The name of this Event is not verified and can lead
 * to interferences with system events if improperly used!
 *
 * @param event the event to add.
 */
void EventCollisionRule::addEvent(Event *event) {
	if(event != 0 && mGroupEvents.indexOf(event) == -1) {
		mGroupEvents.push_back(event);
	}
}


/**
 * Removes an event from this Rule.
 *
 * NOTE: the eventName is verified, so non-verified events can only
 * be removed with method removeEvent(Event).
 *
 * @param eventName the name of the Event to be removed.
 */
void EventCollisionRule::removeEvent(const QString &eventName) {
	Event *event = Core::getInstance()->getEventManager()
						->getEvent(eventName, true);
	if(event != 0) {
		mGroupEvents.removeAll(event);
	}
}

void EventCollisionRule::removeEvent(Event *event) {
	if(event != 0) {
		mGroupEvents.removeAll(event);
	}
}

void EventCollisionRule::replaceEvent(const QString &oldName, const QString &newName) {
	Event *oldEvent = Core::getInstance()->getEventManager()
							->getEvent(oldName, false);

	Event *newEvent = Core::getInstance()->getEventManager()
							->getEvent(newName, true);

	if(oldEvent != 0) {
		int index = mGroupEvents.indexOf(oldEvent);
		if(index != -1) {
			mGroupEvents.removeAt(index);
		}
	}
	if(newEvent != 0 && !mGroupEvents.contains(newEvent)) {
		mGroupEvents.append(newEvent);
	}
	else if(!Core::getInstance()->isInitialized()){
		mEventsToAddOnInit.push_back(newName);
	}
}

void EventCollisionRule::replaceEvent(Event *oldEvent, Event *newEvent) {
	if(oldEvent == 0 || newEvent == 0) {
		return;
	}
	
	int index = mGroupEvents.indexOf(oldEvent);
	if(index != -1) {
		mGroupEvents.removeAt(index);
	}
	if(newEvent != 0) {
		mGroupEvents.append(newEvent);
	}
}


QList<Event*> EventCollisionRule::getEvents() const {
	return mGroupEvents;
}

void EventCollisionRule::setEvents(QList<Event*> events) {
	mGroupEvents = events;
}


bool EventCollisionRule::handleContact(const Contact &contact) {
	if((mSourceGroup.contains(contact.getFirstCollisionObject()) 
			&& mTargetGroup.contains(contact.getSecondCollisionObject())) 
		|| 
			(mSourceGroup.contains(contact.getSecondCollisionObject())
			&& mTargetGroup.contains(contact.getFirstCollisionObject())))
	{
		if(!mNegateRule) {
			triggerEvents();
			return true;
		}
		else {
			return false;
		}
	}
	else if(mNegateRule) {
		triggerEvents();
		return true;
	}
	return false;
}

void EventCollisionRule::triggerEvents() {
	for(int i = 0; i < mGroupEvents.size(); i++) {
		mGroupEvents.at(i)->trigger();
	}
}

bool EventCollisionRule::init() {

	bool ok = CollisionRule::init();

	for(int i = 0; i < mEventsToAddOnInit.size(); i++) {
		addEvent(mEventsToAddOnInit.at(i));
	}

	QString eventsString = mEventList->get();
	QStringList events = eventsString.split(",", QString::SkipEmptyParts);
	
	for(int i = 0; i < events.size(); i++) {
		addEvent(events.at(i));
	}
	
	return ok;
}

}





