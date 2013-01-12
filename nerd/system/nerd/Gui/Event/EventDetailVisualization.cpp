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


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include "EventDetailVisualization.h"
#include "Event/EventListener.h"

#include <iostream>

using namespace std;

namespace nerd{

/**
 * Constructor.
 *
 * @param event the event whos properties will be visualized.
 */
EventDetailVisualization::EventDetailVisualization(Event *event) {
	mEvent = event;

	setAttribute(Qt::WA_DeleteOnClose, true);

	mNameLabel = new QLabel(event->getName());
	mCloseEvent = new QPushButton("x");
	mCloseEvent->setFixedWidth(20);

	QVBoxLayout *layout = new QVBoxLayout;
	QHBoxLayout *subLayout = new QHBoxLayout;

//	layout->setMargin(1);	
	setLayout(layout);
	layout->addLayout(subLayout);

	subLayout->addWidget(mNameLabel);
	subLayout->addWidget(mCloseEvent);

	mTextEdit = new QTextEdit();
	mTextEdit->setReadOnly(true);
	mTextEdit->setLineWrapMode(QTextEdit::NoWrap);
	mTextEdit->setFixedHeight(120);
	layout->addWidget(mTextEdit);

	fillContents();

	setFrameStyle(QFrame::Panel | QFrame::Plain);
	setLineWidth(2);

	connect(mCloseEvent, SIGNAL(clicked()), this, SLOT(close()));
}

/**
 * This slot fills the TextEdit of the Visualization with the properties of the event.
 */
void EventDetailVisualization::fillContents() {
	QString textEditContent;

	textEditContent.append("<b>Names:</b><br>");
	QList<QString> eventNames = mEvent->getNames();
	QListIterator<QString> itNames(eventNames);
	for(; itNames.hasNext(); ) {
		textEditContent.append(itNames.next()).append("<br>");
	}

	textEditContent.append("<b>Description:</b><br>");
	QString description = mEvent->getDescription();
	if(description.isEmpty()) {
		textEditContent.append("--<br>");
	}
	else {
		textEditContent.append(mEvent->getDescription()).append("<br>");
	}

	textEditContent.append("<b>EventListeners:</b><br>");
	QList<EventListener*> eventListeners = mEvent->getEventListeners();
	QListIterator<EventListener*> itListeners(eventListeners);
	if(!itListeners.hasNext()) {
		textEditContent.append("--<br>");
	}
	else {
		for(; itListeners.hasNext(); ) {
			textEditContent.append(itListeners.next()->getName()).append("<br>");
		}
	}

	textEditContent.append("<b>UpstreamEvents:</b><br>");
	QList<Event*> events = mEvent->getUpstreamEvents();
	QListIterator<Event*> itEvents(events);
	if(!itEvents.hasNext()) {
		textEditContent.append("--<br>");
	}
	else {
		for(; itEvents.hasNext(); ) {
			textEditContent.append(itEvents.next()->getName()).append("<br>");
		}
	}
	
	mTextEdit->setText(textEditContent);
}

Event* EventDetailVisualization::getEvent() {
	return mEvent;
}

}
