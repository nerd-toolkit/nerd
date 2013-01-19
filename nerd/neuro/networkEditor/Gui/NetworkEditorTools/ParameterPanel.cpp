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



#include "ParameterPanel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QGridLayout>

using namespace std;

namespace nerd {


/**
 * Constructs a new ParameterPanel.
 */
ParameterPanel::ParameterPanel(QObject *parent, QGridLayout *targetLayout,
							   QString name, Value *parameter)
	: QObject(parent), mRegisteredAsListener(false), mTargetLayout(targetLayout),
	 mParameterName(name), mParameter(parameter), mInvalid(false), mEditScriptButton(0), 
	 mScriptEditor(0)
{

	mParameterContent = new QLineEdit();
	mNameLabel = new QLabel(name);
	
	if(mParameter != 0) {
		mParameterContent->setText(mParameter->getValueAsString());
		mParameter->addValueChangedListener(this);
		mRegisteredAsListener = true;
		mNameLabel->setToolTip(mParameter->getDescription());
	}
	
	if(dynamic_cast<CodeValue*>(parameter) != 0) {
		mEditScriptButton = new QPushButton("Edit");
		mEditScriptButton->setFixedWidth(50);
		mEditScriptButton->setWhatsThis("Open content in a script editor.");
	}
	
	mContentLayout = new QHBoxLayout();
	mContentLayout->addWidget(mParameterContent);
	if(mEditScriptButton != 0) {
		mContentLayout->addWidget(mEditScriptButton);
	}

	int row = targetLayout->rowCount();
	targetLayout->addWidget(mNameLabel, row, 0);
	targetLayout->addLayout(mContentLayout, row, 1);

	connect(mParameterContent, SIGNAL(returnPressed()),
			this, SLOT(parameterContentChanged()));
    connect(mParameterContent, SIGNAL(textEdited(const QString&)),
			this, SLOT(markAsValueEdited()));
	connect(this, SIGNAL(setParameterText(QString)),
			mParameterContent, SLOT(setText(QString)));
	if(mEditScriptButton != 0) {
		connect(mEditScriptButton, SIGNAL(clicked(bool)),
				this, SLOT(editScriptButtonPressed()));
	}
}



/**
 * Destructor.
 */
ParameterPanel::~ParameterPanel() {
	
	//clear script editor and enable saving if there are unsaved changes.
	if(mScriptEditor != 0) {
		mScriptEditor->attachToCodeBase(0);
	}
	
	//remove listener registrations
	if(mParameter != 0 && mRegisteredAsListener) {
		mParameter->removeValueChangedListener(this);
	}
	if(mTargetLayout != 0) {
		mTargetLayout->removeWidget(mParameterContent);
	}
	
	//delete objects
	delete mParameterContent;
	delete mNameLabel;
	if(mEditScriptButton != 0) {
		delete mEditScriptButton;
	}
	delete mContentLayout;
	if(mScriptEditor != 0) {
		delete mScriptEditor;
	}
}


/**
 * Called when the parameter value changed (e.g. by external modifications).
 */
void ParameterPanel::valueChanged(Value *value)  {
	if(value == 0 || mInvalid) {
		return;
	}
	if(value == mParameter) {
		//mParameterContent->setText(mParameter->getValueAsString());
		emit setParameterText(mParameter->getValueAsString());
		emit markAsValueUpdated();
	}
}



/**
 * Releases the observation registration for the managed parameter and all
 * other observed values.
 */
void ParameterPanel::forceListenerDeregistration(Value *value) {
	ValueChangedListener::forceListenerDeregistration(value);

	if(value != 0 && value == mParameter) {
		mRegisteredAsListener = false;
	}
}


/**
 * Removes the added widgets and layouts form the master layout.
 * Here, all objects added in the constructor have to be removed from the layout.
 */
void ParameterPanel::removeFromLayout(QGridLayout *layout) {
	layout->removeWidget(mNameLabel);
	layout->removeItem(mContentLayout);
}


QString ParameterPanel::getName() const {
	return QString("ParameterPanel [" + mParameterName + "]");
}


QString ParameterPanel::getParameterName() {
	return mParameterName;
}


Value* ParameterPanel::getParameterValue() {
	return mParameter;
}

void ParameterPanel::invalidateListeners() {
	if(mParameter != 0) {
		mParameter->removeValueChangedListener(this);
	}
	mInvalid = true;
}


/**
 * Called when the return key was pressed in the line edit.
 */
void ParameterPanel::parameterContentChanged() {
	if(mParameter == 0 || mInvalid) {
		return;
	}
	QString newContent = mParameterContent->text();

	if(mParameter->getValueAsString() == newContent) {
		return;
	}
	mParameterContent->setText(mParameter->getValueAsString());

	emit markAsValueUpdated();
	emit parameterChanged(mParameter, mParameterName, newContent);
}


/**
 * Called when the edit script button is pressed.
 */
void ParameterPanel::editScriptButtonPressed() {
	
	//create script editor on the fly if not already available
	if(mScriptEditor == 0) {
		mScriptEditor = new ScriptEditor(mParameterName, false);
		
		connect(mScriptEditor, SIGNAL(handleChangedContent(QString)),
				this, SLOT(handleChangedContent(QString)));
	}
	
	mScriptEditor->attachToCodeBase(dynamic_cast<CodeValue*>(mParameter));
	mScriptEditor->show();
}

// void ParameterPanel::parameterContentUpdated() {
// 	if(mParameter == 0 || mInvalid) {
// 		return;
// 	}
// 	QString content = mParameter->getValueAsString();
// 	if(mParameterContent->text() != content) {
// 		mParameterContent->setText(content);
// 		emit markAsValueUpdated();
// 	}
// }

/**
 * Marks the background color of the text label red to show that the content
 * has been changed manually.
 */
void ParameterPanel::markAsValueEdited() {
	QPalette p = mParameterContent->palette();
	p.setColor(QPalette::Base, QColor(255,120,120));
	mParameterContent->setPalette(p);
}


/**
 * Marks the background color of the text label white to show that the content
 * originates from the observed Value.
 */
void ParameterPanel::markAsValueUpdated() {
	QPalette p = mParameterContent->palette();
	p.setColor(QPalette::Base, Qt::white);
	mParameterContent->setPalette(p);
}


/**
 * Called when the (optional) script editor tries to store a value.
 * 
 * @param newContent the new code (with linebreaks)
 */
void ParameterPanel::handleChangedContent(const QString &newContent) {
	CodeValue *code = dynamic_cast<CodeValue*>(mParameter);
	
	if(code == 0 || mInvalid) {
		return;
	}
	emit setParameterText(newContent);
	
	if(code->get() == newContent) {
		return;
	}

	emit markAsValueUpdated();
	emit parameterChanged(mParameter, mParameterName, newContent);
}



}




