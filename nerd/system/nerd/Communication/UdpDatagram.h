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

#ifndef NERDUdpDatagram_H
#define NERDUdpDatagram_H

#include <QUdpSocket>
#include <QObject>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QSemaphore>



namespace nerd {

	/**
	 * UdpDatagram.
	 *
	 * A UdpDatagram object represents the byte data of a UDP datagram, either
	 * in write or read mode. The UdpDatagram allows the user to read integers,
	 * floats, strings and bytes in little endian format from a byte array
	 * previously set with setData(), e.g by a receiver socket.
	 * A UdpDatagram can also be used to write integers, floats, strings and bytes
	 * in little endian format to a byte array, that later might be sent by a udp socket
	 *
	 * Note: The same UdpDatagram can NOT be used to read and write a datagram packet
	 * simultaneously. If you want to read and write datagrams, then use two separate
	 * UdpDatagram objects.
	 */
	class UdpDatagram {
	public:
		UdpDatagram();
		virtual ~UdpDatagram();

		void writeInt(int value);
		void writeFloat(float value);
		void writeString(const QString &string);
		void writeByte(unsigned char byte);
		int readNextInt();
		float readNextFloat();
		QString readNextString();
		unsigned char readNextByte();

		void clear();
		int maxUnreadBytes();

		const QByteArray& getData() const;
		int getDataSize() const;
		void setData(const QByteArray &data);

	private:
		QByteArray mData;
		int mDataReadPosition;


		static const int MAX_DATA_SIZE = 1024;
	};

}

#endif



