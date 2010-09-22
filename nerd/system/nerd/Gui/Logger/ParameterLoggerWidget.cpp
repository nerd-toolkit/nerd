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



#include "ParameterLoggerWidget.h"
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include "Core/Core.h"
#include <iostream>
#include "Event/EventManager.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include "NerdConstants.h"

using namespace std;

namespace nerd {

/**
 * Constructor.
 *
 * @param owner the ParameterVisualizationWindow to take the Value selection from.
 */
ParameterLoggerWidget::ParameterLoggerWidget(const QString &name, ValueProvider *owner)
	: QGroupBox("Property Logging", 0), mName(name),
		mValueProvider(owner),
		mLoggingInProgress(false),
		mOutputStream(0), mOutputFile(0), mStepCompletedEvent(0)
{
	setWindowTitle(QString("Property Logger (").append(mName).append(")"));

	//create GUI elements
	mFileNameEdit = new QLineEdit("LogFile.txt", this);
	mFileNameEdit->setWhatsThis(tr("This field contains the name of the file to log"
			" the values to.\n"
			"The file name may include a realative of absolute path if required."));
	mCommentCharEdit = new QLineEdit("#", this);
	mCommentCharEdit->setWhatsThis(tr("This string is added to the begin of each comment line"
			", e.g. to avoid the interpretation of the value names as values"
			" by a program that reads the logged values.\n"
			"It is possible to use \\t to specify a tabulator and \\n to add a line break."));
	mCommentCharEdit->setMaximumWidth(80);
	mSeparatorCharEdit = new QLineEdit(";", this);
	mSeparatorCharEdit->setWhatsThis(tr("This string is put as separator between any two logged"
			" values. Common separators a comma, tabulator or semicolon.\n"
			"It is possible to use \\t to specify a tabulator and \\n to add a line break."));
	mSeparatorCharEdit->setMaximumWidth(80);

	mLogButton = new QPushButton("Log Selected Variables", this);
	mLogButton->setCheckable(true);

	QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
	setLayout(layout);

	QVBoxLayout *fileNameLayout = new QVBoxLayout();
	layout->addLayout(fileNameLayout);
	fileNameLayout->addWidget(new QLabel("Logfile Name:"));
	fileNameLayout->addWidget(mFileNameEdit);

	QVBoxLayout *commentLayout = new QVBoxLayout();
	layout->addLayout(commentLayout);
	commentLayout->addWidget(new QLabel("Comment Char:  "));
	commentLayout->addWidget(mCommentCharEdit);

	QVBoxLayout *separatorLayout = new QVBoxLayout();
	layout->addLayout(separatorLayout);
	separatorLayout->addWidget(new QLabel("Separator:  "));
	separatorLayout->addWidget(mSeparatorCharEdit);

	QVBoxLayout *logButtonLayout = new QVBoxLayout();
	layout->addLayout(logButtonLayout);
	logButtonLayout->addWidget(new QLabel(""));
	logButtonLayout->addWidget(mLogButton);

	connect(mLogButton, SIGNAL(clicked()), this, SLOT(logButtonPressed()));

	resize(450, 50);
}


/**
 * Destructor.
 */
ParameterLoggerWidget::~ParameterLoggerWidget() {
	stopLogging();
	if(mStepCompletedEvent != 0) {
		mStepCompletedEvent->removeEventListener(this);
	}
}



/**
 * Just returns the name [ParameterLoggerWidget] for debugging purposes.
 *
 * @return the name of this object.
 */
QString ParameterLoggerWidget::getName() const {
	return mName;
}


/**
 * Called when the StepCompletedEvent was triggered.
 * If logging is in progress, then all values in the value list
 * are logged to the log file.
 *
 * TODO Maybe EventRegistration should be done on demand only.
 *
 * @param event the event that occured.
 */
void ParameterLoggerWidget::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mStepCompletedEvent) {
		if(mLoggingInProgress) {
			logVariables();
		}
	}
}


/**
 * This slot can be used to show or hide the log dialog.
 * If the widget is visible, then it is hidden, otherwise it is made visible.
 */
void ParameterLoggerWidget::showDialog() {
	if(isVisible()) {
		mDialogPosition = pos();
		hide();
	}
	else {
		if(mStepCompletedEvent == 0) {
			EventManager *em = Core::getInstance()->getEventManager();
			mStepCompletedEvent = em
				->registerForEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, this);

			if(mStepCompletedEvent == 0) {
				qDebug("ParameterLoggerWidget: Did not find required Event "
						"[Control/StepCompleted]!");
			}
		}
		show();
	}
}


/**
 * This slot is called when the mLogButton is toggled.
 */
void ParameterLoggerWidget::logButtonPressed() {
	if(mLogButton->isChecked()) {
		if(startLogging()) {
			mFileNameEdit->setEnabled(false);
			mCommentCharEdit->setEnabled(false);
			mSeparatorCharEdit->setEnabled(false);
			emit loggerStatusChanged(true);
		}
		else {
			mLogButton->setChecked(false);
			mFileNameEdit->setEnabled(true);
			mCommentCharEdit->setEnabled(true);
			mSeparatorCharEdit->setEnabled(true);
			emit loggerStatusChanged(false);
		}
	}
	else {
		stopLogging();
		mFileNameEdit->setEnabled(true);
		mCommentCharEdit->setEnabled(true);
		mSeparatorCharEdit->setEnabled(true);
		emit loggerStatusChanged(false);
	}
}


/**
 * Starts logging of the currently selected Values in the parent
 * ParameterVisualizationWindow.
 *
 * Therefore a new file is opened with the file name taken from QLineEdit
 * mFileNameEdit. This file is initially filled with the names of all
 * selected object property values as a comment. The comment character is
 * taken from QLineEdit mCommentCharEdit. If successfull, then all values
 * will be stored after each simulation step until method stopLogging
 * is called.
 *
 * @return true if successfull, otherwise false.
 */
bool ParameterLoggerWidget::startLogging() {

	if(mLoggingInProgress) {
		stopLogging();
	}

	if(mValueProvider == 0) {
		return false;
	}

	mSeparatorString = formatControlChars(mSeparatorCharEdit->text());
	mCommentString = formatControlChars(mCommentCharEdit->text());

	mOutputFile = new QFile(mFileNameEdit->text());

	//try to open the file and notify failure if neccessary
	if(!mOutputFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
		delete mOutputFile;
		mOutputFile = 0;

		QMessageBox::warning(this, "File Open Error",
                                    QString("Could not open log file\n[")
									.append(mFileNameEdit->text())
									.append("]\nin write mode!\n"),
                                    0, 0, 0);
		return false;
	}

	//Fill list of logged Values with the current selection of mParentParameterVisu.
	//Hereby add the names of the Values as comment to the logfile.
	mLoggedValues.clear();

	mOutputStream = new QTextStream(mOutputFile);

	QHash<QString, Value*> values = mValueProvider->getValueCollection();
	QList<QString> names = values.keys();

	for(int i = 0; i < names.size(); ++i) {
		QString name = names.at(i);
		Value *value = values.value(name);

		if(value == 0) {
			continue;
		}

		(*mOutputStream) << mCommentString << " "
				<< name << endl;
		mLoggedValues.append(value);
	}

	(*mOutputStream) << endl;

	mLoggingInProgress = true;

	return true;
}


/**
 * Stops logging of the values.
 */
void ParameterLoggerWidget::stopLogging() {

	if(!mLoggingInProgress) {
		return;
	}

	//flush output stream, close file and delete stream and objects.
	if(mOutputStream != 0) {
		mOutputStream->flush();
	}
	if(mOutputFile != 0) {
		mOutputFile->close();
	}
	if(mOutputStream != 0) {
		delete mOutputStream;
		mOutputStream = 0;
	}
	if(mOutputFile != 0) {
		delete mOutputFile;
		mOutputFile = 0;
	}
	mLoggedValues.clear();
	mLoggingInProgress = false;
}


/**
 * Logs all variable values of the Values in the current list of logged
 * Values.
 */
void ParameterLoggerWidget::logVariables() {

	if(!mLoggingInProgress || mOutputStream == 0 || mLoggedValues.isEmpty()) {
		return;
	}

	for(int i = 0; i < mLoggedValues.size(); ++i) {
		Value *value = mLoggedValues.at(i);

		if(value == 0) {
			(*mOutputStream) << "<missing>";
		}
		else {
			(*mOutputStream) << value->getValueAsString();
		}

		if(i < mLoggedValues.size() - 1) {
			(*mOutputStream) << mSeparatorString;
		}
	}
	(*mOutputStream) << endl;
}


/**
 * Replaces all occurences of control characters like \\t and \\n with real control
 * characters.
 */
QString ParameterLoggerWidget::formatControlChars(const QString &string) {
	QString text = string;
	text = text.replace("\\t", "\t");
	text = text.replace("\\n", "\n");
	return text;
}


}


