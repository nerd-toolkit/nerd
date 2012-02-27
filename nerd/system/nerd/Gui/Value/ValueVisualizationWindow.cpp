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

#include "ValueVisualizationWindow.h"
#include "Core/Core.h"
#include "Core/Properties.h"
#include <QRegExp>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QHBoxLayout>

namespace nerd{

ValueVisualizationWindow::ValueVisualizationWindow() : QWidget(0),
	mSaveSettingsButton(0), mLoadSettingsButton(0)
{
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowTitle("Simulation Properties");
	mContainer = new QWidget;
	manager = Core::getInstance()->getValueManager();
	mLayout = new QGridLayout;
	createValueList();
	setLayout(mLayout);
	createEditButtons();
	resize(450, 400);
	mAllSelected = true;
	mXPos = 0;
	mYPos = 0;
	connect(this, SIGNAL(showWarning(QString)), this, SLOT(showWarningMessage(QString)));
}

void ValueVisualizationWindow::createEditButtons() {
	mEditButtons = new QWidget;
	mEditLayout = new QGridLayout;
	mSelectAllButton = new QPushButton("Deselect All");
	mRemoveAll = new QPushButton("Remove All");
	mApplyAllChanges = new QPushButton("Apply");

	mLoadSettingsButton = new QPushButton("Load");
	mSaveSettingsButton = new QPushButton("Save");

	QHBoxLayout *mBoxLayout = new QHBoxLayout;
	mEditButtons->setLayout(mBoxLayout);

	mBoxLayout->addWidget(mSelectAllButton);
	mBoxLayout->addWidget(mRemoveAll);
	mBoxLayout->addStretch(1000);

	mBoxLayout->addWidget(mApplyAllChanges);
	mBoxLayout->addStretch(1000);

	mBoxLayout->addWidget(mLoadSettingsButton);
	mBoxLayout->addWidget(mSaveSettingsButton);

	mLayout->addWidget(mEditButtons,2,0);

	connect(mRemoveAll, SIGNAL(clicked()), this, SLOT(deleteValuesFromList()));
	connect(mApplyAllChanges, SIGNAL(clicked()), this, SLOT(applyAllChanges()));
	connect(mSelectAllButton, SIGNAL(clicked()), this, SLOT(modifySelection()));
	connect(mSaveSettingsButton, SIGNAL(clicked()), this, SLOT(saveCurrentSettings()));
	connect(mLoadSettingsButton, SIGNAL(clicked()), this, SLOT(loadSettings()));
}

void ValueVisualizationWindow::createValueList() {
	mValueSearch = new QWidget;
	mLayout->addWidget(mValueSearch, 0,0);
	mSearchLayout = new QGridLayout;
	mValueSearch->setLayout(mSearchLayout);
	mValueSearchField = new QLineEdit;
	mSearchLayout->addWidget(mValueSearchField, 0,0);
	mValueComboBox = new QComboBox;
	mValueComboBox->setEditable(false);
	mNumberOfSelectedValuesLabel = new QLabel;

	mAddSelectedValues = new QPushButton("Add");
	mSearchLayout->addWidget(mNumberOfSelectedValuesLabel, 0, 1);
	mSearchLayout->addWidget(mAddSelectedValues, 1,1);
	mSearchLayout->addWidget(mValueComboBox,1,0);

	mValueComboBox->setMinimumSize(350, 25);
	mValueComboBox->setMaximumSize(350, 25);
	mValueSearchField->setMinimumSize(350, 25);
	mValueSearchField->setMaximumSize(350, 25);

	mSearchLayout->setAlignment(mNumberOfSelectedValuesLabel, Qt::AlignRight);
	mSearchLayout->setAlignment(mValueComboBox, Qt::AlignLeft);
	mSearchLayout->setAlignment(mAddSelectedValues, Qt::AlignRight);
	mSearchLayout->setAlignment(mValueSearchField, Qt::AlignLeft);

	connect(mValueSearchField, SIGNAL(textEdited(const QString&)),
			this, SLOT(fillComboList(const QString&)));
	connect(mValueSearchField, SIGNAL(returnPressed()),
			this, SLOT(showSelectedValues()));
	connect(mValueComboBox, SIGNAL(activated(const QString&)),
			mValueSearchField, SLOT(setText(const QString&)));
	connect(mAddSelectedValues, SIGNAL(clicked()),
			this, SLOT(showSelectedValues()));

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

void ValueVisualizationWindow::fillComboList(const QString  &text) {
	mValueComboBox->clear();
	QString temp = text;
	temp.replace("**", ".*").append(".*");
	QRegExp expr(temp, Qt::CaseInsensitive);
	QList<QString> names = manager->getValueNames();

	for(int i = 0; i< names.size(); i++) {
		if(expr.exactMatch(names.at(i))) {
			mValueComboBox->addItem(names.at(i));
		}
	}
	QString number;
	number.setNum(mValueComboBox->count());
	mNumberOfSelectedValuesLabel->setText(number);
}

void ValueVisualizationWindow::showSelectedValues() {
	QVector<QString> selectedValues;
	QString temp = mValueSearchField->text();
	temp.replace("**", ".*").append(".*");
	QRegExp expr(temp, Qt::CaseInsensitive);
	QList<QString> names = manager->getValueNames();

	for(int i = 0; i< names.size(); i++) {
		if(expr.exactMatch(names.at(i))) {
			selectedValues.push_back(names.at(i));
		}
	}

	if(selectedValues.size() > 100) {
		QMessageBox::information(this, "Too many selected values.",
			"Operation not allowed!\n\nThis operation would add too many values "
			"to the property panel. Use the regular expression field to constrain the number of "
			"values to a number below 100.");
		return;
	}

	for(int i = 0; i < selectedValues.size() ; i++) {
		QString valueName = selectedValues.at(i);
		showSelectedValue(valueName);
	}
}


void ValueVisualizationWindow::showSelectedValue(const QString &text) {
	mValueSearchField->setText(text);
	QMap<QString, ValueVisualization*>::iterator index = mValues.find(text);
	if(index == mValues.end()) {
		ValueVisualization *newVis = new ValueVisualization(this, manager->getValue(text), text);
		mValues[text] = newVis;
		mListLayout->addWidget(newVis);
		connect(newVis, SIGNAL(destroyThis(QString)), this,
			SLOT(deleteValueFromList(QString)), Qt::QueuedConnection);
	}
}


void ValueVisualizationWindow::deleteValuesFromList() {
		QMap<QString, ValueVisualization*>::iterator index;
	for(index = mValues.begin(); index != mValues.end();index++) {
		ValueVisualization *tmp = index.value();
		tmp->setDoUpdateValue(Qt::Unchecked);
		tmp->reset();
		delete tmp;
	}
	mValues.clear();
}

void ValueVisualizationWindow::deleteValueFromList(QString valueName) {
	QMap<QString, ValueVisualization*>::iterator index = mValues.find(valueName);
	if(index != mValues.end()) {
		ValueVisualization *tmp = index.value();
		tmp->reset();
		mListLayout->removeWidget(index.value());
		mValues.erase(index);
		delete tmp;
	}
}

void ValueVisualizationWindow::resetWindow() {
	mValueSearchField->setText("");
}

void ValueVisualizationWindow::applyAllChanges() {
	QMap<QString, ValueVisualization*>::iterator index;
	for(index = mValues.begin(); index != mValues.end();index++) {
		ValueVisualization *tmp = index.value();
		tmp->changeValue();
	}
}

void ValueVisualizationWindow::modifySelection() {
	mAllSelected = !mAllSelected;
	QMap<QString, ValueVisualization*>::iterator index;
	for(index = mValues.begin(); index != mValues.end();index++) {
		ValueVisualization *tmp = index.value();
		tmp->changeUpdateProperty(mAllSelected);
	}
	if(mAllSelected) {
		mSelectAllButton->setText("Deselect All");
	} 
	else{
		mSelectAllButton->setText("Select All");
	}
}

void ValueVisualizationWindow::saveCurrentSettings() {
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
	if(file.open( QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream stream(&file);
		QMap<QString, ValueVisualization*>::iterator index;
		for(index = mValues.begin(); index != mValues.end();index++) {
			ValueVisualization *tmp = index.value();
			stream << index.key() << "=" << tmp->getValue()->getValueAsString() << endl;
		}
		file.close();
		return;
	}
	file.close();
	Core::log("Could not open file: " + fileName + " for saving values.");
}

void ValueVisualizationWindow::loadSettings() {
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
	QString unknownValues = "Could not find the following Values:\n\n";

	deleteValuesFromList();
	bool errorOccured = false;
	QTextStream inputNew(&file);

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

		Value *value = Core::getInstance()->getValueManager()->getValue(name);
		if(value == 0) {
			errorOccured = true;
			unknownValues.append(name).append("\n");
			continue;
		}
		else {
			showSelectedValue(name);
			mValues.value(name)->setCurrentValue(valueContent);
		}
		//TODO warning if setValueFromString failed.

	}
	if(errorOccured) {
		//	emit showWarning(unknownValues);
		QMessageBox::warning(this, "Problem while loading!",
			unknownValues, QMessageBox::Ok, 0, 0);
	}
	file.close();
}

void ValueVisualizationWindow::showWarningMessage(QString message) {
	QMessageBox msgBox;
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setText(message);
	msgBox.exec();
 }

void ValueVisualizationWindow::closeEvent(QCloseEvent*) {
	mXPos = x();
	mYPos = y();
}

void ValueVisualizationWindow::showWindow() {
	move(mXPos, mYPos);
	show();
}

void ValueVisualizationWindow::showEvent(QShowEvent*) {
	//Ensure that the comboBox is initially filled with the current parameter names.
	if(mValueSearchField != 0) {
		fillComboList(mValueSearchField->text());
	}
}

}
