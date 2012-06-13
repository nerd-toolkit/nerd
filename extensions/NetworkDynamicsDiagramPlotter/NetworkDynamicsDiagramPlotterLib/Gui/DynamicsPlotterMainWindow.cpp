/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   till.faber@uni-osnabrueck.de
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

#include "DynamicsPlotterMainWindow.h"
#include "Core/Core.h"
#include "Gui/Control/BoolValueSwitcherAction.h"
#include "Gui/InternalDiagramPlotter/AddInternalDiagramPlotterAction.h"
#include <QDir>
#include <PlugIns/CommandLineArgument.h>
#include "Gui/Visualization/OpenGLVisualization.h"
#include <QGridLayout>

namespace nerd {

DynamicsPlotterMainWindow::DynamicsPlotterMainWindow(bool enableSimulator, bool enableDebugging) 
		: mWidgetLayout(0), mMainMenuBar(0), 
		mDebugLoggerPanel(0), mVisualization(0), mShutDownTriggered(false) 
{
	setup(enableSimulator, enableDebugging);
}

DynamicsPlotterMainWindow::~DynamicsPlotterMainWindow()  {
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

QMenu* DynamicsPlotterMainWindow::getMenu(const QString &name) {
	if(mMenuLookup.contains(name)) {
		return mMenuLookup.value(name);
	}
	else {
		QMenu *menu = mMainMenuBar->addMenu(name);
		mMenuLookup[name] = menu;
		return menu;
	}
}


void DynamicsPlotterMainWindow::setup(bool enableSimulator, bool enableDebugging) {

	setAttribute(Qt::WA_QuitOnClose, false);
	setWindowTitle("Neural Network Dynamics Plotter");
	
	if(enableSimulator) {
		resize(400,400);
	}
	else {
		resize(300, 50);
	}

	//Setup layout
	mWidgetLayout = new QGridLayout();
	setLayout(mWidgetLayout);
	
	setAutoFillBackground(true);
	mWidgetLayout->setSpacing(0);
	mWidgetLayout->setAlignment(Qt::AlignLeft);
	mWidgetLayout->setColumnStretch(0, 1);

	//Create standard menu bar
	mMainMenuBar = new QMenuBar();
	mWidgetLayout->addWidget(mMainMenuBar, 0, 0);
	
	if(enableSimulator) {
		mVisualization = new OpenGLVisualization(true, 0, "Main", true);
		mWidgetLayout->addWidget(mVisualization, 1, 0);
		mWidgetLayout->setRowStretch(1, 1);
	}

	QMenu *toolMenu = getMenu(tr("Tools"));

	//Add object property panel.
	mParameterLists = new MultipleParameterWindowWidget("MainParameterWindow");
	QAction *parameterListAction = toolMenu->addAction(tr("&Object Properties"));
	parameterListAction->setShortcut(tr("Ctrl+o"));
	connect(parameterListAction, SIGNAL(triggered()), mParameterLists, SLOT(show()));
	
// 	AddInternalDiagramPlotterAction *addDiagramPlotterAction = 
// 						new AddInternalDiagramPlotterAction("Diagram Plotter", "Diagram",
// 									toolMenu);
// 	toolMenu->addAction(addDiagramPlotterAction);
	
	

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

}


void DynamicsPlotterMainWindow::timerEvent(QTimerEvent *) {
	show();
}

void DynamicsPlotterMainWindow::closeEvent(QCloseEvent*) {
	if(!mShutDownTriggered) {
		mShutDownTriggered = true;
		Core::getInstance()->scheduleTask(new ShutDownTask());
	}
}

}

