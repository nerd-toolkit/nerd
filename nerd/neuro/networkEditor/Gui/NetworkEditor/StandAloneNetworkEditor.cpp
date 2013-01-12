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



#include "StandAloneNetworkEditor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>

using namespace std;

namespace nerd {


/**
 * Constructs a new StandAloneNetworkEditor.
 */
StandAloneNetworkEditor::StandAloneNetworkEditor(QWidget *parent)
	: NeuralNetworkEditor(parent), mShutDownTriggered(false)
{
}


/**
 * Destructor.
 */
StandAloneNetworkEditor::~StandAloneNetworkEditor() {
}


void StandAloneNetworkEditor::setupMenuBar() {
	addFileMenu();
	QMenu *editMenu = addEditMenu();
	extendEditMenu(editMenu);
	addPlotterMenu();
	addHelpMenu();
}

QMenu* StandAloneNetworkEditor::addFileMenu() {
	QMenuBar *mainMenu = menuBar();

	//File menu
	QMenu *fileMenu = new QMenu("File", mainMenu);

	QAction *loadNetworkAction = fileMenu->addAction("Load Network");
	connect(loadNetworkAction, SIGNAL(triggered()),
			this, SLOT(loadNetwork()));

// 	QAction *saveNetworkAction = fileMenu->addAction("Save Network");
// 	connect(saveNetworkAction, SIGNAL(triggered()),
// 			this, SLOT(saveNetwork()));

	fileMenu->addSeparator();

	QAction *addSubNetworkAction = fileMenu->addAction("Add Subnetwork");
	connect(addSubNetworkAction, SIGNAL(triggered()),
			this, SLOT(addSubNetwork()));

	QAction *saveSubNetworkAction = fileMenu->addAction("Save Subnetwork");
	connect(saveSubNetworkAction, SIGNAL(triggered()),
			this, SLOT(saveSubNetwork()));

	fileMenu->addSeparator();

	QAction *loadLayoutAction = fileMenu->addAction("Load Layout");
	connect(loadLayoutAction, SIGNAL(triggered()),
			this, SLOT(loadLayout()));

	QAction *saveLayoutAction = fileMenu->addAction("Save Layout");
	connect(saveLayoutAction, SIGNAL(triggered()),
			this, SLOT(saveLayout()));

	fileMenu->addSeparator();
	
	QMenu *exportMenu = fileMenu->addMenu("Export as");

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
	

	fileMenu->addSeparator();

	QAction *createMinimalNetworkAction = fileMenu->addAction("Create Minimal Network");
	connect(createMinimalNetworkAction, SIGNAL(triggered()),
			this, SLOT(createMinimalNetwork()));

	fileMenu->addSeparator();

	QAction *createTabAction = fileMenu->addAction("Create Tab");
	connect(createTabAction, SIGNAL(triggered()),
			this, SLOT(createTab()));

	QAction *destroyTabAction = fileMenu->addAction("Destroy Current Tab");
	connect(destroyTabAction, SIGNAL(triggered()),
			this, SLOT(destroyTab()));

	mainMenu->addMenu(fileMenu);
	return fileMenu;
}

void StandAloneNetworkEditor::extendEditMenu(QMenu *editMenu) {
	if(editMenu == 0) {
		return;
	}
}


void StandAloneNetworkEditor::setupTools() {
	NeuralNetworkEditor::setupTools();
}

void StandAloneNetworkEditor::closeEvent(QCloseEvent*) {
	if(!mShutDownTriggered) {
		mShutDownTriggered = true;
		Core::getInstance()->scheduleTask(new ShutDownTask());
	}
}

}



