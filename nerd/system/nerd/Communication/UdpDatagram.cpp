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

#include "UdpDatagram.h"
#include <iostream>

using namespace std;

namespace nerd {


/**
 * Constructor.
 */
UdpDatagram::UdpDatagram() : mDataReadPosition(0) {
}

/**
 * Destructor.
 */
UdpDatagram::~UdpDatagram() {
}


/**
 * Writes the four least significant bytes of an integer to the internal
 * QByteArray, using little endian format.
 *
 * @param value the integer to write.
 */
void UdpDatagram::writeInt(int value) {
	if(mData.size() + 4 < MAX_DATA_SIZE - 1) {
		mData.push_back((char)  value);
		mData.push_back((char) (value >> 8));
		mData.push_back((char) (value >> 16));
		mData.push_back((char) (value >> 24));
	}
}


/**
 * Writes the four least significant bytes of a float to the internal
 * QByteArray, using little endian format.
 *
 * @param value the value to write.
 */
void UdpDatagram::writeFloat(float value) {
	//int *i = (int*) &value;
	int i = 0; 
	char *c = (char*) &value;
	char *ic = (char*) &i;
	ic[0] = c[0];
	ic[1] = c[1];
	ic[2] = c[2];
	ic[3] = c[3];

	writeInt(i);
}


/**
 * Writes a QString to the internal QByteArray.
 * The string will automatically be finalized with a 0 char to allow one to
 * read the string directly out of the QByteArray.
 * The size of the QByteArray therefore is increased by string.size() + 1.
 *
 * @param string the string to write.
 */
void UdpDatagram::writeString(const QString &string) {
	for(int i = 0; i < string.size() && mData.size() + 2 < MAX_DATA_SIZE; ++i) {
		mData.push_back((char) string.at(i).toAscii());
	}
	mData.push_back((char) '\0');
}


/**
 * Writes a single byte to the internal ByteArray.
 *
 * @param byte the byte to write.
 */
void UdpDatagram::writeByte(unsigned char byte) {
	if(mData.size() + 2 < MAX_DATA_SIZE) {
		mData.push_back(byte);
	}
}


/**
 * Reads an integer (4 bytes) in little endian format from the
 * internal ByteArray.
 *
 * Automatically increments the reader position by 4 bytes.
 *
 * @return the read integer, or 0 if not enought bytes are available.
 */
int UdpDatagram::readNextInt() {
	if(mDataReadPosition + 4 > mData.size()) {
		return 0;
	}
	int value = (int)
		((int) 0) |	(((int) (mData[mDataReadPosition + 3]) & 0xff) << 24)
		| (((int) (mData[mDataReadPosition + 2]) & 0xff) << 16)
		| (((int) (mData[mDataReadPosition + 1]) & 0xff) << 8)
		| (((int) mData[mDataReadPosition]) & 0xff);

	mDataReadPosition += 4;
	return value;
}


/**
 * Reads a float (4 bytes) in little endian format from the
 * internal ByteArray.
 *
 * Automatically increments the reader position by 4 bytes.
 *
 * @return the read integer, or 0 if not enought bytes are available.
 */
float UdpDatagram::readNextFloat() {
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

/**
 * Reads a 0 terminated string from the internal QByteArray. If a \0 is
 * detected, all bytes read up to this position are interpreted as string
 * and returned as QString (omitting the \0). If no \0 is found, then
 * all remaining bytes are read from the QByteArray and returned as QString.
 *
 * Automatically increments the reader position by the number of read bytes.
 *
 * @return the next readable string on the internal byte array.
 */
QString UdpDatagram::readNextString() {
	char byte = '0';
	QString string;
	while(mDataReadPosition < mData.size() && byte != '\0') {
		byte = mData.at(mDataReadPosition);
		mDataReadPosition++;
		if(byte != '\0') {
			string.append(byte);
		}
	}
	return string;
}


/**
 * Reads a single byte from the internal byte array.
 *
 * Automatically increments the reader position by 1.
 *
 * @return the next byte found in the data array, or 0 if no byte is available.
 */
unsigned char UdpDatagram::readNextByte() {
	if(mDataReadPosition < mData.size()) {
		char byte = mData.at(mDataReadPosition);
		mDataReadPosition++;
		return byte;
	}
	return 0;
}


/**
 * Returns the number of bytes that have not been read with one of the read
 * messages so far.
 *
 * @param the number of unread bytes available in the internal byte array.
 */
int UdpDatagram::maxUnreadBytes() {
	return mData.size() - mDataReadPosition;
}


/**
 * Clears the entire byte array.
 */
void UdpDatagram::clear() {
	mData.clear();
	mDataReadPosition = 0;
}


/**
 * Returns the internal QByteArray. This can be used to send the data
 * with a udp socket through the network.
 */
const QByteArray& UdpDatagram::getData() const {
	return mData;
}

/**
 * Returns the size of the internal byte array. This size includes
 * all bytes, independently of whether some of the data has already
 * been read with one of the read methods.
 *
 * @return the number of bytes in the internal byte array.
 */
int UdpDatagram::getDataSize() const {
	return mData.size();
}


/**
 * Sets the internal QByteArray.
 * This method can be used to set data read from a udp socket to the
 * UdpDatagram to allow a type specific interpretation of the byte data.
 *
 * Setting the data will also reset the read position.
 *
 * @param data the new data to replace the old data with.
 */
void UdpDatagram::setData(const QByteArray &data) {
	mData = data;
	mDataReadPosition = 0;
}


}


