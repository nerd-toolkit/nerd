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
	 mParameterName(name), mParameter(parameter), mInvalid(false)
{

	mParameterContent = new QLineEdit();
	if(mParameter != 0) {
		mParameterContent->setText(mParameter->getValueAsString());
		mParameter->addValueChangedListener(this);
		mRegisteredAsListener = true;
	}
	mNameLabel = new QLabel(name);
	int row = targetLayout->rowCount();
	targetLayout->addWidget(mNameLabel, row, 0);
	targetLayout->addWidget(mParameterContent, row, 1);

	connect(mParameterContent, SIGNAL(returnPressed()),
			this, SLOT(parameterContentChanged()));
    connect(mParameterContent, SIGNAL(textEdited(const QString&)),
			this, SLOT(markAsValueEdited()));
}



/**
 * Destructor.
 */
ParameterPanel::~ParameterPanel() {
	if(mParameter != 0 && mRegisteredAsListener) {
		mParameter->removeValueChangedListener(this);
	}
	if(mTargetLayout != 0) {
		mTargetLayout->removeWidget(mParameterContent);
	}
	delete mParameterContent;
	delete mNameLabel;
}

void ParameterPanel::valueChanged(Value *value)  {
	if(value == 0 || mInvalid) {
		return;
	}
	if(value == mParameter) {
		mParameterContent->setText(mParameter->getValueAsString());
		emit markAsValueUpdated();
	}
}

void ParameterPanel::forceListenerDeregistration(Value *value) {
	ValueChangedListener::forceListenerDeregistration(value);

	if(value != 0 && value == mParameter) {
		mRegisteredAsListener = false;
	}
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


void ParameterPanel::parameterContentChanged() {
	if(mParameter == 0 || mInvalid) {
		return;
	}
	QString newContent = mParameterContent->text().trimmed();

	if(mParameter->getValueAsString() == newContent) {
		return;
	}

	emit parameterChanged(mParameter, mParameterName, newContent);
	emit markAsValueUpdated();
}

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



}




