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

#include "ScriptableConstraint.h"
#include <QListIterator>
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <iostream>
#include "Value/CodeValue.h"
#include "Core/Core.h"
#include <NerdConstants.h>

using namespace std;

namespace nerd {

ScriptableConstraint::ScriptableConstraint()
	: ScriptingContext("Scripted"), GroupConstraint("Scripted"), mErrorState(0), mOwner(0),
	  mFirstExecution(true), mNextStepEvent(0)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();
	
	mErrorState = new StringValue();
	
	mVar1 = new DoubleValue();
	mVar2 = new DoubleValue();
	mVar3 = new StringValue();
	mVar4 = new StringValue();
	
	mActiveConstraint = new BoolValue(false);
	mNameValue = new StringValue("");
	
	
	mScriptFileName->removeValueChangedListener(this);
	
	//set default code (make sure that the change is NOT notified, otherwise script may be executed
	//before this object is fully constructed!
	mScriptCode->removeValueChangedListener(this);
	mScriptCode->setValueFromString("function reset() {/**/}/**//**/function apply() {/**/\treturn true;/**/}");
	mScriptCode->addValueChangedListener(this);
	mScriptCode->setNotifyAllSetAttempts(true);
	
	mScriptCode->setErrorValue(mErrorState);
	
	addParameter("Code", mScriptCode);
	addParameter("Active", mActiveConstraint);
	addParameter("Name", mNameValue);
	addParameter("Eta", mVar1);
	addParameter("Xi", mVar2);
	addParameter("Kappa", mVar3);
	addParameter("Lambda", mVar4);
	
	//allow script executions and reset also in the GUI thread (to react on changes immediately)
	mRestrictToMainExecutionThread = false;
}

ScriptableConstraint::ScriptableConstraint(const ScriptableConstraint &other)
	: Object(), ValueChangedListener(), EventListener(), ScriptingContext(other), GroupConstraint(other),
	  mErrorState(0), mOwner(0), mFirstExecution(true), mNextStepEvent(0)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();
	
	mErrorState = new StringValue();
	
	mActiveConstraint = dynamic_cast<BoolValue*>(getParameter("Active"));
	mNameValue = dynamic_cast<StringValue*>(getParameter("Name"));
	mVar1 = dynamic_cast<DoubleValue*>(getParameter("Eta"));
	mVar2 = dynamic_cast<DoubleValue*>(getParameter("Xi"));
	mVar3 = dynamic_cast<StringValue*>(getParameter("Kappa"));
	mVar4 = dynamic_cast<StringValue*>(getParameter("Lambda"));
	

	mScriptFileName->removeValueChangedListener(this);
	
	//replace script code value with the one stored as parameter.
	
	delete mScriptCode;
	mScriptCode = dynamic_cast<CodeValue*>(getParameter("Code"));
	mScriptCode->setNotifyAllSetAttempts(true);
	
	mScriptCode->setErrorValue(mErrorState);
	
	//allow script executions and reset also in the GUI thread (to react on changes immediately)
	mRestrictToMainExecutionThread = false;
	
	valueChanged(mActiveConstraint);
}

ScriptableConstraint::~ScriptableConstraint() {
	mScriptCode = 0;
	if(mNetworkManipulator != 0) {
		delete mNetworkManipulator;
	}
}

GroupConstraint* ScriptableConstraint::createCopy() const {
	return new ScriptableConstraint(*this);
}

QString ScriptableConstraint::getName() const {
	return GroupConstraint::getName();
}

void ScriptableConstraint::valueChanged(Value *value) {
	ScriptingContext::valueChanged(value);
	GroupConstraint::valueChanged(value);
	
	if(value == mScriptCode) {
		//additionally call the reset function in the script 
		//(a resetScriptContext has already been triggered by the ScriptingContext)
		executeScriptFunction("reset();");
	}
	else if(value == mActiveConstraint) {
		if(mActiveConstraint->get()) {
			mNextStepEvent = Core::getInstance()->getEventManager()
					->registerForEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP, this);
		}
		else if(mNextStepEvent != 0) {
			mNextStepEvent->removeEventListener(this);
		}
	}
}

void ScriptableConstraint::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	ScriptingContext::eventOccured(event);
	
	if(event == mNextStepEvent) {
		applyConstraint();
	}
}


void ScriptableConstraint::reset() {
	mFirstExecution = true;
}

void ScriptableConstraint::resetScriptContext() {
	mErrorState->set("");
	ScriptingContext::resetScriptContext();
}


// void ScriptableConstraint::reset(Neuron *neuron) {
// 	mOwner = neuron;
// 	ModularNeuralNetwork *network = dynamic_cast<ModularNeuralNetwork*>(neuron->getOwnerNetwork());
// 	if(mNetworkManipulator != 0) {
// 		mNetworkManipulator->setNeuralNetwork(network);
// 	}
// 
// 	resetScriptContext();
// 	executeScriptFunction("reset();");
// }
// 
// 
// double ScriptableConstraint::calculateActivation(Neuron *owner) {
// 	if(owner == 0) {
// 		return 0.0;
// 	}
// 
// 	if(mFirstExecution) {
// 		reset(owner);
// 		mFirstExecution = false;
// 	}
// 
// 	executeScriptFunction("call_calc();");
// 
// 	return mActivation.get();;
// }

bool ScriptableConstraint::isValid(NeuronGroup *owner) {
	
	mOwner = owner;
	if(mOwner == 0) {
		mErrorMessage = "ScriptableConstraint: Did not find an owner group!";
		return false;
	}
	
	//TODO check if a reset() an apply() method are present!
	
	return true;
}


bool ScriptableConstraint::applyConstraint(NeuronGroup *owner, CommandExecutor*,
									 QList<NeuralNetworkElement*>&)
{
	mOwner = owner;
	if(mOwner == 0) {
		mErrorMessage = "ScriptableConstraint: Did not find an owner group!";
		return false;
	}
	
	//TODO figure out a way how to move objects removed via script to the trashcan!

	if(mActiveConstraint->get()) {
		//active constraints are only executed 
		return true;
	}
	return applyConstraint(); //TODO check if this should use the trashcan!
}


/**
 * If called, the apply() function in the script is executed once.
 * Note: the first time this method is called after this.reset() has been called, the script function reset()
 * is called right before the apply() function.
 */
bool ScriptableConstraint::applyConstraint() {
	
	if(mOwner == 0) {
		return false;
	}
	
	ModularNeuralNetwork *network = dynamic_cast<ModularNeuralNetwork*>(mOwner->getOwnerNetwork());
	if(mNetworkManipulator != 0) {
		mNetworkManipulator->setNeuralNetwork(network);
		mNetworkManipulator->setOwnerHint(mOwner);
	}
	
	if(mFirstExecution) {
		resetScriptContext();
		mFirstExecution = false;
		
		executeScriptFunction("reset();");
	}
	
	executeScriptFunction("call_apply();");
	
	return mReturnValue.get();
}

		
bool ScriptableConstraint::equals(GroupConstraint *constraint) const {
	if(GroupConstraint::equals(constraint) == false) {
		return false;
	}
	ScriptableConstraint *af =
 			dynamic_cast<ScriptableConstraint*>(constraint);

	if(af == 0) {
		return false;
	}
	if(!mScriptCode->equals(af->mScriptCode)) {
		return false;
	}
	if(!mVar1->equals(af->mVar1)) {
		return false;
	}
	if(!mVar2->equals(af->mVar2)) {
		return false;
	}
	if(!mVar3->equals(af->mVar3)) {
		return false;
	}
	if(!mVar4->equals(af->mVar4)) {
		return false;
	}
	
	return true;
}


void ScriptableConstraint::setErrorMessage(const QString &message) {
	GroupConstraint::setErrorMessage(message);
	reportError(message);
}


void ScriptableConstraint::setWarningMessage(const QString &message) {
	GroupConstraint::setWarningMessage(message);
	reportError(message);
}



void ScriptableConstraint::reportError(const QString &message) {
	QString msg = message;
	mErrorState->set(msg.replace("\n", " | "));
	Core::log(QString("ScriptableConstraint [") + getName() + "]: " + message);
}


void ScriptableConstraint::addCustomScriptContextStructures() {

	ScriptingContext::addCustomScriptContextStructures();
	
	if(mScript == 0) {
		reportError(QString("There was no script engine object!"));
		return;
	}

	QScriptValue networkManip = mScript->newQObject(mNetworkManipulator,
							QScriptEngine::QtOwnership,
			 				QScriptEngine::ExcludeSuperClassMethods
							  | QScriptEngine::ExcludeSuperClassProperties
							  | QScriptEngine::ExcludeChildObjects);
	mScript->globalObject().setProperty("net", networkManip);

	QScriptValue error = mScript->evaluate("var __returnValue__ = 0;");
	error = mScript->evaluate("function call_apply() {  __returnValue__ = this.apply(); };");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add call_calc() function. ") 
						+ error.toString());
		return;
	}
	
	qulonglong ownerId = 0;
	if(mOwner != 0) {
		ownerId = mOwner->getId();
	}
	
// 	error = mScript->evaluate(QString("var activity = 0;"));
// 	if(mScript->hasUncaughtException()) {
// 		reportError(QString("Could not add variable activity: " + error.toString()));
// 		//ignore error and go on.
// 	}
// 	error = mScript->evaluate(QString("var output = 0;"));
// 	if(mScript->hasUncaughtException()) {
// 		reportError(QString("Could not add variable output: " + error.toString()));
// 		//ignore error and go on.
// 	}
	error = mScript->evaluate(QString("var group = " + QString::number(ownerId) + ";"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable group: " + error.toString()));
		//ignore error and go on.
	}
	
	
	defineVariable("eta", mVar1);
	defineVariable("xi", mVar2);
	defineVariable("kappa", mVar3);
	defineVariable("lambda", mVar4);
}

/**
 * Imports the content of all Values aded with defineVariables to the scripting context.
 * IntValues and DoubleValues are set as native ints and doubles. 
 * BoolValues will be translated to real bools (true / false).
 * All other Value types will be set as Strings with enclosing parantheses.
 */
void ScriptableConstraint::importVariables() {
	ScriptingContext::importVariables();
	
	if(mScript == 0) {
		return;
	}
	
	if(mOwner != 0) {
	
// 		mScript->evaluate("activity = " + QString::number(mOwner->getLastActivation()) + ";");
// 		mScript->evaluate("output = " + QString::number(mOwner->getLastOutputActivation()) + ";");
		mScript->evaluate("group = " + QString::number(mOwner->getId()) + ";");
	}
	
	mScript->evaluate("eta = " + mVar1->getValueAsString() + ";");
	mScript->evaluate("xi = " + mVar2->getValueAsString() + ";");
	mScript->evaluate("kappa = " + mVar3->getValueAsString() + ";");
	mScript->evaluate("lambda = " + mVar4->getValueAsString() + ";");
	
}



void ScriptableConstraint::exportVariables() {
	ScriptingContext::exportVariables();
	
	if(mScript != 0) {
		
		mScript->evaluate(mMainContextName + ".stringBuffer = __returnValue__.toString();");
		mReturnValue.setValueFromString(mVariableBuffer);

	}
}


}


