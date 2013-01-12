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

#include "MultipleParameterWindowWidget.h"
#include "Gui/Parameter/ParameterVisualizationWindow.h"
#include <iostream>
#include "Core/Core.h"
#include "Core/Properties.h"
#include "Core/Task.h"
#include <QDir>
#include <QStringList>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QTabWidget>
#include <QFile>
#include <QDir>
#include <QDate>
#include <QTime>
#include <QMessageBox>
#include "Gui/Parameter/ParameterVisualization.h"
#include <QWhatsThis>

using namespace std;

namespace nerd {

MultipleParameterWindowWidget::MultipleParameterWindowWidget(const QString &name,
					SetInitValueTask *setInitValueTaskPrototpye, QWidget *parent)
		: MultipleWindowWidget(name, parent), mCounter(1),
		  mSetInitValueTaskPrototype(setInitValueTaskPrototpye)
{
	setWindowTitle("Object Properties");

	resize(200, 500);
	addButtonPressed();

	connect(this, SIGNAL(initPhaseCompleted()), this, SLOT(handleInitPhase()));
	connect(this, SIGNAL(shutDownPhaseStarted()), this, SLOT(handleShutDownPhase()));

	QMenuBar *localMenuBar = menuBar();

	QMenu *fileMenu = localMenuBar->addMenu("File");
	QAction *saveSinglePanelsAction = fileMenu->addAction("Save Panel");
	connect(saveSinglePanelsAction, SIGNAL(triggered()),
			this, SLOT(saveSinglePanelToFile()));
	
	QAction *loadSinglePanelsAction = fileMenu->addAction("Load Panel");
	connect(loadSinglePanelsAction, SIGNAL(triggered()),
			this, SLOT(loadSinglePanelFromFile()));
	
	fileMenu->addSeparator();
	
	QAction *saveAllPanelsAction = fileMenu->addAction("Save All Panels");
	connect(saveAllPanelsAction, SIGNAL(triggered()),
			this, SLOT(saveAllPanelsToFile()));

	QAction *loadAllPanelsAction = fileMenu->addAction("Load All Panels");
	connect(loadAllPanelsAction, SIGNAL(triggered()),
			this, SLOT(loadAllPanelsFromFile()));

	QMenu *editMenu = localMenuBar->addMenu("Edit");
	
	QAction *selectSingleAction = editMenu->addAction("Select Panel");
	connect(selectSingleAction, SIGNAL(triggered()),
			this, SLOT(selectSinglePanel()));
	
	QAction *applySingleAction = editMenu->addAction("Apply Panel");
	connect(applySingleAction, SIGNAL(triggered()),
			this, SLOT(applySinglePanel()));
	
	QAction *clearSingleAction = editMenu->addAction("Clear Panel");
	connect(clearSingleAction, SIGNAL(triggered()),
			this, SLOT(clearSinglePanel()));
	
	editMenu->addSeparator();
	
	QAction *applyAllAction = editMenu->addAction("Apply All in All Panels");
	connect(applyAllAction, SIGNAL(triggered()),
			this, SLOT(applyAllProperties()));

	QAction *applyAndSelectAllAction = editMenu->addAction("Apply and Select All in All Panels");
	connect(applyAndSelectAllAction, SIGNAL(triggered()),
			this, SLOT(applyAndSelectAllProperties()));

	QAction *selectAllAction = editMenu->addAction("Select All in All Panels");
	connect(selectAllAction, SIGNAL(triggered()),
			this, SLOT(selectAllProperties()));

	QAction *deselectAllAction = editMenu->addAction("Deselect All in All Panels");
	connect(deselectAllAction, SIGNAL(triggered()),
			this, SLOT(deselectAllProperties()));

	editMenu->addSeparator();

	QAction *copyToAllValuesAction = editMenu->addAction("Copy First To All");
	connect(copyToAllValuesAction, SIGNAL(triggered()),
			this, SLOT(copyFirstValueToAllOtherValues()));

	QAction *setInitForAllValuesAction = editMenu->addAction("Set Init To All");
	connect(setInitForAllValuesAction, SIGNAL(triggered()),
			this, SLOT(setInitToAllValues()));

	
	QMenu *plotterMenu = localMenuBar->addMenu("Plotter");
	
	QAction *activateHistoryPlotter = plotterMenu->addAction("History Plotter");
	connect(activateHistoryPlotter, SIGNAL(triggered()),
			this, SLOT(activateHistoryPlotter()));
	
	//QAction *activateXYPlotter = plotterMenu->addAction("X/Y Plotter");
	//connect(activateXYPlotter, SIGNAL(triggered()),
	//		this, SLOT(activateXYPlotter()));
	
	
    QMenu *helpMenu = localMenuBar->addMenu("Help");
    helpMenu->addAction(QWhatsThis::createAction());
	
	

}

MultipleParameterWindowWidget::~MultipleParameterWindowWidget() {
	delete mSetInitValueTaskPrototype;
}

void MultipleParameterWindowWidget::addTab() {
	addNewVisualizationTab();
}

ParameterVisualizationWindow* MultipleParameterWindowWidget::addNewVisualizationTab() {
	QString name = QString("Properties [")
						.append(QString::number(mCounter++))
						.append("]");
	ParameterVisualizationWindow *pvw = new ParameterVisualizationWindow(name, mSetInitValueTaskPrototype);
	getTabWidget()->addTab(pvw, name);

	updateButtons();

	return pvw;
}



void MultipleParameterWindowWidget::eventOccured(Event *event) {
	MultipleWindowWidget::eventOccured(event);
	if(event == 0) {
		return;
	}

	if(event == mInitCompletedEvent) {
		emit initPhaseCompleted();
	}
	else if(event == mShutDownEvent) {
		emit shutDownPhaseStarted();
	}
}

void MultipleParameterWindowWidget::handleInitPhase() {

	QString path = Core::getInstance()->getConfigDirectoryPath() + "/properties";
	QDir configDir(path);
	QStringList filter;
	filter << (mName + "_*.val");
	configDir.setNameFilters(filter);

	QStringList tabValueFiles = configDir.entryList();

	for(int i = 0; i < tabValueFiles.size(); ++i) {
		QString fileName = tabValueFiles.at(i);

		QFile file(path + "/" + fileName);
		if(file.exists()) {
			ParameterVisualizationWindow *pvw = 0;
			if(i == 0) {
				pvw = dynamic_cast<ParameterVisualizationWindow*>(getTabWidget()->widget(0));
			}
			if(pvw == 0) {
			 	pvw = addNewVisualizationTab();
			}
			QString missing = pvw->loadParametersFromFile(file.fileName(), false);
		}
	}
}


void MultipleParameterWindowWidget::saveAllPanelsToFile() {

	//do not write anything if reduced file writing mode is active.
// 	if(Core::getInstance()->isUsingReducedFileWriting()) {
// 		return;
// 	}

	Properties &props = Core::getInstance()->getProperties();

	QString fileChooserDirectory = ".";
	if(props.hasProperty("CurrentFileChooserPath")) {
		fileChooserDirectory = props.getProperty("CurrentFileChooserPath");
	}

	QString fileName = QFileDialog::getSaveFileName(this,
       tr("Save settings"), fileChooserDirectory);

	if(fileName == "") {
		return;
	}
	fileChooserDirectory = fileName.mid(0, fileName.lastIndexOf("/"));
	props.setProperty("CurrentFileChooserPath", fileChooserDirectory);

	if(!fileName.endsWith(".val")) {
		fileName.append(".val");
	}

	QFile file(fileName);

	int dirIndex = fileName.lastIndexOf("/");
	if(dirIndex > 0) {
		Core::getInstance()->enforceDirectoryPath(file.fileName().mid(0, dirIndex));
	}

	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		Core::log(QString("Could not open file ").append(fileName).append(" to write Values."));
		file.close();
		return;
	}

	QTextStream output(&file);
	output << "#NERD Values" << "\n";
	output << "#Saved at: " << QDate::currentDate().toString("dd.MM.yyyy")
					<< "  " << QTime::currentTime().toString("hh:mm:ss") << "\n";

	for(int i = 0; i < mTabWidget->count(); ++i) {
		ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(
				mTabWidget->widget(i));

		if(visu == 0) {
			continue;
		}

		QString newFileName = fileName;
		newFileName = newFileName.mid(0, fileName.size() - 4)
						.append("_").append(QString::number(i)).append(".val");

		output << "#include " << newFileName << endl;

		visu->saveCurrentParameters(newFileName, true);
	}
	file.close();
}

void MultipleParameterWindowWidget::saveSinglePanelToFile() {
	ParameterVisualizationWindow *currentVisu = dynamic_cast<ParameterVisualizationWindow*>(mTabWidget->currentWidget());
	if(currentVisu != 0) {
			currentVisu->saveSettings();
	}
}






void MultipleParameterWindowWidget::loadAllPanelsFromFile() {
	Properties &props = Core::getInstance()->getProperties();
	QString fileChooserDirectory = ".";
	if(props.hasProperty("CurrentFileChooserPath")) {
		fileChooserDirectory = props.getProperty("CurrentFileChooserPath");
	}

	QString fileName = QFileDialog::getOpenFileName(this, tr("Load Settings"),
							fileChooserDirectory);
	if(fileName == "") {
		return;
	}
	fileChooserDirectory = fileName.mid(0, fileName.lastIndexOf("/"));
	props.setProperty("CurrentFileChooserPath", fileChooserDirectory);


	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		file.close();
		return;
	}

	QTextStream input(&file);

	QString line = "";
	int tabCounter = 0;
	QString missingValues = "";

	while(!input.atEnd()) {

		line = input.readLine();
		if(line.startsWith("#include")) {

			if(tabCounter >= mTabWidget->count()) {
				addTab();
			}

			ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(
					mTabWidget->widget(tabCounter));

			if(visu == 0) {
				continue;
			}

			QString subFileName = line.mid(9);

			missingValues += visu->loadParametersFromFile(subFileName, false);

			tabCounter++;
		}
	}

	while(mTabWidget->count() > tabCounter + 1) {
		mTabWidget->removeTab(mTabWidget->count() - 1);
	}

	if(missingValues != "") {
		QMessageBox::warning(this, "Problem while loading!",
			"Could not find the following Values:\n\n" + missingValues, QMessageBox::Ok, 0, 0);
	}
	file.close();
}

void MultipleParameterWindowWidget::loadSinglePanelFromFile() {
	ParameterVisualizationWindow *currentVisu = dynamic_cast<ParameterVisualizationWindow*>(mTabWidget->currentWidget());
	if(currentVisu != 0) {
		currentVisu->loadSettings();
	}
}


void MultipleParameterWindowWidget::addPanelsFromFile() {

}


void MultipleParameterWindowWidget::applyAllProperties() {

	for(int i = 0; i < mTabWidget->count(); ++i) {
		ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(
					mTabWidget->widget(i));

		if(visu == 0) {
			continue;
		}

		visu->applyAllChanges();
	}
}


void MultipleParameterWindowWidget::applyAndSelectAllProperties() {

	for(int i = 0; i < mTabWidget->count(); ++i) {
		ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(
					mTabWidget->widget(i));

		if(visu == 0) {
			continue;
		}

		visu->applyAllChanges();
		visu->modifySelection(true);
	}
}


void MultipleParameterWindowWidget::selectAllProperties() {
	for(int i = 0; i < mTabWidget->count(); ++i) {
		ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(
					mTabWidget->widget(i));

		if(visu == 0) {
			continue;
		}
		visu->modifySelection(true);
	}
}


void MultipleParameterWindowWidget::deselectAllProperties() {
	for(int i = 0; i < mTabWidget->count(); ++i) {
		ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(
					mTabWidget->widget(i));

		if(visu == 0) {
			continue;
		}
		visu->modifySelection(false);
	}
}


void MultipleParameterWindowWidget::copyFirstValueToAllOtherValues() {
	ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(
					mTabWidget->currentWidget());
	if(visu != 0) {
		QList<ParameterVisualization*> params = visu->getParameterVisualizations();
		if(params.empty()) {
			return;
		}
		QString firstParam = params.at(0)->getCurrentValue();
		params.removeFirst();
		for(QListIterator<ParameterVisualization*> i(params); i.hasNext();) {
			i.next()->setCurrentValue(firstParam);
		}
	}
}

void MultipleParameterWindowWidget::setInitToAllValues()  {
	ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(
					mTabWidget->currentWidget());

	if(visu != 0) {
		visu->applyAllChanges();

		QList<ParameterVisualization*> params = visu->getParameterVisualizations();
		for(QListIterator<ParameterVisualization*> i(params); i.hasNext();) {
			i.next()->updateValueInEnvironmentManager();
		}
	}
}

void MultipleParameterWindowWidget::applySinglePanel() {
	
	ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(mTabWidget->currentWidget());
		
	if(visu == 0) {
		return;
	}
	visu->applyAllChanges();
}


void MultipleParameterWindowWidget::selectSinglePanel() {
	
	ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(mTabWidget->currentWidget());
	
	if(visu == 0) {
		return;
	}
	visu->modifySelection(true);
}


void MultipleParameterWindowWidget::clearSinglePanel() {
	
	ParameterVisualizationWindow *visu = dynamic_cast<ParameterVisualizationWindow*>(mTabWidget->currentWidget());
	
	if(visu == 0) {
		return;
	}
	visu->deleteValuesFromList();
}



void MultipleParameterWindowWidget::activateHistoryPlotter() {
	ParameterVisualizationWindow *currentVisu = dynamic_cast<ParameterVisualizationWindow*>(mTabWidget->currentWidget());
	if(currentVisu != 0) {
		currentVisu->showAndUpdateHistoryPlotter(true);
	}
}


void MultipleParameterWindowWidget::activateXYPlotter() {
	ParameterVisualizationWindow *currentVisu = dynamic_cast<ParameterVisualizationWindow*>(mTabWidget->currentWidget());
	if(currentVisu != 0) {
		currentVisu->showAndUpdateXYPlotter(true);
	}
}




void MultipleParameterWindowWidget::handleShutDownPhase() {

	//do not write parameter files when in reduced file writing mode.
	if(Core::getInstance()->isUsingReducedFileWriting()) {
		return;
	}

	QString path = Core::getInstance()->getConfigDirectoryPath() + "/properties";
	QDir configDir(path);
	QStringList filter;
	filter << (mName + "_*.val");
	configDir.setNameFilters(filter);

	QStringList tabValueFiles = configDir.entryList();

	//erase all previously saved files.
	for(int i = 0; i < tabValueFiles.size(); ++i) {
		QString fileName = tabValueFiles.at(i);

		QFile file(path + "/" + fileName);
		if(file.exists()) {
			file.remove();
		}
	}

	for(int i = 0; i < getTabWidget()->count(); ++i) {
		ParameterVisualizationWindow *pvw = dynamic_cast<ParameterVisualizationWindow*>(
				getTabWidget()->widget(i));
		if(pvw != 0) {
			pvw->saveCurrentParameters(path + "/" + mName + "_" + QString::number(i) + ".val", false);
		}
	}
}





void MultipleParameterWindowWidget::timerEvent(QTimerEvent *) {
	show();
}

}


