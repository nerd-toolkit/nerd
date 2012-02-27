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


#ifndef NERDSimpleUdpMotorInterface_H
#define NERDSimpleUdpMotorInterface_H


#include <QUdpSocket>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QString>
#include <QWaitCondition>
#include "Physics/SimObjectGroup.h"
#include "Value/StringValue.h"
#include "Value/IntValue.h"
#include "Core/SystemObject.h"
#include "Communication/UdpDatagram.h"
#include "Core/Core.h"
#include <iostream>
#include "Event/Event.h"

namespace nerd {

	/**
	 * SimpleUdpMotorInterface
	 */
	class SimpleUdpMotorInterface : public QThread, public virtual SystemObject,
					public virtual EventListener
	{
	Q_OBJECT

	public:
		static const unsigned char UDP_INIT_COMMUNICATION = 5;
		static const unsigned char UDP_INIT_COMMUNICATION_ACK = 6;

		static const unsigned char UDP_END_COMMUNICATION = 7;
		static const unsigned char UDP_END_COMMUNICATION_ACK = 8;

		static const unsigned char UDP_COMMAND_SET_MOTORS = 100;
		static const unsigned char UDP_COMMAND_GET_MOTORS = 150;
		static const unsigned char UDP_COMMAND_MOTOR_SETTINGS = 151;

	public:
		SimpleUdpMotorInterface(const QString &controlledGroupName);
		virtual ~SimpleUdpMotorInterface();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		virtual QString getName() const;
		virtual void eventOccured(Event *event);

		virtual void run();

		void startListening();
		bool startUdpServer();
		void stopUdpServer();
		bool isUdpServerRunning();

		IntValue* getPortValue() const;

		virtual void waitForNextDatagram();
		void sendDatagram();

	public slots:
		void datagramReceived();

	private:
		bool sendMotorData();
		bool setMotorData();
		bool sendInitAck();
		bool sendEndCommunicationAck();

	private:
		Event *mNextStepEvent;
		Event *mStepCompletedEvent;
		Event *mReceivedMotorSetMessageEvent;
        Event *mConnectEvent;
        Event *mDisconnectEvent;

		QUdpSocket *mUdpServerSocket;
		IntValue *mPortValue;
		QHostAddress *mLastReceivedHostAddress;
		quint16 mLastReceivedPort;

		QMutex mSocketMutex;
		QMutex mReadMutex;
		QMutex mMotorMutex;
		QWaitCondition mReadWaitCondition;
		QList<QByteArray> mDatagrams;
		UdpDatagram mReadDatagram;
		UdpDatagram mWriteDatagram;

		bool mRunServerLoop;
		bool mServerRunning;

		SimObjectGroup *mControlledGroup;
		StringValue *mControlledGroupName;
		QList<InterfaceValue*> mInputs;

		DoubleValue *mHeadPanDesiredAngle;
		DoubleValue *mHeadTiltDesiredAngle;
		

	};




}



#endif



