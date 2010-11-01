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



#include "UniversalScriptingContext.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "NerdConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new UniversalScriptingContext.
 */
UniversalScriptingContext::UniversalScriptingContext(const QString &name, 
			const QString &fileName, const QString &triggerEventName, 
			const QString &resetEventName)
	: ScriptingContext(name), mInitialized(false), mTriggerEventName(0),
	  mResetEventName(0), mTriggerEvent(0), mResetEvent(0)
	
{
	ValueManager *vm = Core::getInstance()->getValueManager();

	mTriggerEventName = new StringValue(triggerEventName);
	mTriggerEventName->addValueChangedListener(this);
	mResetEventName = new StringValue(resetEventName);
	mResetEventName->addValueChangedListener(this);

	vm->addValue("/UniversalScripting/" + name + "/ScriptFileName", mScriptFileName);
	vm->addValue("/UniversalScripting/" + name + "/ScriptCode", mScriptCode);
	vm->addValue("/UniversalScripting/" + name + "/TriggerEvent", mTriggerEventName);
	vm->addValue("/UniversalScripting/" + name + "/ResetEvent", mResetEventName);

	mScriptCode->set("function exec() { }");
	mScriptFileName->set(fileName);
	mScriptFileName->setNotifyAllSetAttempts(true);

	Core::getInstance()->addSystemObject(this);
}


/**
 * Copy constructor. 
 * 
 * @param other the UniversalScriptingContext object to copy.
 */
UniversalScriptingContext::UniversalScriptingContext(const UniversalScriptingContext &other) 
	: SystemObject(), ScriptingContext(other), mInitialized(false), mTriggerEvent(0),
	  mResetEvent(0)
{
	//DO NOT COPY
	Core::log("ERROR: UniversalScriptingContext must not be copied!");
}

/**
 * Destructor.
 */
UniversalScriptingContext::~UniversalScriptingContext() {
}

QString UniversalScriptingContext::getName() const {
	return ScriptingContext::getName();
}

bool UniversalScriptingContext::init() {
	bool ok = true;

	return ok;
}

bool UniversalScriptingContext::bind() {
	bool ok = true;

	mInitialized = true;
	setResetEventName(mResetEventName->get());
	setTriggerEventName(mTriggerEventName->get());

	mScriptFileName->set(mScriptFileName->get());

	//if no trigger event is given, execute once during bind() phase.
	if(mTriggerEvent == 0) {
		resetScriptContext();
		executeScriptFunction("exec();");
	}
	
	return ok;
}

bool UniversalScriptingContext::cleanUp() {
	bool ok = true;

	return ok;
}


void UniversalScriptingContext::valueChanged(Value *value) {
	ScriptingContext::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mTriggerEventName) {
		setTriggerEventName(mTriggerEventName->get());
	}
	else if(value == mResetEventName) {
		setResetEventName(mResetEventName->get());
	}
}


void UniversalScriptingContext::eventOccured(Event *event) {
	ScriptingContext::eventOccured(event);
	if(event == 0) {
		return;
	}
	else if(event == mTriggerEvent) {
		if(mScript == 0) {
			//try to do a reset on the fly
			resetScriptContext();
		}
		executeScriptFunction("exec();");
	}
	else if(event == mResetEvent) {
		resetScriptContext();
	}
}

void UniversalScriptingContext::setTriggerEventName(const QString &triggerEventName) {
	if(!mInitialized) {
		mTriggerEventName->set(triggerEventName);
	}
	else {
		if(mTriggerEvent != 0) {
			mTriggerEvent->removeEventListener(this);
			mTriggerEvent = 0;
		}
		mTriggerEventName->set(triggerEventName);
		mTriggerEvent = Core::getInstance()->getEventManager()
				->registerForEvent(triggerEventName, this);

		if(mTriggerEvent == 0 && mTriggerEventName->get().trimmed() != "") {
			reportError("UniversalScriptingContext [" + getName() + "]: "
				+ "Could not find trigger event [" + triggerEventName + "]");
		}
	}
}


QString UniversalScriptingContext::getTriggerEventName() const {
	return mTriggerEventName->get();
}

void UniversalScriptingContext::setResetEventName(const QString &resetEventName) {
	if(!mInitialized) {
		mResetEventName->set(resetEventName);
	}
	else {
		if(mResetEvent != 0) {
			mResetEvent->removeEventListener(this);
			mResetEvent = 0;
		}
		mResetEventName->set(resetEventName);
		mResetEvent = Core::getInstance()->getEventManager()
				->registerForEvent(resetEventName, this);

		if(mResetEvent == 0 && mResetEventName->get().trimmed() != "") {
			reportError("UniversalScriptingContext [" + getName() + "]: "
				+ "Could not find reset event [" + resetEventName + "]");
		}
	}
}


QString UniversalScriptingContext::getResetEventName() const {
	return mResetEventName->get();
}

void UniversalScriptingContext::setScriptFileName(const QString &fileName) {
	mScriptFileName->set(fileName);
}

bool UniversalScriptingContext::loadScriptCode(bool replaceExistingCode) {
	if(mInitialized) {
		return ScriptingContext::loadScriptCode(replaceExistingCode);
	}
	return true;
}

}



