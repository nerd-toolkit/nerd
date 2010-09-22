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


#include "SimpleUdpMotorInterface.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include <iostream>
#include <QByteArray>
#include "Physics/PhysicsManager.h"
#include "Core/Task.h"
#include "Physics/Physics.h"
#include "NerdConstants.h"

using namespace std;

namespace nerd {

class ApplyMotorSettingsTask : public Task {
	public:
	ApplyMotorSettingsTask(QList<InterfaceValue*> &motorValues, QList<double> &values)
		: mMotors(motorValues), mValues(values) {}

	bool runTask() {
		if(mMotors.size() >= mValues.size()) {
			return false;
		}
		for(int i = 0; i < mMotors.size() && i < mValues.size(); ++i) {
			mMotors.at(i)->set(mValues.at(i));
		}
		return true;
	}

	private:
		QList<InterfaceValue*> mMotors;
		QList<double> mValues;
};


SimpleUdpMotorInterface::SimpleUdpMotorInterface(
				const QString &controlledGroupName)
		: mNextStepEvent(0), mStepCompletedEvent(0), mReceivedMotorSetMessageEvent(0),
            mConnectEvent(0), mDisconnectEvent(0),
			mUdpServerSocket(0), mPortValue(0),
			mLastReceivedHostAddress(0), mRunServerLoop(true), mServerRunning(false),
			mControlledGroup(0), mControlledGroupName(0), mHeadPanDesiredAngle(0),
			mHeadTiltDesiredAngle(0)
{
	Core::getInstance()->addSystemObject(this);

	mPortValue = new IntValue(45453);

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/SimpleUdpMotorInterface/ServerPort", mPortValue);

	mControlledGroupName = new StringValue(controlledGroupName);
	vm->addValue("/SimpleUdpMotorInterface/ControlledAgent", mControlledGroupName);

	mHeadPanDesiredAngle = new DoubleValue(512.0);
	vm->addValue("/SimpleUdpMotorInterface/DesiredHeadPanAngleWord", mHeadPanDesiredAngle);

	mHeadTiltDesiredAngle = new DoubleValue(512.0);
	vm->addValue("/SimpleUdpMotorInterface/DesiredHeadTiltAngleWord", mHeadTiltDesiredAngle);

	mLastReceivedHostAddress = new QHostAddress();
	mLastReceivedPort = 0;
}

SimpleUdpMotorInterface::~SimpleUdpMotorInterface() {
	if(mUdpServerSocket != 0)  {
		delete mUdpServerSocket;
	}
	delete mLastReceivedHostAddress;
}

bool SimpleUdpMotorInterface::init() {
    bool ok = true;

	Core::getInstance()->registerThread(this);

	EventManager *em = Core::getInstance()->getEventManager();

	mReceivedMotorSetMessageEvent =
 			em->createEvent("/Communication/SimpleUdpMotorInterface/ReceivedSetEvent");
    mConnectEvent =
 			em->createEvent("/Communication/SimpleUdpMotorInterface/ConnectionEvent");
    mDisconnectEvent =
 			em->createEvent("/Communication/SimpleUdpMotorInterface/DisconnectionEvent");

	if(mReceivedMotorSetMessageEvent == 0) {
		Core::log("SimpleUdpMotorInterface: Could not create Event "
				"[/Communication/SimpleUdpMotorInterface/ReceivedEvent]");
		ok = false;
	}
    if(mConnectEvent == 0) {
		Core::log("SimpleUdpMotorInterface: Could not create Event "
				"[/Communication/SimpleUdpMotorInterface/ConnectionEvent");
		ok = false;
	}
	if(mDisconnectEvent == 0) {
		Core::log("SimpleUdpMotorInterface: Could not create Event "
				"[/Communication/SimpleUdpMotorInterface/DisconnectionEvent]");
		ok = false;
	}

	return ok;
}


bool SimpleUdpMotorInterface::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mNextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	mStepCompletedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);

	if(mNextStepEvent == 0) {
		ok = false;
		qDebug("SimbaUDPCommunication: Could not find Event [mNextStepEvent]");
	}
	if(mStepCompletedEvent == 0) {
		ok = false;
		qDebug("SimbaUDPCommunication: Could not find Event [StepCompleted]");
	}

	return ok;
}


bool SimpleUdpMotorInterface::cleanUp() {
	//shut down the upd communication and the upd server.
	stopUdpServer();
	Core::getInstance()->deregisterThread(this);

	return true;
}


void SimpleUdpMotorInterface::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(event == mNextStepEvent) {
		mMotorMutex.lock();
	}
	else if(event == mStepCompletedEvent) {
		mMotorMutex.unlock();
	}
}




QString SimpleUdpMotorInterface::getName() const {
	return "SimpleUdpMotorInterface";
}

void SimpleUdpMotorInterface::run() {

	startUdpServer();
}

void SimpleUdpMotorInterface::startListening() {
	cerr << "SimpleUdpMotorInterface listening at port: "
			<< mPortValue->get() << endl;
	if(mUdpServerSocket == 0) {
		mUdpServerSocket = new QUdpSocket();
		connect(mUdpServerSocket, SIGNAL(readyRead()),
                this, SLOT(datagramReceived()), Qt::DirectConnection);
		mUdpServerSocket->bind(mPortValue->get());
	}
}

bool SimpleUdpMotorInterface::startUdpServer() {
	if(mUdpServerSocket == 0) {
		return false;
	}

	//get controlled SimObjectGroups
	cerr << "SimpleUpdMotorInterface looking for agent ["
		<< mControlledGroupName->get().toStdString().c_str()
		<< "] ";
	const QList<SimObjectGroup*> &groups = Physics::getPhysicsManager()
					->getSimObjectGroups();
	for(int i = 0; i < groups.size(); ++i) {
		SimObjectGroup *group = groups.at(i);
		if(group != 0 && group->getName().compare(mControlledGroupName->get()) == 0) {
			mControlledGroup = group;
			break;
		}
	}
	if(mControlledGroup == 0) {
		cerr << " Failed! [STOPPING COMMUNICATION]" << endl;
		return false;
	}
	else {
		cerr << " Success!" << endl;
	}

	mInputs = mControlledGroup->getInputValues();

	mRunServerLoop = true;

	while(mRunServerLoop) {
		mServerRunning = true;

		waitForNextDatagram();

		if(!mRunServerLoop) {
			break;
		}

		//check for new clients.
		if(mReadDatagram.maxUnreadBytes() > 0)
		{
			int command = mReadDatagram.readNextByte();

			switch(command) {
				case UDP_COMMAND_GET_MOTORS:
					sendMotorData();
					break;
				case UDP_COMMAND_SET_MOTORS:
					setMotorData();
					break;
				case UDP_INIT_COMMUNICATION:
					sendInitAck();
					break;
				case UDP_END_COMMUNICATION:
					sendEndCommunicationAck();
					break;
				default:
					cerr << "Received unsupported command!" << endl;
			};
		}
	}

	mServerRunning = false;
	mControlledGroup = 0;

	return true;
}


void SimpleUdpMotorInterface::stopUdpServer() {
	mRunServerLoop = false;
	while(mServerRunning) {
		mReadWaitCondition.wakeAll();
		QThread::msleep(10);
	}
	wait();
}

bool SimpleUdpMotorInterface::isUdpServerRunning() {
	return mServerRunning;
}

IntValue* SimpleUdpMotorInterface::getPortValue() const {
	return mPortValue;
}

void SimpleUdpMotorInterface::waitForNextDatagram() {

	mSocketMutex.lock();
		//if there is a datagram available, then do not wait for one to arrive.
		if(!mDatagrams.isEmpty()) {
			mReadDatagram.setData(mDatagrams.first());
			mDatagrams.removeFirst();
			mSocketMutex.unlock();
			return;
		}
	mSocketMutex.unlock();

	mReadMutex.lock();
	mReadWaitCondition.wait(&mReadMutex, 50);
	mReadMutex.unlock();

	mSocketMutex.lock();

	if(mDatagrams.isEmpty()) {
		mReadDatagram.clear();
	}
	else {
		mReadDatagram.setData(mDatagrams.first());
		mDatagrams.removeFirst();
	}

	mSocketMutex.unlock();
}


void SimpleUdpMotorInterface::datagramReceived() {
	if(mUdpServerSocket == 0) {
		return;
	}

	mSocketMutex.lock();
	while(mUdpServerSocket->hasPendingDatagrams()) {
		QByteArray data(mUdpServerSocket->pendingDatagramSize(), 0);
		mUdpServerSocket->readDatagram(data.data(), data.size(),
											mLastReceivedHostAddress, &mLastReceivedPort);
		mDatagrams.append(data);
	}
	mSocketMutex.unlock();

	mReadWaitCondition.wakeAll();
}

void SimpleUdpMotorInterface::sendDatagram() {
	//finalize current Datagram.

	if(mUdpServerSocket == 0 || !mUdpServerSocket->isValid()) {
		return;
	}

	mUdpServerSocket->writeDatagram(mWriteDatagram.getData(), mWriteDatagram.getDataSize(),
				*mLastReceivedHostAddress, mLastReceivedPort);
}


bool SimpleUdpMotorInterface::sendMotorData() {

	if(mControlledGroup == 0) {
		return false;
	}

	double normalizedMin = -1.0;
	double normalizedMax = 1.0;
	double min = 0.0;
	double max = 1023.0;

	mMotorMutex.lock();
	mWriteDatagram.clear();
	mWriteDatagram.writeByte(UDP_COMMAND_MOTOR_SETTINGS);

	for(int i = 0; i < mInputs.size(); ++i) {
		double val = mInputs.at(i)->get();

		val = (val > normalizedMax) ? normalizedMax : val;
		val = (val < normalizedMin) ? normalizedMin : val;

		val = min + (((max - min)
					/ (normalizedMax - normalizedMin)) * (val - normalizedMin));

		val = (val > max) ? max : val;
		val = (val < min) ? min : val;

		mWriteDatagram.writeInt((int) val);
	}
	mMotorMutex.unlock();

	sendDatagram();
	return true;
}


bool SimpleUdpMotorInterface::setMotorData() {
	if(mControlledGroup == 0) {
		return false;
	}

	if(mReadDatagram.maxUnreadBytes() < (mInputs.size() * 4)) {
		cerr << "Did not receive enough bytes (" << mReadDatagram.maxUnreadBytes()
			<< ") instead of (" << (mInputs.size() * 4) << ")" << endl;
	}

	double normalizedMin = -1.0;
	double normalizedMax = 1.0;
	double min = 0.0;
	double max = 1023.0;

	mMotorMutex.lock();

	int motorSettings = mInputs.size() / 2;

	for(int i = 0; i < mInputs.size() && mReadDatagram.maxUnreadBytes() >= 4; ++i) {
		double val = (double) mReadDatagram.readNextInt();

		//TODO undo these changes. This stabilizes the head when controlled by the key frame manager.
		//Remove when torque behavior is fixed.
// 		if(i == mInputs.size() - 4) {
// 			val = mHeadPanDesiredAngle->get();
// 		}
// 		else if(i == mInputs.size() - 3) {
// 			val = 1023;
// 		}
// 		else if(i == mInputs.size() - 2) {
// 			val = mHeadTiltDesiredAngle->get();
// 		}
// 		else if(i == mInputs.size() - 1) {
// 			val = 1023;
// 		}

		if(i == mInputs.size() - motorSettings - 2) {
			val = mHeadPanDesiredAngle->get();
		}
		else if(i == mInputs.size() - motorSettings - 1) {
			val = mHeadTiltDesiredAngle->get();
		}
		else if(i == mInputs.size() - 2) {
			val = 1023;
		}
		else if(i == mInputs.size() - 1) {
			val = 1023;
		}

		val = (val > max) ? max : val;
		val = (val < min) ? min : val;

		if(max != min) {
			val = (normalizedMin + (((normalizedMax - normalizedMin)
						/ (max - min)) * (val - min)));
		}
		else {
			Core::log("SimpeUpdMotorInterface: Warning: Prevented division by zero!");
		}

		val = (val > normalizedMax) ? normalizedMax : val;
		val = (val < normalizedMin) ? normalizedMin : val;

		if(i < motorSettings) {
			mInputs.at(i * 2)->set(val * 150.0);
		}
		else {
			mInputs.at(((i - motorSettings) * 2) + 1)->set((((double) val) + 1.0) / 2.0);
		}
	}
	mMotorMutex.unlock();

	if(mReceivedMotorSetMessageEvent != 0) {
		mReceivedMotorSetMessageEvent->trigger();
	}

	return true;
}


bool SimpleUdpMotorInterface::sendInitAck() {

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(UDP_INIT_COMMUNICATION_ACK);
	mWriteDatagram.writeInt(15);
	mWriteDatagram.writeInt(1);

	sendDatagram();

	if(mConnectEvent != 0) {
        mConnectEvent->trigger();
	}

	return true;
}

bool SimpleUdpMotorInterface::sendEndCommunicationAck() {
	mWriteDatagram.clear();
	mWriteDatagram.writeByte(UDP_END_COMMUNICATION_ACK);

	sendDatagram();

	if(mDisconnectEvent != 0) {
        mDisconnectEvent->trigger();
	}

	return true;
}




}



