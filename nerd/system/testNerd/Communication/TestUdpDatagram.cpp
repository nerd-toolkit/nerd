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


#include "TestUdpDatagram.h"
#include "Core/Core.h"
#include "Communication/UdpDatagram.h"
#include <QByteArray>
#include <iostream>
#include "Math/Math.h"

using namespace std;

using namespace nerd;

//chris
void TestUdpDatagram::testConstructor() {
	Core::resetCore();
	
	UdpDatagram ud;
	
	QCOMPARE(ud.maxUnreadBytes(), 0);
	QCOMPARE(ud.getData().size(), 0);
	QCOMPARE(ud.getDataSize(), 0);
	
	char bytes[] = {(char) 10, (char) 255, (char) 128, (char) 0};
	QByteArray data(bytes, 4);
	
	ud.setData(data);
	QCOMPARE(ud.maxUnreadBytes(), 4);
	QCOMPARE(ud.getData().size(), 4);
	QCOMPARE(ud.getDataSize(), 4);
	
	QCOMPARE((int) ud.readNextByte(), (int) 10);
	QCOMPARE(ud.maxUnreadBytes(), 3);
	QCOMPARE(ud.getData().size(), 4);
	QCOMPARE(ud.getDataSize(), 4);

	ud.clear();
	QCOMPARE(ud.maxUnreadBytes(), 0);
	QCOMPARE(ud.getData().size(), 0);
	QCOMPARE(ud.getDataSize(), 0);
	

}


//chris
void TestUdpDatagram::testReadAndWrite() {
	Core::resetCore();
	
	UdpDatagram ud;
	
	QCOMPARE(ud.getData().size(), 0);
	QCOMPARE(ud.getDataSize(), 0);
	QCOMPARE(ud.maxUnreadBytes(), 0);
	
	ud.writeInt(64000);
	QCOMPARE(ud.getData().size(), 4);
	QCOMPARE(ud.getDataSize(), 4);
	QCOMPARE(ud.maxUnreadBytes(), 4);
	
	ud.writeByte(128);
	QCOMPARE(ud.getData().size(), 5);
	QCOMPARE(ud.getDataSize(), 5);
	QCOMPARE(ud.maxUnreadBytes(), 5);
	
	ud.writeFloat(50.019238);
	QCOMPARE(ud.getData().size(), 9);
	QCOMPARE(ud.getDataSize(), 9);
	QCOMPARE(ud.maxUnreadBytes(), 9);
	
	ud.writeString("Hello");
	QCOMPARE(ud.getData().size(), 15);
	QCOMPARE(ud.getDataSize(), 15);
	QCOMPARE(ud.maxUnreadBytes(), 15);
	
	//check if the entire byte array contains the correct bytes
	char desiredBytes[] = {0,-6,0,0,-128,-77,19,72,66,72,97,108,108,111,0};
	
	QVERIFY(ud.getData().startsWith(QByteArray(desiredBytes)) == true);
	
	QCOMPARE(ud.readNextInt(), 64000);
	QCOMPARE(ud.getData().size(), 15);
	QCOMPARE(ud.getDataSize(), 15);
	QCOMPARE(ud.maxUnreadBytes(), 11);
	
	QCOMPARE((int) ud.readNextByte(), (int) 128);
	QCOMPARE(ud.getData().size(), 15);
	QCOMPARE(ud.getDataSize(), 15);
	QCOMPARE(ud.maxUnreadBytes(), 10);
	
	double readFloat = (double) ud.readNextFloat();
	QVERIFY(readFloat != (double) 50.019238); //because accuracy is less (4 bytes only)
	QVERIFY(Math::compareDoubles(readFloat, (double) 50.019238, 6, false));
	QCOMPARE(ud.getData().size(), 15);
	QCOMPARE(ud.getDataSize(), 15);
	QCOMPARE(ud.maxUnreadBytes(), 6);
	
	ud.writeInt(1234567);
	QCOMPARE(ud.getData().size(), 19);
	QCOMPARE(ud.getDataSize(), 19);
	QCOMPARE(ud.maxUnreadBytes(), 10);
	
	QString message = ud.readNextString();
	QVERIFY(message.compare("Hello") == 0);
	QCOMPARE(ud.getData().size(), 19);
	QCOMPARE(ud.getDataSize(), 19);
	QCOMPARE(ud.maxUnreadBytes(), 4);
	
	QCOMPARE(ud.readNextInt(), 1234567);
	QCOMPARE(ud.getData().size(), 19);
	QCOMPARE(ud.getDataSize(), 19);
	QCOMPARE(ud.maxUnreadBytes(), 0);
	
	//behavior if no value can be read.
	QCOMPARE(ud.readNextInt(), 0);
	QCOMPARE(ud.getData().size(), 19);
	QCOMPARE(ud.getDataSize(), 19);
	QCOMPARE(ud.maxUnreadBytes(), 0);
	
	QCOMPARE((double) ud.readNextFloat(), (double) 0.0);
	QCOMPARE(ud.getData().size(), 19);
	QCOMPARE(ud.getDataSize(), 19);
	QCOMPARE(ud.maxUnreadBytes(), 0);
	
	QCOMPARE((int) ud.readNextByte(), (int) 0);
	QCOMPARE(ud.getData().size(), 19);
	QCOMPARE(ud.getDataSize(), 19);
	QCOMPARE(ud.maxUnreadBytes(), 0);
	
	QVERIFY(ud.readNextString().compare("") == 0);
	QCOMPARE(ud.getData().size(), 19);
	QCOMPARE(ud.getDataSize(), 19);
	QCOMPARE(ud.maxUnreadBytes(), 0);
	
	ud.writeString("Hello World");
	QCOMPARE(ud.getData().size(), 31);
	QCOMPARE(ud.getDataSize(), 31);
	QCOMPARE(ud.maxUnreadBytes(), 12);
	
	ud.clear();
	
	QCOMPARE(ud.getData().size(), 0);
	QCOMPARE(ud.getDataSize(), 0);
	QCOMPARE(ud.maxUnreadBytes(), 0);

}





