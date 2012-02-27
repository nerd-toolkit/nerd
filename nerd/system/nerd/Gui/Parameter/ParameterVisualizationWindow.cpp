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


#include "ParameterVisualizationWindow.h"
#include <QLineEdit>
#include "Core/Core.h"
#include <iostream>
#include <QRegExp>
#include "ParameterVisualization.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include "Value/MultiPartValue.h"
#include <QFileDialog>
#include <QDate>
#include <QTime>
#include "NerdConstants.h"


using namespace std;

namespace nerd {


ParameterVisualizationWindow::ParameterVisualizationWindow(const QString &name,
							SetInitValueTask *setInitValueTaskPrototype)
		: QWidget(0), mName(name),
 			mValueListArea(0), mValueList(0), mListLayout(0),
			mRemoveAll(0), mAction(0),
			mLoggerButton(0), mParameterLoggerWidget(0), mFillingComboBox(false),
			mPhysicsEnvironmentChangedEvent(0), mValueRepositoryChangedEvent(0),
			mSetInitValueTaskPrototype(setInitValueTaskPrototype)
{

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	setWindowTitle("Object Properties");
	mContainer = new QWidget;

	mLayout = new QVBoxLayout;

	mParameterLoggerWidget = new ParameterLoggerWidget(name, this);
	mValuePlotter = new ValuePlotterWidget(name, 0);

	createValueList();
	setLayout(mLayout);
	createEditButtons();


	mLayout->addWidget(mParameterLoggerWidget);
	mParameterLoggerWidget->setVisible(false);

	resize(450, 400);
	mCloseDownRunning = false;
	mAllSelected = true;
	mXPos = 0;
	mYPos = 0;

	connect(this, SIGNAL(executeFillComboList(const QString&)),
			this, SLOT(fillComboList(const QString&)));

}

ParameterVisualizationWindow::~ParameterVisualizationWindow() {
	resetWindow();

	if(mPhysicsEnvironmentChangedEvent != 0) {
		mPhysicsEnvironmentChangedEvent->removeEventListener(this);
	}
	if(mValueRepositoryChangedEvent != 0) {
		mValueRepositoryChangedEvent->removeEventListener(this);
	}

	delete mParameterLoggerWidget;
	delete mValuePlotter;

	//TODO delete all objects not created with this as parent object!
}

QString ParameterVisualizationWindow::getName() const {
	return mName;
}

void ParameterVisualizationWindow::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mPhysicsEnvironmentChangedEvent || event == mValueRepositoryChangedEvent) {
		QList<ParameterVisualization*> visus = mValues.values();
		for(QListIterator<ParameterVisualization*> i(visus); i.hasNext();) {
			ParameterVisualization *visu = i.next();
			visu->setValueObjectByName(visu->getValueName());
		}
		//update combobox and plotter.
		emit executeFillComboList(mValueComboBox->lineEdit()->text());
		showAndUpdatePlotter(false);
	}
	else if(event == Core::getInstance()->getShutDownEvent()) {
		if(mPhysicsEnvironmentChangedEvent != 0) {
			mPhysicsEnvironmentChangedEvent->removeEventListener(this);
		}
		if(mValueRepositoryChangedEvent != 0) {
			mValueRepositoryChangedEvent->removeEventListener(this);
		}
		mPhysicsEnvironmentChangedEvent = 0;
		mValueRepositoryChangedEvent = 0;
	}
}


QHash<QString, Value*> ParameterVisualizationWindow::getValueCollection() const {
	QHash<QString, Value*> values;

	QList<ParameterVisualization*> parameterVisus = mValues.values();

	for(int i = 0; i < parameterVisus.size(); ++i) {
		ParameterVisualization *visu = parameterVisus.at(i);

		if(visu == 0 || !visu->isValueUpdateActive()) {
			continue;
		}
		Value *value = visu->getValue();
		if(value == 0) {
			continue;
		}
		values.insert(visu->getValueName(), value);
	}
	return values;
}


void ParameterVisualizationWindow::saveCurrentParameters(
						const QString &fileName, bool saveValueContent)
{

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

	


	//for(QMap<QString, ParameterVisualization*>::iterator i = mValues.begin(); i != mValues.end(); ++i) {
	//	QString valueName = i.key();
	//	ParameterVisualization *visu = i.value();
	for(int i = 0; i < mListLayout->count(); ++i) {
		ParameterVisualization *visu = dynamic_cast<ParameterVisualization*>(mListLayout->itemAt(i)->widget());
		
		Value *value = visu->getValue();
		if(value == 0) {
			continue;
		}
		QList<QString> optionList = value->getOptionList();
		if(saveValueContent) {
			output << visu->getValueName() << "=" << value->getValueAsString() << "\n";
		}
		else {
			QString valueContent = visu->getCurrentValue();
			output << visu->getValueName() << "=" << valueContent << "\n";
		}
		QList<QString> options = visu->getOptions();
        for(QListIterator<QString> o(options); o.hasNext();) {
			QString option = o.next();
			if(!optionList.contains(option)) {
				//do not save the options of the option list...
				output << "#>" << option << "\n";
			}
        }
	}
	file.close();
}


QString ParameterVisualizationWindow::loadParametersFromFile(const QString &fileName, bool autoApply) {

	//TODO Check if this should be done in a Task

	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		file.close();
		return "";
	}

	QTextStream input(&file);
	QString unknownValues = "";

	deleteValuesFromList();
	QTextStream inputNew(&file);

	ParameterVisualization *lastKnownValuePanel = 0;

	while (!inputNew.atEnd()) {
		QString line = inputNew.readLine();
		line = line.trimmed();

        if(line.startsWith("#>")) {
            if(lastKnownValuePanel != 0) {
                lastKnownValuePanel->addOption(line.mid(2));
            }
        }
		if(line.startsWith("#")) {
			continue;
		}
		int sepIndex = line.indexOf("=");
		if(sepIndex == -1) {
			continue;
		}
		QString name = line.left(sepIndex);
		QString valueContent = line.right(line.length() - sepIndex - 1);

		Value *value = Core::getInstance()->getValueManager()->getValue(name);
		if(value == 0) {
			unknownValues.append(name).append("\n");
			continue;
		}
		else {
			if(autoApply) {
				value->setValueFromString(valueContent);
			}
			lastKnownValuePanel = showSelectedValue(name, !autoApply);
			if(!autoApply) {
				mValues.value(name)->setCurrentValue(valueContent);
			}
		}
		//TODO warning if setValueFromString failed.
	}

	file.close();
	return unknownValues;
}




void ParameterVisualizationWindow::createEditButtons() {

	mEditLayout = new QGridLayout();
	mLayout->addLayout(mEditLayout);

	mSelectAllButton = new QPushButton("Deselect All");
	mEditLayout->addWidget(mSelectAllButton, 0, 0);
	mRemoveAll = new QPushButton("Remove All");
	mEditLayout->addWidget(mRemoveAll, 1, 0);

	mApplyAllChanges = new QPushButton("Apply");
	mEditLayout->addWidget(mApplyAllChanges, 1, 1);
	mLoggerButton = new QPushButton("Log");
	mEditLayout->addWidget(mLoggerButton, 1, 2);
	mPlotterButton = new QPushButton("Plot");
	mEditLayout->addWidget(mPlotterButton, 1, 3);

	mLoadSettingsButton = new QPushButton("Load");
	mEditLayout->addWidget(mLoadSettingsButton, 0, 4);
	mSaveSettingsButton = new QPushButton("Save");
	mEditLayout->addWidget(mSaveSettingsButton, 1, 4);

	connect(mApplyAllChanges, SIGNAL(clicked()),
			this, SLOT(applyAllChanges()));
	connect(mRemoveAll, SIGNAL(clicked()),
			this, SLOT(deleteValuesFromList()));
	connect(mSelectAllButton, SIGNAL(clicked()),
			this, SLOT(modifySelection()));
	connect(mLoggerButton, SIGNAL(clicked()),
			mParameterLoggerWidget, SLOT(showDialog()));
	connect(mParameterLoggerWidget, SIGNAL(loggerStatusChanged(bool)),
			this, SLOT(updateLoggerButton(bool)));
	connect(mPlotterButton, SIGNAL(clicked()),
			this, SLOT(showAndUpdatePlotter()));
	connect(mLoadSettingsButton, SIGNAL(clicked()),
			this, SLOT(loadSettings()));
	connect(mSaveSettingsButton, SIGNAL(clicked()),
			this, SLOT(saveSettings()));

}

void ParameterVisualizationWindow::createValueList()
{
	mValueSearch = new QWidget();
	mLayout->addWidget(mValueSearch, 0,0);
	mSearchLayout = new QGridLayout();
	mValueSearch->setLayout(mSearchLayout);



	mValueComboBox = new ParameterComboBox();
	mValueComboBox->setEditable(true);
	mValueComboBox->setAutoCompletion(false);
	mValueComboBox->setInsertPolicy(QComboBox::NoInsert);
	mValueComboBox->setWhatsThis("Search for properties with regular expressions. For convenience .*"
                    "can be replaced by **. The expression automatically starts and ends with **. "
                    "Example: /Left/**Pos finds e.g. /Sim/Left/Arm/Position.");

	mSearchLayout->addWidget(mValueComboBox, 0, 0);
	mSearchLayout->setColumnStretch(0, 100);
	mSearchLayout->setRowStretch(0, 100);

	mAddSelectedParameters = new QPushButton("Add");
	mAddSelectedParameters->setWhatsThis("Adds all properties matching the regular expression to the panel.");
	mNumberOfSelectedParametersLabel = new QLabel();
	mNumberOfSelectedParametersLabel->setWhatsThis("Number of properties currently matching the regular expression.");
	mSearchLayout->addWidget(mNumberOfSelectedParametersLabel, 0, 1);
	mSearchLayout->addWidget(mAddSelectedParameters, 0, 2);

	mValueComboBox->setMinimumSize(280, 25);
	mNumberOfSelectedParametersLabel->setMinimumSize(40, 25);

	connect(mValueComboBox, SIGNAL(downArrowPressed()),
			this, SLOT(showComboBoxList()));
	connect(mAddSelectedParameters, SIGNAL(clicked()),
			this, SLOT(showSelectedValues()));
	connect(mValueComboBox, SIGNAL(editTextChanged(const QString&)),
			this, SLOT(fillComboList(const QString&)));
	connect(mValueComboBox->lineEdit(), SIGNAL(returnPressed()),
			this, SLOT(showSelectedValues()));
	connect(mValueComboBox, SIGNAL(activated(const QString&)),
			this, SLOT(selectionChanged(const QString&)));

	mValueListArea = new QScrollArea(this);
	mValueListArea->setWidgetResizable(true);
	mValueList = new QWidget(mValueListArea);

	mListLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	mListLayout->setAlignment(Qt::AlignTop);
	mListLayout->setSpacing(1);

	mValueList->setLayout(mListLayout);
	mValueListArea->setWidget(mValueList);
	mLayout->addWidget(mValueListArea, 1,0);
}

void ParameterVisualizationWindow::modifySelection() {

	modifySelection(!mAllSelected);
}


void ParameterVisualizationWindow::showComboBoxList() {
	if(mValueComboBox != 0) {
		mValueComboBox->showPopup();
	}
}


void ParameterVisualizationWindow::showAndUpdatePlotter(bool show) {

	QList<QString> keys = mValues.keys();
	QList<Value*> values;

	//collect all available Values.
	for(int i = 0; i < keys.size(); ++i) {
		Value *value = mValues.value(keys.at(i))->getValue();
		if(value != 0) {
			values.append(value);
		}
	}

	//remove all values that are not in the list any more.
	QList<Value*> plotterValues = mValuePlotter->getValuePlotter()->getPlottedValues();

	for(int i = 0; i < plotterValues.size(); ++i) {
		Value *value = plotterValues.at(i);
		if(!values.contains(value)) {
			bool found = false;
			for(int j = 0; j < values.size(); ++j) {
				MultiPartValue *mpv = dynamic_cast<MultiPartValue*>(values.at(j));
				if(mpv != 0) {
					for(int k = 0; k < mpv->getNumberOfValueParts(); ++k) {
						if(value == mpv->getValuePart(k)) {
							found = true;
							break;
						}
					}
				}
				if(found) {
					break;
				}
			}
			if(!found) {
				mValuePlotter->getValuePlotter()->removeValue(value);
			}
		}
	}

	//add all values (that are not already in the plotter)
	for(int i = 0; i < keys.size(); ++i) {
		QString name = keys.at(i);
		Value *value = mValues.value(name)->getValue();

		mValuePlotter->getValuePlotter()->addValue(name, value);
	}

	if(show) {
		mValuePlotter->showWidget();
	}
}

void ParameterVisualizationWindow::updateLoggerButton(bool loggerActive) {
	if(loggerActive) {
		mLoggerButton->setText("Log (Active)");
	}
	else {
		mLoggerButton->setText("Log");
	}
}

void ParameterVisualizationWindow::saveSettings() {
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
	saveCurrentParameters(fileName, true);
}


void ParameterVisualizationWindow::loadSettings() {

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

	QString missingValues = loadParametersFromFile(fileName, false);
	if(missingValues != "") {
		QMessageBox::warning(this, "Problem while loading!",
			"Could not find the following Values:\n\n" + missingValues, QMessageBox::Ok, 0, 0);
	}
}


void ParameterVisualizationWindow::moveParameterVisualization(ParameterVisualization *visu, bool up) {
	if(visu == 0) {
		return;
	}

	int currentIndex = 0;
	for(; currentIndex < mListLayout->count(); ++currentIndex) {
		if(mListLayout->itemAt(currentIndex)->widget() == visu) {
			break;
		}
	}

	if(up && currentIndex == 0) {
		return;
	}
	if(!up && currentIndex >= (mListLayout->count() - 1)) {
		return;
	}
	if(up) {
		QLayoutItem *item = mListLayout->takeAt(currentIndex);
		mListLayout->insertWidget(currentIndex - 1, item->widget());
	}
	else {
		QLayoutItem *item = mListLayout->takeAt(currentIndex);
		mListLayout->insertWidget(currentIndex + 1, item->widget());
	}
}


void ParameterVisualizationWindow::showEvent(QShowEvent*) {
	//Ensure that the comboBox is initially filled with the current parameter names.

	mPhysicsEnvironmentChangedEvent = Core::getInstance()->getEventManager()->registerForEvent(
				NerdConstants::EVENT_PHYSICS_ENVIRONMENT_CHANGED, this);
	mValueRepositoryChangedEvent = Core::getInstance()->getEventManager()->registerForEvent(
				NerdConstants::EVENT_VALUE_REPOSITORY_CHANGED, this);

	if(mValueComboBox != 0) {
		fillComboList(mValueComboBox->lineEdit()->text());
	}
}

void ParameterVisualizationWindow::selectionChanged(const QString &text) {
	//Memorize the name of the last selected parameter to avoid double selection
	//when parameters have almost similar names.

	if(mValueComboBox->isShiftKeyPressed()) {
		mValueComboBox->lineEdit()->setText(mRestorationBuffer);
	}
	if(mValueComboBox->isCtrlKeyPressed()) {
		showSelectedValue(text, false);
		mSelectedParameterName = text;
	}
}

void ParameterVisualizationWindow::fillComboList(const QString  &text) {

	if(mFillingComboBox) {
		//ignore all activations of this method while executing it.
		return;
	}

	mFillingComboBox = true;

	if((mSelectedParameterName != "") && (mSelectedParameterName == text)) {
		//stop processing if the text in the combo box originates from a user selection.

		mValueComboBox->lineEdit()->setText(mLastEnteredParameterName);
		mFillingComboBox = false;
		return;
	}

	//memorize text and cursor position to be able to replicate it after filling the combobox.
	mRestorationBuffer = mLastEnteredParameterName;
	mLastEnteredParameterName = text;
	int cursorPosition = mValueComboBox->lineEdit()->cursorPosition();

	//invalidate the last user selection.
	mSelectedParameterName = "";

	//fill combobox with the currently matching parameter names
	mValueComboBox->clear();
	QString temp = text;
	temp.replace("**", ".*");
	if(temp.startsWith("^")) {
		temp = temp.mid(1);
	}
	else {
		temp.prepend(".*");
	}
	if(temp.endsWith("$")) {
		temp = temp.mid(0, temp.size() - 1);
	}
	else {
		temp.append(".*");
	}


// 	QRegExp expr(temp, Qt::CaseInsensitive);
// 	QList<QString> names = Physics::getPhysicsManager()->getParameterInfo();
// 	for(int i = 0; i< names.size(); i++) {
// 		if(expr.exactMatch(names.at(i))) {
// 		mValueComboBox->addItem(names.at(i));
// 		}
// 	}

	//CHANGE: by chris. Now the values come from the ValueManager instead of the PhysicsManager.
	QList<QString> names = Core::getInstance()->getValueManager()
				->getValueNamesMatchingPattern(temp, false);
	for(QListIterator<QString> i(names); i.hasNext();) {
		mValueComboBox->addItem(i.next());
	}


	//update the label with the number of matching parameters
	QString number;
	number.setNum(mValueComboBox->count());
	mNumberOfSelectedParametersLabel->setText(number);

	//restore combobox text and cursor position.
	mValueComboBox->lineEdit()->setText(mLastEnteredParameterName);
	mValueComboBox->lineEdit()->setCursorPosition(cursorPosition);

	mFillingComboBox = false;
}

void ParameterVisualizationWindow::showSelectedValues() {
	QVector<QString> selectedParameters;
	QString temp = mValueComboBox->lineEdit()->text();

	if(!((mSelectedParameterName != "") && (mSelectedParameterName != temp))) {
		temp.replace("**", ".*");
		if(temp.startsWith("^")) {
			temp = temp.mid(1);
		}
		else {
			temp.prepend(".*");
		}
		if(temp.endsWith("$")) {
			temp = temp.mid(0, temp.size() - 1);
		}
		else {
			temp.append(".*");
		}
	}

	//CHANGE: by chris. Now the values come from the ValueManager instead of the PhysicsManager.
	QList<QString> names = Core::getInstance()->getValueManager()
				->getValueNamesMatchingPattern(temp, false);
	for(QListIterator<QString> i(names); i.hasNext();) {
		selectedParameters.push_back(i.next());
	}

	bool startDeselected = false;
	if(selectedParameters.size() > 100) {
		QMessageBox::information(this, "Too many selected values.",
					"Operation not allowed!\n\n"
					"This operation would add too many values to the property panel."
					"Use the regular expression field to constrain the number of "
					"values to a number below 100.");
		return;
	}

	for(int i = 0; i < selectedParameters.size() ; i++) {
		QString valueName = selectedParameters.at(i);
		showSelectedValue(valueName, startDeselected);
	}
}


ParameterVisualization* ParameterVisualizationWindow::showSelectedValue(const QString &text, bool startDeselected)
{

	if(mValues.contains(text)) {
		return 0;
	}


	//ParameterVisualization *newVis = new ParameterVisualization(this,
	//Physics::getPhysicsManager()->getParameter(text), text);

	//CHANGED: by chris, Values are read from the ValueManager instead of the PhysicsManager.
	ParameterVisualization *newVis = new ParameterVisualization(this,
			Core::getInstance()->getValueManager()->getValue(text), text, mSetInitValueTaskPrototype);
	mValues[text] = newVis;
	newVis->setDoUpdateValue(!startDeselected);
	mListLayout->addWidget(newVis);


	//NOTE: This connection is done with Qt::QueuedConnection() to ensure that
	//the signal response is later but in the same thread. Otherwise time races
	//can occure between the deletion of the object and its method executions.
	connect(newVis, SIGNAL(destroyThis(QString)),
				this, SLOT(deleteValueFromList(QString)), Qt::QueuedConnection);
	connect(newVis, SIGNAL(move(ParameterVisualization*, bool)),
				this, SLOT(moveParameterVisualization(ParameterVisualization*, bool)));

    return newVis;
}


void ParameterVisualizationWindow::deleteValuesFromList() {

	QMap<QString, ParameterVisualization*>::iterator index;
	for(index = mValues.begin(); index != mValues.end();index++) {
		ParameterVisualization *tmp = index.value();
		tmp->setDoUpdateValue(Qt::Unchecked);
		tmp->reset();
		delete tmp;
	}
	mValues.clear();
}

void ParameterVisualizationWindow::deleteValueFromList(QString valueName) {

	QMap<QString, ParameterVisualization*>::iterator index = mValues.find(valueName);
	if(index != mValues.end()) {
		ParameterVisualization *tmp = index.value();
		tmp->reset();
		mListLayout->removeWidget(index.value());
		mValues.erase(index);
		delete tmp;
	}
}

void ParameterVisualizationWindow::resetWindow() {
	deleteValuesFromList();
	mValueComboBox->lineEdit()->setText("");
}


void ParameterVisualizationWindow::modifySelection(bool select) {

	QMap<QString, ParameterVisualization*>::iterator index;
	for(index = mValues.begin(); index != mValues.end();index++) {
		ParameterVisualization *tmp = index.value();
		tmp->setDoUpdateValue(select);
	}
	mAllSelected = select;

	if(mAllSelected) {
		mSelectAllButton->setText("Deselect All");
	}
	else {
		mSelectAllButton->setText("Select All");
	}
}

QList<ParameterVisualization*> ParameterVisualizationWindow::getParameterVisualizations() const {
	return mValues.values();
}

void ParameterVisualizationWindow::applyAllChanges() {
	QMap<QString, ParameterVisualization*>::iterator index;
	for(index = mValues.begin(); index != mValues.end();index++) {
		index.value()->changeValue();
	}
}


void ParameterVisualizationWindow::closeEvent(QCloseEvent*) {
	mXPos = x();
	mYPos = y();
}

}
