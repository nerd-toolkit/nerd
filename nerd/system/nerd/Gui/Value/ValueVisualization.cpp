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



#include "ValueVisualization.h"
#include "ValueVisualizationWindow.h"
#include "Core/Task.h"
#include "Core/Core.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPalette>


namespace nerd {

class ChangeValue:public Task {

public:
	ChangeValue(Value *value, const QString &newValue) {
		mValue = value;
		mNewValue = newValue;
	}

	~ChangeValue() {}

	bool runTask() {
		mValue->setValueFromString(mNewValue);
		return true;
	}

	private:
		Value *mValue;
		QString mNewValue;
};


ValueVisualization::ValueVisualization(ValueVisualizationWindow *list, Value *value, 
		QString name) 
{
	mValueChanged = false;
	mValueList = list;
	mValueName = name;
	mValue = value;
	mUpdateValue = true;
	mNameLabel = new QLabel(mValueName);
	mValueField = new QLineEdit(this);
	mUpdateValueCheckBox = new QCheckBox();
	mValueField->setText(mValue->getValueAsString());
	connect(mValueField, SIGNAL(returnPressed()), this, SLOT(changeValue()));
	connect(mValueField, SIGNAL(textEdited(const QString&)), this, SLOT(valueEdited()));
	connect(mUpdateValueCheckBox, SIGNAL(stateChanged(int)), this,
		SLOT(changeUpdateProperty(int)));
	mUpdateValueCheckBox->setCheckState(Qt::Checked);
	mCloseButton = new QPushButton("x");
	connect(mCloseButton, SIGNAL(clicked()), this, SLOT(destroy()));
	connect(this, SIGNAL(lineEditTextChanged(QString)), mValueField, SLOT(setText(QString)));

	mValueManipulationFrame = new QFrame();

	QVBoxLayout *layout = new QVBoxLayout;
	setLayout(layout);
	layout->setMargin(1);

	QHBoxLayout *subLayout = new QHBoxLayout;
	layout->addWidget(mNameLabel);
	layout->addLayout(subLayout);

	subLayout->addWidget(mUpdateValueCheckBox);
	subLayout->addWidget(mValueField);
	subLayout->addStretch(1000);
	subLayout->addWidget(mCloseButton);

	mCloseButton->setFixedWidth(20);
	mValueField->setFixedWidth(250);

	QPalette p = palette();
	p.setColor( QPalette::Normal, QPalette::Foreground, Qt::black );
	mValueField->setPalette(p);
	setFrameStyle(QFrame::Panel | QFrame::Plain);
	setLineWidth(2);
	mValue->addValueChangedListener(this);
}

void ValueVisualization::deleteLater() {
	mValue->removeValueChangedListener(this);
	mValueList->deleteValueFromList(mValueName);
}

ValueVisualization::~ValueVisualization() {
}

QString ValueVisualization::getName() const{
	return "ValueVisualization";
}

void ValueVisualization::valueChanged(Value *value) {

	if(value == mValue && mUpdateValue) {
		QString mValueValue = value->getValueAsString();
		lineEditTextChanged(mValueValue);
	}
}

void ValueVisualization::changeUpdateProperty(int newState) {
		mUpdateValue = newState;
		if(!mUpdateValue) {
			mValue->removeValueChangedListener(this);
			mUpdateValueCheckBox->setCheckState(Qt::Unchecked);
		}
		else{
			mValue->addValueChangedListener(this);
			QString mValueValue = mValue->getValueAsString();
			lineEditTextChanged(mValueValue);
			QPalette p = palette();
			p.setColor(QPalette::Base, Qt::white);
			mValueField->setPalette(p);
			mUpdateValueCheckBox->setCheckState(Qt::Checked);
		}
}

void ValueVisualization::changeValue() {
	if(mValueChanged) {
		ChangeValue *task = new ChangeValue(mValue, mValueField->text());
		Core::getInstance()->scheduleTask(task);
		mValueField->setText(mValue->getValueAsString());
	}
	QPalette p = palette();
	p.setColor(QPalette::Base, Qt::white);
	mValueField->setPalette(p);
	mValueChanged = false;
}

void ValueVisualization::valueEdited() {
	QPalette p = palette();
	p.setColor(QPalette::Base, QColor(255,120,120));
	mValueField->setPalette(p);
	mValueChanged = true;
}

void ValueVisualization::reset() {
	if(mValue != 0) {
		mValue->removeValueChangedListener(this);
	}
	mValue = 0;
}


Value* ValueVisualization::getValue() {
	return mValue;
}

void ValueVisualization::setCurrentValue(const QString &currentValue) {
	lineEditTextChanged(currentValue);
	valueEdited();
	mValueChanged = true;
}

void ValueVisualization::destroy() {
	reset();
	emit destroyThis(mValueName);
}

void ValueVisualization::setDoUpdateValue(int doUpdate) {
	mUpdateValue= doUpdate;

	if(!mUpdateValue) {
		mValue->removeValueChangedListener(this);
		mUpdateValueCheckBox->setCheckState(Qt::Unchecked);
	}
	else {
		mValue->addValueChangedListener(this);
		mUpdateValueCheckBox->setCheckState(Qt::Checked);
	}
}

}
