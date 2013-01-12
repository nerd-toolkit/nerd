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



#include "SeedUDPCommunication.h"
#include <QMutexLocker>
#include <QUdpSocket>
#include "Value/ValueManager.h"
#include "Core/Core.h"
#include <iostream>
#include <QTimer>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include "NerdConstants.h"

using namespace std;


namespace nerd {

SeedUDPCommunication::SeedUDPCommunication() : mUdpServerSocket(0),
				mResetRequestCounter(0), mNextStepRequestCounter(0),
				mResetCommunicationRequest(false),
				mGroupControllerMutex(QMutex::Recursive), mRunServerLoop(true), mServerRunning(false),
				mSeed(0), mOwnerThread(0)
{
	Core::getInstance()->addSystemObject(this);

	mPortValue = new IntValue(45454);

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/UDPCommunication/ServerPort", mPortValue);

	mLastReceivedHostAddress = new QHostAddress();
	mLastReceivedPort = 0;

	EventManager *em = Core::getInstance()->getEventManager();
	mNextStepEvent = em->createEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	mStepCompletedEvent = em->createEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);
}

SeedUDPCommunication::~SeedUDPCommunication() {
	for(int i = 0; i < mClientHandlers.size(); ++i) {
		delete mClientHandlers.at(i);
	}
	mClientHandlers.clear();
}


bool SeedUDPCommunication::init() {
	bool ok = true;

	Core::getInstance()->registerThread(this);

	return ok;
}


bool SeedUDPCommunication::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mResetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET);
	mResetFinalizedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED);

	if(mNextStepEvent == 0) {
		ok = false;
		Core::log("SeedUDPCommunication: Could not find Event [mNextStepEvent]");
	}
	if(mStepCompletedEvent == 0) {
		ok = false;
		Core::log("SeedUDPCommunication: Could not find Event [mStepCompletedEvent]");
	}
	if(mResetEvent == 0) {
		ok = false;
		Core::log("SeedUDPCommunication: Could not find Event [mResetEvent]");
	}
	if(mResetFinalizedEvent == 0) {
		ok = false;
		Core::log("SeedUDPCommunication: Could not find Event [mResetFinalizedEvent]");
	}
	return ok;
}


bool SeedUDPCommunication::cleanUp() {
	//shut down the upd communication and the upd server.
	stopUdpServer();
	return true;
}


QString SeedUDPCommunication::getName() const {
	return "SeedUDPCommunication";
}


void SeedUDPCommunication::run() {

	Core::getInstance()->registerThread(this);

	startUdpServer();

	if(mOwnerThread != 0) {
        while(mOwnerThread->isRunning()) {
            mOwnerThread->quit();
        }
	}
	QCoreApplication::instance()->quit();
}


void SeedUDPCommunication::startListening(QThread *owner) {
    mOwnerThread = owner;

	Core::getInstance()->registerThread(this);

	cerr << "Nerd NSP 1.1 server listening at port " << mPortValue->get() << endl;
	Core::log(QString("SeedUDPCommunication: Starting listening at port ")
			.append(QString::number(mPortValue->get())));

	if(mUdpServerSocket == 0) {
		mUdpServerSocket = new QUdpSocket();
		connect(mUdpServerSocket, SIGNAL(readyRead()),
                this, SLOT(datagramReceived()), Qt::DirectConnection);
		mUdpServerSocket->bind(mPortValue->get());
	}
}

bool SeedUDPCommunication::startUdpServer() {

	if(mUdpServerSocket == 0) {
		return false;
	}

	//set the running thread as main execution thread.
	Core::getInstance()->setMainExecutionThread();
	Core::getInstance()->registerThread(this);

	mRunServerLoop = true;

	while(mRunServerLoop) {
		mServerRunning = true;

		waitForNextDatagram();

		Core::getInstance()->executePendingTasks();

		if(!mRunServerLoop) {
			break;
		}

		if(!mHandlersToTerminate.empty()) {

			mGroupControllerMutex.lock();
			QList<SeedUDPClientHandler*> handlers = mHandlersToTerminate;
			mHandlersToTerminate.clear();
			mGroupControllerMutex.unlock();

			for(int i = 0; i < handlers.size(); ++i) {
				SeedUDPClientHandler *handler = handlers.at(i);
				mClientHandlers.removeAll(handler);

				QList<SimObjectGroup*> groups = mControlledObjectGroups.keys();
				for(int j = 0; j < groups.size(); ++j) {
					SimObjectGroup *group = groups.at(j);
					if(mControlledObjectGroups.value(group) == handler) {
						deregisterGroupController(group, handler);
						Core::log("SeedUDPCommunication: Deregistereing group handler");
					}
				}
				handler->stopCommunication();
				handler->terminate();
				handler->wait();
				Core::getInstance()->deregisterThread(handler);
				delete handler;
			}
		}

		//check for new clients.
		if(mReadDatagram.maxUnreadBytes() >= 3 &&
					mReadDatagram.readNextByte() == (char)
					SeedUDPCommunication::UDP_INIT_COMMUNICATION)
		{
			int majorProtocolId = mReadDatagram.readNextByte();
			int minorProtocolId = mReadDatagram.readNextByte();

			if(majorProtocolId == 1 && minorProtocolId == 1) {

				SeedUDPClientHandler *clientHandler = new SeedUDPClientHandler(this,
						*mLastReceivedHostAddress, mLastReceivedPort);

				Core::getInstance()->registerThread(clientHandler);

				clientHandler->start();
				while(!clientHandler->isRunning()) {
					QThread::msleep(100);
				}
				mClientHandlers.append(clientHandler);
			}
			else {
				Core::log(QString("The requested UDP protocol ")
						.append(QString::number(majorProtocolId)).append(".")
						.append(QString::number(minorProtocolId))
						.append(" is not supported."));
			}
		}

		//check for resetCommunication commands.
		if(mResetCommunicationRequest) {
			resetCommunication();
			mNextStepRequestCounter = 0;
			mResetRequestCounter = 0;
			continue;
		}

		//check for next step conditions.
		if(mNextStepRequestCounter <= 0 && !mControlledObjectGroups.keys().isEmpty()) {
			executeNextStep();
		}

		//check for reset conditions
		if(mResetRequestCounter <= 0 && !mControlledObjectGroups.keys().isEmpty()) {
			resetSimulation();
		}

		Core::getInstance()->executePendingTasks();

	}

	mServerRunning = false;

	Core::getInstance()->clearMainExecutionThread();

	return true;
}


void SeedUDPCommunication::stopUdpServer() {
	mRunServerLoop = false;
	for(int i = 0; i < mClientHandlers.size(); ++i) {
		mClientHandlers.at(i)->stopCommunication();
	}
}


void SeedUDPCommunication::setPort(int port) {
	mPortValue->set(port);
}

bool SeedUDPCommunication::isUdpServerRunning() {
	return mServerRunning;
}

void SeedUDPCommunication::delayThread() {
	msleep(10);
}



void SeedUDPCommunication::datagramReceived() {

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


void SeedUDPCommunication::waitForNextDatagram() {

	//TODO use mutex to lock against read events while checking.
	mSocketMutex.lock();

	mSocketMutex.unlock();

	mReadMutex.lock();
	mReadWaitCondition.wait(&mReadMutex, 5);
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


void SeedUDPCommunication::demandCommunicationReset() {
	mResetCommunicationRequest = true;
	mReadWaitCondition.wakeAll();
}

void SeedUDPCommunication::demandSimulationReset(SeedUDPClientHandler*, int seed) {

	mSeed = seed;
	mResetRequestCounter--;

	if(mResetRequestCounter < 0){
		Core::log("SeedUDPCommunication warning: There were more reset requests "
				"than controlled SimGroups!");
	}
	mReadWaitCondition.wakeAll();
}


void SeedUDPCommunication::demandNextSimulationStep(SeedUDPClientHandler*) {
	mNextStepRequestCounter--;

	if(mNextStepRequestCounter < 0){
		Core::log("SeedUDPCommunication warning: There were more next step requests "
				"than controlled SimGroups!");
	}
	mReadWaitCondition.wakeAll();
}


void SeedUDPCommunication::terminateCliendHandler(SeedUDPClientHandler *handler) {
	mGroupControllerMutex.lock();
	if(!mHandlersToTerminate.contains(handler)) {
		mHandlersToTerminate.append(handler);
	}
	mGroupControllerMutex.unlock();
}



bool SeedUDPCommunication::registerGroupController(SimObjectGroup *groupToControl,
						SeedUDPClientHandler *controller)
{
	if(groupToControl == 0 || controller == 0) {
		return false;
	}

	QMutexLocker guard(&mGroupControllerMutex);

	if(mControlledObjectGroups.keys().contains(groupToControl)) {
		if(mControlledObjectGroups.value(groupToControl) == controller) {
			return true; //this controller already controls the group.
		}
		else {
			return false;
		}
	}
	mControlledObjectGroups.insert(groupToControl, controller);
	mResetRequestCounter++;
	mNextStepRequestCounter++;

	return true;
}

bool SeedUDPCommunication::deregisterGroupController(SimObjectGroup *groupToControl,
						SeedUDPClientHandler *controller)
{
	if(groupToControl == 0 || controller == 0) {
		return false;
	}

	QMutexLocker guard(&mGroupControllerMutex);

	if(mControlledObjectGroups.value(groupToControl) != controller) {
		return false;
	}
	mControlledObjectGroups.remove(groupToControl);
	mResetRequestCounter--;
	mNextStepRequestCounter--;

	return true;
}


bool SeedUDPCommunication::isGroupControlled(SimObjectGroup *group) {
	if(mControlledObjectGroups.keys().contains(group)) {
		return true;
	}
	return false;
}


void SeedUDPCommunication::resetCommunication() {

	Core::log("SeedUDPCommunication: Communication is RESET.");

	QList<SeedUDPClientHandler*> handlers(mClientHandlers);
	mClientHandlers.clear();

	for(int i = 0; i < handlers.size(); ++i) {
		handlers.at(i)->sendCommunicationResetMessage();
	}
	for(int i = 0; i < handlers.size(); ++i) {
		SeedUDPClientHandler *handler = handlers.at(i);
		handler->stopCommunication();
		while(handler->isCommunicationRunning()) {
			msleep(10);
		}
		while(handler->isRunning()) {
			handler->quit();
			msleep(10);
		}
	}
	for(int i = 0; i < handlers.size(); ++i) {
		delete handlers.at(i);
	}

	mControlledObjectGroups.clear();
	mDatagrams.clear();
	mReadDatagram.clear();

	mResetCommunicationRequest = false;
}


void SeedUDPCommunication::resetSimulation() {

	if(mResetEvent != 0 && mResetFinalizedEvent != 0) {
		mResetEvent->trigger();
		mResetFinalizedEvent->trigger();
	}
	else {
		Core::log("SeedUDPCommunication Warning: Did not find ResetSimulation or ResetSimulationFinalized Event!");
	}

	for(int i = 0; i < mClientHandlers.size(); ++i) {
		mClientHandlers.at(i)->sendSimulationResetMessage();
	}

	mResetRequestCounter = mControlledObjectGroups.keys().size();
}


void SeedUDPCommunication::executeNextStep() {

	if(mNextStepEvent != 0 && mStepCompletedEvent != 0) {
		mNextStepEvent->trigger();
		mStepCompletedEvent->trigger();
	}

	mNextStepRequestCounter = mControlledObjectGroups.size();

	for(int i = 0; i < mClientHandlers.size(); ++i) {
		mClientHandlers.at(i)->sendStepCompletedMessage();
	}


}



}




