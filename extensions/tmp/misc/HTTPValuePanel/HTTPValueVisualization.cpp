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

#include "HTTPValueVisualization.h"
#include <QCloseEvent>
#include <QPalette>

namespace nerd {

HTTPValueVisualization::HTTPValueVisualization(Value *value, QString valueName) :
	mValue(value), mValueName(valueName)
{
	setAttribute(Qt::WA_DeleteOnClose, true);

	// create widgets
	mNameLabel = new QLabel(mValueName);
	mSelectCheckBox = new QCheckBox;
	mSelectCheckBox->setCheckState(Qt::Checked);
	mValueField = new QLineEdit(value->getValueAsString());
	mCloseButton = new QPushButton("x");
	mCloseButton->setFixedWidth(20);
	mValueField->setFixedWidth(350);

	// create layouts
	mMainLayout = new QVBoxLayout;
	setLayout(mMainLayout);
	mSubLayout = new QHBoxLayout;
	mMainLayout->addWidget(mNameLabel);
	mMainLayout->addLayout(mSubLayout);

	mSubLayout->addWidget(mSelectCheckBox);
	mSubLayout->addWidget(mValueField);
	mSubLayout->addStretch(1000);
	mSubLayout->addWidget(mCloseButton);

	setFrameStyle(QFrame::Panel | QFrame::Plain);
	setLineWidth(2);

	// connect widgets
	connect(mCloseButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(mValueField, SIGNAL(returnPressed()), this, SLOT(changeValue()));
	connect(mValueField, SIGNAL(textEdited(const QString&)), this, SLOT(valueEdited()));

	// register for value changed event
	mValue->addValueChangedListener(this);
	
}

HTTPValueVisualization::~HTTPValueVisualization() {
}

Value* HTTPValueVisualization::getValue() {
	return mValue;
}

void HTTPValueVisualization::changeValue() {
	mValue->setValueFromString(mValueField->text());
	mValueField->setText(mValue->getValueAsString());
	QPalette p = palette();
	p.setColor(QPalette::Base, Qt::white);
	mValueField->setPalette(p);
}

void HTTPValueVisualization::valueEdited() {
	QPalette p = palette();
	p.setColor(QPalette::Base, QColor(255,120,120));
	mValueField->setPalette(p);
}

void HTTPValueVisualization::valueChanged(Value *value) {
	if(value == mValue) {
		QString mValueString = value->getValueAsString();
		mValueField->setText(mValueString);
	}
}

QString HTTPValueVisualization::getName() const {
	return "HTTPValueVisualization";
}

void HTTPValueVisualization::closeEvent(QCloseEvent *event) {
	mValue->removeValueChangedListener(this);
	event->accept();
}

bool HTTPValueVisualization::isSelected() {
	return (mSelectCheckBox->checkState() != Qt::Unchecked);
}

QString HTTPValueVisualization::getValueName() {
	return mValueName;
}

}
