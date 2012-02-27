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


#include "SimbaUDPCommunication.h"
#include <QHostAddress>
#include <QDataStream>
#include <QIODevice>
#include <iostream>
#include <QMutexLocker>
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/SimObject.h"
#include "Event/EventManager.h"
#include "Physics/Physics.h"
#include <QCoreApplication>
#include "NerdConstants.h"

using namespace std;


namespace nerd {

int COM_END              =  -1;
int COM_ID_HANDSK_REQ    =  10;
int COM_ID_HANDSK_ACK    =  11;
int COM_ID_DATA_REQ      =  40;
int COM_ID_DATA_ACK      =  41;
int COM_ID_RESET_REQ     =  50;
int COM_ID_RESET_ACK     =  51;
int COM_ID_INIT_REQ      =  60;
int COM_ID_INIT_ACK      =  61;
int COM_ID_NEXT_TRY_REQ  =  70;
int COM_ID_NEXT_TRY_ACK  =  71;
	
	 /** handshake packet tags */
int COM_HS_TAG_ROBOT     =   0;
int COM_HS_TAG_SEGCHAIN  = 100;
int COM_HS_TAG_SEGMENT   = 200;
int COM_HS_TAG_MOTOR     = 300;
int COM_HS_TAG_SENSOR    = 400;


SimbaUDPCommunication::SimbaUDPCommunication() : SystemObject(), QThread(),
		ParameterizedObject("SimbaUDPCommunication"),
		mResetEvent(0),mResetFinalizedEvent(0),	mNextStepEvent(0),
		mStepCompletedEvent(0), mUdpSocket(0),
		mDataPosition(0), mSocketSemaphore(0), 
		mRunCommunicationLoop(false), mCommunicationRunning(false),
		mReadNextDatagram(true), mOwnerThread(0)
{
	setPrefix("/SimbaUDPCommunication/");
	
	Core::getInstance()->addSystemObject(this);

	mHostAddressValue = new StringValue("127.0.0.1");
	mPortValue = new IntValue(45454);
	mSimObjectGroupName = new StringValue("Default");
	
	addParameter("HostAddress", mHostAddressValue, true);
	addParameter("Port", mPortValue, true);
	addParameter("ControlledAgent", mSimObjectGroupName, true);
	
	mLastReceivedHostAddress = new QHostAddress();
	mLastReceivedPort = 0;

	mData.reserve(MAX_DATA_SIZE);
	
	mCurrentStep = new IntValue(0);
	Core::getInstance()->getValueManager()->addValue(SimulationConstants::VALUE_EXECUTION_CURRENT_STEP, mCurrentStep);
}

SimbaUDPCommunication::~SimbaUDPCommunication() {
	delete mUdpSocket;
	delete mLastReceivedHostAddress;
}


bool SimbaUDPCommunication::init() {
	bool ok = true;

	Core::getInstance()->registerThread(this);

	return ok;
}


bool SimbaUDPCommunication::bind() {
	bool ok = true;
	
	EventManager *em = Core::getInstance()->getEventManager();
	
	mNextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	mStepCompletedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);
	mResetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET);
	mResetFinalizedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED);
	
	if(mNextStepEvent == 0) {
		ok = false;
		qDebug("SimbaUDPCommunication: Could not find Event [mNextStepEvent]");
	}
	if(mStepCompletedEvent == 0) {
		ok = false;
		qDebug("SimbaUDPCommunication: Could not find Event [mStepCompletedEvent]");
	}
	if(mResetEvent == 0) {
		ok = false;
		qDebug("SimbaUDPCommunication: Could not find Event [mResetEvent]");
	}
	if(mResetFinalizedEvent == 0) {
		ok = false;
		qDebug("SimbaUDPCommunication: Could not find Event [mResetFinalzedEvent]");
	}
	
	
	return ok;
}


bool SimbaUDPCommunication::cleanUp() {
	//shut down the upd communication and the upd server.
	stopCommunication();
	return true;
}


void SimbaUDPCommunication::datagramReceived() {

	if(mUdpSocket == 0) {
		Core::log("SimbaUDPCommunication::datagramReceived: mUdpSocket was NULL!");
		return;
	}
	
	mSocketMutex.lock();
	QByteArray data(mUdpSocket->pendingDatagramSize(), 0);
	mUdpSocket->readDatagram(data.data(), data.size(), 
  						mLastReceivedHostAddress, &mLastReceivedPort);
	mDatagrams.append(data);
	mSocketMutex.unlock();

	mReadWaitCondition.wakeAll();

}

void SimbaUDPCommunication::writeInt(int value) {
	mData.push_back((char)  value);
	mData.push_back((char) (value >> 8));
	mData.push_back((char) (value >> 16));
	mData.push_back((char) (value >> 24));
}

void SimbaUDPCommunication::writeFloat(float value) {

	int i = 0;
	char *c = (char*) &value;
	char *ic = (char*) &i;
	ic[0] = c[0];
	ic[1] = c[1];
	ic[2] = c[2];
	ic[3] = c[3];

	writeInt(i);
}

int SimbaUDPCommunication::maxAvailableIntegers() {
	return ((mData.size() - mDataPosition) / 4);
}

int SimbaUDPCommunication::readNextInt() {
	if(mDataPosition + 4 > mData.size()) {
		return -1;
	}
	int value = (int)
		((int) 0) |	(((int) (mData[mDataPosition + 3]) & 0xff) << 24)
		| (((int) (mData[mDataPosition + 2]) & 0xff) << 16)
		| (((int) (mData[mDataPosition + 1]) & 0xff) << 8)
		| (((int) mData[mDataPosition]) & 0xff);
	
	mDataPosition += 4;
	return value;
}

float SimbaUDPCommunication::readNextFloat() {

	int i = readNextInt();
	float value = 0.0;
	char *c = (char*) &value;
	char *ic = (char*) &i;
	c[0] = ic[0];
	c[1] = ic[1];
	c[2] = ic[2];
	c[3] = ic[3];

	return value;
}


void SimbaUDPCommunication::resetDatagram() {
	mData.clear();
	mDataPosition = 0;
}

void SimbaUDPCommunication::sendDatagram() {
	writeInt(COM_END);
	if(mUdpSocket != 0) {
		mUdpSocket->writeDatagram(mData, *mLastReceivedHostAddress, mLastReceivedPort);
	}
	else {
		Core::log("SimbaUDPCommunication::sendDatagram: mUdpSocket was NULL");
	}
	mData.clear();
	mDataPosition = 0;
	
	//cout << "Sending Datagram." << endl;
}


void SimbaUDPCommunication::run() {
	
	Core::getInstance()->setMainExecutionThread();	
	
	startUDPServer();

	Core::getInstance()->clearMainExecutionThread();

	if(mOwnerThread != 0) {
        while(mOwnerThread->isRunning()) {
            mOwnerThread->quit();
        }
	}

	QCoreApplication::instance()->quit();
}


void SimbaUDPCommunication::startListening(QThread *owner) {
	mOwnerThread = owner;
	if(mUdpSocket!= 0) {
		mUdpSocket->close();
		delete mUdpSocket;
	}
	mUdpSocket = new QUdpSocket();
	mUdpSocket->bind(mPortValue->get()); 
	connect(mUdpSocket, SIGNAL(readyRead()),
                this, SLOT(datagramReceived()));
}


void SimbaUDPCommunication::stopListening() {
	disconnect(mUdpSocket, SIGNAL(readyRead()),
                this, SLOT(datagramReceived()));
}


void SimbaUDPCommunication::setPort(int port) {
	mPortValue->set(port);
}


void SimbaUDPCommunication::stopCommunication() {
	mRunCommunicationLoop = false;

	if(QThread::currentThread() != this) {
		wait();
	}
}

void SimbaUDPCommunication::startUDPServer() {
	
	while(mUdpSocket == 0) {
		;
	}
	
	mRunCommunicationLoop = true;
	mCommunicationRunning = true;
	mTimer.stop();

	//wait for Simba to connect.
	if(!waitForNextDatagram()) {
		return;
	}

	if(!mRunCommunicationLoop) {
		mCommunicationRunning = false;
		return;
	}
	else {
		mDataPosition = 1;
		if(mData.size() != 5 || mData[0] != ((char) 0) || readNextInt() != COM_END) {
			qDebug("Received a mismatching datagram packet! Stopping UDP Server. [connect]");
			return;
		}
// 		cout << "runCommunicationLoop 1" << endl;
		resetDatagram();
		
// 		cout << "runCommunicationLoop 2 reset" << endl;
		
		//wait for handshake
		if(!waitForNextDatagram()) {
			return;
		}

// 		cout << "runCommunicationLoop 3 wait" << endl;
	}

	if(!mRunCommunicationLoop) {
		mCommunicationRunning = false;
		return;
	}
	else {
		
// 		cout << "2 runCommunicationLoop 1" << endl;
		if(maxAvailableIntegers() != 3 || 
				readNextInt() != COM_ID_HANDSK_REQ ||
				readNextInt() != COM_END) 
		{
			qDebug("Received a mismatching datagram packet! Stopping UDP Server. [handshake]");
			return;
		}
		
		//cout << "Got handshake info." << endl;
		
		//send handshake messages.
		sendHandShakeInformation();
		
		//cout << "Handshake done." << endl;
	}
	
	while(mRunCommunicationLoop) {
	
// 		cout << "run 1" << endl;
		//wait for next generation request
		if(!waitForNextDatagram()) {
			return;
		}

// 		cout << "run 2" << endl;
		
		if(!mRunCommunicationLoop) {
// 			cout << "run break" << endl;
			break;
		}
		
// 		cout << "run 3" << endl;
		if(mData.size() < 1) {
			qDebug("Received a mismatching datagram packet! Stopping UDP Server. [size]");
			return;
		}
		
// 		cout << "run 4" << endl;
		int id = readNextInt();
		
// 		cout << "run 5" << endl;

		if(id == COM_ID_INIT_REQ) {
			//cout << "Got Generation init." << endl;
			//next generation
			if(maxAvailableIntegers() < 3) {
				qDebug("Received a mismatching datagram packet! Stopping UDP Server. [generation]");
				return;
			}
			
// 			cout << "run - Generation init 1" << endl;
			//TODO read robot number (?) and seed.
			readNextInt();
// 			cout << "run - Generation init 2" << endl;
			readNextInt();
// 			cout << "run - Generation init 3" << endl;
			
			if(readNextInt() != COM_END) {
				qDebug("Received a mismatching datagram packet! Stopping UDP Server. [generationE]");
				return;
			}
			
// 			cout << "run - Generation init 4" << endl;
			resetDatagram();
// 			cout << "run - Generation init 5" << endl;
			writeInt(COM_ID_INIT_ACK);
// 			cout << "run - Generation init 6" << endl;
			sendDatagram();
// 			cout << "run - Generation init 7" << endl;
		}
		else if(id == COM_ID_NEXT_TRY_REQ) {
			//cout << "Got Try init." << endl;
			//next try
			if(maxAvailableIntegers() < 2) {
				qDebug("Received a mismatching datagram packet! Stopping UDP Server. [try]");
				return;
			}
// 			cout << "run - try init 1" << endl;
			readNextInt(); //robot number
// 			cout << "run - try init 2" << endl;
			if(readNextInt() != COM_END) {
				qDebug("Received a mismatching datagram packet! Stopping UDP Server. [tryE]");
				return;
			}
			
// 			cout << "run - try init 3" << endl;
			//reset the simulation for the next try.
			if(mResetEvent == 0) {
				qDebug("SimbaUDPCommunication: mResetEvent was NULL!");
			}
			else {
			mCurrentStep->set(0);
// 			cout << "run - trigger Reset" << endl;
			mResetEvent->trigger();
			mResetFinalizedEvent->trigger();
// 			cout << "run - try init 4" << endl;
			}
			resetDatagram();
// 			cout << "run - try init 5" << endl;
			writeInt(COM_ID_NEXT_TRY_ACK);
// 			cout << "run - try init 6" << endl;
			sendDatagram();
// 			cout << "run - try init 7" << endl;
		}
		else if(id == COM_ID_RESET_REQ) {
			//cout << "Got individual init." << endl;
			//next individual
			if(maxAvailableIntegers() < 2) {
				qDebug("Received a mismatching datagram packet! Stopping UDP Server. [individual]");
				return;
			}
// 			cout << "run - individual init 1" << endl;
			readNextInt();
// 			cout << "run - individual init 2" << endl;
			if(readNextInt() != COM_END) {
				qDebug("Received a mismatching datagram packet! Stopping UDP Server. [individualE]");
				return;
			}
			
// 			cout << "run - individual init 3" << endl;
			//reset the simulation for the next try (for the first try, no additional try 
			//message will  be sent).
			if(mResetEvent == 0) {
				qDebug("SimbaUDPCommunication: mResetEvent was NULL!");
			}
			else {
// 			cout << "run - individual init 4" << endl;
				mCurrentStep->set(0);
				mResetEvent->trigger();
				mResetFinalizedEvent->trigger();
// 			cout << "run - individual init 5" << endl;
			}
			
			resetDatagram();
// 			cout << "run - individual init 6" << endl;
			writeInt(COM_ID_RESET_ACK);
// 			cout << "run - individual init 7" << endl;
			sendDatagram();
// 			cout << "run - individual init 8" << endl;
			
		}
		else if(id == COM_ID_DATA_REQ) {
			if(maxAvailableIntegers() < 3) {
				qDebug("Received a mismatching datagram packet! Stopping UDP Server. [data]");
				return;
			}
// 			cout << "run - data 1" << endl;
			readNextInt(); //robot number
// 			cout << "run - data 2" << endl;
			
			int numberOfActuators = readNextInt();
			
// 			cout << "run - data 3" << endl;

			if(maxAvailableIntegers() < numberOfActuators + 1) {
				qDebug("Received a mismatching datagram packet! Stopping UDP Server. [dataActuators]");
				return;
			}
// 			cout << "run - data 4" << endl;
			//qDebug("Found Actuators: %d %d", numberOfActuators, mInputValues.size());
			for(int i = 0; i < numberOfActuators && i < mInputValues.size(); ++i) {
				//read the actuator values.
// 				cout << readNextFloat() << "  ";
				double value = readNextFloat();
				mInputValues.at(i)->setNormalized(value);
// 				qDebug("got Acutator: %f", value);
			}
// 			cout << endl;
// 			cout << "run - data 5" << endl;;
			
			if(readNextInt() != COM_END) {
				qDebug("Received a mismatching datagram packet! Stopping UDP Server. [dataEnd]");
				return;
			}
			
// 			cout << "run - data 6" << endl;
			//execute simulation step.
			if(mNextStepEvent == 0) {
				qDebug("SimbaUDPCommunication: mNextStepEvent was NULL!");
			}
			else {
// 			cout << "run - data 7" << endl;
				mCurrentStep->set(mCurrentStep->get() + 1);
				mNextStepEvent->trigger();
				if(mStepCompletedEvent== 0) {
					qDebug("SimbaUDPCommunication: mStepCompletedEvent was NULL!");
				}else {
					mStepCompletedEvent->trigger();
				}
// 			cout << "run - data 8" << endl;
				Core::getInstance()->executePendingTasks();
				if(!mRunCommunicationLoop) {
					break;
				}
			}
			
			//send sensor values.
			resetDatagram();
// 			cout << "run - data 9" << endl;
			writeInt(COM_ID_DATA_ACK);
// 			cout << "run - data 10" << endl;
			writeInt(0); //robot number
			
// 			cout << "run - data 11" << endl;
			//robot status (kill agent if not 0)
			writeInt(0);
// 			cout << "run - data 12" << endl;
			
			//send number of sensors.
			writeInt(mOutputValues.size());
			
			//write sensor data 
// 			cout << "run - send sensor: " ;
			for(int i = 0; i < mOutputValues.size(); ++i) {
				
// 			cout << i << " ";
				writeFloat(mOutputValues.at(i)->getNormalized());
			}
// 			cout << "Sending Sensors: " << endl;
			sendDatagram();
			//cout << "run - sensors sended" << endl;
		}
		
	}

	mCommunicationRunning = false;
	
}


bool SimbaUDPCommunication::waitForNextDatagram() {

	bool interruptible = false;

	Core::getInstance()->executePendingTasks();

	//TODO use mutex to lock against read events while checking.
	mSocketMutex.lock();
	if(!mDatagrams.isEmpty()) {
		mData = mDatagrams.first();
		mDatagrams.removeFirst();
		mSocketMutex.unlock();
// 		cerr << "Found one that wa savailable" << endl;
		return true;
	}
	mSocketMutex.unlock();

	bool notified = false;
// 	cerr << "checking for next datagram" << endl;

	int pendingTaskCounter = 0;

	do {
		mReadMutex.lock();
		notified = mReadWaitCondition.wait(&mReadMutex, 50);
		mReadMutex.unlock();

		if(++pendingTaskCounter > 5) {
			pendingTaskCounter = 0;
			Core::getInstance()->executePendingTasks();
		}

		if(!mRunCommunicationLoop) {
			return false;
		}
		if(!mDatagrams.isEmpty()) {
			break;
		}

	} while((!notified || (mDatagrams.isEmpty() && !interruptible)) && mRunCommunicationLoop);

	if(!mRunCommunicationLoop) {
		return false;
	}

// 	cerr << "Done checking for next datagram" << endl;

	mSocketMutex.lock();

	//mDatagramMutex.lock();
	bool receivedData = true;

	if(mDatagrams.isEmpty()) {
		mData.clear();
		receivedData = false;
	}
	else {
		mData = mDatagrams.first();
		mDatagrams.removeFirst();
	}
	//mDatagramMutex.unlock();
	mSocketMutex.unlock();

	return true;

// 	//TODO use mutex to lock against read events while checking.
// 	mSocketMutex.lock();
// 	while(mRunCommunicationLoop && mDatagrams.size() <= 0) {
// 		mSocketMutex.unlock();
// 		
// 		bool gotDatagram = false;
// 		while(!gotDatagram && mRunCommunicationLoop) {
// 			mReadMutex.lock();
// 			gotDatagram = mReadWaitCondition.wait(&mReadMutex, 100);
// 			mReadMutex.unlock();
// 
// 			if(!gotDatagram) {
// 				Core::getInstance()->executePendingTasks();
// 			}
// 		}
// 		
// 		if(!mRunCommunicationLoop) {
// 			mDataPosition = 0;
// 			return;
// 		}
// 		mSocketMutex.lock();
// 	};
// 	mData = mDatagrams.first();
// 	mDatagrams.removeFirst();
// 	mDataPosition = 0;
// 	
// 	mSocketMutex.unlock();
}

void SimbaUDPCommunication::sendHandShakeInformation() {
	
	resetDatagram();

	mInputValues.clear();
	mOutputValues.clear();

	cerr << "Sending handshake " << endl;
	
	SimObjectGroup *group = 0;
	if(mSimObjectGroupName->get() == "Default") {
		cerr << "size: " << Physics::getPhysicsManager()
				->getSimObjectGroups().size() << endl;
		group = Physics::getPhysicsManager()
				->getSimObjectGroups().value(0);
	}
	else {
	 	group = Physics::getPhysicsManager()
				->getSimObjectGroup(mSimObjectGroupName->get());
	}

	if(group == 0) {
		cerr << "Warning: Could not find required AgentInterface! " << endl;
	}
	else {
		//cerr << "Reading " << simObjects.size() << " SimObjects." << endl;
		{
			mInputValues << group->getInputValues();
			mOutputValues << group->getOutputValues();
		}
	}
	
	writeInt(COM_ID_HANDSK_ACK);
	
	//cerr << "Adding " << mInputValues.size() << " Inputs and " 
	//		<< mOutputValues.size() << " Outputs." << endl;
	{
		for(int i = 0; i < mInputValues.size(); ++i) {
			//TODO do this on demand.
			mInputValues.at(i)->setNormalizedMin(-1.0);
			mInputValues.at(i)->setNormalizedMax(1.0);
			writeInt(COM_HS_TAG_MOTOR);
		}
	}
	
	{	
		for(int i = 0; i < mOutputValues.size(); ++i) {
			//TODO do this on demand.
			mOutputValues.at(i)->setNormalizedMin(-1.0);
			mOutputValues.at(i)->setNormalizedMax(1.0);
			writeInt(COM_HS_TAG_SENSOR);
		}
	}

	sendDatagram();
	
}

QString SimbaUDPCommunication::getName() const {
	return ParameterizedObject::getName();
}

}




