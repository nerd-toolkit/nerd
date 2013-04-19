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



#include "YarsComSimulationAlgorithm.h"
#include <iostream>
#include <QList>
#include <QVector>
#include <QtGlobal>
#include "Core/Core.h"
#include <QCoreApplication>
#include "Physics/Physics.h"
#include <QUdpSocket>
#include <QByteArray>
#include <QString>
#include <iostream>
#include "Math/Random.h"
#include "SimulationConstants.h"
#include <QThread>
#include <QTime>


using namespace std;

namespace nerd {


/**
 * Constructs a new YarsComSimulationAlgorithm.
 */
YarsComSimulationAlgorithm::YarsComSimulationAlgorithm()
	: PhysicalSimulationAlgorithm("Yars"), mConnectToYars(0), mPort(0), 
	  mClientPort(_STANDARD_COMPORT_CLIENT),
	  mEnvironmentXML(0), mConnected(false), mShutDown(false), mParallelMode(false),
	  mDebug(false), mRobotStates(0), mAgentInterface(0), mAgentObject(0), mShutDownEvent(0),
	  mTerminateTryEvent(0), mSocket(0), mAddress(0), mSendBuffer(0),
	  mRecBuffer(0), mCurrentTry(0), mSeedValue(0), mGeneration(0), mResetRequested(false),
	  mSendXML(true)
{
	//create the agent interface as control interface to the neural networks.
	mAgentInterface = new SimObjectGroup("Yars", "Agent");
	mAgentObject = new InterfaceSimObject("YarsAgent", "");
	mAgentInterface->addObject(mAgentObject);
	Physics::getPhysicsManager()->addSimObjectGroup(mAgentInterface);

	//get notified when the system shuts down.
	mShutDownEvent = Core::getInstance()->getShutDownEvent();
	mShutDownEvent->addEventListener(this);

	//create and publish parameters
	mConnectToYars = new BoolValue(false);
	mPort = new IntValue(_STANDARD_COMPORT_SERVER);
	mTerminateTryAtAbortSignal = new BoolValue(false);
	mSendBuffer = new UdpDatagram();
	mRecBuffer = new UdpDatagram();
	mAddress = new QHostAddress(_STANDARD_IP_SERVER);
	mSocket = 0;
	mEnvironmentXML = new FileNameValue("");
	mEnvironmentXML->setNotifyAllSetAttempts(true);
	//mEnvironmentXML->useAsFileName(true);
	mRobotStates = _STANDARD_ROBOT_STATE_ABORT;

	addParameter("Yars/ConnectToYars", mConnectToYars);
	addParameter("Yars/Port", mPort);
	addParameter("Yars/EnvironmentXML", mEnvironmentXML);
	addParameter("Yars/TerminateTryAtAbort", mTerminateTryAtAbortSignal);

	publishAllParameters();
}


/**
 * Destructor.
 */
YarsComSimulationAlgorithm::~YarsComSimulationAlgorithm() {
	delete mSendBuffer;
	delete mRecBuffer;
	delete mAddress;
	if(mSocket != 0) {
		delete mSocket;
	}
	
	//Other values (Input, Output, Interface) are deleted by ValueManager.
}


bool YarsComSimulationAlgorithm::resetPhysics() {

	if(mDebug) std::cout << "resetPhysics(): reset requested\n";
	if(mTerminateTryEvent == 0) {
		mTerminateTryEvent = Core::getInstance()->getEventManager()
				->getEvent(SimulationConstants::EVENT_EXECUTION_TERMINATE_TRY);
	}

	mResetRequested = true;
	return true;
}



bool YarsComSimulationAlgorithm::checkForReset() {
	if(mDebug) std::cout << "checkForReset()\n";
	if(!mResetRequested) {
		return true;
	}
	mResetRequested = false;

	checkForConnection();
	
	if(mShutDown) {
		disconnectFromYars();
		return true;
	}

	int currentTry = 0;
	if(mCurrentTry != 0) {
		currentTry = mCurrentTry->get();
	}

	int generation = 0;
	if(mGeneration != 0) {
		generation = mGeneration->get();
	}

	int seed = Random::nextInt();
	if(mSeedValue != 0) {
		seed = mSeedValue->get();
	}

	if(currentTry == 0) {

		// build new environment
		if(generation == 0 && mEnvironmentXML->get() != "" && mSendXML) {
			mSendXML = false;
			if(!newEnvironment()) {
				std::cerr << "Unable to send new environment data in resetPhysics()\n";
				Core::log("YarsComSimulationAlgorithm: resetPhysics(): Unable to send new environment data");
				return false;
			}
			if(mDebug) std::cout << "...new Environment is sent, sending reset signal...\n";
			// send reset
			if(!reset(seed)) {
				std::cerr << "Unable to perform reset() in resetPhysics()\n";
				Core::log("YarsComSimulationAlgorithm: resetPhysics(): Unable to perform reset() in resetPhysics()");
				return false;
			}
			if(mDebug) std::cout << "...conducting new handshake after rebuilt environment and reset...\n";
			if(!handshake()) {
				std::cerr << "Unable to conduct handshake()\n";
				Core::log("YarsComSimulationAlgorithm: resetPhysics(): Unable to conduct handshake");
				return false;
			}
		}
		if(mDebug) std::cout << "sending init() request\n";
		// send init 
		if(!init(seed)) {
			std::cerr << "Unable to perform init() in resetPhysics()\n";
			Core::log("YarsComSimulationAlgorithm: resetPhysics(): Unable to perform reset() in resetPhysics()");
			return false;
		}
		if(mDebug) std::cout << "sending reset() request\n";
		// send reset
		if(!reset(seed)) {
			std::cerr << "Unable to perform reset() in resetPhysics()\n";
			Core::log("YarsComSimulationAlgorithm: resetPhysics(): Unable to perform reset() in resetPhysics()");
			return false;
		}
		
	}
	else {
		//send next try 
		if(!nextTry(seed)) {
			std::cerr << "Unable to send nextTry() in resetPhysics()\n";
			Core::log("YarsComSimulationAlgorithm: resetPhysics(): Unable to send nextTry() in resetPhysics()");
			return false;
		}
		
	}
	return true;
}



bool YarsComSimulationAlgorithm::finalizeSetup() {
	//nothing to be done.
	return true;
}


bool YarsComSimulationAlgorithm::executeSimulationStep(PhysicsManager*) {

	checkForConnection();
	checkForReset();

	if(mShutDown) {
		disconnectFromYars();
		return true;
	}

	// send InputValues to Yars, then trigger next simulation step in Yars.
	// send motor data
	//if(mDebug) std::cout << "Sending motor Data to YARS\n";
  	if(!sendMotorData()) {
		return false;
	}
  
	if(mShutDown) {
		disconnectFromYars();
		return true;
	}
	if(!mConnected) { 
		return true; 
	}
	//if(mDebug) std::cout << "\n---> Receiving sensor data\n";
	// receive robot state and sensor data
  	if(!receiveSensorData()) {
		return false;
	}

	if(mTerminateTryAtAbortSignal->get() && mRobotStates == _STANDARD_ROBOT_STATE_ABORT) {
		//terminate current try
		if(mTerminateTryEvent != 0) {
			mTerminateTryEvent->trigger();
		}
	}

	return true;
}


//will not do anything useful.
QList<Contact> YarsComSimulationAlgorithm::getContacts() const {
	return QList<Contact>();
}


//will not do anything useful
void YarsComSimulationAlgorithm::disableCollisions(CollisionObject*, CollisionObject*, bool)
{
}


//will not do anything useful
void YarsComSimulationAlgorithm::updateCollisionHandler(CollisionManager*) {
}


QString YarsComSimulationAlgorithm::getName() const {
	return PhysicalSimulationAlgorithm::getName();
}


void YarsComSimulationAlgorithm::valueChanged(Value *value) {
	PhysicalSimulationAlgorithm::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mEnvironmentXML) {
		mSendXML = true;
	}
}


void YarsComSimulationAlgorithm::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mShutDownEvent) {
		disconnectFromYars();
		mShutDown = true;
		Physics::getCollisionManager()->setCollisionHandler(0);
	}
}


/**
 * Checks if NERD is connected to a Yars simulator. If not, then this method
 * blocks until a connection could be established.
 */
void YarsComSimulationAlgorithm::checkForConnection() {
	
	if(mCurrentTry == 0) {
		//get current try value
		mCurrentTry = Core::getInstance()->getValueManager()->getIntValue(
						SimulationConstants::VALUE_EXECUTION_CURRENT_TRY);
	}
	if(mSeedValue == 0) {
		mSeedValue = Core::getInstance()->getValueManager()->getIntValue(
						SimulationConstants::VALUE_RANDOMIZATION_SIMULATION_SEED);
	}
	if(mGeneration == 0) {
		mGeneration = Core::getInstance()->getValueManager()->getIntValue(
						SimulationConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER);
	}

	while(!mConnected || !mConnectToYars->get()) {
		if(mConnected) {
			disconnectFromYars();
			mConnected = false;
		}

		while(mConnectToYars->get() == false && !mShutDown) {
			//wait until a connection should be established
			QCoreApplication::instance()->thread()->wait(25);
			Core::getInstance()->executePendingTasks();
		}
		if(mShutDown) {
			return;
		}

		if(mSocket != 0) {
			delete mSocket;
		}
		mSocket = new QUdpSocket();
		bool ok = false;
		while(!ok) {
			ok = mSocket->bind(*mAddress, mClientPort++);
		}
		mSocket->connectToHost(*mAddress, mPort->get());
		//mSocket->moveToThread(QCoreApplication::instance()->thread());

		mSendBuffer->clear();
		mRecBuffer->clear();

		// register		
		mSendBuffer->writeByte(0);
		
		if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
			std::cerr << "Unable to register with YARS server, disconnecting...\n";
			Core::log("YarsComSimulationAlgorithm: checkForConnection(): Unable to register with YARS server");
			mConnectToYars->set(false);
			mConnected = false;
			continue;
		}
		mSendBuffer->clear();	

		if(!handshake()) {
			std::cerr << "YarsClient: Unable to conduct handshake()\n";
			Core::log("YarsComSimulationAlgorithm: checkForConnection(): Unable to conduct handshake");
			mConnectToYars->set(false);
			mConnected = false;
			continue;
		}

		if(mDebug) std::cout << "... receiving robot structure done.\n";
		mConnected = true; //only when successfully.
		
	}
}


bool YarsComSimulationAlgorithm::handshake() {
	// doHandshake: -extended mode-
	
	int robotNum           = -1;
	int compoundNum        = -1;
	int segmentNum         = -1; 
	int motorNum           = -1;
	int sensorCINum        = -1;
	int sensorAUXNum       = -1;
	bool continueReceiving = true;

	if(mDebug) std::cout << "sending extended handshake request to port (" << mPort->get() << ") ...\n";
	mSendBuffer->writeInt(_STANDARD_COM_ID_HANDSHAKE_EXT_REQ);

	if(mParallelMode) {
		mSendBuffer->writeInt(_STANDARD_COM_ID_PARALLEL_REQ);
		if(mDebug) std::cout << "sending PARALLEL REQ (" << mPort->get() << ") ...\n";
	}
	else {
		mSendBuffer->writeInt(_STANDARD_COM_ID_SERIAL_REQ);
		if(mDebug) std::cout << "sending SERIAL REQ (" << mPort->get() << ") ...\n";
	}
	
	mSendBuffer->writeInt(_STANDARD_COMEND);
	mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get());
	mSendBuffer->clear();

	mInputValues.clear();
	mOutputValues.clear();
	mInfoValues.clear();

	if(mDebug) std::cout << "start receiving robot structure information ... \n";
	if(!readNextDatagram()) {
		datagramReceptionErrorOutput();
		mConnectToYars->set(false);
		mConnected = false;
		return false;
	}
	
	// verify the ACKs
	int tmp = mRecBuffer->readNextInt();
	if(tmp != _STANDARD_COM_ID_HANDSHAKE_EXT_ACK) {
		std::cerr << "YarsClient: no Handshake_EXT_ACK received, disconnecting...\n";
		Core::log("YarsComSimulationAlgorithm: handshake(): No Handshake_EXT_ACK received from YARS");
		mRecBuffer->clear();
		return false;
	}
	tmp = mRecBuffer->readNextInt();
	if(mParallelMode) {
		if(tmp != _STANDARD_COM_ID_PARALLEL_ACK) {
			std::cerr << "YarsClient: no Parallel_ACK received, disconnecting...\n";
			Core::log("YarsComSimulationAlgorithm: handshake(): No Parallel_ACK received from YARS");
			mRecBuffer->clear();
			return false;
		}
	}
	else {
		if(tmp != _STANDARD_COM_ID_SERIAL_ACK) {
			std::cerr << "YarsClient: no Serial_ACK received, disconnecting...\n";
			Core::log("YarsComSimulationAlgorithm: handshake(): No Serial_ACK received from YARS");
			mRecBuffer->clear();
			return false;
		}
	}


	// receive robot structure
	while(continueReceiving) {
		tmp = mRecBuffer->readNextInt();	

		switch(tmp)
		{
		case _STANDARD_COM_TAGROBOT:
			{
			robotNum++;
			compoundNum  = -1;
			segmentNum   = -1;
			motorNum     = -1;
			sensorCINum  = -1;
			sensorAUXNum = -1;
			if(mDebug) std::cout << "	  Robot " << robotNum << endl;
			break;
			}
		case _STANDARD_COM_TAGCOMPOUND:
			{
			compoundNum++;
			segmentNum  = -1;
			motorNum     = -1;
			sensorCINum  = -1;
			sensorAUXNum = -1;
			if(mDebug) std::cout << "    Compound " << compoundNum << endl;
			break;
			}
		case _STANDARD_COM_TAGSEGMENT:
			{
			segmentNum++;
			motorNum     = -1;
			sensorCINum  = -1;
			sensorAUXNum = -1;
			if(mDebug) std::cout << "      Segment " << segmentNum << endl;
			break;
			}
		case _STANDARD_COM_TAGMOTOR:
			{
			motorNum++;
			QString prefix = "";
			QString name = "";
			double min = 0;
			double max = 0;
			if(!receiveExtendedSensorMotorInfo(name, min, max, tmp)) {
				return false;
			}
			InterfaceValue *motor = new InterfaceValue(prefix,name,0,min,max);
			mInputValues.append(motor);
			if(mDebug) std::cout << "         Motor " << motorNum << " " << name.toStdString().c_str() 
								<< ", min = " << min << ", max = " << max << endl;
			break;
			}
		case _STANDARD_COM_TAGSENSOR:
			{
			sensorCINum++;
			QString prefix = "";
			QString name = "";
			double min = 0;
			double max = 0;
			if(!receiveExtendedSensorMotorInfo(name, min, max, tmp)) {
				return false;
			}
			InterfaceValue *sensor = new InterfaceValue(prefix,name,0,min,max);
			mOutputValues.append(sensor);
			if(mDebug) std::cout << "         Sensor " << sensorCINum << " " 
						<< name.toStdString().c_str() << ", min = " << min << ", max = " << max << endl;
			break;
			}
		case _STANDARD_COM_TAGSENSOR_AUX:
			{
			sensorAUXNum++;
			QString prefix = "";
			QString name = "";
			double min = 0;
			double max = 0;
			if(!receiveExtendedSensorMotorInfo(name, min, max, tmp)) {
				return false;
			}
			InterfaceValue *sensor = new InterfaceValue(prefix,name,0,min,max);
			mInfoValues.append(sensor);
			if(mDebug) std::cout << "         Auxiliary Sensor " << sensorAUXNum << " " 
					<< name.toStdString().c_str() << ", min = " << min << ", max = " << max << endl;
			break;
			}
		case _STANDARD_COM_TAG_ADD_HANDSHAKE_PACKET:
			{
			// robot structure description does not fit into current packet,
			// additional packet necessary
			if(mDebug) std::cout << "    ADDITIONAL_HANDSHAKE_PACKET\n\n";
			if(!additionalHandshakePacket(tmp)) {
				return false;	
			}
			break;
			}
		case _STANDARD_COMEND:
			{
			continueReceiving = false;
			if(mDebug) std::cout << "  END ROBOT DESCRIPTION\n\n";
			break;
			}
		default:
			// throw some exception here
			break;
		}
	}
	updateAgentInterface();
	return true;
}


// this function is (possibly repeatedly) called in case the robot structure is
// so large that its description may not fit into a single UDP packet
bool YarsComSimulationAlgorithm::additionalHandshakePacket(int &tmp)
{
	if(mDebug) std::cout << "additionalHandshakePacket()\n";
	// 1st: verify that comend tag finalizes packet
	tmp = mRecBuffer->readNextInt();
	if(tmp != _STANDARD_COMEND)
	{
		std::cerr << "StandardYarsClient: no COMEND tag received (instead " << tmp << ") ... exiting \n";
		return false;
	}
	else
	{
	
	if(mDebug) std::cout << "  - COMEND received (more handshake packets to follow) \n";
	}
	mRecBuffer->clear();
	
	// 2nd: send additional handshake request
	if(mDebug) std::cout << "  - sending additional extended handshake request to port " << mPort->get() <<" ...\n";
	mSendBuffer->writeInt(_STANDARD_COM_ID_HANDSHAKE_EXT_REQ);
	mSendBuffer->writeInt(_STANDARD_COMEND);
	mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get());
	mSendBuffer->clear();	

	// 3rd: start receiving additional handshake packet
	if(mDebug) std::cout << "  - start receiving additional handshake packet... \n";
	// verify the ACKs
	if(!readNextDatagram()){
		datagramReceptionErrorOutput();
		return false;
	}
	tmp = mRecBuffer->readNextInt(); 
	
	if(tmp != _STANDARD_COM_ID_HANDSHAKEACK &&
	   tmp != _STANDARD_COM_ID_HANDSHAKE_EXT_ACK)
	{
		std::cerr << "StandardYarsClient: no Handshake(this->_EXT_)ACK received ... exiting \n";
		return false;
	}
	else
	{
		if(mDebug) std::cout << "  - additional Handshake(this->_EXT_)ACK received \n";
	}
	
	// 4th ready to continue receiving robot struct information
	return true;
}



bool YarsComSimulationAlgorithm::receiveExtendedSensorMotorInfo(QString &name, double &min, double &max, int &tmpInt)
{
	int   stringLength = 0;
		if(mDebug) std::cout << "receiveExtendedSensorMotorInfo()\n";
	// receive mapping min/max
	float tmpFloat = mRecBuffer->readNextFloat(); 
	min = tmpFloat;
	tmpFloat = mRecBuffer->readNextFloat(); 
	max = tmpFloat;
	mRecBuffer->clear();
	
	// send handshake req
	mSendBuffer->writeInt(_STANDARD_COM_ID_HANDSHAKE_EXT_REQ);
	mSendBuffer->writeInt(_STANDARD_COMEND);
	mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get());
	mSendBuffer->clear();
	
	// receive handshake ext ack, string length, string
	if(!readNextDatagram()){
		datagramReceptionErrorOutput();
		return false;
	}

	tmpInt = mRecBuffer->readNextInt();
	if(tmpInt != _STANDARD_COM_ID_HANDSHAKE_EXT_ACK)
	{
		std::cerr << "wrong TAG received (should have been HANDSHAKE EXT ACK) ... ABORT \n";
		return false;
	}
	stringLength = mRecBuffer->readNextInt();
	char tmpChar = 0;	

	for(int i=0; i < stringLength; i++)
	{
		tmpChar = mRecBuffer->readNextByte();
		name.append(tmpChar);
	}
	mRecBuffer->clear();	

	// send handshake ext request
	mSendBuffer->writeInt(_STANDARD_COM_ID_HANDSHAKE_EXT_REQ);
	mSendBuffer->writeInt(_STANDARD_COMEND);
	mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get());
	mSendBuffer->clear();

	// wait for handshake ack
	if(!readNextDatagram()){
		datagramReceptionErrorOutput();
		return false;
	}

	tmpInt = mRecBuffer->readNextInt();
	if(tmpInt != _STANDARD_COM_ID_HANDSHAKE_EXT_ACK)
	{
		std::cerr << "wrong TAG received (should have been HANDSHAKE EXT ACK) ... ABORT \n";
		return false;
	}

	return true;
}


// send init() to YARS in resetPhysics()
bool YarsComSimulationAlgorithm::init(int &seed)
{
	int tmp = 0;
	int robotNum = 0;
	if(mDebug) std::cout << "\nYarsClient: Init()\n";
	
	mRecBuffer->clear();
	mSendBuffer->clear();
	
	mSendBuffer->writeInt(_STANDARD_COM_ID_INITREQ);
	// send robot number this client is responsible for
	mSendBuffer->writeInt(robotNum);
	mSendBuffer->writeInt(seed);
	
	if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
		std::cerr << "Unable to send Init() to YARS server\n";
		Core::log("YarsComSimulationAlgorithm: Init(): Unable to send Init() to YARS server");
		return false;
	}
	mSendBuffer->clear();	
	
	
	if(!readNextDatagram()) {
		std::cerr << "YarsClient: error while receiving YARS datagram\n";
		Core::log("YarsComSimulationAlgorithm: Init(): Error while receiving YARS datagram");
		return false;
	}
	
	tmp = mRecBuffer->readNextInt();
	if (tmp != _STANDARD_COM_ID_INITACK)
	{
		std::cerr << "StandardYarsClient: no INIT ACK received, instead: " << tmp << " \n";
		Core::log("YarsComSimulationAlgorithm: Init(): no INIT ACK received");
		return false;
	}
	tmp = mRecBuffer->readNextInt();
	if (tmp != robotNum)
	{
		std::cerr << "StandardYarsClient: wrong robot number received ... ABORT \n";
		Core::log("YarsComSimulationAlgorithm: Init(): wrong robot number received");
		return false;
	}
	mRecBuffer->clear();

	return true;
}


// send reset signal after init() in resetPhysics()
bool YarsComSimulationAlgorithm::reset(int&) {
	if(mDebug) { std::cout << "\nresetPhysics(): reset()\n"; }
	int robotNum = 0;
	int tmp = 0;
	mRecBuffer->clear();
	mSendBuffer->clear();

	mSendBuffer->writeInt(_STANDARD_COM_ID_RESETREQ);
	mSendBuffer->writeInt(robotNum);
	mSendBuffer->writeInt(-1);

	if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
		std::cerr << "Unable to send reset() to YARS server\n";
		Core::log("YarsComSimulationAlgorithm: reset(): Unable to send reset() to YARS server");
		return false;
	}
	mSendBuffer->clear();	

	if(!readNextDatagram()) {
		std::cerr << "YarsClient: error while receiving YARS datagram\n";
		Core::log("YarsComSimulationAlgorithm: reset(): Error while receiving YARS datagram");
		return false;
	}
	tmp = mRecBuffer->readNextInt();
	
	if (tmp != _STANDARD_COM_ID_RESETACK) {
		std::cerr << "YarsClient: no RESET ACK received, instead: " << tmp << " ... ABORT \n";
		Core::log(QString("YarsComSimulationAlgorithm: reset(): No RESET ACK received, instead got ") + QString::number(tmp));
		std::cerr << "printing remaining integers in datagram:\n";
		int tInt = 1;
		while(tInt != 0) {
			tInt = mRecBuffer->readNextInt();
			std::cerr << tInt << "\n";
		}
		return false;
	}


	tmp = mRecBuffer->readNextInt();
	if (tmp != robotNum) {
		std::cerr << "YarsClient: wrong robot number received ... ABORT \n";
		Core::log("YarsComSimulationAlgorithm: reset(): wrong robot number received");
		return false;
	}
	mRecBuffer->clear();
	
	return true;
}


// send nextTry() to YARS, called by resetPhysics
bool YarsComSimulationAlgorithm::nextTry(int &seed) {
	if(mDebug) std::cout << "\nYarsClient: nextTry()\n";	
	int robotNum = 0;
	int tmp = 0;
	mRecBuffer->clear();
	mSendBuffer->clear();
	
	mSendBuffer->writeInt(_STANDARD_COM_ID_NEXT_TRYREQ);
	mSendBuffer->writeInt(robotNum);
	mSendBuffer->writeInt(seed);
	
	if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
		std::cerr << "Unable to send nextTry() to YARS server\n";
		Core::log("YarsComSimulationAlgorithm: nextTry(): Unable to send nextTry() to YARS server");
		return false;
	}
	mSendBuffer->clear();	
	
	
	if(!readNextDatagram()) {
		std::cerr << "YarsClient: error while receiving YARS datagram\n";
		Core::log("YarsComSimulationAlgorithm: nextTry(): Error while receiving YARS datagram");
		return false;
	}
	tmp = mRecBuffer->readNextInt();
	
	if (tmp != _STANDARD_COM_ID_NEXT_TRYACK)
	{
		std::cerr << "YarsClient: no NEXT_TRY ACK received ... ABORT \n";
		Core::log("YarsComSimulationAlgorithm: nextTry(): no NEXT_TRY ACK received");
		return false;
	}
	
	tmp = mRecBuffer->readNextInt();
	if (tmp != robotNum)
	{
		std::cerr << "YarsClient: wrong robot number received ... ABORT \n";
		Core::log("YarsComSimulationAlgorithm: nextTry(): wrong robot number received");
		return false;
	}

	return true;
}


bool YarsComSimulationAlgorithm::sendMotorData() {
	if(mDebug) std::cout << "sendMotorData()\n";
	// udp packet size limit making additional packet necessary:
	//   - udp max packet size minus COM_END TAG minus last motor val size
	int  packetSizeLimit = 4*(3 + _MAX_MOTOR_ITEMS_PER_DATA_REQ - 1);
	bool packetFull      = false;
	bool connectionError = false;
	int  motorDataSent   = 0;
	int robotNum = 0;
	int motorNum = mInputValues.size();	
	
	mRecBuffer->clear();
	mSendBuffer->clear();
	
	mSendBuffer->writeInt(_STANDARD_COM_ID_DATAREQ);
	mSendBuffer->writeInt(robotNum);
	mSendBuffer->writeInt(motorNum);
	
	// send all motor values
	for(int i=0; i < motorNum; i++)
	{
		packetFull = this->addMotorValueToDataReqPacket(packetFull, packetSizeLimit, motorDataSent++, 
								mInputValues[i]->get(), connectionError, motorNum, robotNum);
		//if(mDebug) std::cout << "Sending Data " << mInputValues[i]->get() << " for motor " << i << endl;
		if(connectionError) {
			std::cerr << "Unable to send motorData to YARS server\n";
			Core::log("YarsComSimulationAlgorithm: sendMotorData(): Unable to send motorData to YARS server");
			return false;
		}
	}
	
	mSendBuffer->writeInt(_STANDARD_COMEND);
	if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
		std::cerr << "Unable to send motorData to YARS server\n";
		Core::log("YarsComSimulationAlgorithm: sendMotorData(): Unable to send motorData to YARS server");
		return false;
	}
	mSendBuffer->clear();	
	mRecBuffer->clear();

	return true;
}


bool YarsComSimulationAlgorithm::addMotorValueToDataReqPacket(bool full, int packetSizeLimit, 
			int motorDataSent, double value, bool &connectionError, int motorNum, int robotNum)
{
	if(full == true)
	{
		if(!additionalDataReqPacket(motorDataSent, motorNum, robotNum)) {
			connectionError = true;
		}
	}

	mSendBuffer->writeFloat((float) value);
	if((float) value > packetSizeLimit)
	{
		return true;
	}
	else
	{
		return false;
	}
}



// finalizes and sends current dataReq packet, initializes new one
bool YarsComSimulationAlgorithm::additionalDataReqPacket(int motorDataSent, int motorNum, int robotNum) {
	int packetNum = motorDataSent / _MAX_MOTOR_ITEMS_PER_DATA_REQ;
	int tmp = 0;
	if(mDebug) std::cout << "additionalDataReqPacket()\n";
	// finalize current  data req package
	mSendBuffer->writeInt(_STANDARD_COMEND);
	if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
		std::cerr << "Unable to finalize motorDataPacket\n";
		Core::log("YarsComSimulationAlgorithm: additionalDataReqPacket(): Unable to finalize motorDataPacket");
		return false;
	}
	mSendBuffer->clear();
	
	// every _MAX_NUM_PACKETS_WITHOUT_ACK we wait for a request
	if((packetNum % _MAX_NUM_PACKETS_WITHOUT_ACK) == 0)
	{
		mRecBuffer-> clear();
		if(!readNextDatagram()) {
			std::cerr << "YarsClient: error while receiving YARS datagram\n";
			Core::log("YarsComSimulationAlgorithm: additionalDataReqPacket(): Error while receiving YARS datagram");
			return false;
		}
		// first of all receive robot state
		tmp = mRecBuffer->readNextInt();
		if(tmp != _STANDARD_COM_ID_DATAACK)
		{
			std::cerr << "StandardYarsClient: no DataACK received ... exiting \n";
			Core::log("YarsComSimulationAlgorithm: additionalDataReqPacket(): no DataACK received ...exiting");
			return false;
		}
	}
	
	// initialize new data req package
	mRecBuffer->clear();
	mSendBuffer->writeInt(_STANDARD_COM_ID_DATAREQ);
	mSendBuffer->writeInt(robotNum);           
	
	// send a remaining of this->_robotMotorCount - motorDataSent 
	// values to the client
	mSendBuffer->writeInt(motorNum - motorDataSent);  

	return true;
}



bool YarsComSimulationAlgorithm::receiveSensorData() {
	if(mDebug) std::cout << "receiveSensorData()\n";
	// vector<double> *sensorCIData, vector<double> *sensorAUXData, int *robotState
	int numItemInPacket = 0;
	//int packetIndex = 0;
	int sensorIndex = 0;
	int tmp = 0;
	int robotNum = 0;
	float tmpFloat = 0;
	int sensorCICount = mOutputValues.size(); // of THIS robot
	int sensorAUXCount = mInfoValues.size();  //       "
	QVector<double> tmpSensorValues(sensorCICount + sensorAUXCount);
	int numDataAckPackets = ( MAX(0,(sensorCICount + sensorAUXCount - 1)) 
				/ _MAX_SENSOR_ITEMS_PER_DATA_ACK) + 1;	


	// 1st receive (multiple) packets and insert sensor values into sorted array
	for(int i = 0; i < numDataAckPackets; i++)
	{
		// every _MAX_SENSOR_ITEMS_PER_DATA_ACK we have to send a REQ
		if((i != 0) && ((i % _MAX_NUM_PACKETS_WITHOUT_ACK) == 0))
		{
			mSendBuffer->writeInt(_STANDARD_COM_ID_DATAREQ);
			mSendBuffer->writeInt(_STANDARD_COMEND);
			if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
				std::cerr << "Unable to request sensor data from Yars\n";
				Core::log("YarsComSimulationAlgorithm: receiveSensorData(): Unable to request sensor data from Yars");
				return false;
			}
			mSendBuffer->clear();
		}
		
		mRecBuffer->clear();
		if(!readNextDatagram()) {
			std::cerr << "YarsClient: error while receiving YARS datagram\n";
			Core::log("YarsComSimulationAlgorithm: receiveSensorData(): Error while receiving YARS datagram");
			return false;
		}
		
		// first of all receive robot state
		tmp = mRecBuffer->readNextInt();
		if(tmp != _STANDARD_COM_ID_DATAACK)
		{
			std::cerr << "YarsClient: no DataACK received ... exiting \n";
			Core::log("YarsComSimulationAlgorithm: receiveSensorData(): no DataACKK received");
			return false;
		}
		tmp = mRecBuffer->readNextInt();
		if(tmp != robotNum)           
		{
			std::cerr << "YarsClient: wrong robot number received ... exiting \n";
			Core::log("YarsComSimulationAlgorithm: receiveSensorData(): wrong robot number received");
			return false;
		}
		
		mRobotStates = _STANDARD_ROBOT_STATE_OK;
		tmp = mRecBuffer->readNextInt();
		if(tmp == _STANDARD_ROBOT_STATE_ABORT)
		{
			mRobotStates = _STANDARD_ROBOT_STATE_ABORT;
			std::cout << "StandardYarsClient: Robot state ABORT\n";
		}
		else if(tmp == _STANDARD_ROBOT_STATE_OK)
		{
		}
		else
		{
			std::cerr << "StandardYarsClient: received robot state unknown ... exiting \n";
			Core::log("YarsComSimulationAlgorithm: receiveSensorData(): received robot state unknown");
			return false;
		}
		
		tmp = mRecBuffer->readNextInt();
		if(tmp > (sensorCICount + sensorAUXCount) || tmp < 0)
		{
			std::cerr << "StandardYarsClient: wrong sensor number received ... exiting \n";
			Core::log("YarsComSimulationAlgorithm: receiveSensorData(): wrong sensor number received");
			return false;
		}
		else
		{
			numItemInPacket = 0;
			//packetIndex = 0;
			sensorIndex = 0;
			
			if(tmp >= _MAX_SENSOR_ITEMS_PER_DATA_ACK)
			{
				numItemInPacket = _MAX_SENSOR_ITEMS_PER_DATA_ACK;
			}
			else
			{
				numItemInPacket = tmp;
			}
			sensorIndex = (sensorCICount + sensorAUXCount) - tmp;
			
			for(int j = 0; j < numItemInPacket; j++)
			{
				tmpFloat = mRecBuffer->readNextFloat();
				tmpSensorValues[sensorIndex + j] = tmpFloat;
			}
			
			tmp = mRecBuffer->readNextInt();
			if(tmp != _STANDARD_COMEND)
			{
				std::cerr << "StandardYarsClient: COMEND expected, instead received: " << tmp ;
				Core::log(QString("YarsComSimulationAlgorithm: receiveSensorData(): COMEND expected, instead received: ") + QString::number(tmp));
				return false;
			}
		}
	}
	
	// 2nd: assign received sensor values to specific sensors
	sensorIndex = 0;
	// first assign control input sensor values

	for(int i = 0; i < sensorCICount && i < mOutputValues.size(); i++)
	{
		mOutputValues[i]->set(tmpSensorValues[sensorIndex++]);
	}
	// then assign auxiliary sensor values
	for(int i = 0; i < sensorAUXCount && i < mInfoValues.size(); i++)
	{
		mInfoValues[i]->set(tmpSensorValues[sensorIndex++]);
	}
	
	return true;
}



bool YarsComSimulationAlgorithm::newEnvironment() {
	if(mDebug) std::cout << "newEnvironment()\n";

	if(!clearSim()) {
		std::cerr << "Unable to clearSim() for new Environment\n";
		Core::log("YarsComSimulationAlgorithm: newEnvironment: Unable to send clearSim request");
		return false;
		
	}

	QString inputXMLString = "";
	QString fileName = mEnvironmentXML->get();

	QDir dir(fileName);
	if(dir.exists()) {
		std::cerr << "Tried to lead a directory instead of a file\n";
		Core::log(QString("YarsComSimAlgorithm: newEnvironment: Tried to load a directory instead of a file: [")
		.append(fileName).append("]"));
		return false;
	}

	QFile file(fileName);

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		Core::log(QString("YarsComSimulationAlgorithm: Could not load file ").append(fileName), true);
		file.close();
		return false;
	}

	QTextStream input(&file);
	input.setCodec("UTF-8");
	while (!input.atEnd()) {
		QString line = input.readLine();
		
		inputXMLString += line;
	}
	
	if(!sendXML(&inputXMLString, _STANDARD_SOURCE_TYPE_XML_STRING, 0x07, -1,0)) {
		Core::log("YarsComSimulationAlgorithm: newEnvironment: Unable to send environmentXML data", true);
		file.close();
		return false;
	}
	file.close();

	return true;
}



/** send data to add moveable, environment or simConfig description to the
 * scenarioManager in yars, 
 * - 1st packet structure:
 *     00 -   03  COM_ID_NEW_SIM_REQ
 *     04 -   07  dataID 
 *     08 -   11  totalDataSize 
 *     12 -   15  dataSizePerPacket
 *     16 -   19  sourceType
 *     20 -   23  selectionMask
 *     24 -   27  tryNumber
 *     28 -   31  indyNumber
 *     32 -   35  COMEND
 * - 2nd and following packets structure:
 *      00 -   03  COM_ID_NEW_SIM_REQ
 *      04 -   07  dataID 
 *      08 -   11  packNumber
 *      ...        stringData
 *    1016 - 1019  COMEND
 * @param dataString string data to be send to server
 * @param sourceType dataString either gives filename or xml string, see 
 *          _STANDARD_SOURCE_TYPE_FILE and _STANDARD_SOURCE_TYPE_XML_STRING
 * @param selectionMask bitmask determining which components to add to scenario
 *          manager
 *                 - 0000000x  moveableDescription
 *                 - 000000x0  environmentDescription
 *                 - 00000x00  simConfigDescription
 * @param tryNumber during which try shall the description be loaded
 * @param indyNumber if applicable: for which robot shall the description be
 *                     loaded
 * @return true if data successfully send to server and ack'd, false otherwise
 */
bool YarsComSimulationAlgorithm::sendXML(QString *dataString, int sourceType, int
    selectionMask, int tryNumber, int indyNumber)
{
	int dataID = Random::nextInt(); // number to identify follower packages
	int totalDataSize = (*dataString).size();
	int dataSizePerPacket = PBUF_SIZE_USABLE - 4 * 4; 
	
	int lastPacketSize = 0;
	int numDataPackages = 0;
	int actStringPosition = 0;
	int actDataPerPackage = 0;
	
	if(mDebug) {
		std::cout << "\nStandardYarsClient: doNewSim()...sendXML()\n";
		std::cout << "TotalDataSize: " << totalDataSize << ", dataSizePerPacket: " << dataSizePerPacket << "\n";
	}	
	if(dataString == NULL || totalDataSize == 0)
	{
		std::cerr << "Cannot process empty string - not sending anything.";
		return false;
	}
	else if(mDebug)
	{
		//std::cout << "\n\n ###Here comes the string to be transferred: \n\n" << (*dataString).toStdString().c_str();
	}
	
	lastPacketSize  = totalDataSize % dataSizePerPacket;
	if(mDebug) std::cout << "lastPacketSize: " << lastPacketSize << "\n";
	if (lastPacketSize > 0)
	{
		numDataPackages = totalDataSize / dataSizePerPacket + 1;
	}
	else
	{
		numDataPackages = totalDataSize / dataSizePerPacket;
	}
	if(mDebug) std::cout << "numDataPackages: " << numDataPackages << "\n";
	// send newSim request
	mSendBuffer->clear();
	mSendBuffer->writeInt(_STANDARD_COM_ID_NEW_SIM_REQ);
	mSendBuffer->writeInt(dataID);
	mSendBuffer->writeInt(totalDataSize);
	mSendBuffer->writeInt(dataSizePerPacket);
	mSendBuffer->writeInt(sourceType);
	mSendBuffer->writeInt(selectionMask);
	mSendBuffer->writeInt(tryNumber);
	mSendBuffer->writeInt(indyNumber);
	mSendBuffer->writeInt(_STANDARD_COMEND);
	
	if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
		std::cerr << "Unable to send newSim() request\n";
		Core::log("YarsComSimulationAlgorithm: sendXML(): Unable to send newSim request");
		return false;
	}
	mSendBuffer->clear();

	// check if first package is acknowledged correctly
	if(!checkSimpleAcknowledge(_STANDARD_COM_ID_NEW_SIM_ACK, 
		QString("NEW_SIM_ACK")))
	{
	return false;
	}
	
	std::cout << "\n\nTransmitting XML description to YARS-server. This may take a while..." << "\n";
	// send string data in (multiple) packets
	for(int i = 0; i < numDataPackages; i++)
	{
		if((i != 1) && ((i % _MAX_NUM_PACKETS_WITHOUT_ACK) == 1))
		{
			if(!checkSimpleAcknowledge(_STANDARD_COM_ID_NEW_SIM_ACK, QString("NEW_SIM_ACK")))
			{
				std::cerr << "Correct acknowledgement after MAX_NUM_PACKETS_WITHOUT_ACK data packets failed\n";
				Core::log("YarsComSimulationAlgorithm: sendXML(): Correct acknowledgement of MAX_NUM_PACKETS_WITHOUT_ACK string data packets failed");
			}
		}
		if(mDebug) std::cout << "looping through data Packet: " << (i + 1) << "\n";
		mSendBuffer->clear();
		mSendBuffer->writeInt(_STANDARD_COM_ID_NEW_SIM_REQ);
		mSendBuffer->writeInt(dataID);
		mSendBuffer->writeInt(i);
		if(mDebug) std::cout << "new sim request sent\n";
		if(i < (numDataPackages - 1) || lastPacketSize == 0)
		{
			actDataPerPackage = dataSizePerPacket;
		}
		else if(i == (numDataPackages - 1))
		{
			actDataPerPackage = lastPacketSize;
		}
		if(mDebug) std::cout << "adding data of size " << actDataPerPackage << " to package\n";
		for(int j=0; j<actDataPerPackage; j++)
		{
			mSendBuffer->writeByte((*dataString).toStdString()[actStringPosition++]);
		}
		if(mDebug) std::cout << "standard comend\n";
		mSendBuffer->writeInt(_STANDARD_COMEND);
		if(mDebug) std::cout << "transmitting datagram of package " << (i + 1) << "\n";
		if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
			std::cerr << "Unable to send string data packets\n";
			Core::log("YarsComSimulationAlgorithm: sendXML(): Unable to send string data packets");
			return false;
		}
		mSendBuffer->clear();
	}
	if(mDebug) std::cout << "done sending XML, checking for new SIM ACK\n";	

	// check if all data packages are acknowledged correctly
	if(!checkSimpleAcknowledge(_STANDARD_COM_ID_NEW_SIM_ACK, QString("NEW_SIM_ACK")))
	{
		std::cerr << "Correct acknowledgement of all string data packets failed\n";
		Core::log("YarsComSimulationAlgorithm: sendXML(): Correct acknowledgement of all string data packets failed");
		return false;
	}
	if(mDebug) std::cout << "exitingSendXML()\n";
	std::cout << "Done transmitting XML description.\n\n";
	return true;
}


bool YarsComSimulationAlgorithm::clearSim()
{
	if(mDebug) std::cout << "StandardYarsClient: doClearSim()\n";
	
	// send clearSim request
	mSendBuffer->clear();
	mSendBuffer->writeInt(_STANDARD_COM_ID_CLEAR_SIM_REQ);
	mSendBuffer->writeInt(_STANDARD_COMEND);
	if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
		std::cerr << "Unable to send clearSim() request\n";
		Core::log("YarsComSimulationAlgorithm: clearSim(): Unable to send clearSim request");
		return false;
	}
	mSendBuffer->clear();
	
	// check if successful
	if(!checkSimpleAcknowledge(_STANDARD_COM_ID_CLEAR_SIM_ACK, 
		QString("CLEAR_SIM_ACK")))
	{
		return false;
	}
	
	return true;
}


bool YarsComSimulationAlgorithm::checkSimpleAcknowledge(int ack, QString ackName)
{
	if(mDebug) std::cout << "        checkSimpleAck() for " << ackName.toStdString().c_str() << "\n";
	int tmp = 0;
	// check if first cmd package successful
	mRecBuffer->clear();
	if(!readNextDatagram()) {
		std::cerr << "YarsClient: error while receiving YARS datagram\n";
		Core::log("YarsComSimulationAlgorithm: checkSimpleAcknowledge(): Error while receiving YARS datagram");
		return false;
	}

	tmp = mRecBuffer->readNextInt();
	if (tmp != ack)
	{
		std::cerr << "YarsClient: received: " << tmp << " instead of " << ack << "\n";
		Core::log(QString("YarsComSimulationAlgorithm: checkSimpleAcknowledge(): Error while checking acknowledgement for ")+ackName);
		
		return false;
	}
	if(mDebug) std::cout << "received "<<tmp<<" as expected\n"; 
	
	tmp = mRecBuffer->readNextInt();
	if (tmp != _STANDARD_COMEND)
	{
		std::cerr << "StandardYarsClient: no COMEND for " << ackName.toStdString().c_str() << " received ... \n";
		Core::log(QString("YarsComSimulationAlgorithm: checkSimpleAcknowledge(): No COMEND received for ")+ackName);
		
		return false;
	}
	mRecBuffer->clear();	
	if(mDebug) std::cout << "received " << tmp << " as expected\n";
	return true;
}


// convenience Method to read Datagram from mSocket into mRecBuffer
bool YarsComSimulationAlgorithm::readNextDatagram() {
	while(!mSocket->hasPendingDatagrams() && !mShutDown) {
		Core::getInstance()->executePendingTasks();
		mSocket->waitForReadyRead(25);
	}

	if(mShutDown) {
		return true;
	}

	QByteArray temp(mSocket->pendingDatagramSize(),0);
	quint16 port = mPort->get();
	if(mSocket->readDatagram(temp.data(), temp.size(), mAddress, &port) < 0) {
		mRecBuffer->clear();
		return false;
	}

	mRecBuffer->setData(temp);

	return true;
	
}


void YarsComSimulationAlgorithm::datagramReceptionErrorOutput(){
	std::cerr << "YarsClient: error while receiving YARS datagram, disconnecting...\n";
	Core::log("YarsComSimulationAlgorithm: checkForConnection(): Error while receiving YARS datagram");
}


/**
 * Updates the agent control interface (used e.g. by the neural network)
 * with the current InputValues, OutputValues and InfoValues.
 * The old InterfaceValues are deleted.
 * After the exchange of the InterfaceValues the simulation is reset to force a 
 * reconnection of the InterfaceValues.
 */
void YarsComSimulationAlgorithm::updateAgentInterface() {

	//collect old InterfaceValues for later destruction.
	QList<InterfaceValue*> oldValues = mAgentObject->getInputValues();
	oldValues << mAgentObject->getInfoValues();
	oldValues << mAgentObject->getOutputValues();

	QList<Value*> valuesToRemove;
	for(QListIterator<InterfaceValue*> i(oldValues); i.hasNext();) {
		valuesToRemove.append(i.next());
	}

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->removeValues(valuesToRemove);
	vm->notifyRepositoryChangedListeners();

	//set new InterfaceValues
	mAgentObject->setInputValues(mInputValues);
	mAgentObject->setInfoValues(mInfoValues);
	mAgentObject->setOutputValues(mOutputValues);

	for(QListIterator<InterfaceValue*> i(mInputValues); i.hasNext();) {
		InterfaceValue *value = i.next();
		vm->addValue(QString("/Yars/Input").append(value->getName()), value);
	}
	for(QListIterator<InterfaceValue*> i(mOutputValues); i.hasNext();) {
		InterfaceValue *value = i.next();
		vm->addValue(QString("/Yars/Output").append(value->getName()), value);
	}
	for(QListIterator<InterfaceValue*> i(mInfoValues); i.hasNext();) {
		InterfaceValue *value = i.next();
		vm->addValue(QString("/Yars/Aux").append(value->getName()), value);
	}
	vm->notifyRepositoryChangedListeners();

	//make sure that the networks are reconnected and dependencies to old InterfaceValues are released.
	Event *event = Physics::getPhysicsManager()->getPhysicsEnvironmentChangedEvent();
	event->trigger();



	//destroy old InterfaceValues.
	for(QListIterator<InterfaceValue*> i(mInputValues); i.hasNext();) {
		oldValues.removeAll(i.next());
	}
	for(QListIterator<InterfaceValue*> i(mInfoValues); i.hasNext();) {
		oldValues.removeAll(i.next());
	}
	for(QListIterator<InterfaceValue*> i(mOutputValues); i.hasNext();) {
		oldValues.removeAll(i.next());
	}
	while(!oldValues.empty()) {
		InterfaceValue *value = oldValues.at(0);
		oldValues.removeAll(value);
		delete value;
	}
}


/**
 * Cancels a connection to Yars if one is available.
 */
void YarsComSimulationAlgorithm::disconnectFromYars() {
	//Send disconnection command to yars. Then stop communication.
 	if(mDebug) std::cout << "\nYarsClient: disconnectFromYars()\n";

	// send quit request
	mSendBuffer->clear();
	mSendBuffer->writeInt(_STANDARD_COM_ID_QUIT_REQ);
	mSendBuffer->writeInt(_STANDARD_COMEND);
	if(mSocket->writeDatagram(mSendBuffer->getData(), *mAddress, mPort->get()) < 0) {
		std::cerr << "Unable to send termination request to Yars\n";
		Core::log("YarsComSimulationAlgorithm: disconnectFromYars(): Unable to send termination request to Yars");
	}
	mSendBuffer->clear();
	
// 	QTime time;
// 	time.start();
// 	bool foundAck = false;
//
// 	while(time.elapsed() < 2000 && !foundAck) {
// 		if(mSocket->hasPendingDatagrams()) {
// 			foundAck = checkSimpleAcknowledge(_STANDARD_COM_ID_QUIT_ACK, QString("QUIT_ACK"), false);
// 		}
// 		else {
// 			mSocket->waitForReadyRead(25);
// 		}
// 	}
// 	
// 	// check if successful
// 	if(!foundAck)
// 	{
// 		std::cerr << "No termination acknowledgement received from Yars\n";
// 		Core::log("YarsComSimulationAlgorithm: disconnectFromYars(): No termination acknowledgement received from Yars");			
// 	}

	mConnected = false;
}

}



