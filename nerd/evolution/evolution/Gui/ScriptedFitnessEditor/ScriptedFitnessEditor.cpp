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



#include "ScriptedFitnessEditor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include <QVBoxLayout>
#include "Fitness/FitnessFunction.h"
#include "Value/ChangeValueTask.h"
#include "Util/NerdFileSelector.h"
#include <QKeyEvent>
#include <Gui/ValuePlotter/ValuePlotterItem.h>
#include <NerdConstants.h>


using namespace std;

namespace nerd {


/**
 * Constructs a new ScriptedFitnessEditor.
 */
ScriptedFitnessEditor::ScriptedFitnessEditor(const QString &fitnessFunctionName)
	: mScriptModified(false)
{

	QVBoxLayout *layout = new QVBoxLayout();
	setLayout(layout);

	mTitleLabel = new QLabel(fitnessFunctionName);
	layout->addWidget(mTitleLabel);
	
	QSplitter *splitter = new QSplitter(Qt::Vertical);
	layout->addWidget(splitter);
	
	mFitnessPlotter = new ValuePlotterWidget("Current Fitness");
	mFitnessPlotter->hideHandle(true);
	splitter->addWidget(mFitnessPlotter);

	mCodeArea = new QTextEdit();
	mCodeArea->setTabStopWidth(20);
	mCodeArea->setLineWrapMode(QTextEdit::NoWrap);
	splitter->addWidget(mCodeArea);
	
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 100);

	mErrorMessageField = new QTextEdit();
	mErrorMessageField->setMaximumHeight(40);
	mErrorMessageField->setReadOnly(true);
	layout->addWidget(mErrorMessageField);
	
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	
	mApplyButton = new QPushButton("Apply");
	buttonLayout->addWidget(mApplyButton);

	mReloadFitnessCodeButton = new QPushButton("Reload");
	buttonLayout->addWidget(mReloadFitnessCodeButton);
	
	mLoadFitnessCodeButton = new QPushButton("Load");
	buttonLayout->addWidget(mLoadFitnessCodeButton);

	mStoreFitnessCodeButton = new QPushButton("Save");
	buttonLayout->addWidget(mStoreFitnessCodeButton);

	mCalculationModeSelector = new QComboBox();
	mCalculationModeSelector->addItem("Mean");
	mCalculationModeSelector->addItem("Min");
	mCalculationModeSelector->addItem("Max");
	buttonLayout->addWidget(mCalculationModeSelector);

	layout->addLayout(buttonLayout);

	//Connect gui elements.
	connect(mApplyButton, SIGNAL(pressed()),
			this, SLOT(applyButtonPressed()));
	connect(mReloadFitnessCodeButton, SIGNAL(pressed()),
			this, SLOT(reloadButtonPressed()));
	connect(mLoadFitnessCodeButton, SIGNAL(pressed()),
			this, SLOT(loadButtonPressed()));
	connect(mStoreFitnessCodeButton, SIGNAL(pressed()),
			this, SLOT(saveButtonPressed()));
	connect(mCalculationModeSelector, SIGNAL(currentIndexChanged(int)),
			this, SLOT(calculationModeSelectorChanged(int)));
	connect(this, SIGNAL(changeFitnessCodeArea()),
			this, SLOT(setFitnessCodeArea()));
	connect(this, SIGNAL(changeErrorMessageArea()),
			this, SLOT(setErrorMessageArea()));
	connect(mCodeArea, SIGNAL(textChanged()),
			this, SLOT(markAsModified()));
	connect(this, SIGNAL(markAsUnmodified()),
			this, SLOT(markTitleAsUnmodified()));

	
	//Collect all required values.
	ValueManager *vm = Core::getInstance()->getValueManager();

	mFitnessCode = dynamic_cast<CodeValue*>(vm->getValue(fitnessFunctionName + "/Code"));
	mErrorValue = vm->getStringValue(fitnessFunctionName + "/Config/ErrorState");
	mCurrentFileName = dynamic_cast<FileNameValue*>(vm->getValue(fitnessFunctionName + "/FileName"));
	//mCurrentFileName->useAsFileName(true);
	mCalculationModeValue = vm->getIntValue(fitnessFunctionName + "/Fitness/CalculationMode");

	if(mFitnessCode == 0) {
		Core::log("ScriptedFitnessEditor: Could not find required value ["
				+ fitnessFunctionName + "/Code]", true);
	}
	else {
		mFitnessCode->addValueChangedListener(this);
		setCodeFromValue();
	}
	if(mErrorValue == 0) {
		Core::log("ScriptedFitnessEditor: Could not find required value ["
				+ fitnessFunctionName + "/Config/ErrorState]", true);
	}
	else {
		mErrorValue->addValueChangedListener(this);
	}
	if(mCurrentFileName == 0) {
		Core::log("ScriptedFitnessEditor: Could not find required value ["
				+ fitnessFunctionName + "/FileName]", true);
	}
	else {
		mCurrentFileName->addValueChangedListener(this);
	}
	if(mCalculationModeValue == 0) {
		Core::log("ScriptedFitnessEditor: Could not find required value ["
				+ fitnessFunctionName + "/Fitness/CalculationMode]", true);
	}
	else {
		mCalculationModeValue->addValueChangedListener(this);
		updateFitnessCalculationMode();
	}
	
	
	mFitness =  vm->getDoubleValue(fitnessFunctionName + "/Fitness/Fitness");
	mCurrentFitness =  vm->getDoubleValue(fitnessFunctionName + "/Fitness/CurrentFitness");
	mMaxFitness =  vm->getDoubleValue(fitnessFunctionName + "/Fitness/MaxFitness");
	mMinFitness =  vm->getDoubleValue(fitnessFunctionName + "/Fitness/MinFitness");
	mFitnessVariance =  vm->getDoubleValue(fitnessFunctionName + "/Fitness/Variance");
	
	//Add fitness values to plotter
	mFitnessPlotter->getValuePlotter()->addValue("Current", mCurrentFitness);
	mFitnessPlotter->getValuePlotter()->addValue("All Tries", mFitness);
	mFitnessPlotter->getValuePlotter()->addValue("Max", mMaxFitness);
	mFitnessPlotter->getValuePlotter()->addValue("Min", mMinFitness);
	mFitnessPlotter->getValuePlotter()->addValue("Variance", mFitnessVariance);
	
	//hide less common items
	QList<Value*> hiddenFitnessValues;
	hiddenFitnessValues.append(mMaxFitness);
	hiddenFitnessValues.append(mMinFitness);
	hiddenFitnessValues.append(mFitnessVariance);
	
	//Initialize and setup fitness plotter
	mFitnessPlotter->getValuePlotter()->init();
	mFitnessPlotter->getValuePlotter()->getPlotterWidget()->setUserScaleV(0.1);
	mFitnessPlotter->getValuePlotter()->getPlotterWidget()->setUserOffsetV(0);
	//mFitnessPlotter->setBaseSize(100, 200);
	
	if(mFitnessPlotter->getValuePlotter() != 0 
			&& mFitnessPlotter->getValuePlotter()->getPlotterWidget() != 0) 
	{
		QList<PlotterItem*> items = mFitnessPlotter->getValuePlotter()
										->getPlotterWidget()->getPlotterItems();
		for(QListIterator<PlotterItem*> i(items); i.hasNext();) {
			ValuePlotterItem *item = dynamic_cast<ValuePlotterItem*>(i.next());
			if(item == 0) {
				continue;
			}
			if(hiddenFitnessValues.contains(item->getValue())) {
				item->setVisible(false);
			}
		}
		
		mFitnessPlotter->getValuePlotter()->getPlotterWidget()->showValuesInLegend(true);
	}
	
// 	//set trigger event for fitness editor
// 	Event *stepCompletedEvent = Core::getInstance()->getEventManager()->getEvent(
// 										NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);
// 	if(stepCompletedEvent != 0) {
// 		mFitnessPlotter->getValuePlotter()->setUpdateTriggerEvent(stepCompletedEvent);
// 	}
}



/**
 * Destructor.
 */
ScriptedFitnessEditor::~ScriptedFitnessEditor() {
}


QString ScriptedFitnessEditor::getName() const {
	return "ScriptedFitnessEditor";
}


void ScriptedFitnessEditor::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mFitnessCode) {
		emit changeFitnessCodeArea();
	}
	else if(value == mCalculationModeValue) {
		updateFitnessCalculationMode();
	}
	else if(value == mErrorValue) {
		emit changeErrorMessageArea();
	}
}





void ScriptedFitnessEditor::applyButtonPressed() {
	QString code = mCodeArea->document()->toPlainText();
	//code = code.replace("\n", "/**/");
	if(mFitnessCode != 0) {
		Core::getInstance()->scheduleTask(new ChangeValueTask(mFitnessCode, code));
	}
}


void ScriptedFitnessEditor::reloadButtonPressed() {
	setFitnessCodeArea();
	Core::getInstance()->scheduleTask(new ChangeValueTask(mCurrentFileName, mCurrentFileName->get()));
	emit markAsUnmodified();
}


void ScriptedFitnessEditor::loadButtonPressed() {
	if(mCurrentFileName == 0) {
		return;
	}	
	QString fileName = NerdFileSelector::getFileName("Get FitnessFunction Script", true, this);
	if(fileName.trimmed() != "") {
		setFitnessCodeArea();
		Core::getInstance()->scheduleTask(new ChangeValueTask(mCurrentFileName, fileName));
		emit markAsUnmodified();
	}
}


void ScriptedFitnessEditor::saveButtonPressed() {
	if(mCurrentFileName == 0) {
		return;
	}	
	QString fileName = NerdFileSelector::getFileName("Get FitnessFunction Script", false, this);
	if(fileName.trimmed() == "") {
		return;
	}
	saveCurrentScript(fileName);
}


void ScriptedFitnessEditor::saveCurrentScript(const QString &fileName_) {
	QString name = fileName_;
	if(name == "") {
		name = mCurrentFileName->get();
	}
	if(name.trimmed() != "") {
		if(!name.endsWith(".js")) {
			name = name.append(".js");
		}

		QFile file(name);

		int dirIndex = name.lastIndexOf("/");
		if(dirIndex > 0) {
			Core::getInstance()->enforceDirectoryPath(file.fileName().mid(0, dirIndex));
		}
	
		if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			Core::log(QString("Could not open file ").append(name).append(" to write Values."));
			file.close();
			return;
		}
	
		QTextStream output(&file);
		QString code = mCodeArea->toPlainText();
		output << code;
		file.close();;

		Core::getInstance()->scheduleTask(new ChangeValueTask(mCurrentFileName, name));
		emit markAsUnmodified();
	}
}


void ScriptedFitnessEditor::calculationModeSelectorChanged(int) {
	if(mCalculationModeValue == 0) {
		return;
	}
	switch(mCalculationModeSelector->currentIndex()) {
		case 0:
			Core::getInstance()->scheduleTask(new ChangeValueTask(mCalculationModeValue, 
											  QString::number(FitnessFunction::MEAN_FITNESS)));
			break;
		case 1:
			Core::getInstance()->scheduleTask(new ChangeValueTask(mCalculationModeValue, 
											  QString::number(FitnessFunction::MIN_FITNESS)));
			break;
		case 2:
			Core::getInstance()->scheduleTask(new ChangeValueTask(mCalculationModeValue, 
											  QString::number(FitnessFunction::MAX_FITNESS)));
			break;
		default:
			Core::getInstance()->scheduleTask(new ChangeValueTask(mCalculationModeValue, 
											  QString::number(FitnessFunction::MEAN_FITNESS)));
	}
}

void ScriptedFitnessEditor::setFitnessCodeArea() {
	setCodeFromValue();
}

void ScriptedFitnessEditor::setErrorMessageArea() {
	mErrorMessageField->setText(mErrorValue->get());
}



void ScriptedFitnessEditor::keyPressEvent(QKeyEvent *event) {
	QWidget::keyPressEvent(event);
	if(event->key() == Qt::Key_S && (event->modifiers() == Qt::CTRL)) {
		applyButtonPressed();
		saveCurrentScript();
	} 
}

void ScriptedFitnessEditor::markTitleAsUnmodified() {
	mScriptModified = false;
	QString title = mTitleLabel->text();
	if(title.endsWith("*")) {
		mTitleLabel->setText(title.mid(0, title.size() - 1));
	}
}

void ScriptedFitnessEditor::markAsModified() {
	if(!mScriptModified) {
		mScriptModified = true;
		QString title = mTitleLabel->text();
		if(!title.endsWith("*")) {
			mTitleLabel->setText(title.append("*"));
		}
	}
}


void ScriptedFitnessEditor::updateFitnessCalculationMode() {
	if(mCalculationModeValue != 0) {
		mCalculationModeSelector->setEnabled(true);
		int mode = mCalculationModeValue->get();
		switch(mode) {
			case FitnessFunction::MEAN_FITNESS:
				mCalculationModeSelector->setCurrentIndex(0);
				break;
			case FitnessFunction::MIN_FITNESS:
				mCalculationModeSelector->setCurrentIndex(1);
				break;
			case FitnessFunction::MAX_FITNESS:
				mCalculationModeSelector->setCurrentIndex(2);
				break;
			default:
				mCalculationModeSelector->setCurrentIndex(0);
		}
	}
	else {
		mCalculationModeSelector->setEnabled(false);
	}
}



void ScriptedFitnessEditor::setCodeFromValue() {
	if(mFitnessCode == 0) {
		return;
	}
	QString code = mFitnessCode->get();
	//code = code.replace("/**/", "\n");
	QString oldCode = mCodeArea->toPlainText();
	if(oldCode != code) {
		mCodeArea->document()->setPlainText(code);
	}
	emit markAsUnmodified();
}

}



