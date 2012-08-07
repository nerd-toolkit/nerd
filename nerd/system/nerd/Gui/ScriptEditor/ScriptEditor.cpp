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



#include "ScriptEditor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include <QVBoxLayout>
#include "Value/ChangeValueTask.h"
#include "Util/NerdFileSelector.h"
#include <QKeyEvent>
#include <Gui/ValuePlotter/ValuePlotterItem.h>
#include <NerdConstants.h>


using namespace std;

namespace nerd {


/**
 * Constructs a new ScriptEditor.
 */
ScriptEditor::ScriptEditor(const QString &scriptName)
	: mScriptModified(false)
{
	
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	mScriptCode = 0;
	mErrorValue = 0;
	mCurrentFileName = 0;
	
	QVBoxLayout *layout = new QVBoxLayout();
	setLayout(layout);

	mTitleLabel = new QLabel("Script Editor: " + scriptName);
	layout->addWidget(mTitleLabel);
	
	QSplitter *splitter = new QSplitter(Qt::Vertical);
	layout->addWidget(splitter);

	mCodeArea = new QTextEdit();
	mCodeArea->setTabStopWidth(20);
	mCodeArea->setLineWrapMode(QTextEdit::NoWrap);
	mCodeArea->setEnabled(false);
	splitter->addWidget(mCodeArea);
	
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 100);

	mErrorMessageField = new QTextEdit();
	mErrorMessageField->setMaximumHeight(40);
	mErrorMessageField->setReadOnly(true);
	mErrorMessageField->setEnabled(false);
	layout->addWidget(mErrorMessageField);
	
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	
	mApplyButton = new QPushButton("Apply");
	buttonLayout->addWidget(mApplyButton);

	mReloadScriptCodeButton = new QPushButton("Reload");
	buttonLayout->addWidget(mReloadScriptCodeButton);
	
	mLoadScriptCodeButton = new QPushButton("Load");
	buttonLayout->addWidget(mLoadScriptCodeButton);

	mStoreScriptCodeButton = new QPushButton("Save");
	buttonLayout->addWidget(mStoreScriptCodeButton);

	layout->addLayout(buttonLayout);

	//Connect gui elements.
	connect(mApplyButton, SIGNAL(pressed()),
			this, SLOT(applyButtonPressed()));
	connect(mReloadScriptCodeButton, SIGNAL(pressed()),
			this, SLOT(reloadButtonPressed()));
	connect(mLoadScriptCodeButton, SIGNAL(pressed()),
			this, SLOT(loadButtonPressed()));
	connect(mStoreScriptCodeButton, SIGNAL(pressed()),
			this, SLOT(saveButtonPressed()));
	connect(this, SIGNAL(changeScriptCodeArea()),
			this, SLOT(setScriptCodeArea()));
	connect(this, SIGNAL(changeErrorMessageArea()),
			this, SLOT(setErrorMessageArea()));
	connect(mCodeArea, SIGNAL(textChanged()),
			this, SLOT(markAsModified()));
	connect(this, SIGNAL(markAsUnmodified()),
			this, SLOT(markTitleAsUnmodified()));

	
	resize(400,500);
	
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
ScriptEditor::~ScriptEditor() {
	mScriptCode = 0;
	mErrorValue = 0;
}


QString ScriptEditor::getName() const {
	return "ScriptEditor";
}


void ScriptEditor::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mScriptCode) {
		emit changeScriptCodeArea();
	}
	else if(value == mErrorValue) {
		emit changeErrorMessageArea();
	}
}


bool ScriptEditor::attachToCodeBase(CodeValue *code) {
	
	if(code == 0) {
		mScriptCode = 0;
		mErrorValue = 0;
		mCodeArea->setText("");
		mCodeArea->setEnabled(false);
		mErrorMessageField->setText("");
		mErrorMessageField->setEnabled(false);
		
		emit markAsUnmodified();
	}
	else {
		mScriptCode = code;
		
		
		mCodeArea->setText(mScriptCode->get());
		mCodeArea->setEnabled(true);
		mErrorMessageField->setEnabled(true);
		
		setCodeFromValue();
	}
	
	return true;
}





void ScriptEditor::applyButtonPressed() {
	QString code = mCodeArea->document()->toPlainText();
	if(mScriptCode != 0) {
		Core::getInstance()->scheduleTask(new ChangeValueTask(mScriptCode, code));
	}
	emit markAsUnmodified();
}


void ScriptEditor::reloadButtonPressed() {
	setScriptCodeArea();
	emit markAsUnmodified();
}


void ScriptEditor::loadButtonPressed() {
	QString fileName = NerdFileSelector::getFileName("Get Script", true, this);
	if(fileName.trimmed() != "") {
		setScriptCodeArea();
		if(mCurrentFileName != 0) {
			Core::getInstance()->scheduleTask(new ChangeValueTask(mCurrentFileName, fileName));
		}
		loadScriptCode(fileName, true);
		emit markAsUnmodified();
	}
}

bool ScriptEditor::loadScriptCode(const QString &fileName, bool replaceExistingCode) {

	if(!replaceExistingCode && mScriptCode->get().trimmed() != "") {
		//TODO check if this should return true.
		return false;
	}

	QFile file(fileName);

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		Core::log(QString("ScriptEditor: Could not load script file [")
				+ fileName + "] to ScriptEditor ["
				+ getName() + "]");
		file.close();
		return false;
	}

	QTextStream fileStream(&file);
	
	QString multiLineCode = fileStream.readAll();
	mCodeArea->setText(multiLineCode);
	
	Core::getInstance()->scheduleTask(new ChangeValueTask(mScriptCode, multiLineCode));
	
	file.close();

	Core::log(QString("ScriptEditor: Loaded script code from file [")
			  + fileName + "].");

	return true;
}


void ScriptEditor::saveButtonPressed() {
	QString fileName = NerdFileSelector::getFileName("Get Script", false, this);
	if(fileName.trimmed() == "") {
		return;
	}
	saveCurrentScript(fileName);
}


void ScriptEditor::saveCurrentScript(const QString &fileName_) {
	QString name = fileName_;
	if(name == "" && mCurrentFileName != 0) {
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

		if(mCurrentFileName != 0) {
			Core::getInstance()->scheduleTask(new ChangeValueTask(mCurrentFileName, name));
		}
		emit markAsUnmodified();
	}
}




void ScriptEditor::setScriptCodeArea() {
	setCodeFromValue();
}

void ScriptEditor::setErrorMessageArea() {
	mErrorMessageField->setText(mErrorValue->get());
}



void ScriptEditor::keyPressEvent(QKeyEvent *event) {
	QWidget::keyPressEvent(event);
	if(event->key() == Qt::Key_S && (event->modifiers() == Qt::CTRL)) {
		applyButtonPressed();
		saveCurrentScript();
	} 
}

void ScriptEditor::markTitleAsUnmodified() {
	mScriptModified = false;
	QString title = mTitleLabel->text();
	if(title.endsWith("*")) {
		mTitleLabel->setText(title.mid(0, title.size() - 1));
	}
}

void ScriptEditor::markAsModified() {
	if(!mScriptModified) {
		mScriptModified = true;
		QString title = mTitleLabel->text();
		if(!title.endsWith("*")) {
			mTitleLabel->setText(title.append("*"));
		}
	}
}





void ScriptEditor::setCodeFromValue() {
	if(mScriptCode == 0) {
		return;
	}
	QString code = mScriptCode->get();
	//code = code.replace("/**/", "\n");
	QString oldCode = mCodeArea->toPlainText();
	if(oldCode != code) {
		mCodeArea->document()->setPlainText(code);
	}
	emit markAsUnmodified();
}

}



