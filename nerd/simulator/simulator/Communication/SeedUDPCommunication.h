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


#ifndef NERDSeedUDPCommunication_H
#define NERDSeedUDPCommunication_H

#include <QUdpSocket>
#include <QObject>
#include <QList>
#include <QHash>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include "Event/Event.h"
#include "Value/IntValue.h"
#include "Communication/SeedUDPClientHandler.h"
#include "Physics/SimObjectGroup.h"
#include "Value/StringValue.h"
#include "Core/SystemObject.h"


namespace nerd {

	/**
	 * SeedUDPCommunication
	 */
	class SeedUDPCommunication : public QThread, public virtual SystemObject {
	Q_OBJECT

	public:
		static const unsigned char UDP_INIT_COMMUNICATION = 5;
		static const unsigned char UDP_INIT_COMMUNICATION_ACK = 6;

		static const unsigned char UDP_END_COMMUNICATION = 7;
		static const unsigned char UDP_END_COMMUNICATION_ACK = 8;

		static const unsigned char UDP_RESET_COMMUNICATION = 10;
		static const unsigned char UDP_RESET_COMMUNICATION_ACK = 11;

		static const unsigned char UDP_REGISTER_FOR_EVENT = 20;
		static const unsigned char UDP_REGISTER_FOR_EVENT_ACK = 21;
		static const unsigned char UDP_DEREGISTER_FROM_EVENT = 30;
		static const unsigned char UDP_DEREGISTER_FROM_EVENT_ACK = 31;

		static const unsigned char UDP_GET_VALUE_IDS = 40;
		static const unsigned char UDP_VALUE_IDS = 41;
		static const unsigned char UDP_VALUE_INFO_ACK = 42;
		static const unsigned char UDP_VALUE_INFO = 43;
		static const unsigned char UDP_GET_VALUE = 50;
		static const unsigned char UDP_VALUE = 51;
		static const unsigned char UDP_SET_VALUE = 52;
		static const unsigned char UDP_SET_VALUE_ACK = 53;
		static const unsigned char UDP_REGISTER_FOR_VALUE = 54;
		static const unsigned char UDP_REGISTER_FOR_VALUE_ACK = 55;
		static const unsigned char UDP_DEREGISTER_FROM_VALUE = 56;
		static const unsigned char UDP_DEREGISTER_FROM_VALUE_ACK = 57;

		static const unsigned char UDP_RESET_SIMULATION = 70;
		static const unsigned char UDP_RESET_SIMULATION_ACK = 71;
		static const unsigned char UDP_RESET_SIMULATION_COMPLETED = 75;
		static const unsigned char UDP_RESET_SIMULATION_COMPLETED_ACK = 76;
		static const unsigned char UDP_NEXT_SIMULATION_STEP = 80;
		static const unsigned char UDP_NEXT_SIMULATION_STEP_ACK = 81;
		static const unsigned char UDP_NEXT_SIMULATION_STEP_COMPLETED = 85;
		static const unsigned char UDP_NEXT_SIMULATION_STEP_COMPLETED_ACK = 86;

		static const unsigned char UDP_GET_AGENT_OVERVIEW = 90;
		static const unsigned char UDP_AGENT_OVERVIEW = 91;
		static const unsigned char UDP_AGENT_OVERVIEW_NEXT = 92;
		static const unsigned char UDP_AGENT_OVERVIEW_ACK = 93;
		static const unsigned char UDP_GET_AGENT_INFO = 95;
		static const unsigned char UDP_AGENT_INFO = 96;
		static const unsigned char UDP_AGENT_INFO_ACK = 97;
		static const unsigned char UDP_AGENT_INFO_NEXT = 98;
		static const unsigned char UDP_REGISTER_FOR_AGENT = 100;
		static const unsigned char UDP_REGISTER_FOR_AGENT_ACK = 101;
		static const unsigned char UDP_DEREGISTER_FROM_AGENT = 105;
		static const unsigned char UDP_DEREGISTER_FROM_AGENT_ACK = 106;

		static const unsigned char UDP_DATAGRAM_END = 120;


	public:
		SeedUDPCommunication();
		virtual ~SeedUDPCommunication();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		virtual QString getName() const;

		virtual void run();

		void startListening(QThread *owner);
		bool startUdpServer();
		void stopUdpServer();
		bool isUdpServerRunning();

		void setPort(int port);

		virtual void delayThread();

		virtual void waitForNextDatagram();

		void demandCommunicationReset();
		void demandSimulationReset(SeedUDPClientHandler *handler, int seed);
		void demandNextSimulationStep(SeedUDPClientHandler *handler);
		void terminateCliendHandler(SeedUDPClientHandler *handler);

		bool registerGroupController(SimObjectGroup *groupToControl,
																SeedUDPClientHandler *controller);
		bool deregisterGroupController(SimObjectGroup *groupToControl,
																SeedUDPClientHandler *controller);
		bool isGroupControlled(SimObjectGroup *group);

	public slots:
		void datagramReceived();

	protected:
		virtual void resetCommunication();
		virtual void resetSimulation();
		virtual void executeNextStep();

	private:
		Event *mResetEvent;
		Event *mResetFinalizedEvent;
		Event *mNextStepEvent;
		Event *mStepCompletedEvent;

		QUdpSocket *mUdpServerSocket;
		IntValue *mPortValue;
		QHostAddress *mLastReceivedHostAddress;
		quint16 mLastReceivedPort;

		QList<SeedUDPClientHandler*> mClientHandlers;
		QList<SeedUDPClientHandler*> mHandlersToTerminate;
		int mResetRequestCounter;
		int mNextStepRequestCounter;
		bool mResetCommunicationRequest;

		QHash<SimObjectGroup*, SeedUDPClientHandler*> mControlledObjectGroups;
		QMutex mGroupControllerMutex;

		QMutex mReadMutex;
		QMutex mSocketMutex;
		QWaitCondition mReadWaitCondition;
		QList<QByteArray> mDatagrams;
		UdpDatagram mReadDatagram;

		bool mRunServerLoop;
		bool mServerRunning;

		int mSeed;

        QThread *mOwnerThread;

	};

}

#endif


