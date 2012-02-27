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

#include "BaseApplication.h"
#include "Core/Core.h"
#include "PlugIns/CommandLineParser.h"
#include <QString>
#include <iostream>
#include "Value/ValueManager.h"
#include "PlugIns/PlugInManager.h"
#include <iostream>
#include <QCoreApplication>
#include "Value/ValueAtCommandLineHandler.h"

using namespace std;

namespace nerd {

/**
 * Constructs a BaseApplication and parses the argument list.
 *
 * Note: a BaseApplication will be automatically registered at the
 * Core as SystemObject and therefore has to be created with new on the heap.
 * Since the Core will destroy all SystemObjects, this BaseApplication must
 * not be destroyed manually!
 */
BaseApplication::BaseApplication()
	: mSetUp(false), mShutDown(false), mName("BaseApplication")
{
	Core::getInstance()->addSystemObject(this);
	Core::getInstance()->registerThread(this);
	Core::getInstance()->registerThread(QThread::currentThread());

	connect(this, SIGNAL(quitMainApplication()),
			QCoreApplication::instance(), SLOT(quit()));

	mArgument_val = new CommandLineArgument(
			"loadValues", "val", "<file>",
			"Loads a value file after the initialization phase. "
			"\n    <file> specifies the file to load",
			1, 0,
			true);

	mArgument_ival = new CommandLineArgument(
			"loadInitValues", "ival", "<file>",
			"Loads a value file before the initialization phase."
			"\n    <file> specifies the file to load",
			1, 0,
			true);

	//allow for setting of parameters via command line argument (-sv)
	new ValueAtCommandLineHandler();

	setupGui();
}


/**
 * Destructor.
 */
BaseApplication::~BaseApplication() {
}


/**
 * Called during the initialization phase of the Core.
 * Can be overwritten in subclasses.
 *
 * @return true if successful, otherwise false.
 */
bool BaseApplication::init() {
	mShutDown = false;
	Core::getInstance()->registerThread(this);
	return true;
}


/**
 * Called during the bind phase of the Core.
 * Can be overwritten in subclasses.
 *
 * @return true if successful, otherwise false.
 */
bool BaseApplication::bind() {
	return true;
}


/**
 * Called in the shut down phase of the Core.
 * Stopps the BaseApplication thread and waits for its completion.
 * If overwritten in subclasses always to a call to this method.
 *
 * @return true if successful, otherwise false.
 */
bool BaseApplication::cleanUp() {
	mShutDown = true;
	QThread::quit();
// 	if(QThread::currentThread() != this) {
// 		wait();
// 	}
	return true;
}


/**
 * Returns "BaseApplication". Subclasses may overwrite this with suitable names.
 *
 * @return the name of this SystemObject.
 */
QString BaseApplication::getName() const {
	return mName;
}


/**
 * Starts the BaseApplication thread. Hereby the system is created and executed.
 * To influence the behavior of the BaseApplication overwrite the pure virtual
 * methods to handle the three application phases:
 *
 * setupApplication(): Add all components to the system that are required for your
 * 		application. Make sure that GUI elements are moved to the main thread,
 * 		because this method is called from within the BaseApplication thread.
 *
 * runApplication(): Execute the application. If you use blocking code, then there
 *		will be no event loop in the BaseApplication (except of the main thread).
 * 		However you can start your application control code in a separate thread
 * 		and return from runApplication(), which will not end your program, but will
 * 		start the BaseApplication event loop.
 */
void BaseApplication::run() {

	setUp();

	if(!startSystem()) {
		mShutDown = true;
		Core::getInstance()->shutDown();
	}

	//start the BaseApplication event loop if the application is still running.
	if(!mShutDown) {
		QThread::exec();
	}

// 	Core::getInstance()->deregisterThread(QThread::currentThread());
	Core::getInstance()->waitForAllThreadsToComplete();

	emit quitMainApplication();
}



void BaseApplication::startApplication() {
	setupGui();
	start();
}

/**
 * This method can be overwritten to set up GUI elements. Grafical elements should NOT
 * be added in method setupApplication, because that method is not called in the main 
 * GUI thread of QT (QApplication::instance()->thread()). 
 * 
 * In difference this method here is called in the constructor of the BaseApplication. 
 * So if there are parameters to pass via constructor, these parameters have to be
 * set in the initizaiton list of the constructor. 
 */
bool BaseApplication::setupGui() {
	return true;
}


/**
 * Loads all value files specified as command line arguments. 
 * 
 * @param fileNames a list containing all value files to be loaded.
 */
void BaseApplication::loadValues(QStringList fileNames) {
	ValueManager *vm = Core::getInstance()->getValueManager();

	for(int i = 0; i < fileNames.size(); ++i) {
		vm->loadValues(fileNames.at(i));
	}
}


/**
 * Called to set up the application. This calls the pure virtual method
 * setupApplication().
 *
 * @return true if there was no problem, otherwise false.
 */
bool BaseApplication::setUp() {
	bool ok = true;
	if(!setupApplication()) {
		Core::log("BaseApplication: Problems setting up simulator!");
		ok = false;
	}
	mSetUp = ok;
	return ok;
}


/**
 * Starts the BaseApplication by making sure that setup took place, by initializing the
 * Core, by loading value files (pre and post init) and by calling the user defined
 * pure virtual method runApplication.
 *
 * @return true if successful, otherwise false.
 */
bool BaseApplication::startSystem() {
	Core *core = Core::getInstance();

	if(!mSetUp) {
		setUp();
	}

	loadValues(mArgument_ival->getEntries());

	if(!core->isInitialized()) {
		if(!core->init()) {
			Core::log("Warning: Could not initialize Core. Terminating Application.");
			return false;
		}
	}

	Core::log("Loading post init values.");
	loadValues(mArgument_val->getEntries());

	emit showGui();	

	return runApplication();
}



}


