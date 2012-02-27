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



#ifndef NERDSimbaUDPCommunication_H
#define NERDSimbaUDPCommunication_H

#include <QUdpSocket>
#include <QObject>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include "Event/Event.h"
#include "Core/SystemObject.h"
#include "Core/ParameterizedObject.h"
#include "Value/StringValue.h"
#include "Value/IntValue.h"
#include "Value/InterfaceValue.h"
#include <QTimer>
#include <QSemaphore>
#include "Event/Event.h"
#include "Value/StringValue.h"

namespace nerd {

	/**
	 * SimbaUDPCommunication
	 */
	class SimbaUDPCommunication : public QThread, public virtual SystemObject, 
																public ParameterizedObject
	{
	
	Q_OBJECT
	
	public:
		SimbaUDPCommunication();
		virtual ~SimbaUDPCommunication();
		
		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		
		void setControlledAgent(const QString &simObjectGroupName);
		
		virtual void startListening(QThread *owner);
		virtual void stopListening();
		virtual void stopCommunication();

		virtual QString getName() const;

		void setPort(int port);

		bool waitForNextDatagram();
		void writeInt(int value);
		void writeFloat(float value);
		int maxAvailableIntegers();
		int readNextInt();
		float readNextFloat();
		void resetDatagram();
		void sendDatagram();
		
	protected:
		virtual void run();
	
	private:
		void sendHandShakeInformation();
		
	public slots:
		void datagramReceived();
		void startUDPServer();
		

	private:		
		IntValue *mCurrentStep;

		Event *mResetEvent;
		Event *mResetFinalizedEvent;
		Event *mNextStepEvent;
		Event *mStepCompletedEvent;
		
		QUdpSocket *mUdpSocket;
		StringValue *mHostAddressValue;
		IntValue *mPortValue;
		QWaitCondition mReadWaitCondition;
		QHostAddress *mLastReceivedHostAddress;
		quint16 mLastReceivedPort;
		QByteArray mData;
		int mDataPosition;
		
		QMutex mReadMutex;
		QMutex mSocketMutex;
		QSemaphore mSocketSemaphore;
		bool mRunCommunicationLoop;
		bool mCommunicationRunning;
		QTimer mTimer;
		
		static const int MAX_DATA_SIZE = 1000;
		
		QList<QByteArray> mDatagrams;

		bool mReadNextDatagram;
		
		QList<InterfaceValue*> mInputValues;
		QList<InterfaceValue*> mOutputValues;
		QThread *mOwnerThread;

		StringValue *mSimObjectGroupName;
	};

}

#endif

