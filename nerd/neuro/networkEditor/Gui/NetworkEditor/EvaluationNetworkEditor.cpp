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



#include "EvaluationNetworkEditor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QMenuBar>
#include <QMenu>
#include "Event/EventManager.h"
#include "Value/ValueManager.h"
#include "NeuralNetworkConstants.h"
#include "Network/NeuralNetwork.h"
#include "Network/Neuro.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include <QTabWidget>
#include <QApplication>
#include <QThread>
#include "Util/Tracer.h"
#include "Value/BoolValue.h"
#include "NeuralNetworkConstants.h"
#include "Gui/Control/BoolValueSwitcherAction.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>
#include "PlugIns/PlugIn.h"
#include "PlugIns/PlugInManager.h"
#include "PlugIns/CommandLineArgument.h"
#include "Gui/Control/BoolValueSwitcherAction.h"
#include "NerdConstants.h"
#include <Gui/Control/EventTriggerAction.h>

using namespace std;

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

namespace nerd {


/**
 * Constructs a new EvaluationNetworkEditor.
 */
EvaluationNetworkEditor::EvaluationNetworkEditor(bool enableManualStasisControl)
	: NeuralNetworkEditor(0), mFileMenu(0), mFirstNetwork(true), 
		mEnableManualStasisControl(enableManualStasisControl), mSaveNetworkAction(0)
{

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	Core::getInstance()->addSystemObject(this);

	connect(this, SIGNAL(networksReplacedEvent()),
			this, SLOT(networksReplaced()));
	connect(this, SIGNAL(destroyAllTabsEvent()),
			this, SLOT(destroyAllTabs()));

	
}



/**
 * Destructor.
 */
EvaluationNetworkEditor::~EvaluationNetworkEditor() {
	TRACE("EvaluationNetworkEditor::~EvaluationNetworkEditor");
	if(mStasisValue != 0) {
		mStasisValue->removeValueChangedListener(this);
	}
}



bool EvaluationNetworkEditor::init() {
	bool ok = true;


	return ok;
}


bool EvaluationNetworkEditor::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mNetworksReplacedEvent = em->registerForEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED, this);
	mNetworksModifiedEvent = em->registerForEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED, this);

		
	ValueManager *vm = Core::getInstance()->getValueManager();

	mStasisValue = vm->getBoolValue(NeuralNetworkConstants::VALUE_EVO_STASIS_MODE);

	if(mNetworksReplacedEvent == 0 || mNetworksModifiedEvent == 0 || mStasisValue == 0) {
		//Note: All of these events and values are optional, so no warning and no failure.
	}
	if(mStasisValue != 0) {
		mStasisValue->addValueChangedListener(this);
		triggerEnableNetworkModification(mStasisValue->get());
	}
	else {
		triggerEnableNetworkModification(true);
	}

	handleNetworksReplaced();

	return ok;
}


bool EvaluationNetworkEditor::cleanUp() {
	TRACE("EvaluationNetworkEditor::~cleanUp");
	
	bool ok = true;

	emit destroyAllTabsEvent();
	while(mMainPane->count() > 0) {
		QApplication::instance()->thread()->wait(10);
	}

	if(mNetworksReplacedEvent != 0) {
		mNetworksReplacedEvent->removeEventListener(this);
	}
	if(mNetworksModifiedEvent != 0) {
		mNetworksModifiedEvent->removeEventListener(this);
	}

	Core::getInstance()->removeSystemObject(this);

	return ok;
}


QString EvaluationNetworkEditor::getName() const {
	return "EvaluationNetworkEditor";
}



void EvaluationNetworkEditor::eventOccured(Event *event) {
	TRACE("EvaluationNetworkEditor::eventOccured");
	NeuralNetworkEditor::eventOccured(event);
	if(event == 0) {
		return;
	}
	else if(event == mNetworksReplacedEvent) {
		handleNetworksReplaced();
	}
}


void EvaluationNetworkEditor::valueChanged(Value *value) {
	TRACE("EvaluationNetworkEditor::valueChanged");

	NeuralNetworkEditor::valueChanged(value);

	if(value == 0) {
		return;
	}
	else if(value == mStasisValue) {
		for(int i = 0; i < mMainPane->count(); ++i) {
			NetworkVisualization *visu = getNetworkVisualization(i);
			if(visu != 0) {
				if(mStasisValue->get()) {
					visu->addStatusMessage("STASIS MODE");
				}
				else {
					visu->removeStatusMessage("STASIS MODE");
				}
			}
		}
		triggerEnableNetworkModification(mStasisValue->get());
	}
}


int EvaluationNetworkEditor::addNetworkVisualization(const QString &name) {
	int index = NeuralNetworkEditor::addNetworkVisualization(name);
	if(mStasisValue != 0 && mStasisValue->get()) {
		if(index > 0 && index < mMainPane->count()) {
			NetworkVisualization *visu = getNetworkVisualization(index);
			if(visu != 0) {
				visu->addStatusMessage("STASIS MODE");
			}
		}
	}
	return index;
}

void EvaluationNetworkEditor::renameCurrentNetwork(const QString &name) {
	NeuralNetworkEditor::renameCurrentNetwork(name);
	if(mSaveNetworkAction != 0) {
		NetworkVisualization *visu = getCurrentNetworkVisualization();
		if(visu != 0 && visu->getCurrentNetworkFileName().trimmed() == "") {
			mSaveNetworkAction->setEnabled(false);
		}
		else {
			mSaveNetworkAction->setEnabled(true);
		}
	}
}

void EvaluationNetworkEditor::networksReplaced() {
	TRACE("EvaluationNetworkEditor::networksReplaced");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	QList<NeuralNetwork*> networks = Neuro::getNeuralNetworkManager()->getNeuralNetworks();

	while(mMainPane->count() < networks.size()) {
		addNetworkVisualization("Network");
	}
	int i = 0;
	for(; i < networks.size(); ++i) {
		NetworkVisualization *visu = getNetworkVisualization(i);
		ModularNeuralNetwork *network = dynamic_cast<ModularNeuralNetwork*>(networks.at(i));
		if(visu != 0) {
			visu->setNeuralNetwork(network);
		}
	}
	for(; i < mMainPane->count(); ++i) {
		NetworkVisualization *visu = getNetworkVisualization(i);
		if(visu != 0) {
			visu->setNeuralNetwork(0);
		}
	}

	if(!networks.empty() && mFirstNetwork) {
		mFirstNetwork = false;

		//TODO load layout for each panel, matching the agent name of controller and command line argument

		//check if a layout for the current widget can be loaded.
		CommandLineArgument *loadNetworkArgument = 
				Core::getInstance()->getPlugInManager()->getCommandLineArgument("loadNet");
		if(loadNetworkArgument != 0) {
			QStringList args = loadNetworkArgument->getEntryParameters(0);
			if(args.size() > 1) {
				QString fileName = args.at(1);
				int dotIndex = fileName.lastIndexOf(".");
				if(dotIndex >= 0) {
					fileName = fileName.mid(0, dotIndex).append(".lyt");
					loadLayout(fileName, true);
				}
			}
		}
	}
}


void EvaluationNetworkEditor::destroyAllTabs() {
	TRACE("EvaluationNetworkEditor::destroyAllTabs");
	while(mMainPane->count() > 0) {
		removeNetworkVisualization(0);
	}
}


void EvaluationNetworkEditor::overwriteCurrentNetwork() {
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu != 0 && visu->getCurrentNetworkFileName().trimmed() != "") {
		QString fileName = visu->getCurrentNetworkFileName();
		if(fileName.endsWith("*")) {
			fileName = fileName.mid(0, fileName.size() - 1);
		}
		saveNetwork(fileName, 0, true, true);
		visu->setCurrentNetworkFileName(fileName);
	}
}

void EvaluationNetworkEditor::activationVisualizationModeChanged() {
	for(QListIterator<NetworkVisualization*> i(mNetworkVisualizations); i.hasNext();) {
		NetworkVisualization *visu = i.next();
		visu->setActivationVisualizationMode(mContinuousVisualizationModeAction->isChecked());
	}
}



void EvaluationNetworkEditor::setupMenuBar() {
	addFileMenu();
	addControlMenu();
	QMenu *editMenu = addEditMenu();
	extendEditMenu(editMenu);
	addPlotterMenu();
	addViewModeMenu();
	addHelpMenu();
}

 
QMenu* EvaluationNetworkEditor::addFileMenu() {
	QMenuBar *mainMenu = menuBar();

	//File menu
	mFileMenu = new QMenu("File", mainMenu);

	connect(this, SIGNAL(enableNetworkModifications(bool)),
			mFileMenu, SLOT(setEnabled(bool)));

	QAction *loadNetworkAction = mFileMenu->addAction("Load Network");
	connect(loadNetworkAction, SIGNAL(triggered()),
			this, SLOT(loadNetwork()));

	mFileMenu->addMenu(mRecentNetworksMenu);

	mFileMenu->addSeparator();

// 	mSaveNetworkAction = mFileMenu->addAction("&Save Network");
// 	mSaveNetworkAction->setEnabled(false);
// 	connect(mSaveNetworkAction, SIGNAL(triggered()),
// 			this, SLOT(overwriteCurrentNetwork()));

	QAction *saveNetworkAsAction = mFileMenu->addAction("Save Network As...");
	connect(saveNetworkAsAction, SIGNAL(triggered()),
			this, SLOT(saveNetwork()));

	mFileMenu->addSeparator();

	QAction *addSubNetworkAction = mFileMenu->addAction("Add Subnetwork");
	connect(addSubNetworkAction, SIGNAL(triggered()),
			this, SLOT(addSubNetwork()));

	QAction *saveSubNetworkAction = mFileMenu->addAction("Save Subnetwork");
	connect(saveSubNetworkAction, SIGNAL(triggered()),
			this, SLOT(saveSubNetwork()));

	mFileMenu->addSeparator();

	QAction *loadLayoutAction = mFileMenu->addAction("Load Layout");
	connect(loadLayoutAction, SIGNAL(triggered()),
			this, SLOT(loadLayout()));

	QAction *saveLayoutAction = mFileMenu->addAction("Save Layout");
	connect(saveLayoutAction, SIGNAL(triggered()),
			this, SLOT(saveLayout()));

	mFileMenu->addSeparator();

	QMenu *exportMenu = mFileMenu->addMenu("Export as");

	QAction *exportByteCodeAction = exportMenu->addAction("A-Series Bytecode");
	connect(exportByteCodeAction, SIGNAL(triggered()),
					this, SLOT(exportByteCode()));
	
	QAction *exportASeriesBDNAction = exportMenu->addAction("A-Series BDN");
	connect(exportASeriesBDNAction, SIGNAL(triggered()),
					this, SLOT(exportASeriesBDN()));
	
	QAction *exportMSeriesBDNAction = exportMenu->addAction("M-Series BDN");
	connect(exportMSeriesBDNAction, SIGNAL(triggered()),
					this, SLOT(exportMSeriesBDN()));
	
	QAction *exportAvgGraphicsAction = exportMenu->addAction("SVG Graphics");
	connect(exportAvgGraphicsAction, SIGNAL(triggered()),
					this, SLOT(saveNetworkAsSvgGraphics()));

// 	QAction *exportDebugMSeriesComponentsBDNAction = exportMenu->addAction("DEBUG - M-Series Components BDN");
// 	connect(exportDebugMSeriesComponentsBDNAction, SIGNAL(triggered()),
// 					this, SLOT(exportDebugMSeriesComponentsBDN()));
	
	mFileMenu->addSeparator();

	QAction *createMinimalNetworkAction = mFileMenu->addAction("Create Minimal Network");
	connect(createMinimalNetworkAction, SIGNAL(triggered()),
			this, SLOT(createMinimalNetwork()));

	mainMenu->addMenu(mFileMenu);
	return mFileMenu;
}

QMenu* EvaluationNetworkEditor::addControlMenu() {

	QMenuBar *mainMenu = menuBar();

	QMenu *controlMenu = new QMenu("Control", mainMenu);

	if(mEnableManualStasisControl) {
		QAction *stasisAction = new BoolValueSwitcherAction("Stasis Mode", 
									NeuralNetworkConstants::VALUE_EVO_STASIS_MODE);
		stasisAction->setShortcut(tr("ctrl+t"));
		controlMenu->addAction(stasisAction);
	}

	mContinuousVisualizationModeAction = controlMenu->addAction("Visualize Activity");
	mContinuousVisualizationModeAction->setCheckable(true);
	mContinuousVisualizationModeAction->setChecked(true);
	connect(mContinuousVisualizationModeAction, SIGNAL(changed()),
			this, SLOT(activationVisualizationModeChanged()));
	

	//Add pause checkbox
	BoolValueSwitcherAction *pauseAction =
			new BoolValueSwitcherAction("&Pause", NerdConstants::VALUE_EXECUTION_PAUSE);
	pauseAction->setShortcut(tr("Ctrl+p"));
	pauseAction->setWhatsThis("Pauses and resumes the simulation.");
	controlMenu->addAction(pauseAction);
	
	QList<QString> resetEvents;
	resetEvents.append(NerdConstants::EVENT_EXECUTION_RESET);
	resetEvents.append(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED);
	EventTriggerAction *resetAction = new EventTriggerAction("&Reset Simulation", resetEvents);
	resetAction->setShortcut(tr("Ctrl+r"));
	resetAction->setWhatsThis("Resets the simulation to the initial conditions.");
	controlMenu->addAction(resetAction);
	
	mainMenu->addMenu(controlMenu);

	return controlMenu;
}


void EvaluationNetworkEditor::setupTools() {
	NeuralNetworkEditor::setupTools();
}


void EvaluationNetworkEditor::extendEditMenu(QMenu *editMenu) {
	if(editMenu == 0) {
		return;
	}
}



void EvaluationNetworkEditor::handleNetworksReplaced() {
	TRACE("EvaluationNetworkEditor::handleNetworksReplaced");

	{
		QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

		invalidateNeuralNetworkObservers(); //TODO add network visus as networkObservers.
	}

	for(int i = 0; i < mMainPane->count(); ++i) {
		NetworkVisualization *visu = getNetworkVisualization(i);
		if(visu != 0) {
			visu->invalidateNetwork();	
			QMutexLocker lock(visu->getSelectionMutex());
		}
	}
	emit networksReplacedEvent();
}


void EvaluationNetworkEditor::handleNetworksModified() {
	TRACE("EvaluationNetworkEditor::handleNetworksModified");

}




}



