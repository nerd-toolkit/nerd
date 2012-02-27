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



#ifndef CORE_H_
#define CORE_H_


#include <QMutex>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include "Core/Properties.h"
#include "Core/Task.h"
#include <signal.h>


namespace nerd {

class ValueManager;
class EventManager;
class PhysicsManager;
class CollisionManager;
class PlugInManager;
class SimulationEnvironmentManager;
class Event;
class IntValue;
class BoolValue;
class StringValue;
class SystemObject;

/**
 * Core.
 * The core provides a globally available static instance
 * which represents the entire simulation system.
 *
 * The core provides a number of standard managers, like the
 * ValueManager, the EventManager, the PhysicsManager of the
 * CollisionManager, and corresponding getter methods for these
 * managers. Additionally any other object (must be a subclass of
 * SystemObject) can be registered as global object and associated
 * with a unique name. Such objects then can be accessed given the
 * global name of the object. This way custom managers can be installed
 * to the Core at runtime.
 *
 * The core also is responsible for the initialization and shutdown
 * of the simulatior system. Objects of SystemObject subclasses can be
 * added to the Core with addSystemObject() to get considered in the
 * initialization and shutdown process.
 *
 * During initialization all objects
 * can acquire resources and other objects, as this phase usually is
 * initiated when most of the simulatior objects have been created.
 *
 * After the initialization phase method bind() is called for all
 * registered SystemObjects. This phase can be used to register for
 * Value changes at the ValueManager or to registere for Event notifications
 * at the EventManager. All required Events, Values and
 * otherwise required objects have to be created before this phase is
 * entered, preferebly during object construction or in the initialization
 * phase.
 *
 * Whenever the simulator is terminated, the Core calls the cleanUp() methods
 * of all SystemObjects. During this phase all objects can savely deregister
 * from other objects and release system resources. This is save, because all
 * objects are still available in the simulation system.
 *
 * When the Core is destroyed it autoamtically destroys all registered
 * SystemObjects and thus can be used for memory management. Whenever
 * an Object has to be created on the HEAP without having an object which
 * is responsible for its destruction, then this object can be registered
 * as SystemObject and is automatically destroyed.
 *
 *
 * The Core maintains a Property list that can be accessed by any object to
 * store or read (shared) configurations. The core will automatically store
 * the Property list at shutdown and reload it at the next Core creation.
 * Therefore the Property list can be used to store configurations
 * between application runs, which is useful e.g. to store the positons of
 * GUI windows, the current working directory of the file choosers, and much more.
 */
class Core {

	public:
		~Core();

		static Core* getInstance();
		static void resetCore();

		static void log(const QString &message, bool copyToCout = false);

		EventManager* getEventManager() const;
		ValueManager* getValueManager() const;
		PlugInManager* getPlugInManager() const;


		bool bindSystemObjects();
		bool init();
		bool isInitialized() const;
		bool shutDown();

		bool isShuttingDown() const;

		bool addSystemObject(SystemObject *object);
		bool removeSystemObject(SystemObject *object);
		QList<SystemObject*> getSystemObjects() const;
		bool addGlobalObject(const QString &objectName, SystemObject *object);
		SystemObject* removeGlobalObject(const QString &objectName);
		SystemObject* getGlobalObject(const QString &name) const;
		QList<SystemObject*> getGlobalObjects() const;
		QList<QString> getGlobalObjectNames() const;

		bool scheduleTask(Task *task);
		QList<Task*> getPendingTasks() const;
		void clearPendingTasks();
		void executePendingTasks();

		void logMessage(const QString &message);

		Event* getInitEvent() const;
		Event* getInitCompletedEvent() const;
		Event* getBindEvent() const;
		Event* getShutDownEvent() const;

		bool isPerformanceMeasuringEnabled() const;

		Properties& getProperties();
		QString getConfigDirectoryPath() const;
		bool enforceDirectoryPath(const QString &path);

		void setMainExecutionThread();
		void clearMainExecutionThread();
		bool isMainExecutionThread() const;

		bool registerThread(QThread *thread);
		bool deregisterThread(QThread *thread);
		void waitForAllThreadsToComplete();

		static QString getVersionString();

		bool isUsingReducedFileWriting() const;

	protected:
		Core();

	private:
		void setUpCore();

	private:
		static bool mCoreCreated;
		static Core *sInstance;
		static QMutex mMutex;
		static int sCurrentInstanceId;
		static bool sVerbose;

		QMutex mTaskLocker;
		QThread *mMainExecutionThread;

		bool mInitializedSuccessful;

		ValueManager *mValueManager;
		EventManager *mEventManager;
		PlugInManager *mPlugInManager;

		QList<SystemObject*> mSystemObjects;
		QMap<QString, SystemObject*> mGlobalObjects;
		QList<Task*> mScheduledTasks;

		IntValue *mSimulationDelay;
		IntValue *mInitializationDuration;
		IntValue *mBindingDuration;

		StringValue *mCurrentLogMessage;
		QFile *mLogFile;
		QTextStream *mLogFileStream;

		BoolValue *mRunInPerformanceMode;
		BoolValue *mEnablePerformanceMeasures;

		Event *mInitEvent;
		Event *mInitCompletedEvent;
		Event *mBindEvent;
		Event *mShutDownEvent;
		Event *mTasksExecutedEvent;

		QString mConfigDirectoryName;

		Properties mProperties;
		QList<QThread*> mAvailableThreads;
		bool mShutDownCompleted;
		bool mIsShuttingDown;
		bool mIsUsingReducedFileWriting;
};


class ShutDownTask : public Task {
	public:
		ShutDownTask() {
			mShutdownExecuted = false;
			if(Core::getInstance()->isMainExecutionThread()) {
				//if this is the main execution thread, don't schedule. Execute immediately.
				runTask();
			}
			else {	
				mShutdownExecuted = false;
			}
		}

		virtual ~ShutDownTask() {};

		virtual bool runTask() {
			if(!mShutdownExecuted) {
				Core::getInstance()->shutDown();
				mShutdownExecuted = true;
			}
			return true;
		}

	private:
		bool mShutdownExecuted;
};





}
#endif /*SIMULATOR_H_*/
