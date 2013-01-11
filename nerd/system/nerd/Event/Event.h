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



#ifndef EVENTH_
#define EVENTH_

#include <QString>
#include <QList>

#include "Core/Object.h"


namespace nerd {

class EventListener;
class EventManager;

/**
 * Event.
 * 
 * Events are used to signal events or states between objects. Any object that has access to 
 * an Event can trigger that Event. This will lead to a notification of all registered
 * EventListeners of the Event. 
 * 
 * To use the Event meachanism modules can simply create Events and trigger these Events
 * whenever it is likely that other modules of the system might require to react on 
 * a specific event or phase. Then the Event has to be published at the EventManager.
 *
 * Events should never be created manually. Instead all modules should use the EventManager
 * and its createEvent() methods. These methods ensure that there are no Events with 
 * conflicting names, that the Event is properly registered at the EventListener
 *  and that Events can be found by other objects using their names.
 * 
 * To access an Event users should always use the EventManager to register. First the 
 * object has to implement the EventListener interface. Then the EventManager method
 * registerForEvent() can be used to look for and registere at an Event. 
 *
 * In addition to the name each Event also provides a short description in human readable
 * text, that can be used for debugging purposes or in a graphical user interface to 
 * give details about the function and purpose of the Event.
 *
 * Events support so called upstream Events. Such Events are added to another Event
 * and will be triggered right before the Event itself notifies its EventListeners, when
 * it was triggered.
 */
class Event : public virtual Object {

	public:
		Event(const QString &name, const QString &description = "");
		virtual ~Event();

		virtual void trigger();

		virtual QString getName() const;
		bool addName(const QString &name);
		bool removeName(const QString &name);
		QList<QString> getNames() const;
		
		QString getDescription() const;
		void setDescription(const QString &name);

		bool addEventListener(EventListener *eventListener);
		bool removeEventListener(EventListener *eventListener);

		bool addUpstreamEvent(Event *event);
		bool removeUpstreamEvent(Event *event);
		QList<Event*> getUpstreamEvents() const;
		QList<EventListener*> getEventListeners() const;

	private:
		QList<Event*> mUpstreamEvents;
		QList<EventListener*> mEventListeners;
		QList<EventListener*> mEventBuffer;
		QList<QString> mNames;
		QString mDescription;
		bool mTriggered;
		bool mTraceEventNotifications;
		EventManager *mEventManager;

};

}
#endif /*EVENTH_*/
