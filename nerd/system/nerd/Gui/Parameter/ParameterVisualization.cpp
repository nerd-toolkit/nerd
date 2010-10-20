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



#include "ParameterVisualization.h"
#include "ParameterVisualizationWindow.h"
#include <QString>
#include <QGridLayout>
#include <QPalette>
#include "Core/Task.h"
#include "Core/Core.h"
#include <iostream>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Value/StringValue.h"
#include "Util/NerdFileSelector.h"
#include "NerdConstants.h"

using namespace std;

namespace nerd{

class ChangeValue : public Task {

	public:
		ChangeValue(Value *value, const QString &newValue) {
			mValue = value;
			mNewValue = newValue;
		}

		virtual ~ChangeValue() {}

		bool runTask() {
			if(mValue == 0) {
				return true;
			}
			mValue->setValueFromString(mNewValue);
			return true;
		}

	private:
		Value *mValue;
		QString mNewValue;
};



ParameterVisualization::ParameterVisualization(ParameterVisualizationWindow *list,
				Value *value, QString name, SetInitValueTask *setInitValueTaskPrototype)
	: QFrame(), mValueField(0), mValueBox(0),
	  mUpdateSnapshotButton(0), mSetInitValueTaskPrototype(setInitValueTaskPrototype)
{
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	mValueList = list;
	mValue = value;
	mValueName = name;

	mNameLabel = new QLabel(mValueName);
	mValueBox = new QComboBox(this);
	mValueBox->setEditable(true);

	if(mValue != 0) {

		QString value = mValue->getValueAsString();

		StringValue *stringValue = dynamic_cast<StringValue*>(mValue);
		if(stringValue != 0 && stringValue->isUsedAsFileName()) {
			mValueBox->addItem(NerdConstants::GUI_SELECT_FILE_FOR_STRING_VALUE);
		}
        QList<QString> options = mValue->getOptionList();
        for(QListIterator<QString> i(options); i.hasNext();) {
            mValueBox->addItem(i.next());
        }
		mValueBox->setEditText(value);
	}


	mValueField = mValueBox->lineEdit();
	mCloseButton = new QPushButton("x");
	mCloseButton->setWhatsThis("Remove this property from the list");

	mMoveUpButton = new QPushButton();
	mMoveUpButton->setWhatsThis("Moves this property up in the list");
	mMoveDownButton = new QPushButton();
	mMoveDownButton->setWhatsThis("Moves this property down in the list");

	if(mValue != 0) {
        mNameLabel->setWhatsThis(mValue->getDescription());
        mValueBox->setWhatsThis(mValue->getDescription());
	}

	mUpdateSnapshotButton = new QPushButton("Set Init");
	mUpdateSnapshotButton->setWhatsThis("Use the current setting after a reset");

	mUpdateValue = new QCheckBox();
	mUpdateValue->setWhatsThis("Update content after each change");

	connect(mValueField, SIGNAL(returnPressed()),
			this, SLOT(changeValue()));
	connect(mValueField, SIGNAL(textEdited(const QString&)),
			this, SLOT(markAsValueEdited()));
    connect(mValueBox, SIGNAL(activated(const QString&)),
            this, SLOT(itemSelected(const QString&)));
	connect(mCloseButton, SIGNAL(clicked()),
			this, SLOT(destroy()));
	connect(mUpdateSnapshotButton, SIGNAL(clicked()),
			this, SLOT(updateValueInEnvironmentManager()));
	connect(this, SIGNAL(lineEditTextChanged(QString)),
			mValueBox,  SLOT(setEditText(QString)));
	connect(this, SIGNAL(lineEditTextChanged(QString)),
			this, SLOT(markAsValueUpdated()));
	connect(mUpdateValue, SIGNAL(stateChanged(int)),
			this, SLOT(setDoUpdateValue(int)));
	connect(mMoveUpButton, SIGNAL(pressed()),
			this, SLOT(moveWidgetUp()));
	connect(mMoveDownButton, SIGNAL(pressed()),
			this, SLOT(moveWidgetDown()));

	//lineEditTextChanged(mValue->getValueAsString());
	mUpdateValue->setCheckState(Qt::Checked);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(1);
	setLayout(layout);
	layout->addWidget(mNameLabel);
	layout->setAlignment(mNameLabel, Qt::AlignLeft);

	QHBoxLayout *mFrameLayout = new QHBoxLayout();
	layout->addLayout(mFrameLayout);

	QVBoxLayout *moveLayout = new QVBoxLayout();
	moveLayout->setContentsMargins(0, 0, 0, 0);
	moveLayout->setSpacing(0);
	//moveLayout->addStretch(1000);
	moveLayout->addWidget(mMoveUpButton);
	moveLayout->addWidget(mMoveDownButton);

	mFrameLayout->setMargin(1);
	mFrameLayout->addWidget(mUpdateValue);
	mFrameLayout->addWidget(mValueBox);
	if(mSetInitValueTaskPrototype != 0 && name.startsWith("/Sim/")) {
		mFrameLayout->addWidget(mUpdateSnapshotButton);
	}
	mFrameLayout->addStretch(1000);
	mFrameLayout->addLayout(moveLayout);
	mFrameLayout->addWidget(mCloseButton);

	mUpdateSnapshotButton->setFixedWidth(60);
	mCloseButton->setFixedWidth(20);
	mMoveUpButton->setFixedSize(15,13);
	mMoveDownButton->setFixedSize(15,13);
	mValueBox->setFixedWidth(260);

	setFrameStyle(QFrame::Panel | QFrame::Plain);
	setLineWidth(2);

	mValue->addValueChangedListener(this);
}


/**
 * Destructor.
 */
ParameterVisualization::~ParameterVisualization() {
	reset();
}


/**
 * Called whenever the observer checkbox is checked or unchecked.
 * @param doUpdate indicates whether the update is enabled or not.
 */
void ParameterVisualization::setDoUpdateValue(int doUpdate)  {
	mDoUpdateValue = doUpdate;

	if(mValue == 0) {
		return;
	}

	if(!mDoUpdateValue) {
		mValue->removeValueChangedListener(this);
		mUpdateValue->setCheckState(Qt::Unchecked);
	}
	else {
		//regiser as listener and set the edit text to the current value.
		lineEditTextChanged(mValue->getValueAsString());
		mValue->addValueChangedListener(this);
		mUpdateValue->setCheckState(Qt::Checked);

		markAsValueUpdated();
	}
}


/**
 * This method copies the content of the text field into the
 * reset snapshot of the SimulationEnvironmentManager. This applies
 * value changes permanently, as the values from the reset shapshot are
 * taken to initialize the environment and agents during a reset.
 */
void ParameterVisualization::updateValueInEnvironmentManager() {


	if(mValue == 0 || mSetInitValueTaskPrototype == 0 || !mValueName.startsWith("/Sim/")) {
		return;
	}
	Core::getInstance()->scheduleTask(mSetInitValueTaskPrototype->create(mValue, mValueBox->currentText()));

}


void ParameterVisualization::moveWidgetUp() {
	emit move(this, true);
}


void ParameterVisualization::moveWidgetDown() {
	emit move(this, false);
}



/**
 * Triggers the destruction of this object. In principle this method simply
 * resets the visualization and then emits signal destroyThis().
 * Exactly one object (usually the owning ParameterVisualizationWindow) should
 * connect to this signal WITH MODIFIER Qt::QueuedConnectionto ensure that
 * the ParameterVisualization stopped processing before it is finally deleted
 * in the owner ParameterVisualizationWindow.
 */
void ParameterVisualization::destroy() {
	reset();
	emit destroyThis(mValueName);
}


/**
 * Returns the name of this ParameterVisualization object.
 *
 * @return the name
 */
QString ParameterVisualization::getName() const{
	return QString("ParameterVisualization [").append(mValueName).append("]");
}


/**
 * Returns the name that is associated with the observed Value.
 *
 * @return the name of the Value.
 */
QString ParameterVisualization::getValueName() const {
	return mValueName;
}


/**
 * Called when the observed Value changed. This will update the text field with the
 * current value of the Value.
 *
 * @param value the value that changed.
 */
void ParameterVisualization::valueChanged(Value *value) {

	if(value == 0) {
		return;
	}
	else if(value == mValue) {
		if(mDoUpdateValue) {
			lineEditTextChanged(value->getValueAsString());
		}
	}
}



/**
 * Returns the currently observed Value object.
 *
 * @return the currently observed Value.
 */
Value* ParameterVisualization::getValue() {
	return mValue;
}


/**
 * Returns true if the visualization currently is observing the Value.
 *
 * @return true if observation is active, otherwise false.
 */
bool ParameterVisualization::isValueUpdateActive() const {
	return (mUpdateValue->checkState() == Qt::Checked);
}


void ParameterVisualization::setValueObjectByName(const QString &name) {
	ValueManager *vm = Core::getInstance()->getValueManager();

	if(mValue != 0) {
		mValue->removeValueChangedListener(this);
	}
	mValueName = name;
	mValue = vm->getValue(mValueName);

	if(mValue == 0) {
		mNameLabel->setText("Not Found: " + mValueName);
	}
	else {
		mNameLabel->setText(mValueName);

		setDoUpdateValue(mDoUpdateValue);
	}
}


/**
 * Sets the current value content for the text field.
 *
 * @param currentValue the content of the text field.
 */
void ParameterVisualization::setCurrentValue(const QString &currentValue) {
	emit lineEditTextChanged(currentValue);
	markAsValueEdited();
	mValueModified = true;
}


QString ParameterVisualization::getCurrentValue() const {
	return mValueBox->currentText();
}

void ParameterVisualization::addOption(const QString &optionText) {
	if(mValue == 0) {
		return;
	}
	for(int i = 0; i < mValueBox->count(); ++i) {
        if(mValueBox->itemText(i) == optionText) {
            return;
        }
	}
	QString currentValue = mValueBox->currentText();
	mValueBox->addItem(optionText);
	mValueBox->setEditText(currentValue);
}

QList<QString> ParameterVisualization::getOptions() const {
    QList<QString> options;
    for(int i = 0; i < mValueBox->count(); ++i) {
        options.append(mValueBox->itemText(i));
    }
    return options;
}


void ParameterVisualization::itemSelected(const QString &item) {
	QString currentText = item;
	if(mValue != 0 && item == NerdConstants::GUI_SELECT_FILE_FOR_STRING_VALUE) {
		QString fileName = NerdFileSelector::getFileName("Select File", true, this);
		if(fileName.trimmed() == "") {
			mValueBox->setEditText(mValue->getValueAsString());
			mValueModified = true;
			markAsValueUpdated();
			return;
		}
		currentText = fileName;
	}
    mValueBox->setEditText(currentText);
    mValueModified = true;
    changeValue();
}


/**
 * Called when the user pressed the enter key in the text field.
 * If the text has been changed, then the content is set at the Value object.
 * Setting the new value is done via Task in the simulation thread, so the change
 * might be applied with a short delay.
 */
void ParameterVisualization::changeValue() {
	markAsValueUpdated();
	if(mValueField->isModified() || mValueModified) {
		ChangeValue *changeValueTask = new ChangeValue(mValue, mValueBox->currentText());
		Core::getInstance()->scheduleTask(changeValueTask);
	}
	mValueModified = false;

	//check whether the current text is a new one (if so, add it to the combo list)
	addOption(mValueBox->currentText());
}


/**
 * Resets the Object. Hereby the Value object is released and observation canceled.
 */
void ParameterVisualization::reset() {
	if(mValue != 0) {
		mValue->removeValueChangedListener(this);
	}
	mValue = 0;
}


/**
 * Marks the background color of the text label red to show that the content
 * has been changed manually.
 */
void ParameterVisualization::markAsValueEdited() {
	QPalette p = mValueField->palette();
	p.setColor(QPalette::Base, QColor(255,120,120));
	mValueField->setPalette(p);
}


/**
 * Marks the background color of the text label white to show that the content
 * originates from the observed Value.
 */
void ParameterVisualization::markAsValueUpdated() {
	QPalette p = mValueField->palette();
	p.setColor(QPalette::Base, Qt::white);
	mValueField->setPalette(p);
}


}
