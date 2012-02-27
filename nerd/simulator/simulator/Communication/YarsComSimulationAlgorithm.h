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



#ifndef NERDYarsComSimulationAlgorithm_H
#define NERDYarsComSimulationAlgorithm_H
#define _STANDARD_COMPORT_SERVER              4500
#define _STANDARD_COMPORT_CLIENT              7020

#define _STANDARD_IP_SERVER             "127.0.0.1"
#define _STANDARD_COMEND                        -1
#define _STANDARD_COM_ID_PINGREQ                 0
#define _STANDARD_COM_ID_PINGACK                 1
#define _STANDARD_COM_ID_HANDSHAKEREQ           10
#define _STANDARD_COM_ID_HANDSHAKEACK           11
#define _STANDARD_COM_ID_HANDSHAKE_EXT_REQ      12
#define _STANDARD_COM_ID_HANDSHAKE_EXT_ACK      13
#define _STANDARD_COM_ID_SENSORREQ              20
#define _STANDARD_COM_ID_SENSORACK              21
#define _STANDARD_COM_ID_MOTORREQ               30
#define _STANDARD_COM_ID_MOTORACK               31
#define _STANDARD_COM_ID_DATAREQ                40
#define _STANDARD_COM_ID_DATAACK                41
#define _STANDARD_COM_ID_RESETREQ               50
#define _STANDARD_COM_ID_RESETACK               51
#define _STANDARD_COM_ID_INITREQ                60
#define _STANDARD_COM_ID_INITACK                61
#define _STANDARD_COM_ID_NEXT_TRYREQ            70
#define _STANDARD_COM_ID_NEXT_TRYACK            71

#define _STANDARD_COM_ID_PARALLEL_REQ           80
#define _STANDARD_COM_ID_PARALLEL_ACK           81
#define _STANDARD_COM_ID_SERIAL_REQ             82
#define _STANDARD_COM_ID_SERIAL_ACK             83
#define _STANDARD_COM_ID_NEW_SIM_REQ            90
#define _STANDARD_COM_ID_NEW_SIM_ACK            91
#define _STANDARD_COM_ID_NEW_SIM_FAIL           92
#define _STANDARD_COM_ID_CLEAR_SIM_REQ          93
#define _STANDARD_COM_ID_CLEAR_SIM_ACK          94
#define _STANDARD_COM_ID_CLEAR_SIM_FAIL         95
#define _STANDARD_COM_ID_QUIT_REQ              100 
#define _STANDARD_COM_ID_QUIT_ACK              101 
#define _STANDARD_COM_TAGROBOT                   0
#define _STANDARD_COM_TAGCOMPOUND              100
#define _STANDARD_COM_TAGSEGMENT               200
#define _STANDARD_COM_TAGMOTOR                 300
#define _STANDARD_COM_TAGSENSOR                400
#define _STANDARD_COM_TAGSENSOR_AUX            500
#define _STANDARD_COM_TAG_ADD_HANDSHAKE_PACKET 999

#define _MAX_SENSOR_ITEMS_PER_DATA_ACK         250
#define _MAX_MOTOR_ITEMS_PER_DATA_REQ          251
#define _MAX_NUM_PACKETS_WITHOUT_ACK            80

#define _STANDARD_ROBOT_STATE_OK                 0
#define _STANDARD_ROBOT_STATE_ABORT              1

#define _STANDARD_SOURCE_TYPE_FILE               0
#define _STANDARD_SOURCE_TYPE_XML_STRING         1

#define PBUF_SIZE_USABLE 1020

#include <QString>
#include <QHash>
#include "Collision/CollisionHandler.h"
#include "Physics/PhysicalSimulationAlgorithm.h"
#include "Value/IntValue.h"
#include "Physics/SimObjectGroup.h"
#include "Physics/InterfaceSimObject.h"
#include "Event/EventListener.h"
#include <QUdpSocket>
#include <QHostAddress>
#include "Communication/UdpDatagram.h"
#include <QDir>
#include <QFile>
#include <QTextStream>

#define MAX(a,b) (((a)>(b))?(a):(b))


namespace nerd {

/**
	* YarsComSimulationAlgorithm.
	*
	*/
class YarsComSimulationAlgorithm : public PhysicalSimulationAlgorithm,
								   public CollisionHandler,
								   public EventListener 
{
	public:
		YarsComSimulationAlgorithm();
		virtual ~YarsComSimulationAlgorithm();

		virtual bool resetPhysics();

		bool checkForReset();

		virtual bool finalizeSetup();
		virtual bool executeSimulationStep(PhysicsManager *pmanager);

		virtual QList<Contact> getContacts() const;
		virtual void disableCollisions(CollisionObject *firstCollisionPartner, 
									CollisionObject *secondCollisionPartner, 
									bool disable);
		virtual void updateCollisionHandler(CollisionManager *cManager); 

		virtual QString getName() const;
		virtual void valueChanged(Value *value);
		virtual void eventOccured(Event *event);

	private:
		void checkForConnection();
		bool handshake();
		void updateAgentInterface();
		void disconnectFromYars();
		bool receiveExtendedSensorMotorInfo(QString &name, double &min, double &max, int &tmpInt);
		bool additionalHandshakePacket(int &tmp);
		bool init(int &seed);
		bool reset(int &seed);
		bool nextTry(int &seed);
		bool sendMotorData();
		bool addMotorValueToDataReqPacket(bool full, int packetSizeLimit, int motorDataSent, 
							double value, bool &connectionError, int motorNum, int robotNum);
		bool additionalDataReqPacket(int motorDataSent, int motorNum, int robotNum);
		bool receiveSensorData();
		bool newEnvironment();
		bool sendXML(QString *dataString, int sourceType, int selectionMask, int tryNumber, int indyNumber);
		bool clearSim();
		bool checkSimpleAcknowledge(int ack, QString ackName);
		bool readNextDatagram();		
		void datagramReceptionErrorOutput();

	private:
		BoolValue *mConnectToYars;
		IntValue *mPort;
		int mClientPort;
		StringValue *mEnvironmentXML;
		bool mConnected;
		bool mShutDown;
		bool mParallelMode;
		bool mDebug;
		int mRobotStates;
		QList<InterfaceValue*> mInputValues;
		QList<InterfaceValue*> mOutputValues;
		QList<InterfaceValue*> mInfoValues;
		SimObjectGroup *mAgentInterface;
		InterfaceSimObject *mAgentObject;
		Event *mShutDownEvent;
		Event *mTerminateTryEvent;
		QUdpSocket *mSocket;
		QHostAddress *mAddress;		
		UdpDatagram *mSendBuffer;
		UdpDatagram *mRecBuffer;
		IntValue *mCurrentTry;
		IntValue *mSeedValue;
		IntValue *mGeneration;
		bool mResetRunning;
		bool mResetRequested;
		bool mSendXML;
		BoolValue *mTerminateTryAtAbortSignal;
};

}

#endif




