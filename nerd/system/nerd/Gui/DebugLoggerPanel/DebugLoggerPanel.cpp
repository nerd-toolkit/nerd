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


#include "DebugLoggerPanel.h"
#include "Core/Core.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QApplication>
#include <QTime>
#include <iostream>

using namespace std;

namespace nerd{

DebugLoggerPanel::DebugLoggerPanel(QWidget *parent, const QString &valueName, 
				const QString &loggerName, bool showValueSelectionEdit, bool logWhileNotVisible) 
: PositionMemoryWidget(loggerName, parent), mValueName(valueName), 
	mTextEdit(0), mRecentMessageStr(0), mShowValueSelectionEdit(showValueSelectionEdit),
	mValueToMonitorEdit(0), mLogWhileNotVisible(logWhileNotVisible)
{
	setWindowTitle(loggerName);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);


	mTextEdit = new QTextEdit();
	mTextEdit->setReadOnly(true);
	mTextEdit->setLineWrapMode(QTextEdit::NoWrap);
	mTextEdit->document()->setMaximumBlockCount(30);


 	QVBoxLayout *layout = new QVBoxLayout();
 	layout->addWidget(mTextEdit);

	if(mShowValueSelectionEdit) {
		mValueToMonitorEdit = new QLineEdit(mValueName);
		layout->addWidget(mValueToMonitorEdit);
		mValueToMonitorEdit->setToolTip("Specifies the monitored value");
	}

 	centralWidget()->setLayout(layout);
 
 	mXPos = 0;
 	mYPos = 0;

	connect(this, SIGNAL(messageOccured(const QString &)), mTextEdit, SLOT(append(const QString&)));
	
	if(mValueToMonitorEdit != 0) {
		connect(mValueToMonitorEdit, SIGNAL(returnPressed()),
				this, SLOT(valueNameWasChanged()));
	}

	resize(500, 300);

	Core::getInstance()->addSystemObject(this);
}


DebugLoggerPanel::~DebugLoggerPanel() {
}

bool DebugLoggerPanel::init() {

	return true;
}

bool DebugLoggerPanel::bind() {
	// Bind to value as ValueChangedListener

	if(mValueName == "") {
		return true;
	}

	ValueManager *vm = Core::getInstance()->getValueManager();
	mRecentMessageStr = vm->getValue(mValueName);

	if(mRecentMessageStr == 0) {
		Core::log(QString("DebugLoggerPanel::init Error: Could not find Value [")
				  .append(mValueName).append("]"));
		return false;
	}

	mRecentMessageStr->addValueChangedListener(this);

	return true;
}

bool DebugLoggerPanel::cleanUp() {
	if(mRecentMessageStr != 0) {
		mRecentMessageStr->removeValueChangedListener(this);
	}
	return true;
}

void DebugLoggerPanel::valueChanged(Value *value) {

	// If there is a new /Logger/RecentMessage forward this message with some 
	// formatting to the DebugLoggerWindow (mTextEdit)

	if(!isVisible() && !mLogWhileNotVisible) {
		return;
	}

	if(value == mRecentMessageStr) {
		// get the RecentMessageStr
		QString message(mRecentMessageStr->getValueAsString());
		if(message != "") {
			// log the message time
			QString extendedMessage(QTime::currentTime().toString("hh:mm:ss.zzz"));
			extendedMessage.append(" : ");
			// check if message has to be highlighted
			if(message.startsWith("~")) {
				message.remove(">");
				message.remove("<");
		 		if(message.size() > 1) {
					extendedMessage.append(" <b>* ");
					extendedMessage.append(message.mid(1));
					extendedMessage.append(" #</b> ");
				}
			}
			else {
				extendedMessage.append(message);
			}

			emit messageOccured(extendedMessage);
		}
	}
}

QString DebugLoggerPanel::getName() const {
	return "DebugLoggerPanel";
}

void DebugLoggerPanel::closeEvent(QCloseEvent*) {
	// Event is caught to save the last Window position
	mXPos = x();
	mYPos = y();
}

void DebugLoggerPanel::showWindow() {
	// Slot for restoring last Window position before showing the window
	move(mXPos, mYPos);
	show();
}

void DebugLoggerPanel::valueNameWasChanged() {
	
	mValueName = mValueToMonitorEdit->text();

	if(mRecentMessageStr != 0) {
		mRecentMessageStr->removeValueChangedListener(this);
	}
	ValueManager *vm = Core::getInstance()->getValueManager();
	mRecentMessageStr = vm->getValue(mValueName);

	if(mRecentMessageStr != 0) {
		mRecentMessageStr->addValueChangedListener(this);
	}
}

}
