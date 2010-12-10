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

#include "SimpleEvolutionMainWindow.h"
#include "Core/Core.h"
#include "Gui/Control/BoolValueSwitcherAction.h"
#include "EvolutionConstants.h"
#include <QDir>


namespace nerd {

SimpleEvolutionMainWindow::SimpleEvolutionMainWindow(bool enableDebugging) 
		: mWidgetLayout(0), mMainMenuBar(0), mEvolutionLoggerPanel(0), 
		mDebugLoggerPanel(0), mShutDownTriggered(false) 
{
	setup(enableDebugging);
}

SimpleEvolutionMainWindow::~SimpleEvolutionMainWindow()  {
	delete mOverview;
	delete mDebugLoggerPanel;
	delete mEvolutionLoggerPanel;
	delete mEventDetailPanel;
	delete mValueDetailPanel;
	delete mParameterLists;

	QList<QMenu*> menusToDestroy = mMenuLookup.values();
	while(!menusToDestroy.empty()) {
		QMenu *menu = menusToDestroy.at(0);
		menusToDestroy.removeAll(menu);
		delete menu;
	}
}

QMenu* SimpleEvolutionMainWindow::getMenu(const QString &name) {
	if(mMenuLookup.contains(name)) {
		return mMenuLookup.value(name);
	}
	else {
		QMenu *menu = mMainMenuBar->addMenu(name);
		mMenuLookup[name] = menu;
		return menu;
	}
}


void SimpleEvolutionMainWindow::setup(bool enableDebugging) {

	setAttribute(Qt::WA_QuitOnClose, false);
	setWindowTitle("Cluster-Evo [" + QDir::current().dirName() + "]");

	resize(300, 50);

	//Setup layout
	mWidgetLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	setLayout(mWidgetLayout);

	setAutoFillBackground(true);
	mWidgetLayout->setSpacing(0);
	mWidgetLayout->setAlignment(Qt::AlignLeft);

	//Create standard menu bar
	mMainMenuBar = new QMenuBar();
	mWidgetLayout->addWidget(mMainMenuBar);

	QMenu *toolMenu = getMenu(tr("Tools"));

	//Add evolution logger panel
	mEvolutionLoggerPanel = new DebugLoggerPanel(0, 
		"/Evaluation/EvaluationStatus", "EvaluationStatus", false, true);
	QAction *evolutionLogger = toolMenu->addAction(tr("Evolution Logger Panel"));
	connect(evolutionLogger, SIGNAL(triggered()), mEvolutionLoggerPanel, 
		SLOT(showWindow()));

	//Add object property panel.
	mParameterLists = new MultipleParameterWindowWidget("MainParameterWindow");
	QAction *parameterListAction = toolMenu->addAction(tr("&Object Properties"));
	parameterListAction->setShortcut(tr("Ctrl+o"));
	connect(parameterListAction, SIGNAL(triggered()), mParameterLists, SLOT(show()));

	if(enableDebugging) {
		QMenu *debugMenu = getMenu(QString("Debug"));

		//Add event panel.
		mEventList = new EventVisualizationWindow();
		QAction *eventListAction = debugMenu->addAction(tr("&Event Observer"));
		connect(eventListAction, SIGNAL(triggered()), mEventList, SLOT(showWindow()));

		//Add debug logger panel
		mDebugLoggerPanel = new DebugLoggerPanel();
		QAction *debugLoggerAction = debugMenu->addAction(tr("Debug Logger Panel"));
		connect(debugLoggerAction, SIGNAL(triggered()), mDebugLoggerPanel, 
			SLOT(showWindow()));

		//Add event detail panel
		mEventDetailPanel = new EventDetailPanel();
		QAction *eventDetailAction = debugMenu->addAction(tr("Event Detail Panel"));
		connect(eventDetailAction, SIGNAL(triggered()), mEventDetailPanel, 
			SLOT(showWindow()));

		//Add value detail panel
		mValueDetailPanel = new ValueDetailPanel();
		QAction *valueDetailAction = debugMenu->addAction(tr("Value Detail Panel"));
		connect(valueDetailAction, SIGNAL(triggered()), mValueDetailPanel, 
			SLOT(showWindow()));
	}


	

	//Add population overview panel.
	mOverview = new MultiplePopulationOverviewWindowWidget();
	getMenu("Evolution")->addAction(mOverview->getAction());
	mOverview->getAction()->setShortcut(tr("Ctrl+Shift+p"));

	BoolValueSwitcherAction *runEvolutionButton = new BoolValueSwitcherAction("&Run Evolution",
			EvolutionConstants::VALUE_EVO_RUN_EVOLUTION);
	runEvolutionButton->setShortcut(tr("Ctrl+e"));
	getMenu("Evolution")->addAction(runEvolutionButton);

	mProgressBar = new EvolutionProgressBar();
	mWidgetLayout->addWidget(mProgressBar);
}


void SimpleEvolutionMainWindow::timerEvent(QTimerEvent *) {
	show();
}

void SimpleEvolutionMainWindow::closeEvent(QCloseEvent*) {
	if(!mShutDownTriggered) {
		mShutDownTriggered = true;
		Core::getInstance()->scheduleTask(new ShutDownTask());
	}
}

}
