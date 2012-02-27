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

#include "FileLocker.h"
#include <QThread>
#include "Core/Core.h"
#include <QCoreApplication>

#ifdef linux
#include <sys/file.h>
#include <errno.h>
#endif

namespace nerd {

FileLocker::FileLocker() {
}

FileLocker::~FileLocker() {
}

/**
 * Locks a given file exclusively. If the file is already locked by another process, the function waits for a given time and tries it again. This is repeated for a given number of trials.
 *
 * @param file The file to lock.
 * @param maxTrials The number of trials.
 * @param timeToWaitBetweenTrials The time to wait between the trials.
 * @return Returns true if the lock was successful, otherwise false.
 */
bool FileLocker::lockFileExclusively(const QFile& file, int maxTrials, unsigned long timeToWaitBetweenTrials) {
    #ifdef _WIN32
    return true;
    #endif
    #ifdef linux
	return lockFile(file, LOCK_EX, maxTrials, timeToWaitBetweenTrials);
    #endif
}

/**
 * Locks a given file shared. If the file is already locked by another process, the function waits for a given time and tries it again. This is repeated for a given number of trials.
 *
 * @param file The file to lock.
 * @param maxTrials The number of trials.
 * @param timeToWaitBetweenTrials The time to wait between the trials.
 * @return Returns true if the lock was successful, otherwise false.
 */
bool FileLocker::lockFileShared(const QFile& file, int maxTrials, unsigned long timeToWaitBetweenTrials) {
    #ifdef _WIN32
    return true;
    #endif
    #ifdef linux
	return lockFile(file, LOCK_SH, maxTrials, timeToWaitBetweenTrials);
	#endif
}

/**
 * Locks a given file exclusively or shared. If the file is already locked by another process, the function waits for a given time and tries it again. This is repeated for a given number of trials.
 *
 * @param file The file to lock.
 * @param flag The flag which decides whether the file should be locked exclusively (LOCK_EX) or shared (LOCK_SH).
 * @param maxTrials The number of trials.
 * @param timeToWaitBetweenTrials The time to wait between the trials.
 * @return Returns true if the lock was successful, otherwise false.
 */
bool FileLocker::lockFile(const QFile& file, int flag, int maxTrials, unsigned long timeToWaitBetweenTrials) {
    #ifdef _WIN32
    return true;
    #endif
    #ifdef linux
	int numLockTrials = 0;
	bool locked = false;
	// While not successfully locked and numLockTrials not reached maxTrials
	while(!locked && (numLockTrials < maxTrials)) {
		// Try to lock the file exclusively or shared and non-blocking
		if(!flock(file.handle(), flag | LOCK_NB)) {
			// If successfull, return true
			locked = true;
		} else {
			if(errno == EWOULDBLOCK) {
				// If file is blocked by another process, wait for timeToWait ms
				QCoreApplication::instance()->thread()->wait(timeToWaitBetweenTrials);
			} else {
				// If some other error occured, log an error message and return false
				Core::log(QString("FileLocker::lockFile : %1").arg(strerror(errno)));
				return false;
			}
			numLockTrials++;
		}
	}
	return locked;
    #endif
}

/**
 * Unlocks a given file.
 *
 * @param file The file to unlock.
 * @return Return true if unlocking was successful, otherwise false.
 */
bool FileLocker::unlockFile(const QFile& file) {
    #ifdef _WIN32
    return true;
    #endif
    #ifdef linux
	if(flock(file.handle(), LOCK_UN)) {
		Core::log(QString("FileLocker::unlockFile : %1").arg(strerror(errno)));
		return false;
	}
	return true;
	#endif
}

}
