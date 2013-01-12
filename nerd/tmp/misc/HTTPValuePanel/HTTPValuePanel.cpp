/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
 *                                                                         *
 *   HTTPValuePanel by Ferry Bachmann (bachmann@informatik.hu-berlin.de)   *
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

#include "HTTPValuePanel.h"
#include <QUrl>
#include <QEvent>
#include "Core/Core.h"

namespace nerd {

/*class SetValueFromStringThreadSafe : public Task {

	public:
		SetValueFromStringThreadSafe(Value *value, const QString &newValue, QWaitCondition &waitCondition, bool &returnValue) :
			mValue(value), mNewValue(newValue), mWaitCondition(waitCondition), mReturnValue(returnValue)
	{
	}

	~SetValueFromStringThreadSafe() {}

	bool runTask() {
		mReturnValue = mValue->setValueFromString(mNewValue);
		mWaitCondition.wakeAll();
		return true;
	}
	private:
		Value *mValue;
		QString mNewValue;
		bool &mReturnValue;
		QWaitCondition &mWaitCondition;
};

bool HTTPValuePanel::setValueFromStringThreadSafe(Value *value, const QString &newValue) {
	bool returnValue;
	QMutex waitMutex;
	QWaitCondition waitCondition;
	SetValueFromStringThreadSafe *task = new SetValueFromStringThreadSafe(value, newValue, waitCondition, returnValue);
	Core::getInstance()->scheduleTask(task);
	waitMutex.lock();
	waitCondition.wait(&waitMutex);
	waitMutex.unlock();
	return returnValue;
}
*/

// Value prefix for adding received values to the local ValueManager
const QString HTTPValuePanel::mValuePrefix = ">";

HTTPValuePanel::HTTPValuePanel() :
	mState(None)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle("HTTP Value Panel");
	resize(700, 600);

	// Widgets for ServerSettings
	mServerSettingsBox = new QGroupBox("Server Settings");
	mPostUrlLabel = new QLabel("&PostURL:");
	mPostUrlEdit = new QLineEdit("http://www.ikw.uni-osnabrueck.de/~neurokybernetik/protected/orcsserverscripts/post_values.php");
	mPostUrlLabel->setBuddy(mPostUrlEdit);
	mGetUrlLabel = new QLabel("&GetURL:");
	mGetUrlEdit = new QLineEdit("http://www.ikw.uni-osnabrueck.de/~neurokybernetik/protected/orcsserverscripts/get_values.php");
	mGetUrlLabel->setBuddy(mGetUrlEdit);

	mGetValueFileLabel = new QLabel("Get&ValueFile:");
	mGetValueFileEdit = new QLineEdit("/mnt/genome/work/output_test_php/testval.val");
	mGetValueFileLabel->setBuddy(mGetValueFileEdit);
	mGetValueTypeFileLabel = new QLabel("GetValue&TypeFile:");
	mGetValueTypeFileEdit = new QLineEdit("/mnt/genome/work/output_test_php/testval.info");
	mGetValueTypeFileLabel->setBuddy(mGetValueTypeFileEdit);
	mPostValueFileLabel = new QLabel("Pos&tValueFile:");
	mPostValueFileEdit = new QLineEdit("/mnt/genome/work/output_test_php/testval.val");
	mPostValueFileLabel->setBuddy(mPostValueFileEdit);

	mServerSettingsLayout = new QGridLayout;
	mServerSettingsLayout->addWidget(mPostUrlLabel, 0, 0);
	mServerSettingsLayout->addWidget(mPostUrlEdit, 0, 1);
	mServerSettingsLayout->addWidget(mGetUrlLabel, 1, 0);
	mServerSettingsLayout->addWidget(mGetUrlEdit, 1, 1);
	mServerSettingsLayout->addWidget(mGetValueFileLabel, 2, 0);
	mServerSettingsLayout->addWidget(mGetValueFileEdit, 2, 1);
	mServerSettingsLayout->addWidget(mGetValueTypeFileLabel, 3, 0);
	mServerSettingsLayout->addWidget(mGetValueTypeFileEdit, 3, 1);
	mServerSettingsLayout->addWidget(mPostValueFileLabel, 4, 0);
	mServerSettingsLayout->addWidget(mPostValueFileEdit, 4, 1);
	mServerSettingsBox->setLayout(mServerSettingsLayout);

	// Widgets for Values
	mValuesBox = new QGroupBox("Values");
	mGetAllValuesAvailable = new QPushButton("Get All Values Available");

	mValueEdit = new QLineEdit;
	mValueCountLabel = new QLabel("0");
	mValueComboBox = new QComboBox;
	mAddValue = new QPushButton("Add");

	mValueListArea = new QScrollArea;
	mValueListArea->setWidgetResizable(true);
	mValueList = new QWidget(mValueListArea);
	mListLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	mListLayout->setAlignment(Qt::AlignTop);
	mListLayout->setSpacing(1);
	mValueList->setLayout(mListLayout);
	mValueListArea->setWidget(mValueList);

	mApplyAllChanges = new QPushButton("Apply All Changes");
	mRemoveAll = new QPushButton("Remove All");
	mGetSelectedValues = new QPushButton("Get Selected Values");
	mPostSelectedValues = new QPushButton("Post Selected Values");
	mValueButtonLayout = new QGridLayout;
	mValueButtonLayout->addWidget(mApplyAllChanges, 0, 0);
	mValueButtonLayout->addWidget(mRemoveAll, 0, 1);
	mValueButtonLayout->addWidget(mGetSelectedValues, 1, 0);
	mValueButtonLayout->addWidget(mPostSelectedValues, 1, 1);

	mValuesLayout = new QGridLayout;
	mValuesLayout->addWidget(mGetAllValuesAvailable, 0, 0);
	mValuesLayout->addWidget(mValueEdit, 1, 0);
	mValuesLayout->addWidget(mValueCountLabel, 1, 1);
	mValuesLayout->addWidget(mValueComboBox, 2, 0);
	mValuesLayout->addWidget(mAddValue, 2, 1);
	mValuesLayout->addWidget(mValueListArea, 3, 0);
	mValuesLayout->addLayout(mValueButtonLayout, 4, 0);

	mValuesBox->setLayout(mValuesLayout);

	// Main Layout
	mMainLayout = new QVBoxLayout;
	mMainLayout->addWidget(mServerSettingsBox);
	mMainLayout->addWidget(mValuesBox);
	setLayout(mMainLayout);

	// Connect widgets
	connect(mGetAllValuesAvailable, SIGNAL(clicked()), this, SLOT(getAllValuesAvailable()));
	connect(mValueEdit, SIGNAL(textEdited(const QString&)), this, SLOT(fillValueList(const QString&)));
	connect(mValueComboBox, SIGNAL(activated(const QString&)), mValueEdit, SLOT(setText(const QString&)));
	connect(mValueEdit, SIGNAL(returnPressed()), this, SLOT(showMatchingValues()));
	connect(mAddValue, SIGNAL(clicked()), this, SLOT(showMatchingValues()));
	connect(mRemoveAll, SIGNAL(clicked()), this, SLOT(removeAllValuesFromList()));
	connect(mApplyAllChanges, SIGNAL(clicked()), this, SLOT(applyAllChanges()));
	connect(mGetSelectedValues, SIGNAL(clicked()), this, SLOT(getSelectedValues()));
	connect(mPostSelectedValues, SIGNAL(clicked()), this, SLOT(postSelectedValues()));

	// HTTP Value Access
	mHTTPValueAccess = new HTTPValueAccess(this);
	connect(mHTTPValueAccess, SIGNAL(getValuesFinished(bool)), this, SLOT(requestFinished(bool)));
	connect(mHTTPValueAccess, SIGNAL(postValuesFinished(bool)), this, SLOT(requestFinished(bool)));

	// Get the ValueManager
	mValueManager = Core::getInstance()->getValueManager();
}

HTTPValuePanel::~HTTPValuePanel() {
}

void HTTPValuePanel::getAllValuesAvailable() {
	setDisabled(true);
	mState = WaitingForAllValuesAvailable;
	mHTTPValueAccess->getValues(QUrl(mGetUrlEdit->text()),
			mGetValueFileEdit->text(), mGetValueTypeFileEdit->text());
}

void HTTPValuePanel::getSelectedValues() {
	setDisabled(true);
	// Create QList of selected value names
	QList<QString> selectedValueNames;
	QList<HTTPValueVisualization*> valueVisus = mValueVisualizations.values();
	for(int i = 0; i < valueVisus.size(); ++i) {
		HTTPValueVisualization *valueVisu = valueVisus.at(i);
		if(valueVisu->isSelected()) {
			selectedValueNames.append(valueVisu->getValueName());
		}
	}

	// If some values are selected, send the request
	if(!selectedValueNames.isEmpty()) {
		mState = WaitingForSelectedValues;
		mHTTPValueAccess->getValues(QUrl(mGetUrlEdit->text()),
			mGetValueFileEdit->text(), mGetValueTypeFileEdit->text(), &selectedValueNames);
	} else {
		setDisabled(false);
	}
}

void HTTPValuePanel::postSelectedValues() {
	setDisabled(true);
	// Create QMap of selected value names and contents
	QMap<QString, QString> selectedValueNamesAndContents;
	QMap<Value*, HTTPValueVisualization*>::const_iterator it;
	for(it = mValueVisualizations.constBegin(); it != mValueVisualizations.constEnd(); ++it) {
		HTTPValueVisualization *valueVisu = it.value();
		if(valueVisu->isSelected()) {
			Value *value = it.key();
			selectedValueNamesAndContents.insert(valueVisu->getValueName(), value->getValueAsString());
		}
	}

	// If some values are selected, send the request
	if(!selectedValueNamesAndContents.isEmpty()) {
		mHTTPValueAccess->postValues(QUrl(mPostUrlEdit->text()),
			mPostValueFileEdit->text(), &selectedValueNamesAndContents);
	} else {
		setDisabled(false);
	}
}

void HTTPValuePanel::requestFinished(bool error) {
	if(!error) {
		if(mState == WaitingForAllValuesAvailable) {
			mValueEdit->clear();
			fillValueList("");
		}
	}
	mState = None;
	setDisabled(false);
}

/**
 * This slot creates the value visualizations of the selected values.
 */
void HTTPValuePanel::showMatchingValues() {
	QList<QString> selectedValues;
	QString temp = mValuePrefix + mValueEdit->text();
	temp.append(".*");
	QList<Value*> values = mValueManager->getValuesMatchingPattern(temp);
	for(int i = 0; i < values.size(); i++) {
		Value *value = values.at(i);
		QMap<Value*, HTTPValueVisualization*>::iterator index =
		mValueVisualizations.find(value);
		if(index == mValueVisualizations.end()) {
			QString firstName = mValueManager->getNamesOfValue(value).first();
			HTTPValueVisualization *newVis = new HTTPValueVisualization(value, firstName);
			mValueVisualizations[value] = newVis;
			newVis->installEventFilter(this);
			mListLayout->addWidget(newVis, 0, Qt::AlignTop);
		}
	}
}

bool HTTPValuePanel::eventFilter(QObject *object, QEvent *event) {
	if(event->type() == QEvent::Close) {
		HTTPValueVisualization *visu = dynamic_cast<HTTPValueVisualization*>(object);
		if(visu != NULL) {
			QMap<Value*, HTTPValueVisualization*>::iterator index = mValueVisualizations.find(visu->getValue());
			if(index != mValueVisualizations.end()) {
				mValueVisualizations.erase(index);
			}
		}
	}
	return false;	// send event to the target object itself
}

/**
 * This slot fills the ComboBox with all values matching the search pattern and updates the number of values found matching the pattern.
 * The value prefix is inserted at the beginning of the search pattern to ensure that no local values are shown.
 *
 * @param text the search pattern
 */
void HTTPValuePanel::fillValueList(const QString &text) {
	QString temp = mValuePrefix + text;
	temp.append(".*");
	QList<Value*> values = mValueManager->getValuesMatchingPattern(temp);
	mValueComboBox->clear();
	for(int i = 0; i < values.size(); i++) {
		Value *value = values.at(i);
		QList<QString> names = mValueManager->getNamesOfValue(value);
		QListIterator<QString> it(names);
		for(; it.hasNext(); ) {
			QString name = it.next();
			// Cut of prefix
			mValueComboBox->addItem(name.right(name.length() - mValuePrefix.length()));
		}
	}
	QString number;
	number.setNum(values.size());
	mValueCountLabel->setText(number);
}

/**
 * This slot removes all value visualizations.
 */
void HTTPValuePanel::removeAllValuesFromList() {
	QMap<Value*, HTTPValueVisualization*>::iterator it = mValueVisualizations.begin();
	while(it != mValueVisualizations.end()) {
		HTTPValueVisualization *valueVisu = it.value();
		// erase the Visualization from the QMap before sending close event
		// so the event filter of this class doesn't erase the Visualization (SegFault!)
		it = mValueVisualizations.erase(it);
		valueVisu->close();
	}
}

/**
 * This slot applies all current changes of values.
 */
void HTTPValuePanel::applyAllChanges() {
	QMap<Value*, HTTPValueVisualization*>::iterator it = mValueVisualizations.begin();
	while(it != mValueVisualizations.end()) {
		HTTPValueVisualization *valueVisu = it.value();
		valueVisu->changeValue();
		it++;
	}
}

}
