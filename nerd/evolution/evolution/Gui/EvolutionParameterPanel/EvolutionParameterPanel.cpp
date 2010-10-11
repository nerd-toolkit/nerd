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



#include "EvolutionParameterPanel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Gui/EvolutionParameterPanel/AlgorithmParameterPanel.h"
#include <QVBoxLayout>
#include "Gui/EvolutionParameterPanel/EvolutionMainControlParameterPanel.h"
#include "EvolutionConstants.h"
#include "Value/ChangeValueTask.h"
#include "Gui/PopulationOverview/MultiplePopulationOverviewWindowWidget.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/Evolution.h"
#include "Gui/ScriptedFitnessEditor/MultipleScriptedFitnessEditor.h"
#include <QFileDialog>
#include <QTime>
#include <QDate>
#include <QMenu>
#include <QMenuBar>
#include "Value/ChangeValueTask.h"
#include <QMessageBox>
#include "Gui/CommentWidget/CommentWidget.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new EvolutionParameterPanel.
 */
EvolutionParameterPanel::EvolutionParameterPanel()
	: QMainWindow(), mTabWidget(0), mRunEvolutionButton(0), mRestartGenerationButton(0), 
		mPauseEvaluationButton(0), mRunEvolutionValue(0), mRestartGenerationValue(0), 
		mPauseEvaluationValue(0)
{
	setWindowTitle("Evolution Parameter Panel");
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	QMenuBar *mainMenu = menuBar();

	//Edit Menu
	QMenu *fileMenu = mainMenu->addMenu("File");

	QAction *loadAction = fileMenu->addAction(tr("&Load Settings"));
	loadAction->setShortcut(tr("Ctrl+l"));
	connect(loadAction, SIGNAL(triggered()),
			this, SLOT(loadSettings()));
	
	QAction *saveAction = fileMenu->addAction(tr("&SaveSettings"));
	saveAction->setShortcut(tr("Ctrl+s"));
	connect(saveAction, SIGNAL(triggered()),
			this, SLOT(saveSettings()));


	connect(this, SIGNAL(initialize()),
			this, SLOT(initializePanel()));

	Core::getInstance()->addSystemObject(this);
}


/**
 * Destructor.
 */
EvolutionParameterPanel::~EvolutionParameterPanel() {
}


QString EvolutionParameterPanel::getName() const {
	return "EvolutionParameterPanel";
}


bool EvolutionParameterPanel::init() {
	bool ok = true;

	return ok;
}


bool EvolutionParameterPanel::bind() {
	bool ok = true;

	ValueManager *vm = Core::getInstance()->getValueManager();
	QList<QString> valueNames = vm->getValueNamesMatchingPattern("/Evo/Algorithm/.*/Description");

	for(QListIterator<QString> i(valueNames); i.hasNext();) {
		QString evoPath = i.next();
		evoPath = evoPath.mid(0, evoPath.size() - 11);
		mEvolutionAlgorithmPrefixes.append(evoPath);
	}

	mRunEvolutionValue = vm->getBoolValue(EvolutionConstants::VALUE_EVO_RUN_EVOLUTION);
	mRestartGenerationValue = vm->getBoolValue(EvolutionConstants::VALUE_EVO_RESTART_GENERATION);
	mPauseEvaluationValue = vm->getBoolValue(EvolutionConstants::VALUE_EXECUTION_PAUSE);

	if(mRunEvolutionValue != 0) {
		mRunEvolutionValue->addValueChangedListener(this);
	}
	else {
		Core::log("EvolutionMainControlParameterPanel: Could not find mRunEvolutionValue.", true);
	}
	if(mRestartGenerationValue != 0) {
		mRestartGenerationValue->addValueChangedListener(this);
	}
	else {
		Core::log("EvolutionMainControlParameterPanel: Could not find mRestartGenerationValue.", true);
	}
	if(mPauseEvaluationValue != 0) {
		mPauseEvaluationValue->addValueChangedListener(this);
	}
	else {
		Core::log("EvolutionMainControlParameterPanel: Could not find mPauseEvaluationValue.", true);
	}
	
	emit initialize();

	return ok;
}


bool EvolutionParameterPanel::cleanUp() {
	if(mRunEvolutionValue != 0) {
		mRunEvolutionValue->removeValueChangedListener(this);
		mRunEvolutionValue = 0;
	}
	if(mRestartGenerationValue != 0) {
		mRestartGenerationValue->removeValueChangedListener(this);
		mRestartGenerationValue = 0; 
	}
	if(mPauseEvaluationValue != 0) {
		mPauseEvaluationValue->removeValueChangedListener(this);
		mPauseEvaluationValue = 0;
	}
	Core::getInstance()->removeSystemObject(this);

	return true;
}


void EvolutionParameterPanel::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mRunEvolutionValue) {
		if(mRunEvolutionButton != 0) {
			mRunEvolutionButton->setChecked(mRunEvolutionValue->get());
		}
	}
	else if(value == mRestartGenerationValue) {
		if(mRestartGenerationButton != 0) {
			mRestartGenerationButton->setChecked(mRestartGenerationValue->get());
		}
	}
	else if(value == mPauseEvaluationValue) {
		if(mPauseEvaluationButton != 0) {
			mPauseEvaluationButton->setChecked(mPauseEvaluationValue->get());
		}
	}
}


void EvolutionParameterPanel::initializePanel() {

	//try to get all required events and values
	QWidget *mainWidget = new QWidget();
	setCentralWidget(mainWidget);

	QVBoxLayout *layout = new QVBoxLayout();
	mainWidget->setLayout(layout);


	mTabWidget = new QTabWidget(this);
	layout->addWidget(mTabWidget);

	//Button layout
	mRunEvolutionButton = new QPushButton("Run Evolution");
	mRunEvolutionButton->setCheckable(true);

	mRestartGenerationButton = new QPushButton("RestartGeneration");
	mRestartGenerationButton->setCheckable(true);
	
	mPauseEvaluationButton = new QPushButton("Pause");
	mPauseEvaluationButton->setCheckable(true);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(mRunEvolutionButton);
	buttonLayout->addWidget(mRestartGenerationButton);
	buttonLayout->addWidget(mPauseEvaluationButton);

	layout->addLayout(buttonLayout);

	connect(mRunEvolutionButton, SIGNAL(toggled(bool)),
			this, SLOT(runEvolutionButtonToggled(bool)));
	connect(mRestartGenerationButton, SIGNAL(toggled(bool)),
			this, SLOT(restartEvolutionButtonToggled(bool)));
	connect(mPauseEvaluationButton, SIGNAL(toggled(bool)),
			this, SLOT(pauseButtonToggled(bool)));

	//Create main control parameter panel.
	EvolutionMainControlParameterPanel *mainPanel = new EvolutionMainControlParameterPanel();
	mTabWidget->addTab(mainPanel, "Main");


	//Create one AlgorithmParameterPanel per detected algorithm.
	for(QListIterator<QString> i(mEvolutionAlgorithmPrefixes); i.hasNext();) {
		QString prefix = i.next();

		AlgorithmParameterPanel *panel = new AlgorithmParameterPanel(prefix);	
		mTabWidget->addTab(panel, prefix.mid(15, prefix.size() - 16));
	}

	MultiplePopulationOverviewWindowWidget *popOverview = new MultiplePopulationOverviewWindowWidget();
	popOverview->showOverview();
	popOverview->show();

	mTabWidget->addTab(new MultipleScriptedFitnessEditor("/Evo/Pop/.*/Fitness/.*/Code", true), "Fitness");

	CommentWidget *commentWidget = new CommentWidget();
	mTabWidget->addTab(commentWidget, "Journal");

	mTabWidget->addTab(popOverview, "Overview");

	if(mRunEvolutionValue != 0) {
		mRunEvolutionButton->setChecked(mRunEvolutionValue->get());
	}
	if(mPauseEvaluationValue != 0) {
		mPauseEvaluationButton->setChecked(mPauseEvaluationValue->get());
	}
}

void EvolutionParameterPanel::runEvolutionButtonToggled(bool) {
	if(mRunEvolutionValue == 0) {
		return;
	}
	if(mRunEvolutionValue->get() != mRunEvolutionButton->isChecked()) {
		Core::getInstance()->scheduleTask(new ChangeValueTask(mRunEvolutionValue, 
											mRunEvolutionButton->isChecked() ? "t" : "f"));
	}
}

void EvolutionParameterPanel::restartEvolutionButtonToggled(bool) {
	if(mRestartGenerationValue == 0) {
		return;
	}
	if(mRestartGenerationValue->get() != mRestartGenerationButton->isChecked()) {
		Core::getInstance()->scheduleTask(new ChangeValueTask(mRestartGenerationValue,
											mRestartGenerationButton->isChecked() ? "t" : "f"));
	}
}

void EvolutionParameterPanel::pauseButtonToggled(bool) {
	if(mPauseEvaluationValue == 0) {
		return;
	}
	if(mPauseEvaluationValue->get() != mPauseEvaluationButton->isChecked()) {
		Core::getInstance()->scheduleTask(new ChangeValueTask(mPauseEvaluationValue,
											mPauseEvaluationButton->isChecked() ? "t" : "f"));
	}
}


/** 
 * Experimental Add-on. Check if this has to be synchronized in Tasks.
 */
void EvolutionParameterPanel::loadSettings() {

	Properties &props = Core::getInstance()->getProperties();
	QString fileChooserDirectory = ".";
	if(props.hasProperty("CurrentFileChooserPath")) {
		fileChooserDirectory = props.getProperty("CurrentFileChooserPath");
	}

	QString fileName = QFileDialog::getOpenFileName(this, tr("Load Evolution Settings"),
							fileChooserDirectory);
	if(fileName == "") {
		return;
	}
	fileChooserDirectory = fileName.mid(0, fileName.lastIndexOf("/"));
	props.setProperty("CurrentFileChooserPath", fileChooserDirectory);

	QString missingValues = loadParametersFromFile(fileName);

	if(missingValues != "") {
		QMessageBox::warning(this, "Problem while loading!",
			"Could not load the following Values:\n\n" + missingValues, QMessageBox::Ok, 0, 0);
	}
}

QString EvolutionParameterPanel::loadParametersFromFile(const QString &fileName) {

	//TODO Check if this should be done in a Task

	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		file.close();
		return "";
	}

	QTextStream input(&file);
	QString unknownValues = "";

	QTextStream inputNew(&file);

	QList<QString> valueNames = getParameterValueNames();
	Core *core = Core::getInstance();

	QString missingValuesText;

	while (!inputNew.atEnd()) {
		QString line = inputNew.readLine();
		line = line.trimmed();

		if(line.startsWith("#")) {
			continue;
		}
		int sepIndex = line.indexOf("=");
		if(sepIndex == -1) {
			continue;
		}
		QString name = line.left(sepIndex);
		QString valueContent = line.right(line.length() - sepIndex - 1);

		//load only values that are part of the EvolutionParameterPanel.
		if(!valueNames.contains(name)) {
			//TODO warn
			missingValuesText.append(name + " [invalid]");
			continue;
		}

		Value *value = Core::getInstance()->getValueManager()->getValue(name);
		if(value == 0) {
			missingValuesText.append(name + " [not found]");
			continue;
		}
		else {
			//TODO check if this should be a task.
			//value->setValueFromString(valueContent);
			core->scheduleTask(new ChangeValueTask(value, valueContent));
		}
	}

	file.close();

	return missingValuesText;
}

void EvolutionParameterPanel::saveSettings() {
	Properties &props = Core::getInstance()->getProperties();

	QString fileChooserDirectory = ".";
	if(props.hasProperty("CurrentFileChooserPath")) {
		fileChooserDirectory = props.getProperty("CurrentFileChooserPath");
	}

	QString fileName = QFileDialog::getSaveFileName(this,
       tr("Save Evolution Settings"), fileChooserDirectory);

	if(fileName == "") {
		return;
	}
	fileChooserDirectory = fileName.mid(0, fileName.lastIndexOf("/"));
	props.setProperty("CurrentFileChooserPath", fileChooserDirectory);

	if(!fileName.endsWith(".val")) {
		fileName.append(".val");
	}
	saveCurrentParameters(fileName);
}

void EvolutionParameterPanel::saveCurrentParameters(const QString &fileName) {

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
	output << "#NERD Values (Evolution Parameters)" << "\n";
	output << "#Saved at: " << QDate::currentDate().toString("dd.MM.yyyy")
					<< "  " << QTime::currentTime().toString("hh:mm:ss") << "\n";

	ValueManager *vm = Core::getInstance()->getValueManager();
	QList<QString> valueNames = getParameterValueNames();

	for(QListIterator<QString> i(valueNames); i.hasNext();) {
		QString valueName = i.next();

		Value *value = vm->getValue(valueName);
		if(value == 0) {
			continue;
		}

		output << valueName << "=" << value->getValueAsString() << "\n";
	}
	file.close();
}

QList<QString> EvolutionParameterPanel::getParameterValueNames() {

	ValueManager *vm = Core::getInstance()->getValueManager();

	QList<QString> valueNames = vm->getValueNamesMatchingPattern(
					"(?!.*/Evo/.*/Fitness/.*/Fitness/.*)(?!.*/Performance/.*)/Evo/Algorithm/.*");
	valueNames << vm->getValueNamesMatchingPattern(
					"(?!.*/Evo/.*/Fitness/.*/Fitness/.*)(?!.*/Performance/.*)/Evo/Pop/.*");
	valueNames << vm->getValueNamesMatchingPattern("/Evo/.*/Fitness/.*/Fitness/CalculationMode");
	valueNames.append("/Control/NumberOfSteps");
	valueNames.append("/Control/NumberOfTries");

	return valueNames;
}

}



