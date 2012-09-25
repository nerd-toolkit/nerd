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
#include "Core/Core.h"
#include <Network/NeuralNetworkManager.h>
#include <Network/Neuro.h>

using namespace std;

namespace nerd {

ScriptableActivationFunction::ScriptableActivationFunction()
	: ScriptingContext("Scripted"), NeuroModulatorActivationFunction("Scripted"), 
		mErrorState(0), mOwner(0), mFirstExecution(true), mDefaultActivationFunction(0)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();
	
	mErrorState = new StringValue();
	
	mVar1 = new DoubleValue();
	mVar2 = new DoubleValue();
	mVar3 = new StringValue();
	mVar4 = new StringValue();
	
	addObserableOutput("Eta", mVar1);
	addObserableOutput("Xi", mVar2);
	
	
	mScriptFileName->removeValueChangedListener(this);
	
	//set default code (make sure that the change is NOT notified, otherwise script may be executed
	//before this object is fully constructed!
	mScriptCode->removeValueChangedListener(this);
	mScriptCode->setValueFromString("function reset() {/**/}/**//**/function calc() {/**/\treturn 1.0;/**/}");
	mScriptCode->addValueChangedListener(this);
	mScriptCode->setNotifyAllSetAttempts(true);
	
	mScriptCode->setErrorValue(mErrorState);
	
	addParameter("Code", mScriptCode);
	addParameter("Eta", mVar1);
	addParameter("Xi", mVar2);
	addParameter("Kappa", mVar3);
	addParameter("Lambda", mVar4);
	
	
	//allow script executions and reset also in the GUI thread (to react on changes immediately)
	mRestrictToMainExecutionThread = false;
}

ScriptableActivationFunction::ScriptableActivationFunction(
			const ScriptableActivationFunction &other)
	: Object(), ValueChangedListener(), EventListener(), ScriptingContext(other), 
	  NeuroModulatorActivationFunction(other), mErrorState(0), mOwner(0), mFirstExecution(true),
	  mDefaultActivationFunction(0)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();
	
	mErrorState = new StringValue();
	
	mVar1 = dynamic_cast<DoubleValue*>(getParameter("Eta"));
	mVar2 = dynamic_cast<DoubleValue*>(getParameter("Xi"));
	mVar3 = dynamic_cast<StringValue*>(getParameter("Kappa"));
	mVar4 = dynamic_cast<StringValue*>(getParameter("Lambda"));
	
	
	addObserableOutput("Eta", mVar1);
	addObserableOutput("Xi", mVar2);
	
	mScriptFileName->removeValueChangedListener(this);
	
	//replace script code value with the one stored as parameter.
	
	delete mScriptCode;
	mScriptCode = dynamic_cast<CodeValue*>(getParameter("Code"));
	mScriptCode->setNotifyAllSetAttempts(true);
	
	mScriptCode->setErrorValue(mErrorState);
	
	//allow script executions and reset also in the GUI thread (to react on changes immediately)
	mRestrictToMainExecutionThread = false;
	
	if(other.mDefaultActivationFunction != 0) {
		mDefaultActivationFunction = other.mDefaultActivationFunction->createCopy();
	}
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
	return NeuroModulatorActivationFunction::getName();
}

void ScriptableActivationFunction::valueChanged(Value *value) {
	ScriptingContext::valueChanged(value);
	NeuroModulatorActivationFunction::valueChanged(value);
	
	if(value == mScriptCode) {
		//additionally call the reset function in the script 
		//(a resetScriptContext has already been triggered by the ScriptingContext)
		executeScriptFunction("reset();");
	}
}


void ScriptableActivationFunction::resetScriptContext() {
	mErrorState->set("");
	ScriptingContext::resetScriptContext();
}


void ScriptableActivationFunction::reset(Neuron *neuron) {
	NeuroModulatorActivationFunction::reset(neuron);
	mOwner = neuron;
	ModularNeuralNetwork *network = dynamic_cast<ModularNeuralNetwork*>(neuron->getOwnerNetwork());
	if(mNetworkManipulator != 0) {
		mNetworkManipulator->setNeuralNetwork(network);
		mNetworkManipulator->setOwnerHint(neuron);
	}
	
	resetScriptContext();
	executeScriptFunction("reset();");
	
}


double ScriptableActivationFunction::calculateActivation(Neuron *owner) {
	if(owner == 0) {
		return 0.0;
	}
	//just call the parent function to update the neuroModulators, but ignore
	//the return value of this method (is constant 0.0)
	NeuroModulatorActivationFunction::calculateActivation(owner);

	if(mFirstExecution) {
		reset(owner);
		mFirstExecution = false;
	}

	executeScriptFunction("call_calc();");

	return mActivation.get();;
}

bool ScriptableActivationFunction::equals(ActivationFunction *activationFunction) const {
	if(NeuroModulatorActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	ScriptableActivationFunction *af =
 			dynamic_cast<ScriptableActivationFunction*>(activationFunction);

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
	if(mDefaultActivationFunction == 0 && af->mDefaultActivationFunction != 0) {
		return false;
	}
	if(af->mDefaultActivationFunction != 0 && mDefaultActivationFunction != 0) {
		if(!mDefaultActivationFunction->equals(af->mDefaultActivationFunction)) {
			return false;
		}
	}
	
	return true;
}


double ScriptableActivationFunction::useDefaultActivationFunction() {
	if(mDefaultActivationFunction == 0) {
		return 0.0;
	}
	return mDefaultActivationFunction->calculateActivation(mOwner);
}


bool ScriptableActivationFunction::setDefaultActivationFunction(const QString &name) {
	if(mDefaultActivationFunction != 0) {
		if(mDefaultActivationFunction->getName() == name) {
			//don't change anything
			return true;
		}
		delete mDefaultActivationFunction;
		mDefaultActivationFunction = 0;
	}
	if(name == "") {
		//indicates to remove the default activation function
		return true;
	}
	
	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();
	QList<ActivationFunction*> prototypes = nnm->getActivationFunctionPrototypes();
	for(QListIterator<ActivationFunction*> i(prototypes); i.hasNext();) {
		ActivationFunction *af = i.next();
		if(af->getName() == name) {
			mDefaultActivationFunction = af->createCopy();
			return true;
		}
	}
	return false;
}


void ScriptableActivationFunction::reportError(const QString &message) {
	QString msg = message;
	mErrorState->set(msg.replace("\n", " | "));
	Core::log(QString("ScriptableActivationFunction [") + getName() + "]: " + message);
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
	error = mScript->evaluate(QString("var bias = 0;"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable bias: " + error.toString()));
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
void ScriptableActivationFunction::importVariables() {
	ScriptingContext::importVariables();
	
	if(mScript == 0) {
		return;
	}
	
	if(mOwner != 0) {
	
		mScript->evaluate("activity = " + QString::number(mOwner->getLastActivation()) + ";");
		mScript->evaluate("output = " + QString::number(mOwner->getLastOutputActivation()) + ";");
		mScript->evaluate("neuron = " + QString::number(mOwner->getId()) + ";");
		mScript->evaluate("bias = " + mOwner->getBiasValue().getValueAsString() + ";");
	}
}



void ScriptableActivationFunction::exportVariables() {
	ScriptingContext::exportVariables();
	
	if(mScript != 0) {
		
		mScript->evaluate(mMainContextName + ".stringBuffer = __returnValue__.toString();");
		mActivation.setValueFromString(mVariableBuffer);

	}
}


}


