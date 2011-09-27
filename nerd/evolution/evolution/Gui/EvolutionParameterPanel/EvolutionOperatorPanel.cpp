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



#include "EvolutionOperatorPanel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "Value/ValueManager.h"
#include "Gui/EvolutionParameterPanel/ParameterControlWidget.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new EvolutionOperatorPanel.
 */
EvolutionOperatorPanel::EvolutionOperatorPanel(const QString &prefix, const QString &operatorName)
	: mEnableValue(0), mPrefix(prefix), mOperatorName(operatorName)
{
	setWindowTitle("Evolution Operator Panel");
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(2, 0, 2, 0);
	setLayout(layout);

	QFrame *upperWidget = new QFrame();
	QHBoxLayout *upperLayout = new QHBoxLayout();
	upperWidget->setLayout(upperLayout);
	layout->addWidget(upperWidget);

	upperWidget->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
	upperWidget->setLineWidth(1);

	mHideParameters = new QPushButton("Hide");
	mHideParameters->setCheckable(true);
	mHideParameters->setContentsMargins(0, 0, 0, 0);
	mHideParameters->setMaximumWidth(60);
	mDisableOperator = new QPushButton("Disable");
	mDisableOperator->setCheckable(true);
	mDisableOperator->setContentsMargins(0, 0, 0, 0);
	mDisableOperator->setMaximumWidth(65);
	mNameLabel = new QLabel(operatorName);

	upperLayout->addWidget(mNameLabel);
	upperLayout->addStretch(4);
	upperLayout->addWidget(mHideParameters);
	upperLayout->addWidget(mDisableOperator);
	upperLayout->setContentsMargins(1, 1, 1, 1);

	setContentsMargins(1, 0, 1, 0);

	mParameterWidget = new QFrame();
	layout->addWidget(mParameterWidget);
	mParameterWidget->setContentsMargins(1, 0, 1, 0);

	QHBoxLayout *parameterMainLayout = new QHBoxLayout();
	mParameterWidget->setLayout(parameterMainLayout);

	QGridLayout *parameterLayout = new QGridLayout();
	parameterMainLayout->addLayout(parameterLayout);
	parameterMainLayout->addStretch(5);

	//Create Variable content
	int parameterWidgetWidth = 120;
	ValueManager *vm = Core::getInstance()->getValueManager();

	int prefixSize = prefix.size() + operatorName.size() + 1;
	QStringList parameters = vm->getValueNamesMatchingPattern(prefix + operatorName + "/.*");
	for(QListIterator<QString> i(parameters); i.hasNext();) {
		QString name = i.next();
		if(name.contains("/Config/") || name.contains("/Performance/")) {
			continue;
		}
		Value *value = vm->getValue(name);
		if(value == 0) {
			continue;
		}
		ParameterControlWidget *controlWidget = 
						new ParameterControlWidget(name.mid(prefixSize), value, parameterWidgetWidth);
		mControlWidgets.append(controlWidget);
	}

	for(int i = 0; i < mControlWidgets.size(); ++i) {
		ParameterControlWidget *widget = mControlWidgets.at(i);
		parameterLayout->addWidget(widget, i / 4, i % 4);
	}
	int horSize = mControlWidgets.size();
	while(horSize < 4) {
		QWidget *placeHolder = new QWidget();
		placeHolder->setFixedWidth(parameterWidgetWidth);
		parameterLayout->addWidget(placeHolder, 0, horSize);
		++horSize;
	}
		
	//set up enableValue to control the enable state of the operator.
	mCanBeDisabled = vm->getBoolValue(prefix + operatorName + "/Config/CanBeDisabled");
	mEnableValue = vm->getBoolValue(prefix + operatorName + "/Config/Enable");
	if(mEnableValue != 0) {
		mEnableValue->addValueChangedListener(this);
	}
	else {
		mDisableOperator->setChecked(false);
	}
	checkEnableState();
	mHideParameters->setChecked(false);


	connect(mDisableOperator, SIGNAL(toggled(bool)),
			this, SLOT(checkEnableState(bool)));
	connect(mHideParameters, SIGNAL(toggled(bool)),
			this, SLOT(checkHideState(bool)));
	
}


/**
 * Destructor.
 */
EvolutionOperatorPanel::~EvolutionOperatorPanel() {
}

int EvolutionOperatorPanel::getIndex() const {
	return 0;
}


void EvolutionOperatorPanel::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mEnableValue) {
		mDisableOperator->setChecked(!mEnableValue->get());
	}
}


QString EvolutionOperatorPanel::getName() const {
	return "EvolutionOperatorPanel";
}


QString EvolutionOperatorPanel::getOperatorName() const {
	return mOperatorName;
}


QString EvolutionOperatorPanel::getPrefix() const {
	return mPrefix;
}

void EvolutionOperatorPanel::cleanUp() {
	mCanBeDisabled = 0;
	if(mEnableValue != 0) {
		mEnableValue->removeValueChangedListener(this);
		mEnableValue = 0;
	}
	for(QListIterator<ParameterControlWidget*> i(mControlWidgets); i.hasNext();) {
		i.next()->cleadUp();
	}
}


void EvolutionOperatorPanel::checkEnableState(bool) {
	if(mEnableValue == 0) {
		mDisableOperator->setChecked(false);
		mDisableOperator->setEnabled(false);
		mNameLabel->setEnabled(true);
		mParameterWidget->setEnabled(true);
	}
	else {
		if(mEnableValue != 0 && mCanBeDisabled != 0) {
			mDisableOperator->setEnabled(mCanBeDisabled->get());
		}
		else {
			mDisableOperator->setEnabled(true);
		}
		mEnableValue->set(!mDisableOperator->isChecked());
		mNameLabel->setEnabled(mEnableValue->get());
		mParameterWidget->setEnabled(mEnableValue->get());
	}
}


void EvolutionOperatorPanel::checkHideState(bool) {
	if(mHideParameters->isChecked()) {
		mParameterWidget->hide();
	}
	else {
		mParameterWidget->show();
	}
}



}



