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



#include "SeedUDPClientHandler.h"
#include "Communication/SeedUDPCommunication.h"
#include "Event/EventManager.h"
#include "Event/Event.h"
#include "Value/ValueManager.h"
#include "Value/Value.h"
#include "Core/Core.h"
#include <QVector>
#include <QList>
#include "Physics/PhysicsManager.h"
#include <QMutexLocker>
#include <iostream>
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include "Physics/Physics.h"


using namespace std;

namespace nerd {

SeedUDPClientHandler::SeedUDPClientHandler(SeedUDPCommunication *owner,
							const QHostAddress &clientAddress, quint16 clientPort)
				: mOwner(owner), mRunCommunicationLoop(true), mCommunicationLoopRunning(false),
					mCountNextStepCommands(0), mClientRunner(0),
					mClientAddress(clientAddress), mClientPort(clientPort),
					mUdpSocket(0), mEventIdCounter(1),
					mValueIdCounter(1), mObjectGroupCounter(1),
					mSendConnectionInfoMessage(false),
					mSendCommunicationResetInfoMessage(false),
					mSendResetCompletedMessage(false),
					mSendStepCompletedMessage(false)
{
}


SeedUDPClientHandler::~SeedUDPClientHandler() {
	Core::getInstance()->deregisterThread(this);

	QList<InterfaceGroup*> groups = mInterfaceGroups.values();
	for(int i = 0; i < groups.size(); ++i) {
		delete groups.at(i);
	}
	mInterfaceGroups.clear();
}



void SeedUDPClientHandler::run() {
	
	Core::getInstance()->registerThread(this);

	//connect socket to client adress and port.
	if(mUdpSocket != 0) {
		delete mUdpSocket;
		mUdpSocket = 0;
	}
	mUdpSocket = new QUdpSocket();
	mUdpSocket->bind();
	mUdpSocket->connectToHost(mClientAddress, mClientPort);

	connect(mUdpSocket, SIGNAL(readyRead()),
          this, SLOT(datagramReceived()), Qt::DirectConnection);

	if(mClientRunner != 0) {
		delete mClientRunner;
	}
	mClientRunner = new SeedUDPClientHandlerRunner(this);
	mClientRunner->start();

	QThread::exec();

	delete mUdpSocket;
	mUdpSocket = 0;
}

void SeedUDPClientHandler::runCommunication() {

	if(mUdpSocket == 0) {
		return;
	}

	sendConnectionInformation();

	mRunCommunicationLoop = true;


	while(mRunCommunicationLoop) {
		mCommunicationLoopRunning = true;

		//mDatagramMutex.lock();
		while(mRunCommunicationLoop && !waitForNextDatagram(true)) {
			if(mSendStepCompletedMessage) {
				executeSendStepCompletedMessage();
			}
			else if(mSendResetCompletedMessage) {
				executeSendSimulationResetMessage();
			}
			else if(mSendCommunicationResetInfoMessage) {
				executeSendCommunicationResetMessage();
			}
			else if(mSendConnectionInfoMessage) {
				executeSendConnectionInformation();
			}
		}

		if(!mRunCommunicationLoop) {
			break;
		}

		//mDatagramMutex.lock();

		unsigned char command = mReadDatagram.readNextByte();

		switch(command) {
			case SeedUDPCommunication::UDP_RESET_COMMUNICATION:
				handleResetCommunicationCommand();
				break;
			case SeedUDPCommunication::UDP_REGISTER_FOR_EVENT:
				handleEventRegistrationCommand();
				break;
			case SeedUDPCommunication::UDP_DEREGISTER_FROM_EVENT:
				handleEventDeregistrationCommand();
				break;
			case SeedUDPCommunication::UDP_GET_VALUE_IDS:
				handleValueListCommand();
				break;
			case SeedUDPCommunication::UDP_GET_VALUE:
				handleGetValueCommand();
				break;
			case SeedUDPCommunication::UDP_REGISTER_FOR_VALUE:
				handleValueRegistrationCommand();
				break;
			case SeedUDPCommunication::UDP_DEREGISTER_FROM_VALUE:
				handleValueDeregistrationCommand();
				break;
			case SeedUDPCommunication::UDP_SET_VALUE:
				handleSetValueCommand();
				break;
			case SeedUDPCommunication::UDP_GET_AGENT_OVERVIEW:
				handleAgentOverviewCommand();
				break;
			case SeedUDPCommunication::UDP_GET_AGENT_INFO:
				handleAgentInfoCommand();
				break;
			case SeedUDPCommunication::UDP_REGISTER_FOR_AGENT:
				handleAgentRegistrationCommand();
				break;
			case SeedUDPCommunication::UDP_DEREGISTER_FROM_AGENT:
				handleAgentDeregistrationCommand();
				break;
			case SeedUDPCommunication::UDP_RESET_SIMULATION:
				handleResetSimulationCommand();
				break;
			case SeedUDPCommunication::UDP_NEXT_SIMULATION_STEP:
				handleNextStepCommand();
				break;
			case SeedUDPCommunication::UDP_END_COMMUNICATION:
				handleEndCommunicationCommand();
				break;
			default:
				handleUnexpectedCommand(command);
		};

		//mDatagramMutex.unlock();
	}

	mCommunicationLoopRunning = false;
}


void SeedUDPClientHandler::stopCommunication() {

	mRunCommunicationLoop = false;
	while(mCommunicationLoopRunning) {
		mReadWaitCondition.wakeAll();
		msleep(5);
	}

	while(mClientRunner->isRunning()) {
		mClientRunner->quit();
		msleep(5);
	}

	while(isRunning()) {
        quit();
        msleep(5);
	}
}


bool SeedUDPClientHandler::isCommunicationRunning() const {
	return mCommunicationLoopRunning;
}


void SeedUDPClientHandler::datagramReceived() {

	mSocketMutex.lock();
	while(mUdpSocket->hasPendingDatagrams()) {
		QByteArray data(mUdpSocket->pendingDatagramSize(), 0);
		int readBytes = mUdpSocket->readDatagram(data.data(), data.size());

		//TODO readBytes were only used for debugging purposes. The following code is only to disable compiler warning:
		if(readBytes != 0) {
			readBytes = 0;
		}

		mDatagrams.append(data);
	}
	mSocketMutex.unlock();
	mReadWaitCondition.wakeAll();
}


bool SeedUDPClientHandler::waitForNextDatagram() {
	return waitForNextDatagram(false);
}

bool SeedUDPClientHandler::waitForNextDatagram(bool interruptible) {

	//TODO use mutex to lock against read events while checking.
	mSocketMutex.lock();
	if(!mDatagrams.isEmpty()) {
		mReadDatagram.setData(mDatagrams.first());
		mDatagrams.removeFirst();
		mSocketMutex.unlock();
		return true;
	}
	mSocketMutex.unlock();

	bool notified = false;
	do {
		mReadMutex.lock();
		notified = mReadWaitCondition.wait(&mReadMutex, 50);
		mReadMutex.unlock();

		if(interruptible) {
			if(mSendResetCompletedMessage || mSendStepCompletedMessage
				|| mSendCommunicationResetInfoMessage || mSendConnectionInfoMessage)
			{
				break;
			}
		}
		if(!mRunCommunicationLoop) {
			break;
		}
		if(!mDatagrams.isEmpty()) {
			break;
		}

	} while(!notified || (mDatagrams.isEmpty() && !interruptible));

	mSocketMutex.lock();

	//mDatagramMutex.lock();
	bool receivedData = true;

	if(mDatagrams.isEmpty()) {
		mReadDatagram.clear();
		receivedData = false;
	}
	else {
		mReadDatagram.setData(mDatagrams.first());
		mDatagrams.removeFirst();
	}
	//mDatagramMutex.unlock();
	mSocketMutex.unlock();
	return receivedData;
}


bool SeedUDPClientHandler::waitForConfirmation(unsigned char expectedHeader) {

	while(!waitForNextDatagram() && mRunCommunicationLoop) {
		;
	}
	if(!mRunCommunicationLoop) {
		return false;
	}
	unsigned char reply = mReadDatagram.readNextByte();
	if(reply != expectedHeader) {
		if(!handleUnexpectedCommand(reply)) { //intercept anytime-commands like reset communication.
			QString message = QString("Expected ").append(expectedHeader)
													.append(" but received a ").append(reply);
			reportCommunicationFailure(message);
		}

		return false;
	}
	return true;
}



void SeedUDPClientHandler::sendDatagram() {
	//finalize current Datagram.
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_DATAGRAM_END);

	if(mUdpSocket == 0 || !mUdpSocket->isValid()) {
		return;
	}

	mUdpSocket->write(mWriteDatagram.getData(), mWriteDatagram.getDataSize());
	mWriteDatagram.clear();
}


void SeedUDPClientHandler::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}

	//If one of the registered events was triggered, then memorize its occurence.
	if(mRegisteredEvents.keys().contains(event)) {
		if(!mOccuredEvents.contains(event)) {
			mOccuredEvents.append(event);
		}
	}
}


QString SeedUDPClientHandler::getName() const {
	if(mUdpSocket == 0) {
		return "SeedUDPClientHandler (no socket)";
	}
	return QString("SeedUDPClientHander (At port ").append(mUdpSocket->localPort())
					.append(" to IP ").append(mUdpSocket->peerName()).append(" at ")
					.append(mUdpSocket->peerPort()).append(")");
}


bool SeedUDPClientHandler::executeSendConnectionInformation() {

	mSendConnectionInfoMessage = false;

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_INIT_COMMUNICATION_ACK);
	mWriteDatagram.writeByte(1);
	mWriteDatagram.writeByte(1);
	sendDatagram();

	return true;
}


bool SeedUDPClientHandler::executeSendCommunicationResetMessage() {

	mSendCommunicationResetInfoMessage = false;

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_RESET_COMMUNICATION_ACK);
	sendDatagram();

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_RESET_SIMULATION_ACK);
	sendDatagram();

	return true;
}


bool SeedUDPClientHandler::executeSendSimulationResetMessage() {

	mSendResetCompletedMessage = false;

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_RESET_SIMULATION_COMPLETED);
	sendDatagram();

	if(!waitForConfirmation(SeedUDPCommunication::UDP_RESET_SIMULATION_COMPLETED_ACK)) {
		return false;
	}
	return true;
}


bool SeedUDPClientHandler::executeSendStepCompletedMessage() {

	mSendStepCompletedMessage = false;

	QList<int> ids = mInterfaceGroups.keys();

	for(int i = 0; i < ids.size(); ++i) {
		int id = ids.at(i);

		InterfaceGroup *group = mInterfaceGroups.value(id);

		mWriteDatagram.clear();
		mWriteDatagram.writeByte(SeedUDPCommunication::UDP_NEXT_SIMULATION_STEP_COMPLETED);
		mWriteDatagram.writeInt(id);
		mWriteDatagram.writeInt(group->mOutputValues.size());
		mWriteDatagram.writeInt(group->mInfoValues.size());
		mWriteDatagram.writeInt(mOccuredEvents.size());
		mWriteDatagram.writeInt(0); //TODO registered values

		for(int j = 0; j < group->mOutputValues.size(); ++j) {
			mWriteDatagram.writeFloat(group->mOutputValues.at(j)->get());
		}
		for(int j = 0; j < group->mInfoValues.size(); ++j) {
			mWriteDatagram.writeFloat(group->mInfoValues.at(j)->get());
		}
		for(int j = 0; j < mOccuredEvents.size(); ++j) {
			mWriteDatagram.writeInt(mRegisteredEvents.value(mOccuredEvents.at(j)));
		}
		//TODO add registered Values

		mOccuredEvents.clear();

		sendDatagram();

		if(!waitForConfirmation(SeedUDPCommunication::UDP_NEXT_SIMULATION_STEP_COMPLETED_ACK)) {
			return false;
		}
	}

	mCountNextStepCommands = 0;
	return true;
}



bool SeedUDPClientHandler::sendConnectionInformation() {

	mSendConnectionInfoMessage = true;
	mReadWaitCondition.wakeAll();
	return true;
}

bool SeedUDPClientHandler::sendCommunicationResetMessage() {

	mSendCommunicationResetInfoMessage = true;
	mReadWaitCondition.wakeAll();
	return true;
}

bool SeedUDPClientHandler::sendSimulationResetMessage() {

	mSendResetCompletedMessage = true;
	mReadWaitCondition.wakeAll();
	return true;
}

bool SeedUDPClientHandler::sendStepCompletedMessage() {

	mSendStepCompletedMessage = true;
	mReadWaitCondition.wakeAll();
	return true;
}


bool SeedUDPClientHandler::handleEventRegistrationCommand() {

	int id = 0;

	QString eventName = mReadDatagram.readNextString();

	if(eventName.compare("") != 0) {
		EventManager *em = Core::getInstance()->getEventManager();
		Event *event = em->registerForEvent(eventName, this);

		if(event != 0) {
			if(mRegisteredEvents.keys().contains(event)) {
				id = mRegisteredEvents.value(event);
			}
			else {
				id = mEventIdCounter++;
				mRegisteredEvents.insert(event, id);
			}
		}
	}

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_REGISTER_FOR_EVENT_ACK);
	mWriteDatagram.writeInt(id);
	sendDatagram();

	return true;
}

//TODO use mutexes.
bool SeedUDPClientHandler::handleEventDeregistrationCommand() {


	int id = mReadDatagram.readNextInt();

	if(mRegisteredEvents.values().contains(id)) {
		QList<Event*> events = mRegisteredEvents.keys();
		for(int i = 0; i < events.size(); ++i) {
			Event *event = events.at(i);
			if(mRegisteredEvents.value(event) == id) {
				mRegisteredEvents.remove(event);
				break;
			}
		}
	}

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_DEREGISTER_FROM_EVENT_ACK);
	mWriteDatagram.writeInt(id);
	sendDatagram();

	return true;
}


bool SeedUDPClientHandler::handleValueListCommand() {


	ValueManager *vm = Core::getInstance()->getValueManager();

	QString regExp = mReadDatagram.readNextString();

	QList<QString> valueNames = vm->getValueNamesMatchingPattern(regExp);

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_VALUE_IDS);
	mWriteDatagram.writeInt(valueNames.size());
	sendDatagram();

	for(int i = 0; i < valueNames.size(); ++i) {

		Value *value = vm->getValue(valueNames.at(i));
		int id = -1;
		if(value != 0) {
			if(mKnownValues.keys().contains(value)) {
				id = mKnownValues.value(value);
			}
			else {
				id = mValueIdCounter++;
				mKnownValues.insert(value, id);
			}
		}
		mWriteDatagram.clear();
		mWriteDatagram.writeByte(SeedUDPCommunication::UDP_VALUE_INFO);
		mWriteDatagram.writeInt(i);
		mWriteDatagram.writeInt(id);
		if(value == 0) {
			mWriteDatagram.writeString("");
		}
		else {
			mWriteDatagram.writeString(value->getName());
		}
		mWriteDatagram.writeString(valueNames.at(i));
		sendDatagram();
	}
	return true;
}


bool SeedUDPClientHandler::handleValueRegistrationCommand() {

	int numberOfIds = mReadDatagram.readNextInt();

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_REGISTER_FOR_VALUE_ACK);
	mWriteDatagram.writeInt(numberOfIds);

	for(int i = 0; i < numberOfIds; ++i) {
		int id = mReadDatagram.readNextInt();
		bool foundId = false;

		QList<Value*> knownValues = mKnownValues.keys();
		for(int j = 0; j < knownValues.size(); ++j) {
			Value *value = knownValues.at(i);
			if(mKnownValues.value(value) == id) {
				foundId = true;
				if(!mRegisteredValues.contains(value)) {
					mRegisteredValues.append(value);
				}
			}
		}
		if(foundId) {
			mWriteDatagram.writeInt(id);
		}
		else {
			mWriteDatagram.writeInt(0); //id was not known, so no registration took place
		}
	}
	sendDatagram();

	return true;
}


bool SeedUDPClientHandler::handleValueDeregistrationCommand() {

	int numberOfIds = mReadDatagram.readNextInt();

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_DEREGISTER_FROM_VALUE_ACK);
	mWriteDatagram.writeInt(numberOfIds);

	for(int i = 0; i < numberOfIds; ++i) {
		int id = mReadDatagram.readNextInt();
		bool foundId = false;

		QList<Value*> knownValues = mKnownValues.keys();
		for(int j = 0; j < knownValues.size(); ++j) {
			Value *value = knownValues.at(i);
			if(mKnownValues.value(value) == id) {
				foundId = true;
				if(mRegisteredValues.contains(value)) {
					mRegisteredValues.removeAll(value);
				}
			}
		}
		if(foundId) {
			mWriteDatagram.writeInt(id);
		}
		else {
			mWriteDatagram.writeInt(0); //id was not known, so no deregistration took place
		}
	}
	sendDatagram();

	return true;
}


bool SeedUDPClientHandler::handleSetValueCommand() {

	int id = mReadDatagram.readNextInt();
	QString valueToSet = mReadDatagram.readNextString();

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_SET_VALUE_ACK);
	mWriteDatagram.writeInt(id);

	QList<Value*> knownValues = mKnownValues.keys();
	for(int i = 0; i < knownValues.size(); ++i) {
		Value *value = knownValues.at(i);
		if(mKnownValues.value(value) == id) {
			bool ok = value->setValueFromString(valueToSet);
			if(ok) {
				mWriteDatagram.writeByte(1);
			}
			else {
				mWriteDatagram.writeByte(0);
			}
			sendDatagram();
			return true;
		}
	}
	mWriteDatagram.writeByte(2); //notify: failed to find value
	sendDatagram();

	return true;
}

bool SeedUDPClientHandler::handleGetValueCommand() {

	int id = mReadDatagram.readNextInt();

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_VALUE);

	QList<Value*> knownValues = mKnownValues.keys();
	for(int i = 0; i < knownValues.size(); ++i) {
		Value *value = knownValues.at(i);
		if(mKnownValues.value(value) == id) {
			mWriteDatagram.writeInt(id);
			mWriteDatagram.writeString(value->getValueAsString());
			sendDatagram();
			return true;
		}
	}
	mWriteDatagram.writeInt(-1); //notify: failed to find value
	mWriteDatagram.writeString("");
	sendDatagram();

	return true;
}



bool SeedUDPClientHandler::handleAgentOverviewCommand() {

	if(mOwner == 0) {
		return false;
	}

	QList<SimObjectGroup*> agentGroups = Physics::getPhysicsManager()->getSimObjectGroups();

	{
		for(int i = 0; i < agentGroups.size(); ++i) {
			SimObjectGroup *group = agentGroups.at(i);
			if(!mKnownObjectGroups.values().contains(group)) {
				mKnownObjectGroups.insert(mObjectGroupCounter++, group);
			}
		}
	}
	QList<int> knownGroupIds = mKnownObjectGroups.keys();
	{
		for(int i = 0; i < knownGroupIds.size(); ++i) {
			int id = knownGroupIds.at(i);
			SimObjectGroup *group = mKnownObjectGroups.value(id);
			if(!agentGroups.contains(group)) {
				//a known group does not exist any more.
				mKnownObjectGroups.remove(id);
			}
		}
	}

	knownGroupIds = mKnownObjectGroups.keys();

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_AGENT_OVERVIEW);
	mWriteDatagram.writeInt(knownGroupIds.size());
	sendDatagram();

	for(int i = 0; i < agentGroups.size(); ++i) {

		//mDatagramMutex.unlock();

		int id = knownGroupIds.at(i);
		SimObjectGroup *group = mKnownObjectGroups.value(id);

		//mDatagramMutex.lock();

		mWriteDatagram.clear();
		mWriteDatagram.writeByte(SeedUDPCommunication::UDP_AGENT_OVERVIEW_NEXT);
		mWriteDatagram.writeInt(i); //index counter
		mWriteDatagram.writeInt(id); //group id
		mWriteDatagram.writeString(group->getType()); //type
		int stillAvailable = (mOwner->isGroupControlled(group)) ? 0 : 1;
		mWriteDatagram.writeByte(stillAvailable); //availability
		mWriteDatagram.writeString(group->getName());
		sendDatagram();
	}
	if(!waitForConfirmation(SeedUDPCommunication::UDP_AGENT_OVERVIEW_ACK)) {
		return false;
	}
	return true;
}


bool SeedUDPClientHandler::handleAgentInfoCommand() {

	int id = mReadDatagram.readNextInt();

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_AGENT_INFO);

	SimObjectGroup *group = mKnownObjectGroups.value(id);
	if(group == 0) {
		mWriteDatagram.writeInt(0); //failure, could not find group.
		mWriteDatagram.writeInt(0);
		mWriteDatagram.writeInt(0);
		mWriteDatagram.writeInt(0);
		mWriteDatagram.writeString("");
		sendDatagram();

		return true;
	}
	else {
		QList<InterfaceValue*> inputValues = group->getInputValues();
		QList<InterfaceValue*> outputValues = group->getOutputValues();
		QList<InterfaceValue*> infoValues = group->getInfoValues();

		mWriteDatagram.writeInt(id);
		mWriteDatagram.writeInt(inputValues.size());
		mWriteDatagram.writeInt(outputValues.size());
		mWriteDatagram.writeInt(infoValues.size());
		mWriteDatagram.writeString(group->getName());
		sendDatagram();

		int numberOfSentValues = 0;
		{
			for(int i = 0; i < inputValues.size(); ++i) {

				//mDatagramMutex.unlock();

				InterfaceValue *value = inputValues.at(i);

				//mDatagramMutex.lock();

				mWriteDatagram.clear();
				mWriteDatagram.writeByte(SeedUDPCommunication::UDP_AGENT_INFO_NEXT);
				mWriteDatagram.writeInt(numberOfSentValues);
				mWriteDatagram.writeFloat(value->getNormalizedMin());
				mWriteDatagram.writeFloat(value->getNormalizedMax());
				mWriteDatagram.writeString(value->getName());
				sendDatagram();
				numberOfSentValues++;
			}
		}
		{
			for(int i = 0; i < outputValues.size(); ++i) {

				//mDatagramMutex.unlock();

				InterfaceValue *value = outputValues.at(i);

				//mDatagramMutex.lock();

				mWriteDatagram.clear();
				mWriteDatagram.writeByte(SeedUDPCommunication::UDP_AGENT_INFO_NEXT);
				mWriteDatagram.writeInt(numberOfSentValues);
				mWriteDatagram.writeFloat(value->getNormalizedMin());
				mWriteDatagram.writeFloat(value->getNormalizedMax());
				mWriteDatagram.writeString(value->getName());
				sendDatagram();
				numberOfSentValues++;
			}
		}
		{
			for(int i = 0; i < infoValues.size(); ++i) {

				//mDatagramMutex.unlock();

				InterfaceValue *value = infoValues.at(i);

				//mDatagramMutex.lock();

				mWriteDatagram.clear();
				mWriteDatagram.writeByte(SeedUDPCommunication::UDP_AGENT_INFO_NEXT);
				mWriteDatagram.writeInt(numberOfSentValues);
				mWriteDatagram.writeFloat(value->getNormalizedMin());
				mWriteDatagram.writeFloat(value->getNormalizedMax());
				mWriteDatagram.writeString(value->getName());
				sendDatagram();
				numberOfSentValues++;
			}
		}
	}
	if(!waitForConfirmation(SeedUDPCommunication::UDP_AGENT_INFO_ACK)) {
		return false;
	}

	return true;
}


bool SeedUDPClientHandler::handleAgentRegistrationCommand() {

	if(mOwner == 0) {
		return false;
	}

	int id = mReadDatagram.readNextInt();

	SimObjectGroup *group = mKnownObjectGroups.value(id);

	bool ok = true;
	if(group == 0) {
		ok = false;
	}
	else {
		ok = mOwner->registerGroupController(group, this);
	}
	//add InterfaceGroup to controlled groups
	if(ok) {
		InterfaceGroup *interfaceGroup = 0;
		if(mInterfaceGroups.keys().contains(id)) {
			interfaceGroup = mInterfaceGroups.value(id);
		}
		else {
			interfaceGroup = new InterfaceGroup(id);
			mInterfaceGroups.insert(id, interfaceGroup);
		}
		interfaceGroup->mId = id;
		interfaceGroup->mAssociatedObjectGroup = group;
		interfaceGroup->mInputValues = group->getInputValues().toVector();
		interfaceGroup->mOutputValues = group->getOutputValues().toVector();
		interfaceGroup->mInfoValues = group->getInfoValues().toVector();
	}

	int registrationOk = ok ? 1 : 0;

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_REGISTER_FOR_AGENT_ACK);
	mWriteDatagram.writeInt(id);
	mWriteDatagram.writeByte(registrationOk);
	sendDatagram();

	return true;
}

bool SeedUDPClientHandler::handleAgentDeregistrationCommand() {

	if(mOwner == 0) {
		return false;
	}


	int id = mReadDatagram.readNextInt();

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_DEREGISTER_FROM_AGENT_ACK);
	mWriteDatagram.writeInt(id);

	if(mInterfaceGroups.keys().contains(id)) {
		InterfaceGroup *iGroup = mInterfaceGroups.take(id);
		if(iGroup == 0 || iGroup->mAssociatedObjectGroup == 0) {
			Core::log("SeedUPDClientHandler: Invalid state of interface group.");
			return false;
		}
		bool ok = mOwner->deregisterGroupController(iGroup->mAssociatedObjectGroup, this);

		mWriteDatagram.writeByte(ok ? 1 : 0);

		delete iGroup;
	}
	else {

		mWriteDatagram.writeByte(2); //signals: was not registered before.
	}
	sendDatagram();

	return true;
}



bool SeedUDPClientHandler::handleResetSimulationCommand() {
	if(mOwner == 0) {
		return false;
	}

	int seed = mReadDatagram.readNextInt();

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_RESET_SIMULATION_ACK);
	sendDatagram();

	mOwner->demandSimulationReset(this, seed);

	//do not confirm this message
	//when all clients have sent a reset command, then all get notified in method
	//sendSimulationResetMethod().

	return true;
}


bool SeedUDPClientHandler::handleResetCommunicationCommand() {
	if(mOwner == 0) {
		return false;
	}
	mOwner->demandCommunicationReset();

	return true;
}


bool SeedUDPClientHandler::handleNextStepCommand() {

	if(mOwner == 0) {
		return false;
	}

	int groupId = mReadDatagram.readNextInt();

	InterfaceGroup *group = mInterfaceGroups.value(groupId);

	if(group == 0) {
		Core::log(QString("SeedUDPClientHandler Failure: Received next step "
				"command with wrong group id ").append(QString::number(groupId)));
		//TODO DON'T stop here!
		return false;
	}


	int numberOfInputs = mReadDatagram.readNextInt();

	if(numberOfInputs != group->mInputValues.size()) {
		Core::log(QString("SeedUDPClientHandler Warning: Received number of inputs ")
				.append(QString::number(numberOfInputs))
				.append(" unequal to number of controlled inputs ")
				.append(QString::number(group->mInputValues.size())));
	}

	for(int i = 0; i < numberOfInputs && i < group->mInputValues.size(); ++i) {
		group->mInputValues.at(i)->setNormalized(mReadDatagram.readNextFloat());
	}

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_NEXT_SIMULATION_STEP_ACK);
	mWriteDatagram.writeInt(groupId);
	sendDatagram();

	mOwner->demandNextSimulationStep(this);

	return true;
}

bool SeedUDPClientHandler::handleEndCommunicationCommand() {

	mWriteDatagram.clear();
	mWriteDatagram.writeByte(SeedUDPCommunication::UDP_END_COMMUNICATION_ACK);
	sendDatagram();

	//end communication loop.
	mRunCommunicationLoop = false;

	mOwner->terminateCliendHandler(this);

	return true;
}


bool SeedUDPClientHandler::handleUnexpectedCommand(char command) {
	//TODO implement unexpected resetCommunication command.
	switch(command) {
		case SeedUDPCommunication::UDP_RESET_COMMUNICATION:
			handleResetCommunicationCommand();
			break;
		default:
			;
	};

	return true;
}



void SeedUDPClientHandler::reportCommunicationFailure(const QString &message) {
	Core::log(QString("SeedUDPClientHandler: Failure: ").append(message));
}


}





