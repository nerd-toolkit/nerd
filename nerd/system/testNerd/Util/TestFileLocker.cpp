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

#include "TestFileLocker.h"
#include "Core/Core.h"
#include <iostream>
#include <QFile>
#include "Util/FileLocker.h"

using namespace std;

namespace nerd{

void TestFileLocker::testLockAndUnlockFile() {
	// Create new TestFile and open it for writing and reading
	QFile file("LockTestFile");
	QVERIFY(file.open(QIODevice::ReadWrite));
	// Close it and open it again for reading, to bypass possible OpSys-locking
	file.close();
	QVERIFY(file.open(QIODevice::ReadOnly));
	// Open the file with second file handle
	QFile secondFileHandle("LockTestFile");
	QVERIFY(secondFileHandle.open(QIODevice::ReadOnly));

	// Lock the files shared
	QVERIFY(FileLocker::lockFileShared(file));
	QVERIFY(FileLocker::lockFileShared(secondFileHandle));
	// Unlock the files
	QVERIFY(FileLocker::unlockFile(file));
	QVERIFY(FileLocker::unlockFile(secondFileHandle));

	// Lock file exklusively
	QVERIFY(FileLocker::lockFileExclusively(file));
	// file cannot be locked shared now
	QVERIFY(!FileLocker::lockFileShared(secondFileHandle));
	// file can also not be locked exclusively
	QVERIFY(!FileLocker::lockFileExclusively(secondFileHandle));
	// Unlock the file
	QVERIFY(FileLocker::unlockFile(file));
	// Now the file can be locked shared
	QVERIFY(FileLocker::lockFileShared(secondFileHandle));
	// And now the file cannot be locked exclusively
	QVERIFY(!FileLocker::lockFileExclusively(file));
	// Unlock the file
	QVERIFY(FileLocker::unlockFile(secondFileHandle));

	// Close all file handles
	file.close();
	secondFileHandle.close();

	// Remove TestFile
	QVERIFY(file.remove());	
	QVERIFY(!file.exists());
}

}
