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


#ifndef NERDSeedUDPClientHandler_H
#define NERDSeedUDPClientHandler_H

#include <QList>
#include <QUdpSocket>
#include <QObject>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QSemaphore>
#include "Value/Value.h"
#include "Value/InterfaceValue.h"
#include "Event/EventListener.h"
#include <QHostAddress>
#include "Communication/UdpDatagram.h"
#include "Physics/SimObjectGroup.h"
#include "InterfaceGroup.h"
#include <iostream>

namespace nerd {

	class SeedUDPCommunication;
	class SeedUDPClientHandlerRunner;
	

	/**
	 * SeedUDPClientHandler
	 */
	class SeedUDPClientHandler : public QThread, public virtual EventListener {
	Q_OBJECT
	
	public:
		SeedUDPClientHandler(SeedUDPCommunication *owner, 
						const QHostAddress &clientAddress, quint16 clientPort);

		virtual ~SeedUDPClientHandler();
		
		virtual void runCommunication();
		virtual void stopCommunication();
		virtual bool isCommunicationRunning() const;
		
		virtual bool waitForConfirmation(unsigned char expectedHeader);
		virtual bool waitForNextDatagram();
		virtual bool waitForNextDatagram(bool interruptible);
		virtual void sendDatagram();
		
		
		
		virtual void eventOccured(Event *event);
		virtual QString getName() const;
		
		virtual bool sendConnectionInformation();
		virtual bool sendCommunicationResetMessage();
		virtual bool sendSimulationResetMessage();
		virtual bool sendStepCompletedMessage();
		
		
		virtual void reportCommunicationFailure(const QString &message);
		
	public slots:
		void datagramReceived();

	protected:
		virtual void run();

		virtual bool executeSendConnectionInformation();
		virtual bool executeSendCommunicationResetMessage();
		virtual bool executeSendSimulationResetMessage();
		virtual bool executeSendStepCompletedMessage();
		
		
		virtual bool handleEventRegistrationCommand();
		virtual bool handleEventDeregistrationCommand();
		virtual bool handleValueListCommand();
		virtual bool handleValueRegistrationCommand();
		virtual bool handleValueDeregistrationCommand();
		virtual bool handleSetValueCommand();
		virtual bool handleGetValueCommand();
		virtual bool handleAgentOverviewCommand();
		virtual bool handleAgentInfoCommand();
		virtual bool handleAgentRegistrationCommand();
		virtual bool handleAgentDeregistrationCommand();
		virtual bool handleResetSimulationCommand();
		virtual bool handleResetCommunicationCommand();
		virtual bool handleNextStepCommand();

		virtual bool handleEndCommunicationCommand();
		
		virtual bool handleUnexpectedCommand(char command);
		
		//TODO if client disconnects, the udpcom object should be notified to delete the clienthandler.

	protected:		
		SeedUDPCommunication *mOwner;
		bool mRunCommunicationLoop;
		bool mCommunicationLoopRunning;
		
		QHash<int, InterfaceGroup*> mInterfaceGroups;
		int mCountNextStepCommands;
		
		QHash<int, SimObjectGroup*> mKnownObjectGroups;
		QHash<Value*, int> mKnownValues;
		QList<Value*> mRegisteredValues;
		QHash<Event*, int> mRegisteredEvents;
		QList<Event*> mOccuredEvents;
		
		QMutex mReadMutex;
		QMutex mSocketMutex;
		QMutex mDatagramMutex;
		QMutex mSendMutex;
		QSemaphore mSocketSemaphore;
		QWaitCondition mReadWaitCondition;
		QList<QByteArray> mDatagrams;
		
		SeedUDPClientHandlerRunner *mClientRunner;
		
		QHostAddress mClientAddress;
		quint16 mClientPort;
		
		QUdpSocket *mUdpSocket;
		UdpDatagram mReadDatagram;
		UdpDatagram mWriteDatagram;
		
		int mEventIdCounter;
		int mValueIdCounter;
		int mObjectGroupCounter;

		bool mSendConnectionInfoMessage;
		bool mSendCommunicationResetInfoMessage;
		bool mSendResetCompletedMessage;
		bool mSendStepCompletedMessage;


	};
	
	
	
	
	class SeedUDPClientHandlerRunner : public QThread {
	Q_OBJECT
	
	public:
		SeedUDPClientHandlerRunner(SeedUDPClientHandler *owner) : mOwner(owner) {
		}
		
	protected:
		virtual void run() {
			mOwner->runCommunication();
		}
		
	private:
		SeedUDPClientHandler *mOwner;
	
	};

}

#endif


