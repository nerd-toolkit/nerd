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

#include "ValueDetailPanel.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "ValueDetailVisualization.h"
#include <QBoxLayout>
#include <QLabel>
#include <QVector>
#include <QScrollArea>
#include <QDebug>
#include <QEvent>
#include <QMessageBox>

namespace nerd {

/**
 * Constructor.
 */
ValueDetailPanel::ValueDetailPanel() : QWidget(0)
{
	setWindowTitle("Value Detail Panel");
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	mValueManager = Core::getInstance()->getValueManager();
	mLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	setLayout(mLayout);

	createSearchView();
	createListView();
	createControlButtonView();

	resize(400, 400);
}

/**
 * Creates the Gui-elements for searching for values and adding values.
 */
void ValueDetailPanel::createSearchView() {
	mSearchLayout = new QGridLayout;
	mValueSearch = new QWidget;
	mValueSearch->setLayout(mSearchLayout);
	mLayout->addWidget(mValueSearch);

	mValueLineEdit = new QLineEdit;
	mValueLineEdit->setMinimumSize(350, 25);
	mValueLineEdit->setMaximumSize(350, 25);
	mSearchLayout->addWidget(mValueLineEdit, 0, 0);
	mSearchLayout->setAlignment(mValueLineEdit, Qt::AlignLeft);

	mAvailableValueCount = new QLabel;
	mAvailableValueCount->setMinimumSize(50, 25);
	mAvailableValueCount->setMaximumSize(50, 25);
	mSearchLayout->addWidget(mAvailableValueCount, 0, 1);
	mSearchLayout->setAlignment(mAvailableValueCount, Qt::AlignLeft);

	mValueComboBox = new QComboBox;
	mValueComboBox->setMinimumSize(350, 25);
	mValueComboBox->setMaximumSize(350, 25);
	mValueComboBox->setEditable(false);
	mSearchLayout->addWidget(mValueComboBox, 1, 0);
	mSearchLayout->setAlignment(mValueComboBox, Qt::AlignLeft);

	mAddSelectedValues = new QPushButton("Add");
	mSearchLayout->addWidget(mAddSelectedValues, 1, 1);
	mSearchLayout->setAlignment(mAddSelectedValues, Qt::AlignRight);

	connect(mValueLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(fillValueList(const QString&)));
	connect(mValueLineEdit, SIGNAL(returnPressed()), this, SLOT(showMatchingValues()));
	connect(mValueComboBox, SIGNAL(activated(const QString&)), mValueLineEdit, SLOT(setText(const QString&)));
	connect(mAddSelectedValues, SIGNAL(clicked()), this, SLOT(showMatchingValues()));
}

/**
 * Creates the Gui-elements for displaying the value visualizations.
 */
void ValueDetailPanel::createListView() {
	mListArea = new QScrollArea;
	mListArea->setWidgetResizable(true);
	mListWidget = new QWidget(mListArea);
	mListLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	mListLayout->setAlignment(Qt::AlignTop);
	mListLayout->setSpacing(1);
	mListWidget->setLayout(mListLayout);
	mListArea->setWidget(mListWidget);
	mLayout->addWidget(mListArea, 1, 0);
}

/**
 * Creates the Gui-elements for removing value visualizations and update data.
 */
void ValueDetailPanel::createControlButtonView() {
	mControlButtons = new QWidget;
	mControlButtonLayout = new QHBoxLayout();
	mControlButtons->setLayout(mControlButtonLayout);
	mLayout->addWidget(mControlButtons);
	mRemoveAllValues = new QPushButton("Remove All");
	mUpdateData = new QPushButton("Update Data");
	mControlButtonLayout->addWidget(mUpdateData);
	mControlButtonLayout->addWidget(mRemoveAllValues);

	connect(mRemoveAllValues, SIGNAL(clicked()), this, SLOT(removeAllValuesFromList()));
	connect(mUpdateData, SIGNAL(clicked()), this, SLOT(updateData()));
}

/**
 * This slot creates the value visualizations of the selected values.
 */
void ValueDetailPanel::showMatchingValues() {
	QVector<QString> selectedValues;
	QString temp = mValueLineEdit->text();
	temp = temp.replace("**", ".*");
	temp.append(".*").prepend(".*");
	QList<Value*> values = mValueManager->getValuesMatchingPattern(temp, false);

	if(values.size() > 100) {
		QMessageBox::information(this, "Too many selected values.",
					"Operation not allowed!\n\n"
					"This operation would add too many values to the property panel."
					"Use the regular expression field to constrain the number of "
					"values to a number below 100.");
		return;
	}

	for(int i = 0; i < values.size(); i++) {
		Value *value = values.at(i);
		QMap<Value*, ValueDetailVisualization*>::iterator index =
		mValueDetailVisualizations.find(value);
		if(index == mValueDetailVisualizations.end()) {
			ValueDetailVisualization *newVis = new ValueDetailVisualization(value, mValueManager);
			mValueDetailVisualizations[value] = newVis;
			newVis->installEventFilter(this);
			mListLayout->addWidget(newVis, 0, Qt::AlignTop);
		}
	}
}

/**
 * This slot removes all value visualizations.
 */
void ValueDetailPanel::removeAllValuesFromList() {
	QMap<Value*, ValueDetailVisualization*>::iterator it = mValueDetailVisualizations.begin();
	while(it != mValueDetailVisualizations.end()) {
		ValueDetailVisualization *valueVisu = it.value();
		// erase the Visualization from the QMap before sending close event
		// so the event filter of this class doesn't erase the Visualization (SegFault!)
		it = mValueDetailVisualizations.erase(it);
		valueVisu->close();
	}
}

/**
 * This slot reloads all value visualizations.
 */
void ValueDetailPanel::updateData() {
	QMap<Value*, ValueDetailVisualization*>::iterator it;
	for(it = mValueDetailVisualizations.begin(); it != mValueDetailVisualizations.end(); it++) {
		it.value()->fillContents();
	}
}

/**
 * This slot fills the ComboBox with all values matching the search pattern and updates the number of values found matching the pattern.
 *
 * @param text the search pattern from the TextEdit
 */
void ValueDetailPanel::fillValueList(const QString &text) {
	QString temp = text;
	temp = temp.replace("**", ".*");
	temp.append(".*").prepend(".*");
	QList<Value*> values = mValueManager->getValuesMatchingPattern(temp, false);
	mValueComboBox->clear();
	for(int i = 0; i < values.size(); i++) {
		Value *value = values.at(i);
		QList<QString> names = mValueManager->getNamesOfValue(value);
		QListIterator<QString> it(names);
		for(; it.hasNext(); ) {
			QString name = it.next();
			mValueComboBox->addItem(name);
		}
	}
	QString number;
	number.setNum(values.size());
	mAvailableValueCount->setText(number);
}

/**
 * Catches the close event of the ValueDetailVisualizations to remove them from mValueDetailVisualizations.
 *
 * @param object the sender object
 * @param event the event to filter
 */
bool ValueDetailPanel::eventFilter(QObject *object, QEvent *event) {
	if(event->type() == QEvent::Close) {
		ValueDetailVisualization *visu = dynamic_cast<ValueDetailVisualization*>(object);
		if(visu != NULL) {
			QMap<Value*, ValueDetailVisualization*>::iterator index = mValueDetailVisualizations.find(visu->getValue());
			if(index != mValueDetailVisualizations.end()) {
				mValueDetailVisualizations.erase(index);
			}
		}
		else {
			qDebug("ValueDetailPanel: Close Event received from ValueDetailVisualization, but could not cast sender object to ValueDetailVisualization! Event ignored.");
			return true;
		}
	}
	return false;	// send event to the target object itself
}

/**
 * Destruktor.
 */
ValueDetailPanel::~ValueDetailPanel() {
}

/**
 * Catches the close event to save the last Window position
 */
void ValueDetailPanel::closeEvent(QCloseEvent*) {
	mXPos = x();
	mYPos = y();
}

/**
 * This slot restores the last Window position before showing the window
 */
void ValueDetailPanel::showWindow() {
	move(mXPos, mYPos);
	show();
}

/**
 * Catches the show event to ensure that the comboBox is initially filled with the current parameter names.
 */
void ValueDetailPanel::showEvent(QShowEvent*) {
	if(mValueLineEdit != 0) {
		fillValueList(mValueLineEdit->text());
	}
}

}
