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


#include "KeyFrameSniffer.h"
#include <iostream>
#include "Physics/PhysicsManager.h"
#include <QCoreApplication>
#include "Physics/Physics.h"

using namespace std;

namespace nerd {

KeyFrameSniffer::KeyFrameSniffer(const QString &fileName, SimObjectGroup *group)
	: SystemObject(), EventListener(), mInterface(group),
        mMotorSetMessageReceivedEvent(0), mDisconnectEvent(0), mTimeStarted(false),
		mLastSettings("")
{
	Physics::getPhysicsManager()->addSimObjectGroup(group);

	mKeyFrameListener = new SimpleUdpMotorInterface(group->getName());

	mFile = new QFile(fileName);
	if(!mFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
		Core::log(QString("SimpleUdpMotorInterface: Could not open file ").append(fileName));
		delete mFile;
		mFile = 0;
	}
	else {
		mWriter = new QTextStream(mFile);
		(*mWriter) << "#KeyFrame Sniffer Data File" << endl;
	}

	Core::getInstance()->addSystemObject(this);
}

KeyFrameSniffer::~KeyFrameSniffer() {

	if(mWriter != 0) {
		delete mWriter;
	}
	if(mFile != 0) {
		mFile->close();
		delete mFile;
	}
}


bool KeyFrameSniffer::init() {

	mRunner = new SimpleUdpMotorInterfaceRunner(mKeyFrameListener);
	mRunner->start();

	return true;
}


bool KeyFrameSniffer::bind() {

	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mMotorSetMessageReceivedEvent = em->registerForEvent(
			"/Communication/SimpleUdpMotorInterface/ReceivedSetEvent", this);
	mDisconnectEvent = em->registerForEvent(
			"/Communication/SimpleUdpMotorInterface/DisconnectionEvent", this);

	if(mMotorSetMessageReceivedEvent == 0) {
		Core::log("Could not find required Event "
				"[/Communication/SimpleUdpMotorInterface/ReceivedSetEvent]");
		ok = false;
	}
    if(mDisconnectEvent == 0) {
		Core::log("Could not find required Event "
				"[/Communication/SimpleUdpMotorInterface/DisconnectionEvent]");
		ok = false;
	}

	return ok;
}


bool KeyFrameSniffer::cleanUp() {

	return true;
}


void KeyFrameSniffer::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mMotorSetMessageReceivedEvent) {
        storeKeyFrame();
	}
	else if(event == mDisconnectEvent) {
        QCoreApplication::instance()->quit();
	}
}


QString KeyFrameSniffer::getName() const {
	return "KeyFrameSniffer";
}


void KeyFrameSniffer::storeKeyFrame() {
	if(!mTimeStarted) {
		mTime.start();
		mTimeStarted = true;
	}

	long ms = mTime.elapsed();

	if(mWriter == 0 || mInterface == 0) {
		return;
	}
	QString settings;

	QList<InterfaceValue*> inputs = mInterface->getInputValues();
	for(int i = 0; i < inputs.size(); ++i) {
		settings.append(QString::number(inputs.at(i)->get()));
		if(i < inputs.size() - 1) {
			settings.append(",");
		}
	}

	if(settings != mLastSettings) {
		mLastSettings = settings;
		(*mWriter) << ms << "," << mLastSettings << endl;;
	}

	mWriter->flush();
}


}


