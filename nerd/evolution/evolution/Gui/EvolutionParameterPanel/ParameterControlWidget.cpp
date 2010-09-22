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


#include "ParameterControlWidget.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QVBoxLayout>
#include <QLabel>
#include "Math/Math.h"
#include "Value/ChangeValueTask.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new ParameterControlWidget.
 */
ParameterControlWidget::ParameterControlWidget(const QString &name, Value *value, int width)
	: mValue(value), mWidth(width)
{
	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(1, 0, 1, 0);
	setLayout(layout);

	QLabel *nameLabel = new QLabel(name);
	nameLabel->setToolTip(name);
	nameLabel->setFixedWidth(mWidth);
	mValueContent = new QLineEdit("");
	mValueContent->setFixedWidth(mWidth);

	if(mValue != 0) {
		mValue->addValueChangedListener(this);
		mValueContent->setText(mValue->getValueAsString());
	}

	layout->addWidget(nameLabel);
	layout->addWidget(mValueContent);

	connect(mValueContent, SIGNAL(returnPressed()),
			this, SLOT(returnPressed()));
	connect(mValueContent, SIGNAL(textEdited(const QString&)),
			this, SLOT(valueTextChanged(const QString&)));
}


/**
 * Destructor.
 */
ParameterControlWidget::~ParameterControlWidget() {
}


void ParameterControlWidget::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mValue) {
		mValueContent->setText(mValue->getValueAsString());
	}
}


QString ParameterControlWidget::getName() const {
	return "ParameterControlWidget";
}

int ParameterControlWidget::getWidth() {
	return mWidth;
}


void ParameterControlWidget::cleadUp() {
	if(mValue != 0) {
		mValue->removeValueChangedListener(this);
		mValue = 0;
	}
}


void ParameterControlWidget::valueTextChanged(const QString&) {
	if(mValue == 0) {
		return;
	}
	if(mValueContent->text() == mValue->getValueAsString()) {
		markAsValueUpdated();
	}
	else {
		markAsValueEdited();
	}
}

void ParameterControlWidget::returnPressed() {
	if(mValueContent->text() != mValue->getValueAsString()) {
		Core::getInstance()->scheduleTask(new ChangeValueTask(mValue, mValueContent->text()));
	}
	markAsValueUpdated();
}

/**
 * Marks the background color of the text label red to show that the content
 * has been changed manually.
 */
void ParameterControlWidget::markAsValueEdited() {
	QPalette p = mValueContent->palette();
	p.setColor(QPalette::Base, QColor(255,120,120));
	mValueContent->setPalette(p);
}


/**
 * Marks the background color of the text label white to show that the content
 * originates from the observed Value.
 */
void ParameterControlWidget::markAsValueUpdated() {
	QPalette p = mValueContent->palette();
	p.setColor(QPalette::Base, Qt::white);
	mValueContent->setPalette(p);
}

}



