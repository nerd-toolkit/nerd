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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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



#include "GuiMainWindow.h"
#include <QMenu>
#include "Gui/Parameter/MultipleParameterWindowWidget.h"
#include "Core/Core.h"
#include "Gui/Control/MotorControlGui.h"
#include "Gui/Control/MotorControlManager.h"
#include "Gui/Control/BoolValueSwitcherAction.h"
#include "Gui/Control/EventTriggerAction.h"
#include <QWhatsThis>
#include "Gui/HtmlInfoWidget/HtmlInfoWidget.h"
#include "Gui/AboutWidget/AboutInfoWidget.h"
#include <QIcon>
#include <iostream>
#include "NerdConstants.h"
#include "SimulationConstants.h"
#include "Gui/Control/NextStepTriggerAction.h"
#include "Util/SetSnapshotValueTask.h"
#include "Gui/GuiManager.h"


using namespace std;

namespace nerd{



GuiMainWindow::GuiMainWindow(bool simulationIsControllable, bool enableDebugging) 
	: mVisualization(0), mControlArea(0), mWidgetLayout(0), mMainMenuBar(0),
	  mEventList(0), mVisualizationChooser(0), mEventDetailPanel(0),
	  mValueDetailPanel(0), mDebugLoggerPanel(0), mValueLoggerPanel(0),
	  mMotorControlGui(0), mParameterLists(0), mShutDownTriggered(false),
	  mWindowToggleState(true)
{
	GuiManager::getGlobalGuiManager()->addWidget(NerdConstants::GUI_MAIN_SIMULATION_WINDOW, this);

	setup(simulationIsControllable, enableDebugging);
}

GuiMainWindow::~GuiMainWindow() {

	delete mParameterLists;
	delete mVisualizationChooser;
// 	delete mEventList;
	while(mPluginWindows.size() > 0) {
		QWidget *tmp = mPluginWindows.at(0);
		mPluginWindows.remove(0);

		SystemObject *sysOb = dynamic_cast<SystemObject*>(tmp);
		if(sysOb != 0 && Core::getInstance()->getSystemObjects().contains(sysOb)) {
			//DO NOT DELETE tmp !
		}
		else {
			delete tmp;
		}
	}
	while(mViewList.size() > 0) {
		QWidget *tmp = mViewList.at(0);
		mViewList.removeFirst();

		SystemObject *sysOb = dynamic_cast<SystemObject*>(tmp);
		if(sysOb != 0 && Core::getInstance()->getSystemObjects().contains(sysOb)) {
			//DO NOT DELETE tmp !
		}
		else {
			delete tmp;
		}
	}

	QList<QMenu*> menusToDestroy = mMenuLookup.values();
	while(!menusToDestroy.empty()) {
		QMenu *menu = menusToDestroy.at(0);
		menusToDestroy.removeAll(menu);
		delete menu;
	}
}

QString GuiMainWindow::getIconName() {
	return "orcsLogo.png";
}

void GuiMainWindow::setup(bool openGLControllable, bool enableDebugging) {

	CommandLineArgument *windowTitleArg = new CommandLineArgument("setTitle", "setTitle", "", 
						"Sets the title of the main application.", 1, 10, false, false);
	if(windowTitleArg->getNumberOfEntries() > 0) {
		QStringList titleFragments = windowTitleArg->getEntryParameters(0);
		QString title = "";
		for(QListIterator<QString> i(titleFragments); i.hasNext();) {
			title += i.next();
			if(i.hasNext()) {
				title += " ";
			}
		}
		setWindowTitle(title);
	}
	else {
		setWindowTitle("NERD");
	}
	QPixmap image;
	QImage jpgImage;

	QString name = QString(":/resources").append("/pics/")
								.append(GuiMainWindow::getIconName());
	if(!jpgImage.load(name)) {
		Core::log("Could not load Window-Icon");
	}else{
		image = QPixmap::fromImage(jpgImage);
		QIcon orcsIcon(image);
		setWindowIcon(orcsIcon);
	}
	setAttribute(Qt::WA_QuitOnClose, false);
	resize(400,400);

	mShutDownTriggered = false;

	//create main widgets
	mVisualization = new OpenGLVisualization(true, 0, "Main", true);
	mVisualization->getParameter("DisplaySimulationTime")->setValueFromString("true");
	mViewList.push_back(mVisualization);
	mControlArea = new ControlWindow();

	//Setup layout
	mWidgetLayout = new QGridLayout();
	setLayout(mWidgetLayout);

	mWidgetLayout->addWidget(mVisualization, 1, 0);
	mWidgetLayout->addWidget(mControlArea, 2, 0);
	mWidgetLayout->setRowStretch(1, 1);
	mWidgetLayout->setRowStretch(2, 0);
	mWidgetLayout->setColumnStretch(0, 1);
	setAutoFillBackground(true);

	//Create standard menu bar
	mMainMenuBar = new QMenuBar();
	mWidgetLayout->addWidget(mMainMenuBar, 0,0);

	if(openGLControllable) {
		createBasicControlMenu();
	}

	QMenu *toolMenu = getMenu(tr("Tools"));

	QMenu *viewMenu = getMenu(tr("View"));

	if(enableDebugging) {
		QMenu *debugMenu = getMenu(QString("Debug"));

		//Add event panel.
		mEventList = new EventVisualizationWindow();
		QAction *eventListAction = debugMenu->addAction(tr("&Event Observer"));
		connect(eventListAction, SIGNAL(triggered()), mEventList, SLOT(showWindow()));

		//Add debug logger panel
		mDebugLoggerPanel = new DebugLoggerPanel();
		QAction *debugLoggerAction = debugMenu->addAction(tr("Debug Logger Panel"));
		connect(debugLoggerAction, SIGNAL(triggered()), mDebugLoggerPanel, SLOT(showWindow()));

		mValueLoggerPanel = new DebugLoggerPanel(0, "", "Value Logger", true);
		QAction *valueLoggerAction = debugMenu->addAction(tr("Value Logger Panel"));
		connect(valueLoggerAction, SIGNAL(triggered()), mValueLoggerPanel, SLOT(showWindow()));

		//Add event detail panel
		mEventDetailPanel = new EventDetailPanel();
		QAction *eventDetailAction = debugMenu->addAction(tr("Event Detail Panel"));
		connect(eventDetailAction, SIGNAL(triggered()), mEventDetailPanel, SLOT(showWindow()));

		//Add value detail panel
		mValueDetailPanel = new ValueDetailPanel();
		QAction *valueDetailAction = debugMenu->addAction(tr("Value Detail Panel"));
		connect(valueDetailAction, SIGNAL(triggered()), mValueDetailPanel, SLOT(showWindow()));
	}

	QMenu *helpMenu = getMenu(tr("Help"));

	//Add object property panel.
	mParameterLists = new MultipleParameterWindowWidget("MainParameterWindow", new SetSnapshotValueTask(0, ""));
	QAction *parameterListAction = toolMenu->addAction(tr("&Object Properties"));
	parameterListAction->setShortcut(tr("Ctrl+o"));
	connect(parameterListAction, SIGNAL(triggered()), mParameterLists, SLOT(show()));

	//Add visualization chooser panel.
	mVisualizationChooser = new VisualizationChooser();
	QAction *visuChooserAction = viewMenu->addAction(tr("&Camera Manager"));
	visuChooserAction->setShortcut(tr("Ctrl+c"));
	visuChooserAction->setWhatsThis("Allows the creation and management of additional "
									"camera viewpoints of the simulation.");
	connect(visuChooserAction, SIGNAL(triggered()), mVisualizationChooser, SLOT(showWindow()));

	//add help window.
	HtmlInfoWidget *helpWindow = new HtmlInfoWidget(":/resources/doc/NERDHelp.html", "Help");
	mPluginWindows.append(helpWindow);
	helpWindow->getAction()->setShortcut(tr("F1"));
	helpMenu->addAction(helpWindow->getAction());

	helpMenu->addAction(QWhatsThis::createAction());

	helpMenu->addSeparator();

	AboutInfoWidget *aboutWindow = new AboutInfoWidget(tr("About NERD"));
	mPluginWindows.append(aboutWindow);
	helpMenu->addAction(aboutWindow->getAction());

	mToggleWindowArgument = new CommandLineArgument("toggle", "toggle", "", 
						"Makes the OpenGL windows toggle.", 0, 0, true, false);
}


void GuiMainWindow::createBasicControlMenu() {
	QMenu *controlMenu = getMenu(tr("Control"));

	//Add control slider
	mMotorControlGui = new MotorControlManager();
	QAction *showSliderWindow = controlMenu->addAction(tr("&Motor Control Manager"));
	showSliderWindow->setShortcut(tr("Ctrl+m"));
	connect(showSliderWindow, SIGNAL(triggered()),
			mMotorControlGui, SLOT(changeVisibility()));

	//Add bypass neurocontroller checkbox
	BoolValueSwitcherAction *bypassNetworksAction =
			new BoolValueSwitcherAction("Bypass NeuroControllers", 
							SimulationConstants::VALUE_NNM_BYPASS_NETWORKS);
	bypassNetworksAction->setWhatsThis("Enables or disables the NeuroControllers.");
	controlMenu->addAction(bypassNetworksAction);

	controlMenu->addSeparator();

	//Add realtime checkbox
	QAction *realTimeAction = new BoolValueSwitcherAction("Realtime", SimulationConstants::VALUE_RUN_SIMULATION_IN_REALTIME);
	realTimeAction->setWhatsThis("Toggles realtime mode on and off. In realtime mode "
								"the system tries to hold an update frequency based on the "
								"current step size to simulate the agents in realtime.");
	controlMenu->addAction(realTimeAction);

	//Add performance mode checkbox
	QAction *performanceAction = new BoolValueSwitcherAction("Per&formance Mode",
 								NerdConstants::VALUE_RUN_IN_PERFORMANCE_MODE);
	performanceAction->setWhatsThis("Toggles performance mode on and off. "
								"In performance mode all non-essential calcualtions and "
								"visualizations are disabled to maximize simulation performance.");
	performanceAction->setShortcut(tr("Ctrl+f"));
	controlMenu->addAction(performanceAction);

	controlMenu->addSeparator();

	//Add pause checkbox
	BoolValueSwitcherAction *pauseAction =
			new BoolValueSwitcherAction("&Pause", SimulationConstants::VALUE_EXECUTION_PAUSE);
	pauseAction->setShortcut(tr("Ctrl+p"));
	pauseAction->setWhatsThis("Pauses and resumes the simulation.");
	controlMenu->addAction(pauseAction);

	//Add next step trigger
	NextStepTriggerAction *nextStepAction = 
			new NextStepTriggerAction("&Next Step");
	nextStepAction->setShortcut(tr("Ctrl+n"));
	nextStepAction->setWhatsThis("If the simulator is paused, then a single simulation step "
								"can be executed with this action.");

	controlMenu->addAction(nextStepAction);

	controlMenu->addSeparator();

	//Add reset trigger
	QList<QString> resetEvents;
	resetEvents.append(NerdConstants::EVENT_EXECUTION_RESET);
	resetEvents.append(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED);
	EventTriggerAction *resetAction =
			new EventTriggerAction("&Reset Simulation", resetEvents);
	resetAction->setShortcut(tr("Ctrl+r"));
	resetAction->setWhatsThis("Resets the simulation to the initial conditions.");
	controlMenu->addAction(resetAction);
}


void GuiMainWindow::closeEvent(QCloseEvent*) {
	if(!mShutDownTriggered) {
		mShutDownTriggered = true;
		Core::getInstance()->scheduleTask(new ShutDownTask());
	}
}

void GuiMainWindow::toggleTimerExpired() {
	if(mWindowToggleState) {
		move(pos() + QPoint(1, 0));
	}
	else {
		move(pos() + QPoint(-1, 0));
	}
	mWindowToggleState = !mWindowToggleState;
}

QMenuBar* GuiMainWindow::getMenuBar() {
	return mMainMenuBar;
}

QMenu* GuiMainWindow::getMenu(const QString &name) {
	if(mMenuLookup.contains(name)) {
		return mMenuLookup.value(name);
	}
	else {
		QMenu *menu = mMainMenuBar->addMenu(name);
		mMenuLookup[name] = menu;
		return menu;
	}
}

QPushButton* GuiMainWindow::addButton(const QString &buttonName, QWidget *newControlWindow) {
	mPluginWindows.push_back(newControlWindow);
	return mControlArea->addButton(buttonName);
}

QMenu* GuiMainWindow::addMenu(const QString &menuName, QWidget *newControlWindow) {
	mPluginWindows.push_back(newControlWindow);
	if(mMenuLookup.contains(menuName)) {
		return mMenuLookup.value(menuName);
	}else{
		return mMainMenuBar->addMenu(menuName);
	}
}

void GuiMainWindow::addWidget(QWidget *newView) {
 	int size = mViewList.size();
	mWidgetLayout->addWidget(newView, size + 1, 0);
	mWidgetLayout->setRowStretch(size + 1, 0);
	mViewList.push_back(newView);
	newView->setParent(this);
	//resize((size+1)*400,400);
}

void GuiMainWindow::showWindow() {
	if(mVisualization != 0) {
		mVisualization->resetViewport();
	}
	show();
	//mWindowToggleTimer.setInterval(100);
	if(mToggleWindowArgument->getNumberOfEntries() > 0) {
		mWindowToggleTimer.setInterval(100);
		mWindowToggleTimer.start(100);
		connect(&mWindowToggleTimer, SIGNAL(timeout()),
				this, SLOT(toggleTimerExpired()));
	}
}

}
