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

#include <QCoreApplication>
#include "Core.h" 
#include "Event/EventManager.h"
#include "Event/Event.h"
#include "Value/ValueManager.h" 
#include "Value/IntValue.h"
#include "Value/BoolValue.h"
#include "Value/StringValue.h"
#include "Core/Task.h"
#include "Core/Properties.h"
#include "SystemObject.h"
#include <QMutexLocker>
#include <QApplication> 
#include <QTime>
#include <QDir>
#include <QMutex>
#include <iostream>
#include "NerdConstants.h"
#include "PlugIns/PlugInManager.h"  
#include <QPluginLoader>
#include "PlugIns/PlugIn.h"
#include <QStringList>
#include <QListIterator>
#include "Util/Tracer.h"
#include "Version.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {


/*
 * Initialization of the static members.
 */
bool Core::mCoreCreated = false;
Core* Core::sInstance = 0;
QMutex Core::mMutex;
int Core::sCurrentInstanceId = 0;
bool Core::sVerbose = false;



void terminationSignalCatcher(int sig){
  	if(sig == SIGINT) {
		Core::getInstance()->scheduleTask(new ShutDownTask());
	}
}


/**
 * Constructor.
 *
 * Creates a new core with a ValueManager, an EventManager, a PlugInManager, a Properties list
 * and other useful core utilities.
 */
Core::Core()
	: mTaskLocker(QMutex::Recursive), mMainExecutionThread(0),
		mInitializedSuccessful(false),
		mValueManager(0), mEventManager(0), mPlugInManager(0), mSimulationDelay(0),
		mInitializationDuration(0), mBindingDuration(0),
		mCurrentLogMessage(0), mLogFile(0), mLogFileStream(0),
		mRunInPerformanceMode(0), mEnablePerformanceMeasures(0), mInitEvent(0), 
		mInitCompletedEvent(0), mBindEvent(0),
		mShutDownEvent(0), mTasksExecutedEvent(0), mShutDownCompleted(false),
		mIsShuttingDown(false)
{
	signal(SIGINT,terminationSignalCatcher);

	QStringList commandLineArguments = QCoreApplication::instance()->arguments();
	mIsUsingReducedFileWriting = false;
	if(commandLineArguments.contains("-disableLogging")) {
		mIsUsingReducedFileWriting = true;
	}
	if(commandLineArguments.contains("-enableLogging")) {
		mIsUsingReducedFileWriting = false;
	}
	
	mConfigDirectoryName = QDir::currentPath().append("/").append(NerdConstants::NERD_CONFIG_DIRECTORY);
	
	for(int i = 0; i < commandLineArguments.size() - 1; ++i) {
		if(commandLineArguments.at(i) == "-configDir") {
			QString confDir = commandLineArguments.at(i + 1);
			if(!confDir.startsWith("-")) {
				mConfigDirectoryName = confDir;
			}
		}
	}

	mEnablePerformanceMeasures = new BoolValue(false);

	if(!mIsUsingReducedFileWriting) {
		//setup log file.
		enforceDirectoryPath(getConfigDirectoryPath().append("/debug"));
	
		QString fileName = getConfigDirectoryPath().append("/debug/system")
						.append(QString::number(sCurrentInstanceId)).append(".log");

		mLogFile = new QFile(fileName);
		sCurrentInstanceId++;
		if(mLogFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
			mLogFileStream = new QTextStream(mLogFile);
		}
		else {
			delete mLogFile;
			mLogFile = 0;
		}
	}

	logMessage("NERD Debug Log [Version: " + getVersionString() + "]");
	logMessage(QString("~System started."));
}


/**
 * Destructor. 
 * Destroys all registered SystemObjects.
 *
 * If there are unhandled Tasks scheduled they will be destroyed without executing them.
 *
 * The EventManager, the ValueManager and the PlugInManager will be destroyed during 
 * this destructor, so all Events, Values, PlugIns and CommandLineArguments 
 * will be destroyed as well.
 *
 * Note that the destructor still uses the logMessage() method until its completion.
 */
Core::~Core() {
	TRACE("Core::~Core");

	if(sVerbose) {
		cerr << "Clearning pending tasks" << endl;
	}
	clearPendingTasks();

	if(sVerbose) {
		cerr << "Starting to destroy all [" 
			 << mSystemObjects.size() 
			 << "] SystemObjects." << endl;
	}

	//destroy all system objects.
// 	for(int i = mSystemObjects.size() - 1; i >= 0 ; --i) {
	for(int i = 0; i < mSystemObjects.size(); ++i) {
		if(sVerbose) {
			cerr << "> " << flush;
			QString message = QString("Core destroying SystemObject [")
						.append(mSystemObjects.at(i)->getName())
						.append("] with index [").append(QString::number(i))
						.append("].");
			cerr << message.toStdString().c_str() << endl;
			logMessage(message);
		}

		delete mSystemObjects.at(i);

		if(sVerbose) {
			QString message("Core succeeded destroying SystemObject.");
			cerr << message.toStdString().c_str() << endl;
			logMessage(message);
		}
	}

	if(sVerbose) {
		cerr << "All SimObjects destroyed." << endl;
	}

	mSystemObjects.clear();
	mGlobalObjects.clear();

	delete mValueManager;
	delete mEventManager;
	delete mPlugInManager;

	//Note: mCurrentLogMessage was destroyed together with the EventManager.
	mCurrentLogMessage = 0;

	if(mLogFile != 0) {
		mLogFile->close();
	}
	delete mLogFileStream;
	delete mLogFile;

	mCoreCreated = false;

}


/**
 * Returns the global instance of the Core. If no global instance is available 
 * one is created and used until the Core is destroyed or resetCore() is called.
 *
 * @return the current global instance of the Core.
 */
Core* Core::getInstance() {
	if(!mCoreCreated) {
		QMutexLocker guard(&mMutex);
		if(!mCoreCreated) {
			sInstance = new Core();
			mCoreCreated = true;
			sInstance->setUpCore();
		}
	}
	return sInstance;
}


/**
 * Resets the static instance of the Core by deleting it and preparing the static
 * instance pointer for a reinstantiation. The next call to Core::getInstance()
 * will create a new, clean Core.
 */
void Core::resetCore() {
	TRACE("Core::resetCore");
	QMutexLocker guard(&mMutex);
	if(sInstance != 0) {
		sInstance->shutDown();
 		delete sInstance;
	}
	mCoreCreated = false;
	sInstance = 0;
}




/**
 * This static method uses the logMessage() method of the global Core instance.
 * The methos is provided for convenience only.
 *
 * @param message the message to log.
 */
void Core::log(const QString &message, bool copyToCout) {
	Core::getInstance()->logMessage(message);
	if(copyToCout) {
		cout << message.toStdString().c_str() << endl;
	}
}


/**
 * Creates all required objects for the Core.
 */
void Core::setUpCore() {
	TRACE("Core::setUpCore");

	logMessage("~Setting up Core.");
	Core::log("NERD Version " + getVersionString() + ")", true);

	//Core infrastructure managers
	mEventManager = new EventManager();
	mValueManager = new ValueManager(mEventManager);
	mPlugInManager = new PlugInManager();

	mValueManager->addValue(NerdConstants::VALUE_NERD_ENABLE_PERFORMANCE_MEASUREMENTS, 
							mEnablePerformanceMeasures);

	//try to load default properties.
	if(!mIsUsingReducedFileWriting) {
		QString propertyPath = getConfigDirectoryPath().append("/properties");
		enforceDirectoryPath(propertyPath);
		QString mainPropertyFileName = propertyPath + "/main.props";
	
		CommandLineArgument *clearConfig = new CommandLineArgument("clearConfig", "clear", "", 
					"Erases all configuration files that were automatically saved during last use.", 
					0, 0, false, false);
		clearConfig->setProperty("debug");
		if(clearConfig->getNumberOfEntries() > 0) {
			//clear the properties directory before starting
			QDir dir(propertyPath);
			QStringList files = dir.entryList();
			for(QListIterator<QString> i(files); i.hasNext();) {
				QFile file(propertyPath + "/" + i.next());
				file.remove();
			}
		}
	
		if(!mProperties.loadFromFile(mainPropertyFileName)) {
			mProperties.saveToFile(mainPropertyFileName);
		}
	}

	mInitializedSuccessful = false;

	mCurrentLogMessage = new StringValue("");
	mCurrentLogMessage->setNotifyAllSetAttempts(true);
	mValueManager->addValue(NerdConstants::VALUE_NERD_RECENT_LOGGER_MESSAGE,
					mCurrentLogMessage);

	mInitializationDuration = new IntValue(0);
	mBindingDuration = new IntValue(0);
	mRunInPerformanceMode = new BoolValue(false);

	mValueManager->addValue("/ExecutionTime/CoreInit",
					mInitializationDuration);
	mValueManager->addValue("/ExecutionTime/CoreBinding",
					mBindingDuration);
	mValueManager->addValue(NerdConstants::VALUE_RUN_IN_PERFORMANCE_MODE,
					mRunInPerformanceMode);

	mShutDownEvent = mEventManager->createEvent(
			NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN,
			"Triggered BEFORE the shutdown methods\n of all SystemObjects are executed.");

	mInitEvent = mEventManager->createEvent(
			NerdConstants::EVENT_NERD_SYSTEM_INIT,
			"Triggered when the NERD initialization is completed.");

	mInitCompletedEvent = mEventManager->createEvent(
			NerdConstants::EVENT_NERD_SYSTEM_INIT_COMPLETED,
			"Triggered when the entire NERD initialization phase (including bind()) is completed.");

	mBindEvent = mEventManager->createEvent(	
			NerdConstants::EVENT_NERD_SYSTEM_BIND,
			"Triggered after the NERD binding process.");

	mTasksExecutedEvent = mEventManager->createEvent(
			NerdConstants::EVENT_NERD_EXECUTE_PENDING_TASKS,
			"Triggered when the pending tasks get executed.");

	logMessage("~Setting up Core done.");
}



/**
 * Returns the system wide EventManager.
 * 
 * @return the EventManager.
 */
EventManager* Core::getEventManager() const {
	return mEventManager;
}


/**
 * Returns the system wide ValueManager.
 * 
 * @return the ValueManager.
 */
ValueManager* Core::getValueManager() const {
	return mValueManager;
}


/** 
 * Returns the system wide PlugInManager.
 * 
 * @return the PlugInManager.
 */
PlugInManager* Core::getPlugInManager() const {
	return mPlugInManager;
}



/**
 * This method forces a (re-)binding of all SystemObjects.
 * The Core uses this method during the initialization phase, which should always be
 * the very first usage of this method. If called upon an uninitialized Core, it will
 * immediately return with false. 
 * 
 * This method can be used after the successful initialization of the Core to force
 * a rebind of all SystemObjects. This is useful if the content of the EventManager
 * or the ValueManager significantly changed and some objects might work with
 * dangling pointers to destroyed Values or Events. Such a system state should be 
 * avoided at all if possible. If there is a good reason to enter such a state, the
 * bindSystemObjects() method should be called to force all SystmeObjects to reaccess
 * Values and Events, or to replace missing Values and Events by real NULL pointers. 
 * 
 * Note that a system wide binding phase is computationally expensive, because
 * many string comparisons take place. Therefore this method should if ever not be used
 * frequently during the system execution. 
 *
 * Objects can also use the bind event to take part of the binding part of the 
 * initialization, but such objects
 * can not report a failure with a return value.
 *
 * @return true if all bindings of all SystemObjects were successful, otherwise false.
 */
bool Core::bindSystemObjects() {
	TRACE("Core::bindSystemObjects");
	logMessage("Binding SystemObjects.");

	bool ok = true;

	//Bind objects
	for(int i = 0; i < mSystemObjects.size(); i++) {
		SystemObject *obj = mSystemObjects.at(i);
		if(!obj->bind()) {
			logMessage(QString("Core: Could not bind SystemObject [")
					 .append(obj->getName()).append("]"));
			ok = false;
		}
	}
	//allow non-SystemObjects to react on the bind phase (no error detection).
	mBindEvent->trigger();

	QString statusMessage = "Binding SystemObjects ";
	if(ok) {
		statusMessage.append("was successful!");
	}
	else {
		statusMessage.append("failed!");
	}
	logMessage(statusMessage);
	return ok;
}


/**
 * Returns true if the Core has already been successfully initialized.
 *
 * @return true if the Core has been successfully initialized, otherwise false.
 */
bool Core::isInitialized() const {
	return mInitializedSuccessful;
}


/**
 * Initializes the Core. Hereby all registered SystemObjects are also initialized and bound.
 * First for all SystemObjects the init() method is called to give any SystemObject the
 * chance to reference other objects in the system, especially global objects. This can be 
 * done savely only in this phase of the Core live cycle, as the existence of other objects
 * can not be guaranteed during construction time of each object. When the Core is initialized
 * it is assumed that all relevant "persistent" objects have been created and added to the 
 * Core. 
 * After the initialization of each object, the bind() method is called for each SystemObject. 
 * During this phase all objects can register for Events or reference and start with the 
 * observation of Values in the variable repository (ValueManager). This is only save during this 
 * phase of later, because afore it can not be guaranteed that all Events and Values have 
 * been created and been published in the EventManager or ValueManager. 
 * 
 * If at least on SystemObject returned false in its init() or bind() method, the Core's init()
 * method als will return false, which usually results in a termination of of the application.
 * Hereby the Core usually is shut down regularly with shutDown(), so SystemObjects have to 
 * be prepared to be shut down without a proper init() or bind().
 *
 * Because of this drastic reaction in case of a failed initialization or binding, programmers
 * should always distingush between fatal errors during on of the phases - which means that the
 * simuator will not be able to work properly - and minor errors - which prevent a module from
 * performing its not so relevant function, e.g. showing the execution time. In the latter case
 * it is recommended that the init() or bind() method still returns true, but the module
 * handles the missing ressources, Events or Values in a suitable way, e.g. by a default 
 * behavior or by inactivating itself. This allows at least the main execution of the system.
 *
 * @return true if all initializations and bindings were successful, otherwise false.
 */
bool Core::init() {
	TRACE("Core::init");

	//temorarily make the current thread the main execution thread.
	//after initialzation this may change!
	setMainExecutionThread();

	bool ok = true;
	bool helpMessageRequest = false;
	bool quitApplication = false;
	mShutDownCompleted = false;

	QDir plugInDir(getConfigDirectoryPath().append("/plugins"));
	if(!mIsUsingReducedFileWriting) {
		logMessage("~Loading PlugIns.");
		enforceDirectoryPath(plugInDir.absolutePath());
	}
	mPlugInManager->loadPlugIns(plugInDir.absolutePath());
	
	{
		CommandLineArgument *optionalPlugInDirs = new CommandLineArgument("pluginDir", "pdir", "<directory",
						"Loads all plugins in <directory>.", 1, 0, true, true);
						
		int numberOfEntries = optionalPlugInDirs->getNumberOfEntries();
		for(int i = 0; i < numberOfEntries; ++i) {
			QStringList parameters = optionalPlugInDirs->getEntryParameters(i);
				if(parameters.size() != 1) {
					continue;
				}
				mPlugInManager->loadPlugIns(parameters[0]);
			}
	}
	
	//check version request
	CommandLineArgument *versionArgument = new CommandLineArgument("version", "", "",
					"Prints the version of this NERD application.", 0, 0, false, true);
	if(versionArgument->getNumberOfEntries() > 0) {
		cerr << "Neurodynamics and Evolutionary Robotics Development Kit (Version " 
				<< getVersionString().toStdString().c_str() << ")" << endl;
	}

	//add command line arguments for -disableLogging and -enableLogging (are handled in constructor)
	new CommandLineArgument("enableLogging", "enableLogging", "",
					"Enables the log files.", 0, 0, false, false);
	new CommandLineArgument("disableLogging", "disableLogging", "",
					"Disables the log files.", 0, 0, false, false);
	
	//add command line arguments for -configDir (is handled in constructor)
	new CommandLineArgument("configDir", "configDir", "<directory>",
					"Switches the configuration directory to the given path.", 1, 0, false, false);

	//check for application quit request
	CommandLineArgument *quitArgument = new CommandLineArgument("quit", "q", "",
					"Quits the application after initialization", 0, 0, false, false);
	quitArgument->setProperty("debug");
	if(quitArgument->getNumberOfEntries() > 0) {
		quitApplication = true;
	}

	logMessage("~Initializing Core.");

	QTime time;
	time.start();

	for(int i = 0; i< mSystemObjects.size(); i++) {
		SystemObject *obj = mSystemObjects.at(i);
		if(!obj->init()) {
			ok = false;
			logMessage(QString("Core: Could not initialize SystemObject [")
					.append(obj->getName()).append("]"));
		}
	}
	//allow non-SystemObjects to do their initialiation
	mInitEvent->trigger();

	mInitializationDuration->set(time.restart());

	//Reset RepositoryChangedCounter because only changes after the init() are relevant
	IntValue *repoCounter = 
		mValueManager->getIntValue(NerdConstants::VALUE_NERD_REPOSITORY_CHANGED_COUNTER);
	if(repoCounter != 0) {
		repoCounter->set(0);
	}

	//Execute binding of system objects.
	if(ok) {
		if(!bindSystemObjects()) {
			ok = false;
			logMessage("Core: Could not bind all SystemObjects! [TERMINATING]");
		}
	}

	mInitializedSuccessful = ok;
	if(!mInitializedSuccessful) {
		logMessage("Core: The initialization failed! The system can not be started!");
	}

	mBindingDuration->set(time.elapsed());

	mInitCompletedEvent->trigger();

	//check for help message:
	if(mPlugInManager->checkHelpMessageRequest()) {
		helpMessageRequest = true;
		logMessage("There was a help message request: System only shows the help message and then returns.");
	}

	//check for correct command line syntax
	if(!mPlugInManager->validateCommandLineSyntax()) {
		logMessage(QString("Core: Command line argument syntax was invalid! [TERMINATING]"));
		mPlugInManager->printCommandLineHelpMessage();
		ok = false;
	}

	QString statusMessage = "~Initializing Core ";
	if(ok) {
		statusMessage.append("was successful!");
	}
	else {
		statusMessage.append("failed!");
	}
	logMessage(statusMessage);

	if(!ok) {
		//put a message to the command line.
		cout << "Core: Initialization was not successful! See the log file in config/debug "
			 << "for details! [TERMINATING]" << endl;
	}

	executePendingTasks();

	return ok && !helpMessageRequest && !quitApplication;
}


/**
 * Adds a SystemObject to the Core. SystemObjects, once registered, are considered
 * during the initialization, binding and shutdown phases. Their corresonding methods
 * init(), bind() and shutdown() are called in each of the phases. 
 *
 * Furthermore SystemObjects are automatically destroyed when the Core is destroyed. 
 * Since it is not possible to remove a SystemObject from the Core, all SystemObjects
 * have to be designed to allow a destruction by the Core. This means especially that
 * they are created on the heap (with new) and that they are not destroyed anywhere else.
 *
 * SystemObjects can only be added once to avoid calling phase methods multiple times and
 * to avoid double deletion during the Core destruction.
 *
 * @param object the object to add. 
 * @return true if successful, false if it was already registered.
 */
bool Core::addSystemObject(SystemObject *object) {
	if(object == 0 || mSystemObjects.contains(object)) {
		return false;
	}
	if(sVerbose) {
		cerr << "Adding SystemObject " << object->getName().toStdString().c_str() << endl;
	}

	logMessage(QString("Adding SystemObject ").append(object->getName()));
	mSystemObjects.append(object);
	return true;
}


/**
 * Removes a SystemObject from the Core. This object will not be notified during the init(),
 * bind() and cleanUp() phases. Also the object will not be destroyed automatically by the core.
 * 
 * If the removed object was also a global object (method addGlobalObject()), then this object
 * is also removed from the global objects and not accessible any more using getGlobalObject().
 *
 * @param object the object to remove.
 * @return true if successful, false if object was not contained.
 */
bool Core::removeSystemObject(SystemObject *object) {
	if(object == 0 || !mSystemObjects.contains(object)) {
		return false;
	}

	if(sVerbose) {
		cerr << "Removing SystemObject " << object->getName().toStdString().c_str() << endl;
	}

	mSystemObjects.removeAll(object);

	//check if a global object has to be removed.
	if(mGlobalObjects.values().contains(object)) {
		QList<QString> keys = mGlobalObjects.keys();

		for(int i = 0; i < keys.size(); ++i) {
			QString key = keys.at(i);
			if(mGlobalObjects.value(key) == object) {
				mGlobalObjects.remove(key);
			}
		}
	}

	return true;
}


/**
 * Returns a vector with all currently registered SystemObjects. 
 * The returned vector is the internal vector, so it might change as SystemObjects
 * are added with method addSystemObject(). 
 *
 * @return a reference to the internal vector holding all registered SystemObjects. 
 */
QList<SystemObject*> Core::getSystemObjects() const {
	return mSystemObjects;
}


/**
 * Adds a global object. The global object will automatically be registered
 * as SystemObject - using method addSystemObject - if it is not already
 * registered there. Therefore global objects can not be removed from the 
 * Core any more. Only their association with a name can be cancled by calling 
 * method removeGlobalObject(). 
 *
 * Global objects use system wide unique names. If there is already a global object
 * with the given objectName, then the global object is not added and false is returned.
 * 
 * @param objectName the name to associate the global SystemObject with. 
 * @param object the object to add.
 * @return true if successful, false if already registered, of the name is already used.
 */
bool Core::addGlobalObject(const QString &objectName, SystemObject *object) {
	if(object == 0 || mGlobalObjects.keys().contains(objectName)) {
		return false;
	}
	logMessage(QString("Adding global object [").append(object->getName())
			.append("] with global name [").append(objectName).append("]"));

	addSystemObject(object);
	//TODO memory leak detected
	mGlobalObjects.insert(objectName, object);
	return true;
}


/**
 * Removes a global object. A pointer to the removed SystemObject is returned. 
 * Thus the SystemObject is not destroyed. Further it is not removed from 
 * the list or registered SystemObject. The object therefore will still
 * be destroyed when the Core is.
 *
 * @param objectName the name of the global object to remove.
 * @return a pointer to the removed SystemObject, or NULL if no object was found.
 */
SystemObject* Core::removeGlobalObject(const QString &objectName) {
	if(!mGlobalObjects.keys().contains(objectName)) {
		return 0;
	}
	logMessage(QString("Removing global with global name [")
			.append(objectName).append("]"));

	return mGlobalObjects.take(objectName);
}


/**
 * Returns the global object associated with the given name.
 * If no such object exists, NULL is returned. 
 *
 * @param name the name of the global object.
 * @return the global object if one exists with the given name, otherwise false.
 */
SystemObject* Core::getGlobalObject(const QString &name) const {
	if(!mGlobalObjects.keys().contains(name)) {
		return false;
	}
	return mGlobalObjects.value(name);
}


/**
 * Returns a list with all currently known global objects. 
 *
 * @return a list with all global objects. 
 */
QList<SystemObject*> Core::getGlobalObjects() const {
	return mGlobalObjects.values();
}


/**
 * Initiates the shutdown sequence of the Core. This will first trigger the 
 * ShutDown Event to allow non-SystemObjects to release resources. Then 
 * the cleanUp() methods of all SystemObjects are called to allow all objects
 * to cleanly shut down, to store settings or to release resources.
 * 
 * The Core Properties list will be stored at the end of the shutdown sequence, 
 * so all object can use this Properties list during shutdown to store relevant 
 * properties that should persist up to the next application start.
 *
 * @return true if successful, false if at least one SystemObject returned false in 
 *          its cleanup method.
 */
bool Core::shutDown() {
	TRACE("Core::shutDown");

	mIsShuttingDown = true;

	if(mShutDownCompleted) {
		return false;
	}

	mShutDownCompleted = true;

	logMessage("~Shutting down Core.");
	bool ok = true;

	//work with a copy to allow SystemObjects to deregister during shud
	QList<SystemObject*> systemObjects = mSystemObjects;

	for(int i = 0; i < systemObjects.size(); i++) {
		if(sVerbose) {
			cerr << "> " << flush;
			cerr << "Shutting down  " 
				 << systemObjects.at(i)->getName().toStdString().c_str() 
				 << " with index " << i << endl;
		}
		if(!systemObjects.at(i)->cleanUp()) {
			cout << "Core: Warning: Could not cleanUp SystemObject ["
					 << systemObjects.at(i)->getName().toStdString().c_str() << "]" << endl;
			ok = false;
		}
		if(sVerbose) {
			cerr << "Shutdown ok" << endl;
		}
	}

	if(sVerbose) {
		cerr << "Triggering shutdown event" << endl;
	}

	if(mShutDownEvent != 0) {
		mShutDownEvent->trigger();
	}

	if(sVerbose) {
		cerr << "ShutDown event listeners completed." << endl;
	}

	mInitializedSuccessful = false;

	//try to save properties
	if(!mIsUsingReducedFileWriting) {
		mProperties.saveToFile(getConfigDirectoryPath().append("/properties/main.props"));
	}

	QString statusMessage = "~Shutting down Core ";
	if(ok) {
		statusMessage.append("was successful!");
	}
	else {
		statusMessage.append("failed!");
	}
	logMessage(statusMessage);

	if(sVerbose) {
		cerr << "Removing all listeners from ValueManager and Values." << endl;
	}

	//disable notifications at Value changes or at changes of the repository.
	mValueManager->removeAllListeners();

	if(sVerbose) {
		cerr << "Deregistration from ValueManager and Values completed." << endl;
	}

	return ok;
}


bool Core::isShuttingDown() const {
	return mIsShuttingDown;
}

/**
 * Schedules a Task to be executed later in the main execution thread. 
 * This method can be used to wrap a non-thread-save piece of code
 * into a method that is executed withint the main execution thread. This
 * avoids many problems usually demanding thread synchronization, which has to 
 * be avoided for the main thread. This should lessen the chance of dead-locking the
 * system.
 *
 * Scheduled Tasks are automatically destroyed after execution, so all Tasks have to be
 * created at the heap and external destruction of the Task is forbidden. Also a Task 
 * can not be scheduled more than once, because it will be destroyed after its first
 * execution.
 *
 * @param task the Task to schedule for execution in the main execution thread.
 * @return true if successful, false if the Task was already registered.
 */
bool Core::scheduleTask(Task *task) {
	QMutexLocker locker(&mTaskLocker);
	if(task == 0 || mScheduledTasks.contains(task)) {
		return false;
	}
	mScheduledTasks.append(task);
	return true;
}


/**
 * Returns a list with all currently pending Tasks, that have been registered with
 * method schedule Task. 
 * Note that the real internal list of Task is returned, which means that it might change
 * at any time while you are using it (if another thread schedules a Task). To ensure 
 * a fixed list, create a copy of the referenced Task list. 
 *
 * @return the list with all currently pending Tasks. 
 */
QList<Task*> Core::getPendingTasks() const {
	return mScheduledTasks;
}


/**
 * Removes all pending Tasks without executing them. This can lead to unpredicted behavior
 * and should only be used during or after shutdown of the system.
 *
 * TODO: Maybe method should be protected or private.
 */
void Core::clearPendingTasks() {
	QMutexLocker locker(&mTaskLocker);
	while(!mScheduledTasks.empty()) {
		Task *task = mScheduledTasks.first();
		mScheduledTasks.removeAll(task);
		delete task;
	}
}


/**
 * Executes all scheduled Tasks that are pending. 
 * While this method is executing new Tasks are scheduled, but 
 * not executed in the same execution loop. They are scheduled for the next 
 * call of executePendingTasks.
 * 
 * Only the current main execution thread is allowed to call this method, if 
 * a thread registered to execute the main execution thread. Otherwise any
 * thread can execute the method, but time races between threads might require
 * synchronization of objects then. 
 */
void Core::executePendingTasks() {

	if(!isMainExecutionThread()) {
		log("Core Warning: Method executePendingTasks was called from a thread "
			" other than the main execution thread. Execution was denied!");
		return;
	}

	mTaskLocker.lock();
	if(mScheduledTasks.empty()) {
		mTaskLocker.unlock();
		return;
	}
	QList<Task*> tasks(mScheduledTasks);
	mScheduledTasks.clear();
	mTaskLocker.unlock();

	for(QListIterator<Task*> i(tasks); i.hasNext();) {
		Task *task = i.next();
		task->runTask();
		delete task;
	}
	
	//TODO remove
	//mTasksExecutedEvent->trigger();
}


/**
 * Logs a string to the Core logger. This logger automatically writes the logger message
 * to a file, adding the current time. Additionally the message is written to a
 * StringValue available in the ValueManager with name "/Logger/RecentMessage"
 * that can be observed to capture all occuring logger messages. 
 *
 * Messages starting with a "~" are especially highlighted to simplify reading of log files.
 * 
 * @param message the message to log.
 */
void Core::logMessage(const QString &message) {
	if(mCurrentLogMessage != 0) {
		mCurrentLogMessage->set(message);
	}
	if(mLogFileStream != 0) {
		bool highlight = message.startsWith("~");
		if(highlight) {
			(*mLogFileStream) << endl
				<< QTime::currentTime().toString("hh:mm:ss") << " : "
				<< message.mid(1) << endl << endl;
		}
		else {
			(*mLogFileStream) << QTime::currentTime().toString("hh:mm:ss") << " : "
				<< message << endl;
		}
		mLogFileStream->flush();
	}
}


/**
 * Returns a list with all names of all currently known global objects. 
 * Any SystemObject can be registered as global object and hereby be associated
 * with a name. This method can be used to check which global objects are 
 * registered. With the names and method getGlobalObject() it is also possible
 * to get all available global object and check for instance for their class type. 
 *
 * @return a list with the names of all registered global objects. 
 */
QList<QString> Core::getGlobalObjectNames() const {
	return mGlobalObjects.keys();
}


/**
 * Returns the Event that is triggered immediately after the initialization of all 
 * SystemObjects is completed, independently of whether the initialization was 
 * successful. This event can be used by objects that can not implement the SystemObject
 * interface to react on the initialization process. 
 * It is highly recommended to use the normal way to react on the initiaization, 
 * thus adding SystemObjects to the Core and overwriting the init() method.
 * 
 * @return the init event.
 */
Event* Core::getInitEvent() const {
	return mInitEvent;
}


/**
 * Returns the Event that is triggered immediately after the entire initialization phase 
 * is completed (so including init() and bind() of all SystemObjets),
 * independently of whether the initialization was 
 * successful. This event can be used by objects that can not implement the SystemObject
 * interface to react on the initialization process. 
 * It is highly recommended to use the normal way to react on the initiaization, 
 * thus adding SystemObjects to the Core and overwriting the init() method.
 * 
 * @return the init event.
 */
Event* Core::getInitCompletedEvent() const {
	return mInitCompletedEvent;
}


/**
 * Returns the Event that is triggered immediately after the binding phase ofor all 
 * SystemObjects is completed, independently of whether the binding phase was 
 * successful. This event can be used by objects that can not implement the SystemObject
 * interface to react on the binding process. 
 * It is highly recommended to use the normal way to react on the binding phase, 
 * thus adding SystemObjects to the Core and overwriting the bind() method..
 * 
 * @return the init event.
 */
Event* Core::getBindEvent() const {
	return mBindEvent;
}


/**
 * Returns the Event triggered before the shutdown procedure is initiated.
 * This Event can be used by objects which are not SystemObject, but require
 * a clean shutdown when the system is destroyed. Such object can register
 * as EventListeners and get a notification at shutdown, where they can 
 * release ressources or cancle connection to other objects in the system savely, 
 * as all objects are still existing.
 * 
 * @return the ShutDown Event of the Core.
 */
Event* Core::getShutDownEvent() const {
	return mShutDownEvent;
}


bool Core::isPerformanceMeasuringEnabled() const {
	return mEnablePerformanceMeasures->get();
}


/**
 * Returns the Core Properties list. 
 * This list can be used by any object in the system to store or read
 * parameters which should be persistent between application starts. 
 * 
 * The Properties list of the Core is automatically stored to a file at
 * shutdown and loaded during the setup phase. So it is an ideal place to store
 * persistent data, like file chooser paths, window sizes, etc.
 *
 * @return the Properties list of the Core.
 */
Properties& Core::getProperties() {
	return mProperties;
}


/**
 * Returns the absolute path of the NERD configuration file used by this
 * application. An NERD application uses this directory to store settings, 
 * logfiles and other locally used information that are specific for this
 * appliction.
 * 
 * A common usage of this file can be a GUI module, which stores its
 * current settings to an own Properties file and reloads them at startup 
 * of the application. The path for the Properties file can be requested
 * with this method and - if required - by applying enforceDirectoryPath 
 * on a subdirectory of the configuration directory.
 *
 * @return the absolute path name for the configuration directory.
 */
QString Core::getConfigDirectoryPath() const {
	return mConfigDirectoryName;
}


/**
 * Makes sure that the given directory path is available. If the path is not available,
 * then all required directories are created to match the specified path.
 * If a path does not exist and if it could not be created, then false is returned.
 * In case the path already exists no changes to the file system are made.
 * 
 * @param path the path to force to exist.
 * @return true if successful, otherwise false.
 */
bool Core::enforceDirectoryPath(const QString &path) {
	bool ok = true;
	QString localPath = path;
	if(localPath.startsWith(QDir::currentPath())) {
		localPath.mid(QDir::currentPath().size());
	}
	QFile file(QDir::currentPath().append(localPath));
	if(!file.exists()) {
		QFile localFile(localPath);
		QFileInfo info(localFile);
		ok = QDir::current().mkpath(info.absoluteFilePath());
	}
	return ok;
}


/**
 * The thread in which this method was called the last time is assumed to be the
 * main execution thread. The main execution thread is responsible for the 
 * execution of the pending tasks (executePendingTasks()) in suitable intervals.
 *
 * Note: if the current thread is not already registered at the Core, it will be
 * registered using method registerThread().
 */
void Core::setMainExecutionThread() {
	mMainExecutionThread = QThread::currentThread();
	if(mMainExecutionThread != 0 
		&& !mAvailableThreads.contains(mMainExecutionThread)) 
	{
		registerThread(mMainExecutionThread);
	}
}

/**
 * Unsets the main execution thread. After calling this method the Core does not 
 * assume a main execution thread to be existing. This method should be called by
 * the main execution thread right before it is stopped to allow other threads
 * to use trigger Events without warnings (e.g. during shutdown).
 */
void Core::clearMainExecutionThread() {
	mMainExecutionThread = 0;
}

/**
 * Returns true if a thread was set as the main execution thread
 * and the calling thread it the same thread. If a main execution
 * thread was set and the calling thread is not the same thread,
 * then false is returned.
 *
 * If no main execution thread was set (by calling setMainExecutionThread()),
 * then this method always returns true.
 *
 * @return true, if there is no main execution thread or the
 * 			calling thread is the main execution thread.
 */
bool Core::isMainExecutionThread() const {
	if(mMainExecutionThread == 0) {
		return true;
	}
	return (mMainExecutionThread == QThread::currentThread());
}


/**
 * Registeres a thread at the Core. At system shutdown the Core can wait for the
 * completion of all registered threads by calling method waitForAllThreadsToComplete().
 *
 * @param thread the thread to register
 * @return true if successful, false if NULL or already registered.
 */
bool Core::registerThread(QThread *thread) {
	if(thread == 0 || mAvailableThreads.contains(thread) 
		|| (QCoreApplication::instance() != 0 && thread == QCoreApplication::instance()->thread())) 
	{
		return false;
	}
	mAvailableThreads.append(thread);
	return true;
}


/**
 * Removes a thread from the Core. This should has to be done whenever a registered 
 * thread is going to be destroyed somewhere (except if it is automatically destroyed
 * by the Core as a SystemObject).
 * 
 * @param thread the thread to deregister.
 * @return true if successful, false if NULL or not registered.
 */
bool Core::deregisterThread(QThread *thread) {
	if(thread == 0 || !mAvailableThreads.contains(thread)) {
		return false;
	}
	mAvailableThreads.removeAll(thread);
	return true;
}


/**
 * Waits for the completion of all running threads. 
 * 
 * Note: Make sure all threads are notified to stop before calling this method
 * as this may lead to a dead lock of the system. 
 */
void Core::waitForAllThreadsToComplete() {
	TRACE("Core::waitForAllThreadsToComplete");
	QList<QThread*> threads(mAvailableThreads);

	for(int i = 0; i < threads.size(); ++i) {
		QThread *thread = threads.at(i);
		if(thread == QCoreApplication::instance()->thread()
			|| thread == QThread::currentThread()) {
			continue;
		}
		while(!thread->wait(100)) {
			if(isMainExecutionThread()) {
				executePendingTasks();
			}
		}
	}
}


QString Core::getVersionString() {
	QString version(NERD_VERSION);
	//add SVN revision number (requires QMake to be configured to provide this macro (VER)!
	QString buildNumber(VER);
	//buildNumber = buildNumber.mid(0, buildNumber.size() - 1);
	version = version.append(" Rev. ").append(buildNumber);
	return version;
}

bool Core::isUsingReducedFileWriting() const {
	return mIsUsingReducedFileWriting;
}


}

