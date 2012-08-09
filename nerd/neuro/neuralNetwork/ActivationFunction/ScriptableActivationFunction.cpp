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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
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
 *   clearly by citing the nerd homepage and the nerd overview paper.      *
 ***************************************************************************/

#include "ScriptableActivationFunction.h"
#include <QListIterator>
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <iostream>
#include "Value/CodeValue.h"

using namespace std;

namespace nerd {

ScriptableActivationFunction::ScriptableActivationFunction()
	: ScriptingContext("Scripted"), ActivationFunction("Scripted"), mOwner(0),
	  mFirstExecution(true)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();
	
	mVar1 = new DoubleValue();
	mVar2 = new DoubleValue();
	mVar3 = new DoubleValue();
	mVar4 = new DoubleValue();
	
	addObserableOutput("Alpha", mVar1);
	addObserableOutput("Beta", mVar2);
	addObserableOutput("Gamma", mVar3);
	addObserableOutput("Delta", mVar4);
	
	
	mScriptFileName->removeValueChangedListener(this);
	
	//set default code (make sure that the change is NOT notified, otherwise script may be executed
	//before this object is fully constructed!
	mScriptCode->removeValueChangedListener(this);
	mScriptCode->setValueFromString("function reset() {/**/}/**//**/function calc() {/**/  return 1.0;/**/}");
	mScriptCode->addValueChangedListener(this);
	mScriptCode->setNotifyAllSetAttempts(true);
	
	addParameter("Code", mScriptCode);
	addParameter("Alpha", mVar1);
	addParameter("Beta", mVar2);
	addParameter("Gamma", mVar3);
	addParameter("Delta", mVar4);
	
	//allow script executions and reset also in the GUI thread (to react on changes immediately)
	mRestrictToMainExecutionThread = false;
}

ScriptableActivationFunction::ScriptableActivationFunction(
			const ScriptableActivationFunction &other)
	: Object(), ValueChangedListener(), EventListener(), ScriptingContext(other), ActivationFunction(other),
	  mOwner(0), mFirstExecution(true)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();
	
	mVar1 = dynamic_cast<DoubleValue*>(getParameter("Alpha"));
	mVar2 = dynamic_cast<DoubleValue*>(getParameter("Beta"));
	mVar3 = dynamic_cast<DoubleValue*>(getParameter("Gamma"));
	mVar4 = dynamic_cast<DoubleValue*>(getParameter("Delta"));
	
	addObserableOutput("Alpha", mVar1);
	addObserableOutput("Beta", mVar2);
	addObserableOutput("Gamma", mVar3);
	addObserableOutput("Delta", mVar4);
	
	mScriptFileName->removeValueChangedListener(this);
	
	//replace script code value with the one stored as parameter.
	
	delete mScriptCode;
	mScriptCode = dynamic_cast<CodeValue*>(getParameter("Code"));
	mScriptCode->setNotifyAllSetAttempts(true);
	
	//allow script executions and reset also in the GUI thread (to react on changes immediately)
	mRestrictToMainExecutionThread = false;
}

ScriptableActivationFunction::~ScriptableActivationFunction() {
	mScriptCode = 0;
	if(mNetworkManipulator != 0) {
		delete mNetworkManipulator;
	}
}

ActivationFunction* ScriptableActivationFunction::createCopy() const {
	return new ScriptableActivationFunction(*this);
}

QString ScriptableActivationFunction::getName() const {
	return ActivationFunction::getName();
}

void ScriptableActivationFunction::valueChanged(Value *value) {
	ScriptingContext::valueChanged(value);
	ActivationFunction::valueChanged(value);
	
	if(value == mScriptCode) {
		//additionally call the reset function in the script 
		//(a resetScriptContext has already been triggered by the ScriptingContext)
		executeScriptFunction("reset();");
	}
}

void ScriptableActivationFunction::reset(Neuron *neuron) {
	mOwner = neuron;
	ModularNeuralNetwork *network = dynamic_cast<ModularNeuralNetwork*>(neuron->getOwnerNetwork());
	if(mNetworkManipulator != 0) {
		mNetworkManipulator->setNeuralNetwork(network);
	}

	resetScriptContext();
	executeScriptFunction("reset();");
}


double ScriptableActivationFunction::calculateActivation(Neuron *owner) {
	if(owner == 0) {
		return 0.0;
	}

	if(mFirstExecution) {
		reset(owner);
		mFirstExecution = false;
	}

	executeScriptFunction("call_calc();");

	return mActivation.get();;
}

bool ScriptableActivationFunction::equals(ActivationFunction *activationFunction) const {
	if(ActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	ScriptableActivationFunction *af =
 			dynamic_cast<ScriptableActivationFunction*>(activationFunction);

	if(af == 0) {
		return false;
	}
	return true;
}


void ScriptableActivationFunction::addCustomScriptContextStructures() {

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
	error = mScript->evaluate("function call_calc() {  __returnValue__ = this.calc(); };");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add call_calc() function. ") 
						+ error.toString());
		return;
	}
	
	qulonglong ownerId = 0;
	if(mOwner != 0) {
		ownerId = mOwner->getId();
	}
	
	error = mScript->evaluate(QString("var activity = 0;"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable activity: " + error.toString()));
		//ignore error and go on.
	}
	error = mScript->evaluate(QString("var output = 0;"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable output: " + error.toString()));
		//ignore error and go on.
	}
	error = mScript->evaluate(QString("var neuron = " + QString::number(ownerId) + ";"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable neuron: " + error.toString()));
		//ignore error and go on.
	}
	
	error = mScript->evaluate(QString("var alpha = " + QString::number(mVar1->get()) + ";"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable alpha: " + error.toString()));
		//ignore error and go on.
	}
	error = mScript->evaluate(QString("var beta = " + QString::number(mVar2->get()) + ";"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable beta: " + error.toString()));
		//ignore error and go on.
	}
	error = mScript->evaluate(QString("var gamma = " + QString::number(mVar3->get()) + ";"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable gamma: " + error.toString()));
		//ignore error and go on.
	}
	error = mScript->evaluate(QString("var delta = " + QString::number(mVar4->get()) + ";"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable delta: " + error.toString()));
		//ignore error and go on.
	}
}

/**
 * Imports the content of all Values aded with defineVariables to the scripting context.
 * IntValues and DoubleValues are set as native ints and doubles. 
 * BoolValues will be translated to real bools (true / false).
 * All other Value types will be set as Strings with enclosing parantheses.
 */
void ScriptableActivationFunction::importVariables() {
	ScriptingContext::importVariables();
	
	if(mScript == 0) {
		return;
	}
	
	if(mOwner != 0) {
	
		mScript->evaluate("activity = " + QString::number(mOwner->getLastActivation()) + ";");
		mScript->evaluate("output = " + QString::number(mOwner->getLastOutputActivation()) + ";");
		mScript->evaluate("neuron = " + QString::number(mOwner->getId()) + ";");
	}
	
	mScript->evaluate("alpha = " + QString::number(mVar1->get()) + ";");
	mScript->evaluate("beta = " + QString::number(mVar2->get()) + ";");
	mScript->evaluate("gamma = " + QString::number(mVar3->get()) + ";");
	mScript->evaluate("delta = " + QString::number(mVar4->get()) + ";");
}



void ScriptableActivationFunction::exportVariables() {
	ScriptingContext::exportVariables();
	
	if(mScript != 0) {
		
		mScript->evaluate(mMainContextName + ".stringBuffer = __returnValue__.toString();");
		mActivation.setValueFromString(mVariableBuffer);

	}
}


}


