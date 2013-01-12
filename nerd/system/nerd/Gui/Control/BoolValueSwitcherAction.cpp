/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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



#include "BoolValueSwitcherAction.h"
#include "Core/Core.h"

namespace nerd {

BoolValueSwitcherAction::BoolValueSwitcherAction(const QString &name, const QString &boolValueName)
	: QAction(name, 0), mBoolValue(0), mBoolValueName(boolValueName)
{
	setCheckable(true);
	mName = QString("BoolValueSwitcherAction (").append(name).append(")");
	connect(this, SIGNAL(triggered(bool)), this, SLOT(checkBoxActivated(bool)));
	connect(this, SIGNAL(boolStateChanged(bool)), this, SLOT(setChecked(bool)));
	connect(this, SIGNAL(enableAction(bool)), this, SLOT(setEnabled(bool)));

	Core::getInstance()->addSystemObject(this);
}


BoolValueSwitcherAction::~BoolValueSwitcherAction() {
}


bool BoolValueSwitcherAction::init() {
	return true;
}


bool BoolValueSwitcherAction::bind() {
	bool ok = true;

	ValueManager *vm = Core::getInstance()->getValueManager();

	mBoolValue = vm->getBoolValue(mBoolValueName);

	if(mBoolValue == 0) {
		setChecked(false);
		emit enableAction(false);
	}
	else {
		mBoolValue->addValueChangedListener(this);
		setChecked(mBoolValue->get());
		emit enableAction(true);
	}

	return ok;
}


bool BoolValueSwitcherAction::cleanUp() {
	if(mBoolValue != 0) {
		mBoolValue->removeValueChangedListener(this);
	}
	Core::getInstance()->removeSystemObject(this);
	return true;
}


void BoolValueSwitcherAction::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mBoolValue) {
		boolStateChanged(mBoolValue->get());
	}
}


QString BoolValueSwitcherAction::getName() const {
	return mName;
}

void BoolValueSwitcherAction::checkBoxActivated(bool checked) {
	if(mBoolValue == 0) {
		return;
	}
	if(checked && !mBoolValue->get()) {
		mBoolValue->set(true);
	}
	else if(!checked && mBoolValue->get()) {
		mBoolValue->set(false);
	}
}

}


